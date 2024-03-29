
#include "loader.hpp"
#include "endian.hpp"
#include "sim.hpp"
#include "eio.hpp"

loader::loader(sim *S)
{
	Sim = S;
	
	/* program text (code) segment base */
	ld_text_base = 0;

	/* program text (code) size in bytes */
	ld_text_size = 0;

	/* program initialized data segment base */
	ld_data_base = 0;

	/* program initialized ".data" and uninitialized ".bss" size in bytes */
	ld_data_size = 0;

	/* top of the data segment */
	ld_brk_point = 0;

	/* program stack segment base (highest address in stack) */
	ld_stack_base = MD_STACK_BASE;

	/* program initial stack size */
	ld_stack_size = 0;

	/* lowest address accessed on the stack */
	ld_stack_min = (md_addr_t)-1;

	/* program file name */
	ld_prog_fname = NULL;

	/* program entry point (initial PC) */
	ld_prog_entry = 0;

	/* program environment base address address */
	ld_environ_base = 0;
}

/* register simulator-specific statistics */
void
loader::ld_reg_stats(struct stat_sdb_t *sdb)	/* stats data base */
{
  Sim->stat_reg_addr(sdb, "ld_text_base",
		"program text (code) segment base",
		&ld_text_base, ld_text_base, "  0x%08p");
  Sim->STATS->stat_reg_uint(sdb, "ld_text_size",
		"program text (code) size in bytes",
		&ld_text_size, ld_text_size, NULL);
  Sim->stat_reg_addr(sdb, "ld_data_base",
		"program initialized data segment base",
		&ld_data_base, ld_data_base, "  0x%08p");
  Sim->STATS->stat_reg_uint(sdb, "ld_data_size",
		"program init'ed `.data' and uninit'ed `.bss' size in bytes",
		&ld_data_size, ld_data_size, NULL);
  Sim->stat_reg_addr(sdb, "ld_stack_base",
		"program stack segment base (highest address in stack)",
		&ld_stack_base, ld_stack_base, "  0x%08p");
  Sim->STATS->stat_reg_uint(sdb, "ld_stack_size",
		"program initial stack size",
		&ld_stack_size, ld_stack_size, NULL);
#if 0 /* FIXME: broken... */
  stat_reg_addr(sdb, "ld_stack_min",
		"program stack segment lowest address",
		&ld_stack_min, ld_stack_min, "  0x%08p");
#endif
  Sim->stat_reg_addr(sdb, "ld_prog_entry",
		"program entry point (initial PC)",
		&ld_prog_entry, ld_prog_entry, "  0x%08p");
  Sim->stat_reg_addr(sdb, "ld_environ_base",
		"program environment base address address",
		&ld_environ_base, ld_environ_base, "  0x%08p");
  Sim->STATS->stat_reg_int(sdb, "ld_target_big_endian",
	       "target executable endian-ness, non-zero if big endian",
	       &ld_target_big_endian, ld_target_big_endian, NULL);
}


/* load program text and initialized data into simulated virtual memory
   space and initialize program segment range variables */
