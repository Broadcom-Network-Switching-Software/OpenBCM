/** \file vlan_port.c
 *
 *  VLAN port procedures for DNX. Allows creation of
 *  VLAN-Port(AC) entities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/vlan.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>

#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port_match.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/vswitch/vswitch.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/instru/instru_sflow.h>

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define ETHERTYPE_IPV4 (0x0800)
#define ETHERTYPE_IPV6 (0x86dd)
#define ETHERTYPE_MPLS (0x8847)
#define ETHERTYPE_ARP  (0x0806)
#define ETHERTYPE_INITIALIZATION  (0)
/**
 * \brief MPLS L2-VPNs may have native VLAN port. Check whether the gport is an MPLS L2-VPN type.
 */
#define DNX_GPORT_IS_MPLS_L2VPN(_gport_) (BCM_GPORT_IS_MPLS_PORT(_gport_) || BCM_GPORT_IS_TUNNEL(_gport_))
#define DNX_CRITERIA_IS_PON(_criteria_) ((_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED)|| \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL) || \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN) || \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN) || \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED) || \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED) || \
                                         (_criteria_ == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED))

/*
 * }
 */

/** Prefix value that is added to the destination field when building the expected payload in the VTT */
#define LIF_LEARN_INFO_OPTIMIZED_PREFIX 12
#define IN_AC_TCAM_DB_PCP_MASK (0xE)

/**
 * \brief
 *   Enumeration regarding the required In-LIF learn info field
 *   NO_REQUIREMENT: No hard requirement, the actual learn info
 *       can be used to select a suitable learn info type.
 *   BASIC_LIF: A result type with basic LIF learn info is
 *       required.
 *   EXTENDED_LIF: A result type with Extended LIF learn info is
 *       required.
 */
typedef enum
{
    LIF_LEARN_INFO_REQ_TYPE_INVALID = -1,
    LIF_LEARN_INFO_REQ_TYPE_FIRST,
    LIF_LEARN_INFO_REQ_TYPE_NO_REQUIREMENT = LIF_LEARN_INFO_REQ_TYPE_FIRST,
    LIF_LEARN_INFO_REQ_TYPE_BASIC_LIF,
    LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF,
    LIF_LEARN_INFO_REQ_TYPE_LAST = LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF,
    LIF_LEARN_INFO_REQ_TYPE_COUNT
} lif_learn_info_type_e;

/**
 * \brief
 *   Holds information for the entry to be inserted to the in
 *   lif table
 */
typedef struct
{
    /**
     * \brief
     *  Dbal table id
     */
    dbal_tables_e dbal_table_id;
    /**
     * \brief
     *   Global lif of the entry
     */
    int global_lif;
    /**
     * \brief
     *   vsi of the LIF
     */
    bcm_vlan_t vsi;
    /**
     * \brief
     *   fodo assignment mode of the LIF
     */
    int fodo_assignment_mode;
    /**
     * \brief
     *   in lif profile
     */
    int in_lif_profile;
    /**
     * \brief
     *   learn_enable
     */
    uint32 learn_enable;
    /**
     * \brief
     *   learn_context
     */
    dbal_enum_value_field_learn_payload_context_e learn_context;
    /**
     * \brief
     *   learn_info
     */
    uint32 learn_info;
    /**
     * \brief
     *   protection_pointer
     */
    int protection_pointer;
    /**
     * \brief
     *   protection_path
     */
    int protection_path;
    /**
     * \brief
     *   flush_group
     */
    uint32 flush_group;
    /**
     * \brief
     *   large_learn_field
     */
    lif_learn_info_type_e learn_info_type;
    /**
     * \brief
     *   old_local_inlif - the local_inlif to be "replaced", used only in case of REPLACE
     */
    int old_local_inlif;
} dnx_ingress_ac_table_info_t;

/**
 * \brief
 *   Holds information for ingress table ids (outlif) and the
 *   accompanying result types
 */
typedef struct
{
    /**
     * \brief
     *  Dbal table id
     */
    dbal_tables_e dbal_table_id;
    /**
     * \brief
     *   EEDB phase of the given table
     */
    int outlif_phase;
    /**
     * \brief
     *   Local lif of the next EEDB entry, pointed from this entry.
     */
    int next_outlif;
    /**
     * \brief
     *   out lif profile.
     */
    int out_lif_profile;
    /**
     * \brief
     *   protection_pointer
     */
    int protection_pointer;
    /**
     * \brief
     *   protection_path
     */
    int protection_path;
    /**
     * \brief
     *   is last ethernet layer indication
     *   (similar to is native)
     */
    int last_eth_layer;
    /**
     * nwk_qos_index
     */
    int nwk_qos_idx;
    /**
     * egress qos model
     */
    dbal_enum_value_field_encap_qos_model_e qos_model;
    /**
     * \brief
     *   old_local_outlif - the local_outlif to be "replaced", used only in case of REPLACE
     */
    int old_local_outlif;
    /**
     * pkt_pri - the pcp to be "replaced", used only in case of REPLACE
     */
    uint8 pkt_pri;
    /**
     * pkt_cfi - the dei to be "replaced", used only in case of REPLACE
     */
    uint8 pkt_cfi;
    /**
    * tunnel_id
    */
    uint16 tunnel_id;
} dnx_egress_ac_table_info_t;

/**
 * \brief
 *   Holds HW fields values that are read form the VLAN-Port table (during find/replace/destroy APIs)
 */
typedef struct
{

    /**
     * \brief
     *   in_lif_profile
     */
    uint32 in_lif_profile;

    /**
     * \brief
     *   destination
     */
    uint32 destination;

    /**
     * \brief
     *   service-type
     */
    uint32 service_type;

    /**
     * \brief
     *   is the LIF P2P
     */
    int is_p2p;

    /**
     * \brief
     *   glob_out_lif_or_eei
     */
    uint32 glob_out_lif_or_eei_value;

    /**
     * \brief
     *   is global-out-lif exist
     */
    int is_glob_out_lif_exist;

    /**
     * \brief
     *   is eei exist
     */
    int is_eei_exist;
} dnx_ingress_ac_fields_table_info_get_t;

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
     *  Performs Egress/Ingress VLAN edit only
     */
    VLAN_TRANSLATION_AC_APPLICATION_TYPE,
    /**
     *  Aggregates VLANs on a port to one In-LIF
     */
    OPTIMIZATION_AC_APPLICATION_TYPE,
    NUM_AC_APPLICATION_TYPE_E
} ac_application_type_e;

static shr_error_e dnx_vlan_port_find_by_gport(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info);

/**
 * \brief - function returns whether a given VLAN-Port gport is allocated
 */
static shr_error_e
dnx_vlan_port_is_allocated(
    int unit,
    bcm_gport_t gport,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = FALSE;

    /*
     * Use VLAN-Port sub type for each case, checking its algo "is_allocaed:
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
    {
        /*
         * Handle Ingress Virtual Native Vlan port
         */
        int sw_handle;

        sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(gport);

        SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.is_allocated(unit, sw_handle, is_allocated));

    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        /*
         * Handle Egress Virtual Vlan ports: ESEM Native or non-Native:
         */
        int esem_handle;

        esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(gport);

        SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.is_allocated(unit, esem_handle, is_allocated));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Egress ESEM Default:
         */
        int esem_handle;
        /*
         * Decode the esem_handle from the virtual gport
         */
        esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport);

        /*
         * Allow the esem hanlde to be found if they are reserved as AC, even though they are not created by user
         */
        if (esem_handle == dnx_data_esem.default_result_profile.default_native_get(unit) ||
            esem_handle == dnx_data_esem.default_result_profile.default_ac_get(unit))
        {
            *is_allocated = TRUE;
        }
        else if (esem_handle >= dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Given ESEM default result profile (%d) is not in a valid range (0 - %d),"
                         " and not resverved for default AC (%d) or default native AC (%d).",
                         esem_handle, dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit),
                         dnx_data_esem.default_result_profile.default_ac_get(unit),
                         dnx_data_esem.default_result_profile.default_native_get(unit));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.is_allocated(unit, esem_handle, is_allocated));
        }
    }
    else
    {
        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 flags;

        if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY)
        {
            /*
             * Ingress only:
             */
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
        }
        else if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)
        {
            /*
             * Egress Only
             */
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
        else
        {
            flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF;
        }

        /*
         * Check local LIFs allocation using DNX Algo Gport Management:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &gport_hw_resources));

        /*
         * If we got here, the LIF was allocated.
         */
        *is_allocated = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create/find
 * The function verifies flags field - for correct ingress/egress only setting.
 */
static shr_error_e
dnx_vlan_port_direction_flags_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Verify that both flags BCM_VLAN_PORT_CREATE_INGRESS_ONLY BCM_VLAN_PORT_CREATE_EGRESS_ONLY are not set
     */
    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, Both BCM_VLAN_PORT_CREATE_INGRESS_ONLY and BCM_VLAN_PORT_CREATE_EGRESS_ONLY are set! flags = 0x%08X\n",
                     vlan_port->flags);
    }
    else if ((is_egress == TRUE) && (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, Unavailable criteria BCM_VLAN_PORT_MATCH_PORT (%d) in egress, flags = 0x%08X\n",
                     vlan_port->criteria, vlan_port->flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create/find
 * The function verifies that criteria field is supported and
 * it is also verifies the proper match fields.
 */
static shr_error_e
dnx_vlan_port_criteria_verify(
    int unit,
    int is_match_none_valid,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Check supported criterias
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
            if (is_match_none_valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. criteria = BCM_VLAN_PORT_MATCH_NONE (%d) is not supported! flags = 0x%08X\n",
                             vlan_port->criteria, vlan_port->flags);
            }
            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            break;

        case BCM_VLAN_PORT_MATCH_PORT:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
            /*
             * Match port and port X CVLAN, port X VLAN X PCP, port X VLAN X VLAN X PCP, port X SVALN X SVLAN, port X CVLAN X CVLAN are not supported for Ingress Native AC !
             */
            if ((is_ingress == TRUE) && (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)))
            {

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! Ingress Native unsupported criteria. flags = 0x%08X, criteria = %d\n"
                             "Only BCM_VLAN_PORT_MATCH_NONE(%d),BCM_VLAN_PORT_MATCH_PORT_VLAN(%d) and "
                             "BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED(%d) are supported for Ingress Native.\n",
                             vlan_port->flags, vlan_port->criteria, BCM_VLAN_PORT_MATCH_NONE,
                             BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED);
            }
            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:

            if (is_ingress == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) but flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is not set! flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }

            if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) but flag BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) is not set! flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) is not supported for native! (flag BCM_VLAN_PORT_NATIVE (0x%08X) is set!) flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_NATIVE, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  %d (Namespace-VSI (%d) or Namespace-Port (%d)) is not supported with flag BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS (0x%08X)! flags = 0x%08X\n",
                             vlan_port->criteria, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, BCM_VLAN_PORT_MATCH_NAMESPACE_PORT,
                             BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS, vlan_port->flags);
            }

            break;

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:

            if (is_ingress == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  BCM_VLAN_PORT_MATCH_PORT_UNTAGGED (%d) is ingress configuration! (flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is set!) flags = 0x%08X\n",
                             BCM_VLAN_PORT_MATCH_PORT_UNTAGGED, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria =  BCM_VLAN_PORT_MATCH_PORT_UNTAGGED (%d) is not supported for native! (flag BCM_VLAN_PORT_NATIVE (0x%08X) is set!) flags = 0x%08X\n",
                             BCM_VLAN_PORT_MATCH_PORT_UNTAGGED, BCM_VLAN_PORT_NATIVE, vlan_port->flags);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported criteria (%d)!\n"
                         "Only BCM_VLAN_PORT_MATCH_NONE(%d), BCM_VLAN_PORT_MATCH_PORT(%d), "
                         "and BCM_VLAN_PORT_MATCH_PORT_VLAN(%d), BCM_VLAN_PORT_MATCH_PORT_CVLAN(%d), "
                         "and BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN(%d), BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED(%d), BCM_VLAN_PORT_MATCH_NAMESPACE_VSI(%d), "
                         "BCM_VLAN_PORT_MATCH_NAMESPACE_PORT(%d), BCM_VLAN_PORT_MATCH_PORT_UNTAGGED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED(%d), BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_TUNNEL(%d), BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN(%d), BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED(%d), BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED(%d), "
                         "BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED(%d) are supported\n",
                         vlan_port->criteria, BCM_VLAN_PORT_MATCH_NONE, BCM_VLAN_PORT_MATCH_PORT,
                         BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_CVLAN,
                         BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN, BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED,
                         BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                         BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, BCM_VLAN_PORT_MATCH_PORT_UNTAGGED,
                         BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED, BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED,
                         BCM_VLAN_PORT_MATCH_PORT_TUNNEL, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN,
                         BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED,
                         BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED, BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED,
                         BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED);
            break;
    }

    /*
     * Verify match criteria:
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:

            /*
             * For criteria NONE, match_vlan and match_inner_vlan must be 0!
             * Note: port can be non-zero, since it used for setting learning!
             */

            if (vlan_port->match_vlan != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NONE (%d) but match_vlan = %d is not 0! flags = 0x%08X.\n",
                             BCM_VLAN_PORT_MATCH_NONE, vlan_port->match_vlan, vlan_port->flags);
            }

            if (vlan_port->match_inner_vlan != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NONE (%d) but match_inner_vlan = %d is not 0! flags = 0x%08X\n",
                             BCM_VLAN_PORT_MATCH_NONE, vlan_port->match_inner_vlan, vlan_port->flags);
            }

            break;
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
            /*
             * Verify match_vlan is in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:

            /*
             * Verify match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_inner_vlan);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            /*
             * Verify match_pcp match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_PCP_CHK_ID(unit, vlan_port->match_pcp);
            break;

        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            /*
             * Verify match_pcp match_vlan and match_inner_vlan are in range:
             */
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_vlan);
            BCM_DNX_VLAN_CHK_ID(unit, vlan_port->match_inner_vlan);
            BCM_DNX_PCP_CHK_ID(unit, vlan_port->match_pcp);
            break;

        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Verify Namespace range:
             */
            uint32 network_domain_max_val;
            int nof_vsis;

            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, DBAL_FIELD_NETWORK_DOMAIN, FALSE,
                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI, 0,
                             (int *) &network_domain_max_val));

            if (vlan_port->match_class_id > network_domain_max_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) but match_class_id = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, vlan_port->match_class_id, network_domain_max_val);
            }

            /*
             * Verify VSI range:
             */
            SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
            if (vlan_port->vsi >= nof_vsis)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) but vsi = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_VSI, vlan_port->vsi, (nof_vsis - 1));
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            uint32 namespace_max_val;

            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, DBAL_FIELD_NAMESPACE, FALSE,
                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT, 0,
                             (int *) &namespace_max_val));

            if (vlan_port->match_class_id > namespace_max_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) but match_class_id = %d is out of range [0:%d]!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->match_class_id, namespace_max_val);
            }

            /*
             * Verify port:
             *   - should be encoded as gport system port.
             * Note:
             *  Trunk is also supported
             */
            if (!(BCM_GPORT_IS_SYSTEM_PORT(vlan_port->port) || (BCM_GPORT_IS_TRUNK(vlan_port->port))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error ! criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) but port = 0x%08X is not SYSTEM_PORT (%d << %d) or TRUNK (%d << %d)!\n",
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->port, _SHR_GPORT_TYPE_SYSTEM_PORT,
                             _SHR_GPORT_TYPE_SHIFT, _SHR_GPORT_TYPE_TRUNK, _SHR_GPORT_TYPE_SHIFT);
            }

            break;
        }
        default:
            /*
             *Type of supported criterias has been checked before.
             */
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Determines the outlif profile allocation table based on input flags/criteria
 */
