#ifndef	_MEGA_H
#define	_MEGA_H
/*
 *	Definitions of data structures used by the adapter and by our
 *	high-level interface.
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


#include	"logpage.h"

#include	<sys/types.h>
#include	<sys/uio.h>
#include	<scg/scsireg.h>


typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#define BITS_PER_LONG 32

#include	<stdint.h>
#ifdef	NEED_UINT8_T

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;
typedef		__u64		uint64_t;

#endif

/* DMA addresses come in generic and 64-bit flavours.  */

#ifdef CONFIG_HIGHMEM
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif
typedef u64 dma64_addr_t;

/* Hacks to get kernel module headers to compile. We're not using any data structures where these matter. */
typedef	struct
{
    volatile unsigned int	lock;
}				spinlock_t;
struct semaphore
{
    int				foo;
};

typedef struct { volatile int counter; } atomic_t;
struct tasklet_struct
{
	struct tasklet_struct *next;
	unsigned long state;
	atomic_t count;
	void (*func)(unsigned long);
	unsigned long data;
};

#define	__iomem

#define	__user
#define	wait_queue_head_t	void *
/* typedef	void	wait_queue_head_t; */

struct list_head {
	struct list_head *next, *prev;
};


/* Okay, should be able to include module headers now, hopefully. */
#include	"megaraid/mbox_defs.h"
#include	"megaraid/megaraid_ioctl.h"
#include	"megaraid/megaraid_sas.h"

#define	MAX_CONTROLLERS	32

#define	M_RD_IOCTL_CMD			0x80
#define	M_RD_IOCTL_CMD_NEW		0x81
#define	M_RD_DRIVER_IOCTL_INTERFACE	0x82


#define	SCSI_SELFTEST_DEFAULT		0x00
#define	SCSI_SELFTEST_BACKGROUND_SHORT	0x01
#define	SCSI_SELFTEST_BACKGROUND_LONG	0x02
#define	SCSI_SELFTEST_BACKGROUND_ABORT	0x04
#define	SCSI_SELFTEST_FOREGROUND_SHORT	0x05
#define	SCSI_SELFTEST_FOREGROUND_LONG	0x06


/* megaraid2 header file gets this wrong. */
typedef struct {
	uint8_t		max_commands;
	uint8_t		rebuild_rate;
	uint8_t		max_targ_per_chan;
	uint8_t		nchannels;
	uint8_t		fw_version[4];
	uint16_t	age_of_flash;
	uint8_t		chip_set_value;
	uint8_t		dram_size;
	uint8_t		cache_flush_interval;
	uint8_t		bios_version[4];
	uint8_t		board_type;
	uint8_t		sense_alert;
	uint8_t		write_config_count;
	uint8_t		drive_inserted_count;
	uint8_t		inserted_drive;
	uint8_t		battery_status;
	uint8_t		dec_fault_bus_info;
} __attribute__ ((packed)) mraid_adapinfo1_t;

typedef struct {
	mraid_adapinfo1_t	adapter_info;
	mraid_ldrv_info_t	logdrv_info;
	mraid_pdrv_info_t	pdrv_info;
} __attribute__ ((packed)) mraid_inquiry1_t;

typedef struct {
	mraid_inquiry1_t	raid_inq;
	uint16_t	phys_drv_format[MAX_MBOX_CHANNELS];
	uint8_t		stack_attn;
	uint8_t		modem_status;
	uint8_t		rsvd[2];
} __attribute__ ((packed)) mraid_extinq1_t;


/* Structures we've figured out over many hours of staring at hex data. */

struct mega_physical_drive_error_info
{
    uint8_t			media;
    uint8_t			other;
} __attribute__ ((packed));


struct mega_predictive_map
{
    uint8_t			map[FC_MAX_PHYSICAL_DEVICES / 8];
} __attribute__ ((packed));


