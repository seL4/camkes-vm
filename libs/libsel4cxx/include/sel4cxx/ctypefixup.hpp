/* @TAG(OTHER_GPL) */
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* These definitions taken from the GNU C libraries ctype.h file */

#ifndef	SEL4_CXX_CTYPE_FIXUP_HPP
#define	SEL4_CXX_CTYPE_FIXUP_HPP

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ISbit
/* These are all the characteristics of characters.
   If there get to be more than 16 distinct characteristics,
   many things must be changed that use `unsigned short int's.

   The characteristics are stored always in network byte order (big
   endian).  We define the bit value interpretations here dependent on the
   machine's byte order.  */

# include <endian.h>
# if __BYTE_ORDER == __BIG_ENDIAN
#  define _ISbit(bit)   (1 << (bit))
# else /* __BYTE_ORDER == __LITTLE_ENDIAN */
#  define _ISbit(bit)   ((bit) < 8 ? ((1 << (bit)) << 8) : ((1 << (bit)) >> 8))
# endif

enum
{
  _ISupper = _ISbit (0),    /* UPPERCASE.  */
  _ISlower = _ISbit (1),    /* lowercase.  */
  _ISalpha = _ISbit (2),    /* Alphabetic.  */
  _ISdigit = _ISbit (3),    /* Numeric.  */
  _ISxdigit = _ISbit (4),   /* Hexadecimal numeric.  */
  _ISspace = _ISbit (5),    /* Whitespace.  */
  _ISprint = _ISbit (6),    /* Printing.  */
  _ISgraph = _ISbit (7),    /* Graphical.  */
  _ISblank = _ISbit (8),    /* Blank (usually SPC and TAB).  */
  _IScntrl = _ISbit (9),    /* Control character.  */
  _ISpunct = _ISbit (10),   /* Punctuation.  */
  _ISalnum = _ISbit (11)    /* Alphanumeric.  */
};
#endif /* ! _ISbit  */

#ifdef __cplusplus
}
#endif

#endif