static shr_error_e
dnx_vlan_port_outlif_profile_table_determine(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_egress_ac_table_info_t * egress_ac_table_info,
    dbal_tables_e * egress_outlif_profile_table,
    uint32 *default_outlif_profile)
{

    SHR_FUNC_INIT_VARS(unit);
    /**
     * If the criteria is MATCH_NAMESPACE_VSI;
     * the orientation should be allocated per DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB,
     * which is a subject to ARR Prefix, which is used for VXLAN and should be supported only in the IP namespace scope
     */
    if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
    {
        *egress_outlif_profile_table = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
        *default_outlif_profile = DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE;
    }
    else
    {
        *egress_outlif_profile_table = egress_ac_table_info->dbal_table_id;
        *default_outlif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_verify(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    int is_virtual_ac)
{
    int is_ingress, is_egress, fec, fec_id, ing_table_capacity, eg_table_capacity, nof_vsis;
    uint32 max_fec_id_value;
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    /*
     * Verify that legacy FEC information wasn't provided.
     * In DNX FEC allocation moved to a unified API - bcm_l3_egress_Create().
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CASCADED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_CASCADED (0x%08X) flag is not supported. flags = 0x%08X. "
                     "FEC creation is done in bcm_l3_egress_create() API.\n", BCM_VLAN_PORT_CASCADED, vlan_port->flags);
    }

    /*
     * Split Horizon
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NETWORK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_NETWORK (0x%08X) flag is not supported. flags = 0x%08X. "
                     "Orientation is configured through network_group field.\n", BCM_VLAN_PORT_NETWORK,
                     vlan_port->flags);
    }

    /** Verify only supported flags are set */
    supported_flags =
        BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_ENCAP_WITH_ID | BCM_VLAN_PORT_CREATE_INGRESS_ONLY
        | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_EGRESS_PROTECTION | BCM_VLAN_PORT_VSI_BASE_VID |
        BCM_VLAN_PORT_INGRESS_WIDE | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_DEFAULT |
        BCM_VLAN_PORT_STAT_INGRESS_ENABLE | BCM_VLAN_PORT_STAT_EGRESS_ENABLE | BCM_VLAN_PORT_ALLOC_SYMMETRIC |
        BCM_VLAN_PORT_RECYCLE | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;

    if (vlan_port->flags & (~supported_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: 0x%08x\n",
                     vlan_port->flags, supported_flags);
    }

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;

    /*
     * Verify direction flags BCM_VLAN_PORT_CREATE_INGRESS_ONLY / BCM_VLAN_PORT_CREATE_EGRESS_ONLY
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_direction_flags_verify(unit, vlan_port));

    /*
     * Check supported criterias and Verify match criteria.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_criteria_verify(unit, TRUE, vlan_port));

    /** Verify L2 protection information */
    SHR_IF_ERR_EXIT(dnx_failover_l2_protection_info_verify(unit, vlan_port, NULL));

    /**ingress qos*/
    if ((vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelInvalid) ||
        (vlan_port->ingress_qos_model.ingress_remark != bcmQosIngressModelInvalid) ||
        (vlan_port->ingress_qos_model.ingress_ttl != bcmQosIngressModelInvalid))
    {

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            /*
             * Only for Control LIF, ingress QOS model is supported:
             *   - ingress_phb, ingress_remark and ingress_ttl - should be the same.
             *   - Only Uniform or Short-pipe are supported.
             */
            if ((vlan_port->ingress_qos_model.ingress_phb != vlan_port->ingress_qos_model.ingress_remark) ||
                (vlan_port->ingress_qos_model.ingress_phb != vlan_port->ingress_qos_model.ingress_ttl))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "For Control LIF - all ingress qos model values should be the same! ingress_phb = %d, ingress_remark = %d, ingress_ttl = %d",
                             vlan_port->ingress_qos_model.ingress_phb, vlan_port->ingress_qos_model.ingress_remark,
                             vlan_port->ingress_qos_model.ingress_ttl);
            }

            if ((vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelUniform) &&
                (vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelShortpipe))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "For Control LIF - ingress qos model values should be bcmQosIngressModelUniform = %d, or bcmQosIngressModelShortpipe =%d ! ingress_phb = %d, ingress_remark = %d, ingress_ttl = %d",
                             bcmQosIngressModelUniform, bcmQosIngressModelShortpipe,
                             vlan_port->ingress_qos_model.ingress_phb, vlan_port->ingress_qos_model.ingress_remark,
                             vlan_port->ingress_qos_model.ingress_ttl);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Ingress qos model is supported only for Control LIF - flag BCM_VLAN_PORT_RECYCLE (0x%08X) is not set! flags = 0x%08X ingress_phb = %d, ingress_remark = %d, ingress_ttl = %d",
                         BCM_VLAN_PORT_RECYCLE, vlan_port->flags, vlan_port->ingress_qos_model.ingress_phb,
                         vlan_port->ingress_qos_model.ingress_remark, vlan_port->ingress_qos_model.ingress_ttl);
        }
    }
    /**egress qos*/
    if (vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelInitial)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress qos model does not support bcmQosEgressModelInitial!");
    }
    if (vlan_port->egress_qos_model.egress_ecn == bcmQosEgressEcnModelEligible)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "vlan port does not support Egress qos model ecn_eligible!");
    }
    if (((vlan_port->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
         (vlan_port->egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace)) &&
        (vlan_port->pkt_pri != 0 || vlan_port->pkt_cfi != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "pkt_pri and pkt_cfi must be 0 if egress qos model is neither PipeMyNameSpace nor PipeNextNameSpace");
    }
    if ((vlan_port->pkt_pri > dnx_data_qos.qos.packet_max_priority_get(unit)) ||
        (vlan_port->pkt_cfi > dnx_data_qos.qos.packet_max_cfi_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pkt_pri or pkt_cfi is out of range");
    }
    /*
     * Verify next pointer (tunnel_id):
     */
    if (vlan_port->tunnel_id != 0)
    {
        /*
         * next outlif pointer (tunnel_id) is relevant only for egress non-virtual native AC is configured
         */
        if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == FALSE) ||
            (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == TRUE) ||
            (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, tunnel_id = 0x%08X can be configured only for non-virtual native AC egress vlan ports!\n"
                         "BCM_VLAN_PORT_NATIVE (0x%08X) flag should be set, BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) should not be set.\n"
                         "flags = 0x%08X\n",
                         vlan_port->tunnel_id, BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
        }

        /*
         * next outlif pointer (tunnel_id) can only be PWE or TUNNEL gport
         */
        if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->tunnel_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, creating Egress non-virtual Native LIF but tunnel_id = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                         vlan_port->tunnel_id, vlan_port->flags);
        }
        
        if (!BCM_GPORT_IS_MPLS_PORT(vlan_port->tunnel_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Egress non-virtual Native LIF may only have PWE GPort as 'tunnel_id' value (given: 0x%08x)\n",
                         vlan_port->tunnel_id);
        }
    }

    /*
     * BCM_VLAN_PORT_RECYCLE flag verification:
     *  Must be:
     *      - ingress-only.
     *      - non-native.
     *      - criteria - none.
     *      - vsi is 0.
     *      - port is 0.
     *      - QOS model is Uniform / Short-pipe
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
    {
        uint32 supported_flags =
            BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE | BCM_VLAN_PORT_STAT_INGRESS_ENABLE |
            BCM_VLAN_PORT_WITH_ID;

        /** Verify ingress-only */
        if (is_egress == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_RECYCLE must be ingress-only!\n", vlan_port->flags);

        }

        /** Verify only supported flags are set */
        if (vlan_port->flags & ~supported_flags)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_RECYCLE with unsupported flags!\n", vlan_port->flags);
        }

        /** Verify criteria is NONE */
        if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_RECYCLE: criteria = %d - must be BCM_VLAN_PORT_MATCH_NONE (%d) !\n",
                         vlan_port->flags, vlan_port->criteria, BCM_VLAN_PORT_MATCH_NONE);
        }

        /** Verify vsi is 0 */
        if (vlan_port->vsi != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! vsi = %d! BCM_VLAN_PORT_RECYCLE must have vsi = 0!\n",
                         vlan_port->flags, vlan_port->vsi);
        }

        /** Verify port is 0 */
        if (vlan_port->port != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! port = %d! BCM_VLAN_PORT_RECYCLE must have port = 0!\n",
                         vlan_port->flags, vlan_port->port);
        }

        /** Verify ingress QOS model type */
        if ((vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelUniform) &&
            (vlan_port->ingress_qos_model.ingress_phb != bcmQosIngressModelShortpipe))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_RECYCLE - ingress qos model values should be bcmQosIngressModelUniform = %d or bcmQosIngressModelShortpipe =%d ! ingress_phb = %d, ingress_remark = %d, ingress_ttl = %d",
                         vlan_port->flags, bcmQosIngressModelUniform, bcmQosIngressModelShortpipe,
                         vlan_port->ingress_qos_model.ingress_phb, vlan_port->ingress_qos_model.ingress_remark,
                         vlan_port->ingress_qos_model.ingress_ttl);
        }
    }

    /*
     * BCM_VLAN_PORT_VLAN_TRANSLATION flag verification:
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
    {

        if ((is_ingress == TRUE) && (is_egress == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X! BCM_VLAN_PORT_VLAN_TRANSLATION can't be used for both ingress and egress in the same call!\n",
                         vlan_port->flags);
        }

        if (is_ingress == TRUE)
        {
            if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, flags = 0x%08X! BCM_VLAN_PORT_VLAN_TRANSLATION flag is applicable for ingress only with BCM_VLAN_PORT_NATIVE flag set.\n",
                             vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) && vlan_port->ingress_network_group_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "vlan_port.ingress_network_group_id(%d) must be 0 for ingress VLAN with BCM_VLAN_PORT_VLAN_TRANSLATION and BCM_VLAN_PORT_NATIVE set.\n",
                             vlan_port->ingress_network_group_id);
            }
        }
        /*
         * If we got here, is_egress is set ...
         *
         * For Egress VLAN translation (Native and non-native) there are two options:
         *   ESEM-Default: criteria must be NONE
         *   ESM: criteria must be port-vlan
         */
        else
        {
            /*
             * Check for ESEM-Default:
             *  - criteria - must be "none"
             *  - vsi - must be 0
             *  - port - must be 0
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT flag is applicable only with criteria BCM_VLAN_PORT_MATCH_NONE (=%d). criteria = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_MATCH_NONE, vlan_port->criteria);
                }

                if (vlan_port->vsi != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT (0x%08X) flag is applicable only with vsi =0! vsi = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_DEFAULT, vlan_port->vsi);
                }

                if (vlan_port->port != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM-default: BCM_VLAN_PORT_DEFAULT (0x%08X) flag is applicable only with port =0! port = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_DEFAULT, vlan_port->port);
                }

                if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "No statistic support for default egress esem default entry (flags set: 0x%x)\n",
                                 vlan_port->flags);
                }
            }
            else
            {
                /*
                 * Check for ESEM:
                 *  - criteria
                 *  - vsi
                 */
                if ((vlan_port->criteria != BCM_VLAN_PORT_MATCH_PORT_VLAN)
                    && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                    && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_PORT))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flags = 0x%08X! For Egress ESEM: applicable only with criteria BCM_VLAN_PORT_MATCH_PORT_VLAN (=%d)"
                                 " or BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) or BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) . criteria = %d\n",
                                 vlan_port->flags, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                                 BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->criteria);
                }

                /*
                 * ESEM out lif:
                 * Need to check that VSI is in range (as the VSI is one of the keys to the ESEM table):
                 * Note:
                 * VSI is valid only for ESEM and ESEM-namesapce-vsi, not valid for ESEM-namesapce-port.
                 */
                if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_VLAN ||
                    vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                {
                    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
                    if (vlan_port->vsi >= nof_vsis && vlan_port->vsi != BCM_VLAN_ALL)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "vsi = %d is not a valid vsi. Valid range is [0:%d], or 0x%04X for all vsis.\n",
                                     vlan_port->vsi, (nof_vsis - 1), BCM_VLAN_ALL);
                    }
                }
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT) ||
                (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI))
            {
                if (vlan_port->egress_qos_model.egress_qos != bcmQosEgressModelUniform)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Only bcmQosEgressModelUniform is supported in case of egress port default AC or criteria is BCM_VLAN_PORT_MATCH_NAMESPACE_VSI.\n");
                }
            }
        }

    }

    /*
     * Verify that each LIF application type is configured coherently across the VLAN-Port structure
     */
    if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
    {
        /*
         * AC Optimization is relevant for INGRESS Only, check that BCM_VLAN_PORT_CREATE_INGRESS_ONLY is set
         */
        if (is_egress == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "AC Optimization is relevant for INGRESS Only,"
                         "Error! BCM_VLAN_PORT_VSI_BASE_VID flag is for ingress only, need to set BCM_VLAN_PORT_CREATE_INGRESS_ONLY."
                         "flags = 0x%08X\n", vlan_port->flags);
        }

        if (((vlan_port->vsi % 4096) == 0) &&
            ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NONE) || (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT)))
        {

            /*
             * Verify that BCM_VLAN_PORT_VLAN_TRANSLATION is not set:
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "BCM_VLAN_PORT_VLAN_TRANSLATION flag cannot be set with BCM_VLAN_PORT_VSI_BASE_VID.\n");
            }

            /*
             * Verify that a flush group isn't configured, as a learning context for Optimized AC
             * with a flush group isn't defined.
             */
            if (vlan_port->group)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flush group can't be supported by an Optimized AC.\n");
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "When BCM_VLAN_PORT_VSI_BASE_VID is set, vsi must be modulo 4096 and criteria must be NONE or MATCH_PORT.\n");

        }
    }
    else if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
    {
        /*
         * Verify that group parameter is in the correct range:
         */
        SHR_MAX_VERIFY(vlan_port->group, SAL_UPTO_BIT(dnx_data_l2.dma.flush_group_size_get(unit)), _SHR_E_PARAM,
                       "group exceeded allowed range\n");
    }
    else
    {
        /*
         * MC Group is a learning attribute, therefore not applicable for a VLAN Translation
         */
        if (vlan_port->failover_mc_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "There's no learning inforamtion for the LIF, failover_mc_group(%u) should be reset\n",
                         vlan_port->failover_mc_group);
        }
    }

    /*
     * Verify ingress_network_group_id range:
     */
    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, vlan_port->ingress_network_group_id));
    }

    /*
     * Verify egress_network_group_id range:
     */
    if (is_egress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, vlan_port->egress_network_group_id));
    }

    /*
     * Verify ingress wide data flag:
     * Only applicable for:
     *  - ingress.
     *  - non-native
     *  - native (AC_MP -not virtual)
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
    {
        if (is_ingress == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) is only valid for symmetric or ingress-only LIF! flags = 0x%08X\n",
                         BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) flag is not valid for virtual native LIF! flags = 0x%08X\n",
                             BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
            }
        }
    }

    /*
     * Verify protection input parameters
     * In case of FEC Protection verify the FEC gport
     */
    if (vlan_port->failover_port_id)
    {
        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(fec, vlan_port->failover_port_id);
        if (fec == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Invalid Gport for FEC protection, failover_port_id = 0x%08X\n",
                         vlan_port->failover_port_id);
        }
        else
        {
            /*
             * Verify that the FEC ID is in range
             */
            fec_id = BCM_L3_ITF_VAL_GET(fec);
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id_value));
            SHR_RANGE_VERIFY(fec_id, 0, max_fec_id_value - 1, _SHR_E_PARAM, "FEC ID is out of bound.\n");
        }

        /*
         * At the FEC protection case the failover MC group should not be set
         */
        if (vlan_port->failover_mc_group)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "At the FEC protection case the failover MC group should not be set. failover_mc_group(%u)\n",
                         vlan_port->failover_mc_group);
        }
    }
    else
    {
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
        {
            if (is_virtual_ac == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Non-LIF protection is not allowed\n");
            }
        }
    }

    /*
     * BCM_VLAN_PORT_WITH_ID verification:
     * Check it is vlan-port.
     * Check the sub-type.
     * Also, check that LIF ID is in range.
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        uint32 low_global_lif_id_bound;
        uint32 high_global_lif_id_bound;
        uint32 global_lif_id_value;

        /*
         * check it is a VLAN-PORT
         */
        if (!BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "LIF type is not VLAN-PORT (%d), check the bits starting at %d with mask 0x%x\n",
                         vlan_port->flags, vlan_port->vlan_port_id, _SHR_GPORT_TYPE_VLAN_PORT, _SHR_GPORT_TYPE_SHIFT,
                         _SHR_GPORT_TYPE_MASK);
        }

        /*
         * for AC LIF, check sub-values
         */
        if (BCM_GPORT_SUB_TYPE_IS_LIF(vlan_port->vlan_port_id))
        {
            global_lif_id_value = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
            low_global_lif_id_bound = dnx_data_l3.rif.nof_rifs_get(unit);
            high_global_lif_id_bound = dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit);

            switch (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id))
            {
                case BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY:
                {
                    if (is_egress)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates INGRESS ONLY LIF but the flag BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08X) is not set!!!\n",
                                     vlan_port->flags, vlan_port->vlan_port_id, BCM_VLAN_PORT_CREATE_INGRESS_ONLY);
                    }
                    break;
                }
                case BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY:
                {

                    if (is_ingress)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates EGRESS ONLY LIF but the flag BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X) is not set!!!\n",
                                     vlan_port->flags, vlan_port->vlan_port_id, BCM_VLAN_PORT_CREATE_EGRESS_ONLY);
                    }
                    break;
                }
                case 0:
                {

                    if ((is_ingress == FALSE) || (is_egress == FALSE))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X), gport = 0x%08X which indicates symmetric LIF but the flags indicates non-symmetric\n",
                                     vlan_port->flags, vlan_port->vlan_port_id);
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X\n",
                                 vlan_port->flags, vlan_port->vlan_port_id);
                }
            }
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
        {
            /*
             * Ingress Virtual Native Vlan port
             */

            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &ing_table_capacity));

            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = 0;
            high_global_lif_id_bound = low_global_lif_id_bound + ing_table_capacity;
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port->vlan_port_id))
        {
            /*
             * Egress Virtual Vlan ports (ESEM Native or non-Native)
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &ing_table_capacity));
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_ESEM, &eg_table_capacity));

            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = ing_table_capacity;
            high_global_lif_id_bound = low_global_lif_id_bound + eg_table_capacity;
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
        {
            /*
             * Egress ESEM Default
             */
            global_lif_id_value = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
            low_global_lif_id_bound = 0;
            high_global_lif_id_bound = dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit);

            /*
             * Don't allow allocation of reserved Egress ESEM Default entries
             */
            if (global_lif_id_value >= high_global_lif_id_bound || global_lif_id_value < low_global_lif_id_bound)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Error, flag BCM_VLAN_PORT_WITH_ID is set with an illegal Virtual Egress Default entry (%d)\n",
                             global_lif_id_value);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "LIF subtype is unknown, subtype = 0x%08X, check the bits starting at %d with mask 0x%x\n",
                         vlan_port->flags, vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_GET(vlan_port->vlan_port_id),
                         _SHR_GPORT_SUB_TYPE_SHIFT, _SHR_GPORT_SUB_TYPE_MASK);
        }

        /*
         * Verify LIF ID is in range
         */
        if ((global_lif_id_value >= high_global_lif_id_bound) || (global_lif_id_value < low_global_lif_id_bound))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_WITH_ID is set (flags = 0x%08X) - wrong gport encoding! gport = 0x%08X,\n"
                         "Global LIF ID = 0x%08X is out of range [0x%08X:0x%08X]\n",
                         vlan_port->flags, vlan_port->vlan_port_id, global_lif_id_value, low_global_lif_id_bound,
                         high_global_lif_id_bound - 1);
        }
    }

    /*
     * BCM_VLAN_PORT_ENCAP_WITH_ID verification:
     * 1. Can only be set for Egress LIF.
     * 2. Can't be set for Egress Virtual LIF (because no global LIF is allocated)
     * 3. If BCM_VLAN_PORT_WITH_ID is used, check that both global_lif values are the same.
     * 4. Check encap_id range
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ENCAP_WITH_ID))
    {
        uint32 high_global_l2_gport_lifs_bound;
        if (is_egress == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_ENCAP_WITH_ID can only be set for Egress LIF! flags = 0x%08X\n",
                         vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)
            || _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flag BCM_VLAN_PORT_ENCAP_WITH_ID can't be set for Egress Virtual LIF! flags = 0x%08X\n",
                         vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            int vlan_port_id_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
            int encap_id_lif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
            if (vlan_port_id_lif != encap_id_lif)
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, both flags BCM_VLAN_PORT_WITH_ID and BCM_VLAN_PORT_ENCAP_WITH_ID are set but their global LIFs value do not match! flags = 0x%08X, vlan_port_id = 0x%08X, encap_id = 0x%08X\n",
                             vlan_port->flags, vlan_port->vlan_port_id, vlan_port->encap_id);
        }

        high_global_l2_gport_lifs_bound = dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit);
        if ((vlan_port->encap_id >= high_global_l2_gport_lifs_bound) || (vlan_port->encap_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, encap_id = 0x%08X is out of range [0:0x%08X]!\n", vlan_port->encap_id,
                         high_global_l2_gport_lifs_bound - 1);
        }
    }

    /*
     * Single side symmetric allocation
     * Invalid with: WITH_ID, REPLACE and without NATIVE
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
    {
        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_VLAN_PORT_ALLOC_SYMMETRIC flag is not supported "
                         "without BCM_VLAN_PORT_NATIVE\n");
        }
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "BCM_VLAN_PORT_ALLOC_SYMMETRIC and BCM_VLAN_PORT_REPLACE cannot be used together");
        }
    }

    
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        /*
         * For non-native, port is irrelevant (=0) when:
         *   - criteria is NONE + ingress only + vsi is not 0 (so the appl type is vlan_translation thus no learning and the port is irrelevant)
         *   - EEDB only
         *   - ESEM Namespace-vsi
         *   - ESEM Default
         */
        if (((vlan_port->criteria == BCM_VLAN_PORT_MATCH_NONE)
             && (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY)) && (vlan_port->vsi != 0))
            || (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY)
                && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
            || (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
            || (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT)))
        {
            if (vlan_port->port != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error!!! port = 0x%08X is irrelevant for this case! flags = 0x%08X, criteria = %d\n",
                             vlan_port->port, vlan_port->flags, vlan_port->criteria);
            }
        }
        else
        {
            dnx_algo_gpm_gport_phy_info_t gport_info;

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
        }
    }
    else
    {
        /*
         * Ingress Native LIF:
         *  - if the criteria is not NONE, the port has to be PWE gport
         */
        if (is_ingress == TRUE)
        {
            if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, creating Ingress Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X, criteria = %d\n",
                                 vlan_port->port, vlan_port->flags, vlan_port->criteria);
                }
            }
        }
        else if (is_egress == TRUE)
        {
            /*
             * Egress Native LIF:
             *  - non-virtual - verify the tunnel_id (is already done above).
             *  - ESEM virtual - verify the port is PWE LIF
             *  - ESEM virtual - Verify CEP lookup isn't applied (match_vlan field is set)
             *  - ESEM-Default - verify the port is 0 (already done above).
             */
            if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == TRUE) &&
                (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT) == FALSE))
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, creating Egress virtual Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                                 vlan_port->port, vlan_port->flags);
                }

                if (vlan_port->match_vlan)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, creating Egress virtual Native LIF with a CEP lookup isn't supported, match_vlan = %d instead of 0\n",
                                 vlan_port->match_vlan);
                }

            }
        }
    }

    /*
     * EEDB only:
     * Verify criteria:
     *  - native - criteria is meaningless (since we "get" the native out-lif from PWE ingress lif) - thus check it is NONE.
     *  - non native -verify that the criteria is NONE
     * Verify vsi is 0!
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) &&
        (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
    {
        if (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, creating Egress Only Native/non-native LIF at EEDB, criteria must be BCM_VLAN_PORT_MATCH_NONE (%d)! flags = 0x%08X, criteria = %d\n",
                         BCM_VLAN_PORT_MATCH_NONE, vlan_port->flags, vlan_port->criteria);
        }

        if (vlan_port->vsi != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, creating Egress Only LIF at EEDB, vsi (%d) is irrelevant and must be0! flags = 0x%08X\n",
                         vlan_port->vsi, vlan_port->flags);
        }
    }

    
    if (vlan_port->vsi == 0)
    {
        int is_symmetric = is_ingress && is_egress;
        int is_ingress_only = is_ingress && (!is_egress);
        int is_esem_default = is_egress && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT)
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
        int is_egress_only_non_virtual = (!is_symmetric) && is_egress
            && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION));
        int is_egress_esem_namespace_port = (!is_symmetric) && is_egress
            && _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) &&
            (vlan_port->criteria == BCM_VLAN_PORT_MATCH_NAMESPACE_PORT);

        if (((!is_symmetric) && (!is_ingress_only) &&
             (ll_app_type != OPTIMIZATION_AC_APPLICATION_TYPE) &&
             (!is_esem_default) && (!is_egress_only_non_virtual) && (!is_egress_esem_namespace_port)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, vsi = 0 for wrong settings!!! flags = 0x%08X\n", vlan_port->flags);
        }
    }

    /**Validate ingress stat_pp flag*/
    if ((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE)) && (!is_ingress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, BCM_VLAN_PORT_STAT_INGRESS_ENABLE is for ingress mode");
    }

    /*
     * Verify BCM_VLAN_PORT_DEFAULT:
     * - must be egress-only
     * - must be virtual
     * - cannot contain "two VIDs"
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
    {
        if (is_ingress)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, the flag BCM_VLAN_PORT_DEFAULT (0x%08X) is an Egress-only flag! flags = 0x%08X",
                         BCM_VLAN_PORT_DEFAULT, vlan_port->flags);
        }

        if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, the flag BCM_VLAN_PORT_DEFAULT (0x%08X) must be set together with BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X)! flags = 0x%08X",
                         BCM_VLAN_PORT_DEFAULT, BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, the flag BCM_VLAN_PORT_DEFAULT (0x%08X) can not be set together with BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS (0x%08X)! flags = 0x%08X",
                         BCM_VLAN_PORT_DEFAULT, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS, vlan_port->flags);
        }
    }

    /*
     * "Two VIDs" Egress virtual flag must be set together with:
     *   - Egress only flag.
     *   - ESEM virtual flag.
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS))
    {

        if (is_ingress)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, the flag BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS (0x%08X) must be set together with BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08X)! flags = 0x%08X",
                         BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS, BCM_VLAN_PORT_CREATE_EGRESS_ONLY,
                         vlan_port->flags);
        }

        if (!is_virtual_ac)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, the flag BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS (0x%08X) must be set together with BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X)! flags = 0x%08X",
                         BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS, BCM_VLAN_PORT_VLAN_TRANSLATION,
                         vlan_port->flags);
        }
    }

    /*
     * Verify PON DTC configuration:
     */
    if ((vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED) ||
        (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED) ||
        (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED))
    {
        if (is_ingress)
        {
            int is_pon_enable = 0;
            int is_pon_dtc_enable = 0;

            SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, vlan_port->port, &is_pon_enable));
            if (is_pon_enable)
            {
                SHR_IF_ERR_EXIT(bcm_vlan_control_port_get
                                (unit, vlan_port->port, bcmVlanPortDoubleLookupEnable, &is_pon_dtc_enable));
            }
            if (!is_pon_dtc_enable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, the criteria (0x%08X) is not supported for PON STC port", vlan_port->criteria);
            }
        }
    }

    /*
     * Verify unsupport configuration:
     */
    if (vlan_port->egress_service_tpid ||
        vlan_port->egress_vlan ||
        vlan_port->egress_inner_vlan ||
        vlan_port->policer_id ||
        vlan_port->egress_policer_id ||
        (vlan_port->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
        (vlan_port->outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
        vlan_port->if_class || vlan_port->class_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, No support for egress_service_tpid, egress_vlan, egress_inner_vlan, policer_id, egress_policer_id, inlif_counting_profile, outlif_counting_profile, if_class, class_id");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create - REPLACE case
 */
static shr_error_e
dnx_vlan_port_create_replace_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    /*
     * Verify API input parameters:
     */
    uint8 is_allocated;
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the flag BCM_VLAN_PORT_WITH_ID is set:
     */
    if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_VLAN_PORT_REPLACE (0x%08X) is set."
                     "Wrong setting. flag BCM_VLAN_PORT_WITH_ID (0x%08X) must be set as well!! flags = 0x%08X.\n",
                     BCM_VLAN_PORT_REPLACE, BCM_VLAN_PORT_WITH_ID, vlan_port->flags);
    }

    /*
     * Verify that the gport is vlan port:
     */
    if (!BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_VLAN_PORT_REPLACE (0x%08X) is set."
                     "Wrong setting. vlan_port_id = 0x%08X is not a VLAN Port!!!\n",
                     BCM_VLAN_PORT_REPLACE, vlan_port->vlan_port_id);
    }

    /*
     * Verify that the gport is allocated:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_is_allocated(unit, vlan_port->vlan_port_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Wrong setting. vlan_port_id = 0x%08X is not allocated!!!\n",
                     vlan_port->vlan_port_id);
    }

    /*
     * Verify ingress wide data flag:
     * Only applicable for:
     *  - ingress.
     *  - non-native
     *  - native (AC_MP -not virtual)
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
    {
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) is only valid for symmetric or ingress-only LIF! flags = 0x%08X\n",
                         BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! BCM_VLAN_PORT_INGRESS_WIDE (0x%08X) flag is not valid for virtual native LIF! flags = 0x%08X\n",
                             BCM_VLAN_PORT_INGRESS_WIDE, vlan_port->flags);
            }
        }
    }

    /*
     * Verify unsupport configuration:
     */
    if (vlan_port->egress_service_tpid ||
        vlan_port->egress_vlan ||
        vlan_port->egress_inner_vlan ||
        vlan_port->policer_id ||
        vlan_port->egress_policer_id ||
        (vlan_port->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
        (vlan_port->outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
        vlan_port->if_class || vlan_port->class_id)

    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, No support for egress_service_tpid, egress_vlan, egress_inner_vlan, policer_id, egress_policer_id, inlif_counting_profile, outlif_counting_profile, if_class, class_id");
    }

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    /*
     * Verify ingress_network_group_id range:
     */
    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, TRUE, vlan_port->ingress_network_group_id));
    }

    /*
     * Verify egress_network_group_id range:
     */
    if (is_egress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_switch_network_group_id_verify(unit, FALSE, vlan_port->egress_network_group_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_create - REPLACE case
 * It compares the new configuration vs old configuration.
 * Fields that cannot be changed need to be same as the old
 * configuration!
 */
static shr_error_e
dnx_vlan_port_create_replace_new_old_compare_verify(
    int unit,
    bcm_vlan_port_t * new_vlan_port,
    bcm_vlan_port_t * old_vlan_port)
{
    uint8 is_physical_port;
    int is_ingress, is_egress, is_native_ac, is_virtual_ac;
    uint32 supported_flags;
    int is_support_change_protect;

    SHR_FUNC_INIT_VARS(unit);

    supported_flags =
        BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_ENCAP_WITH_ID | BCM_VLAN_PORT_INGRESS_WIDE |
        BCM_VLAN_PORT_STAT_INGRESS_ENABLE | BCM_VLAN_PORT_STAT_EGRESS_ENABLE |
        BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;

    /*
     * Verify flags are same as previous configuration (apart from the above supported flag that can be set):
     */
    if ((new_vlan_port->flags & ~supported_flags) != ((old_vlan_port->flags & ~supported_flags)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! VLAN-Port REPLACE: Cannot change flags!\n"
                     "New flags = 0x%08X, previous flags = 0x%08X differ in flags other then "
                     "BCM_VLAN_PORT_REPLACE(0x%08X), BCM_VLAN_PORT_WITH_ID(0x%08X), BCM_VLAN_PORT_ENCAP_WITH_ID(0x%08X), "
                     "BCM_VLAN_PORT_INGRESS_WIDE(0x%08X) or BCM_VLAN_PORT_STAT_INGRESS_ENABLE(0x%08X)!\n",
                     new_vlan_port->flags, old_vlan_port->flags, BCM_VLAN_PORT_REPLACE, BCM_VLAN_PORT_WITH_ID,
                     BCM_VLAN_PORT_ENCAP_WITH_ID, BCM_VLAN_PORT_INGRESS_WIDE, BCM_VLAN_PORT_STAT_INGRESS_ENABLE);
    }

    /*
     * Verify that fields that cannot be changed are the same:
     *   1. All the match-info fields:
     *           - criteria
     *           - match_vlan
     *           - match_inner_vlan
     *           - match_pcp
     *           - match_ethertype
     *           - match_class_id
     */
    if ((new_vlan_port->criteria != old_vlan_port->criteria) ||
        (new_vlan_port->match_vlan != old_vlan_port->match_vlan) ||
        (new_vlan_port->match_inner_vlan != old_vlan_port->match_inner_vlan) ||
        (new_vlan_port->match_pcp != old_vlan_port->match_pcp) ||
        (new_vlan_port->match_ethertype != old_vlan_port->match_ethertype) ||
        (new_vlan_port->match_class_id != old_vlan_port->match_class_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! VLAN-Port REPLACE: Cannot change match-info!\n"
                     "criteria(%d Vs %d)/match_vlan(%d Vs %d)/match_inner_vlan(%d Vs %d)/match_pcp(%d Vs %d)/match_ethertype(%d Vs %d/match_class_id(%d Vs %d) must be the same!\n",
                     new_vlan_port->criteria, old_vlan_port->criteria,
                     new_vlan_port->match_vlan, old_vlan_port->match_vlan,
                     new_vlan_port->match_inner_vlan, old_vlan_port->match_inner_vlan,
                     new_vlan_port->match_pcp, old_vlan_port->match_pcp,
                     new_vlan_port->match_ethertype, old_vlan_port->match_ethertype,
                     new_vlan_port->match_class_id, old_vlan_port->match_class_id);
    }

    is_ingress = _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    is_native_ac = _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual_ac = _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    /*
     * protect allow to change only when:
     *      small wide-AC(8bits) <--> normal AC without Protect
     *      small wide-AC(8bits) <--> large wide-AC(28bits)
     * So not allow to change protect bwteen non-wide ACs
     */
    is_support_change_protect = 1;
    if (!_SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE)
        && (!_SHR_IS_FLAG_SET(new_vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE)))
    {
        is_support_change_protect = 0;
    }

    /*
     * For Ingress - need to verify port:
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, new_vlan_port->port, &is_physical_port));

        if (is_physical_port)
        {
            uint32 new_vlan_domain;
            uint32 old_vlan_domain;

            /*
             * Get port's Vlan Domain
             */
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, new_vlan_port->port, &new_vlan_domain));
            SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, old_vlan_port->port, &old_vlan_domain));

            if (new_vlan_domain != old_vlan_domain)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! VLAN-Port REPLACE: cannot change port! port = %d and previous port = %d has different VLAN-Domain (%d Vs %d)!\n",
                             new_vlan_port->port, old_vlan_port->port, new_vlan_domain, old_vlan_domain);
            }
        }
        else
        {
            /*
             * verify the native AC match key
             */
            if (is_native_ac)
            {
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                int old_match_lif, new_match_lif;
                uint32 old_next_layer_nwk_domain, new_next_layer_nwk_domain;
                uint32 old_is_intf_namespace, new_is_intf_namespace;

                /** Get Global In-LIF of the new vlan port*/
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, new_vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                             &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                new_match_lif = gport_hw_resources.global_in_lif;

                /** get vlan_domain and LIF or NETWORK scope for new vlan-port*/
                SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                                (unit, gport_hw_resources, &new_next_layer_nwk_domain, &new_is_intf_namespace));

                /** Get Global In-LIF of the old vlan port*/
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, old_vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                             &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                old_match_lif = gport_hw_resources.global_in_lif;

                /** get vlan_domain and LIF or NETWORK scope for new vlan-port*/
                SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                                (unit, gport_hw_resources, &old_next_layer_nwk_domain, &old_is_intf_namespace));

                if ((old_is_intf_namespace != new_is_intf_namespace) ||
                    ((new_is_intf_namespace == TRUE) && (old_match_lif != new_match_lif)) ||
                    ((new_is_intf_namespace == FALSE) && (old_next_layer_nwk_domain != new_next_layer_nwk_domain)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error! VLAN-Port REPLACE: Current and the original Ingress Native AC\n"
                                 "have different match scope type (%d v.s. %d) or scope objects (%d v.s. %d)!\n",
                                 new_is_intf_namespace, old_is_intf_namespace,
                                 ((new_is_intf_namespace == TRUE) ? new_match_lif : new_next_layer_nwk_domain),
                                 ((new_is_intf_namespace == FALSE) ? old_match_lif : old_next_layer_nwk_domain));
                }

                if ((is_virtual_ac) &&
                    (old_vlan_port->ingress_network_group_id != new_vlan_port->ingress_network_group_id))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error! VLAN-Port REPLACE: Current and the original Ingress Native AC\n"
                                 "have different ingress network group (%d v.s. %d)!\n",
                                 new_vlan_port->ingress_network_group_id, old_vlan_port->ingress_network_group_id);
                }
            }
        }

        /*
         * For symmetric case, VSI cannot be change!
         * VSI change should be done by bcm_vswitch_port_add
         */

        if ((is_egress) && (new_vlan_port->vsi != old_vlan_port->vsi))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! VLAN-Port REPLACE: Symmetric LIF: cannot change vsi! vsi = %d and previous vsi = %d are different!\n"
                         "Please use bcm_vswitch_port_add to change VSI.\n", new_vlan_port->vsi, old_vlan_port->vsi);
        }

        /*
         * Verify that Ingress Protection wasn't changed between Protected and UnProtected states
         */
        if (!is_support_change_protect)
        {
            if ((!(new_vlan_port->ingress_failover_id)) != (!(old_vlan_port->ingress_failover_id)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! VLAN-Port REPLACE: Ingress failover state can't change between Protected and Unprotected.\n"
                             "Previous value - %d, New value - %d\n", old_vlan_port->ingress_failover_id,
                             new_vlan_port->ingress_failover_id);
            }
        }

        /*
         * Verify that Ingress Protection learning mode wasn't changed
         */
        if ((!(new_vlan_port->failover_mc_group)) != (!(old_vlan_port->failover_mc_group)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! VLAN-Port REPLACE: Ingress failover learning can't change between MC group and Port learning.\n"
                         "Previous value - %d, New value - %d\n", old_vlan_port->failover_mc_group,
                         new_vlan_port->failover_mc_group);
        }

        /*
         * Verify that the MACT group wasn't changed between No-group and Valid group states
         */
        if ((!(new_vlan_port->group)) != (!(old_vlan_port->group)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! VLAN-Port REPLACE: MACT group can't change between No-group and a Valid group.\n"
                         "Previous value - %d, New value - %d\n", old_vlan_port->group, new_vlan_port->group);
        }
    }

    /*
     * Egress Verifications
     */
    if (is_egress)
    {
        if (is_virtual_ac)
        {
            /*
             * For Egress Virtual, verify that the keys (that is match-info) are not changed:
             */
            switch (old_vlan_port->criteria)
            {

                case BCM_VLAN_PORT_MATCH_NONE:
                {
                    /*
                     * ESEM Default:
                     *   - the key is "taken "from the gport thus nothing to check.
                     */
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_VLAN:
                {
                    /*
                     * ESEM AC - native or non-native:
                     */
                    if (is_native_ac)
                    {
                        /*
                         * ESEM AC Native:
                         * The keys are:
                         *      - VSI
                         *      - local_out_lif - taken from port field - must be same as previous.
                         */

                        if (new_vlan_port->port != old_vlan_port->port)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Error! VLAN-Port REPLACE: Egress Virtual ESEM Native: cannot change port! port = 0x%08X and previous port = 0x%08X are different!\n",
                                         new_vlan_port->port, old_vlan_port->port);
                        }

                        if (new_vlan_port->vsi != old_vlan_port->vsi)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Error! VLAN-Port REPLACE: Egress Virtual ESEM Native: cannot change vsi! vsi = %d and previous vsi = %d are different!\n",
                                         new_vlan_port->vsi, old_vlan_port->vsi);
                        }
                    }
                    else
                    {
                        /*
                         * ESEM AC non-native
                         * The keys are:
                         *      - VLAN-Domain - taken from port field - must be same as previous.
                         *      - VSI
                         *      - C-VID - (taken from match_vlan field - already checked in dnx_vlan_port_create_replace_verify)
                         */

                        if (new_vlan_port->port != old_vlan_port->port)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Error! VLAN-Port REPLACE: Egress Virtual ESEM : cannot change port! port = 0x%08X and previous port = 0x%08X are different!\n",
                                         new_vlan_port->port, old_vlan_port->port);
                        }

                        if (new_vlan_port->vsi != old_vlan_port->vsi)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Error! VLAN-Port REPLACE: Egress Virtual ESEM: cannot change vsi! vsi = %d and previous vsi = %d are different!\n",
                                         new_vlan_port->vsi, old_vlan_port->vsi);
                        }

                        if (new_vlan_port->match_vlan != old_vlan_port->match_vlan)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "Error! VLAN-Port REPLACE: Egress Virtual ESEM: cannot change match_vlan! match_vlan = %d and previous match_vlan = %d are different!\n",
                                         new_vlan_port->match_vlan, old_vlan_port->match_vlan);
                        }
                    }
                    break;
                }
                case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
                {
                    /*
                     * ESEM Namespace-vsi
                     * The keys are:
                     *      - VLAN-Domain (taken from match_class_id field - already checked above)
                     *      - VSI
                     *      - C-VID - (taken from match_vlan field - already checked abvoe)
                     */

                    if (new_vlan_port->vsi != old_vlan_port->vsi)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error! VLAN-Port REPLACE: Egress Virtual ESEM namespace-vsi: cannot change vsi! vsi = %d and previous vsi = %d are different!\n",
                                     new_vlan_port->vsi, old_vlan_port->vsi);
                    }

                    break;
                }
                case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
                {
                    /*
                     * ESEM Namespace-port
                     * The keys are:
                     *      - VLAN-Domain (taken from match_class_id field - already checked above)
                     *      - system-port-aggregate - taken from port field - must be set to same!
                     */
                    if (new_vlan_port->port != old_vlan_port->port)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error! VLAN-Port REPLACE: Egress Virtual ESEM namespace-port: cannot change port! port = 0x%08X and previous port = 0x%08X are different!\n",
                                     new_vlan_port->port, old_vlan_port->port);
                    }

                    break;
                }
                default:

                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                                 new_vlan_port->criteria, new_vlan_port->vlan_port_id, new_vlan_port->flags);
            }
        }
        else
        {
            /*
             * For Egress Non-Virtual, verify that Egress Protection
             * wasn't changed between Protected and UnProtected states
             */
            if (_SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION) !=
                _SHR_IS_FLAG_SET(new_vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! VLAN-Port REPLACE: Egress failover state can't change between Protected and Unprotected.\n"
                             "Previous value - %d, New value - %d\n",
                             _SHR_IS_FLAG_SET(old_vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION),
                             _SHR_IS_FLAG_SET(new_vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION));
            }
            if (_SHR_IS_FLAG_SET(new_vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION) &&
                ((!(new_vlan_port->egress_failover_id)) != (!(old_vlan_port->egress_failover_id))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! VLAN-Port REPLACE: Egress failover state can't change between Protected and Unprotected.\n"
                             "Previous value - %d, New value - %d\n", old_vlan_port->egress_failover_id,
                             new_vlan_port->egress_failover_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_verify(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");
    /*
     * For Find using lookup:
     */
    if (vlan_port->vlan_port_id == 0)
    {
        /*
         * Verify direction falgs BCM_VLAN_PORT_CREATE_INGRESS_ONLY / BCM_VLAN_PORT_CREATE_EGRESS_ONLY
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_direction_flags_verify(unit, vlan_port));
        /*
         * Check supported criterias and verify match criteria.
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_criteria_verify(unit, FALSE, vlan_port));
        /*
         * For Egress only, only ESEM is supported:
         *   - EEDB - can't find (the key is OUT_LIF).
         *   - ESEM-Default - can't find (the key is ESEM_DEFAULT_RESULT_PROFILE)
         *   - ESEM - can check for lookup - check EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW to find such a lookup
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == TRUE)
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Egress only VLAN-PORT find by lookup only support ESEM lookup! the flag BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08X) should be set. flags = 0x%08X \n",
                             BCM_VLAN_PORT_VLAN_TRANSLATION, vlan_port->flags);
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Egress only VLAN-PORT find by lookup only support ESEM lookup! the flag BCM_VLAN_PORT_DEFAULT (0x%08X) should not be set. flags = 0x%08X \n",
                             BCM_VLAN_PORT_DEFAULT, vlan_port->flags);
            }

            if ((vlan_port->criteria != BCM_VLAN_PORT_MATCH_PORT_VLAN)
                && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_VSI)
                && (vlan_port->criteria != BCM_VLAN_PORT_MATCH_NAMESPACE_PORT))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, flags = 0x%08X! For Egress ESEM: applicable only with criteria BCM_VLAN_PORT_MATCH_PORT_VLAN (=%d)"
                             " or BCM_VLAN_PORT_MATCH_NAMESPACE_VSI (%d) or BCM_VLAN_PORT_MATCH_NAMESPACE_PORT (%d) . criteria = %d\n",
                             vlan_port->flags, BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_NAMESPACE_VSI,
                             BCM_VLAN_PORT_MATCH_NAMESPACE_PORT, vlan_port->criteria);
            }
        }

        /*
         * Verify Native AC find by lookup:
         *   - native flag must be set.
         *   - ingress_only or egress_only flag must be set.
         *   - port must be PWE or TUNNEL gport.
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
        {
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) == TRUE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Find by lookup of Ingress Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X, criteria = %d\n",
                                 vlan_port->port, vlan_port->flags, vlan_port->criteria);
                }
            }
            else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) == TRUE)
            {
                if (!DNX_GPORT_IS_MPLS_L2VPN(vlan_port->port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Error, Find by lookup of Egress virtual Native LIF but port = 0x%08X is not MPLS-L2VPN gport! flags = 0x%08X\n",
                                 vlan_port->port, vlan_port->flags);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, Find by lookup of Native LIF but no ingress_only (0x%08X) or egress_only (0x%08X) flags are set! flags = 0x%08X\n",
                             BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, vlan_port->flags);
            }
        }

        /*
         * Verify unsupport configuration:
         */
        if (vlan_port->egress_service_tpid ||
            vlan_port->egress_vlan ||
            vlan_port->egress_inner_vlan ||
            vlan_port->policer_id ||
            vlan_port->egress_policer_id ||
            (vlan_port->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
            (vlan_port->outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) ||
            vlan_port->if_class || vlan_port->class_id)

        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, No support for egress_service_tpid, egress_vlan, egress_inner_vlan, policer_id, egress_policer_id, inlif_counting_profile, outlif_counting_profile, if_class, class_id");
        }
    }
    else
    {
        /*
         * Verify that the gport is vlan port:
         */
        if (!BCM_GPORT_IS_VLAN_PORT(vlan_port->vlan_port_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vlan_port_id = 0x%08X is not a VLAN Port!!!\n",
                         vlan_port->vlan_port_id);
        }

        /*
         * Verify that the gport is allocated:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_is_allocated(unit, vlan_port->vlan_port_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Wrong setting. vlan_port_id = 0x%08X is not allocated!!!\n",
                         vlan_port->vlan_port_id);
        }

        /*
         * Don't allow data rerieval for a Default ESEM Default entry with no data
         */
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
        {
            int default_dual_homing_val;

            default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);

            if (BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id) == default_dual_homing_val)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, attemptting to retrieve data for Virtual Egress Default entry with no VLAN-Port data (0x%08X)\n",
                             vlan_port->vlan_port_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_verify(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_GPORT_IS_VLAN_PORT(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, gport = 0x%08X is not a VLAN PORT\n", gport);
    }

    /*
     * Don't delete a reserved ESEM default entry
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Don't allow destroy of a reserved Egress ESEM Default entry
         */
        int ac_profile;
        ac_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport);
        if (ac_profile >= dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, attemptting to destroy an illegal Virtual Egress Default entry (%d)\n", ac_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vlan_port_traverse
 */
static shr_error_e
dnx_vlan_port_traverse_verify(
    int unit,
    bcm_vlan_port_traverse_info_t * additional_info,
    bcm_vlan_port_traverse_cb trav_fn)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(additional_info, _SHR_E_PARAM, "additional_info");
    SHR_NULL_CHECK(trav_fn, _SHR_E_PARAM, "trav_fn");

    /*
     * Only the following flags are supported:
     *  - BCM_VLAN_PORT_CREATE_INGRESS_ONLY
     *  - BCM_VLAN_PORT_CREATE_EGRESS_ONLY
     *  - BCM_VLAN_PORT_VLAN_TRANSLATION
     *  - BCM_VLAN_PORT_NATIVE
     *  - BCM_VLAN_PORT_DEFAULT
     */
    if (additional_info->flags &
        ~(BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION |
          BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_DEFAULT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, only BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08x), BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08x)"
                     ", BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08x), BCM_VLAN_PORT_NATIVE (0x%08x) and BCM_VLAN_PORT_DEFAULT (0x%08x)"
                     " are supported! flags = 0x%08X\n",
                     BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_CREATE_EGRESS_ONLY,
                     BCM_VLAN_PORT_VLAN_TRANSLATION, BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_DEFAULT,
                     additional_info->flags);
    }

    /*
     * Verify that both flags BCM_VLAN_PORT_CREATE_INGRESS_ONLY BCM_VLAN_PORT_CREATE_EGRESS_ONLY are not set
     */
    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) &&
        _SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, Both BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08x) and BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08x) are set! flags = 0x%08X\n",
                     BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, additional_info->flags);
    }

    /*
     * Verify BCM_VLAN_PORT_VLAN_TRANSLATION flag
     *  - ingress native
     *  - egress ESEM and ESEM-default
     */
    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
    {
        if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
        {
            if (!_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_NATIVE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error, BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08x) is supported for BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08x) "
                             "only with BCM_VLAN_PORT_NATIVE (0x%08x)!, flags = 0x%08X\n",
                             BCM_VLAN_PORT_VLAN_TRANSLATION, BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_NATIVE,
                             additional_info->flags);
            }
        }
        else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
        {
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08x) must be set with BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08x) or BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08x)! flags = 0x%08X\n",
                         BCM_VLAN_PORT_VLAN_TRANSLATION, BCM_VLAN_PORT_CREATE_INGRESS_ONLY,
                         BCM_VLAN_PORT_CREATE_EGRESS_ONLY, additional_info->flags);

        }
    }

    /*
     * Verify BCM_VLAN_PORT_NATIVE flag (only ingress supported)
     */
    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_NATIVE) &&
        !_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, BCM_VLAN_PORT_NATIVE (0x%08x) is supported only with BCM_VLAN_PORT_CREATE_INGRESS_ONLY (0x%08x)! flags = 0x%08X\n",
                     BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_CREATE_INGRESS_ONLY, additional_info->flags);
    }

    /*
     * Verify BCM_VLAN_PORT_DEFAULT flag (egress + virtual must be set)
     */
    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_DEFAULT))
    {
        if (!_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ||
            !_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, BCM_VLAN_PORT_DEFAULT (0x%08x) is supported only with BCM_VLAN_PORT_CREATE_EGRESS_ONLY (0x%08x)"
                         " and BCM_VLAN_PORT_VLAN_TRANSLATION (0x%08x)! flags = 0x%08X\n",
                         BCM_VLAN_PORT_DEFAULT, BCM_VLAN_PORT_CREATE_EGRESS_ONLY, BCM_VLAN_PORT_VLAN_TRANSLATION,
                         additional_info->flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_default(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_1, match_inner_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_1, match_inner_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_value_ethertype_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    bcm_port_ethertype_t match_ethertype)
{
    SHR_FUNC_INIT_VARS(unit);
    if (match_ethertype == ETHERTYPE_INITIALIZATION)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                     DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION);
        SHR_EXIT();
    }
    switch (match_ethertype)
    {
        case ETHERTYPE_MPLS:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
            break;
        case ETHERTYPE_IPV4:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
            break;
        case ETHERTYPE_IPV6:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
            break;
        case ETHERTYPE_ARP:
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", match_ethertype);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_value_ethertype_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int inst_id,
    bcm_port_ethertype_t * match_ethertype)
{
    uint32 layer_type = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, inst_id, &layer_type));
    switch (layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
            *match_ethertype = ETHERTYPE_MPLS;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
            *match_ethertype = ETHERTYPE_IPV4;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
            *match_ethertype = ETHERTYPE_IPV6;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_ARP:
            *match_ethertype = ETHERTYPE_ARP;
            break;
        case DBAL_ENUM_FVAL_LAYER_TYPES_INITIALIZATION:
            *match_ethertype = ETHERTYPE_INITIALIZATION;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", layer_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
static shr_error_e
dnx_vlan_port_dbal_key_ethertype_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    bcm_port_ethertype_t match_ethertype)
{
    SHR_FUNC_INIT_VARS(unit);
    if (match_ethertype != 0)
    {
        switch (match_ethertype)
        {
            case ETHERTYPE_MPLS:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS);
                break;
            case ETHERTYPE_IPV4:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4);
                break;
            case ETHERTYPE_IPV6:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6);
                break;
            case ETHERTYPE_ARP:
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_ENUM_FVAL_LAYER_TYPES_ARP);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported InAC match ethertype - 0x%x\n", match_ethertype);
                break;
        }
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, 0x0, 0x0);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    int core = DBAL_CORE_ALL;
    dbal_entry_action_flags_e flags = DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));

    if (!is_local_in_lif_update)
    {
            /** Create TCAM access id */
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, DBAL_TABLE_IN_AC_TCAM_DB, 3, &entry_access_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);

    if (is_local_in_lif_update)
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        flags = DBAL_COMMIT_UPDATE;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_c_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_c_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_c_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_untagged(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure untagged lookup for ingress native
 *          The result is an ingress native ac.
 *          Currently this match can't be configured directly
 *          from the VLAN-Port API, but can be added by a
 *          Port-Match add API that uses the general VLAN-Port
 *          match setting function.
 *   \param [in] unit - unit id
 *   \param [in] port - The gport that is looked up.
 *   \param [in] local_in_lif - The pointed local In-LIF ID.
 *   \param [in] is_local_in_lif_update - Indication
 *          to only update the local In-LIF for an existing entry.
 *   \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_vlan_port_match_inlif_set
 */
static shr_error_e
dnx_vlan_port_create_ingress_native_match_port(
    int unit,
    bcm_gport_t port,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_0_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PORT x VLAN in ingress native configuration.
 *          ISEM lookup (LIF x VID), result is ingress native ac
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 * Parameters:
 *              - bcm_gport_t port (in) - the L2 VPN LIF
 *              - bcm_vlan_t match_vlan (in) - the matched VID
 *              - bcm_gport_t local_in_lif (in) - the ingress native ac LIF
 *              - int is_local_in_lif_update (in) - Indication
 *                to only update the local In-LIF for an
 *                existing entry.
 */
static shr_error_e
dnx_vlan_port_create_ingress_native_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * set the vlan to match.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PORT x VLAN in ingress native configuration.
 *          ISEM lookup (LIF x VID), result is ingress native ac
 *
 * Called by:
 *              - bcm_dnx_vlan_port_create
 * Parameters:
 *              - bcm_gport_t port (in) - the L2 VPN LIF
 *              - bcm_vlan_t match_vlan (in) - the matched VID
 *              - bcm_vlan_t match_inner_vlan (in) - the inner matched VID
 *              - bcm_gport_t local_in_lif (in) - the ingress native ac LIF
 *              - int is_local_in_lif_update (in) - Indication
 *                to only update the local In-LIF for an
 *                existing entry.
 */
static shr_error_e
dnx_vlan_port_create_ingress_native_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif,
    int is_local_in_lif_update)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;
    dbal_entry_action_flags_e dbal_flags = (is_local_in_lif_update) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * set the vlans to match.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    /*
     * Set the core id in the key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * Set the new local in-LIF to be used
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_unmatched(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_UNMATCHED_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_untagged(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_UNTAGGED_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_S_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_vlan);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_svlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_S_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_C_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_2, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_1, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See header file bcm_int/dnx/vlan/vlan.h for description.
 */
shr_error_e
dnx_vlan_port_create_ingress_match_port_tunnel_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_gport_t local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_S_S_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_2, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_1, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_in_lif);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate qos AC's nwk_qos_idx
 */
