/*! \file bcmpmac_common_cdmac_defs.h
 *
 * CDMAC common definitions.
 *
 * This file contains all the common definitions for CDMAC across different
 * device. Please make sure to add only the common definitions and any device
 * specific CDMAC definitions should go into device specific CDMAC header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_COMMON_CDMAC_DEFS_H
#define BCMPMAC_COMMON_CDMAC_DEFS_H

/*! Common CDMAC Receive statistic Vector (RSV) macros.  */
#define CDMAC_RSV_MASK_WRONG_SA             (0x1 << (0))
#define CDMAC_RSV_MASK_STACK_VLAN_DETECT    (0x1 << (1))
/* unsupported PFC DA*/
#define CDMAC_RSV_MASK_PFC_DA_ERR           (0x1 << (2))
/* same bit as PFC */
#define CDMAC_RSV_MASK_PAUSE_DA_ERR         (0x1 << (2))
#define CDMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR (0x1 << (3))
#define CDMAC_RSV_MASK_CRC_ERR              (0x1 << (4))
/* IEEE length check fail */
#define CDMAC_RSV_MASK_FRAME_LEN_ERR        (0x1 << (5))
/* truncated/out of range */
#define CDMAC_RSV_MASK_TRUNCATED_FRAME      (0x1 << (6))
#define CDMAC_RSV_MASK_FRAME_RCV_OK         (0x1 << (7))
#define CDMAC_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define CDMAC_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define CDMAC_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define CDMAC_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define CDMAC_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define CDMAC_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define CDMAC_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define CDMAC_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define CDMAC_RSV_MASK_RESERVED_1           (0x1 << (16))
#define CDMAC_RSV_MASK_RUNT_FRAME           (0x1 << (17))
#define CDMAC_RSV_MASK_PFC_FRAME            (0x1 << (18))

#define CDMAC_RSV_MASK_MIN           CDMAC_RSV_MASK_WRONG_SA
#define CDMAC_RSV_MASK_MAX           CDMAC_RSV_MASK_PFC_FRAME
#define CDMAC_RSV_MASK_ALL           ((CDMAC_RSV_MASK_PFC_FRAME) | \
                                     ((CDMAC_RSV_MASK_PFC_FRAME) - 1))

#endif /* BCMPMAC_COMMON_CDMAC_DEFS_H */
