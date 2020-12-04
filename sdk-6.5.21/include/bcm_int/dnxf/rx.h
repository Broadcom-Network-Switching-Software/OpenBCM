/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF FABRIC H
 */
#ifndef _BCM_INT_DNXF_RX_H_
#define _BCM_INT_DNXF_RX_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <sal/types.h>
#include <bcm/types.h>


int dnxf_rx_init(int unit);
int dnxf_rx_deinit(int unit);

#endif /*_BCM_INT_DNXF_RX_H_*/
