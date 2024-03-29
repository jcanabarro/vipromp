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

  sc_nbutils.cpp -- External and friend functions for both sc_signed and
                    sc_unsigned classes.
 
  Original Author: Ali Dasdan, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include <ctype.h>
#include "systemc/datatypes/int/sc_int_ids.h"
#include "systemc/datatypes/int/sc_nbutils.h"
#include "systemc/kernel/sc_macros.h"


namespace sc_dt
{

// ----------------------------------------------------------------------------
//  SECTION: General utility functions.
// ----------------------------------------------------------------------------

// Return the number of characters to advance the source of c.  This
// function implements one move of the FSM to parse the following
// regular expressions. Error checking is done in the caller.

small_type
fsm_move(char c, small_type &b, small_type &s, small_type &state)
{

  // Possible regular expressions (REs):
  // Let N = any digit depending on the base.
  //    1. [0|1|..|9]N*
  //    2. [+|-][0|1|..|9]N*
  //    3. 0[b|B|d|D|o|O|x|X][0|1|..|F]N*
  //    4. [+|-]?0[b|B|d|D|o|O|x|X][0|1|..|F]N*
  //
  // The finite state machine (FMS) to parse these regular expressions
  // has 4 states, 0 to 3. 0 is the initial state and 3 is the final
  // state.
  //
  // Default sign = SC_POS, default base = NB_DEFAULT_BASE.

  switch (state) {

  case 0: // The initial state.
    switch (c) { 
    case '0': s = SC_POS; state = 1; return 0; // RE 1 or 3
    case '+': s = SC_POS; state = 2; return 1; // RE 2
    case '-': s = SC_NEG; state = 2; return 1; // RE 2
    default:  s = SC_POS; b = NB_DEFAULT_BASE; state = 3; return 0; // RE 1
    }
    // break; //unreachable code
  case 1: // 0...
    switch (c) {
    case 'x': case 'X': b = SC_HEX; state = 3; return 2; // RE 3 or 4
    case 'd': case 'D': b = SC_DEC; state = 3; return 2; // RE 3 or 4
    case 'o': case 'O': b = SC_OCT; state = 3; return 2; // RE 3 or 4
    case 'b': case 'B': b = SC_BIN; state = 3; return 2; // RE 3 or 4
    default:  b = NB_DEFAULT_BASE; state = 3; return 0; // RE 1
    }
    // break; //unreachable code
  case 2: // +... or -...
    switch (c) {
    case '0': state = 1; return 0; // RE 2 or 4
    default:  b = NB_DEFAULT_BASE; state = 3; return 0; // RE 2
    }
    // break; //unreachable code
  case 3: // The final state.
    break;

  default:
    // Any other state is not possible.
    assert((0 <= state) && (state <= 3));

  } // switch

  return 0;

}  


// Get base b and sign s of the number in the char string v. Return a
// pointer to the first char after the point where b and s are
// determined or where the end of v is reached. The input string v has
// to be null terminated.
const char 
*get_base_and_sign(const char *v, small_type &b, small_type &s)
{

#ifdef DEBUG_SYSTEMC
  assert(v != NULL);
#endif

  const small_type STATE_START = 0;
  const small_type STATE_FINISH = 3;

  // Default sign = SC_POS, default base = 10.
  s = SC_POS;
  b = NB_DEFAULT_BASE;

  small_type state = STATE_START;
  small_type nskip = 0; // Skip that many chars.
  const char *u = v;

  while (*u) {
    if (isspace(*u))  // Skip white space.
      ++u;
    else {
      nskip += fsm_move(*u, b, s, state);
      if (state == STATE_FINISH)
        break;
      else
        ++u;
    }
  }

#ifdef DEBUG_SYSTEMC
  // Test to see if the above loop executed more than it should
  // have. The max number of skipped chars is equal to the length of
  // the longest format specifier, e.g., "-0x".
  assert(nskip <= 3);
#endif

  v += nskip;

  // Handles empty strings or strings without any digits after the
  // base or base and sign specifier.
  if (*v == '\0') { 
      char msg[BUFSIZ];
      sprintf( msg,
	       "get_base_and_sign( const char* v, small_type&, small_type& ) : "
	       "v = \"\" is not valid" );
      SC_REPORT_ERROR( SC_ID_CONVERSION_FAILED_, msg );
  }

  return v;

}


// ----------------------------------------------------------------------------
//  SECTION: Utility functions involving unsigned vectors.
// ----------------------------------------------------------------------------

// Read u from a null terminated char string v. Note that operator>>
// in sc_nbcommon.cpp is similar to this function.
small_type
vec_from_str(int unb, int und, unsigned long *u, 
             const char *v, sc_numrep base) 
{

#ifdef DEBUG_SYSTEMC
  assert((unb > 0) && (und > 0) && (u != NULL));
  assert(v != NULL);
#endif

  is_valid_base(base);

  small_type b, s;  // base and sign.

  v = get_base_and_sign(v, b, s);

  if (base != SC_NOBASE) {
    if (b == NB_DEFAULT_BASE)
      b = base;
    else {
	char msg[BUFSIZ];
	sprintf( msg,
		 "vec_from_str( int, int, unsigned long*, const char*, sc_numrep base ) : "
		 "base = %s does not match the default base",
		 to_string( base ).c_str() );
	SC_REPORT_ERROR( SC_ID_CONVERSION_FAILED_, msg );
    }
  }

  vec_zero(und, u);

  char c;
  
  for ( ; (c = *v); ++v) {

    if (isalnum(c)) {
      
      small_type val;  // Numeric value of a char.
    
      if (isalpha(c)) // Hex digit.
        val = toupper(c) - 'A' + 10;
      else
        val = c - '0';
      
      if (val >= b) {
	  char msg[BUFSIZ];
	  sprintf( msg,
		   "vec_from_str( int, int, unsigned long*, const char*, sc_numrep base ) : "
		   "'%c' is not a valid digit in base %d",
		   *v, b );
	  SC_REPORT_ERROR( SC_ID_CONVERSION_FAILED_, msg );
      }
      
      // digit = digit * b + val;
      vec_mul_small_on(und, u, b);
      
      if (val)
        vec_add_small_on(und, u, val);

    }
    else {
	char msg[BUFSIZ];
	sprintf( msg,
		 "vec_from_str( int, int, unsigned long*, const char*, sc_numrep base ) : "
		 "'%c' is not a valid digit in base %d",
		 *v, b );
	SC_REPORT_ERROR( SC_ID_CONVERSION_FAILED_, msg );
    }
  }

  return convert_signed_SM_to_2C_to_SM(s, unb, und, u);
}


// All vec_ functions assume that the vector to hold the result,
// called w, has sufficient length to hold the result. For efficiency
// reasons, we do not test whether or not we are out of bounds.

// Compute w = u + v, where w, u, and v are vectors. 
// - ulen >= vlen
// - wlen >= sc_max(ulen, vlen) + 1
void
vec_add(int ulen, const unsigned long *u,
        int vlen, const unsigned long *v,
        unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(w != NULL);
  assert(ulen >= vlen);
#endif

  const unsigned long *uend = (u + ulen);
  const unsigned long *vend = (v + vlen);

  register unsigned long carry = 0;   // Also used as sum to save space.

  // Add along the shorter v.
  while (v < vend) {
    carry += (*u++) + (*v++);
    (*w++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

  // Propagate the carry.
  while (carry && (u < uend)) {
    carry = (*u++) + 1;
    (*w++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

  // Copy the rest of u to the result.
  while (u < uend)
    (*w++) = (*u++);

  // Propagate the carry if it is still 1.
  if (carry)
    (*w) = 1;

}


// Compute u += v, where u and v are vectors.
// - ulen >= vlen
void
vec_add_on(int ulen, unsigned long *ubegin, 
           int vlen, const unsigned long *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (ubegin != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(ulen >= vlen);
#endif

  register unsigned long *u = ubegin;
  const unsigned long *uend = (u + ulen);
  const unsigned long *vend = (v + vlen);

  register unsigned long carry = 0;   // Also used as sum to save space.

  // Add along the shorter v.
  while (v < vend) {
    carry += (*u) + (*v++);
    (*u++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

  // Propagate the carry.
  while (carry && (u < uend)) {
    carry = (*u) + 1;
    (*u++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

#ifdef DEBUG_SYSTEMC
  if( carry != 0 ) {
      SC_REPORT_WARNING( SC_ID_WITHOUT_MESSAGE_,
			 "vec_add_on( int, unsigned long*, int, const "
			 "unsigned long* ) : "
			 "result of addition is wrapped around" );
  }
#endif

}


// Compute u += v, where u and v are vectors.
// - ulen < vlen
void
vec_add_on2(int ulen, unsigned long *ubegin, 
            int
#ifdef DEBUG_SYSTEMC
            vlen
#endif
            , const unsigned long *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (ubegin != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(ulen < vlen);
#endif

  register unsigned long *u = ubegin;
  const unsigned long *uend = (u + ulen);

  register unsigned long carry = 0;   // Also used as sum to save space.

  // Add along the shorter u.
  while (u < uend) {
    carry += (*u) + (*v++);
    (*u++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

#ifdef DEBUG_SYSTEMC
  if( carry != 0 ) {
      SC_REPORT_WARNING( SC_ID_WITHOUT_MESSAGE_,
                         "vec_add_on2( int, unsigned long*, int, const "
			 "unsigned long* ) : "
			 "result of addition is wrapped around" );
  }
#endif
}


// Compute w = u + v, where w and u are vectors, and v is a scalar.
void
vec_add_small(int ulen, const unsigned long *u,
              unsigned long v, 
              unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert(w != NULL);
#endif

  const unsigned long *uend = (u + ulen);

  // Add along the shorter v.
  register unsigned long carry = (*u++) + v;
  (*w++) = carry & DIGIT_MASK;
  carry >>= BITS_PER_DIGIT;

  // Propagate the carry.
  while (carry && (u < uend)) {
    carry = (*u++) + 1;
    (*w++) = carry & DIGIT_MASK;
    carry >>= BITS_PER_DIGIT;
  }

  // Copy the rest of u to the result.
  while (u < uend)
    (*w++) = (*u++);

  // Propagate the carry if it is still 1.
  if (carry)
    (*w) = 1;

}

// Compute u += v, where u is vectors, and v is a scalar.
void 
vec_add_small_on(int ulen, unsigned long *u, unsigned long v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
#endif

  register int i = 0;

  while (v && (i < ulen)) {
    v += u[i];
    u[i++] = v & DIGIT_MASK;
    v >>= BITS_PER_DIGIT;
  }

#ifdef DEBUG_SYSTEMC
  if( v != 0 ) {
      SC_REPORT_WARNING( SC_ID_WITHOUT_MESSAGE_,
                         "vec_add_small_on( int, unsigned long*, unsigned "
			 "long ) : "
			 "result of addition is wrapped around" );
  }
#endif

}

// Compute w = u - v, where w, u, and v are vectors.
// - ulen >= vlen
// - wlen >= sc_max(ulen, vlen)
void
vec_sub(int ulen, const unsigned long *u,
        int vlen, const unsigned long *v,
        unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(w != NULL);
  assert(ulen >= vlen);
#endif

  const unsigned long *uend = (u + ulen);
  const unsigned long *vend = (v + vlen);

  register unsigned long borrow = 0;   // Also used as diff to save space.

  // Subtract along the shorter v.
  while (v < vend) {
    borrow = ((*u++) + DIGIT_RADIX) - (*v++) - borrow;
    (*w++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

  // Propagate the borrow.
  while (borrow && (u < uend)) {
    borrow = ((*u++) + DIGIT_RADIX) - 1;
    (*w++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

#ifdef DEBUG_SYSTEMC
  assert(borrow == 0);
#endif

  // Copy the rest of u to the result.
  while (u < uend)
    (*w++) = (*u++);

}

// Compute u = u - v, where u and v are vectors.
// - u > v
// - ulen >= vlen
void
vec_sub_on(int ulen, unsigned long *ubegin,
           int vlen, const unsigned long *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (ubegin != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(ulen >= vlen);
#endif

  register unsigned long *u = ubegin;
  const unsigned long *uend = (u + ulen);
  const unsigned long *vend = (v + vlen);

  register unsigned long borrow = 0;   // Also used as diff to save space.

  // Subtract along the shorter v.
  while (v < vend) {
    borrow = ((*u) + DIGIT_RADIX) - (*v++) - borrow;
    (*u++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

  // Propagate the borrow.
  while (borrow && (u < uend)) {
    borrow = ((*u) + DIGIT_RADIX) - 1;
    (*u++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

#ifdef DEBUG_SYSTEMC
  assert(borrow == 0);
#endif

}

// Compute u = v - u, where u and v are vectors.
// - v > u
// - ulen <= vlen or ulen > ulen
void
vec_sub_on2(int ulen, unsigned long *ubegin,
            int vlen, const unsigned long *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (ubegin != NULL));
  assert((vlen > 0) && (v != NULL));
#endif

  register unsigned long *u = ubegin;
  const unsigned long *uend = (u + sc_min(ulen, vlen));

  register unsigned long borrow = 0;   // Also used as diff to save space.

  // Subtract along the shorter u.
  while (u < uend) {
    borrow = ((*v++) + DIGIT_RADIX) - (*u) - borrow;
    (*u++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

#ifdef DEBUG_SYSTEMC
  if( borrow != 0 ) {
      SC_REPORT_WARNING( SC_ID_WITHOUT_MESSAGE_,
                         "vec_sub_on2( int, unsigned long*, int, const "
			 "unsigned long* ) : "
			 "result of subtraction is wrapped around" );
  }
#endif
}

// Compute w = u - v, where w and u are vectors, and v is a scalar.
void
vec_sub_small(int ulen, const unsigned long *u,
              unsigned long v, 
              unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert(ulen > 0);
  assert(u != NULL);
#endif

  const unsigned long *uend = (u + ulen);

  // Add along the shorter v.
  register unsigned long borrow = ((*u++) + DIGIT_RADIX) - v;
  (*w++) = borrow & DIGIT_MASK;
  borrow = 1 - (borrow >> BITS_PER_DIGIT);

  // Propagate the borrow.
  while (borrow && (u < uend)) {
    borrow = ((*u++) + DIGIT_RADIX) - 1;
    (*w++) = borrow & DIGIT_MASK;
    borrow = 1 - (borrow >> BITS_PER_DIGIT);
  }

#ifdef DEBUG_SYSTEMC
  assert(borrow == 0);
#endif

  // Copy the rest of u to the result.
  while (u < uend)
    (*w++) = (*u++);

}


// Compute u -= v, where u is vectors, and v is a scalar.
void 
vec_sub_small_on(int ulen, unsigned long *u, unsigned long v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
#endif

  for (register int i = 0; i < ulen; ++i) {
    v = (u[i] + DIGIT_RADIX) - v;    
    u[i] = v & DIGIT_MASK;
    v = 1 - (v >> BITS_PER_DIGIT);
  }

#ifdef DEBUG_SYSTEMC
  assert(v == 0);
#endif

}

// Compute w = u * v, where w, u, and v are vectors.
void
vec_mul(int ulen, const unsigned long *u,
        int vlen, const unsigned long *vbegin, 
        unsigned long *wbegin)
{

  /* Consider u = Ax + B and v = Cx + D where x is equal to
     HALF_DIGIT_RADIX. In other words, A is the higher half of u and
     B is the lower half of u. The interpretation for v is
     similar. Then, we have the following picture:

              u_h     u_l
     u: -------- --------
               A        B

              v_h     v_l
     v: -------- --------
               C        D
 
     result (d):                     
     carry_before:                           -------- --------
                                              carry_h  carry_l
     result_before:        -------- -------- -------- --------
                               R1_h     R1_l     R0_h     R0_l
                                             -------- --------
                                                 BD_h     BD_l
                                    -------- --------
                                        AD_h     AD_l
                                    -------- --------
                                        BC_h     BC_l
                           -------- --------
                               AC_h     AC_l
     result_after:         -------- -------- -------- --------
                              R1_h'    R1_l'    R0_h'    R0_l'
 
     prod_l = R0_h|R0_l + B * D  + 0|carry_l
            = R0_h|R0_l + BD_h|BD_l + 0|carry_l
 
     prod_h = A * D + B * C + high_half(prod_l) + carry_h
            = AD_h|AD_l + BC_h|BC_l + high_half(prod_l) + 0|carry_h
 
     carry = A * C + high_half(prod_h)
           = AC_h|AC_l + high_half(prod_h)
 
     R0_l' = low_half(prod_l)
 
     R0_h' = low_half(prod_h)
 
     R0 = high_half(prod_h)|low_half(prod_l)
 
     where '|' is the concatenation operation and the suffixes 0 and 1
     show the iteration number, i.e., 0 is the current iteration and 1
     is the next iteration.
 
     NOTE: sc_max(prod_l, prod_h, carry) <= 2 * x^2 - 1, so any
     of these numbers can be stored in a digit.

     NOTE: low_half(u) returns the lower BITS_PER_HALF_DIGIT of u,
     whereas high_half(u) returns the rest of the bits, which may
     contain more bits than BITS_PER_HALF_DIGIT.  
  */

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (vbegin != NULL));
  assert(wbegin != NULL);
#endif

#define prod_h carry

  const unsigned long *uend = (u + ulen);
  const unsigned long *vend = (vbegin + vlen);

  while (u < uend) {

    unsigned long u_h = (*u++);        // A|B
    unsigned long u_l = low_half(u_h); // B
    u_h = high_half(u_h);           // A

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(u_h == (u_h & HALF_DIGIT_MASK));
#endif

    register unsigned long carry = 0;

    register unsigned long *w = (wbegin++);

    register const unsigned long *v = vbegin;

    while (v < vend) {

      unsigned long v_h = (*v++);         // C|D
      unsigned long v_l = low_half(v_h);  // D

      v_h = high_half(v_h);            // C

#ifdef DEBUG_SYSTEMC
      // The overflow bits must be zero.
      assert(v_h == (v_h & HALF_DIGIT_MASK));
#endif

      unsigned long prod_l = (*w) + u_l * v_l + low_half(carry);

      prod_h = u_h * v_l + u_l * v_h + high_half(prod_l) + high_half(carry);

      (*w++) = concat(low_half(prod_h), low_half(prod_l));

      carry = u_h * v_h + high_half(prod_h);

    }

    (*w) = carry;

  }

#undef prod_h

}

// Compute w = u * v, where w and u are vectors, and v is a scalar. 
// - 0 < v < HALF_DIGIT_RADIX.
void
vec_mul_small(int ulen, const unsigned long *u,
              unsigned long v, unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert(w != NULL);
  assert((0 < v) && (v < HALF_DIGIT_RADIX));
#endif

#define prod_h carry

  const unsigned long *uend = (u + ulen);

  register unsigned long carry = 0;

  while (u < uend) {

    unsigned long u_AB = (*u++);

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(high_half(u_AB) == high_half_masked(u_AB));
#endif

    unsigned long prod_l = v * low_half(u_AB) + low_half(carry);

    prod_h = v * high_half(u_AB) + high_half(prod_l) + high_half(carry);

    (*w++) = concat(low_half(prod_h), low_half(prod_l));

    carry = high_half(prod_h);

  }

  (*w) = carry;

#undef prod_h

}

// Compute u = u * v, where u is a vector, and v is a scalar.
// - 0 < v < HALF_DIGIT_RADIX. 
void
vec_mul_small_on(int ulen, unsigned long *u, unsigned long v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((0 < v) && (v < HALF_DIGIT_RADIX));
#endif

#define prod_h carry

  register unsigned long carry = 0;

  for (register int i = 0; i < ulen; ++i) {

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(high_half(u[i]) == high_half_masked(u[i]));
#endif

    unsigned long prod_l = v * low_half(u[i]) + low_half(carry);

    prod_h = v * high_half(u[i]) + high_half(prod_l) + high_half(carry);

    u[i] = concat(low_half(prod_h), low_half(prod_l));

    carry = high_half(prod_h);

  }

#undef prod_h

#ifdef DEBUG_SYSTEMC
  if( carry != 0 ) {
      SC_REPORT_WARNING( SC_ID_WITHOUT_MESSAGE_,
                         "vec_mul_small_on( int, unsigned long*, unsigned "
			 "long ) : "
			 "result of multiplication is wrapped around" );
  }
#endif
}

// Compute w = u / v, where w, u, and v are vectors. 
// - u and v are assumed to have at least two digits as uchars.
void
vec_div_large(int ulen, const unsigned long *u,
              int vlen, const unsigned long *v,
              unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(w != NULL);
  assert(BITS_PER_DIGIT >= 3 * BITS_PER_BYTE);
#endif

  // We will compute q = x / y where x = u and y = v. The reason for
  // using x and y is that x and y are BYTE_RADIX copies of u and v,
  // respectively. The use of BYTE_RADIX radix greatly simplifies the
  // complexity of the division operation. These copies are also
  // needed even when we use DIGIT_RADIX representation.

  int xlen = BYTES_PER_DIGIT * ulen + 1;
  int ylen = BYTES_PER_DIGIT * vlen;

#ifdef SC_MAX_NBITS
  uchar x[DIV_CEIL2(SC_MAX_NBITS, BITS_PER_BYTE)];
  uchar y[DIV_CEIL2(SC_MAX_NBITS, BITS_PER_BYTE)];
  uchar q[DIV_CEIL2(SC_MAX_NBITS, BITS_PER_BYTE)];
#else
  uchar *x = new uchar[xlen];
  uchar *y = new uchar[ylen];
  uchar *q = new uchar[xlen - ylen + 1];
#endif

  // q corresponds to w.

  // Set (uchar) x = (unsigned long) u.
  xlen = vec_to_char(ulen, u, xlen, x);

  // Skip all the leading zeros in x.
  while ((--xlen >= 0) && (! x[xlen]));
  xlen++;

  // Set (uchar) y = (unsigned long) v.
  ylen = vec_to_char(vlen, v, ylen, y);

  // Skip all the leading zeros in y.
  while ((--ylen >= 0) && (! y[ylen]));
  ylen++;

#ifdef DEBUG_SYSTEMC
  assert(xlen > 1);
  assert(ylen > 1);
#endif

  // At this point, all the leading zeros are eliminated from x and y.

  // Zero the last digit of x.
  x[xlen] = 0;

  // The first two digits of y.
  register unsigned long y2 = (y[ylen - 1] << BITS_PER_BYTE) + y[ylen - 2];

  const unsigned long DOUBLE_BITS_PER_BYTE = 2 * BITS_PER_BYTE;

  // Find each q[k].
  for (register int k = xlen - ylen; k >= 0; --k) {

    // qk is a guess for q[k] such that q[k] = qk or qk - 1.
    register unsigned long qk;

    // Find qk by just using 2 digits of y and 3 digits of x. The
    // following code assumes that sizeof(unsigned long) >= 3 BYTEs.
    int k2 = k + ylen;

    qk = ((x[k2] << DOUBLE_BITS_PER_BYTE) + 
          (x[k2 - 1] << BITS_PER_BYTE) + x[k2 - 2]) / y2;

    if (qk >= BYTE_RADIX)     // qk cannot be larger than the largest
      qk = BYTE_RADIX - 1;    // digit in BYTE_RADIX.

    // q[k] = qk or qk - 1. The following if-statement determines which:
    if (qk) {

      register uchar *xk = (x + k);  // A shortcut for x[k].

      // x = x - y * qk :
      register unsigned long carry = 0;

      for (register int i = 0; i < ylen; ++i) {
        carry += y[i] * qk;
        unsigned long diff = (xk[i] + BYTE_RADIX) - (carry & BYTE_MASK);
        xk[i] = (uchar)(diff & BYTE_MASK);
        carry = (carry >> BITS_PER_BYTE) + (1 - (diff >> BITS_PER_BYTE));
      }

      // If carry, qk may be one too large.
      if (carry) {

        // 2's complement the last digit.
        carry = (xk[ylen] + BYTE_RADIX) - carry;
        xk[ylen] = (uchar)(carry & BYTE_MASK);
        carry = 1 - (carry >> BITS_PER_BYTE);
        
        if (carry) {

          // qk was one too large, so decrement it.
          --qk;
        
          // Since qk was decreased by one, y must be added to x:
          // That is, x = x - y * (qk - 1) = x - y * qk + y = x_above + y.
          carry = 0;

          for (register int i = 0; i < ylen; ++i) {
            carry += xk[i] + y[i];
            xk[i] = (uchar)(carry & BYTE_MASK);
            carry >>= BITS_PER_BYTE;
          }

          if (carry)
            xk[ylen] = (uchar)((xk[ylen] + 1) & BYTE_MASK);

        }  // second if carry
      }  // first if carry
    }  // if qk

    q[k] = (uchar)qk;

  }  // for k

  // Set (unsigned long) w = (uchar) q.
  vec_from_char(xlen - ylen + 1, q, ulen, w);

#ifndef SC_MAX_NBITS
  delete [] x;
  delete [] y;
  delete [] q;
#endif

}

// Compute w = u / v, where u and w are vectors, and v is a scalar.
// - 0 < v < HALF_DIGIT_RADIX. Below, we rename w to q.
void 
vec_div_small(int ulen, const unsigned long *u,
              unsigned long v, unsigned long *q)
{

  // Given (u = u_1u_2...u_n)_b = (q = q_1q_2...q_n) * v + r, where b
  // is the base, and 0 <= r < v. Then, the algorithm is as follows:
  //
  // r = 0; 
  // for (j = 1; j <= n; j++) {
  //   q_j = (r * b + u_j) / v;
  //   r = (r * b + u_j) % v;
  // }
  // 
  // In our case, b = DIGIT_RADIX, and u = Ax + B and q = Cx + D where
  // x = HALF_DIGIT_RADIX. Note that r < v < x and b = x^2. Then, a
  // typical situation is as follows:
  //
  // ---- ----
  // 0    r
  //           ---- ----
  //           A    B
  //      ---- ---- ----
  //      r    A    B     = r * b + u
  //
  // Hence, C = (r|A) / v.
  //        D = (((r|A) % v)|B) / v
  //        r = (((r|A) % v)|B) % v

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert(q != NULL);
  assert((0 < v) && (v < HALF_DIGIT_RADIX));
#endif

#define q_h r

  register unsigned long r = 0;
  const unsigned long *ubegin = u;

  u += ulen;
  q += ulen;

  while (ubegin < u) {

    unsigned long u_AB = (*--u);       // A|B

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(high_half(u_AB) == high_half_masked(u_AB));
#endif

    register unsigned long num = concat(r, high_half(u_AB));  // num = r|A
    q_h = num / v;                           // C
    num = concat((num % v), low_half(u_AB)); // num = (((r|A) % v)|B) 
    (*--q) = concat(q_h, num / v);           // q = C|D
    r = num % v;

  }

#undef q_h

}

// Compute w = u % v, where w, u, and v are vectors. 
// - u and v are assumed to have at least two digits as uchars.
void
vec_rem_large(int ulen, const unsigned long *u,
              int vlen, const unsigned long *v,
              unsigned long *w)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(w != NULL);
  assert(BITS_PER_DIGIT >= 3 * BITS_PER_BYTE);
#endif

  // This function is adapted from vec_div_large.

  int xlen = BYTES_PER_DIGIT * ulen + 1;
  int ylen = BYTES_PER_DIGIT * vlen;

#ifdef SC_MAX_NBITS
  uchar x[DIV_CEIL2(SC_MAX_NBITS, BITS_PER_BYTE)];
  uchar y[DIV_CEIL2(SC_MAX_NBITS, BITS_PER_BYTE)];
#else
  uchar *x = new uchar[xlen];
  uchar *y = new uchar[ylen];
#endif

  // r corresponds to w.

  // Set (uchar) x = (unsigned long) u.
  xlen = vec_to_char(ulen, u, xlen, x);

  // Skip all the leading zeros in x.
  while ((--xlen >= 0) && (! x[xlen]));
  xlen++;

  // Set (uchar) y = (unsigned long) v.
  ylen = vec_to_char(vlen, v, ylen, y);

  // Skip all the leading zeros in y.
  while ((--ylen >= 0) && (! y[ylen]));
  ylen++;

#ifdef DEBUG_SYSTEMC
  assert(xlen > 1);
  assert(ylen > 1);
#endif

  // At this point, all the leading zeros are eliminated from x and y.

  // Zero the last digit of x.
  x[xlen] = 0;

  // The first two digits of y.
  register unsigned long y2 = (y[ylen - 1] << BITS_PER_BYTE) + y[ylen - 2];

  const unsigned long DOUBLE_BITS_PER_BYTE = 2 * BITS_PER_BYTE;

  // Find each q[k].
  for (register int k = xlen - ylen; k >= 0; --k) {

    // qk is a guess for q[k] such that q[k] = qk or qk - 1.
    register unsigned long qk;

    // Find qk by just using 2 digits of y and 3 digits of x. The
    // following code assumes that sizeof(unsigned long) >= 3 BYTEs.
    int k2 = k + ylen;

    qk = ((x[k2] << DOUBLE_BITS_PER_BYTE) +
      (x[k2 - 1] << BITS_PER_BYTE) + x[k2 - 2]) / y2;

    if (qk >= BYTE_RADIX)     // qk cannot be larger than the largest
      qk = BYTE_RADIX - 1;    // digit in BYTE_RADIX.

    // q[k] = qk or qk - 1. The following if-statement determines which.
    if (qk) {

      register uchar *xk = (x + k);  // A shortcut for x[k].

      // x = x - y * qk;
      register unsigned long carry = 0;

      for (register int i = 0; i < ylen; ++i) {
        carry += y[i] * qk;
        unsigned long diff = (xk[i] + BYTE_RADIX) - (carry & BYTE_MASK);
        xk[i] = (uchar)(diff & BYTE_MASK);
        carry = (carry >> BITS_PER_BYTE) + (1 - (diff >> BITS_PER_BYTE));
      }

      if (carry) {

        // 2's complement the last digit.
        carry = (xk[ylen] + BYTE_RADIX) - carry;
        xk[ylen] = (uchar)(carry & BYTE_MASK);
        carry = 1 - (carry >> BITS_PER_BYTE);
        
        if (carry) {

          // qk was one too large, so decrement it.
          // --qk;
        
          // x = x - y * (qk - 1) = x - y * qk + y = x_above + y.
          carry = 0;

          for (register int i = 0; i < ylen; ++i) {
            carry += xk[i] + y[i];
            xk[i] = (uchar)(carry & BYTE_MASK);
            carry >>= BITS_PER_BYTE;
          }

          if (carry)
            xk[ylen] = (uchar)((xk[ylen] + 1) & BYTE_MASK);

        }  // second if carry
      } // first if carry
    }  // if qk
  }  // for k

  // Set (unsigned long) w = (uchar) x for the remainder.
  vec_from_char(ylen, x, ulen, w);

#ifndef SC_MAX_NBITS
  delete [] x;
  delete [] y;
#endif

}

// Compute r = u % v, where u is a vector, and r and v are scalars.
// - 0 < v < HALF_DIGIT_RADIX. 
// - The remainder r is returned.
unsigned long
vec_rem_small(int ulen, const unsigned long *u, unsigned long v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((0 < v) && (v < HALF_DIGIT_RADIX));
#endif

  // This function is adapted from vec_div_small().

  register unsigned long r = 0;
  const unsigned long *ubegin = u;

  u += ulen;

  while (ubegin < u) {
    register unsigned long u_AB = (*--u);  // A|B

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(high_half(u_AB) == high_half_masked(u_AB));
#endif

    // r = (((r|A) % v)|B) % v
    r = (concat(((concat(r, high_half(u_AB))) % v), low_half(u_AB))) % v;
  }

  return r;

}

// u = u / v, r = u % v.
unsigned long
vec_rem_on_small(int ulen, unsigned long *u, unsigned long v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert(v > 0);
#endif

#define q_h r

  register unsigned long r = 0;
  const unsigned long *ubegin = u;

  u += ulen;

  while (ubegin < u) {

    unsigned long u_AB = (*--u);       // A|B

#ifdef DEBUG_SYSTEMC
    // The overflow bits must be zero.
    assert(high_half(u_AB) == high_half_masked(u_AB));
#endif

    register unsigned long num = concat(r, high_half(u_AB));  // num = r|A
    q_h = num / v;                           // C
    num = concat((num % v), low_half(u_AB)); // num = (((r|A) % v)|B) 
    (*u) = concat(q_h, num / v);             // q = C|D
    r = num % v;

  }

#undef q_h

  return r;

}

// Set (uchar) v = (unsigned long) u. Return the new vlen.
int
vec_to_char(int ulen, const unsigned long *u,
            int vlen, uchar *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
#endif

  register int nbits = ulen * BITS_PER_DIGIT;

  register int right = 0;
  register int left = right + BITS_PER_BYTE - 1;

  vlen = 0;

  while (nbits > 0) {

    int left_digit = left / BITS_PER_DIGIT;
    int right_digit = right / BITS_PER_DIGIT;

    register int nsr = ((vlen << LOG2_BITS_PER_BYTE) % BITS_PER_DIGIT);

    int d = u[right_digit] >> nsr;

    if (left_digit != right_digit) {

      if (left_digit < ulen)
        d |= u[left_digit] << (BITS_PER_DIGIT - nsr);

    }

    v[vlen++] = (uchar)(d & BYTE_MASK);

    left += BITS_PER_BYTE;
    right += BITS_PER_BYTE;
    nbits -= BITS_PER_BYTE;

  }

  return vlen;

}

// Set (unsigned long) v = (uchar) u. 
// - sizeof(uchar) <= sizeof(unsigned long), 
void 
vec_from_char(int ulen, const uchar *u, 
              int vlen, unsigned long *v)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
  assert((vlen > 0) && (v != NULL));
  assert(sizeof(uchar) <= sizeof(unsigned long));
#endif

  unsigned long *vend = (v + vlen);

  const int nsr = BITS_PER_DIGIT - BITS_PER_BYTE;
  const unsigned long mask = one_and_ones(nsr);

  (*v) = (unsigned long) u[ulen - 1];

  for (register int i = ulen - 2; i >= 0; --i) {

    // Manual inlining of vec_shift_left().

    register unsigned long *viter = v;

    register unsigned long carry = 0;

    while (viter < vend) {
      register unsigned long vval = (*viter);
      (*viter++) = (((vval & mask) << BITS_PER_BYTE) | carry);
      carry = vval >> nsr;
    }

    if (viter < vend)
      (*viter) = carry;

    (*v) |= (unsigned long) u[i];

  }

}

// Set u <<= nsl.
// If nsl is negative, it is ignored.
void 
vec_shift_left(int ulen, unsigned long *u, int nsl)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
#endif

  if (nsl <= 0)
    return;

  // Shift left whole digits if nsl is large enough.
  if (nsl >= (int) BITS_PER_DIGIT) {

    int nd;

    if (nsl % BITS_PER_DIGIT == 0) {
      nd = nsl / BITS_PER_DIGIT;  // No need to use DIV_CEIL(nsl).
      nsl = 0;
    }
    else {
      nd = DIV_CEIL(nsl) - 1;
      nsl -= nd * BITS_PER_DIGIT;
    }

    if (nd) {

      // Shift left for nd digits.
      for (register int j = ulen - 1; j >= nd; --j)
        u[j] = u[j - nd];
      
      vec_zero( sc_min( nd, ulen ), u );
      
    }

    if (nsl == 0)
      return;

  }

  // Shift left if nsl < BITS_PER_DIGIT.
  register unsigned long *uiter = u;
  unsigned long *uend = uiter + ulen;

  int nsr = BITS_PER_DIGIT - nsl;
  unsigned long mask = one_and_ones(nsr);

  register unsigned long carry = 0;

  while (uiter < uend) {
    register unsigned long uval = (*uiter);
    (*uiter++) = (((uval & mask) << nsl) | carry);
    carry = uval >> nsr;
  }

  if (uiter < uend)
    (*uiter) = carry;

}

// Set u >>= nsr.
// If nsr is negative, it is ignored.
void 
vec_shift_right(int ulen, unsigned long *u, int nsr, unsigned long fill)
{

#ifdef DEBUG_SYSTEMC
  assert((ulen > 0) && (u != NULL));
#endif

  // fill is usually either 0 or DIGIT_MASK; it can be any value.

  if (nsr <= 0)
    return;

  // Shift right whole digits if nsr is large enough.
  if (nsr >= (int) BITS_PER_DIGIT) {

    int nd;

    if (nsr % BITS_PER_DIGIT == 0) {
      nd = nsr / BITS_PER_DIGIT;
      nsr = 0;
    }
    else {
      nd = DIV_CEIL(nsr) - 1;
      nsr -= nd * BITS_PER_DIGIT;
    }
    
    if (nd) {

      // Shift right for nd digits.
      for (register int j = 0; j < (ulen - nd); ++j)
        u[j] = u[j + nd];

      if (fill) {
        for (register int j = ulen - sc_min( nd, ulen ); j < ulen; ++j)
          u[j] = fill;
      }
      else
        vec_zero(ulen - sc_min( nd, ulen ), ulen, u);
     
    }

    if (nsr == 0)
      return;

  }

  // Shift right if nsr < BITS_PER_DIGIT.
  unsigned long *ubegin = u;
  register unsigned long *uiter = (ubegin + ulen);

  int nsl = BITS_PER_DIGIT - nsr;
  unsigned long mask = one_and_ones(nsr);

  register unsigned long carry = (fill & mask) << nsl;

  while (ubegin < uiter) {
    register unsigned long uval = (*--uiter);
    (*uiter) = (uval >> nsr) | carry;
    carry = (uval & mask) << nsl;
  }

}


// Let u[l..r], where l and r are left and right bit positions
// respectively, be equal to its mirror image.
void
vec_reverse(int unb, int und, unsigned long *ud, 
            int l, int r)
{

#ifdef DEBUG_SYSTEMC
  assert((unb > 0) && (und > 0) && (ud != NULL));
  assert((0 <= r) && (r <= l) && (l < unb));
#endif

  if (l < r) {
      char msg[BUFSIZ];
      sprintf( msg, "vec_reverse( int, int, unsigned long*, int l, int r ) : "
	       "l = %d < r = %d is not valid",
	       l, r );
      SC_REPORT_ERROR( SC_ID_CONVERSION_FAILED_, msg );
  }

  // Make sure that l and r are within bounds.
  r = sc_max(r, 0);
  l = sc_min(l, unb - 1);

  // Allocate memory for processing.
#ifdef SC_MAX_NBITS
  unsigned long d[MAX_NDIGITS];
#else
  unsigned long *d = new unsigned long[und];
#endif

  // d is a copy of ud.
  vec_copy(und, d, ud);

  // Based on the value of the ith in d, find the value of the jth bit
  // in ud.

  for (register int i = l, j = r; i >= r; --i, ++j) {

    if ((d[digit_ord(i)] & one_and_zeros(bit_ord(i))) != 0) // Test.
      ud[digit_ord(j)] |= one_and_zeros(bit_ord(j));     // Set.
    else  
      ud[digit_ord(j)] &= ~(one_and_zeros(bit_ord(j)));  // Clear.

  }

#ifndef SC_MAX_NBITS
  delete [] d;
#endif
    
}

} // namespace sc_dt


// End of file.
