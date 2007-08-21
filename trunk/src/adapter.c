/*
 *	High-level interface to adapter information.
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<malloc.h>

#include	<scg/scsireg.h>


static void batteryStatus (struct adapter_config *a, uint8_t status)
{
    a->battery.module_missing = (status & BATTERY_MODULE_MISSING) != 0;
    a->battery.pack_missing = (status & BATTERY_PACK_MISSING) != 0;
    a->battery.low_voltage = (status & BATTERY_LOW_VOLTAGE) != 0;
    a->battery.high_temperature = (status & BATTERY_TEMP_HIGH) != 0;
    a->battery.cycles_exceeded = (status & BATTERY_CYCLES_EXCEEDED) != 0;
    switch (status & BATTERY_CHARGE_MASK)
    {
     case BATTERY_CHARGE_FAIL:		a->battery.charger_state = ChargerStateFailed; break;
     case BATTERY_CHARGE_DONE:		a->battery.charger_state = ChargerStateComplete; break;
     case BATTERY_CHARGE_INPROG:	a->battery.charger_state = ChargerStateInProgress; break;
     default:				a->battery.charger_state = ChargerStateUnknown; break;
    }
    a->battery.voltage = -1;
    a->battery.temperature = -1;
    a->battery.healthy = !(a->battery.module_missing || a->battery.pack_missing || a->battery.low_voltage || a->battery.high_temperature || a->battery.cycles_exceeded || (a->battery.charger_state != ChargerStateComplete));
}

static void batteryStatus5 (struct adapter_config *a)
{
    struct mega_battery_state_sas	*b = &a->q.v5.battery.state;

    a->battery.module_missing = !(a->q.v5.adapinfo.hw_present.bbu);
    a->battery.pack_missing = b->type == MEGA_BATTERY_TYPE_NONE;
    a->battery.low_voltage = b->remaining_capacity_alarm || b->remaining_time_alarm || b->fully_discharged;
    a->battery.high_temperature = b->over_temperature != 0;
    a->battery.over_charged = b->over_charged != 0;
    switch (b->charger_status)
    {
     case 0:				a->battery.charger_state = ChargerStateFailed; break;
     case 1:				a->battery.charger_state = ChargerStateComplete; break;
     case 2:				a->battery.charger_state = ChargerStateInProgress; break;
     default:				a->battery.charger_state = ChargerStateUnknown; break;
    }
    a->battery.voltage = b->voltage;
    a->battery.temperature = b->temperature;
    a->battery.healthy = !(a->battery.module_missing || a->battery.pack_missing || a->battery.low_voltage || a->battery.high_temperature || a->battery.cycles_exceeded || (a->battery.charger_state != ChargerStateComplete) || (!b->health));

}


static struct log_page_list *getPage (struct physical_drive_info *d, uint8_t page)
{
    struct log_page_list	*p;

    if ((p = (struct log_page_list *) malloc (sizeof (*p))) == NULL)
	return NULL;
    memset (p, 0, sizeof (*p));

    if (megaScsiLogSense (&d->adapter->target, d->target, &p->buf, sizeof (p->buf), 1, page, 0) < 0)
    {
	free (p);
	return NULL;
    }

    if (parseLogPage (&p->buf, sizeof (p->buf), &p->log) < 0)
    {
	free (p);
	return NULL;
    }

    return p;
}


struct log_page_list *getDriveLogPage (struct physical_drive_info *d, uint8_t page)
{
    struct supportedLogsPage	*supported = NULL;
    struct log_page_list	*p;

    for (p = d->log; p; p = p->next)
    {
	if (p->log.h.page_code == page)
	    return p;
	if (p->log.h.page_code == 0)
	    supported = &p->log.u.supported;
    }

    if (supported == NULL)
    {
	if ((p = getPage (d, 0)) == NULL)
	    return NULL;
	p->next = d->log;
	d->log = p;
	if (page == 0)
	    return p;

	supported = &p->log.u.supported;
    }

    /* Is the requested page supported? */
    if (supported->page[page] == 0)
	return NULL;

    if ((p = getPage (d, page)) == NULL)
	return NULL;
    p->next = d->log;
    d->log = p;

    return p;
}