void
loader::ld_load_prog(char *fname,		/* program to load */
	     int argc, char **argv,	/* simulated program cmd line args */
	     char **envp,		/* simulated program environment */
	     struct regs_t *regs,	/* registers to initialize for load */
	     struct mem_t *mem,		/* memory space to load prog into */
	     int zero_bss_segs)		/* zero uninit data segment? */
{
  int i;
  word_t temp;
  md_addr_t sp, data_break = 0, null_ptr = 0, argv_addr, envp_addr;

  if (Sim->EIOobj->eio_valid(fname))
    {
      if (argc != 1)
	{
	  fprintf(stderr, "error: EIO file has arguments\n");
	  exit(1);
	}

      fprintf(stderr, "sim: loading EIO file: %s\n", fname);

      Sim->GLOBAL->sim_eio_fname = Sim->MISC->mystrdup(fname);

      /* open the EIO file stream */
      Sim->GLOBAL->sim_eio_fd = Sim->EIOobj->eio_open(fname);

      /* load initial state checkpoint */
      if (Sim->EIOobj->eio_read_chkpt(regs, mem, Sim->GLOBAL->sim_eio_fd) != -1)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "bad initial checkpoint in EIO file");

      /* load checkpoint? */
      if (Sim->GLOBAL->sim_chkpt_fname != NULL)
	{
	  counter_t restore_icnt;

	  FILE *chkpt_fd;

	  fprintf(stderr, "sim: loading checkpoint file: %s\n",
		  Sim->GLOBAL->sim_chkpt_fname);

	  if (!Sim->EIOobj->eio_valid(Sim->GLOBAL->sim_chkpt_fname))
	    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "file `%s' does not appear to be a checkpoint file",
		  Sim->GLOBAL->sim_chkpt_fname);

	  /* open the checkpoint file */
	  chkpt_fd = Sim->EIOobj->eio_open(Sim->GLOBAL->sim_chkpt_fname);

	  /* load the state image */
	  restore_icnt = Sim->EIOobj->eio_read_chkpt(regs, mem, chkpt_fd);

	  /* fast forward the baseline EIO trace to checkpoint location */
	  fprintf(stderr, "sim: fast forwarding to instruction %d\n",
		  (int)restore_icnt);
	  Sim->EIOobj->eio_fast_forward(Sim->GLOBAL->sim_eio_fd, restore_icnt);
	}

      /* computed state... */
      ld_environ_base = regs->regs_R[MD_REG_SP];
      ld_prog_entry = regs->regs_PC;

      /* fini... */
      return;
    }

  if (Sim->GLOBAL->sim_chkpt_fname != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "checkpoints only supported while EIO tracing");

