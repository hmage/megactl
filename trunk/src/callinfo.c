/*
 *	Sysctl call data for ptrace(2).
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

#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/syscall.h>

#include	"callinfo.h"

#ifndef	SYS_pread
#define	SYS_pread	SYS_pread64
#endif

#ifndef	SYS_pwrite
#define	SYS_pwrite	SYS_pwrite64
#endif

struct callinfo		callinfo[] =
{
    { 0,			0,	NULL },
    { SYS_exit,			0,	"exit" },
    { SYS_fork,			0,	"fork" },
    { SYS_read,			0,	"read" },
    { SYS_write,		0,	"write" },
    { SYS_open,			0,	"open" },
    { SYS_close,		0,	"close" },
    { SYS_waitpid,		0,	"waitpid" },
    { SYS_creat,		0,	"creat" },
    { SYS_link,			0,	"link" },
    { SYS_unlink,		0,	"unlink" },
    { SYS_execve,		0,	"execve" },
    { SYS_chdir,		0,	"chdir" },
    { SYS_time,			0,	"time" },
    { SYS_mknod,		0,	"mknod" },
    { SYS_chmod,		0,	"chmod" },
    { SYS_lchown,		0,	"lchown" },
    { SYS_break,		0,	"break" },
    { SYS_oldstat,		0,	"oldstat" },
    { SYS_lseek,		0,	"lseek" },
    { SYS_getpid,		0,	"getpid" },
    { SYS_mount,		0,	"mount" },
    { SYS_umount,		0,	"umount" },
    { SYS_setuid,		0,	"setuid" },
    { SYS_getuid,		0,	"getuid" },
    { SYS_stime,		0,	"stime" },
    { SYS_ptrace,		1,	"ptrace" },
    { SYS_alarm,		0,	"alarm" },
    { SYS_oldfstat,		0,	"oldfstat" },
    { SYS_pause,		0,	"pause" },
    { SYS_utime,		0,	"utime" },
    { SYS_stty,			0,	"stty" },
    { SYS_gtty,			0,	"gtty" },
    { SYS_access,		0,	"access" },
    { SYS_nice,			0,	"nice" },
    { SYS_ftime,		0,	"ftime" },
    { SYS_sync,			0,	"sync" },
    { SYS_kill,			0,	"kill" },
    { SYS_rename,		0,	"rename" },
    { SYS_mkdir,		0,	"mkdir" },
    { SYS_rmdir,		0,	"rmdir" },
    { SYS_dup,			0,	"dup" },
    { SYS_pipe,			0,	"pipe" },
    { SYS_times,		0,	"times" },
    { SYS_prof,			0,	"prof" },
    { SYS_brk,			1,	"brk" },
    { SYS_setgid,		0,	"setgid" },
    { SYS_getgid,		0,	"getgid" },
    { SYS_signal,		0,	"signal" },
    { SYS_geteuid,		0,	"geteuid" },
    { SYS_getegid,		0,	"getegid" },
    { SYS_acct,			0,	"acct" },
    { SYS_umount2,		0,	"umount2" },
    { SYS_lock,			0,	"lock" },
    { SYS_ioctl,		0,	"ioctl" },
    { SYS_fcntl,		0,	"fcntl" },
    { SYS_mpx,			0,	"mpx" },
    { SYS_setpgid,		0,	"setpgid" },
    { SYS_ulimit,		0,	"ulimit" },
    { SYS_oldolduname,		0,	"oldolduname" },
    { SYS_umask,		0,	"umask" },
    { SYS_chroot,		0,	"chroot" },
    { SYS_ustat,		0,	"ustat" },
    { SYS_dup2,			0,	"dup2" },
    { SYS_getppid,		0,	"getppid" },
    { SYS_getpgrp,		0,	"getpgrp" },
    { SYS_setsid,		0,	"setsid" },
    { SYS_sigaction,		0,	"sigaction" },
    { SYS_sgetmask,		0,	"sgetmask" },
    { SYS_ssetmask,		0,	"ssetmask" },
    { SYS_setreuid,		0,	"setreuid" },
    { SYS_setregid,		0,	"setregid" },
    { SYS_sigsuspend,		0,	"sigsuspend" },
    { SYS_sigpending,		0,	"sigpending" },
    { SYS_sethostname,		0,	"sethostname" },
    { SYS_setrlimit,		0,	"setrlimit" },
    { SYS_getrlimit,		0,	"getrlimit" },
    { SYS_getrusage,		0,	"getrusage" },
    { SYS_gettimeofday,		0,	"gettimeofday" },
    { SYS_settimeofday,		0,	"settimeofday" },
    { SYS_getgroups,		0,	"getgroups" },
    { SYS_setgroups,		0,	"setgroups" },
    { SYS_select,		0,	"select" },
    { SYS_symlink,		0,	"symlink" },
    { SYS_oldlstat,		0,	"oldlstat" },
    { SYS_readlink,		0,	"readlink" },
    { SYS_uselib,		0,	"uselib" },
    { SYS_swapon,		0,	"swapon" },
    { SYS_reboot,		0,	"reboot" },
    { SYS_readdir,		0,	"readdir" },
    { SYS_mmap2,		1,	"mmap2" },
    { SYS_munmap,		0,	"munmap" },
    { SYS_truncate,		0,	"truncate" },
    { SYS_ftruncate,		0,	"ftruncate" },
    { SYS_fchmod,		0,	"fchmod" },
    { SYS_fchown,		0,	"fchown" },
    { SYS_getpriority,		0,	"getpriority" },
    { SYS_setpriority,		0,	"setpriority" },
    { SYS_profil,		0,	"profil" },
    { SYS_statfs,		0,	"statfs" },
    { SYS_fstatfs,		0,	"fstatfs" },
    { SYS_ioperm,		0,	"ioperm" },
    { SYS_socketcall,		0,	"socketcall" },
    { SYS_syslog,		0,	"syslog" },
    { SYS_setitimer,		0,	"setitimer" },
    { SYS_getitimer,		0,	"getitimer" },
    { SYS_stat,			0,	"stat" },
    { SYS_lstat,		0,	"lstat" },
    { SYS_fstat,		0,	"fstat" },
    { SYS_olduname,		0,	"olduname" },
    { SYS_iopl,			0,	"iopl" },
    { SYS_vhangup,		0,	"vhangup" },
    { SYS_idle,			0,	"idle" },
    { SYS_vm86old,		0,	"vm86old" },
    { SYS_wait4,		0,	"wait4" },
    { SYS_swapoff,		0,	"swapoff" },
    { SYS_sysinfo,		0,	"sysinfo" },
    { SYS_ipc,			0,	"ipc" },
    { SYS_fsync,		0,	"fsync" },
    { SYS_sigreturn,		0,	"sigreturn" },
    { SYS_clone,		0,	"clone" },
    { SYS_setdomainname,	0,	"setdomainname" },
    { SYS_uname,		0,	"uname" },
    { SYS_modify_ldt,		0,	"modify_ldt" },
    { SYS_adjtimex,		0,	"adjtimex" },
    { SYS_mprotect,		0,	"mprotect" },
    { SYS_sigprocmask,		0,	"sigprocmask" },
    { SYS_create_module,	0,	"create_module" },
    { SYS_init_module,		0,	"init_module" },
    { SYS_delete_module,	0,	"delete_module" },
    { SYS_get_kernel_syms,	0,	"get_kernel_syms" },
    { SYS_quotactl,		0,	"quotactl" },
    { SYS_getpgid,		0,	"getpgid" },
    { SYS_fchdir,		0,	"fchdir" },
    { SYS_bdflush,		0,	"bdflush" },
    { SYS_sysfs,		0,	"sysfs" },
    { SYS_personality,		0,	"personality" },
    { SYS_afs_syscall,		0,	"afs_syscall" },
    { SYS_setfsuid,		0,	"setfsuid" },
    { SYS_setfsgid,		0,	"setfsgid" },
    { SYS__llseek,		0,	"_llseek" },
    { SYS_getdents,		0,	"getdents" },
    { SYS__newselect,		0,	"_newselect" },
    { SYS_flock,		0,	"flock" },
    { SYS_msync,		0,	"msync" },
    { SYS_readv,		0,	"readv" },
    { SYS_writev,		0,	"writev" },
    { SYS_getsid,		0,	"getsid" },
    { SYS_fdatasync,		0,	"fdatasync" },
    { SYS__sysctl,		0,	"_sysctl" },
    { SYS_mlock,		0,	"mlock" },
    { SYS_munlock,		0,	"munlock" },
    { SYS_mlockall,		0,	"mlockall" },
    { SYS_munlockall,		0,	"munlockall" },
    { SYS_sched_setparam,	0,	"sched_setparam" },
    { SYS_sched_getparam,	0,	"sched_getparam" },
    { SYS_sched_setscheduler,	0,	"sched_setscheduler" },
    { SYS_sched_getscheduler,	0,	"sched_getscheduler" },
    { SYS_sched_yield,		0,	"sched_yield" },
    { SYS_sched_get_priority_max,	0,	"sched_get_priority_max" },
    { SYS_sched_get_priority_min,	0,	"sched_get_priority_min" },
    { SYS_sched_rr_get_interval,	0,	"sched_rr_get_interval" },
    { SYS_nanosleep,		0,	"nanosleep" },
    { SYS_mremap,		0,	"mremap" },
    { SYS_setresuid,		0,	"setresuid" },
    { SYS_getresuid,		0,	"getresuid" },
    { SYS_vm86,			0,	"vm86" },
    { SYS_query_module,		0,	"query_module" },
    { SYS_poll,			0,	"poll" },
    { SYS_nfsservctl,		0,	"nfsservctl" },
    { SYS_setresgid,		0,	"setresgid" },
    { SYS_getresgid,		0,	"getresgid" },
    { SYS_prctl,		0,	"prctl" },
    { SYS_rt_sigreturn,		0,	"rt_sigreturn" },
    { SYS_rt_sigaction,		0,	"rt_sigaction" },
    { SYS_rt_sigprocmask,	0,	"rt_sigprocmask" },
    { SYS_rt_sigpending,	0,	"rt_sigpending" },
    { SYS_rt_sigtimedwait,	0,	"rt_sigtimedwait" },
    { SYS_rt_sigqueueinfo,	0,	"rt_sigqueueinfo" },
    { SYS_rt_sigsuspend,	0,	"rt_sigsuspend" },
    { SYS_pread,		0,	"pread" },
    { SYS_pwrite,		0,	"pwrite" },
    { SYS_chown,		0,	"chown" },
    { SYS_getcwd,		0,	"getcwd" },
    { SYS_capget,		0,	"capget" },
    { SYS_capset,		0,	"capset" },
    { SYS_sigaltstack,		0,	"sigaltstack" },
    { SYS_sendfile,		0,	"sendfile" },
    { SYS_getpmsg,		0,	"getpmsg" },
    { SYS_putpmsg,		0,	"putpmsg" },
    { SYS_vfork,		0,	"vfork" },
    { SYS_ugetrlimit,		0,	"ugetrlimit" },
    { SYS_mmap2,		1,	"mmap2" },
    { SYS_truncate64,		0,	"truncate64" },
    { SYS_ftruncate64,		0,	"ftruncate64" },
    { SYS_stat64,		0,	"stat64" },
    { SYS_lstat64,		0,	"lstat64" },
    { SYS_fstat64,		0,	"fstat64" },
    { SYS_lchown32,		0,	"lchown32" },
    { SYS_getuid32,		0,	"getuid32" },
    { SYS_getgid32,		0,	"getgid32" },
    { SYS_geteuid32,		0,	"geteuid32" },
    { SYS_getegid32,		0,	"getegid32" },
    { SYS_setreuid32,		0,	"setreuid32" },
    { SYS_setregid32,		0,	"setregid32" },
    { SYS_getgroups32,		0,	"getgroups32" },
    { SYS_setgroups32,		0,	"setgroups32" },
    { SYS_fchown32,		0,	"fchown32" },
    { SYS_setresuid32,		0,	"setresuid32" },
    { SYS_getresuid32,		0,	"getresuid32" },
    { SYS_setresgid32,		0,	"setresgid32" },
    { SYS_getresgid32,		0,	"getresgid32" },
    { SYS_chown32,		0,	"chown32" },
    { SYS_setuid32,		0,	"setuid32" },
    { SYS_setgid32,		0,	"setgid32" },
    { SYS_setfsuid32,		0,	"setfsuid32" },
    { SYS_setfsgid32,		0,	"setfsgid32" },
    { SYS_pivot_root,		0,	"pivot_root" },
    { SYS_mincore,		0,	"mincore" },
    { SYS_madvise1,		0,	"madvise1" },
    { SYS_getdents64,		0,	"getdents64" },
    { SYS_fcntl64,		0,	"fcntl64" },
    { 222,			0,	NULL },
    { 223 /* SYS_security */,	0,	NULL /* "security" */ },
    { SYS_gettid,		0,	"gettid" },
    { SYS_readahead,		0,	"readahead" },
    { SYS_setxattr,		0,	"setxattr" },
    { SYS_lsetxattr,		0,	"lsetxattr" },
    { SYS_fsetxattr,		0,	"fsetxattr" },
    { SYS_getxattr,		0,	"getxattr" },
    { SYS_lgetxattr,		0,	"lgetxattr" },
    { SYS_fgetxattr,		0,	"fgetxattr" },
    { SYS_listxattr,		0,	"listxattr" },
    { SYS_llistxattr,		0,	"llistxattr" },
    { SYS_flistxattr,		0,	"flistxattr" },
    { SYS_removexattr,		0,	"removexattr" },
    { SYS_lremovexattr,		0,	"lremovexattr" },
    { SYS_fremovexattr,		0,	"fremovexattr" },
    { SYS_tkill,		0,	"tkill" },
    { SYS_sendfile64,		0,	"sendfile64" },
    { SYS_futex,		0,	"futex" },
    { SYS_sched_setaffinity,	0,	"sched_setaffinity" },
    { SYS_sched_getaffinity,	0,	"sched_getaffinity" },