static shr_error_e
dnx_vlan_port_create_egress_qos_index(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int *nwk_qos_idx,
    int old_nwk_qos_idx,
    uint8 is_update)
{
    uint16 new_nwk_qos = 0;
    int new_qos_index = 0;
    uint8 is_first = 0, is_last = 0;

    SHR_FUNC_INIT_VARS(unit);

    new_nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(vlan_port->pkt_pri, vlan_port->pkt_cfi);

    if (is_update)
    {
        if (nwk_qos_idx != NULL)
        {
            new_qos_index = old_nwk_qos_idx;
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_update
                            (unit, new_nwk_qos, new_nwk_qos, 0, &new_qos_index, &is_first, &is_last));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free(unit, old_nwk_qos_idx, &is_last));
        }
    }
    else if (nwk_qos_idx != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate(unit,
                                                                         new_nwk_qos, new_nwk_qos, 0,
                                                                         &new_qos_index, &is_first));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error ! nwk_qos_idx is NULL");
    }

    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set(unit, new_qos_index, new_nwk_qos, new_nwk_qos, 0));
    }
    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, old_nwk_qos_idx));
    }

    if (nwk_qos_idx != NULL)
    {
        *nwk_qos_idx = new_qos_index;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure one entry of VLAN PORT egress AC using ESEM
 *  Mapping (Namespace + system port) to egress vlan.
 *  resultType is updated with the value written to the HW (to be saved at SW DB)
 *
 *  Called by dnx_vlan_port_create_egress_match_esem_namespace_port
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem_namespace_port_add(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_egress_ac_table_info_t * egress_ac_table_info,
    int is_replace,
    uint32 spa,
    uint32 *resultType)
{
    uint32 entry_handle_id;
    int nwk_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** zero nwk_qos_idx in case it is invalid */
    nwk_qos_idx = (egress_ac_table_info->nwk_qos_idx == DNX_QOS_INVALID_MAP_ID) ? 0 : egress_ac_table_info->nwk_qos_idx;

    /*
     * Take table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_port->match_class_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, spa);
    /*
     * Set values:
     */
    *resultType = DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_AC_STAT;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *resultType);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                 egress_ac_table_info->qos_model);
    /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                 !_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                 egress_ac_table_info->out_lif_profile);
    /** Don't set these field as default(0) when the operation is replacement */
    if (!is_replace)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }

    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure VLAN PORT egress AC using ESEM
 *  Mapping (Namespace + system port) to egress vlan.
 * resultType is updated with the value written to the HW (to be save at SW DB)
 *
 * Called by dnx_vlan_port_create_egress_match_esem
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem_namespace_port(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_egress_ac_table_info_t * egress_ac_table_info,
    int is_replace,
    uint32 *resultType)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((BCM_GPORT_IS_TRUNK(vlan_port->port)) &&
        dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_key_with_sspa_contains_member_id))
    {
        
        int member;
        int pool;
        int max_members = 0;
        uint32 spa;

        /*
         * Get max members
         */
        BCM_TRUNK_ID_POOL_GET(pool, BCM_GPORT_TRUNK_GET(vlan_port->port));
        max_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;

        /*
         * Add entry for each member.
         */
        for (member = 0; member < max_members; member++)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, vlan_port->port, member, &spa));
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_namespace_port_add
                            (unit, vlan_port, egress_ac_table_info, is_replace, spa, resultType));
        }
    }
    else
    {
        dnx_algo_gpm_gport_phy_info_t gport_phy_info;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_namespace_port_add
                        (unit, vlan_port, egress_ac_table_info, is_replace, gport_phy_info.sys_port, resultType));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check if the given gport resource is in LIF scope or Network scope.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] gport_hw_resources - gport hardware resource.
 * \param [out] vlan_domain - pointer to name space the gport belongs to.
 * \param [out] is_intf_namespace - If the gport requires a LIF scope native AC.
 *
 * \return
 *   \retval Negative if error was detected. See \ref bcm_error_t
 */
static shr_error_e
dnx_vlan_port_create_egress_native_ac_interface_namespace_check(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources,
    uint32 *vlan_domain,
    uint32 *is_intf_namespace)
{
    uint32 entry_handle_id;
    uint32 esem_cmd;
    dnx_esem_cmd_data_t esem_cmd_data;
    dbal_table_field_info_t field_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get esem-name-space(vlan_domain) and esem-command
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get vlan domain if required. */
    if (vlan_domain)
    {
        rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources.outlif_dbal_table_id,
                                               DBAL_FIELD_ESEM_NAME_SPACE, 0,
                                               gport_hw_resources.outlif_dbal_result_type, 0, &field_info);
        if (rv == _SHR_E_NOT_FOUND)
        {
            /** In case of EEDB_SRV6, etc., no such field*/
            *vlan_domain = 0;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ESEM_NAME_SPACE, INST_SINGLE, vlan_domain));
        }
    }

    /** Get esem command if existing. */
    rv = dbal_tables_field_info_get_no_err(unit, gport_hw_resources.outlif_dbal_table_id, DBAL_FIELD_ESEM_NAME_SPACE,
                                           0, gport_hw_resources.outlif_dbal_result_type, 0, &field_info);
    if (rv == _SHR_E_NOT_FOUND)
    {
        /** In case of EEDB_SRV6, etc., no such field*/
        *is_intf_namespace = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ESEM_COMMAND, INST_SINGLE, &esem_cmd));

        /** Get esem command data: */
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, esem_cmd, &esem_cmd_data, NULL));

        /** If any of the esem lookup key is {fodo, outlif} for native AC, we think it as LIF scope.*/
        *is_intf_namespace =
            (((esem_cmd_data.esem[ESEM_ACCESS_IF_1].valid == TRUE) &&
              (esem_cmd_data.esem[ESEM_ACCESS_IF_1].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_OUTLIF)) ||
             ((esem_cmd_data.esem[ESEM_ACCESS_IF_2].valid == TRUE) &&
              (esem_cmd_data.esem[ESEM_ACCESS_IF_2].app_db_id == DBAL_ENUM_FVAL_ESEM_APP_DB_ID_FODO_OUTLIF)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure VLAN PORT egress AC using ESEM
 *      Mapping (VLAN domain + VSI) or (local-out-lif + VSI) or
 *      (Namespace + VSI) or (Namespace + system port) to egress vlan.
 * resultType is updated with the value written to the HW (to be save at SW DB)
 *
 * Called by bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_egress_ac_table_info_t * egress_ac_table_info,
    int is_replace,
    uint32 *resultType)
{
    uint32 entry_handle_id;
    uint32 old_result_type, vid_1, vid_2, action_profile, vlan_edit_profile, remark_profile, stat_obj_id, stat_obj_cmd,
        egress_last_layer;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vid_2 = 0;
    vid_1 = 0;
    action_profile = 0;
    vlan_edit_profile = 0;
    remark_profile = 0;
    stat_obj_id = 0;
    stat_obj_cmd = 0;
    egress_last_layer = 0;
    old_result_type = 0;

    /*
     * Write to EGRESS ESEM OUT AC table (Clean AC entry)
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            uint32 vlan_domain;
            uint32 is_native_intf_name_space = FALSE;
            uint32 is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
            /*
             * ESEM AC - native or non-native:
             */
            if (is_native)
            {
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                /*
                 * Handle ESEM AC Native:
                 */
                /** Get the PWE/Tunnel local Out-LIF using DNX Algo Gport Management */
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);

                /** Check if it is LIF scope or NWK scope.*/
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_native_ac_interface_namespace_check
                                (unit, gport_hw_resources, &vlan_domain, &is_native_intf_name_space));
                if (is_native_intf_name_space == TRUE)
                {
                    /*
                     * ETPS_AC or ETPS_AC_STAT entry
                     */
                    int is_large_entry =
                        BCM_DNX_ESEM_IS_LARGE_ENTRY_NEEDED(vlan_port->flags, egress_ac_table_info->nwk_qos_idx);

                    /** Take table handle */
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB,
                                     &entry_handle_id));
                    /** On REPLACE operation and in case the result type changed, need to update all fields.,Thus, get all fields "old" values. */
                    if (is_replace)
                    {
                        /** Set keys: */
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF,
                                                   gport_hw_resources.local_out_lif);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &old_result_type));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, &vid_1));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                         &action_profile));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                         &vlan_edit_profile));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                         &remark_profile));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                         &egress_last_layer));
                        if (old_result_type ==
                            DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC_STAT)
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                            (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, &vid_2));
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                             &stat_obj_id));
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                             &stat_obj_cmd));
                        }
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                                        (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB,
                                         entry_handle_id));
                    }
                    /** Set keys: */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF,
                                               gport_hw_resources.local_out_lif);

                    /** Set values: */
                    if (is_large_entry)
                    {
                        /** zero nwk_qos_idx in case it is invalid */
                        int nwk_qos_idx = (egress_ac_table_info->nwk_qos_idx == DNX_QOS_INVALID_MAP_ID) ?
                            0 : egress_ac_table_info->nwk_qos_idx;

                        *resultType =
                            DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC_STAT;
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                     *resultType);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                                     nwk_qos_idx);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                                                     vid_2);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                     stat_obj_id);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                                     stat_obj_cmd);
                    }
                    else
                    {
                        *resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC;
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                     *resultType);
                    }

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                 egress_ac_table_info->qos_model);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                                 egress_ac_table_info->out_lif_profile);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, vid_1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                                 action_profile);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                                 vlan_edit_profile);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                                 remark_profile);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                                 egress_last_layer);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
            }

            if (!is_native || !is_native_intf_name_space)
            {
                /*
                 * ETPS_AC or ETPS_AC_STAT entry
                 */
                int is_large_entry =
                    BCM_DNX_ESEM_IS_LARGE_ENTRY_NEEDED(vlan_port->flags, egress_ac_table_info->nwk_qos_idx);

                /*
                 * Handle ESEM AC non-native or native in NWK scope
                 */

                /** Take table handleV*/
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
                /** On REPLACE operation and in case the result type changed, need to update all fields.,Thus, get all fields "old" values. */
                if (is_replace)
                {
                    /** Set keys: */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, vlan_port->match_vlan);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &old_result_type));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, &vid_1));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, &action_profile));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                     &vlan_edit_profile));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &remark_profile));
                    if (old_result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, &vid_2));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_obj_id));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                         &stat_obj_cmd));
                    }
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, entry_handle_id));
                }

                /** Set keys: */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, vlan_port->match_vlan);

                /** Set values: */
                if (is_large_entry)
                {
                    /** zero nwk_qos_idx in case it is invalid */
                    int nwk_qos_idx = (egress_ac_table_info->nwk_qos_idx == DNX_QOS_INVALID_MAP_ID) ?
                        0 : egress_ac_table_info->nwk_qos_idx;

                    *resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 *resultType);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE,
                                                 nwk_qos_idx);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, vid_2);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                 stat_obj_id);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                                 stat_obj_cmd);
                }
                else
                {
                    *resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 *resultType);
                }

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, vid_1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE,
                                             action_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                             vlan_edit_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                             remark_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                             egress_ac_table_info->qos_model);

                /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                             (!is_native));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             egress_ac_table_info->out_lif_profile);
            }

            if (!is_replace)
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            shr_error_e retVal;
            /*
             * Take table handle
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_port->match_class_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vlan_port->vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
            /*
             * Check if the entry exist:
             * Note: the ESEM Namespace-vsi entry may be created by bcm_vxlan_network_domain_config_add,
             * thus need to check if already exist.
             */
            retVal = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (retVal == _SHR_E_NOT_FOUND)
            {
                /*
                 * The entry does not exist!
                 *   - Create it with result type set to default (ETPS_L2_FODO).
                 *   - Update relevant fields (vni=0 and out_lif_profile).
                 */
                /*
                 * Set values:
                 */
                *resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, *resultType);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             egress_ac_table_info->out_lif_profile);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_DOMAIN_NWK_NAME, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else if (retVal == _SHR_E_NONE)
            {
                /*
                 * The entry exist!
                 *   - Verify it's result type is the default (ETPS_L2_FODO) -
                 *      This is true only if it is created by bcm_vxlan_network_domain_config_add.
                 *      In case of REPLACE, the entry was already created and might be set by bcm_vlan_port_translation_set.
                 *   - Update relevant fields (out_lif_porfile).
                 */
                uint32 hw_resultType;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &hw_resultType));
                if (!is_replace)
                {
                    if (hw_resultType != DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Error!, accessing ESEM table (%d) with the keys VLAN_DOMAIN = %d, VSI = %d, C_VID = %d but"
                                     " resultType =%d (supposed to be %d)\n",
                                     DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port->match_class_id,
                                     vlan_port->vsi, 0, hw_resultType,
                                     DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO);
                    }
                }

                /*
                 * Set values:
                 */
                *resultType = hw_resultType;

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                             egress_ac_table_info->out_lif_profile);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                /*
                 * Return the error code!
                 */
                SHR_ERR_EXIT(retVal,
                             "Error!, accessing ESEM table (%d) with the keys VLAN_DOMAIN (namespace) = %d, VSI = %d, C_VID = %d failed!!!\n",
                             DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port->match_class_id, vlan_port->vsi, 0);
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {

            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_namespace_port
                            (unit, vlan_port, egress_ac_table_info, is_replace, resultType));

            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add Esem match-info to SW state.
 * Called by bcm_dnx_vlan_port_create
 * Parameters: bcm_vlan_port_t (in)
 */
static shr_error_e
dnx_vlan_port_create_egress_match_esem_sw_state(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 resultType)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            /*
             * Handle ESEM Default:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_DEFAULT);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);
            break;
        }

        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */
            uint32 is_stat_egress = 0;
            uint32 is_native_intf_name_space = 0;
            uint32 is_two_vlans = 0;
            uint32 vlan_domain = 0;

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE))
            {
                is_stat_egress = 1;
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS))
            {
                is_two_vlans = 1;
            }

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                /*
                 * Handle ESEM AC Native:
                 */
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                /** Get the PWE/Tunnel local Out-LIF using DNX Algo Gport Management */
                SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                            (unit, vlan_port->port,
                                             DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                            _SHR_E_NOT_FOUND, _SHR_E_PARAM);

                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_native_ac_interface_namespace_check
                                (unit, gport_hw_resources, &vlan_domain, &is_native_intf_name_space));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                             gport_hw_resources.local_out_lif);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_EGRESS_ENABLE_FLAG_SET, INST_SINGLE,
                                             is_stat_egress);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TWO_VLANS_FLAG_SET, INST_SINGLE,
                                             is_two_vlans);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE,
                                             is_native_intf_name_space);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
            }
            else
            {
                uint32 vlan_domain;
                /*
                 * Handle ESEM AC non-native
                 */

                SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_EGRESS_ENABLE_FLAG_SET, INST_SINGLE,
                                             is_stat_egress);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TWO_VLANS_FLAG_SET, INST_SINGLE,
                                             is_two_vlans);

            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vlan_port->vsi);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE,
                                         vlan_port->match_class_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE,
                                         vlan_port->match_class_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure VLAN PORT egress EEDB Mapping OutLif to
 *        egress AC information
 *
 * Called by bcm_dnx_vlan_port_create
 * \param [in] unit - the relevant unit.
 * \param [in] entry_handle_id -EEDB table handler ID.
 * \param [in] is_native_ac: is the EEDB entry native.
 * \param [in] flags: vlan_port flags.
 * \param [in] egress_ac_table_info: pointer to struct containing:
 *       [in] dbal_table_id: the id of the table.
 *       [in] result_type: result type of the entry.
 *       [in] outlif_profile: outlif profile associated with
 *       this entry.
 *       [in] next_outlif: next EEDB entry, pointed from this
 *       entry.
 */
static shr_error_e
dnx_vlan_port_create_egress_match_eedb(
    int unit,
    uint32 entry_handle_id,
    int is_native_ac,
    uint32 flags,
    dnx_egress_ac_table_info_t * egress_ac_table_info)
{
    int nwk_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** zero nwk_qos_idx in case it is invalid */
    nwk_qos_idx = (egress_ac_table_info->nwk_qos_idx == DNX_QOS_INVALID_MAP_ID) ? 0 : egress_ac_table_info->nwk_qos_idx;

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                 egress_ac_table_info->protection_path);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                 egress_ac_table_info->protection_pointer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                 egress_ac_table_info->qos_model);

    /** Set EGRESS_LAST_LAYER - indicates outer Ethernet layer */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE,
                                 egress_ac_table_info->last_eth_layer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                 egress_ac_table_info->out_lif_profile);

    STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(flags, BCM_VLAN_PORT_STAT_EGRESS_ENABLE);

    /*
     * Set pointer to next EEDB entry
     * - For native - it is the local-out-lif of the gport got from the user (vla_port->tunnel_id)
     */
    if (is_native_ac)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     egress_ac_table_info->next_outlif);
    }

    if (egress_ac_table_info->tunnel_id)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SUB_TYPE_FORMAT, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECID_OR_PON_TAG, INST_SINGLE,
                                     egress_ac_table_info->tunnel_id);
    }

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SW state of gport to forward information
 *       according to vlan_port struct and application type.
 *       Also returns the forward information for further use
 *       Called by bcm_dnx_vlan_port_create
 *
 * \param [in] unit - the relevant unit.
 * \param [in] vlan_port - VLAN-Port information, specifically
 *        the port
 * \param [in] ll_app_type - The Link-Layer In-LIF application
 *        type
 * \param [in] glob_out_lif - Global Out-LIF for forward info
 * \param [out] forward_info - Struct to be filled with forward
 *        information
 */