#ifdef BFD_LOADER

  {
    bfd *abfd;
    asection *sect;

    /* set up a local stack pointer, this is where the argv and envp
       data is written into program memory */
    ld_stack_base = MD_STACK_BASE;
    sp = ROUND_DOWN(MD_STACK_BASE - MD_MAX_ENVIRON, sizeof(dfloat_t));
    ld_stack_size = ld_stack_base - sp;

    /* initial stack pointer value */
    ld_environ_base = sp;

    /* load the program into memory, try both endians */
    if (!(abfd = bfd_openr(argv[0], "ss-coff-big")))
      if (!(abfd = bfd_openr(argv[0], "ss-coff-little")))
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot open executable `%s'", argv[0]);

    /* this call is mainly for its side effect of reading in the sections.
       we follow the traditional behavior of `strings' in that we don't
       complain if we don't recognize a file to be an object file.  */
    if (!bfd_check_format(abfd, bfd_object))
      {
	bfd_close(abfd);
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot open executable `%s'", argv[0]);
      }

    /* record profile file name */
    ld_prog_fname = argv[0];

    /* record endian of target */
    ld_target_big_endian = abfd->xvec->byteorder_big_p;

    debug("processing %d sections in `%s'...",
	  bfd_count_sections(abfd), argv[0]);

    /* read all sections in file */
    for (sect=abfd->sections; sect; sect=sect->next)
      {
	char *p;

	debug("processing section `%s', %d bytes @ 0x%08x...",
	      bfd_section_name(abfd, sect), bfd_section_size(abfd, sect),
	      bfd_section_vma(abfd, sect));

	/* read the section data, if allocated and loadable and non-NULL */
	if ((bfd_get_section_flags(abfd, sect) & SEC_ALLOC)
	    && (bfd_get_section_flags(abfd, sect) & SEC_LOAD)
	    && bfd_section_vma(abfd, sect)
	    && bfd_section_size(abfd, sect))
	  {
	    /* allocate a section buffer */
	    p = calloc(bfd_section_size(abfd, sect), sizeof(char));
	    if (!p)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot allocate %d bytes for section `%s'",
		    bfd_section_size(abfd, sect),
		    bfd_section_name(abfd, sect));

	    if (!bfd_get_section_contents(abfd, sect, p, (file_ptr)0,
					  bfd_section_size(abfd, sect)))
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read entire `%s' section from executable",
		    bfd_section_name(abfd, sect));

	    /* copy program section it into simulator target memory */
	    Sim->MEMORY->mem_bcopy(/*mem_access, */mem, Write, bfd_section_vma(abfd, sect),
		      p, bfd_section_size(abfd, sect));

	    /* release the section buffer */
	    free(p);
	  }
	/* zero out the section if it is loadable but not allocated in exec */
	else if (zero_bss_segs
		 && (bfd_get_section_flags(abfd, sect) & SEC_LOAD)
		 && bfd_section_vma(abfd, sect)
		 && bfd_section_size(abfd, sect))
	  {
	    /* zero out the section region */
	    Sim->MEMORY->mem_bzero(/*mem_access,*/ mem,
		      bfd_section_vma(abfd, sect),
		      bfd_section_size(abfd, sect));
	  }
	else
	  {
	    /* else do nothing with this section, it's probably debug data */
	    debug("ignoring section `%s' during load...",
		  bfd_section_name(abfd, sect));
	  }

	/* expected text section */
	if (!strcmp(bfd_section_name(abfd, sect), ".text"))
	  {
	    /* .text section processing */
	    ld_text_size =
	      ((bfd_section_vma(abfd, sect) + bfd_section_size(abfd, sect))
	       - MD_TEXT_BASE)
		+ /* for speculative fetches/decodes */TEXT_TAIL_PADDING;

	    /* create tail padding and copy into simulator target memory */
	    Sim->MEMORY->mem_bzero(/*mem_access,*/ mem,
		      bfd_section_vma(abfd, sect)
		      + bfd_section_size(abfd, sect),
		      TEXT_TAIL_PADDING);
	  }
	/* expected data sections */
	else if (!strcmp(bfd_section_name(abfd, sect), ".rdata")
		 || !strcmp(bfd_section_name(abfd, sect), ".data")
		 || !strcmp(bfd_section_name(abfd, sect), ".sdata")
		 || !strcmp(bfd_section_name(abfd, sect), ".bss")
		 || !strcmp(bfd_section_name(abfd, sect), ".sbss"))
	  {
	    /* data section processing */
	    if (bfd_section_vma(abfd, sect) + bfd_section_size(abfd, sect) >
		data_break)
	      data_break = (bfd_section_vma(abfd, sect) +
			    bfd_section_size(abfd, sect));
	  }
	else
	  {
	    /* what is this section??? */
	    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "encountered unknown section `%s', %d bytes @ 0x%08x",
		  bfd_section_name(abfd, sect), bfd_section_size(abfd, sect),
		  bfd_section_vma(abfd, sect));
	  }
      }

    /* compute data segment size from data break point */
    ld_text_base = MD_TEXT_BASE;
    ld_data_base = MD_DATA_BASE;
    ld_prog_entry = bfd_get_start_address(abfd);
    ld_data_size = data_break - ld_data_base;

    /* done with the executable, close it */
    if (!bfd_close(abfd))
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not close executable `%s'", argv[0]);
  }

#else /* !BFD_LOADER, i.e., standalone loader */

  {
    FILE *fobj;
    long floc;
    struct ecoff_filehdr fhdr;
    struct ecoff_aouthdr ahdr;
    struct ecoff_scnhdr shdr;

    /* set up a local stack pointer, this is where the argv and envp
       data is written into program memory */
    ld_stack_base = MD_STACK_BASE;
    sp = ROUND_DOWN(MD_STACK_BASE - MD_MAX_ENVIRON, sizeof(dfloat_t));
    ld_stack_size = ld_stack_base - sp;

    /* initial stack pointer value */
    ld_environ_base = sp;

    /* record profile file name */
    ld_prog_fname = argv[0];

    /* load the program into memory, try both endians */
#if defined(__CYGWIN32__) || defined(_MSC_VER)
    fobj = fopen(argv[0], "rb");
#else
    fobj = fopen(argv[0], "r");
#endif
    if (!fobj)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot open executable `%s'", argv[0]);

    if (fread(&fhdr, sizeof(struct ecoff_filehdr), 1, fobj) < 1)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot read header from executable `%s'", argv[0]);

    /* record endian of target */
    if (fhdr.f_magic == ECOFF_EB_MAGIC)
      ld_target_big_endian = TRUE;
    else if (fhdr.f_magic == ECOFF_EL_MAGIC)
      ld_target_big_endian = FALSE;
    else if (fhdr.f_magic == ECOFF_EB_OTHER || fhdr.f_magic == ECOFF_EL_OTHER)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "PISA binary `%s' has wrong endian format", argv[0]);
    else if (fhdr.f_magic == ECOFF_ALPHAMAGIC)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "PISA simulator cannot run Alpha binary `%s'", argv[0]);
    else
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "bad magic number in executable `%s' (not an executable?)",
	    argv[0]);

    if (fread(&ahdr, sizeof(struct ecoff_aouthdr), 1, fobj) < 1)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot read AOUT header from executable `%s'", argv[0]);

    data_break = MD_DATA_BASE + ahdr.dsize + ahdr.bsize;

#if 0
    Data_start = ahdr.data_start;
    Data_size = ahdr.dsize;
    Bss_size = ahdr.bsize;
    Bss_start = ahdr.bss_start;
    Gp_value = ahdr.gp_value;
    Text_entry = ahdr.entry;
#endif

    /* seek to the beginning of the first section header, the file header comes
       first, followed by the optional header (this is the aouthdr), the size
       of the aouthdr is given in Fdhr.f_opthdr */
    fseek(fobj, sizeof(struct ecoff_filehdr) + fhdr.f_opthdr, 0);

    debug("processing %d sections in `%s'...", fhdr.f_nscns, argv[0]);

    /* loop through the section headers */
    floc = ftell(fobj);
    for (i = 0; i < fhdr.f_nscns; i++)
      {
	char *p;

	if (fseek(fobj, floc, 0) == -1)
	  Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not reset location in executable");
	if (fread(&shdr, sizeof(struct ecoff_scnhdr), 1, fobj) < 1)
	  Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read section %d from executable", i);
	floc = ftell(fobj);

	switch (shdr.s_flags)
	  {
	  case ECOFF_STYP_TEXT:
	    ld_text_size = ((shdr.s_vaddr + shdr.s_size) - MD_TEXT_BASE) 
	      + TEXT_TAIL_PADDING;

	    p = (char *) calloc(shdr.s_size, sizeof(char));
	    if (!p)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

	    if (fseek(fobj, shdr.s_scnptr, 0) == -1)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read `.text' from executable", i);
	    if (fread(p, shdr.s_size, 1, fobj) < 1)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read text section from executable");

	    /* copy program section into simulator target memory */
	    Sim->MEMORY->mem_bcopy(/*mem_access,*/ mem, Write, shdr.s_vaddr, p, shdr.s_size);

	    /* create tail padding and copy into simulator target memory */
	    Sim->MEMORY->mem_bzero(/*mem_access,*/ mem,
		      shdr.s_vaddr + shdr.s_size, TEXT_TAIL_PADDING);
  
	    /* release the section buffer */
	    free(p);

#if 0
	    Text_seek = shdr.s_scnptr;
	    Text_start = shdr.s_vaddr;
	    Text_size = shdr.s_size / 4;
	    /* there is a null routine after the supposed end of text */
	    Text_size += 10;
	    Text_end = Text_start + Text_size * 4;
	    /* create_text_reloc(shdr.s_relptr, shdr.s_nreloc); */
#endif
	    break;

	  case ECOFF_STYP_RDATA:
	    /* The .rdata section is sometimes placed before the text
	     * section instead of being contiguous with the .data section.
	     */
#if 0
	    Rdata_start = shdr.s_vaddr;
	    Rdata_size = shdr.s_size;
	    Rdata_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_DATA:
#if 0
	    Data_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_SDATA:
#if 0
	    Sdata_seek = shdr.s_scnptr;
#endif

	    p = (char *) calloc(shdr.s_size, sizeof(char));
	    if (!p)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "out of virtual memory");

	    if (fseek(fobj, shdr.s_scnptr, 0) == -1)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read `.text' from executable", i);
	    if (fread(p, shdr.s_size, 1, fobj) < 1)
	      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read text section from executable");

	    /* copy program section it into simulator target memory */
	    Sim->MEMORY->mem_bcopy(/*mem_access,*/ mem, Write, shdr.s_vaddr, p, shdr.s_size);

	    /* release the section buffer */
	    free(p);

	    break;

	  case ECOFF_STYP_BSS:
	    break;

	  case ECOFF_STYP_SBSS:
	    break;
	  }
      }

    /* compute data segment size from data break point */
    ld_text_base = MD_TEXT_BASE;
    ld_data_base = MD_DATA_BASE;
    ld_prog_entry = ahdr.entry;
    ld_data_size = data_break - ld_data_base;

    /* done with the executable, close it */
    if (fclose(fobj))
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not close executable `%s'", argv[0]);
  }
#endif /* BFD_LOADER */

  /* perform sanity checks on segment ranges */
  if (!ld_text_base || !ld_text_size)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "executable is missing a `.text' section");
  if (!ld_data_base || !ld_data_size)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "executable is missing a `.data' section");
  if (!ld_prog_entry)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "program entry point not specified");

  /* determine byte/words swapping required to execute on this host */
  Sim->GLOBAL->sim_swap_bytes = (Sim->ENDIAN->endian_host_byte_order() != Sim->ENDIAN->endian_target_byte_order());
  if (Sim->GLOBAL->sim_swap_bytes)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping bytes to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
