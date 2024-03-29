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

  sc_fxval_observer.cpp - 

  Original Author: Martin Janssen, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "systemc/datatypes/fx/sc_fxval_observer.h"


namespace sc_dt
{

// ----------------------------------------------------------------------------
//  CLASS : sc_fxval_observer
//
//  Abstract base class for fixed-point value type observers;
//  arbitrary precision.
// ----------------------------------------------------------------------------

sc_fxval_observer* (*sc_fxval_observer::default_observer) () = 0;


// ----------------------------------------------------------------------------
//  CLASS : sc_fxval_fast_observer
//
//  Abstract base class for fixed-point value type observers;
//  limited precision.
// ----------------------------------------------------------------------------

sc_fxval_fast_observer* (*sc_fxval_fast_observer::default_observer) () = 0;

} // namespace sc_dt


// Taf!
