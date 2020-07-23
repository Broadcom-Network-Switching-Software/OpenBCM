/** \file ppp_init.c
 *  General PPP encapsulation functionality for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PPP
/*
 * Include files.
 * {
 */
#include <bcm/ppp.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * MACROs and ENUMs
 * { 
 */
#define DNX_PPP_INIT_SESSION_ID_MAX     0xffff
#define DNX_PPP_INIT_TUNNEL_ID_MAX      0xffff
/*
 * End of MACROs
 * }
 */

/*
 * Verify functions
 * {
 */

/**
 * \brief
 * Verify ppp type is supported
 * \param [in] unit - the unit number.
 * \param [in] ppp_type - supporyed ppp type.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_ppp_initiator_type_verify(
    int unit,
    bcm_ppp_type_t ppp_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that ppp type is supported */
    switch (ppp_type)
    {
        case bcmPPPTypePPPoE:
        case bcmPPPTypeL2TPv2:
        {
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP encapsulation: invalid type(%d)", ppp_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_initiator_create
 * \param [in] unit - the unit number.
 * \param [in] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->flag - See BCM_PPP_INIT_XXX \n
 *        BCM_PPP_INIT_WITH_ID - Create object with ID \n
 *        BCM_PPP_INIT_REPLACE - Update existing object \n
 *        BCM_PPP_INIT_STAT_ENABLE - Statistics enabled \n
 *   [in] info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [in] info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [in] info->encap_access - Encapsulation phase  \n
 *   [in] info->egress_qos_model - Egress qos and ttl model \n
 *   [in] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_ppp_initiator_create_verify(
    int unit,
    bcm_ppp_initiator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Verify that ppp type is supported */
    SHR_IF_ERR_EXIT(dnx_ppp_initiator_type_verify(unit, info->type));

    /** Verify TTL */
    if (info->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "model support only bcmQosEgressModelUniform");
    }

    /** Verify WITH_ID and REPLACE flags */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_REPLACE) && !_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PPP_INIT_REPLACE can't be used without BCM_TUNNEL_WITH_ID");
    }

    /** Verify WITH_ID */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_WITH_ID))
    {
        /** Check Tunnel Gport is available */
        if (!BCM_GPORT_IS_TUNNEL(info->ppp_initiator_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given when BCM_PPP_INIT_WITH_ID");
        }
    }

    /** Check fields is encapsulaton */
    if (info->type == bcmPPPTypeL2TPv2)
    {
        if (info->session_id > DNX_PPP_INIT_SESSION_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid session id 0x%x", info->session_id);
        }
        if (info->l2tpv2_tunnel_id > DNX_PPP_INIT_TUNNEL_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid l2tpv2 tuennel id 0x%x", info->l2tpv2_tunnel_id);
        }
    }
    else if (info->type == bcmPPPTypePPPoE)
    {
        if (info->session_id > DNX_PPP_INIT_SESSION_ID_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "PPP termination: invalid session id 0x%x", info->session_id);
        }
    }

    /** Check QOS model is allowed */
    if (info->egress_qos_model.egress_qos != bcmQosEgressModelUniform)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "qos model (%d) is not allowed. Available models are Uniform.", info->egress_qos_model.egress_qos);
    }
    if (info->egress_qos_model.egress_ttl != bcmQosEgressModelUniform)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ttl model (%d) is not allowed. Available models are Uniform.", info->egress_qos_model.egress_ttl);
    }
    if (info->egress_qos_model.egress_ecn == bcmQosEgressEcnModelEligible)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ecn eligible is not supported");
    }

    /** Check Encapsulation phase for PPPoE, all phases is available for L2TP */
    if ((info->type == bcmPPPTypePPPoE)
        && (info->encap_access != bcmEncapAccessRif)
        && (info->encap_access != bcmEncapAccessNativeArp) && (info->encap_access != bcmEncapAccessTunnel1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation phase %d is not allowed for PPPoE", info->encap_access);
    }

    if ((info->type == bcmPPPTypeL2TPv2)
        && ((info->encap_access == bcmEncapAccessInvalid) || (info->encap_access == bcmEncapAccessNativeAc)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation phase %d is not allowed for L2TP", info->encap_access);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_initiator_get
 * \param [in] unit - the unit number.
 * \param [in] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [in] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_ppp_initiator_get_verify(
    int unit,
    bcm_ppp_initiator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Check Tunnel Gport is available */
    if (!BCM_GPORT_IS_TUNNEL(info->ppp_initiator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for BCM-API: bcm_dnx_ppp_initiator_delete
 * \param [in] unit - the unit number.
 * \param [in] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [in] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_ppp_initiator_delete_verify(
    int unit,
    bcm_ppp_initiator_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    /** Check Tunnel Gport is available */
    if (!BCM_GPORT_IS_TUNNEL(info->ppp_initiator_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Gport is not given");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Used by bcm_dnx_ppp_initiator_traverse to verify info struct.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - A pointer to the struct that holds addtional information
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of an error.
 */
static shr_error_e
dnx_ppp_initiator_traverse_verify(
    int unit,
    bcm_ppp_initiator_additional_info_t * additional_info,
    bcm_ppp_initiator_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(additional_info, _SHR_E_PARAM, "additional info");
    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "user callback");
    SHR_NULL_CHECK(user_data, _SHR_E_PARAM, "user data");
exit:
    SHR_FUNC_EXIT;
}

/*
 * End of Verify functions
 * }
 */

/*
 * Inner functions
 * {
 */

/**
 * \brief
 *  Allocate local lif and Write to IP tunnel encapsulation outLIF table.
 *
 * \param [in] unit - Relevant unit
 * \param [in] info - A pointer to the struct that holds information for the PPP initiator entry.
 * \param [in] dbal_table_id - DBAL table id.
 * \param [in] global_outlif - Global outLIF.
 * \param [in] next_local_outlif - Next pointer in EEDB.
 * \param [out] local_outlif - Local outLIF.
 * \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \ref shr_error_e.
 */
static shr_error_e
dnx_ppp_initiator_allocate_local_lif_and_out_lif_table_set(
    int unit,
    bcm_ppp_initiator_t * info,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 next_local_outlif,
    uint32 *local_outlif)
{
    uint32 entry_handle_id;
    uint32 tmp_raw_data = 0;
    lif_table_mngr_outlif_info_t lif_table_mngr_outlif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(local_outlif, _SHR_E_PARAM, "local_outlif");

    sal_memset(&lif_table_mngr_outlif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    /** Don't allocate lif if replace  */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_REPLACE))
    {
        lif_table_mngr_outlif_info.flags |= LIF_TABLE_MNGR_LIF_INFO_REPLACE;
    }

    /** Set outlif phase */
    if (info->encap_access == bcmEncapAccessRif)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1;
    }
    else if (info->encap_access == bcmEncapAccessNativeArp)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2;
    }
    else if (info->encap_access == bcmEncapAccessTunnel1)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3;
    }
    else if (info->encap_access == bcmEncapAccessTunnel2)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4;
    }
    else if (info->encap_access == bcmEncapAccessTunnel3)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5;
    }
    else if (info->encap_access == bcmEncapAccessTunnel4)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6;
    }
    else if (info->encap_access == bcmEncapAccessArp)
    {
        lif_table_mngr_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7;
    }

    lif_table_mngr_outlif_info.global_lif = global_outlif;

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Set DATA fields */
    /** Result type is superset, will be decided internally by lif table manager */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    if (next_local_outlif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                                     next_local_outlif);
    }

    /** Set DATA fields */
    if (info->type == bcmPPPTypePPPoE)
    {
        tmp_raw_data = info->session_id;
    }
    else if (info->type == bcmPPPTypeL2TPv2)
    {
        tmp_raw_data = ((info->l2tpv2_tunnel_id & 0x0000ffff) << 16);
        tmp_raw_data += info->session_id;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, tmp_raw_data);

    STAT_PP_CHECK_AND_SET_STAT_OBJECT_LIF_FIELDS(info->flags, BCM_PPP_INIT_STAT_ENABLE);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
                    (unit, entry_handle_id, (int *) local_outlif, &lif_table_mngr_outlif_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The tunnel Gport may be passed to the API by info->ppp_initiator_id or intf->l3a_intf_id. This function converts both
 *  parameters to GPort ids and returns the value, in addition it also returns indication of whether the found GPort
 *
 * \param [in] unit - Relevant unit
 * \param [in] ppp_type - PPP tunnel type
 * \param [out] dbal_table_id - Dbal table id.
 * \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \ref shr_error_e.
 */
static shr_error_e
dnx_ppp_initiator_get_table_id(
    int unit,
    bcm_ppp_type_t ppp_type,
    dbal_tables_e * dbal_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ppp_type == bcmPPPTypePPPoE)
    {
        *dbal_table_id = DBAL_TABLE_EEDB_PPPOE;
    }
    else if (ppp_type == bcmPPPTypeL2TPv2)
    {
        *dbal_table_id = DBAL_TABLE_EEDB_L2TP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PPP encapsulation: invalid type(%d)", ppp_type);
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * End of Inner functions
 * }
 */

/*
 * APIs
 * {
 */
/**
 * \brief
 *  Create PPP initiator object and set its properties.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [in] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [in] info->flag - See BCM_PPP_INIT_XXX \n
 *        BCM_PPP_INIT_WITH_ID - Create object with ID \n
 *        BCM_PPP_INIT_REPLACE - Update existing object \n
 *        BCM_PPP_INIT_STAT_ENABLE - Statistics enabled \n
 *   [in] info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [in] info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [in] info->encap_access - Encapsulation phase  \n
 *   [in] info->egress_qos_model - Egress qos and ttl model \n
 *   [in,out] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *    \retval Zero in case of NO ERROR.
 *    \retval Negative in case of an error, see \ref shr_error_e.
 */
int
bcm_dnx_ppp_initiator_create(
    int unit,
    bcm_ppp_initiator_t * info)
{
    uint32 local_outlif;
    int global_lif_id = 0;
    bcm_gport_t tunnel_id = 0;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 next_outlif_pointer = 0;
    uint32 lif_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_initiator_create_verify(unit, info));

    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

    /** WITH_ID flag is used - get global outLIF ID */
    if (_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_WITH_ID))
    {
        if (BCM_GPORT_IS_TUNNEL(info->ppp_initiator_id))
        {
            global_lif_id = BCM_GPORT_TUNNEL_ID_GET(info->ppp_initiator_id);
            lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        }
    }

    /** Allocate global lif if needed (not replace) */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_REPLACE))
    {
        /** Allocate LIF - WITH_ID if the flag was set */
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, lif_alloc_flags, DNX_ALGO_LIF_EGRESS, &global_lif_id));
    }

    BCM_GPORT_TUNNEL_ID_SET(tunnel_id, global_lif_id);

    /** Get next outlif if exist */
    if ((info->l3_intf_id != 0) && (info->l3_intf_id != BCM_IF_INVALID))
    {
        bcm_gport_t gport_tunnel;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_tunnel, info->l3_intf_id);
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gport_tunnel, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_resources));
        /** Next outlif for PPP */
        next_outlif_pointer = gport_hw_resources.local_out_lif;
    }
    else if (info->l3_intf_id == BCM_IF_INVALID)
    {
        next_outlif_pointer = DNX_ALGO_GPM_LIF_INVALID;
    }

    /** Allocate new tunnel outLIF - REPLACE flag is not set */
    if (!_SHR_IS_FLAG_SET(info->flags, BCM_PPP_INIT_REPLACE))
    {
        dbal_tables_e dbal_table_id = DBAL_TABLE_EEDB_PPPOE;

        SHR_IF_ERR_EXIT(dnx_ppp_initiator_get_table_id(unit, info->type, &dbal_table_id));

        /** Add entry to outLIF table */
        SHR_IF_ERR_EXIT(dnx_ppp_initiator_allocate_local_lif_and_out_lif_table_set
                        (unit, info, dbal_table_id, global_lif_id, next_outlif_pointer, &local_outlif));
    }
    /** Replace existing outLIF */
    else
    {
        uint32 lif_get_flags = 0;

        /** get GPort HW resources (global and local tunnel outlif) */
        lif_get_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, tunnel_id, lif_get_flags, &gport_hw_resources));

        /** Check that eep is an index of a Tunnel OutLIF (in SW DB) */
        if ((info->type == bcmPPPTypePPPoE) && (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_PPPOE))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non PPPoE-Tunnel OutLIF to PPPoE Tunnel OutLIF");
        }
        else if ((info->type == bcmPPPTypeL2TPv2) && (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_L2TP))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal to replace non L2TP-Tunnel OutLIF to L2TP Tunnel OutLIF");
        }

        local_outlif = (uint32) gport_hw_resources.local_out_lif;

        /** Update Next local OutLIF only in next_local_outlif */

        /** Add entry to outLIF table */
        SHR_IF_ERR_EXIT(dnx_ppp_initiator_allocate_local_lif_and_out_lif_table_set
                        (unit, info, gport_hw_resources.outlif_dbal_table_id,
                         global_lif_id, next_outlif_pointer, &local_outlif));

    }

    /** update returned Tunnel ID's */
    info->ppp_initiator_id = tunnel_id;
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get a PPP initiator object pointed by info->ppp_initiator_id.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [out] info->type - See bcmPPPTypeXXX \n
 *        bcmPPPTypePPPoE - PPPoE tunnel \n
 *        bcmPPPTypeL2TPv2 - L2TPv2 tunnel \n
 *   [out]info->flag - See BCM_PPP_INIT_XXX \n
 *        BCM_PPP_INIT_STAT_ENABLE - Statistics enabled \n
 *   [out]info->session_id - Session ID for both PPPoE and L2TPv2 \n
 *   [out]info->l2tpv2_tunnel_id - Tunnel ID for L2TPv2 \n
 *   [out]info->encap_access - Encapsulation phase  \n
 *   [out]info->egress_qos_model - Egress qos and ttl model \n
 *   [in] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 */
