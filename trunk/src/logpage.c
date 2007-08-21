/*
 *	Parse and print SCSI log sense pages.
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


#include	"megaioctl.h"
#include	"logpage.h"
#include	"ntrim.h"
#include	"dumpbytes.h"

#include	<stdio.h>
#include	<memory.h>
#include	<netinet/in.h>


static char			*logPageType[] = {
    /* 0 */	"supported log pages",
    /* 1 */	"buffer over-run/under-run",
    /* 2 */	"write error counter",
    /* 3 */	"read error counter",
    /* 4 */	"read reverse error counter",
    /* 5 */	"verify error counter",
    /* 6 */	"non-medium error",
    /* 7 */	"last n error events",
    /* 8 */	"format status",
    /* 9 */	NULL,
    /* a */	NULL,
    /* b */	"last n deferred errors os asynchronous events",
    /* c */	"sequential-access device",
    /* d */	"temperature",
    /* e */	"start-stop cycle counter",
    /* f */	"application client",
    /* 10 */	"self-test results",
    /* 11 */	"DTD status",
    /* 12 */	"TapeAlert response",
    /* 13 */	"requested recover",
    /* 14 */	"device statistics",
    /* 15 */	NULL,
    /* 16 */	NULL,
    /* 17 */	"non-volatile cache",
    /* 18 */	"protocol specific port",
    /* 19 */	NULL,
    /* 1a */	NULL,
    /* 1b */	NULL,
    /* 1c */	NULL,
    /* 1d */	NULL,
    /* 1e */	NULL,
    /* 1f */	NULL,
    /* 20 */	NULL,
    /* 21 */	NULL,
    /* 22 */	NULL,
    /* 23 */	NULL,
    /* 24 */	NULL,
    /* 25 */	NULL,
    /* 26 */	NULL,
    /* 27 */	NULL,
    /* 28 */	NULL,
    /* 29 */	NULL,
    /* 2a */	NULL,
    /* 2b */	NULL,
    /* 2c */	NULL,
    /* 2d */	NULL,
    /* 2e */	"TapeAlert",
    /* 2f */	"informational exceptions",
    /* 30 */	"vendor specific",
    /* 31 */	"vendor specific",
    /* 32 */	"vendor specific",
    /* 33 */	"vendor specific",
    /* 34 */	"vendor specific",
    /* 35 */	"vendor specific",
    /* 36 */	"vendor specific",
    /* 37 */	"vendor specific",
    /* 38 */	"vendor specific",
    /* 39 */	"vendor specific",
    /* 3a */	"vendor specific",
    /* 3b */	"vendor specific",
    /* 3c */	"vendor specific",
    /* 3d */	"vendor specific",
    /* 3e */	"vendor specific",
    /* 3f */	NULL,
};


static char *friendlySize (uint64_t b, char *unit)
{
    static char		*suffix[] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi", "Yi", };
    int			k;
    static char		bytes[128];

    for (k = 0; (b >= 1024) && (k < sizeof (suffix) / sizeof (suffix[0]) - 1); ++k, b /= 1024)
	;
    snprintf (bytes, sizeof bytes, "%3llu%s%s", b, suffix[k], unit);
    return bytes;
}


static uint32_t blocksToGB (uint32_t blocks)
{
    return (long) (((uint64_t) blocks) * 512 / 1000000000);
}


static uint32_t blocksToGiB (uint32_t blocks)
{
    return blocks / 2 / 1024 / 1024;
}


static uint64_t extractInt64 (void *u, size_t len)
{
    uint64_t			x;
    uint8_t			*v;

    for (x = 0, v = u; len > 0; --len, ++v)
	x = (x << 8) + *v;
    return x;
}


