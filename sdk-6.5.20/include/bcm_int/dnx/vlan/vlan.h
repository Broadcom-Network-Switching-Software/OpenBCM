/**
 * \file bcm_int/dnx/vlan/vlan.h
 *
 * Internal DNX VLAN APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#ifndef _VLAN_API_INCLUDED__
/*
 * {
 */
#define _VLAN_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/**
 * Defines for virtual gport subfields - VLAN-Domain and VSI in ESEM subtype and
 * AC-Profile in Egress Port Default subtype
 * Those won't be required once the ESEM entry will have a hashed representation
 */

#define VIRTUAL_ESEM_GPORT_VD_SHIFT                        (0)
#define VIRTUAL_ESEM_GPORT_VD_MASK                      (0xFF)

#define VIRTUAL_ESEM_GPORT_VSI_SHIFT                       (8)
#define VIRTUAL_ESEM_GPORT_VSI_MASK                   (0x3FFF)

#define VIRTUAL_EGRESS_PORT_DEFAULT_AC_PROFILE_SHIFT       (0)
#define VIRTUAL_EGRESS_PORT_DEFAULT_AC_PROFILE_MASK     (0xFF)
#define BCM_DNX_PCP_MAX                                    (7)
#define BCM_DNX_PCP_INVALID                                (BCM_DNX_PCP_MAX+1)

/**
 * \brief
 *      Temporary value - setting FODO (Forward Domain) Assignment Mode to INVALID
 *      just before setting it to correct value.
 *      Used for checking that the FODO (Forward Domain) Assignment Mode is set to a valid value.
 */
#define FODO_ASSIGNMENT_MODE_INVALID   (-1)

/*
 * }
 */
/*
 * MACROs
 * {
 */

/**
 * \brief
 * Verify vlanId is within range (0-4K, not including 4K).
 */
#define BCM_DNX_VLAN_CHK_ID(unit, vid) { \
  if (vid > BCM_VLAN_MAX) SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VID\r\n"); \
  }

/**
 * \brief
 * Verify vlanId is within range (0-4K, not including 4K).
 */
#define BCM_DNX_PCP_CHK_ID(unit, pcp) { \
  if (pcp > BCM_DNX_PCP_MAX) SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PCP\r\n"); \
  }

/**
 * \brief
 * Verify vlanId is none (0).
 */
#define BCM_DNX_VLAN_CHK_ID_NONE(unit, vid, err_code) {                    \
    if ((vid) == BCM_VLAN_NONE)                                            \
    {                                                                      \
        if (SHR_FAILURE(err_code))                                         \
        {                                                                  \
            LOG_ERROR_EX(BSL_LOG_MODULE,                                   \
                         " Error: %s, because vlan_id is NONE %s%s%s\r\n", \
                         shrextend_errmsg_get(err_code), EMPTY, EMPTY, EMPTY); \
        }                                                                  \
        _func_rv = err_code;                                               \
        SHR_EXIT();                                                        \
    }                                                                      \
}

/**
 * \brief
 * Check that the ESEM result-type is NAMESPACE-VSI
 */
#define BCM_DNX_ESEM_IS_RESULT_TYPE_NAMESPACE_VSI(resultType) \
    ((resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO) || \
     (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS))

/**
 * \brief
 * Is ESEM Entry size should be "large" (ETPS_AC_STAT) or not (ETPS_AC):
 * For ETPS_AC_STAT:
 *   - "two vlans" flag is set. Or
 *   - "statistic enable" flag is set. Or
 *   - nwk_qos_idx is not invalid.
 * Else ETPS_AC.
 */
#define BCM_DNX_ESEM_IS_LARGE_ENTRY_NEEDED(flags,nwk_qos_idx)  \
    (_SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS)) || \
    (_SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE)) || \
    (nwk_qos_idx != DNX_QOS_INVALID_MAP_ID);

/*
 * }
 */

/*
 * INERNAL TYPES
 * {
 */

