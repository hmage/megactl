/*
 *	Main program for ioctl tracer.
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

megatrace

Program to ptrace another program and inspect megaraid-related ioctl
syscalls it makes.

Author: Jefferson Ogata (JO317) <ogata@antibozo.net>
Date: 2006/01/23

TODO:

Fixes for 64-bit systems.

********************************************************************/


#include	"mega.h"
#include	"megaioctl.h"


#include	<stdio.h>
#include	<unistd.h>
#include	<errno.h>
#include	<string.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<sys/ptrace.h>
#include	<sys/wait.h>
#include	<asm/user.h>
#include	<sys/syscall.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include	<time.h>
#include	<scsi/scsi.h>


#include	"callinfo.h"
#include	"dumpbytes.h"

enum state { UNTRACED, INBOUND, OUTBOUND };


void copyout (void *buf, size_t len, pid_t pid, uint32_t addr)
{
    off_t		k;
    uint32_t		*z = (uint32_t *) buf;
    uint32_t		sd;

    for (k = 0; k < (len + 3) / 4; ++k)
    {
	sd = ptrace (PTRACE_PEEKDATA, pid, (void *) (addr + 4 * k), 0);
	z[k] = sd;
    }
}


void copyin (void *buf, size_t len, pid_t pid, uint32_t addr)
{
    off_t		k;
    uint32_t		*z = (uint32_t *) buf;

    for (k = 0; k < (len + 3) / 4; ++k)
	ptrace (PTRACE_POKEDATA, pid, (void *) (addr + 4 * k), z[k]);
}


