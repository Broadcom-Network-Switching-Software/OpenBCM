/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DPP H
 */
#ifndef _BCM_INT_DPP_EYESCAN_H_
#define _BCM_INT_DPP_EYESCAN_H_

extern uint64* dpp_saved_counter_1[BCM_LOCAL_UNITS_MAX];
extern uint64* dpp_saved_counter_2[BCM_LOCAL_UNITS_MAX];

int bcm_dpp_eyescan_init(int unit);
int bcm_dpp_eyescan_deinit(int unit);



  
#endif /*_BCM_INT_DFE_EYESCAN_H_*/
