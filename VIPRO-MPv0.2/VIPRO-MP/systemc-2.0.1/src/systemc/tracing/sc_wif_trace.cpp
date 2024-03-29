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

  sc_wif_trace.cpp - Implementation of WIF tracing.

  Original Author - Abhijit Ghosh, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affliation, date and
  changes you are making here.

      Name, Affiliation, Date: Ali Dasdan, Synopsys, Inc.
  Description of Modification: - Replaced 'width' of sc_(u)int with their
                                 'bitwidth()'.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

/*****************************************************************************

   Acknowledgement: The tracing mechanism is based on the tracing
   mechanism developed at Infineon (formerly Siemens HL). Though this
   code is somewhat different, and significantly enhanced, the basics
   are identical to what was originally contributed by Infineon.  The
   contribution of Infineon in the development of this tracing
   technology is hereby acknowledged.

 *****************************************************************************/

/*****************************************************************************

   Instead of creating the binary WIF format, we create the ASCII
   WIF format which can be converted to the binary format using
   a2wif (utility that comes with VSS from Synopsys). This way, 
   a user who does not have Synopsys VSS can still create WIF 
   files, but they can only be viewed by users who have VSS.

 *****************************************************************************/


#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "systemc/kernel/sc_simcontext.h"
#include "systemc/kernel/sc_ver.h"
#include "systemc/datatypes/bit/sc_bit.h"
#include "systemc/datatypes/bit/sc_logic.h"
#include "systemc/datatypes/bit/sc_lv_base.h"
#include "systemc/datatypes/int/sc_signed.h"
#include "systemc/datatypes/int/sc_unsigned.h"
#include "systemc/datatypes/int/sc_int_base.h"
#include "systemc/datatypes/int/sc_uint_base.h"
#include "systemc/datatypes/fx/fx.h"
#include "systemc/tracing/sc_wif_trace.h"
#include "systemc/utils/sc_string.h"


static bool running_regression = false;

// Forward declarations for functions that come later in the file
static char map_sc_logic_state_to_wif_state(char in_char);

const char* wif_names[wif_trace_file::WIF_LAST] = {"BIT","MVL","real"};


// ----------------------------------------------------------------------------
//  CLASS : wif_trace
//
//  Base class for WIF traces.
// ----------------------------------------------------------------------------

class wif_trace
{
public:

    wif_trace(const sc_string& name_, const sc_string& wif_name_);

    // Needs to be pure virtual as has to be defined by the particular
    // type being traced
    virtual void write(FILE* f) = 0;
    
    virtual void set_width();

    // Comparison function needs to be pure virtual too
    virtual bool changed() = 0;

    // Got to declare this virtual as this will be overwritten
    // by one base class
    virtual void print_variable_declaration_line(FILE* f);

    virtual ~wif_trace();

    const sc_string name;     // Name of the variable
    const sc_string wif_name; // Name of the variable in WIF file
    const char* wif_type;     // WIF data type
    int bit_width; 
};


wif_trace::wif_trace(const sc_string& name_, const sc_string& wif_name_)
        : name(name_), wif_name(wif_name_), bit_width(-1)
{
    /* Intentionally blank */
}
        
void
wif_trace::print_variable_declaration_line( FILE* f )
{
    char buf[2000];

    if( bit_width < 0 ) {
        sprintf( buf, "Traced object \"%s\" has < 0 Bits, cannot be traced.",
		 (const char *) name );
        wif_put_error_message( buf, false );
    } else {
        fprintf( f, "declare  %s   \"%s\"  %s  ",
		 (const char *) wif_name, (const char *) name, wif_type );
	if( bit_width > 0 ) {
	    fprintf( f, "0 %d ", bit_width - 1 );
	}
	fprintf( f, "variable ;\n" );
	fprintf( f, "start_trace %s ;\n", (const char *) wif_name );
    }
}

void
wif_trace::set_width()
{
    /* Intentionally Blank, should be defined for each type separately */
}

wif_trace::~wif_trace()
{
    /* Intentionally Blank */
}

// Classes for tracing individual data types

/*****************************************************************************/

class wif_bool_trace
: public wif_trace
{
public:

    wif_bool_trace( const bool& object_,
		    const sc_string& name_,
		    const sc_string& wif_name_ );
    void write( FILE* f );
    bool changed();

protected:    

    const bool& object;
    bool        old_value;
};

wif_bool_trace::wif_bool_trace( const bool& object_,
				const sc_string& name_,
				const sc_string& wif_name_ )
: wif_trace( name_, wif_name_ ),
  object( object_ )
{
    bit_width = 0;
    old_value = object_;
    wif_type = "BIT";
}

bool
wif_bool_trace::changed()
{
    return object != old_value;
}