/**
 * \brief
 *  Virtual Native-AC DB in ESEM - key values.
 */
typedef struct
{
    /**
     * \brief
     *   Local out-LIF field of the key
     */
    uint32 local_out_lif;
    /**
     * \brief
     *   VSI field of the key
     */
    uint32 vsi;
} dnx_egress_virtual_native_ac_info_t;

/**
 * \brief
 *  Virtual ingress native AC are not managed by GPort mgmt.
 *  This structure can be used to get similar information.
 */
typedef struct
{
    /**
     * \brief
     *   Local In-LIF. Key to the table below.
     */
    uint32 local_in_lif;
    /**
     * \brief
     *   InLIF App-DB logical table ID.
     */
    uint32 dbal_table_id;
    /**
     * \brief
     *   Result type in the App-DB
     */
    uint32 dbal_result_type;
} dnx_ingress_virtual_native_ac_info_t;
typedef struct
{
    uint32 vlan_range_outer_index;
    uint32 vlan_range_outer_mask;
    uint32 vlan_range_inner_index;
    uint32 vlan_range_inner_mask;
} dnx_vlan_domain_info_t;

typedef struct
{
    uint32 vlan_range_lower;
    uint32 vlan_range_upper;
    uint8 is_outer;
    uint8 is_add;

} dnx_vlan_range_info_t;

/*
 * }
 */
/**
 * \brief
 * Initialize BCM VLAN module
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT
 *   * Initialized BCM VLAN module
 * \remark
 *   * The function is used on init time
 */
shr_error_e dnx_vlan_init(
    int unit);

/**
 * \brief
 * Set default vlan value to the sw state, based on the incoming vid
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *    \param [in] vid -
 *     Relevant vid.
 *    \param [in] is_init -
 *     Indicates if the function is called on init time or not
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Write to SW state the value of the default vlan on init and non init time
 * \remark
 *   * None
 */
shr_error_e dnx_vlan_default_set_internal(
    int unit,
    bcm_vlan_t vid,
    int is_init);

/**
 * \brief - Configure VLAN PORT default ingress configuration
 *          (ac inlif) per port
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 *
 * Parameters:
 *              - bcm_gport_t port(in)
 *              - bcm_gport_t local_in_lif(in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_default(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x VLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x INNER_VLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * \param [in] unit - relevant unit.
 * \param [in] port - port for match,
 * \param [in] match_c_vlan - customer vlan for match.
 * \param [in] local_in_lif - Ac lif, the match result.
 * \param [in] is_local_in_lif_update - Indication to only
 *        Update the local In-LIF for an existing entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_port_create
 * bcm_dnx_port_match_add
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_c_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_c_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x untag ingress configuration.
 *          ISEM lookup (port X untag), result is ingress ac
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 *
 * Parameters:
 *              - bcm_gport_t port(in)
 *              - bcm_gport_t local_in_lif(in)
 *              - int is_local_in_lif_update(in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_untagged(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x CVLAN x CVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t bcm_gport_t (in)
 *              - int is_local_in_lif_update (in) - Indication
 *                to only update the local In-LIF for an
 *                existing entry.
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x SVLAN x SVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t bcm_gport_t (in)
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x VLAN x VLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t bcm_gport_t (in)
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x VLAN x VLAN x PCP x ETHERTYPE in ingress configuration.
 *          TCAM lookup, result is ac inlif
 *
 * \param [in] unit - relevant unit.
 * \param [in] port - port for match,
 * \param [in] match_vlan - outer vlan for match.
 * \param [in] match_inner_vlan - outer vlan for match.
 * \param [in] match_ethertype - outer vlan for match.
 * \param [in] match_pcp - outer vlan for match.
 * \param [in] local_in_lif - Ac lif, the match result.
 * \param [in] is_local_in_lif_update - Indication to only
 *        Update the local In-LIF for an existing entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_port_create
 * bcm_dnx_port_match_add
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief - Configure PORT x TUNNEL in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_unmatched(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_untagged(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL x VLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL x CVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL x SVLAN x CVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL x CVLAN x CVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif);

/**
 * \brief - Configure PORT x TUNNEL x SVLAN x SVLAN in ingress configuration.
 *          ISEM lookup, result is ac inlif
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 *              - bcm_dnx_port_match_add
 * Parameters:
 *              - bcm_gport_t port (in)
 *              - bcm_vlan_t tunnel_id (in)
 *              - bcm_vlan_t match_vlan (in)
 *              - bcm_vlan_t match_inner_vlan (in)
 *              - bcm_gport_t local_in_lif (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif);

/**
 * \brief -
 * This function returns the default (local) lif value of the given port
 */
