/*
 * ! \file bcm_int/dnx/l3/l3_fec.h Internal DNX L3 FEC APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _L3_FEC_API_INCLUDED__
/*
 * {
 */
#define _L3_FEC_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/l3.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb.h>

/*
 * Enumeration.
 * {
 */

/**
 * A list of forwarding directions that a FEC DB can be used for.
 */
typedef enum dnx_l3_fec_directions_e
{
    /*
     * Database can be used for both - FWD and RPF, so no direction should be indicate
     */
    DNX_L3_FEC_DIRECTION_NA = 0,
    /*
     * Database can be used for forward only
     */
    DNX_L3_FEC_DIRECTION_FWD = 0,
    /*
     * Database can be used for RPF only
     */
    DNX_L3_FEC_DIRECTION_RPF = 1,
    DNX_L3_FEC_NOF_DIRECTIONS = 2
} dnx_l3_fec_directions_e;

/**
 * A list of FEC entry actions, use for dnx_l3_egress_fec_entry_update API
 */
typedef enum dnx_l3_fec_entry_actions_e
{
    DNX_L3_FEC_ENTRY_ACTION_DELETE = 0,
    DNX_L3_FEC_ENTRY_ACTION_ADD = 1
} dnx_l3_fec_entry_actions_e;

/**
 * A list of FER HW versions.
 */
typedef enum dnx_l3_fer_hw_version_e
{
    DNX_L3_FER_HW_VERSION_1 = 1,
    DNX_L3_FER_HW_VERSION_2 = 2
} dnx_l3_fer_hw_version_e;

/*
 * }
 */

/*
 * Structures
 * {
 */

/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 *  Flags for l3_fec_entry_info_t struct
 * EEI has 4 different usage:
 * EEI[23:22] = 3: encap pointer;
 * EEI[23:20] = 8: MPLS SWAP;
 * EEI[23:23] = 9: MPLS POP;
 * EEI[23] = 0: MPLS PUSH;
 */
#define L3_FEC_ENTRY_INFO_EEI_PHP             SAL_BIT(0)
#define L3_FEC_ENTRY_INFO_EEI_ENC_POINTER     SAL_BIT(1)
#define L3_FEC_ENTRY_INFO_EEI_PUSH_COMMAND    SAL_BIT(5)
/**
 * If, when calling dnx_l3_egress_fec_entry_get(), this bit is set then, upon return,
 * the flag L3_FEC_ENTRY_INFO_HIT_BIT_VALUE is set or reset depending on the value of
 * the hit bit on the table that was used. Note that, in this case, the 'hit bit' is NOT
 * reset.
 */
#define L3_FEC_ENTRY_INFO_GET_HIT_BIT         SAL_BIT(2)
/**
 * If, when calling dnx_l3_egress_fec_entry_get(), this bit is set then, upon return,
 * the flag L3_FEC_ENTRY_INFO_HIT_BIT_VALUE is set or reset depending on the value of
 * the hit bit on the table that was used. Note that, in this case, the 'hit bit' is
 * reset on that entry on that table.
 */
#define L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT   SAL_BIT(3)
/**
 * See L3_FEC_ENTRY_INFO_GET_HIT_BIT and L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT above.
 */
#define L3_FEC_ENTRY_INFO_HIT_BIT_VALUE       SAL_BIT(4)

/*
 * Get the MC RPF type according to the MC RPF flags which can be either explicit MC RPF or SIP base MC RPF, otherwise MC RPF is disabled.
 */
#define L3_EGRESS_GET_MC_RPF_TYPE(flags) ((flags & BCM_L3_MC_RPF_EXPLICIT) == BCM_L3_MC_RPF_EXPLICIT) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT : \
        ((flags & BCM_L3_MC_RPF_EXPLICIT_ECMP) == BCM_L3_MC_RPF_EXPLICIT_ECMP) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP : \
        ((flags & BCM_L3_MC_NO_RPF) == BCM_L3_MC_NO_RPF) ? DBAL_ENUM_FVAL_MC_RPF_MODE_DISABLE :\
        ((flags & BCM_L3_MC_RPF_SIP_BASE) == BCM_L3_MC_RPF_SIP_BASE) ? DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP : DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED

