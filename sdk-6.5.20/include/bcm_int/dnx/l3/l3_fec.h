/*
 * ! \file bcm_int/dnx/l3/l3_fec.h Internal DNX L3 FEC APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _L3_FEC_API_INCLUDED__
/*
 * {
 */
#define _L3_FEC_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/l3.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
/*
 * DEFINES
 * {
 */

/*
 * Each FEC belongs to one of three hierarchies where each hierarchy has its own FEC table (FEC_1/2/3), the following MACRO selects
 * the correct FEC table according to the hierarchy.
 */
#define L3_FEC_TABLE_FROM_HIERARCHY_GET(hierarchy) ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1) ? \
        DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY : ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2) ? \
        DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY : DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY))

/*
 *  Translating fec_id to the fec_enyry instance in super fec
 *  entry
 */
#define L3_FEC_ID_TO_FEC_INSTANCE(_fec_id_) (_fec_id_ & 1)

/** Translate fec_id to odd fec id (bitwise OR with 1) */
#define L3_FEC_ID_TO_ODD_FEC_ID(_fec_id_) (_fec_id_ | 1)

/** Translate fec_id to even fec id (divide and multiply by 2) */
#define L3_FEC_ID_TO_EVEN_FEC_ID(_fec_id_) ((_fec_id_ / 2) * 2 )

/** Return TRUE if fec id is odd */
#define L3_FEC_ID_IS_ODD_FEC_ID(_fec_id_) ((_fec_id_ % 2) == 1)

/** Return TRUE if fec id is even */
#define L3_FEC_ID_IS_EVEN_FEC_ID(_fec_id_) ((_fec_id_ % 2) == 0)

/*
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
* \brief
*   Creation function for bcm_l3_egress_create with BCM_L3_INGRESS_ONLY flag.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
*    \param [in] if_id - Similar to bcm_l3_egress_create api input
* \return
*    shr_error_e - 
*      Error return value
*/
shr_error_e dnx_l3_egress_create_fec(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_if_t * if_id);

/**
* \brief
* Retreive a L3 egress object ingress side information
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*   \param [in] egr  -    structure holding the egress object .
*        [in]    egr.flags, bit BCM_L3_HIT_CLEAR -
*                    If set then, when getting the 'hit' bit, it is also cleared.
*        [out]   egr.flags, bit BCM_L3_HIT -
*                    If set then 'hit' bit was found to have been 'set'.
*        [out]   egr.mac_addr - Next hop forwarding destination mac Next hop vlan id
*        [out]   egr.vlan - Next hop vlan id Encapsulation index
*        [out]   egr.encap_id - Encapsulation index
*        [out]   egr.port - Outgoing port id (if !BCM_L3_TGID)
*        [out]   egr.trunk - Outgoing Trunk id (if BCM_L3_TGID) General QOS map id
*        [out]   egr.qos_map_id - General QOS map id
*        [out]   egr.failover_id - Failover Object Index Failover Egress Object index
*        [out]   egr.failover_if_id - Failover Egress Object index
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_l3_egress_fec_info_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr);

/**
* \brief
* Delete L3 egress object ingress side information
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/

shr_error_e dnx_l3_egress_fec_info_delete(
    int unit,
    bcm_if_t intf);

/* L3 CINT exported API - START */

shr_error_e dnx_cint_l3_fec_hierarchy_stage_map_set(
    int unit,
    int *bank_ids,
    char *hierarchy_name);

/* L3 CINT exported API - END */
/*
 * }
 */

/*
 * }
 */
#endif/*_L3_FEC_API_INCLUDED__*/
