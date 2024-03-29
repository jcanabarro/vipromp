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

  sc_lv_base.cpp -- Arbitrary size logic vector class.

  Original Author: Gene Bushuyev, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "systemc/datatypes/bit/sc_bit_ids.h"
#include "systemc/datatypes/bit/sc_lv_base.h"


namespace sc_dt
{

// ----------------------------------------------------------------------------
//  CLASS : sc_lv_base
//
//  Arbitrary size logic vector base class.
// ----------------------------------------------------------------------------

static const unsigned long data_array[] =
    { UL_ZERO, ~UL_ZERO, UL_ZERO, ~UL_ZERO };

static const unsigned long ctrl_array[] =
    { UL_ZERO, UL_ZERO, ~UL_ZERO, ~UL_ZERO };


void
sc_lv_base::init( int length_, const sc_logic& init_value )
{
    // check the length
    if( length_ <= 0 ) {
	SC_REPORT_ERROR( SC_ID_ZERO_LENGTH_, 0 );
    }
    // allocate memory for the data and control words
    m_len = length_;
    m_size = (m_len - 1) / UL_SIZE + 1;
    m_data = new unsigned long[m_size * 2];
    m_ctrl = m_data + m_size;
    // initialize the bits to 'init_value'
    unsigned long dw = data_array[init_value.value()];
    unsigned long cw = ctrl_array[init_value.value()];
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	m_data[i] = dw;
	m_ctrl[i] = cw;
    }
    clean_tail();
}


void
sc_lv_base::assign_from_string( const sc_string& s )
{
    // s must have been converted to bin
    int len = m_len;
    int s_len = s.length() - 1;
    int min_len = sc_min( len, s_len );
    int i = 0;
    for( ; i < min_len; ++ i ) {
	char c = s[s_len - i - 1];
	set_bit( i, sc_logic::char_to_logic[c] );
    }
    // if formatted, fill the rest with sign(s), otherwise fill with zeros
    sc_logic_value_t fill = (s[s_len] == 'F' ? sc_logic_value_t( s[0] - '0' )
		                             : sc_logic_value_t( 0 ));
    for( ; i < len; ++ i ) {
	set_bit( i, fill );
    }
}


// constructors

sc_lv_base::sc_lv_base( const char* a )
    : m_len( 0 ), m_size( 0 ), m_data( 0 ), m_ctrl( 0 )
{
    sc_string s = convert_to_bin( a );
    init( s.length() - 1 );
    assign_from_string( s );
}

sc_lv_base::sc_lv_base( const char* a, int length_ )
    : m_len( 0 ), m_size( 0 ), m_data( 0 ), m_ctrl( 0 )
{
    init( length_ );
    assign_from_string( convert_to_bin( a ) );
}

sc_lv_base::sc_lv_base( const sc_lv_base& a )
    : m_len( a.m_len ),
      m_size( a.m_size ),
      m_data( new unsigned long[m_size * 2] ),
      m_ctrl( m_data + m_size )
{
    // copy the bits
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	m_data[i] = a.m_data[i];
	m_ctrl[i] = a.m_ctrl[i];
    }
}


// assignment operators

sc_lv_base&
sc_lv_base::operator = ( const char* a )
{
    assign_from_string( convert_to_bin( a ) );
    return *this;
}


// returns true if logic vector contains only 0's and 1's

bool
sc_lv_base::is_01() const
{
    int sz = m_size;
    for( int i = 0; i < sz; ++ i ) {
	if( m_ctrl[i] != 0 ) {
	    return false;
	}
    }
    return true;
}

} // namespace sc_dt