int main (int argc, char **argv, char **environ)
{
    pid_t		pid;
    int			state = UNTRACED;
    struct timeval	starttv;
    int			printproc = 0;

    if ((pid = fork ()) == 0)
    {
	ptrace (PTRACE_TRACEME);
	++argv;
	execvp (argv[0], argv);
	fprintf (stderr, "execve: %s: %s\n", argv[0], strerror (errno));
	_exit (1);
    }

    if (pid < 0)
    {
	fprintf (stderr, "fork: %s\n", strerror (errno));
	exit (1);
    }

    gettimeofday (&starttv, NULL);

    /*fprintf (stderr, "%d\n", sizeof (mega_host_config));*/

    while (1)
    {
	pid_t		w;
	int		st;

	w = waitpid (pid, &st, /*WNOHANG*/ 0);

	if (w == 0)
	{
	    fprintf (stderr, "wait: no child\n");
	    usleep (10);
	    continue;
	}

	if (w < 0)
	{
	    fprintf (stderr, "wait: %s\n", strerror (errno));
	    exit (1);
	}

	if (WIFEXITED (st))
	{
	    if (printproc)
		fprintf (stderr, "child exited with status %d\n", WEXITSTATUS (st));
	    break;
	}

	if (WIFSTOPPED (st))
	{
	    if (WSTOPSIG (st) == SIGTRAP)
	    {
		struct timeval		tv;
		long			secs;
		long			usecs;
		char			tbuf[1024];
		int			printcalls = (getenv ("LOG_CALLS") != NULL);
		int			printthis = 0;
		int			printregs = (getenv ("LOG_REGS") != NULL);

		struct user_regs_struct	r;
		u32			call;

		gettimeofday (&tv, NULL);
		secs = tv.tv_sec - starttv.tv_sec;
		usecs = tv.tv_usec - starttv.tv_usec;
		if (usecs < 0)
		{
		    usecs += 1000000;
		    secs -= 1;
		}
		snprintf (tbuf, sizeof tbuf, "%ld.%06ld", secs, usecs);

		if (ptrace (PTRACE_GETREGS, pid, 0, &r) < 0)
		{
		    fprintf (stderr, "ptrace:getregs: %s\n", strerror (errno));
		    exit (1);
		}
		call = r.orig_eax;
		/*printthis = call == SYS_ioctl;*/

		if (state == INBOUND)
		{
		    if (printcalls || printthis)
		    {
			if ((call >= 0) && (call < callmax) && (callinfo[call].name != NULL))
			    fprintf (stderr, "%s()", callinfo[call].name);
			else
			    fprintf (stderr, "syscall(%u)", call);
		    }
		}

		if (state == OUTBOUND)
		{
		    if ((call < 0) || (call > callmax) || (callinfo[call].name == NULL))
		    {
			fprintf (stderr, "= 0x%08lx\n", (unsigned long) r.eax);
		    }
		    else
		    {
			if (callinfo[call].ptrval)
			{
			    if (printcalls || printthis)
				fprintf (stderr, " = 0x%08lx\n", r.eax);
			}
			else
			{
			    long	rv = r.eax;
			    if (rv < 0)
			    {
				if (printcalls || printthis)
				    fprintf (stderr, " = -1 (%s)\n", strerror (-rv));
			    }
			    else
			    {
				if (printcalls || printthis)
				    fprintf (stderr, " = %lu\n", rv);
			    }
			}
		    }
		}

		if ((call == SYS_ioctl) && ((state == OUTBOUND) || getenv ("LOG_INBOUND")))
		{
		    unsigned int		len = 16;
		    unsigned char		buf[65536];

		    unsigned long		fd = r.ebx;

		    unsigned long		ioc = r.ecx;
		    unsigned int		iocdir = _IOC_DIR(ioc);
		    unsigned char		ioctype = _IOC_TYPE(ioc);
		    unsigned int		iocnr = _IOC_NR(ioc);
		    unsigned int		iocsize = _IOC_SIZE(ioc);
		    char			*iocdirname;

		    unsigned long		arg = r.edx;

		    switch (iocdir)
		    {
		     case _IOC_READ:		iocdirname = "r"; break;
		     case _IOC_WRITE:		iocdirname = "w"; break;
		     case _IOC_READ|_IOC_WRITE:	iocdirname = "rw"; break;
		     default:			iocdirname = "none"; break;
		    }

		    fprintf (stderr, "%s: ioctl(%ld, _IOC(\"%s\",'%c',0x%02x,0x%02x), 0x%08lx)", tbuf, fd, iocdirname, ioctype, iocnr, iocsize, arg);
		    if (state == OUTBOUND)
			fprintf (stderr, " = %ld\n", r.eax);
		    if (getenv ("LOG_INBOUND"))
			fprintf (stderr, "\n");

		    if (_IOC_SIZE(ioc) > len)
			len = _IOC_SIZE(ioc);
		    if (len > sizeof buf)
			len = sizeof buf;

		    if (printregs)
			fprintf (stderr, "    ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx ebp=%08lx eax=%08lx ds=%04x __ds=%04x es=%04x __es=%04x fs=%04x __fs=%04x gs=%04x __gs=%04x orig_eax=%08lx eip=%08lx cs=%04x __cs=%04x eflags=%08lx esp=%08lx ss=%04x __ss=%04x\n", r.ebx, r.ecx, r.edx, r.esi, r.edi, r.ebp, r.eax, r.ds, r.__ds, r.es, r.__es, r.fs, r.__fs, r.gs, r.__gs, r.orig_eax, r.eip, r.cs, r.__cs, r.eflags, r.esp, r.ss, r.__ss);

		    copyout (buf, len, pid, r.edx);

		    if ((ioctype == 'm') && (iocnr == 0) && (iocsize == sizeof (struct uioctl_t)))
		    {
			/* megaraid_mbox-style ioctl */
			struct uioctl_t		*uio = (struct uioctl_t *) buf;
			mraid_passthru_t	p = uio->pthru;
			unsigned long		datalen;
			int_mbox_t		*m = (int_mbox_t *) uio->mbox;

			fprintf (stderr, "    uio:\tinlen %u, outlen %u, { op %02x, subop %02x, adap %04x, buf %08lx, len %u }, data %08lx\n", uio->inlen, uio->outlen, uio->ui.fcs.opcode, uio->ui.fcs.subopcode, uio->ui.fcs.adapno, (unsigned long) uio->ui.fcs.buffer, uio->ui.fcs.length, (unsigned long) uio->data);
			dumpbytes (stderr, uio, sizeof (*uio), uio, "uioc");
			fprintf (stderr, "    mbox:\tcmd %02x, cmdid %02x, op %02x, subop %02x, lba %08x, xfer %08x, ldrv %02x, numsge %u, busy %u, nstatus %u, status %u\n", m->cmd, m->cmdid, m->opcode, m->subopcode, m->lba, m->xferaddr, m->logdrv, m->rsvd[0], m->rsvd[2], m->numstatus, m->status);
			dumpbytes (stderr, uio->mbox, sizeof (uio->mbox), uio->mbox, "mbox");
			fprintf (stderr, "    pass:\ttimeout %u, ars %u, isldrv %u, ldrv %u, chan %u, targ %02x, qtag %u, qact %u, numsge %u, scsistat %u\n", p.timeout, p.ars, p.islogical, p.logdrv, p.channel, p.target, p.queuetag, p.queueaction, p.numsge, p.scsistatus);
			dumpbytes (stderr, &p.cdb, p.cdblen, &p.cdb, "cdb");
			dumpbytes (stderr, &p.reqsensearea, p.reqsenselen, &p.reqsensearea, "rqsense");
			dumpbytes (stderr, &uio->pthru, sizeof (uio->pthru), &uio->pthru, "pass");
			if (p.dataxferaddr && p.dataxferlen)
			{
			    unsigned char	data[4096];
			    unsigned long	len = p.dataxferlen;
			    if (len > sizeof data)
				len = sizeof data;
			    copyout (data, len, pid, p.dataxferaddr);
			    dumpbytes (stderr, data, len, (void *) p.dataxferaddr, "parm");
			}

			datalen = uio->ui.fcs.length;
			if (datalen < uio->outlen)
			    datalen = uio->outlen;
			if (datalen > sizeof buf)
			    datalen = sizeof buf;
			if (datalen < 16)
			    datalen = 16;
			if (datalen && uio->data)
			{
//			    if ((state == INBOUND) && (datalen > uio->inlen))
//				datalen = uio->inlen;
//			    else if ((state == OUTBOUND) && (datalen > uio->outlen))
//				datalen = uio->outlen;
//			    if (datalen)
//			    {
				copyout (buf, datalen, pid, (unsigned long) uio->data);
				dumpbytes (stderr, buf, datalen, uio->data, "data");
//			    }
			}
		    }
		    else if ((ioctype == 'M') && (iocnr == 1) && (iocsize == sizeof (struct megasas_iocpacket)))
		    {
			/* megaraid_sas-style ioctl */
			struct megasas_iocpacket	*iocp = (struct megasas_iocpacket *) buf;
			struct megasas_header		*mh = &(iocp->frame.hdr);
			int				k;
			u8				cmd;
			struct megasas_sge32		*sge = (struct megasas_sge32 *) ((u32) (&iocp->frame) + iocp->sgl_off);
			int				log = 1;

			cmd = mh->cmd;
			if (cmd == MFI_CMD_DCMD)
			{
			    struct megasas_dcmd_frame	*f = (struct megasas_dcmd_frame *) mh;

			    if ((getenv ("LOG_OPCODE") != 0) && (strtoul (getenv ("LOG_OPCODE"), NULL, 0) != f->opcode))
			    {
				log = 0;
			    }

/* Lie like a rug. */
if ((f->opcode == 0x02020000) && (state == OUTBOUND))
{
    u32					sgbase = sge[0].phys_addr;
    size_t				sglen = sge[0].length;
    struct mega_physical_disk_info_sas	disk;

    if (sglen > sizeof disk)
	sglen = sizeof disk;

    copyout (&disk, sglen, pid, sgbase);

//if (!((disk.device_id == 16) || (disk.device_id == 15)))
//    log = 0;

    copyin (&disk, sglen, pid, sgbase);
}

if ((f->opcode == 0x04010000) && (state == OUTBOUND))
{
    u32					sgbase = sge[0].phys_addr;
    size_t				sglen = sge[0].length;
    u8					config[4096];

    if (sglen > sizeof config)
	sglen = sizeof config;

    copyout (&config, sglen, pid, sgbase);

    {
	struct mega_array_header_sas		*header = (struct mega_array_header_sas *) config;
	struct mega_array_span_def_sas		*span = (struct mega_array_span_def_sas *) (header + 1);
	struct mega_array_disk_def_sas		*disk = (struct mega_array_disk_def_sas *) (span + header->num_span_defs);
	struct mega_array_hotspare_def_sas	*hotspare = (struct mega_array_hotspare_def_sas *) (disk + header->num_disk_defs);
	int					k;

	for (k = 0; k < header->num_disk_defs; ++k)
	{
//	    disk[k].state = 0xffff;
	}
    }

    copyin (&config, sglen, pid, sgbase);
}

if ((f->opcode == 0x05010000) && (state == OUTBOUND))
{
    u32					sgbase = sge[0].phys_addr;
    size_t				sglen = sge[0].length;
    struct mega_battery_state_sas	bs;

    if (sglen > sizeof bs)
	sglen = sizeof bs;

    copyout (&bs, sglen, pid, sgbase);

    if (getenv ("BATTERY_X"))
	bs.type = strtoul (getenv ("BATTERY_X"), NULL, 0);
    if (getenv ("BATTERY_Y"))
	bs.foo = strtoul (getenv ("BATTERY_Y"), NULL, 0);
    bs.over_charged = 1;

    copyin (&bs, sglen, pid, sgbase);
}

			    if (log)
			    {
				fprintf (stderr, "    host %d, off 0x%04x, count %d, sense_off 0x%08x, sense_len 0x%08x\n", iocp->host_no, iocp->sgl_off, iocp->sge_count, iocp->sense_off, iocp->sense_len);
				fprintf (stderr, "    DCMD: cmd_status %d, sge_count %d, context 0x%08x, flags 0x%04x, timeout %d, data_xfer_len 0x%x, opcode 0x%08x\n", f->cmd_status, f->sge_count, f->context, f->flags, f->timeout, f->data_xfer_len, f->opcode);
				dumpbytes (stderr, &f->mbox, sizeof (f->mbox), &f->mbox, "mbox");
			    }
			}
			else if (cmd == MFI_CMD_PD_SCSI_IO)
			{
			    struct megasas_pthru_frame	*f = (struct megasas_pthru_frame *) mh;

			    fprintf (stderr, "    host %d, off 0x%04x, count %d, sense_off 0x%08x, sense_len 0x%08x\n", iocp->host_no, iocp->sgl_off, iocp->sge_count, iocp->sense_off, iocp->sense_len);
			    fprintf (stderr, "    SCSI: cmd_status %d, sge_count %d, context 0x%08x, flags 0x%04x, timeout %d, data_xfer_len 0x%x\n", f->cmd_status, f->sge_count, f->context, f->flags, f->timeout, f->data_xfer_len);
			    fprintf (stderr, "        : scsi_status %d, target_id 0x%02x, lun %d, sense_len %d, sense_lo 0x%08x, sense_hi 0x%08x\n", f->scsi_status, f->target_id, f->lun, f->sense_len, f->sense_buf_phys_addr_lo, f->sense_buf_phys_addr_hi);

			    dumpbytes (stderr, &f->cdb, f->cdb_len, &f->cdb, "cdb");
			}
			else
			{
			    fprintf (stderr, "    host %d, off 0x%04x, count %d, sense_off 0x%08x, sense_len 0x%08x\n", iocp->host_no, iocp->sgl_off, iocp->sge_count, iocp->sense_off, iocp->sense_len);
			    dumpbytes (stderr, buf, len, (void *) r.edx, NULL);
			}
			if (log)
			{
			    for (k = 0; k < iocp->sge_count; ++k)
			    {
				u32			sgbase = sge[k].phys_addr;
				size_t			sglen = sge[k].length;
				char			sgname[16];
				char			sgdata[4096];

				snprintf (sgname, sizeof sgname, "sg%d", k);
				fprintf (stderr, "    %s at 0x%08x [0x%x]:\n", sgname, sgbase, sglen);
				if (sglen > sizeof sgdata)
				    sglen = sizeof sgdata;
				copyout (sgdata, sglen, pid, sgbase);
				dumpbytes (stderr, sgdata, sglen, (void *) sgbase, sgname);
			    }
			}
		    }
		    else
		    {
			dumpbytes (stderr, buf, len, (void *) r.edx, NULL);
		    }
		    fprintf (stderr, "\n");
		}

		switch (state)
		{
		    static u32		lasteip = 0;

		 case UNTRACED:
		    /* We don't know whether we were inbound or outbound on the first signal; this
		       appears to differ between kernels. So we defer until we see the same eip in
		       two successive traps, at which point we know we were outbound, so the next
		       trap is inbound. */
		    if (lasteip == r.eip)
			state = INBOUND;
		    lasteip = r.eip;
		    break;
		 case INBOUND:		state = OUTBOUND; break;
		 case OUTBOUND:		state = INBOUND; break;
		}

		if (ptrace (PTRACE_SYSCALL, pid, 0, 0) < 0)
		{
		    fprintf (stderr, "ptrace:syscall: %s\n", strerror (errno));
		    exit (1);
		}
	    }
	}
	else
	{
	    fprintf (stderr, "running\n");
	}
    }

    exit (0);
}


