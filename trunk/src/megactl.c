/*
 *	Main program.
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

/********************************************************************

megactl/megasasctl

Program to do the things you wish dellmgr or MegaCli could do, e.g.
report device log pages, run device self tests, report disk error
counts without having to pop in and out of countless dellmgr menus,
and actually document the adapter configuration concisely (dellmgr
gives you no way to do this).

Author: Jefferson Ogata (JO317) <ogata@antibozo.net>
Date: 2006/01/23

Version 0.4.0 major changes, including SAS support: 2007/08/20

TODO:

Other log page parsers.

Cleaner log page output.

Fixes for 64-bit systems. Currently builds only with -m32.

********************************************************************/

#include	"mega.h"
#include	"megaioctl.h"
#include	"adapter.h"

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<unistd.h>
#include	<errno.h>
#include	<string.h>
#include	<memory.h>
#include	<malloc.h>
#include	<fcntl.h>
#include	<signal.h>
#include	<ctype.h>
#include	<sys/types.h>

#include	<scg/scsireg.h>

#include	"dumpbytes.h"

#include	"logpage.h"


#ifdef	MEGA_SAS_CTL

#define		MEGA_DEVICE	"/dev/megaraid_sas_ioctl_node"

#else

#define		MEGA_DEVICE	"/dev/megadev0"

#define		MEGA_MIN_VERSION	0x118c

#endif	/* defined(MEGA_SAS_CTL) */


static char		*version = "0.4.1";


static int		verbosity = 0;


static char		*me;
static char		*usages[] = {
"usage: %p [-vest] [-H] [-l log-page-nr] [-T long|short] [target ...]",
"",
"Reports diagnostics on megaraid adapters and attached disks. Permits",
"dumping of controller log pages for inspection of error, temperature,",
"and self-test conditions, initiates self-test diagnostics, and documents",
"adapter and logical drive configuration. Target devices may be adapters,",
#ifdef	MEGA_SAS_CTL
"(e.g. a0), enclosures (e.g. a0e0), or individual disks (e.g. a0e0s0). If",
#else
"(e.g. a0), channels (e.g. a0c0), or individual disks (e.g. a0c0t0). If",
#endif
"no target is specified, reports configuration and drive state on all",
"adapters. If a target matches a collection of disks, operations are",
"applied to all matching devices. Options are:",
"-v       Increase program verbosity.",
"-e       Dump read (0x03), write (0x02), and verify (0x05) error log",
"         pages.",
"-s       Dump self-test (0x10) log page.",
"-t       Dump temperature (0x0d) log page.",
"-l page  Dump the specified log page. Log page 0 documents the log pages",
"         the device supports.",
"-p       Do not report physical disks. Reports only adapters and logical",
"         drives. Useful for concisely documenting adapter configuration.",
"-T test  Initiate the background short or long self-test procedure. The",
"         test may take up to an hour to complete, but does not inhibit",
"         access to the device. The test may be monitored using the -s",
"         option.",
"-H       Perform an adapter health check. Inspects state of all logical",
"         and physical drives and battery backup unit and reports problem",
"         conditions. If all is well, generates no output. Useful in a",
"         cron job.",
"-B       When performing health check, do not treat battery problems as",
"         failures.",
"-V       Show version.",
"",
"N.B. The background long self test is a useful tool for diagnosing",
"problems with individual disks. But be cautious with program usage.",
"\"%p -T long\" with no targets will initiate a background long self",
"test on every drive on every adapter. This may not be what you want.",
"",
"By default, the health check option inspects log pages 0x02, 0x03, and",
"0x05 for uncorrected read, write, and verify errors, 0x0d for excess",
"temperature conditions, and 0x10 for failed self tests. If, however, any",
"of the log page options is specified, only the designated log pages are",
"inspected.",
"",
#ifdef	MEGA_SAS_CTL
"This program requires the device file " MEGA_DEVICE " to be",
"present on the system. If your system does not have this device file,",
"you may create it either by executing LSI\'s \"MegaCli\" program once,",
"or by locating the megadev_sas_ioctl entry in /proc/devices and creating",
MEGA_DEVICE " as a character device with suitable",
"permissions with a matching major device number and a minor number of 0.",
#else
"This program requires the device file " MEGA_DEVICE " to be present on",
"the system. If your system does not have this device file, you may",
"create it either by executing Dell\'s \"dellmgr\" program once, or by",
"locating the megadev entry in /proc/devices and creating " MEGA_DEVICE,
"as a character device with suitable permissions with a matching major",
"device number and a minor number of 0.",
#endif
    0,
};