struct mega_device_entry_sas {
    uint16_t				device_id;
    uint16_t				enclosure;
    uint8_t				value_1;	/* ? 1, 2 enclosure number + 1? backend port number? */
    uint8_t				slot;
    uint8_t				type;		/* INQ_DASD, INQ_ENCL */
    uint8_t				port;		/* 1 << connected port number */
    uint64_t				sas_address[2];
} __attribute__ ((packed));

/* opcode 0x02010000 */
struct mega_device_list_sas {
    uint32_t				length;
    uint16_t				num_devices;
    uint16_t				rsvd0;
    struct mega_device_entry_sas	device[32];	/* actually any number */
} __attribute__ ((packed));


struct mega_array_header_sas {
    uint32_t				length;
    uint16_t				num_span_defs;
    uint16_t				span_def_size;		/* 0x0120 */
    uint16_t				num_disk_defs;
    uint16_t				disk_def_size;		/* 0x0100 */
    uint16_t				num_hot_spares;
    uint16_t				value_0028;		/* ? 0x0028 */
    uint32_t				pad0[4];
} __attribute__ ((packed));

struct mega_array_span_disk_sas
{
    uint16_t				device_id;		/* 0xffff if device missing */
    uint16_t				sequence;		/* ? 0x0002, 0x0004, 0x0006... as disks are created */
    uint8_t				flag_0:1;
    uint8_t				hotspare:1;
    uint8_t				rebuild:1;
    uint8_t				online:1;
    uint8_t				present:1;
    uint8_t				flag_1;
    uint8_t				enclosure;
    uint8_t				slot;
} __attribute__ ((packed));

struct mega_array_span_def_sas
{
    uint64_t				sectors_per_disk;
    uint16_t				span_size;		/* number of disks in span */
    uint16_t				span_index;		/* 0, 1, 2... */
    uint32_t				value_1;		/* ? 0 */
    uint32_t				pad0[4];
    struct mega_array_span_disk_sas	disk[32];		/* real number is (config.span_def_size - offset .disks) / sizeof span_entry */
} __attribute__ ((packed));

struct mega_array_disk_entry_sas
{
    uint64_t				offset;			/* offset in sectors of this vd */
    uint64_t				sectors_per_disk;	/* sectors used for this vd on each disk */
    uint16_t				span_index;		/* number of this span */
    uint16_t				pad2;			/* ? 0 */
    uint32_t				pad3;			/* ? 0 */
} __attribute__ ((packed));

#define	MEGA_SAS_LD_OFFLINE		0
#define	MEGA_SAS_LD_PARTIALLY_DEGRADED	1
#define	MEGA_SAS_LD_DEGRADED		2
#define	MEGA_SAS_LD_OPTIMAL		3

struct mega_array_disk_def_sas {
    uint16_t				disk_index;		/* 0, 1, 2... */
    uint16_t				sequence;		/* ? 0x0004, 0x0003 */
    char				name[16];		/* null-terminated, max 15 chars */
    uint32_t				flags;			/* ? 0x01000001, 0x00000000 */
    uint32_t				pad0[2];		/* ? 0 */
    uint8_t				raid_level;		/* 0, 1, 5 */
    uint8_t				raid_level_secondary;	/* ? 3 for raid 5 with 4 spans, 0 for raid1 with 1 span */
    uint8_t				raid_level_qualifier;	/* ? 3 for raid 5 with 4 spans, 0 for raid1 with 1 span */
    uint8_t				stripe_size;		/* (2 << this) sectors per stripe; 4 == 8K, 5 == 16K, etc. */
    uint8_t				disks_per_span;
    uint8_t				num_spans;
    uint16_t				state;			/* ? 0 == offline, 1 == partially degraded, 2 == degraded, 3 == optimal */
    uint32_t				value_4;		/* ? 0x00000001, 0x00000000 */
    uint32_t				pad1[5];		/* ? 0 */
    struct mega_array_disk_entry_sas	span[8];		/* real number is (config.disk_def_size - offset .spans) / sizeof disk_entry */
} __attribute__ ((packed));

