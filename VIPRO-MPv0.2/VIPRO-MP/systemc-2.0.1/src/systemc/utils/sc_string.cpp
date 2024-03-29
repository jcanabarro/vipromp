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

  sc_string.cpp -- Implementation of a simple string class.

  Original Author: Stan Y. Liao, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "systemc/utils/sc_iostream.h"
#include "systemc/utils/sc_string.h"
#include "systemc/utils/sc_utils_ids.h"


inline int
sc_roundup( int n, int m )
{
    return ((n - 1) / m + 1) * m;
}


// ----------------------------------------------------------------------------
//  ENUM : sc_numrep
//
//  Enumeration of number representations for character string conversion.
// ----------------------------------------------------------------------------

const sc_string
to_string( sc_numrep numrep )
{
    switch( numrep )
    {
        case SC_DEC:
	    return sc_string( "SC_DEC" );
        case SC_BIN:
	    return sc_string( "SC_BIN" );
        case SC_BIN_US:
	    return sc_string( "SC_BIN_US" );
        case SC_BIN_SM:
	    return sc_string( "SC_BIN_SM" );
        case SC_OCT:
	    return sc_string( "SC_OCT" );
        case SC_OCT_US:
	    return sc_string( "SC_OCT_US" );
        case SC_OCT_SM:
	    return sc_string( "SC_OCT_SM" );
        case SC_HEX:
	    return sc_string( "SC_HEX" );
        case SC_HEX_US:
	    return sc_string( "SC_HEX_US" );
        case SC_HEX_SM:
	    return sc_string( "SC_HEX_SM" );
        case SC_CSD:
	    return sc_string( "SC_CSD" );
	default:
	    return sc_string( "unknown" );
    }
}


// ----------------------------------------------------------------------------
//  CLASS : sc_string_rep
//
//  Reference counting string implementation class.
// ----------------------------------------------------------------------------

class sc_string_rep
{
    friend class sc_string;
    friend ostream& operator<<( ostream&, const sc_string& );
    friend istream& operator>>( istream&, sc_string& );
    friend sc_string operator+( const char*, const sc_string& );

    sc_string_rep( int size = 16 )
    {
        ref_count = 1;
        alloc = sc_roundup( size, 16 );
        str = new char[alloc];
        *str = '\0';
    }

    sc_string_rep( const char* s )
    {
        ref_count = 1;
        if (s) {
            alloc = 1 + strlen(s);
            str = strcpy( new char[alloc], s );
        }
        else {
            alloc = 16;
            str = strcpy( new char[alloc], "" );
        }
    }

    sc_string_rep( const char* s, int n); // get first n chars from the string

    ~sc_string_rep()
    {
        assert( ref_count == 0 );
        delete[] str;
    }

    void resize( int new_size );
    void set_string( const char* s );