void usage (const int ec, const char *format, ...)
{
    char		**u;
    va_list		ap;

    va_start (ap, format);
    if (format)
    {
	fprintf (stderr, "%s: ", me);
	vfprintf (stderr, format, ap);
	fprintf (stderr, "\n\n");
    }
    for (u = usages; *u; ++u)
    {   
	char            *s;
	int             esc;
	for (s = *u, esc = 0; *s; ++s)
	{   
	    if (esc)
	    {   
		switch (*s)
		{
		 case 'p':      fputs (me, stderr); break;
		 case '%':      fputc ('%', stderr); break;
		 default:       fputc ('%', stderr); fputc (*s, stderr); break;
		}
		esc = 0;
	    }
	    else
	    {   
		switch (*s)
		{
		 case '%':      esc = 1; break;
		 default:       fputc (*s, stderr); break;
		}
	    }
	}
	fputc ('\n', stderr);
    }

    exit (ec);
}


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


void describePhysicalDrive (FILE *f, struct physical_drive_info *d, int verbosity)
{
    char			*state;

    if (d->present)
	switch (d->state)
	{
	 case PdStateUnconfiguredGood:	state = "ready"; break;
	 case PdStateUnconfiguredBad:	state = "BAD"; break;
	 case PdStateOnline:		state = "online"; break;
	 case PdStateFailed:		state = d->span ? "FAILED" : "rdy/fail"; break;
	 case PdStateRebuild:		state = "rebuild"; break;
	 case PdStateHotspare:		state = "hotspare"; break;
	 default:			state = "???"; break;
	}
    else
	state = "absent";

    fprintf (f, "%-8s", d->name);
    if (verbosity > 0)
	fprintf (f, " %8s %-16s", d->vendor, d->model);
    if (verbosity > 1)
	fprintf (f, " rev:%-4s s/n:%-20s", d->revision, d->serial);
    fprintf (f, "  %7s", friendlySize (d->blocks << 9, "B"));
    fprintf (f, " %5s%c", d->span && d->span->num_logical_drives ? d->span->logical_drive[0]->name : "", d->span && (d->span->num_logical_drives > 1) ? '+' : ' ');
    fprintf (f, " %-8s", state);
    if (d->media_errors || d->other_errors)
	fprintf (f, " errs: media:%-2u other:%u", d->media_errors, d->other_errors);
    if (d->predictive_failures)
	fprintf (f, " predictive-failure");
    fprintf (f, "\n");
    if (d->present && d->error_string)
	fprintf (f, "\t%s\n", d->error_string);
}


void describeLogicalDrive (FILE *f, struct logical_drive_info *l, int verbosity)
{
    char			*state;
    uint64_t			blocks;
    int				k;
    struct span_reference	*r;

    switch (l->state)
    {
     case LdStateOffline:		state = "OFFLINE"; break;
     case LdStatePartiallyDegraded:
     case LdStateDegraded:		state = "DEGRADED"; break;
     case LdStateOptimal:		state = "optimal"; break;
     case LdStateDeleted:		state = "deleted"; break;
     default:				state = "???"; break;
    }

    for (k = 0, blocks = 0; k < l->num_spans; ++k)
    {
	r = &l->span[k];
	switch (l->raid_level)
	{
	 case 0:	blocks += r->blocks_per_disk * r->span->num_disks; break;
	 case 1:	blocks += r->blocks_per_disk * r->span->num_disks / 2; break;
	 case 5:	blocks += r->blocks_per_disk * (r->span->num_disks - 1); break;
	}
    }

    fprintf (f, "%-8s", l->name);
    fprintf (f, "  %s", friendlySize (blocks << 9, "B"));
    fprintf (f, " RAID %u%s", l->raid_level, l->num_spans > 1 ? "0" : " ");
    fprintf (f, " %2ux%-2u", l->num_spans, l->span_size);
    fprintf (f, " %s", state);
    fprintf (f, "\n");
    if (verbosity > 0)
    {
	for (k = 0; k < l->num_spans; ++k)
	{
	    struct physical_drive_info	**p;
	    int				j;

	    r = &l->span[k];
	    fprintf (f, "       row %2d:", k);
	    for (j = 0, p = r->span->disk; j < r->span->num_disks; ++j, ++p)
	    {
		char			*flag = (*p)->state != PdStateOnline ? "*" : " ";
		fprintf (f, " %s%-8s", flag, (*p)->name);
	    }
	    fprintf (f, "\n");
	}
    }
}