static shr_error_e
vlan_port_gport_to_forward_information_set(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    uint32 glob_out_lif,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    bcm_gport_t destination;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * SW State should be filled only if ingress is configured because it represents
     * the learning information relevant for this gport
     */
    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Handle FEC and 1+1 protection case.
     * In case of FEC Protection use the FEC as the destination else
     * In case of 1+1 Protection use the MC as the destination.
     * otherwise use destination port as the destination.
     */
    if ((vlan_port->failover_port_id))
    {
        destination = vlan_port->failover_port_id;
    }
    else
    {
        if (vlan_port->failover_mc_group)
        {
            _SHR_GPORT_MCAST_SET(destination, _SHR_MULTICAST_ID_GET(vlan_port->failover_mc_group));
        }
        else
        {
            destination = vlan_port->port;
        }
    }

    /*
     * Protection will be added
     */
    if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
    {
        /*
         * Forwarding is done according to destination and Outlif
         */
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &(forward_info->destination)));

        if ((vlan_port->group != 0) && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE))
        {
            /*
             * To suppport fast flush at IOP mode, set forwarding type as dest_outlif.
             * In this case, failover_port_id is FEC. 
             */
            forward_info->outlif = glob_out_lif;
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        }
        else if (vlan_port->failover_port_id || vlan_port->failover_mc_group)
        {
            /*
             * In case of FEC and MC Protection Global OutLIF must be 0.
             */
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        }
        else
        {
            forward_info->outlif = glob_out_lif;
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
        }
    }
    else
    {
        /*
         * Forwarding is done according to destination only
         */
        forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, destination, &(forward_info->destination)));
    }

    /*
     * Fill destination (from Gport) info Forward Info table (SW state)
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_add(unit,
                                                       _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE),
                                                       vlan_port->vlan_port_id, forward_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Save gport VLAN-PORT info in sw state dbal table
 *
 * \param [in] unit - the unit number
 * \param [in] vlan_port_id - the vlan port id (gport)
 * \param [in] is_native - is native LIF
 * \param [in] is_virtual - is virtual LIF
 * \param [in] local_inlif - local_inlif Id
 * \param [in] is_replace - is replace
 *
 * \return
 *   statis shr_error_e
 *
 * \remark
 *  This SW table is used for saving gport info in the following types:
 *  - Ingress Native Virtual - save the local_inlif (used by dnx_algo_gpm_gport_to_hw_resources to retrive it)
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_port_ingress_gport_info_save(
    int unit,
    bcm_gport_t vlan_port_id,
    int is_native,
    int is_virtual,
    int local_inlif,
    int is_replace)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For Native Virtual:
     *   save local-inlif (used by dnx_algo_gpm_gport_to_hw_resources to retrive the local_inlif)
     */
    if (is_native && is_virtual)
    {
        uint32 entry_handle_id;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, vlan_port_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, INST_SINGLE, local_inlif);

        if (!is_replace)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This functions calculates the IN-LIF table.
 * \param [in] unit - relevant unit.
 * \param [in] vlan_port - A pointer to a struct containing:
 *        [in] flags - for logical layer 2 port,
 *        BCM_VLAN_PORT_xxx.
 *
 * \param [out] ingress_ac_table_info - pointer to structure containig ingress lif table info:
 *  - dbal_table_id - the associated table id.
 *  - large_entry - whether large entry to be used for non native case.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_ingress_large_entry_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    ingress_ac_table_info->learn_info_type = LIF_LEARN_INFO_REQ_TYPE_NO_REQUIREMENT;

    /*
     * Outer ACs Database, located in VT1
     */
    if (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE)))
    {
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id) || vlan_port->group)
        {
            ingress_ac_table_info->learn_info_type = LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF;
        }

        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
        {
            /*
             * For wide-data with 28 bits, it use small size of learn field.
             * It should be indicated by ingress_ac_table_info.large_learn_field=FALSE
             */
            if (ingress_ac_table_info->learn_info_type != LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF)
            {
                ingress_ac_table_info->learn_info_type = LIF_LEARN_INFO_REQ_TYPE_BASIC_LIF;
            }
        }
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE))
        {
            if (ingress_ac_table_info->learn_info_type != LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF)
            {
                ingress_ac_table_info->learn_info_type = LIF_LEARN_INFO_REQ_TYPE_BASIC_LIF;
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This functions calculates the IN-LIF table.
 * \param [in] unit - relevant unit.
 * \param [in] vlan_port - A pointer to a struct containing:
 *        [in] flags - for logical layer 2 port,
 *        BCM_VLAN_PORT_xxx.
 *
 * \param [out] ingress_ac_table_info - pointer to structure containig ingress lif table info:
 *  - dbal_table_id - the associated table id.
 *  - large_entry - whether large entry to be used for non native case.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_table_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info)
{
    int is_pon_enable = 0;
    int is_pon_dtc_enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Native AC DBs
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        /** Glifless (virtual AC) DB */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
        {
            ingress_ac_table_info->dbal_table_id = DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT;
        }
        /** Gliful (Non virtual) DB */
        else
        {
            ingress_ac_table_info->dbal_table_id = DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION;
        }
    }
    /*
     * Outer ACs Database, located in VT1
     */
    else
    {
        if (DNX_CRITERIA_IS_PON(vlan_port->criteria))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, vlan_port->port, &is_pon_enable));
        }

        if (is_pon_enable)
        {
            SHR_IF_ERR_EXIT(bcm_vlan_control_port_get
                            (unit, vlan_port->port, bcmVlanPortDoubleLookupEnable, &is_pon_dtc_enable));
        }
        /*
         * PON_DTC_CLASSIFICATION is accessed by VTT5(INLIF2)
         */
        if (is_pon_dtc_enable)
        {
            ingress_ac_table_info->dbal_table_id = DBAL_TABLE_PON_DTC_CLASSIFICATION;
        }
        else
        {
            /** AC_INFO_DB table commit is done in
             *  dnx_vlan_port_create_ingress_lif_info_set(), called by
             *  the api */
            ingress_ac_table_info->dbal_table_id = DBAL_TABLE_IN_AC_INFO_DB;
        }

    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_large_entry_calc(unit, vlan_port, ingress_ac_table_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -
* This functions calculates the OUT-LIF table id and
* result_type.
* \param [in] unit - relevant unit.
* \param [in] vlan_port - A pointer to a struct containing:
* \param [out] egress_ac_table_info - pointer to structure containig egress lif table info:
*  - dbal_table_id - the associated table id.
*  - outlif_phase -  phase of the entry.
*
* \return
*   shr_error_e
*
* \remark
*   * None
* \see
* bcm_dnx_vlan_port_create
*/
static shr_error_e
dnx_vlan_port_create_egress_non_virtual_lif_table_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_egress_ac_table_info_t * egress_ac_table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    egress_ac_table_info->dbal_table_id = DBAL_TABLE_EEDB_OUT_AC;

    /*
     * Native AC Database
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
    {
        egress_ac_table_info->outlif_phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_AC;
    }
    /*
     * Outer AC Database
     */
    else
    {
        egress_ac_table_info->outlif_phase = LIF_MNGR_OUTLIF_PHASE_AC;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function for deciding whether an AC LIF should have VSI field.
 * See dnx_vlan_port_create_ingress_lif_info_set.
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_vsi_exist_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int is_p2p,
    int *has_vsi)
{
    int is_native, is_virtual;

    SHR_FUNC_INIT_VARS(unit);

    is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);

    if (is_native == FALSE)
    {
        
        if (is_p2p == FALSE)
        {
            *has_vsi = TRUE;
        }
        else
        {
            *has_vsi = FALSE;
        }
    }
    else
    {
        /*
         * Ingress Native AC:
         *   - Non virtual: has VSI.
         *   - Virtual: with/without VSI (based on VSI value - valid value or not)
         */
        if (!is_virtual)
        {
            *has_vsi = TRUE;
        }
        else
        {
            if (vlan_port->vsi == 0)
            {
                *has_vsi = FALSE;
            }
            else
            {
                *has_vsi = TRUE;
            }
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_create_learn_info_set(
    int unit,
    uint32 entry_handle_id,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    int is_replace,
    int is_native)
{
    uint32 full_dbal_struct_data = 0;
    dbal_enum_value_field_learn_payload_context_e learn_context;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The Learn enable is set during standard creation, but during update the previous
     * value remains.
     */
    if (is_replace == FALSE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE,
                                     ingress_ac_table_info->learn_enable);
    }

    learn_context = ingress_ac_table_info->learn_context;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE, learn_context);

    /*
     *  Set the learn context and info for an Outer-AC
     */
    if (is_native == FALSE)
    {
        switch (learn_context)
        {
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC:
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC:
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, DBAL_FIELD_DESTINATION,
                                 &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, INST_SINGLE,
                                             full_dbal_struct_data);
                break;
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED:
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED:
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                                 &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                                 &ingress_ac_table_info->flush_group, &full_dbal_struct_data));
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, INST_SINGLE,
                                             full_dbal_struct_data);
                break;
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED:
                /*
                 * No real Learn payload, but the correct learn info field
                 * should be selected
                 */
                if (ingress_ac_table_info->learn_info_type == LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id,
                                                 DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, INST_SINGLE, 0);
                }
                else if (ingress_ac_table_info->learn_info_type == LIF_LEARN_INFO_REQ_TYPE_BASIC_LIF)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id,
                                                 DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC, INST_SINGLE, 0);
                }
                else
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_INFO, INST_SINGLE, 0);
                }
                break;
            case DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED:
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_INFO, INST_SINGLE, 0);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported AC Learn Context - %d\n",
                             ingress_ac_table_info->learn_context);
        }
    }

    /*
     *  Set the learn context and info for a Native-AC.
     *  The only supported native result type IN_ETH_AC_MP, uses the DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED
     *  context
     */
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_DESTINATION,
                         &(ingress_ac_table_info->learn_info), &full_dbal_struct_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                         &ingress_ac_table_info->flush_group, &full_dbal_struct_data));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
                                     INST_SINGLE, full_dbal_struct_data);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function allocates IN-LIF and updates the IN-LIF Info table with
 * the following info:
 *  - result type.
 *  - Global IN-LIF.
 *  - vsi.
 *  - fodo_assignment_mode.
 *  - Learn information
 *
 * \param [in] unit - the relevant unit.
 * \param [in] entry_handle_id -table handler ID.
 * \param [in] vlan_port -Pointer to a structure contains input
 *      configuration parameters of the VLAN PORT.
 * \param [in] ingress_ac_table_info - pointer to a struct
 *        containing:
 *       [in] dbal_table_id - ID of the associated table.
 *       [in] global_lif - global IN-LIF ID.
 *       [in] result_type - Result type of the associated entry.
 *       [in] vsi - the VSI. [in] fodo_assignment_mode - the
 *       forwarding domain assignment mode.
 *       [in] large_learn_field - whether large learn field used at non native case.
 * \param [in] ingress_ac_fields_info - pointer to a struct containing fields value.
 * \param [in] is_replace - is replace.
 * \param [out] is_p2p - is the LIF P2P.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_create_ingress_lif_info_set(
    int unit,
    uint32 entry_handle_id,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info,
    int is_replace,
    int *is_p2p)
{
    int is_native, has_vsi, is_native_virtual, is_virtual, has_global_lif, has_learning, has_protection;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    is_native_virtual = is_native && is_virtual;

    *is_p2p = FALSE;

    if (is_replace == TRUE)
    {
        *is_p2p = ingress_ac_fields_info->is_p2p;
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_vsi_exist_calc(unit, vlan_port, *is_p2p, &has_vsi));

    has_global_lif = (!is_native) || (is_native && !is_native_virtual);

    has_protection = DNX_FAILOVER_IS_PROTECTION_ENABLED(ingress_ac_table_info->protection_pointer);

    has_learning = (!(*is_p2p)) && (!is_native_virtual) && (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        && (!(_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE) && has_protection));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** AC only 1 bit for propagation profile, native ac use uniform, outer ac use pipe*/
    if (is_native)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                     DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM);
    }
    else
    {
        /*
         * Control LIF: Uniform or Short-pipe.
         * Ingress AC: Pipe.
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            if (vlan_port->ingress_qos_model.ingress_phb == bcmQosIngressModelUniform)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                             DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                             DNX_QOS_INGRESS_PROPAGATION_PROFILE_SHORT_PIPE);
            }
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE,
                                         DNX_QOS_INGRESS_PROPAGATION_PROFILE_PIPE);
        }
    }

    /*
     * QOS Ingress ECN model
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
    {
        uint32 ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT;

        if (vlan_port->ingress_qos_model.ingress_ecn == bcmQosIngressEcnModelEligible)
        {
            ecn_mapping_profile = DNX_QOS_ECN_MAPPING_PROFILE_ELIGIBLE;
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE,
                                     ecn_mapping_profile);
    }

    if (has_vsi)
    {
        /*
         * FIXM SDK-180912 - JR2 - LIF table manager and FODO field
         * Control LIF has FODO field and not VSI, need to change to:
         *   if BCM_VLAN_PORT_RECYCLE
         *       set FODO
         *   else
         *        set VSI
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, ingress_ac_table_info->vsi);

        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     ingress_ac_table_info->fodo_assignment_mode);
    }

    if (has_global_lif)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE,
                                     ingress_ac_table_info->in_lif_profile);

        if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE,
                                         ingress_ac_table_info->global_lif);
        }
    }

    /*
     * Set the Learn information for applicable In-LIF types
     */
    if (has_learning)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_learn_info_set(unit,
                                                            entry_handle_id, ingress_ac_table_info, is_replace,
                                                            is_native));
    }

    /*
     * setting ingress protection information for appropriate result types
     */
    if (has_protection)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE,
                                     ingress_ac_table_info->protection_pointer);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                                     ingress_ac_table_info->protection_path);
    }

    /*
     * Set back the P2P information if it is REPLACE of P2P
     */
    if (*is_p2p == TRUE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                     ingress_ac_fields_info->service_type);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     ingress_ac_fields_info->destination);

        if (ingress_ac_fields_info->is_glob_out_lif_exist == TRUE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                         ingress_ac_fields_info->glob_out_lif_or_eei_value);
        }
        else if (ingress_ac_fields_info->is_eei_exist == TRUE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE,
                                         ingress_ac_fields_info->glob_out_lif_or_eei_value);
        }
    }

    /*
     * setting ingress default pcp_dei_extend_profile with explicit map profile 0
     */
    if (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
    {
        uint32 vlan_extend_pcp_dei_profile, pcp_dei_profile, pcp_dei_map_extend;

        vlan_extend_pcp_dei_profile = 0;
        pcp_dei_profile = 0;
        pcp_dei_map_extend = DBAL_ENUM_FVAL_PCP_DEI_MAP_TYPE_EXPLICIT;
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_QOS_PROFILE, &pcp_dei_profile,
                         &vlan_extend_pcp_dei_profile));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, DBAL_FIELD_PCP_DEI_MAP_EXTEND, &pcp_dei_map_extend,
                         &vlan_extend_pcp_dei_profile));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE,
                                     vlan_extend_pcp_dei_profile);
    }

    /*
     * Set stat fields (STAT_OBJECT_ID,  STAT_OBJECT_CMD) to default value (0), if indicated needed by user
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_STAT_INGRESS_ENABLE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, 0);
    }

    /*
     * Set user's data field (LIF_GENERIC_DATA_0)  to default value (0), if indicated needed by user
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_INGRESS_WIDE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, INST_SINGLE, 0);
        /** Set LIF_GENERIC_DATA_1 if no protection to support 28bits, otherwise 8bits */
        if (!has_protection)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_1, INST_SINGLE, 0);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Create a virtual gport (Non-lif) for egress AC.
 *  The virtual gport is encoded with index for ESEM default result table,
 *  or a handle to egress vitual gport table according to criteria.
 *
 * \param [in] unit - the relevant unit.
 * \param [in,out] vlan_port - Vlan port information.
 *                 vlan_port_id is updated with gport encoded with virtual subtype.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Here create the virtual gport only. The related hw configuration is done
 *  in other functions.
 *
 * \see
 *  dnx_vlan_port_egress_port_default_create
 *  dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_id_egress_virtual_gport_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int esem_handle;
    uint32 sw_handle_alloc_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Create virtual gport based on flags:
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
    {
        /** Virtual gport is for ESEM-default match entries */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            /*
             * SW handle can be allocated WITH_ID
             */
            sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
            esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
        }

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single(unit,
                                                                                    sw_handle_alloc_flags,
                                                                                    NULL, &esem_handle));
        /** Encode the esem_handle as vitual gport with port-default subtype.*/
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(vlan_port->vlan_port_id, esem_handle);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }
    else
    {
        /** Virtual gport is for ESEM match entries */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
        {
            /*
             * SW handle can be allocated WITH_ID
             */
            sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
            esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
        }
        SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.allocate_single(unit,
                                                                                  sw_handle_alloc_flags,
                                                                                  NULL, &esem_handle));
        /** Encode the esem_handle as virtual gport with match subtype. */
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(vlan_port->vlan_port_id, esem_handle);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Configure ESEM_DEFAULT_RESULT_TABLE with vlan_port information.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] vlan_port - Vlan port information.
 * \param [in] out_lif_profile - out-lif-profile according to vlan_port.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The index is retrieved from vlan_port_id which is encoded as a virtual gport.
 *  The virtual gport is created by bcm_vlan_port_create with flag of
 *  BCM_VLAN_PORT_VLAN_TRANSLATION and criteria of BCM_VLAN_PORT_MATCH_NONE.
 *
 * \see
 *  dnx_vlan_port_id_egress_virtual_gport_create
 */
static shr_error_e
dnx_vlan_port_egress_port_default_create(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int out_lif_profile)
{
    uint32 entry_handle_id;
    int esem_default_result_profile;
    int egress_last_layer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Retrieve esem_default_result_profile from vlan_port_id*/
    esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /** Check esem_default_result_profile is valid*/
    if ((esem_default_result_profile < 0)
        || (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "vlan_port_id (0x%08X) is an illegal virtual gport for esem default entry!\n",
                     vlan_port->vlan_port_id);
    }

    /** Determine if it is for out-ac*/
    egress_last_layer = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? FALSE : TRUE;
    /** Configure ESEM default table*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                               esem_default_result_profile);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Creates a virtual native-ac entry in the inlif table that can be used to edit inner-eth vlan tags.
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 *
 * \return shr_error_e Standard error handling
 */
static shr_error_e
dnx_vlan_port_ingress_virtual_native_ac_gport_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int sw_handle;
    uint32 sw_handle_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate a SW handle and generate gport
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        /*
         * SW handle can be allocated WITH_ID
         */
        sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
    }
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.allocate_single(unit,
                                                                               sw_handle_alloc_flags,
                                                                               NULL, &sw_handle));
    /*
     * Encode GPort subtype and type
     */
    BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_SET(vlan_port->vlan_port_id, sw_handle);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);

exit:
    SHR_FUNC_EXIT;
}

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

static shr_error_e
dnx_vlan_port_create_ingress_fodo_mode_calc(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e ll_app_type,
    int *fodo_assignment_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    *fodo_assignment_mode = FODO_ASSIGNMENT_MODE_INVALID;

    /*
     * AC Optimization LIF - Fodo is an In-LIF Base value plus a VID value
     */
    if (ll_app_type == OPTIMIZATION_AC_APPLICATION_TYPE)
    {
        *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN;
    }
    else
    {
        /*
         * AC Control LIF -
         *      Fodo from lookup.
         * AC Service LIF / AC VLAN Translation -
         *      Fodo from the In-LIF.
         */
        if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
        {
            *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP;
        }
        else
        {
            *fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF;
        }
    }

    if (*fodo_assignment_mode == FODO_ASSIGNMENT_MODE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "dnx_vlan_port_create_ingress_fodo_calc: Error - something went wrong!!! FODO mode was not set! flags = 0x%08X, vsi = %d, criteria = %d\n",
                     vlan_port->flags, vlan_port->vsi, vlan_port->criteria);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get LIF's inlif-profile
* \see
*  bcm_dnx_vlan_port_destroy
*/
static shr_error_e
dnx_ingress_inlif_profile_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 *in_lif_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    /*
     * Get all fields:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get in_lif_profile:
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_ingress_inlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int native_indexed_mode,
    int in_lif_profile,
    int *new_in_lif_profile,
    dbal_tables_e inlif_dbal_table_id)
{
    in_lif_profile_info_t in_lif_profile_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get in_lif_porfile data:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
    /*
     * Update in_lif_porfile with the new incoming_orientation value:
     */
    in_lif_profile_info.egress_fields.in_lif_orientation = network_group_id;

    /*
     * Update in_lif_porfile with the native index mode:
     */
    in_lif_profile_info.native_indexed_mode = native_indexed_mode;

    /*
     * Exchange in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_exchange
                    (unit, &in_lif_profile_info, in_lif_profile, new_in_lif_profile, LIF, inlif_dbal_table_id));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Deallocs inlif-profile.
* \see
*  bcm_dnx_vlan_port_destroy
*/
shr_error_e
dnx_ingress_inlif_profile_dealloc(
    int unit,
    uint32 in_lif_profile)
{
    int new_in_lif_profile;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Free in_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_in_lif_profile_dealloc(unit, in_lif_profile, &new_in_lif_profile, LIF));
exit:
    SHR_FUNC_EXIT;
}

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
static shr_error_e
dnx_vlan_port_create_ingress_lif_match_info_set(
    int unit,
    int local_in_lif,
    int old_in_lif,
    int is_replace,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port)
{
    uint32 entry_handle_id;
    int is_native_ac;
    int is_pon_enable = 0;
    int is_pon_dtc_enable = 0;
    int is_dpc_in_lif = 0;
    uint32 result_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);

    if (DNX_CRITERIA_IS_PON(vlan_port->criteria))
    {
        SHR_IF_ERR_EXIT(dnx_port_sit_pon_enable_get(unit, vlan_port->port, &is_pon_enable));
    }

    if (is_pon_enable)
    {
        SHR_IF_ERR_EXIT(bcm_vlan_control_port_get
                        (unit, vlan_port->port, bcmVlanPortDoubleLookupEnable, &is_pon_dtc_enable));
    }
    is_dpc_in_lif = is_native_ac || is_pon_dtc_enable;
    /*
     * Take table handle and set core key, according to native indication.
     */
    if (is_dpc_in_lif)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    /*
     * Set inlif key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);

    if (!is_replace)
    {
        /*
         * Set values:
         */
        switch (vlan_port->criteria)
        {
            case BCM_VLAN_PORT_MATCH_NONE:
            {
                result_type = is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT:
            {
                result_type = is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                /*
                 * Ethertype is not supported for Ingress Native.
                 */
                if (!is_native_ac)
                {
                    dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                           vlan_port->match_ethertype);
                }

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                /*
                 * Ethertype is not supported for Ingress Native.
                 */
                if (!is_native_ac)
                {
                    dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                           vlan_port->match_ethertype);
                }

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_SVLAN_SVLAN);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN_CVLAN);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, vlan_port->match_pcp);
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                result_type =
                    is_dpc_in_lif ?
                    DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, vlan_port->match_pcp);
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
                break;
            }

            case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_UNTAGGED);

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dnx_vlan_port_dbal_value_ethertype_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE,
                                                       vlan_port->match_ethertype);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
            {
                result_type =
                    is_dpc_in_lif ?
                    DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN
                    : DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
            {
                result_type =
                    is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN
                    : DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
            {
                result_type =
                    is_dpc_in_lif ?
                    DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED :
                    DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_CVLAN);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN_CVLAN);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_SVLAN);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE,
                                             vlan_port->match_tunnel_value);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_vlan);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE,
                                             vlan_port->match_inner_vlan);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "dnx_vlan_port_create_ingress_lif_match_info_set: Error!! criteria = %d is unknown!",
                             vlan_port->criteria);
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Handle REPLACE -
         * LIF table manager also handling the match-info reallocation
         * (see dnx_lif_table_mngr_update_termination_match_information)
         * Only port may be changed during REPLACE (only if it's VLAN-DOMAIN is the same!).
         * Thus, check if port was changed and update the SW DB if needed.
         */
        if (vlan_port->port != old_vlan_port->port)
        {
            /*
             * Set port:
             */
            switch (vlan_port->criteria)
            {
                case BCM_VLAN_PORT_MATCH_NONE:
                {
                    result_type =
                        is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT:
                {
                    result_type =
                        is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_VLAN:
                {
                    result_type =
                        is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
                {
                    result_type =
                        is_dpc_in_lif ? DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_SVLAN_SVLAN);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN_CVLAN);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }

                case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_UNTAGGED);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
                {
                    result_type =
                        is_dpc_in_lif ?
                        DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED :
                        DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 result_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_CVLAN);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN_CVLAN);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_SVLAN);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, vlan_port->port);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "dnx_vlan_port_create_ingress_lif_match_info_set: REPLACE: Error!! criteria = %d is unknown!",
                                 vlan_port->criteria);
                }
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
dnx_vlan_port_create_ingress_lif_match_info_get(
    int unit,
    dbal_tables_e inlif_dbal_table_id,
    int local_in_lif,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 result_type, tmp32bit;
    int is_native_ac;
    int is_pon_dtc_ac;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle and set core key, according to native indication.
     * The In-LIF can be virtual native or non-virtual native -
     */
    is_native_ac =
        ((inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ||
         (inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)) ? TRUE : FALSE;
    is_pon_dtc_ac = (inlif_dbal_table_id == DBAL_TABLE_PON_DTC_CLASSIFICATION) ? TRUE : FALSE;

    if (is_native_ac || is_pon_dtc_ac)
    {
        /*
         * Set table:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        /*
         * Set keys:
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
        /*
         * Get values:
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        switch (result_type)
        {
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN_SVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "dnx_vlan_port_create_ingress_lif_match_info_get: Error!! result_type = %d is unknown!",
                             result_type);
            }
        }
    }
    else
    {

        /*
         * Set table:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        /*
         * Set inlif key:
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);
        /*
         * Get values:
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        switch (result_type)
        {
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_NONE:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_VLAN_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_SVLAN_SVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_CVLAN_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_INNER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_inner_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_PCP, INST_SINGLE, &tmp32bit));
                vlan_port->match_pcp = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
                break;
            }

            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_UNTAGGED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;

                SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_value_ethertype_get
                                (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, INST_SINGLE,
                                 &(vlan_port->match_ethertype)));

                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;

                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNMATCHED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_UNTAGGED:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_SVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
                break;
            }
            case DBAL_RESULT_TYPE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW_MATCH_AC_VLAN_PORT_PORT_TUNNEL_CVLAN:
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &tmp32bit));
                vlan_port->port = (bcm_gport_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_TUNNEL_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_tunnel_value = (bcm_tunnel_id_t) tmp32bit;
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &tmp32bit));
                vlan_port->match_vlan = (bcm_vlan_t) tmp32bit;
                vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "dnx_vlan_port_create_ingress_lif_match_info_get: Error!! result_type = %d is unknown!",
                             result_type);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears the entry in the SW state IN-LIF Info table (DBAL_TABLE_LOCAL_IN_LIF_MATCH_INFO_SW).
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_lif_match_info_clear(
    int unit,
    int is_native_ac,
    int local_in_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle and set core key, according to native indication.
     */
    if (is_native_ac)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    /*
     * Set inlif key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, local_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_egress_outlif_profile_alloc(
    int unit,
    bcm_switch_network_group_t network_group_id,
    int out_lif_profile,
    int *new_out_lif_profile,
    dbal_tables_e dbal_table_id,
    uint32 flags)
{
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get out_lif_profile data:
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    /*
     * Update out_lif_porfile with the new outgoing_orientation value:
     */
    out_lif_profile_info.out_lif_orientation = network_group_id;
    /*
     * Exchange out_lif_porfile:
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, out_lif_profile, new_out_lif_profile, &is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Frees AC outlif-profile.
* \see
*  bcm_dnx_vlan_port_destroy
*/
shr_error_e
dnx_egress_ac_outlif_profile_free(
    int unit,
    dbal_tables_e dbal_table_id,
    int local_out_lif,
    uint32 result_type,
    bcm_vlan_t vsi,
    uint32 vlan_domain,
    bcm_vlan_t c_vid,
    uint32 esem_default_result_profile,
    uint32 system_port)
{
    uint32 entry_handle_id;
    uint32 out_lif_profile;
    int new_out_lif_profile;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    switch (dbal_table_id)
    {
        case DBAL_TABLE_EEDB_OUT_AC:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            break;
        }

        case DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB,
                             &entry_handle_id));

            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
            break;
        }

        case DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);
            break;
        }

        case DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                                       esem_default_result_profile);
            break;
        }
        case DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB:
        {
            /*
             * Take table handle:
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
            /*
             * Set keys:
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error! table_id = %d is wrong!\n", dbal_table_id);
        }

    }

    /*
     * Get out_lif_profile:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));
    /*
     * Free out_lif_porfile:
     * It is done by calling exchange function with default profile data:
     */
    etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                    (unit, dbal_table_id, 0, &out_lif_profile_info, out_lif_profile, &new_out_lif_profile, &is_last));

    if ((dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB)
        && (out_lif_profile >= DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE))
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT_ORIENTATION_PROFILE, new_out_lif_profile);
        }
    }
    else
    {
        if (new_out_lif_profile != DNX_OUT_LIF_PROFILE_DEFAULT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error! Something is wrong! on freeing oulif_profile, didn't get default out lif profile, default outlif profile = %d, new_out_lif_profile = %d",
                         DNX_OUT_LIF_PROFILE_DEFAULT, new_out_lif_profile);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function determening the application of the
 * specified Link-Layer LIF
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains the configuration information that is required to
 *   determine the application type.
 * \param [out] ll_app_type - The type of Link-Layer AC
 *   application that is determined by this function.
 * \return shr_error_e Standard error handeling
 */
static shr_error_e
dnx_vlan_port_set_ll_app_type(
    int unit,
    bcm_vlan_port_t * vlan_port,
    ac_application_type_e * ll_app_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vlan_port, _SHR_E_PARAM, "vlan_port");

    *ll_app_type = INVALID_AC_APPLICATION_TYPE;

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VSI_BASE_VID))
    {
        /*
         * AC Optimization LIF
         */
        *ll_app_type = OPTIMIZATION_AC_APPLICATION_TYPE;
    }
    else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION))
    {
        *ll_app_type = VLAN_TRANSLATION_AC_APPLICATION_TYPE;
    }
    else
    {
        /*
         * For symmetric - the application type is SERVICE_AC
         * For ingress/egress-only - the application type is VLAN_TRANSLATION_AC
         */
        if ((!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY)) &&
            (!_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY)))
        {
            /*
             * AC Service LIF
             */
            *ll_app_type = SERVICE_AC_APPLICATION_TYPE;
        }
        else
        {
            /*
             * AC VLAN Translation LIF
             */
            *ll_app_type = VLAN_TRANSLATION_AC_APPLICATION_TYPE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * A function for storing the required AC learning info
 * within an In-LIF structure
 * \param [in] unit - Unit #
 * \param [in] forward_info - Pointer to the forward information
 *        that also serves as a learn information.
 * \param [in] ll_app_type - The type of Link-Layer AC
 *        application. Required in oredr to determine the learn
 *        info values.
 * \param [in] vlan_port - Pointer to the VLAN-Port structure
 *        that holds the user's destination information
 * \param [out] ingress_ac_table_info - Pointer to an internal
 *        Ingress info structure that also holds the learn info.
 * \return shr_error_e Standard error handeling
 */
static shr_error_e
dnx_vlan_port_learn_info_fill(
    int unit,
    dnx_algo_gpm_forward_info_t * forward_info,
    ac_application_type_e ll_app_type,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info)
{
    int is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? TRUE : FALSE;
    int is_optimized_allowed = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the learn enable to TRUE for VLAN-Translation ACs and for asymmetrical Optimization ACs.
     * For Service ACs the learn is only temporarily disabled as it becomes enabled when the user associates the
     * VLAN-Port with a VSI using bcm_vswitch_port_add().
     */
    ingress_ac_table_info->learn_enable = FALSE;

    /*
     * Fill learn information for an Outer-AC
     */
    if (is_native == FALSE)
    {
        dbal_fields_e dbal_dest_type;
        uint32 dest_val;

        /*
         * Retrieve the destination type in order to use it as part of the learn context selection
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, forward_info->destination, &dbal_dest_type, &dest_val));

        /*
         * Allow Optimization learn context only if the Destination is of a port type
         * and only if the port remains unchanged.
         */
        if (dbal_dest_type != DBAL_FIELD_PORT_ID)
        {
            is_optimized_allowed = FALSE;
        }
        else
        {
            dnx_algo_gpm_gport_phy_info_t phy_port_info;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &phy_port_info));

            if (phy_port_info.sys_port != dest_val)
            {
                is_optimized_allowed = FALSE;
            }
        }

        /*
         * Set a default learn context that is suitable for Service AC cases
         * or as a default for other future cases.
         * Prioritize Extended over Optimized as there's no learn context for both.
         */
        if (ingress_ac_table_info->learn_info_type == LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF)
        {
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED;
        }
        else if (is_optimized_allowed
                 && (ingress_ac_table_info->learn_info_type == LIF_LEARN_INFO_REQ_TYPE_NO_REQUIREMENT))
        {
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED;
        }
        else
        {
            ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC;
        }

        /*
         * Cross-Connect ACs may require learning when the opposite side is MP. The learn information is not from
         * the In-LIF - It is taken from the Src-System-Port and Global Out-LIF
         * This option isn't supported by the API for now.
         */

        /*
         * Set the learn context according to the AC application, the learn information and the In-LIF
         * learn info type requirements.
         * Prioritize Extended over Optimized as there's no learn context for both.
         */
        if (!(ll_app_type == SERVICE_AC_APPLICATION_TYPE))
        {
            ingress_ac_table_info->learn_enable = TRUE;

            if (ingress_ac_table_info->learn_info_type == LIF_LEARN_INFO_REQ_TYPE_EXTENDED_LIF)
            {
                ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
            }
            else if (is_optimized_allowed)
            {
                ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED;
            }
            else
            {
                ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC;
            }
        }
        else
        {
            /*
             * In case of a service, there are protection cases when only the destination should be learnt -
             * modify the learn context to such an appropriate value.                                                                                -
             */
            if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
            {
                if (ingress_ac_table_info->learn_context == DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_EXTENDED)
                {
                    ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
                }
                if (ingress_ac_table_info->learn_context ==
                    DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED)
                {
                    ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED;
                }
                if (ingress_ac_table_info->learn_context == DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_BASIC)
                {
                    ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_BASIC;
                }
            }
        }

        /*
         * Set the learn information to the Forward information destination for all MP cases (even when not relevant)
         * Entry group setting isn't supported.
         */
        ingress_ac_table_info->learn_info = forward_info->destination;
    }
    else
    {
        /*
         * Set the learn information to the Forward information destination for all MP cases (even when not relevant)
         * The strength is always 0, entry group setting isn't supported.
         */
        ingress_ac_table_info->learn_context = DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_EXTENDED;
        ingress_ac_table_info->learn_info = 0;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function for creating ingress non-virtual LIF.
 * It handles FODO calculation, inlif allocation, forwarding information and learning information update.
 */
static shr_error_e
dnx_vlan_port_create_ingress_non_virtual(
    int unit,
    int global_lif,
    ac_application_type_e ll_app_type,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    int is_replace,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info,
    bcm_vlan_port_t * vlan_port)
{
    int is_native, is_virtual_ac;
    int indexed_mode_enable = 0, native_indexed_mode = 0;
    dnx_algo_gpm_forward_info_t forward_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    is_native = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? TRUE : FALSE;
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION) ? TRUE : FALSE;

    ingress_ac_table_info->vsi = vlan_port->vsi;

    /*
     * Set native index mode only when
     *  - inlif is native and non virtual
     *  - indexed mode is allocated in in_lif_profile
     */
    indexed_mode_enable = dnx_data_lif.in_lif.in_lif_profile_allocate_indexed_mode_get(unit);
    if ((indexed_mode_enable == IN_LIF_PROFILE_PWE_INDEXED_MODE_NOF_VALUES) && is_native && (!is_virtual_ac))
    {
        native_indexed_mode = 1;
    }

    /*
     * Calculate IN-LIF info:
     *  - fodo - Forwarding Domain Assignment Mode
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                    (unit, vlan_port, ll_app_type, &(ingress_ac_table_info->fodo_assignment_mode)));

    /*
     * Allocate IN-LIF profile:
     * Since we are in vlan port create function, call the function with default inlif profile (no need to read it from inlif table):
     */
    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_alloc
                        (unit, vlan_port->ingress_network_group_id, native_indexed_mode, DNX_IN_LIF_PROFILE_DEFAULT,
                         &(ingress_ac_table_info->in_lif_profile), ingress_ac_table_info->dbal_table_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_alloc
                        (unit, vlan_port->ingress_network_group_id, native_indexed_mode,
                         (int) ingress_ac_fields_info->in_lif_profile,
                         &(ingress_ac_table_info->in_lif_profile), ingress_ac_table_info->dbal_table_id));
    }

    if (!is_native)
    {
        /*
         * Store the forwarding information for the gport in the dedicated SW DB
         * and use it for filling the learn info
         */
        SHR_IF_ERR_EXIT(vlan_port_gport_to_forward_information_set
                        (unit, vlan_port, ll_app_type, global_lif, &forward_info));
    }

    /*
     * Store learning information according to the Application type
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_learn_info_fill(unit, &forward_info, ll_app_type, vlan_port, ingress_ac_table_info));

    /*
     * Set the global outlif field only for symmetrical LIFs to use for learning,
     * same interface filtering etc
     */
    if (ll_app_type == SERVICE_AC_APPLICATION_TYPE)
    {
        ingress_ac_table_info->global_lif = global_lif;
    }

    /*
     * flush group information
     */
    if (vlan_port->group)
    {
        ingress_ac_table_info->flush_group = vlan_port->group;
    }

exit:

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function for creating ingress LIF.
 * It handles local-inlif allocation and HW update, SW DB update and ISEM update.
 */