    int ref_count;
    int alloc;
    char* str;
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

sc_string_rep::sc_string_rep( const char* s, int n)
{
    ref_count = 1;
    if (s && n>0) {
        alloc = 1 + n;
        str = strncpy( new char[alloc], s,n );
        str[n] = 00;
    }
    else {
        alloc = 16;
        str = strcpy( new char[alloc], "" );
    }
}

void
sc_string_rep::resize( int new_size )
{
    if (new_size <= alloc) return;
    alloc = sc_roundup( new_size, 16 );
    char* new_str = strcpy( new char[alloc], str );
    delete[] str;
    str = new_str;
}

void
sc_string_rep::set_string( const char* s )
{
    int len = strlen(s);
    resize( len + 1 );
    strcpy( str, s );
}


// ----------------------------------------------------------------------------
//  CLASS : sc_string
//
//  String class (yet another).
// ----------------------------------------------------------------------------

// constructors

sc_string::sc_string( int size )
{
    rep = new sc_string_rep( size );
}

sc_string::sc_string( const char* s )
{
    rep = new sc_string_rep( s );
}

sc_string::sc_string( const char* s, int n )
{
    rep = new sc_string_rep( s, n );
}

sc_string::sc_string( const sc_string& s )
{
    rep = s.rep;
    rep->ref_count ++;
}

sc_string::sc_string( sc_string_rep* r )
{
    rep = r;
}


// destructor

sc_string::~sc_string()
{
    if( -- (rep->ref_count) == 0 ) {
        delete rep;
    }
}


int
sc_string::length() const
{
    return strlen(rep->str);
}

sc_string
sc_string::operator+( const char* s ) const
{
    int len = length();
    sc_string_rep* r = new sc_string_rep( len + strlen(s) + 1 );
    strcpy( r->str, rep->str );
    strcpy( r->str + len, s );
    return sc_string(r);
}

sc_string sc_string::operator+(char c) const
{
    int len = length();
    sc_string_rep* r = new sc_string_rep( len + 2 );
    strcpy( r->str, rep->str );
    r->str[len] = c;
    r->str[len+1] = 00;
    return sc_string(r);
}

sc_string
operator+( const char* s, const sc_string& t )
{
    int len = strlen(s);
    sc_string_rep* r = new sc_string_rep( len + t.length() + 1 );
    strcpy( r->str, s );
    strcpy( r->str + len, t );
    return sc_string(r);
}

sc_string
sc_string::operator+( const sc_string& s ) const
{
    int len = length();
    sc_string_rep* r = new sc_string_rep( len + s.length() + 1 );
    strcpy( r->str, rep->str );
    strcpy( r->str + len, s.rep->str );
    return sc_string(r);
}

sc_string&
sc_string::operator=( const char* s )
{
    if (rep->ref_count > 1) {
        --rep->ref_count;
        rep = new sc_string_rep(s);
    }
    else {
        rep->set_string(s);
    }
    return *this;
}

sc_string&
sc_string::operator=( const sc_string& s )
{
    if (&s == this)
        return *this;
    if (--(rep->ref_count) == 0)
        delete rep;
    rep = s.rep;
    rep->ref_count++;
    return *this;
}

sc_string&
sc_string::operator+=( const char* s )
{
    int oldlen = length();
    int slen   = strlen(s);
    if (rep->ref_count > 1) {
        sc_string_rep* oldrep = rep;
        --rep->ref_count;
        rep = new sc_string_rep( oldlen + slen + 1 );
        strcpy( rep->str, oldrep->str );
        strcpy( rep->str + oldlen, s );
    }
    else {
        rep->resize( oldlen + slen + 1 );
        strcpy( rep->str + oldlen, s );
    }
    return *this;
}

sc_string& sc_string::operator+=(char c)
{
    int oldlen = length();
    if (rep->ref_count > 1) {
        sc_string_rep* oldrep = rep;
        --rep->ref_count;
        rep = new sc_string_rep( oldlen + 2 );
        strcpy( rep->str, oldrep->str );
        rep->str[oldlen]=c;
        rep->str[oldlen+1]=00;
    }
    else {
        rep->resize( oldlen + 2 );
        rep->str[oldlen]=c;
        rep->str[oldlen+1]=00;
    }
    return *this;
}

sc_string&
sc_string::operator+=( const sc_string& s )
{
    return this->operator+=( s.rep->str );
}

int
sc_string::cmp( const char* s ) const
{
    return strcmp( rep->str, s );
}

int
sc_string::cmp( const sc_string& s ) const
{
    return strcmp( rep->str, s.rep->str );
}

const char* sc_string::c_str() const
{
  return rep->str;
}

// get substring
sc_string sc_string::substr(int first,int last) const
{
  if(first<0 || last<0 || first>last || first>=length() || last>=length())
    return "";
  return sc_string(rep->str+first, last-first+1);
}


sc_string sc_string::make_str(long n) // convert integer to string
{
  char buf[32];
  ::sprintf(buf,"%ld",n);
  return sc_string(buf);
}


#define DEFINE_RELOP(op) \
bool sc_string::operator op( const char* s ) const \
{						\
    return strcmp( rep->str, s ) op 0;		\
}						\
bool sc_string::operator op( const sc_string& s ) const \
{						\
    return strcmp( rep->str, s.rep->str ) op 0;	\
}

DEFINE_RELOP(==)
DEFINE_RELOP(!=)
DEFINE_RELOP(<)
DEFINE_RELOP(<=)
DEFINE_RELOP(>)
DEFINE_RELOP(>=)

sc_string::operator const char*() const
{
    return rep->str;
}

char
sc_string::operator[]( int i ) const
{
    return rep->str[i];
}

char& sc_string::operator[]( int i )
{
    return rep->str[i];
}

void
sc_string::set( int i, char c )
{
    if (rep->ref_count > 1) {
        rep->ref_count--;
        rep = new sc_string_rep(rep->str);
    }
    rep->str[i] = c;
}

sc_string sc_string::to_string(const char* format, ...)
{
   va_list argptr;
   int cnt;
   sc_string result;
   char buffer[1024]; // static string buffer
   buffer[1023]=000;

   va_start(argptr, format);
#if defined(WIN32)
   // Windows provides safer implementation
#if defined(_MSC_VER)
   cnt = _vsnprintf(buffer, 1024, format, argptr);
#else
   cnt = vsnprintf(buffer, 1024, format, argptr);
#endif
   if(cnt>1023) // string too long
   {
     int buf_size = 1024;
     const int max_size = 65000;
     char* buf; // dynamic buffer
     do
     {
       buf_size*=2;
       buf = new char[buf_size];
#if defined(_MSC_VER)
       cnt = _vsnprintf(buffer, buf_size, format, argptr);
#else
       cnt = vsnprintf(buffer, buf_size, format, argptr);
#endif
       if(buf_size<max_size && cnt>=buf_size)
         delete[] buf;
     }
     while( buf_size<max_size && cnt>=buf_size);
     if(cnt>=buf_size)
     {
       // string is longer the the maximum buffer size (max_size)
       SC_REPORT_WARNING( SC_ID_STRING_TOO_LONG_, "truncated" );
       buf[buf_size-1] = 000;
     }
     result = buf;
     delete[] buf;
   }
   else
     result = buffer;
#else
   try {
     // this may end up in a core dump
     // if we are lucky we can catch exception
     cnt = vsprintf(buffer, format, argptr);
   }
   catch(...)
   {
     SC_REPORT_WARNING( SC_ID_STRING_TOO_LONG_,
			"program may become unstable" );
   }
   buffer[1023]=000; // in case it's longer
   result = buffer;
#endif
   va_end(argptr);

   return result;
}

void
sc_string::print( ostream& os ) const
{
    os << rep->str;
}

void sc_string::test(int position)const
{
	if(position<0 || position>=length())
		SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, "sc_string::test" );
}

