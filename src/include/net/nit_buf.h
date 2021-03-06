/*****************************************************************************

 @(#) src/include/net/nit_buf.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

#ifndef _NET_NIT_BUF_H
#define _NET_NIT_BUF_H

/* This file can be processed with doxygen(1). */

/** @addtogroups nit
  * @{ */

/** @file
  * Network Interface Tap buffer (nit_buf) header file.  */

/*
 * STREAMS Network Interface Tab buffer "nit_buf" header file.
 */

#ifndef NIOC
#define NIOC ('p' << 8)
#endif

#ifndef HAVE_STRUCT_TIMEVAL32
#if defined(_LP64) || defined (_IL32LPx)
typedef int __time32_t;
typedef int __suseconds32_t;
struct timeval32 {
    __time32_t tv_sec;
    __suseconds32_t tv_usec;
};
#define HAVE_STRUCT_TIMEVAL32
#endif
#endif

/** @name Old SNIT buffer input-output controls.
  * @{ */
#define NIOCSTIME	_IOW('p', 6, struct timeval)	/**< Set timeout value.  */
#define NIOCGTIME	_IOWR('p', 7, struct timeval)	/**< Get timeout value.  */
#define NIOCCTIME	_IO('p', 8)			/**< Clear timeout value. */
#define NIOCSCHUNK	_IOW('p', 9, u_int)		/**< Set chunk size. */
#define NIOCGCHUNK	_IOWR('p', 10, u_int)		/**< Get chunk size. */
/** @} */

/** Default chunk size.
  *
  * The selection of value is completely arbitrary.
  */
#define NB_DFLT_CHUNK		8192

/** Header structure.
  *
  * When adding a given message to an accumulating chunk, the module first
  * converts all leading M_PROTO data blocks to M_DATA data blocks.  It then
  * constructs a nit_bufhdr (defined below), prepends it to the message, and
  * pads the result out to force its length to at least sizeof(u_long).  It then
  * adds the padded message to the chunk.
  *
  * The first field of the header is the length of the message after the M_PROTO
  * => M_DATA conversion, but before adding the header.
  *
  * The second field of the header is the total length of the message, including
  * both the header itself and the trailing padding bytes.
  */
struct nit_bufhdr {
	u_int nhb_msglen;
	u_int nhb_totlen;
};

/** @} */

#endif				/* _NET_NIT_BUF_H */
