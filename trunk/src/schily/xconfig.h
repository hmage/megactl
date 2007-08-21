/* xconfig.h.  Generated automatically by configure.  */
/* @(#)xconfig.h.in	1.76 04/07/26 Copyright 1998-2003 J. Schilling */
/*
 *	Dynamic autoconf C-include code.
 *
 *	Copyright (c) 1998-2003 J. Schilling
 */

/*
 * Header Files
 */
#define PROTOTYPES 1	/* if Compiler supports ANSI C prototypes */
#define HAVE_STDARG_H 1	/* to use stdarg.h, else use varargs.h NOTE: SaberC on a Sun has prototypes but no stdarg.h */
/* #undef HAVE_VARARGS_H */	/* to use use varargs.h NOTE: The free HP-UX C-compiler has stdarg.h but no PROTOTYPES */
#define HAVE_STDLIB_H 1	/* to use general utility defines (malloc(), size_t ...) and general C library prototypes */
#define HAVE_STDDEF_H 1	/* to use offsetof(), ptrdiff_t, wchar>t, size_t */
#define HAVE_STRING_H 1	/* to get NULL and ANSI C string function prototypes */
#define HAVE_STRINGS_H 1	/* to get BSD string function prototypes */
#define STDC_HEADERS 1	/* if ANSI compliant stdlib.h, stdarg.h, string.h, float.h are present */
#define HAVE_UNISTD_H 1	/* to get POSIX syscall prototypes XXX sys/file.h fcntl.h (unixstd/fctl)XXX*/
#define HAVE_GETOPT_H 1	/* to get getopt() prototype from getopt.h instead of unistd.h */
#define HAVE_LIMITS_H 1	/* to get POSIX numeric limits constants */
#define HAVE_A_OUT_H 1	/* if a.out.h is present (may be a system using a.out format) */
/* #undef HAVE_AOUTHDR_H */	/* if aouthdr.h is present. This is a COFF system */
#define HAVE_ELF_H 1	/* if elf.h is present. This is an ELF system */
#define HAVE_FCNTL_H 1	/* to access, O_XXX constants for open(), otherwise use sys/file.h */
#define HAVE_SYS_FILE_H 1	/* to use O_XXX constants for open() and flock() defs */
#define HAVE_INTTYPES_H 1	/* to use UNIX-98 inttypes.h */
#define HAVE_DIRENT_H 1	/* to use POSIX dirent.h */
/* #undef HAVE_SYS_DIR_H */	/* to use BSD sys/dir.h */
/* #undef HAVE_NDIR_H */	/* to use ndir.h */
/* #undef HAVE_SYS_NDIR_H */	/* to use sys/ndir.h */
#define HAVE_ALLOCA_H 1	/* if alloca.h exists */
#define HAVE_MALLOC_H 1	/* if malloc.h exists */
#define HAVE_TERMIOS_H 1	/* to use POSIX termios.h */
#define HAVE_TERMIO_H 1	/* to use SVR4 termio.h */
#define HAVE_PWD_H 1	/* if pwd.h exists */
#define HAVE_GRP_H 1	/* if grp.h exists */
#define HAVE_SYS_ACL_H 1	/* to use <sys/acl.h> for ACL definitions */
/* #undef HAVE_ACLLIB_H */	/* if HP-UX <acllib.h> is present */
#define HAVE_SHADOW_H 1	/* if shadow.h exists */
#define HAVE_SYSLOG_H 1	/* if syslog.h exists */
#define HAVE_SYS_TIME_H 1	/* may include sys/time.h for struct timeval */
#define TIME_WITH_SYS_TIME 1	/* may include both time.h and sys/time.h */
#define HAVE_TIMES 1	/* to use times() and sys/times.h */
#define HAVE_SYS_TIMES_H 1	/* may include sys/times.h for struct tms */
#define HAVE_UTIME 1		/* to use AT&T utime() and utimbuf */
#define HAVE_UTIMES 1		/* to use BSD utimes() and sys/time.h */
#define HAVE_UTIME_H 1		/* to use utime.h for the utimbuf structure declaration, else declare struct utimbuf yourself */
/* #undef HAVE_SYS_UTIME_H */		/* to use sys/utime.h if utime.h does not exist */
#define HAVE_SYS_IOCTL_H 1		/* if sys/ioctl.h is present */
/* #undef HAVE_SYS_FILIO_H */		/* if sys/ioctl.h is present */
#define HAVE_SYS_PARAM_H 1		/* if sys/param.h is present */
#define HAVE_MNTENT_H 1		/* if mntent.h is present */
/* #undef HAVE_SYS_MNTENT_H */	/* if sys/mntent.h is present */
/* #undef HAVE_SYS_MNTTAB_H */	/* if sys/mnttab.h is present */
#define HAVE_SYS_MOUNT_H 1		/* if sys/mount.h is present */
#define HAVE_WAIT_H 1		/* to use wait.h for prototypes and union wait */
#define HAVE_SYS_WAIT_H 1		/* else use sys/wait.h */
#define HAVE_SYS_RESOURCE_H 1	/* to use sys/resource.h for rlimit() and wait3() */
#define HAVE_SYS_PROCFS_H 1	/* to use sys/procfs.h for wait3() emulation */
/* #undef HAVE_SYS_SYSTEMINFO_H */	/* to use SVr4 sysinfo() */
#define HAVE_SYS_UTSNAME_H 1	/* to use uname() */
/* #undef HAVE_SYS_PRIOCNTL_H */	/* to use SVr4 priocntl() instead of nice()/setpriority() */
/* #undef HAVE_SYS_RTPRIOCNTL_H */	/* if the system supports SVr4 real time classes */
#define HAVE_SYS_SYSCALL_H 1	/* to use syscall() */
#define HAVE_SYS_MTIO_H 1		/* to use mtio definitions from sys/mtio.h */
/* #undef HAVE_SYS_TAPE_H */		/* to use mtio definitions from AIX sys/tape.h */
#define HAVE_SYS_MMAN_H 1		/* to use definitions for mmap()/madvise()... from sys/mman.h */
#define HAVE_SYS_SHM_H 1		/* to use definitions for shmget() ... from sys/shm.h */
#define HAVE_SYS_IPC_H 1		/* to use definitions for shmget() ... from sys/ipc.h */
/* #undef MAJOR_IN_MKDEV */		/* if we should include sys/mkdev.h to get major()/minor()/makedev() */
#define MAJOR_IN_SYSMACROS 1	/* if we should include sys/sysmacros.h to get major()/minor()/makedev() */
/* #undef HAVE_SYS_DKIO_H */		/* if we may include sys/dkio.h for disk ioctls */
/* #undef HAVE_SYS_DKLABEL_H */	/* if we may include sys/dklabel.h for disk label */
/* #undef HAVE_SUN_DKIO_H */		/* if we may include sun/dkio.h for disk ioctls */
/* #undef HAVE_SUN_DKLABEL_H */	/* if we may include sun/dklabel.h for disk label */
#define HAVE_SYS_TYPES_H 1		/* if we may include sys/types.h (the standard) */
#define HAVE_SYS_STAT_H 1		/* if we may include sys/stat.h (the standard) */
/* #undef HAVE_TYPES_H */		/* if we may include types.h (rare cases e.g. ATARI TOS) */
/* #undef HAVE_STAT_H */		/* if we may include stat.h (rare cases e.g. ATARI TOS) */
#define HAVE_POLL_H 1		/* if we may include poll.h to use poll() */
#define HAVE_SYS_POLL_H 1		/* if we may include sys/poll.h to use poll() */
#define HAVE_SYS_SELECT_H 1	/* if we may have sys/select.h nonstandard use for select() on some systems*/
/* #undef NEED_SYS_SELECT_H */	/* if we need sys/select.h to use select() (this is nonstandard) */
#define HAVE_NETDB_H 1		/* if we have netdb.h for get*by*() and rcmd() */
#define HAVE_SYS_SOCKET_H 1	/* if we have sys/socket.h for socket() */
/* #undef NEED_SYS_SOCKET_H */	/* if we need sys/socket.h to use select() (this is nonstandard) */
#define HAVE_LINUX_PG_H 1		/* if we may include linux/pg.h for PP ATAPI sypport */
/* #undef HAVE_CAMLIB_H */		/* if we may include camlib.h for CAM SCSI transport definitions */
/* #undef HAVE_IEEEFP_H */		/* if we may include ieeefp.h for finite()/isnand() */
/* #undef HAVE_FP_H */		/* if we may include fp.h for FINITE()/IS_INF()/IS_NAN() */
#define HAVE_VALUES_H 1		/* if we may include values.h for MAXFLOAT */
#define HAVE_FLOAT_H 1		/* if we may include float.h for FLT_MAX */
/* #undef HAVE__FILBUF */		/* if we have _filbuf() for USG derived STDIO */
/* #undef HAVE___FILBUF */		/* if we have __filbuf() for USG derived STDIO */
/* #undef HAVE_USG_STDIO */		/* if we have USG derived STDIO */
#define HAVE_ERRNO_DEF 1		/* if we have errno definition in <errno.h> */
/* #undef HAVE_VFORK_H */		/* if we should include vfork.h for vfork() definitions */
#define HAVE_ARPA_INET_H 1		/* if we have arpa/inet.h (missing on BeOS) */
				/* BeOS has inet_ntoa() in <netdb.h> */