struct mega_array_hotspare_def_sas
{
    uint16_t				device_id;
    uint16_t				sequence;		/* ? 0x001c, 0x001e, 0x0020 */
    uint32_t				flags;			/* ? 0x00000000 for global, 0x01000001 for dedicated */
    uint32_t				array;			/* dedicated array index */
    uint32_t				pad0[7];		/* ? 0 */
} __attribute__ ((packed));

/* opcode 0x04010000: array config is { header span_def* disk_def* hotspare_def* } */
struct mega_array_config_sas
{
    struct mega_array_header_sas	*header;
    struct mega_array_span_def_sas	*span;
    struct mega_array_disk_def_sas	*disk;
    struct mega_array_hotspare_def_sas	*hotspare;
};

/* opcode 0x05010000 */
#define	MEGA_BATTERY_TYPE_NONE	0
#define	MEGA_BATTERY_TYPE_ITBBU	1
#define	MEGA_BATTERY_TYPE_TBBU	2

struct mega_battery_state_sas
{
    uint8_t				type;			/* see above */
    uint8_t				foo;			/* ? */
    uint16_t				voltage;		/* millivolts */
    uint16_t				current;		/* milliamps */
    uint16_t				temperature;		/* celsius */
    uint32_t				firmware_status;
    uint32_t				pad0[5];		/* ? 0 */

    uint8_t				pad1:4;

    uint8_t				fully_discharged:1;
    uint8_t				fully_charged:1;
    uint8_t				discharging:1;
    uint8_t				initialized:1;

    uint8_t				remaining_time_alarm:1;
    uint8_t				remaining_capacity_alarm:1;
    uint8_t				pad2:1;
    uint8_t				discharge_terminated:1;

    uint8_t				over_temperature:1;
    uint8_t				pad3:1;
    uint8_t				charging_terminated:1;
    uint8_t				over_charged:1;

    uint16_t				charge;			/* percentage */
    uint16_t				charger_status;		/* charger status 0 == off, 1 == complete, 2 == in progress */
    uint16_t				capacity_remaining;	/* milliamp-hours */
    uint16_t				capacity_full;		/* milliamp-hours */
    uint16_t				health;			/* state of health 0 == no, * == good */
    uint32_t				pad9[5];		/* ? 0 */
} __attribute__ ((packed));

/* opcode 0x05020000 */
struct mega_battery_capacity_sas
{
    uint16_t				charge_relative;	/* percentage */
    uint16_t				charge_absolute;	/* percentage */
    uint16_t				capacity_remaining;	/* milliamp-hours */
    uint16_t				capacity_full;		/* milliamp-hours */
    uint16_t				time_empty_run;		/* minutes */
    uint16_t				time_empty_average;	/* minutes */
    uint16_t				time_full_average;	/* minutes */
    uint16_t				cycles;
    uint16_t				error_max;		/* percentage */
    uint16_t				alarm_capacity;		/* milliamp-hours */
    uint16_t				alarm_time;		/* minutes */
    uint16_t				pad0;			/* ? 0 */
    uint32_t				pad1[6];		/* ? 0 */
} __attribute__ ((packed));

/* opcode 0x05030000 */
struct mega_battery_design_sas
{
    uint32_t				manufacture_date;	/* weird encoding: 0xfae87 == 2007/04/07, 0xfaebf == 2007/05/31 */
    uint16_t				design_capacity;	/* milliamp-hours */
    uint16_t				design_voltage;		/* millivolts */
    uint16_t				specification_info;
    uint16_t				serial_number;
    uint16_t				pack_stat_configuration;
    char				manufacturer[12];
    char				device_name[8];
    char				device_chemistry[5];
    char				device_vendor[5];
    uint32_t				pad0[5];		/* ? 0 */
} __attribute__ ((packed));

/* opcode 0x05050100 */
struct mega_battery_properties_sas
{
    uint32_t				device_learn_period;	/* seconds */
    uint32_t				next_learn_time;	/* seconds */
    uint32_t				learn_delay_interval;	/* ? hours */
    uint32_t				auto_learn_mode;	/* ? */
    uint32_t				pad0[4];		/* ? 0 */
} __attribute__ ((packed));