void describeBattery (FILE *f, struct adapter_config *a, int verbosity)
{
    if (a->battery.healthy)
	fprintf (f, "good");
    else
    {
	fprintf (f, "FAULT");
	if (a->battery.module_missing)
	    fprintf (f, ", module missing");
	if (a->battery.pack_missing)
	    fprintf (f, ", pack missing");
	if (a->battery.low_voltage)
	    fprintf (f, ", low voltage");
	if (a->battery.high_temperature)
	    fprintf (f, ", high temperature");
	if (a->battery.cycles_exceeded)
	    fprintf (f, ", cycles exceeded");
	if (a->battery.over_charged)
	    fprintf (f, ", over charged");
	switch (a->battery.charger_state)
	{
	 case ChargerStateComplete:		break;
	 case ChargerStateFailed:		fprintf (f, ", charge failed"); break;
	 case ChargerStateInProgress:		fprintf (f, ", charging"); break;
	 default:				fprintf (f, ", unknown charge state"); break;
	}
    }
    if (verbosity)
    {
	if (a->battery.voltage >= 0)
	    fprintf (f, "/%dmV", a->battery.voltage);
	if (a->battery.temperature >= 0)
	    fprintf (f, "/%dC", a->battery.temperature);
    }
}


void describeAdapter (FILE *f, struct adapter_config *a, int verbosity)
{
    fprintf (f, "%-8s %-24s", a->name, a->product);
    if (verbosity > 0)
	fprintf (f, " bios:%s fw:%s", a->bios, a->firmware);
    fprintf (f, " %s:%u ldrv:%-2u", a->is_sas ? "encl" : "chan", a->num_channels, a->num_logicals);
    if (verbosity > 0)
	fprintf (f, " rbld:%u%%", a->rebuild_rate);
    if (verbosity > 1)
	fprintf (f, " mem:%uMiB", a->dram_size);
    fprintf (f, " batt:");
    describeBattery (f, a, verbosity);
    fprintf (f, "\n");
}


