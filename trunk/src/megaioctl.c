/*
 *	Low-level interface to adapter information.
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

/* Don't include <sys/types.h> */

#include	<memory.h>
#include	<malloc.h>
#include	<errno.h>
#include	<sys/ioctl.h>
#include	<scsi/scsi.h>


int		megaErrno = 0;


static int doIoctl (struct mega_adapter_path *adapter, void *u)
{
    switch (adapter->type)
    {
     case MEGA_ADAPTER_V2:
     case MEGA_ADAPTER_V34:
	return ioctl (adapter->fd, _IOWR(MEGAIOC_MAGIC, 0, struct uioctl_t), u);
     case MEGA_ADAPTER_V5:
	return ioctl (adapter->fd, MEGASAS_IOC_FIRMWARE, u);
    }
    return -1;
}


static int driverQuery (int fd, uint16_t adap, void *data, uint32_t len, uint8_t subop)
{
    struct uioctl_t		u;
    struct mega_adapter_path	adapter;

    memset (&u, 0, sizeof u);
    u.outlen = len;
    u.ui.fcs.opcode = M_RD_DRIVER_IOCTL_INTERFACE;
    u.ui.fcs.subopcode = subop;
    u.ui.fcs.length = len;
    u.data = data;
    if (data)
	memset (data, 0, len);

    adapter.fd = fd;
    adapter.type = MEGA_ADAPTER_V34;
    if (doIoctl (&adapter, &u) < 0)
    {
	megaErrno = errno;
	return -1;
    }
    return 0;
}


static int oldCommand (struct mega_adapter_path *adapter, void *data, uint32_t len, uint8_t cmd, uint8_t opcode, uint8_t subopcode)
{
    struct uioctl_t		u;
    int_mbox_t			*m = (int_mbox_t *) &u.mbox;

    memset (&u, 0, sizeof u);
    u.outlen = len;
    u.ui.fcs.opcode = M_RD_IOCTL_CMD;
    u.ui.fcs.adapno = MKADAP(adapter->adapno);
    u.data = data;
    m->cmd = cmd;
    m->opcode = opcode;
    m->subopcode = subopcode;
    m->xferaddr = (uint32_t) data;
    if (data)
	memset (data, 0, len);

    if (doIoctl (adapter, &u) < 0)
    {
	megaErrno = errno;
	return -1;
    }
    return 0;
}


static int newCommand (struct mega_adapter_path *adapter, void *data, uint32_t len, uint8_t cmd, uint8_t opcode, uint8_t subopcode)
{
    struct uioctl_t		u;
    int_mbox_t			*m = (int_mbox_t *) &u.mbox;

    memset (&u, 0, sizeof u);
    u.outlen = len;
    u.ui.fcs.opcode = M_RD_IOCTL_CMD_NEW;
    u.ui.fcs.adapno = MKADAP(adapter->adapno);
    u.ui.fcs.buffer = data;
    u.ui.fcs.length = len;
    u.data = data;
    m->cmd = cmd;
    m->opcode = opcode;
    m->subopcode = subopcode;
    m->xferaddr = (uint32_t) data;
    if (data)
	memset (data, 0, len);

    if (doIoctl (adapter, &u) < 0)
    {
	megaErrno = errno;
	return -1;
    }
    return 0;
}


static int sasCommand (struct mega_adapter_path *adapter, void *data, uint32_t len, uint32_t opcode, uint16_t flags, void *mbox, uint32_t mboxlen)
{
    struct megasas_iocpacket	u;
    struct megasas_dcmd_frame	*f = (struct megasas_dcmd_frame *) &u.frame;

    memset (&u, 0, sizeof u);
    u.host_no = (u16) adapter->adapno;

    f->cmd = MFI_CMD_DCMD;
    f->flags = (u16) flags;
    f->opcode = (u32) opcode;

    if ((data != NULL) && (len > 0))
    {
	u.sgl_off = ((void *) &f->sgl) - ((void *) f);
	u.sge_count = 1;
	u.sgl[0].iov_base = data;
	u.sgl[0].iov_len = len;
	f->sge_count = 1;
	f->data_xfer_len = (u32) len;
	f->sgl.sge32[0].phys_addr = (u32) data;
	f->sgl.sge32[0].length = (u32) len;
    }

    if (mbox != NULL)
	memcpy (&f->mbox, mbox, mboxlen);

    if (doIoctl (adapter, &u) < 0)
    {
	megaErrno = errno;
	return -1;
    }
    return f->cmd_status;
}


