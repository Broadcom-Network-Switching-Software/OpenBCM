/** \file srv6.c
 * 
 *
 * General SRv6 functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6
/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/srv6/srv6.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <include/bcm/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>

/** SW state access of the SRv6 */
#include <soc/dnx/swstate/auto_generated/access/srv6_access.h>

/** DNX data */
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

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
 * DEFINES
 * {
 */
/**
 * \brief - SRv6 Min packet size.
 * Minimum packet size in bytes to perform the SID swap
 * mechanism
 * The size are calculated as:
 * ETH0+IPv6+SRH_BASE+SID = 14+36+8+16 = 74Bytes
 */
#define SRV6_MIN_PACKET_SIZE                        (74)

/**
 * \brief - SRv6 factor between Classical (128b) and Unified (32b) SID
 */
#define SRV6_UNIFIED_CLASSICAL_SID_FACTOR           (4)

/**
 * \brief - SRv6 marking packet as Unified SID in SRH Flags by 2LSB bits as 0b10
 */
#define SRV6_SRH_FLAGS_UNIFIED_MARKING              (2)

/*
 * }
 */

/*
 * see .h file for description
 */
shr_error_e
dnx_srv6_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** the configuration below allow access to SIDs above the
     *  144 MSBs of packets. In J2P, the feature ECOLogic is
     *  responsible for accessing it. */
    if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {

        /** Global configuration init */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_GLOBAL_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_EXTENSION_HEADER_TYPE, INST_SINGLE,
                                     DBAL_DEFINE_IPV6_EXTENSION_HEADER_TYPE_SRV6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ETHERTYPE, INST_SINGLE,
                                     DBAL_DEFINE_ETHERTYPE_IPV6);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_MIN_PACKET_SIZE, INST_SINGLE,
                                     SRV6_MIN_PACKET_SIZE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Per PTC configuration */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PTC_PORT, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_SID_SWAP_ENABLE, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    /** init the SRv6 sw state structure */
    SHR_IF_ERR_EXIT(srv6_modes.init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for sid_initiator traverse API
 */
static shr_error_e
dnx_srv6_sid_initiator_traverse_verify(
    int unit,
    bcm_srv6_sid_initiator_traverse_info_t additional_info,
    bcm_srv6_sid_initiator_traverse_cb cb)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check that the callback in non NULL */
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb function");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for sid_initiator get/delete APIs
 *
 *   \param [in] unit
 *   \param [in] info - the structure which includes the RCH Port and RCH Encap_id
 *          [in] info.flags - BCM_SRV6_SID_INITIATOR_XXX flags
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *          [in] info.sid -  SID to encapsulate
 *          [in] info.encap_access - Encapsulation Access stage
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_general_verify(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** check that the structure in non NULL */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** check that the tunnel_id is none zero, cause used as key for get and delete APIs */
    if (info->tunnel_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel_id cannot be 0 for get and delete APIs !");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for sid_initiator create API
 *
 *   \param [in] unit
 *   \param [in] info - the structure which includes the RCH Port and RCH Encap_id
 *          [in] info.flags - BCM_SRV6_SID_INITIATOR_XXX flags
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *          [in] info.sid -  SID to encapsulate
 *          [in] info.encap_access - Encapsulation Access stage
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_create_verify(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** check that the structure in non NULL */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** check that if tunnle_id is given, it's a GPORT Tunnel and flags must be set */
    if (info->tunnel_id != 0)
    {
        if (!(_SHR_GPORT_IS_TUNNEL(info->tunnel_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id (0x%x) is not a GPORT Tunnel or 0", info->tunnel_id);
        }

        if (!(_SHR_IS_FLAG_SET(info->flags, (BCM_SRV6_SID_INITIATOR_REPLACE | BCM_SRV6_SID_INITIATOR_WITH_ID))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id is non 0, must include Replace or With_id flags (0x%x)", info->flags);
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(info->flags, (BCM_SRV6_SID_INITIATOR_REPLACE | BCM_SRV6_SID_INITIATOR_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id is 0, can not include Replace or With_id flags (0x%x)", info->flags);
        }
    }

    /** check that ITF is of LIF type */
    if (info->next_encap_id != 0)
    {
        if (!(BCM_L3_ITF_TYPE_IS_LIF(info->next_encap_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "next_encap_id (0x%x) must be 0 or ITF or LIF type", info->next_encap_id);
        }
    }

    /** Verify WITH_ID and REPLACE flags */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE)
        && !_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_SRV6_SID_INITIATOR_REPLACE can't be used without BCM_SRV6_SID_INITIATOR_WITH_ID");
    }

    /** Verify that REPLACE can EEDB entries of same type (With Global-LIF / EEDB Pointed Only) */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE))
    {
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(info->tunnel_id) &&
            !_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "EGRESS_POINTED gport can be only replaced WITH SET flag BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED");
        }

        if (!BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(info->tunnel_id) &&
            _SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "non EGRESS_POINTED gport can be only replaced WITHOUT flag BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED");
        }
    }

    /** check encap_access flags */
    if (((info->encap_access < bcmEncapAccessTunnel1) || (info->encap_access > bcmEncapAccessTunnel4)) &&
        (info->encap_access != bcmEncapAccessNativeArp) && (info->encap_access != bcmEncapAccessArp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "encap_access (%d) must be one of bcmEncapAccessTunnel[1..4], bcmEncapAccessArp or bcmEncapAccessNativeArp\n",
                     info->encap_access);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for srh_base_initiator traverse API
 */
static shr_error_e
dnx_srv6_srh_base_initiator_traverse_verify(
    int unit,
    bcm_srv6_srh_base_initiator_traverse_info_t additional_info,
    bcm_srv6_srh_base_initiator_traverse_cb cb)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check that the callback in non NULL */
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb function");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for sid_initiator get/delete APIs
 *
 *   \param [in] unit
 *   \param [in] info - Structure that is used in SRv6 SRH Base initiator APIs,
 *                      for managing device behavior at SRv6 Tunnel Encapsulation
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID (GPORT)
 *          [in] info.nof_sids -  number of SIDs in the SID list
 *          [in] info.qos_map_id - QOS map identifier
 *          [in] info.ttl - Tunnel header TTL
 *          [in] info.dscp - Tunnel header DSCP value
 *          [in] info.egress_qos_model - Egress QOS and TTL model
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t)
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_general_verify(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** check that the structure in non NULL */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** check that the statistics flag is not set, it's not yet supported */
    if ((_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_STAT_ENABLE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SRH Base statistics is not relevant, since statistics are available by default");
    }

    /** check that the tunnel_id is none zero, cause used as key for get and delete APIs */
    if (info->tunnel_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tunnel_id cannot be 0 for get and delete APIs !");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for sid_initiator create API
 *
 *   \param [in] unit
 *   \param [in] info - Structure that is used in SRv6 SRH Base initiator APIs,
 *                      for managing device behavior at SRv6 Tunnel Encapsulation
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID (GPORT)
 *          [in] info.nof_sids -  number of SIDs in the SID list
 *          [in] info.qos_map_id - QOS map identifier
 *          [in] info.ttl - Tunnel header TTL
 *          [in] info.dscp - Tunnel header DSCP value
 *          [in] info.egress_qos_model - Egress QOS and TTL model
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t)
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_create_verify(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    /** check that the structure in non NULL */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** check that if tunnle_id is given, it's a GPORT Tunnel and flags must be set */
    if (info->tunnel_id != 0)
    {
        if (!(_SHR_GPORT_IS_TUNNEL(info->tunnel_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id (0x%x) is not a GPORT Tunnel or 0", info->tunnel_id);
        }

        if (!
            (_SHR_IS_FLAG_SET
             (info->flags, (BCM_SRV6_SRH_BASE_INITIATOR_REPLACE | BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id is non 0, must include Replace or With_id flags (0x%x)", info->flags);
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(info->flags, (BCM_SRV6_SRH_BASE_INITIATOR_REPLACE | BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel_id is 0, can not include Replace or With_id flags (0x%x)", info->flags);
        }
    }

    /** check that ITF is of LIF type */
    if (info->next_encap_id != 0)
    {
        if (!(BCM_L3_ITF_TYPE_IS_LIF(info->next_encap_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "next_encap_id (0x%x) must be 0 or ITF of LIF type", info->next_encap_id);
        }
    }

    /** Verify WITH_ID and REPLACE flags */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_REPLACE)
        && !_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_SRV6_SRH_BASE_INITIATOR_REPLACE can't be used without BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID");
    }

    /** check that nof_sids can be 1 to max encap support and  in case of Classic/uSID  */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP) &&
        ((info->nof_sids < 1)
         || (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED) &&
             (info->nof_sids > dnx_data_srv6.encapsulation.max_nof_encap_sids_main_pass_get(unit) + 1))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of SIDs (%d) needs to be 1 to %d", info->nof_sids,
                     dnx_data_srv6.encapsulation.max_nof_encap_sids_main_pass_get(unit) + 1);
    }

    /** check that nof_sids can be < max encap support (x4) in case of Unified-SID */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP) &&
        ((info->nof_sids < 1)
         || (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED) &&
             (info->nof_sids >
              SRV6_UNIFIED_CLASSICAL_SID_FACTOR * (dnx_data_srv6.encapsulation.max_nof_encap_sids_main_pass_get(unit) +
                                                   1)))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of SIDs (%d) needs to be 1 to %d", info->nof_sids,
                     SRV6_UNIFIED_CLASSICAL_SID_FACTOR *
                     (dnx_data_srv6.encapsulation.max_nof_encap_sids_main_pass_get(unit) + 1));
    }

    /** check that nof_additional_sids is in valid range in case of Classic/uSID */
    if ((info->nof_additional_sids_extended_encap != 0)
        && (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED)
            && (info->nof_additional_sids_extended_encap !=
                dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of additional SIDs (%d) is 0 or %d",
                     info->nof_additional_sids_extended_encap,
                     dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit));
    }

    /** check that nof_additional_sids is in valid range in case of Unified-SID */
    if ((info->nof_additional_sids_extended_encap != 0)
        && (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED)
            && (info->nof_additional_sids_extended_encap != SRV6_UNIFIED_CLASSICAL_SID_FACTOR *
                dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of additional SIDs (%d) is 0 or %d",
                     info->nof_additional_sids_extended_encap, SRV6_UNIFIED_CLASSICAL_SID_FACTOR *
                     dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit));
    }

    /** check that nof_sids is in valid range for extended pass in case of Classic/uSID  */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP) &&
        ((info->nof_sids != 0)
         && (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED) &&
             (info->nof_sids != dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit)))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of SIDs (%d) needs to be 0 or %d", info->nof_sids,
                     dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit));
    }

    /** check that nof_sids is in valid range for extended pass in case of Unified-SID */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP) &&
        ((info->nof_sids != 0)
         && (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED) &&
             (info->nof_sids != SRV6_UNIFIED_CLASSICAL_SID_FACTOR *
              dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit)))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of SIDs (%d) needs to be 0 or %d", info->nof_sids,
                     SRV6_UNIFIED_CLASSICAL_SID_FACTOR *
                     dnx_data_srv6.encapsulation.nof_encap_sids_extended_pass_get(unit));
    }

    /** check that extended pass is not set with additional_sids */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP) &&
        (info->nof_additional_sids_extended_encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of additional SIDs cannot be set to non-zero value"
                     " with the flag BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP");
    }

    /*
     * Verify TTL
     */

    if ((info->egress_qos_model.egress_ttl != bcmQosEgressModelUniform) &&
        (info->egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ttl model (%d) is not allowed. Available models are "
                     "bcmQosEgressModelPipeMyNameSpace and bcmQosEgressModelUniform",
                     info->egress_qos_model.egress_ttl);
    }

    if ((info->egress_qos_model.egress_ttl != bcmQosEgressModelPipeMyNameSpace) && (info->ttl != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "If ttl model is not bcmQosEgressModelPipeMyNameSpace, ttl should be 0");
    }
    if (!BCM_TTL_VALID(info->ttl))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ttl (%d) is not valid, valid range is 0-255", info->ttl);
    }

    /*
     * Verify QOS
     */

    /** Check QOS model is allowed */
    if ((info->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (info->egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace) &&
        (info->egress_qos_model.egress_qos != bcmQosEgressModelUniform))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "qos model (%d) is not allowed. Available models are PipeNextNameSpace, PipeMyNameSpace and Uniform.",
                     info->egress_qos_model.egress_qos);
    }

    /** If QOS mode is not pipe, dscp should be 0*/
    if ((info->egress_qos_model.egress_qos != bcmQosEgressModelPipeNextNameSpace) &&
        (info->egress_qos_model.egress_qos != bcmQosEgressModelPipeMyNameSpace) && (info->dscp != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "if QOS model is not pipe, dscp should be 0");
    }

    /*
     * Verify QOS map id
     */
    if ((info->qos_map_id != DNX_QOS_INITIAL_MAP_ID) && !DNX_QOS_MAP_IS_REMARK(info->qos_map_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "QOS profile(%d) is not remark profile", info->qos_map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The following function allocates TTL and QOS profiles. It writes the TTL and QOS values to HW if new
 * templates were allocated. And returns the created template profiles (ttl_pipe_profile
 * and nwk_qos_profile).
 *
 *   \param [in] unit
 *   \param [in] info - Structure that is used in SRv6 SRH Base initiator APIs,
 *                      for managing device behavior at SRv6 Tunnel Encapsulation
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *          [in] info.tunnel_id - (non relevant) Tunnel SRv6 SRH base object ID (GPORT)
 *          [in] info.nof_sids - (non relevant) number of SIDs in the SID list
 *          [in] info.qos_map_id - (non relevant) QOS map identifier
 *          [in] info.ttl - Tunnel header TTL
 *          [in] info.dscp - Tunnel header DSCP value
 *          [in] info.egress_qos_model - Egress QOS and TTL model
 *          [in] info.next_encap_id - (non relevant) Next pointer interface ID (bcm_if_t)
 *
 *   \param [out] ttl_pipe_profile - TTL pipe profile
 *   \param [out] nwk_qos_profile - QOS profile
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_qos_ttl_template_alloc_and_hw_set(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    uint32 *ttl_pipe_profile,
    uint32 *nwk_qos_profile)
{
    uint8 is_first_ttl_profile_ref;
    uint8 is_first_pipe_propag_profile_ref;
    uint32 alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "tunnel");
    SHR_NULL_CHECK(ttl_pipe_profile, _SHR_E_PARAM, "ttl_pipe_profile");
    SHR_NULL_CHECK(nwk_qos_profile, _SHR_E_PARAM, "nwk_qos_profile");

    is_first_ttl_profile_ref = 0;
    alloc_flags = 0;

    *nwk_qos_profile = 0;
    *ttl_pipe_profile = 0;

    /** Allocate TTL model */
    if (info->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
    {
        /** allocate TTL PIPE profile */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.allocate_single
                        (unit, alloc_flags, &info->ttl, NULL, (int *) ttl_pipe_profile, &is_first_ttl_profile_ref));

        /** Write to HW if new TTL profile was allocated */
        if (is_first_ttl_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, *ttl_pipe_profile, (uint32) info->ttl));
        }
    }

    /** QOS PIPE Model */
    if ((info->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace) ||
        (info->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace))
    {
        /** Allocate QOS profile. The API allocates profile using template manager and writes to HW */
        /** currently only JR mode is supported, in which current and next layer values are equal and next layer dp
         *  value is 0 */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_allocate
                        (unit, (uint8) info->dscp, (uint8) info->dscp, 0, (int *) nwk_qos_profile,
                         &is_first_pipe_propag_profile_ref));

        if (is_first_pipe_propag_profile_ref)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_set
                            (unit, *nwk_qos_profile, (uint8) info->dscp, (uint8) info->dscp, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate global and local LIFs for the given SID in EEDB
 *          -also add mapping to GLEM between the LIFs
 *          -support with_id flag in case user provides the
 *           global-LIF id
 *
 *   \param [in] unit
 *   \param [in] info - the structure which includes the RCH Port and RCH Encap_id
 *          [in] info.flags - BCM_SRV6_SID_INITIATOR_XXX flags
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *          [in] info.encap_access - Encapsulation Access stage
 *
 *   \param [out] global_lif - pointer to global_lif for the SID entry in EEDB which
 *                             is mapped to the local_lif
 *   \param [out] egr_pointed_id - pointer to egr_pointed_id for the SID entry in EEDB which
 *                             in case  BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED is used
 *                             (only EEDB entry is used without Global-LIF)
 *   \param [out] local_lif -  pointer to local_lif for the SID entry in EEDB which
 *                             is mapped to the global_lif
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_lif_allocate(
    int unit,
    bcm_srv6_sid_initiator_info_t * info,
    int *global_lif,
    dnx_egr_pointed_t * egr_pointed_id,
    int *local_lif)
{

    int temp_global_lif = 0;

    lif_mngr_local_outlif_info_t outlif_info = { 0 };

    uint32 lif_alloc_flags = 0;
    uint32 mapping_flags = 0;

    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** Setting Egress Pointer Id to be invalid for case the Global-LIF is created */
    *egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;

    /** in case user provided the global-LIF id himself */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_WITH_ID | BCM_SRV6_SID_INITIATOR_REPLACE))
    {
        lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;

        /** convert from the tunnel-id GPORT the global-LIF*/
        temp_global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    }

    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED))
    {
        lif_alloc_flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
    }

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE)))
    {
        /*
         * in the case that's it's not Replace, allocate the local and global LIFs
         */

        /** set phase according to encap_access be LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_[1..4] */
        if (info->encap_access == bcmEncapAccessNativeArp)
        {
            outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2;
        }
        else if (info->encap_access == bcmEncapAccessArp)
        {
            outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7;
        }
        else
        {
            outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1 + info->encap_access - bcmEncapAccessTunnel1;
        }
        outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
        outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID;
        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &temp_global_lif, NULL, &outlif_info));
        *local_lif = outlif_info.local_outlif;

        if (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED))
        {
            *global_lif = temp_global_lif;
        }
        else
        {
            uint32 egr_alloc_flags;
            int element;

            /** in case of EEDB entry only without Global-LIF set it to invalid value */
            *global_lif = DNX_ALGO_GPM_LIF_INVALID;

            /** in case user provided the global-LIF id himself */
            if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_WITH_ID))
            {

                /** Get from the GPORT 'tunnel_id' the 'egr_pointed' value. */
                *egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(info->tunnel_id);

                /*
                 * Allocate resource corresponding to id specified on input.
                 */
                element = *egr_pointed_id;
                egr_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
                SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.
                                egr_pointed_res_manager.allocate_single(unit, egr_alloc_flags, NULL, &element));
            }

            else
            {
                /*
                 * Allocate a new 'egr_pointed' object.
                 */
                egr_alloc_flags = 0;
                SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.
                                egr_pointed_res_manager.allocate_single(unit, egr_alloc_flags, NULL, &element));
                *egr_pointed_id = element;
            }

            /*
             * Create mapping between 'egr_pointed' and 'local_lif'
             */
            SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_add
                            (unit, mapping_flags, *egr_pointed_id, *local_lif));

        } /** of EEDB object only case */
    }
    else                                                                                                                                                                                                                                         /** Case of Replace */
    {
        /*
         * in case of Replace, skip global and local LIF allocations,
         * since they are present. just retrieve the local and global LIFs from tunnel-id GPORT
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                         &tunnel_gport_hw_resources));

        

        if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE))
        {
            int prev_encap_access;
            if (tunnel_gport_hw_resources.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2)
            {
                prev_encap_access = bcmEncapAccessNativeArp;
            }
            else if (tunnel_gport_hw_resources.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7)
            {
                prev_encap_access = bcmEncapAccessArp;
            }
            else
            {
                prev_encap_access =
                    tunnel_gport_hw_resources.outlif_phase + bcmEncapAccessTunnel1 - LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1;
            }
            if (info->encap_access != prev_encap_access)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "encap access cannot be updated when REPLACE is set. prev=%d, new=%d \n",
                             prev_encap_access, info->encap_access);
            }

        }

        /** check if using Global-LIF or EEDB Pointed Only object in Replace */
        if (!_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED))
        {
            /** return the local and global out LIFs from the GPORT */
            *local_lif = tunnel_gport_hw_resources.local_out_lif;

            *global_lif = temp_global_lif;
        }
        else                                                                                                                                                                                                                                         /** if using EEDB Pointed only and Replace */
        {
            uint32 mapping_flags = 0;
            dnx_egr_pointed_t local_egr_pointed_id;
            int egr_mapped_local_lif;

            /** get the Egress EEDB Pointed Only Id */
            local_egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(info->tunnel_id);
            /** get from the mapping of the Egress EEDB Pointed Id the local-LIF */
            SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_to_local_lif_get
                            (unit, mapping_flags, local_egr_pointed_id, &egr_mapped_local_lif));

            *local_lif = egr_mapped_local_lif;
            /** in case of EEDB entry only without Global-LIF set it to invalid value */
            *global_lif = DNX_ALGO_GPM_LIF_INVALID;

            *egr_pointed_id = local_egr_pointed_id;
        }

    } /** of if not Replace */

    /*
     * Validate that either Global-LIF of Egress Pointed Id are valid but not both
     */
    if ((*egr_pointed_id == DNX_ALGO_EGR_POINTED_INVALID) && (*global_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Both 'egr_pointed' and 'global_lif' are marked as 'invalid' - Probably internal sw error.");
    }
    else if ((*egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID) && (*global_lif != DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Both 'egr_pointed' and 'global_lif' are marked as 'valid' - Probably internal sw error.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate global and local LIFs for the SRH Base
 *          -also add mapping to GLEM between the LIFs
 *          -support with_id flag in case user provides the
 *           global-LIF id
 *
 *   \param [in] unit
 *   \param [in] info - Structure that is used in SRv6 SRH Base initiator APIs,
 *                      for managing device behavior at SRv6 Tunnel Encapsulation
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *                    BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID - if set user will provide tunnel_id
 *                    BCM_SRV6_SRH_BASE_INITIATOR_REPLACE - replace existing SRH EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID (GPORT)
 *
 *   \param [out] global_lif - pointer to global_lif for the SID entry in EEDB which
 *                             is mapped to the local_lif
 *   \param [out] local_lif -  pointer to local_lif for the SID entry in EEDB which
 *                             is mapped to the global_lif
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_lif_allocate(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    int *global_lif,
    int *local_lif)
{

    int temp_global_lif = 0;

    lif_mngr_local_outlif_info_t outlif_info = { 0 };

    uint32 lif_alloc_flags = 0;

    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /** in case user provided the global-LIF id himself */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID | BCM_SRV6_SRH_BASE_INITIATOR_REPLACE))
    {
        lif_alloc_flags = LIF_MNGR_GLOBAL_LIF_WITH_ID;

        /** convert from the tunnel-id GPORT the global-LIF*/
        temp_global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    }

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_REPLACE)))
    {
        /*
         * in the case that's it's not Replace, allocate the local and global LIFs
         */

        outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_SRV6_BASE;
        outlif_info.dbal_table_id = DBAL_TABLE_EEDB_SRV6;

        if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT))
        {
            outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6_T_INSERT;
        }
        else
        {
            outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6;
        }

        SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &temp_global_lif, NULL, &outlif_info));
        *local_lif = outlif_info.local_outlif;
        *global_lif = temp_global_lif;

    }
    else
    {
        /*
         * in case of Replace, skip global and local LIF allocations,
         * since they are present. just retrieve the local-LIF from tunnel-id GPORT
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, info->tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                         &tunnel_gport_hw_resources));

        /** compare current encapsulation if it's T_Insert and matched the T_Insert flag of replace, this mode cannot be changed*/
        if (((tunnel_gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6) &&
             _SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT)) ||
            ((tunnel_gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6_T_INSERT) &&
             !_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Replacing SRH base info, cannot change from Classic Encap to T_Insert and vice versa\n");
        }

        /** return the local and global out LIFs from the GPORT */
        *local_lif = tunnel_gport_hw_resources.local_out_lif;
        *global_lif = temp_global_lif;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the SID EEDB entry, as well as point to a next encap_id
 *
 *   \param [in] unit
 *   \param [in] info - the structure which includes the SID encapsulation info
 *          [in] info.sid -  SID to encapsulate
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *   \param [in] local_lif - pointer to local_lif for the SID entry in EEDB
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_eedb_entry_set(
    int unit,
    bcm_srv6_sid_initiator_info_t * info,
    int local_lif)
{

    uint32 entry_handle_id;

    bcm_gport_t next_encap_id_gport;

    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Add the SID EEDB entry and set next out-LIF pointer */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_SID, INST_SINGLE, info->sid);

    /** if next_encap_id is non zero, append it to SID */
    if (info->next_encap_id != 0)
    {

        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_encap_id_gport, info->next_encap_id);

        /** convert GPORT to local out-LIF */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, next_encap_id_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                         &tunnel_gport_hw_resources));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     tunnel_gport_hw_resources.local_out_lif);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configures the given SID's global and local LIFs as well as its EEDB entry
 *          may link the SID EEDB entry to an additional EEDB entry of SID or other objects
 *          (such as an IPv6 encapsulation) as linked list entries
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SID EEDB entry
 *          [in] info.flags - BCM_SRV6_SID_INITIATOR_XXX flags
 *                            BCM_SRV6_SID_INITIATOR_WITH_ID - specify own tunnel_id
 *                            BCM_SRV6_SID_INITIATOR_REPLACE - update SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *          [in] info.sid -  SID to encapsulate
 *          [in] info.encap_access - Encapsulation Access stage
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_sid_initiator_create(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{

    int local_lif = 0;
    int global_lif = 0;
    dnx_egr_pointed_t egr_pointed_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_srv6_sid_initiator_info_t), info);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_sid_initiator_create_verify(unit, info));

    /*
     * Allocate the global and local LIFs for the SID EEDB entry and add GLEM mapping
     * the global-LIF is taken/updated from/to the info->tunnel_id (GPORT)
     */

    SHR_IF_ERR_EXIT(dnx_srv6_sid_initiator_lif_allocate(unit, info, &global_lif, &egr_pointed_id, &local_lif));

    /*
     * configure the SID EEDB entry and
     * concatenate the next interface-id to the SID EEDB entry
     */
    SHR_IF_ERR_EXIT(dnx_srv6_sid_initiator_eedb_entry_set(unit, info, local_lif));

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE)) &&
        !(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED)))
    {
        /** add GLEM mapping between global and local LIFs, if not in Replace case (GLEM already done) */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif, local_lif));

        /** update and convert back the global_lif into the tunnel-id GPORT if not Replace */
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_lif);
    }
    else if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_REPLACE)) &&
             (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED)))
    {
        /** in this case, we don't have a Global-LIF but an Egress Pointed Id element - setting as tunnel_id */
        int dummy_gport;

        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(dummy_gport, egr_pointed_id);
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, dummy_gport);

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - low level function for freeing and removing all SID resources with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_tunnel_id_delete(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{

    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** check if using Global-LIF or EEDB Pointed Only object */
    if (!BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(info->tunnel_id))
    {
        /*
         * Remove Local-LIF mapping to Global-LIF
         */

        /** get GPort hw resources (global and local tunnel outlif) */
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->tunnel_id, lif_flags, &gport_hw_resources));

        /** delete global to local mapping from GLEM */
        SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources.global_out_lif));

        /*
         * Remove EEDB entry of the SID
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID);

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Delete global and local out lifs allocations
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_free
                        (unit, gport_hw_resources.global_out_lif, NULL, gport_hw_resources.local_out_lif));
    }
    else
    {
        /*
         * Remove Local-LIF mapping to Egress Pointed Only object
         */
        uint32 mapping_flags = 0;
        dnx_egr_pointed_t egr_pointed_id;
        int egr_mapped_local_lif;
        int element;

        /** get the id of the EEDB Pointed Only object */
        egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(info->tunnel_id);
        /** save the local-LIF from the mapping to the EEDB Pointed Only object */
        SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_to_local_lif_get
                        (unit, mapping_flags, egr_pointed_id, &egr_mapped_local_lif));
        /** remove the mapping between the local-LIF to the EEDB Pointed Only object */
        SHR_IF_ERR_EXIT(dnx_algo_l3_egr_pointed_local_lif_mapping_remove(unit, mapping_flags, egr_pointed_id));

        /*
         * Release (deallocate) this 'egr_pointed' object.
         */
        element = egr_pointed_id;
        SHR_IF_ERR_EXIT(algo_l3_db.egr_pointed_alloc.egr_pointed_res_manager.free_single(unit, element));

        /*
         * Remove EEDB entry of the SID
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, egr_mapped_local_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID);

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Delete Local out lif allocation
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, egr_mapped_local_lif));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_sid_initiator_delete(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    /** verification of supported features */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_sid_initiator_general_verify(unit, info));

    /** delete the SID tunnel_id and its resources */
    SHR_IF_ERR_EXIT(dnx_srv6_sid_initiator_tunnel_id_delete(unit, info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Low level function to get the SID entry information with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_sid_initiator_tunnel_id_param_get(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{

    uint32 entry_handle_id = 0;
    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    uint32 next_local_out_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** return the BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED flag if entry is EEDB Pointed Only */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(info->tunnel_id))
    {
        info->flags |= BCM_SRV6_SID_INITIATOR_VIRTUAL_EGRESS_POINTED;
    }

    /** convert GPORT Tunnel to get its local-LIF which serves as key to SRv6 EEDB table*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, info->tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                     &tunnel_gport_hw_resources));

    /** set the local-out-LIF as key and get the SID EEDB entry fields  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, tunnel_gport_hw_resources.local_out_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get the current SID address */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_SID,
                                                           INST_SINGLE, info->sid));

    /** get next local-out-LIF to an EEDB entry which is pointed by this SRH Base tunnel_id */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, &next_local_out_lif));

    /** convert the next local-out-LIF to L3 Int*/
    if (next_local_out_lif != 0)
    {
        bcm_gport_t next_encap_id_gport = BCM_GPORT_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    next_local_out_lif, &next_encap_id_gport));
        if (next_encap_id_gport != BCM_GPORT_INVALID)
        {
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(info->next_encap_id, next_encap_id_gport);
        }
        else
        {
            info->next_encap_id = 0;
        }
    }

    /** get the encap_access from the outlif_phase */
    if (tunnel_gport_hw_resources.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2)
    {
        info->encap_access = bcmEncapAccessNativeArp;
    }
    else if (tunnel_gport_hw_resources.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7)
    {
        info->encap_access = bcmEncapAccessArp;
    }
    else
    {
        info->encap_access =
            tunnel_gport_hw_resources.outlif_phase + bcmEncapAccessTunnel1 - LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets the SID entry information with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SID initiator object ID (GPORT)
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_sid_initiator_get(
    int unit,
    bcm_srv6_sid_initiator_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_sid_initiator_general_verify(unit, info));

    /*
     * Retrieve the SID tunnel_id parameters
     */
    SHR_IF_ERR_EXIT(dnx_srv6_sid_initiator_tunnel_id_param_get(unit, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function traverses all sid_initiator objects and runs a callback function provided by the user on this data, along
 *          with user_data.
 *
 *   \param [in] unit - device id
 *   \param [in] additional_info - structure for additional control over cb traverse
 *          [in] additional_info.flags - BCM_SRV6_SID_INITIATOR_XXX flags
 *          [in] additional_info.traverse_flags - BCM_SRV6_SID_INITIATOR_TRAVERSE_XXX flags
 *
 *   \param [in] user_data - user data to be used along with the call back function
 *   \param [in] cb - user call back function of the following structure:
 *
 *    typedef int (*bcm_srv6_sid_initiator_traverse_cb)(
 *            int unit,
 *            bcm_srv6_sid_initiator_info_t *info,
 *            void *user_data);
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_sid_initiator_traverse(
    int unit,
    bcm_srv6_sid_initiator_traverse_info_t additional_info,
    bcm_srv6_sid_initiator_traverse_cb cb,
    void *user_data)
{
    int is_end = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_sid_initiator_traverse_verify(unit, additional_info, cb));

    /*
     * Iterate the EEDB_SRV6 table, which holds the SRH Base information
     * Translate to srh base info and call the srh base get API
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE,
                                                         0, DBAL_CONDITION_EQUAL_TO,
                                                         DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_SRV6_SID, UTILEX_U32_MAX));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 local_out_lif[1];
        bcm_srv6_sid_initiator_info_t entry_info;

        bcm_srv6_sid_initiator_info_t_init(&entry_info);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, (int) local_out_lif[0], &entry_info.tunnel_id));
        SHR_IF_ERR_EXIT(bcm_dnx_srv6_sid_initiator_get(unit, &entry_info));
        SHR_IF_ERR_EXIT(cb(unit, &entry_info, user_data));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the SRH Base tunnel_id parameters
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SRH Base EEDB entry
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *                            BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID - specify own tunnel_id
 *                            BCM_SRV6_SRH_BASE_INITIATOR_REPLACE - update SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID
 *          [out] info.nof_sids - Number of SIDs in the SID list
 *          [out] info.qos_map_id - QOS map identifier
 *          [out] info.ttl -  Tunnel header TTL
 *          [out] info.dscp - Tunnel header DSCP value
 *          [out] info.egress_qos_model - Egress qos and ttl model
 *          [out] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *   \param [out] ttl_pipe_profile - TTL pipe profile
 *   \param [out] nwk_qos_profile - QOS profile
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_tunnel_id_param_get(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    uint32 *ttl_pipe_profile,
    uint32 *nwk_qos_profile)
{

    uint32 entry_handle_id = 0;
    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;
    uint32 next_local_out_lif = 0;

    uint32 ttl_mode;
    uint32 local_ttl_pipe_profile;
    int ref_count;
    uint32 qos_model;
    uint32 local_qos_index = 0;
    uint8 dscp;
    uint8 qos_profile_get_dummy_var;
    uint32 remark_profile = 0;
    uint32 total_sids_on_header;
    uint8 build_srh;
    uint8 is_reduced;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** convert GPORT Tunnel to get its local-LIF which serves as key to SRv6 EEDB table*/

    /** Get the SW state for SRv6 encapsulation Reduced/Normal mode */
    SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &is_reduced));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, info->tunnel_id, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                     &tunnel_gport_hw_resources));

    /** set the local-out-LIF as key and get the SRH EEDB entry fields  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, tunnel_gport_hw_resources.local_out_lif);
    /** taking the resutl-type from the tunnel, cause the table can contain multiple results */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 tunnel_gport_hw_resources.outlif_dbal_result_type);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get the nof_sids, not dependent on psp/usp (in psp LE is -1 vs usp) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NOF_SIDS,
                                                        INST_SINGLE, (uint32 *) &info->nof_sids));

    /** check is build srh is set, to set the ext_encap flag */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_BUILD_SRH,
                                                       INST_SINGLE, &build_srh));
    if (build_srh)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SRH_HEADER_EXT_LENGTH,
                                                            INST_SINGLE, &total_sids_on_header));
        total_sids_on_header /= 2;
        if (is_reduced)
        {
            total_sids_on_header += 1;
        }
        info->nof_additional_sids_extended_encap = total_sids_on_header - info->nof_sids;
    }
    else
    {
        info->flags |= BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP;
    }

    /** set T_Insert flag in case the current result-type is T_Insert */
    if (tunnel_gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6_T_INSERT)
    {
        info->flags |= BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT;
    }

    /** get next local-out-LIF to an EEDB entry which is pointed by this SRH Base tunnel_id */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                                        INST_SINGLE, &next_local_out_lif));

    /** convert the next local-out-LIF to L3 Int*/
    if (next_local_out_lif != 0)
    {
        bcm_gport_t next_encap_id_gport = BCM_GPORT_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit,
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                                    DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, _SHR_CORE_ALL,
                                                    next_local_out_lif, &next_encap_id_gport));
        if (next_encap_id_gport != BCM_GPORT_INVALID)
        {
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(info->next_encap_id, next_encap_id_gport);
        }
        else
        {
            info->next_encap_id = 0;
        }
    }

    /** Get TTL configuration */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                                        &ttl_mode));
    if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
    {
        info->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE,
                                                            INST_SINGLE, &local_ttl_pipe_profile));
        /** get TTL value from template manager */
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.profile_data_get
                        (unit, local_ttl_pipe_profile, &ref_count, (void *) &(info->ttl)));

    }
    else if (ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_UNIFORM)
    {
        info->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }

    /** Get QOS configuration */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ENCAP_1_QOS_MODEL, INST_SINGLE, &qos_model));

    SHR_IF_ERR_EXIT(dnx_qos_egress_model_dbal_to_bcm(unit, qos_model, &(info->egress_qos_model)));

    if ((qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE) ||
        (qos_model == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX,
                                                            INST_SINGLE, &local_qos_index));
        /** get DHCP value from template manager */
        /** Currently: network_qos_pipe=network_qos_pipe_mapped and dp_pipe_mapped=0 */
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_attr_get
                        (unit, local_qos_index, &dscp, &qos_profile_get_dummy_var, &qos_profile_get_dummy_var));
        info->dscp = (int) dscp;
    }
    else
    {
        info->dscp = 0;
    }

    /** remark profile, used for previous layer */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE,
                                                        INST_SINGLE, &remark_profile));
    if (remark_profile != DNX_QOS_INITIAL_MAP_ID)
    {
        DNX_QOS_REMARK_MAP_SET(remark_profile);
        DNX_QOS_EGRESS_MAP_SET(remark_profile);
        info->qos_map_id = remark_profile;
    }

    *ttl_pipe_profile = local_ttl_pipe_profile;
    *nwk_qos_profile = local_qos_index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function receives template profiles and removes them from template manager.
 * Supports TTL and QOS templates.
 *
 *   \param [in] unit
 *   \param [in] ttl_pipe_profile - TTL pipe profile
 *   \param [in] is_ttl_pipe_mode - 0 if not pipe mode
 *   \param [in] qos_model_idx - QOS profile
 *   \param [in] is_qos_model_pipe_mode - 0 if not pipe mode
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_qos_ttl_template_hw_free(
    int unit,
    uint32 ttl_pipe_profile,
    uint32 is_ttl_pipe_mode,
    uint32 qos_model_idx,
    uint32 is_qos_model_pipe_mode)
{

    uint8 remove_ttl_pipe_profile = 0;
    uint8 remove_qos_pipe_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Free TTL profile template */
    if (is_ttl_pipe_mode)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_ttl_pipe_profile.free_single
                        (unit, ttl_pipe_profile, &remove_ttl_pipe_profile));
    }

    /** remove QOS profile from template manager */
    if (is_qos_model_pipe_mode)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_free
                        (unit, (int) qos_model_idx, &remove_qos_pipe_profile));
    }

    /** Remove TTL entry from HW */
    if (remove_ttl_pipe_profile)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_clear(unit, ttl_pipe_profile));
    }
    /** Remove QOS entry from HW */
    if (remove_qos_pipe_profile)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_network_qos_pipe_profile_hw_clear(unit, qos_model_idx));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the SRH Base EEDB entry, as well as point to a next encap_id
 *
 *   \param [in] unit
 *   \param [in] info - Structure that is used in SRv6 SRH Base initiator APIs,
 *                      for managing device behavior at SRv6 Tunnel Encapsulation
 *          [in] info.nof_sids -  number of SIDs in the SID list
 *          [in] info.qos_map_id - QOS map identifier
 *          [in] info.ttl - Tunnel header TTL
 *          [in] info.dscp - Tunnel header DSCP value
 *          [in] info.egress_qos_model - Egress QOS and TTL model
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t)
 *   \param [in] local_lif - pointer to local_lif for the SRH entry in EEDB
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_eedb_entry_set(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info,
    int local_lif)
{

    uint32 entry_handle_id;

    bcm_gport_t next_encap_id_gport;

    dnx_algo_gpm_gport_hw_resources_t tunnel_gport_hw_resources;

    uint32 ttl_pipe_profile;
    uint32 nwk_qos_profile;
    dbal_enum_value_field_encap_qos_model_e encap_qos_model;

    uint32 nof_classic_sids_on_out_packet;
    uint32 last_entry;
    uint8 build_srh = TRUE;
    uint8 has_additional_sids = FALSE;
    uint8 is_reduced = 0;

    int num_of_cur_pass_classic_sids = info->nof_sids;

    uint32 srv6_table_result_type = DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the SW state for SRv6 encapsulation Reduced/Normal mode */
    SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &is_reduced));

    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_T_INSERT))
    {
        srv6_table_result_type = DBAL_RESULT_TYPE_EEDB_SRV6_ETPS_SRV6_T_INSERT;
    }

    
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_REPLACE))
    {
        uint32 prev_nof_sid = 0;

        /** Add the SRH EEDB entry and set next out-LIF pointer */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     srv6_table_result_type);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        /** get the segment left (not last entry) - which is (nof_sids-1), to not be dependent on psp/usp (in psp LE is -1 vs usp) */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_NOF_SIDS,
                                                            INST_SINGLE, &prev_nof_sid));

        if (prev_nof_sid != info->nof_sids)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing SRH base info, cannot update the number of SID. prev=%d, new=%d \n",
                         prev_nof_sid, info->nof_sids);
        }
    }
    /** Add the SRH EEDB entry and set next out-LIF pointer */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, srv6_table_result_type);
    /*
     * CS_VAR field is used both to set SRH (Header Externsion Length), which describes the length
     * of the SID list extension in quants of 64bits, ie. 2*nof_sids.
     * Also it's used for ETPS context selection to let each Encap stage know how many SIDs to
     * construct in each
     */

    last_entry = info->nof_sids + info->nof_additional_sids_extended_encap - 1;
    if (is_reduced)
    {
        last_entry -= 1;
    }

    nof_classic_sids_on_out_packet = last_entry + 1;

    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_EXTENDED_ENCAP))
    {
        build_srh = FALSE;
    }
    if (info->nof_additional_sids_extended_encap > 0)
    {
        has_additional_sids = TRUE;
    }

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_BUILD_SRH, INST_SINGLE, build_srh);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_HAS_ADDITIONAL_SIDS, INST_SINGLE,
                                has_additional_sids);

    /** if Unified-SID is used, mark in SRH Flags as Unified and factor SL, LE */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_UNIFIED))
    {
        /*
         * NOF SIDs is given for Unified-SID in 32b resolution, and ENCAP stages contexts are selected by looking
         * NOF_SIDs ETPS field in 128b resolution
         */
        num_of_cur_pass_classic_sids = sal_ceil_func(info->nof_sids, SRV6_UNIFIED_CLASSICAL_SID_FACTOR);

        nof_classic_sids_on_out_packet = sal_ceil_func(info->nof_additional_sids_extended_encap + info->nof_sids,
                                                       SRV6_UNIFIED_CLASSICAL_SID_FACTOR);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_FLAGS, INST_SINGLE,
                                     SRV6_SRH_FLAGS_UNIFIED_MARKING);

    }

    /** this is not an actual SRH field, but used to set the Encap stages contexts per each pass */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_SIDS, INST_SINGLE, num_of_cur_pass_classic_sids);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_HEADER_EXT_LENGTH, INST_SINGLE,
                                 nof_classic_sids_on_out_packet * 2);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, INST_SINGLE, last_entry);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_SEGMENT_LEFT, INST_SINGLE,
                                 info->nof_additional_sids_extended_encap + info->nof_sids - 1);

    /** if next_encap_id is non zero, append to the SRH Base EEDB entry */
    if (info->next_encap_id != 0)
    {
        /** convert the next_encap_id from bcm_if_t to GPORT */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_encap_id_gport, info->next_encap_id);

        /** convert GPORT to local out-LIF */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, next_encap_id_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                         &tunnel_gport_hw_resources));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     tunnel_gport_hw_resources.local_out_lif);
    }

    /** TTL decrement is always set */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, INST_SINGLE, FALSE);

   /** remark profile (qos_map_id) always set, used for previous layer */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                 DNX_QOS_MAP_PROFILE_GET(info->qos_map_id));

    /*
     * In case of Replace, clear the template manager DB QOS/TTL entry if used only here
     * -for UNIFOM mode modification this is crucial, since the template DB needs now to be free of PIPE
     * -for PIPE mode modification this is crucial, in case PIPE mode is modified, and need to delete the previous
     */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_REPLACE))
    {
        bcm_srv6_srh_base_initiator_info_t existing_info;
        uint32 existing_ttl_pipe_profile;
        uint32 existing_nwk_qos_profile;

        uint32 was_ttl_pipe_mode = 0;
        uint32 was_qos_model_pipe_mode = 0;

        bcm_srv6_srh_base_initiator_info_t_init(&existing_info);
        existing_info.tunnel_id = info->tunnel_id;

        /** get the existing info before the replacing and TTL and QOS Profiles */
        SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_tunnel_id_param_get
                        (unit, &existing_info, &existing_ttl_pipe_profile, &existing_nwk_qos_profile));

        /*
         * Remove allocated TTL and QOS templates from template manager and HW
         */
        if ((existing_info.egress_qos_model.egress_ttl == bcmQosEgressModelPipeNextNameSpace) ||
            (existing_info.egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace))
        {
            was_ttl_pipe_mode = 1;
        }

        if ((existing_info.egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
            (existing_info.egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
        {
            was_qos_model_pipe_mode = 1;
        }

        SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_qos_ttl_template_hw_free
                        (unit, existing_ttl_pipe_profile, was_ttl_pipe_mode, existing_nwk_qos_profile,
                         was_qos_model_pipe_mode));
    }

    /** Update template manager DB (used for PIPE mode) */
    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_qos_ttl_template_alloc_and_hw_set
                    (unit, info, &ttl_pipe_profile, &nwk_qos_profile));

   /** Set TTL model */
    if (info->egress_qos_model.egress_ttl == bcmQosEgressModelUniform)
    {
       /** TTL UNIFORM model */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_UNIFORM);
    }
    else
    {
        /** TTL PIPE model */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, INST_SINGLE, ttl_pipe_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_TTL_MODE_PIPE);
    }

    /** Set QOS model */
    SHR_IF_ERR_EXIT(dnx_qos_egress_model_bcm_to_dbal(unit, &info->egress_qos_model, &encap_qos_model));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_1_QOS_MODEL, INST_SINGLE, encap_qos_model);

    if ((info->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
        (info->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, nwk_qos_profile);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_IDX, INST_SINGLE, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - low level function for freeing and removing all SRH Base resources with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SRH Base EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID
 *
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_srh_base_initiator_tunnel_id_delete(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    uint32 ttl_pipe_profile;
    uint32 nwk_qos_profile;
    uint32 is_ttl_pipe_mode = 0;
    uint32 is_qos_model_pipe_mode = 0;

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->tunnel_id, lif_flags, &gport_hw_resources));

    /** get tunnel_id EEDB entry parameters */
    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_tunnel_id_param_get(unit, info, &ttl_pipe_profile, &nwk_qos_profile));

    /** Delete global to local mapping from GLEM */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources.global_out_lif));

    /*
     * Remove allocated TTL and QOS templates from template manager and HW
     */
    if ((info->egress_qos_model.egress_ttl == bcmQosEgressModelPipeNextNameSpace) ||
        (info->egress_qos_model.egress_ttl == bcmQosEgressModelPipeMyNameSpace))
    {
        is_ttl_pipe_mode = 1;
    }

    if ((info->egress_qos_model.egress_qos == bcmQosEgressModelPipeNextNameSpace) ||
        (info->egress_qos_model.egress_qos == bcmQosEgressModelPipeMyNameSpace))
    {
        is_qos_model_pipe_mode = 1;
    }

    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_qos_ttl_template_hw_free(unit, ttl_pipe_profile, is_ttl_pipe_mode,
                                                                         nwk_qos_profile, is_qos_model_pipe_mode));

    /** Remove EEDB entry of the SRH Base */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources.local_out_lif);
    /** There are multiple results for the EEDB_SRV6 table, hence getting this info on the tunnel */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.outlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Delete global and local out lifs allocations */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, gport_hw_resources.global_out_lif, NULL, gport_hw_resources.local_out_lif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the SRH Base global and local LIFs as well as its EEDB entry
 *          Supports next encap_id to link to the SRH EEDB entry.
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SRH Base EEDB entry
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *                            BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID - specify own tunnel_id
 *                            BCM_SRV6_SRH_BASE_INITIATOR_REPLACE - update SID EEDB entry
 *          [in,out] info.tunnel_id - Tunnel SRv6 SRH base object ID
 *          [in] info.nof_sids - Number of SIDs in the SID list
 *          [in] info.qos_map_id - QOS map identifier
 *          [in] info.ttl -  Tunnel header TTL
 *          [in] info.dscp - Tunnel header DSCP value
 *          [in] info.egress_qos_model - Egress qos and ttl model
 *          [in] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_srh_base_initiator_create(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    int local_lif = 0;
    int global_lif = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(unit, info->tunnel_id);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_srh_base_initiator_create_verify(unit, info));

    /*
     * Allocate the global and local LIFs for the SID EEDB entry and add GLEM mapping
     * the global-LIF is taken/updated from/to the info->tunnel_id (GPORT)
     */

    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_lif_allocate(unit, info, &global_lif, &local_lif));

    /*
     * configure the SID EEDB entry and
     * concatenate the next interface-id to the SID EEDB entry
     */
    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_eedb_entry_set(unit, info, local_lif));

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_SRH_BASE_INITIATOR_REPLACE)))
    {
        /** add GLEM mapping between global and local LIFs, if not in Replace case (GLEM already done) */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif, local_lif));

        /** update and convert back the global_lif into the tunnel-id GPORT if not Replace */
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id, global_lif);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove and free all SRH Base resources with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SRH Base EEDB entry
 *          [in,out] info.tunnel_id - Tunnel SRv6 SRH base object ID
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_srh_base_initiator_delete(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR(unit, info->tunnel_id);

     /** verification of supported features */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_srh_base_initiator_general_verify(unit, info));

    /** delete the SRH Base tunnel_id and its resources */
    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_tunnel_id_delete(unit, info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets the SRH Base information with using the GPORT Tunnel-id as key
 *
 *   \param [in] unit
 *   \param [in] info - the structure which configures the SRH Base EEDB entry
 *          [in] info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *                            BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID - specify own tunnel_id
 *                            BCM_SRV6_SRH_BASE_INITIATOR_REPLACE - update SID EEDB entry
 *          [in] info.tunnel_id - Tunnel SRv6 SRH base object ID
 *          [out] info.nof_sids - Number of SIDs in the SID list
 *          [out] info.qos_map_id - QOS map identifier
 *          [out] info.ttl -  Tunnel header TTL
 *          [out] info.dscp - Tunnel header DSCP value
 *          [out] info.egress_qos_model - Egress qos and ttl model
 *          [out] info.next_encap_id - Next pointer interface ID (bcm_if_t),
 *                                    can be next SID initiator or
 *                                    next-encapsulation interface
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_srh_base_initiator_get(
    int unit,
    bcm_srv6_srh_base_initiator_info_t * info)
{

    uint32 ttl_pipe_profile;
    uint32 nwk_qos_profile;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_srh_base_initiator_general_verify(unit, info));

    /*
     * Retrieve the SRH Base tunnel_id parameters
     */
    SHR_IF_ERR_EXIT(dnx_srv6_srh_base_initiator_tunnel_id_param_get(unit, info, &ttl_pipe_profile, &nwk_qos_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function traverses all srh_base_initiator objects and runs a callback function provided by the user on this data, along
 *          with user_data.
 *
 *   \param [in] unit - device id
 *   \param [in] additional_info - structure for additional control over cb traverse
 *          [in] additional_info.flags - BCM_SRV6_SRH_BASE_INITIATOR_XXX flags
 *          [in] additional_info.traverse_flags - BCM_SRV6_SRH_BASE_INITIATOR_TRAVERSE_XXX flags
 *
 *   \param [in] user_data - user data to be used along with the call back function
 *   \param [in] cb - user call back function of the following structure:
 *
 *    typedef int (*bcm_srv6_srh_base_initiator_traverse_cb)(
 *            int unit,
 *            bcm_srv6_srh_base_initiator_info_t *info,
 *            void *user_data);
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_srh_base_initiator_traverse(
    int unit,
    bcm_srv6_srh_base_initiator_traverse_info_t additional_info,
    bcm_srv6_srh_base_initiator_traverse_cb cb,
    void *user_data)
{
    int is_end = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_srh_base_initiator_traverse_verify(unit, additional_info, cb));

    /*
     * Iterate the EEDB_SRV6 table, which holds the SRH Base information
     * Translate to srh base info and call the srh base get API
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_SRV6, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 local_out_lif[1];
        bcm_srv6_srh_base_initiator_info_t entry_info;

        bcm_srv6_srh_base_initiator_info_t_init(&entry_info);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_out_lif));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif(unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                    _SHR_CORE_ALL, (int) local_out_lif[0], &entry_info.tunnel_id));
        SHR_IF_ERR_EXIT(bcm_dnx_srv6_srh_base_initiator_get(unit, &entry_info));
        SHR_IF_ERR_EXIT(cb(unit, &entry_info, user_data));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for nof_sids add/get/delete APIs
 *
 *   \param [in] unit
 *   \param [in] info - the structure which includes the RCH Port and RCH Encap_id
 *          [in] info.flags - BCM_SRV6_EXTENSION_TERMINATOR_XXX flags
 *          [in] info.nof_sids - number of SIDs in the SRv6 packet
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_general_verify(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info)
{

    int min_nof_terminated_usp_sids = 0;
    int max_nof_terminated_usp_sids = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** check that the structure in non NULL */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    min_nof_terminated_usp_sids = dnx_data_srv6.termination.min_nof_terminated_sids_usp_get(unit);
    max_nof_terminated_usp_sids = dnx_data_srv6.termination.max_nof_terminated_sids_usp_get(unit);
    /** check that nof_sids is supported */
    if ((info->nof_sids < min_nof_terminated_usp_sids) || (info->nof_sids > max_nof_terminated_usp_sids))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SRv6 nof_sids:%d is not supported, should be between:%d and %d",
                     info->nof_sids, min_nof_terminated_usp_sids, max_nof_terminated_usp_sids);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - General verification for extension_terminator traverse API
 */
static shr_error_e
dnx_srv6_extension_terminator_traverse_verify(
    int unit,
    bcm_srv6_extension_terminator_traverse_info_t additional_info,
    bcm_srv6_extension_terminator_traverse_cb cb)
{
    SHR_FUNC_INIT_VARS(unit);

    /** check that the callback in non NULL */
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb function");

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Allocate for the SRv6 ESR USP the P2P IN_LIF for 1st Pass
 *
 *   \param [in] unit
 *   \param [out] local_inlif - the allocated local_inlif
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_in_lif_allocate(
    int unit,
    int *local_inlif)
{

    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED;
    inlif_info.core_id = _SHR_CORE_ALL;

    /*
     * don't need a global-LIF since the LIF is used only for P2P information and no need to go over
     * the system headers for further processing
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, &inlif_info, NULL));
    *local_inlif = inlif_info.local_inlif;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - Free the Egress SRv6 tunnel P2P IN_LIF
 *
 *   \param [in] unit
 *   \param [in] local_inlif - the local_inlif to free
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_in_lif_free(
    int unit,
    uint32 local_inlif)
{

    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Delete the local in_lif */
    sal_memset(&inlif_info, 0, sizeof(inlif_info));
    inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED;
    inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED;
    inlif_info.core_id = _SHR_CORE_ALL;
    inlif_info.local_inlif = local_inlif;
    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &inlif_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Maps the nof_sids to local IN_LIF
 *
 *
 *   \param [in] unit
 *   \param [in] nof_sids
 *   \param [in] local_inlif
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_lookup_add(
    int unit,
    int nof_sids,
    int local_inlif)
{

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_TERMINATED_LIF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, nof_sids - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Deletes the mapping of nof_sids to a local IN_LIF
 *
 *
 *   \param [in] unit
 *   \param [in] nof_sids
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_lookup_delete(
    int unit,
    int nof_sids)
{

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_TERMINATED_LIF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, nof_sids - 1);

    /** delete the lookup */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets  local IN_LIF which was mapped by nof_sids
 *
 *
 *   \param [in] unit
 *   \param [in] nof_sids
 *   \param [out] local_inlif - mapped in_lif
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_lookup_get(
    int unit,
    int nof_sids,
    uint32 *local_inlif)
{

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get the in_lif mapped from nof_sids */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_TERMINATED_LIF, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, nof_sids - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, local_inlif));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - IN_LIF parameters setting
 *
 *
 *   \param [in] unit
 *   \param [in] info - nof_sids mapping to RCH port and encapsulation structure
 *          [in] info.encap_id -  GPORT encoded as Encapsulation-id global-LIF
 *          [in] info.port - GPORT encoded as destination
 *   \param [in] local_inlif - the inlif for which to set the configuration
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_in_lif_table_set(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info,
    int local_inlif)
{

    uint32 entry_handle_id = 0;
    uint32 global_lif_encap_id = 0;

    /*
     * this structure contains pre-fec forward information,
     * specifically we will use for converting Port GPORT to destination port and encap_id global lif in our case,
     * which suit DBAL access - uint32 type
     */
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (info->encap_id != 0)
    {
        global_lif_encap_id = BCM_GPORT_TUNNEL_ID_GET(info->encap_id);
    }

    /*
     * convert the Port GPORT to destination field, as set by DBAL in forwarding tables
     * the 'forward_info' structure contains also the global_lif_encap_id, currently transforming 1:1
     * to 'forward_info.outlif'
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, info->port, global_lif_encap_id, &forward_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED);
    /** in order to not use the P2P IN_LIF Destination and Global_OUT_LIF but go to FWD */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SERVICE_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_VTT_LIF_SERVICE_TYPE_SERVICE_TYPE_P2P);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, forward_info.outlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, forward_info.destination);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete in_lif from lif table, that was mapped by SRv6 nof_sids
 *        - Also delete the in_lif's Ingress Propagation Profile from HW if it's not used anywhere else
 *
 *
 *   \param [in] unit
 *   \param [in] local_inlif    -  the in lif which is used as the key
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_in_lif_table_delete(
    int unit,
    uint32 local_inlif)
{

    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** getting the Propagation Profile of the P2P LIF */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED);

    /** Delete the in_lif from its table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the IN_LIF parameters
 *
 *
 * \param [in]  unit
 * \param [in]  local_inlif    -  the in lif which is used as the key
 * \param [out] info - structure which describes the SRv6 next protocol mapping to port and encapsulation
 *        [out] info.encap_id -  GPORT encoded as Encapsulation-id global-LIF
 *        [out] info.port - GPORT encoded as destination
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
static shr_error_e
dnx_srv6_extension_terminator_in_lif_param_get(
    int unit,
    uint32 local_inlif,
    bcm_srv6_extension_terminator_mapping_t * info)
{

    uint32 entry_handle_id = 0;
    uint32 destination = 0;
    uint32 global_lif_encap_id = 0;

    /*
     * this structure contains pre-fec forward information,
     * specifically we will use for converting DBAL fields (uint32) destination and ecnap_id to GPORT types
     */
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, local_inlif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_IN_LIF_FORMAT_SRV6_TERMINATED_IN_LIF_SRV6_TERMINATED);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &global_lif_encap_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));

    /*
     * convert the DBAL fields (uint32) destination and ecnap_id to GPORT types
     */
    forward_info.destination = destination;
    forward_info.outlif = global_lif_encap_id;

    /** user can decide to use only destination (FEC) and take the encap_id from there */
    if (global_lif_encap_id == 0)
    {
        forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
    }
    else
    {
        forward_info.fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
    }

    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information
                    (unit, &(info->port), &global_lif_encap_id, &forward_info, FALSE));

    /** get from the encap_id GPORT the global_lif to use in for DBAL */
    if (global_lif_encap_id != 0)
    {
        BCM_GPORT_TUNNEL_ID_SET(info->encap_id, global_lif_encap_id);
    }
    else
    {
        info->encap_id = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Creation and configuration of a Next Protocol (above SRv6) mapping to
 *          and RCH port and Encapsulation.
 *
 *          This is required only in ESR USP 2-Pass case, where the ESR SRv6 node, which is
 *          last in SRv6 Tunnel needs to:
 *          - Terminate SRv6 and IPv6 headers (1st-Pass)
 *          - Do Forwarding on the layer, which was above SRv6 (2nd-Pass)
 *
 *          This API creates the mapping needed, after recognizing the layer protocol above SRv6,
 *          for creating for it - the user given RCH encapsulation,and the user specified RCH port,
 *          for sending the packet for 2nd-Pass via Recycling Port.
 *
 *   \param [in] unit - device id
 *   \param [in] info - structure which describes the SRv6 next protocol mapping to port and encapsulation
 *          [in] info.flags - BCM_SRV6_EXTENSION_TERMINATOR_XXX flags
 *                            BCM_SRV6_EXTENSION_TERMINATOR_REPLACE - Replace functionality,
 *                            must be called when object already created
 *          [in] info.nof_sids - number of SIDs in the SRv6 Packet
 *          [in] info.encap_id -  GPORT encoded as Encapsulation-id global-LIF
 *          [in] info.port - GPORT encoded as destination
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all is ok.
 */
int
bcm_dnx_srv6_extension_terminator_add(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info)
{

    int local_inlif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_extension_terminator_general_verify(unit, info));

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_EXTENSION_TERMINATOR_REPLACE)))
    {
        /*
         * Allocate the SRv6 tunnel terminator
         */
        SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_in_lif_allocate(unit, &local_inlif));
    }
    else
    {
        uint32 local_inlif_u32 = 0;
        /*
         * read the local in_lif
         */
        SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_lookup_get(unit, info->nof_sids, &local_inlif_u32));
        local_inlif = local_inlif_u32;
    }

    /*
     * IN_LIF parameters setting
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_in_lif_table_set(unit, info, local_inlif));

    if (!(_SHR_IS_FLAG_SET(info->flags, BCM_SRV6_EXTENSION_TERMINATOR_REPLACE)))
    {
        /*
         * Received a local_inlif from allocation manager, map the nof_sids to local IN_LIF
         */
        SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_lookup_add(unit, info->nof_sids, local_inlif));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function deletes the port and encap_id that were mapped by the nof_sids (above SRv6 layer)
 *
 *   \param [in] unit - device id
 *   \param [in,out] info - structure which describes the SRv6 next protocol mapping to port and encapsulation
 *          [in] info.nof_sids -number of SIDs in the SRv6 packet
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_extension_terminator_delete(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info)
{

    uint32 local_inlif = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * opt in for Error Recovery
     */
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_extension_terminator_general_verify(unit, info));

    /*
     * store the local_inlif, before deleting the lookup to it (to use as key for deleting from LIF table)
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_lookup_get(unit, info->nof_sids, &local_inlif));

    /*
     * delete nof_sids to in_lif lookup
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_lookup_delete(unit, info->nof_sids));

    /*
     * delete the in_lif from its table
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_in_lif_table_delete(unit, local_inlif));

    /*
     * free the in_lif allocation
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_in_lif_free(unit, local_inlif));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function returns the port and encap_id, that were mapped by the nof_sids (above SRv6 layer)
 *
 *   \param [in] unit - device id
 *   \param [in,out] info - structure which describes the SRv6 next protocol mapping to port and encapsulation
 *          [in] info.nof_sids - number of SIDs in the SRv6 packet
 *          [out] info.encap_id -  GPORT encoded as Encapsulation-id global-LIF
 *          [out] info.port - GPORT encoded as destination
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_extension_terminator_get(
    int unit,
    bcm_srv6_extension_terminator_mapping_t * info)
{

    uint32 local_inlif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verification of supported features.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_extension_terminator_general_verify(unit, info));

    /*
     * Received a local_inlif from allocation manager, map the nof_sids to local IN_LIF
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_lookup_get(unit, info->nof_sids, &local_inlif));

    /*
     * IN_LIF parameters setting into the info structure (update port + encap_id)
     */
    SHR_IF_ERR_EXIT(dnx_srv6_extension_terminator_in_lif_param_get(unit, local_inlif, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function traverses all nof_sids mappings and runs a callback function provided by the user on this data, along
 *          with user_data.
 *
 *   \param [in] unit - device id
 *   \param [in] additional_info - structure for additional control over cb traverse
 *          [in] additional_info.flags - BCM_SRV6_EXTENSION_TERMINATOR_XXX flags
 *          [in] additional_info.traverse_flags - BCM_SRV6_EXTENSION_TERMINATOR_TRAVERSE_XXX flags
 *
 *   \param [in] user_data - user data to be used along with the call back function
 *   \param [in] cb - user call back function of the following structure:
 *
 *    typedef int (*bcm_srv6_extension_terminator_traverse_cb)(
 *            int unit,
 *            bcm_srv6_extension_terminator_mapping_t *info,
 *            void *user_data);
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
int
bcm_dnx_srv6_extension_terminator_traverse(
    int unit,
    bcm_srv6_extension_terminator_traverse_info_t additional_info,
    bcm_srv6_extension_terminator_traverse_cb cb,
    void *user_data)
{
    int is_end = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_INVOKE_VERIFY_DNX(dnx_srv6_extension_terminator_traverse_verify(unit, additional_info, cb));

    /*
     * Iterate the Next protocol mapping table
     * Translate to next protocol info and call the next protocol get API
     */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_TERMINATED_LIF, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 last_entry[1];
        bcm_srv6_extension_terminator_mapping_t entry_info;

        bcm_srv6_extension_terminator_mapping_t_init(&entry_info);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_SRH_LAST_ENTRY, last_entry));

        entry_info.flags = 0;
        entry_info.nof_sids = last_entry[0] + 1;
        SHR_IF_ERR_EXIT(bcm_dnx_srv6_extension_terminator_get(unit, &entry_info));
        SHR_IF_ERR_EXIT(cb(unit, &entry_info, user_data));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - API sets the SRv6 Virtual Register and SW States for:
 *          - Set to Egress PSP mode - '1', or USP - '0'.
 *            Default after BCM Init is USP.
 *          - Set Encapsulation Reduced mode - '1', or Normal - '0'.
 *            Default after BCM Init is Normal.
 *
 *   \param [in] unit - unit ID
 *   \param [in] type - switch control type:
 *                        bcmSwitchSRV6EgressPSPEnable - use to set to Egress PSP mode
 *                        bcmSwitchSRV6ReducedModeEnable use to set to Reduced encapsulation mode
 *    \param [in] value - the value to set above modes - 1 or 0.
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
shr_error_e
dnx_switch_srv6_modes_set(
    int unit,
    bcm_switch_control_t type,
    int value)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((value != 0) && (value != 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "value (%d) is not supported, must be 0 or 1", value);
    }

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmSwitchSRV6EgressPSPEnable:
        {
            /** Configure the SRv6 VR for PSP/USP mode  */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SRV6, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENDPOINT, INST_SINGLE, value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Configure the SW state to PSP/USP mode */
            SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.set(unit, value));
            break;
        }
        case bcmSwitchSRV6ReducedModeEnable:
        {
            /** Configure the SRv6 VR for Reduced/Normal mode */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SRV6, &entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAPSULATION, INST_SINGLE, value);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Configure the SW state to Reduced/Normal mode */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.set(unit, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch control not supported!, %d", type);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - API gets the SRv6 Virtual Register and SW States for:
 *          - Get the Egress PSP mode - '1', or USP - '0'.
 *          - Get Encapsulation Reduced mode - '1', or Normal - '0'.
 *
 *   \param [in] unit - unit ID
 *   \param [in] type - switch control type:
 *                        bcmSwitchSRV6EgressPSPEnable - use to set to Egress PSP mode
 *                        bcmSwitchSRV6ReducedModeEnable use to set to Reduced encapsulation mode
 *    \param [in] value - the value of the Egress or encapsulation mode as set by type
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
shr_error_e
dnx_switch_srv6_modes_get(
    int unit,
    bcm_switch_control_t type,
    int *value)
{

    uint8 value_uint8;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
        case bcmSwitchSRV6EgressPSPEnable:
        {
            /** Get the SW state if SRv6 Egress is PSP/USP mode */
            SHR_IF_ERR_EXIT(srv6_modes.egress_is_psp.get(unit, &value_uint8));
            break;
        }
        case bcmSwitchSRV6ReducedModeEnable:
        {
            /** Get the SW state if SRv6 Encapsulation is Reduced/Normal mode */
            SHR_IF_ERR_EXIT(srv6_modes.encap_is_reduced.get(unit, &value_uint8));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Switch control not supported!, %d", type);
        }
    }

    *value = value_uint8;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
