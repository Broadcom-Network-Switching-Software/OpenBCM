/** \file port_pp.h
 * 
 *
 * Internal DNX Port APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PORT_PP_API_INCLUDED__
/*
 * {
 */
#define _PORT_PP_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/port.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>

/*
 * Defines
 * {
 */

/**
 * Specify default port LB Profile
 */
#define DEFAULT_PP_PORT_LB_PROFILE (0)

/*
 * }
 */

/*
 * MACROs
 * {
 */
/** Indicate whether the gport is a RIF */
#define DNX_PORT_PP_GPORT_IS_RIF(unit, gport) (BCM_GPORT_IS_TUNNEL(gport) && (BCM_GPORT_TUNNEL_ID_GET(gport) < dnx_data_l3.rif.nof_rifs_get(unit)))
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */
/*
 * Structures
 * {
 */

/**
 * \brief
 *  Egress PP Port result info
 */
typedef struct
{
    /*
     * Trap context port profile
     */
    dbal_enum_value_field_etpp_trap_context_port_profile_e trap_context_port_profile;
    /*
     * Enabler for egress PP filters
     */
    int egress_pp_filters_enable;
    /*
     * Enabler for egress TM filters
     */
    int egress_tm_filters_enable;
    /*
     * Enabler for Exclude Src TM filter
     */
    int exclude_src_enable;
    /*
     * default port ESEM command
     */
    uint32 default_esem_cmd;
    /*
     * Port profile
     */
    dbal_enum_value_field_out_pp_port_egress_forward_code_selection_profile_e fwd_code_port_profile;
    /*
     * PRP port profile
     */
    dbal_enum_value_field_etpp_prp2_port_cs_var_e prp2_port_profile;
    /*
     * Termination port profile
     */
    dbal_enum_value_field_etpp_termination_port_cs_var_e term_port_profile;
    /*
     * Forward port profile
     */
    dbal_enum_value_field_etpp_forward_port_cs_var_e forward_port_profile;
    /*
     * Encap1 port profile
     */
    dbal_enum_value_field_etpp_enc1_port_cs_var_e enc1_port_profile;
    /*
     * Encap2 port profile
     */
    dbal_enum_value_field_etpp_enc2_port_cs_var_e enc2_port_profile;
    /*
     * Encap3 port profile
     */
    dbal_enum_value_field_etpp_enc3_port_cs_var_e enc3_port_profile;
    /*
     * Encap4 port profile
     */
    dbal_enum_value_field_etpp_enc4_port_cs_var_e enc4_port_profile;
    /*
     * Encap5 port profile
     */
    dbal_enum_value_field_etpp_enc5_port_cs_var_e enc5_port_profile;
    /*
     *  estimate bytes to remove
     */
    int estimated_btr;
    /*
     *  estimate byte to remove - consider system header termination
     */
    int estimated_btr_system_headers_termination_enable;
    /*
     *  estimate byte to remove - consider network header termination
     */
    int estimated_btr_network_headers_termination_enable;

} dnx_port_pp_egress_port_info_t;

/**
 * \brief
 *  Egress PP Port result info
 */
typedef struct
{
    /*
     * VTT1 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_vtt1_e vtt1_ptc_cs_profile;
    /*
     * VTT2 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_vtt2_e vtt2_ptc_cs_profile;
    /*
     * VTT3 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_vtt3_e vtt3_ptc_cs_profile;
    /*
     * VTT4 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_vtt4_e vtt4_ptc_cs_profile;
    /*
     * VTT5 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_vtt5_e vtt5_ptc_cs_profile;
    /*
     * FWD1 PTC CS profile
     */
    dbal_enum_value_field_ptc_cs_profile_fwd1_e fwd1_ptc_cs_profile;
    /*
     * VTT5 PTC Routing enable profile
     */
    dbal_enum_value_field_ptc_routing_enable_profile_vtt1_e vtt1_ptc_routing_enable_profile;
    /*
     * Eco-logic PTC profile
     */
    dbal_enum_value_field_ecol_ptc_profile_e eco_logic_ptc_profile;

} dnx_port_pp_ingress_port_ptc_info_t;