#ifdef	SYS_set_thread_area
    { SYS_set_thread_area,	0,	"set_thread_area " },
#else
    { 243,			0,	NULL },
#endif
#ifdef	SYS_get_thread_area
    { SYS_get_thread_area,	0,	"get_thread_area" },
#else
    { 244,			0,	NULL },
#endif
#ifdef	SYS_io_setup
    { SYS_io_setup,		0,	"io_setup" },
#else
    { 245,			0,	NULL },
#endif
#ifdef	SYS_io_destroy
    { SYS_io_destroy,		0,	"io_destroy" },
#else
    { 246,			0,	NULL },
#endif
#ifdef	SYS_io_getevents
    { SYS_io_getevents,		0,	"io_getevents" },
#else
    { 247,			0,	NULL },
#endif
#ifdef	SYS_io_submit
    { SYS_io_submit,		0,	"io_submit" },
#else
    { 248,			0,	NULL },
#endif
#ifdef	SYS_io_cancel
    { SYS_io_cancel,		0,	"io_cancel" },
#else
    { 249,			0,	NULL },
#endif
#ifdef	SYS_fadvise64
    { SYS_fadvise64,		0,	"fadvise64" },
#else
    { 250,			0,	NULL },
#endif
    { 251,			0,	NULL },
#ifdef	SYS_exit_group
    { SYS_exit_group,		0,	"exit_group" },
