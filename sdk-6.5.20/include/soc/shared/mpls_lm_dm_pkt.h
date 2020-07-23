/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mpls_lm_dm_pkt.h
 * Purpose:     MPLS_LM_DM Packet Format definitions
 *              common to SDK and uKernel.
 */

#ifndef   _SOC_SHARED_MPLS_LM_DM_PKT_H_
#define   _SOC_SHARED_MPLS_LM_DM_PKT_H_

#ifdef BCM_UKERNEL
  /* Build for uKernel not SDK */
  #include "sdk_typedefs.h"
#else
  #include <sal/types.h>
#endif
#include <shared/pack.h>

/*
 * Network Packet Format definitions
 *
 * Note: LENGTH is specified in bytes unless noted otherwise
 */
 
/* MPLS_LM_DM OLP HEADER LENGTH Tx */
#define MPLS_LM_DM_OLP_L2_HDR_LEN                   18
#define MPLS_LM_DM_OLP_OAM_HDR_LEN                  16
#define MPLS_LM_DM_OLP_HDR_LEN          (MPLS_LM_DM_OLP_L2_HDR_LEN + \
                                         MPLS_LM_DM_OLP_OAM_HDR_LEN)
/* MPLS_LM_DM OLP HEADER LENGTH Rx */
#define MPLS_LM_DM_OLP_OAM_HDR_LEN_RX               20
#define MPLS_LM_DM_OLP_HDR_LEN_RX       (MPLS_LM_DM_OLP_L2_HDR_LEN + \
                                         MPLS_LM_DM_OLP_OAM_HDR_LEN_RX)

/* MPLS_LM_DM Control Packet lengths */
#define SHR_MPLS_LM_DM_HEADER_LENGTH                4
#define SHR_MPLS_LM_DM_ACH_LENGTH                   4
#define SHR_MPLS_LM_DM_ACH_TYPE_LENGTH              2

/* Associated Channel Header */
#define SHR_MPLS_LM_DM_ACH_FIRST_NIBBLE             0x1
#define SHR_MPLS_LM_DM_ACH_VERSION                  0x0

/* ACH Types for MPLS LM/DM Feature */
#define SHR_MPLS_LM_DM_ACH_TYPE_DLM                 0x000A
#define SHR_MPLS_LM_DM_ACH_TYPE_ILM                 0x000B
#define SHR_MPLS_LM_DM_ACH_TYPE_DM                  0x000C
#define SHR_MPLS_LM_DM_ACH_TYPE_DLM_DM              0x000D
#define SHR_MPLS_LM_DM_ACH_TYPE_ILM_DM              0x000E

/* Msg Flags */
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DLM            (1<<0)
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DM             (1<<1)
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DLM_DM         (1<<2)
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM            (1<<3)
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM_DM         (1<<4)
#define SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED       (1<<5)

#define SHR_MPLS_LM_DM_PASSIVE_FLAG                 (1<<6)                          
#define SHR_MPLS_LM_DM_SINGLE_ENDED_FLAG            (1<<7)

#define SHR_MPLS_LM_DM_FLAG_LOSS_ADDED              (1<<8)
#define SHR_MPLS_LM_DM_FLAG_DELAY_ADDED             (1<<9)

#define SHR_MPLS_LM_DM_BYTE_COUNT_FLAG              (1<<10)
#define SHR_MPLS_LM_DM_64_BIT_FLAG                  (1<<11)

/* Init Flags */
#define SHR_MPLS_LM_DM_PM_MODE_RAW                  (1<<0)
#define SHR_MPLS_LM_DM_PM_MODE_STATS                (1<<1)


/* ACH type Mask */
#define SHR_MPLS_LM_DM_PKT_TYPE_MASK                0x001F

/* MPLS */
#define SHR_MPLS_LM_DM_MPLS_ROUTER_ALERT_LABEL      1
#define SHR_MPLS_LM_DM_MPLS_GAL_LABEL               13
#define SHR_MPLS_LM_DM_MPLS_PW_LABEL                0xFF
#define SHR_MPLS_LM_DM_L2_ETYPE_MPLS_UCAST          0x8847

#endif /* _SOC_SHARED_MPLS_LM_DM_PKT_H_ */