int cmpPhysical (const void *a, const void *b)
{
    struct physical_drive_info		*x = *((struct physical_drive_info **) a);
    struct physical_drive_info		*y = *((struct physical_drive_info **) b);

    if (x->adapter->target.adapno != y->adapter->target.adapno)
	return (int) (x->adapter->target.adapno) - (int) (y->adapter->target.adapno);
    if (x->channel != y->channel)
	return (int) (x->channel) - (int) (y->channel);
    if (x->id != y->id)
	return (int) (x->id) - (int) (y->id);
    return 0;
}

struct physical_drive_info *getPhysicalDriveInfo (struct adapter_config *a, uint16_t target, int fetch)
{
    int					k;
    struct physical_drive_info		*d;

    /* Look for it. */
    for (k = 0, d = a->physical; k < a->num_physicals; ++k, ++d)
    {
	if (d->adapter == NULL)
	    break;
	if (d->target == target)
	    return d->present ? d : NULL;
    }

    /* Not there and no place for it. That's just wrong. */
    if (k >= a->num_physicals)
    {
	fprintf (stderr, "me so crazy, me think adapter crazy too. sorry, mister.\n");
	return NULL;
    }

    /* If we don't want to query it, we're done. */
    if (!fetch)
	return NULL;

    d->adapter = a;
    d->target = target;

    if (a->is_sas)
    {
	struct mega_physical_disk_info_sas	*info = &d->q.v5.info;

	if (megaSasGetDiskInfo (&a->target, target, info) < 0)
	{
	    d->error_string = megaErrorString ();
	    d->present = 0;
	    return NULL;
	}

	d->channel = info->enclosure;
	d->id = info->slot;

	snprintf (d->name, sizeof (d->name), "%se%us%u", a->name, d->channel, d->id);

	d->inquiry = info->inquiry.inq;
	strncpy (d->vendor, d->inquiry.vendor_info, sizeof (d->vendor) - 1);
	d->vendor[sizeof (d->vendor) - 1] = '\0';
	ntrim (d->vendor);
	strncpy (d->model, d->inquiry.prod_ident, sizeof (d->model) - 1);
	d->model[sizeof (d->model) - 1] = '\0';
	ntrim (d->model);
	strncpy (d->revision, d->inquiry.prod_revision, sizeof (d->revision) - 1);
	d->revision[sizeof (d->revision) - 1] = '\0';
	ntrim (d->revision);

	if ((d->inquiry.qualifier == INQ_DEV_PRESENT) && (d->inquiry.type == INQ_DASD))
	{
	    d->present = 1;
	}
	else
	{
	    d->present = 0;
	    return NULL;
	}

	strncpy (d->serial, (char *) info->inquiry.buf + sizeof (info->inquiry.inq), sizeof (d->serial));
	d->serial[sizeof (d->serial) - 1] = '\0';
	ntrim (d->serial);

	if (info->configured)
	{
	    if (info->online)
		d->state = PdStateOnline;
	    else if (info->rebuild)
		d->state = PdStateRebuild;
	    else if (info->failure)
		d->state = PdStateFailed;
	    else
		d->state = PdStateUnknown;
	}
	else
	{
	    if (info->hotspare)
		d->state = PdStateHotspare;
	    else if (info->failure)
		d->state = PdStateUnconfiguredBad;
	    else
		d->state = PdStateUnconfiguredGood;
	}
	d->blocks = info->raw_size;
	d->media_errors = info->media_errors;
	d->other_errors = info->other_errors;
	d->predictive_failures = info->predictive_failures;
    }
    else
    {
	int					status;
	struct scsi_inquiry			inq;
	uint8_t					evpd[128];
	struct mega_physical_drive_error_info	errors;

	d->channel = (target >> 4) & 0xf;
	d->id = target & 0xf;

	snprintf (d->name, sizeof (d->name), "%sc%ut%u", a->name, d->channel, d->id);

	if (megaScsiDriveInquiry (&a->target, target, &inq, sizeof (inq), 0, 0) == 0)
	{
	    d->inquiry = inq;
	    strncpy (d->vendor, d->inquiry.vendor_info, sizeof (d->vendor) - 1);
	    d->vendor[sizeof (d->vendor) - 1] = '\0';
	    ntrim (d->vendor);
	    strncpy (d->model, d->inquiry.prod_ident, sizeof (d->model) - 1);
	    d->model[sizeof (d->model) - 1] = '\0';
	    ntrim (d->model);
	    strncpy (d->revision, d->inquiry.prod_revision, sizeof (d->revision) - 1);
	    d->revision[sizeof (d->revision) - 1] = '\0';
	    ntrim (d->revision);

	    if ((d->inquiry.qualifier == INQ_DEV_PRESENT) && (d->inquiry.type == INQ_DASD))
	    {
		d->present = 1;
	    }
	    else
	    {
		d->present = 0;
		return NULL;
	    }
	}
	else
	{
	    d->error_string = megaErrorString ();
	    d->present = 0;
	    return NULL;
	}

	if (megaScsiDriveInquiry (&a->target, target, evpd, sizeof evpd, 0x80, 1) == 0)
	{
	    uint8_t			len = evpd[3];

	    if ((evpd[1] == 0x80) && (len + 4 <= sizeof evpd))
	    {
		if (len > sizeof (d->serial) - 1)
		    len = sizeof (d->serial) - 1;
		strncpy (d->serial, (char *) evpd + 4, len);
		d->serial[len] = '\0';
		ntrim (d->serial);
	    }
	}

	if ((status = megaGetDriveErrorCount (&a->target, target, &errors)) == 0)
	{
	    d->media_errors = errors.media;
	    d->other_errors = errors.other;
	}
	else
	    d->error_string = megaErrorString ();
    }

