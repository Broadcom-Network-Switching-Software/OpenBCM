/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON RX H
 */


#ifndef _SOC_RAMON_RX_H_
#define _SOC_RAMON_RX_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 

#include <soc/error.h>

shr_error_e soc_ramon_rx_cpu_address_modid_set(int unit, int num_of_cpu_addresses, int *cpu_addresses);
shr_error_e soc_ramon_rx_cpu_address_modid_init(int unit);

#endif