#else
    { 252,			0,	NULL },
#endif
#ifdef	SYS_lookup_dcookie
    { SYS_lookup_dcookie,	0,	"lookup_dcookie" },
#else
    { 253,			0,	NULL },
#endif
#ifdef	SYS_epoll_create
    { SYS_epoll_create,		0,	"epoll_create" },
#else
    { 254,			0,	NULL },
#endif
#ifdef	SYS_epoll_ctl
    { SYS_epoll_ctl,		0,	"epoll_ctl" },
#else
    { 255,			0,	NULL },
#endif
#ifdef	SYS_epoll_wait
    { SYS_epoll_wait,		0,	"epoll_wait" },
#else
    { 256,			0,	NULL },
#endif
#ifdef	SYS_remap_file_pages
    { SYS_remap_file_pages,	0,	"remap_file_pages" },
#else
    { 257,			0,	NULL },
#endif
#ifdef	SYS_set_tid_address
    { SYS_set_tid_address,	0,	"set_tid_address" },
#else
    { 258,			0,	NULL },
#endif
#ifdef	SYS_timer_create
    { SYS_timer_create,		0,	"timer_create" },
#else
    { 259,			0,	NULL },
#endif
#ifdef	SYS_timer_settime
    { SYS_timer_settime,	0,	"timer_settime" },