static int passthruCommand (struct mega_adapter_path *adapter, void *data, uint32_t len, uint8_t target, uint8_t *cdb, uint8_t cdblen)
{
    if ((adapter->type == MEGA_ADAPTER_V2) || (adapter->type == MEGA_ADAPTER_V34))
    {
	struct uioctl_t			u;
	int_mbox_t			*m = (int_mbox_t *) &u.mbox;
	mraid_passthru_t		*p = &u.pthru;

	memset (&u, 0, sizeof u);
	u.outlen = len;
	u.ui.fcs.opcode = M_RD_IOCTL_CMD;
	u.ui.fcs.adapno = MKADAP(adapter->adapno);
	u.data = data;
	m->cmd = MBOXCMD_PASSTHRU;
	m->xferaddr = (uint32_t) p;
	p->timeout = 3;
	p->ars = 1;
	p->target = target;
	p->dataxferaddr = (uint32_t) data;
	p->dataxferlen = len;
	p->scsistatus = 239;	/* HMMM */
	memcpy (p->cdb, cdb, cdblen);
	p->cdblen = cdblen;
	if (data)
	    memset (data, 0, len);

	if (doIoctl (adapter, &u) < 0)
	{
	    megaErrno = errno;
	    return -1;
	}

	if (m->status)
	{
	    megaErrno = - (m->status);
	    return -1;
	}

	if (p->scsistatus & CHECK_CONDITION)
	{
	    megaErrno = - CHECK_CONDITION;
	    return -1;
	}
	if ((p->scsistatus & STATUS_MASK) != GOOD)
	{
	    megaErrno = - (p->scsistatus & STATUS_MASK);
	    return -1;
	}
    }
    else
    {
	struct megasas_iocpacket	u;
	struct megasas_pthru_frame	*f = (struct megasas_pthru_frame *) &u.frame;

	memset (&u, 0, sizeof u);
	u.host_no = (u16) adapter->adapno;

	f->cmd = MFI_CMD_PD_SCSI_IO;
	f->target_id = target;
	f->cdb_len = cdblen;
	f->flags = MFI_FRAME_DIR_READ;
	memcpy (f->cdb, cdb, cdblen);

	if ((data != NULL) && (len > 0))
	{
	    u.sgl_off = ((void *) &f->sgl) - ((void *) f);
	    u.sge_count = 1;
	    u.sgl[0].iov_base = data;
	    u.sgl[0].iov_len = len;

	    f->sge_count = 1;
	    f->data_xfer_len = (u32) len;
	    f->sgl.sge32[0].phys_addr = (u32) data;
	    f->sgl.sge32[0].length = (u32) len;
	}

	if (doIoctl (adapter, &u) < 0)
	{
	    megaErrno = errno;
	    return -1;
	}

	if (f->cmd_status)
	{
	    megaErrno = - (f->cmd_status);
	    return -1;
	}

	if ((f->scsi_status & STATUS_MASK) != GOOD)
	{
	    megaErrno = - (f->scsi_status & STATUS_MASK);
	    return -1;
	}
    }

    return 0;
}


int megaScsiDriveInquiry (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageCode, uint8_t evpd)
{
    uint8_t		cdb[6];

    cdb[0] = INQUIRY;
    cdb[1] = (evpd != 0);
    cdb[2] = pageCode;
    cdb[3] = (len >> 8) & 0xff;
    cdb[4] = len & 0xff;
    cdb[5] = 0;

    return passthruCommand (adapter, data, len, target, cdb, sizeof cdb);
}


int megaScsiModeSense (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageControl, uint8_t page, uint8_t subpage)
{
#ifdef	USE_MODE_SENSE_6
    uint8_t		cdb[6];

    cdb[0] = MODE_SENSE;
    cdb[1] = 0;		/* dbd in bit 3 */
    cdb[2] = ((pageControl & 0x3) << 6) | (page & 0x3f);
    cdb[3] = subpage;
    cdb[4] = len & 0xff;
    cdb[5] = 0;
#else
    uint8_t		cdb[10];

    cdb[0] = MODE_SENSE_10;
    cdb[1] = 0;		/* llbaa in bit 4, dbd in bit 3 */
    cdb[2] = ((pageControl & 0x3) << 6) | (page & 0x3f);
    cdb[3] = subpage;
    cdb[4] = 0;
    cdb[5] = 0;
    cdb[6] = 0;
    cdb[7] = (len >> 8) & 0xff;
    cdb[8] = len & 0xff;
    cdb[9] = 0;
#endif

    return passthruCommand (adapter, data, len, target, cdb, sizeof cdb);
}


