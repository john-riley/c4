/*
   Copyright (C) CFEngine AS

   This file is part of CFEngine 3 - written and maintained by CFEngine AS.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

  To the extent this program is licensed as part of the Enterprise
  versions of CFEngine, the applicable Commercial Open Source License
  (COSL) may apply to this file if you as a licensee so wish it. See
  included file COSL.txt.
*/

#ifndef CFENGINE_PLATFORM_H
#define CFENGINE_PLATFORM_H

/*
 * Platform-specific definitions and declarations.
 *
 * INCLUDE THIS HEADER ALWAYS FIRST in order to define appropriate macros for
 * including system headers (such as _FILE_OFFSET_BITS).
 */

#ifdef HAVE_CONFIG_H
# include "../src/conf.h"
#endif

#define _GNU_SOURCE 1

#ifdef _WIN32
# define MAX_FILENAME 227
# define WINVER 0x501
# if defined(__CYGWIN__)
#  undef FD_SETSIZE
# endif
  /* Increase select(2) FD limit from 64. It's documented and valid to do it
   * like that provided that we define it *before* including winsock2.h. */
# define FD_SETSIZE 4096
#else
# define MAX_FILENAME 254
#endif

#ifdef __MINGW32__
# include <winsock2.h>
# include <windows.h>
# include <accctrl.h>
# include <aclapi.h>
# include <psapi.h>
# include <wchar.h>
# include <sddl.h>
# include <tlhelp32.h>
# include <iphlpapi.h>
# include <ws2tcpip.h>
# include <objbase.h>           // for disphelper
# ifndef SHUT_RDWR              // for shutdown()
#  define SHUT_RDWR SD_BOTH
# endif
#endif

/* Standard C. */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stddef.h>                                     /* offsetof, size_t */

/* POSIX but available in all platforms. */
#include <strings.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

/* We now require a pthreads implementation. */
#include <pthread.h>

#ifndef _GETOPT_H
# include "getopt.h"
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_UNAME
# include <sys/utsname.h>
#else
# define _SYS_NMLN       257

struct utsname
{
    char sysname[_SYS_NMLN];
    char nodename[_SYS_NMLN];
    char release[_SYS_NMLN];
    char version[_SYS_NMLN];
    char machine[_SYS_NMLN];
};

#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#ifdef HAVE_SYS_SYSTEMINFO_H
# include <sys/systeminfo.h>
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#ifdef HAVE_SYS_MOUNT_H
# include <sys/mount.h>
#endif

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(s) ((unsigned)(s) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(s) (((s) & 255) == 0)
#endif
#ifndef WIFSIGNALED
# define WIFSIGNALED(s) ((s) & 0)       /* Can't use for BSD */
#endif
#ifndef WTERMSIG
# define WTERMSIG(s) ((s) & 0)
#endif

#include "bool.h"

#include <errno.h>

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#else
# define dirent direct
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#ifndef PATH_MAX
# ifdef _MAX_PATH
#  define PATH_MAX _MAX_PATH
# else
#  define PATH_MAX 4096
# endif
#endif

#include <signal.h>

#ifdef __MINGW32__
# define LOG_LOCAL0      (16<<3)
# define LOG_LOCAL1      (17<<3)
# define LOG_LOCAL2      (18<<3)
# define LOG_LOCAL3      (19<<3)
# define LOG_LOCAL4      (20<<3)
# define LOG_LOCAL5      (21<<3)
# define LOG_LOCAL6      (22<<3)
# define LOG_LOCAL7      (23<<3)
# define LOG_USER        (1<<3)
# define LOG_DAEMON      (3<<3)

/* MinGW added this flag only in latest version. */
# ifndef IPV6_V6ONLY
#  define IPV6_V6ONLY 27
# endif

// Not available in MinGW headers unless you raise WINVER and _WIN32_WINNT, but
// that is very badly supported in MinGW ATM.
ULONGLONG WINAPI GetTickCount64(void);

#else /* !__MINGW32__ */
# include <syslog.h>
#endif

#ifdef _AIX
# ifndef ps2
#  include <sys/statfs.h>
# endif

# include <sys/systemcfg.h>
#endif

#ifdef __sun
# include <sys/statvfs.h>
# undef nfstype

#include <sys/mkdev.h>

#ifndef timersub
# define timersub(a, b, result)                             \
    do                                                      \
    {                                                       \
           (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;    \
           (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
           if ((result)->tv_usec < 0)                       \
           {                                                \
               --(result)->tv_sec;                          \
               (result)->tv_usec += 1000000;                \
           }                                                \
    } while (0)
#endif

#endif

#if !HAVE_DECL_DIRFD
int dirfd(DIR *dirp);
#endif

/* strndup is defined as a macro on many systems */
#if !HAVE_DECL_STRNDUP
# ifndef strndup
char *strndup(const char *s, size_t n);
# endif
#endif

#if !HAVE_DECL_STRNLEN
size_t strnlen(const char *str, size_t maxlen);
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if !HAVE_DECL_STRLCPY
size_t strlcpy(char *destination, const char *source, size_t size);
#endif

#if !HAVE_DECL_STRLCAT
size_t strlcat(char *destination, const char *source, size_t size);
#endif

/* Must be always the last one! */
#include "deprecated.h"
#include "config.post.h"

#endif  /* CFENGINE_PLATFORM_H */
