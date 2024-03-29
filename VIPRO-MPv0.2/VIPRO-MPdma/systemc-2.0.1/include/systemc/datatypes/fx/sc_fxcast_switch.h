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

  sc_fxcast_switch.h - 

  Original Author: Martin Janssen, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef SC_FXCAST_SWITCH_H
#define SC_FXCAST_SWITCH_H


#include "systemc/datatypes/fx/sc_context.h"


namespace sc_dt
{

// classes defined in this module
class sc_fxcast_switch;


// ----------------------------------------------------------------------------
//  CLASS : sc_fxcast_switch
//
//  Fixed-point cast switch class.
// ----------------------------------------------------------------------------

class sc_fxcast_switch
{

public:

             sc_fxcast_switch();
             sc_fxcast_switch( sc_switch );
             sc_fxcast_switch( const sc_fxcast_switch& );
    explicit sc_fxcast_switch( sc_without_context );

    sc_fxcast_switch& operator = ( const sc_fxcast_switch& );

    friend bool operator == ( const sc_fxcast_switch&,
			      const sc_fxcast_switch& );
    friend bool operator != ( const sc_fxcast_switch&,
			      const sc_fxcast_switch& );

    const sc_string to_string() const;

    void print( ostream& = cout ) const;
    void dump( ostream& = cout ) const;

private:

    sc_switch m_sw;

};


// ----------------------------------------------------------------------------
//  TYPEDEF : sc_fxcast_context
//
//  Context type for the fixed-point cast switch parameter.
// ----------------------------------------------------------------------------

typedef sc_context<sc_fxcast_switch> sc_fxcast_context;


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

inline
sc_fxcast_switch::sc_fxcast_switch()
{
    *this = sc_fxcast_context::default_value();
}

inline
sc_fxcast_switch::sc_fxcast_switch( sc_switch sw_ )
: m_sw( sw_ )
{}

inline
sc_fxcast_switch::sc_fxcast_switch( const sc_fxcast_switch& a )
: m_sw( a.m_sw )
{}

inline
sc_fxcast_switch::sc_fxcast_switch( sc_without_context )
: m_sw( SC_DEFAULT_CAST_SWITCH_ )
{}


inline
sc_fxcast_switch&
sc_fxcast_switch::operator = ( const sc_fxcast_switch& a )
{
    if( &a != this )
    {
        m_sw = a.m_sw;
    }
    return *this;
}


inline
bool
operator == ( const sc_fxcast_switch& a, const sc_fxcast_switch& b )
{
    return ( a.m_sw == b.m_sw );
}


inline
bool
operator != ( const sc_fxcast_switch& a, const sc_fxcast_switch& b )
{
    return ( a.m_sw != b.m_sw );
}


inline
ostream&
operator << ( ostream& os, const sc_fxcast_switch& a )
{
    a.print( os );
    return os;
}

} // namespace sc_dt


#endif

// Taf!
