
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_MUTEX_H_

#define _DNXC_SW_STATE_MUTEX_H_

#include <sal/core/sync.h>

#define SW_STATE_MUTEX_DESC_LENGTH 128

typedef struct
{
    sal_mutex_t mtx;
    char description[SW_STATE_MUTEX_DESC_LENGTH];
} sw_state_mutex_t;

#endif