// TODO: conveniece formatting functions for common types
//       e.g. sc_string("a=%d, s is %s").fmt(1).fmt("string")
//       should produce a=1, s is string
//       it should be safe: if less arguments specified
//       it should print %specifier; extra arguments should be ignored
//       if the type of the argument is incompatible with format 
//       specifier it should be ignored
//

unsigned
sc_string::fmt_length()const
{
    unsigned result=0;
    if((*this)[0]!='%')
	return 0;
    else
	result++;
    if(is_delimiter("-+0 #",result)) // flags
	result++;
    while(is_delimiter("0123456789*",result)) // width
	result++;
    if(rep->str[result]=='.') // precision
    {
	result++;
	unsigned old_result = result;
	while(is_delimiter("0123456789*",result)) result++;
	if(old_result == result) //error in format
	    return 0;
    }
    if(is_delimiter("hlL",result)) result++; // I64 is not supported
    if(is_delimiter("cCdiouxXeEfgGnpsS",result)) 
	result++;
    else // error in format
	return 0;
    return result;
}

sc_string&
sc_string::fmt(const sc_string& s)
{
    return fmt(s.c_str());
}

int
sc_string::pos( const sc_string& sub_string ) const
{
    int sub_len = sub_string.length();
    if( sub_len == 0 ) {
        return 0; // empty string always matches
    }
    int ind = 0;
    int len = length();
    bool found = false;
    while( ind < len && ! found )
    {
        found = ( sub_string == substr( ind, ind + sub_len - 1 ) );
        ++ ind;
    }
    if( found ) {
        return -- ind;
    } else {
        return -1;
    }
}

sc_string&
sc_string::remove(unsigned index, unsigned length)
{
    test((int)index);
    if(length!=0)
	(*this) = substr(0,index-1) + substr(index+length,this->length()-1);
    return *this;
}

sc_string&
sc_string::insert(const sc_string& sub_string, unsigned index)
{
    if(index>(unsigned)length())   
	SC_REPORT_ERROR( SC_ID_OUT_OF_BOUNDS_, "sc_string::insert" );
    return (*this) = substr(0,index-1)+sub_string+substr(index,length()-1);
}

bool
sc_string::is_delimiter(const sc_string& str, unsigned index)const
{
    test((int)index);
    return str.contains(rep->str[index]);
}

bool
sc_string::contains(char c)const
{
    int len = length();
    int i=0;
    bool found = false;
    while(!found && i<len)
	found = rep->str[i++]==c;
    return found;
}

sc_string
sc_string::uppercase()const
{
    int len = length();
    sc_string temp(*this);
    for(int i=0; i<len; i++)
    {
	char c = temp.rep->str[i];
	if(c>='a' && c<='z')
	    temp.rep->str[i] = static_cast<char>( c-20 );
    }
    return temp;
}

sc_string
sc_string::lowercase()const
{
    int len = length();
    sc_string temp(*this);
    for(int i=0; i<len; i++)
    {
	char c = temp.rep->str[i];
	if(c>='A' && c<='Z')
	    temp.rep->str[i] = static_cast<char>( c+20 );
    }
    return temp;
}


// ----------------------------------------------------------------------------

istream&
operator >> ( istream& is, sc_string& s )
{
    if( s.rep->ref_count > 1 ) {
        -- s.rep->ref_count;
        s.rep = new sc_string_rep;
    }

    int i = 0;
    char* p = s.rep->str;
    char c;

    // skip white spaces
    while( is.get( c ) && isspace( c ) )
        ;

    for( ; is.good() && ! isspace( c ); is.get( c ) ) {
        if( i > s.rep->alloc - 2 ) {
            s.rep->resize( (int) (s.rep->alloc * 1.5) );
            p = s.rep->str + i;
        }
        *p ++ = c;
        i ++;
    }
    *p = '\0';

    return is;
}