void
wif_bool_trace::write( FILE* f )
{
    if( object == true ) {
	fprintf( f, "assign %s \'1\' ;\n", (const char *) wif_name );
    } else {
	fprintf( f, "assign %s \'0\' ;\n", (const char *) wif_name );
    }
    old_value = object;
}

//*****************************************************************************

class wif_sc_bit_trace : public wif_trace {
public:
    wif_sc_bit_trace(const sc_bit& object_, const sc_string& name_,
                   const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const sc_bit& object;
    sc_bit old_value;
};

wif_sc_bit_trace::wif_sc_bit_trace(const sc_bit& object_,
				   const sc_string& name_,
				   const sc_string& wif_name_)
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = 0;
    old_value = object_;
    wif_type = "BIT";
}

bool wif_sc_bit_trace::changed()
{
    return object != old_value;
}

void wif_sc_bit_trace::write(FILE* f)
{
    if (object == true) {
        fprintf(f, "assign %s \'1\' ;\n", (const char *) wif_name);
    } else {
	fprintf(f, "assign %s \'0\' ;\n", (const char *) wif_name);
    }
    old_value = object;
}

/*****************************************************************************/

class wif_sc_logic_trace: public wif_trace {
public:
    wif_sc_logic_trace(const sc_logic& object_,
		       const sc_string& name_,
		       const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const sc_logic& object;
    sc_logic old_value;
};


wif_sc_logic_trace::wif_sc_logic_trace(const sc_logic& object_,
				       const sc_string& name_,
				       const sc_string& wif_name_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = 0;
    old_value = object;
    wif_type = "MVL";
}


bool wif_sc_logic_trace::changed()
{
    return object != old_value;
}


void wif_sc_logic_trace::write(FILE* f)
{
    char wif_char;
    fprintf(f, "assign %s \'", (const char *) wif_name);
    wif_char = map_sc_logic_state_to_wif_state(object.to_char());
    fputc(wif_char, f); 
    fprintf(f,"\' ;\n");
    old_value = object;
}


/*****************************************************************************/

class wif_sc_unsigned_trace: public wif_trace {
public:
    wif_sc_unsigned_trace(const sc_unsigned& object_,
			  const sc_string& name_,
			  const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_unsigned& object;
    sc_unsigned old_value;
};


wif_sc_unsigned_trace::wif_sc_unsigned_trace(const sc_unsigned& object_,
					     const sc_string& name_,
					     const sc_string& wif_name_) 
: wif_trace(name_, wif_name_), object(object_), old_value(object_.length())
{
    old_value = object;
    wif_type = "BIT";
}

bool wif_sc_unsigned_trace::changed()
{
    return object != old_value;
}

void wif_sc_unsigned_trace::write(FILE* f)
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for(bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *buf_ptr++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

void wif_sc_unsigned_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class wif_sc_signed_trace: public wif_trace {
public:
    wif_sc_signed_trace(const sc_signed& object_,
			const sc_string& name_,
			const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_signed& object;
    sc_signed old_value;
};


wif_sc_signed_trace::wif_sc_signed_trace(const sc_signed& object_,
					 const sc_string& name_,
					 const sc_string& wif_name_) 
: wif_trace(name_, wif_name_), object(object_), old_value(object_.length())
{
    old_value = object;
    wif_type = "BIT";
}

bool wif_sc_signed_trace::changed()
{
    return object != old_value;
}

void wif_sc_signed_trace::write(FILE* f)
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for(bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *buf_ptr++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';

    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

void wif_sc_signed_trace::set_width()
{
    bit_width = object.length();
}

/*****************************************************************************/

class wif_sc_uint_base_trace: public wif_trace {
public:
    wif_sc_uint_base_trace(const sc_uint_base& object_,
			   const sc_string& name_,
			   const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_uint_base& object;
    sc_uint_base old_value;
};


wif_sc_uint_base_trace::wif_sc_uint_base_trace(const sc_uint_base& object_,
					       const sc_string& name_,
					       const sc_string& wif_name_) 
: wif_trace(name_, wif_name_), object(object_), old_value(object_.length())
{
    old_value = object;
    wif_type = "BIT";
}

bool wif_sc_uint_base_trace::changed()
{
    return object != old_value;
}

void wif_sc_uint_base_trace::write(FILE* f)
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for(bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *buf_ptr++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

void wif_sc_uint_base_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class wif_sc_int_base_trace: public wif_trace {
public:
    wif_sc_int_base_trace(const sc_int_base& object_,
			  const sc_string& name_,
			  const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_int_base& object;
    sc_int_base old_value;
};


wif_sc_int_base_trace::wif_sc_int_base_trace(const sc_int_base& object_,
					     const sc_string& name_,
					     const sc_string& wif_name_) 
: wif_trace(name_, wif_name_), object(object_), old_value(object_.length())
{
    old_value = object;
    wif_type = "BIT";
}

bool wif_sc_int_base_trace::changed()
{
    return object != old_value;
}

void wif_sc_int_base_trace::write(FILE* f)
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for(bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *buf_ptr++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';

    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

void wif_sc_int_base_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class wif_sc_fxval_trace: public wif_trace
{
public:

    wif_sc_fxval_trace( const sc_fxval& object_,
			const sc_string& name_,
			const sc_string& wif_name_ );
    void write( FILE* f );
    bool changed();

protected:

    const sc_fxval& object;
    sc_fxval old_value;

};

wif_sc_fxval_trace::wif_sc_fxval_trace( const sc_fxval& object_,
				        const sc_string& name_,
					const sc_string& wif_name_ )
: wif_trace( name_, wif_name_ ),
  object( object_ )
{
    bit_width = 0;
    old_value = object;
    wif_type = "real";
}

bool
wif_sc_fxval_trace::changed()
{
    return object != old_value;
}

void
wif_sc_fxval_trace::write( FILE* f )
{
    fprintf( f, "assign  %s %f ; \n", (const char *) wif_name,
	     object.to_double() );
    old_value = object;
}

/*****************************************************************************/

class wif_sc_fxval_fast_trace: public wif_trace
{
public:

    wif_sc_fxval_fast_trace( const sc_fxval_fast& object_,
			     const sc_string& name_,
			     const sc_string& wif_name_ );
    void write( FILE* f );
    bool changed();

protected:

    const sc_fxval_fast& object;
    sc_fxval_fast old_value;

};

wif_sc_fxval_fast_trace::wif_sc_fxval_fast_trace( const sc_fxval_fast& object_,
						  const sc_string& name_,
						  const sc_string& wif_name_ )
: wif_trace( name_, wif_name_ ),
  object( object_ )
{
    bit_width = 0;
    old_value = object;
    wif_type = "real";
}

bool
wif_sc_fxval_fast_trace::changed()
{
    return object != old_value;
}

void
wif_sc_fxval_fast_trace::write( FILE* f )
{
    fprintf( f, "assign  %s %f ; \n", (const char *) wif_name,
	     object.to_double() );
    old_value = object;
}

/*****************************************************************************/

class wif_sc_fxnum_trace: public wif_trace
{
public:

    wif_sc_fxnum_trace( const sc_fxnum& object_,
			const sc_string& name_,
			const sc_string& wif_name_ );
    void write( FILE* f );
    bool changed();
    void set_width();

protected:

    const sc_fxnum& object;
    sc_fxnum old_value;

};

wif_sc_fxnum_trace::wif_sc_fxnum_trace( const sc_fxnum& object_,
				        const sc_string& name_,
					const sc_string& wif_name_ )
: wif_trace( name_, wif_name_ ),
  object( object_ ),
  old_value( object_.m_params.type_params(),
	     object_.m_params.enc(),
	     object_.m_params.cast_switch(),
	     0 )
{
    old_value = object;
    wif_type = "BIT";
}

bool
wif_sc_fxnum_trace::changed()
{
    return object != old_value;
}

void
wif_sc_fxnum_trace::write( FILE* f )
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for( bitindex = object.wl() - 1; bitindex >= 0; -- bitindex )
    {
        *buf_ptr ++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';

    fprintf( f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf );
    old_value = object;
}

void
wif_sc_fxnum_trace::set_width()
{
    bit_width = object.wl();
}

/*****************************************************************************/

class wif_sc_fxnum_fast_trace: public wif_trace
{
public:

    wif_sc_fxnum_fast_trace( const sc_fxnum_fast& object_,
			     const sc_string& name_,
			     const sc_string& wif_name_ );
    void write( FILE* f );
    bool changed();
    void set_width();

protected:

    const sc_fxnum_fast& object;
    sc_fxnum_fast old_value;

};

wif_sc_fxnum_fast_trace::wif_sc_fxnum_fast_trace( const sc_fxnum_fast& object_,
						  const sc_string& name_,
						  const sc_string& wif_name_ )
: wif_trace( name_, wif_name_ ),
  object( object_ ),
  old_value( object_.m_params.type_params(),
	     object_.m_params.enc(),
	     object_.m_params.cast_switch(),
	     0 )
{
    old_value = object;
    wif_type = "BIT";
}

bool
wif_sc_fxnum_fast_trace::changed()
{
    return object != old_value;
}

void
wif_sc_fxnum_fast_trace::write( FILE* f )
{
    char buf[1000], *buf_ptr = buf;

    int bitindex;
    for( bitindex = object.wl() - 1; bitindex >= 0; -- bitindex )
    {
        *buf_ptr ++ = "01"[(object)[bitindex]];
    }
    *buf_ptr = '\0';

    fprintf( f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf );
    old_value = object;
}

void
wif_sc_fxnum_fast_trace::set_width()
{
    bit_width = object.wl();
}


/*****************************************************************************/

class wif_unsigned_int_trace: public wif_trace {
public:
    wif_unsigned_int_trace(const unsigned& object_,
			   const sc_string& name_,
			   const sc_string& wif_name_, int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned& object;
    unsigned old_value;
    unsigned mask; 
};


wif_unsigned_int_trace::wif_unsigned_int_trace(const unsigned& object_,
					       const sc_string& name_,
					       const sc_string& wif_name_,
					       int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_unsigned_int_trace::changed()
{
    return object != old_value;
}


void wif_unsigned_int_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex] = '0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}


/*****************************************************************************/

class wif_unsigned_short_trace: public wif_trace {
public:
    wif_unsigned_short_trace(const unsigned short& object_,
			     const sc_string& name_,
			     const sc_string& wif_name_,
			     int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned short& object;
    unsigned short old_value;
    unsigned short mask; 
};


wif_unsigned_short_trace::wif_unsigned_short_trace(const unsigned short& object_,
						   const sc_string& name_,
						   const sc_string& wif_name_,
						   int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 16) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_unsigned_short_trace::changed()
{
    return object != old_value;
}


void wif_unsigned_short_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_unsigned_char_trace: public wif_trace {
public:
    wif_unsigned_char_trace(const unsigned char& object_,
			    const sc_string& name_,
			    const sc_string& wif_name_,
			    int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned char& object;
    unsigned char old_value;
    unsigned char mask; 
};


wif_unsigned_char_trace::wif_unsigned_char_trace(const unsigned char& object_,
						 const sc_string& name_,
						 const sc_string& wif_name_,
						 int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 8) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_unsigned_char_trace::changed()
{
    return object != old_value;
}


void wif_unsigned_char_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_unsigned_long_trace: public wif_trace {
public:
    wif_unsigned_long_trace(const unsigned long& object_,
			    const sc_string& name_,
			    const sc_string& wif_name_,
			    int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned long& object;
    unsigned long old_value;
    unsigned long mask; 
};


wif_unsigned_long_trace::wif_unsigned_long_trace(const unsigned long& object_,
						 const sc_string& name_,
						 const sc_string& wif_name_,
						 int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_unsigned_long_trace::changed()
{
    return object != old_value;
}


void wif_unsigned_long_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_signed_int_trace: public wif_trace {
public:
    wif_signed_int_trace(const int& object_,
			 const sc_string& name_,
			 const sc_string& wif_name_,
			 int width_);
    void write(FILE* f);
    bool changed();

protected:
    const int& object;
    int old_value;
    unsigned mask; 
};


wif_signed_int_trace::wif_signed_int_trace(const signed& object_,
					   const sc_string& name_,
					   const sc_string& wif_name_,
					   int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_signed_int_trace::changed()
{
    return object != old_value;
}


void wif_signed_int_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned) object & mask) != (unsigned) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_signed_short_trace: public wif_trace {
public:
    wif_signed_short_trace(const short& object_,
			   const sc_string& name_,
			   const sc_string& wif_name_,
			   int width_);
    void write(FILE* f);
    bool changed();

protected:
    const short& object;
    short old_value;
    unsigned short mask; 
};


wif_signed_short_trace::wif_signed_short_trace(const short& object_,
					       const sc_string& name_,
					       const sc_string& wif_name_,
					       int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 16) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_signed_short_trace::changed()
{
    return object != old_value;
}


void wif_signed_short_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned short) object & mask) != (unsigned short) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_signed_char_trace: public wif_trace {
public:
    wif_signed_char_trace(const char& object_,
			  const sc_string& name_,
			  const sc_string& wif_name_,
			  int width_);
    void write(FILE* f);
    bool changed();

protected:
    const char& object;
    char old_value;
    unsigned char mask; 
};


wif_signed_char_trace::wif_signed_char_trace(const char& object_,
					     const sc_string& name_,
					     const sc_string& wif_name_,
					     int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 8) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_signed_char_trace::changed()
{
    return object != old_value;
}


void wif_signed_char_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned char) object & mask) != (unsigned char) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}

/*****************************************************************************/

class wif_signed_long_trace: public wif_trace {
public:
    wif_signed_long_trace(const long& object_,
			  const sc_string& name_,
			  const sc_string& wif_name_,
			  int width_);
    void write(FILE* f);
    bool changed();

protected:
    const long& object;
    long old_value;
    unsigned long mask; 
};


wif_signed_long_trace::wif_signed_long_trace(const long& object_,
					     const sc_string& name_,
					     const sc_string& wif_name_,
					     int width_) 
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    old_value = object;
    wif_type = "BIT";
}


bool wif_signed_long_trace::changed()
{
    return object != old_value;
}


void wif_signed_long_trace::write(FILE* f)
{
    char buf[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned long) object & mask) != (unsigned long) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            buf[bitindex]='0';
        }
    } else {
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            buf[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    buf[bitindex] = '\0';
    fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name, buf); 
    old_value = object;
}


/*****************************************************************************/

class wif_float_trace: public wif_trace {
public:
    wif_float_trace(const float& object_,
		    const sc_string& name_,
		    const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const float& object;
    float old_value;
};

wif_float_trace::wif_float_trace(const float& object_,
				 const sc_string& name_,
				 const sc_string& wif_name_)
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = 0;
    old_value = object;
    wif_type = "real";
}

bool wif_float_trace::changed()
{
    return object != old_value;
}

void wif_float_trace::write(FILE* f)
{
    fprintf(f,"assign  %s %f ; \n", (const char *) wif_name, object);
    old_value = object;
}

/*****************************************************************************/

class wif_double_trace: public wif_trace {
public:
    wif_double_trace(const double& object_,
		     const sc_string& name_,
		     const sc_string& wif_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const double& object;
    double old_value;
};

wif_double_trace::wif_double_trace(const double& object_,
				   const sc_string& name_,
				   const sc_string& wif_name_)
: wif_trace(name_, wif_name_), object(object_)
{
    bit_width = 0;
    old_value = object;
    wif_type = "real";
}

bool wif_double_trace::changed()
{
    return object != old_value;
}

void wif_double_trace::write(FILE* f)
{
    fprintf(f,"assign  %s %f ; \n", (const char *) wif_name, object);
    old_value = object;
}


/*****************************************************************************/

class wif_enum_trace : public wif_trace {
public:
    wif_enum_trace(const unsigned& object_,
		   const sc_string& name_,
		   const sc_string& wif_name_,
		   const char** enum_literals);
    void write(FILE* f);
    bool changed();
    // Hides the definition of the same (virtual) function in wif_trace
    void print_variable_declaration_line(FILE* f);

protected:
    const unsigned& object;
    unsigned old_value;
    
    const char** literals;
    unsigned nliterals;
    sc_string type_name;

    ~wif_enum_trace();
};


wif_enum_trace::wif_enum_trace(const unsigned& object_,
			       const sc_string& name_,
			       const sc_string& wif_name_,
			       const char** enum_literals_) 
: wif_trace(name_, wif_name_), object(object_), literals(enum_literals_)
{
    // find number of enumeration literals - counting loop
    for (nliterals = 0; enum_literals_[nliterals]; nliterals++);

    bit_width = 0;
    old_value = object;
    type_name = name_ + "__type__";
    wif_type = (const char *) type_name;
}       

void wif_enum_trace::print_variable_declaration_line(FILE* f)
{
    fprintf(f, "type scalar \"%s\" enum ", wif_type);

    for (unsigned i = 0; i < nliterals; i++)
      fprintf(f, "\"%s\", ", literals[i]);
    fprintf(f, "\"SC_WIF_UNDEF\" ;\n");

    fprintf(f, "declare  %s   \"%s\"  \"%s\" ",
	    (const char *) wif_name, (const char *) name, wif_type);
    fprintf(f, "variable ;\n");
    fprintf(f, "start_trace %s ;\n", (const char *) wif_name);
}

bool wif_enum_trace::changed()
{
    return object != old_value;
}

void wif_enum_trace::write(FILE* f)
{
    char buf[2000];
    static bool warning_issued = false;

    if (object >= nliterals) { // Note unsigned value is always greater than 0
        if (!warning_issued) {
	    sprintf(buf, "Tracing error: Value of enumerated type undefined");
	    wif_put_error_message(buf, false);
	    warning_issued = true;
	}
	fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name,
		"SC_WIF_UNDEF");
    }
    else 
        fprintf(f, "assign %s \"%s\" ;\n", (const char *) wif_name,
		literals[object]);
    old_value = object;
}

wif_enum_trace::~wif_enum_trace()
{
    /* Intentionally blank */
}


template <class T>
class wif_T_trace
: public wif_trace
{
public:

    wif_T_trace( const T& object_,
		 const sc_string& name_,
		 const sc_string& wif_name_,
		 wif_trace_file::wif_enum type_ )
    : wif_trace( name_, wif_name_),
      object( object_ ),
      old_value( object_ )
    {  wif_type = wif_names[type_]; }

    void write( FILE* f )
    {
       fprintf( f,
		"assign %s \"%s\" ;\n",
		wif_name.c_str(),
		object.to_string().c_str() );
       old_value = object;
    }

    bool changed()
        { return !(object == old_value); }

    void set_width()
        { bit_width = object.length(); }

protected:

    const T& object;
    T        old_value;
};

typedef wif_T_trace<sc_bv_base> wif_sc_bv_trace;
typedef wif_T_trace<sc_lv_base> wif_sc_lv_trace;


//***********************************************************************
//           wif_trace_file functions
//***********************************************************************


wif_trace_file::wif_trace_file(const char * name)
{
    sc_string file_name = name ;
    file_name += ".awif";
    fp = fopen((const char *) file_name, "w");
    if (!fp) {
        sc_string msg = sc_string("Cannot write trace file '") + file_name +
	    "'";
	cerr << "FATAL: " << msg << "\n";
        exit(1);
    }
    trace_delta_cycles = false; // make it the default
    initialized = false;
    wif_name_index = 0;

    // default time step is the time resolution
    timescale_unit = sc_get_time_resolution().to_seconds();

    timescale_set_by_user = false;
}


void wif_trace_file::initialize()
{
    char buf[2000];

    // init
    fprintf(fp, "init ;\n\n");

    //timescale:
    if     (timescale_unit == 1e-15) sprintf(buf,"0");
    else if(timescale_unit == 1e-14) sprintf(buf,"1");
    else if(timescale_unit == 1e-13) sprintf(buf,"2");
    else if(timescale_unit == 1e-12) sprintf(buf,"3");
    else if(timescale_unit == 1e-11) sprintf(buf,"4");
    else if(timescale_unit == 1e-10) sprintf(buf,"5");
    else if(timescale_unit == 1e-9)  sprintf(buf,"6");
    else if(timescale_unit == 1e-8)  sprintf(buf,"7");
    else if(timescale_unit == 1e-7)  sprintf(buf,"8");
    else if(timescale_unit == 1e-6)  sprintf(buf,"9");
    else if(timescale_unit == 1e-5)  sprintf(buf,"10");
    else if(timescale_unit == 1e-4)  sprintf(buf,"11");
    else if(timescale_unit == 1e-3)  sprintf(buf,"12");
    else if(timescale_unit == 1e-2)  sprintf(buf,"13");
    else if(timescale_unit == 1e-1)  sprintf(buf,"14");
    else if(timescale_unit == 1e0)   sprintf(buf,"15");
    else if(timescale_unit == 1e1)   sprintf(buf,"16");
    else if(timescale_unit == 1e2)   sprintf(buf,"17");
    fprintf(fp,"header  %s \"%s\" ;\n\n", buf, sc_version());

    //date:
    time_t long_time;
    time(&long_time);
    struct tm* p_tm;
    p_tm = localtime(&long_time);
    strftime(buf, 199, "%b %d, %Y       %H:%M:%S", p_tm);
    fprintf(fp, "comment \"ASCII WIF file produced on date:  %s\" ;\n", buf);
 
    //version:
    fprintf(fp, "comment \"Created by %s\" ;\n", sc_version());
    //conversion info
    fprintf(fp, "comment \"Convert this file to binary WIF format using a2wif\" ;\n\n");


    running_regression = ( getenv( "SYSTEMC_REGRESSION" ) != NULL );
    // Don't print message if running regression
    if( ! timescale_set_by_user && ! running_regression ) {
	cout << "WARNING: Default time step is used for WIF tracing." << endl;
    }

    // Define the two types we need to represent bool and sc_logic
    fprintf(fp, "type scalar \"BIT\" enum '0', '1' ;\n");
    fprintf(fp, "type scalar \"MVL\" enum '0', '1', 'X', 'Z', '?' ;\n");
    fprintf(fp, "\n");

    //variable definitions:
    int i;
    for (i = 0; i < traces.size(); i++) {
        wif_trace* t = traces[i];
        t->set_width(); //needed for all vectors
        t->print_variable_declaration_line(fp);
    }

    double inittime = sc_time_stamp().to_seconds();
    previous_time = inittime/timescale_unit;

    // Dump all values at initial time
    sprintf(buf,
            "All initial values are dumped below at time "
            "%g sec = %g timescale units.",
            inittime,
            inittime/timescale_unit
            );
    write_comment(buf);

    double_to_special_int64(inittime/timescale_unit,
			    &previous_time_units_high,
			    &previous_time_units_low );

    for (i = 0; i < traces.size(); i++) {
        wif_trace* t = traces[i];
        t->write(fp);
    }
}


void wif_trace_file::sc_set_wif_time_unit(int exponent10_seconds)
{
    if(initialized){
        wif_put_error_message("WIF trace timescale unit cannot be changed once tracing has begun.\n"
                              "To change the scale, create a new trace file.",
                              false);
        return;
    }

    if(exponent10_seconds < -15 || exponent10_seconds >  2){
        wif_put_error_message(
	    "set_wif_time_unit() has valid exponent range -15...+2.", false);
        return;
    }

    if     (exponent10_seconds == -15) timescale_unit = 1e-15;
    else if(exponent10_seconds == -14) timescale_unit = 1e-14;
    else if(exponent10_seconds == -13) timescale_unit = 1e-13;
    else if(exponent10_seconds == -12) timescale_unit = 1e-12;
    else if(exponent10_seconds == -11) timescale_unit = 1e-11;
    else if(exponent10_seconds == -10) timescale_unit = 1e-10;
    else if(exponent10_seconds ==  -9) timescale_unit = 1e-9;
    else if(exponent10_seconds ==  -8) timescale_unit = 1e-8;
    else if(exponent10_seconds ==  -7) timescale_unit = 1e-7;
    else if(exponent10_seconds ==  -6) timescale_unit = 1e-6;
    else if(exponent10_seconds ==  -5) timescale_unit = 1e-5;
    else if(exponent10_seconds ==  -4) timescale_unit = 1e-4;
    else if(exponent10_seconds ==  -3) timescale_unit = 1e-3;
    else if(exponent10_seconds ==  -2) timescale_unit = 1e-2;
    else if(exponent10_seconds ==  -1) timescale_unit = 1e-1;
    else if(exponent10_seconds ==   0) timescale_unit = 1e0;
    else if(exponent10_seconds ==   1) timescale_unit = 1e1;
    else if(exponent10_seconds ==   2) timescale_unit = 1e2;

    char buf[200];
    sprintf(buf,
	    "Note: WIF trace timescale unit is set by user to 1e%d sec.\n",
	    exponent10_seconds);
    cout << buf << flush;
    timescale_set_by_user = true;
}


// ----------------------------------------------------------------------------

#define DEFN_TRACE_METHOD(tp)                                                 \
void                                                                          \
wif_trace_file::trace( const tp& object_, const sc_string& name_ )            \
{                                                                             \
    if( initialized ) {                                                       \
        wif_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new wif trace file.", false );           \
    }                                                                         \
    sc_string temp_wif_name;                                                  \
    create_wif_name( &temp_wif_name );                                        \
    traces.push_back( new wif_ ## tp ## _trace( object_,                      \
						name_,                        \
						temp_wif_name ) );            \
}

DEFN_TRACE_METHOD(bool)
DEFN_TRACE_METHOD(sc_bit)
DEFN_TRACE_METHOD(sc_logic)

DEFN_TRACE_METHOD(float)
DEFN_TRACE_METHOD(double)

DEFN_TRACE_METHOD(sc_signed)
DEFN_TRACE_METHOD(sc_unsigned)
DEFN_TRACE_METHOD(sc_int_base)
DEFN_TRACE_METHOD(sc_uint_base)

DEFN_TRACE_METHOD(sc_fxval)
DEFN_TRACE_METHOD(sc_fxval_fast)
DEFN_TRACE_METHOD(sc_fxnum)
DEFN_TRACE_METHOD(sc_fxnum_fast)

#undef DEFN_TRACE_METHOD


#define DEFN_TRACE_METHOD_SIGNED(tp)                                          \
void                                                                          \
wif_trace_file::trace( const tp&        object_,                              \
                       const sc_string& name_,                                \
                       int              width_ )                              \
{                                                                             \
    if( initialized ) {                                                       \
        wif_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new wif trace file.", false );           \
    }                                                                         \
    sc_string temp_wif_name;                                                  \
    create_wif_name( &temp_wif_name );                                        \
    traces.push_back( new wif_signed_ ## tp ## _trace( object_,               \
						       name_,                 \
						       temp_wif_name,         \
                                                       width_ ) );            \
}

#define DEFN_TRACE_METHOD_UNSIGNED(tp)                                        \
void                                                                          \
wif_trace_file::trace( const unsigned tp& object_,                            \
                       const sc_string&   name_,                              \
                       int                width_ )                            \
{                                                                             \
    if( initialized ) {                                                       \
        wif_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new wif trace file.", false );           \
    }                                                                         \
    sc_string temp_wif_name;                                                  \
    create_wif_name( &temp_wif_name );                                        \
    traces.push_back( new wif_unsigned_ ## tp ## _trace( object_,             \
						         name_,               \
						         temp_wif_name,       \
                                                         width_ ) );          \
}

DEFN_TRACE_METHOD_SIGNED(char)
DEFN_TRACE_METHOD_SIGNED(short)
DEFN_TRACE_METHOD_SIGNED(int)
DEFN_TRACE_METHOD_SIGNED(long)

DEFN_TRACE_METHOD_UNSIGNED(char)
DEFN_TRACE_METHOD_UNSIGNED(short)
DEFN_TRACE_METHOD_UNSIGNED(int)
DEFN_TRACE_METHOD_UNSIGNED(long)

#undef DEFN_TRACE_METHOD_SIGNED
#undef DEFN_TRACE_METHOD_UNSIGNED


void
wif_trace_file::trace( const unsigned& object_,
		       const sc_string& name_,
		       const char** enum_literals_ )
{
    if( initialized ) {
        wif_put_error_message(
	    "No traces can be added once simulation has started.\n"
	    "To add traces, create a new wif trace file.", false );
    }
    sc_string temp_wif_name;
    create_wif_name( &temp_wif_name );
    traces.push_back( new wif_enum_trace( object_,
					  name_,
					  temp_wif_name,
					  enum_literals_ ) );
}

void
wif_trace_file::trace( const sc_bv_base& object_, const sc_string& name_ )
{
   traceT( object_, name_, WIF_BIT );
}

void
wif_trace_file::trace( const sc_lv_base& object_, const sc_string& name_ )
{
   traceT( object_, name_, WIF_MVL );
}


void
wif_trace_file::write_comment(const sc_string& comment)
{
    //no newline in comments allowed
    fprintf(fp, "comment \"%s\" ;\n", (const char *) comment);
}


void
wif_trace_file::delta_cycles(bool flag)
{
    trace_delta_cycles = flag;
}

void
wif_trace_file::cycle(bool this_is_a_delta_cycle)
{
    unsigned now_units_high, now_units_low;

    // Trace delta cycles only when enabled
    if (!trace_delta_cycles && this_is_a_delta_cycle) return;

    // Check for initialization
    if (!initialized) {
        initialize();
        initialized = true;
        return;
    };

    // double now_units = sc_simulation_time() / timescale_unit;
    double now_units = sc_time_stamp().to_seconds() / timescale_unit;
    
    double_to_special_int64(now_units, &now_units_high, &now_units_low );

    // Now do the real stuff
    unsigned delta_units_high, delta_units_low;
    double diff_time;
    diff_time = now_units - previous_time;
    double_to_special_int64(diff_time, &delta_units_high, &delta_units_low);
    if (this_is_a_delta_cycle && (diff_time == 0.0))
	delta_units_low++; // Increment time for delta cycle simulation
    // Note that in the last statement above, we are assuming no more
    // than 2^32 delta cycles - seems realistic
    
    bool time_printed = false;
    wif_trace* const* const l_traces = traces.raw_data();
    for (int i = 0; i < traces.size(); i++) {
        wif_trace* t = l_traces[i];
        if(t->changed()){
            if(time_printed == false){
                if(delta_units_high){
                    fprintf(fp, "delta_time %u%09u ;\n", delta_units_high,
			    delta_units_low);
                }
                else{ 
                    fprintf(fp, "delta_time %u ;\n", delta_units_low);
                }
                time_printed = true;
            }

	    // Write the variable
            t->write(fp);
        }
    }

    if(time_printed) {
        fprintf(fp, "\n");     // Put another newline
	// We update previous_time_units only when we print time because
	// this field stores the previous time that was printed, not the
	// previous time this function was called
	previous_time_units_high = now_units_high;
	previous_time_units_low = now_units_low;
	previous_time = now_units;
    }
}

// Create a WIF name for a variable
void
wif_trace_file::create_wif_name(sc_string* ptr_to_str)
{
    char buf[50];
    sprintf(buf,"O%d", wif_name_index);
    *ptr_to_str = buf; 
    wif_name_index++;
}

sc_string
wif_trace_file::obtain_new_index()
{
    char buf[32];
    sprintf( buf, "O%d", wif_name_index ++ );
    return sc_string( buf );
}

// Cleanup and close trace file
wif_trace_file::~wif_trace_file()
{
    int i;
    for (i = 0; i < traces.size(); i++) {
        wif_trace* t = traces[i];
        delete t;
    }
    fclose(fp);
}

// Map sc_logic values to values understandable by WIF
static char
map_sc_logic_state_to_wif_state(char in_char)
{
    char out_char;

    switch(in_char){
        case 'U':
        case 'X': 
        case 'W':
        case 'D':
            out_char = 'X';
            break;
        case '0':
        case 'L':
            out_char = '0';
            break;
        case  '1':
        case  'H': 
            out_char = '1';
            break;
        case  'Z': 
            out_char = 'Z';
            break;
        default:
            out_char = '?';
    }
    return out_char;
}


// no output should be done directly to cout, cerr, etc.
void
wif_put_error_message(const char* msg, bool just_warning)
{
    if(just_warning){
	cout << "WIF Trace Warning:\n" << msg << "\n" << endl;
    }
    else{
	cout << "WIF Trace ERROR:\n" << msg << "\n" << endl;
    }
}

// Create the trace file
sc_trace_file*
sc_create_wif_trace_file(const char * name)
{
    sc_trace_file *tf;

    tf = new wif_trace_file(name);
    sc_get_curr_simcontext()->add_trace_file(tf);
    return tf;
}

void
sc_close_wif_trace_file( sc_trace_file* tf )
{
    wif_trace_file* wif_tf = (wif_trace_file*)tf;
    delete wif_tf;
}
