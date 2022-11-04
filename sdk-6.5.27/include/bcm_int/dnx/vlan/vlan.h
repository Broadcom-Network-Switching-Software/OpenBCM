/**
 * \file bcm_int/dnx/vlan/vlan.h
 *
 * Internal DNX VLAN APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

#ifndef _VLAN_API_INCLUDED__
/*
 * {
 */
#define _VLAN_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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

#define BCM_DNX_PCP_MAX                                    (7)
#define BCM_DNX_PCP_INVALID                                (BCM_DNX_PCP_MAX+1)

/**
 * \brief
 *      Temporary value - setting FODO (Forward Domain) Assignment Mode to INVALID
 *      just before setting it to correct value.
 *      Used for checking that the FODO (Forward Domain) Assignment Mode is set to a valid value.
 */
#define FODO_ASSIGNMENT_MODE_INVALID   (-1)

#define NOF_MAX_BITS_VSI_60B_RESULT_TYPES   60
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

/**
 * \brief
 *   Enumeration for the various AC applications:
 *   SERVICE - Symmetrical LIF that enables learning
 *   VLAN_TRANSLATION - Performs Egress/Ingress VLAN edit only
 *   OPTIMIZATION - Aggregates VLANs on a port to one In-LIF
 */
typedef enum
{
    INVALID_AC_APPLICATION_TYPE = -1,
    FIRST_AC_APPLICATION_TYPE = 0,
    /**
     *  Symmetrical LIF that enables learning
     */
    SERVICE_AC_APPLICATION_TYPE = FIRST_AC_APPLICATION_TYPE,
    /**
     *  Symmetrical LIF that aggregates VLANs on a port to one
     *  In-LIF
     */
    SERVICE_OPTIMIZATION_AC_APPLICATION_TYPE,
    /**
     *  Performs Egress/Ingress VLAN edit only
     */
    VLAN_TRANSLATION_AC_APPLICATION_TYPE,
    /**
     *  Asymmetrical LIF that aggregates VLANs on a port to one
     *  In-LIF
     */
    OPTIMIZATION_AC_APPLICATION_TYPE,
    NUM_AC_APPLICATION_TYPE_E
} ac_application_type_e;

/**
 * \brief
 * Enumeration for the valid VSI types
 */
typedef enum
{
    VSI_TYPE_INVALID = -1,
    VSI_TYPE_BRIDGE_ONLY = 0,
    VSI_TYPE_ROUTE_ONLY = 1,
    NOF_VSI_TYPES
} dnx_vsi_intf_type_e;

/**
 * \brief
 * Enumeration for the VSI fields that have different sizes in 60b VSI result types
 */

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
 *
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
 *
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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_unmatched(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_untagged(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

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
 *              - int is_local_in_lif_update (in)
 */
shr_error_e dnx_vlan_port_create_ingress_match_port_tunnel_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update);

/**
 * \brief -
 * This function converts EtherType value to DBAL layer_type value
 */
shr_error_e dnx_vlan_port_dbal_ethertype_convert(
    int unit,
    bcm_port_ethertype_t match_ethertype,
    uint32 *layer_type_val);

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
    int is_p2p,
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
 * \brief
 *  Decide (ingress) VSI entry initial result type
 * \param [in] unit - Relevant unit.
 * \param [in] vsi - The incoming VSI ID.
 * \param [in] vsi_type - Indicate if the result type is for an L2 VSI or an L3.
 * \param [out] result_type - DBAL table result type.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vsi_table_initial_result_type_get(
    int unit,
    bcm_vlan_t vsi,
    dnx_vsi_intf_type_e vsi_type,
    uint32 *result_type);

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
 * This function gets the ESEM access keys from the given SW table.
 * In addition, it sets the ESEM access keys (if the ESEM table handler is not NULL).
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \param [out] vlan_port -pointer to vlan_port structure to update its vsi, match_vlan and port fields.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 */
shr_error_e dnx_vlan_port_egress_virtual_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port);

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
    int lif_on_lag_is_shown,
    int oam_primary_vlan_mode,
    int native_indexed_mode,
    int in_lif_profile,
    int *new_in_lif_profile,
    dbal_tables_e inlif_dbal_table_id,
    dnx_in_lif_profile_last_info_t * last_profile);

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
    uint32 flags,
    uint8 *is_last);

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

