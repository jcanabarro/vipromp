## ****************************************************************************
##
##  The following code is derived, directly or indirectly, from the SystemC
##  source code Copyright (c) 1996-2002 by all Contributors.
##  All Rights reserved.
##
##  The contents of this file are subject to the restrictions and limitations
##  set forth in the SystemC Open Source License Version 2.3 (the "License");
##  You may not use this file except in compliance with such restrictions and
##  limitations. You may obtain instructions on how to receive a copy of the
##  License at http://www.systemc.org/. Software distributed by Contributors
##  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
##  ANY KIND, either express or implied. See the License for the specific
##  language governing rights and limitations under the License.
##
## ****************************************************************************
##
##  src/systemc/kernel/Makefile.am --
##  Process this file with automake to produce a Makefile.in file.
##
##  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21
##
## ****************************************************************************
##
##  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
##  changes you are making here.
##
##      Name, Affiliation, Date:
##  Description of Modification:
##
## ****************************************************************************

include $(top_srcdir)/config/Make-rules

noinst_LIBRARIES = libkernel.a

H_FILES = \
	sc_attribute.h \
	sc_cmnhdr.h \
	sc_constants.h \
	sc_cor.h \
	sc_event.h \
	sc_externs.h \
	sc_kernel_ids.h \
	sc_lambda.h \
	sc_lambda_defs.h \
	sc_lambda_exps.h \
	sc_lambda_friends.h \
	sc_macros.h \
	sc_module.h \
	sc_module_name.h \
	sc_object.h \
	sc_process.h \
	sc_process_b.h \
	sc_runnable.h \
	sc_sensitive.h \
	sc_simcontext.h \
	sc_time.h \
	sc_ver.h \
	sc_wait.h \
	sc_wait_cthread.h

NO_H_FILES = \
	sc_cor_fiber.h \
	sc_cor_qt.h \
	sc_except.h \
	sc_macros_int.h \
	sc_module_registry.h \
	sc_name_gen.h \
	sc_object_manager.h \
	sc_process_int.h \
	sc_simcontext_int.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sc_attribute.cpp \
	sc_cor_fiber.cpp \
	sc_cor_qt.cpp \
	sc_event.cpp \
	sc_kernel_ids.cpp \
	sc_lambda.cpp \
	sc_main.cpp \
	sc_module.cpp \
	sc_module_name.cpp \
	sc_module_registry.cpp \
	sc_name_gen.cpp \
	sc_object.cpp \
	sc_object_manager.cpp \
	sc_process_int.cpp \
	sc_runnable.cpp \
	sc_sensitive.cpp \
	sc_simcontext.cpp \
	sc_time.cpp \
	sc_ver.cpp \
	sc_wait.cpp \
	sc_wait_cthread.cpp

NO_CXX_FILES =

libkernel_a_SOURCES = $(H_FILES) $(CXX_FILES)

INCLUDE_DIR = $(prefix)/include/systemc/kernel

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(INCLUDE_DIR)
	for file in $(noinst_HEADERS); do \
	  $(INSTALL_DATA) $(srcdir)/$$file $(INCLUDE_DIR)/$$file; \
	done
	$(AR_TOOL) $(top_builddir)/src/$(MAIN_LIBRARY) \
	  $(libkernel_a_OBJECTS)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	for file in $(noinst_HEADERS); do \
	  rm -f $(INCLUDE_DIR)/$$file; \
	done
	-rmdir $(INCLUDE_DIR)

## Taf!
