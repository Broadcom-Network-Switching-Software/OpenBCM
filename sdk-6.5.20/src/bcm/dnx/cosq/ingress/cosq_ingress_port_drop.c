/** \file cosq_ingress_port_drop.c
 * 
 *
 * cosq ingree port drop API functions for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/cosq.h>
#include <shared/gport.h>

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/sand/sand_signals.h>
#include <bcm_int/dnx/switch/switch.h>

#include <shared/utilex/utilex_framework.h>

/*
 * Defines
 * {
 */
#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION  _SHR_PORT_PRD_ETH_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION   _SHR_PORT_PRD_TM_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION   _SHR_PORT_PRD_IP_MAP_INDICATION
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION _SHR_PORT_PRD_MPLS_MAP_INDICATION

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_SET(key)  ( key |= DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_SET(key)   ( key |= DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_SET(key)   ( key |= DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_SET(key) ( key |= DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_GET(key)  ( key & DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_GET(key)   ( key & DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_GET(key)   ( key & DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_GET(key) ( key & DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

#define DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_CLEAR(key)  ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_CLEAR(key)   ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_CLEAR(key)   ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_INDICATION )
#define DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_CLEAR(key) ( key &= ~DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_INDICATION )

/*
 * }
 */
/**
 * \brief - Decode user priority to is_tdm and priority
 */
static shr_error_e
dnx_cosq_ingress_port_drop_user_prio_decode(
    int unit,
    int user_prio,
    uint32 *prio,
    uint32 *is_tdm)
{
    SHR_FUNC_INIT_VARS(unit);
    if (user_prio == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM)
    {
        *is_tdm = 1;
        *prio = 3;
    }
    else
    {
        *is_tdm = 0;
        *prio = user_prio;
    }

    SHR_FUNC_EXIT;
}
/**
 * \brief - Encode priority and is_tdm to user priority
 */
static shr_error_e
dnx_cosq_ingress_port_drop_user_prio_encode(
    int unit,
    uint32 prio,
    uint32 is_tdm,
    int *user_prio)
{
    SHR_FUNC_INIT_VARS(unit);
    if (is_tdm)
    {
        *user_prio = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM;
    }
    else
    {
        *user_prio = prio;
    }

    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify common parameters for the ingress port drop
 *        module (PRD)
 */
static int
dnx_cosq_ingress_port_drop_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FLEXE_MAC_L1, ILKN_L1 and ETH_L1 port types are not supported on Ingress priority drop for port %d\n.",
                     port);
    }

    /** Verify port is NIF or FlexE MAC, not including ELK ports */
    if (!
        (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, FALSE, TRUE, TRUE)
         || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress port drop is not supported for port %d\n", port);
    }

    /** Verify that this is not a TDM only interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
    if (if_tdm_mode == DNX_ALGO_PORT_IF_TDM_ONLY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress port drop is not supported for TDM only interface (port %d)", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for dnx_cosq_ingress_port_set
 */
static int
dnx_cosq_ingress_port_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    int nof_profiles;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
        case bcmCosqControlIngressPortDropTpid2:
        case bcmCosqControlIngressPortDropTpid3:
        case bcmCosqControlIngressPortDropTpid4:
            if (arg > dnx_data_nif.prd.tpid_max_get(unit) || arg < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, TPID value %d is invalid", unit, arg);
            }
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            if (arg != 0 && arg != 1)   /* i.e. invalid value */
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be 0 or 1", unit, arg);
            }
            break;
        case bcmCosqControlIngressPortDropPortProfileMap:
        {
            SHR_IF_ERR_EXIT(imb_prd_nof_port_profiles_get(unit, port, &nof_profiles));

            if (arg > (nof_profiles - 1) || arg < 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid set argument %d, should be between 0 - %d", unit, arg,
                             (nof_profiles - 1));
            }
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
dnx_cosq_ingress_port_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_set_verify(unit, port, type, arg));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 0, arg));
            break;
        case bcmCosqControlIngressPortDropTpid2:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 1, arg));
            break;
        case bcmCosqControlIngressPortDropTpid3:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 2, arg));
            break;
        case bcmCosqControlIngressPortDropTpid4:
            SHR_IF_ERR_EXIT(imb_prd_tpid_set(unit, port, 3, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreIpDscp, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreMplsExp, arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreInnerTag, arg));
            break;
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdIgnoreOuterTag, arg));
            break;
        case bcmCosqControlIngressPortDropPortProfileMap:
            SHR_IF_ERR_EXIT(imb_prd_port_profile_map_set(unit, port, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_cosq_ingress_port_get
 */
static int
dnx_cosq_ingress_port_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "dnx_cosq_ingress_port_get_arg");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
dnx_cosq_ingress_port_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_get_verify(unit, port, type, arg));

    switch (type)
    {
        case bcmCosqControlIngressPortDropTpid1:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 0, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid2:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 1, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid3:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 2, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropTpid4:
            SHR_IF_ERR_EXIT(imb_prd_tpid_get(unit, port, 3, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreIpDscp, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreMplsExp, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreInnerTag, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropIgonreOuterTag:
            SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdIgnoreOuterTag, (uint32 *) arg));
            break;
        case bcmCosqControlIngressPortDropPortProfileMap:
            SHR_IF_ERR_EXIT(imb_prd_port_profile_map_get(unit, port, (uint32 *) arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify paramenters for
 *        bcm_dnx_cosq_ingress_port_drop_control_frame_get
 */
static int
dnx_cosq_ingress_port_drop_control_frame_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(control_frame_config, _SHR_E_PARAM, "control_frame_config");

    /*
     * check control frame index is valid
     */
    if (control_frame_index >= dnx_data_nif.prd.nof_control_frames_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, control frame index %d is out of range\n", unit, control_frame_index);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_control_frame_get_verify
                          (unit, port, flags, control_frame_index, control_frame_config));

    SHR_IF_ERR_EXIT(imb_prd_control_frame_get(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify paramenters for
 *        bcm_dnx_cosq_ingress_port_drop_control_frame_set
 */
static int
dnx_cosq_ingress_port_drop_control_frame_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint64 mac_da_max_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_control_frame_get_verify
                    (unit, port, flags, control_frame_index, control_frame_config));

    /*
     * check ether type code and mask are valid
     */
    if (control_frame_config->ether_type_code > dnx_data_nif.prd.ether_type_code_max_get(unit) ||
        control_frame_config->ether_type_code_mask > dnx_data_nif.prd.ether_type_code_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d or mask %d are out of range\n", unit,
                     control_frame_config->ether_type_code, control_frame_config->ether_type_code_mask);
    }
    /*
     * check MAC DA and MAC DA mas kare valid
     */
    COMPILER_64_ZERO(mac_da_max_val);
    COMPILER_64_MASK_CREATE(mac_da_max_val, UTILEX_PP_MAC_ADDRESS_NOF_BITS, 0);
    if (COMPILER_64_GT(control_frame_config->mac_da_val, mac_da_max_val)
        || COMPILER_64_GT(control_frame_config->mac_da_mask, mac_da_max_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, MAC DA " COMPILER_64_PRINTF_FORMAT " or mask " COMPILER_64_PRINTF_FORMAT
                     " are out of range\n", unit, COMPILER_64_PRINTF_VALUE(control_frame_config->mac_da_val),
                     COMPILER_64_PRINTF_VALUE(control_frame_config->mac_da_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_control_frame_set_verify
                          (unit, port, flags, control_frame_index, control_frame_config));

    SHR_IF_ERR_EXIT(imb_prd_control_frame_set(unit, port, control_frame_index, control_frame_config));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_custom_ether_type_get
 */
static int
dnx_cosq_ingress_port_drop_custom_ether_type_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 ether_type_code_min, ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(ether_type_val, _SHR_E_PARAM, "ether_type_val");

    /*
     * check ether type code is valid
     */
    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    if (ether_type_code < ether_type_code_min || ether_type_code > ether_type_code_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Eth type code %d is invalid. Valid Eth type codes are 1-6\n", ether_type_code);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_custom_ether_type_get_verify
                          (unit, port, flags, ether_type_code, ether_type_val));

    SHR_IF_ERR_EXIT(imb_prd_custom_ether_type_get(unit, port, ether_type_code, ether_type_val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_custom_ether_type_set
 */
static int
dnx_cosq_ingress_port_drop_custom_ether_type_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_min, ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check ether type code is valid
     */
    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    if (ether_type_code < ether_type_code_min || ether_type_code > ether_type_code_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Eth type code %d is invalid. Valid Eth type codes are 1-6\n", ether_type_code);
    }

    /*
     * check ether type val is valid
     */
    if (ether_type_val > dnx_data_nif.prd.ether_type_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type %d is out of range\n", unit, ether_type_val);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_custom_ether_type_set_verify
                          (unit, port, flags, ether_type_code, ether_type_val));

    SHR_IF_ERR_EXIT(imb_prd_custom_ether_type_set(unit, port, ether_type_code, ether_type_val));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_default_priority_get
 */
static int
dnx_cosq_ingress_port_drop_default_priority_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(default_priority, _SHR_E_PARAM, "default_priority");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_default_priority_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 *default_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_default_priority_get_verify(unit, port, flags, default_priority));

    SHR_IF_ERR_EXIT(imb_prd_properties_get(unit, port, imbImbPrdDefaultPriority, default_priority));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_default_priority_set
 */
static int
dnx_cosq_ingress_port_drop_default_priority_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 default_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check default priority is valid
     */
    if (default_priority >= dnx_data_nif.prd.nof_priorities_get(unit)
        && default_priority != BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, default priority %d is out of range\n", unit, default_priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_default_priority_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 default_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_default_priority_set_verify(unit, port, flags, default_priority));

    SHR_IF_ERR_EXIT(imb_prd_properties_set(unit, port, imbImbPrdDefaultPriority, default_priority));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_enable_get
 */
static int
dnx_cosq_ingress_port_drop_enable_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int *enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(enable_mode, _SHR_E_PARAM, "enable_mode");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_enable_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int *enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_enable_get_verify(unit, port, flags, enable_mode));

    SHR_IF_ERR_EXIT(imb_prd_enable_get(unit, port, enable_mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_enable_set
 */
static int
dnx_cosq_ingress_port_drop_enable_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    switch (enable_mode)
    {
        case bcmCosqIngressPortDropDisable:
        case bcmCosqIngressPortDropEnableHardStage:
        case bcmCosqIngressPortDropEnableHardAndSoftStage:
            /*
             * valid input
             */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, port %d: NIF priority drop enable mode %d is invalid\n", unit, port,
                         enable_mode);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_enable_set_verify(unit, port, flags, enable_mode));

    SHR_IF_ERR_EXIT(imb_prd_enable_set(unit, port, enable_mode));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_construct_get
 */
static int
dnx_cosq_ingress_port_drop_flex_key_construct_get_verify(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id, _SHR_E_PARAM, "key_id");
    SHR_NULL_CHECK(flex_key_config, _SHR_E_PARAM, "flex_key_config");

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, key_id->port));

    /*
     * check ether type code is valid
     */
    if (key_id->ether_type_code > dnx_data_nif.prd.ether_type_code_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d is out of range\n", unit, key_id->ether_type_code);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_construct_get(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_construct_get_verify
                          (unit, key_id, flags, flex_key_config));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_flex_key_construct_get(unit, key_id->port, key_id->ether_type_code, flex_key_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_construct_set
 */
static int
dnx_cosq_ingress_port_drop_flex_key_construct_set_verify(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    int offset_index;
    uint32 is_configurable_ether_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_flex_key_construct_get_verify(unit, key_id, flags, flex_key_config));

    /*
     * check offset array size is valid
     */
    if (flex_key_config->array_size != BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset array size in flex_key_config should be %d\n", unit,
                     BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS);
    }
    /*
     * check offsets in array are valid
     */
    for (offset_index = 0; offset_index < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_index)
    {
        if (flex_key_config->offset_array[offset_index] > dnx_data_nif.prd.flex_key_offset_key_max_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset %d is out of range\n", unit,
                         flex_key_config->offset_array[offset_index]);
        }
    }
    /*
     * check offset base is valid
     */
    switch (flex_key_config->offset_base)
    {
        case bcmCosqIngressPortDropFlexKeyBasePacketStart:
        case bcmCosqIngressPortDropFlexKeyBaseEndOfEthHeader:
        case bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, offset base %d is invalid\n", unit, flex_key_config->offset_base);
    }
    /*
     * if offset base is "end of header after eth" and the ether type code is configurable, than user must provide the ether type size
     * in all other cases, ether type size is expected to be -1
     */
    SHR_IF_ERR_EXIT(dnx_cosq_prd_is_custom_ether_type_get
                    (unit, key_id->port, key_id->ether_type_code, &is_configurable_ether_type));
    if (flex_key_config->offset_base == bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader
        && is_configurable_ether_type)
    {
        if (flex_key_config->ether_type_header_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, when setting a key for configurable ether type code %d, "
                         "and offset base is set to EndOfHeaderAfterEthHeader, ether type size should be provided\n",
                         unit, key_id->ether_type_code);
        }
    }
    else if (flex_key_config->ether_type_header_size != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "unit %d, ether type size should only be set for configurable ether types, when offste base is EndOfHeaderAfterEthHeader. "
                     "in all other cases it is expected to be -1\n", unit);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_construct_set(
    int unit,
    bcm_cosq_ingress_drop_flex_key_construct_id_t * key_id,
    uint32 flags,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_construct_set_verify
                          (unit, key_id, flags, flex_key_config));

    SHR_IF_ERR_EXIT(dnx_cosq_prd_flex_key_construct_set(unit, key_id->port, key_id->ether_type_code, flex_key_config));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_entry_get
 */
static int
dnx_cosq_ingress_port_drop_flex_key_entry_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(flex_key_info, _SHR_E_PARAM, "flex_key_info");

    /*
     * check TCAM entry index is valid
     */
    if (key_index >= dnx_data_nif.prd.nof_tcam_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, port %d: Flex key entry key_index %d is out of range\n", unit, port,
                     key_index);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    dnx_cosq_prd_tcam_entry_info_t entry_info;
    int offset_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_entry_get_verify
                          (unit, port, flags, key_index, flex_key_info));

    SHR_IF_ERR_EXIT(imb_prd_flex_key_entry_get(unit, port, key_index, &entry_info));

    flex_key_info->ether_code.value = entry_info.ether_code_val;
    flex_key_info->ether_code.mask = entry_info.ether_code_mask;
    flex_key_info->priority = entry_info.priority;
    /*
     * TBD: Handle TDM priority
     */

    for (offset_idx = 0; offset_idx < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_idx)
    {
        /*
         * zero the values first - just in case
         */
        flex_key_info->key_fields[offset_idx].value = 0;
        flex_key_info->key_fields[offset_idx].mask = 0;

        DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_GET(entry_info.offset_array_val, offset_idx,
                                           flex_key_info->key_fields[offset_idx].value);
        DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_GET(entry_info.offset_array_mask, offset_idx,
                                           flex_key_info->key_fields[offset_idx].mask);
    }
    flex_key_info->num_key_fields = BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_flex_key_entry_set
 */
static int
dnx_cosq_ingress_port_drop_flex_key_entry_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    int offset_index;
    uint32 priority, is_tdm;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_flex_key_entry_get_verify(unit, port, flags, key_index, flex_key_info));

    /** check entry information is valid */
    /*
     * check ether type code is valid
     */
    if (flex_key_info->ether_code.value > dnx_data_nif.prd.ether_type_code_max_get(unit) ||
        flex_key_info->ether_code.mask > dnx_data_nif.prd.ether_type_code_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, ether type code %d or mask %d is out of range\n", unit,
                     flex_key_info->ether_code.value, flex_key_info->ether_code.mask);
    }
    /*
     * check number of offsets in array
     */
    if (flex_key_info->num_key_fields > BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, number of key fields %d is out of range\n", unit,
                     flex_key_info->num_key_fields);
    }
    /*
     * check offsets in array
     */
    for (offset_index = 0; offset_index < flex_key_info->num_key_fields; ++offset_index)
    {
        if (flex_key_info->key_fields[offset_index].value > dnx_data_nif.prd.flex_key_offset_result_max_get(unit) ||
            flex_key_info->key_fields[offset_index].mask > dnx_data_nif.prd.flex_key_offset_result_max_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key_field value %d or mask %d is out of range\n", unit,
                         flex_key_info->key_fields[offset_index].value, flex_key_info->key_fields[offset_index].mask);
        }
    }
    /*
     * Verify if TDM can be configured.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_user_prio_decode(unit, flex_key_info->priority, &priority, &is_tdm));
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_usage_verify(unit, is_tdm));
    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit) && !is_tdm)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, flex_key_info->priority);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 key_index,
    bcm_cosq_ingress_drop_flex_key_entry_t * flex_key_info)
{
    dnx_cosq_prd_tcam_entry_info_t entry_info;
    int offset_idx;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_flex_key_entry_set_verify
                          (unit, port, flags, key_index, flex_key_info));

    entry_info.ether_code_val = flex_key_info->ether_code.value;
    entry_info.ether_code_mask = flex_key_info->ether_code.mask;
    entry_info.priority = flex_key_info->priority;
    /*
     * TBD: Handle TDM priority
     */
    entry_info.is_entry_valid = 1;
    entry_info.offset_array_val = 0;
    entry_info.offset_array_mask = 0;
    entry_info.tdm = 0;

    for (offset_idx = 0; offset_idx < flex_key_info->num_key_fields; ++offset_idx)
    {
        DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_SET(flex_key_info->key_fields[offset_idx].value, offset_idx,
                                           entry_info.offset_array_val);
        DNX_COSQ_PRD_TCAM_ENTRY_OFFSET_SET(flex_key_info->key_fields[offset_idx].mask, offset_idx,
                                           entry_info.offset_array_mask);
    }

    SHR_IF_ERR_EXIT(imb_prd_flex_key_entry_set(unit, port, key_index, &entry_info));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the key is valid and that key matches the
 *        correct PRD map for
 *        bcm_dnx_cosq_ingress_port_drop_map_set/get APIs
 */