int parseLogPage (void *log, size_t len, struct logData *x)
{
    struct logPageHeader	*h = log;
    void			*u = log + sizeof (*h);
    struct logParameterHeader	*p;
    size_t			pageLen;

    memset (x, 0, sizeof (*x));

    if (len < sizeof (*h))
	return -1;
    pageLen = ntohs (h->length) + sizeof (*h);
    if (len > pageLen)
	len = pageLen;
    len -= sizeof (*h);

    x->h = *h;
    x->h.length = pageLen;

    if (h->page_code == 0)
    {
	for ( ; len > 0; --len, ++u)
	{
	    uint8_t		code = *((unsigned char *) u);

	    if (code < sizeof (x->u.supported.page) / sizeof (x->u.supported.page[0]))
		x->u.supported.page[code] = 1;
	}
	return 0;
    }

    while (len >= sizeof (*p))
    {
	uint16_t			code;
	uint64_t			e;
	struct selfTestLogParameter	*t;

	p = u;
	if (p->length + sizeof (*p) > len)
	    break;
	len -= sizeof (*p);
	u += sizeof (*p);

	code = ntohs (p->parameter_code);
	switch (h->page_code)
	{
	 case 0x02:
	 case 0x03:
	 case 0x04:
	 case 0x05:
	    e = extractInt64 (u, p->length);
	    switch (code)
	    {
	     case 0x0000:	x->u.error.corrected = e; break;
	     case 0x0001:	x->u.error.delayed = e; break;
	     case 0x0002:	x->u.error.reread = e; break;
	     case 0x0003:	x->u.error.total_corrected = e; break;
	     case 0x0004:	x->u.error.total_algorithm = e; break;
	     case 0x0005:	x->u.error.total_bytes = e; break;
	     case 0x0006:	x->u.error.total_uncorrected = e; if (e) x->problem = 1; break;
	     default:		break;
	    }
	    break;
	 case 0x0d:
	    switch (code)
	    {
	     case 0x0000:	x->u.temperature.current = ((uint8_t *) u)[1]; break;
	     case 0x0001:	x->u.temperature.reference = ((uint8_t *) u)[1]; break;
	     default:		break;
	    }
	    break;
	 case 0x0e:
	    switch (code)
	    {
	     case 0x0001:
		strncpy (x->u.startstop.manufacture_year, u, sizeof (x->u.startstop.manufacture_year) - 1);
		x->u.startstop.manufacture_year[sizeof (x->u.startstop.manufacture_year) - 1] = '\0';
		ntrim (x->u.startstop.manufacture_year);
		strncpy (x->u.startstop.manufacture_week, u + 4, sizeof (x->u.startstop.manufacture_week) - 1);
		x->u.startstop.manufacture_week[sizeof (x->u.startstop.manufacture_week) - 1] = '\0';
		ntrim (x->u.startstop.manufacture_week);
		break;
	     case 0x0002:
		strncpy (x->u.startstop.accounting_year, u, sizeof (x->u.startstop.accounting_year) - 1);
		x->u.startstop.accounting_year[sizeof (x->u.startstop.accounting_year) - 1] = '\0';
		ntrim (x->u.startstop.accounting_year);
		strncpy (x->u.startstop.accounting_week, u + 4, sizeof (x->u.startstop.accounting_week) - 1);
		x->u.startstop.accounting_week[sizeof (x->u.startstop.accounting_week) - 1] = '\0';
		ntrim (x->u.startstop.accounting_week);
		break;
	     case 0x0003:
		x->u.startstop.recommended_starts = ntohl (*((uint32_t *) u));
		break;
	     case 0x0004:
		x->u.startstop.accumulated_starts = ntohl (*((uint32_t *) u));
		break;
	     default:
		break;
	    }
	    break;
	 case 0x10:
	    if ((code < 1) || (code > sizeof (x->u.selftest.entry) / sizeof (x->u.selftest.entry[0])))
		break;
	    if (p->length != sizeof (x->u.selftest.entry[0]) - sizeof (*p))
		break;
	    --code;
	    t = &x->u.selftest.entry[code];
	    *t = *((struct selfTestLogParameter *) p);
	    t->h.parameter_code = code;
	    t->timestamp = ntohs (t->timestamp);
	    t->lba = extractInt64 (&t->lba, sizeof (t->lba));
	    if (t->self_test_code || t->self_test_results || t->timestamp || t->number || t->lba)
		if ((t->self_test_results >= 4) && (t->self_test_results <= 7))
		    x->problem = 1;
	    break;
	}

	len -= p->length;
	u += p->length;
    }

    /* flag any other problems */
    switch (h->page_code)
    {
     case 0x0d:
	if (x->u.temperature.reference && (x->u.temperature.reference != 0xff) && (x->u.temperature.current >= x->u.temperature.reference))
	    x->problem = 1;
	break;
    }

    return 0;
}


