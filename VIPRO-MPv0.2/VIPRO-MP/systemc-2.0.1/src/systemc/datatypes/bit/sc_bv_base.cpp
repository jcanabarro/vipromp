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

  sc_bv_base.cpp -- Arbitrary size bit vector class.

  Original Author: Gene Bushuyev, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "systemc/datatypes/bit/sc_bit_ids.h"
#include "systemc/datatypes/bit/sc_bv_base.h"
#include "systemc/datatypes/fx/sc_fix.h"
#include "systemc/datatypes/fx/sc_ufix.h"
#include "systemc/utils/sc_exception.h"


namespace sc_dt
{

// ----------------------------------------------------------------------------
//  CLASS : sc_bv_base
//
//  Arbitrary size bit vector base class.
// ----------------------------------------------------------------------------

void
sc_bv_base::init( int length_, bool init_value )
{
    // check the length
    if( length_ <= 0 ) {
	SC_REPORT_ERROR( SC_ID_ZERO_LENGTH_, 0 );
    }
    // allocate memory for the data and control words
    m_len = length_;
    m_size = (m_len - 1) / UL_SIZE + 1;
    m_data = new unsigned long[m_size];
    // initialize the bits to 'init_value'
    unsigned long dw = init_value ? ~UL_ZERO : UL_ZERO;
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	m_data[i] = dw;
    }
    clean_tail();
}


void
sc_bv_base::assign_from_string( const sc_string& s )
{
    // s must have been converted to bin
    int len = m_len;
    int s_len = s.length() - 1;
    int min_len = sc_min( len, s_len );
    int i = 0;
    for( ; i < min_len; ++ i ) {
	char c = s[s_len - i - 1];
	if( c != '0' && c != '1' ) {
	    SC_REPORT_ERROR( SC_ID_CANNOT_CONVERT_,
	        "string can contain only '0' and '1' characters" );
	}
	set_bit( i, sc_logic_value_t( c - '0' ) );
    }
    // if formatted, fill the rest with sign(s), otherwise fill with zeros
    sc_logic_value_t fill = (s[s_len] == 'F' ? sc_logic_value_t( s[0] - '0' )
		                             : sc_logic_value_t( 0 ));
    for( ; i < len; ++ i ) {
	set_bit( i, fill );
    }
}


// constructors

sc_bv_base::sc_bv_base( const char* a )
    : m_len( 0 ), m_size( 0 ), m_data( 0 )
{
    sc_string s = convert_to_bin( a );
    init( s.length() -  1 );
    assign_from_string( s );
}

sc_bv_base::sc_bv_base( const char* a, int length_ )
    : m_len( 0 ), m_size( 0 ), m_data( 0 )
{
    init( length_ );
    assign_from_string( convert_to_bin( a ) );
}

sc_bv_base::sc_bv_base( const sc_bv_base& a )
    : m_len( a.m_len ),
      m_size( a.m_size ),
      m_data( new unsigned long[m_size] )
{
    // copy the bits
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	m_data[i] = a.m_data[i];
    }
}


// assignment operators

sc_bv_base&
sc_bv_base::operator = ( const char* a )
{
    assign_from_string( convert_to_bin( a ) );
    return *this;
}


#if 0

// bitwise complement

sc_bv_base&
sc_bv_base::b_not()
{
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	m_data[i] = ~m_data[i];
    }
    clean_tail();
    return *this;
}


// bitwise left shift

sc_bv_base&
sc_bv_base::operator <<= ( int n )
{
    if( n < 0 ) {
	char msg[BUFSIZ];
	sprintf( msg,
		 "left shift operation is only allowed with positive "
		 "shift values, shift value = %d", n );
	SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, msg );
    }
    int sz = m_size;
    if( n >= m_len ) {
	for( int i = 0; i < sz; ++ i ) {
	    m_data[i] = UL_ZERO;
	}
	// clean_tail();
	return *this;
    }
    int wn = n / UL_SIZE;
    int bn = n % UL_SIZE;
    if( wn != 0 ) {
	// shift words
	int i = sz - 1;
	for( ; i >= wn; -- i ) {
	    m_data[i] = m_data[i - wn];
	}
	for( ; i >= 0; -- i ) {
	    m_data[i] = UL_ZERO;
	}
    }
    if( bn != 0 ) {
	// shift bits
	for( int i = sz - 1; i >= 1; -- i ) {
	    m_data[i] <<= bn;
	    m_data[i] |= m_data[i - 1] >> (UL_SIZE - bn);
	}
	m_data[0] <<= bn;
    }
    clean_tail();
    return *this;
}