/* #undef HAVE_BSD_DEV_SCSIREG_H */	/* if we have a NeXT Step compatible sg driver */
/* #undef HAVE_SYS_BSDTTY_H */	/* if we have sys/bsdtty.h on HP-UX for TIOCGPGRP */
/* #undef HAVE_OS_H */		/* if we have the BeOS kernel definitions in OS.h */
#define HAVE_ATTR_XATTR_H 1	/* if we have the Linux Extended File Attr definitions in attr/xattr.h */
#define HAVE_FNMATCH_H 1		/* if we may include fnmatch.h */

/*
 * Convert to SCHILY name
 */
#ifdef	STDC_HEADERS
#	ifndef	HAVE_STDC_HEADERS
#		define	HAVE_STDC_HEADERS
#	endif
#endif

#ifdef	HAVE_ELF_H
#define	HAVE_ELF			/* This system uses ELF */
#else
#	ifdef	HAVE_AOUTHDR_H
#	define	HAVE_COFF		/* This system uses COFF */
#	else
#		ifdef HAVE_A_OUT_H
#		define HAVE_AOUT	/* This system uses AOUT */
#		endif
#	endif
#endif

/*
 * Library Functions
 */
#define HAVE_ACCESS 1		/* access() is present in libc */
/* #undef HAVE_EACCESS */		/* eaccess() is present in libc */
#define HAVE_EUIDACCESS 1		/* euidaccess() is present in libc */
/* #undef HAVE_ACCESS_E_OK */		/* access() implements E_OK (010) for effective UIDs */
#define HAVE_CRYPT 1		/* crypt() is present in libc or libcrypt */
#define HAVE_STRERROR 1		/* strerror() is present in libc */
#define HAVE_MEMMOVE 1		/* memmove() is present in libc */
#define HAVE_MADVISE 1		/* madvise() is present in libc */
#define HAVE_MLOCK 1		/* mlock() is present in libc */
#define HAVE_MLOCKALL 1		/* working mlockall() is present in libc */
#define HAVE_MMAP 1		/* working mmap() is present in libc */
/* #undef _MMAP_WITH_SIZEP */		/* mmap() needs address of size parameter */
#define HAVE_FLOCK 1		/* *BSD flock() is present in libc */
#define HAVE_LOCKF 1		/* lockf() is present in libc (XOPEN) */
#define HAVE_FCNTL_LOCKF 1		/* file locking via fcntl() is present in libc */
#define HAVE_FCHDIR 1		/* fchdir() is present in libc */
#define HAVE_STATVFS 1		/* statvfs() is present in libc */
#define HAVE_QUOTACTL 1		/* quotactl() is present in libc */
/* #undef HAVE_QUOTAIOCTL */		/* use ioctl(f, Q_QUOTACTL, &q) instead of quotactl() */
#define HAVE_SETREUID 1		/* setreuid() is present in libc */
#define HAVE_SETRESUID 1		/* setresuid() is present in libc */
#define HAVE_SETEUID 1		/* seteuid() is present in libc */
#define HAVE_SETUID 1		/* setuid() is present in libc */
#define HAVE_SETREGID 1		/* setregid() is present in libc */
#define HAVE_SETRESGID 1		/* setresgid() is present in libc */
#define HAVE_SETEGID 1		/* setegid() is present in libc */
#define HAVE_SETGID 1		/* setgid() is present in libc */
#define HAVE_GETPGID 1		/* getpgid() is present in libc (POSIX) */
#define HAVE_SETPGID 1		/* setpgid() is present in libc (POSIX) */
#define HAVE_GETPGRP 1		/* getpgrp() is present in libc (ANY) */
#define HAVE_SETPGRP 1		/* setpgrp() is present in libc (ANY) */
/* #undef HAVE_BSD_GETPGRP */		/* getpgrp() in libc is BSD-4.2 compliant */
/* #undef HAVE_BSD_SETPGRP */		/* setpgrp() in libc is BSD-4.2 compliant */
#define HAVE_GETSPNAM 1		/* getspnam() in libc (SVR4 compliant) */
/* #undef HAVE_GETSPWNAM */		/* getspwnam() in libsec.a (HP-UX) */
#define HAVE_SYNC 1		/* sync() is present in libc */
#define HAVE_FSYNC 1		/* fsync() is present in libc */
#define HAVE_TCGETATTR 1		/* tcgetattr() is present in libc */
#define HAVE_TCSETATTR 1		/* tcsetattr() is present in libc */
#define HAVE_WAIT3 1		/* working wait3() is present in libc */
#define HAVE_WAIT4 1		/* wait4() is present in libc */
#define HAVE_WAITID 1		/* waitid() is present in libc */
#define HAVE_WAITPID 1		/* waitpid() is present in libc */
#define HAVE_GETHOSTID 1		/* gethostid() is present in libc */
#define HAVE_GETHOSTNAME 1		/* gethostname() is present in libc */
#define HAVE_GETDOMAINNAME 1	/* getdomainname() is present in libc */
#define HAVE_GETPAGESIZE 1		/* getpagesize() is present in libc */
#define HAVE_GETDTABLESIZE 1	/* getdtablesize() is present in libc */
#define HAVE_GETRUSAGE 1		/* getrusage() is present in libc */
#define HAVE_GETRLIMIT 1		/* getrlimit() is present in libc */
#define HAVE_SETRLIMIT 1		/* setrlimit() is present in libc */
#define HAVE_ULIMIT 1		/* ulimit() is present in libc */
#define HAVE_GETTIMEOFDAY 1	/* gettimeofday() is present in libc */
#define HAVE_SETTIMEOFDAY 1	/* settimeofday() is present in libc */
#define HAVE_TIME 1		/* time() is present in libc */
#define HAVE_STIME 1		/* stime() is present in libc */
#define HAVE_POLL 1		/* poll() is present in libc */
#define HAVE_SELECT 1		/* select() is present in libc */
#define HAVE_CHOWN 1		/* chown() is present in libc */
#define HAVE_LCHOWN 1		/* lchown() is present in libc */
#define HAVE_BRK 1			/* brk() is present in libc */
#define HAVE_SBRK 1		/* sbrk() is present in libc */
#define HAVE_VA_COPY 1		/* va_copy() is present in varargs.h/stdarg.h */
#define HAVE__VA_COPY 1		/* __va_copy() is present in varargs.h/stdarg.h */
/* #undef HAVE_DTOA */		/* BSD-4.4 __dtoa() is present in libc */
/* #undef HAVE_DTOA_R */		/* BSD-4.4 __dtoa() with result ptr (reentrant) */
#define HAVE_DUP2 1		/* dup2() is present in libc */
#define HAVE_GETCWD 1		/* POSIX getcwd() is present in libc */
#define HAVE_SMMAP 1		/* may map anonymous memory to get shared mem */
#define HAVE_SHMAT 1		/* shmat() is present in libc */
#define HAVE_SEMGET 1		/* semget() is present in libc */
#define HAVE_LSTAT 1		/* lstat() is present in libc */
#define HAVE_READLINK 1		/* readlink() is present in libc */
#define HAVE_SYMLINK 1		/* symlink() is present in libc */
#define HAVE_LINK 1		/* link() is present in libc */
#define HAVE_HARD_SYMLINKS 1	/* link() allows hard links on symlinks */
#define HAVE_LINK_NOFOLLOW 1	/* link() does not follow symlinks when hard linking symlinks */
#define HAVE_RENAME 1		/* rename() is present in libc */
#define HAVE_MKFIFO 1		/* mkfifo() is present in libc */
#define HAVE_MKNOD 1		/* mknod() is present in libc */
#define HAVE_ECVT 1		/* ecvt() is present in libc */
#define HAVE_FCVT 1		/* fcvt() is present in libc */
#define HAVE_GCVT 1		/* gcvt() is present in libc */
#define HAVE_ECVT_R 1		/* ecvt_r() is present in libc */
#define HAVE_FCVT_R 1		/* fcvt_r() is present in libc */
/* #undef HAVE_GCVT_R */		/* gcvt_r() is present in libc */
/* #undef HAVE_ECONVERT */		/* econvert() is present in libc */
/* #undef HAVE_FCONVERT */		/* fconvert() is present in libc */
/* #undef HAVE_GCONVERT */		/* gconvert() is present in libc */
#define HAVE_ISINF 1		/* isinf() is present in libc */
#define HAVE_ISNAN 1		/* isnan() is present in libc */
#define HAVE_RAND 1		/* rand() is present in libc */
#define HAVE_DRAND48 1		/* drand48() is present in libc */
#define HAVE_SETPRIORITY 1		/* setpriority() is present in libc */
#define HAVE_NICE 1		/* nice() is present in libc */
/* #undef HAVE_DOSSETPRIORITY */	/* DosSetPriority() is present in libc */
/* #undef HAVE_DOSALLOCSHAREDMEM */	/* DosAllocSharedMem() is present in libc */
#define HAVE_SEEKDIR 1		/* seekdir() is present in libc */
#define HAVE_PUTENV 1		/* putenv() is present in libc (preferred function) */
#define HAVE_SETENV 1		/* setenv() is present in libc (use instead of putenv()) */
#define HAVE_UNAME 1		/* uname() is present in libc */
#define HAVE_SNPRINTF 1		/* snprintf() is present in libc */
#define HAVE_STRCASECMP 1		/* strcasecmp() is present in libc */
#define HAVE_STRDUP 1		/* strdup() is present in libc */
#define HAVE_STRSIGNAL 1		/* strsignal() is present in libc */
/* #undef HAVE_STR2SIG */		/* str2sig() is present in libc */
/* #undef HAVE_SIG2STR */		/* sig2str() is present in libc */
#define HAVE_KILLPG 1		/* killpg() is present in libc */
#define HAVE_SIGRELSE 1		/* sigrelse() is present in libc */
#define HAVE_SIGPROCMASK 1		/* sigprocmask() is present in libc (POSIX) */
#define HAVE_SIGSETMASK 1		/* sigsetmask() is present in libc (BSD) */
#define HAVE_SIGSET 1		/* sigset() is present in libc (POSIX) */
#define HAVE_SYS_SIGLIST 1		/* char *sys_siglist[] is present in libc */
#define HAVE_NANOSLEEP 1		/* nanosleep() is present in libc */
#define HAVE_USLEEP 1		/* usleep() is present in libc */
#define HAVE_FORK 1		/* fork() is present in libc */
#define HAVE_EXECL 1		/* execl() is present in libc */
#define HAVE_EXECLE 1		/* execle() is present in libc */
#define HAVE_EXECLP 1		/* execlp() is present in libc */
#define HAVE_EXECV 1		/* execv() is present in libc */
#define HAVE_EXECVE 1		/* execve() is present in libc */
#define HAVE_EXECVP 1		/* execvp() is present in libc */
#define HAVE_ALLOCA 1		/* alloca() is present (else use malloc())*/
#define HAVE_VALLOC 1		/* valloc() is present in libc (else use malloc())*/
/* #undef vfork */

