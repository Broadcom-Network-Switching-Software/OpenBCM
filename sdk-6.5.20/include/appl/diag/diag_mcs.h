/*
 * $Id: diag_mcs.h,v 1.0
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_mcs.h
 * Purpose:
 */

#ifndef   _DIAG_MCS_H_
#define   _DIAG_MCS_H_

#define MAX_UCS     3
typedef struct uKernel_info_s {
    char status[12];
    char fw_version[100];
    unsigned int cpsr;
    unsigned int type;
    unsigned int armReg[16];
} uKernel_info_t;

#endif /* _DIAG_MCS_H_ */
