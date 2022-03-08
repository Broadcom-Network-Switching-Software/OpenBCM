/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * IPMC Internal header
 */

#ifndef _BCM_INT_DPP_TRUNK_H_
#define _BCM_INT_DPP_TRUNK_H_

#include <sal/types.h>
#include <sal/core/sync.h>
#include <bcm/types.h>

#include <soc/dpp/PPD/ppd_api_lag.h>

#include <bcm_int/dpp/stack.h>
#include <shared/swstate/sw_state.h>

/* Max Trunk ID */
#define BCM_DPP_MAX_TRUNKS      (SOC_PPC_LAG_MAX_ID)
#define BCM_DPP_MAX_STK_TRUNKS      (SOC_TMC_NOF_TM_DOMAIN_IN_SYSTEM)

/* Check valid Trunk ID */
#define BCM_DPP_TRUNK_VALID(tid) (((tid) >= 0) && ((tid) < BCM_DPP_MAX_TRUNKS))

#define  BCM_DPP_TRUNK_FLAGS_MEMBER (1<<25)
#define  BCM_DPP_TRUNK_MEMBER_IS_SET(gport) (((gport) & BCM_DPP_TRUNK_FLAGS_MEMBER) != 0)
#define  BCM_DPP_TRUNK_MEMBER_SET(gport, trunk_member) ((gport) = BCM_DPP_TRUNK_FLAGS_MEMBER | (trunk_member))
#define  BCM_DPP_TRUNK_MEMBER_GET(gport) ((gport) & 0x1FFFFFF)

typedef struct bcm_dpp_trunk_private_s {
    bcm_trunk_t     trunk_id;       /* trunk group ID */
    int             in_use;         /* whether particular trunk is in use */
    int             nof_members;    /* number of members in trunk */
    int             is_stateful;    /* indicates if member is stateful - relevant for arad+ and higher - relevant for network trunk */
    int             psc;            /* indicates the port selection criteria - relevant for network trunk */
} bcm_dpp_trunk_private_t;

typedef enum trunk_init_state_e {
    ts_none,
    ts_recovering, /* phase 1 recovered, second phase recovery required to
                    * complete init sequence */
    ts_init        /* completely initialized */
} trunk_init_state_t;

typedef struct bcm_dpp_trunk_member_s {
    bcm_gport_t         system_port;    /* destination */
    uint32              flags;          /* member flags */
    uint32              member_id;      /* member id in trunk */
    uint32              trunk_id;       /* trunk id */
} bcm_dpp_trunk_member_t;

typedef struct trunk_state_s {
    trunk_init_state_t                          init_state;     /* Initialized, recovering, or none */
    int                                         ngroups;        /* max number of trunk groups */
    int                                         stk_ngroups;    /* max number of trunk groups */
    int                                         nports;         /* max port count per trunk group */
    PARSER_HINT_ARR bcm_dpp_trunk_private_t     *t_info;
    PARSER_HINT_ARR bcm_dpp_trunk_member_t      *trunk_members; /* should contain max amount of members possible (16K) */
} trunk_state_t;


/*
 * Function:
 *      _bcm_dpp_trunk_system_port_to_spa_map_get
 * Purpose:
 *      Get the spa corresponding to given system port and trunk id.
 *      The function will fail if system port is not member in given trunk.
 * Parameters:
 *      unit - (IN) Unit number.
 *      system_port - (IN) system port gport
 *      trunk_id - (IN) Trunk id.
 *      system_port_aggregate - (OUT) system port aggregate.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_dpp_trunk_system_port_to_spa_map_get(
    int unit,
    bcm_gport_t system_port,
    bcm_trunk_t trunk_id,
    uint32* system_port_aggregate);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_trunk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 	 

#endif /* _BCM_INT_DPP_TRUNK_H_ */