/* #undef HAVE_CHFLAGS */		/* chflags() is present in libc */
/* #undef HAVE_FCHFLAGS */		/* fchflags() is present in libc */
/* #undef HAVE_FFLAGSTOSTR */		/* fflagstostr() is present in libc */
/* #undef HAVE_STRTOFFLAGS */		/* strtofflags() is present in libc */

#define HAVE_FNMATCH 1		/* fnmatch() is present in libc */

/*
 * Linux Extended File Attributes
 */
#define HAVE_GETXATTR 1		/* getxattr()				*/
#define HAVE_SETXATTR 1		/* setxattr()				*/
#define HAVE_LISTXATTR 1		/* listxattr()				*/

/*
 * Important:	This must be a result from a check _before_ the Large File test
 *		has been run. It then tells us whether these functions are
 *		available even when not in Large File mode.
 *
 *	Do not run the AC_FUNC_FSEEKO test from the GNU tar Large File test
 *	siute. It will use the same cache names and interfere with our test.
 *	Instead use the tests AC_SMALL_FSEEKO/AC_SMALL/STELLO and make sure
 *	they are placed before the large file tests.
 */
/* #undef HAVE_FSEEKO */		/* fseeko() is present in default compile mode */
/* #undef HAVE_FTELLO */		/* ftello() is present in default compile mode */

