/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_STG_H_
#define _BCM_INT_DPP_STG_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <shared/swstate/sw_state.h>

/*
 * The STG info structure is protected by STG_DB_LOCK. The hardware PTABLE and
 * hardware STG table are protected by memory locks in the lower level.
 */
typedef struct bcm_stg_info_s {
                    int         init;       /* TRUE if STG module has been initialized */
                    bcm_stg_t   stg_min;    /* STG table min index */
                    bcm_stg_t   stg_max;    /* STG table max index */
                    bcm_stg_t   stg_defl;   /* Default STG */
    PARSER_HINT_ARR SHR_BITDCL *stg_bitmap; /* Bitmap of allocated STGs */
    PARSER_HINT_ARR bcm_pbmp_t *stg_enable; /* array of port bitmaps indicating whether the
                                               port+stg has STP enabled */
    PARSER_HINT_ARR bcm_pbmp_t *stg_state_h;/* array of port bitmaps indicating STP state for the */
    PARSER_HINT_ARR bcm_pbmp_t *stg_state_l;/* port+stg combo. Only valid if stg_enable = TRUE */
                    int         stg_count;  /* Number STGs allocated */
                    /* STG reverse map - keep a linked list of VLANs in each STG */
    PARSER_HINT_ARR bcm_vlan_t *vlan_first; /* Indexed by STG (also links free list) */
    PARSER_HINT_ARR bcm_vlan_t *vlan_next;  /* Indexed by VLAN ID */
} bcm_stg_info_t;

/* Convert STG to PPD stg topology id */
#define BCM_DPP_STG_TO_TOPOLOGY_ID(stg) \
            (stg - 1)

/* Convert PPD stg topology id to STG */
#define BCM_DPP_STG_FROM_TOPOLOGY_ID(topology_id) \
            (topology_id + 1)

extern bcm_stg_info_t  stg_info[BCM_MAX_NUM_UNITS];

extern int _bcm_petra_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid);

extern int bcm_petra_stg_detach(int unit);

#endif /* _BCM_INT_DPP_IPMC_H_ */