int megaScsiLogSense (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageControl, uint8_t page, uint16_t parameterPointer)
{
    uint8_t		cdb[10];

    cdb[0] = LOG_SENSE;
    cdb[1] = 0;		/* ppc in bit 1, sp in bit 1 */
    cdb[2] = ((pageControl & 0x3) << 6) | (page & 0x3f);
    cdb[3] = 0;
    cdb[4] = 0;
    cdb[5] = (parameterPointer >> 8) & 0xff;
    cdb[6] = parameterPointer & 0xff;
    cdb[7] = (len >> 8) & 0xff;
    cdb[8] = len & 0xff;
    cdb[9] = 0;

    return passthruCommand (adapter, data, len, target, cdb, sizeof cdb);
}


int megaScsiSendDiagnostic (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t testCode, uint8_t unitOffline, uint8_t deviceOffline)
{
    uint8_t		cdb[6];

    cdb[0] = SEND_DIAGNOSTIC;
    cdb[1] = ((testCode & 0x7) << 5) | ((deviceOffline != 0) << 1) | (unitOffline != 0);
    cdb[2] = 0;
    cdb[3] = 0;
    cdb[4] = 0;
    cdb[5] = 0;

    return passthruCommand (adapter, data, len, target, cdb, sizeof cdb);
}


int megaGetAdapterConfig8 (struct mega_adapter_path *adapter, disk_array_8ld_span8_t *config)
{
    return oldCommand (adapter, config, sizeof (*config), NEW_READ_CONFIG_8LD, 0, 0);
}


int megaGetAdapterConfig40 (struct mega_adapter_path *adapter, disk_array_40ld_t *config)
{
    return newCommand (adapter, config, sizeof (*config), FC_NEW_CONFIG, OP_DCMD_READ_CONFIG, 0);
}


int megaGetAdapterInquiry (struct mega_adapter_path *adapter, mraid_inquiry1_t *data)
{
    return oldCommand (adapter, data, sizeof (*data), MBOXCMD_ADAPTERINQ, 0, 0);
}


int megaGetAdapterExtendedInquiry (struct mega_adapter_path *adapter, mraid_extinq1_t *data)
{
    return oldCommand (adapter, data, sizeof (*data), MBOXCMD_ADPEXTINQ, 0, 0);
}


int megaGetAdapterEnquiry3 (struct mega_adapter_path *adapter, mraid_inquiry3_t *data)
{
    return newCommand (adapter, data, sizeof (*data), FC_NEW_CONFIG, NC_SUBOP_ENQUIRY3, 0);
}


int megaGetPredictiveMap (struct mega_adapter_path *adapter, struct mega_predictive_map *data)
{
    return oldCommand (adapter, data, sizeof (*data), MAIN_MISC_OPCODE, 0x0f, 0);
}


int megaGetDriveErrorCount (struct mega_adapter_path *adapter, uint8_t target, struct mega_physical_drive_error_info *data)
{
    return oldCommand (adapter, data, sizeof (*data), 0x77, 0, target);
}