int
bcm_dnx_ppp_initiator_get(
    int unit,
    bcm_ppp_initiator_t * info)
{
    uint32 lif_flags, entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    lif_table_mngr_outlif_info_t outlif_info;
    uint8 is_field_valid = 0;
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verification */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_initiator_get_verify(unit, info));

    /** Get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->ppp_initiator_id, lif_flags, &gport_hw_resources));

    /** Get PPP tunnel types */
    if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_PPPOE)
    {
        info->type = bcmPPPTypePPPoE;
    }
    else if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_L2TP)
    {
        info->type = bcmPPPTypeL2TPv2;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "invalid PPP encapsulation type");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                    (unit, gport_hw_resources.local_out_lif, entry_handle_id, &outlif_info));

    /** Get statistics enable flag */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
    if (is_field_valid == TRUE)
    {
        info->flags |= BCM_PPP_INIT_STAT_ENABLE;
    }

    /** Get outLIF phase */
    if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1)
    {
        info->encap_access = bcmEncapAccessRif;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2)
    {
        info->encap_access = bcmEncapAccessNativeArp;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3)
    {
        info->encap_access = bcmEncapAccessTunnel1;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4)
    {
        info->encap_access = bcmEncapAccessTunnel2;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5)
    {
        info->encap_access = bcmEncapAccessTunnel3;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6)
    {
        info->encap_access = bcmEncapAccessTunnel4;
    }
    else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7)
    {
        info->encap_access = bcmEncapAccessArp;
    }

    /** Get next outLIF pointer */
    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE, dbal_field));
    info->l3_intf_id = dbal_field[0];

    /** Get session id */
    sal_memset(dbal_field, 0, sizeof(dbal_field));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, dbal_field));
    if (info->type == bcmPPPTypePPPoE)
    {
        info->session_id = dbal_field[0];
    }
    else if (info->type == bcmPPPTypeL2TPv2)
    {
        info->l2tpv2_tunnel_id = (dbal_field[0] >> 16) & 0xffff;
        info->session_id = dbal_field[0] & 0xffff;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete a PPP initiator.
 *
 * \param [in] unit - Relevant unit.
 * \param [in] info - A pointer to the struct that holds information for the PPP initiator object \n
 *   [in] info->ppp_initiator_id - PPP tunnel Gport.
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 */
int
bcm_dnx_ppp_initiator_delete(
    int unit,
    bcm_ppp_initiator_t * info)
{
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verification */
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_initiator_delete_verify(unit, info));

    /** Get GPort hw resources (global and local tunnel outlif) */
    lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, info->ppp_initiator_id, lif_flags, &gport_hw_resources));

    /** Remove tunnel data from EEDB outlif table */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_outlif_info_clear(unit, gport_hw_resources.local_out_lif, 0));

    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources.global_out_lif));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Traverse PPP Initiator Info.
 * \param [in] unit - Relevant unit.
 * \param [in] additional_info - A pointer to the struct that holds addtional information
 * \param [in] cb - user callback
 * \param [in] user_data - user data
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_ppp_initiator_traverse(
    int unit,
    bcm_ppp_initiator_additional_info_t * additional_info,
    bcm_ppp_initiator_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_tables[2] = { 0 };
    uint32 table, nof_tables = 0;
    int local_outlif, global_lif_id = 0;
    int is_end;
    bcm_ppp_initiator_t info;
    lif_table_mngr_outlif_info_t outlif_info;
    uint8 is_field_valid = 0;
    uint32 dbal_field[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_ppp_initiator_traverse_verify(unit, additional_info, cb, user_data));

    /** Apply additional limit */
    if (additional_info->type == bcmPPPTypePPPoE)
    {
        dbal_tables[nof_tables] = DBAL_TABLE_EEDB_PPPOE;
        nof_tables++;
    }
    else if (additional_info->type == bcmPPPTypeL2TPv2)
    {
        dbal_tables[nof_tables] = DBAL_TABLE_EEDB_L2TP;
        nof_tables++;
    }
    else if (additional_info->type == bcmPPPTypeCount)
    {
        dbal_tables[nof_tables] = DBAL_TABLE_EEDB_PPPOE;
        nof_tables++;
        dbal_tables[nof_tables] = DBAL_TABLE_EEDB_L2TP;
        nof_tables++;
    }

    /** Iterate over all tables and all their entries */
    for (table = 0; table < nof_tables; table++)
    {
        /*
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            /** Get Local OutLIF */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, (uint32 *) &local_outlif));
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info(unit, local_outlif, entry_handle_id, &outlif_info));

            /** Get outLIF phase */
            if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1)
            {
                info.encap_access = bcmEncapAccessRif;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_2)
            {
                info.encap_access = bcmEncapAccessNativeArp;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_3)
            {
                info.encap_access = bcmEncapAccessTunnel1;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_4)
            {
                info.encap_access = bcmEncapAccessTunnel2;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_5)
            {
                info.encap_access = bcmEncapAccessTunnel3;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_6)
            {
                info.encap_access = bcmEncapAccessTunnel4;
            }
            else if (outlif_info.outlif_phase == LIF_MNGR_OUTLIF_PHASE_EXPLICIT_7)
            {
                info.encap_access = bcmEncapAccessArp;
            }

            /** Get statistics enable flag */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                            (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, &is_field_valid));
            if (is_field_valid == TRUE)
            {
                info.flags |= BCM_PPP_INIT_STAT_ENABLE;
            }

            /** Get next outLIF pointer */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                             (uint32 *) &info.l3_intf_id));

            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                            (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL, local_outlif,
                             &global_lif_id));
            BCM_GPORT_TUNNEL_ID_SET(info.ppp_initiator_id, global_lif_id);

            /** Get info about PPPoE */
            if (dbal_tables[table] == DBAL_TABLE_EEDB_PPPOE)
            {
                info.type = bcmPPPTypePPPoE;
                /** Get session id */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, &info.session_id));
            }
            /** Get info about L2TPv2 */
            else if (dbal_tables[table] == DBAL_TABLE_EEDB_L2TP)
            {
                info.type = bcmPPPTypeL2TPv2;
                /** Get session id and tunnel id */
                sal_memset(dbal_field, 0, sizeof(dbal_field));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, dbal_field));
                info.l2tpv2_tunnel_id = (dbal_field[0] >> 16) & 0xffff;
                info.session_id = dbal_field[0] & 0xffff;
            }
            if (cb != NULL)
            {
                /** Invoke callback function */
                SHR_IF_ERR_EXIT((*cb) (unit, &info, user_data));
            }
            /** Receive next entry in table.*/
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * End of APIs
 * }
 */
