/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	memlog.h
 * Purpose: 	Supports memlog memory leak tool
 */

#ifndef _SAL_MEMLOG_H
#define _SAL_MEMLOG_H

#if defined(MEMLOG_SUPPORT) && defined(__GNUC__)

#include <stdio.h>
#include <unistd.h>
#include <sal/core/sync.h>

#define MEM_LOG_BUF_SIZE 512

extern sal_mutex_t memlog_lock;
extern int memlog_fd;
extern char memlog_buf[MEM_LOG_BUF_SIZE];

#define MEMLOG_ALLOC(func,ptr,size,string) \
    { \
        int n; \
        if (memlog_fd > 0) { \
            if (memlog_lock) { \
                sal_mutex_take(memlog_lock, sal_mutex_FOREVER); \
            } \
            if ((string) != NULL) { \
                n = sal_snprintf(memlog_buf, MEM_LOG_BUF_SIZE, "%s %p %p %d %s\n", \
                    (func), __builtin_return_address(0), (ptr), (size), (string)); \
            } else {\
                n = sal_snprintf(memlog_buf, MEM_LOG_BUF_SIZE, "%s %p %p %d NULL\n", \
                    (func), __builtin_return_address(0), (ptr), (size)); \
            } \
            write(memlog_fd, memlog_buf, n); \
            fsync(memlog_fd); \
            if (memlog_lock) { \
                sal_mutex_give(memlog_lock); \
            } \
        } \
    }

#define MEMLOG_FREE(func,ptr) \
    { \
        int n; \
        if (memlog_fd > 0) { \
            if (memlog_lock) { \
                sal_mutex_take(memlog_lock, sal_mutex_FOREVER); \
            } \
            n = sal_snprintf(memlog_buf, MEM_LOG_BUF_SIZE, "%s %p\n", (func), (ptr)); \
            write(memlog_fd, memlog_buf, n); \
            fsync(memlog_fd); \
            if (memlog_lock) { \
                sal_mutex_give(memlog_lock); \
            } \
        } \
    }

#else

#define MEMLOG_ALLOC(func,ptr,size,string)
#define MEMLOG_FREE(func,ptr)

#endif

#endif	/* !_SAL_MEMLOG_H */