static int
dnx_cosq_ingress_port_drop_map_key_validate(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key)
{
    uint32 key_mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_TM_KEY_MASK;
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_IP_KEY_MASK;
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_ETH_KEY_MASK;
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            if (DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_GET(*key) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is not a match to map %d\n", unit, *key, map);
            }
            DNX_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_CLEAR(*key);
            key_mask = _SHR_PORT_PRD_MPLS_KEY_MASK;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, invalid map %d\n", unit, map);
    }

    if (*key & ~key_mask)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, key %d is invalid for map %d\n", unit, *key, map);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_map_get
 */
static int
dnx_cosq_ingress_port_drop_map_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");

    /*
     * check map and key are valid and key match the correct map
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_map_key_validate(unit, port, map, key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_map_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    int *priority)
{
    uint32 prd_priority, is_tdm;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_map_get_verify(unit, port, flags, map, &key, priority));

    SHR_IF_ERR_EXIT(imb_prd_map_get(unit, port, map, key, &prd_priority, &is_tdm));
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_user_prio_encode(unit, prd_priority, is_tdm, priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_map_set
 */
static int
dnx_cosq_ingress_port_drop_map_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 *key,
    int priority)
{
    dnx_algo_port_info_s port_info;
    uint32 prio, is_tdm;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check map and key are valid and key match the correct map
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_map_key_validate(unit, port, map, key));

    /*
     * check priority is valid
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_user_prio_decode(unit, priority, &prio, &is_tdm));

    /*
     * Verify if TDM can be configured.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_usage_verify(unit, is_tdm));

    /** verify prio, TDM is allowed.*/
    if (prio >= dnx_data_nif.prd.nof_priorities_get(unit) && !is_tdm)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, prio);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (is_tdm && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "In ILKN ports, is_tdm indication set per channel\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_map_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    int priority)
{
    uint32 prio, is_tdm;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_map_set_verify(unit, port, flags, map, &key, priority));

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_user_prio_decode(unit, priority, &prio, &is_tdm));
    SHR_IF_ERR_EXIT(imb_prd_map_set(unit, port, map, key, prio, is_tdm));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_threshold_get
 */