static shr_error_e
dnx_vlan_port_create_ingress(
    int unit,
    int global_lif,
    dnx_ingress_ac_table_info_t * ingress_ac_table_info,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info,
    int is_replace,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;
    int local_inlif;
    int is_native_ac;
    int is_virtual_ac;
    int is_p2p;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);

    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    /*
     * Note:
     * In case of ingress-only LIF, the GLOB_IN_LIF field in the LIF HW table is set to 0, thus
     * cannot use ingress_ac_table_info.global_lif (because it is 0). Use global_lif which is
     * updated on the allocation (see calling dnx_algo_global_lif_allocation_allocate above).
     */
    lif_info.global_lif = global_lif;

    if (global_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    if (is_replace)
    {
        lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
        local_inlif = ingress_ac_table_info->old_local_inlif;
    }

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ingress_ac_table_info->dbal_table_id, &entry_handle_id));

    /*
     * Set IN-LIF info:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_info_set
                    (unit, entry_handle_id, vlan_port, ingress_ac_table_info, ingress_ac_fields_info,
                     is_replace, &is_p2p));

    /** For P2P REPLACE of non-native - need to set table-specific-flags */
    if ((is_p2p == TRUE) && (!is_native_ac))
    {
        lif_info.table_specific_flags = DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P;
    }

    /*
     * Allocate local-inlif and write its info to HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

    /*
     * Update Ingress SW state match info:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_set
                    (unit, local_inlif, ingress_ac_table_info->old_local_inlif, is_replace, vlan_port, old_vlan_port));

    /*
     * Save the local-inlif is SW DB (if necessary).
     * Used by bcm_vlan_port_find API.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_gport_info_save(unit, vlan_port->vlan_port_id, is_native_ac,
                                                          is_virtual_ac, local_inlif, is_replace));
    /*
     * For created In-LIFs add lookup entries according to the criteria
     */
    if (!is_replace)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_match_inlif_set(unit, vlan_port, local_inlif, FALSE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Internal function for creating egress non-virtual LIF.
 * It updates next_outlif and last_eth_layer at egress_ac_table_info.
 */
static shr_error_e
dnx_vlan_port_create_egress_non_virtual(
    int unit,
    bcm_gport_t tunnel_id,
    int is_native_ac,
    dnx_egress_ac_table_info_t * egress_ac_table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Native AC non virtual table.
     */
    if (is_native_ac)
    {
        /*
         * In case the tunnel LIF was not found (the gport is valid but either it was deleted or not yet allocated,
         * don't return an error, just set the next pointer to NULL.
         */
        egress_ac_table_info->next_outlif = 0;

        /*
         * Get local out lifs and pointer to the next
         *  tunnel in the EEDB linked list.
         */
        if ((tunnel_id != BCM_GPORT_INVALID) && BCM_GPORT_IS_SET(tunnel_id))
        {
            dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
            uint32 lif_flags;

            lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_id, lif_flags, &gport_hw_resources));

            if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                egress_ac_table_info->next_outlif = gport_hw_resources.local_out_lif;
            }
        }

        egress_ac_table_info->last_eth_layer = 0;
    }
    else
    {
        egress_ac_table_info->last_eth_layer = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Internal function for creating egress LIF.
 * It handles local-outlif allocation and HW update and SW DB update.
 */
static shr_error_e
dnx_vlan_port_create_egress(
    int unit,
    int global_lif,
    dnx_egress_ac_table_info_t * egress_ac_table_info,
    int is_replace,
    bcm_vlan_port_t * vlan_port,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    int is_virtual_ac, is_native_ac;
    dbal_enum_value_field_encap_qos_model_e qos_model;
    int nwk_qos_idx = DNX_QOS_INVALID_MAP_ID;
    dbal_tables_e outlif_profile_dbal_table;
    uint32 default_outlif_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);

    SHR_IF_ERR_EXIT(dnx_vlan_port_outlif_profile_table_determine(unit, vlan_port, egress_ac_table_info,
                                                                 &outlif_profile_dbal_table, &default_outlif_profile));
    if (!is_replace)
    {
        /*
         * Allocate OUT-LIF profile:
         * Since we are in vlan port create function, call the function with default outlif profile (no need to read it from outlif table):
         */
        SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                        (unit, vlan_port->egress_network_group_id, default_outlif_profile,
                         &(egress_ac_table_info->out_lif_profile), outlif_profile_dbal_table, 0));
        /** egress qos*/
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &vlan_port->egress_qos_model, &qos_model));
        if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_qos_index
                            (unit, vlan_port, &nwk_qos_idx, DNX_QOS_INVALID_MAP_ID, FALSE));
        }
        else if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)
        {
            nwk_qos_idx = DNX_QOS_EXPLICIT_IDX_L2(vlan_port->pkt_pri, vlan_port->pkt_cfi);
        }

        egress_ac_table_info->qos_model = qos_model;
        egress_ac_table_info->nwk_qos_idx = nwk_qos_idx;
    }
    else
    {
        /*
         * Exchange outlif profile:
         */
        uint32 entry_handle_id;
        uint32 old_out_lif_profile;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, outlif_profile_dbal_table, &entry_handle_id));

        SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set
                        (unit, entry_handle_id, vlan_port->vlan_port_id, gport_hw_resources));
        /*
         * Get out_lif_profile:
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &old_out_lif_profile));

        SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                        (unit, vlan_port->egress_network_group_id, (int) old_out_lif_profile,
                         &(egress_ac_table_info->out_lif_profile), outlif_profile_dbal_table, 0));

        /** egress qos*/
        SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &vlan_port->egress_qos_model, &qos_model));
        if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
        {
            if (egress_ac_table_info->qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
            {
                if ((vlan_port->pkt_pri != egress_ac_table_info->pkt_pri) ||
                    (vlan_port->pkt_cfi != egress_ac_table_info->pkt_cfi))
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_qos_index
                                    (unit, vlan_port, &nwk_qos_idx, egress_ac_table_info->nwk_qos_idx, TRUE));
                }
                else
                {
                    nwk_qos_idx = egress_ac_table_info->nwk_qos_idx;
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_qos_index
                                (unit, vlan_port, &nwk_qos_idx, DNX_QOS_INVALID_MAP_ID, FALSE));
            }
        }
        else
        {
            if (egress_ac_table_info->qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_qos_index
                                (unit, vlan_port, NULL, egress_ac_table_info->nwk_qos_idx, TRUE));
            }
            if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)
            {
                nwk_qos_idx = DNX_QOS_EXPLICIT_IDX_L2(vlan_port->pkt_pri, vlan_port->pkt_cfi);
            }
        }
        egress_ac_table_info->qos_model = qos_model;
        egress_ac_table_info->nwk_qos_idx = nwk_qos_idx;
    }

    /*
     * Match criteia can be either PORT (default configuration per port)
     * or other (per VLAN for example)
     */
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            /*
             * BCM_VLAN_PORT_DEFAULT means creating entry in ESEM default entry
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_DEFAULT))
            {
                if (is_virtual_ac == TRUE)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_egress_port_default_create
                                    (unit, vlan_port, egress_ac_table_info->out_lif_profile));

                    /*
                     * Add ESEM match-info to SW DB:
                     * Note: replace cannot change match-info thus call can be skipped.
                     */
                    if (!is_replace)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state
                                        (unit, vlan_port, DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC));
                    }
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * Four configuration modes:
             * 1. Symmetric (InLif + OutLif EEDB)
             * 2. Ingress only (InLif)
             * 3. Egress only (ESEM)
             * 4. Egress only (OutLif)
             */

            /*
             * Handle ESEM (non-native and native):
             */
            if (is_virtual_ac)
            {
                uint32 resultType;

                /*
                 * Configure the ESEM entry:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem
                                (unit, vlan_port, egress_ac_table_info, is_replace, &resultType));

                /*
                 * Add ESEM match-info to SW DB:
                 */
                if (!is_replace)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state(unit, vlan_port, resultType));
                }
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            /*
             * Case 1/2 above: Symmetric/Ingress only
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            uint32 resultType;
            /*
             * Configure the ESEM entry:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem
                            (unit, vlan_port, egress_ac_table_info, is_replace, &resultType));

            /*
             * Add ESEM match-info to SW DB:
             */
            if (!is_replace)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state(unit, vlan_port, resultType));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
        {
            egress_ac_table_info->tunnel_id = vlan_port->egress_tunnel_value;
            break;
        }
        default:
        {
            break;
        }
    }

    if (!is_virtual_ac)
    {
        uint32 entry_handle_id;
        int local_outlif;
        lif_table_mngr_outlif_info_t outlif_info;

        sal_memset(&outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

        if (is_replace)
        {
            outlif_info.flags = LIF_TABLE_MNGR_LIF_INFO_REPLACE;
            local_outlif = egress_ac_table_info->old_local_outlif;
        }

        outlif_info.outlif_phase = egress_ac_table_info->outlif_phase;
        outlif_info.global_lif = global_lif;

        /*
         * Take table handle:
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, egress_ac_table_info->dbal_table_id, &entry_handle_id));

        /*
         * Update out-lif info
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_eedb
                        (unit, entry_handle_id, is_native_ac, vlan_port->flags, egress_ac_table_info));

        /*
         * Allocate local-outlif and write its info to HW
         * Note: glem is update by this function.
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                        (unit, entry_handle_id, &local_outlif, &outlif_info));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_port_create_global_lif_alloc(
    int unit,
    int *global_lif,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress;
    uint32 global_lif_flags;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;

    /*************************************************************************************/
    /****************************** SW Allocations ************************************/
    /*************************************************************************************/

    /*
     * LIF allocation via DNX SW Algorithm for non virtual ACs (native and outer ACs)
     */
    global_lif_flags = LIF_MNGR_L2_GPORT_GLOBAL_LIF;

    /*
     * Note:
     * Flags BCM_VLAN_PORT_WITH_ID and BCM_VLAN_PORT_ENCAP_WITH_ID should indicate same global_lif
     * ID in case both are set, thus can take either one of them!
     * See dnx_vlan_port_create_verify.
     */
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_WITH_ID))
    {
        /*
         * BCM_VLAN_PORT_WITH_ID means:
         * Use the specific LIF ID value from vlan_port->vlan_port_id
         */
        *global_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(vlan_port->vlan_port_id));
        global_lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    else if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ENCAP_WITH_ID))
    {
        /*
         * BCM_VLAN_PORT_ENCAP_WITH_ID means:
         * Use the specific LIF ID value from vlan_port->encap_id
         */
        *global_lif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
        global_lif_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
    }
    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_ALLOC_SYMMETRIC))
    {
        /*
         * BCM_VLAN_PORT_ALLOC_SYMMETRIC means:
         * single side allocation of lif which is free in both sides
         */
        global_lif_flags |= LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF;
    }

    /*
     * Both ingress and egress are allocated
     */
    if (is_ingress && is_egress)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, global_lif_flags, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, global_lif));

        /*
         * Update vlan_port_id with the allocated new global lif.
         * Encode subtype:
         */
        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, 0, *global_lif);
        vlan_port->encap_id = *global_lif;
    }
    /*
     * Only ingress is allocated
     */
    else if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, global_lif_flags, DNX_ALGO_LIF_INGRESS, global_lif));
        /*
         * Update vlan_port_id with the allocated new global lif.
         * Encode subtype:
         */
        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, *global_lif);
        vlan_port->encap_id = 0;
    }
    /*
     * Only egress is allocated
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, global_lif_flags, DNX_ALGO_LIF_EGRESS, global_lif));
        /*
         * Update vlan_port_id with the allocated new global lif.
         * Encode subtype:
         */
        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port->vlan_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, *global_lif);
        vlan_port->encap_id = *global_lif;
    }

    /*
     * Update vlan_port_id with the allocated new global lif.
     * Encode type:
     */
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port->vlan_port_id, vlan_port->vlan_port_id);

exit:
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_vlan_port_replace_ingress_p2p_to_p2p(
    int unit,
    bcm_gport_t gport1,
    uint32 entry_handle_id,
    dnx_algo_gpm_forward_info_t * forward_info2)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_inlif_info_t lif_info;
    uint8 is_field_on_handle;
    int local_inlif;
    int is_native_ac;
    int is_virtual_ac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));

    /*
     * Get gport ingress HW info:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport1,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                     &gport_hw_resources));

    is_native_ac = (gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_IN_AC_INFO_DB);
    is_virtual_ac = (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT);

    /*
     * Unset EEI and global-out-lif
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, &is_field_on_handle));
    if (is_field_on_handle)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE);
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_EEI, &is_field_on_handle));
    if (is_field_on_handle)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE);
    }

    /*
     * Set global-out-lif or eei
     */
    if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                     forward_info2->outlif);
    }
    else if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info2->eei);
    }
    else if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        /** For Destination only - no global-out-lif or eei! */
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                 forward_info2->destination);

    /*
     * "replace" the local-inlif and write its info to HW
     */
    lif_info.global_lif = gport_hw_resources.global_in_lif;

    if (gport_hw_resources.global_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    if (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
    {
        lif_info.table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P;
    }

    local_inlif = gport_hw_resources.local_in_lif;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

    /*
     * Save the local-inlif is SW DB (if necessary).
     * Used by bcm_vlan_port_find API.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_gport_info_save
                    (unit, gport1, is_native_ac, is_virtual_ac, local_inlif, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_vlan_port_replace_ingress_mp_to_p2p(
    int unit,
    bcm_gport_t gport1,
    dnx_algo_gpm_forward_info_t * forward_info2)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_inlif_info_t lif_info;
    uint8 is_field_on_handle;
    int local_inlif;
    int is_native_ac;
    int is_virtual_ac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get gport ingress HW info:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport1,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                     &gport_hw_resources));

    is_native_ac = ((gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_IN_AC_INFO_DB) &&
                    (gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_PON_DTC_CLASSIFICATION));
    is_virtual_ac = (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    /*
     * Set keys and gets all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &lif_info));

    /*
     * Change the LIF to P2P:
     *
     *  1. Unset MP fields that are not exist in P2P:
     *      - FODO_ASSIGNMENT_MODE
     *      - VSI
     *      - learn enable.
     *      - MP learning context and info (LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC or LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, see dnx_vlan_port_learn_info_fill)
     */
    
    if (is_native_ac)
    {
        /** P2P with VSI is not supported. For Native -  set fields values to DEFAULT values */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DNX_VSI_DEFAULT);
    }
    else
    {
        /** P2P with VSI is not supported. For Non-native - unset fields */
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE);
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE);
    }

    if (!is_virtual_ac)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE);

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, &is_field_on_handle));
        if (is_field_on_handle)
        {
            dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE);
        }

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                        (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_INFO, &is_field_on_handle));
        if (is_field_on_handle)
        {
            dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_INFO, INST_SINGLE);
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                            (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC,
                             &is_field_on_handle));
            if (is_field_on_handle)
            {
                dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_BASIC_SYM_LIF_BASIC,
                                             INST_SINGLE);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                                (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
                                 &is_field_on_handle));
                if (is_field_on_handle)
                {
                    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT,
                                                 INST_SINGLE);
                }
            }
        }
    }
    /*
     * 2. Set P2P relevant fields:
     *  - SERVICE_TYPE (to P2P).
     *  - GLOBAL_OUT_LIF / EEI
     *  - DESTINATION
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);

    /*
     * Set global-out-lif or eei
     */
    if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                     forward_info2->outlif);
    }
    else if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info2->eei);
    }
    else if (forward_info2->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        /** For Destination only - no global-out-lif or eei! */
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                 forward_info2->destination);

    /*
     * "replace" the local-inlif and write its info to HW
     */
    lif_info.global_lif = gport_hw_resources.global_in_lif;

    if (gport_hw_resources.global_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    if ((gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB) ||
        (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_PON_DTC_CLASSIFICATION))
    {
        lif_info.table_specific_flags |= DNX_ALGO_LIF_TABLE_SPECIFIC_AC_INGRESS_NON_NATIVE_P2P;
    }

    local_inlif = gport_hw_resources.local_in_lif;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

    /*
     * Save the local-inlif is SW DB (if necessary).
     * Used by bcm_vlan_port_find API.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_gport_info_save
                    (unit, gport1, is_native_ac, is_virtual_ac, local_inlif, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  For an In-LIF transition from P2P to MP, determine the learn
 *  info and set it to the DBAL handler.
 * \param [in] unit - unit id
 * \param [in] gport - The P2P In-LIF gport to be changed to a
 *        MP In-LIF.
 * \param [in] entry_handle_id - The DBAL handle on which the
 *        learn info will be stored.
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 * dnx_vlan_port_replace_ingress_p2p_to_mp
 */
static shr_error_e
dnx_vlan_port_p2p_to_mp_learn_info_set(
    int unit,
    bcm_gport_t gport,
    uint32 entry_handle_id)
{
    bcm_vlan_port_t vlan_port;
    dnx_ingress_ac_table_info_t ingress_ac_table_info = { 0 };
    dnx_algo_gpm_forward_info_t sw_state_forward_info = { 0 };
    ac_application_type_e ll_app_type;
    int is_native;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the VLAN-Port parameters that are required in order recalculate the LIF information. 
     * The required fields are listed below in each step of the calculation. 
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = gport;
    SHR_IF_ERR_EXIT(dnx_vlan_port_find_by_gport(unit, &vlan_port, NULL));
    is_native = (_SHR_IS_FLAG_SET(vlan_port.flags, BCM_VLAN_PORT_NATIVE)) ? TRUE : FALSE;

    /*
     * Determine whether the original MP LIF was a Service LIF
     * Requires: 
     * vlan_port->flags: BCM_VLAN_PORT_VSI_BASE_VID, BCM_VLAN_PORT_VLAN_TRANSLATION, 
     *                   BCM_VLAN_PORT_CREATE_INGRESS_ONLY, BCM_VLAN_PORT_CREATE_EGRESS_ONLY
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, &vlan_port, &ll_app_type));

    /*
     * Recalculate the the LIF-Info requirements based on the expected result-type
     * Requires: 
     * vlan_port->flags: BCM_VLAN_PORT_NATIVE, BCM_VLAN_PORT_INGRESS_WIDE, BCM_VLAN_PORT_STAT_INGRESS_ENABLE
     * vlan_port->ingress_failover_id
     * vlan_port->group
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_large_entry_calc(unit, &vlan_port, &ingress_ac_table_info));

    /*
     * Retrieve the gport's forwarding information from the L2 SW-State
     */
    if (!is_native)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, gport, &sw_state_forward_info));
    }

    /*
     * Calculate the learn information based on all the retrieved info above
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_learn_info_fill
                    (unit, &sw_state_forward_info, ll_app_type, &vlan_port, &ingress_ac_table_info));

    /*
     * Set the calculated learn info to the handler 
     */
    ingress_ac_table_info.learn_enable = FALSE;
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_learn_info_set(unit,
                                                        entry_handle_id, &ingress_ac_table_info, FALSE, is_native));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*  See vlan.h for function description.
*/
shr_error_e
dnx_vlan_port_replace_ingress_p2p_to_mp(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_inlif_info_t lif_info;
    uint8 is_field_on_handle;
    int local_inlif;
    int is_native_ac;
    int is_virtual_ac;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get gport HW info:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                     &gport_hw_resources));

    is_native_ac = (gport_hw_resources.inlif_dbal_table_id != DBAL_TABLE_IN_AC_INFO_DB);
    is_virtual_ac = (gport_hw_resources.inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));

    /*
     * Set keys and gets all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, entry_handle_id, &lif_info));

    /*
     * Change the LIF to MP:
     *
     *  1. Unset P2P fields that are not exist in MP:
     *      - SERVICE_TYPE.
     *      - DESTINATION.
     *      - GLOB_OUT_LIF / EEI.
     *      - Learn context (the field is fixed value while in MP may be 1 or 4 bits)
     */
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE);
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE);

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, &is_field_on_handle));
    if (is_field_on_handle)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE);
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_EEI, &is_field_on_handle));
    if (is_field_on_handle)
    {
        dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE);
    }

    /*
     * 2. Determine the learning parameters and set them 
     *      - LEARN_INFO - According to the expected MP result type using the gport's forwarding info
     *      - LEARN_PAYLOAD_CONTEXT - Recaluculated
     *      - LEARN_ENABLE - Set to Disable
     */
    if (!is_virtual_ac)
    {
        if (_SHR_GPORT_IS_VLAN_PORT(gport))
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_p2p_to_mp_learn_info_set(unit, gport, entry_handle_id));
        }
    }

    /*
     * 3. Set MP relevant fields:
     *      - FODO_ASSIGNMENT_MODE - set to default value - "from LIF"
     *      - VSI - set to default value
     *      - SERVICE_TYPE - set to MP (relevant only for ingress native virtual)
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DNX_VSI_DEFAULT);

    /*
     * "replace" the local-inlif and write its info to HW
     */
    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));
    lif_info.global_lif = gport_hw_resources.global_in_lif;

    if (gport_hw_resources.global_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
    }

    lif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;

    local_inlif = gport_hw_resources.local_in_lif;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, &local_inlif, &lif_info));

    /*
     * Save the local-inlif is SW DB (if necessary).
     * Used by bcm_vlan_port_find API.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_gport_info_save(unit, gport, is_native_ac, is_virtual_ac, local_inlif, TRUE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Replaces a created VLAN PORT.
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains input configuration parameters to be replaced in
 *   the VLAN PORT.
 * \param [in] old_vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains the previous configuration parameters.
 * \param [in] ingress_ac_fields_info -pointer to structure containing relevant fields values
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * Note: parameters that cannot be replaced should be set to the
 * same value as when the VLAN-Port was created.
 * \see
 * bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_replace_set(
    int unit,
    bcm_vlan_port_t * vlan_port,
    bcm_vlan_port_t * old_vlan_port,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info)
{
    int is_ingress, is_egress, is_virtual_ac;
    int protection_pointer;
    dnx_ingress_ac_table_info_t ingress_ac_table_info;
    dnx_egress_ac_table_info_t egress_ac_table_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /*************************************************************************************/
    /****************************** HW configurations ************************************/
    /*************************************************************************************/
    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);

    if (is_egress && is_virtual_ac)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL, &gport_hw_resources));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
    }

    if (is_ingress == TRUE)
    {
        ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;

        sal_memset(&ingress_ac_table_info, 0, sizeof(ingress_ac_table_info));

        ingress_ac_table_info.dbal_table_id = gport_hw_resources.inlif_dbal_table_id;
        ingress_ac_table_info.old_local_inlif = gport_hw_resources.local_in_lif;

        /*
         * Decide whether a large entry is required
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_large_entry_calc(unit, vlan_port, &ingress_ac_table_info));

        /*
         * Calcultate the app_type
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));

        /*
         * Set Ingress Protection information
         */
        if (DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
        {
            ingress_ac_table_info.protection_path = (vlan_port->ingress_failover_port_id) ? 0 : 1;
            DNX_FAILOVER_ID_GET(protection_pointer, vlan_port->ingress_failover_id);
            DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(ingress_ac_table_info.protection_pointer, protection_pointer);
        }

        if (!is_virtual_ac)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_non_virtual
                            (unit, gport_hw_resources.global_in_lif, ll_app_type, &ingress_ac_table_info, TRUE,
                             ingress_ac_fields_info, vlan_port));
        }
        else
        {
            ingress_ac_table_info.vsi = vlan_port->vsi;

            /*
             * Calculate IN-LIF info:
             *  - fodo - Forwarding Domain Assignment Mode
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                            (unit, vlan_port, ll_app_type, &(ingress_ac_table_info.fodo_assignment_mode)));
        }

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress
                        (unit, gport_hw_resources.global_in_lif, &ingress_ac_table_info,
                         ingress_ac_fields_info, TRUE, vlan_port, old_vlan_port));
    }

    if (is_egress == TRUE)
    {
        uint8 nwk_qos;
        dbal_enum_value_field_encap_qos_model_e qos_model;
        int nwk_qos_idx = DNX_QOS_INVALID_MAP_ID;

        SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &old_vlan_port->egress_qos_model, &qos_model));
        if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
        {
            nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(old_vlan_port->pkt_pri, old_vlan_port->pkt_cfi);
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_get(unit, nwk_qos, nwk_qos, 0, &nwk_qos_idx));
        }

        sal_memset(&egress_ac_table_info, 0, sizeof(egress_ac_table_info));

        egress_ac_table_info.dbal_table_id = gport_hw_resources.outlif_dbal_table_id;
        egress_ac_table_info.old_local_outlif = gport_hw_resources.local_out_lif;
        egress_ac_table_info.pkt_pri = old_vlan_port->pkt_pri;
        egress_ac_table_info.pkt_cfi = old_vlan_port->pkt_cfi;
        egress_ac_table_info.qos_model = qos_model;
        egress_ac_table_info.nwk_qos_idx = nwk_qos_idx;

        if (!is_virtual_ac)
        {
            /*
             * Update egress_ac_table_info.outlif_phase
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_non_virtual_lif_table_calc
                            (unit, vlan_port, &egress_ac_table_info));

            /*
             * Update egress_ac_table_info.next_outlif and egress_ac_table_info.last_eth_layer
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_non_virtual
                            (unit, vlan_port->tunnel_id, _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE),
                             &egress_ac_table_info));
            /*
             * Set Egress Protection information
             */
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION))
            {
                egress_ac_table_info.protection_path = (vlan_port->egress_failover_port_id) ? 0 : 1;
                DNX_FAILOVER_ID_GET(egress_ac_table_info.protection_pointer, vlan_port->egress_failover_id);
            }
        }

        SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress
                        (unit, gport_hw_resources.global_out_lif, &egress_ac_table_info, TRUE, vlan_port,
                         &gport_hw_resources));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
  * This API replaces a created VLAN PORT.
 * \param [in] unit - Unit #
 * \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains input configuration parameters to be replaced in
 *   the VLAN PORT.
 * \return
 *   shr_error_e
 *
 * \remark
 * Note: parameters that cannot be replaced should be set to the
 * same value as when the VLAN-Port was created.
 * \see
 * bcm_dnx_vlan_port_create
 */
static shr_error_e
dnx_vlan_port_replace(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    bcm_vlan_port_t old_vlan_port;
    dnx_ingress_ac_fields_table_info_get_t ingress_ac_fields_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify REPLACE case - check the API input parameters:
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_create_replace_verify(unit, vlan_port));

    /*
     * Retrieve previous VLAN-Port information:
     */
    sal_memset(&old_vlan_port, 0, sizeof(bcm_vlan_port_t));
    old_vlan_port.vlan_port_id = vlan_port->vlan_port_id;
    SHR_IF_ERR_EXIT(dnx_vlan_port_find_by_gport(unit, &old_vlan_port, &ingress_ac_fields_info));

    /*
     * Verify that the new VLAN-Port configuration is same as old VLAN-Port configuraion
     */
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_create_replace_new_old_compare_verify(unit, vlan_port, &old_vlan_port));

    SHR_IF_ERR_EXIT(dnx_vlan_port_replace_set(unit, vlan_port, &old_vlan_port, &ingress_ac_fields_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API creates a VLAN PORT.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   Relevant unit.
 *   \param [in] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 *  \b As \b input - \n
 *   vlan_port->flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   BCM_VLAN_PORT_STAT_xxx_ENABLE - marks enabling
 *   statistics. When on, a table entry with statistics will be
 *   chosen.
 *   vlan_port->vlan_port_id
 *   - if vlan_port->flags contain BCM_VLAN_PORT_WITH_ID, then
 *   this variable contain the gport that has to be encoded with
 *   at least a type and a subtype.
 *   vlan_port->port - Gport,local or remote Physical or logical gport.
 *   vlan_port->criteria - logical layer 2 port match criteria.
 *   vlan_port->match_vlan - Outer VLAN ID to match.
 *   vlan_port->match_inner_vlan - Inner VLAN ID to match.
 *   vlan_port->tunnel_id - Pointer from native AC to next entry
 *   in the EEDB list. Only relevant for egress native AC entries
 *   signaled by flag BCM_VLAN_PORT_NATIVE.
 *   vlan_port->egress_qos_model - qos pipe or uniform model
 *  \b As \b output - \n
 *   vlan_port->vlan_port_id - if vlan_port->flags does not
 *   contain BCM_VLAN_PORT_WITH_ID, the procedure will load
 *   this variable with the value of the newly allocated VLAN
 *   PORT ID (Gport encoded with at least a type and subtype).
 *   vlan_port->encap_id - contains the global outlif.
 * \par INDIRECT INPUT:
 *  \b vlan_port - see `vlan_port' in DIRECT_INPUT above.
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 * \b vlan_port - see `vlan_port' in DIRECT_INPUT above.
 * \b DBAL_TABLE_GPORT_TO_FORWARDING_SW_INFO -\n
 *  Fields DBAL_FIELD_DESTINATION and DBAL_FIELD_OUT_LIF are
 * updated.
 * \b DBAL_TABLE_IN_AC_INFO_DB -\n
 * Fields DBAL_FIELD_GLOB_IN_LIF and
 * DBAL_FIELD_VSI are updated.
 * \b DBAL_TABLE_INGRESS_PP_PORT -\n
 * field DBAL_FIELD_DEFAULT_LIF is updated.
 * \b DBAL_TABLE_EGRESS_DEFAULT_AC_PROF -\n
 * fields DBAL_FIELD_ACTION_PROFILE, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_NWK_QOS_IDX, DBAL_FIELD_LAYER_TYPES,
 * DBAL_FIELD_QOS_MODEL, DBAL_FIELD_VLAN_EDIT_PROFILE,
 * DBAL_FIELD_VLAN_EDIT_VID_1 and DBAL_FIELD_VLAN_EDIT_VID_2 are
 * updated.
 * \b DBAL_TABLE_IN_AC_S_C_VLAN_DB -\n
 * field DBAL_FIELD_IN_LIF is updated.
 * \b DBAL_TABLE_EEDB_OUT_AC -\n
 * fields DBAL_FIELD_VLAN_EDIT_VID_1,
 * DBAL_FIELD_VLAN_EDIT_VID_2, DBAL_FIELD_VLAN_EDIT_PROFILE,
 * DBAL_FIELD_ACTION_PROFILE, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_OAM_LIF_SET, DBAL_FIELD_NWK_QOS_IDX,
 * DBAL_FIELD_PROTECTION_PATH, DBAL_FIELD_PROTECTION_POINTER,
 * DBAL_FIELD_STAT_OBJECT_CMD and DBAL_FIELD_QOS_MODEL are
 * updated.
 * \b DBAL_TABLE_IN_AC_S_VLAN_DB -\n
 * field DBAL_FIELD_IN_LIF is updated.
 * \b DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB -\n
 * fields DBAL_FIELD_VLAN_EDIT_VID_1,
 * DBAL_FIELD_VLAN_EDIT_VID_2, DBAL_FIELD_ACTION_PROFILE,
 * DBAL_FIELD_VLAN_EDIT_PROFILE, DBAL_FIELD_NWK_QOS_IDX,
 * DBAL_FIELD_REMARK_PROFILE, DBAL_FIELD_QOS_MODEL,
 * DBAL_FIELD_OAM_LIF_SET, DBAL_FIELD_OUT_LIF_PROFILE,
 * DBAL_FIELD_PROTECTION_PATH, DBAL_FIELD_PROTECTION_POINTER and
 * DBAL_FIELD_STAT_OBJECT_CMD are updated.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_port_create(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress, is_out_lif, is_lif, is_native_ac, is_virtual_ac;
    int global_lif;
    dnx_ingress_ac_table_info_t ingress_ac_table_info;
    dnx_egress_ac_table_info_t egress_ac_table_info;
    ac_application_type_e ll_app_type = NUM_AC_APPLICATION_TYPE_E;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int protection_pointer_encoded[1] = { 0 };
    int protection_pointer[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_replace(unit, vlan_port));
    }
    else
    {

        is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
        is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
        /*
         * Calc once - is the required vlan port a native (inner) AC lif.
         */
        is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
        /*
         * Get an is_virtual indication
         */
        is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
        /*
         * ESEM and Egress-Default-AC configurations allocate a non-LIF entry
         */
        is_out_lif = is_egress && !is_virtual_ac;
        is_lif = (is_ingress && !is_virtual_ac) || is_out_lif;
        /*************************************************************************************/
        /****************************** API parameters verification **************************/
        /*************************************************************************************/
        /*
         * Determine the Link-Layer LIF application type: Service, VLAN-Translation or Optimization
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_set_ll_app_type(unit, vlan_port, &ll_app_type));
        SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_create_verify(unit, vlan_port, ll_app_type, is_virtual_ac));

        /*************************************************************************************/
        /****************************** SW configurations ************************************/
        /*************************************************************************************/

        /*
         * Initialize containers for ingress and egress AC tables (all types, including ESEM)
         */
        sal_memset(&ingress_ac_table_info, 0, sizeof(ingress_ac_table_info));
        ingress_ac_table_info.protection_pointer = dnx_data_failover.path_select.ing_no_protection_get(unit);
        sal_memset(&egress_ac_table_info, 0, sizeof(egress_ac_table_info));
        egress_ac_table_info.protection_pointer = dnx_data_failover.path_select.egr_no_protection_get(unit);

        global_lif = DNX_ALGO_GPM_LIF_INVALID;

        if (is_ingress)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_table_calc(unit, vlan_port, &ingress_ac_table_info));
        }
        if (is_egress && !is_virtual_ac)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_non_virtual_lif_table_calc
                            (unit, vlan_port, &egress_ac_table_info));
        }

        /*************************************************************************************/
        /****************************** SW Allocations ************************************/
        /*************************************************************************************/
        /*
         * LIF allocation via DNX SW Algorithm for non virtual ACs (native and outer ACs)
         */
        if (is_lif)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_global_lif_alloc(unit, &global_lif, vlan_port));

            if (is_ingress == TRUE)
            {

                /*
                 * 1. Calculate IN-LIF info:
                 *  - fodo - Forwarding Domain Assignment Mode
                 * 2. Allocate IN-LIF profile:
                 * Since we are in vlan port create function, call the function with default inlif profile (no need to read it from inlif table)
                 * 3. Set Forward Info and learning according to application type
                 * 4. Store the forwarding information for the gport in the dedicated SW DB
                 * and use it for filling the learn info
                 * 5. Store learning information according to the Application type
                 * 6. Set the global outlif field only for symmetrical LIFs to use for learning,
                 * same interface filtering etc
                 * 7. flush group information
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_non_virtual
                                (unit, global_lif, ll_app_type, &ingress_ac_table_info, FALSE, NULL, vlan_port));

            }

            /*
             * Protection information
             */
            if (is_ingress == TRUE && DNX_FAILOVER_IS_PROTECTION_ENABLED(vlan_port->ingress_failover_id))
            {
                ingress_ac_table_info.protection_path = (vlan_port->ingress_failover_port_id) ? 0 : 1;
                DNX_FAILOVER_ID_GET(protection_pointer[0], vlan_port->ingress_failover_id);
                /** Encode IN-LIF protection pointer format from Failover ID */
                DNX_FAILOVER_ID_PROTECTION_POINTER_ENCODE(protection_pointer_encoded[0], protection_pointer[0]);
                ingress_ac_table_info.protection_pointer = protection_pointer_encoded[0];
            }
            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_EGRESS_PROTECTION))
            {
                egress_ac_table_info.protection_path = (vlan_port->egress_failover_port_id) ? 0 : 1;
                DNX_FAILOVER_ID_GET(egress_ac_table_info.protection_pointer, vlan_port->egress_failover_id);
            }

            if (is_egress == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_non_virtual
                                (unit, vlan_port->tunnel_id, is_native_ac, &egress_ac_table_info));
            }
        }
        else if (is_virtual_ac)
        {
            /** Currently, virtual ac can't be created for both ingress and egress in the same call.*/
            if ((is_ingress == TRUE) && is_native_ac)
            {
                ingress_ac_table_info.vsi = vlan_port->vsi;
                /*
                 * Calculate IN-LIF info:
                 *  - fodo - Forwarding Domain Assignment Mode
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_fodo_mode_calc
                                (unit, vlan_port, ll_app_type, &(ingress_ac_table_info.fodo_assignment_mode)));

                /*
                 * Ingress native AC allocation - allocate gport
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_virtual_native_ac_gport_create(unit, vlan_port));
            }

            if (is_egress == TRUE)
            {
                /** Create a Non-LIF vlan port for ETH EVE */
                SHR_IF_ERR_EXIT(dnx_vlan_port_id_egress_virtual_gport_create(unit, vlan_port));
            }
        }

        /*************************************************************************************/
        /****************************** HW configurations ************************************/
        /*************************************************************************************/
        if (is_ingress == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress
                            (unit, global_lif, &ingress_ac_table_info, NULL, FALSE, vlan_port, NULL));
        }
        if (is_egress == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress
                            (unit, global_lif, &egress_ac_table_info, FALSE, vlan_port, NULL));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, vsi, vlan_domain, c_vid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and port from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &c_vid));
    /*
     * Get port's vlan_domain
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

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
static shr_error_e
dnx_vlan_port_egress_virtual_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 port, vsi, c_vid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI, port and C-VID from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &c_vid));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    vlan_port->vsi = (bcm_vlan_t) vsi;
    vlan_port->match_vlan = (bcm_vlan_t) c_vid;
    vlan_port->port = (bcm_gport_t) port;
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        uint32 vlan_domain;
        /*
         * Get port's vlan_domain
         */
        SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_C_VID, c_vid);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_vsi_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 vsi, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, &vlan_domain));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_vsi_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_t * vsi,
    uint32 *match_class_id)
{
    uint32 tmp32;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &tmp32));
    *vsi = (bcm_vlan_t) tmp32;
    {
        uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

        sal_memset(dbal_field, 0, sizeof(dbal_field));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, dbal_field));
        *match_class_id = dbal_field[0];
    }
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, *match_class_id);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VSI, *vsi);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_C_VID, 0);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - see vlan.h for function description
 */
