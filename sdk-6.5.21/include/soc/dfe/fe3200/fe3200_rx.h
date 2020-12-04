/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 RX H
 */


#ifndef _SOC_FE3200_RX_H_
#define _SOC_FE3200_RX_H_

#include <soc/error.h>

#define SOC_FE3200_RX_MAX_NOF_CPU_BUFFERS (4)


soc_error_t soc_fe3200_rx_cpu_address_modid_set(int unit, int num_of_cpu_addresses, int *cpu_addresses);
soc_error_t soc_fe3200_rx_cpu_address_modid_init(int unit);


#endif