static int
dnx_cosq_ingress_port_drop_threshold_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_threshold_get_verify(unit, port, flags, priority, value));

    SHR_IF_ERR_EXIT(imb_prd_threshold_get(unit, port, priority, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_threshold_set
 */
static int
dnx_cosq_ingress_port_drop_threshold_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 value)
{
    dnx_algo_port_info_s port_info;
    uint32 max_threshold;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * check priority is valid
     */
    if (priority >= dnx_data_nif.prd.nof_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range, max priority is %d\n", unit, priority,
                     dnx_data_nif.prd.nof_priorities_get(unit) - 1);
    }
    /*
     * Check threshold value is valid
     */
    SHR_IF_ERR_EXIT(imb_prd_threshold_max_get(unit, port, &max_threshold));

    if (value > max_threshold)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, threshold %d is out of range, max threshold is %d\n", unit, value,
                     max_threshold);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int priority,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_threshold_set_verify(unit, port, flags, priority, value));

    SHR_IF_ERR_EXIT(imb_prd_threshold_set(unit, port, priority, value));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_mpls_special_label_get
 */
static int
dnx_cosq_ingress_port_drop_mpls_special_label_get_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(label_config, _SHR_E_PARAM, "label_config");

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_verify(unit, port));

    /*
     * check label index is valid
     */
    if (label_index >= dnx_data_nif.prd.nof_mpls_special_labels_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, label index %d is invalid\n", unit, label_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_mpls_special_label_get_verify
                          (unit, port, flags, label_index, label_config));

    SHR_IF_ERR_EXIT(imb_prd_mpls_special_label_get(unit, port, label_index, label_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for
 *        bcm_dnx_cosq_ingress_port_drop_mpls_special_label_set
 */
static int
dnx_cosq_ingress_port_drop_mpls_special_label_set_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_port_drop_mpls_special_label_get_verify
                    (unit, port, flags, label_index, label_config));

    /*
     * Verify if TDM can be configured.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_usage_verify(unit, label_config->is_tdm));

    /*
     * check label value is valid
     */
    if (label_config->label_value > dnx_data_nif.prd.mpls_special_label_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, label value %d is invalid\n", unit, label_config->label_value);
    }
    /*
     * check priority is valid
     */
    if (label_config->priority >= dnx_data_nif.prd.nof_priorities_get(unit) && !label_config->is_tdm)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, priority %d is out of range\n", unit, label_config->priority);
    }
    /*
     * check is_tdm is valid
     */
    if (label_config->is_tdm > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, is_tdm %d is out of range\n", unit, label_config->is_tdm);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
bcm_dnx_cosq_ingress_port_drop_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_ingress_port_drop_mpls_special_label_set_verify
                          (unit, port, flags, label_index, label_config));

    SHR_IF_ERR_EXIT(imb_prd_mpls_special_label_set(unit, port, label_index, label_config));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - get outer ITMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int