#define HAVE_RCMD 1		/* rcmd() is present in libc/libsocket */
#define HAVE_SOCKET 1		/* socket() is present in libc/libsocket */
#define HAVE_SOCKETPAIR 1		/* socketpair() is present in libc/libsocket */
#define HAVE_GETSERVBYNAME 1	/* getservbyname() is present in libc/libsocket */
#define HAVE_INET_NTOA 1		/* inet_ntoa() is present in libc/libsocket */
#define HAVE_GETADDRINFO 1		/* getaddrinfo() is present in libc/libsocket */
#define HAVE_GETNAMEINFO 1		/* getnameinfo() is present in libc/libsocket */

#if	defined(HAVE_QUOTACTL) || defined(HAVE_QUOTAIOCTL)
#	define HAVE_QUOTA	/* The system inludes quota */
#endif

/*
 * We need to test for the include files too because Apollo Domain/OS has a
 * libc that includes the functions but the includes files are not visible
 * from the BSD compile environment.
 */
#if	defined(HAVE_SHMAT) && defined(HAVE_SYS_SHM_H) && defined(HAVE_SYS_IPC_H)
#	define	HAVE_USGSHM	/* USG shared memory is present */
#endif
#if	defined(HAVE_SEMGET) && defined(HAVE_SYS_SHM_H) && defined(HAVE_SYS_IPC_H)
#	define	HAVE_USGSEM	/* USG semaphores are present */
#endif

