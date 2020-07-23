/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    bhh.h
 * Purpose: BHH definitions common to SDK and uKernel
 *
 * Notes:   Definition changes should be avoided in order to
 *          maintain compatibility between SDK and uKernel since
 *          both images are built and loaded separately.
 */

#ifndef _SOC_SHARED_BHH_H
#define _SOC_SHARED_BHH_H

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif



/*****************************************
 * BHH uController Error codes
 */
typedef enum shr_bhh_uc_error_e {
    SHR_BHH_UC_E_NONE      = 0,
    SHR_BHH_UC_E_INTERNAL  = 1,
    SHR_BHH_UC_E_MEMORY    = 2,
    SHR_BHH_UC_E_PARAM     = 3,
    SHR_BHH_UC_E_RESOURCE  = 4,
    SHR_BHH_UC_E_EXISTS    = 5,
    SHR_BHH_UC_E_NOT_FOUND = 6,
    SHR_BHH_UC_E_UNAVAIL   = 7,
    SHR_BHH_UC_E_VERSION   = 8,
    SHR_BHH_UC_E_INIT      = 9
} shr_bhh_uc_error_t;

/*
 * BHH message types
 */
typedef enum shr_bhh_msg_type_e {
    BHH_CCM_MSG,
    BHH_LB_MSG,
    BHH_DM_MSG,
    BHH_LM_MSG,
    BHH_CSF_MSG,
    BHH_MAX_MSG_TYPE
} shr_bhh_msg_type_t;

/*
 * BHH endpoint types
 */
typedef enum shr_bhh_endpoint_type_e {
    _SHR_BHH_EP_TYPE_PW_VCCV_1,
    _SHR_BHH_EP_TYPE_PW_VCCV_2,
    _SHR_BHH_EP_TYPE_PW_VCCV_3,
    _SHR_BHH_EP_TYPE_PW_VCCV_4,
    _SHR_BHH_EP_TYPE_LSP,
    _SHR_BHH_EP_TYPE_PORT_SECTION,
    _SHR_BHH_EP_TYPE_PORT_SECTION_OUTERVLAN,
    _SHR_BHH_EP_TYPE_PORT_SECTION_INNERVLAN,
    _SHR_BHH_EP_TYPE_PORT_SECTION_OUTER_PLUS_INNERVLAN,
    _SHR_BHH_EP_TYPE_MAX
} shr_bhh_endpoint_type_t;

#define SHR_BHH_UC_SUCCESS(rv)              ((rv) == SHR_BHH_UC_E_NONE)
#define SHR_BHH_UC_FAILURE(rv)              ((rv) != SHR_BHH_UC_E_NONE)

/*
 * Macro:
 *      SHR_BHH_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_BHH_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_BHH_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

/*
 * BHH Session Set flags
 */
#define SHR_BHH_SESS_SET_F_CREATE               0x00000001
#define SHR_BHH_SESS_SET_F_LOCAL_DISC           0x00000002
#define SHR_BHH_SESS_SET_F_PERIOD               0x00000004
#define SHR_BHH_SESS_SET_F_LOCAL_MIN_RX         0x00000008
#define SHR_BHH_SESS_SET_F_LOCAL_MIN_ECHO_RX    0x00000010
#define SHR_BHH_SESS_SET_F_LOCAL_DIAG           0x00000020
#define SHR_BHH_SESS_SET_F_LOCAL_DEMAND         0x00000040
#define SHR_BHH_SESS_SET_F_LOCAL_DETECT_MULT    0x00000080
#define SHR_BHH_SESS_SET_F_SESSION_DESTROY      0x00000100
#define SHR_BHH_SESS_SET_F_SHA1_XMT_SEQ_INCR    0x00000200
#define SHR_BHH_SESS_SET_F_ENCAP                0x00000400
#define SHR_BHH_SESS_SET_F_CCM                  0x00001000
#define SHR_BHH_SESS_SET_F_LB                   0x00002000
#define SHR_BHH_SESS_SET_F_LM                   0x00004000
#define SHR_BHH_SESS_SET_F_DM                   0x00008000
#define SHR_BHH_SESS_SET_F_MIP                  0x00010000
#define SHR_BHH_SESS_SET_F_PASSIVE              0x00020000
#define SHR_BHH_SESS_SET_F_RDI                  0x00040000

/*
 * BHH Encapsulation types
 */
#define SHR_BHH_ENCAP_TYPE_RAW      0

/*
 * Op Codes
 */
#define SHR_BHH_OPCODE_CCM 1
#define SHR_BHH_OPCODE_LBM 3
#define SHR_BHH_OPCODE_LBR 2
#define SHR_BHH_OPCODE_AIS 33
#define SHR_BHH_OPCODE_LCK 35
#define SHR_BHH_OPCODE_LMM 43
#define SHR_BHH_OPCODE_LMR 42
#define SHR_BHH_OPCODE_1DM 45
#define SHR_BHH_OPCODE_DMM 47
#define SHR_BHH_OPCODE_DMR 46
#define SHR_BHH_OPCODE_CSF 52
#define SHR_BHH_OPCODE_SLM 55
#define SHR_BHH_OPCODE_SLR 54

/*
 * CSF type
 */
#define SHR_CSF_TYPE_LOS 0
#define SHR_CSF_TYPE_FDI 1
#define SHR_CSF_TYPE_RDI 2
#define SHR_CSF_TYPE_DCI 3

/*
 * Flags
 */
#define BCM_BHH_INC_REQUESTING_MEP_TLV         0x00000001
#define BCM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV  0x00000010
#define BCM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV   0x00000020
#define BCM_BHH_LBM_ICC_MEP_TLV                0x00000040
#define BCM_BHH_LBM_ICC_MIP_TLV                0x00000080
#define BCM_BHH_LBR_ICC_MEP_TLV                0x00000100
#define BCM_BHH_LBR_ICC_MIP_TLV                0x00000200
#define BCM_BHH_LM_SINGLE_ENDED                0x00000400
#define BCM_BHH_DM_ONE_WAY                     0x00000800
#define BCM_BHH_LM_TX_ENABLE                   0x00001000
#define BCM_BHH_DM_TX_ENABLE                   0x00002000
#define BCM_BHH_LM_SLM                         0x00004000
#define BCM_BHH_TX_ENABLE                      0x00008000

 /*
 */
#define BCM_BHH_DM_TYPE_NTP                0
#define BCM_BHH_DM_TYPE_PTP                1

/*
 * Macro:
 *      SHR_BHH_IF_ERROR_RETURN
 * Purpose:
 *      Evaluate _op as an expression, and if an error, return.
 * Notes:
 *      This macro uses a do-while construct to maintain expected
 *      "C" blocking, and evaluates "op" ONLY ONCE so it may be
 *      a function call that has side affects.
 */

#define SHR_BHH_IF_ERROR_RETURN(op)                                     \
    do {                                                                \
        int __rv__;                                                     \
        if ((__rv__ = (op)) != SHR_BHH_UC_E_NONE) {                     \
            return(__rv__);                                             \
        }                                                               \
    } while(0)

#define SHR_BHH_SESSION_UPDATE_CCM_RDI               0x00000001
#define SHR_BHH_SESSION_UPDATE_ENCAP                 0x00000002
#define SHR_BHH_SESSION_UPDATE_PROACTIVE_LM_ENABLE   0x00000004
#define SHR_BHH_SESSION_UPDATE_PROACTIVE_LM_DISABLE  0x00000008

#endif /* _SOC_SHARED_BHH_H */
