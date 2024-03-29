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

  sc_unsigned_bitref.h -- Proxy class that is declared in sc_unsigned.h.  

  Original Author: Ali Dasdan, Synopsys, Inc.
 
 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


// ----------------------------------------------------------------------------
//  CLASS : sc_unsigned_bitref_r
//
//  Proxy class for sc_unsigned bit selection (r-value only).
// ----------------------------------------------------------------------------

// implicit conversion to bool

sc_unsigned_bitref_r::operator bool () const
{
    return m_obj.test( m_index );
}

bool
sc_unsigned_bitref_r::operator ! () const
{
    return ( ! m_obj.test( m_index ) );
}

bool
sc_unsigned_bitref_r::operator ~ () const
{
    return ( ! m_obj.test( m_index ) );
}


// ----------------------------------------------------------------------------
//  CLASS : sc_unsigned_bitref
//
//  Proxy class for sc_unsigned bit selection (r-value and l-value).
// ----------------------------------------------------------------------------

// assignment operators

sc_unsigned_bitref&
sc_unsigned_bitref::operator = ( const sc_unsigned_bitref_r& b )
{
    m_obj.set( m_index, (bool) b );
    return *this;
}

sc_unsigned_bitref&
sc_unsigned_bitref::operator = ( const sc_unsigned_bitref& b )
{
    m_obj.set( m_index, (bool) b );
    return *this;
}

sc_unsigned_bitref&
sc_unsigned_bitref::operator = ( bool b )
{
    m_obj.set( m_index, b );
    return *this;
}


sc_unsigned_bitref&
sc_unsigned_bitref::operator &= ( bool b )
{
    if( ! b ) {
	m_obj.clear( m_index );
    }
    return *this;
}

sc_unsigned_bitref&
sc_unsigned_bitref::operator |= ( bool b )
{
    if( b ) {
	m_obj.set( m_index );
    }
    return *this;
}

sc_unsigned_bitref&
sc_unsigned_bitref::operator ^= ( bool b )
{
    if( b ) {
	m_obj.invert( m_index );
    }
    return *this;
}


// other methods

void
sc_unsigned_bitref::scan( istream& is )
{
    bool b;
    is >> b;
    *this = b;
}


// End of file
