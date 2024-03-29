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

  sc_int.h -- A sc_int is a signed integer whose length is less than the
              machine's native integer length. We provide two implementations
              (i) sc_int with length between 1 - 64, and (ii) sc_int with
              length between 1 - 32. Implementation (i) is the default
              implementation, while implementation (ii) can be used only if
              the class library is compiled with -D_32BIT_. Unlike arbitrary
              precision, arithmetic and bitwise operations are performed
              using the native types (hence capped at 32/64 bits). The sc_int
              integer is useful when the user does not need arbitrary
              precision and the performance is superior to
              sc_bigint/sc_biguint.

  Original Author: Amit Rao, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Ali Dasdan, Synopsys, Inc.
  Description of Modification: - Resolved ambiguity with sc_(un)signed.
                               - Merged the code for 64- and 32-bit versions
                                 via the constants in sc_nbdefs.h.
                               - Eliminated redundant file inclusions.
    
      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef SC_INT_H
#define SC_INT_H


#include "systemc/datatypes/int/sc_int_base.h"


namespace sc_dt
{

// classes defined in this module
template <int W> class sc_int;


// ----------------------------------------------------------------------------
//  CLASS TEMPLATE : sc_int<W>
//
//  Template class sc_int<W> is the interface that the user sees. It is
//  derived from sc_int_base and most of its methods are just wrappers
//  that call the corresponding method in the parent class. Note that
//  the length of sc_int datatype is specified as a template parameter.
// ----------------------------------------------------------------------------

template <int W>
class sc_int
    : public sc_int_base
{
public:

    // constructors

    sc_int()
	: sc_int_base( W )
	{}

    sc_int( int_type v )
	: sc_int_base( v, W )
	{}

    sc_int( const sc_int<W>& a )
	: sc_int_base( a )
	{}

    sc_int( const sc_int_base& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( const sc_int_subref_r& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    template <class T1, class T2>
    sc_int( const sc_int_concref_r<T1,T2>& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( const sc_signed& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( const sc_unsigned& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

#ifdef SC_INCLUDE_FX

    explicit sc_int( const sc_fxval& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    explicit sc_int( const sc_fxval_fast& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    explicit sc_int( const sc_fxnum& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    explicit sc_int( const sc_fxnum_fast& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

#endif

    sc_int( const sc_bv_base& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( const sc_lv_base& a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( const char* a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( unsigned long a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( long a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( unsigned int a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( int a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( uint64 a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }

    sc_int( double a )
	: sc_int_base( W )
	{ sc_int_base::operator = ( a ); }


    // assignment operators

    sc_int<W>& operator = ( int_type v )
	{ sc_int_base::operator = ( v ); return *this; }

    sc_int<W>& operator = ( const sc_int_base& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_int_subref_r& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_int<W>& a )
	{ m_val = a.m_val; return *this; }

    template <class T1, class T2>
    sc_int<W>& operator = ( const sc_int_concref_r<T1,T2>& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_signed& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_unsigned& a )
	{ sc_int_base::operator = ( a ); return *this; }

#ifdef SC_INCLUDE_FX

    sc_int<W>& operator = ( const sc_fxval& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_fxval_fast& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_fxnum& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_fxnum_fast& a )
	{ sc_int_base::operator = ( a ); return *this; }

#endif

    sc_int<W>& operator = ( const sc_bv_base& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const sc_lv_base& a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( const char* a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( unsigned long a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( long a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( unsigned int a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( int a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( uint64 a )
	{ sc_int_base::operator = ( a ); return *this; }

    sc_int<W>& operator = ( double a )
	{ sc_int_base::operator = ( a ); return *this; }


    // arithmetic assignment operators

    sc_int<W>& operator += ( int_type v )
	{ sc_int_base::operator += ( v ); return *this; }

    sc_int<W>& operator -= ( int_type v )
	{ sc_int_base::operator -= ( v ); return *this; }

    sc_int<W>& operator *= ( int_type v )
	{ sc_int_base::operator *= ( v ); return *this; }

    sc_int<W>& operator /= ( int_type v )
	{ sc_int_base::operator /= ( v ); return *this; }

    sc_int<W>& operator %= ( int_type v )
	{ sc_int_base::operator %= ( v ); return *this; }


    // bitwise assignment operators

    sc_int<W>& operator &= ( int_type v )
	{ sc_int_base::operator &= ( v ); return *this; }

    sc_int<W>& operator |= ( int_type v )
	{ sc_int_base::operator |= ( v ); return *this; }

    sc_int<W>& operator ^= ( int_type v )
	{ sc_int_base::operator ^= ( v ); return *this; }


    sc_int<W>& operator <<= ( int_type v )
	{ sc_int_base::operator <<= ( v ); return *this; }

    sc_int<W>& operator >>= ( int_type v )
	{ sc_int_base::operator >>= ( v ); return *this; }


    // prefix and postfix increment and decrement operators

    sc_int<W>& operator ++ () // prefix
	{ sc_int_base::operator ++ (); return *this; }

    const sc_int<W> operator ++ ( int ) // postfix
	{ return sc_int<W>( sc_int_base::operator ++ ( 0 ) ); }

    sc_int<W>& operator -- () // prefix
	{ sc_int_base::operator -- (); return *this; }

    const sc_int<W> operator -- ( int ) // postfix
	{ return sc_int<W>( sc_int_base::operator -- ( 0 ) ); }
};

} // namespace sc_dt


#endif

// Taf!