#if	defined(HAVE_GETPGRP) && !defined(HAVE_BSD_GETPGRP)
#define	HAVE_POSIX_GETPGRP 1	/* getpgrp() in libc is POSIX compliant */
#endif
#if	defined(HAVE_SETPGRP) && !defined(HAVE_BSD_SETPGRP)
#define	HAVE_POSIX_SETPGRP 1	/* setpgrp() in libc is POSIX compliant */
#endif

/*
 * Structures
 */
#define HAVE_MTGET_TYPE 1		/* if struct mtget contains mt_type (drive type) */
/* #undef HAVE_MTGET_MODEL */		/* if struct mtget contains mt_model (drive type) */
#define HAVE_MTGET_DSREG 1		/* if struct mtget contains mt_dsreg (drive status) */
/* #undef HAVE_MTGET_DSREG1 */	/* if struct mtget contains mt_dsreg1 (drive status msb) */
/* #undef HAVE_MTGET_DSREG2 */	/* if struct mtget contains mt_dsreg2 (drive status lsb) */
#define HAVE_MTGET_GSTAT 1		/* if struct mtget contains mt_gstat (generic status) */
#define HAVE_MTGET_ERREG 1		/* if struct mtget contains mt_erreg (error register) */
#define HAVE_MTGET_RESID 1		/* if struct mtget contains mt_resid (residual count) */
#define HAVE_MTGET_FILENO 1	/* if struct mtget contains mt_fileno (file #) */
#define HAVE_MTGET_BLKNO 1		/* if struct mtget contains mt_blkno (block #) */
/* #undef HAVE_MTGET_FLAGS */		/* if struct mtget contains mt_flags (flags) */
/* #undef HAVE_MTGET_BF */		/* if struct mtget contains mt_bf (optimum blocking factor) */
#define HAVE_STRUCT_RUSAGE 1	/* have struct rusage in sys/resource.h */
/* #undef HAVE_SI_UTIME */		/* if struct siginfo contains si_utime */
/* #undef HAVE_UNION_SEMUN */		/* have an illegal definition for union semun in sys/sem.h */
#define HAVE_UNION_WAIT 1		/* have union wait in wait.h */
/*
 * SCO UnixWare has st_atim.st__tim.tv_nsec but the st_atim.tv_nsec tests also
 * succeeds. If you use st_atim.tv_nsec on UnixWare, you get a warning about
 * illegal structure usage. For this reason, your code needs to have
 * #ifdef HAVE_ST__TIM before #ifdef HAVE_ST_NSEC.
 */
