/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: soc_log_buf.h
 * Purpose: Thread safe circular buffer for logs
 */

#ifndef _SOC_LOG_BUF_H_
#define _SOC_LOG_BUF_H_

#include <sal/core/sync.h>

extern int soc_log_buf_init(void *location, int size, sal_mutex_t mutex);
extern int soc_log_buf_inc_boot_count(void *location);
extern int soc_log_buf_validate(void *location, int size);
extern int soc_log_buf_add(void *location, int size);
extern int soc_log_buf_entry_get_size(void *location, int id, int *size);
extern int soc_log_buf_entry_read(void *location, int id, int offset, 
    int size, void *data);
extern int soc_log_buf_entry_write(void *location, int id, int offset,
    int size, void *data);
extern int soc_log_buf_entry_valid(void *location, int id);
extern int soc_log_buf_get_boot_count(void *location);
extern int soc_log_buf_print(void *location);
extern int soc_log_buf_set_mutex(void *location, sal_mutex_t mutex);
extern int soc_log_buf_get_next_id(void *location, int id);
extern sal_mutex_t soc_log_buf_get_mutex(void *location);
extern int soc_log_buf_search(void *location,
                              void *buffer,
                              int buf_size,
                              void *criteria, 
                              int (*match)(void * crit, void * entry));
#endif
