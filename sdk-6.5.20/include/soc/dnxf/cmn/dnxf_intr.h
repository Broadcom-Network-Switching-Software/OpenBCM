/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF INTERRUPT H
 */
 
#ifndef _SOC_DNXF_INTERRUPT_H_
#define _SOC_DNXF_INTERRUPT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>

int
soc_dnxf_ser_init(int unit);

void soc_dnxf_block_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4);


#endif 
