/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXC IPROC
 */

#ifndef _SOC_DNXC_CMIC_DRV_H_
#define _SOC_DNXC_CMIC_DRV_H_

#include <soc/defs.h>
#include <soc/dnxc/dnxc_defs.h>
#ifdef BCM_DNX_SUPPORT

#endif

int soc_dnxc_cmic_device_hard_reset(
    int unit,
    int reset_action);

int soc_dnxc_cmic_sbus_timeout_set(
    int unit,
    uint32 core_freq_khz,
    int schan_timeout);
int soc_dnxc_cmic_mdio_config(
    int unit,
    int dividend,
    int divisor,
    int delay);

int soc_dnxc_cmic_mdio_set(
    int unit);

int soc_dnxc_mdio_config_set(
    int unit);

void soc_dnxc_data_mdio_config_get(
    int unit,
    int *int_divisor,
    int *ext_divisor,
    int *int_delay,
    int *ext_delay);
#endif