void dumpLogPage (FILE *f, struct logData *x, void *log, size_t len, int verbosity)
{
    struct logPageHeader	*h = log;
    void			*u = log + sizeof (*h);
    struct logParameterHeader	*p;
    size_t			pageLen;
    int				k;

    switch (x->h.page_code)
    {
     case 0x00:
	fprintf (f, "    %s:", logPageType[h->page_code]);
	if (verbosity > 1)
	    fprintf (f, "\n");
	for (k = 0; k < sizeof (x->u.supported.page) / sizeof (x->u.supported.page[0]); ++k)
	{
	    if (!(x->u.supported.page[k]))
		continue;

	    if (verbosity > 1)
	    {
		char		*name;
		if (logPageType[k])
		    name = logPageType[k];
		else
		    name = "unknown log page";
		fprintf (f, "        %02x %s\n", k, name);
	    }
	    else
		fprintf (f, " %02x", k);
	}
	if (!(verbosity > 1))
	    fprintf (f, "\n");
	break;
     case 0x02:
     case 0x03:
     case 0x04:
     case 0x05:
	switch (x->h.page_code)
	{
	 case 0x02:	fprintf (f, "     write errors:"); break;
	 case 0x03:	fprintf (f, "      read errors:"); break;
	 case 0x04:	fprintf (f, "  read/rev errors:"); break;
	 case 0x05:	fprintf (f, "    verify errors:"); break;
	}
	fprintf (f, " corr:%-6s", friendlySize (x->u.error.corrected, ""));
	fprintf (f, " delay:%-6s", friendlySize (x->u.error.delayed, ""));
	switch (x->h.page_code)
	{
	 case 0x02:	fprintf (f, " rewrit:%-6s", friendlySize (x->u.error.reread, "")); break;
	 case 0x03:	fprintf (f, " reread:%-6s", friendlySize (x->u.error.reread, "")); break;
	 case 0x04:	fprintf (f, " reread:%-6s", friendlySize (x->u.error.reread, "")); break;
	 case 0x05:	fprintf (f, " revrfy:%-6s", friendlySize (x->u.error.reread, "")); break;
	}
	fprintf (f, " tot/corr:%-6s", friendlySize (x->u.error.total_corrected, ""));
	if (verbosity > 1)
	    fprintf (f, " tot/alg:%-6s", friendlySize (x->u.error.total_algorithm, ""));
	if (verbosity > 1)
	    fprintf (f, " tot/bytes:%-6s", friendlySize (x->u.error.total_bytes, "B"));
	fprintf (f, " tot/uncorr:%-6s", friendlySize (x->u.error.total_uncorrected, ""));
	fprintf (f, "\n");
	break;
     case 0x0d:
	fprintf (f, "    temperature: current:%uC threshold:%uC%s\n", x->u.temperature.current, x->u.temperature.reference, x->problem ? " warning:temperature threshold exceeded" : "");
	break;
     case 0x0e:
	fprintf (f, "   ");
	if (strlen (x->u.startstop.manufacture_year) && strlen (x->u.startstop.manufacture_week))
	    fprintf (f, " manufactured:%s/%s", x->u.startstop.manufacture_year, x->u.startstop.manufacture_week);
	if (strlen (x->u.startstop.accounting_year) && strlen (x->u.startstop.accounting_week))
	    fprintf (f, " accounting:%s/%s", x->u.startstop.accounting_year, x->u.startstop.accounting_week);
	fprintf (f, " starts:%d/%d", x->u.startstop.accumulated_starts, x->u.startstop.recommended_starts);
	fprintf (f, "\n");
	break;
     case 0x10:
	for (k = 0; k < sizeof (x->u.selftest.entry) / sizeof (x->u.selftest.entry[0]); ++k)
	{
	    struct selfTestLogParameter		*t = &x->u.selftest.entry[k];

	    if (t->self_test_code || t->self_test_results || t->timestamp || t->number || t->lba)
	    {
		char				*test;
		char				*result;
		int				notice = 0;

		switch (t->self_test_code)
		{
		 case SCSI_SELFTEST_DEFAULT:		test = "default"; break;
		 case SCSI_SELFTEST_BACKGROUND_SHORT:	test = "bg short"; break;
		 case SCSI_SELFTEST_BACKGROUND_LONG:	test = "bg long"; break;
		 case SCSI_SELFTEST_BACKGROUND_ABORT:	test = "bg aborted"; break;
		 case SCSI_SELFTEST_FOREGROUND_SHORT:	test = "fg short"; break;
		 case SCSI_SELFTEST_FOREGROUND_LONG:	test = "fg long"; break;
		 default:				test = "unknown"; break;
		}
		switch (t->self_test_results)
		{
		 case 0x0:				result = "completed without error"; notice = 1; break;
		 case 0x1:				result = "aborted via send diagnostic"; notice = 1; break;
		 case 0x2:				result = "aborted via other method"; notice = 1; break;
		 case 0x3:				result = "unable to complete"; notice = 1; break;
		 case 0x4:				result = "failed in unknown segment"; notice = 0; break;
		 case 0x5:				result = "failed in segment 1"; notice = 0; break;
		 case 0x6:				result = "failed in segment 2"; notice = 0; break;
		 case 0x7:				result = "failed in other segment"; notice = 0; break;
		 case 0xf:				result = "in progress"; notice = 0; break;
		 default:				result = "unknown result"; notice = 0; break;
		}
		if (verbosity > notice)
		    fprintf (f, "    %2d: timestamp %4ud%02uh: %10s %-30s seg:%u lba:%-8lld sk:%u asc:%u ascq:%u vs:%u\n", k, t->timestamp / 24, t->timestamp % 24, test, result, t->number, t->lba, t->sense_key, t->additional_sense_code, t->additional_sense_code_qualifier, t->vendor_specific);
	    }
	}
	break;
     default:
	break;
    }

    if (!(verbosity > 2))
	return;

    if (len < sizeof (*h))
	return;
    pageLen = ntohs (h->length) + sizeof (*h);
    if (len > pageLen)
	len = pageLen;
    len -= sizeof (*h);

    fprintf (f, "    log page %02x, length %u%s\n", h->page_code, ntohs (h->length), len < pageLen - sizeof (*h) ? " warning: truncated" : "");

    while (len >= sizeof (*p))
    {
	uint16_t		code;

	p = u;
	if (p->length + sizeof (*p) > len)
	    break;
	len -= sizeof (*p);
	u += sizeof (*p);

	code = ntohs (p->parameter_code);
	fprintf (f, "    param %04x, du %u, ds %u, tsd %u, etc %u, tmc %u, lbin %u, lp %u, length %u\n", code, p->du, p->ds, p->tsd, p->etc, p->tmc, p->lbin, p->lp, p->length);
	dumpbytes (f, u, p->length, u, "param");

	len -= p->length;
	u += p->length;
    }
}