// bitwise right shift

sc_bv_base&
sc_bv_base::operator >>= ( int n )
{
    if( n < 0 ) {
	char msg[BUFSIZ];
	sprintf( msg,
		 "right shift operation is only allowed with positive "
		 "shift values, shift value = %d", n );
	SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, msg );
    }
    int sz = m_size;
    if( n >= m_len ) {
	for( int i = 0; i < sz; ++ i ) {
	    m_data[i] = UL_ZERO;
	}
	// clean_tail();
	return *this;
    }
    int wn = n / UL_SIZE;
    int bn = n % UL_SIZE;
    if( wn != 0 ) {
	// shift words
	int i = 0;
	for( ; i < (sz - wn); ++ i ) {
	    m_data[i] = m_data[i + wn];
	}
	for( ; i < sz; ++ i ) {
	    m_data[i] = UL_ZERO;
	}
    }
    if( bn != 0 ) {
	// shift bits
	for( int i = 0; i < (sz - 1); ++ i ) {
	    m_data[i] >>= bn;
	    m_data[i] |= m_data[i + 1] << (UL_SIZE - bn);
	}
	m_data[sz - 1] >>= bn;
    }
    clean_tail();
    return *this;
}


// bitwise left rotate

sc_bv_base&
sc_bv_base::lrotate( int n )
{
    if( n < 0 ) {
	char msg[BUFSIZ];
	sprintf( msg,
		 "left rotate operation is only allowed with positive "
		 "rotate values, rotate value = %d", n );
	SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, msg );
    }
    int len = m_len;
    n %= len;
    *this = (*this << n) | (*this >> (len - n));
    return *this;
}


// bitwise right rotate

sc_bv_base&
sc_bv_base::rrotate( int n )
{
    if( n < 0 ) {
	char msg[BUFSIZ];
	sprintf( msg,
		 "right rotate operation is only allowed with positive "
		 "rotate values, rotate value = %d", n );
	SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, msg );
    }
    int len = m_len;
    n %= len;
    *this = (*this >> n) | (*this << (len - n));
    return *this;
}

#endif


// ----------------------------------------------------------------------------

// convert formatted string to binary string

const sc_string
convert_to_bin( const char* s )
{
    // Beware: logic character strings cannot start with '0x' or '0X',
    //         because this is seen as a hexadecimal encoding prefix!

    if( s == 0 ) {
	SC_REPORT_ERROR( SC_ID_CANNOT_CONVERT_, "character string is zero" );
    }
    if( *s == 0 ) {
	SC_REPORT_ERROR( SC_ID_CANNOT_CONVERT_, "character string is empty" );
    }

    int n = strlen( s );
    int i = 0;
    if( s[0] == '-' || s[0] == '+' ) {
	++ i;
    }
    if( n > (i + 2) && s[i] == '0' &&
	(s[i+1] == 'b' || s[i+1] == 'B' ||
	 s[i+1] == 'c' || s[i+1] == 'C' ||
	 s[i+1] == 'd' || s[i+1] == 'D' ||
	 s[i+1] == 'o' || s[i+1] == 'O' ||
	 s[i+1] == 'x' || s[i+1] == 'X') )
    {
	try {
	    // worst case length = n * 4
	    sc_fix a( s, n * 4, n * 4, SC_TRN, SC_WRAP, 0, SC_ON );
	    sc_string str = a.to_bin();
	    str += "F"; // mark the string as formatted
	    // get rid of prefix (0b) and redundant leading bits
	    const char* p = str.c_str() + 2;
	    while( p[1] && p[0] == p[1] ) {
		++ p;
	    }
	    return sc_string( p );
	} catch( sc_exception ) {
	    char msg[BUFSIZ];
	    sprintf( msg, "character string '%s' is not valid", s );
	    SC_REPORT_ERROR( SC_ID_CANNOT_CONVERT_, msg );
	    // never reached
	    return sc_string();
	}
    }
    else {
	// bin by default
	sc_string str( s );
	str += "U"; // mark the string as unformatted
	return str;
    }
}

// convert binary string to formatted string

const sc_string
convert_to_fmt( const sc_string& s, sc_numrep numrep, bool w_prefix )
{
    int n = s.length();
    sc_string str = "0bus" + s;
    sc_ufix a( str.c_str(), n, n, SC_TRN, SC_WRAP, 0, SC_ON );
    return a.to_string( numrep, w_prefix );
}

} // namespace sc_dt