dnx_cosq_prd_itmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset)
{
    int size, offset;
    int itmh_size, itmh_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base", &itmh_offset, &itmh_size));

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base.TC", &offset, &size));
    *tc_offset = itmh_size - offset - size;
    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "ITMH_Base.DP", &offset, &size));
    *dp_offset = itmh_size - offset - size;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get outer FTMH header TC (Traffic Class) and DP (Drop Precedence) field offsets
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] tc_offset - TC (Traffic Class) field offset
 * \param [out] dp_offset - DP (Drop Precedence) field offset
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_ftmh_offsets_get(
    int unit,
    bcm_port_t port,
    int *tc_offset,
    int *dp_offset)
{
    int size, offset;
    int ftmh_size, ftmh_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base", &ftmh_offset, &ftmh_size));

    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base.FTMH_PP_TC", &offset, &size));
    *tc_offset = ftmh_size - offset - size;
    SHR_IF_ERR_EXIT(sand_signal_field_params_get(unit, "FTMH_Base.FTMH_DP", &offset, &size));
    *dp_offset = ftmh_size - offset - size;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_ingress_port.h
 */
int
dnx_cosq_prd_outer_tag_size_get(
    int unit,
    bcm_port_t port,
    uint32 *outer_tag_size)
{
    int port_ext_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlExtenderType, &port_ext_type));

    switch (port_ext_type)
    {
        case BCM_PORT_EXTENDER_TYPE_SWITCH:
            *outer_tag_size = DBAL_ENUM_FVAL_VLAN_OUTER_TAG_SIZE_8B;
            break;
        case BCM_PORT_EXTENDER_TYPE_NONE:
            *outer_tag_size = DBAL_ENUM_FVAL_VLAN_OUTER_TAG_SIZE_4B;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported extender type %d\n", port_ext_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_ethu_prd_flex_key_construct_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_ether_type_size_set(
    int unit,
    bcm_port_t port)
{
    const dnxc_data_table_info_t *ether_type_table_info;
    uint32 ether_type_code, nof_ether_types, ether_type_size;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_table_info = dnx_data_nif.prd.ether_type_info_get(unit);
    nof_ether_types = ether_type_table_info->key_size[0];
    /*
     * Set Ether type size for the fixed ether types. the configurable ether types sizes should be defined by the user
     * in API imb_ethu_prd_flex_key_construct_set in using offset base of EndOfHeaderAfterEthHeader
     */
    for (ether_type_code = 0; ether_type_code < nof_ether_types; ++ether_type_code)
    {
        ether_type_size = dnx_data_nif.prd.ether_type_get(unit, ether_type_code)->ether_type_size;
        if (ether_type_size > 0)
        {
            SHR_IF_ERR_EXIT(imb_prd_ether_type_size_set(unit, port, ether_type_code, ether_type_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD port type from port header type. in addition
 *        get TM/Eth port indication, depending on the header
 *        type of the port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] prd_port_type - PRD port type. see
 *        NIF_PRD_PORT_TYPE in DBAL
 * \param [in] is_eth_port - is eth port type
 * \param [in] is_tm_port - is TM port type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_port_type_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_port_type,
    uint32 *is_eth_port,
    uint32 *is_tm_port)
{
    int header_type;
    SHR_FUNC_INIT_VARS(unit);

    *is_eth_port = 0;
    *is_tm_port = 0;

    SHR_IF_ERR_EXIT(dnx_switch_header_type_get(unit, port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &header_type));

    switch (header_type)
    {
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_ETH;
            *is_eth_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TDM_ETH:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_TDM_ETH_HYBRID;
            *is_eth_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TM:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_ITMH;
            *is_tm_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_STACKING:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_FTMH;
            *is_tm_port = 1;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP:
        case BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2:
            *prd_port_type = DBAL_ENUM_FVAL_NIF_PRD_PORT_TYPE_PTCH_2;
            *is_eth_port = 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unsupported header type %d\n", header_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * See cosq_ingress_port.h
 */
int
dnx_cosq_prd_flex_key_construct_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    const bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    uint32 ether_type_code_min, ether_type_code_max;
    int offset_index, is_configurable_ether_type;
    SHR_FUNC_INIT_VARS(unit);

    for (offset_index = 0; offset_index < flex_key_config->array_size; ++offset_index)
    {
        SHR_IF_ERR_EXIT(imb_prd_tcam_entry_key_offset_set
                        (unit, port, ether_type_code, offset_index, flex_key_config->offset_array[offset_index]));
    }

    SHR_IF_ERR_EXIT(imb_prd_tcam_entry_key_offset_base_set(unit, port, ether_type_code, flex_key_config->offset_base));

    ether_type_code_min = dnx_data_nif.prd.custom_ether_type_code_min_get(unit);
    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    is_configurable_ether_type = (ether_type_code >= ether_type_code_min) && (ether_type_code <= ether_type_code_max);

    /*
     * only set the ether type size for configurable ether types with offset base of "end of header after eth"
     */
    if (is_configurable_ether_type
        && flex_key_config->offset_base == bcmCosqIngressPortDropFlexKeyBaseEndOfHeaderAfterEthHeader)
    {
        SHR_IF_ERR_EXIT(imb_prd_ether_type_size_set
                        (unit, port, ether_type_code, flex_key_config->ether_type_header_size));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get key constructed per ether type code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code of the key to
 *        retrieve
 * \param [in] flex_key_config - returned key info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_flex_key_construct_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    bcm_cosq_ingress_drop_flex_key_construct_t * flex_key_config)
{
    uint32 offset_index, is_mpls;
    SHR_FUNC_INIT_VARS(unit);

    for (offset_index = 0; offset_index < BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS; ++offset_index)
    {
        SHR_IF_ERR_EXIT(imb_prd_tcam_entry_key_offset_get
                        (unit, port, ether_type_code, offset_index, &flex_key_config->offset_array[offset_index]));
    }
    flex_key_config->array_size = BCM_COSQ_INGRESS_PORT_DROP_MAX_FLEX_KEY_FIELDS;

    SHR_IF_ERR_EXIT(imb_prd_tcam_entry_key_offset_base_get(unit, port, ether_type_code, &flex_key_config->offset_base));

    /*
     * MPLS protocol has no size in HW register, since it is automatically calculated,
     * so before accessing the DBAL, we need to make sure we dont have MPLS
     */
    is_mpls = sal_strcmp(dnx_data_nif.prd.ether_type_get(unit, ether_type_code)->ether_type_name, "MPLS") ? 0 : 1;
    if (!is_mpls)
    {
        SHR_IF_ERR_EXIT(imb_prd_ether_type_size_get
                        (unit, port, ether_type_code, &flex_key_config->ether_type_header_size));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get the correct RMC of the port to which the PRD
 *        priority is mapped
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to look for
 * \param [in] rmc - returned RMC for the Priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_threshold_rmc_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    dnx_algo_port_rmc_info_t * rmc)
{
    uint32 priority_flag = 0, max_rmcs_per_port, rmc_found = 0;
    int rmc_index;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.eth.priority_groups_nof_get(unit);

    switch (priority)
    {
        case 0:
            priority_flag = BCM_PORT_F_PRIORITY_0;
            break;
        case 1:
            priority_flag = BCM_PORT_F_PRIORITY_1;
            break;
        case 2:
            priority_flag = BCM_PORT_F_PRIORITY_2;
            break;
        case 3:
            priority_flag = BCM_PORT_F_PRIORITY_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Priority %d\n", priority);
    }

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, rmc));
        if ((rmc->rmc_id != IMB_COSQ_RMC_INVALID) && (rmc->prd_priority & priority_flag))
        {
            rmc_found = 1;
            break;
        }
    }

    if (!rmc_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority %d is not mapped to any of the port's priority groups\n", priority);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return true if the ether type code is configurable
 *        to the user. the PRD parser recognizes several ether
 *        types - some are fixed values, and some are open for
 *        user preference.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to check
 * \param [in] is_custom_ether_type - returned indication
 *        whether the ether type is configurable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_cosq_prd_is_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *is_custom_ether_type)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_custom_ether_type = (ether_type_code >= dnx_data_nif.prd.custom_ether_type_code_min_get(unit) &&
                             ether_type_code <= dnx_data_nif.prd.custom_ether_type_code_max_get(unit));

    SHR_FUNC_EXIT;

}

#undef _ERR_MSG_MODULE_NAME
