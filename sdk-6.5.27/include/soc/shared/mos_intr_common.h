/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    mos_intr_common.h
 */

#ifndef _SOC_SHARED_MOS_INTR_COMMON_H
#define _SOC_SHARED_MOS_INTR_COMMON_H


/*
 * Common definitions for cross-cpu interrupts
 */

/* Define each of the 4 bits in the S/W interrupt reg based on the
   source of the interrupt */
#define CMICM_SW_INTR_HOST 0
#define CMICM_SW_INTR_RCPU 1
#define CMICM_SW_INTR_UC0  2
#define CMICM_SW_INTR_UC1  3
#define CMICM_SW_INTR_UC2  4
#define CMICM_SW_INTR_UC3  5

#define CMICM_NUM_UCS           4   /* This is now the max UCs supported. */
#define CMICM_SW_INTR_NUM_HOSTS (CMICM_NUM_UCS+2)

#ifdef BCM_CMICX_SUPPORT
#define CMICM_SW_INTR_UC(num)                                   \
    (((num) == 0) ? CMICM_SW_INTR_UC0 :                         \
     ((num) == 1) ? CMICM_SW_INTR_UC1 :                         \
     ((num) == 2) ? CMICM_SW_INTR_UC2 :                         \
     ((num) == 3 ? CMICM_SW_INTR_UC3 : CMICM_SW_INTR_RCPU))
#else
#define CMICM_SW_INTR_UC(num)                                   \
    (((num) == 0) ? CMICM_SW_INTR_UC0 :                         \
     ((num) == 1 ? CMICM_SW_INTR_UC1 : CMICM_SW_INTR_RCPU))
#endif

#endif
