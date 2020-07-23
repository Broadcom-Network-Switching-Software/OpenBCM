/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    eth_lm_dm.h
 * Purpose: ETH_LM_DM definitions common to SDK and uKernel
 */

#ifndef _SOC_SHARED_ETH_LM_DM_H
#define _SOC_SHARED_ETH_LM_DM_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif



/*****************************************
 * ETH_LM_DM uController Error codes
 */
typedef enum shr_eth_lm_dm_uc_error_e {
    SHR_ETH_LM_DM_UC_E_NONE = 0,
    SHR_ETH_LM_DM_UC_E_INTERNAL,
    SHR_ETH_LM_DM_UC_E_MEMORY,
    SHR_ETH_LM_DM_UC_E_PARAM,
    SHR_ETH_LM_DM_UC_E_RESOURCE,
    SHR_ETH_LM_DM_UC_E_EXISTS,
    SHR_ETH_LM_DM_UC_E_NOT_FOUND,
    SHR_ETH_LM_DM_UC_E_UNAVAIL,
    SHR_ETH_LM_DM_UC_E_VERSION,
    SHR_ETH_LM_DM_UC_E_INIT
} shr_eth_lm_dm_uc_error_t;

#define SHR_ETH_LM_DM_UC_SUCCESS(rv)              ((rv) == SHR_ETH_LM_DM_UC_E_NONE)
#define SHR_ETH_LM_DM_UC_FAILURE(rv)              ((rv) != SHR_ETH_LM_DM_UC_E_NONE)

/*
 * Macro:
 *      SHR_ETH_LM_DM_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_ETH_LM_DM_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_ETH_LM_DM_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

/*
 * ETH_LM_DM Session Set flags
 */
#define SHR_ETH_LM_DM_SESS_SET_F_CREATE               0x00000001
#define SHR_ETH_LM_DM_SESS_SET_F_PERIOD               0x00000002
#define SHR_ETH_LM_DM_SESS_SET_F_LM                   0x00000004
#define SHR_ETH_LM_DM_SESS_SET_F_DM                   0x00000008
#define SHR_ETH_LM_DM_SESS_SET_F_PASSIVE              0x00000010
#define SHR_ETH_LM_DM_SESS_SET_F_SLM                  0x00000020


/*
 * ETH_LM_DM Encapsulation types
 */
#define SHR_ETH_LM_DM_ENCAP_TYPE_RAW      0

/*
 * Op Codes
 */

#define SHR_ETH_LM_DM_OPCODE_LMM 43
#define SHR_ETH_LM_DM_OPCODE_LMR 42
#define SHR_ETH_LM_DM_OPCODE_1DM 45
#define SHR_ETH_LM_DM_OPCODE_DMM 47
#define SHR_ETH_LM_DM_OPCODE_DMR 46
#define SHR_ETH_LM_DM_OPCODE_SLM 55
#define SHR_ETH_LM_DM_OPCODE_SLR 54
#define SHR_ETH_LM_DM_OPCODE_CFS XX

/*
 * Flags
 */
#define BCM_ETH_INC_REQUESTING_MEP_TLV         0x00000001
#define BCM_ETH_LM_SLM			               0x00000002
#define BCM_ETH_LM_SINGLE_ENDED                0x00000004
#define BCM_ETH_DM_ONE_WAY                     0x00000008
#define BCM_ETH_LM_TX_ENABLE                   0x00000010
#define BCM_ETH_DM_TX_ENABLE                   0x00000020
#define BCM_ETH_LM_UPDATE					   0x00000040
#define BCM_ETH_DM_UPDATE					   0x00000080



 /*
  * Replacement type in OLP Tx header
  */
/* OAM Replacement types */
#define BCM_ETH_LM_DM_RPL_TYPE_NONE        0 
#define BCM_ETH_LM_DM_RPL_TYPE_PTP_DM      1 
#define BCM_ETH_LM_DM_RPL_TYPE_NTP_DM      2 
#define BCM_ETH_LM_DM_RPL_TYPE_LM          3 


/*
 * Macro:
 *      SHR_ETH_LM_DM_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_ETH_LM_DM_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_ETH_LM_DM_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

#endif /* _SOC_SHARED_ETH_LM_DM_H */