/*
 * }
 */

/**
 * \brief Set VLAN domain on a port.
 *
 * This function is used to set both ingress and egress VLAN domain \n
 * HW tables on given pp port.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit unit gport belongs to
 *   \param [in] port port to set vlan domain on
 *   \param [in] vlan_domain required VLAN domain
 * \par DIRECT OUTPUT:
 *   \retval Error indication according to shr_error_e enum
 */
shr_error_e dnx_port_pp_vlan_domain_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_domain);

/**
 * \brief
 *    Set DP profile on LIF for IVE outer or inner PCP mapping.
 * \param [in] unit - unit gport belongs to
 * \param [in] port - LIF to set DP profile on
 * \param [in] type - relevant type
              QOS_DP_PROFILE_OUTER
              QOS_DP_PROFILE_INNER
 * \param [in] map_id - qos map id
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

shr_error_e dnx_port_pp_ive_dp_profile_set(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int map_id);

/**
 * \brief
 *     Get DP profile from LIF for IVE outer or inner PCP mapping.
 *
 * \par DIRECT INPUT:
 * \param [in] unit - unit gport belongs to
 * \param [in] port - LIF to set DP profile on
 * \param [in] type - relevant type
              QOS_DP_PROFILE_OUTER
              QOS_DP_PROFILE_INNER
 * \param [out] map_id - qos map id
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

shr_error_e dnx_port_pp_ive_dp_profile_get(
    int unit,
    bcm_port_t port,
    qos_dp_profile_application_type_e type,
    int *map_id);

/**
 * \brief -
 * This function is used to set ingress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_ingress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain);

/**
 * \brief -
 * This function is used to set egress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [in] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_egress_vlan_domain_set(
    int unit,
    bcm_gport_t gport_lif,
    uint32 vlan_domain);

/**
 * \brief -
 * This function is used to get ingress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [out] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_ingress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain);

/**
 * \brief -
 * This function is used to get egress VLAN domain per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_lif - Gport LIF
 * \param [out] vlan_domain - VLAN domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_egress_vlan_domain_get(
    int unit,
    bcm_gport_t gport_lif,
    uint32 *vlan_domain);

/**
 * \brief -
 * This function is used to configure OAM default profile per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport - Gport LIF
 * \param [in] oam_default_profile - OAM default profile to be associated with the LIF
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_oam_default_profile_set(
    int unit,
    bcm_gport_t gport,
    int oam_default_profile);

/**
 * \brief -
 * This function gets the OAM default profile per Lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport - Gport LIF
 * \param [in] oam_default_profile - OAM default profile configured on the LIF
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_oam_default_profile_get(
    int unit,
    bcm_gport_t gport,
    int *oam_default_profile);

/**
 * \brief -
 * This function is used to get a status whether a egress port is configure to DROP all packets or not.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Relevant port
 * \param [out] is_drop - out indication of whether the port drop all traffic or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_port_pp_egress_trap_port_profile_drop_get(
    int unit,
    bcm_port_t port,
    uint8 *is_drop);

/**
 * \brief -
 * This function is used to set an egress port configuration to DROP all packets.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Relevant port
 * \param [in] is_drop - if set, the port trap profile is set to DROP_ALL, if not it is set to NORMAL
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_egress_trap_port_profile_drop_set(
    int unit,
    bcm_port_t port,
    uint8 is_drop);

/**
 * \brief - Get VLAN domain on a port
 *  Get both ingress and egress VLAN domains
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] vlan_domain - Vlan domain
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_domain_get(
    int unit,
    bcm_port_t port,
    uint32 *vlan_domain);

/**
 * \brief
 * Configure Default Port Match, called by
 * bcm_dnx_port_match_add in case match criteria is PORT
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] port -
 *     port - gport
 *   \param [in] match_info - bcm_dnx_port_match_add param.
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: MAC table is full
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_port_pp_egress_match_port_add(
    int unit,
    bcm_gport_t port,
    bcm_port_match_info_t * match_info);

/**
 * \brief -
 * Initialize PP port module. As part of the initialization, the
 * function configure default VLAN membership if values, called
 * durring initialization of the device.
 *
 * \par DIRECT INPUT:
 *    \param [in] unit -
 *     Relevant unit.
 * \par DIRECT OUTPUT:
 *    \retval Error indication according to shr_error_e enum
 * \par INDIRECT OUTPUT:
 *    \retval
 *    Set default 1:1 mapping for VLAN membership according to
 *    Port x VLAN
 */
