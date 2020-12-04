/*----------------------------------------------------------------------
 * $Id: tefmod_device.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
/*
 *  
 * $Copyright:
 * All Rights Reserved.$
 */
#ifndef _TEFMOD_DEVICE_H_
#define _TEFMOD_DEVICE_H_

typedef struct tefmod_device_aux_modes_s {
    uint32_t core_id ; 
    uint16_t st_current_entry ;
    uint16_t st_hcd[4] ;
    uint16_t st_pll_div[4] ;
    uint16_t st_os[4] ;

    uint16_t hto_enable[4]  ;
    uint16_t hto_pll_div[4] ;
    uint16_t hto_os[4] ;
} tefmod_device_aux_modes_t ;

#endif
