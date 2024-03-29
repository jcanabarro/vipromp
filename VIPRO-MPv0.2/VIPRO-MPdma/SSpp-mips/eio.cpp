
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif

#include "host.hpp"
#include "misc.hpp"
#include "machine.hpp"
#include "regs.hpp"
#include "memory.hpp"
#include "loader.hpp"

#include "syscall.hpp"
#include "sim.hpp"
#include "endian.hpp"
#include "eio.hpp"
#include "libexo.hpp"


#undef GAMBA_MEM
#define GAMBA_MEM Sim->MEMORY

eio::eio(sim *S)
{
	/* EIO transaction count, i.e., number of last transaction completed */
	eio_trans_icnt = -1;
	Sim = S;
}



FILE *
eio::eio_create(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int target_big_endian;

  target_big_endian = (Sim->ENDIAN->endian_host_byte_order() == endian_big);

  fd = Sim->MISC->gzopen(fname, "w");
  if (!fd)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "unable to create EIO file `%s'", fname);

  /* emit EIO file header */
  fprintf(fd, "%s\n", EIO_FILE_HEADER);
  fprintf(fd, "/* file_format: %d, file_version: %d, big_endian: %d */\n",
	  MD_EIO_FILE_FORMAT, EIO_FILE_VERSION, Sim->LOADER->ld_target_big_endian);
  exo = Sim->LIBEXO->exo_new(ec_list,
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)MD_EIO_FILE_FORMAT),
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)EIO_FILE_VERSION),
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)target_big_endian),
		NULL);
  Sim->LIBEXO->exo_print(exo, fd);
  Sim->LIBEXO->exo_delete(exo);
  fprintf(fd, "\n\n");

  return fd;
}

FILE *
eio::eio_open(char *fname)
{
  FILE *fd;
  struct exo_term_t *exo;
  int file_format, file_version, big_endian, target_big_endian;

  target_big_endian = (Sim->ENDIAN->endian_host_byte_order() == endian_big);

  fd = Sim->MISC->gzopen(fname, "r");
  if (!fd)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "unable to open EIO file `%s'", fname);

  /* read and check EIO file header */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_integer
      || exo->as_list.head->next->next->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO file header");

  file_format = exo->as_list.head->as_integer.val;
  file_version = exo->as_list.head->next->as_integer.val;
  big_endian = exo->as_list.head->next->next->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  if (file_format != MD_EIO_FILE_FORMAT)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO file `%s' has incompatible format", fname);

  if (file_version != EIO_FILE_VERSION)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO file `%s' has incompatible version", fname);

  if (!!big_endian != !!target_big_endian)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO file `%s' has incompatible endian format", fname);

  return fd;
}

/* returns non-zero if file FNAME has a valid EIO header */
int
eio::eio_valid(char *fname)
{
  FILE *fd;
  char buf[512];

  /* open possible EIO file */
  fd = Sim->MISC->gzopen(fname, "r");
  if (!fd)
    return FALSE;

  /* read and check EIO file header */
  fgets(buf, 512, fd);

  /* check the header */
  if (strcmp(buf, EIO_FILE_HEADER))
    return FALSE;

  /* all done, close up file */
  Sim->MISC->gzclose(fd);

  /* else, has a valid header, go with it... */
  return TRUE;
}

void
eio::eio_close(FILE *fd)
{
  Sim->MISC->gzclose(fd);
}

/* check point current architected state to stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio::eio_write_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to write to */
{
  int i;
  struct exo_term_t *exo;
  struct mem_pte_t *pte;

  fprintf(fd, "/* ** start checkpoint @ %d... */\n\n", (int)eio_trans_icnt);

  fprintf(fd, "/* EIO file pointer: %d... */\n", (int)eio_trans_icnt);
  exo = Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)eio_trans_icnt);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  /* dump misc regs: icnt, PC, NPC, etc... */
  fprintf(fd, "/* misc regs icnt, PC, NPC, etc... */\n");
  exo = MD_MISC_REGS_TO_EXO(regs);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  /* dump integer registers */
  fprintf(fd, "/* integer regs */\n");
  exo = Sim->LIBEXO->exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_IREGS; i++)
    exo->as_list.head = Sim->LIBEXO->exo_chain(exo->as_list.head, MD_IREG_TO_EXO(regs, i));
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  /* dump FP registers */
  fprintf(fd, "/* FP regs (integer format) */\n");
  exo = Sim->LIBEXO->exo_new(ec_list, NULL);
  for (i=0; i < MD_NUM_FREGS; i++)
    exo->as_list.head = Sim->LIBEXO->exo_chain(exo->as_list.head, MD_FREG_TO_EXO(regs, i));
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  fprintf(fd, "/* writing `%d' memory pages... */\n", (int)mem->page_count);
  exo = Sim->LIBEXO->exo_new(ec_list,
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)mem->page_count),
		Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)Sim->LOADER->ld_brk_point),
		Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)Sim->LOADER->ld_stack_min),
		NULL);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  fprintf(fd, "/* text segment specifiers (base & size) */\n");
  exo = Sim->LIBEXO->exo_new(ec_list,
		Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)Sim->LOADER->ld_text_base),
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)Sim->LOADER->ld_text_size),
		NULL);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  fprintf(fd, "/* data segment specifiers (base & size) */\n");
  exo = Sim->LIBEXO->exo_new(ec_list,
		Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)Sim->LOADER->ld_data_base),
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)Sim->LOADER->ld_data_size),
		NULL);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  fprintf(fd, "/* stack segment specifiers (base & size) */\n");
  exo = Sim->LIBEXO->exo_new(ec_list,
		Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)Sim->LOADER->ld_stack_base),
		Sim->LIBEXO->exo_new(ec_integer, (exo_integer_t)Sim->LOADER->ld_stack_size),
		NULL);
  Sim->LIBEXO->exo_print(exo, fd);
  fprintf(fd, "\n\n");
  Sim->LIBEXO->exo_delete(exo);

  /* visit all active memory pages, and dump them to the checkpoint file */
  MEM_FORALL(mem, i, pte)
    {
      /* dump this page... */
      exo = Sim->LIBEXO->exo_new(ec_list,
		    Sim->LIBEXO->exo_new(ec_address, (exo_integer_t)MEM_PTE_ADDR(pte, i)),
		    Sim->LIBEXO->exo_new(ec_blob, MD_PAGE_SIZE, pte->page),
		    NULL);
      Sim->LIBEXO->exo_print(exo, fd);
      fprintf(fd, "\n\n");
      Sim->LIBEXO->exo_delete(exo);
    }

  fprintf(fd, "/* ** end checkpoint @ %d... */\n\n", (int)eio_trans_icnt);

  return eio_trans_icnt;
}