    /* Add it to the device list and sort it. */
    for (k = 0; k < a->num_physicals; ++k)
	if (a->physical_list[k] == NULL)
	    break;
    if (k >= a->num_physicals)
    {
	fprintf (stderr, "not ok at the ok corral. freak out, mama!\n");
	return NULL;
    }
    a->physical_list[k++] = d;
    qsort (a->physical_list, k, sizeof (*a->physical_list), cmpPhysical);

    return d;
}


/* Adapter handling for PERC2. */
static char *getAdapterConfig2 (struct adapter_config *a)
{
    int					k;
    logdrv_8ld_span8_t			*ml;
    int					spanIndex;
    mraid_adapinfo1_t			*pinfo = &a->q.v2.inquiry.adapter_info;
    mraid_inquiry1_t			*inquiry = &a->q.v2.inquiry;
    disk_array_8ld_span8_t		*config = &a->q.v2.config;

    a->target.type = MEGA_ADAPTER_V2;

    if (megaGetAdapterInquiry (&a->target, inquiry) < 0)
	return "cannot query adapter";
    if (megaGetAdapterConfig8 (&a->target, config) < 0)
	return "cannot read adapter config";
    if (megaGetPredictiveMap (&a->target, &a->q.v2.map) < 0)
	return "cannot read adapter predictive map";

    a->rebuild_rate = pinfo->rebuild_rate;
    a->dram_size = pinfo->dram_size;

    snprintf (a->name, sizeof (a->name), "a%u", a->target.adapno);
    strcpy (a->product, "PERC2/");
    switch (pinfo->nchannels)
    {
     case 1:	strcat (a->product, "SC"); break;
     case 2:	strcat (a->product, "DC"); break;
     case 4:	strcat (a->product, "QC"); break;
     default:	return "invalid number of channels";
    }
    strncpy (a->bios, (char *) pinfo->bios_version, sizeof (a->bios));
    a->bios[sizeof (a->bios) - 1] = '\0';
    ntrim (a->bios);
    strncpy (a->firmware, (char *) pinfo->fw_version, sizeof (a->firmware));
    a->firmware[sizeof (a->firmware) - 1] = '\0';
    ntrim (a->firmware);

    batteryStatus (a, pinfo->battery_status);

    if (config->numldrv > sizeof (config->ldrv) / sizeof (config->ldrv[0]))
	return "invalid number of logical drives";

    a->num_channels = pinfo->nchannels;
    if ((a->channel = (uint8_t *) malloc (a->num_channels * sizeof (*a->channel))) == NULL)
	return "out of memory (channels)";
    for (k = 0; k < a->num_channels; ++k)
	a->channel[k] = k;

    a->num_physicals = FC_MAX_PHYSICAL_DEVICES;
    if ((a->physical = (struct physical_drive_info *) malloc (a->num_physicals * sizeof (*a->physical))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical, 0, a->num_physicals * sizeof (*a->physical));
    if ((a->physical_list = (struct physical_drive_info **) malloc (a->num_physicals * sizeof (*a->physical_list))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical_list, 0, a->num_physicals * sizeof (*a->physical_list));

    a->num_logicals = config->numldrv;
    if ((a->logical = (struct logical_drive_info *) malloc (a->num_logicals * sizeof (*a->logical))) == NULL)
	return "out of memory (logical drives)";
    memset (a->logical, 0, a->num_logicals * sizeof (*a->logical));

    /* Count how many spans there are. */
    for (k = 0, ml = config->ldrv, a->num_spans = 0; k < config->numldrv; ++k, ++ml)
	a->num_spans += ml->lparam.span_depth;

    if ((a->span = (struct span_info *) malloc (a->num_spans * sizeof (*a->span))) == NULL)
	return "out of memory (spans)";
    memset (a->span, 0, a->num_spans * sizeof (*a->span));

    /* Copy drive states. */
    for (k = 0; k < sizeof (inquiry->pdrv_info.pdrv_state) / sizeof (inquiry->pdrv_info.pdrv_state[0]); ++k)
	switch (inquiry->pdrv_info.pdrv_state[k] & 0xf)
	{
	 case PDRV_UNCNF:	a->physical[k].state = PdStateUnconfiguredGood; continue;
	 case PDRV_ONLINE:	a->physical[k].state = PdStateOnline; continue;
	 case PDRV_FAILED:	a->physical[k].state = PdStateFailed; continue;
	 case PDRV_RBLD:	a->physical[k].state = PdStateRebuild; continue;
	 case PDRV_HOTSPARE:	a->physical[k].state = PdStateHotspare; continue;
	 default:		a->physical[k].state = PdStateUnknown; continue;
	}

    /* Copy drive sizes. */
    for (k = 0; k < sizeof (config->pdrv) / sizeof (config->pdrv[0]); ++k)
	a->physical[k].blocks = config->pdrv[k].size;

    /* Copy drive predictive failures flag */
    for (k = 0; k < 8 * sizeof (a->q.v2.map.map) / sizeof (a->q.v2.map.map[0]); ++k)
	a->physical[k].predictive_failures = ((a->q.v2.map.map[k >> 3] & (1 << (k & 0x7))) != 0);

    /* Examine all the logical drives. */
    for (k = 0, ml = config->ldrv, spanIndex = 0; k < config->numldrv; ++k, ++ml)
    {
	struct span_info		*span;
	adap_span_8ld_t			*mr;
	int				j;
	struct logical_drive_info	*l = &a->logical[k];

	l->adapter = a;
	snprintf (l->name, sizeof (l->name), "a%ud%u", a->target.adapno, k);
	l->target = k;
	switch (ml->lparam.status)
	{
	 case RDRV_OFFLINE:	l->state = LdStateOffline; break;
	 case RDRV_DEGRADED:	l->state = LdStateDegraded; break;
	 case RDRV_OPTIMAL:	l->state = LdStateOptimal; break;
	 case RDRV_DELETED:	l->state = LdStateDeleted; break;
	 default:		l->state = LdStateUnknown; break;
	}
	l->raid_level = ml->lparam.level;
	l->span_size = ml->lparam.row_size;

	l->num_spans = ml->lparam.span_depth;
	if ((l->span = (struct span_reference *) malloc (l->num_spans * sizeof (*l->span))) == NULL)
	    return "out of memory (span references)";

	for (j = 0, mr = ml->span; j < ml->lparam.span_depth; ++j, ++mr)
	{
	    int				i;

	    span = &a->span[spanIndex++];
	    span->adapter = a;
	    span->num_logical_drives = 1;
	    if ((span->logical_drive = (struct logical_drive_info **) malloc (span->num_logical_drives * sizeof (*span->logical_drive))) == NULL)
		return "out of memory (span -> ldrv pointers)";
	    span->logical_drive[0] = l;
	    span->blocks_per_disk = mr->num_blks;
	    span->num_disks = ml->lparam.row_size;
	    if ((span->disk = (struct physical_drive_info **) malloc (span->num_disks * sizeof (*span->disk))) == NULL)
		return "out of memory (span -> disk pointers)";

	    /* Logical drives use the whole span. */
	    l->span[j].offset = 0;
	    l->span[j].blocks_per_disk = span->blocks_per_disk;
	    l->span[j].span = span;

	    for (i = 0; i < span->num_disks; ++i)
	    {
		span->disk[i] = &a->physical[mr->device[i].target];
		span->disk[i]->span = span;
	    }
	}
    }

    return NULL;
}


/* Adapter handling for PERC3 and PERC4 adapters. */
static char *getAdapterConfig3 (struct adapter_config *a)
{
    int					k;
    logdrv_40ld_t			*ml;
    int					spanIndex;
    mraid_pinfo_t			*pinfo = &a->q.v3.adapinfo;
    mraid_inquiry3_t			*enquiry3 = &a->q.v3.enquiry3;
    disk_array_40ld_t			*config = &a->q.v3.config;

    a->target.type = MEGA_ADAPTER_V34;

    if (megaGetAdapterEnquiry3 (&a->target, &a->q.v3.enquiry3) < 0)
	return "cannot query adapter";
    if (megaGetAdapterConfig40 (&a->target, config) < 0)
	return "cannot read adapter config";
    if (megaGetPredictiveMap (&a->target, &a->q.v3.map) < 0)
	return "cannot read adapter predictive map";

    a->rebuild_rate = enquiry3->rebuild_rate;
    a->dram_size = pinfo->dram_size;

    snprintf (a->name, sizeof (a->name), "a%u", a->target.adapno);
    switch (pinfo->nchannels)
    {
     case 1:	break;
     case 2:	break;
     case 4:	break;
     default:	return "invalid number of channels";
    }
    strncpy (a->product, (char *) pinfo->product_name, sizeof (pinfo->product_name));
    a->product[sizeof (a->product) - 1] = '\0';
    ntrim (a->product);
    strncpy (a->bios, (char *) pinfo->bios_version, sizeof (a->bios));
    a->bios[sizeof (a->bios) - 1] = '\0';
    ntrim (a->bios);
    strncpy (a->firmware, (char *) pinfo->fw_version, sizeof (a->firmware));
    a->firmware[sizeof (a->firmware) - 1] = '\0';
    ntrim (a->firmware);

    batteryStatus (a, enquiry3->battery_status);

    if (config->numldrv > sizeof (config->ldrv) / sizeof (config->ldrv[0]))
	return "invalid number of logical drives";

    a->num_channels = pinfo->nchannels;
    if ((a->channel = (uint8_t *) malloc (a->num_channels * sizeof (*a->channel))) == NULL)
	return "out of memory (channels)";
    for (k = 0; k < a->num_channels; ++k)
	a->channel[k] = k;

    a->num_physicals = FC_MAX_PHYSICAL_DEVICES;
    if ((a->physical = (struct physical_drive_info *) malloc (a->num_physicals * sizeof (*a->physical))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical, 0, a->num_physicals * sizeof (*a->physical));
    if ((a->physical_list = (struct physical_drive_info **) malloc (a->num_physicals * sizeof (*a->physical_list))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical_list, 0, a->num_physicals * sizeof (*a->physical_list));

    a->num_logicals = config->numldrv;
    if ((a->logical = (struct logical_drive_info *) malloc (a->num_logicals * sizeof (*a->logical))) == NULL)
	return "out of memory (logical drives)";
    memset (a->logical, 0, a->num_logicals * sizeof (*a->logical));

    /* Count how many spans there are. */
    for (k = 0, ml = config->ldrv, a->num_spans = 0; k < config->numldrv; ++k, ++ml)
	a->num_spans += ml->lparam.span_depth;

    if ((a->span = (struct span_info *) malloc (a->num_spans * sizeof (*a->span))) == NULL)
	return "out of memory (spans)";
    memset (a->span, 0, a->num_spans * sizeof (*a->span));

    /* Copy drive states. */
    for (k = 0; k < sizeof (enquiry3->pdrv_state) / sizeof (enquiry3->pdrv_state[0]); ++k)
	switch (enquiry3->pdrv_state[k] & 0xf)
	{
	 case PDRV_UNCNF:	a->physical[k].state = PdStateUnconfiguredGood; continue;
	 case PDRV_ONLINE:	a->physical[k].state = PdStateOnline; continue;
	 case PDRV_FAILED:	a->physical[k].state = PdStateFailed; continue;
	 case PDRV_RBLD:	a->physical[k].state = PdStateRebuild; continue;
	 case PDRV_HOTSPARE:	a->physical[k].state = PdStateHotspare; continue;
	 default:		a->physical[k].state = PdStateUnknown; continue;
	}

    /* Copy drive sizes. */
    for (k = 0; k < sizeof (config->pdrv) / sizeof (config->pdrv[0]); ++k)
	a->physical[k].blocks = config->pdrv[k].size;

    /* Copy drive predictive failures flag */
    for (k = 0; k < 8 * sizeof (a->q.v3.map.map) / sizeof (a->q.v3.map.map[0]); ++k)
	a->physical[k].predictive_failures = ((a->q.v3.map.map[k >> 3] & (1 << (k & 0x7))) != 0);

    /* Examine all the logical drives. */
    for (k = 0, ml = config->ldrv, spanIndex = 0; k < config->numldrv; ++k, ++ml)
    {
	struct span_info		*span;
	adap_span_40ld_t		*mr;
	int				j;
	struct logical_drive_info	*l = &a->logical[k];

	l->adapter = a;
	snprintf (l->name, sizeof (l->name), "a%ud%u", a->target.adapno, k);
	l->target = k;
	switch (ml->lparam.status)
	{
	 case RDRV_OFFLINE:	l->state = LdStateOffline; break;
	 case RDRV_DEGRADED:	l->state = LdStateDegraded; break;
	 case RDRV_OPTIMAL:	l->state = LdStateOptimal; break;
	 case RDRV_DELETED:	l->state = LdStateDeleted; break;
	 default:		l->state = LdStateUnknown; break;
	}
	l->raid_level = ml->lparam.level;
	l->span_size = ml->lparam.row_size;

	l->num_spans = ml->lparam.span_depth;
	if ((l->span = (struct span_reference *) malloc (l->num_spans * sizeof (*l->span))) == NULL)
	    return "out of memory (span references)";

	for (j = 0, mr = ml->span; j < ml->lparam.span_depth; ++j, ++mr)
	{
	    int				i;

	    span = &a->span[spanIndex++];
	    span->adapter = a;
	    span->num_logical_drives = 1;
	    if ((span->logical_drive = (struct logical_drive_info **) malloc (span->num_logical_drives * sizeof (*span->logical_drive))) == NULL)
		return "out of memory (span -> ldrv pointers)";
	    span->logical_drive[0] = l;
	    span->blocks_per_disk = mr->num_blks;
	    span->num_disks = ml->lparam.row_size;
	    if ((span->disk = (struct physical_drive_info **) malloc (span->num_disks * sizeof (*span->disk))) == NULL)
		return "out of memory (span -> disk pointers)";

	    /* Logical drives use the whole span. */
	    l->span[j].offset = 0;
	    l->span[j].blocks_per_disk = span->blocks_per_disk;
	    l->span[j].span = span;

	    for (i = 0; i < span->num_disks; ++i)
	    {
		span->disk[i] = &a->physical[mr->device[i].target];
		span->disk[i]->span = span;
	    }
	}
    }

#if	0
    /* Go ahead and hit all the other devices that have a non-zero scsi transfer rate. */
    for (k = 0; k < sizeof (a->q.v3.enquiry3.targ_xfer) / sizeof (a->q.v3.enquiry3.targ_xfer[0]); ++k)
	if (a->q.v3.enquiry3.targ_xfer[k])
	    (void) getPhysicalDriveInfo (a, (uint8_t) k, 1);
#endif

    return NULL;
}


int cmpChannel (const void *a, const void *b)
{
    int			x = (int) *((uint8_t *) a);
    int			y = (int) *((uint8_t *) b);
    return x - y;
}

/* Adapter handling for PERC5 adapters. */
static char *getAdapterConfig5 (struct adapter_config *a)
{
    int					k;
    struct mega_array_span_def_sas	*ms;
    struct mega_array_disk_def_sas	*ml;
    struct megasas_ctrl_info		*pinfo = &a->q.v5.adapinfo;
    struct mega_device_list_sas		*device;
    struct mega_array_config_sas	*config = &a->q.v5.config;

    a->target.type = MEGA_ADAPTER_V5;

    if (megaSasGetDeviceList (&a->target, &(a->q.v5.device)) < 0)
	return "cannot retrieve device list";
    device = a->q.v5.device;
    if (megaSasGetArrayConfig (&a->target, &(a->q.v5.config)) < 0)
	return "cannot retrieve array configuration";
    if (megaSasGetBatteryInfo (&a->target, &(a->q.v5.battery)) < 0)
	return "cannot retrieve battery info";

    a->rebuild_rate = pinfo->properties.rebuild_rate;
    a->dram_size = pinfo->memory_size;

    snprintf (a->name, sizeof (a->name), "a%u", a->target.adapno);
    strncpy (a->product, (char *) pinfo->product_name, sizeof (pinfo->product_name));
    a->product[sizeof (a->product) - 1] = '\0';
    ntrim (a->product);

    for (k = 0; k < pinfo->image_component_count; ++k)
    {
	if (!strcmp (pinfo->image_component[k].name, "BIOS"))
	{
	    strncpy (a->bios, pinfo->image_component[k].version, sizeof (a->bios));
	    a->bios[sizeof (a->bios) - 1] = '\0';
	    ntrim (a->bios);
	}
	else if (!strcmp (pinfo->image_component[k].name, "APP "))
	{
	    strncpy (a->firmware, pinfo->image_component[k].version, sizeof (a->firmware));
	    a->firmware[sizeof (a->firmware) - 1] = '\0';
	    ntrim (a->firmware);
	}
    }

    batteryStatus5 (a);

    /* Build enclosure map. */
    for (k = 0, a->num_channels = 0, a->channel = NULL; k < device->num_devices; ++k)
    {
	int			j;

	for (j = 0; j < a->num_channels; ++j)
	    if (device->device[k].enclosure == a->channel[j])
		break;
	if (j < a->num_channels)
	    continue;

	/* Didn't find this enclosure; extend the map */
	++a->num_channels;
	if ((a->channel = (uint8_t *) realloc (a->channel, a->num_channels * sizeof (*a->channel))) == NULL)
	    return "out of memory (channels)";
	a->channel[a->num_channels - 1] = device->device[k].enclosure;
    }
    qsort (a->channel, a->num_channels, sizeof (*a->channel), cmpChannel);

    a->num_physicals = pinfo->pd_present_count;
    if ((a->physical = (struct physical_drive_info *) malloc (a->num_physicals * sizeof (*a->physical))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical, 0, a->num_physicals * sizeof (*a->physical));
    if ((a->physical_list = (struct physical_drive_info **) malloc (a->num_physicals * sizeof (*a->physical_list))) == NULL)
	return "out of memory (physical drives)";
    memset (a->physical_list, 0, a->num_physicals * sizeof (*a->physical_list));

    a->num_logicals = config->header->num_disk_defs;
    if ((a->logical = (struct logical_drive_info *) malloc (a->num_logicals * sizeof (*a->logical))) == NULL)
	return "out of memory (logical drives)";
    memset (a->logical, 0, a->num_logicals * sizeof (*a->logical));

    a->num_spans = config->header->num_span_defs;
    if ((a->span = (struct span_info *) malloc (a->num_spans * sizeof (*a->span))) == NULL)
	return "out of memory (spans)";
    memset (a->span, 0, a->num_spans * sizeof (*a->span));

    /* Get drive info. (This is fast on a PERC5.) */
    for (k = 0; k < device->num_devices; ++k)
	if ((device->device[k].type == INQ_DASD) && (getPhysicalDriveInfo (a, device->device[k].device_id, 1) == NULL))
	    return "cannot get physical device info";

    /* Examine all the spans. */
    for (k = 0, ms = config->span; k < config->header->num_span_defs; ++k, ++ms)
    {
	struct span_info	*span = &a->span[k];
	int			i;

	span->adapter = a;
	span->num_logical_drives = 0;
	span->logical_drive = NULL;
	span->blocks_per_disk = ms->sectors_per_disk;
	span->num_disks = ms->span_size;
	if ((span->disk = (struct physical_drive_info **) malloc (span->num_disks * sizeof (*span->disk))) == NULL)
	    return "out of memory (span -> disk pointers)";

	for (i = 0; i < span->num_disks; ++i)
	{
	    span->disk[i] = getPhysicalDriveInfo (a, ms->disk[i].device_id, 1);
	    span->disk[i]->span = span;
	}
    }

    /* Examine all the logical drives. */
    for (k = 0, ml = config->disk; k < config->header->num_disk_defs; ++k, ++ml)
    {
	struct span_info			*span;
	struct mega_array_disk_entry_sas	*mr;
	int					j;
	struct logical_drive_info		*l = &a->logical[k];

	l->adapter = a;
	snprintf (l->name, sizeof (l->name), "a%ud%u", a->target.adapno, k);
	l->target = k;
	switch (ml->state)
	{
	 case MEGA_SAS_LD_OFFLINE:		l->state = LdStateOffline; break;
	 case MEGA_SAS_LD_PARTIALLY_DEGRADED:	l->state = LdStatePartiallyDegraded; break;
	 case MEGA_SAS_LD_DEGRADED:		l->state = LdStateDegraded; break;
	 case MEGA_SAS_LD_OPTIMAL:		l->state = LdStateOptimal; break;
	 default:				l->state = LdStateUnknown; break;
	}
	l->raid_level = ml->raid_level;
	l->span_size = ml->disks_per_span;

	l->num_spans = ml->num_spans;
	if ((l->span = (struct span_reference *) malloc (l->num_spans * sizeof (*l->span))) == NULL)
	    return "out of memory (span references)";

	for (j = 0, mr = ml->span; j < ml->num_spans; ++j, ++mr)
	{
	    span = &a->span[mr->span_index];
	    ++(span->num_logical_drives);
	    if ((span->logical_drive = (struct logical_drive_info **) realloc (span->logical_drive, span->num_logical_drives * sizeof (*span->logical_drive))) == NULL)
		return "out of memory (span -> ldrv pointers)";
	    span->logical_drive[span->num_logical_drives - 1] = l;

	    l->span[j].offset = mr->offset;
	    l->span[j].blocks_per_disk = mr->sectors_per_disk;
	    l->span[j].span = span;
	}
    }

    return NULL;
}


struct adapter_config *getAdapterConfig (int fd, uint8_t adapno, int sas)
{
    static struct adapter_config	*cf = NULL;
    struct adapter_config		*a;
    char				*status;

    for (a = cf; a; a = a->next)
	if ((a->target.adapno == adapno) && (a->is_sas == sas))
	    return a;

    if ((a = (struct adapter_config *) malloc (sizeof (*a))) == NULL)
	return NULL;
    memset (a, 0, sizeof (*a));

    a->target.fd = fd;
    a->target.adapno = adapno;
    a->is_sas = sas;

    if (sas)
    {
	if (megaSasGetAdapterProductInfo (fd, adapno, &a->q.v5.adapinfo) < 0)
	    return NULL;

	status = getAdapterConfig5 (a);
    }
    else
    {
	mraid_pinfo_t			pinfo;

	if (megaGetAdapterProductInfo (fd, adapno, &pinfo) < 0)
	    return NULL;

	if (pinfo.data_size == 0)
	    status = getAdapterConfig2 (a);
	else
	{
	    a->q.v3.adapinfo = pinfo;
	    status = getAdapterConfig3 (a);
	}
    }

    if (status)
    {
	free (a);
	fprintf (stderr, "adapter %d: %s\n", adapno, status);
	return NULL;
    }

    a->next = cf;
    cf = a;

    return a;
}
