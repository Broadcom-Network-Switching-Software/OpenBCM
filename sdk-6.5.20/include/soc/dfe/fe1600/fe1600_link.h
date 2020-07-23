/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE1600 STAT H
 */
 
#ifndef _SOC_FE1600_LINK_H_
#define _SOC_FE1600_LINK_H_

#include <soc/linkctrl.h>

extern soc_linkctrl_driver_t  soc_linkctrl_driver_fe1600;

soc_error_t soc_fe1600_linkctrl_init(int unit);

#endif 