/* #undef HAVE_ST_SPARE1 */		/* if struct stat contains st_spare1 (usecs) */
/* #undef HAVE_ST_ATIMENSEC */	/* if struct stat contains st_atimensec (nanosecs) */
#define HAVE_ST_NSEC 1		/* if struct stat contains st_atim.tv_nsec (nanosecs) */
/* #undef HAVE_ST__TIM */		/* if struct stat contains st_atim.st__tim.tv_nsec (nanosecs) */
/* #undef HAVE_ST_ATIMESPEC */	/* if struct stat contains st_atimespec.tv_nsec (nanosecs) */
#define HAVE_ST_BLKSIZE 1		/* if struct stat contains st_blksize */
#define HAVE_ST_BLOCKS 1		/* if struct stat contains st_blocks */
/* #undef HAVE_ST_FSTYPE */		/* if struct stat contains st_fstype */
#define HAVE_ST_RDEV 1		/* if struct stat contains st_rdev */
/* #undef HAVE_ST_FLAGS */		/* if struct stat contains st_flags */
/* #undef STAT_MACROS_BROKEN */	/* if the macros S_ISDIR, S_ISREG .. don't work */

#define DEV_MINOR_BITS 32		/* # if bits needed to hold minor device number */
#define DEV_MINOR_NONCONTIG 1	/* if bits in minor device number are noncontiguous */

