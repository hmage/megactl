#ifndef	_MODEPAGE_H
#define	_MODEPAGE_H
/*
 *	Definitions for unused mode page code.
 *
 *	Copyright (c) 2007 by Jefferson Ogata
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include	<stdio.h>
#include	<sys/types.h>

#include	<scg/scsireg.h>


struct modePage0Header
{
#if	defined(_BIT_FIELDS_LTOH)	/* Intel byteorder (thanks, Schily) */
    uint8_t			page_code:6;
    uint8_t			spf:1;
    uint8_t			ps:1;
#else	/* Motorola byteorder */
    uint8_t			ps:1;
    uint8_t			spf:1;
    uint8_t			page_code:6;
#endif
    uint8_t			length;
} __attribute__ ((packed));

struct modeSubPageHeader
{
#if	defined(_BIT_FIELDS_LTOH)	/* Intel byteorder (thanks, Schily) */
    uint8_t			page_code:6;
    uint8_t			spf:1;
    uint8_t			ps:1;
#else	/* Motorola byteorder */
    uint8_t			ps:1;
    uint8_t			spf:1;
    uint8_t			page_code:6;
#endif
    uint8_t			subpage_code;
    uint16_t			length;
} __attribute__ ((packed));

struct modeData
{
    union
    {
	struct modePage0Header			error;
	struct modeSubPageHeader		selftest;
    }				u;
} __attribute__ ((packed));


#if	0
extern int parseLogPage (void *log, size_t len, struct logData *x);
#endif
extern void dumpModePage (FILE *f, struct modeData *x, void *mode, size_t len, int verbosity);

#endif
