/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2002 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.3 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************
 
  simple_bus_master_non_blocking.h : The master using the non-blocking BUS
                                     interface.
 
  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11
 
 *****************************************************************************/
 
/*****************************************************************************
 
  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.
 
      Name, Affiliation, Date:
  Description of Modification:
 
 *****************************************************************************/

#ifndef __simple_bus_master_non_blocking_h
#define __simple_bus_master_non_blocking_h

#include <systemc.h>

#include "simple_bus_types.h"
#include "simple_bus_non_blocking_if.h"


SC_MODULE(simple_bus_master_non_blocking)
{
  // ports
  sc_in_clk clock;
  sc_port<simple_bus_non_blocking_if> bus_port;

  SC_HAS_PROCESS(simple_bus_master_non_blocking);

  // constructor
  simple_bus_master_non_blocking(sc_module_name _name
				 , unsigned int unique_priority
                                 , unsigned int start_address
                                 , bool lock
                                 , int timeout)
    : sc_module(_name)
    , m_unique_priority(unique_priority)
    , m_start_address(start_address)
    , m_lock(lock)
    , m_timeout(timeout)
  {
    // process declaration
    SC_THREAD(main_action);
    sensitive_pos << clock;
  }
  
  // process
  void main_action();

private:
  unsigned int m_unique_priority;
  unsigned int m_start_address;
  bool m_lock;
  int m_timeout;

}; // end class simple_bus_master_non_blocking

#endif
