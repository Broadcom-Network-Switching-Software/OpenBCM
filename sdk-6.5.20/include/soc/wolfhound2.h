/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wolfhound2.h
 * Purpose:
 * Requires:
 */

#ifndef _SOC_WOLFHOUND2_H_
#define _SOC_WOLFHOUND2_H_
#include <soc/drv.h>

extern int soc_wolfhound2_dport_init(int unit);
extern int soc_wolfhound2_features(int unit, soc_feature_t feature);
extern int soc_wolfhound2_port_config_init(int unit, uint16 dev_id);
extern int soc_wolfhound2_chip_reset(int unit);
extern int soc_wolfhound2_mem_config(int unit, int dev_id);
extern int soc_wolfhound2_port_reset(int unit);
extern int
soc_wolfhound2_gphy_get(int unit, int qgphy_id, uint8 *is_gphy);
extern soc_functions_t soc_wolfhound2_drv_funs;

#endif /* _SOC_WOLFHOUND2_H_ */