shr_error_e dnx_pp_port_init(
    int unit);

/**
 * \brief - Get the VLAN Membership-IF from internal PP port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] internal_core_id - Internal Core id
 * \param [in] internal_port_pp - Internal PP port
 * \param [out] vlan_mem_if - Vlan membership interface
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *  We assume that VLAN-membership-IF mapping from local port is
 *  symmetric.Because of that, it is enough to get the
 *  information from Ingress Port table.
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_membership_if_get(
    int unit,
    bcm_core_t internal_core_id,
    bcm_port_t internal_port_pp,
    uint32 *vlan_mem_if);

/**
 * \brief - Set the VLAN Membership-IF per local port.
 *  Write to INGRESS/EGRESS_PP_PORT_TABLE.
 *  We assume that VLAN-membership-IF mapping per local port is symmetric and
 *  because of that we need to set the appropriate information to Ingress and Egress PP Port tables.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port - physical port
 * \param [in] vlan_mem_if - VLAN-membership-if
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_vlan_membership_if_set(
    int unit,
    bcm_port_t port,
    uint32 vlan_mem_if);

/**
 * \brief - Provide mapping between source system port and gport
 *
 * \param [in] unit - Unit id
 * \param [in] core_id - In core id
 * \param [in] pp_port - In pp port
 * \param [in] src_system_port - Source system port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_pp_src_system_port_set(
    int unit,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 src_system_port);

/**
 * \brief - Get mapping between source system port and gport
 *
 * \param [in] unit - Unit id
 * \param [in] core_id - In core id
 * \param [in] pp_port - In pp port
 * \param [out] src_system_port - Source system port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_pp_src_system_port_get(
    int unit,
    bcm_core_t core_id,
    uint32 pp_port,
    uint32 *src_system_port);

/**
 * \brief - Provide mapping between PTC, source system port and pp port. Skip the configuration of pp port in case of COE port.
 *
 * \param [in] unit - Unit id
 * \param [in] ptc - PTC
 * \param [in] core_id - In core id
 * \param [in] src_system_port - Source system port
 * \param [in] pp_port - PP port
 * \param [in] flags - Used for indication if the port is COE port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_pp_ingress_ptc_port_mapping_set(
    int unit,
    uint32 ptc,
    bcm_core_t core_id,
    uint32 src_system_port,
    uint32 pp_port,
    uint32 flags);

/**
 * \brief -
 *  Set the filter enablers to pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] filter_flags - Indications for filter enablers. Possible values:
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC,
 *                            BCM_PORT_CONTROL_FILTER_DISABLE_ALL.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_unknown_filter_get
 */
shr_error_e dnx_port_pp_l2_unknown_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] filter_flags - Indications for filter enablers. Possible values:
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC,
 *                             BCM_PORT_CONTROL_FILTER_DISABLE_ALL.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_unknown_filter_set
 */
shr_error_e dnx_port_pp_l2_unknown_filter_get(
    int unit,
    bcm_port_t port,
    int *filter_flags);
/**
 * \brief
 *   Get the PTC and core per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] ptc - PRT ptc_profile
 * \param [out] core - LLR ptc_profile

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e dnx_port_ptc_get(
    int unit,
    bcm_port_t port,
    uint32 *ptc,
    bcm_core_t * core);

/**
 * \brief
 *   Get the Port per PTC and Core.
 *
 * \param [in] unit - unit ID
 * \param [in] ptc - PRT ptc_profile
 * \param [in] core - LLR ptc_profile
 * \param [out] port - port ID (-1 if no matching port was found).

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *
 */
