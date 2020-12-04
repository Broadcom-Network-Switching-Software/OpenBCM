/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Failover APIs
 *
 * Purpose:
 *     Failover API for Dune Soc_petra using PPD
 */
#ifndef _BCM_INT_DPP_FAILOVER_H_
#define _BCM_INT_DPP_FAILOVER_H_

#include <bcm/types.h>
#include <bcm/failover.h>


/* Failover types to be used in order to distinguish between failover object types */
#define DPP_FAILOVER_TYPE_NONE              (0)     /* No Failover type */
#define DPP_FAILOVER_TYPE_L2_LOOKUP         (1)     /* L2 lookup (fast flush) FEC ID */
#define DPP_FAILOVER_TYPE_FEC               (2)     /* Failover ID of type FEC (1:1 UC) */
#define DPP_FAILOVER_TYPE_INGRESS           (3)     /* Failover ID of type Ingress (1+1) */
#define DPP_FAILOVER_TYPE_ENCAP             (4)     /* Failover ID of type Egress (1:1 MC) */

#define DPP_FAILOVER_TYPE_SHIFT             (29)
#define DPP_FAILOVER_VAL_SHIFT              (0)
#define DPP_FAILOVER_TYPE_MASK              (0x7)
#define DPP_FAILOVER_VAL_MASK               (0x1FFFFFFF)

#define DPP_FAILOVER_TYPE_GET(failover_type, failover_id)                               \
        ((failover_type) = (((failover_id) >> DPP_FAILOVER_TYPE_SHIFT) &                \
            DPP_FAILOVER_TYPE_MASK))                  \

#define DPP_FAILOVER_IS_L2_LOOKUP(failover_id)                                          \
        (((((failover_id) >> DPP_FAILOVER_TYPE_SHIFT) & DPP_FAILOVER_TYPE_MASK) ==      \
                DPP_FAILOVER_TYPE_L2_LOOKUP))

#define DPP_FAILOVER_L2_LOOKUP_SET(failover_id, object_id)                              \
        ((failover_id) = ((DPP_FAILOVER_TYPE_L2_LOOKUP << DPP_FAILOVER_TYPE_SHIFT) |    \
         (((object_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_TYPE_SET(object_id, failover_id, failover_type)                    \
        ((object_id) = (((failover_type) << DPP_FAILOVER_TYPE_SHIFT) |                  \
         (((failover_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_TYPE_RESET(object_id, failover_id)                                 \
        ((object_id) = ((DPP_FAILOVER_TYPE_NONE << DPP_FAILOVER_TYPE_SHIFT) |           \
         (((failover_id) & DPP_FAILOVER_VAL_MASK) << DPP_FAILOVER_VAL_SHIFT)))

#define DPP_FAILOVER_ID_GET(failover_id_val, failover_id)                               \
        ((failover_id_val) = (((failover_id) >> DPP_FAILOVER_VAL_SHIFT) &               \
            DPP_FAILOVER_VAL_MASK))

typedef struct bcm_dpp_failover_info_s {
    uint8 id_sequence[SOC_DPP_DEFS_MAX(NOF_FAILOVER_EGRESS_IDS)];
} bcm_dpp_failover_info_t;

int _bcm_dpp_failover_is_valid_id(
    int unit,
    int32 failover_id,
    int32 failover_type_match);

int _bcm_dpp_failover_state_hw_get(
    int unit,
    int32 failover_id,
    int32 failover_type,
    uint8 *failover_status);

#endif /* _BCM_INT_DPP_FAILOVER_H_ */


