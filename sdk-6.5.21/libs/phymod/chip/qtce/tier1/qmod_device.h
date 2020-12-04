/*----------------------------------------------------------------------
 * $Id: qmod_device.h,
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
#ifndef _QMOD_DEVICE_H_
#define _QMOD_DEVICE_H_

/* debug mask are used by TEMOD */
#define QMOD_DBG_CL72       (1L << 16) /* CL72 */
#define QMOD_DBG_FWL        (1L << 15) /* FW loading debug */
#define QMOD_DBG_REGACC     (1L << 14) /* Print all register accesses */
#define QMOD_DBG_CFG        (1L << 13) /* CFG */
#define QMOD_DBG_LNK        (1L << 12) /* Link */
#define QMOD_DBG_SPD        (1L << 11) /* Speed */
#define QMOD_DBG_AN         (1L << 10) /* AN */
#define QMOD_DBG_LPK        (1L << 9) /* Local or remote loopback */
#define QMOD_DBG_PMD        (1L << 8) /* PMD */
#define QMOD_DBG_SCN        (1L << 7) /* Link scan*/
#define QMOD_DBG_TST        (1L << 6) /* Testing and PRBS */
#define QMOD_DBG_TOP        (1L << 5) /* Lane swap and polarity */
#define QMOD_DBG_MEM        (1L << 4) /* allocation/object */

#define QMOD_DBG_FUNCVALOUT (1L << 2) /* All values returned by Tier1*/
#define QMOD_DBG_FUNCVALIN  (1L << 1) /* All values pumped into Tier1*/
#define QMOD_DBG_FUNC       (1L << 0) /* Every time we enter a  Tier1*/

typedef struct qmod_device_aux_modes_s {
    uint32_t core_id ; 
    uint16_t st_current_entry ;
    uint16_t st_hcd[4] ;
    uint16_t st_pll_div[4] ;
    uint16_t st_os[4] ;

    uint16_t hto_enable[4]  ;
    uint16_t hto_pll_div[4] ;
    uint16_t hto_os[4] ;
} qmod_device_aux_modes_t ;

#endif
