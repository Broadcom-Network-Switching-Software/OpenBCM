/*! \file bcma_io_ctrlc.c
 *
 * System inteface for handling Ctrl-C traps.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

#include <bsl/bsl.h>
#include <bcma/io/bcma_io_ctrlc.h>

/*! Maximum number of nested Ctrl-C trapped function calls. */
#ifndef IO_CTRLC_LVL_MAX
#define IO_CTRLC_LVL_MAX        8
#endif

/*! Prototype for Ctrlc-C handler callback function. */
typedef int (*io_ctrlc_handler_cb_f)(void *);

/*!
 * \brief Ctrl-C handler information.
 */
typedef struct io_ctrlc_handler_s {

    /*! Jump buffers for Ctrl-C exceptions. */
    sigjmp_buf jmp_buf;

    /*! Callback functions for Ctrl-C exceptions. */
    io_ctrlc_handler_cb_f cb;

    /*! User data of callback functions for Ctrl-C exceptions. */
    void *cb_data;

} io_ctrlc_handler_t;

/*!
 * \brief Ctrl-C trap control.
 */
typedef struct io_ctrlc_s {

    /*! Handlers for Ctrl-C exceptions (level 0 is unused). */
    io_ctrlc_handler_t hdl[IO_CTRLC_LVL_MAX + 1];

    /*! Current function nesting level. */
    int cur_lvl;

    /*! Current Ctrl-C enable state. */
    int enable;

    /*! Current Ctrl-C suspend state. */
    int suspend;

} io_ctrlc_t;

static io_ctrlc_t ctrlc_data;

static void
ctrl_c_handler(int sig)
{
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META("Ctrl-C\n")));

    if (ctrlc_data.cur_lvl > 0) {
        io_ctrlc_handler_t *hdl = &ctrlc_data.hdl[ctrlc_data.cur_lvl];

        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META(
                        "Current Ctrl_C level = %d\n"),
                     ctrlc_data.cur_lvl));

        if (hdl->cb && hdl->cb(hdl->cb_data) == 0) {
            return;
        }

        /* Flush output buffers to prevent garbled output after longjmp */
        fflush(stdout);
        siglongjmp(hdl->jmp_buf, 1);
    }
}

int
bcma_io_ctrlc_enable_set(int enable)
{
    struct sigaction sigact;

    if (enable != ctrlc_data.enable) {
        if (enable) {
            sigemptyset(&sigact.sa_mask);
            sigact.sa_handler = ctrl_c_handler;
            sigact.sa_flags = SA_NODEFER;
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Enable Ctrl-C\n")));
            if (sigaction(SIGINT, &sigact, NULL) < 0) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META("sigaction failed\n")));
                return -1;
            }
        } else {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Disable Ctrl-C\n")));
            signal(SIGINT, SIG_DFL);
        }
        ctrlc_data.enable = enable;
    }
    return 0;
}

int
bcma_io_ctrlc_suspend_set(int suspend)
{
    sigset_t sigset;

    if (suspend) {
        if (ctrlc_data.suspend == 0) {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Suspend Ctrl-C\n")));
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGINT);
            pthread_sigmask(SIG_BLOCK, &sigset, NULL);
        }
        ctrlc_data.suspend++;
    } else if (ctrlc_data.suspend) {
        ctrlc_data.suspend--;
        if (ctrlc_data.suspend == 0) {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META("Resume Ctrl-C\n")));
            sigemptyset(&sigset);
            sigaddset(&sigset, SIGINT);
            pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
        }
    }
    return 0;
}

/*
 * Please refer to standard C sigsetjmp/siglongjmp documentation.
 */
int
bcma_io_ctrlc_exec(int (*func)(void *), void *data,
                   int (*ctrlc_sig_cb)(void *), void *cb_data,
                   int rv_intr)
{
    int rv = rv_intr;
    io_ctrlc_handler_t *hdl;

    /* Check nesting level */
    if (++ctrlc_data.cur_lvl >= IO_CTRLC_LVL_MAX) {
        LOG_WARN(BSL_LS_APPL_SHELL,
                 (BSL_META("Maximum Ctrl-C nesting level exceeded\n")));
        ctrlc_data.cur_lvl--;
        return 0;
    }

    hdl = &ctrlc_data.hdl[ctrlc_data.cur_lvl];
    hdl->cb = ctrlc_sig_cb;
    hdl->cb_data = cb_data;

    if (sigsetjmp(hdl->jmp_buf, 0) == 0) {
        /* Normal return from setjmp */
        rv = func(data);
    }

    ctrlc_data.cur_lvl--;

    return rv;
}