/* read check point of architected state from stream FD, returns
   EIO transaction count (an EIO file pointer) */
counter_t
eio::eio_read_chkpt(struct regs_t *regs,		/* regs to dump */
		struct mem_t *mem,		/* memory to dump */
		FILE *fd)			/* stream to read */
{
  int i, page_count, trans_icnt;
  struct exo_term_t *exo, *elt;

  /* read the EIO file pointer */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_integer)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO file pointer");
  trans_icnt = exo->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  /* read misc regs: icnt, PC, NPC, HI, LO, FCC */
  exo = Sim->LIBEXO->exo_read(fd);
  MD_EXO_TO_MISC_REGS(exo, Sim->GLOBAL->sim_num_insn, regs);
  Sim->LIBEXO->exo_delete(exo);

  /* read integer registers */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO integer regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_IREGS; i++)
    {
      if (!elt)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO integer regs (too few)");

      if (elt->ec != ec_address)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO integer regs (bad value)");

      MD_EXO_TO_IREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO integer regs (too many)");
  Sim->LIBEXO->exo_delete(exo);

  /* read FP registers */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO FP regs");
  elt = exo->as_list.head;
  for (i=0; i < MD_NUM_FREGS; i++)
    {
      if (!elt)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO FP regs (too few)");

      if (elt->ec != ec_address)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO FP regs (bad value)");

      MD_EXO_TO_FREG(elt, regs, i);
      elt = elt->next;
    }
  if (elt != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO FP regs (too many)");
  Sim->LIBEXO->exo_delete(exo);

  /* read the number of page defs, and memory config */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_integer
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_address
      || !exo->as_list.head->next->next
      || exo->as_list.head->next->next->ec != ec_address
      || exo->as_list.head->next->next->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO memory page count");
  page_count = exo->as_list.head->as_integer.val;
  Sim->LOADER->ld_brk_point = (md_addr_t)exo->as_list.head->next->as_integer.val;
  Sim->LOADER->ld_stack_min = (md_addr_t)exo->as_list.head->next->next->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  /* read text segment specifiers */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "count not read EIO text segment specifiers");
  Sim->LOADER->ld_text_base = (md_addr_t)exo->as_list.head->as_integer.val;
  Sim->LOADER->ld_text_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  /* read data segment specifiers */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "count not read EIO data segment specifiers");
  Sim->LOADER->ld_data_base = (md_addr_t)exo->as_list.head->as_integer.val;
  Sim->LOADER->ld_data_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  /* read stack segment specifiers */
  exo = Sim->LIBEXO->exo_read(fd);
  if (!exo
      || exo->ec != ec_list
      || !exo->as_list.head
      || exo->as_list.head->ec != ec_address
      || !exo->as_list.head->next
      || exo->as_list.head->next->ec != ec_integer
      || exo->as_list.head->next->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "count not read EIO stack segment specifiers");
  Sim->LOADER->ld_stack_base = (md_addr_t)exo->as_list.head->as_integer.val;
  Sim->LOADER->ld_stack_size = (unsigned int)exo->as_list.head->next->as_integer.val;
  Sim->LIBEXO->exo_delete(exo);

  for (i=0; i < page_count; i++)
    {
      int j;
      md_addr_t page_addr;
      struct exo_term_t *blob;

      /* read the page */
      exo = Sim->LIBEXO->exo_read(fd);
      if (!exo
	  || exo->ec != ec_list
	  || !exo->as_list.head
	  || exo->as_list.head->ec != ec_address
	  || !exo->as_list.head->next
	  || exo->as_list.head->next->ec != ec_blob
	  || exo->as_list.head->next->next != NULL)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not read EIO memory page");
      page_addr = (md_addr_t)exo->as_list.head->as_integer.val;
      blob = exo->as_list.head->next;

      /* write data to simulator memory */
      for (j=0; j < blob->as_blob.size; j++)
	{
	  byte_t val;

	  val = blob->as_blob.data[j];
	  /* unchecked access... */
	  MEM_WRITE_BYTE(mem, page_addr, val);
	  page_addr++;
	}
      Sim->LIBEXO->exo_delete(exo);
    }

  return trans_icnt;
}