#define HAVE_SOCKADDR_STORAGE 1	/* if socket.h defines struct sockaddr_storage */


/*
 * Byteorder/Bitorder
 */
#define	HAVE_C_BIGENDIAN	/* Flag that WORDS_BIGENDIAN test was done */
/* #undef WORDS_BIGENDIAN */		/* If using network byte order             */
#define	HAVE_C_BITFIELDS	/* Flag that BITFIELDS_HTOL test was done  */
/* #undef BITFIELDS_HTOL */		/* If high bits come first in structures   */

/*
 * Types/Keywords
 */
#define SIZEOF_CHAR 1
#define SIZEOF_SHORT_INT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG_INT 4
#define SIZEOF_LONG_LONG 8
#define SIZEOF_CHAR_P 4
#define SIZEOF_UNSIGNED_CHAR 1
#define SIZEOF_UNSIGNED_SHORT_INT 2
#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_UNSIGNED_LONG_INT 4
#define SIZEOF_UNSIGNED_LONG_LONG 8
#define SIZEOF_UNSIGNED_CHAR_P 4

#define HAVE_LONGLONG 1		/* Compiler defines long long type */
/* #undef CHAR_IS_UNSIGNED */		/* Compiler defines char to be unsigned */

/* #undef const */			/* Define to empty if const doesn't work */
/* #undef uid_t */			/* To be used if uid_t is not present  */
/* #undef gid_t */			/* To be used if gid_t is not present  */
/* #undef size_t */			/* To be used if size_t is not present */
/* #undef ssize_t */			/* To be used if ssize_t is not present */
/* #undef pid_t */			/* To be used if pid_t is not present  */
/* #undef off_t */			/* To be used if off_t is not present  */
/* #undef mode_t */			/* To be used if mode_t is not present */
/* #undef time_t */			/* To be used if time_t is not present */
/* #undef caddr_t */			/* To be used if caddr_t is not present */
/* #undef daddr_t */			/* To be used if daddr_t is not present */
/* #undef dev_t */			/* To be used if dev_t is not present */
/* #undef ino_t */			/* To be used if ino_t is not present */
/* #undef nlink_t */			/* To be used if nlink_t is not present */
#define blksize_t long		/* To be used if blksize_t is not present */
/* #undef blkcnt_t */			/* To be used if blkcnt_t is not present */
/*
 * Important:	Next Step needs time.h for clock_t (because of a bug)
 */
/* #undef clock_t */			/* To be used if clock_t is not present */
/* #undef socklen_t */		/* To be used if socklen_t is not present */

/*
 * These types are present on all UNIX systems but should be avoided
 * for portability.
 * On Apollo/Domain OS we don't have them....
 *
 * Better include <utypes.h> and use Uchar, Uint & Ulong
 */
/* #undef u_char */			/* To be used if u_char is not present	*/
/* #undef u_short */			/* To be used if u_short is not present	*/
/* #undef u_int */			/* To be used if u_int is not present	*/
/* #undef u_long */			/* To be used if u_long is not present	*/

/*#undef HAVE_SIZE_T*/
/*#undef NO_SIZE_T*/
/* #undef VA_LIST_IS_ARRAY */		/* va_list is an array */
#define GETGROUPS_T gid_t
#define GID_T		GETGROUPS_T

/*
 * Define as the return type of signal handlers (int or void).
 */
#define RETSIGTYPE void

/*
 * Defines needed to get large file support.
 */
#ifdef	USE_LARGEFILES

#define	HAVE_LARGEFILES 1

#ifdef	HAVE_LARGEFILES		/* If we have working largefiles at all	   */
				/* This is not defined with glibc-2.1.3	   */

#define _FILE_OFFSET_BITS 64	/* # of bits in off_t if settable	   */
#define _LARGEFILE_SOURCE 1	/* To make ftello() visible (HP-UX 10.20). */
/* #undef _LARGE_FILES */		/* Large file defined on AIX-style hosts.  */
/* #undef _XOPEN_SOURCE */		/* To make ftello() visible (glibc 2.1.3). */
				/* XXX We don't use this because glibc2.1.3*/
				/* XXX is bad anyway. If we define	   */
				/* XXX _XOPEN_SOURCE we will loose caddr_t */

/* #undef HAVE_FSEEKO */		/* Do we need this? If HAVE_LARGEFILES is  */
				/* defined, we have fseeko()		   */

#endif	/* HAVE_LARGEFILES */
#endif	/* USE_LARGEFILES */

