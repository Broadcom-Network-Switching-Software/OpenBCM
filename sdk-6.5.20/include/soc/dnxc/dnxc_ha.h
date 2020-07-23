
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_HA_H

#define _DNXC_HA_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <shared/utilex/utilex_seq.h>

int dnxc_ha_init(
    int unit);

int dnxc_ha_deinit(
    int unit);

int dnxc_ha_tid_set(
    int unit);

int dnxc_ha_tid_clear(
    int unit);

int dnxc_ha_appl_deinit_state_change(
    int unit,
    uint8 is_start);

int dnxc_ha_is_access_disabled(
    int unit,
    utilex_seq_allow_access_e access_type);

int dnxc_ha_tmp_allow_access_enable(
    int unit,
    utilex_seq_allow_access_e access_type);

int dnxc_ha_tmp_allow_access_disable(
    int unit,
    utilex_seq_allow_access_e access_type);

int dnxc_ha_appl_is_deinit(
    int unit,
    uint8 *is_deinit);
#endif
