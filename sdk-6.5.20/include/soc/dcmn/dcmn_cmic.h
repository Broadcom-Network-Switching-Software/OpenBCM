/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN IPROC
 */

#ifndef _SOC_DCMN_CMIC_DRV_H_
#define _SOC_DCMN_CMIC_DRV_H_

#include <soc/defs.h>
#include <soc/dcmn/dcmn_defs.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#endif 



int soc_dcmn_cmic_device_hard_reset(int unit, int reset_action);



int soc_dcmn_cmic_sbus_timeout_set(int unit, uint32 core_freq_khz, int schan_timeout);
int soc_dcmn_cmic_pcie_userif_purge_ctrl_init(int unit);
int soc_dcmn_cmic_mdio_config(int unit, int dividend, int divisor, int delay);


int soc_dcmn_cmic_mdio_set(int unit);

#endif 