/*
 * Each FEC belongs to one of three hierarchies where each hierarchy has its own FEC table (FEC_1/2/3), the following MACRO selects
 * the correct FEC table according to the hierarchy.
 */
#define L3_FEC_TABLE_FROM_HIERARCHY_GET(hierarchy) ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1) ? \
        DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY : ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2) ? \
        DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY : DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY))

/** Retrieve the hierarchy enum value for the hierarchy of the FEC based on input flags */
#define DNX_L3_FEC_HIERARCHY_FROM_FLAGS_GET(flags) ((_SHR_IS_FLAG_SET(flags, BCM_L3_2ND_HIERARCHY) ? DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2 : \
         (_SHR_IS_FLAG_SET(flags, BCM_L3_3RD_HIERARCHY) ? DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3 : \
          DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1)))

/** Retrieve the direction enum value for the direction of the FEC based on input flags.
 *  In case there are not any direction flags in an indifferent direction devices FWD direction is used. */
#define DNX_L3_FEC_DIR_FROM_FLAGS_GET(flags2) ((_SHR_IS_FLAG_SET(flags2, BCM_L3_FLAGS2_RPF_ONLY) ? DNX_L3_FEC_DIRECTION_RPF : \
         (_SHR_IS_FLAG_SET(flags2, BCM_L3_FLAGS2_FWD_ONLY) ? DNX_L3_FEC_DIRECTION_FWD : \
          DNX_L3_FEC_DIRECTION_NA)))

/** Select the correct FEC table according to the hierarchy. */
#define L3_EGRESS_GET_FEC_HIERARCHICAL_TABLE(flags) _SHR_IS_FLAG_SET(flags, BCM_L3_2ND_HIERARCHY) ? \
        DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY : (_SHR_IS_FLAG_SET(flags, BCM_L3_3RD_HIERARCHY) ? \
        DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY : DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY)

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
 * Get the the MC RPF flags according to MC RPF type .
 */
#define L3_EGRESS_RPF_FLAG_GET(types) ((types == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT) ? BCM_L3_MC_RPF_EXPLICIT : \
            ((types == DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP) ? BCM_L3_MC_RPF_SIP_BASE : \
            ((types == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP) ? BCM_L3_MC_RPF_EXPLICIT_ECMP : \
            ((types == DBAL_ENUM_FVAL_MC_RPF_MODE_DISABLE) ? BCM_L3_MC_NO_RPF : 0))))

/*
 * Returns TRUE if FEC is facility protected.
 * If failover_id (protection pointer) is -2, then the FEC is facility protected
 */
#define L3_EGRESS_FAILOVER_ID_IS_FACILITY(protection_ptr) (protection_ptr == BCM_FAILOVER_ID_LOCAL)

/** Translate fec_id to odd fec id (bitwise OR with 1) */
#define L3_FEC_ID_TO_ODD_FEC_ID(_fec_id_) (_fec_id_ | 1)

/** Translate fec_id to even fec id (divide and multiply by 2) */
#define L3_FEC_ID_TO_EVEN_FEC_ID(_fec_id_) ((_fec_id_ / 2) * 2 )

/** Return TRUE if fec id is odd */
#define L3_FEC_ID_IS_ODD_FEC_ID(_fec_id_) ((_fec_id_ % 2) == 1)

/** Return TRUE if fec id is even */
#define L3_FEC_ID_IS_EVEN_FEC_ID(_fec_id_) ((_fec_id_ % 2) == 0)