shr_error_e dnx_port_ptc_to_port_get(
    int unit,
    uint32 ptc,
    bcm_core_t core,
    bcm_port_t * port);

/**
 * \brief
 *   Get the PRT and LLR ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] prt_profile - PRT ptc_profile

 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_port_ptc_profile_set
 */
shr_error_e dnx_port_prt_ptc_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_port_termination_ptc_profile_e * prt_profile);

/**
 * \brief
 *   Set the  llr cs profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] llr_cs_profile - llr cs profile
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 * dbal_enum_value_field_llr_cs_port_cs_profile_e,
 */
shr_error_e dnx_port_pp_llr_cs_profile_set(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_llr_cs_port_cs_profile_e llr_cs_profile);
/**
 * \brief
 *   Get the LLR and VT ptc profile per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] llr_cs_profile - llr cs profile
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 * dbal_enum_value_field_llr_cs_port_cs_profile_e,
 */
shr_error_e dnx_port_pp_llr_cs_profile_get(
    int unit,
    bcm_port_t port,
    dbal_enum_value_field_llr_cs_port_cs_profile_e * llr_cs_profile);

/**
 * \brief
 *   Set the overlay recycle per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] enable - enable
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 */
shr_error_e dnx_port_pp_overlay_recycle_set(
    int unit,
    bcm_port_t port,
    uint32 enable);

/**
 * \brief
 *   Get overlay recycle status per Port.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] enable - enable
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 */
shr_error_e dnx_port_pp_overlay_recycle_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief
 *   Set the port to specific recycle application type
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] recycle_app_type - recycle application type
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 */
shr_error_e dnx_port_pp_recycle_app_set(
    int unit,
    bcm_port_t port,
    int recycle_app_type);

/**
 * \brief
 *   wrapper of function dnx_port_pp_recycle_app_set
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [in] recycle_app_type - recycle application type
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 */
shr_error_e dnx_port_pp_recycle_app_set_wrapper(
    int unit,
    bcm_port_t port,
    int recycle_app_type);

/**
 * \brief
 *   Get specific recycle application type of the port
 *
 * \param [in] unit - unit ID
 * \param [in] port - port ID
 * \param [out] recycle_app_type - recycle application type
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 */
shr_error_e dnx_port_pp_recycle_app_get(
    int unit,
    bcm_port_t port,
    int *recycle_app_type);

/* To be used when/if MACT SA drop is supported */
/**
 * \brief -
 *  Set the action profile for sa-drop to port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] action_flags - Action flags for SA drop. Possible values:
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_NONE,
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_DROP,
 *                            BCM_PORT_CONTROL_DISCARD_MACSA_TRAP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_sa_drop_action_profile_get
 */
shr_error_e dnx_port_pp_l2_sa_drop_action_profile_set(
    int unit,
    bcm_port_t port,
    int action_flags);

/**
 * \brief -
 *  Get the action profile for sa-drop from port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] action_flags - Action flags for SA drop. Possible values:
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_NONE,
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_DROP,
 *                             BCM_PORT_CONTROL_DISCARD_MACSA_TRAP.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_pp_l2_sa_drop_action_profile_set
 */
shr_error_e dnx_port_pp_l2_sa_drop_action_profile_get(
    int unit,
    bcm_port_t port,
    int *action_flags);

/**
 * \brief - Configure Ingress Ethernet properties
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *  In accordance with it the ethernet properties will be enabled
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_initial_eth_properties_set(
    int unit,
    bcm_port_t port,
    uint32 header_type);

/**
 * \brief - Unset Ingress Ethernet properties
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *  In accordance with it the ethernet properties will be cleared
 *
 * \return
 *   int
 *
 * \remark
 *  This function frees any resources allocated by dnx_port_initial_eth_properties_set
  * It also perform proper HW clear to the HW set by dnx_port_initial_eth_properties_set.
 * \see
 *   * None
 */
