/*! \file sal_sysrq.c
 *
 * Platform-dependent system requests.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sysrq.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>

/* Backtrace is currently supported only as a glibc extension */
#ifndef SAL_BACKTRACE_SUPPORT
#if defined(__GLIBC__) && !defined(__UCLIBC__)
#define SAL_BACKTRACE_SUPPORT 1
#else
#define SAL_BACKTRACE_SUPPORT 0
#endif
#endif

#if SAL_BACKTRACE_SUPPORT == 1

#include <execinfo.h>

#define BT_REC_MAX      64

static void
gnu_backtrace(void)
{
    int nrecs;
    void *bt_recs[BT_REC_MAX];

    /* Link with -rdynamic to get function names. */
    nrecs = backtrace(bt_recs, BT_REC_MAX);
    backtrace_symbols_fd(bt_recs, nrecs, STDOUT_FILENO);
}

static int
gdb_present(void)
{
  int status;
  int pid = fork();

  if (pid == -1) {
      return -1;
  }

  if (pid == 0) {
      /*
       * Use a child to test for gdb, since a ptrace test from the
       * primary thread will prevent the backtrace gdb instance from
       * attaching later.
       */
      int ppid = getppid();
      if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0) {
          /* Wait for the parent to stop and let it continue */
          waitpid(ppid, NULL, 0);
          ptrace(PTRACE_CONT, NULL, NULL);
          /* Presumably gdb is not present */
          ptrace(PTRACE_DETACH, getppid(), NULL, NULL);
          exit(0);
      }
      /* Trace failed so assume that gdb is present */
      exit(1);
  }

  /* Let parent wait for child to detemine trace status */
  waitpid(pid, &status, 0);

  return WEXITSTATUS(status);
}

static int
gdb_backtrace(void)
{
    char pid_buf[30];
    char cmd_buf[384];
    char name_buf[256];
    int name_len;
    int output_enable;
    int tid = 0;
    FILE *gdb_out;

#ifdef SYS_gettid
    tid = syscall(SYS_gettid);
#endif

    /* Get our own name */
    name_len = readlink("/proc/self/exe", name_buf, sizeof(name_buf) - 1);
    if (name_len <= 0) {
        return -1;
    }
    name_buf[name_len] = 0;

    /*
     * Since gdb has its own thread numbering system, we cannot
     * request a backtrace for a specific thread based on the thread
     * ID (tid). Instead we request backtraces for all threads and
     * filter the output to suit our needs.
     */
    snprintf(cmd_buf, sizeof(cmd_buf) - 1,
             "gdb --batch -n -ex 'thread apply all bt' --pid=%d %s",
             getpid(), name_buf);

    snprintf(pid_buf, sizeof(pid_buf) - 1, "LWP %d", tid);
    printf("Attaching gdb for %s ...\n", pid_buf);
    if ((gdb_out = popen(cmd_buf, "r")) == NULL) {
        printf("FAILED\n");
        return -1;
    }
    output_enable = 0;
    while (fgets(cmd_buf, sizeof(cmd_buf) - 1, gdb_out) != NULL) {
        if (output_enable) {
            if (tid && *cmd_buf != '#') {
                /* End of backtrace for this thread */
                output_enable = 0;
            }
        } else {
            if (tid == 0 || strstr(cmd_buf, pid_buf) != NULL) {
                /* Found matching thread ID */
                output_enable = 1;
            }
        }
        if (output_enable) {
            /* Only show backtrace for matching thread ID */
            printf("%s", cmd_buf);
        }
    }
    pclose(gdb_out);
    return 0;
}

static void
show_backtrace(int try_gdb)
{
    if (try_gdb == 0 || gdb_present()) {
        gnu_backtrace();
    } else {
        if (gdb_backtrace() < 0) {
            gnu_backtrace();
        }
    }
}

#else

/* No built-in backtrace support */
static void
show_backtrace(int try_gdb)
{
    printf("Backtrace not available\n");
}

#endif

void
sal_sysrq(sal_sysrq_t req, unsigned long prm)
{
    switch (req) {
    case SAL_SYSRQ_EXIT:
        exit((int)prm);
        break;
    case SAL_SYSRQ_RESET:
        exit((int)prm);
        break;
    case SAL_SYSRQ_BACKTRACE:
        show_backtrace(prm);
        break;
    case SAL_SYSRQ_ABORT:
    default:
        abort();
        break;
    }
}