struct mega_battery_info_sas
{
    struct mega_battery_state_sas	state;
    struct mega_battery_capacity_sas	capacity;
    struct mega_battery_design_sas	design;
    struct mega_battery_properties_sas	properties;
};


/* opcode 0x02020000 */
struct mega_physical_disk_info_sas
{
    uint16_t				device_id;
    uint16_t				sequence;
    union
    {
	struct scsi_inquiry			inq;
	uint8_t					buf[96];
    }					inquiry;
    uint16_t				value_x;		/* ? 0x8300 */					/* 0x064 */
    uint16_t				value_y;		/* ? 0x4800, 0x2000 */
    struct
    {
	uint8_t					value[60];
    }					mystery_struct;								/* 0x0a4 */
    uint16_t				value_0;		/* ? 0x0000 */
    uint8_t				port;			/* 1 << connected port number */
    uint8_t				value_1;		/* ? 0 */
    uint32_t				media_errors;
    uint32_t				other_errors;
    uint32_t				predictive_failures;
    uint32_t				predictive_failure_event_sequence;
    uint8_t				failure:1;
    uint8_t				hotspare:1;
    uint8_t				rebuild:1;
    uint8_t				online:1;
    uint8_t				configured:1;
    uint8_t				flags_0:3;
    uint8_t				flags_1;
    uint16_t				value_4;		/* ? 0x0000 */
    uint32_t				value_5;		/* ? 0x00002002, 0x00003003, 0x00003009 */
    uint32_t				sas_address_count;	/* number of sas addresses */
    uint32_t				pad_sas_addr;		/* ? 0x00000000 */
    uint64_t				sas_address[4];
    uint64_t				raw_size;		/* sectors; MegaCli only sees 32 bits */	/* 0x0e8 */
    uint64_t				noncoerced_size;	/* sectors; MegaCli only sees 32 bits */	/* 0x0f0 */
    uint64_t				coerced_size;		/* sectors; MegaCli only sees 32 bits */	/* 0x0f8 */
    uint16_t				enclosure;								/* 0x100 */
    uint8_t				value_9;		/* 1 or 2, not sure what it means, goes with enclosure */
    uint8_t				slot;
    uint8_t				value_10[0xfc];
} __attribute__ ((packed));


/* Unified config structures for generic high-level interface. */

enum mega_adapter_enum {
    MEGA_ADAPTER_V2,				/* PERC2 */
    MEGA_ADAPTER_V34,				/* PERC3 or PERC4 */
    MEGA_ADAPTER_V5,				/* PERC5 (SAS) */
};

/* Structure for io to adapters. */
struct mega_adapter_path
{
    int				fd;		/* block device descriptor for adapter access */
    uint8_t			adapno;		/* adapter number */
    enum mega_adapter_enum	type;		/* adapter variant */
};


struct log_page_list
{
    struct logData		log;
    uint8_t			buf[4095];	/* rhl 7.3 croaks on >= 4096 */
    struct log_page_list	*next;
};

enum physical_drive_state
{
    PdStateUnknown,
    PdStateUnconfiguredGood,
    PdStateUnconfiguredBad,
    PdStateHotspare,
    PdStateFailed,
    PdStateRebuild,
    PdStateOnline,
};

struct physical_drive_info
{
    uint8_t			present;		/* whether drive responds to inquiry */
    struct adapter_config	*adapter;		/* adapter this drive belongs to */
    struct span_info		*span;			/* span this disk is a member of */
    char			name[16];		/* drive name (AxCyTz) */
    uint16_t			target;			/* scsi channel+id or device_id */
    uint16_t			channel;		/* channel or enclosure */
    uint8_t			id;			/* scsi id or enclosure slot */
    enum physical_drive_state	state;			/* drive state */
    char			*error_string;		/* status error string (NULL if okay) */
    uint64_t			blocks;			/* number of blocks */
    char			vendor[9];		/* vendor name */
    char			model[17];		/* vendor model */
    char			revision[5];		/* firmware version */
    char			serial[32];		/* serial number */
    uint32_t			predictive_failures;	/* predictive failure count */
    uint32_t			media_errors;
    uint32_t			other_errors;
    struct scsi_inquiry		inquiry;		/* scsi inquiry result */
    struct log_page_list	*log;

