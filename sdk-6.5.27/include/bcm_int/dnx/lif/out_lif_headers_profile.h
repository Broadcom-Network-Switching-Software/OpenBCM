/** \file out_lif_headers_profile.h
 *
 * out_lif additional headers profile management for DNX.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef OUT_LIF_HEADERS_PROFILE_H_INCLUDED
/*
 * {
 */
#define OUT_LIF_HEADERS_PROFILE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm_int/dnx/algo/swstate/auto_generated/types/algo_tunnel_types.h>

/***************************************************************************************/
/*
 * Defines {
 */

/** No Additional Headers */
#define ADDITIONAL_HEADERS_NONE                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_NONE
/** The additional Headers are CW, ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_CW_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI_CW
/** The additional Header is ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_ESI_EVPN             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ESI
/** The additional Headers are CW, FL and ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_CW_FL_ESI_EVPN       DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFLOESI
/** The additional Header is FL and ESI for EVPN MC Label */
#define ADDITIONAL_HEADERS_FL_ESI_EVPN          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FLOESI
/** The additional Headers are CW, EL and ELI for PWE, new RFC */
#define ADDITIONAL_HEADERS_CW_EL_ELI_PWE        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOELOELI
/** The additional Headers are CW, FL for PWE, old RFC */
#define ADDITIONAL_HEADERS_CW_FL_PWE            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CWOFL
/** The additional Headers are EL, ELI for EVPN */
#define ADDITIONAL_HEADERS_EL_ELI               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_ELOELI
/** The additional Header is CW for PWE */
#define ADDITIONAL_HEADERS_CW_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_CW
/** The additional Header is FL for PWE */
#define ADDITIONAL_HEADERS_FL_PWE               DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_FL
/** The additional Headers are VXLAN-GPE and UDP */
#define ADDITIONAL_HEADERS_VXLAN_GPE_UDP        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_GPE
/** The additional Headers are GENEVE and UDP */
#define ADDITIONAL_HEADERS_GENEVE_UDP           DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GENEVE
/** The additional Headers are VXLAN and UDP */
#define ADDITIONAL_HEADERS_VXLAN_UDP            DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN
/** The additional Header is GRE4 */
#define ADDITIONAL_HEADERS_GRE4                 DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE4
/** The additional Header is GRE8 with LB-Key with TNI, or without TNI */
#define ADDITIONAL_HEADERS_WO_TNI_GRE8          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_KEY
/** The additional Header is GRE8 with LB-Key with TNI */
#define ADDITIONAL_HEADERS_TNI_GRE8             DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V4
/** The additional Header is UDP */
#define ADDITIONAL_HEADERS_UDP                  DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_UDP
/** The additional Header is GRE12 */
#define ADDITIONAL_HEADERS_GRE12                DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE12
/** The additional Header is GRE8 with LB-Key with TNIfrom entry over IPV6 */
#define ADDITIONAL_HEADERS_IPV6_TNI_GRE8        DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_TNI_FROM_ENTRY_V6
/** The additional Header is GRE8 with LB-Key with TNIfrom entry over IPV6 */
#define ADDITIONAL_HEADERS_PROFILE_GRE8_W_SN    DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_GRE8_W_SN
/** The additional headers are UDP, UDP and VXLAN */
#define ADDITIONAL_HEADERS_VXLAN_DOUBLE_UDP_ESP DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_VXLAN_DOUBLE_UDP_ESP
/** The additional Header are FL, FLI for IOAM labels */
#define ADDITIONAL_HEADERS_IOAM_FL_FLI          DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IOAM_FLOFLI
/** The additional Header are CW, FL, FLI for IOAM labels */
#define ADDITIONAL_HEADERS_IOAM_CW_FL_FLI       DBAL_ENUM_FVAL_ETPP_ADDITIONAL_HEADER_PROFILE_STACK_ATTRIBUTES_AHS_IOAM_CWOFLOFLI

/*
 * Defines }
 */

/***************************************************************************************/
/*
 * Enums  {
 */

/* Enum defining classifications of kinds of additional headers stack */
typedef enum
{
    AHS_NAMESPACE_GENERAL = 0,
    AHS_NAMESPACE_MPLS = 1,
    AHS_NAMESPACE_MPLS_IML = 2,
    AHS_NAMESPACE_MPLS_IOAM = 3,
    AHS_NAMESPACE_IP_TUNNEL = 4,
    AHS_NAMESPACE_VXLAN = 5,
    AHS_NAMESPACE_GRE_OR_GENEVE = 6,
    AHS_NAMESPACE_UDP = 7,
    /** Not used, always the last one.*/
    AHS_NAMESPACE_TOTAL_NUM
} dnx_ahs_namespace_e;

/*
 * Enums  }
 */

/***************************************************************************************/
/*
 * Structures {
 */
/**
 * \brief
 *   Generic parameter container for additional headers profile hw configuration
 */