int dnx_port_initial_eth_properties_unset(
    int unit,
    bcm_gport_t port,
    uint32 header_type);

/**
 * \brief - Unset Ingress Ethernet properties per pp port
 *
 * \param [in] unit - Relevant unit
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core ID
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *  In accordance with it the ethernet properties will be cleared
 * \param [in] tpid_ref_dealloc - Indicates if a reference should be removed from egress_acceptable_frame_type_profile and port_pp_tpid_profile_egress template managers
 *
 * \return
 *   int
 *
 * \remark
 *  This function frees any resources allocated by dnx_port_initial_eth_properties_set
  * It also perform proper HW clear to the HW set by dnx_port_initial_eth_properties_set.
 * \see
 *   * None
 */
int dnx_port_initial_eth_properties_per_pp_port_unset(
    int unit,
    uint32 pp_port,
    bcm_core_t core_id,
    uint32 header_type,
    uint8 tpid_ref_dealloc);

/**
 * \brief - Configure Trap Context Port Profile according to switch header type and configure Egress Ethernet properties
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] switch_header_type - Switch header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_egress_set(
    int unit,
    bcm_port_t port,
    int switch_header_type);

/**
 * \brief -  Unset Egress Ethernet properties
 * It also perform HW clear to the HW set by dnx_port_pp_egress_set.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port id
 * \param [in] switch_header_type - Switch header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function frees any resources allocated by dnx_port_pp_egress_set
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_egress_unset(
    int unit,
    bcm_gport_t port,
    int switch_header_type);

/**
 * \brief -  Unset Egress Ethernet properties
 * It also perform HW clear to the HW set by dnx_port_pp_egress_set
 *
 * \param [in] unit - Relevant unit
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core id
 * \param [in] switch_header_type - Switch header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 * \param [in] ref_dealloc - Indicates if a reference should be removed from egress_acceptable_frame_type_profile, port_pp_tpid_profile_egress and esem_cmd template managers
 * \param [in] remove_svtag_configuration - indicates if SVTAG configuration should be removed from ESEM command in case it exist
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function frees any resources allocated by dnx_port_pp_egress_set
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_egress_per_pp_port_unset(
    int unit,
    uint32 pp_port,
    bcm_core_t core_id,
    int switch_header_type,
    uint8 ref_dealloc,
    uint8 remove_svtag_configuration);

/**
 * \brief - Configure Port Termination PTC Profile according to the header type
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port number
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_prt_ptc_profile_set(
    int unit,
    bcm_port_t port,
    uint32 header_type);

/**
 * \brief - Configure Port Termination PTC Profile according to the header type based on gport_info
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_info - gport info
 * \param [in] header_type - Header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * this function is used in places where algo_gpm cannot return all needed
 *     info to make this configuration and additional actions are required. one such places is lag.
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_prt_ptc_profile_internal_set(
    int unit,
    dnx_algo_gpm_gport_phy_info_t * gport_info,
    uint32 header_type);

/**
 * \brief -
 *  Set the flooding destinations for the unknown packets.
 *
 * \param [in] unit - The unit ID.
 * \param [in] port_flood_profile - Port flooding profile.
 * \param [in] lif_flood_profile - InLIF flooding profile.
 * \param [in] default_frwrd_dst - Forwarding destinations for unknwon packets.
 *
 * \return
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *  port_flood_profile = lif_flood_profile = 0 is used as system default
 *  configurations in which VSI.default-forward-destination is used.
 *  In case of port flooding, port_flood_profile = 1 and lif_flood_profile = 0.
 *  In case of LIF flooding, port_flood_profile = 0/1 and lif_flood_profile = input.
 *
 * \see
 *  bcm_dnx_port_flood_group_set.
 */
