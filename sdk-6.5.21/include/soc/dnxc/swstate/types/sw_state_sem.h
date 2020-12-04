
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_SEM_H_

#define _DNXC_SW_STATE_SEM_H_

#include <sal/core/sync.h>

#define SW_STATE_SEM_DESC_LENGTH 128

typedef struct
{
    sal_sem_t sem;
    int is_binary;
    int initial_count;
    char description[SW_STATE_SEM_DESC_LENGTH];
} sw_state_sem_t;

#endif
