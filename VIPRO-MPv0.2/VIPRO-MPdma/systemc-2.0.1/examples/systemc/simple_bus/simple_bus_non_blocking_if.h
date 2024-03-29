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
 
  simple_bus_non_blocking_if.h : The non-blocking BUS interface.
 
  Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11
 
 *****************************************************************************/
 
/*****************************************************************************
 
  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.
 
      Name, Affiliation, Date:
  Description of Modification:
 
 *****************************************************************************/

#ifndef __simple_bus_non_blocking_if_h
#define __simple_bus_non_blocking_if_h

#include <systemc.h>

#include "simple_bus_types.h"

class simple_bus_non_blocking_if
  : public virtual sc_interface
{
public:
  // non-blocking BUS interface
  virtual void read(unsigned int unique_priority
		    , int *data
		    , unsigned int address
		    , bool lock = false) = 0;
  virtual void write(unsigned int unique_priority
		     , int *data
		     , unsigned int address
		     , bool lock = false) = 0;

  virtual simple_bus_status get_status(unsigned int unique_priority) = 0;

}; // end class simple_bus_non_blocking_if

#endif
