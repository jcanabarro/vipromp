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

  sc_uint.h -- A sc_uint is an unsigned integer whose length is less than the
               machine's native integer length. We provide two implementations
               (i) sc_uint with length between 1 - 64, and (ii) sc_uint with
               length between 1 - 32. Implementation (i) is the default
               implementation, while implementation (ii) can be used only if
               compiled with -D_32BIT_. Unlike arbitrary precision, arithmetic
               and bitwise operations are performed using the native types
               (hence capped at 32/64 bits). The sc_uint integer is useful
               when the user does not need arbitrary precision and the
               performance is superior to sc_bigint/sc_biguint.

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

#ifndef SC_UINT_H
#define SC_UINT_H


#include "systemc/datatypes/int/sc_uint_base.h"


namespace sc_dt
{

// classes defined in this module
template <int W> class sc_uint;


// ----------------------------------------------------------------------------
//  CLASS TEMPLATE : sc_uint<W>
//
//  Template class sc_uint<W> is the interface that the user sees. It
//  is derived from sc_uint_base and most of its methods are just
//  wrappers that call the corresponding method in the parent
//  class. Note that the length of sc_uint datatype is specified as a
//  template parameter.
// ----------------------------------------------------------------------------

template <int W>
class sc_uint
    : public sc_uint_base
{
public:

    // constructors

    sc_uint()
	: sc_uint_base( W )
	{}

    sc_uint( uint_type v )
	: sc_uint_base( v, W )
	{}

    sc_uint( const sc_uint<W>& a )
	: sc_uint_base( a )
	{}

    sc_uint( const sc_uint_base& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( const sc_uint_subref_r& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    template <class T1, class T2>
    sc_uint( const sc_uint_concref_r<T1,T2>& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( const sc_signed& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( const sc_unsigned& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

#ifdef SC_INCLUDE_FX

    explicit sc_uint( const sc_fxval& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    explicit sc_uint( const sc_fxval_fast& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    explicit sc_uint( const sc_fxnum& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    explicit sc_uint( const sc_fxnum_fast& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

#endif

    sc_uint( const sc_bv_base& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( const sc_lv_base& a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( const char* a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( unsigned long a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( long a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( unsigned int a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( int a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( int64 a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }

    sc_uint( double a )
	: sc_uint_base( W )
	{ sc_uint_base::operator = ( a ); }


    // assignment operators

    sc_uint<W>& operator = ( uint_type v )
	{ sc_uint_base::operator = ( v ); return *this; }

    sc_uint<W>& operator = ( const sc_uint_base& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_uint_subref_r& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_uint<W>& a )
	{ m_val = a.m_val; return *this; }

    template <class T1, class T2>
    sc_uint<W>& operator = ( const sc_uint_concref_r<T1,T2>& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_signed& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_unsigned& a )
	{ sc_uint_base::operator = ( a ); return *this; }

#ifdef SC_INCLUDE_FX

    sc_uint<W>& operator = ( const sc_fxval& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_fxval_fast& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_fxnum& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_fxnum_fast& a )
	{ sc_uint_base::operator = ( a ); return *this; }

#endif

    sc_uint<W>& operator = ( const sc_bv_base& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const sc_lv_base& a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( const char* a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( unsigned long a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( long a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( unsigned int a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( int a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( int64 a )
	{ sc_uint_base::operator = ( a ); return *this; }

    sc_uint<W>& operator = ( double a )
	{ sc_uint_base::operator = ( a ); return *this; }


    // arithmetic assignment operators

    sc_uint<W>& operator += ( uint_type v )
	{ sc_uint_base::operator += ( v ); return *this; }

    sc_uint<W>& operator -= ( uint_type v )
	{ sc_uint_base::operator -= ( v ); return *this; }

    sc_uint<W>& operator *= ( uint_type v )
	{ sc_uint_base::operator *= ( v ); return *this; }

    sc_uint<W>& operator /= ( uint_type v )
	{ sc_uint_base::operator /= ( v ); return *this; }

    sc_uint<W>& operator %= ( uint_type v )
	{ sc_uint_base::operator %= ( v ); return *this; }

  
    // bitwise assignment operators

    sc_uint<W>& operator &= ( uint_type v )
	{ sc_uint_base::operator &= ( v ); return *this; }

    sc_uint<W>& operator |= ( uint_type v )
	{ sc_uint_base::operator |= ( v ); return *this; }

    sc_uint<W>& operator ^= ( uint_type v )
	{ sc_uint_base::operator ^= ( v ); return *this; }


    sc_uint<W>& operator <<= ( uint_type v )
	{ sc_uint_base::operator <<= ( v ); return *this; }

    sc_uint<W>& operator >>= ( uint_type v )
	{ sc_uint_base::operator >>= ( v ); return *this; }


    // prefix and postfix increment and decrement operators

    sc_uint<W>& operator ++ () // prefix
	{ sc_uint_base::operator ++ (); return *this; }

    const sc_uint<W> operator ++ ( int ) // postfix
	{ return sc_uint<W>( sc_uint_base::operator ++ ( 0 ) ); }

    sc_uint<W>& operator -- () // prefix
	{ sc_uint_base::operator -- (); return *this; }

    const sc_uint<W> operator -- ( int ) // postfix
	{ return sc_uint<W>( sc_uint_base::operator -- ( 0 ) ); }
};

} // namespace sc_dt


#endif

// Taf!
