#ifndef	_LOGPAGE_H
#define	_LOGPAGE_H
/*
 *	Definitions for SCSI log sense page parsing and printing.
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


struct logPageHeader
{
    uint8_t			page_code;
    uint8_t			rsvd0;
    uint16_t			length;
} __attribute__ ((packed));

struct logParameterHeader
{
    uint16_t			parameter_code;
#if	defined(_BIT_FIELDS_LTOH)	/* Intel byteorder (thanks, Schily) */
    uint8_t			lp:1;
    uint8_t			lbin:1;
    uint8_t			tmc:2;
    uint8_t			etc:1;
    uint8_t			tsd:1;
    uint8_t			ds:1;
    uint8_t			du:1;
#else	/* Motorola byteorder */
    uint8_t			du:1;
    uint8_t			ds:1;
    uint8_t			tsd:1;
    uint8_t			etc:1;
    uint8_t			tmc:2;
    uint8_t			lbin:1;
    uint8_t			lp:1;
#endif
    uint8_t			length;
};

#define	LOG_PAGE_MAX	0x40

struct supportedLogsPage
{
    uint8_t			page[LOG_PAGE_MAX];
};

struct errorLogPage
{
    uint64_t			corrected;
    uint64_t			delayed;
    uint64_t			reread;
    uint64_t			total_corrected;
    uint64_t			total_algorithm;
    uint64_t			total_bytes;
    uint64_t			total_uncorrected;
};

struct selfTestLogParameter
{
    struct logParameterHeader	h;
#if	defined(_BIT_FIELDS_LTOH)	/* Intel byteorder (thanks, Schily) */
    uint8_t			self_test_results:4;
    uint8_t			rsvd0:1;
    uint8_t			self_test_code:3;
#else	/* Motorola byteorder */
    uint8_t			self_test_code:3;
    uint8_t			rsvd0:1;
    uint8_t			self_test_results:4;
#endif	
    uint8_t			number;
    uint16_t			timestamp;
    uint64_t			lba;
#if	defined(_BIT_FIELDS_LTOH)	/* Intel byteorder (thanks, Schily) */
    uint8_t			sense_key:4;
    uint8_t			rsvd1:4;
#else	/* Motorola byteorder */
    uint8_t			rsvd1:4;
    uint8_t			sense_key:4;
#endif	
    uint8_t			additional_sense_code;
    uint8_t			additional_sense_code_qualifier;
    uint8_t			vendor_specific;
};

struct selfTestLogPage
{
    struct selfTestLogParameter		entry[20];
};

struct startStopCycleCounterLogPage
{
    char			manufacture_year[5];
    char			manufacture_week[3];
    char			accounting_year[5];
    char			accounting_week[3];
    uint32_t			recommended_starts;
    uint32_t			accumulated_starts;
};

struct temperatureLogPage
{
    uint8_t			current;
    uint8_t			reference;
};

struct logData
{
    struct logPageHeader	h;
    uint8_t			problem;
    uint8_t			pad[3];
    union
    {
	struct supportedLogsPage		supported;
	struct errorLogPage			error;
	struct selfTestLogPage			selftest;
	struct startStopCycleCounterLogPage	startstop;
	struct temperatureLogPage		temperature;
    }				u;
} __attribute__ ((packed));


extern int parseLogPage (void *log, size_t len, struct logData *x);
extern void dumpLogPage (FILE *f, struct logData *x, void *log, size_t len, int verbosity);


#endif