shr_error_e dnx_vlan_control_port_default_lif_get(
    int unit,
    int port,
    uint32 *default_lif);

/**
 * \brief
 *   Set default LIF for the given pp-port
 */
shr_error_e dnx_vlan_control_port_default_lif_set(
    int unit,
    int port,
    uint32 default_lif);

/**
 * \brief -
 * This function updates SW table with the local lif value of the created initial bridge In-LIF
 */
shr_error_e dnx_vlan_port_ingress_initial_bridge_lif_set(
    int unit,
    int initial_bridge_lif);

/**
 * \brief -
 * This function returns the local lif value of the created initial bridge In-LIF
 */
shr_error_e dnx_vlan_port_ingress_initial_bridge_lif_get(
    int unit,
    int *initial_bridge_lif);

/**
 * \brief -
 * This function updates SW table with the local lif value of the created initial default In-LIF
 */
shr_error_e dnx_vlan_port_ingress_initial_default_lif_set(
    int unit,
    int initial_default_lif);

/**
 * \brief -
 * This function returns the local lif value of the created initial default In-LIF
 */
shr_error_e dnx_vlan_port_ingress_initial_default_lif_get(
    int unit,
    int *initial_default_lif);

/**
 * \brief -
 * This function returns the local lif value of the
 * initial default recycle port In-LIF.
 * Initial default recycle port in-lif is created at init.
 */
shr_error_e dnx_vlan_port_ingress_default_recyle_lif_get(
    int unit,
    uint32 *default_recycle_lif);

/**
 * \brief -
 * Initialization function for VLAN-PORT module.
 * This function creates an Ingress VLAN-Port (In-LIF) to be used as
 * default In-LIF per port (all ETH PP ports are set to this In-LIF at initialization).
 * The default In-LIF properites are:
 *      - ingress only.
 *      - VSI = VID.
 *      - VLAN Edit profile = 0
 *
 * DBAL SW database is updated with the local lif value of this LIF.
 * In addtion, drop In-LIF is initialized to be used as a LIF for dropping a packet per port.
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 * None
 * \see
 * dnx_vlan_init
 * dnx_port_initial_eth_properties_set
 * vlan_port_db.in_lif.default_in_lif
 * dnx_data_lif.in_lif.drop_in_lif
 */
shr_error_e dnx_vlan_port_default_init(
    int unit);

/**
 * \brief -
 * Initialization function for VLAN-PORT module.
 * Allocates and sets Default ESEM reserved AC entries. Those
 * entries can't be deleted.
 * The entries are set mostely with reset value.
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 * None
 * \see
 * dnx_vlan_init
 */
shr_error_e dnx_vlan_port_default_esem_init(
    int unit);