shr_error_e
dnx_vlan_port_egress_virtual_namespace_port_key_set(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, vlan_domain;
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve system_port and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, gport_phy_info.sys_port);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the ESEM Namespace-port access keys from the given SW table.
 * In addition, it sets the ESEM access keys (if the ESEM table handler is not NULL).
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \param [out] vlan_port -pointer to vlan_port structure to update its port and port match_class_id fields.
 * \return shr_error_e Standard error handeling
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_egress_virtual_namespace_port_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 *entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 port, vlan_domain;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve system_port and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));
    vlan_port->port = (bcm_gport_t) port;
    vlan_port->match_class_id = vlan_domain;

    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    if (entry_handle_id != NULL)
    {
        dnx_algo_gpm_gport_phy_info_t gport_phy_info;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, gport_phy_info.sys_port);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  This function gets the ESEM Native access keys from the given SW table.
 * \see
 *  vlan.h for more information.
 */
shr_error_e
dnx_vlan_port_egress_virtual_native_key_get(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vsi, local_out_lif, vlan_domain;
    uint32 is_native_intf_name_space;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and local_out_lif from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, &local_out_lif));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE, &is_native_intf_name_space));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, &vlan_domain));

    /*
     * Set HW lookup key to native AC table
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    if (is_native_intf_name_space)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);
    }

    /*
     * Convert local_out_lif back to gport:
     * Note: the PWE/tunnel may have been deleted - thus the value may be invalid.
     */
    if (vlan_port != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                         _SHR_CORE_ALL, (int) local_out_lif, &(vlan_port->port)));
        vlan_port->vsi = (bcm_vlan_t) vsi;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See Vlan.h for description
 */
shr_error_e
dnx_vlan_port_egress_virtual_ac_match_info_get(
    int unit,
    bcm_gport_t gport,
    uint32 sw_table_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Access SW DB
     */
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create bridge In-LIF:
 *   - Ingress only.
 *   - VSI = VLAN (by setting VSI = 0 and FODO assignment mode = base + vid)
 *   - criteria = none (at ingress port init, this default in-lif is set to each port)
 */
static shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_create(
    int unit,
    int *created_local_lif)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * Update In-LIF table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Set values:
     * VSI = 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN);

    /*
     * Set default learning
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, TRUE);
    /*
     * Jericho mode beginning
     * JR mode only support Out-LIF + Dest payload
     * {
     */
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                     DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_SYMMETRIC_LIF_OPTIMIZED);

    }
    /*
     * Jericho mode end
     * }
     */
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_PAYLOAD_CONTEXT, INST_SINGLE,
                                     DBAL_ENUM_FVAL_LEARN_PAYLOAD_CONTEXT_NO_LIF_OPTIMIZED);
    }

    /*
     * Zero all other fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_SET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE, 0);

    /*
     * Alloc local in-lif (no global lif allocation)
     */
    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));
    lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, created_local_lif, &lif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Create recycle port in-lif:
 *    - Ingress only
 *    - VSI = result from esem lookup (by setting VSI = 0 and FODO assignment mode = in_lif_id)
 *    - criteria = none (at bcm_port_control_set for recycle port (bcmPortControlOverlayRecycle),
 *                 set the default in-lif for the port.
 */
static shr_error_e
dnx_vlan_port_ingress_initial_recycle_lif_create(
    int unit,
    int *created_local_lif)
{
    uint32 entry_handle_id;
    lif_table_mngr_inlif_info_t lif_info;

    uint8 qos_propagation_prof_first_ref;
    uint8 qos_propagation_prof_last_ref;
    int old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    int propag_profile;
    dnx_qos_propagation_type_e ttl_propagation_type = DNX_QOS_PROPAGATION_TYPE_UNIFORM;
    dnx_qos_propagation_type_e phb_propagation_type = DNX_QOS_PROPAGATION_TYPE_UNIFORM;
    dnx_qos_propagation_type_e remark_propagation_type = DNX_QOS_PROPAGATION_TYPE_UNIFORM;
    dnx_qos_propagation_type_e ecn_propagation_type = DNX_QOS_PROPAGATION_TYPE_SHORT_PIPE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * allocate propagation profile.
     */
    old_propag_profile = DNX_QOS_INGRESS_DEFAULT_PROPAGATION_PROFILE;
    SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_alloc(unit, phb_propagation_type,
                                                              remark_propagation_type, ecn_propagation_type,
                                                              ttl_propagation_type, old_propag_profile,
                                                              &propag_profile, &qos_propagation_prof_first_ref,
                                                              &qos_propagation_prof_last_ref));
    /*
     * Update H/W in case the profile doesn't exist already there
     */
    if (qos_propagation_prof_first_ref == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_propagation_profile_hw_set(unit, propag_profile, phb_propagation_type,
                                                                   remark_propagation_type, ecn_propagation_type,
                                                                   ttl_propagation_type));
    }

    /*
     * Update In-LIF table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, propag_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, 0);

    /*
     * Fill lif table manager info, no global lif
     */
    sal_memset(&lif_info, 0, sizeof(lif_table_mngr_inlif_info_t));
    lif_info.flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
                    (unit, _SHR_CORE_ALL, entry_handle_id, created_local_lif, &lif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_set(
    int unit,
    int initial_bridge_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_BRIDGE_VLAN_PORT_IN_LIF, INST_SINGLE,
                                 initial_bridge_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_bridge_lif_get(
    int unit,
    int *initial_bridge_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_BRIDGE_VLAN_PORT_IN_LIF, INST_SINGLE,
                               (uint32 *) initial_bridge_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create drop In-LIF:
 *   - no global or local lif allocation
 *   - P2P
 *   - destination: ingress trap, trap Id = "ingress trap with action drop"
 */
static shr_error_e
dnx_vlan_port_ingress_initial_drop_lif_create(
    int unit)
{
    uint32 drop_in_lif;
    uint32 entry_handle_id;
    uint32 trap_dest, snp_strength, fwd_strength, trap_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Set Drop In-LIF:
     *   - no global\local lif allocation
     *   - P2P
     *   - destination: ingress trap, trap Id = "ingress trap with action drop"
     */
    drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, drop_in_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE);
    /*
     * Set service-type to P2P
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);

    /** Configure ingress trap with action drop */
    bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, (int *) &trap_id);
    fwd_strength = dnx_data_trap.strength.default_trap_strength_get(unit);
    snp_strength = 0;
    /*
     * Must initialize this variable since it is a uint32 but only 16 bits are used.
     * The procedure dbal_entry_value_field32_set() is not aware of that and uses
     * the full 32 bits.
     */
    trap_dest = 0;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_SNP_STRENGTH, &snp_strength, &trap_dest));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_FWD_STRENGTH, &fwd_strength, &trap_dest));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_INGRESS_TRAP_ID, &trap_id, &trap_dest));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_DEST, INST_SINGLE, trap_dest);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create VTT5 dummy In-LIF:
 *   - no global or local lif allocation
 *   - MP
 */
static shr_error_e
dnx_vlan_port_ingress_initial_vtt5_dummy_lif_create(
    int unit)
{
    uint32 vtt5_dummy_in_lif;
    uint32 entry_handle_id;
    lif_mngr_local_inlif_info_t inlif_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY;
    inlif_info.core_id = _SHR_CORE_ALL;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    vtt5_dummy_in_lif = inlif_info.local_inlif;
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DUMMY_LIF_LOOKUP, &entry_handle_id));
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIF_INDEX, INST_SINGLE, vtt5_dummy_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_DUMMY_MPLS, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, vtt5_dummy_in_lif);
    /*
     * Set values:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_DUMMY_MPLS_IN_LIF_MPLS_DUMMY);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_recycle_port_lif_set(
    int unit,
    int initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_RECYCLE_PORT_VLAN_PORT_IN_LIF,
                                 INST_SINGLE, initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_lif_set(
    int unit,
    int initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_VLAN_PORT_IN_LIF, INST_SINGLE,
                                 initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_initial_default_lif_get(
    int unit,
    int *initial_default_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_VLAN_PORT_IN_LIF, INST_SINGLE,
                               (uint32 *) initial_default_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_ingress_default_recyle_lif_get(
    int unit,
    uint32 *default_recycle_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VLAN_PORT_GENERAL_INFO_SW, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_DEFAULT_RECYCLE_PORT_VLAN_PORT_IN_LIF,
                               INST_SINGLE, default_recycle_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Create VTT5 PPPoE spoofing check In-LIF pair:
 *   - No global allocation
 *   - Allocate two successive In-LIFs
 *   - In-LIF values are: PPPOE_SESSION_SPOOFING_CHECK_MSB(18),Next_Layer_Nextwork_Domain(1),0(1)
 *   - The second last bit of first allocated In-LIF should be 0 to make sure In-LIF pair has same high 18 bits
 *   - Next_Layer_Nextwork_Domain=0,  In-LIF with destinaton BLACK_HOLE to discard packets
 *   - Next_Layer_Nextwork_Domain=1,  In-LIF with nothing
 */
static shr_error_e
dnx_vlan_port_ingress_initial_vtt5_pppoe_spoofing_check_default_lif_create(
    int unit)
{
    uint32 local_inlif_dummy = 0;
    uint32 local_inlif_0 = 0;
    uint32 local_inlif_1 = 0;
    uint32 local_inlif_msb = 0;
    uint32 entry_handle_id;
    uint32 destination;
    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Allocate GLIFless with BCM_GPORT_BLACK_HOLE destination */
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
    inlif_info.core_id = _SHR_CORE_ALL;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));

    /** Given last bit is always 0, and second last bit is correspond to Next_Layer_Nextwork_Domain(1)} */
    if (inlif_info.local_inlif & 0x2)
    {
        local_inlif_dummy = inlif_info.local_inlif;
        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
        inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
        inlif_info.core_id = _SHR_CORE_ALL;
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
        if (inlif_info.local_inlif & 0x2)
        {
            /** Given sencond last bit is 0, or free dummy inLIF and return error */
            if (local_inlif_dummy)
            {
                sal_memset(&inlif_info, 0, sizeof(inlif_info));
                inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
                inlif_info.dbal_result_type =
                    DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
                inlif_info.core_id = _SHR_CORE_ALL;
                inlif_info.local_inlif = local_inlif_dummy;
                dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);
            }
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to allocate GLIFless with a destination for PPPoE 0x%08x/0x%08x.\n",
                         (uint32) local_inlif_dummy, (uint32) local_inlif_0);
        }
        else
        {
            local_inlif_0 = inlif_info.local_inlif;
        }
    }
    else
    {
        local_inlif_0 = inlif_info.local_inlif;
    }

    /** Set BLACK_HOLE to destination */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif_0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION);
    /** Translate the gport to DBAL destination*/
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, BCM_GPORT_BLACK_HOLE, &destination));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Allocate GLIFless with nothing */
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY;
    inlif_info.core_id = _SHR_CORE_ALL;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    if (inlif_info.local_inlif & 0x2)
    {
        local_inlif_1 = inlif_info.local_inlif;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, inlif_info.local_inlif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Given sencond last bit of this second allocated In-LIF is 1, or free allocated In-LIF and return error */
        if (local_inlif_dummy)
        {
            sal_memset(&inlif_info, 0, sizeof(inlif_info));
            inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
            inlif_info.dbal_result_type =
                DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
            inlif_info.core_id = _SHR_CORE_ALL;
            inlif_info.local_inlif = local_inlif_dummy;
            dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);
        }
        /** Free GLIFless with a destination */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif_0);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
        inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
        inlif_info.core_id = _SHR_CORE_ALL;
        inlif_info.local_inlif = local_inlif_0;
        dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);

        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to allocate GLIFless with nothing for PPPoE 0x%08x/0x%08x.\n",
                     (uint32) local_inlif_dummy, (uint32) local_inlif_1);
    }

    /** Done, free dummy inLIF if any */
    if (local_inlif_dummy)
    {
        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
        inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
        inlif_info.core_id = _SHR_CORE_ALL;
        inlif_info.local_inlif = local_inlif_dummy;
        dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);
    }

    /** Check In-LIF pair has same 18 msbs */
    if ((local_inlif_0 >> 2) != (local_inlif_1 >> 2))
    {
        /** Free GLIFless with a destination */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif_0);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
        inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY_WITH_DESTINATION;
        inlif_info.core_id = _SHR_CORE_ALL;
        inlif_info.local_inlif = local_inlif_0;
        dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);

        /** Free GLIFless with nothing */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif_1);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        sal_memset(&inlif_info, 0, sizeof(inlif_info));
        inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK;
        inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_LIF_SPOOFING_CHECK_IN_LIF_DUMMY;
        inlif_info.core_id = _SHR_CORE_ALL;
        inlif_info.local_inlif = local_inlif_1;
        dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &inlif_info, LIF_MNGR_INVALID);

        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to allocate GLIFless pair for PPPoE 0x%08x/0x%08x.\n",
                     (uint32) local_inlif_0, (uint32) local_inlif_1);
    }

    /** Update vrCfg.PPPoE_SESSION_SPOOFING_CHECK_MSB(19) */
    local_inlif_msb = (local_inlif_0 >> 2) & 0x7ffff;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_PPPOE_SESSION_SPOOFING_CHECK_MSB, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF_MSB, INST_SINGLE, local_inlif_msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_default_init(
    int unit)
{
    int initial_vlan_port = 0;
    int initial_recycle_vlan_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create Drop In-LIF:
     *   - no global\local lif allocation
     *   - P2P
     *   - destination: ingress trap, trap Id = "ingress trap with action drop"
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_drop_lif_create(unit));
    /*
     * Create VTT5 dummy In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_vtt5_dummy_lif_create(unit));
    /*
     * Create bridge In-LIF:
     *   - Ingress only.
     *   - VSI = VLAN (by setting VSI = 0 and FODO assignment mode = base + vid)
     *   - criteria = none (at ingress port init, this default in-lif is set to each port)
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_create(unit, &initial_vlan_port));
    /*
     * Save the initial bridge In-LIF gport:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_set(unit, initial_vlan_port));
    /*
     * Choose which LIF is the initial default In-LIF gport:
     * Set it to be drop LIF.
     */
    initial_vlan_port = (int) dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_set(unit, initial_vlan_port));
    /*
     * Create recycle port in-lif:
     *    - Ingress only
     *    - VSI = result from esem lookup (by setting VSI = 0 and FODO assignment mode = in_lif_id)
     *    - criteria = none (at bcm_port_control_set for recycle port (bcmPortControlOverlayRecycle),
     *                 set the default in-lif for the port.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_recycle_lif_create(unit, &initial_recycle_vlan_port));
    /*
     * Save the initial bridge In-LIF gport:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_recycle_port_lif_set(unit, initial_recycle_vlan_port));

    /*
     * Create VTT5 PPPoE spoofing check In-LIF pair:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_vtt5_pppoe_spoofing_check_default_lif_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * See header file for description (vlan.h).
 */
shr_error_e
dnx_vlan_port_default_esem_init(
    int unit)
{
    int esem_handle, out_lif_profile, egress_last_layer;
    uint32 entry_handle_id;
    bcm_switch_network_group_t egress_network_group_id;
    bcm_vlan_port_t esem_default_vlan_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Reserve an AC default ESEM entry
     */
    esem_handle = dnx_data_esem.default_result_profile.default_ac_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);

    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ACTION_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, 0);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE, 0);
    out_lif_profile = DNX_OUT_LIF_PROFILE_DEFAULT;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    egress_last_layer = TRUE;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Add ESEM match-info to SW DB:
     * Note: this is needed since algo_gpm_gport_egress_hw_key_set uses the SW DB to set the keys.
     */
    bcm_vlan_port_t_init(&esem_default_vlan_port);
    esem_default_vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;

    BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(esem_default_vlan_port.vlan_port_id, esem_handle);
    BCM_GPORT_VLAN_PORT_ID_SET(esem_default_vlan_port.vlan_port_id, esem_default_vlan_port.vlan_port_id);

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state
                    (unit, &esem_default_vlan_port, DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC));

    /*
     * Reserve a Native-AC default ESEM entry
     */
    esem_handle = dnx_data_esem.default_result_profile.default_native_get(unit);
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);

    egress_network_group_id = 1;
    SHR_IF_ERR_EXIT(dnx_egress_outlif_profile_alloc
                    (unit, egress_network_group_id, DNX_OUT_LIF_PROFILE_DEFAULT, &out_lif_profile,
                     DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, 0));
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, out_lif_profile);
    egress_last_layer = FALSE;
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, egress_last_layer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Add ESEM match-info to SW DB:
     * Note: this is needed since algo_gpm_gport_egress_hw_key_set uses the SW DB to set the keys.
     */
    BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(esem_default_vlan_port.vlan_port_id, esem_handle);
    BCM_GPORT_VLAN_PORT_ID_SET(esem_default_vlan_port.vlan_port_id, esem_default_vlan_port.vlan_port_id);

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_match_esem_sw_state
                    (unit, &esem_default_vlan_port, DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC));

    /*
     * Reserve a SFLOW default ESEM entry
     */
    esem_handle = dnx_data_esem.default_result_profile.sflow_sample_interface_get(unit);

    /** Configure ESEM default table*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, entry_handle_id));
    /** Set Key */
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_handle);
    /** Set result type */
    
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_DUAL_HOMING);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE,
                                 SFLOW_PACKET_INTERFACE_UNKNOWN);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get the forwarding information from Gport (used for retrieving protection parameters).
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] vlan_port  -  see detail explanation at bcm_vlan_port_create()
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_vlan_port_gport_to_forward_information_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    dnx_algo_gpm_forward_info_t forward_info;
    bcm_gport_t destination;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, vlan_port->vlan_port_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));
    if ((forward_info.fwd_info_result_type !=
         DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        && (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_type (%d) is not supported for get api \r\n",
                     forward_info.fwd_info_result_type);
    }

    /*
     * returning learning information depends on whether the destination is FEC or MC or physical port
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info.destination, &destination));
    if (_SHR_GPORT_IS_FORWARD_PORT(destination))
    {
        vlan_port->failover_port_id = destination;
    }
    else if (_SHR_GPORT_IS_MCAST(destination))
    {
        vlan_port->failover_mc_group = _SHR_GPORT_MCAST_GET(destination);
    }
    /*
     * Note: vlan_port->port is retrieved by SW DB (IN_LIF_MATCH_INFO_SW)
     * See dnx_vlan_port_create_ingress_lif_match_info_get.
     * Thus, no need to update it here.
     else
     {
     vlan_port->port = destination;
     }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function returns the default (local) lif value of the given port
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port(
    int unit,
    bcm_gport_t port,
    int *local_in_lif)
{
    uint32 port_default_lif;
    uint32 initial_drop_in_lif;
    int initial_bridge_in_lif;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Retrieve the port's default In-LIF from the HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_control_port_default_lif_get(unit, port, &port_default_lif));
    /*
     * In case the port.default_lif is one of the initial SDK default LIFs (drop-lif or simple-bridge-lif) -return error "not found"!
     */
    initial_drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, &initial_bridge_in_lif));
    if ((port_default_lif == initial_drop_in_lif) || (port_default_lif == initial_bridge_in_lif))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! port = %d, the port_default_lif = %d is one of SDK initial LIF defaults (drop-lif=%d, bridge-lif=%d)\n",
                     port, port_default_lif, initial_drop_in_lif, initial_bridge_in_lif);
    }

    *local_in_lif = (int) port_default_lif;
exit:
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X c_vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_c_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_c_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_c_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /*
  * This function returns the local-lif value of the port X cvlan X cvlan ISEM entry
  * \see
  * bcm_dnx_vlan_port_find
  */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_cvlan_cvlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_1, match_inner_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* This function returns the local-lif value of the port X svlan X svlan ISEM entry
