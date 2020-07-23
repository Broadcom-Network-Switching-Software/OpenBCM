/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC NIF ILKN DRV H
 */
 
#ifndef _SOC_NIF_ILKN_DRV_H_
#define _SOC_NIF_ILKN_DRV_H_

#include <soc/error.h>

soc_error_t soc_ilkn_link_status_get(int unit, soc_port_t port, int *is_link_up, int *is_latch_down);
soc_error_t soc_ilkn_drv_num_of_lanes_get(int unit, soc_port_t port,  uint32 *num_of_lanes);
soc_error_t soc_ilkn_drv_num_of_lanes_set(int unit, soc_port_t port,  uint32 num_of_lanes);
soc_error_t soc_ilkn_link_status_clear(int unit, soc_port_t port);

#endif 