shr_error_e dnx_port_lif_flood_profile_action_set(
    int unit,
    int port_flood_profile,
    int lif_flood_profile,
    dnx_default_frwrd_dst_t default_frwrd_dst[DBAL_NOF_ENUM_DESTINATION_DISTRIBUTION_TYPE_VALUES]);

/**
 * \brief -
 *  Set the filter enablers per pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] filter_flags - Indications for filter enablers.
 *                Possible values:   TRUE - Logical Same interface filter is enabled.
 *                Possible values:   FALSE - Logical Same interface filter is disabled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_lif_same_interface_disable_set(
    int unit,
    bcm_port_t port,
    int filter_flags);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] filter_flags - Indications for filter enablers.
 *                Possible values:   TRUE - Logical Same interface filter is enabled.
 *                Possible values:   FALSE - Logical Same interface filter is disabled.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_lif_same_interface_disable_get(
    int unit,
    bcm_port_t port,
    int *filter_flags);

/**
 * \brief -
 *  Set the preserve dscp per inLIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port ID.
 * \param [in] preserve_dscp_mode - Enable/Disable DSCP preserve.

 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_preserve_dscp_set(
    int unit,
    bcm_port_t gport,
    int preserve_dscp_mode);

/**
 * \brief -
 *  Get the preserve dscp per inLIF.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - port id.
 * \param [out] preserve_dscp_mode - Indications for preserve DSCP enabler.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_preserve_dscp_get(
    int unit,
    bcm_port_t gport,
    int *preserve_dscp_mode);

/**
 * \brief -
 *  Set speculative parsingp per port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_speculative_parsing_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief -
 *  Get speculative parsingp per port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_speculative_parsing_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief -
 *  Set ipvx over MPLS parsing per port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_ipvx_parsing_over_mpls_set(
    int unit,
    bcm_port_t port,
    uint8 enable);

/**
 * \brief -
 *  Get ipvx over MPLS parsing per port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - port id.
 * \param [out] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_ipvx_parsing_over_mpls_get(
    int unit,
    bcm_port_t port,
    uint8 *enable);

/**
 * \brief -
 *  Set the filter enablers per pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port id.
 * \param [in] in_lif_same_interface_mode - Set the LIF profile mode.
 *                Possible values:   0 - Device Scope LIF.
 *                Possible values:   1 - System Scope LIF.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_in_lif_same_interface_set(
    int unit,
    bcm_port_t gport,
    int in_lif_same_interface_mode);

/**
 * \brief -
 *  Get the filter enablers from pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - logical port id.
 * \param [out] in_lif_same_interface_mode - Get the LIF Mode.
 *                Possible values:   0 - Device scope LIF.
 *                Possible values:   1 - System Scope LIF.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 */
shr_error_e dnx_port_pp_in_lif_same_interface_get(
    int unit,
    bcm_port_t gport,
    int *in_lif_same_interface_mode);

