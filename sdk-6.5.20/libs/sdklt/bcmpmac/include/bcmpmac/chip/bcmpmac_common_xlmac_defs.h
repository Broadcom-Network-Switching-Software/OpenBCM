
/*! \file bcmpmac_common_xlmac_defs.h
 *
 * XLMAC common definitions.
 *
 * This file contains all the common definitions for XLMAC across different
 * device. Please make sure to add only the common definitions and any device
 * specific XLMAC definitions should go into device specific XLMAC header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_COMMON_XLMAC_DEFS_H
#define BCMPMAC_COMMON_XLMAC_DEFS_H

/*! Common XLMAC Receive statistic Vector (RSV) macros. */
#define XLMAC_RSV_MASK_WRONG_SA             (0x1 << (0))
#define XLMAC_RSV_MASK_STACK_VLAN_DETECT    (0x1 << (1))
/* unsupported PFC DA*/
#define XLMAC_RSV_MASK_PFC_DA_ERR           (0x1 << (2))
/* same bit as PFC */
#define XLMAC_RSV_MASK_PAUSE_DA_ERR         (0x1 << (2))
#define XLMAC_RSV_MASK_RCV_TERM_OR_CODE_ERR (0x1 << (3))
#define XLMAC_RSV_MASK_CRC_ERR              (0x1 << (4))
/* IEEE length check fail */
#define XLMAC_RSV_MASK_FRAME_LEN_ERR        (0x1 << (5))
/* truncated/out of range */
#define XLMAC_RSV_MASK_TRUNCATED_FRAME      (0x1 << (6))
#define XLMAC_RSV_MASK_FRAME_RCV_OK         (0x1 << (7))
#define XLMAC_RSV_MASK_MCAST_FRAME          (0x1 << (8))
#define XLMAC_RSV_MASK_BCAST_FRAME          (0x1 << (9))
#define XLMAC_RSV_MASK_PROMISCUOUS_FRAME    (0x1 << (10))
#define XLMAC_RSV_MASK_CONTROL_FRAME        (0x1 << (11))
#define XLMAC_RSV_MASK_PAUSE_FRAME          (0x1 << (12))
#define XLMAC_RSV_MASK_OPCODE_ERR           (0x1 << (13))
#define XLMAC_RSV_MASK_VLAN_TAG_DETECT      (0x1 << (14))
#define XLMAC_RSV_MASK_UCAST_FRAME          (0x1 << (15))
#define XLMAC_RSV_MASK_RX_FIFO_FULL         (0x1 << (16))
#define XLMAC_RSV_MASK_RUNT_FRAME           (0x1 << (17))
#define XLMAC_RSV_MASK_PFC_FRAME            (0x1 << (18))
#define XLMAC_RSV_MASK_SCH_CRC_ERR          (0x1 << (19))
#define XLMAC_RSV_MASK_MACSEC_FRAME         (0x1 << (20))
#define XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR   (0x1 << (21))

#define XLMAC_RSV_MASK_MIN     XLMAC_RSV_MASK_WRONG_SA
#define XLMAC_RSV_MASK_MAX     XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR
#define XLMAC_RSV_MASK_ALL     ((XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR) | \
                               ((XLMAC_RSV_MASK_DRIBBLE_NIBBLE_ERR) - 1))

#endif /* BCMPMAC_COMMON_XLMAC_DEFS_H */