#ifdef USE_ACL			/* Enable/disable ACL support */
/*
 * POSIX ACL support
 */
#define HAVE_ACL_GET_FILE 1	/* acl_get_file() function */
#define HAVE_ACL_SET_FILE 1	/* acl_set_file() function */
#define HAVE_ACL_FROM_TEXT 1	/* acl_from_text() function */
#define HAVE_ACL_TO_TEXT 1		/* acl_to_text() function */
#define HAVE_ACL_FREE 1		/* acl_free() function */
#define HAVE_ACL_DELETE_DEF_FILE 1	/* acl_delete_def_file() function */

#if defined(HAVE_ACL_GET_FILE) && defined(HAVE_ACL_SET_FILE) && \
    defined(HAVE_ACL_FROM_TEXT) && defined(HAVE_ACL_TO_TEXT) && \
    defined(HAVE_ACL_FREE)
#	define	HAVE_POSIX_ACL	1 /* POSIX ACL's present */
#endif

/*
 * Sun ACL support.
 * Note: unfortunately, HP-UX has an (undocumented) acl() function in libc.
 */
/* #undef HAVE_ACL */			/* acl() function */
/* #undef HAVE_FACL */		/* facl() function */
/* #undef HAVE_ACLFROMTEXT */		/* aclfromtext() function */
/* #undef HAVE_ACLTOTEXT */		/* acltotext() function */

#if defined(HAVE_ACL) && defined(HAVE_FACL) && \
    defined(HAVE_ACLFROMTEXT) && defined(HAVE_ACLTOTEXT)
#	define	HAVE_SUN_ACL	1 /* Sun ACL's present */
#endif

/*
 * HP-UX ACL support.
 * Note: unfortunately, HP-UX has an (undocumented) acl() function in libc.
 */
/* #undef HAVE_GETACL */		/* getacl() function */
/* #undef HAVE_FGETACL */		/* fgetacl() function */
/* #undef HAVE_SETACL */		/* setacl() function */
/* #undef HAVE_FSETACL */		/* fsetacl() function */
/* #undef HAVE_STRTOACL */		/* strtoacl() function */
/* #undef HAVE_ACLTOSTR */		/* acltostr() function */
/* #undef HAVE_CPACL */		/* cpacl() function */
/* #undef HAVE_FCPACL */		/* fcpacl() function */
/* #undef HAVE_CHOWNACL */		/* chownacl() function */
/* #undef HAVE_SETACLENTRY */		/* setaclentry() function */
/* #undef HAVE_FSETACLENTRY */	/* fsetaclentry() function */

#if defined(HAVE_GETACL) && defined(HAVE_FGETACL) && \
    defined(HAVE_SETACL) && defined(HAVE_FSETACL) && \
    defined(HAVE_STRTOACL) && defined(HAVE_ACLTOTEXT)
#	define	HAVE_HP_ACL	1 /* HP-UX ACL's present */
#endif

/*
 * Global definition whether ACL support is present.
 * As HP-UX differs too much from other implementations, HAVE_HP_ACL is not
 * included in HAVE_ANY_ACL.
 */
#if defined(HAVE_POSIX_ACL) || defined(HAVE_SUN_ACL)
#	define	HAVE_ANY_ACL	1 /* Any ACL implementation present */
#endif

#endif	/* USE_ACL */

/*
 * Misc CC / LD related stuff
 */
/* #undef NO_USER_MALLOC */		/* If we cannot define our own malloc()	*/
#define HAVE_DYN_ARRAYS 1		/* If the compiler allows dynamic sized arrays */

/*
 * Strings that help to maintain OS/platform id's in C-programs
 */
#define HOST_ALIAS "i686-pc-linux-gnu"		/* Output from config.guess (orig)	*/
#define HOST_SUB "i686-pc-linux-gnu"			/* Output from config.sub (modified)	*/
#define HOST_CPU "i686"			/* CPU part from HOST_SUB		*/
#define HOST_VENDOR "pc"		/* VENDOR part from HOST_SUB		*/
#define HOST_OS "linux-gnu"			/* CPU part from HOST_SUB		*/


/*
 * Begin restricted code for quality assurance.
 *
 * Warning: you are not allowed to include the #define below if you are not
 * using the Schily makefile system or if you modified the autoconfiguration
 * tests.
 *
 * If you only added other tests you are allowed to keep this #define.
 *
 * This restiction is introduced because this way, I hope that people
 * contribute to the project instead of creating branches.
 */
#define	IS_SCHILY_XCONFIG
/*
 * End restricted code for quality assurance.
 */
