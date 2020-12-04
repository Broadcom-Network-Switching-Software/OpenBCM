/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ccm.h
 * Purpose: CCM Application definitions common to SDK and uKernel.
 */
#ifndef   _SOC_SHARED_CCM_H_
#define   _SOC_SHARED_CCM_H_

#define _FP_OAM_CCM_EVENT_THREAD_PRIO   200 /* CCM Event handler thread Prio */
#define _FP_OAM_MAX_MDL     7   /* Max num of MDLs (0-7) */
#define _FP_OAM_MAX_CTRS    3   /* Max num of ctrs that can be modified */
#define _FP_OAM_MAID_LEN    48  /* MAID Length */

/* MEPID range 1-8191 */
#define _FP_OAM_EP_NAME_MIN 1
#define _FP_OAM_EP_NAME_MAX 8191

#define _FP_OAM_MEP_TYPE_PORT       1
#define _FP_OAM_MEP_TYPE_CVLAN      2
#define _FP_OAM_MEP_TYPE_SVLAN      3
#define _FP_OAM_MEP_TYPE_S_C_VLAN   4

/* DA(12) + SA(12) + TAG1(4) + TAG2(4) + ETH_TYPE(2) + Rsvd(4) = 38 (~40) */
#define _FP_OAM_CCM_L2_ENCAP_LEN_MAX    40 /* Max L2 Encap Size */

#define _FP_OAM_OLP_L2_HDR_LEN          18 /* Size of OLP L2 Hdr */
#define _FP_OAM_OLP_OAM_TX_HDR_LEN      16 /* Size of OLP Tx OAM Hdr */
/* Size of Complete OLP Tx Header (L2 + OAM) */
#define _FP_OAM_OLP_TX_LEN              (_FP_OAM_OLP_L2_HDR_LEN + \
                                         _FP_OAM_OLP_OAM_TX_HDR_LEN)
#define _FP_OAM_OLP_OAM_RX_HDR_LEN      20 /* Size of OLP Rx OAM Hdr */
/* Size of Complete OLP Rx Header (L2 + OAM) */
#define _FP_OAM_OLP_RX_LEN              (_FP_OAM_OLP_L2_HDR_LEN + \
                                         _FP_OAM_OLP_OAM_RX_HDR_LEN)

#define _FP_OAM_OLP_CTR_WIDTH       11 /* Num bits for CNTR Idx */
#define _FP_OAM_OLP_CTR_POOL_WIDTH  3  /* Num bits for CNTR POOL */

#define _FP_OAM_OLP_CTR_LOC_IP      0  /* CTR Location IP */
#define _FP_OAM_OLP_CTR_LOC_EP      1  /* CTR Location EP */

#define _FP_OAM_OLP_CTR_ACTN_INC    1  /* CTR Action Incr */
#define _FP_OAM_OLP_CTR_ACYN_SAMPLE 2  /* CTR Action Sample */

/* Group Data Flags */
#define _FP_OAM_GROUP_SW_RDI_FLAG   (1 << 0)
#define _FP_OAM_INVALID_LMEP_ID     (-1)

/* MEP Data Flags */
#define _FP_OAM_REMOTE_MEP          (1 << 0)
#define _FP_OAM_INTERFACE_STATUS_TX (1 << 1) /* Tx Intf Status TLV */
#define _FP_OAM_PORT_STATUS_TX      (1 << 2) /* Tx Port Status TLV */
#define _FP_OAM_REMOTE_DEFECT_TX    (1 << 3) /* Tx RDI */
#define _FP_OAM_DIRECTION_UP        (1 << 4) /* UP MEP */


#endif