* \see
* bcm_dnx_vlan_port_find
*/
static shr_error_e
dnx_vlan_port_find_ingress_match_port_svlan_svlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_1, match_inner_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
* This function returns the local-lif value of the port X vlan X vlan ISEM entry
* \see
* bcm_dnx_vlan_port_find
*/
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 tmp32bit;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /*
  * This function returns the local-lif value of the port X vlan X vlan X pcp X ethertype TCAM entry
  * \see
  * bcm_dnx_vlan_port_find
  */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp,
    int *local_in_lif)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    uint32 tmp32bit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the native local-lif value of the port X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_native_match_port_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 tmp32bit;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the native local-lif value of the port X vlan X vlan ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_native_match_port_vlan_vlan(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 tmp32bit;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *local_in_lif = (int) tmp32bit;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - see description at vlan.h
 */
shr_error_e
dnx_vlan_port_find_ingress_native_virtual_match_sw_state(
    int unit,
    int local_in_lif,
    bcm_gport_t * vlan_port_id)
{
    uint32 entry_handle_id;
    uint32 gport;
    int is_end;
    uint32 local_inlif;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /*
     * Set iterator rules:
     * All the fields (local_in_lif) need to equal the given values
     */
    local_inlif = local_in_lif;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &local_inlif, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (is_end == TRUE)
    {
        /*
         * No entry found, return "not found"
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! No entry found in INGRESS_GPORT_TO_LOCAL_IN_LIF_SW matching this input! local_in_lif = %d\n",
                     local_in_lif);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GPORT, &gport));
    *vlan_port_id = gport;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the local-lif value of the port X untagged ISEM entry
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress_match_port_untagged(
    int unit,
    bcm_gport_t port,
    int *local_in_lif)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 tmp32bit;

    uint32 pp_port, core, mapped_pp_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    pp_port = gport_info.internal_port_pp_info.pp_port[0];
    core = gport_info.internal_port_pp_info.core_id[0];
    mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &tmp32bit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *local_in_lif = (int) tmp32bit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the P2P in-LIF info.
 * \see
 *  dnx_ingress_ac_fields_table_info_get_t
 *  dnx_vlan_port_find_ingress
 */
static shr_error_e
dnx_vlan_port_find_ingress_p2p_info_get(
    int unit,
    uint32 entry_handle_id,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info)
{
    uint32 tmp32;
    uint8 is_field_on_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get SERVICE-TYPE */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, &is_field_on_handle, &tmp32));

    /** If SERVICE-TYPE exist and it's value is not MP - the entry is P2P */
    if ((is_field_on_handle) && (tmp32 != DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2MP))
    {
        ingress_ac_fields_info->is_p2p = TRUE;

        /** get all relevant P2P information */
        ingress_ac_fields_info->service_type = tmp32;

        /** Get Destination */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &tmp32));
        ingress_ac_fields_info->destination = tmp32;

        /** Get glob-out-lif */
        ingress_ac_fields_info->is_glob_out_lif_exist = FALSE;
        ingress_ac_fields_info->is_eei_exist = FALSE;

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, &is_field_on_handle, &tmp32));

        if (is_field_on_handle)
        {
            ingress_ac_fields_info->glob_out_lif_or_eei_value = tmp32;
            ingress_ac_fields_info->is_glob_out_lif_exist = TRUE;
        }
        else
        {

            /** Get eei */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                            (unit, entry_handle_id, DBAL_FIELD_EEI, &is_field_on_handle, &tmp32));

            if (is_field_on_handle)
            {
                ingress_ac_fields_info->glob_out_lif_or_eei_value = tmp32;
                ingress_ac_fields_info->is_eei_exist = TRUE;
            }
        }

    }
    else
    {
        ingress_ac_fields_info->is_p2p = FALSE;
        ingress_ac_fields_info->service_type = DBAL_NOF_ENUM_VTT_LIF_SERVICE_TYPE_VALUES;
        ingress_ac_fields_info->destination = 0;
        ingress_ac_fields_info->glob_out_lif_or_eei_value = 0;
        ingress_ac_fields_info->is_glob_out_lif_exist = FALSE;
        ingress_ac_fields_info->is_eei_exist = FALSE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * This function gets the Native/non-Native in-LIF info.
 * For non-Native in-LIF:
 * - flags
 * - vsi
 * - port
 * - match_vlan
 * - match_inner_vlan
 * - ingress nw group id
 * For Native in-LIF:
 * - flags
 * - vsi
 * \see
 *  bcm_dnx_vlan_port_create
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_ingress(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info)
{
    uint32 entry_handle_id;
    uint32 in_lif_profile;
    uint32 vsi;
    bcm_gport_t failover_port_id;
    uint32 dbal_struct_data;
    in_lif_profile_info_t in_lif_profile_info;
    int is_native, is_native_non_virtual;
    uint8 has_vsi, is_wide, has_stat, is_field_on_handle;
    lif_table_mngr_inlif_info_t lif_info;

    /** Needed to prevent Coverity issue of Out-of-bounds access (ARRAY_VS_SINGLETON) */
    int failover_id[1] = { 0 };
    int failover_id_decoded[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Parameters from HW table
     */
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    /*
     * Set keys and gets all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                    (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif, entry_handle_id, &lif_info));

    /*
     * Retrieve Split Horizon ingress information:
     *  - Non-Native
     *  - Native non-virtual
     */
    is_native =
        ((gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ||
         (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)) ?
        TRUE : FALSE;

    is_native_non_virtual = is_native && (gport_hw_resources->global_in_lif != DNX_ALGO_GPM_LIF_INVALID);

    if ((!is_native) || is_native_non_virtual)
    {
        /*
         * Get value:
         * - ingress_network_group_id - via in_lif_profile
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, &in_lif_profile));
        SHR_IF_ERR_EXIT(dnx_in_lif_profile_get_data(unit, in_lif_profile, &in_lif_profile_info, LIF));
        vlan_port->ingress_network_group_id = in_lif_profile_info.egress_fields.in_lif_orientation;
    }
    else
    {
        /** set  in_lif_profile to invalid value */
        in_lif_profile = NOF_IN_LIF_PROFILES;
    }

    
    /*
     * FIXM SDK-180912 - JR2 - LIF table manager and FODO field
     * Control LIF has FODO field and not VSI thus how to handle it here?
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_VSI, &has_vsi, &vsi));
    if (has_vsi == TRUE)
    {
        vlan_port->vsi = (bcm_vlan_t) vsi;
    }

    /*
     * Retrieve flush group information:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, &is_field_on_handle,
                     &dbal_struct_data));

    if (is_field_on_handle == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_LEARN_INFO_NO_LIF_EXT_SYM_LIF_EXT, DBAL_FIELD_FLUSH_GROUP,
                         &(vlan_port->group), &dbal_struct_data));
    }

    
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, &is_field_on_handle, &dbal_struct_data));

    if (is_field_on_handle == TRUE)
    {
        if (dbal_struct_data == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_VLAN)
        {
            vlan_port->flags |= BCM_VLAN_PORT_VSI_BASE_VID;
        }
        else if (dbal_struct_data == DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP)
        {
            vlan_port->flags |= BCM_VLAN_PORT_RECYCLE;

            /*
             * FIXM SDK-180912 - JR2 - LIF table manager and FODO field
             * Control LIF has FODO field, should I read it here ?
             */
        }
    }

    /*
     * Get the parameters from match info SW table:
     * - port
     * - criteria
     * - match_vlan
     * - match_inner_vlan
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get
                    (unit, gport_hw_resources->inlif_dbal_table_id, gport_hw_resources->local_in_lif, vlan_port));

    /*
     * Update Native flag:
     */
    if (is_native == TRUE)
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    /*
     * Restore BCM_VLAN_PORT_INGRESS_WIDE and BCM_VLAN_PORT_STAT_INGRESS_ENABLE flags:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_LIF_GENERIC_DATA_0, &is_wide));

    if (is_wide)
    {
        vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &has_stat));

    if (has_stat)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }

    /*
     * Get Protection parameters for supported formats
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, &is_field_on_handle,
                     (uint32 *) (&failover_id[0])));
    if (is_field_on_handle)
    {
        if (failover_id[0] != dnx_data_failover.path_select.ing_no_protection_get(unit))
        {
            /** Get Failovr ID from IN-LIF protection pointer format */
            DNX_FAILOVER_ID_PROTECTION_POINTER_DECODE(failover_id_decoded[0], failover_id[0]);
            DNX_FAILOVER_SET(vlan_port->ingress_failover_id, failover_id_decoded[0], DNX_FAILOVER_TYPE_INGRESS);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE,
                         (uint32 *) (&failover_port_id)));
        vlan_port->ingress_failover_port_id = (failover_port_id) ? 0 : 1;
    }

    /*
     * get forwarding info for protection
     * Note: protection info is not set for Native or for VLAN-Translation In-LIFs.
     */
    if (((_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) == FALSE) ||
         (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VSI_BASE_VID) == TRUE)) &&
        (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_gport_to_forward_information_get(unit, vlan_port));
    }

    /*
     * Get the internal fields:
     *   -in_lif_profile
     *   - destination
     *   - service_type
     *   - is_p2p
     *   - glob_out_lif_or_eei_value
     *   - is_eei_exist
     */
    if (ingress_ac_fields_info != NULL)
    {
        ingress_ac_fields_info->in_lif_profile = in_lif_profile;

        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_p2p_info_get(unit, entry_handle_id, ingress_ac_fields_info));
    }

    if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_RECYCLE))
    {
        uint32 prop_prof, ecn_map_prof;

        /*
         * Retrive fields:
         *   - DBAL_FIELD_PROPAGATION_PROF
         *   - DBAL_FIELD_ECN_MAPPING_PROFILE
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROPAGATION_PROF, INST_SINGLE, &prop_prof));

        if (prop_prof == DNX_QOS_INGRESS_PROPAGATION_PROFILE_UNIFORM)
        {
            vlan_port->ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
            vlan_port->ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
            vlan_port->ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        }
        else
        {
            vlan_port->ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
            vlan_port->ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
            vlan_port->ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ECN_MAPPING_PROFILE, INST_SINGLE, &ecn_map_prof));

        if (ecn_map_prof == DNX_QOS_ECN_MAPPING_PROFILE_DEFAULT)
        {
            vlan_port->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;
        }
        else
        {
            vlan_port->ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelEligible;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * This function returns the gport value of the ESEM LIF that is pointed by the
 * given input:
 *  - criteria - the matching criteria
 *  - flags - indicates native/non-native
 *  - vsi X local_out_lif (via port): for ESEM native.
 *  - vsi X vlan X vlan_domain (port): for ESEM non-native
 *  - vsi X vlan X vlan_domain (network_domain): for ESEM Namespace-vsi
 *  - system_port X vlan_domain (namespace): for ESEM Namespace-port
 * \remark
 *  - use_parameter, parameter_value: use the parameter value instead of the value at vlan_port
 * use_parameter distinguishes between find case where the global Out-LIF is supplied, so we need to deduct the local Out-LIF,
 * and a lookup like traverse where we find the entry from the internal DB in the form of a local Out-LIF.
 * \see
 * bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_match_esem_sw_state(
    int unit,
    bcm_vlan_port_t * vlan_port,
    int use_parameter,
    uint32 parameter_value)
{
    uint32 entry_handle_id;
    uint32 gport;
    int is_end;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * ESEM AC - native or non-native:
             */

            if (_SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE))
            {
                /*
                 * Handle ESEM AC Native:
                 */
                uint32 resultType, fodo_vsi, local_out_lif, vlan_domain, is_native_intf_name_space;
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
                bcm_gport_t match_port;

                resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC;

                /*
                 * For ESEM native find by lookup - the input is the tunnel LIF gport (gotten from the user via the BCM API) thus need to use ALGO GPM to get to local out-lif.
                 * For ESEM native traverse - the input is the key of the ESEM entry that is local-out-lif, thus need to skip ALGO GPM.
                 */
                if (use_parameter == FALSE)
                {
                    /** Get local Out-LIF using DNX Algo Gport Management */
                    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                                (unit, vlan_port->port,
                                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources),
                                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

                    local_out_lif = gport_hw_resources.local_out_lif;
                }
                else
                {
                    local_out_lif = parameter_value;

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                                DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                                DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                                                                _SHR_CORE_ALL, local_out_lif, &match_port));
                    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                                (unit, match_port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
                }

                SHR_IF_ERR_EXIT(dnx_vlan_port_create_egress_native_ac_interface_namespace_check
                                (unit, gport_hw_resources, &vlan_domain, &is_native_intf_name_space));

                fodo_vsi = vlan_port->vsi;

                /*
                 * Set iterator rules:
                 * All the fields (resultType, VSI, local_out_lif) need to equal the given values
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &fodo_vsi, NULL));
                if (is_native_intf_name_space)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                     DBAL_CONDITION_EQUAL_TO, &local_out_lif, NULL));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE,
                                     DBAL_CONDITION_EQUAL_TO, &vlan_domain, NULL));
                }
            }
            else
            {
                /*
                 * Handle ESEM AC non-native
                 */
                uint32 resultType, fodo_vsi, outer_vlan_id, port_id;

                resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC;

                fodo_vsi = vlan_port->vsi;
                outer_vlan_id = vlan_port->match_vlan;

                /*
                 * Set iterator rules:
                 * All the fields (resultType, VSI, VLAN, port/vlan_domain) need to equal the given values
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &resultType, NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &fodo_vsi,
                                 NULL));
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 &outer_vlan_id, NULL));

                /*
                 * On ESEM non-native creation (see dnx_vlan_port_create_egress_match_esem_sw_state) both the physical port and it's vlan-domain are saved in the SW DB.
                 * For ESEM non-native find by lookup - the input is the physical port (gotten from the user via the BCM API), thus lookup at the SW DB by port.
                 * For ESEM non-native traverse - the input is the key of the ESEM entry that is vlan-domain, thus lookup at the SW DB by vlan-domain.
                 */
                if (use_parameter == FALSE)
                {
                    port_id = vlan_port->port;

                    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                    (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                     &port_id, NULL));
                }
                else
                {
                    port_id = parameter_value;

                    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE,
                                     DBAL_CONDITION_EQUAL_TO, &port_id, NULL));
                }

            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            uint32 resultType, fodo_vsi, network_domain;
            resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI;
            fodo_vsi = vlan_port->vsi;
            network_domain = vlan_port->match_class_id;
            /*
             * Set iterator rules:
             * All the fields (resultType, VSI, network_domain) need to equal the given values
             */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &fodo_vsi, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &network_domain, NULL));
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            uint32 resultType, port, esem_namespace;
            resultType = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT;
            port = vlan_port->port;
            esem_namespace = vlan_port->match_class_id;
            /*
             * Set iterator rules:
             * All the fields (resultType, port, namespace) need to equal the given values
             */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                             DBAL_CONDITION_EQUAL_TO, &resultType, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &port, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                             &esem_namespace, NULL));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error ! criteria = %d is not supported for Egress Virtual AC! gport = 0x%08X, flags = 0x%08X\n",
                         vlan_port->criteria, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (is_end == TRUE)
    {
        /*
         * No entry found, return "not found"
         */

        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error! No ESEM entry found matching this input! criteria = %d, vsi = %d, match_vlan = %d, match_class_id = %d, port = %d, , gport = 0x%08X, flags = 0x%08X\n",
                     vlan_port->criteria, vlan_port->vsi, vlan_port->match_vlan, vlan_port->match_class_id,
                     vlan_port->port, vlan_port->vlan_port_id, vlan_port->flags);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GPORT, &gport));
    vlan_port->vlan_port_id = gport;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the following info from Port Default AC Profile table:
 *  - egress_network_group_id (via out_lif_profile)
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_port_default_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    uint32 ac_profile;
    uint32 out_lif_profile;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint32 egress_last_layer;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get ac_profile from port using encoding
     */
    ac_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, ac_profile);
    /*
     * Get values:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));
    /*
     *  Retrieve the Split Horizon egress information
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;
    /*
     * Native or non-native:
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &egress_last_layer));
    if (egress_last_layer == FALSE)
    {
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_vlan_port_find_egress_qos_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 table_handle_id)
{
    dbal_enum_value_field_encap_qos_model_e qos_model;
    uint32 nwk_qos_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, table_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, INST_SINGLE,
                                                        &qos_model));

    /*
     * NWK_QOS_IDX field exist at:
     *   - ESEM (with statistics or 2 VIDs)
     *   - ESEM Namespace-port
     *   - EEDB (non-virtual out-lif)
     * If not exist, 0 is returned by DBAL.
     */
    SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_entry_handle_value_field32_get
                              (unit, table_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, &nwk_qos_idx),
                              _SHR_E_NOT_FOUND);

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, qos_model, &(vlan_port->egress_qos_model)));
    if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE)
    {
        uint8 nwk_pipe, nwk_pipe_mapped, dp_pipe_mapped;
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get(unit, nwk_qos_idx,
                                                                         &nwk_pipe, &nwk_pipe_mapped, &dp_pipe_mapped));
        vlan_port->pkt_pri = DNX_QOS_IDX_NWK_QOS_PCP_GET(nwk_pipe_mapped);
        vlan_port->pkt_cfi = DNX_QOS_IDX_NWK_QOS_CFI_GET(nwk_pipe_mapped);
    }
    else if (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)
    {
        vlan_port->pkt_pri = DNX_QOS_EXPLICIT_IDX_PCP_GET(nwk_qos_idx);
        vlan_port->pkt_cfi = DNX_QOS_EXPLICIT_IDX_CFI_GET(nwk_qos_idx);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF info.
 * For ESEM non-Native LIF:
 *  -vsi
 *  -match_vlan
 *  -port
 *  - egress_network_group_id (via out_lif_profile)
 * For ESEM Native LIF:
 *  -vsi
 *  -port
 * For ESEM Namespace-vsi LIF:
 *  - vsi
 *  - match_class_id
 *  - egress_network_group_id (via out_lif_profile)
 * For ESEM Namespace-port LIF:
 *  - port
 *   -match_class_id
 *  - egress_network_group_id (via out_lif_profile)
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_esem_info_get(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 sw_table_handle_id;
    uint32 entry_type;
    uint32 out_lif_profile;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));

    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            uint32 esem_handle_id;
            uint32 is_stat_egress;
            uint32 is_native_intf_name_space;
            uint32 is_two_vlans;

            /*
             * Handle ESEM AC Native:
             */
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_NATIVE;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_STAT_EGRESS_ENABLE_FLAG_SET, INST_SINGLE,
                             &is_stat_egress));
            if (is_stat_egress)
            {
                vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
            }

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_TWO_VLANS_FLAG_SET, INST_SINGLE, &is_two_vlans));
            if (is_two_vlans)
            {
                vlan_port->flags |= BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
            }

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE,
                             &is_native_intf_name_space));
            if (is_native_intf_name_space)
            {
                dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB;
            }
            else
            {
                dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &esem_handle_id));
            /*
             * Get the ESEM keys {VSI, out_lif} or {VSI, name_space}:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_get
                            (unit, sw_table_handle_id, esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {
            uint32 esem_handle_id;
            uint32 is_stat_egress;
            uint32 is_two_vlans;

            /*
             * Handle ESEM AC non-native:
             */
            dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_STAT_EGRESS_ENABLE_FLAG_SET, INST_SINGLE,
                             &is_stat_egress));
            if (is_stat_egress)
            {
                vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
            }

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_TWO_VLANS_FLAG_SET, INST_SINGLE, &is_two_vlans));
            if (is_two_vlans)
            {
                vlan_port->flags |= BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (VSI, port):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_key_get(unit, sw_table_handle_id, &esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            uint32 esem_handle_id;
            bcm_vlan_t vsi;
            uint32 match_class_id;

            dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (VSI, match_class_id):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_vsi_key_get
                            (unit, sw_table_handle_id, &esem_handle_id, &vsi, &match_class_id));
            vlan_port->vsi = vsi;
            vlan_port->match_class_id = match_class_id;
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            uint32 esem_handle_id;
            dbal_table_id = DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB;
            vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
            vlan_port->criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &esem_handle_id));
            /*
             * Get (and set) the ESEM keys (port, match_class_id):
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_namespace_port_key_get
                            (unit, sw_table_handle_id, &esem_handle_id, vlan_port));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, esem_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile));

            /**pcp/dei and qos model*/
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, esem_handle_id));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, vlan_port->vlan_port_id, entry_type);
    }

    /*
     *  Retrieve the Split Horizon egress information
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function gets the following info from EEDB table:
 *  - egress_network_group_id (via out_lif_profile)
 *  - tunnel_id (relevant in case of native AC)
 * If tunnel_id is not zero, flag BCM_VLAN_PORT_NATIVE is set.
 * \see
 *  bcm_dnx_vlan_port_find
 */
static shr_error_e
dnx_vlan_port_find_egress_eedb_info_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 entry_handle_id;
    lif_table_mngr_outlif_info_t outlif_info;
    uint32 out_lif_profile;
    uint32 next_local_out_lif = 0;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    uint32 failover_id, failover_port_id;
    uint8 has_stat;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));

    /*
     * Set keys and get all fields:
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources->local_out_lif, entry_handle_id, &outlif_info));

    /*
     * Determine if it's a Native Out-LIF and retrieve relevant information
     */
    if (gport_hw_resources->outlif_phase == LIF_MNGR_OUTLIF_PHASE_NATIVE_AC)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, &next_local_out_lif));

        /*
         * This result-type is for EEDB Native, see dnx_vlan_port_create_egress_non_virtual_lif_table_result_type_calc
         */
        vlan_port->flags |= BCM_VLAN_PORT_NATIVE;
    }

    if (next_local_out_lif != 0)
    {
        /*
         * next_local_out_lif is the local out lif, get the related gport.
         * In case the tunnel LIF was not found (either it was deleted or not yet allocated,
         * don't return an error, just set the tunnel_id to INVALID gport.
         */
        vlan_port->tunnel_id = BCM_GPORT_INVALID;

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT,
                         _SHR_CORE_ALL, (int) next_local_out_lif, &(vlan_port->tunnel_id)));
    }
    else
    {
        vlan_port->tunnel_id = 0;
    }

    /**
     * outlif profile
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                                        &out_lif_profile));

    /*
     * Retrieve the Split Horizon egress information
     */
    SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
    vlan_port->egress_network_group_id = out_lif_profile_info.out_lif_orientation;

    /*
     * Retrieve Protection parameters
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PROTECTION_POINTER, INST_SINGLE, &failover_id));

    if (failover_id != dnx_data_failover.path_select.egr_no_protection_get(unit))
    {
        DNX_FAILOVER_SET(vlan_port->egress_failover_id, failover_id, DNX_FAILOVER_TYPE_ENCAP);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_PROTECTION_PATH, INST_SINGLE, &failover_port_id));
        vlan_port->egress_failover_port_id = (failover_port_id) ? 0 : 1;
        vlan_port->flags |= BCM_VLAN_PORT_EGRESS_PROTECTION;
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &has_stat));

    /*
     * Retrieve statistic parameters
     */
    if (has_stat)
    {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    }

    /*
     * In symmetric case, criteria is updated by ingress function (see dnx_vlan_port_create_ingress_lif_match_info_get).
     * If this is Egress (EEDB) only, criteria need to be set to NONE.
     */
    if (gport_hw_resources->local_in_lif == DNX_ALGO_GPM_LIF_INVALID)
    {
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
    }

    /**pcp/dei and qos model*/
    SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_qos_info_get(unit, vlan_port, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API retrieves VLAN Port gport according to a given VLAN Port criteria and match-info.
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in,out] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 *   [out] vlan_port.vlan_port_id - contains the found gport
 *   [in] vlan_port.criteria - logical layer 2 port match criteria.
 *   [in] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [in] vlan_port.vsi - virtual switching instance.
 *   [in] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [in] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [in] vlan_port.port - Physical or logical gport.
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 * flags field is used to signal which LIF the user is looking for; ingress-only, egress-only, symmetric,
 * native and virtual.
 * Thus, the following flags are relevant:
 *  BCM_VLAN_PORT_CREATE_EGRESS_ONLY
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY
 *  BCM_VLAN_PORT_NATIVE
 *  BCM_VLAN_PORT_VLAN_TRANSLATION
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_port_find_by_lookup(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int is_ingress, is_egress, is_native_ac, is_virtual_ac;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Find the VLAN-PORT using lookup:
     *  - criteria
     *  - match_vlan
     *  - match_inner_vlan
     *  - flags
     *  - port
     *  - vsi
     */
    is_ingress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY) ? FALSE : TRUE;
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    is_virtual_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    /*
     * For Ingress only LIFs and Symmetric LIFs, find the gport by finding the local in-lif using the criteria and match-info:
     */
    if (is_ingress == TRUE)
    {
        int local_in_lif = -1;

        switch (vlan_port->criteria)
        {
            case BCM_VLAN_PORT_MATCH_PORT:
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port(unit, vlan_port->port, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN:
            {
                if (is_native_ac == FALSE)
                {
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_match_port_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_c_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, &local_in_lif));
                }
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
            {

                if (is_native_ac == FALSE)
                {
                    if (vlan_port->match_ethertype)
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan
                                        (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                         &local_in_lif));
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_match_port_vlan_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                     &local_in_lif));
                }

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
            {

                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_cvlan_cvlan
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 &local_in_lif));

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
            {

                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_svlan_svlan
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 &local_in_lif));

                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, &local_in_lif));
                break;
            }
            case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 vlan_port->match_ethertype, vlan_port->match_pcp, &local_in_lif));
                break;
            }

            case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
            {

                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, &local_in_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_match_port_untagged
                                    (unit, vlan_port->port, &local_in_lif));
                }

                break;
            }

            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. criteria = %d is not supported for VLAN-Port find using lookup!!!\n",
                             vlan_port->criteria);
            }
        }

        /*
         * Retrieves the gport from the given local in-lif:
         */
        if (is_virtual_ac == FALSE)
        {
            /*
             * The local in-lif is not "virtual", retrieve it's gport using the GPM
             */
            if (is_native_ac == FALSE)
            {
                /*
                 * For AC LIF - it is shared between cores thus use SBC flag:
                 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS,
                                                            _SHR_CORE_ALL, local_in_lif, &(vlan_port->vlan_port_id)));
            }
            else
            {
                /*
                 * For Native LIF - it is dedicated per core thus use DPC flag:
                 */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                            DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS,
                                                            _SHR_CORE_ALL, local_in_lif, &(vlan_port->vlan_port_id)));
            }
        }
        else
        {
            /*
             * The local in-lif is "virtual", look for it in the SW DB
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state(unit, (int) local_in_lif,
                                                                                     &(vlan_port->vlan_port_id)));
        }

    }
    else if (is_egress == TRUE)
    {
        /*
         * For Egress only, only ESEM is supported (Default-ESEM and EEDB are not supported for lookup):
         *   - check EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW to find such a lookup
         * See dnx_vlan_port_find_verify
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_match_esem_sw_state(unit, vlan_port, FALSE, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API retrieves VLAN Port information according to a given gport.
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in,out] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 * For retrieves according to gport:
 *   [in] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [out] vlan_port.criteria - logical layer 2 port match criteria.
 *   [out] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [out] vlan_port.vsi - virtual switching instance.
 *   [out] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [out] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [out] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 * For retrieves according to criteria and match-info (lookup):
 *   [out] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [in] vlan_port.criteria - logical layer 2 port match criteria.
 *   [in] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [in] vlan_port.vsi - virtual switching instance.
 *   [in] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [in] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [in] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 *   \param [out] ingress_ac_fields_info -pointer to strcture to be filled with the desired
 *   fields value.
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_vlan_port_find_by_gport(
    int unit,
    bcm_vlan_port_t * vlan_port,
    dnx_ingress_ac_fields_table_info_get_t * ingress_ac_fields_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Find the VLAN-PORT using its gport:
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(vlan_port->vlan_port_id))
    {
        /*
         * Handle Ingress Virtual Native Vlan port
         */
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * get Local In-LIF:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                         &gport_hw_resources));
        vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_VLAN_TRANSLATION;
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress(unit, &gport_hw_resources, vlan_port, ingress_ac_fields_info));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(vlan_port->vlan_port_id))
    {
        /*
         * Handle Egress Virtual Vlan ports:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_esem_info_get(unit, vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port->vlan_port_id))
    {
        /*
         * Handle Egress ESEM Default:
         */
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_DEFAULT;
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_port_default_info_get(unit, vlan_port));
    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        /*
         * Get local LIFs using DNX Algo Gport Managment:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, vlan_port->vlan_port_id,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
        vlan_port->flags = 0;
        if (gport_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {

            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY))
            {
                vlan_port->flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
                vlan_port->encap_id = 0;
            }

            SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress(unit, &gport_hw_resources, vlan_port, ingress_ac_fields_info));
        }

        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            /*
             * EEDB:
             */
            vlan_port->encap_id = gport_hw_resources.global_out_lif;
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port->vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
            {
                vlan_port->flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            }

            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_eedb_info_get(unit, &gport_hw_resources, vlan_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API retrieves VLAN Port information according to a given gport or
 * according to a given VLAN Port criteria and match-info (lookup).
 *
 *   \param [in] unit -Relevant unit.
 *   \param [in,out] vlan_port -
 *   Pointer to a structure of type bcm_vlan_port_t that
 *   contains both input and output configuration parameters of
 *   the VLAN PORT.
 * For retrieves according to gport:
 *   [in] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [out] vlan_port.criteria - logical layer 2 port match criteria.
 *   [out] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [out] vlan_port.vsi - virtual switching instance.
 *   [out] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [out] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [out] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 * For retrieves according to criteria and match-info (lookup):
 *   [out] vlan_port.vlan_port_id - contains the gport
 *   that has to be encoded with at least a type and a subtype.
 *   [in] vlan_port.criteria - logical layer 2 port match criteria.
 *   [in] vlan_port.flags - for logical layer 2 port,
 *   BCM_VLAN_PORT_xxx.
 *   [in] vlan_port.vsi - virtual switching instance.
 *   [in] vlan_port.match_vlan - Outer VLAN ID to match.
 *   [in] vlan_port.match_inner_vlan - Inner VLAN ID to match.
 *   [in] vlan_port.port - Physical or logical gport.
 *   [out] vlan_port.ingress_network_group_id - ingress network group id (for Split Horizon)
 *   [out] vlan_port.egress_network_group_id - egress network group id (for Split Horizon) ;
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vlan_port_find(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_find_verify(unit, vlan_port));
    /*
     * Find the VLAN-PORT using lookup:
     *   - criteria
     *   - match_vlan
     *   - match_inner_vlan
     *   - flags
     *   - port
     */
    if (vlan_port->vlan_port_id == 0)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_by_lookup(unit, vlan_port));
    }

    /*
     * Find the VLAN-PORT using its gport:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_find_by_gport(unit, vlan_port, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x CVLAN x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_cvlan_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_C_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_1, match_inner_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x SVLAN x SVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_svlan_svlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_S_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_2, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_1, match_inner_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x VLAN x VLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_S_C_VLAN_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x VLAN x VLAN x PCP x ETHERTYPE match in ingress configuration.
 *          TCAM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan,
    bcm_port_ethertype_t match_ethertype,
    int match_pcp)
{
    uint32 vlan_domain = 0;
    uint32 entry_handle_id;
    uint32 entry_access_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Vlan Domain from Port
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Ingress - Clear AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_TCAM_DB, &entry_handle_id));
    /** Create TCAM access id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    if (match_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_inner_vlan != BCM_VLAN_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, 0x0, 0x0);
    }

    if (match_pcp != BCM_DNX_PCP_INVALID)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, match_pcp << 1,
                                          IN_AC_TCAM_DB_PCP_MASK);
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0x0, 0x0);
    }

    if (match_ethertype)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_dbal_key_ethertype_set
                        (unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, match_ethertype));
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPE, 0x0, 0x0);
    }

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_OUTER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX_INNER_VLAN, 0x0, 0x0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_PCP_DEI_INNER_VLAN, 0x0, 0x0);
    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, DBAL_TABLE_IN_AC_TCAM_DB, entry_access_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x untagged match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_untagged_clear(
    int unit,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_UNTAGGED_DB, &entry_handle_id));

    /*
     * Ingress - Clear AC lookup entry
     */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_unmatched_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_UNMATCHED_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_untagged_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress default action
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_UNTAGGED_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL x VLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_S_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_vlan);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL x SVLAN x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_S_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL x CVLAN x CVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_cvlan_cvlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_C_C_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_2, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID_1, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear PORT x TUNNEL x SVLAN x SVLAN match in ingress configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_svlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t tunnel_id,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Port + Core
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PON_DTC_S_S_TAG_CLASSIFICATION, &entry_handle_id));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        uint32 pp_port = gport_info.internal_port_pp_info.pp_port[pp_port_index];
        uint32 core = gport_info.internal_port_pp_info.core_id[pp_port_index];
        uint32 mapped_pp_port = pp_port | (core << dnx_data_port.general.pp_port_bits_size_get(unit));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_PP_PORT, mapped_pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PON_CHANNEL_ID, tunnel_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_2, match_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_S_VID_1, match_inner_vlan);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See header file bcm_int/dnx/vlan/vlan.h for description
 */
shr_error_e
dnx_vlan_port_destroy_ingress_non_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            /*
             * No matching, nothing to do
             */
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                int initial_default_in_lif;
                /*
                 * Get initial default_in_lif (local in-lif)
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_default_lif_get(unit, &initial_default_in_lif));
                /*
                 * Set port back to default in-LIF
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_default
                                (unit, vlan_port->port, initial_default_in_lif));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_cvlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_clear
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_cvlan_cvlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_svlan_svlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * Clear the TCAM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                             vlan_port->match_ethertype, vlan_port->match_pcp));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * Clear the TCAM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                             vlan_port->match_ethertype, vlan_port->match_pcp));
            break;
        }

        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {
            if (vlan_port->match_ethertype)
            {
                /*
                 * Clear the TCAM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_vlan_vlan_pcp_ethertype_clear
                                (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID));
            }
            else
            {
                /*
                 * Clear the ISEM entry
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_untagged_clear(unit, vlan_port->port));
            }

            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_unmatched_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, vlan_port->match_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_cvlan_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, vlan_port->match_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_cvlan_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_cvlan_cvlan_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_svlan_svlan_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_match_port_tunnel_untagged_clear
                            (unit, vlan_port->port, vlan_port->match_tunnel_value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error, gport = 0x%08X contains unknown/unsupported criteria (=%d)!\n",
                         vlan_port->vlan_port_id, vlan_port->criteria);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT match in ingress native configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_native_match_port_clear(
    int unit,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_0_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Ingress - Clear Native AC lookup entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN match in ingress native configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_native_match_port_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Ingress - Clear Native AC lookup entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears PORT x VLAN x VLAN match in ingress native configuration.
 *          ISEM lookup is cleared.
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_native_match_port_vlan_vlan_clear(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t match_vlan,
    bcm_vlan_t match_inner_vlan)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources_match;
    uint32 matched_global_in_lif;
    uint32 vlan_domain = 0, is_intf_namespace = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get Global In-LIF of the port using DNX Algo Gport Management:
     */
    sal_memset(&gport_hw_resources_match, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources_match),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    matched_global_in_lif = gport_hw_resources_match.global_in_lif;

    /*
     * get vlan_domain and check whether LIF or NETWORK scope should be used
     */
    SHR_IF_ERR_EXIT(dnx_ingress_native_ac_interface_namespace_check
                    (unit, gport_hw_resources_match, &vlan_domain, &is_intf_namespace));

    if (is_intf_namespace)
    {
        /*
         * Port * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_LIF_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted global lif in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, matched_global_in_lif);
    }
    else
    {
        /*
         * vlan_domain * VLAN * VLAN match, take handle.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS,
                         &entry_handle_id));
        /** Set the extracted vlan domain in the key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    }

    /*
     * Ingress - Clear Native AC lookup entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_OUTER_VLAN, match_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_INNER_VLAN, match_inner_vlan);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  See header file bcm_int/dnx/vlan/vlan.h for description
 */
shr_error_e
dnx_vlan_port_destroy_ingress_native_match_clear(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            /*
             * Clear the ISEM entry.
             * Currently this entry can be created only through the bcm_port_match_add() API
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_port_clear(unit, vlan_port->port));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_port_vlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            /*
             * Clear the ISEM entry
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_port_vlan_vlan_clear
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error, gport = 0x%08X contains unknown/unsupported criteria (=%d) for Native In-LIF!\n",
                         vlan_port->vlan_port_id, vlan_port->criteria);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears IN-LIF Info table
 * \see
 *  bcm_dnx_vlan_port_destroy_ingress
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_lif_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources->inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources->local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources->inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function clears OUT-LIF Info table (EEDB)
 * \see
 *  bcm_dnx_vlan_port_destroy_egress
 */
static shr_error_e
dnx_vlan_port_destroy_egress_lif_info_clear(
    int unit,
    uint32 result_type,
    int local_out_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress AC at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_non_native_info_clear(
    int unit,
    bcm_vlan_t vsi,
    uint32 vlan_domain,
    bcm_vlan_t c_vid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress Native AC at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_native_info_clear(
    int unit,
    dbal_tables_e dbal_table_id,
    int local_out_lif,
    bcm_vlan_t vsi,
    uint32 vlan_domain,
    bcm_vlan_t c_vid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear entry in Native ESEM
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    if (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif);
    }
    else if (dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL TABLE(%d) for native AC is not expected!\n", dbal_table_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears VLAN PORT egress Namespace-port at ESEM
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_port_info_clear(
    int unit,
    uint32 system_port,
    uint32 vlan_domain)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, system_port);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears entry at ESEM Default table
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_default_info_clear(
    int unit,
    int esem_default_handle)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, esem_default_handle);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function frees an allocated LIF
 * Its frees the in-LIF and if the LIF is symmetric, also the out-LIF is freed.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_lif_free(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint32 direction_flags;
    int global_lif;

    SHR_FUNC_INIT_VARS(unit);

    direction_flags = 0;
    global_lif = DNX_ALGO_GPM_LIF_INVALID;

    if (gport_hw_resources->local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        uint32 flags = 0;
        /*
         * It's ingress virtual native AC object, hasn't ingress local_lif to global_lif mapping in sw state
         */
        if (gport_hw_resources->inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)
        {
            flags = LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING;
        }
        /*
         * Free Lif table and local LIF allocation
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_inlif_info_clear
                        (unit, _SHR_CORE_ALL, gport_hw_resources->local_in_lif, gport_hw_resources->inlif_dbal_table_id,
                         flags));

        direction_flags = DNX_ALGO_LIF_INGRESS;
        global_lif = gport_hw_resources->global_in_lif;

    }

    if (gport_hw_resources->local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        /*
         * Free Lif table and local LIF allocation
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, gport_hw_resources->local_out_lif, 0));

        direction_flags |= DNX_ALGO_LIF_EGRESS;

        /*
         * Note: for symmetric LIF, global_out_lif is equal to global_in_lif thus no problem to overwrite it.
         */
        global_lif = gport_hw_resources->global_out_lif;
    }

    /*
     * Delete global lif - if valid (for example, for ingress native virtual, it is skipped)
     */
    if (global_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, direction_flags, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated in-LIF.
 * It frees all relevant allocated HW and SW resources, except for the In-LIF resource manager allocation.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_non_virtual(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_vlan_port_t * vlan_port)
{
    uint32 in_lif_profile;
    int is_native_ac;
    SHR_FUNC_INIT_VARS(unit);
    is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE) ? TRUE : FALSE;
    /*
     * 1. Clear match table
     */
    if (is_native_ac == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_native_match_clear(unit, vlan_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_clear(unit, vlan_port));
    }

    /*
     * 2. Dealloc in-lif profiles:
     *      - in_lif_profile
     *      - qos_profile:
     *      qos_profile should be freed by the user.
     *      See bcm_qos_map_create and bcm_qos_map_destroy.
     */
    SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_get(unit, gport_hw_resources, &in_lif_profile));
    SHR_IF_ERR_EXIT(dnx_ingress_inlif_profile_dealloc(unit, in_lif_profile));
    /*
     * 3. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_match_info_clear
                    (unit, is_native_ac, gport_hw_resources->local_in_lif));
    /*
     * Clear forwarding info SW state DB
     * See vlan_port_gport_to_forward_information_set (which calls algo_gpm_gport_l2_forward_info_add)
     */
    if (!is_native_ac)
    {
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, vlan_port->vlan_port_id));
    }

    /*
     * Clear SW state entry - non
     */

    /*
     * 4. Clear In-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_info_clear(unit, gport_hw_resources));
    /*
     * 5. Dealloc the IN-LIF
     * Note: because of symmetric LIF, need to free the IN-LIF and the OUT-LIF together thus
     * it is done after the Egress destroy function (see dnx_vlan_port_destroy_lif_free).
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clears Ingress Virtual Native SW state  entry.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_virtual_native_ac_sw_state_clear(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated Ingress Virtual Native LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_ingress_virtual_native(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int sw_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the Local In-LIF:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, vlan_port->vlan_port_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                     &gport_hw_resources));
    /*
     * 1. Clear match table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_native_match_clear(unit, vlan_port));
    /*
     * 2. Dealloc in-lif profiles:
     *      Ingress Virtual Native does not have in-lif profile, only qos_profile.
     *      qos_profile should be freed by the user.
     *      See bcm_qos_map_create and bcm_qos_map_destroy.
     *      Thus, nothing to do.
     */
    /*
     * 3. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_match_info_clear(unit, TRUE, gport_hw_resources.local_in_lif));
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_virtual_native_ac_sw_state_clear(unit, vlan_port->vlan_port_id));
    /*
     * 4. Clear In-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_lif_info_clear(unit, &gport_hw_resources));
    /*
     * 5. Dealloc sw handle
     */
    sw_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.free_single(unit, sw_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - free AC's egress network_qos pipe profile if qos model is pipeMyNameSpace
 */
static shr_error_e
dnx_vlan_port_destroy_egress_qos_pipe_profile(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint8 is_last = 0;
    int nwk_qos_idx = 0;
    uint8 nwk_qos;

    SHR_FUNC_INIT_VARS(unit);

    nwk_qos = DNX_QOS_IDX_PCP_DEI_TO_NWK_QOS(vlan_port->pkt_pri, vlan_port->pkt_cfi);
    SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_get(unit, nwk_qos, nwk_qos, 0, &nwk_qos_idx));
    SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free(unit, nwk_qos_idx, &is_last));

    if (is_last)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, nwk_qos_idx));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Default LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_default(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    int esem_default_handle;
    uint32 sw_table_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_default_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port->vlan_port_id);
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, 0, 0, 0, 0, 0, esem_default_handle, 0));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));

    /*
     * 3. Clear the ESEM Default table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_default_info_clear(unit, esem_default_handle));
    /*
     * 4. Dealloc Egress Default handle
     */
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.free_single(unit, esem_default_handle));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_non_native(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get port's vlan_domain
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0, 0, vlan_port->vsi, vlan_domain,
                     vlan_port->match_vlan, 0, 0));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Clear the ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native_info_clear
                    (unit, vlan_port->vsi, vlan_domain, vlan_port->match_vlan));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));
    /*
     * 5. clear QOS pipe profile
     */
    if (vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_qos_pipe_profile(unit, vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM  Native LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_native(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    int esem_handle;
    uint32 is_lif_scope;
    uint32 vlan_domain;
    uint32 local_out_lif;
    dbal_tables_e native_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, &local_out_lif));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE, &is_lif_scope));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, &vlan_domain));

    if (is_lif_scope)
    {
        native_table = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB;
    }
    else
    {
        native_table = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
    }

    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, native_table, (int) local_out_lif, 0, vlan_port->vsi, vlan_domain, 0, 0, 0));
    /*
     * 2. Clear the Native ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_native_info_clear
                    (unit, native_table, (int) local_out_lif, vlan_port->vsi, vlan_domain, 0));
    /*
     * 3. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));
    /*
     * 4. clear QOS pipe profile
     */
    if (vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_qos_pipe_profile(unit, vlan_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Namespace-vsi LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_vsi(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    int esem_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    vlan_domain = vlan_port->match_class_id;
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, 0, 0, vlan_port->vsi, vlan_domain, 0, 0, 0));
    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));
    /*
     * 3. Clear the ESEM table content:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native_info_clear(unit, vlan_port->vsi, vlan_domain, 0));
    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated ESEM Namespace-port LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem_namespace_port(
    int unit,
    uint32 sw_table_handle_id,
    bcm_vlan_port_t * vlan_port)
{
    uint32 vlan_domain;
    int esem_handle;
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, vlan_port->port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /*
     * 1. Free profile:
     *      - out_lif_profile
     */
    vlan_domain = vlan_port->match_class_id;
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, 0, 0, vlan_port->vsi,
                     vlan_domain, 0, 0, gport_phy_info.sys_port));

    /*
     * 2. Clear SW state DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_table_handle_id, DBAL_COMMIT));

    /*
     * 3. Clear the ESEM table content:
     */
    if ((BCM_GPORT_IS_TRUNK(vlan_port->port)) &&
        dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_key_with_sspa_contains_member_id))
    {
        
        int member;
        int pool;
        int max_members = 0;
        uint32 spa;

        /*
         * Get max members
         */
        BCM_TRUNK_ID_POOL_GET(pool, BCM_GPORT_TRUNK_GET(vlan_port->port));
        max_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;

        /*
         * Clear entry for each member.
         */
        for (member = 0; member < max_members; member++)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, vlan_port->port, member, &spa));
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_port_info_clear(unit, spa, vlan_domain));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_port_info_clear
                        (unit, gport_phy_info.sys_port, vlan_domain));
    }

    /*
     * 4. Dealloc ESEM handle
     */
    esem_handle = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.free_single(unit, esem_handle));
    /*
     * 5. clear QOS pipe profile
     */
    if (vlan_port->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_qos_pipe_profile(unit, vlan_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function destroys ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF.
 * This function deletes an allocated ESEM Native/non-Native/Namespace-vsi/Namespace-port LIF.
 * It frees all relevant allocated HW and SW resources.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_esem(
    int unit,
    bcm_vlan_port_t * vlan_port)
{
    uint32 sw_table_handle_id;
    uint32 entry_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
    dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, vlan_port->vlan_port_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));
    
    switch (entry_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
        {
            /*
             * Handle ESEM AC Native:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_native(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {
            /*
             * Handle ESEM AC non-native:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_non_native(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
        {
            /*
             * Handle ESEM Namespace-vsi:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_vsi(unit, sw_table_handle_id, vlan_port));
            break;
        }
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
        {
            /*
             * Handle ESEM Namespace-port:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_namespace_port(unit, sw_table_handle_id, vlan_port));
            break;
        }
        default:

            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                         " resultType = %d is not supported.\n",
                         DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, vlan_port->vlan_port_id, entry_type);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function deletes an allocated (EEDB) out-LIF.
 * It frees all relevant allocated HW and SW resources, except for the Out-LIF resource manager allocation.
 * \see
 *  bcm_dnx_vlan_port_destroy
 */
static shr_error_e
dnx_vlan_port_destroy_egress_eedb(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * 1. Remove Global to local mapping (from GLEM).
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources->global_out_lif));
    /*
     * 2. Free profile:
     *      - out_lif_profile
     */
    SHR_IF_ERR_EXIT(dnx_egress_ac_outlif_profile_free
                    (unit, DBAL_TABLE_EEDB_OUT_AC, gport_hw_resources->local_out_lif,
                     gport_hw_resources->outlif_dbal_result_type, 0, 0, 0, 0, 0));
    /*
     * 3. Clear SW state DB
     * Note: no SW state DB, nothing to clear!
     */
    /*
     * Clear forwarding info SW state DB
     * Forward Info is only for symmetric case.
     * Note: clearing forwarding info SW state DB is done in the Ingress function (see dnx_vlan_port_destroy_ingress)
     */
    /*
     * 4. Clear Out-LIF table
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_lif_info_clear
                    (unit, gport_hw_resources->outlif_dbal_result_type, gport_hw_resources->local_out_lif));
    /*
     * 5. Dealloc the OUT-LIF
     * Note: done together with the IN-LIF free becasue of symmetric case (see dnx_vlan_port_destroy_lif_free).
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API destroys a VLAN PORT.
 *
 * \param [in] unit - relevant unit.
 * \param [in] gport - VLAN PORT ID (Gport encoded with at least a type and subtype).
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark
 * Only LIF resources that were allocated/created during bcm_vlan_port_create are freed/destroyed.
 * LIF resources that are allocated/created later on (like bcm_port_match_add, etc) are not treated
 * by this API and should be handled by user.
 * \see
 *   None
 */
shr_error_e
bcm_dnx_vlan_port_destroy(
    int unit,
    bcm_gport_t gport)
{
    bcm_vlan_port_t vlan_port;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_destroy_verify(unit, gport));
    bcm_vlan_port_t_init(&vlan_port);
    /*
     * Retrieve the VLAN-Port information:
     */
    vlan_port.vlan_port_id = gport;
    SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
    {
        /*
         * Handle Egress Virtual Vlan ports
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem(unit, &vlan_port));
    }
    else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
    {
        /*
         * Egress Default AC:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_esem_default(unit, &vlan_port));
    }
    else
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        uint32 gpm_flags;

        /*
         * Handle In-LIF / Out-LIFs AC:
         */
        /*
         * Get local LIFs using DNX Algo Gport Managment:
         * No need for strict check because it was already done in _get.
         */
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
        {
            gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gpm_flags, &gport_hw_resources));
            /*
             * Ingress Virtual Native Vlan port
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_virtual_native(unit, &vlan_port));
        }
        else
        {
            gpm_flags =
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF;
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gpm_flags, &gport_hw_resources));
            if (gport_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_ingress_non_virtual(unit, &gport_hw_resources, &vlan_port));
            }

            if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                /*
                 * EEDB:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_eedb(unit, &gport_hw_resources));
                /*
                 * clear QOS pipe profile
                 */
                if (vlan_port.egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_egress_qos_pipe_profile(unit, &vlan_port));
                }
            }
        }

        /*
         * Free the IN-LIF and OUT-LIF
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_destroy_lif_free(unit, &gport_hw_resources));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Ingress non-native objects. \n
 *
 * \remark
 * symmetric LIFs will be skipped based on "is_symmetric" indication:
 * is_symmetric = TRUE - traverse ingress-only AND symmetric LIFs
 * is_symmetric = FALSE - traverse ingress-only LIFS
 *
 * \see
 *   dnx_vlan_port_traverse_ingress
 */