/**
 * \brief - Configure PORT_VID in INGRESS_PP_PORT table
 *
 * \param [in] unit - Relevant unit
 * \param [in] pp_port - PP port
 * \param [in] core_id - Core id
 * \param [in] vid - VLAN id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_untagged_vlan_set_internal(
    int unit,
    bcm_port_t pp_port,
    bcm_core_t core_id,
    bcm_vlan_t vid);

/**
 * \brief -
 *  Set the learning mode based on the flags parameter to a physical port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] core_id - Core ID the port resides in.
 * \param [in] pp_port - Physical port ID in the core.
 * \param [in] flags - Flags for learning mode. Possible values:
 *                     BCM_PORT_LEARN_ARL, BCM_PORT_LEARN_FWD
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_learn_set(
    int unit,
    int core_id,
    int pp_port,
    uint32 flags);

/**
 * \brief - Update the tm_port of trunk member with the pp properties
 *    of trunk`s pp port
 *
 * \param [in] unit - Unit number
 * \param [in] trunk_id - Trunk id
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_pp_port_update(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_port_t logical_port);

/**
 * \brief - Update the deleted trunk member - apply pp properties
 *    to the pp port of the deleted member
 *
 * \param [in] unit - Relevant unit
 * \param [in] trunk_id - Trunk id
 * \param [in] logical_port - logical port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_deleted_trunk_member_update(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_port_t logical_port);

/**
 * \brief - Configure parsing context per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] parsing_context - Parsing context
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_parsing_context_set(
    int unit,
    bcm_port_t port,
    uint32 parsing_context);

/**
 * \brief - Configure termination and forward context port profiles per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] term_context_port_profile - Termination context port profile
 * \param [in] forward_context_port_profile - Forward context port profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_term_and_forward_context_port_profile_set(
    int unit,
    bcm_port_t port,
    int term_context_port_profile,
    int forward_context_port_profile);

/**
 * \brief - Get termination and forward context port profiles per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport - gport
 * \param [out] term_context_port_profile - Termination context port profile
 * \param [out] forward_context_port_profile - Forward context port profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_term_and_forward_context_port_profile_get(
    int unit,
    bcm_gport_t gport,
    int *term_context_port_profile,
    int *forward_context_port_profile);

/**
 * \brief
 * This function is used to set the supported flags
 * for disabling the load balancing layers.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [in] flags -flags . Supported flags: \n
 *      BCM_SWITCH_LB_LAYERS_ETHERNET_DISABLE
 *      BCM_SWITCH_LB_LAYERS_IPV4_DISABLE
 *      BCM_SWITCH_LB_LAYERS_IPV6_DISABLE
 *      BCM_SWITCH_LB_LAYERS_MPLS_DISABLE
 *      BCM_SWITCH_LB_LAYERS_TCP_DISABLE
 *      BCM_SWITCH_LB_LAYERS_UDP_DISABLE
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 */
shr_error_e dnx_port_load_balancing_hash_layers_disable_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief
 * Gets the port_lb_profile and returns the flags
 * \param [in] unit -
 *   The unit number.
 * \param [in] lb_profile -
 *   lb_profile id to get data for the flags
 * \param [out] flags -
 *   Pointer to the flags
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pp_port_profile_get_data(
    int unit,
    uint32 lb_profile,
    uint32 *flags);

/**
 * \brief
 * This function is used to get the supported flags
 * for disabling the load balancing layers.
 *
 * \param [in] unit - unit ID
 * \param [in] port - port number
 * \param [out] flags - return flags.
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 */
shr_error_e dnx_port_load_balancing_hash_layers_disable_get(
    int unit,
    bcm_port_t port,
    uint32 *flags);