int megaSasGetDeviceList (struct mega_adapter_path *adapter, struct mega_device_list_sas **data)
{
    unsigned char		buf[0x20];
    uint32_t			len;

    if (sasCommand (adapter, buf, sizeof buf, 0x02010000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
	return -1;
    len = ((struct mega_device_list_sas *) buf)->length;
    if ((*data = (struct mega_device_list_sas *) malloc (len)) == NULL)
    {
	megaErrno = errno;
	return -1;
    }
    return sasCommand (adapter, *data, len, 0x02010000, MFI_FRAME_DIR_READ, NULL, 0);
}


int megaSasGetDiskInfo (struct mega_adapter_path *adapter, uint8_t target, struct mega_physical_disk_info_sas *data)
{
    uint8_t 			mbox[0xc];

    memset (&mbox, 0, sizeof mbox);
    mbox[0] = target;
    return sasCommand (adapter, data, sizeof (*data), 0x02020000, MFI_FRAME_DIR_READ, mbox, sizeof mbox);
}


int megaSasGetArrayConfig (struct mega_adapter_path *adapter, struct mega_array_config_sas *data)
{
    unsigned char		buf[0x20];
    uint32_t			len;

    if (sasCommand (adapter, buf, sizeof buf, 0x04010000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
	return -1;
    len = ((struct mega_array_header_sas *) buf)->length;
    if ((data->header = (struct mega_array_header_sas *) malloc (len)) == NULL)
    {
	megaErrno = errno;
	return -1;
    }
    if (sasCommand (adapter, data->header, len, 0x04010000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
    {
	megaErrno = errno;
	return -1;
    }

    data->span = (struct mega_array_span_def_sas *) (data->header + 1);
    data->disk = (struct mega_array_disk_def_sas *) (data->span + data->header->num_span_defs);
    data->hotspare = (struct mega_array_hotspare_def_sas *) (data->disk + data->header->num_disk_defs);

    return 0;
}

int megaSasGetBatteryInfo (struct mega_adapter_path *adapter, struct mega_battery_info_sas *data)
{
    if (sasCommand (adapter, &(data->state), sizeof (data->state), 0x05010000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
	return -1;
    if (sasCommand (adapter, &(data->capacity), sizeof (data->capacity), 0x05020000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
	return -1;
    if (sasCommand (adapter, &(data->design), sizeof (data->design), 0x05030000, MFI_FRAME_DIR_READ, NULL, 0) < 0)
	return -1;
    return sasCommand (adapter, &(data->properties), sizeof (data->properties), 0x05050100, MFI_FRAME_DIR_READ, NULL, 0);
}


int megaGetDriverVersion (int fd, uint32_t *version)
{
    return driverQuery (fd, 0, version, sizeof (*version), 'e');
}


int megaGetNumAdapters (int fd, uint32_t *numAdapters, int sas)
{
    if (sas)
    {
	uint8_t		k;
	for (k = 0; k < 16; ++k)
	    if (megaSasAdapterPing (fd, k) < 0)
		break;
	*numAdapters = k;
	return 0;
    }
    else
	return driverQuery (fd, 0, numAdapters, sizeof (*numAdapters), 'm');
}


int megaGetAdapterProductInfo (int fd, uint8_t adapno, mraid_pinfo_t *data)
{
    struct mega_adapter_path	adapter;
    adapter.fd = fd;
    adapter.adapno = adapno;
    adapter.type = MEGA_ADAPTER_V34;
    return newCommand (&adapter, data, sizeof (*data), FC_NEW_CONFIG, NC_SUBOP_PRODUCT_INFO, 0);
}


int megaSasGetAdapterProductInfo (int fd, uint8_t adapno, struct megasas_ctrl_info *data)
{
    struct mega_adapter_path	adapter;
    adapter.fd = fd;
    adapter.adapno = adapno;
    adapter.type = MEGA_ADAPTER_V5;
    return sasCommand (&adapter, data, sizeof (*data), MR_DCMD_CTRL_GET_INFO, MFI_FRAME_DIR_READ, NULL, 0);
}


int megaSasAdapterPing (int fd, uint8_t adapno)
{
    struct mega_adapter_path	adapter;
    unsigned char		data[0xc4];
    adapter.fd = fd;
    adapter.adapno = adapno;
    adapter.type = MEGA_ADAPTER_V5;
    return sasCommand (&adapter, data, sizeof data, 0x04060100, MFI_FRAME_DIR_READ, NULL, 0);
}


char *megaErrorString (void)
{
    if (megaErrno >= 0)
	return strerror (megaErrno);
    switch (-megaErrno)
    {
     case CHECK_CONDITION:		return "scsi command status CHECK_CONDITION"; break;
     case CONDITION_GOOD:		return "scsi command status CONDITION_GOOD"; break;
     case BUSY:				return "scsi command status BUSY"; break;
     case INTERMEDIATE_GOOD:		return "scsi command status INTERMEDIATE_GOOD"; break;
     case INTERMEDIATE_C_GOOD:		return "scsi command status INTERMEDIATE_C_GOOD"; break;
     case RESERVATION_CONFLICT:		return "scsi command status RESERVATION_CONFLICT"; break;
     case COMMAND_TERMINATED:		return "scsi command status COMMAND_TERMINATED"; break;
     case QUEUE_FULL:			return "scsi command status QUEUE_FULL"; break;
     default:				return "scsi command status unknown"; break;
    }
}