int main (int argc, char **argv)
{
    int				k;
    int				fd;
    uint32_t			numAdapters;
    uint32_t			driverVersion;
    int				startSelfTest = -1;
    int				healthCheck = 0;
    int				checkBattery = 1;
    char			*device = MEGA_DEVICE;
    struct query_object {
	int			    adapter;
	int			    channel;
	int			    id;
    }				*object = NULL;
    int				numObjects = 0;
    uint8_t			readLog[LOG_PAGE_MAX] = { 0, };
    int				reportPhysical = 1;
    int				showVersion = 0;
#ifdef	MEGA_SAS_CTL
    int				sas = 1;
#else
    int				sas = 0;
#endif

    if ((me = strrchr (argv[0], '/')))
	++me;
    else
	me = argv[0];

    if (argc > 1)
    {
	if ((object = (struct query_object *) malloc ((argc - 1) * sizeof (*object))) == NULL)
	{
	    perror ("malloc");
	    return 1;
	}
    }

    for (k = 1; k < argc; ++k)
    {
	if (argv[k][0] == '-')
	{
	    char		*s;

	    for (s = argv[k] + 1; *s; ++s)
	    {
		if (*s == 'v')
		{
		    ++verbosity;
		    continue;
		}
		if (*s == 'e')
		{
		    /* read error log pages */
		    readLog[0x02] = 1;	/* write errors */
		    readLog[0x03] = 1;	/* read errors */
		    readLog[0x05] = 1;	/* read errors */
		    continue;
		}
		if (*s == 's')
		{
		    /* read self test log page */
		    readLog[0x10] = 1;
		    continue;
		}
		if (*s == 't')
		{
		    /* read temperature log page */
		    readLog[0x0d] = 1;
		    continue;
		}
		if (*s == 'l')
		{
		    /* read specific log page */
		    char		*t;
		    unsigned long	u;

		    if ((++k) >= argc)
			usage (2, "no log page specified");
		    u = strtoul (argv[k], &t, 0);
		    if (*t)
			usage (2, "invalid log page \"%s\"", argv[k]);
		    if (u >= sizeof (readLog) / sizeof (readLog[0]))
			usage (2, "log page out of range: \"%s\"", argv[k]);
		    readLog[u] = 1;
		    continue;
		}
		if (*s == 'D')
		{
		    /* specify device file */
		    if ((++k) >= argc)
			usage (2, "no device specified");
		    device = argv[k];
		    continue;
		}
		if (*s == 'p')
		{
		    reportPhysical = 0;;
		    continue;
		}
		if (*s == 'B')
		{
		    /* skip battery check */
		    checkBattery = 0;
		    continue;
		}
		if (*s == 'H')
		{
		    /* perform adapter health check */
		    ++healthCheck;
		    continue;
		}
		if (*s == 'T')
		{
		    /* start self test */
		    if ((++k) >= argc)
			usage (2, "must specify short or long self-test");
		    if (!strcmp (argv[k], "short"))
			startSelfTest = SCSI_SELFTEST_BACKGROUND_SHORT;
		    else if (!strcmp (argv[k], "long"))
			startSelfTest = SCSI_SELFTEST_BACKGROUND_LONG;
		    else
			usage (2, "invalid self test: \"%s\"; must specify short or long", argv[k]);
		    continue;
		}
		else if ((*s == '?') || (*s == 'h'))
		    usage (0, NULL);
		else if (*s == 'V')
		{
		    ++showVersion;
		    continue;
		}
		usage (2, "invalid flag \"%s\"", s);
	    }
	}
	else
	{
	    char		*s;
	    char		*t;
	    unsigned long	l;

	    s = argv[k];
	    object[numObjects].adapter = -1;
	    object[numObjects].channel = -1;
	    object[numObjects].id = -1;

	    if (*s)
	    {
		if (tolower (*s) != 'a')
		    usage (2, "invalid specifier \"%s\"", argv[k]);
		++s;
		l = strtoul (s, &t, 10);
		if (s == t)
		    usage (2, "invalid specifier \"%s\"", argv[k]);
#ifndef	MEGA_SAS_CTL
		if (l >= MAX_CONTROLLERS)
		    usage (2, "adapter out of range: \"%s\"", argv[k]);
#endif
		object[numObjects].adapter = l;
		s = t;
	    }

	    if (*s)
	    {
		if (tolower (*s) != (sas ? 'e' : 'c'))
		    usage (2, "invalid specifier \"%s\"", argv[k]);
		++s;
		l = strtoul (s, &t, 10);
		if (s == t)
		    usage (2, "invalid specifier \"%s\"", argv[k]);
#ifndef	MEGA_SAS_CTL
		if (l >= MAX_MBOX_CHANNELS)
		    usage (2, "channel out of range: \"%s\"", argv[k]);
#endif
		object[numObjects].channel = l;
		s = t;
	    }

	    if (*s)
	    {
		if (tolower (*s) != (sas ? 's' : 't'))
		    usage (2, "invalid specifier \"%s\"", argv[k]);
		++s;
		l = strtoul (s, &t, 10);
		if (s == t)
		    usage (2, "invalid specifier \"%s\"", argv[k]);
#ifndef	MEGA_SAS_CTL
		if (l > MAX_MBOX_TARGET)
		    usage (2, "target out of range: \"%s\"", argv[k]);
#endif
		object[numObjects].id = l;
		s = t;
	    }

	    ++numObjects;
	}
    }

    if (showVersion)
    {
	if (verbosity)
	    fprintf (stdout, "%s: version %s by Jefferson Ogata\n", me, version);
	else
	    fprintf (stdout, "%s\n", version);
	return 0;
    }

    if (healthCheck)
    {
	int			set = 0;

	for (k = 0; k < numObjects; ++k)
	    if ((object[k].channel >= 0) || (object[k].id >= 0))
		usage (2, "for health check, must specify adapter only");
	for (k = 0; k < sizeof readLog / sizeof (readLog[0]); ++k)
	    if (readLog[k])
	    {
		set = 1;
		break;
	    }
	if (set == 0)
	{
	    /* No specific log pages requested; check read/write/verify errors and temperature. */
	    readLog[0x02] = 1;
	    readLog[0x03] = 1;
	    readLog[0x05] = 1;
	    readLog[0x0d] = 1;
	}
    }

    if ((fd = open (device, O_RDONLY)) < 0)
    {
	fprintf (stderr, "unable to open device %s: %s\n", device, strerror (errno));
	return 1;
    }

#ifndef	MEGA_SAS_CTL
    if (megaGetDriverVersion (fd, &driverVersion) < 0)
    {
	fprintf (stderr, "unable to determine megaraid driver version: %s\n", megaErrorString ());
	return 1;
    }

    if (driverVersion < MEGA_MIN_VERSION)
    {
	fprintf (stderr, "megaraid driver version %x too old.\n", driverVersion);
	return 1;
    }
#endif

    if (megaGetNumAdapters (fd, &numAdapters, sas) < 0)
    {
	fprintf (stderr, "unable to determine number of adapters: %s\n", megaErrorString ());
	return 1;
    }

    if (verbosity > 2)
	fprintf (stderr, "%u adapters, driver version %08x\n\n", numAdapters, driverVersion);

    /* Default to enumerating all adapters. */
    if (numObjects == 0)
    {
	if (object)
	    free (object);
	if ((object = (struct query_object *) malloc (numAdapters * sizeof (*object))) == NULL)
	{
	    perror ("malloc");
	    return 1;
	}
	for (k = 0; k < numAdapters; ++k)
	{
	    object[k].adapter = k;
	    object[k].channel = -1;
	    object[k].id = -1;
	}
	numObjects = k;
    }

    for (k = 0; k < numObjects; ++k)
    {
	int				adapter = object[k].adapter;
	int				channel = object[k].channel;
	int				id = object[k].id;
	char				name[32];
	struct adapter_config		*a;
	uint32_t			c;
	uint32_t			i;
	int				j;

	if (id >= 0)
	    snprintf (name, sizeof name, "a%u%c%u%c%u", adapter, sas ? 'e' : 'c', channel, sas ? 's' : 't', id);
	else if (channel >= 0)
	    snprintf (name, sizeof name, "a%u%c%u", adapter, sas ? 'e' : 'c', channel);
	else
	    snprintf (name, sizeof name, "a%u", adapter);

	if (adapter >= numAdapters)
	{
	    fprintf (stderr, "%s: no such adapter\n", name);
	    continue;
	}

	if ((a = getAdapterConfig (fd, adapter, sas)) == NULL)
	{
	    fprintf (stderr, "%s: cannot read adapter configuration: %s\n", name, megaErrorString ());
	    break;
	}

	if (healthCheck)
	{
	    int				adapterReported = 0;
	    struct logical_drive_info	*l;

	    if (checkBattery && (!a->battery.healthy))
	    {
		if (!(adapterReported++))
		    describeAdapter (stdout, a, verbosity);
	    }

#ifndef	MEGA_SAS_CTL
	    /* Scan all physical devices. */
	    for (c = 0; c < a->num_channels; ++c)
	    {
		for (i = 0; i <= MAX_MBOX_TARGET; ++i)
		{
		    uint8_t			target = (a->channel[c] << 4) | i;

		    (void) getPhysicalDriveInfo (a, target, 1);
		}
	    }
#endif

	    for (i = 0, l = a->logical; i < a->num_logicals; ++i, ++l)
	    {
		int				reportDrive = 0;

		if ((l->state != LdStateOptimal) && (l->state != LdStateDeleted))
		    ++reportDrive;

		if (reportDrive)
		{
		    if (!(adapterReported++))
			describeAdapter (stdout, a, verbosity);
		    describeLogicalDrive (stdout, l, verbosity);
		}
	    }

	    for (i = 0; i < a->num_physicals; ++i)
	    {
		struct physical_drive_info	*d = a->physical_list[i];
		int				reportDrive = 0;
		struct log_page_list		*log;

		if (d == NULL)
		    break;
		if (!(d->present))
		    continue;

//describePhysicalDrive (stdout, d, verbosity);

		/* check for drive problems */
		if ((d->state == PdStateRebuild) || (d->span && (d->state == PdStateFailed)))
		    ++reportDrive;
		if (d->media_errors)
		    ++reportDrive;
		if (d->predictive_failures)
		    ++reportDrive;

		/* check interesting log pages */
		for (j = 0; j < sizeof (readLog) / sizeof (readLog[0]); ++j)
		{
		    if (readLog[j] == 0)
			continue;

		    if ((log = getDriveLogPage (d, j)) == NULL)
			continue;

		    if (log->log.problem)
			++reportDrive;
		}

		if (reportDrive)
		{
		    if (!(adapterReported++))
			describeAdapter (stdout, a, verbosity);
		    describePhysicalDrive (stdout, d, verbosity);
		    for (j = 0; j < sizeof (readLog) / sizeof (readLog[0]); ++j)
		    {
			if (readLog[j] == 0)
			    continue;

			if ((log = getDriveLogPage (d, j)) == NULL)
			    continue;

			dumpLogPage (stdout, &log->log, NULL, 0, verbosity);
		    }
		}
	    }
	    continue;
	}

	if (channel >= 0)
	{
	    for (c = 0; c < a->num_channels; ++c)
		if (channel == a->channel[c])
		    break;
	    if (c >= a->num_channels)
	    {
		fprintf (stderr, "%s: no such channel\n", name);
		continue;
	    }
	}

	if ((channel < 0) && (id < 0))
	{
	    struct logical_drive_info	*l;
	    int				x;

	    describeAdapter (stdout, a, verbosity);

#ifndef	MEGA_SAS_CTL
	    /* Scan all physical devices. */
	    for (c = 0; c < a->num_channels; ++c)
	    {
		for (i = 0; i <= MAX_MBOX_TARGET; ++i)
		{
		    uint8_t			target = (a->channel[c] << 4) | i;

		    (void) getPhysicalDriveInfo (a, target, 1);
		}
	    }
#endif

	    for (i = 0, l = a->logical; i < a->num_logicals; ++i, ++l)
		describeLogicalDrive (stdout, l, verbosity);

	    x = 0;

	    for (i = 0; i < a->num_physicals; ++i)
	    {
		struct physical_drive_info	*d = a->physical_list[i];

		if (d == NULL)
		    break;
		if (!(d->present))
		    continue;

		if (d->state == PdStateHotspare)
		{
		    if (x == 0)
			fprintf (stdout, "hot spares   :");
		    else if ((x % 8) == 0)
			fprintf (stdout, "             :");
		    fprintf (stdout, "  %-8s", d->name);
		    if (((++x) % 8) == 0)
			fprintf (stdout, "\n");
		}
	    }
	    if (x % 8)
		fprintf (stdout, "\n");

	    x = 0;
	    for (i = 0; i < a->num_physicals; ++i)
	    {
		struct physical_drive_info	*d = a->physical_list[i];

		if (d == NULL)
		    break;
		if (!(d->present))
		    continue;

		if ((!(d->span)) && (d->state != PdStateHotspare))
		{
		    if (x == 0)
			fprintf (stdout, "unconfigured :");
		    else if ((x % 8) == 0)
			fprintf (stdout, "             :");
		    fprintf (stdout, "  %-8s", d->name);
		    if (((++x) % 8) == 0)
			fprintf (stdout, "\n");
		}
	    }
	    if (x % 8)
		fprintf (stdout, "\n");
	}

	for (c = 0; c < a->num_channels; ++c)
	{
	    if ((channel >= 0) && (channel != a->channel[c]))
		continue;

#ifndef	MEGA_SAS_CTL
	    if (id >= 0)
	    {
		uint8_t			target = (a->channel[c] << 4) | id;
		(void) getPhysicalDriveInfo (a, target, 1);
	    }
	    else
	    {
		/* Scan all devices on this channel. */
		for (i = 0; i <= MAX_MBOX_TARGET; ++i)
		{
		    uint8_t			target = (a->channel[c] << 4) | i;

		    (void) getPhysicalDriveInfo (a, target, 1);
		}
	    }
#endif

	    for (i = 0; i < a->num_physicals; ++i)
	    {
		struct physical_drive_info	*d = a->physical_list[i];

		if (d == NULL)
		    break;

		if (d->channel != a->channel[c])
		    continue;

		if ((id >= 0) && (id != d->id))
		    continue;

		if (startSelfTest >= 0)
		{
		    uint8_t		diag[256];

		    memset (diag, 0, sizeof diag);
		    if (megaScsiSendDiagnostic (&a->target, d->target, diag, sizeof diag, startSelfTest, 0, 0) < 0)
			fprintf (stderr, "self test: %s\n", megaErrorString ());
		}

		if (reportPhysical)
		{
		    describePhysicalDrive (stdout, d, verbosity);
		    for (j = 0; j < sizeof (readLog) / sizeof (readLog[0]); ++j)
		    {
			struct log_page_list	*log;

			if (readLog[j] == 0)
			    continue;

			if ((log = getDriveLogPage (d, j)) == NULL)
			    continue;

			dumpLogPage (stdout, &log->log, &log->buf, sizeof (log->buf), verbosity);
		    }
		}
	    }
	}

	fprintf (stdout, "\n");
    }

    return 0;
}


#if	0
	if (0)
	{
	    uint8_t		modes[1024];

	    memset (modes, 0, sizeof modes);
	    megaScsiModeSense (fd, adapter, target, modes, sizeof modes, 0, 0x3f, 0xff);
	}
#endif