#else
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "binary endian does not match host endian");
#endif
    }
  Sim->GLOBAL->sim_swap_words = (Sim->ENDIAN->endian_host_word_order() != Sim->ENDIAN->endian_target_word_order());
  if (Sim->GLOBAL->sim_swap_words)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping words to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
#else
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "binary endian does not match host endian");
#endif
    }

  /* write [argc] to stack */
  temp = SWAP_WORD(argc);
  Sim->MEMORY->mem_access(mem, Write, sp, &temp, sizeof(word_t));
  sp += sizeof(word_t);

  /* skip past argv array and NULL */
  argv_addr = sp;
  sp = sp + (argc + 1) * sizeof(md_addr_t);

  /* save space for envp array and NULL */
  envp_addr = sp;
  for (i=0; envp[i]; i++)
    sp += sizeof(md_addr_t);
  sp += sizeof(md_addr_t);

  /* fill in the argv pointer array and data */
  for (i=0; i<argc; i++)
    {
      /* write the argv pointer array entry */
      temp = SWAP_WORD(sp);
      Sim->MEMORY->mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      Sim->MEMORY->mem_strcpy(/*mem_access,*/ mem, Write, sp, argv[i]);
      sp += strlen(argv[i]) + 1;
    }
  /* terminate argv array with a NULL */
  Sim->MEMORY->mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* write envp pointer array and data to stack */
  for (i = 0; envp[i]; i++)
    {
      /* write the envp pointer array entry */
      temp = SWAP_WORD(sp);
      Sim->MEMORY->mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      Sim->MEMORY->mem_strcpy(/*mem_access,*/ mem, Write, sp, envp[i]);
      sp += strlen(envp[i]) + 1;
    }
  /* terminate the envp array with a NULL */
  Sim->MEMORY->mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* did we tromp off the stop of the stack? */
  if (sp > ld_stack_base)
    {
      /* we did, indicate to the user that MD_MAX_ENVIRON must be increased,
	 alternatively, you can use a smaller environment, or fewer
	 command line arguments */
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "environment overflow, increase MD_MAX_ENVIRON in ss.h");
    }

  /* initialize the bottom of heap to top of data segment */
  ld_brk_point = ROUND_UP(ld_data_base + ld_data_size, MD_PAGE_SIZE);

  /* set initial minimum stack pointer value to initial stack value */
  ld_stack_min = regs->regs_R[MD_REG_SP];

  /* set up initial register state */
  regs->regs_R[MD_REG_SP] = ld_environ_base;
  regs->regs_PC = ld_prog_entry;

  debug("ld_text_base: 0x%08x  ld_text_size: 0x%08x",
	ld_text_base, ld_text_size);
  debug("ld_data_base: 0x%08x  ld_data_size: 0x%08x",
	ld_data_base, ld_data_size);
  debug("ld_stack_base: 0x%08x  ld_stack_size: 0x%08x",
	ld_stack_base, ld_stack_size);
  debug("ld_prog_entry: 0x%08x", ld_prog_entry);

  /* finally, predecode the text segment... */
  {
    md_addr_t addr;
    md_inst_t inst;
    enum md_fault_type fault;

    if (OP_MAX > 255)
      Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot perform fast decoding, too many opcodes");

    debug("sim: decoding text segment...");
    for (addr=ld_text_base;
	 addr < (ld_text_base+ld_text_size);
	 addr += sizeof(md_inst_t))
      {
	fault = Sim->MEMORY->mem_access(mem, Read, addr, &inst, sizeof(inst));
	if (fault != md_fault_none)
	  Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read instruction memory");
	inst.a = (inst.a & ~0xff) | (word_t)MD_OP_ENUM(MD_OPFIELD(inst));
	fault = Sim->MEMORY->mem_access(mem, Write, addr, &inst, sizeof(inst));
	if (fault != md_fault_none)
	  Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not write instruction memory");
      }
  }
}