static shr_error_e
dnx_vlan_port_traverse_ingress_non_native(
    int unit,
    int is_symmetric,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 drop_in_lif;
    uint32 initial_bridge_in_lif;
    uint32 recycle_default_inlif;
    uint32 global_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse Ingress non-native.
     *  Note: symmetric LIFs will be skipped based on "is_symmetric" indication
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_port_default_init):
     *  - drop LIF
     *  - bridge LIF
     *  - recycle LIF
     */
    drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &drop_in_lif, NULL));

    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, (int *) &initial_bridge_in_lif));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &initial_bridge_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, &recycle_default_inlif));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &recycle_default_inlif,
                     NULL));

    /*
     * Skip symmetric LIF:
     *   - ingress only lifs - the global-lif is zero!
     */
    if (is_symmetric == FALSE)
    {
        global_lif = 0;

        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         &global_lif, NULL));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Get gport from In-LIF
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS, DBAL_CORE_ALL, local_in_lif,
                         &(vlan_port.vlan_port_id)));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    /*
     * Control LIF:
     * Skip symmetric LIF causes skipping also Control LIF, since it does not have GLOB_IN_LIF field.
     * Thus, need to traverse Ingress non-native control LIF only!
     */
    if (is_symmetric == FALSE)
    {
        uint32 fodo_assignment_mode;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_IN_AC_INFO_DB, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /*
         * Set iterator rules:
         * All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_port_default_init):
         *  - drop LIF
         *  - bridge LIF
         *  - recycle LIF
         */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &drop_in_lif, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &initial_bridge_in_lif,
                         NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &recycle_default_inlif,
                         NULL));

        /*
         *   Traverse Ingress non-native control LIF only!
         */
        fodo_assignment_mode = DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LOOKUP;

        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         &fodo_assignment_mode, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            uint32 local_in_lif;
            bcm_vlan_port_t vlan_port;

            bcm_vlan_port_t_init(&vlan_port);

            /*
             * Get local In-LIF -- table key
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

            /*
             * Get gport from In-LIF
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS, DBAL_CORE_ALL, local_in_lif,
                             &(vlan_port.vlan_port_id)));

            /*
             * Get vlan_port struct
             */
            SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

            /*
             * Run callback function
             */
            SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -
* Traversing Ingress native virtual objects. \n
* \see
*   dnx_vlan_port_traverse_ingress
*/
static shr_error_e
dnx_vlan_port_traverse_ingress_native_virtual(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 native_default_handle_id;
    uint32 vtt5_default_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse Ingress native virtual.
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Get lif value from virtual register of vtt5 native default 0 tags
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &native_default_handle_id));
    dbal_value_field32_request(unit, native_default_handle_id, DBAL_FIELD_VTT5___NATIVE_ETHERNET___0_TAGS_DEFAULT_LIF,
                               INST_SINGLE, &vtt5_default_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, native_default_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Virtual native
         * The local in-lif is "virtual", look for it in the SW DB
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state(unit, (int) local_in_lif,
                                                                                 &(vlan_port.vlan_port_id)));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief -
* Traversing Ingress native non-virtual objects. \n
* \see
*   dnx_vlan_port_traverse_ingress
*/
static shr_error_e
dnx_vlan_port_traverse_ingress_native(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 vtt5_default_trap_in_lif, vtt5_default_0_tag_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get lif value from virtual register of vtt5 native default trap
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VTT5___NATIVE_ETHERNET___TRAP_DEFAULT_LIF, INST_SINGLE,
                               &vtt5_default_trap_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get lif value from virtual register of vtt5 native default 0 tags
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VTT5___NATIVE_ETHERNET___0_TAGS_DEFAULT_LIF,
                               INST_SINGLE, &vtt5_default_0_tag_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Traverse Ingress native non-virtual.
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_native_vpls_service_tagged_miss_init):
     *  - PWE tagged native miss lif
     *  - 0 tags native miss lif
     */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_trap_in_lif,
                     NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_0_tag_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Non-virtual native
         * Get gport from In-LIF
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, DBAL_CORE_ALL, local_in_lif,
                         &(vlan_port.vlan_port_id)));

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Ingress objects, non-native and native, virtual and non-virtual, \n
 * based on flags configuration:
 *  0 - all ingress objects \n
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY: \n
 *      - Traverse Ingress non-native objects only \n
  *      - Note: symmetric LIF objects will be skipped.
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY + BCM_VLAN_PORT_NATIVE: \n
 *      - Traverse Ingress native objects only \n
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY + BCM_VLAN_PORT_NATIVE + BCM_VLAN_PORT_VLAN_TRANSLATION: \n
 *      - Traverse Ingress native virtual objects only \n
 *
 * \remark
 *
 * \see
 *   bcm_dnx_vlan_port_traverse
 */
static shr_error_e
dnx_vlan_port_traverse_ingress(
    int unit,
    uint32 flags,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    int is_all, is_native, is_virtual;

    SHR_FUNC_INIT_VARS(unit);

    is_all = (flags == 0) ? TRUE : FALSE;
    is_native = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_NATIVE);
    is_virtual = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION);

    /*
     * Traverse Ingress non-native objects
     *      Note: symmetric LIFs will be skipped based on "is_symmetric" indication.
     */
    if ((is_all == TRUE) || (is_native == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_ingress_non_native(unit, is_all, trav_fn, user_data));
    }

    /*
     * Traverse Ingress native non-virtual objects
     */
    if ((is_all == TRUE) || (is_native == TRUE && is_virtual == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_ingress_native(unit, trav_fn, user_data));
    }

    /*
     * Traverse Ingress native virtual objects
     */
    if ((is_all == TRUE) || (is_native == TRUE && is_virtual == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_ingress_native_virtual(unit, trav_fn, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress virtual (ESEM) non-native objects. \n
 * \see
 *   dnx_vlan_port_traverse_egress
 */
static shr_error_e
dnx_vlan_port_traverse_egress_virtual_esem_non_native(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 l3_fodo_resultType;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse egress virtual:
     *  Virtual (ESEM) non native
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all esem out-lifs) that are not created by default
     * and result type is not "L3-FODO"
     */
    l3_fodo_resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_NOT_EQUAL_TO, &l3_fodo_resultType, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 resultType;

        /*
         * The entry can be AC, L2 FODO or L3 FODO, get result type to identify it:
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &resultType));

        if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO)
        {
            /*
             * This entry is not a VLAN-PORT, it is L3-FODO! DBAL iterator should have skip it!!!
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!!! resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L3_FODO! It should have been skipped!\n");
        }
        else
        {
            uint32 vlan_domain, vsi;
            uint8 skip_callback = FALSE;

            bcm_vlan_port_t vlan_port;

            bcm_vlan_port_t_init(&vlan_port);

            /*
             * Get table keys
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &vlan_domain));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_FODO, &vsi));

            if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT ||
                resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC)
            {
                uint32 match_vlan;
                uint32 egress_last_layer = TRUE;

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_C_VID, &match_vlan));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &egress_last_layer));
                /*
                 * Find the gport
                 */
                if (egress_last_layer)
                {
                    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                    vlan_port.vsi = vsi;
                    vlan_port.match_vlan = match_vlan;
                    SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_match_esem_sw_state(unit, &vlan_port, TRUE, vlan_domain));
                }
                else
                {
                    skip_callback = TRUE;
                }

            }
            else if (BCM_DNX_ESEM_IS_RESULT_TYPE_NAMESPACE_VSI(resultType))
            {
                /*
                 * Find the gport
                 * Note: the ESEM Namespace-vsi entry may be created by bcm_vxlan_network_domain_config_add -
                 * If bcm_vlan_port_create was not called - need to skip this entry!
                 */
                shr_error_e rv;
                vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
                vlan_port.vsi = vsi;
                vlan_port.match_class_id = vlan_domain;
                rv = dnx_vlan_port_find_egress_match_esem_sw_state(unit, &vlan_port, FALSE, 0);

                /** The gport was not found, skip it! */
                if (rv == _SHR_E_NOT_FOUND)
                {
                    skip_callback = TRUE;
                }

                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            }

            if (skip_callback == FALSE)
            {
                /*
                 * Get vlan_port struct
                 */
                SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

                /*
                 * Run callback function
                 */
                SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress virtual (ESEM) native objects. \n
 * \see
 *   dnx_vlan_port_traverse_egress
 */
static shr_error_e
dnx_vlan_port_traverse_egress_virtual_esem_native(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse egress virtual:
     *  Virtual (ESEM) native key: {vsi, name_space} or {vsi, outlif}
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    result_type = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &result_type, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        bcm_vlan_port_t vlan_port;
        uint32 vlan_port_id;

        bcm_vlan_port_t_init(&vlan_port);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GPORT, &vlan_port_id));

        /*
         * Find the gport
         */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = BCM_VLAN_PORT_NATIVE;
        vlan_port.vlan_port_id = vlan_port_id;
        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress virtual namespace-port objects. \n
 * \see
 *   dnx_vlan_port_traverse_egress
 */
static shr_error_e
dnx_vlan_port_traverse_egress_virtual_esem_namespace_port(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 dual_homing_resultType;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse egress virtual:
     *  Virtual namespace-port (ESEM).
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all esem out-lifs) that are not created by default
     * and result-type is not "DUAL-HOMING"
     */
    dual_homing_resultType = DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_NOT_EQUAL_TO, &dual_homing_resultType, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 system_port, match_class_id;
        bcm_vlan_port_t vlan_port;
        int is_spa;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get table keys
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &match_class_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &system_port));

        /*
         * The system_port key can be physical system port or trunk spa
         */
        SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, system_port, &is_spa));

        if (is_spa == FALSE)
        {
            /*
             * Find the gport this physical system port.
             */
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
            vlan_port.match_class_id = match_class_id;
            BCM_GPORT_SYSTEM_PORT_ID_SET(vlan_port.port, system_port);
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_match_esem_sw_state(unit, &vlan_port, FALSE, 0));
        }
        else
        {
            int member;
            bcm_gport_t trunk_gport;

            /*
             * Get the member for this trunk spa.
             */
            SHR_IF_ERR_EXIT(dnx_trunk_spa_to_gport(unit, system_port, &member, &trunk_gport));

            if (member)
            {
                /*
                 * For LAG, call the user's cb, only for member 0
                 */
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }

            /*
             * Find the gport this trunk spa.
             */
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
            vlan_port.match_class_id = match_class_id;
            vlan_port.port = trunk_gport;
            SHR_IF_ERR_EXIT(dnx_vlan_port_find_egress_match_esem_sw_state(unit, &vlan_port, FALSE, 0));

        }

        /*
         * Get vlan_port struct
         */
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress virtual default (ESEM-default) objects. \n
 * \see
 *   dnx_vlan_port_traverse_egress
 */
static shr_error_e
dnx_vlan_port_traverse_egress_virtual_esem_default(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 esem_handle_nof_profiles;
    uint32 dual_homing_resultType;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse egress virtual:
     *  Virtual default (ESEM-Default) native and non-native
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all esem default out-lifs) that are not created by default
     * (see dnx_tune_switch_default_ac_init and dnx_mpls_init_dual_homing_default_result)
     * and result-type is not "DUAL-HOMING"
     */
    esem_handle_nof_profiles = dnx_data_esem.default_result_profile.nof_allocable_profiles_get(unit);
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, DBAL_CONDITION_LOWER_THAN,
                     &esem_handle_nof_profiles, NULL));

    dual_homing_resultType = DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_DUAL_HOMING;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_NOT_EQUAL_TO, &dual_homing_resultType, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 esem_default_result_profile;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE, &esem_default_result_profile));

        /*
         * Get vlan_port struct
         */
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(vlan_port.vlan_port_id, esem_default_result_profile);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

        /*
         * Run callback function
         */
        SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress non-virtual (EEDB) objects. \n
 * \see
 *   bcm_dnx_vlan_port_traverse_egress
 */
static shr_error_e
dnx_vlan_port_traverse_egress_non_virtual(
    int unit,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse Egress EEDB native and non-native.
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OUT_AC, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * All the entries (that is all out-lifs) that are not created by default
     */

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_out_lif;
        bcm_vlan_port_t vlan_port;

        bcm_vlan_port_t_init(&vlan_port);

        /*
         * Get local Out-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif));

        /*
         * How to know if the out-ac is symmetric LIF or not?
         * Perform "gport from LIF"
         * If the gport "egress only" bits are not set, it is symmetric and need to "skip" it.
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, local_out_lif, &vlan_port.vlan_port_id));

        if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(vlan_port.vlan_port_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
        {
            /*
             * This entry is Engress-only.
             * Get vlan_port struct
             */
            SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));

            /*
             * Run callback function
             */
            SHR_IF_ERR_EXIT(trav_fn(unit, &vlan_port, user_data));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing Egress objects, non-native and native, virtual and non-virtual, \n
 * based on flags configuration:
 * 0 - all egress asymmetric objects \n
 * BCM_VLAN_PORT_CREATE_EGRESS_ONLY: \n
 *      - Traverse Egress non virtual (EEDB) objects only \n
 *      - Note: symmetric LIF objects will be skipped.
 * BCM_VLAN_PORT_CREATE_EGRESS_ONLY + BCM_VLAN_PORT_VLAN_TRANSLATION: \n
 *      - Traverse Egress Virtual (ESEM) objects only \n
 * BCM_VLAN_PORT_CREATE_EGRESS_ONLY + BCM_VLAN_PORT_VLAN_TRANSLATION + BCM_VLAN_PORT_DEFAULT: \n
 *      - Traverse Egress Virtual default (ESEM-default) objects only \n
 * \see
 *   bcm_dnx_vlan_port_traverse
 */
static shr_error_e
dnx_vlan_port_traverse_egress(
    int unit,
    uint32 flags,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    int is_all, is_virtual, is_default;

    SHR_FUNC_INIT_VARS(unit);

    is_all = (flags == 0) ? TRUE : FALSE;
    is_virtual = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_VLAN_TRANSLATION);
    is_default = _SHR_IS_FLAG_SET(flags, BCM_VLAN_PORT_DEFAULT);

    /*
     * Traverse Egress non virtual (EEDB) objects only
     *      Note: symmetric LIF objects will be skipped.
     */
    if ((is_all == TRUE) || (is_virtual == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress_non_virtual(unit, trav_fn, user_data));
    }

    /*
     * Traverse Egress Virtual (ESEM) objects only
     */
    if ((is_all == TRUE) || (is_virtual == TRUE && is_default == FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress_virtual_esem_non_native(unit, trav_fn, user_data));
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress_virtual_esem_native(unit, trav_fn, user_data));
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress_virtual_esem_namespace_port(unit, trav_fn, user_data));
    }

    /*
     * Traverse Egress Virtual default (ESEM-default) objects only
     */
    if ((is_all == TRUE) || (is_virtual == TRUE && is_default == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress_virtual_esem_default(unit, trav_fn, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Traversing VLAN PORT objects, according to given rules. \n
 * Each returned object is called with the given trav_fn, the traverse callback. \n
 *
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] additional_info - Additional info which can translate to traverse rules \n
 *   flags: \n
 *  0 - all objects \n
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY: \n
 *      - Traverse Ingress non-native objects only \n
  *      - Note: symmetric LIF objects will be skipped.
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY + BCM_VLAN_PORT_NATIVE: \n
 *      - Traverse Ingress native objects only \n
 *  BCM_VLAN_PORT_CREATE_INGRESS_ONLY + BCM_VLAN_PORT_NATIVE + BCM_VLAN_PORT_VLAN_TRANSLATION: \n
 *      - Traverse Ingress native virtual objects only \n
 *  BCM_VLAN_PORT_CREATE_EGRESS_ONLY: \n
 *      - Traverse Egress non virtual (EEDB) objects only \n
 *      - Note: symmetric LIF objects will be skipped.
 *  BCM_VLAN_PORT_CREATE_EGRESS_ONLY + BCM_VLAN_PORT_VLAN_TRANSLATION: \n
 *      - Traverse Egress Virtual (ESEM) objects only \n
 *  BCM_VLAN_PORT_CREATE_EGRESS_ONLY + BCM_VLAN_PORT_VLAN_TRANSLATION + BCM_VLAN_PORT_DEFAULT: \n
 *      - Traverse Egress Virtual default (ESEM-default) objects only \n
 * \param [in] trav_fn - The function to call with every object that is returned \n
 * \param [in] user_data - additional data to pass to the trav_fn, along with the returned object \n
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   This function skips VLAN PORT object that are created by SDK init.
 * \see
 *   None
 */
shr_error_e
bcm_dnx_vlan_port_traverse(
    int unit,
    bcm_vlan_port_traverse_info_t * additional_info,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_traverse_verify(unit, additional_info, trav_fn));

    if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_INGRESS_ONLY))
    {
        /*
         * Traverse only Ingress objects
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_ingress(unit, additional_info->flags, trav_fn, user_data));
    }
    else if (_SHR_IS_FLAG_SET(additional_info->flags, BCM_VLAN_PORT_CREATE_EGRESS_ONLY))
    {
        /*
         * Traverse only Egress objects
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress(unit, additional_info->flags, trav_fn, user_data));
    }
    else
    {
        /*
         * Traverse ALL objects (Ingress and Egress)
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_ingress(unit, additional_info->flags, trav_fn, user_data));
        SHR_IF_ERR_EXIT(dnx_vlan_port_traverse_egress(unit, additional_info->flags, trav_fn, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See vlan.h
 */
shr_error_e
dnx_vlan_port_match_inlif_set(
    int unit,
    bcm_vlan_port_t * vlan_port,
    uint32 local_in_lif_id,
    int is_local_in_lif_update)
{
    int is_native_ac = _SHR_IS_FLAG_SET(vlan_port->flags, BCM_VLAN_PORT_NATIVE);
    SHR_FUNC_INIT_VARS(unit);

    switch (vlan_port->criteria)
    {
        case BCM_VLAN_PORT_MATCH_NONE:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT:
        {
            /*
             * Ingress default action
             * Note: no need to check if Native because match port is not supported for Ingress Native.
             * see dnx_vlan_port_create_verify.
             */
            if (is_native_ac == FALSE)
            {
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, local_in_lif_id,
                                     is_local_in_lif_update));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_default
                                    (unit, vlan_port->port, local_in_lif_id));
                }
            }
            else
            {
                /*
                 * Ingress Native. Only applicable for port-match configuration.
                 * Currently this match can't be configured directly from the
                 * VLAN-Port API, but can be added by a Port-Match add API that
                 * uses this general VLAN-Port match setting function.
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_native_match_port
                                (unit, vlan_port->port, local_in_lif_id, is_local_in_lif_update));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN:
        {
            /*
             * Four configuration modes:
             * 1. Symmetric (InLif + OutLif EEDB)
             * 2. Ingress only (InLif)
             * 3. Egress only (ESEM)
             * 4. Egress only (OutLif)
             */
            if (is_native_ac == FALSE)
            {
                /*
                 * Case 1/2 above: Symmetric/Ingress only
                 */
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, local_in_lif_id,
                                     is_local_in_lif_update));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, local_in_lif_id,
                                     is_local_in_lif_update));
                }
            }
            else
            {
                /*
                 * Ingress Native:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_native_match_port_vlan
                                (unit, vlan_port->port, vlan_port->match_vlan, local_in_lif_id,
                                 is_local_in_lif_update));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN:
        {
            /*
             * Case 1/2 above: Symmetric/Ingress only
             */
            if (vlan_port->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, local_in_lif_id,
                                 is_local_in_lif_update));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_c_vlan
                                (unit, vlan_port->port, vlan_port->match_vlan, local_in_lif_id,
                                 is_local_in_lif_update));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            if (is_native_ac == FALSE)
            {
                /*
                 * case 1/2: Symmetric/Ingress only
                 */
                if (vlan_port->match_ethertype)
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                    (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                     vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, local_in_lif_id,
                                     is_local_in_lif_update));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan
                                    (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                     local_in_lif_id, is_local_in_lif_update));
                }
            }
            else
            {
                /*
                 * Ingress Native:
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_native_match_port_vlan_vlan
                                (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                                 local_in_lif_id, is_local_in_lif_update));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            /*
             * case 1/2: Symmetric/Ingress only
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_cvlan_cvlan
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                             local_in_lif_id, is_local_in_lif_update));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            /*
             * case 1/2: Symmetric/Ingress only
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_svlan_svlan
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                             local_in_lif_id, is_local_in_lif_update));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            /*
             * case 1/2: Symmetric/Ingress only
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                            (unit, vlan_port->port, vlan_port->match_vlan, BCM_VLAN_INVALID,
                             vlan_port->match_ethertype, vlan_port->match_pcp, local_in_lif_id,
                             is_local_in_lif_update));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED:
        {
            /*
             * Three configuration modes: 1. Symmetric (InLif + Outlif EEDB) 2. Ingress only (InLif) 3. Egress only
             * (Outlif)
             */
            /*
             * case 1/2: Symmetric/Ingress only
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                            (unit, vlan_port->port, vlan_port->match_vlan, vlan_port->match_inner_vlan,
                             vlan_port->match_ethertype, vlan_port->match_pcp, local_in_lif_id,
                             is_local_in_lif_update));
            break;
        }
        case BCM_VLAN_PORT_MATCH_NAMESPACE_VSI:
        case BCM_VLAN_PORT_MATCH_NAMESPACE_PORT:
        {
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_UNTAGGED:
        {
            /*
             * Symmetric/Ingress only:
             */
            if (vlan_port->match_ethertype)
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_vlan_vlan_pcp_ethertype
                                (unit, vlan_port->port, BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                 vlan_port->match_ethertype, BCM_DNX_PCP_INVALID, local_in_lif_id,
                                 is_local_in_lif_update));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_untagged
                                (unit, vlan_port->port, local_in_lif_id, is_local_in_lif_update));
            }
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_unmatched
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_svlan
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, vlan_port->match_vlan,
                             local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_cvlan
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, vlan_port->match_vlan,
                             local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_svlan_cvlan
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan, local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_CVLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_cvlan_cvlan
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan, local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_SVLAN_STACKED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_svlan_svlan
                            (unit, vlan_port->port, vlan_port->match_tunnel_value,
                             vlan_port->match_vlan, vlan_port->match_inner_vlan, local_in_lif_id));
            break;
        }
        case BCM_VLAN_PORT_MATCH_PORT_TUNNEL_UNTAGGED:
        {
            SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_match_port_tunnel_untagged
                            (unit, vlan_port->port, vlan_port->match_tunnel_value, local_in_lif_id));
            break;
        }
        default:
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Support a modification of local In-LIF ID by all
 *  relevant VLAN-Port match HW tables as part of a REPLACE
 *  operation. The function should handle all the ISEM and
 *  PP-Port entries that were created by the VLAN-Port APIs and
 *  point to the specified In-LIF ID that is to be modified.
 *  Note: Entries that are created by Port-Match APIs are
 *  handled separately.
 *
 * \param [in] unit - unit ID.
 * \param [in] inlif_dbal_table_id - The DBAL table ID of the
 *        modified In-LIF.
 * \param [in] old_local_in_lif_id - The local In-LIF ID to be
 *        modified.
 * \param [in] new_local_in_lif_id - The new local In-LIF ID
 *        that needs to be set
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_update_isem_and_port_pointers
 */
shr_error_e
dnx_vlan_port_match_inlif_update(
    int unit,
    dbal_tables_e inlif_dbal_table_id,
    uint32 old_local_in_lif_id,
    uint32 new_local_in_lif_id)
{
    bcm_vlan_port_t vlan_port;
    uint32 algo_flags;
    int is_virtual;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vlan_port);

    SHR_IF_ERR_EXIT(dnx_vlan_port_create_ingress_lif_match_info_get
                    (unit, inlif_dbal_table_id, old_local_in_lif_id, &vlan_port));

    /*
     * Set the Native indication that is required
     */
    if ((inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION) ||
        (inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT))
    {
        vlan_port.flags |= BCM_VLAN_PORT_NATIVE;

        if ((inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT))
        {
            vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        }
    }

    SHR_IF_ERR_EXIT(dnx_vlan_port_match_inlif_set(unit, &vlan_port, new_local_in_lif_id, TRUE));

    /*
     * Handle the ISEM and Port entries that were added using the port_match APIs.
     */
    algo_flags = (_SHR_IS_FLAG_SET(vlan_port.flags, BCM_VLAN_PORT_NATIVE)) ?
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS : DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS;
    is_virtual = (inlif_dbal_table_id == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT);
    SHR_IF_ERR_EXIT(dnx_port_match_inlif_update
                    (unit, algo_flags, is_virtual, old_local_in_lif_id, new_local_in_lif_id));

exit:
    SHR_FUNC_EXIT;
}
