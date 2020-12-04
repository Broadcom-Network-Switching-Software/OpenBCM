/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        evlog.h
 * Purpose:     
 */

#ifndef   _SHARED_EVLOG_H_
#define   _SHARED_EVLOG_H_

#include <shared/error.h>

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/thread.h>
#include <sal/core/spl.h>

#if defined(INCLUDE_SHARED_EVLOG)

typedef struct shared_evlog_s {
    int max_entries;       /* Max number of entries in log */
    int max_args;          /* Max number of arguments per entry */

    int cur_idx;           /* Current input pointer */
    uint32 ev_class_bmp;   /* Which event classes are enabled */
    int ev_count;          /* How many events */

    /* The following are arrays indexed by log event number */
    char **ev_strs;        /* Pointer to description string (may be NULL) */
    uint32 *ev_args;       /* Pointer to array of (max_args) integer args */
    sal_usecs_t *ev_ts;    /* Timestamp of event */
    sal_thread_t *ev_tid;  /* Thread ID of event */
    uint32 *ev_flags;      /* Was this in interrupt context */

} shared_evlog_t;

#define SHARED_EVLOG_INTR_CONTEXT 0x1

#define SHARED_EVLOG_DECLARE(log_ptr) shared_evlog_t *log_ptr;
#define SHARED_EVLOG_EXTERN(log_ptr)  extern SHARED_EVLOG_DECLARE(log_ptr)

#define SHARED_EVLOG_CLASS_BMP(ev)      ((ev)->ev_class_bmp)
#define SHARED_EVLOG_CLASS_ALL          0xffffffff
#define SHARED_EVLOG_ENABLE_SET(ev, enable)     \
    SHARED_EVLOG_CLASS_BMP(ev) = (enable) ? SHARED_EVLOG_CLASS_ALL : 0
#define SHARED_EVLOG_ENABLE_GET(ev)     SHARED_EVLOG_CLASS_BMP(ev)
#define SHARED_EVLOG_ENABLE_ALL(ev)     \
    SHARED_EVLOG_ENABLE_SET(ev, SHARED_EVLOG_CLASS_ALL)
#define SHARED_EVLOG_COUNT(ev)          ((ev)->ev_count)

#define SHARED_EVENT_LOG_CLASS(ev, class, str, arg0, arg1, arg2, arg3)  \
    if (((ev) != NULL) && (((ev)->ev_class_bmp & (class)) != 0)) do {   \
        int idx, sp;                                                    \
        shared_evlog_t *loc_ev;                                         \
        sp = sal_splhi();                                               \
        idx = ((ev)->cur_idx)++;                                        \
        ((ev)->cur_idx) %= (ev)->max_entries;                           \
        ((ev)->ev_count)++;                                             \
        loc_ev = ev;                                                    \
        sal_spl(sp);                                                    \
        SHARED_EVLOG_STR(loc_ev, idx) = (str);                          \
        SHARED_EVLOG_FLAGS(loc_ev, idx) =                               \
            sal_int_context() ? SHARED_EVLOG_INTR_CONTEXT : 0;          \
        SHARED_EVLOG_RECORD_TS(loc_ev, idx);                            \
        SHARED_EVLOG_RECORD_TID(loc_ev, idx);                           \
        SHARED_EVLOG_ARG(loc_ev, idx, 0) = arg0;                        \
        SHARED_EVLOG_ARG(loc_ev, idx, 1) = arg1;                        \
        SHARED_EVLOG_ARG(loc_ev, idx, 2) = arg2;                        \
        SHARED_EVLOG_ARG(loc_ev, idx, 3) = arg3;                        \
    } while (0)

#define SHARED_EVENT_LOG(ev, str, arg0, arg1, arg2, arg3)               \
    SHARED_EVENT_LOG_CLASS(ev, 0xffffffff, str, arg0, arg1, arg2, arg3)

#define SHARED_EVLOG_ARG(ev, ent, n)                                    \
    ((ev)->ev_args[(ev)->max_args * (ent) + (n)])
#define SHARED_EVLOG_TIMESTAMP(ev, ent) ((ev)->ev_ts[ent])
#define SHARED_EVLOG_TID(ev, ent)       ((ev)->ev_tid[ent])
#define SHARED_EVLOG_STR(ev, ent)       ((ev)->ev_strs[ent])
#define SHARED_EVLOG_FLAGS(ev, ent)     ((ev)->ev_flags[ent])

#define SHARED_EVLOG_MAXARGS_DEFAULT 4


/* Should timestamp be recorded in interrupt? */
#if !defined(SHARED_EVLOG_NO_INTR_TIMESTAMP)
#define SHARED_EVLOG_RECORD_TS(ev, idx)                            \
    SHARED_EVLOG_TIMESTAMP(ev, idx) = sal_time_usecs()
#else
#define SHARED_EVLOG_RECORD_TS(ev, idx)                            \
    if (!sal_int_context()) SHARED_EVLOG_TIMESTAMP(ev, idx) = sal_time_usecs()
#endif

/* Should thread ID be recorded in interrupt? */
#if !defined(SHARED_EVLOG_NO_INTR_TID)
#define SHARED_EVLOG_RECORD_TID(ev, idx)                            \
    SHARED_EVLOG_TID(ev, idx) = sal_thread_self()
#else
#define SHARED_EVLOG_RECORD_TID(ev, idx)                            \
    if (!sal_int_context()) SHARED_EVLOG_TID(ev, idx) = sal_thread_self()
#endif

#else /* SAL_NO_EVLOG defined */

/* Nullify API macros */
#define SHARED_EVLOG_DECLARE(log_ptr)
#define SHARED_EVLOG_EXTERN(log_ptr)
#define SHARED_EVLOG_CREATE(max_entries)
#define SHARED_EVENT_LOG(ev, str, arg0, arg1, arg2, arg3)
#define SHARED_EVENT_LOG_CLASS(ev, class, str, arg0, arg1, arg2, arg3)

#define SHARED_EVLOG_ARG(ev, ent, n)
#define SHARED_EVLOG_TIMESTAMP(ev, ent)
#define SHARED_EVLOG_TID(ev, ent)
#define SHARED_EVLOG_STR(ev, ent)
#define SHARED_EVLOG_FLAGS(ev, ent)

#define SHARED_EVLOG_MAXARGS_DEFAULT 4

#endif  /* !SHARED_NO_EVLOG */

#endif /* !_SHARED_EVLOG_H_ */