/**
 * \brief
 * Internal function to configure EGRESS VLAN Translation
 * parameters for a virtual ESEM Namespace-vsi LIF.
 * \see
 * bcm_dnx_vlan_port_translation_set
 * bcm_dnx_vlan_port_destroy
 */
shr_error_e dnx_vlan_port_translation_set_egress_esem_namespace_vsi(
    int unit,
    uint32 esem_handle_id,
    bcm_vlan_port_translation_t * vlan_port_translation);

/*
* \brief
*  This function returns the gport value of the ESEM LIF that is pointed by the given input:
*  - criteria - the matching criteria
*  - flags - indicates native/non-native
*  - vsi X local_out_lif (via port): for ESEM native.
*  - vsi X vlan X vlan_domain (port): for ESEM non-native
*  - vsi X vlan X vlan_domain (network_domain): for ESEM Namespace-vsi
*  - system_port X vlan_domain (namespace): for ESEM Namespace-port
*
*   \param [in] unit             -  Relevant unit.
*   \param [in,out] vlan_port    -  vlan port information.
*   \param [in] use_parameter    -  If the parameter_value should be used..
*   \param [in] parameter_value  -  given parameter.
*   \param [in] not_found_log_enable  -  If to print an error log if the entry is not found.
* \return
*   shr_error_e - Non-zero in case of an error.
* \remark
*  - use_parameter, parameter_value: use the parameter value instead of the value at vlan_port
* use_parameter distinguishes between find case where the global Out-LIF is supplied, so we need to deduct the local Out-LIF,
* and a lookup like traverse where we find the entry from the internal DB in the form of a local Out-LIF.
* \see
* bcm_dnx_vlan_port_find
*/
shr_error_e dnx_vlan_port_find_egress_match_esem_sw_state(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int use_parameter,
    uint32 parameter_value,
    uint8 not_found_log_enable);

/**
*  See vlan.h for function description.
*/
shr_error_e dnx_vlan_port_set_ll_app_type(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e * ll_app_type);

/**
 * \brief
 * Internal function for calculating Ingress Forwarding Domain (FODO) assignment mode.
 * \param [in] unit - Unit #
 * \param [in] vlan_port - vlan port info.
 * \param [in] ll_app_type - Link-Layer LIF application type.
 * \param [out] fodo_assignment_mode - Forwarding Domain (FODO)
 *        assignment mode.
 * \return shr_error_e Standard error handeling
 */

shr_error_e dnx_vlan_port_create_ingress_fodo_mode_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    int *fodo_assignment_mode);

/**
* \brief
* Get the forwarding information from Gport (used for retrieving protection parameters).
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vlan_port  -  see detail explanation at bcm_vlan_port_create()
* \par DIRECT OUTPUT:
*   \param [out] out_lif_is_zero  -  present outlif is 0 and forwarding information result type is dest_outlif.
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_vlan_port_gport_to_forward_information_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 *out_lif_is_zero);

/**
 * \brief -
 * This function updates SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW) with
 * the following info:
 * - flags
 * - vsi
 * - port
 * - match_vlan
 * - match_inner_vlan
 * - match_pcp
 * - nw group id
 * \see
 *  bcm_dnx_vlan_port_create
 */
shr_error_e dnx_vlan_port_create_ingress_lif_match_info_set(
    int unit,
    int local_in_lif,
    int old_in_lif,
    int is_replace,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port);
/**
 * \brief -
 * This function gets the following info from SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW):
 * - port
 * - criteria
 * - match_vlan
 * - match_inner_vlan
 * \see
 *  bcm_dnx_vlan_port_find
 */
shr_error_e dnx_vlan_port_create_ingress_lif_match_info_get(
    int unit,
    dbal_tables_e inlif_dbal_table_id,
    int local_in_lif,
    bcm_vlan_port_t * vlan_port);

/**
 * \brief -
 * This function clears the entry in the SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW).
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
shr_error_e dnx_vlan_port_destroy_ingress_lif_match_info_clear(
    int unit,
    int is_dpc_ac,
    int local_in_lif);

/**
 * \brief - Verify function for dnx_vlan_port_create - REPLACE case
 * It compares the new configuration vs old configuration.
 * Fields that cannot be changed need to be same as the old
 * configuration!
 */
shr_error_e dnx_vlan_port_create_replace_new_old_compare_verify(
    int unit,
    bcm_vlan_port_t * new_vlan_port,
    bcm_vlan_port_t * old_vlan_port);

/*
 * }
 */
#endif/*_VLAN_API_INCLUDED__*/
