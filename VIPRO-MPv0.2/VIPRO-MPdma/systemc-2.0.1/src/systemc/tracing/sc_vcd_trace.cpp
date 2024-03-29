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

  sc_vcd_trace.cpp - Implementation of VCD tracing.

  Original Author - Abhijit Ghosh, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
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
#include "systemc/tracing/sc_vcd_trace.h"
#include "systemc/utils/sc_string.h"


static bool running_regression = false;

// Forward declarations for functions that come later in the file
// Map sc_logic to printable VCD
static char map_sc_logic_state_to_vcd_state(char in_char);

// Remove name problems associated with [] in vcd names
static void remove_vcd_name_problems(sc_string& name);

const char* vcd_types[vcd_trace_file::VCD_LAST]={"wire","real"};


// ----------------------------------------------------------------------------
//  CLASS : vcd_trace
//
//  Base class for VCD traces.
// ----------------------------------------------------------------------------

class vcd_trace
{
public:

    vcd_trace(const sc_string& name_, const sc_string& vcd_name_);

    // Needs to be pure virtual as has to be defined by the particular
    // type being traced
    virtual void write(FILE* f) = 0;
    
    virtual void set_width();

    static const char* strip_leading_bits(const char* originalbuf);

    // Comparison function needs to be pure virtual too
    virtual bool changed() = 0;

    // Make this virtual as some derived classes may overwrite
    virtual void print_variable_declaration_line(FILE* f);

    void compose_data_line(char* rawdata, char* compdata);
    sc_string compose_line(const sc_string data);

    virtual ~vcd_trace();

    const sc_string name;
    const sc_string vcd_name;
    const char* vcd_var_typ_name;
    int bit_width; 
};


vcd_trace::vcd_trace(const sc_string& name_,
		     const sc_string& vcd_name_)
: name(name_),
  vcd_name(vcd_name_),
  bit_width(0)
{
    /* Intentionally blank */
}
        
void
vcd_trace::compose_data_line(char* rawdata, char* compdata)
{
    assert(rawdata != compdata);

    if(bit_width == 0)
    {
        compdata[0] = '\0';
    }
    else
    {
        if(bit_width == 1)
        {
            compdata[0] = rawdata[0];
            strcpy(&(compdata[1]), vcd_name);
        }
        else
        {
            const char* effective_begin = strip_leading_bits(rawdata);
            sprintf(compdata, "b%s %s",
		    effective_begin,
		    (const char *) vcd_name);
        }
    }
}

// same as above but not that ugly
sc_string
vcd_trace::compose_line(const sc_string data)
{
  if(bit_width == 0)
    return "";
  if(bit_width == 1)
    return data + vcd_name;
  return sc_string("b")+strip_leading_bits(data.c_str())+" "+vcd_name;
}

void
vcd_trace::print_variable_declaration_line(FILE* f)
{
    char buf[2000];

    if(bit_width == 0){
        sprintf(buf, "Traced object \"%s\" has 0 Bits, cannot be traced.",
		(const char *) name);
        vcd_put_error_message(buf, false);
    }
    else{
        sc_string namecopy = name; 
        remove_vcd_name_problems(namecopy);
        sprintf(buf, "$var %s  % 3d  %s  %s       $end\n",
                vcd_var_typ_name,
		bit_width,
		(const char *) vcd_name,
		(const char *) namecopy);
        fputs(buf, f);
    }
}

void
vcd_trace::set_width()
{
  /* Intentionally Blank, should be defined for each type separately */
}

const char*
vcd_trace::strip_leading_bits(const char* originalbuf)
{
    //*********************************************************************
    // - Remove multiple leading 0,z,x, and replace by only one
    // - For example,
    //    b000z100    -> b0z100
    //    b00000xxx   -> b0xxx
    //    b000        -> b0
    //    bzzzzz1     -> bz1
    //    bxxxz10     -> xz10
    // - For leading 0's followed by 1, remove all leading 0's
    //    b0000010101 -> b10101  

    const char* position = originalbuf;

    if( strlen(originalbuf) < 2 ||
	(originalbuf[0] != 'z' && originalbuf[0] != 'x' &&
	 originalbuf[0] != '0' ))
      return originalbuf;

    char first_char = *position;
    while(*position == first_char)
    {
        position++;
    }

    if(first_char == '0' && *position == '1')
        return position;
    // else
    return position-1;
}

vcd_trace::~vcd_trace()
{
  /* Intentionally Blank */
}