typedef struct
{
    /**
     * \brief
     *   The original additional_headers_profile info used to clear the entries.
     */
    /**The original additional_headers_profile */
    int old_additional_headers_profile;

    /**  The original additional_headers_profile map info */
    dnx_additional_headers_profile_map_t old_profile_map;

    /** Is the original additional_headers_profile last in-use (need clear) */
    uint8 is_old_profile_last_referred;

    /** Is the original additional_headers_profile map last in-use (need clear) */
    uint8 is_old_profile_map_last_referred;

    /**
     * \brief
     *   The new additional_headers_profile info used to set the entries.
     */
    /** The New additional_headers_profile */
    int new_additional_headers_profile;

    /** Is the original additional_headers_profile first in-use (need set) */
    int8 is_new_profile_first_referred;

    /** The new additional_headers_profile map info */
    dnx_additional_headers_profile_map_t new_profile_map;

    /** Is the original additional_headers_profile map first in-use (need set) */
    uint8 is_new_profile_map_first_referred;

} dnx_egress_headers_profile_mngr_info_t;

/*
 * Structures }
 */

/***************************************************************************************/
/*
 * APIs {
 */

/**
 * \brief - The function initialize the egress additional_headers_profile definitions,
 *          including the profile properties and allocate some default profiles.
 * \param [in] unit - Relevant unit.
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_profile_init(
    int unit);

/**
 * \brief - Allocate additional_header_profiles and use them for the given lif_additional_headers_profile.
 *          In case of replace, free the in-use additional_headers_profiles used by the given
 *          lif_additional_headers_profile_old first. * \param [in] unit - Unit ID.
 * \param [in] is_replace - Replace or Create
 * \param [in] lif_additional_headers_profile_old - Current lif_additional_headers_profile
 *             Valid in replace case only.
 * \param [in] lif_additional_headers_profile - New lif_additional_headers_profile that will be used now.
 * \param [in] ahs_namespace - lif_additional_headers_profile namespace.
 * \param [out] headers_profile_mngr_info - additional headers profile management info.
 *              
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_profile_alloc(
    int unit,
    uint8 is_replace,
    int lif_additional_headers_profile_old,
    int lif_additional_headers_profile,
    dnx_ahs_namespace_e ahs_namespace,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info);

/**
 * \brief - De-allocate the additional_headers_profiles used by the given lif_additional_headers_profile.
 *
 * \param [in] unit - unit id.
 * \param [in] lif_additional_headers_profile - lif_additional_headers_profile that is used now.
 * \param [in] ahs_namespace - lif_additional_headers_profile namespace.
 * \param [out] headers_profile_mngr_info - additional headers profile management info.
 *              
 * \return
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_profile_free(
    int unit,
    int lif_additional_headers_profile,
    dnx_ahs_namespace_e ahs_namespace,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info);

/**
 * \brief - Configure the additional header profile hw table according to the
 *          first reference indication.
 * \param [in] unit - Relevant unit.
 * \param [in] headers_profile_mngr_info - Additional_headers_profile management info
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_profile_first_set(
    int unit,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info);

/**
 * \brief - Clear the additional header profile hw table according to the
 *          last reference indication.
 * \param [in] unit - Relevant unit.
 * \param [in] headers_profile_mngr_info - Additional_headers_profile management info
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_profile_last_clear(
    int unit,
    dnx_egress_headers_profile_mngr_info_t * headers_profile_mngr_info);

/**
 * \brief - Update control word flag bits  in additional header map for
 *        MPLS header
 * \param [in] unit - Relevant unit.
 * \param [in] cw -control word
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_mpls_control_word_set(
    int unit,
    int cw);

/**
 * \brief - Update special mpls label value  in additional header map for
 *        MPLS header
 * \param [in] unit - Relevant unit.
 * \param [in] special_label_value -special mpls label value
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e egress_additional_headers_rfc8321_special_label_set(
    int unit,
    int special_label_value);

/**
 * \brief - clear an entry in
 *        additional header map for udp session. Used for sflow
 *        application.
 * \param [in] unit - Relevant unit.
 * \param [in] udp_ports_profile - udp port profile
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egress_additional_header_map_tables_configuration_udp_ports_clear(
    int unit,
    uint32 udp_ports_profile);

/**
 * \brief - Update udp src port and udp dest port in additional
 *        header map for udp session. Used for sflow
 *        application.
 * \param [in] unit - Relevant unit.
* \param [in] udp_src_port - udp src port value to update
 * \param [in] udp_dest_port - udp dest port value to update
 * \param [in] udp_ports_profile - udp port profile
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egress_additional_header_map_tables_configuration_udp_ports_update(
    int unit,
    uint32 udp_src_port,
    uint32 udp_dest_port,
    uint32 udp_ports_profile);

/**
 * \brief - Update udp dest port in additional header map for
 *        vlxan/vxlan-gpe.
 * \param [in] unit - Relevant unit.
 * \param [in] udp_dest_port - udp dest port value to update
 * \param [in] additional_header_profile - vxlan/vxlan gpe
 *        additional header profile
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egress_additional_headers_map_tables_configuration_vxlan_udp_dest_update(
    int unit,
    uint32 udp_dest_port,
    int additional_header_profile);

/**
 * \brief - Update udp dest port in additional header map for
 *        geneve.
 * \param [in] unit - Relevant unit.
 * \param [in] udp_dest_port - udp dest port value to update
 * \param [in] additional_header_profile - geneve
 *        additional header profile
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egress_additional_headers_map_tables_configuration_geneve_udp_dest_update(
    int unit,
    uint32 udp_dest_port,
    int additional_header_profile);

/*
 * APIs }
 */
#endif /* OUT_LIF_HEADERS_PROFILE_H_INCLUDED */