/**
 * \brief
 *  Set VSI table entry to its default values
 *
 * \param [in] unit -
 *  Relevant unit.
 * \param [in] vsi -
 *   The incoming VSI ID, must be in the range from 0-nof_vsis.
 *   VSI - Virtual Switching Instance is a generalization of the VLAN concept used primarily in advanced bridging application.
 *   A VSI interconnects Logical Interfaces (LIFs). VSI is a logical partition of the MAC table and a flooding domain
 *   (comprising its member interfaces). For more information about VSI , see the Programmer's Guide PP document.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vsi_table_default_set(
    int unit,
    bcm_vlan_t vsi);

/**
 * \brief
 *  Clear VSI fields to its default values
 *  Should be called when VSI is destroy (the opposite of dnx_vsi_table_default_set)
 *
 * \param [in] unit -
 *  Relevant unit.
 * \param [in] vsi -
 *   The incoming VSI ID, must be in the range from 0-nof_vsis.
 *   VSI - Virtual Switching Instance is a generalization of the VLAN concept used primarily in advanced bridging application.
 *   A VSI interconnects Logical Interfaces (LIFs). VSI is a logical partition of the MAC table and a flooding domain
 *   (comprising its member interfaces). For more information about VSI , see the Programmer's Guide PP document.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vsi_table_clear_set(
    int unit,
    bcm_vlan_t vsi);

/**
 * \brief - Iinitlize the Vlan range template
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vlan_range_template_init(
    int unit);
/**
 * \brief - Clears match in ingress non-native configuration.
 * - Port is set back to default-LIF
 * - ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 *  bcm_dnx_port_match_delete
 */
shr_error_e dnx_vlan_port_destroy_ingress_non_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port);

/**
 * \brief - Clears match in ingress native configuration.
 * - ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 *  bcm_dnx_port_match_delete
 */
shr_error_e dnx_vlan_port_destroy_ingress_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port);

/**
 * \brief -
 * This function gets from SW data base the ESEM Virtual LIF match-info: Native/non-Native/Namespace-vsi/Namespace-port.
 * It "gets" from the SW handle "ALL fields" so consecutive code can get it's relevant fields.
 * \param [in] unit -
 * \param [in] gport - ESEM Virtual GPort which is the handle for the SW DB.
 * \param [in] sw_table_handle_id - the sw table entry containing all info.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_translation_set
 * bcm_dnx_vlan_port_translation_get
 * bcm_dnx_gport_stat_set
 * bcm_dnx_gport_stat_get
 */
shr_error_e dnx_vlan_port_egress_virtual_ac_match_info_get(
    int unit,
    bcm_gport_t gport,
    uint32 sw_table_handle_id);

/**
 * \brief -
 * This function sets ESEM table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_translation_set
 * bcm_dnx_vlan_port_translation_get
 * bcm_dnx_gport_stat_set
 * bcm_dnx_gport_stat_get
 */
shr_error_e dnx_vlan_port_egress_virtual_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id);

/**
 * \brief -
 * This function sets ESEM Namespace-vsi table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info
 * \param [out] entry_handle_id -ESEM table handler updated with the access keys
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 * bcm_dnx_vlan_port_translation_get
 */
shr_error_e dnx_vlan_port_egress_virtual_namespace_vsi_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id);

/**
 * \brief -
 * This function gets the ESEM Namespace-vsi access keys from the given SW table.
 * In addition, it sets the ESEM acces keys (if the ESEM table handler is not NULL).
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info.
 * \param [in] entry_handle_id -ESEM table handler updated with the access keys.
 * \param [out] vsi -pointer to the vsi.
 * \param [out] match_class_id -pointer to the namespace.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
shr_error_e dnx_vlan_port_egress_virtual_namespace_vsi_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_t * vsi,
    uint32 *match_class_id);

/**
 * \brief -
 * This function sets ESEM Namespace-port table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info
 * \param [out] entry_handle_id -ESEM table handler updated with the access keys
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_translation_set
 * bcm_dnx_vlan_port_translation_get
 * bcm_dnx_gport_stat_set
 * bcm_dnx_gport_stat_get
 */
shr_error_e dnx_vlan_port_egress_virtual_namespace_port_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id);

/**
 * \brief -
 * This function sets ESEM native table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info
 * \param [in] entry_handle_id - ESEM table handler updated with the access keys
 * \param [out] vlan_port - pointer to vlan port. Can be NULL if not make sense.
 * \return shr_error_e Standard error handeling
 * \see
 * None
 */
shr_error_e dnx_vlan_port_egress_virtual_native_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id,
    bcm_vlan_port_t * vlan_port);