template <class T>
class vcd_T_trace
: public vcd_trace
{
public:

    vcd_T_trace( const T& object_,
		 const sc_string& name_,
		 const sc_string& vcd_name_,
		 vcd_trace_file::vcd_enum type_ )
    : vcd_trace( name_, vcd_name_ ),
      object( object_ ),
      old_value( object_ )
    {
        vcd_var_typ_name = vcd_types[type_];
    }

    void write( FILE* f )
    {
        fprintf( f, "%s", compose_line( object.to_string() ).c_str() );
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

typedef vcd_T_trace<sc_bv_base> vcd_sc_bv_trace;
typedef vcd_T_trace<sc_lv_base> vcd_sc_lv_trace;

// Trace sc_bv_base (sc_bv)
void
vcd_trace_file::trace(const sc_bv_base& object, const sc_string& name)
{
   traceT(object,name);
}   

// Trace sc_lv_base (sc_lv)
void
vcd_trace_file::trace(const sc_lv_base& object, const sc_string& name)
{
   traceT(object,name);
}

/*****************************************************************************/

class vcd_bool_trace : public vcd_trace {
public:
    vcd_bool_trace(const bool& object_,
		   const sc_string& name_,
		   const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();

protected:
    const bool& object;
    bool old_value;
};

vcd_bool_trace::vcd_bool_trace(const bool& object_,
			       const sc_string& name_,
			       const sc_string& vcd_name_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    vcd_var_typ_name = "wire";
    bit_width = 1;
    old_value = object;
}

bool
vcd_bool_trace::changed()
{
    return object != old_value;
}

void
vcd_bool_trace::write(FILE* f)
{
    if (object == true) fputc('1', f);
    else fputc('0', f);

    fprintf(f,"%s", (const char *) vcd_name);

    old_value = object;
}

//*****************************************************************************

class vcd_sc_bit_trace : public vcd_trace {
public:
    vcd_sc_bit_trace(const sc_bit& , const sc_string& , const sc_string& );
    void write(FILE* f);
    bool changed();

protected:
    const sc_bit& object;
    sc_bit old_value;
};

vcd_sc_bit_trace::vcd_sc_bit_trace( const sc_bit& object_,
				    const sc_string& name,
				    const sc_string& vcd_name)
: vcd_trace(name, vcd_name), object( object_ )
{
    vcd_var_typ_name = "wire";
    bit_width = 1;
    old_value = object;
}

bool
vcd_sc_bit_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_bit_trace::write(FILE* f)
{
    if (object == true) fputc('1', f);
    else fputc('0', f);

    fprintf(f,"%s", (const char *) vcd_name);

    old_value = object;
}

/*****************************************************************************/

class vcd_sc_logic_trace : public vcd_trace {
public:
    vcd_sc_logic_trace(const sc_logic& object_,
		       const sc_string& name_,
		       const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const sc_logic& object;
    sc_logic old_value;
};


vcd_sc_logic_trace::vcd_sc_logic_trace(const sc_logic& object_,
				       const sc_string& name_,
				       const sc_string& vcd_name_) 
: vcd_trace(name_, vcd_name_), object(object_)
{
    vcd_var_typ_name = "wire";
    bit_width = 1;
    old_value = object;
}


bool
vcd_sc_logic_trace::changed()
{
    return object != old_value;
}


void
vcd_sc_logic_trace::write(FILE* f)
{
    char out_char;
    out_char = map_sc_logic_state_to_vcd_state(object.to_char());
    fputc(out_char, f); 

    fprintf(f,"%s", (const char *) vcd_name);

    old_value = object;
}


/*****************************************************************************/

class vcd_sc_unsigned_trace : public vcd_trace {
public:
    vcd_sc_unsigned_trace(const sc_unsigned& object,
			  const sc_string& name_,
			  const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_unsigned& object;
    sc_unsigned old_value;
};


vcd_sc_unsigned_trace::vcd_sc_unsigned_trace(const sc_unsigned& object_,
					     const sc_string& name_,
					     const sc_string& vcd_name_) 
: vcd_trace(name_, vcd_name_), object(object_), old_value(object_.length())
// The last may look strange, but is correct
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_unsigned_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_unsigned_trace::write(FILE* f)
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for (bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *rawdata_ptr++ = "01"[(object)[bitindex]];
    }
    *rawdata_ptr = '\0';
    compose_data_line(rawdata, compdata);

    fputs(compdata, f);
    old_value = object;
}

void
vcd_sc_unsigned_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class vcd_sc_signed_trace : public vcd_trace {
public:
    vcd_sc_signed_trace(const sc_signed& object,
			const sc_string& name_,
			const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_signed& object;
    sc_signed old_value;
};


vcd_sc_signed_trace::vcd_sc_signed_trace(const sc_signed& object_,
					 const sc_string& name_,
					 const sc_string& vcd_name_) 
: vcd_trace(name_, vcd_name_), object(object_), old_value(object_.length())
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_signed_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_signed_trace::write(FILE* f)
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for (bitindex = object.length() - 1; bitindex >= 0; --bitindex) {
        *rawdata_ptr++ = "01"[(object)[bitindex]];
    }
    *rawdata_ptr = '\0';
    compose_data_line(rawdata, compdata);

    fputs(compdata, f);
    old_value = object;
}

void
vcd_sc_signed_trace::set_width()
{
    bit_width = object.length();
}

/*****************************************************************************/

class vcd_sc_uint_base_trace : public vcd_trace {
public:
    vcd_sc_uint_base_trace(const sc_uint_base& object,
			   const sc_string& name_,
			   const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_uint_base& object;
    sc_uint_base old_value;
};


vcd_sc_uint_base_trace::vcd_sc_uint_base_trace(const sc_uint_base& object_,
					       const sc_string& name_,
					       const sc_string& vcd_name_) 
: vcd_trace(name_, vcd_name_), object(object_), old_value(object_.length())
// The last may look strange, but is correct
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_uint_base_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_uint_base_trace::write(FILE* f)
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for (bitindex = object.length()-1; bitindex >= 0; --bitindex) {
        *rawdata_ptr++ = "01"[int((object)[bitindex])];
    }
    *rawdata_ptr = '\0';
    compose_data_line(rawdata, compdata);

    fputs(compdata, f);
    old_value = object;
}

void
vcd_sc_uint_base_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class vcd_sc_int_base_trace : public vcd_trace {
public:
    vcd_sc_int_base_trace(const sc_int_base& object,
			  const sc_string& name_,
			  const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();
    void set_width();

protected:    
    const sc_int_base& object;
    sc_int_base old_value;
};


vcd_sc_int_base_trace::vcd_sc_int_base_trace(const sc_int_base& object_,
					     const sc_string& name_,
					     const sc_string& vcd_name_) 
: vcd_trace(name_, vcd_name_), object(object_), old_value(object_.length())
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_int_base_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_int_base_trace::write(FILE* f)
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for (bitindex = object.length()-1; bitindex >= 0; --bitindex) {
        *rawdata_ptr++ = "01"[int((object)[bitindex])];
    }
    *rawdata_ptr = '\0';
    compose_data_line(rawdata, compdata);

    fputs(compdata, f);
    old_value = object;
}

void
vcd_sc_int_base_trace::set_width()
{
    bit_width = object.length();
}


/*****************************************************************************/

class vcd_sc_fxval_trace : public vcd_trace
{
public:

    vcd_sc_fxval_trace( const sc_fxval& object,
			const sc_string& name_,
			const sc_string& vcd_name_ );
    void write( FILE* f );
    bool changed();

protected:

    const sc_fxval& object;
    sc_fxval old_value;

};

vcd_sc_fxval_trace::vcd_sc_fxval_trace( const sc_fxval& object_,
				        const sc_string& name_,
					const sc_string& vcd_name_ )
: vcd_trace( name_, vcd_name_ ),
  object( object_ )
{
    vcd_var_typ_name = "real";
    bit_width = 1;
    old_value = object;
}

bool
vcd_sc_fxval_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_fxval_trace::write( FILE* f )
{
    fprintf( f, "r%.16g %s", object.to_double(), (const char*) vcd_name );
    old_value = object;
}

/*****************************************************************************/

class vcd_sc_fxval_fast_trace : public vcd_trace
{
public:

    vcd_sc_fxval_fast_trace( const sc_fxval_fast& object,
			     const sc_string& name_,
			     const sc_string& vcd_name_ );
    void write( FILE* f );
    bool changed();

protected:

    const sc_fxval_fast& object;
    sc_fxval_fast old_value;

};

vcd_sc_fxval_fast_trace::vcd_sc_fxval_fast_trace( const sc_fxval_fast& object_,
						  const sc_string& name_,
						  const sc_string& vcd_name_ )
: vcd_trace( name_, vcd_name_ ),
  object( object_ )
{
    vcd_var_typ_name = "real";
    bit_width = 1;
    old_value = object;
}

bool
vcd_sc_fxval_fast_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_fxval_fast_trace::write( FILE* f )
{
    fprintf( f, "r%.16g %s", object.to_double(), (const char*) vcd_name );
    old_value = object;
}

/*****************************************************************************/

class vcd_sc_fxnum_trace : public vcd_trace
{
public:

    vcd_sc_fxnum_trace( const sc_fxnum& object,
			const sc_string& name_,
			const sc_string& vcd_name_ );
    void write( FILE* f );
    bool changed();
    void set_width();

protected:

    const sc_fxnum& object;
    sc_fxnum old_value;

};

vcd_sc_fxnum_trace::vcd_sc_fxnum_trace( const sc_fxnum& object_,
				        const sc_string& name_,
					const sc_string& vcd_name_ )
: vcd_trace( name_, vcd_name_ ),
  object( object_ ),
  old_value( object_.m_params.type_params(),
	     object_.m_params.enc(),
	     object_.m_params.cast_switch(),
	     0 )
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_fxnum_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_fxnum_trace::write( FILE* f )
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for( bitindex = object.wl() - 1; bitindex >= 0; -- bitindex )
    {
        *rawdata_ptr ++ = "01"[(object)[bitindex]];
    }
    *rawdata_ptr = '\0';
    compose_data_line( rawdata, compdata );

    fputs( compdata, f );
    old_value = object;
}

void
vcd_sc_fxnum_trace::set_width()
{
    bit_width = object.wl();
}

/*****************************************************************************/

class vcd_sc_fxnum_fast_trace : public vcd_trace
{
public:

    vcd_sc_fxnum_fast_trace( const sc_fxnum_fast& object,
			     const sc_string& name_,
			     const sc_string& vcd_name_ );
    void write( FILE* f );
    bool changed();
    void set_width();

protected:

    const sc_fxnum_fast& object;
    sc_fxnum_fast old_value;

};

vcd_sc_fxnum_fast_trace::vcd_sc_fxnum_fast_trace( const sc_fxnum_fast& object_,
						  const sc_string& name_,
						  const sc_string& vcd_name_ )
: vcd_trace( name_, vcd_name_ ),
  object( object_ ),
  old_value( object_.m_params.type_params(),
	     object_.m_params.enc(),
	     object_.m_params.cast_switch(),
	     0 )
{
    vcd_var_typ_name = "wire";
    old_value = object;
}

bool
vcd_sc_fxnum_fast_trace::changed()
{
    return object != old_value;
}

void
vcd_sc_fxnum_fast_trace::write( FILE* f )
{
    char rawdata[1000], *rawdata_ptr = rawdata;
    char compdata[1000];

    int bitindex;
    for( bitindex = object.wl() - 1; bitindex >= 0; -- bitindex )
    {
        *rawdata_ptr ++ = "01"[(object)[bitindex]];
    }
    *rawdata_ptr = '\0';
    compose_data_line( rawdata, compdata );

    fputs( compdata, f );
    old_value = object;
}

void
vcd_sc_fxnum_fast_trace::set_width()
{
    bit_width = object.wl();
}


/*****************************************************************************/

class vcd_unsigned_int_trace : public vcd_trace {
public:
    vcd_unsigned_int_trace(const unsigned& object,
			   const sc_string& name_,
			   const sc_string& vcd_name_,
			   int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned& object;
    unsigned old_value;
    unsigned mask; 
};


vcd_unsigned_int_trace::vcd_unsigned_int_trace(const unsigned& object_,
					       const sc_string& name_,
					       const sc_string& vcd_name_,
					       int width_) 
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool
vcd_unsigned_int_trace::changed()
{
    return object != old_value;
}


void
vcd_unsigned_int_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_unsigned_short_trace : public vcd_trace {
public:
    vcd_unsigned_short_trace(const unsigned short& object,
			     const sc_string& name_,
			     const sc_string& vcd_name_,
			     int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned short& object;
    unsigned short old_value;
    unsigned short mask; 
};


vcd_unsigned_short_trace::vcd_unsigned_short_trace(const unsigned short& object_,
						   const sc_string& name_,
						   const sc_string& vcd_name_,
						   int width_) 
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 16) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool
vcd_unsigned_short_trace::changed()
{
    return object != old_value;
}


void
vcd_unsigned_short_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_unsigned_char_trace : public vcd_trace {
public:
    vcd_unsigned_char_trace(const unsigned char& object,
			    const sc_string& name_,
			    const sc_string& vcd_name_,
			    int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned char& object;
    unsigned char old_value;
    unsigned char mask; 
};


vcd_unsigned_char_trace::vcd_unsigned_char_trace(const unsigned char& object_,
						 const sc_string& name_,
						 const sc_string& vcd_name_,
						 int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 8) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_unsigned_char_trace::changed()
{
    return object != old_value;
}


void vcd_unsigned_char_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_unsigned_long_trace : public vcd_trace {
public:
    vcd_unsigned_long_trace(const unsigned long& object,
			    const sc_string& name_,
			    const sc_string& vcd_name_,
			    int width_);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned long& object;
    unsigned long old_value;
    unsigned long mask; 
};


vcd_unsigned_long_trace::vcd_unsigned_long_trace(const unsigned long& object_,
						 const sc_string& name_,
						 const sc_string& vcd_name_,
						 int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_unsigned_long_trace::changed()
{
    return object != old_value;
}


void vcd_unsigned_long_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_signed_int_trace : public vcd_trace {
public:
    vcd_signed_int_trace(const int& object,
			 const sc_string& name_,
			 const sc_string& vcd_name_,
			 int width_);
    void write(FILE* f);
    bool changed();

protected:
    const int& object;
    int old_value;
    unsigned mask; 
};


vcd_signed_int_trace::vcd_signed_int_trace(const signed& object_,
					   const sc_string& name_,
					   const sc_string& vcd_name_,
					   int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_signed_int_trace::changed()
{
    return object != old_value;
}


void vcd_signed_int_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned) object & mask) != (unsigned) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_signed_short_trace : public vcd_trace {
public:
    vcd_signed_short_trace(const short& object,
			   const sc_string& name_,
			   const sc_string& vcd_name_,
			   int width_);
    void write(FILE* f);
    bool changed();

protected:
    const short& object;
    short old_value;
    unsigned short mask; 
};


vcd_signed_short_trace::vcd_signed_short_trace(const short& object_,
					       const sc_string& name_,
					       const sc_string& vcd_name_,
					       int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 16) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_signed_short_trace::changed()
{
    return object != old_value;
}


void vcd_signed_short_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned short) object & mask) != (unsigned short) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_signed_char_trace : public vcd_trace {
public:
    vcd_signed_char_trace(const char& object,
			  const sc_string& name_,
			  const sc_string& vcd_name_,
			  int width_);
    void write(FILE* f);
    bool changed();

protected:
    const char& object;
    char old_value;
    unsigned char mask; 
};


vcd_signed_char_trace::vcd_signed_char_trace(const char& object_,
					     const sc_string& name_,
					     const sc_string& vcd_name_,
					     int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 8) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_signed_char_trace::changed()
{
    return object != old_value;
}


void vcd_signed_char_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned char) object & mask) != (unsigned char) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}

