/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* acserver2.c - generic server dispatch */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acserver2.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acserver2.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acserver2.c,v
 * Revision 9.0  1992/06/16  12:05:59  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include <signal.h>
#include "psap.h"
#include "tsap.h"
#include "sys.file.h"
#include "tailor.h"

/*  */

static int is_nfds;
static fd_set is_mask;
static char is_single;

/*  */

iserver_init(argc, argv, aei, initfnx, td)
	int argc;
	char **argv;
	IFP initfnx;
	AEI aei;
	struct TSAPdisconnect *td;
{
	struct PSAPaddr *pa = NULLPA;

	if (argc <= 1 && (pa = aei2addr(aei)) == NULLPA)
		return tsaplose(td, DR_ADDRESS, NULLCP, "address translation failed");
	return iserver_initAux(argc, argv, pa, initfnx, NULLIFP, 0, td);
}

int
iserver_init_aux(argc, argv, aei, initfnx, magicfnx, flag, td)
	int argc;
	char **argv;
	AEI aei;
	IFP initfnx, magicfnx;
	struct TSAPdisconnect *td;
{
	struct PSAPaddr *pa = NULLPA;

	if (argc <= 1 && (pa = aei2addr(aei)) == NULLPA)
		return tsaplose(td, DR_ADDRESS, NULLCP, "address translation failed");

	return iserver_initAux(argc, argv, pa, initfnx, magicfnx, flag, td);
}

int
iserver_initAux(argc, argv, pa, initfnx, magicfnx, flag, td)
	int argc;
	char **argv;
	struct PSAPaddr *pa;
	IFP initfnx, magicfnx;
	struct TSAPdisconnect *td;
{
	int fd;

	isodetailor(NULLCP, 0);

	is_nfds = 0;
	FD_ZERO(&is_mask);

	if (argc > 1) {
		is_single = 1;
		if ((fd = (*initfnx) (argc, argv)) == NOTOK)
			return tsaplose(td, DR_NETWORK, NULLCP, "initialization failed");

		is_nfds = fd + 1;
		FD_SET(fd, &is_mask);
	} else {
		is_single = 0;

		if (pa == NULLPA)
			return tsaplose(td, DR_ADDRESS, NULLCP, "No presentation address given");

		if (TNetListenAux(&pa->pa_addr.sa_addr, magicfnx, td) == NOTOK)
			return NOTOK;

		if (flag == 0 && !isatty(2)) {
			int i;

			for (i = 0; i < 5; i++) {
				switch (fork()) {
				case NOTOK:
					sleep(5);
					continue;

				case OK:
					break;

				default:
					_exit(0);
				}
				break;
			}

			(void) chdir("/");

			if ((fd = open("/dev/null", O_RDWR)) != NOTOK) {
				if (fd != 0)
					(void) dup2(fd, 0), (void) close(fd);
				(void) dup2(0, 1);
				(void) dup2(0, 2);
			}
#ifdef	SETSID
			(void) setsid();
#endif
#ifdef	TIOCNOTTY
			if ((fd = open("/dev/tty", O_RDWR)) != NOTOK) {
				(void) ioctl(fd, TIOCNOTTY, NULLCP);
				(void) close(fd);
			}
#else
#ifdef	SYS5
			(void) setpgrp();
			(void) signal(SIGINT, SIG_IGN);
			(void) signal(SIGQUIT, SIG_IGN);
#endif
#endif
			isodexport(NULLCP);	/* re-initialize logfiles */
		}
	}

	return OK;
}

/*  */

int
iserver_wait(initfnx, workfnx, losefnx, nfds, rfds, wfds, efds, secs, td)
	IFP initfnx, workfnx, losefnx;
	int nfds;
	fd_set *rfds, *wfds, *efds;
	int secs;
	struct TSAPdisconnect *td;
{
	int fd, vecp;
	fd_set ifds, ofds, xfds;
	char *vec[4];

	ifds = is_mask;		/* struct copy */
	FD_ZERO(&ofds);
	FD_ZERO(&xfds);
	if (is_nfds > nfds)
		nfds = is_nfds + 1;

	if (rfds)
		for (fd = 0; fd < nfds; fd++)
			if (FD_ISSET(fd, rfds))
				FD_SET(fd, &ifds);
	if (wfds)
		ofds = *wfds;
	if (efds)
		xfds = *efds;

	if (TNetAccept(&vecp, vec, nfds, &ifds, &ofds, &xfds, secs, td)
	    == NOTOK) {
		(void) (*losefnx) (td);

		return OK;
	}
	if (wfds)
		*wfds = ofds;
	if (efds)
		*efds = xfds;

	if (vecp > 0 && (fd = (*initfnx) (vecp, vec)) != NOTOK) {
		if (fd >= is_nfds)
			is_nfds = fd + 1;
		FD_SET(fd, &is_mask);
	}

	for (fd = 0; fd < nfds; fd++)
		if (FD_ISSET(fd, &is_mask) && FD_ISSET(fd, &ifds)) {
			if (workfnx == NULLIFP) {
				(void) TNetClose(NULLTA, td);
				return tsaplose(td, DR_OPERATION, NULLCP,
						"no worker routine for connected fd");
			}
			FD_CLR(fd, &ifds);
			if ((*workfnx) (fd) == NOTOK) {
				FD_CLR(fd, &is_mask);
				if (is_nfds == fd + 1)
					is_nfds--;

				if (is_single) {
					int xd;

					for (xd = 0; xd < nfds; xd++)
						if (FD_ISSET(xd, &is_mask))
							break;
					if (rfds)
						FD_ZERO(rfds);
					if (xd >= is_nfds)
						return DONE;
				}
			}
		}
	if (rfds)
		*rfds = ifds;
	return OK;
}

/*  */

fd_set
iserver_mask()
{
	return is_mask;
}