/** Receive the hierarchy flags of the FEC entry using the hierarchy value */
#define L3_FEC_FLAGS_FROM_HIERARCHY_GET(hierarchy) ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3) ? \
        BCM_L3_3RD_HIERARCHY : ((hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2) ? \
        BCM_L3_2ND_HIERARCHY : 0))

/*
 * Invoke a FEC verification function only if the FEC verification mode is enabled.
 */
#define L3_FEC_INVOKE_VERIFY_IF_ENABLE(unit,_expr) DNX_SWITCH_VERIFY_IF_ENABLE_DNX(unit,_expr,bcmModuleL3EgressFec)

/*
 * EEI[21:20] = Out-LIF[17:16]
 * EEI[15:0]  = Out-LIF[15:0]
 */
#define L3_FEC_ENCAP_ID_TO_EEI_POINTER(encap_id) ((encap_id & 0xFFFF) | ((encap_id & 0x30000) << 4))
#define L3_FEC_EEI_POINTER_TO_ENCAP_ID(eei)   ((eei & 0xFFFF) | ((eei & 0x300000)>>4))

/*
 * }
 */
/*
 * Globals
 * {
 */

/*
 * }
 */

/*
 * STRUCTs
 * {
 */
typedef struct
{
    uint32 flags;                       /** L3_FEC_ENTRY_INFO_XXX*/
    uint32 fec_id;                      /** fec id*/
    dbal_fields_e fec_entry_type;       /** fec entry type, represented as DBAL_FIELDS_XXX*/
    uint32 destination;                 /** destination*/
    dbal_enum_value_field_mc_rpf_mode_e mc_rpf_mode; /**MC_RPF mode*/
    uint32 glob_out_lif;                /** global out-lif*/
    uint32 glob_out_lif_2nd;            /** 2nd global out-lif*/
    uint8 protection_enable;            /** protection indication */
    uint32 protection_ptr;              /** Protection ptr, validity according to algo*/
    uint32 stat_obj_id;                 /** statistics object id, validity according to algo*/
    uint32 stat_obj_cmd;                /** statistics object command, validity according to algo*/
    uint32 eei;                         /** EEI value, in case fec type is eei (holds dest, eei)*/
    uint32 hierarchical_gport;          /** Hierarchical TM-Flow. */
    uint8 hierarchy;                    /** FEC hierarchy - DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_*.*/
    uint8 direction;                    /** FEC direction - DNX_L3_FEC_DIRECTION_*.*/

} l3_fec_entry_info_t;
/*
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
* \brief
* Update am MPLS label structure from the bcm_l3_egress_t user input
* \param [in] unit - Relevant unit.
* \param [in] egr - user bcm_l3_egress_t input parameter holding the MPLS label values
* \param [out]  label_entry - Similar to bcm_l3_egress_create API input
*
*/
void dnx_fill_mpls_label_entry_with_l3_egr_info(
    int unit,
    bcm_l3_egress_t * egr,
    bcm_mpls_egress_label_t * label_entry);

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
 * \brief -
 * This API writes the data from fec_entry_info to HW.
 * \param [in] unit - Relevant unit.
 * \param [in] fec_entry_info - holds the fec entry type and the result fields
 * values.
 * \param [in] entry_action - Entry action indication, it can be either add or delete.

 * If (fec_entry_info->flags & L3_FEC_ENTRY_INFO_GET_CLEAR_HIT_BIT)
 *   is set then clear the 'hit bit' after adding the new entry.
 * This option is added here because the clearing is very time
 * consuming.
 */
shr_error_e dnx_l3_egress_fec_entry_update(
    int unit,
    l3_fec_entry_info_t * fec_entry_info,
    dnx_l3_fec_entry_actions_e entry_action);

/**
* \brief
* Read a L3 egress FEC entry from HW table
*   \param [in] unit - Relevant unit.
*   \param [out] fec_entry_info - Storage for fec entry.
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
*/
shr_error_e dnx_l3_egress_fec_entry_get(
    int unit,
    l3_fec_entry_info_t * fec_entry_info);

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