/*****************************************************************************/

class vcd_signed_long_trace : public vcd_trace {
public:
    vcd_signed_long_trace(const long& object,
			  const sc_string& name_,
			  const sc_string& vcd_name_,
			  int width_);
    void write(FILE* f);
    bool changed();

protected:
    const long& object;
    long old_value;
    unsigned long mask; 
};


vcd_signed_long_trace::vcd_signed_long_trace(const long& object_,
					     const sc_string& name_,
					     const sc_string& vcd_name_,
					     int width_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    bit_width = width_;
    if (bit_width < 32) {
        mask = ~(-1 << bit_width);
    } else {
        mask = 0xffffffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}


bool vcd_signed_long_trace::changed()
{
    return object != old_value;
}


void vcd_signed_long_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if (((unsigned long) object & mask) != (unsigned long) object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    }
    else{
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}


/*****************************************************************************/

class vcd_float_trace : public vcd_trace {
public:
    vcd_float_trace(const float& object,
		    const sc_string& name_,
		    const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const float& object;
    float old_value;
};

vcd_float_trace::vcd_float_trace(const float& object_,
				 const sc_string& name_,
				 const sc_string& vcd_name_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    vcd_var_typ_name = "real";
    bit_width = 1;
    old_value = object;
}

bool vcd_float_trace::changed()
{
    return object != old_value;
}

void vcd_float_trace::write(FILE* f)
{
    fprintf(f, "r%.16g %s", object, (const char *) vcd_name);
    old_value = object;
}

/*****************************************************************************/

class vcd_double_trace : public vcd_trace {
public:
    vcd_double_trace(const double& object,
		     const sc_string& name_,
		     const sc_string& vcd_name_);
    void write(FILE* f);
    bool changed();

protected:    
    const double& object;
    double old_value;
};

vcd_double_trace::vcd_double_trace(const double& object_,
				   const sc_string& name_,
				   const sc_string& vcd_name_)
: vcd_trace(name_, vcd_name_), object(object_)
{
    vcd_var_typ_name = "real";
    bit_width = 1;
    old_value = object;
}

bool vcd_double_trace::changed()
{
    return object != old_value;
}

void vcd_double_trace::write(FILE* f)
{
    fprintf(f, "r%.16g %s", object, (const char *) vcd_name);
    old_value = object;
}


/*****************************************************************************/

class vcd_enum_trace : public vcd_trace {
public:
    vcd_enum_trace(const unsigned& object_,
		   const sc_string& name_,
		   const sc_string& vcd_name_,
		   const char** enum_literals);
    void write(FILE* f);
    bool changed();

protected:
    const unsigned& object;
    unsigned old_value;
    unsigned mask;
    const char** literals;
    unsigned nliterals;
};


vcd_enum_trace::vcd_enum_trace(const unsigned& object_,
			       const sc_string& name_,
			       const sc_string& vcd_name_,
			       const char** enum_literals_)
: vcd_trace(name_, vcd_name_), object(object_), literals(enum_literals_)
{
    // find number of bits required to represent enumeration literal - counting loop
    for (nliterals = 0; enum_literals_[nliterals]; nliterals++);

    // Figure out number of bits required to represent the number of literals
    bit_width = 0;
    unsigned shifted_maxindex = nliterals-1;
    while(shifted_maxindex != 0){
        shifted_maxindex >>= 1;
        bit_width++;
    }

    // Set the mask
    if (bit_width < 32) {
      mask = ~(-1 << bit_width);
    } else {
      mask = 0xffffffff;
    }

    vcd_var_typ_name = "wire";
    old_value = object;
}       

bool vcd_enum_trace::changed()
{
    return object != old_value;
}

void vcd_enum_trace::write(FILE* f)
{
    char rawdata[1000];
    char compdata[1000];
    int bitindex;

    // Check for overflow
    if ((object & mask) != object) {
        for (bitindex = 0; bitindex < bit_width; bitindex++){
            rawdata[bitindex] = 'x';
        }
    } else {
        unsigned bit_mask = 1 << (bit_width-1);
        for (bitindex = 0; bitindex < bit_width; bitindex++) {
            rawdata[bitindex] = (object & bit_mask)? '1' : '0';
            bit_mask = bit_mask >> 1;
        }
    }
    rawdata[bitindex] = '\0';
    compose_data_line(rawdata, compdata);
    fputs(compdata, f);
    old_value = object;
}


/*****************************************************************************
           vcd_trace_file functions
 *****************************************************************************/

vcd_trace_file::vcd_trace_file(const char *name)
{
    sc_string file_name = name ;
    file_name += ".vcd";
    fp = fopen((const char *) file_name, "w");
    if (!fp) {
        sc_string msg = sc_string("Cannot write trace file '") +
	                file_name + "'";
	cerr << "FATAL: " << msg << "\n";
        exit(1);
    }
    trace_delta_cycles = false; // Make this the default
    initialized = false;
    vcd_name_index = 0;

    // default time step is the time resolution
    timescale_unit = sc_get_time_resolution().to_seconds();

    timescale_set_by_user = false;
}


void vcd_trace_file::initialize()
{
    char buf[2000];

    //date:
    time_t long_time;
    time(&long_time);
    struct tm* p_tm;
    p_tm = localtime(&long_time);
    strftime(buf, 199, "%b %d, %Y       %H:%M:%S", p_tm);
    fprintf(fp, "$date\n     %s\n$end\n\n", buf);

    //version:
    fprintf(fp, "$version\n %s\n$end\n\n", sc_version());

    //timescale:
    if     (timescale_unit == 1e-15) sprintf(buf, "1 fs");
    else if(timescale_unit == 1e-14) sprintf(buf, "10 fs");
    else if(timescale_unit == 1e-13) sprintf(buf, "100 fs");
    else if(timescale_unit == 1e-12) sprintf(buf, "1 ps");
    else if(timescale_unit == 1e-11) sprintf(buf, "10 ps");
    else if(timescale_unit == 1e-10) sprintf(buf, "100 ps");
    else if(timescale_unit == 1e-9)  sprintf(buf, "1 ns");
    else if(timescale_unit == 1e-8)  sprintf(buf, "10 ns");
    else if(timescale_unit == 1e-7)  sprintf(buf, "100 ns");
    else if(timescale_unit == 1e-6)  sprintf(buf, "1 us");
    else if(timescale_unit == 1e-5)  sprintf(buf, "10 us");
    else if(timescale_unit == 1e-4)  sprintf(buf, "100 us");
    else if(timescale_unit == 1e-3)  sprintf(buf, "1 ms");
    else if(timescale_unit == 1e-2)  sprintf(buf, "10 ms");
    else if(timescale_unit == 1e-1)  sprintf(buf, "100 ms");
    else if(timescale_unit == 1e0)   sprintf(buf, "1 s");
    else if(timescale_unit == 1e1)   sprintf(buf, "10 s");
    else if(timescale_unit == 1e2)   sprintf(buf, "100 s");
    fprintf(fp,"$timescale\n     %s\n$end\n\n",buf);

    running_regression = ( getenv( "SYSTEMC_REGRESSION" ) != NULL );
    // Don't print message if running regression
    if( ! timescale_set_by_user && ! running_regression ) {
	cout << "WARNING: Default time step is used for VCD tracing." << endl;
    }

    // Create a dummy scope
    fputs("$scope module SystemC $end\n", fp);

    //variable definitions:
    int i;
    for (i = 0; i < traces.size(); i++) {
        vcd_trace* t = traces[i];
        t->set_width(); // needed for all vectors
        t->print_variable_declaration_line(fp);
    }

    fputs("$upscope $end\n", fp);

    fputs("$enddefinitions  $end\n\n", fp);

    // double inittime = sc_simulation_time();
    double inittime = sc_time_stamp().to_seconds();
    
    sprintf(buf,
            "All initial values are dumped below at time "
            "%g sec = %g timescale units.",
            inittime, inittime/timescale_unit
            );
    write_comment(buf);

    double_to_special_int64(inittime/timescale_unit,
                            &previous_time_units_high,
                            &previous_time_units_low );


    fputs("$dumpvars\n",fp);
    for (i = 0; i < traces.size(); i++) {
        vcd_trace* t = traces[i];
        t->write(fp);
        fputc('\n', fp);
    }
    fputs("$end\n\n", fp);
}


void vcd_trace_file::sc_set_vcd_time_unit(int exponent10_seconds)
{
    if(initialized){
        vcd_put_error_message("VCD trace timescale unit cannot be changed once tracing has begun.\n"
                              "To change the scale, create a new trace file.",
                              false);
        return;
    }

 
    if(exponent10_seconds < -15 || exponent10_seconds >  2){
        vcd_put_error_message("set_vcd_time_unit() has valid exponent range -15...+2.", false);
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
	    "Note: VCD trace timescale unit is set by user to 1e%d sec.\n",
	    exponent10_seconds);
    cout << buf << flush;
    timescale_set_by_user = true;
}


// ----------------------------------------------------------------------------

#define DEFN_TRACE_METHOD(tp)                                                 \
void                                                                          \
vcd_trace_file::trace( const tp& object_, const sc_string& name_ )            \
{                                                                             \
    if( initialized ) {                                                       \
        vcd_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new vcd trace file.", false );           \
    }                                                                         \
    sc_string temp_vcd_name;                                                  \
    create_vcd_name( &temp_vcd_name );                                        \
    traces.push_back( new vcd_ ## tp ## _trace( object_,                      \
						name_,                        \
						temp_vcd_name ) );            \
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
vcd_trace_file::trace( const tp&        object_,                              \
                       const sc_string& name_,                                \
                       int              width_ )                              \
{                                                                             \
    if( initialized ) {                                                       \
        vcd_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new vcd trace file.", false );           \
    }                                                                         \
    sc_string temp_vcd_name;                                                  \
    create_vcd_name( &temp_vcd_name );                                        \
    traces.push_back( new vcd_signed_ ## tp ## _trace( object_,               \
					               name_,                 \
						       temp_vcd_name,         \
                                                       width_ ) );            \
}

#define DEFN_TRACE_METHOD_UNSIGNED(tp)                                        \
void                                                                          \
vcd_trace_file::trace( const unsigned tp& object_,                            \
                       const sc_string&   name_,                              \
                       int                width_ )                            \
{                                                                             \
    if( initialized ) {                                                       \
        vcd_put_error_message(                                                \
	    "No traces can be added once simulation has started.\n"           \
            "To add traces, create a new vcd trace file.", false );           \
    }                                                                         \
    sc_string temp_vcd_name;                                                  \
    create_vcd_name( &temp_vcd_name );                                        \
    traces.push_back( new vcd_unsigned_ ## tp ## _trace( object_,             \
				                         name_,               \
						         temp_vcd_name,       \
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
vcd_trace_file::trace( const unsigned& object_,
		       const sc_string& name_,
		       const char** enum_literals_ )
{
    if( initialized ) {
        vcd_put_error_message(
	    "No traces can be added once simulation has started.\n"
	    "To add traces, create a new vcd trace file.", false );
    }
    sc_string temp_vcd_name;
    create_vcd_name( &temp_vcd_name );
    traces.push_back( new vcd_enum_trace( object_,
					  name_,
					  temp_vcd_name,
					  enum_literals_ ) );
}


void
vcd_trace_file::write_comment(const sc_string& comment)
{
    //no newline in comments allowed, as some viewers may crash
    fputs("$comment\n", fp);
    fputs((const char *) comment, fp);
    fputs("\n$end\n\n", fp);
}

void
vcd_trace_file::delta_cycles(bool flag)
{
    trace_delta_cycles = flag;
}

void
vcd_trace_file::cycle(bool this_is_a_delta_cycle)
{
    char message[4000];
    unsigned this_time_units_high, this_time_units_low;
    
    // Just to make g++ shut up in the optimized mode
    this_time_units_high = this_time_units_low = 0;

    // Trace delta cycles only when enabled
    if (!trace_delta_cycles && this_is_a_delta_cycle) return;

    // Check for initialization
    if (!initialized) {
        initialize();
        initialized = true;
        return;
    };


    double now_units = sc_time_stamp().to_seconds() / timescale_unit;
    unsigned now_units_high, now_units_low;
    double_to_special_int64(now_units, &now_units_high, &now_units_low );

    bool now_later_than_previous_time = false;
    if( now_units_low > previous_time_units_low 
        && now_units_high == previous_time_units_high
        || now_units_high > previous_time_units_high){
        now_later_than_previous_time = true;
    }

    bool now_equals_previous_time = false;
    if(now_later_than_previous_time){
        this_time_units_high = now_units_high;
        this_time_units_low = now_units_low;
    } else {
        if( now_units_low == previous_time_units_low
	    && now_units_high == previous_time_units_high){
	    now_equals_previous_time = true;
            this_time_units_high = now_units_high;
            this_time_units_low = now_units_low;
	}
    }

    // Since VCD does not understand 0 time progression, we have to fake
    // delta cycles with progressing time by one unit
    if(this_is_a_delta_cycle){
        this_time_units_high = previous_time_units_high;
        this_time_units_low = previous_time_units_low + 1;
        if(this_time_units_low == 1000000000){
            this_time_units_high++;
            this_time_units_low=0;
        }
        static bool warned = false;
        if(!warned){
	    cout << "Note: VCD delta cycling with pseudo timesteps (1 unit) "
                    "is performed.\n" << endl;
            warned = true;
        }
    }


    // Not a delta cycle and time has not progressed
    if( ! this_is_a_delta_cycle && now_equals_previous_time &&
	( now_units_high != 0 || now_units_low != 0 ) ) {
	// Don't print the message at time zero
        static bool warned = false;
        if( ! warned && ! running_regression ) {
            sprintf(message,
                    "Multiple cycles found with same (%u) time units count.\n"
                    "Waveform viewers will only show the states of the last one.\n"
                    "Use ((vcd_trace_file*)vcdfile)->sc_set_vcd_time_unit(int exponent10_seconds)\n"
                    "to increase time resolution.",
                    now_units_low
                    );
            vcd_put_error_message(message, true);
            warned = true;
        }
    }

    // Not a delta cycle and time has gone backward
    // This will happen with large number of delta cycles between two real
    // advances of time
    if(!this_is_a_delta_cycle && !now_equals_previous_time && !now_later_than_previous_time){
        static bool warned = false;
        if(!warned){
            sprintf(message,
                    "Cycle found with falling (%u -> %u) time units count.\n"
                    "This can occur when delta cycling is activated.\n"
                    "Cycles with falling time are not shown.\n"
                    "Use ((vcd_trace_file*)vcdfile)->sc_set_vcd_time_unit(int exponent10_seconds)\n"
                    "to increase time resolution.",
                    previous_time_units_low, now_units_low);
            vcd_put_error_message(message, true);
            warned = true;
        }
	// Note that we don't set this_time_units_high/low to any value only
        // in this case because we are not going to do any tracing. In the
        // optimized mode, the compiler complains because of this. Therefore,
        // we include the lines at the very beginning of this function to make
        // the compiler shut up.
        return; 
    }

    // Now do the actual printing 
    bool time_printed = false;
    vcd_trace* const* const l_traces = traces.raw_data();
    for (int i = 0; i < traces.size(); i++) {
        vcd_trace* t = l_traces[i];
        if(t->changed()){
            if(time_printed == false){
                char buf[200];
                if(this_time_units_high){
                    sprintf(buf, "#%u%09u", this_time_units_high, this_time_units_low);
                }
                else{ 
                    sprintf(buf, "#%u", this_time_units_low);
                }
                fputs(buf, fp);
                fputc('\n', fp);
                time_printed = true;
            }

	    // Write the variable
            t->write(fp);
            fputc('\n', fp);
        }
    }
    // Put another newline after all values are printed
    if(time_printed) fputc('\n', fp);

    if(time_printed){
        // We update previous_time_units only when we print time because
        // this field stores the previous time that was printed, not the
        // previous time this function was called
        previous_time_units_high = this_time_units_high;
        previous_time_units_low = this_time_units_low;
    }
}

void
vcd_trace_file::create_vcd_name(sc_string* p_destination)
{
    const char first_type_used = 'a';
    const int used_types_count = 'z' - 'a' + 1;
    int result;

    char char4 = (char)(vcd_name_index % used_types_count);

    result = vcd_name_index / used_types_count;
    char char3 = (char)(result % used_types_count);

    result = result / used_types_count;
    char char2 = (char)(result % used_types_count);

    char buf[20];
    sprintf(buf, "%c%c%c",
            char2 + first_type_used,
            char3 + first_type_used,
            char4 + first_type_used);
    *p_destination = buf;
    vcd_name_index++;
}

// same as above
sc_string
vcd_trace_file::obtain_name()
{
    const char first_type_used = 'a';
    const int used_types_count = 'z' - 'a' + 1;
    int result;

    char char4 = (char)(vcd_name_index % used_types_count);

    result = vcd_name_index / used_types_count;
    char char3 = (char)(result % used_types_count);

    result = result / used_types_count;
    char char2 = (char)(result % used_types_count);

    char buf[20];
    sprintf(buf, "%c%c%c",
            char2 + first_type_used,
            char3 + first_type_used,
            char4 + first_type_used);
    vcd_name_index++;
    return sc_string(buf);
}

vcd_trace_file::~vcd_trace_file()
{
    int i;
    for (i = 0; i < traces.size(); i++) {
        vcd_trace* t = traces[i];
        delete t;
    }
    fclose(fp);
}


// Functions specific to VCD tracing

static char
map_sc_logic_state_to_vcd_state(char in_char)
{
    char out_char;

    switch(in_char){
        case 'U':
        case 'X': 
        case 'W':
        case 'D':
            out_char = 'x';
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
            out_char = 'z';
            break;
        default:
            out_char = '?';
    }

    return out_char;
}


void
vcd_put_error_message(const char* msg, bool just_warning)
{
    if(just_warning){
	cout << "VCD Trace Warning:\n" << msg << "\n" << endl;
    }
    else{
	cout << "VCD Trace ERROR:\n" << msg << "\n" << endl;
    }
}


static
void
remove_vcd_name_problems(sc_string& name)
{
    char message[4000];
    static bool warned = false;

    bool braces_removed = false;
    for (int i = 0; i< name.length(); i++) {
      if (name[i] == '[') {
	name.set(i, '(');
	braces_removed = true;
      }
      else if (name[i] == ']') {
	name.set(i, ')');
	braces_removed = true;
      }
    }

    if(braces_removed && !warned){
        sprintf(message,
                "Traced objects found with name containing [], which may be\n"
                "interpreted by the waveform viewer in unexpected ways.\n"
                "So the [] is automatically replaced by ().");
        vcd_put_error_message(message, true);
        warned = true;
    }
}


sc_trace_file*
sc_create_vcd_trace_file(const char * name)
{
  sc_trace_file *tf;

  tf = new vcd_trace_file(name);
  sc_get_curr_simcontext()->add_trace_file(tf);
  return tf;
}

void
sc_close_vcd_trace_file( sc_trace_file* tf )
{
    vcd_trace_file* vcd_tf = (vcd_trace_file*)tf;
    delete vcd_tf; 
}
