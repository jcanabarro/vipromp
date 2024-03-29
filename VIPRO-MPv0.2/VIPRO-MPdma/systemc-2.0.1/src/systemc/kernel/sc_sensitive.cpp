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

  sc_sensitive.cpp --

  Original Author: Stan Y. Liao, Synopsys, Inc.
                   Martin Janssen, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "systemc/kernel/sc_event.h"
#include "systemc/kernel/sc_kernel_ids.h"
#include "systemc/kernel/sc_module.h"
#include "systemc/kernel/sc_process_int.h"
#include "systemc/kernel/sc_sensitive.h"
#include "systemc/communication/sc_signal_ports.h"


// support functions

static
sc_method_handle
as_method_handle( sc_process_b* handle_ )
{
    return DCAST<sc_method_handle>( handle_ );
}

static
sc_thread_handle
as_thread_handle( sc_process_b* handle_ )
{
    return DCAST<sc_thread_handle>( handle_ );
}


// ----------------------------------------------------------------------------
//  CLASS : sc_sensitive
//
//  Static sensitivity class for events.
// ----------------------------------------------------------------------------

// constructor

sc_sensitive::sc_sensitive( sc_module* module_ )
: m_module( module_ ),
  m_mode( SC_NONE_ ),
  m_handle( 0 )
{}


// destructor

sc_sensitive::~sc_sensitive()
{}


// changing between process handles

sc_sensitive&
sc_sensitive::operator << ( sc_method_handle handle_ )
{
    m_mode = SC_METHOD_;
    m_handle = handle_;
    return *this;
}

sc_sensitive&
sc_sensitive::operator << ( sc_thread_handle handle_ )
{
    m_mode = SC_THREAD_;
    m_handle = handle_;
    return *this;
}


sc_sensitive&
sc_sensitive::operator () ( const sc_event& event_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( event_ );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( const sc_interface& interface_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( interface_.default_event() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( const sc_port_base& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ) );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ) );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_event_finder& event_finder_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	event_finder_.port().make_sensitive( as_method_handle( m_handle ),
					     &event_finder_ );
	break;
    }
    case SC_THREAD_: {
	event_finder_.port().make_sensitive( as_thread_handle( m_handle ),
					     &event_finder_ );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}


sc_sensitive&
sc_sensitive::operator << ( const sc_event& event_ )
{
    return operator () ( event_ );
}

sc_sensitive&
sc_sensitive::operator << ( const sc_interface& interface_ )
{
    return operator () ( interface_ );
}

sc_sensitive&
sc_sensitive::operator << ( const sc_port_base& port_ )
{
    return operator () ( port_ );
}

sc_sensitive&
sc_sensitive::operator << ( sc_event_finder& event_finder_ )
{
    return operator () ( event_finder_ );
}


sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    sc_event_finder& event_finder_ )
{
    event_finder_.port().make_sensitive( handle_, &event_finder_ );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const in_if_b_type& interface_ )
{
    handle_->add_static_event( interface_.posedge_event() );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const in_if_l_type& interface_ )
{
    handle_->add_static_event( interface_.posedge_event() );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const in_port_b_type& port_ )
{
    port_.make_sensitive( handle_, &port_.pos() );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const in_port_l_type& port_ )
{
    port_.make_sensitive( handle_, &port_.pos() );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const inout_port_b_type& port_ )
{
    port_.make_sensitive( handle_, &port_.pos() );
    return *this;
}

sc_sensitive&
sc_sensitive::operator () ( sc_cthread_handle handle_,
			    const inout_port_l_type& port_ )
{
    port_.make_sensitive( handle_, &port_.pos() );
    return *this;
}


// ----------------------------------------------------------------------------
//  CLASS : sc_sensitive_pos
//
//  Static sensitivity class for positive edge events.
// ----------------------------------------------------------------------------

// constructor

sc_sensitive_pos::sc_sensitive_pos( sc_module* module_ )
: m_module( module_ ),
  m_mode( SC_NONE_ ),
  m_handle( 0 )
{}


// destructor

sc_sensitive_pos::~sc_sensitive_pos()
{}


// changing between process handles

sc_sensitive_pos&
sc_sensitive_pos::operator << ( sc_method_handle handle_ )
{
    m_mode = SC_METHOD_;
    m_handle = handle_;
    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( sc_thread_handle handle_ )
{
    m_mode = SC_THREAD_;
    m_handle = handle_;
    return *this;
}


sc_sensitive_pos&
sc_sensitive_pos::operator () ( const in_if_b_type& interface_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( interface_.posedge_event() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator () ( const in_if_l_type& interface_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( interface_.posedge_event() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator () ( const in_port_b_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator () ( const in_port_l_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator () ( const inout_port_b_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_pos&
sc_sensitive_pos::operator () ( const inout_port_l_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_POS_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.pos() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}


sc_sensitive_pos&
sc_sensitive_pos::operator << ( const in_if_b_type& interface_ )
{
    return operator () ( interface_ );
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( const in_if_l_type& interface_ )
{
    return operator () ( interface_ );
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( const in_port_b_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( const in_port_l_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( const inout_port_b_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_pos&
sc_sensitive_pos::operator << ( const inout_port_l_type& port_ )
{
    return operator () ( port_ );
}


// ----------------------------------------------------------------------------
//  CLASS : sc_sensitive_neg
//
//  Static sensitivity class for negative edge events.
// ----------------------------------------------------------------------------

// constructor

sc_sensitive_neg::sc_sensitive_neg( sc_module* module_ )
: m_module( module_ ),
  m_mode( SC_NONE_ ),
  m_handle( 0 )
{}


// destructor

sc_sensitive_neg::~sc_sensitive_neg()
{}


// changing between process handles

sc_sensitive_neg&
sc_sensitive_neg::operator << ( sc_method_handle handle_ )
{
    m_mode = SC_METHOD_;
    m_handle = handle_;
    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( sc_thread_handle handle_ )
{
    m_mode = SC_THREAD_;
    m_handle = handle_;
    return *this;
}


sc_sensitive_neg&
sc_sensitive_neg::operator () ( const in_if_b_type& interface_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( interface_.negedge_event() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator () ( const in_if_l_type& interface_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_:
    case SC_THREAD_: {
	m_handle->add_static_event( interface_.negedge_event() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator () ( const in_port_b_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator () ( const in_port_l_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator () ( const inout_port_b_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}

sc_sensitive_neg&
sc_sensitive_neg::operator () ( const inout_port_l_type& port_ )
{
    // check
    if( m_module->simcontext()->is_running() ) {
	SC_REPORT_ERROR( SC_ID_MAKE_SENSITIVE_NEG_, "simulation running" );
    }

    // make sensitive
    switch( m_mode ) {
    case SC_METHOD_: {
	port_.make_sensitive( as_method_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_THREAD_: {
	port_.make_sensitive( as_thread_handle( m_handle ), &port_.neg() );
	break;
    }
    case SC_NONE_:
        /* do nothing */
        break;
    }

    return *this;
}


sc_sensitive_neg&
sc_sensitive_neg::operator << ( const in_if_b_type& interface_ )
{
    return operator () ( interface_ );
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( const in_if_l_type& interface_ )
{
    return operator () ( interface_ );
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( const in_port_b_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( const in_port_l_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( const inout_port_b_type& port_ )
{
    return operator () ( port_ );
}

sc_sensitive_neg&
sc_sensitive_neg::operator << ( const inout_port_l_type& port_ )
{
    return operator () ( port_ );
}


// Taf!