    union
    {
	struct
	{
	    struct mega_physical_disk_info_sas	info;
	}			v5;
    }				q;
};

struct span_info
{
    struct adapter_config	*adapter;		/* adapter this span belongs to */
    uint32_t			blocks_per_disk;	/* blocks used per disk for this span */
    uint32_t			num_disks;		/* number of disks in this span */
    struct physical_drive_info	**disk;			/* pointers to component disks */
    uint32_t			num_logical_drives;	/* how many logical drives this span belongs to */
    struct logical_drive_info	**logical_drive;	/* pointers to logical drives */
};

struct span_reference
{
    uint64_t			offset;			/* offset into each disk */
    uint64_t			blocks_per_disk;	/* number of blocks used per disk */
    struct span_info		*span;			/* the span */
};

enum logical_drive_state
{
    LdStateUnknown,
    LdStateOffline,
    LdStatePartiallyDegraded,
    LdStateDegraded,
    LdStateOptimal,
    LdStateDeleted,
};

struct logical_drive_info
{
    struct adapter_config	*adapter;		/* adapter this drive belongs to */
    char			name[16];		/* logical drive name (AxLDy) */
    uint16_t			target;			/* logical drive number */
    enum logical_drive_state	state;			/* logical drive state */
    uint8_t			raid_level;		/* raid level */
    uint8_t			num_spans;		/* how many spans in this logical drive */
    struct span_reference	*span;			/* pointers to component spans */
    uint8_t			span_size;		/* number of disks per span */
};

enum battery_charger_state
{
    ChargerStateUnknown,
    ChargerStateFailed,
    ChargerStateInProgress,
    ChargerStateComplete,
};

struct adapter_config
{
    struct mega_adapter_path	target;			/* adapter access path */
    uint8_t			is_sas;			/* adapter is a sas adapter */
    char			name[16];		/* adapter name (Ax) */
    char			product[81];		/* adapter product name */
    char			bios[17];		/* adapter bios version */
    char			firmware[17];		/* adapter firmware version */
    struct
    {
	uint8_t				healthy:1;
	uint8_t				module_missing:1;
	uint8_t				pack_missing:1;
	uint8_t				low_voltage:1;
	uint8_t				high_temperature:1;
	uint8_t				cycles_exceeded:1;
	uint8_t				over_charged:1;
	enum battery_charger_state	charger_state;
	int16_t				voltage;
	int16_t				temperature;
    }				battery;
    uint16_t			dram_size;		/* size of DRAM in MB */
    uint16_t			rebuild_rate;		/* rebuild rate as percentage */
    uint16_t			num_channels;		/* number of channels or enclosures */
    uint8_t			*channel;		/* channel/enclosure map */
    uint16_t			num_physicals;
    struct physical_drive_info	*physical;
    struct physical_drive_info	**physical_list;	/* ordered list of physical devices */
    uint16_t			num_spans;		/* number of spans */
    struct span_info		*span;
    uint16_t			num_logicals;		/* number of logical drives */
    struct logical_drive_info	*logical;		/* logical drives */
    struct adapter_config	*next;

    /* adapter-specific data structures */
    union
    {
	struct
	{
	    mraid_inquiry1_t			inquiry;
	    struct mega_predictive_map		map;
	    disk_array_8ld_span8_t		config;
	}			v2;
	struct
	{
	    mraid_pinfo_t			adapinfo;
	    mraid_inquiry3_t			enquiry3;
	    struct mega_predictive_map		map;
	    disk_array_40ld_t			config;
	}			v3;
	struct
	{
	    struct megasas_ctrl_info		adapinfo;
	    struct mega_device_list_sas		*device;
	    struct mega_array_config_sas	config;
	    struct mega_battery_info_sas	battery;
	}			v5;
    }				q;
};


#endif