#if 0

/* tracing memory access function */
/*static*/ enum md_fault_type
eio::my_mem_fn(struct mem_t *mem,		/* memory space to access */
	  enum mem_cmd cmd,		/* Read (from sim mem) or Write */
	  md_addr_t addr,		/* target address to access */
	  void *vp,			/* host memory address to access */
	  int nbytes)			/* number of bytes to access */
{
  int i;
  unsigned char *p = vp;
  struct mem_rec_t *mem_rec = NULL;
  struct exo_term_t *mem_list = NULL;
  enum md_fault_type fault = md_fault_none;

  if (cmd == Read && seen_write)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "Read after Write in eio_syscall()");

  if (cmd == Write)
    seen_write = TRUE;

  /* record the memory access */
  if (cmd == Read)
    {
      mem_rec = &input_mem_rec;
      mem_list = input_mem;
    }
  else if (cmd == Write)
    {
      mem_rec = &output_mem_rec;
      mem_list = output_mem;
    }
  else
    Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "bogus memory access command");

  /* perform the memory access, Read's first so we can probe *p for data */
  if (cmd == Read /* simulator memory */)
    fault = (*local_mem_fn)(mem, cmd, addr, p, nbytes);

  /* the following freakish code simply coalesces subsequent reads and
     writes to memory into the same EXO blob structure, this greatly
     reduces the size of the EIO output files... */
  if (mem_rec->exo != NULL
      && (mem_rec->addr + mem_rec->size == addr)
      && (mem_rec->size + nbytes < mem_rec->maxsize))
    {
      /* add to last blob */
      for (i=0; i < nbytes; i++)
	mem_rec->blob->as_blob.data[mem_rec->size + i] = p[i];
      mem_rec->size += nbytes;
      mem_rec->blob->as_blob.size = mem_rec->size;
    }
  else
    {
      /* add to a new blob */
      mem_list->as_list.head =
	exo_chain(mem_list->as_list.head,
		  (mem_rec->exo =
		   exo_new(ec_list,
			   exo_new(ec_address, (exo_integer_t)addr),
			   (mem_rec->blob =
			    exo_new(ec_blob, nbytes + BLOB_TAIL_SIZE, NULL)),
			   NULL)));
      for (i=0; i < nbytes; i++)
	mem_rec->blob->as_blob.data[i] = p[i];
      mem_rec->addr = addr;
      mem_rec->size = nbytes;
      mem_rec->maxsize = nbytes + BLOB_TAIL_SIZE;
      mem_rec->blob->as_blob.size = mem_rec->size;
    }

  /* perform the memory access */
  if (cmd == Write /* simulator memory */)
    fault = (*local_mem_fn)(mem, cmd, addr, p, nbytes);

  return fault;
}