/*
 * This function returns the gport value of the Ingress Virtual Native LIF for the given local in-lif
 * \see
 * bcm_dnx_vlan_port_find
 * bcm_dnx_vlan_port_traverse
 * bcm_dnx_vswitch_cross_connect_traverse
 */
shr_error_e dnx_vlan_port_find_ingress_native_virtual_match_sw_state(
    int unit,
    int local_in_lif,
    bcm_gport_t * vlan_port_id);

/**
* \brief
*  Allocates inlif-profile.
*  It takes the inlif-profile and updates its in_lif_orientation value.
*  A new inlif-profile may be allocated (by the "exchange" function).
* \see
*  bcm_dnx_vlan_port_create
*/
shr_error_e dnx_ingress_inlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int native_indexed_mode,
    int in_lif_profile,
    int *new_in_lif_profile,
    dbal_tables_e inlif_dbal_table_id);

/**
* \brief
*  Allocates outlif-profile.
*  It takes default outlif-profile and updates its out_lif_orientation value.
*  A new outlif-profile may be allocated (by the "exchange" function).
* \see
*  bcm_dnx_vlan_port_create
*/
shr_error_e dnx_egress_outlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int out_lif_profile,
    int *new_out_lif_profile,
    dbal_tables_e dbal_table_id,
    uint32 flags);

/**
 * \brief
 *  Replaces a created Ingress P2P VLAN PORT to Ingress P2P VLAN PORT -
 *  for the case were the new forward information is different then current forward information, the In-LIF
 *  is "replaced" to accommodate the new forward information.
 * \param [in] unit - Unit #
 * \param [in] gport1 -the MP-LIF's gport to be changed to P2P-LIF.
 * \param [in] entry_handle_id -table handler to be used to update the fields and commit.
 * \param [in] forward_info2 -pointer to the forward-information of the destiantion port.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
shr_error_e dnx_vlan_port_replace_ingress_p2p_to_p2p(
    int unit,
    bcm_gport_t gport1,
    uint32 entry_handle_id,
    dnx_algo_gpm_forward_info_t * forward_info2);

/**
 * \brief
 *  Replaces a created Ingress MP VLAN PORT to Ingress P2P VLAN PORT.
 * \param [in] unit - Unit #
 * \param [in] gport1 -the MP-LIF's gport to be changed to P2P-LIF.
 * \param [in] forward_info2 -pointer to the forward-information of the destiantion port.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 * bcm_dnx_vswitch_cross_connect_add
 */
shr_error_e dnx_vlan_port_replace_ingress_mp_to_p2p(
    int unit,
    bcm_gport_t gport1,
    dnx_algo_gpm_forward_info_t * forward_info2);

/**
 * \brief
 *  Replaces a created Ingress P2P VLAN PORT to Ingress MP VLAN PORT.
 * \param [in] unit - Unit #
 * \param [in] gport -the P2P-LIF's gport to be changed to MP-LIF.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 * bcm_dnx_vswitch_cross_connect_delete
 */
shr_error_e dnx_vlan_port_replace_ingress_p2p_to_mp(
    int unit,
    bcm_gport_t gport);

/**
 * \brief - Set the match HW entry for a VLAN-Port In-LIF.
 *
 *   \param [in] unit - unit id
 *   \param [in] vlan_port - Pointer to the VLAN-Port struct the
 *          stores all the required match parameters.
 *   \param [in] local_in_lif_id - The value of the local In-LIF
 *          ID to be set as the match result.
 *   \param [in] is_local_in_lif_update - Indication to only
 *          Update the local In-LIF for an existing entry.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_vlan_port_create_ingress
 *   dnx_vlan_port_match_inlif_update
 *   bcm_dnx_port_match_add
 *   dnx_port_match_inlif_update
 */
shr_error_e dnx_vlan_port_match_inlif_set(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 local_in_lif_id,
    int is_local_in_lif_update);

/*
 * }
 */
#endif/*_VLAN_API_INCLUDED__*/