#else
    { 260,			0,	NULL },
#endif
#ifdef	SYS_timer_gettime
    { SYS_timer_gettime,	0,	"timer_gettime" },
#else
    { 261,			0,	NULL },
#endif
#ifdef	SYS_timer_getoverrun
    { SYS_timer_getoverrun,	0,	"timer_getoverrun" },
#else
    { 262,			0,	NULL },
#endif
#ifdef	SYS_timer_delete
    { SYS_timer_delete,		0,	"timer_delete" },
#else
    { 263,			0,	NULL },
#endif
#ifdef	SYS_clock_settime
    { SYS_clock_settime,	0,	"clock_settime" },
#else
    { 264,			0,	NULL },
#endif
#ifdef	SYS_clock_gettime
    { SYS_clock_gettime,	0,	"clock_gettime" },
#else
    { 265,			0,	NULL },
#endif
#ifdef	SYS_clock_getres
    { SYS_clock_getres,		0,	"clock_getres" },
#else
    { 266,			0,	NULL },
#endif
#ifdef	SYS_clock_nansleep
    { SYS_clock_nanosleep,	0,	"clock_nanosleep" },
#else
    { 267,			0,	NULL },
#endif
    { 268,			0,	NULL },
    { 269,			0,	NULL },
#ifdef	SYS_tgkill
    { SYS_tgkill,		0,	"tgkill" },
#else
    { 270,			0,	NULL },
#endif
};

int		callmax = sizeof (callinfo) / sizeof (callinfo[0]);

