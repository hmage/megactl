#ifndef	_MEGAIOCTL_H
#define	_MEGAIOCTL_H
/*
 *	Definitions for low-level adapter interface.
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


#include	"mega.h"


/* Old-style ioctl structure for megaraid 1 & 2 drivers. Cribbed from version 1 megaraid.h. */
#pragma pack(1)
struct uioctl_t {
	uint32_t inlen;
	uint32_t outlen;
	union {
		uint8_t fca[16];
		struct {
			uint8_t opcode;
			uint8_t subopcode;
			uint16_t adapno;
#if BITS_PER_LONG == 32
			uint8_t *buffer;
			uint8_t pad[4];
#endif
#if BITS_PER_LONG == 64
			uint8_t *buffer;
#endif
			uint32_t length;
		} fcs;
	} ui;
	uint8_t mbox[18];		/* 16 bytes + 2 status bytes */
	mraid_passthru_t pthru;
#if BITS_PER_LONG == 32
	char *data;		/* buffer <= 4096 for 0x80 commands */
	char pad[4];
#endif
#if BITS_PER_LONG == 64
	char *data;
#endif
};
#pragma pack()


extern int	megaErrno;

extern int	megaScsiDriveInquiry (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageCode, uint8_t evpd);
extern int	megaScsiModeSense (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageControl, uint8_t page, uint8_t subpage);
extern int	megaScsiLogSense (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t pageControl, uint8_t page, uint16_t parameterPointer);
extern int 	megaScsiSendDiagnostic (struct mega_adapter_path *adapter, uint8_t target, void *data, uint32_t len, uint8_t testCode, uint8_t unitOffline, uint8_t deviceOffline);
extern int	megaGetAdapterConfig8 (struct mega_adapter_path *adapter, disk_array_8ld_span8_t *data);
extern int	megaGetAdapterConfig40 (struct mega_adapter_path *adapter, disk_array_40ld_t *data);
extern int	megaGetAdapterInquiry (struct mega_adapter_path *adapter, mraid_inquiry1_t *data);
extern int	megaGetAdapterExtendedInquiry (struct mega_adapter_path *adapter, mraid_extinq1_t *data);
extern int	megaGetAdapterEnquiry3 (struct mega_adapter_path *adapter, mraid_inquiry3_t *data);
extern int	megaGetPredictiveMap (struct mega_adapter_path *adapter, struct mega_predictive_map *data);
extern int	megaGetDriveErrorCount (struct mega_adapter_path *adapter, uint8_t target, struct mega_physical_drive_error_info *data);

extern int	megaSasGetDeviceList (struct mega_adapter_path *adapter, struct mega_device_list_sas **data);
extern int	megaSasGetDiskInfo (struct mega_adapter_path *adapter, uint8_t target, struct mega_physical_disk_info_sas *data);
extern int	megaSasGetArrayConfig (struct mega_adapter_path *adapter, struct mega_array_config_sas *data);
extern int	megaSasGetBatteryInfo (struct mega_adapter_path *adapter, struct mega_battery_info_sas *data);

extern int	megaGetDriverVersion (int fd, uint32_t *version);

/* You must call megaGetNumAdapters() before you issue any other inquiries, other than megaGetDriverVersion(). */
extern int	megaGetNumAdapters (int fd, uint32_t *numAdapters, int sas);

extern int	megaGetAdapterProductInfo (int fd, uint8_t adapno, mraid_pinfo_t *info);
extern int	megaSasGetAdapterProductInfo (int fd, uint8_t adapno, struct megasas_ctrl_info *info);
extern int	megaSasAdapterPing (int fd, uint8_t adapno);

extern char	*megaErrorString (void);

#endif
