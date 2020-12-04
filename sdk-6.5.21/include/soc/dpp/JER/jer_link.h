/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#ifndef _SOC_JER_LINK_H_
#define _SOC_JER_LINK_H_

#include <soc/error.h>
#include <soc/dcmn/error.h>

#include <soc/linkctrl.h>

extern CONST soc_linkctrl_driver_t  soc_linkctrl_driver_jer;
extern int
soc_jer_linkcan_fault_to_cmiclink_enable_set(int unit, pbmp_t *pbm,
                                           int remote_fault_enable, int local_fault_enable);
#endif