/* syscall proxy handler, with EIO tracing support, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio::eio_write_trace(FILE *eio_fd,			/* EIO stream file desc */
		counter_t icnt,			/* instruction count */
		struct regs_t *regs,		/* registers to update */
		mem_access_fn mem_fn,		/* generic memory accessor */
		struct mem_t *mem,		/* memory to update */
		md_inst_t inst)			/* system call inst */
{
  int i;
  struct exo_term_t *exo;

  /* write syscall register inputs ($r2..$r7) */
  input_regs = exo_new(ec_list, NULL);
  for (i=MD_FIRST_IN_REG; i <= MD_LAST_IN_REG; i++)
    {
      input_regs->as_list.head =
	exo_chain(input_regs->as_list.head, MD_IREG_TO_EXO(regs, i));
    }

  /* initialize memory inputs */
  input_mem = exo_new(ec_list, NULL); input_mem_rec.exo = NULL;
  output_mem = exo_new(ec_list, NULL); output_mem_rec.exo = NULL;

  /* perform the system call, record inputs and outputs */
  seen_write = FALSE;
  local_mem_fn = mem_fn;

  if (sim_eio_fd != NULL)
    eio_read_trace(sim_eio_fd, icnt, regs, my_mem_fn, mem, inst);
  else
    {
      sys_syscall(regs, my_mem_fn, mem, inst, FALSE);
    }

  /* write syscall breakpoint and register outputs ($r2..$r7) */
  output_regs = exo_new(ec_list, NULL);
  output_regs->as_list.head =
	exo_chain(output_regs->as_list.head,
		  exo_new(ec_address, (exo_integer_t)ld_brk_point));
  for (i=MD_FIRST_OUT_REG; i <= MD_LAST_OUT_REG; i++)
    {
      output_regs->as_list.head =
	exo_chain(output_regs->as_list.head, MD_IREG_TO_EXO(regs, i));
    }

  /* write the whole enchalada to output stream */
  exo = exo_new(ec_list,
		/* icnt */exo_new(ec_integer, (exo_integer_t)icnt),
		/* PC */exo_new(ec_address, (exo_integer_t)regs->regs_PC),
		input_regs, input_mem,
		output_regs, output_mem,
		NULL);
  exo_print(exo, eio_fd);
  fprintf(eio_fd, "\n\n");

  /* release input storage */
  Sim->LIBEXO->exo_delete(exo);
}

#endif

/* syscall proxy handler from an EIO trace, architect registers
   and memory are assumed to be precise when this function is called,
   register and memory are updated with the results of the sustem call */