/**
 * \brief
 *   updates template manager for port load balancing profiles, and exchanges the old port_lb_profile with
 *   new port_lb_profile according to flags.
 * \param [in] unit -
 *   The unit number.
 * \param [in] old_port_profile -
 *  old_port_lb_profile id to exchange
 * \param [in] flags-
 *   can be one of the following:
 *   BCM_SWITCH_LB_LAYERS_ETHERNET_DISABLE
 *   BCM_SWITCH_LB_LAYERS_IPV4_DISABLE
 *   BCM_SWITCH_LB_LAYERS_IPV6_DISABLE
 *   BCM_SWITCH_LB_LAYERS_MPLS_DISABLE
 *   BCM_SWITCH_LB_LAYERS_TCP_DISABLE
 *   BCM_SWITCH_LB_LAYERS_UDP_DISABLE
 * \param [out] new_port_profile
 *   Pointer to new_port_profile
* \param [out] is_first -
 *   Pointer to the first reference
* \param [out] is_last -
 *   Pointer to the last reference

 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e dnx_pp_profile_exchange(
    int unit,
    int old_port_profile,
    uint32 flags,
    int *new_port_profile,
    uint8 *is_first,
    uint8 *is_last);

/**
 * \brief - Helper Function for updating LOAD_BALANCING_USE_LAYER_RECORD table
 * Set disabled layer protocols per port/lif.
 *
 * \param [in] unit - Relevant unit
 * \param [in] flag - Layer protocol flag.
 *      Supported flags:
 *      BCM_SWITCH_LB_LAYERS_ETHERNET_DISABLE
 *      BCM_SWITCH_LB_LAYERS_IPV4_DISABLE
 *      BCM_SWITCH_LB_LAYERS_IPV6_DISABLE
 *      BCM_SWITCH_LB_LAYERS_MPLS_DISABLE
 *      BCM_SWITCH_LB_LAYERS_TCP_DISABLE
 *      BCM_SWITCH_LB_LAYERS_UDP_DISABLE
 * \param [in] field_id - Dbal field id
 * \param [in] lb_profile - Lb profile(port LB profile or lif LB profile)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_lif_lb_profile_dbal_configure(
    int unit,
    uint32 flag,
    dbal_fields_e field_id,
    int lb_profile);

/**
 * \brief -  Enable Same interface Filter per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] filter_flags - '0' means disable, '1' means enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_same_interface_filter_set(
    int unit,
    bcm_port_t port,
    int filter_flags);

/**
 * \brief -  Enable/Disable SA multicast ingress trap per port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] enable - indicates enable/disable SA multicast ingress trap
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_sa_mc_trap_set(
    int unit,
    bcm_port_t port,
    int enable);

/**
 * \brief -  Get the value of Enable/Disable SA multicast ingress trap per port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] enable - indicates enable/disable SA multicast ingress trap
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_pp_sa_mc_trap_get(
    int unit,
    bcm_port_t port,
    int *enable);

/**
 * \brief -
 *  Set the vpmi to a pp port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - local port, or logical port in gport format.
 * \param [in] vmpi - vmpi.
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *  * None
 * \see
 *  * None
 */
bcm_error_t bcm_dnx_port_vmpi_set(
    int unit,
    bcm_port_t port,
    uint32 vmpi);

/**
 * \brief -
 *  Get the vpmi from a port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - physical port, or logical port in gport format.
 * \param [out] vmpi - vmpi
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
bcm_error_t bcm_dnx_port_vmpi_get(
    int unit,
    bcm_port_t port,
    uint32 *vmpi);

/**
 * \brief -  set estimated bytes to add per port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Logical port
 * \param [in] size - estimated size to add in bytes
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_estimate_bta_set(
    int unit,
    bcm_port_t port,
    int size);

/**
 * \brief - set estimated header mode per port
 *
 * \param [in] unit - unit number
 * \param [in] port - Logical port
 * \param [in] out_header_type - out direction header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_estimate_header_mode_set(
    int unit,
    bcm_port_t port,
    int out_header_type);

/**
 * \brief -
 *  Translate the destination gport to default desitinations or offsets for
 *  unknown packtes.
 *
 * \param [in] unit - Relevant unit
 * \param [in] dst_gport - gport indicates a destination.
 * \param [out] default_frwrd_dst - destination or offset to the destination.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_flood_dst_gport_to_dst_offset(
    int unit,
    bcm_gport_t dst_gport,
    dnx_default_frwrd_dst_t * default_frwrd_dst);

/**
 * \brief -
 *  Translate the default desitinations or offsets to destination gport for
 *  unknown packtes.
 *
 * \param [in] unit - Relevant unit
 * \param [in] default_frwrd_dst - destination or offset to the destination.
 * \param [out] dst_gport - gport indicates a destination.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_flood_dst_gport_from_dst_offset(
    int unit,
    dnx_default_frwrd_dst_t default_frwrd_dst,
    bcm_gport_t * dst_gport);

/**
 * \brief - Get one of ports of given VLAN domain
 *  Get one of ports of given VLAN domain
 *
 * \param [in] unit - Relevant unit
 * \param [in] vlan_domain - Vlan domain
 * \param [out] port - Port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_vlan_domain_port_pp_get(
    int unit,
    uint32 vlan_domain,
    bcm_port_t * port);

/*
 * }
 */
#endif /**_PORT_PP_API_INCLUDED__*/