void
eio::eio_read_trace(FILE *eio_fd,			/* EIO stream file desc */
	       counter_t icnt,			/* instruction count */
	       struct regs_t *regs,		/* registers to update */
	       /*mem_access_fn mem_fn,*/		/* generic memory accessor */
	       struct mem_t *mem,		/* memory to update */
	       md_inst_t inst)			/* system call inst */
{
  int i;
  struct exo_term_t *exo, *exo_icnt, *exo_pc;
  struct exo_term_t *exo_inregs, *exo_inmem, *exo_outregs, *exo_outmem;
  struct exo_term_t *brkrec, *regrec, *memrec;

  /* exit() system calls get executed for real... */
  if (MD_EXIT_SYSCALL(regs))
    {
      Sim->SYSC->sys_syscall(regs, /*mem_fn,*/ mem, inst, FALSE);
      Sim->MISC->_panic(__FILE__, __FUNCTION__, __LINE__, "returned from exit() system call");
    }

  /* else, read the external I/O (EIO) transaction */
  exo = Sim->LIBEXO->exo_read(eio_fd);

  /* one more transaction processed */
  eio_trans_icnt = icnt;

  /* pull apart the EIO transaction (EXO format) */
  if (!exo
      || exo->ec != ec_list
      || !(exo_icnt = exo->as_list.head)
      || exo_icnt->ec != ec_integer
      || !(exo_pc = exo_icnt->next)
      || exo_pc->ec != ec_address
      || !(exo_inregs = exo_pc->next)
      || exo_inregs->ec != ec_list
      || !(exo_inmem = exo_inregs->next)
      || exo_inmem->ec != ec_list
      || !(exo_outregs = exo_inmem->next)
      || exo_outregs->ec != ec_list
      || !(exo_outmem = exo_outregs->next)
      || exo_outmem->ec != ec_list
      || exo_outmem->next != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot read EIO transaction");

  /*
   * check the system call inputs
   */

  /* check ICNT input */
  if (icnt != (counter_t)exo_icnt->as_integer.val)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: ICNT mismatch");

  /* check PC input */
  if (regs->regs_PC != (md_addr_t)exo_pc->as_integer.val)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: PC mismatch");

  /* check integer register inputs */
  for (i=MD_FIRST_IN_REG, regrec=exo_inregs->as_list.head;
       i <= MD_LAST_IN_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: missing input reg");

      if (MD_EXO_CMP_IREG(regrec, regs, i))
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: R[%d] input mismatch", i);
#ifdef VERBOSE
      fprintf(stderr, "** R[%d] checks out...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: too many input regs");

  /* check memory inputs */
  for (memrec=exo_inmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  unsigned char val;

	  /* was: val = MEM_READ_BYTE(loc); */
	  Sim->MEMORY->mem_access(mem, Read, loc, &val, sizeof(unsigned char));

	  if (val != blob->as_blob.data[i])
	    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: addr 0x%08p input mismatch", loc);

#ifdef VERBOSE
	  Sim->MISC->myfprintf(stderr, "** 0x%08p checks out...\n", loc);
#endif /* VERBOSE */
	}

      /* simulate view'able I/O */
      if (MD_OUTPUT_SYSCALL(regs))
	{
	  if (Sim->GLOBAL->sim_progfd)
	    {
	      /* redirect program output to file */
	      fwrite(blob->as_blob.data, 1, blob->as_blob.size, Sim->GLOBAL->sim_progfd);
	    }
	  else
	    {
	      /* write the output to stdout/stderr */
	      write(MD_STREAM_FILENO(regs),
		    blob->as_blob.data, blob->as_blob.size);
	    }
	}
    }

  /*
   * write system call outputs
   */

  /* adjust breakpoint */
  brkrec = exo_outregs->as_list.head;
  if (!brkrec || brkrec->ec != ec_address)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: missing memory breakpoint");
  Sim->LOADER->ld_brk_point = (md_addr_t)brkrec->as_integer.val;

  /* write integer register outputs */
  for (i=MD_FIRST_OUT_REG, regrec=exo_outregs->as_list.head->next;
       i <= MD_LAST_OUT_REG; i++, regrec=regrec->next)
    {
      if (!regrec || regrec->ec != ec_address)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: missing output reg");

      MD_EXO_TO_IREG(regrec, regs, i);

#ifdef VERBOSE
      fprintf(stderr, "** R[%d] written...\n", i);
#endif /* VERBOSE */
    }
  if (regrec != NULL)
    Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace inconsistency: too many output regs");

  /* write memory outputs */
  for (memrec=exo_outmem->as_list.head; memrec != NULL; memrec=memrec->next)
    {
      md_addr_t loc;
      struct exo_term_t *addr, *blob;

      /* check the mem transaction format */
      if (!memrec
	  || memrec->ec != ec_list
	  || !(addr = memrec->as_list.head)
	  || addr->ec != ec_address
	  || !(blob = addr->next)
	  || blob->ec != ec_blob
	  || blob->next != NULL)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "EIO trace icnonsistency: bad memory transaction");

      for (loc=addr->as_integer.val, i=0; i < blob->as_blob.size; loc++,i++)
	{
	  /* was: MEM_WRITE_BYTE(loc, blob->as_blob.data[i]); */
	Sim->MEMORY->mem_access(mem, Write,
		    loc, &blob->as_blob.data[i], sizeof(unsigned char));

#ifdef VERBOSE
	  fprintf(stderr, "** 0x%08p written...\n", loc);
#endif /* VERBOSE */
	}
    }

  /* release the EIO EXO node */
  Sim->LIBEXO->exo_delete(exo);
}

/* fast forward EIO trace EIO_FD to the transaction just after ICNT */
void
eio::eio_fast_forward(FILE *eio_fd, counter_t icnt)
{
  struct exo_term_t *exo, *exo_icnt;

  do
    {
      /* read the next external I/O (EIO) transaction */
      exo = Sim->LIBEXO->exo_read(eio_fd);

      if (!exo)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "could not fast forward to EIO checkpoint");

      /* one more transaction processed */
      eio_trans_icnt = icnt;

      /* pull apart the EIO transaction (EXO format) */
      if (!exo
	  || exo->ec != ec_list
	  || !(exo_icnt = exo->as_list.head)
	  || exo_icnt->ec != ec_integer)
	Sim->MISC->_fatal(__FILE__, __FUNCTION__, __LINE__, "cannot read EIO transaction (during fast forward)");
    }
  while ((counter_t)exo_icnt->as_integer.val != icnt);

  /* found it! */
}
