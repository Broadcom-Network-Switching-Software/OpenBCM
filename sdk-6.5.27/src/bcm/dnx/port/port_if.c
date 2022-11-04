/**
 * \file port_if.c $Id$ PORT Interfaces procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/dnxc_port.h>

#include <shared/bslenum.h>
#include <shared/util.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_ile.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_arb.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_oft.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_calendar.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/dnx/cosq/egress/rate_measurement.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_ilkn_reg_access.h>
#include <bcm_int/dnx/port/port_custom_reg_access.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/port/port_signal_quality.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <bcm_int/dnx/failover/failover.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/dnx/tune/ecgm_tune_dispatch.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/auto_generated/dnx_tdm_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <soc/sand/sand_aux_access.h>
#include "port_utils.h"
#include "nif/dnx_port_nif_arb_internal.h"
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <soc/counter.h>
#include <soc/sand/sand_mem.h>
#include <soc/portmod/portmod.h>
#include <soc/link.h>
#include <sal/core/boot.h>
#include <soc/util.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/auto_generated/dnx_xflow_macsec_dispatch.h>
#endif

/*
 * }
 */

/*
 * Structs
 * {
 */

/**
 * \brief
 *   Information needed for handling bcm_port_resource_multi_set
 */
typedef struct
{
    /** Original user's port */
    bcm_gport_t original_port;
    /** type of the port */
    dnx_algo_port_info_s port_info;
    /** If the tx speed has been configured */
    int has_tx_speed;
    /** If the tx speed is going to be changed */
    int is_tx_speed_change;
    /** If the rx speed is going to be changed */
    int is_rx_speed_change;
    /** is port enabled */
    int is_port_enable;
    /** original rx speed*/
    int original_rx_speed;
    /** original tx speed*/
    int original_tx_speed;
} dnx_port_resource_port_info_t;

/*
 * }
 */

/*
 * Macros.
 * {
 */
/**
 * \brief - supported flags for API bcm_dnx_lane_to_serdes_map_set()
 */
#define DNX_PORT_LANE_MAP_SUPPORTED_FLAGS  (BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)

#define FLEXE_L1_PORT_NOF_PRIO_GROUPS 1
/* Port min priority groups. */
#define DNX_PORT_MIN_NOF_PRIORITY_GROUPS    1

/*
 * }
 */

static int dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value);

static int dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value);

static shr_error_e dnx_port_hw_init(
    int unit);

/**
 * \brief - INTERNAL FUNCTION initialize the Port module
 *
 * \param [in] unit - chip unit id.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e
dnx_port_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Create SW State instance (if not initialized already)
         */
        if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported)
            || dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported)
            || dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
        {
            SHR_IF_ERR_EXIT(dnx_port_nif_db.init(unit));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.g_hao_active_client.set(unit, -1));
            SHR_IF_ERR_EXIT(dnx_port_nif_calendars_db_init(unit));
        }

        /*
         * Initializing OFR block if supported
         */
        if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
        {
            if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_used))
            {
                SHR_IF_ERR_EXIT(dnx_port_ofr_init(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_ofr_power_down(unit));
            }
        }

        /*
         * Initializing OFT block if supported
         */
        if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
        {
            if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_used))
            {
                SHR_IF_ERR_EXIT(dnx_port_oft_init(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_oft_power_down(unit));
            }

        }

        /*
         * Initializing arbiter block if supported
         */
        if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
        {
            if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_used))
            {
                SHR_IF_ERR_EXIT(dnx_port_arb_init(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_arb_power_down(unit));
            }
        }
    }

    /** init IMB  module*/
    SHR_IF_ERR_EXIT(imb_init_all(unit));

    /** init custumer reg/memmory access */
    SHR_IF_ERR_EXIT(dnx_custom_access_init(unit));

    /** init HW */
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_port_hw_init(unit));

        /*
         * Initializing signal quality BER modules
         */
        if (dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
        {
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_init(unit));
        }
    }
    else
    {
        /*
         * clear signal quality stale state information
         */
        if (dnx_data_nif.signal_quality.feature_get(unit, dnx_data_nif_signal_quality_is_supported))
        {
            SHR_IF_ERR_EXIT(dnx_port_signal_quality_er_state_db_clear(unit));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize the Port module
 *
 * \param [in] unit - chip unit id.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e
dnx_port_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** deinit custumer access */
    SHR_IF_ERR_EXIT(dnx_custom_access_deinit(unit));
    /** deinit IMB  module*/
    SHR_IF_ERR_EXIT(imb_deinit_all(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_autoneg_ability_advert_get
 *     bcm_dnx_port_autoneg_ability_advert_set
 *     bcm_dnx_port_speed_ability_local_get
 *     bcm_dnx_port_autoneg_ability_remote_get
 */

static shr_error_e
dnx_port_autoneg_ability_verify(
    int unit,
    bcm_port_t port,
    int num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability,
    int is_get)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(abilities, _SHR_E_PARAM, "speed abilities");
    /*
     * For GET API, need to verify the actual_num_ability parameter.
     */
    if (is_get)
    {
        SHR_NULL_CHECK(actual_num_ability, _SHR_E_PARAM, "actual_num_ability");
    }

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    if (!is_get && (num_ability > dnx_data_nif.eth.an_max_nof_abilities_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max auto-negotiation abilities is %d.\r\n",
                     dnx_data_nif.eth.an_max_nof_abilities_get(unit));
    }
    if (num_ability <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max auto-negotiation abilities must be greater than 0.\r\n");
    }

    if (!is_get)
    {
        /*
         * IMB type specified ability verification
         */
        SHR_IF_ERR_EXIT(imb_port_autoneg_ability_verify(unit, port, num_ability, abilities));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_autoneg_ability_advert_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of autoneg ability this port can advertise
 *      ability - (OUT) values indicating the ability this port advertise
 *      actual_num_ability - (OUT) the actual num of ability that this port advertise
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_autoneg_ability_advert_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_ability_verify
                           (unit, port, max_num_ability, abilities, actual_num_ability, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_advert_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *      bcm_port_autoneg_ability_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      num_ability - number of ability the port will advertise
 *      ability - Local advertisement for each ability.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call WILL NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

shr_error_e
bcm_dnx_port_autoneg_ability_advert_set(
    int unit,
    bcm_port_t port,
    int num_ability,
    bcm_port_speed_ability_t * abilities)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_ability_verify(unit, port, num_ability, abilities, NULL, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_advert_set(unit, logical_port, num_ability, abilities));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_speed_ability_local_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of speed ability this port support
 *      ability - (OUT) values indicating the ability of the MAC/PHY
 *      actual_num_ability - (OUT) the actual num of ability that this port can support
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_speed_ability_local_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_ability_verify
                           (unit, port, max_num_ability, abilities, actual_num_ability, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_speed_ability_local_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_autoneg_ability_advert_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of speed ability remote partner can advertise
 *      ability - (OUT) values indicating the ability of the MAC/PHY
 *      actual_num_ability - (OUT) the actual num of ability that remote partner advertise
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_autoneg_ability_remote_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_ability_verify
                           (unit, port, max_num_ability, abilities, actual_num_ability, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_remote_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_autoneg_get
 *     bcm_dnx_port_autoneg_set
 */

static shr_error_e
dnx_port_autoneg_verify(
    int unit,
    bcm_port_t port,
    int *autoneg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(autoneg, _SHR_E_PARAM, "autoneg");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve autoneg enable indication
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] autoneg - enable indication
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_autoneg_get(
    int unit,
    bcm_port_t port,
    int *autoneg)
{
    bcm_port_t logical_port = 0;
    phymod_autoneg_control_t an;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_verify(unit, port, autoneg));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    phymod_autoneg_control_t_init(&an);
    /*
     *Not supported for Fabric ports - IMB will return an error.
     */
    SHR_IF_ERR_EXIT(imb_port_autoneg_get(unit, logical_port, 0 /* flags */ , &an));
    *autoneg = an.enable;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable autoneg
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] autoneg - enable indication
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_autoneg_set(
    int unit,
    bcm_port_t port,
    int autoneg)
{
    bcm_port_t logical_port = 0;
    phymod_autoneg_control_t an;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_autoneg_verify(unit, port, &autoneg));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    phymod_autoneg_control_t_init(&an);
    /*
     *Not supported for Fabric ports - IMB will return an error.
     */
    SHR_IF_ERR_EXIT(imb_port_autoneg_get(unit, logical_port, 0 /* flags */ , &an));
    an.enable = autoneg;
    SHR_IF_ERR_EXIT(imb_port_autoneg_set(unit, logical_port, 0 /* flags */ , &an));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_config_get
 */

static shr_error_e
dnx_port_config_get_verify(
    int unit,
    bcm_port_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "port_config");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve all ports configuration info.
 *
 * \param [in] unit - chip unit id.
 * \param [out] config - configuration info.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_config_get(
    int unit,
    bcm_port_config_t * config)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_config_get_verify(unit, config));

    bcm_port_config_t_init(config);

    {
        pbmp_t *pbmp_p;

        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_IL_ALL_BITMAP, &pbmp_p));
        config->il = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_XL_ALL_BITMAP, &pbmp_p));
        config->xl = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CE_ALL_BITMAP, &pbmp_p));
        config->ce = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_XE_ALL_BITMAP, &pbmp_p));
        config->xe = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_HG_ALL_BITMAP, &pbmp_p));
        config->hg = *pbmp_p;

        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_SFI_ALL_BITMAP, &pbmp_p));
        config->sfi = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_PORT_ALL_BITMAP, &pbmp_p));
        config->port = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &pbmp_p));
        config->cpu = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_RCY_ALL_BITMAP, &pbmp_p));
        config->rcy = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_E_ALL_BITMAP, &pbmp_p));
        config->e = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_GE_ALL_BITMAP, &pbmp_p));
        config->ge = *pbmp_p;

        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_ALL_BITMAP, &pbmp_p));
        config->all = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CDE_BITMAP, &pbmp_p));
        config->cd = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_LE_BITMAP, &pbmp_p));
        config->le = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CC_BITMAP, &pbmp_p));
        config->cc = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_D3CE_BITMAP, &pbmp_p));
        config->d3c = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_FLEXE_CLIENT_BITMAP, &pbmp_p));
        config->flexe_client = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_FLEXE_MAC_BITMAP, &pbmp_p));
        config->framer_mac = *pbmp_p;
        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_FLEXE_SAR_BITMAP, &pbmp_p));
        config->framer_sar = *pbmp_p;
    }

    /** Special ports that are not included in SOC PBMP, require iteration over all ports */
    {
        _SHR_PBMP_CLEAR(config->erp);
        _SHR_PBMP_CLEAR(config->olp);
        _SHR_PBMP_CLEAR(config->oamp);
        _SHR_PBMP_CLEAR(config->eventor);
        _SHR_PBMP_CLEAR(config->sat);
        _SHR_PBMP_CLEAR(config->rcy_mirror);
        _SHR_PBMP_CLEAR(config->stat);
        _SHR_PBMP_CLEAR(config->crps);
        _SHR_PBMP_CLEAR(config->otn);

        /** Get all ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0     /* flags 
                                                                                                                 */ , &pbmp));
        /** iterate over the ports bitmap and search for the right port types */
        _SHR_PBMP_ITER(pbmp, port)
        {
            /** get current port type */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            /** ERP port */
            if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
                                                            /** ERP port */
            {
                _SHR_PBMP_PORT_ADD(config->erp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_info))
                                                                 /** OLP port*/
            {
                _SHR_PBMP_PORT_ADD(config->olp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_info))
                                                                  /** OAMP port */
            {
                _SHR_PBMP_PORT_ADD(config->oamp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_EVENTOR(unit, port_info))
                                                                     /** EVENTOR port */
            {
                _SHR_PBMP_PORT_ADD(config->eventor, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_info))
                                                                 /** SAT port */
            {
                _SHR_PBMP_PORT_ADD(config->sat, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
                                                                        /** RCY_MIRROR port */
            {
                _SHR_PBMP_PORT_ADD(config->rcy_mirror, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
                                                                           /** STAT port */
            {
                _SHR_PBMP_PORT_ADD(config->stat, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_CRPS(unit, port_info))
                                                                   /** CRPS port */
            {
                _SHR_PBMP_PORT_ADD(config->crps, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))  /** OTN port */
            {
                _SHR_PBMP_PORT_ADD(config->otn, port);
            }
        }
    }

    /** NIF ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF        /* flags 
                                                                                                                                                                                         */ ,
                                               &config->nif));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_internal_get() API
 */
static shr_error_e
dnx_port_internal_get_verify(
    int unit,
    uint32 flags,
    int internal_ports_max,
    bcm_gport_t * internal_gport,
    int *internal_ports_count)
{
    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(internal_gport, _SHR_E_PARAM, "internal_gport");
    SHR_NULL_CHECK(internal_ports_count, _SHR_E_PARAM, "internal_ports_count");
    /** Supported flags check */
    if (flags & BCM_PORT_INTERNAL_RECYCLE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Recycle ports are no longer internal - they are available in pbmp.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - returns gport of type "local" or "mod:port"
 * This returns handle to port. Not to Interface. From a port the interface handle is retrieved
 * via bcm_petra_fabric_port_get() API.
 * Following internal functionality required by other modules (internal functions)
 *    - convert the port to "fap port"
 *    - get the port type (e.g. to indicate that some operations cannot occur on some port types)
 *
 * \param [in] unit - Unit ID
 * \param [in] flags -  Flags that determine the requested port type. See flags starting with BCM_PORT_INTERNAL_CONF...
 * \param [in] internal_ports_max - Number of ports to get
 * \param [in] internal_gport - Array of returned gports
 * \param [in] internal_ports_count - Number of actual returned ports
 *
 * \return
 *   int
 *
 * \remark
 *   * This API is legacy, there is a better API that returns all port types bitmaps.
 * \see
 *   * bcm_port_config_get API
 */
int
bcm_dnx_port_internal_get(
    int unit,
    uint32 flags,
    int internal_ports_max,
    bcm_gport_t * internal_gport,
    int *internal_ports_count)
{
    int count = 0;
    bcm_gport_t gport;
    int core, core_i, nof_cores;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_internal_get_verify(unit, flags, internal_ports_max, internal_gport,
                                                        internal_ports_count));
    /** Initialize */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    /** Determine the requested core */
    if ((flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0) && (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1))
    {
        core = BCM_CORE_ALL;
    }
    else if (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0)
    {
        core = 0;
    }
    else if (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1)
    {
        if (nof_cores < 2)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Requested Invalid core\n");
        }
        core = 1;
    }
    else
    {
        core = BCM_CORE_ALL;
    }

    /** Return the requested ports */
    DNXCMN_CORES_ITER(unit, core, core_i)
    {
        /** get tm ports on the requested core */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_i, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0      /* flags 
                                                                                                                 */ , &pbmp));
        /** iterate over the ports bitmap and search for the right port types */
        _SHR_PBMP_ITER(pbmp, port)
        {
            /** get current port type */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            /** ERP port */
            if ((flags & BCM_PORT_INTERNAL_EGRESS_REPLICATION) && DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding ERP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }

            /** OLP port*/
            if ((flags & BCM_PORT_INTERNAL_OLP) && DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_info))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding OLP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }

            /** OAMP port */
            if ((flags & BCM_PORT_INTERNAL_OAMP) && DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_info))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding OAMP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }
        }
    }

    *internal_ports_count = count;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_detach
 */

static shr_error_e
dnx_port_detach_verify(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    int fabric_port_base;
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(detached, _SHR_E_PARAM, "detached_ports");
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    BCM_PBMP_ITER(pbmp, port)
    {
        if (port < fabric_port_base)
        {
            SHR_ERR_EXIT(_SHR_E_PORT,
                         "bcm_port_detach API is only supported for Fabric ports. please see bcm_port_remove.\n");
        }

        if (!dnx_drv_is_sfi_port(unit, port))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Port %d is not probed. Can't detach ports that aren't probed.\n", port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Detach all ports in pbmp bitmap.
 *
 * \param [in] unit - chip unit id.
 * \param [in] pbmp - bitmap of ports to be detached
 * \param [out] detached - ports actually detached
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * This method is not supported for NIF ports,
 *   see bcm_port_remove instead.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_detach_verify(unit, pbmp, detached));
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_detach(unit, pbmp, detached));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_duplex_set
 *     bcm_dnx_port_duplex_get
 */

static shr_error_e
dnx_port_duplex_verify(
    int unit,
    bcm_port_t port,
    int *duplex)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(duplex, _SHR_E_PARAM, "duplex_mode");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the port duplex mode.
 * 0 => Half duplex
 * 1 => Full duplex
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] duplex - duplex mode.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * BCM_PORT_DUPLEX_*
 */
shr_error_e
bcm_dnx_port_duplex_get(
    int unit,
    bcm_port_t port,
    int *duplex)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_duplex_verify(unit, port, duplex));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /*
     *Not supported for Fabric ports - IMB will return an error.
     */
    SHR_IF_ERR_EXIT(imb_port_duplex_get(unit, logical_port, duplex));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the port duplex mode.
 * 0 => Half duplex
 * 1 => Full duplex
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] duplex - duplex mode.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * BCM_PORT_DUPLEX_*
 */
shr_error_e
bcm_dnx_port_duplex_set(
    int unit,
    bcm_port_t port,
    int duplex)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_duplex_verify(unit, port, &duplex));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /*
     *Not supported for Fabric ports - IMB will return an error.
     */
    SHR_IF_ERR_EXIT(imb_port_duplex_set(unit, logical_port, duplex));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_enable_set
 *     bcm_dnx_port_enable_get
 */

static shr_error_e
dnx_port_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve enable status for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_enable_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    SHR_IF_ERR_CONT(imb_port_enable_get(unit, logical_port, enable));
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    if (SHR_FUNC_ERR())
    {
        SHR_EXIT();
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable a port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in] enable - enable indication
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    int current_enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_enable_verify(unit, port, &enable));
    /*
     * Check if the port is already in the requested enabled/disabled state 
     * If it is, no need to do enything, just exit
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port, &current_enable));
    if (current_enable == enable)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, logical_port, 0, enable));
    /*
     * Note: in Jericho, if MAC loopback is enabled, linkscan would force the link up.
     * from now on, linkscan should not force the link in case of MAC loopback.
     */
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for imb_port_max_packet_size_set
 */
static shr_error_e
dnx_port_frame_max_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    bcm_port_t *port = NULL;
    int *origin_size = NULL;

    SHR_FUNC_INIT_VARS(unit);

    port = (bcm_port_t *) metadata;
    origin_size = (int *) (payload);

    SHR_IF_ERR_EXIT(imb_port_max_packet_size_set(unit, *port, *origin_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_frame_max_get
 */
static shr_error_e
dnx_port_frame_max_get_verify(
    int unit,
    bcm_port_t port,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(size, _SHR_E_PARAM, "max_packet_size");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_frame_max_set
 */
static shr_error_e
dnx_port_frame_max_set_verify(
    int unit,
    bcm_port_t port,
    int size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    if (size < 0 || size > dnx_data_nif.eth.packet_size_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong max frame size %d for port %d\r\n", size, port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve max packet size supported
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - max packet size
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_frame_max_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_frame_max_get_verify(unit, port, size));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_max_packet_size_get(unit, logical_port, size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set max packet size to support
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - max packet size
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_frame_max_set(
    int unit,
    bcm_port_t port,
    int size)
{
    int orig_size = 0;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_frame_max_set_verify(unit, port, size));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     * Get original RX frame max size
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_frame_max_get(unit, port, &orig_size));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_port_max_packet_size_set(unit, logical_port, size));
    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(bcm_port_t),
                                                        (uint8 *) &orig_size,
                                                        sizeof(int), &dnx_port_frame_max_er_rollback, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_ifg_set
 *     bcm_dnx_port_ifg_get
 */

static shr_error_e
dnx_port_ifg_verify(
    int unit,
    bcm_port_t port,
    bcm_port_duplex_t duplex,
    int *bit_times,
    int is_set)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bit_times, _SHR_E_PARAM, "bit_times");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (duplex != BCM_PORT_DUPLEX_FULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only Full Duplex Mode supported");
    }

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, 1, 1, 1))
    {
        if (is_set && *bit_times > BYTES2BITS(dnx_data_nif.mac_client.tx_average_ipg_max_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong IPG value[%d] is out of range[0-%d]", *bit_times,
                         dnx_data_nif.mac_client.tx_average_ipg_max_get(unit));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve inter-frame gap for the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] speed - port speed
 * \param [in] duplex - port duplex mode (half / full)
 * \param [out] bit_times - inter-frame gap in bit-times
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ifg_get(
    int unit,
    bcm_port_t port,
    int speed,
    bcm_port_duplex_t duplex,
    int *bit_times)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_ifg_verify(unit, port, duplex, bit_times, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_tx_average_ipg_get(unit, logical_port, bit_times));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set inter-frame gap for the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] speed - port speed
 * \param [in] duplex - port duplex mode (half / full)
 * \param [in] bit_times - inter-frame gap in bit-times
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ifg_set(
    int unit,
    bcm_port_t port,
    int speed,
    bcm_port_duplex_t duplex,
    int bit_times)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_ifg_verify(unit, port, duplex, &bit_times, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_tx_average_ipg_set(unit, logical_port, bit_times));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_link_state_get
 */

static shr_error_e
dnx_port_link_state_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_link_state_t * state)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(state, _SHR_E_PARAM, "link_state");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve port link state
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags (currently not in use)
 * \param [in] state - link state info.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_link_state_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_link_state_t * state)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_state_get_verify(unit, port, state));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_link_state_get(unit, logical_port, 1, state));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_loopback_get
 *     bcm_dnx_port_loopback_set
 */

static shr_error_e
dnx_port_loopback_verify(
    int unit,
    bcm_port_t port,
    int *loopback,
    int is_set)
{
    int retimer_supported =
        dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_retimer_functionality_supported);
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    int imb_type;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(loopback, _SHR_E_PARAM, "loopback");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /*
     * Both BusA and BusC don't support loopback.
     * BusB supports only feature mac_client_is_loopback_supported=TRUE
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info) ||
        DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE) ||
        (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE) &&
         !dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_loopback_supported)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "loopback operation is not supported for port=%d", port);
    }

    if (is_set)
    {
        /*
         * Verify the Loopback Mode
         */
        switch (*loopback)
        {
            case BCM_PORT_LOOPBACK_NONE:
                break;
            case BCM_PORT_LOOPBACK_MAC:
            {
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
                    && dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_LOOPBACK_MAC is not supported for L1 ETH port %d.\r\n", port);
                }
                if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_LOOPBACK_MAC is not supported for flexe phy port %d.\r\n",
                                 port);
                }
                if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_LOOPBACK_MAC is not supported for OTN phy port %d.\r\n", port);
                }
                break;
            }
            case BCM_PORT_LOOPBACK_PHY:
            case BCM_PORT_LOOPBACK_PHY_REMOTE:
            case BCM_PORT_LOOPBACK_MAC_REMOTE:
            {
                /*
                 * J2X BusB only supports BCM_PORT_LOOPBACK_MAC/NONE loopback mode
                 */
                if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Loopback type %d is not supported for BusB port %d.\r\n", *loopback,
                                 port);
                }
                /*
                 * MAC REMOTE Loopback not supported on fabric ports without retimer available
                 */
                if (*loopback == BCM_PORT_LOOPBACK_MAC_REMOTE)
                {
                    if (dnx_drv_is_sfi_port(unit, logical_port) && !retimer_supported)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_LOOPBACK_MAC_REMOTE is not supported on this device.\r\n");
                    }
                    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "BCM_PORT_LOOPBACK_MAC_REMOTE is not supported for flexe phy port %d.\r\n", port);
                    }
                    if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "BCM_PORT_LOOPBACK_MAC_REMOTE is not supported for OTN phy port %d.\r\n", port);
                    }
                }
                break;
            }
            case BCM_PORT_LOOPBACK_NIF:
                SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
                imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_info.ethu_id)->type;
                if (dnx_data_nif.eth.is_nif_loopback_supported_get(unit, imb_type)->is_supported == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_PORT_LOOPBACK_NIF is not supported..\r\n");
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Loopback type %d is not supported.\r\n", *loopback);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve loopback status for the port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] loopback - loopback mode. see
 *        bcm_port_loopback_t
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_loopback_verify(unit, port, loopback, 0     /* is set */
                           ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, logical_port, loopback));
exit:

    SHR_FUNC_EXIT;
}

/**
 * \brief - Set loopback for the port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] loopback - loopback mode. see bcm_port_loopback_t
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    bcm_port_t logical_port = 0;
    bcm_port_resource_t resource;
    dnx_algo_port_info_s port_info;
    int current_loopback = BCM_PORT_LOOPBACK_NONE;
    /*
     * uint32 cl72_start;
     */
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_loopback_verify(unit, port, &loopback, 1    /* is set */
                           ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_port_loopback_set(unit, logical_port, loopback));
    }
    else
    {
        /**
         * Sync resource port and logical port
         */
        resource.port = logical_port;
        bcm_port_resource_t_init(&resource);
        /*
         * Disable CL72 when closing loopback
         */
        SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, logical_port, &current_loopback));
        if ((loopback != BCM_PORT_LOOPBACK_NONE) && (current_loopback == BCM_PORT_LOOPBACK_NONE)
            && (soc_sand_is_emulation_system(unit) == FALSE))
        {
            /*
             * Store original link training state / phy lane config
             */
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));
            SHR_IF_ERR_EXIT(dnx_algo_port_loopback_original_link_training_set
                            (unit, logical_port, resource.link_training));
            SHR_IF_ERR_EXIT(dnx_algo_port_loopback_original_phy_lane_config_set
                            (unit, logical_port, resource.phy_lane_config));
            /*
             * Disable link training
             */
            resource.link_training = 0;
            resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_default_get(unit, logical_port, 0, &resource));

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_set(unit, logical_port, &resource));
        }

        SHR_IF_ERR_EXIT(imb_port_loopback_set(unit, logical_port, loopback));

        /*
         * Note: in Jericho, if MAC loopback is enabled, linkscan would force the link up.
         * from now on, linkscan should not force the link in case of MAC loopback.
         */

        if ((loopback == BCM_PORT_LOOPBACK_NONE) && (current_loopback != BCM_PORT_LOOPBACK_NONE)
            && (soc_sand_is_emulation_system(unit) == FALSE))
        {
            /*
             * Restore original link training / phy lane config state
             */
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));
            SHR_IF_ERR_EXIT(dnx_algo_port_loopback_original_link_training_get
                            (unit, logical_port, &resource.link_training));
            SHR_IF_ERR_EXIT(dnx_algo_port_loopback_original_phy_lane_config_get
                            (unit, logical_port, &resource.phy_lane_config));

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_set(unit, logical_port, &resource));
        }
    }

    /*
     * Configrure loopback configure info to M0 
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (dnx_drv_is_sfi_port(unit, logical_port))
        {
            SHR_IF_ERR_EXIT(soc_linkscan_loopback_info_set(unit, logical_port, loopback));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_probe() API
 */
static shr_error_e
dnx_port_probe_verify(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    bcm_port_t logical_port = 0;
    int fabric_port_base = 0;
    int nof_fabric_links = 0;
    bcm_pbmp_t fabric_ports_bitmap;
    dnx_algo_port_info_s port_info_fabric;
    dnx_algo_port_info_indicator_t indicator;
    SHR_FUNC_INIT_VARS(unit);
    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_probe API is not supported for devices without fabric links\n");
    }

    /** NULL check */
    SHR_NULL_CHECK(okay_pbmp, _SHR_E_PARAM, "okay_pbmp");
    if (BCM_PBMP_IS_NULL(pbmp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pbmp is empty. Need to have at least 1 port to probe.\n");
    }

    BCM_PBMP_CLEAR(fabric_ports_bitmap);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
    /** Fabric ports range check */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (logical_port < fabric_port_base)
        {
            SHR_ERR_EXIT(_SHR_E_PORT,
                         "bcm_port_probe API is only supported for Fabric ports. please see bcm_port_add.\n");
        }

        if (logical_port >= fabric_port_base + nof_fabric_links)
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Fabric logical port is out of range %d. Max is %d.\n", logical_port,
                         fabric_port_base + nof_fabric_links - 1);
        }

        /** Validate port is not already probed */
        if (BCM_PBMP_MEMBER(fabric_ports_bitmap, logical_port))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Port %d is already probed.\n", logical_port);
        }

        /** Validate lane map */
        sal_memset(&indicator, 0, sizeof(dnx_algo_port_info_indicator_t));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, BCM_PORT_IF_SFI, &indicator, &port_info_fabric));
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_port_add_verify
                        (unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, port_info_fabric, 0, (logical_port - fabric_port_base)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Attach all ports in pbmp bitmap
 *
 * \param [in] unit - chip unit id.
 * \param [in] pbmp - bitmap of ports to be added
 * \param [in] okay_pbmp - ports actually added
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * This method is not supported for NIF ports,
 *   see bcm_port_add instead.
 * \see
 *   * bcm_port_add
 */
shr_error_e
bcm_dnx_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_probe_verify(unit, pbmp, okay_pbmp));
    SHR_IF_ERR_EXIT(dnx_fabric_if_port_probe(unit, pbmp, okay_pbmp));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_priority_config_get
 *     bcm_dnx_port_priority_config_set
 */

shr_error_e
dnx_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config,
    int is_set)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    int enable, ii, nof_prio_groups;
    uint8 is_fixed_priority;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(priority_config, _SHR_E_PARAM, "priority_config");
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * Check port type which don't support priority configuration
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_MGMT(unit, port_info) ||
        DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info) ||
        DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0) ||
        DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE) || DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority config cannot be applied to port %d\n.", port);
    }

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    if (is_set)
    {
        nof_prio_groups = priority_config->nof_priority_groups;
        /*
         * Check is valid only when setting priority group config.
         */
        if (nof_prio_groups < DNX_PORT_MIN_NOF_PRIORITY_GROUPS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of priority groups(RMCs) lower than %d is not supported.",
                         DNX_PORT_MIN_NOF_PRIORITY_GROUPS);
        }

        /*
         * FLEXE L1 verify
         */
        if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
        {
            if (nof_prio_groups > FLEXE_L1_PORT_NOF_PRIO_GROUPS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "bcm_port_priority_config_set for L1 port %d should not have more than %d group/groups. \n",
                             port, FLEXE_L1_PORT_NOF_PRIO_GROUPS);
            }
            else
            {
                /*
                 * Setting priority config on L1 ports is allowed but only for
                 * bcmPortNifSchedulerTDM scheduler priority and
                 * IMB_PRD_PRIORITY_ALL source priority
                 */
                for (ii = 0; ii < FLEXE_L1_PORT_NOF_PRIO_GROUPS; ++ii)
                {
                    if (!(priority_config->priority_groups[ii].sch_priority == bcmPortNifSchedulerTDM &&
                          priority_config->priority_groups[ii].source_priority == IMB_PRD_PRIORITY_ALL))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "FLEXE_MAC_L1, ILKN_L1 and ETH_L1 port types support only bcmPortNifSchedulerTDM scheduler priority and IMB_PRD_PRIORITY_ALL source priority \
                                    on Ingress priority drop for port %d\n.",
                                     port);
                    }
                }
            }
        }

        else
        {
            for (ii = 0; ii < nof_prio_groups; ++ii)
            {
                if (priority_config->priority_groups[ii].sch_priority == bcmPortNifSchedulerTDM)
                {
                    /*
                     * Verify if TDM can be configured.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_usage_verify(unit, 1 /* is_tdm */ ));
                    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, TRUE, TRUE))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "bcmPortNifSchedulerTDM scheduler priority can't be applied to flexe mac port %d.\n",
                                     port);
                    }
                }
            }
        }
        /*
         * check if the port is disabled
         */
        SHR_IF_ERR_EXIT(imb_port_enable_get(unit, logical_port, &enable));
        if (enable)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "bcm_port_priority_config_set API should be called only for disabled ports. please disable port %d.\n",
                         logical_port);
        }

        /*
         * check if there is a limitaion on changing priority
         */
        SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_get(unit, logical_port, &is_fixed_priority));
        if (is_fixed_priority)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Priority config can't be changed for port %d. Priority can be change only before creating src VSQ and/or setting port compensation\n",
                         logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Returns per_priority_exist array
 * such that per_priority_exist[i] = 1 if priority "i" exists  in the priority_config
 *
 * \note - this function creates data for allocation ingress reassembly context,
 * thus it contains only priorities relevant for ingress reassembly context.
 */
static int
dnx_port_priority_config_priority_exist_get(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int per_priority_exist[])
{
    int ii, nof_prio_groups;
    SHR_FUNC_INIT_VARS(unit);
    nof_prio_groups = priority_config->nof_priority_groups;
    for (ii = 0; ii < DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF; ++ii)
    {
        /** initialize exist array -- all priorities do not exist */
        per_priority_exist[ii] = 0;
    }

    for (ii = 0; ii < nof_prio_groups; ++ii)
    {
        /** set priority, if exists in the user config */
        if (priority_config->priority_groups[ii].sch_priority != bcmPortNifSchedulerTDM)
        {
            /** TDM priority does not require reassembly context */
            int channel_mapping = dnx_data_nif.eth.ingress_reassembly_context_mapping_get(unit,
                                                                                          priority_config->
                                                                                          priority_groups[ii].
                                                                                          sch_priority)->channel;
            per_priority_exist[channel_mapping] = 1;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the priority groups for the port. each
 *        priority group represent a logical FIFO.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_priority_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    bcm_port_t logical_port = 0;
    int per_priority_exist[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;
    bcm_tdm_interface_config_t interface_config;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_priority_config_verify(unit, port, priority_config, 1 /* is_set */ ));
    sal_memset(&interface_config, 0, sizeof(bcm_tdm_interface_config_t));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, logical_port, &if_tdm_mode));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    /** get old TDM stream ID base */
    if ((if_tdm_mode != DNX_ALGO_PORT_IF_NO_TDM) && (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_PACKET))
    {
        if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) &&
            DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
        {
            {
                /** get current base */
                SHR_IF_ERR_EXIT(bcm_dnx_tdm_interface_config_get(unit, 0, logical_port, &interface_config));
            }
        }
    }

    SHR_IF_ERR_EXIT(imb_port_priority_config_set(unit, logical_port, priority_config));
    /** For L1 port, no need to configure reassembly context */
    if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
    {
        SHR_EXIT();
    }

    /*
     * configure reassembly context for the new / changed priorities
     */
    if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_priority_config_priority_exist_get
                        (unit, logical_port, priority_config, per_priority_exist));
        /** set context for existing priorities */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_per_priority_context_set(unit, logical_port, per_priority_exist));
        /** unset context for non existing priorities */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_per_priority_context_unset(unit, logical_port, per_priority_exist));
    }

    /** reconfigure tdm base stream according to new channel due to RMC change */
    if ((if_tdm_mode != DNX_ALGO_PORT_IF_NO_TDM) && (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_PACKET))
    {
        if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported) &&
            DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
        {
            /**
             * modify only ports that have an active stream ID configured, unconfigured ports are using base stream of 0
             * since 0 is the HW default and can't be actively set by the user.
             * Notice - base != 0 doesn't necessarily means that this stream is not default (configured by user), but
             * key_size is a good indication for it since it has to be set by the user to value bigger than 0.
             */
            if ((interface_config.stream_id_enable == FALSE) && (interface_config.stream_id_base != 0) &&
                (interface_config.stream_id_key_size > 0))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_tdm_interface_config_set(unit, 0, logical_port, &interface_config));
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get priority groups information for the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups info
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_priority_config_verify(unit, port, priority_config, 0       /* is_set */
                           ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_priority_config_get(unit, logical_port, priority_config));
exit:

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_resource_lane_config_soc_properties_get(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * port_resource)
{
    soc_dnxc_port_dfe_mode_t dfe;
    int media_type;
    int unreliable_los;
    int cl72_auto_polarity_enable;
    int cl72_restart_timeout_enable;
    soc_dnxc_port_channel_mode_t channel_mode;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    dfe = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->dfe;
    /** if dfe SoC property was configured */
    if ((int) dfe != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (dfe)
        {
            case soc_dnxc_port_dfe_on:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_dfe_off:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_lp_dfe:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(port_resource->phy_lane_config);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_dfe SoC property value. Allowed values are on|off|lp",
                             port);
            }
        }
    }

    media_type = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->media_type;
    /*
     * if media type SoC property was configured
     */
    if (media_type != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (media_type)
        {
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(port_resource->phy_lane_config, media_type);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_media_type SoC property value. Allowed values are backplane|copper|optics",
                             port);
            }
        }
    }

    unreliable_los = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->unreliable_los;
    /*
     * if unreliable los SoC property was configured
     */
    if (unreliable_los != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (unreliable_los)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_auto_polarity_enable = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_auto_polarity_enable;
    /*
     * if cl72 auto polarity SoC property was configured
     */
    if (cl72_auto_polarity_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_auto_polarity_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_restart_timeout_enable =
        dnx_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_restart_timeout_enable;
    /*
     * if cl72 restart timeout SoC property was configured
     */
    if (cl72_restart_timeout_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_restart_timeout_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    channel_mode = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->channel_mode;
    /*
     * if channel mode SoC property was configured
     */
    if ((int) channel_mode != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (channel_mode)
        {
            case soc_dnxc_port_force_nr:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_force_er:
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
                    || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
                {
                    if (dnx_data_port.static_add.flr_config_get(unit, port)->flr_fw_support)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "port %d: Force ES cannot be enabled when FLR firmware is downloaded", port);
                    }
                }
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(port_resource->phy_lane_config);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_channel_mode SoC property value. Allowed values are force_nr|force_er",
                             port);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_otn_phy_resource_set(
    int unit,
    bcm_port_resource_t * resource)
{
    int rv;
    int nof_phys, nof_logical_lanes;
    int *logical_lanes = NULL;
    int logical_otn_lane, nof_lanes;
    int lane_speed;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource->port, &nof_phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource->port, &port_info));
    lane_speed = DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info) ? resource->speed : resource->speed / nof_phys;
    nof_logical_lanes = DNXC_PORT_PHY_SPEED_IS_PAM4(lane_speed) ? nof_phys * 2 : nof_phys;

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_get(unit, resource->port, &nof_lanes, &logical_otn_lane));

    if (logical_otn_lane != BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        if ((nof_lanes == nof_logical_lanes)
            && (resource->logical_otn_lane == BCM_PORT_RESOURCE_DEFAULT_REQUEST
                || resource->logical_otn_lane == logical_otn_lane))
        {
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_port_lane_map_set(unit, resource->port, 0, NULL, 0));
            SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_free(unit, resource->port));
        }
    }

    SHR_ALLOC_SET_ZERO(logical_lanes, sizeof(int) * nof_logical_lanes, "OTN lanes", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_alloc
                    (unit, resource->port, resource->logical_otn_lane, nof_logical_lanes, logical_lanes));
    rv = imb_port_lane_map_set(unit, resource->port, 0, logical_lanes, nof_logical_lanes);
    if (SHR_FAILURE(rv))
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_free(unit, resource->port));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (logical_lanes)
    {
        SHR_FREE(logical_lanes);
    }
    SHR_FUNC_EXIT;
}
#ifdef INCLUDE_FLEXE_OTN
/**
 * \brief - Configure FLR resource for advanced FLR
 *    - M/N divider in Framer
 *    - Logical lane to serdes lane mapping
 */
static shr_error_e
dnx_port_advanced_flr_resource_set(
    int unit,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource->port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    /*
     * Config M/N divider for non-OTN ports.
     * For OTN ports, the divider is configured when adding ports
     */
    if (!DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(flexe_core_flr_rate_generator_divider_set(unit, logical_port));
    }
    /*
     * Config logical lane to serdes lane mapping
     */
    SHR_IF_ERR_EXIT(flexe_core_flr_logical_to_serdes_lane_set(unit, logical_port));
exit:
    SHR_FUNC_EXIT;
}
#endif
/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_resource_set
 *     bcm_dnx_port_resource_get
 *     bcm_dnx_port_resource_default_get
 */

static shr_error_e
dnx_port_resource_verify(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "This API doesn't support FlexE client. Please use FlexE cal slots related APIs to config/get the speed of FlexE clients");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify that total aggregated speed is not exceeding device capacity.
 */

static shr_error_e
dnx_port_aggregated_speed_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int aggregated_capacity = 0, resource_index, current_set_speeds = 0;
    int speed, has_rx_speed, rx_speed, has_tx_speed, tx_speed;
    int max_framer_mgmt_speed, framer_mgmt_speed = 0;
    int num_of_lanes, data_rate, max_nif_capacity;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t all_ports, all_mac_clients, all_sar_clients;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    /*
     * Get all logical ports. For NIF ports, exclude the L1 ETH, FlexE PHY and Framer mgmt, as
     * they don't consume Arb BW. The BW of these ports will be verified in
     * dnx_port_resource_multi_set_resource_verify by checking the total BW of BusA side.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_L1 | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FRAMER_MGMT |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_OTN_PHY, &all_ports));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC, 0, &all_mac_clients));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_SAR, 0, &all_sar_clients));
    _SHR_PBMP_OR(all_ports, all_mac_clients);
    _SHR_PBMP_OR(all_ports, all_sar_clients);
    /*
     * Summing the speed of the ports which reconfigure in resource_set.
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource[resource_index].port, &logical_port));
        if (_SHR_PBMP_MEMBER(all_ports, logical_port))
        {
            /*
             * Modify the rate just for ilkn
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
            if (resource[resource_index].flags & BCM_PORT_RESOURCE_ASYMMETRICAL)
            {
                speed =
                    (resource[resource_index].speed >
                     resource[resource_index].rx_speed) ? resource[resource_index].
                    speed : resource[resource_index].rx_speed;
            }
            else
            {
                speed = resource[resource_index].speed;
            }
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &num_of_lanes));
                SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv
                                (unit, logical_port, speed, num_of_lanes, &data_rate));
                current_set_speeds += data_rate;
            }
            else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
            {
                if (speed > dnx_data_nif.global.single_mgmt_port_capacity_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Exceeded speed limit for mgmt port %d, speed %dG",
                                 logical_port, speed / 1000);
                }
                framer_mgmt_speed += speed;
            }
            else
            {
                current_set_speeds += speed;
            }

            _SHR_PBMP_PORT_REMOVE(all_ports, logical_port);
        }
    }

    /*
     * Summing the speed of all the other ports which reconfigure in resource_set.
     */
    _SHR_PBMP_ITER(all_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        tx_speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        if (has_tx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
        }

        rx_speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        if (has_rx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
        }

        speed = (tx_speed > rx_speed) ? tx_speed : rx_speed;
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &num_of_lanes));
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv
                            (unit, logical_port, speed, num_of_lanes, &speed));
        }

        /*
         * Handle mgmt ports separately
         */
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            framer_mgmt_speed += speed;
        }
        else
        {
            aggregated_capacity += speed;
        }
    }
    /*
     * Check the speed of total Framer mgmt
     */
    max_framer_mgmt_speed = dnx_data_nif.global.single_mgmt_port_capacity_get(
    unit) * dnx_data_nif.framer.nof_mgmt_channels_get(
    unit);
    if (framer_mgmt_speed > max_framer_mgmt_speed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Exceeded speed limit for mgmt port by %dG.\n",
                     framer_mgmt_speed - max_framer_mgmt_speed);
    }
    /*
     * If ETH can use the Framer mgmt bandwidth, add the remaining bandwidth into the total capacity
     */
    max_nif_capacity = dnx_data_nif.global.total_data_capacity_get(
    unit);
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_eth_use_framer_mgmt_bw))
    {
        max_nif_capacity += (max_framer_mgmt_speed - framer_mgmt_speed);
    }
    if (current_set_speeds + aggregated_capacity > max_nif_capacity)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Exceeded speed limit by %dG. Current session tries to set %dG for normal traffic.\n",
                     (current_set_speeds + aggregated_capacity - max_nif_capacity) / 1000, current_set_speeds / 1000);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate input params for bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * validation
     */
    if (nport < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nport %d is invalid", nport);
    }

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, resource[resource_index].port));
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource[resource_index].port, &logical_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "This API doesn't support FlexE client. Please use FlexE cal slots related APIs to config/get the speed of FlexE clients");
        }
    }

    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_shared_arch))
    {
        SHR_IF_ERR_EXIT(dnx_port_aggregated_speed_verify(unit, nport, resource));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the 'resource' struct members for bcm_dnx_port_resource_multi_set g.hao operation.
 */
static shr_error_e
dnx_port_resource_multi_set_resource_g_hao_verify(
    int unit,
    int nport,
    int resource_index,
    bcm_port_resource_t * resource)
{
    int has_speed = 0, speed = 0, speed_limit;
    int rx_is_equal = 1, tx_is_equal = 1;
    int new_rx_speed;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** check if G.HAO is supported */
    if (!dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "G.HAO is not supported on this device.\n");
    }

    /** Since there's a strict G.HAO sequence, only a single port is allowed to be modified */
    if (nport != 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "G.HAO allows only a single port change due to a strict sequence of operations . "
                     "Changed %d ports.\r\n", nport);
    }

    /*
     * only framer clients (FLEXE/OTN, L1/L2) support G.HAO.
     * BusA clients use other APIs to perform G.hao
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE, FALSE) &&
        !DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Port %d is not framer MAC/SAR client, only framer MAC/SAR clients use this API for G.hao.\n",
                     resource[resource_index].port);
    }
#ifdef INCLUDE_XFLOW_MACSEC
    /*
     * Check if MACSEC is enabled on the port
     */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE, FALSE)
        && dnx_data_macsec.general.is_macsec_enabled_get(unit))
    {
        bcm_xflow_macsec_port_info_t port_info;
        SHR_IF_ERR_EXIT(bcm_dnx_xflow_macsec_port_control_get
                        (unit, XFLOW_MACSEC_ENCRYPT_DECRYPT_NONE, resource[resource_index].port,
                         bcmXflowMacsecPortEnable, &port_info));
        if (!COMPILER_64_IS_ZERO(port_info.value))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "MACSEC is enabled on port %d. G.hao is not supported on the port with MACSEC enabled.\n",
                         resource[resource_index].port);
        }
    }
#endif
    /** verify it's not the first time the API is being called - meaning tx speed already exists */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (!has_speed && resource[resource_index].speed != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d tx speed is not set, can't perfrom G.HAO.\n",
                     resource[resource_index].port);
    }
    if (has_speed && resource[resource_index].speed == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d, the new tx speed is not set, can't perfrom G.HAO.\n",
                     resource[resource_index].port);
    }
    new_rx_speed =
        (resource[resource_index].
         flags & BCM_PORT_RESOURCE_ASYMMETRICAL) ? resource[resource_index].rx_speed : resource[resource_index].speed;
    /** verify it's not the first time the API is being called - meaning rx speed already exists */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
    if (!has_speed && new_rx_speed != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Port %d rx speed is not set, can't perform G.HAO.\n", resource[resource_index].port);
    }
    if (has_speed && new_rx_speed == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d, the new rx speed is not set, can't perfrom G.HAO.\n",
                     resource[resource_index].port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
    speed_limit = dnx_data_nif.global.ghao_speed_limit_get(
    unit);
    if (speed != resource[resource_index].speed)
    {
        tx_is_equal = 0;
        /** G.HAO speed change from a speed above 1.2Tbps to another speed above 1.2Tbps is not allowed for Tx speed */
        if ((speed > speed_limit) && (resource[resource_index].speed > speed_limit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "G.HAO speed change from a speed above 1.2 Tbps to another speed above 1.2 Tbps is not allowed.\n"
                         "Port %d old speed is %d, new speed is %d.\n",
                         resource[resource_index].port, speed, resource[resource_index].speed);
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
    /** G.HAO speed change from a speed above 1.2Tbps to another speed above 1.2Tbps is not allowed for Rx speed*/
    if (speed != new_rx_speed)
    {
        rx_is_equal = 0;
        if ((speed > speed_limit) && (new_rx_speed > speed_limit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "G.HAO speed change from a speed above 1.2 Tbps to another speed above 1.2 Tbps is not allowed.\n"
                         "Port %d old speed is %d, new speed is %d.\n",
                         resource[resource_index].port, speed, resource[resource_index].rx_speed);
        }
    }
    if (rx_is_equal && tx_is_equal)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "There is no change for either Rx or Tx speed. No need to perform G.hao\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Validate the 'resource' for different FlexE device mode.
 */
static shr_error_e
dnx_port_flexe_multi_device_mode_resource_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t ilkn_pbmp, ilu_distributed_pbmp;
    bcm_port_t port, master_port;
    int has_speed, speed, num_of_lanes;
    int flexe_mode_centralized = 0, flexe_mode_distributed = 0;
    bcm_pbmp_t nif_pbmp, flexe_mac_pbmp;
    uint32 total_rate_nif = 0;
    uint32 total_rate_flexe = 0;
    uint32 total_rate_ilu_distributed = 0;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int i, valid, is_flexe_in_use = 0, is_flexe_enabled = 0;

    SHR_FUNC_INIT_VARS(unit);

    flexe_mode_centralized = (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_CENTRALIZED);
    flexe_mode_distributed = (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISTRIBUTED);
    is_flexe_enabled = ((flexe_mode_centralized || flexe_mode_distributed)
                        && !dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_avoid_bw_verification));
    /*
     * Only when there is valid FlexE group we limit the total NIF rate
     */
    if (is_flexe_enabled)
    {
        for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); i++)
        {
            dnx_algo_flexe_general_db.group_info.valid.get(unit, i, &valid);
            if (valid)
            {
                is_flexe_in_use = 1;
                break;
            }
        }
    }
    /*
     * Skip BW checking if FlexE is not used
     */
    if (!is_flexe_enabled || !is_flexe_in_use)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_PHY, &nif_pbmp));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC, 0, &flexe_mac_pbmp));

    if (flexe_mode_distributed)
    {
        BCM_PBMP_CLEAR(ilu_distributed_pbmp);
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
                         DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &ilkn_pbmp));
        BCM_PBMP_ITER(ilkn_pbmp, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
            if (ilkn_access_info.ilkn_core == dnx_data_nif.flexe.distributed_ilu_id_get(unit))
            {
                _SHR_PBMP_PORT_REMOVE(nif_pbmp, port);
                _SHR_PBMP_PORT_ADD(ilu_distributed_pbmp, port);
            }
        }
    }

    /*
     * basic validation that given resource configurations are legal per port
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT)))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, resource[resource_index].port, 0, &master_port));
            if (_SHR_PBMP_MEMBER(nif_pbmp, master_port))
            {
                total_rate_nif += resource[resource_index].speed;
                _SHR_PBMP_PORT_REMOVE(nif_pbmp, master_port);
            }
        }
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, FALSE, FALSE))
        {
            if (_SHR_PBMP_MEMBER(flexe_mac_pbmp, resource[resource_index].port))
            {
                total_rate_nif += resource[resource_index].speed;
                _SHR_PBMP_PORT_REMOVE(flexe_mac_pbmp, resource[resource_index].port);
            }
        }
        if (flexe_mode_distributed && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
        {
            if (_SHR_PBMP_MEMBER(ilu_distributed_pbmp, resource[resource_index].port))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource[resource_index].port, &num_of_lanes));
                total_rate_ilu_distributed += resource[resource_index].speed * num_of_lanes;
                _SHR_PBMP_PORT_REMOVE(ilu_distributed_pbmp, resource[resource_index].port);
            }
        }
    }
    BCM_PBMP_ITER(nif_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, master_port, DNX_ALGO_PORT_SPEED_F_MBPS, &speed));
            total_rate_nif += speed;
        }
    }
    BCM_PBMP_ITER(flexe_mac_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
            total_rate_flexe += speed;
        }
    }
    if (flexe_mode_centralized)
    {
        /** When flexe in centralized mode, total NIF ports speed cannot exceed 1.2T(exclude 80,81) */
        if ((total_rate_flexe + total_rate_nif) > dnx_data_nif.flexe.max_nif_rate_centralized_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "total NIF speed(%dMbps) exceeds the device limitation(%dMbps).",
                         (total_rate_flexe + total_rate_nif), dnx_data_nif.flexe.max_nif_rate_centralized_get(unit));
        }
    }
    else if (flexe_mode_distributed)
    {
        /** When flexe in distributed mode, total NIF ports speed cannot exceed 400G(exclude 80,81) */
        if ((total_rate_flexe + total_rate_nif) > dnx_data_nif.flexe.max_nif_rate_distributed_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "total NIF speed(%dMbps) exceeds the device limitation(%dMbps).",
                         (total_rate_flexe + total_rate_nif), dnx_data_nif.flexe.max_nif_rate_distributed_get(unit));
        }
        BCM_PBMP_ITER(ilu_distributed_pbmp, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));
            if (has_speed)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                                (unit, master_port, DNX_ALGO_PORT_SPEED_F_MBPS, &speed));
                total_rate_ilu_distributed += speed;
            }
        }
        if (total_rate_ilu_distributed > dnx_data_nif.flexe.max_ilu_rate_distributed_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "total ILKN distributed speed(%dMbps) exceeds the device limitation(%dMbps).",
                         total_rate_ilu_distributed, dnx_data_nif.flexe.max_ilu_rate_distributed_get(unit));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the 'resource' struct members for bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_resource_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t tm_egr_pbmp, ilkn_pbmp, nif_pbmp;
    uint32 max_speed_Mbps;
    uint32 total_framer_phy_speed = 0;
    uint32 total_logical_otn_lanes = 0, multi;
    uint32 total_speed_Mbps[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = {
        0
    };
    bcm_core_t core;
    int has_speed, speed;
    bcm_port_t port, master_port;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int is_flr_fw_support;
    int num_of_lanes;
    uint64 u64_calc;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &tm_egr_pbmp));
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF,
                     DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF, &nif_pbmp));
    /*
     * In distributed mode, ILKN port on ILKN core 1 doesn't consume EGR TM resources,
     * All the traffic to ILKN are from BusC or BusB directly, hence excludng it when
     * calculating total EGR resources.
     */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported)
        && (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISTRIBUTED))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
                         DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &ilkn_pbmp));
        BCM_PBMP_ITER(ilkn_pbmp, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
            if (ilkn_access_info.ilkn_core == dnx_data_nif.flexe.distributed_ilu_id_get(unit))
            {
                _SHR_PBMP_PORT_REMOVE(tm_egr_pbmp, port);
            }
        }
    }

    /*
     * basic validation that given resource configurations are legal per port
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_resource_validate(unit, &resource[resource_index]));
            /*
             * Validate the FEC+SPEED combinations for NIF ports
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF
                (unit, port_info,
                 DNX_ALGO_PORT_TYPE_INCLUDE(ELK, FLEXE, STIF, L1, MGMT, OTN_PHY, FRAMER_MGMT, FEEDBACK))
                || DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_IF_ERR_EXIT(imb_port_fec_speed_validate
                                (unit, resource[resource_index].port, resource[resource_index].fec_type,
                                 resource[resource_index].speed));
            }
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_resource_validate(unit, &port_info, &resource[resource_index]));
            }
            /*
             * Validate CL72 and FLR
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT))
                || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE)
                || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get
                                (unit, resource[resource_index].port, &is_flr_fw_support));
                if ((resource[resource_index].link_training == 1) && is_flr_fw_support)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Fast link recovery has been enabled, link training for port %d should be disabled. \r\n",
                                 resource[resource_index].port);
                }
                if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource[resource_index].phy_lane_config)
                    && is_flr_fw_support)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Fast link recovery has been enabled, force ES for port %d should be disabled. \r\n",
                                 resource[resource_index].port);
                }
                if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_otn_phy_resource_validate(unit, &resource[resource_index]));
                }
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_resource_validate(unit, &resource[resource_index]));
        }
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            /** Framer mgmt is validated separately */
            SHR_IF_ERR_EXIT(dnx_algo_framer_mgmt_resource_validate(unit, &resource[resource_index]));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info))
        {
            /** Validate resource for Framer PHY ports */
            total_framer_phy_speed += resource[resource_index].speed;
            _SHR_PBMP_PORT_REMOVE(nif_pbmp, resource[resource_index].port);
        }
        else if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
        {
            /** Validate resource for OTN PHY ports */
            multi = DNXC_PORT_PHY_SPEED_IS_PAM4(resource[resource_index].speed) ? 2 : 1;
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource[resource_index].port, &num_of_lanes));
            total_logical_otn_lanes += num_of_lanes * multi;
            _SHR_PBMP_PORT_REMOVE(nif_pbmp, resource[resource_index].port);
        }
        /** count the speed of all ports configured here */
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                            (unit, resource[resource_index].port, DNX_ALGO_PORT_MASTER_F_TM, &master_port));
            if (_SHR_PBMP_MEMBER(tm_egr_pbmp, master_port))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, resource[resource_index].port, &core));
                total_speed_Mbps[core] += resource[resource_index].speed;
                /** remove the corresponding port from the bitmap, so it will not be counted twice  (see bleow) */
                _SHR_PBMP_PORT_REMOVE(tm_egr_pbmp, master_port);
            }
        }
        /** G.HAO verify */
        if (resource[resource_index].flags & BCM_PORT_RESOURCE_G_HAO)
        {
            SHR_IF_ERR_EXIT(dnx_port_resource_multi_set_resource_g_hao_verify(unit, nport, resource_index, resource));
        }
        /** AM transparent flag verify */
        if (resource[resource_index].flags & BCM_PORT_RESOURCE_AM_TRANSPARENT)
        {
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE) &&
                !dnx_data_nif.global.otn_supported_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "AM transparent is only relevant for L1 ETH ports with OTN enabled. \n");
            }
        }
    }
    /** go over all tm egr master ports (beside the ones that configured now by the API), and calc the total speed */
    BCM_PBMP_ITER(tm_egr_pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_TM, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                            (unit, master_port, DNX_ALGO_PORT_SPEED_F_MBPS | DNX_ALGO_PORT_SPEED_F_TX, &speed));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, master_port, &core));
            total_speed_Mbps[core] += speed;
        }
    }
    /** max allowed speed at the Egress TM is the bus size each clock */
    /** Calculate max allowed speed in MHz */
    COMPILER_64_ZERO(u64_calc);
    COMPILER_64_SET(u64_calc, 0, dnx_data_device.general.core_clock_khz_get(unit));
    COMPILER_64_UMUL_32(u64_calc, dnx_data_device.general.bus_size_in_bits_get(unit));
    COMPILER_64_UDIV_32(u64_calc, 1000);
    max_speed_Mbps = COMPILER_64_LO(u64_calc);
    /** verify total speed do not exceed the max allowed per core */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        if (total_speed_Mbps[core] > max_speed_Mbps)
        {
            if (dnx_data_device.general.enforce_maximal_core_bandwidth_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The total speed in the device for core%d (%dMbps) exceeds the egress TM limitation(%dMbps).",
                             core, total_speed_Mbps[core], max_speed_Mbps);
            }
            else
            {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U
                          (unit,
                           "The total speed in the device for core%d (%dMbps) exceeds the egress TM limitation(%dMbps). It is the user's responsibility to ensure traffic does not exceed the actual bandwidth limit."),
                          core, total_speed_Mbps[core], max_speed_Mbps));
            }

        }
    }
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        /** Check the BW limition for different FlexE modes */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support))
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_multi_device_mode_resource_verify(unit, nport, resource));
        }
        /** Verify the total Framer PHY BW */
        BCM_PBMP_ITER(nif_pbmp, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info)
                && !DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));
                if (has_speed)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
                    total_framer_phy_speed += speed;
                }
            }
        }
        if (total_framer_phy_speed > dnx_data_nif.flexe.max_flexe_core_speed_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Exceeded Framer limit BW by %dG.\n",
                         (total_framer_phy_speed - dnx_data_nif.flexe.max_flexe_core_speed_get(unit)) / 1000);
        }
    }
    if (dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported)
        && dnx_data_nif.global.otn_supported_get(unit))
    {
        /** Verify the total logical OTN lane limitation */
        BCM_PBMP_ITER(nif_pbmp, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &has_speed));
                if (has_speed)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
                    multi = DNXC_PORT_PHY_SPEED_IS_PAM4(speed) ? 2 : 1;
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &num_of_lanes));
                    total_logical_otn_lanes += num_of_lanes * multi;
                }
            }
        }
        if (total_logical_otn_lanes > dnx_data_nif.otn.nof_lanes_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Exceeded Framer OTN lane limitation (%d - %d).\n",
                         total_logical_otn_lanes, dnx_data_nif.otn.nof_lanes_get(unit));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Fill ports_info according to resource.
 *   ports_info.original_port was already filled in
 *   bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_port_info_get(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0;
    int port_enable = 0;
    int has_tx_speed = 0, has_rx_speed = 0;
    int tx_speed = 0, rx_speed = 0;
    SHR_FUNC_INIT_VARS(unit);
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        /*
         * Per port type handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get
                        (unit, resource[resource_index].port, &ports_info[resource_index].port_info));
        /*
         * Check if this is the first invocation of resource_set for this interface
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed
                        (unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        ports_info[resource_index].has_tx_speed = has_tx_speed;
        /*
         * Check if the tx and rx speed is changed
         */
        tx_speed = 0;
        if (has_tx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get
                            (unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
            ports_info[resource_index].original_tx_speed = tx_speed;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed
                        (unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        rx_speed = 0;
        if (has_rx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get
                            (unit, resource[resource_index].port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
            ports_info[resource_index].original_rx_speed = rx_speed;
        }
        ports_info[resource_index].is_tx_speed_change = (tx_speed != resource[resource_index].speed);
        ports_info[resource_index].is_rx_speed_change =
            (resource[resource_index].flags & BCM_PORT_RESOURCE_ASYMMETRICAL) ? (rx_speed !=
                                                                                 resource
                                                                                 [resource_index].rx_speed)
            : (rx_speed != resource[resource_index].speed);
        /*
         * Save all enabled ports.
         * The enabled ports will be disabled during the resource set,
         * and will be re-enabled at the end.
         */
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_info, TRUE))
        {
            /** store enable state of the port */
            SHR_IF_ERR_EXIT(imb_port_enable_get(unit, resource[resource_index].port, &port_enable));
            ports_info[resource_index].is_port_enable = port_enable;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Pre-configure needed modules before port disable during resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_modules_pre_config_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0, rx_speed;
    SHR_FUNC_INIT_VARS(unit);

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        bcm_pbmp_t logical_ports_bitmap_on_interface;
        bcm_port_t port_iter;
        /** configure same speed to all ports on interface */
        /** get from resource[resource_index].port a bit map with all the ports on that interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                        (unit, resource[resource_index].port, 0, &logical_ports_bitmap_on_interface));
        /** iterate on all ports of the interface and set their speed */
        _SHR_PBMP_ITER(logical_ports_bitmap_on_interface, port_iter)
        {
            /** set Tx speed in mgmt db */
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set
                            (unit, port_iter, DNX_ALGO_PORT_SPEED_F_TX, resource[resource_index].speed));
            /** set Rx speed in mgmt db */
            rx_speed =
                (resource[resource_index].flags & BCM_PORT_RESOURCE_ASYMMETRICAL) ?
                resource[resource_index].rx_speed : resource[resource_index].speed;
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_set(unit, port_iter, DNX_ALGO_PORT_SPEED_F_RX, rx_speed));
        }

        /*
         * set fec type
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_fec_type_set
                        (unit, resource[resource_index].port, resource[resource_index].fec_type));
    }

    /*
     * dnx_egr_queuing_nif_calendar_refresh is used to refresh calendar configurations
     * to a inactive calendar for all EGR TM ports, and not per port.
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_info))
        {
            SHR_IF_ERR_EXIT(dnx_egr_tm_scheduling_stand_by_calendar_set(unit, BCM_CORE_ALL));
            break;
        }
    }

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_info)
            || DNX_ALGO_PORT_TYPE_IS_L1(unit, ports_info[resource_index].port_info))
        {
            /*
             * Egress Shared Buffers calendar is a configuration for all ports, not per port.
             * Needs to be re-calculated each time a port is added, this includes Egress TM ports and L1 ports.
             */
            if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
                && dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_is_used))
            {
                SHR_IF_ERR_EXIT(dnx_esb_nif_calendar_refresh(unit, BCM_CORE_ALL));
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure needed modules after resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_modules_config_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0;
    int is_ext_txpi_en = 0;
    uint32 flexe_flags;
    dnx_flexe_cal_info_t cal_info;
    int is_flr_fw_support = 0, is_advanced_flr_enabled = 0;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * this loop should contain only per interface code,
     * Flex-E ports can be addressed to as interfaces in the sense that each port's
     * configuration is not depended on other interface's configuration
     */
    if (DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        cal_info.mac_client_cal_is_updated = FALSE;
        cal_info.flexe_phy_cal_is_updated = FALSE;

        /** FlexE configurations required to configure after speed is ready */
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            flexe_flags = 0;
            if (DNX_ALGO_PORT_TYPE_IS_FRAMER(unit, ports_info[resource_index].port_info))
            {
                /** Update flags */
                if (ports_info[resource_index].is_rx_speed_change)
                {
                    IMB_FLEXE_DIRECTION_RX_SET(flexe_flags);
                }
                if (ports_info[resource_index].is_tx_speed_change)
                {
                    IMB_FLEXE_DIRECTION_TX_SET(flexe_flags);
                }
                if (resource[resource_index].flags & BCM_PORT_RESOURCE_AM_TRANSPARENT)
                {
                    IMB_FLEXE_AM_TRANSPARENT_SET(flexe_flags);
                }
                SHR_IF_ERR_EXIT(dnx_port_flexe_config_set(unit, resource[resource_index].port, flexe_flags, &cal_info));
            }
        }
    }
    /**
     * this loop should contain only per interface code,
     * Configure the OTN lane map and configurations for
     * advanced FLR mode
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF
            (unit, ports_info[resource_index].port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, FLEXE, MGMT, OTN_PHY)))
        {
            /*
             * Check if FLR firmware is supported
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get
                            (unit, resource[resource_index].port, &is_flr_fw_support));
            is_advanced_flr_enabled = (is_flr_fw_support
                                       && dnx_data_nif.flr.feature_get(unit,
                                                                       dnx_data_nif_flr_is_advanced_flr_supported));
            /*
             * Configure OTN and FLR resources
             */
            if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, ports_info[resource_index].port_info) || is_advanced_flr_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_port_otn_phy_resource_set(unit, &resource[resource_index]));
                if (is_advanced_flr_enabled)
                {
#ifdef INCLUDE_FLEXE_OTN
                    SHR_IF_ERR_EXIT(dnx_port_advanced_flr_resource_set(unit, &resource[resource_index]));
#endif
                }
            }
        }
    }
    /**
     * E2E SCH ports configure
     * this loop should contain only per interface code,
     * code inside is iterating on all ports in the interface
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, ports_info[resource_index].port_info))
        {
            /*
             * if this is first resource_set on the interface and the Tx speed is configured
             * color all ports under the interface according to interface speed
             */
            if ((ports_info[resource_index].has_tx_speed == 0) && (resource[resource_index].speed != 0))
            {
                SHR_IF_ERR_EXIT(dnx_sch_port_interface_ports_color(unit, resource[resource_index].port));
            }
        }
    }

    /** EGR TM ports configure - interface level configuration */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if ((resource[resource_index].speed != 0) && ports_info[resource_index].is_tx_speed_change)
        {
            SHR_IF_ERR_EXIT(dnx_egr_tm_port_speed_set_config
                            (unit, resource[resource_index].port, resource[resource_index].speed));
        }
    }

    /** Activate refreshed calendar configurations. */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_info))
        {
            SHR_IF_ERR_EXIT(dnx_egr_tm_scheduling_calendar_switch(unit, BCM_CORE_ALL));
            break;
        }
    }

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_info)
            || DNX_ALGO_PORT_TYPE_IS_L1(unit, ports_info[resource_index].port_info))
        {
            /*
             * Activate the re-calculated Egress Shared Buffers calendar.
             */
            if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
                && dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_is_used))
            {
                SHR_IF_ERR_EXIT(dnx_esb_nif_calendar_activate(unit, BCM_CORE_ALL));
                break;
            }
        }
    }

    /** Fabric ports config */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, ports_info[resource_index].port_info))
        {
            /*
             * ALDWP should be configured after init time for every fabric speed change.
             * Avoiding ALDWP config at init time.
             */
            if (dnxc_init_is_init_done_get(unit) == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_aldwp_config(unit));
            }
            break;
        }
    }

    /**  Config start TX threshold */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        /** Stif port should default tx_start value and not according to the port speed */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_info,
                                          DNX_ALGO_PORT_TYPE_INCLUDE(MGMT))
            || (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, ports_info[resource_index].port_info, TRUE)
                && !dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
            || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index].port_info, TRUE, FALSE, FALSE)
            || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, ports_info[resource_index].port_info,
                                              DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
        {
            SHR_IF_ERR_EXIT(imb_port_tx_start_thr_set
                            (unit, resource[resource_index].port, resource[resource_index].speed, -1));
        }
    }

    /**  Config start TXPI SDM divider */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF
            (unit, ports_info[resource_index].port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, FLEXE, OTN_PHY, FEEDBACK)))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ext_txpi_enable_get
                            (unit, resource[resource_index].port, &is_ext_txpi_en));
            if (is_ext_txpi_en || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, ports_info[resource_index].port_info)
                || DNX_ALGO_PORT_TYPE_IS_NIF_ETH_FEEDBACK(unit, ports_info[resource_index].port_info))
            {
                SHR_IF_ERR_EXIT(imb_port_txpi_sdm_divider_set
                                (unit, resource[resource_index].port, resource[resource_index].speed));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune needed modules after resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_tune(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int is_ofr_supported = dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported);
    int resource_index = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** EGR TM ports tune */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_info) &&
            (resource[resource_index].speed != 0) && ports_info[resource_index].is_tx_speed_change &&
            !(_SHR_IS_FLAG_SET(resource[resource_index].flags, BCM_PORT_RESOURCE_ECGM_AUTO_ADJUST_DISABLE)))
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_interface_tune
                            (unit, resource[resource_index].port, resource[resource_index].speed));
        }
    }

    /** Configure CDU scheduler */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, ports_info[resource_index].port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1)))
        {
            /** filter out non tm stif ports */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, ports_info[resource_index].port_info) &&
                !DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, ports_info[resource_index].port_info))
            {
                continue;
            }

            /** no configuration needed if device has OFR and port is non-ILKN */
            if (is_ofr_supported
                && !DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, ports_info[resource_index].port_info,
                                                   DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
            {
                continue;
            }

            SHR_IF_ERR_EXIT(imb_port_scheduler_config_set(unit, resource[resource_index].port, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Wrapper which will call the verification function one time per egress interface,
 *   to make sure we are not going over the max limit of total egress interface credits
 */
static shr_error_e
dnx_port_if_total_egr_if_credits_sw_update(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index;
    uint32
        if_bitmap[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][_SHR_BITDCLSIZE
                                                         (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)];
    int if_id;
    dnx_algo_port_info_s port_info;
    int num_of_lanes;
    int speed;
    int if_speed_i;
    int has_speed;
    int interface_offset;
    int interface_idx;
    bcm_core_t core;
    uint32 rcy_new_speed[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_RCY_INTERFACES][DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 rcy_old_speed[DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_RCY_INTERFACES][DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint8 update_rcy_ports = FALSE;
    bcm_pbmp_t all_rcy_master_ports;
    bcm_port_t rcy_port_i;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(if_bitmap, 0x0,
               (DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES *
                _SHR_BITDCLSIZE(DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES) * sizeof(uint32)));
    sal_memset(rcy_new_speed, 0x0,
               (DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES *
                DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_RCY_INTERFACES * sizeof(uint32)));
    sal_memset(rcy_old_speed, 0x0,
               (DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES *
                DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_RCY_INTERFACES * sizeof(uint32)));
    /**
     * The below code is relevant only for RCY ports. Recycle interface is special in a way that it has 2 egress interfaces, but one TXI.
     * The credits need to be updated based on the total rate of both interfaces. The iteration below will determine the 
     * new rate for the RCY interfaces and to indicate that we need to update the credits for RCY.
     */
    if ((dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit) > 1))
    {
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, resource[resource_index].port, &core));
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get
                                (unit, resource[resource_index].port, &interface_offset));
                rcy_new_speed[interface_offset][core] = resource[resource_index].speed;
                update_rcy_ports = TRUE;
            }
        }
    }

    /**
     * The below code is only relevant for RCY interfaces. It is used to determine if RCY ports previously existed on
     * any of the two interfaces, and if yes, to take their rate into account when setting the RCY TXI credits.
     */
    if (update_rcy_ports == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, _SHR_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_RCY,
                         DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &all_rcy_master_ports));
        BCM_PBMP_ITER(all_rcy_master_ports, rcy_port_i)
        {
            /** Check if the other RCY master port already was set and have speed */
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, rcy_port_i, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
            if (has_speed == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, rcy_port_i, &core));
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, rcy_port_i, &interface_offset));
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                                (unit, rcy_port_i, DNX_ALGO_PORT_SPEED_F_TX, &if_speed_i));
                rcy_old_speed[interface_offset][core] = if_speed_i;
            }
        }
    }

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, resource[resource_index].port, &if_id));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, resource[resource_index].port, &core));
            /** only if this interface was not update yet, update it */
            if (SHR_BITGET(if_bitmap[core], if_id) == FALSE)
            {
                if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info)
                    && (dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit) > 1))
                {
                    speed = 0;
                    for (interface_idx = 0;
                         interface_idx < dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit); interface_idx++)
                    {
                        if (rcy_new_speed[interface_idx][core] != 0)
                        {
                            speed += rcy_new_speed[interface_idx][core];
                        }
                        else
                        {
                            speed += rcy_old_speed[interface_idx][core];
                        }
                    }
                }
                else
                {
                    speed = resource[resource_index].speed;
                }

                /** Only for ILKN, the speed represent the speed of one lane. */
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource[resource_index].port, &num_of_lanes));
                    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv
                                    (unit, resource[resource_index].port, resource[resource_index].speed, num_of_lanes,
                                     &speed));
                }
                SHR_IF_ERR_EXIT(dnx_egr_queuing_total_egr_if_credits_sw_update(unit,
                                                                               resource[resource_index].port, speed));
                SHR_BITSET(if_bitmap[core], if_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Perform G.HAO tx speed change
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] old_speed - old port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_resource_g_hao_tx_set(
    int unit,
    bcm_port_t port,
    int old_speed)
{
    int is_increase;
    int speed;
    int client_channel;
    uint32 flags = 0;
    bcm_pbmp_t calendars_to_be_set;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        /** Framer */
        if (!DNX_ALGO_PORT_TYPE_IS_OTN_SAR(unit, port_info))
        {
            SHR_IF_ERR_EXIT(flexe_core_sar_client_ghao_config_set(unit, client_channel, BCM_PORT_FLEXE_TX, speed));
        }
    }
    else
    {
        is_increase = speed > old_speed;

        /**
         * To avoid data loss the sequence is strict according to the packet datapath and rate change direction
         * (reversed sequence between speed increase/decrease).
         *
         * */

        /*
         * Preparing Arbiter & OFT shadow calendars before switching
         */
        if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
        {
            _SHR_PBMP_CLEAR(calendars_to_be_set);
            SHR_IF_ERR_EXIT(dnx_port_arb_tx_calendar_ids_get(unit, port, &calendars_to_be_set));
            SHR_IF_ERR_EXIT(dnx_port_arb_shadow_calendar_build(unit, calendars_to_be_set));
        }

        if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
        {
            SHR_IF_ERR_EXIT(dnx_port_oft_shadow_calendar_build(unit));
        }
        /*
         * Prepare WB resources, link list and calendar
         */
        {
            IMB_FLEXE_DIRECTION_TX_SET(flags);
            SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_prepare(unit, port, flags));
            SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_prepare(unit, port, flags));
        }

        if (is_increase)
        {
            /** TinyMAC */
            {
                /** Configure new speed in TinyMAC */
                SHR_IF_ERR_EXIT(imb_port_ghao_speed_config_set(unit, port, speed));

                /** Configure new threshold in TinyMAC */
                SHR_IF_ERR_EXIT(imb_port_mac_tx_threshold_set(unit, port, speed));
            }

            /** Arbiter */
            if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
            {
                /** switch calendar active id*/
                SHR_IF_ERR_EXIT(dnx_port_arb_calendar_switch_active_id(unit, calendars_to_be_set));

                /** switch Arbiter FIFO to new linked list */
                SHR_IF_ERR_EXIT(dnx_port_arb_tx_tmac_link_list_switch(unit, port));

                /** set arbiter tx threshold*/
                SHR_IF_ERR_EXIT(dnx_port_arb_tmac_start_tx_threshold_set(unit, port));
            }

            /** OFT */
            if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
            {
                /** modify OFT credits to arbiter */
                SHR_IF_ERR_EXIT(dnx_port_oft_port_credits_change_set(unit, port, old_speed));

                /** switch calendar active id*/
                SHR_IF_ERR_EXIT(dnx_port_oft_calendar_switch_active_id(unit));

                /** switch OFT FIFO to new linked list */
                SHR_IF_ERR_EXIT(dnx_port_oft_link_list_switch(unit, port));

                /** set start tx threshold */
                SHR_IF_ERR_EXIT(imb_port_tx_start_thr_set(unit, port, speed, -1));
            }

            /** ESB + EGQ */
            {
                /** set ESB credits to OFT */
                SHR_IF_ERR_EXIT(dnx_esb_port_txi_irdy_thr_set(unit, port, TRUE));

                /** set ESB and EGQ speed related attributes (min_gap, txq_max_bytes, irdy, etc...) */
                SHR_IF_ERR_EXIT(dnx_egr_tm_port_speed_set_config(unit, port, speed));

                /** trigger ESB calendar switch, the stand by calendar was already recalculated */
                SHR_IF_ERR_EXIT(dnx_esb_nif_calendar_refresh(unit, BCM_CORE_ALL));

                /** update FQP credits to ESB */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_update(unit, port, old_speed));

                /** trigger EGQ calendars switch, the stand by calendars were already recalculated */
                SHR_IF_ERR_EXIT(dnx_egr_tm_scheduling_stand_by_calendar_set(unit, BCM_CORE_ALL));
            }

            /** WB */
            {
                /** switch to new FIFO link list */
                SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_switch(unit, port, flags));

                /** switch new calendar */
                SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_switch(unit, port, flags));
            }

            /** Framer */
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_ghao_config_set(unit, client_channel, speed));
            }
        }
        else
        {
            /** decrease speed - order of operations is reversed */

            /** Framer */
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_ghao_config_set(unit, client_channel, speed));
            }

            /** WB */
            {
                /** switch to new FIFO link list */
                SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_switch(unit, port, flags));

                /** switch new calendar */
                SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_switch(unit, port, flags));
            }

            /** ESB + EGQ */
            {
                /** update FQP credits to ESB */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_update(unit, port, old_speed));

                /** set ESB and EGQ speed related attributes (min_gap, txq_max_bytes, irdy, etc...) */
                SHR_IF_ERR_EXIT(dnx_egr_tm_port_speed_set_config(unit, port, speed));

                /** trigger ESB calendar switch, the stand by calendar was already recalculated */
                SHR_IF_ERR_EXIT(dnx_esb_nif_calendar_refresh(unit, BCM_CORE_ALL));

                /** trigger EGQ calendars switch, the stand by calendars were already recalculated */
                SHR_IF_ERR_EXIT(dnx_egr_tm_scheduling_stand_by_calendar_set(unit, BCM_CORE_ALL));

                /** set ESB credits to OFT */
                SHR_IF_ERR_EXIT(dnx_esb_port_txi_irdy_thr_set(unit, port, TRUE));
            }

            /** OFT */
            if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
            {
                /** switch OFT FIFO to new linked list */
                SHR_IF_ERR_EXIT(dnx_port_oft_link_list_switch(unit, port));

                /** switch calendar active id*/
                SHR_IF_ERR_EXIT(dnx_port_oft_calendar_switch_active_id(unit));

                /** set start tx threshold */
                SHR_IF_ERR_EXIT(imb_port_tx_start_thr_set(unit, port, speed, -1));

                /** modify OFT credits to arbiter */
                SHR_IF_ERR_EXIT(dnx_port_oft_port_credits_change_set(unit, port, old_speed));
            }

            /** Arbiter */
            if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
            {
                /** switch Arbiter FIFO to new linked list */
                SHR_IF_ERR_EXIT(dnx_port_arb_tx_tmac_link_list_switch(unit, port));

                /** switch calendar active id*/
                SHR_IF_ERR_EXIT(dnx_port_arb_calendar_switch_active_id(unit, calendars_to_be_set));

                /** set arbiter tx threshold*/
                SHR_IF_ERR_EXIT(dnx_port_arb_tmac_start_tx_threshold_set(unit, port));
            }

            /** TinyMAC */
            {
                /** Configure new speed in TinyMAC */
                SHR_IF_ERR_EXIT(imb_port_ghao_speed_config_set(unit, port, speed));

                /** Configure new threshold in TinyMAC */
                SHR_IF_ERR_EXIT(imb_port_mac_tx_threshold_set(unit, port, speed));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Perform G.HAO rx speed change
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port ID
 * \param [in] old_speed - old port speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_resource_g_hao_rx_set(
    int unit,
    bcm_port_t logical_port,
    int old_speed)
{
    int is_increase;
    int speed, priority_group;
    uint8 **alloc_info = NULL;
    uint32 flags = 0;
    int client_channel;
    bcm_pbmp_t calendars_to_be_set;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        /** Framer */
        if (!DNX_ALGO_PORT_TYPE_IS_OTN_SAR(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &client_channel));
            SHR_IF_ERR_EXIT(flexe_core_sar_client_ghao_config_set(unit, client_channel, BCM_PORT_FLEXE_RX, speed));
        }
    }
    else
    {
        is_increase = speed > old_speed;

        /*
         * Verify new OFR link list(s) can be allocated
         */
        if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
        {
            SHR_ALLOC_SET_ZERO_ERR_EXIT(alloc_info, sizeof(uint8 *) * dnx_data_nif.eth.priority_groups_nof_get(unit),
                                        "Alloc info per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            for (priority_group = 0; priority_group < dnx_data_nif.eth.priority_groups_nof_get(unit); priority_group++)
            {
                SHR_ALLOC_SET_ZERO_ERR_EXIT(alloc_info[priority_group],
                                            sizeof(uint8) * dnx_data_nif.ofr.nof_rx_memory_groups_get(unit),
                                            "Alloc info per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            }

            SHR_IF_ERR_EXIT(dnx_port_ofr_link_list_switch_prepare(unit, logical_port, alloc_info));
        }

        /*
         * Build Arbiter's Rx shadow calendar
         */
        if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
        {
            _SHR_PBMP_CLEAR(calendars_to_be_set);
            SHR_IF_ERR_EXIT(dnx_port_arb_rx_calendar_ids_get(unit, logical_port, &calendars_to_be_set));
            SHR_IF_ERR_EXIT(dnx_port_arb_shadow_calendar_build(unit, calendars_to_be_set));
        }
        /*
         * Prepare WB resources, link list and calendar
         */
        {
            IMB_FLEXE_DIRECTION_RX_SET(flags);
            SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_prepare(unit, logical_port, flags));
            SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_prepare(unit, logical_port, flags));
        }
        /*
         * To avoid data loss the sequence is strict according to the packet datapath and rate change direction
         * (reversed sequence between speed increase/decrease).
         */
        if (is_increase)
        {
            if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
            {
                /*
                 * Set the OFR scheduler for this context
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_set(unit, logical_port));

                /*
                 * Switch OFR's calendar active id
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_link_list_switch(unit, logical_port, alloc_info));
            }

            if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
            {
                /*
                 * Switch Arbiter's calendar active id
                 */
                SHR_IF_ERR_EXIT(dnx_port_arb_calendar_switch_active_id(unit, calendars_to_be_set));
            }

            /** WB */
            {
                /** switch to new FIFO link list */
                SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_switch(unit, logical_port, flags));

                /** switch new calendar */
                SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_switch(unit, logical_port, flags));
            }

            /** Framer */
            {
                /** Configure 66b switching */
                SHR_IF_ERR_EXIT(dnx_port_flexe_66b_switch_ghao_config_set(unit, logical_port, speed));
            }
        }
        else
        {
            /** Framer */
            {
                /** Configure 66b switching */
                SHR_IF_ERR_EXIT(dnx_port_flexe_66b_switch_ghao_config_set(unit, logical_port, speed));
            }

            /** WB */
            {
                /** switch to new FIFO link list */
                SHR_IF_ERR_EXIT(dnx_port_flexe_fifo_resource_switch(unit, logical_port, flags));

                /** switch new calendar */
                SHR_IF_ERR_EXIT(dnx_port_flexe_calendar_switch(unit, logical_port, flags));
            }

            if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
            {
                /*
                 * Switch Arbiter's calendar active id
                 */
                SHR_IF_ERR_EXIT(dnx_port_arb_calendar_switch_active_id(unit, calendars_to_be_set));
            }

            if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
            {
                /*
                 * Switch OFR's calendar active id
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_link_list_switch(unit, logical_port, alloc_info));

                /*
                 * Set the OFR scheduler for this context
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_set(unit, logical_port));
            }
        }
    }
exit:
    if (alloc_info != NULL)
    {
        for (priority_group = 0; priority_group < dnx_data_nif.eth.priority_groups_nof_get(unit); priority_group++)
        {
            SHR_FREE(alloc_info[priority_group]);
        }
    }
    SHR_FREE(alloc_info);
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_resource_multi_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    dnx_port_resource_port_info_t *ports_info = NULL;
    bcm_port_t *master_ports = NULL;
    bcm_port_t port_to_add, port_to_remove;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;
    bcm_pbmp_t ports_with_modified_rx_speed, ports_with_modified_tx_speed;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Input validation */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_resource_multi_set_verify(unit, nport, resource));
    /*
     * Save user's ports which reside in the 'resource' struct.
     * These ports will be overriden by their logical ports, and will
     * be restored back to the 'resource' struct at the end of this function.
     *
     * This solution is quite ugly, but it needed since we shouldn't change the
     * user's input 'resource' param.
     */
    SHR_ALLOC_SET_ZERO(ports_info,
                       nport * sizeof(dnx_port_resource_port_info_t),
                       "User resource ports info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(master_ports, nport * sizeof(bcm_port_t), "Master Ports", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        ports_info[resource_index].original_port = resource[resource_index].port;
        /*
         * Convert user's gports to logical ports.
         * User's gports will be restored at the end of bcm_dnx_port_resource_multi_set.
         */
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource[resource_index].port, &resource[resource_index].port));
        /*
         * Get master logical ports
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, resource[resource_index].port, 0, &master_ports[resource_index]));
    }

    /** Validate 'resource' members */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_resource_multi_set_resource_verify(unit, nport, resource));
    /*
     * Get port info prior to calling resource_multi_set
     */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_port_info_get(unit, nport, resource, ports_info));
    /** Validate PHY loopback is not set on fabric ports */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, ports_info[resource_index].port_info))
        {
            int loopback;
            SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_get(unit, master_ports[resource_index], &loopback));
            if (loopback == BCM_PORT_LOOPBACK_PHY)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Can't configure resource while PHY loopback is set on port %d.",
                             master_ports[resource_index]);
            }
        }
    }

    _SHR_PBMP_CLEAR(ports_with_modified_rx_speed);
    _SHR_PBMP_CLEAR(ports_with_modified_tx_speed);
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported)
            || dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported))
        {
            if (ports_info[resource_index].is_tx_speed_change &&
                (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_info, 0)
                 || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index].port_info, 0, 0, 0)))
            {
                _SHR_PBMP_PORT_ADD(ports_with_modified_tx_speed, resource[resource_index].port);
            }

            if (ports_info[resource_index].is_rx_speed_change &&
                (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_info, 0)
                 || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index].port_info, 0, 0, 0)))
            {
                _SHR_PBMP_PORT_ADD(ports_with_modified_rx_speed, resource[resource_index].port);
            }
        }
    }

    /**
     * Devices without ESB have unlimited egress interface credits,
     * but on devices with ESB the maximum amount of credits is limited by the ESB size.
     * In case ESB exists, verify not crossing the maximum value of egress interface credits.
     */
    if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
    {
        SHR_IF_ERR_EXIT(dnx_port_if_total_egr_if_credits_sw_update(unit, nport, resource));
    }

    /** Run per module pre-configurations that are needed in resource_multi_set before disabling port. */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_set_modules_pre_config_set(unit, nport, resource, ports_info));

    /**
     * Handle G.HAO speed change.
     * Only a single port can be modified so index 0 is used hard coded
     * At this point new speed is already updated in db.
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (resource[resource_index].flags & BCM_PORT_RESOURCE_G_HAO)
        {
            /*
             * Mark the port as in g.hao sequence to pass calendars checks 
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_db.g_hao_active_client.set(unit, resource[resource_index].port));
            if (ports_info[resource_index].is_tx_speed_change)
            {
                SHR_IF_ERR_EXIT(dnx_port_resource_g_hao_tx_set(unit, resource[resource_index].port,
                                                               ports_info[resource_index].original_tx_speed));
            }

            if (ports_info[resource_index].is_rx_speed_change)
            {
                SHR_IF_ERR_EXIT(dnx_port_resource_g_hao_rx_set(unit, resource[resource_index].port,
                                                               ports_info[resource_index].original_rx_speed));
            }

            /** Run per module tunings that are needed after calling resource_multi_set */
            SHR_IF_ERR_EXIT(dnx_port_resource_multi_tune(unit, nport, resource, ports_info));

            SHR_IF_ERR_EXIT(dnx_port_nif_db.g_hao_active_client.set(unit, -1));
            SHR_EXIT();
        }
    }

    /** Get counter pbmp, */
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
    /** Disable ports. All enabled ports will be re-enabled at the end */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_info, TRUE))
        {
            port_to_remove =
                (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_info,
                                               DNX_ALGO_PORT_TYPE_INCLUDE(MGMT))) ?
                master_ports[resource_index] : resource[resource_index].port;
            /** Remove port from counter thread. */
            if (BCM_PBMP_MEMBER(counter_pbmp, port_to_remove))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port_to_remove, 0));
            }
            SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port_to_remove, 0, 0));
        }
    }

    /** Configure all resources. */
    SHR_IF_ERR_EXIT(imb_common_port_resource_multi_set(unit, nport, resource));

    /** Run per module configurations that are needed after calling resource_multi_set */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_set_modules_config_set(unit, nport, resource, ports_info));

        /** Run per IMB configurations that are needed after calling resource_multi_set */
    SHR_IF_ERR_EXIT(imb_common_post_resource_multi_set(unit, nport,
                                                       ports_with_modified_rx_speed, ports_with_modified_tx_speed,
                                                       resource));

    /** Run per module tunings that are needed after calling resource_multi_set */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_tune(unit, nport, resource, ports_info));
    /** Re-enable ports that was previously enabled */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_info, TRUE))
        {
            port_to_add =
                (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_info,
                                               DNX_ALGO_PORT_TYPE_INCLUDE(MGMT))) ?
                master_ports[resource_index] : resource[resource_index].port;
            if (ports_info[resource_index].is_port_enable == 1)
            {
                /** enable port */
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port_to_add, 0, 1));
            }

            /** Restore port to counter thread. */
            if (BCM_PBMP_MEMBER(counter_pbmp, port_to_add))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port_to_add, 1));
            }
        }
    }

exit:
    if (ports_info != NULL)
    {
        /** Restore user's ports configurations */
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            resource[resource_index].port = ports_info[resource_index].original_port;
        }
    }

    SHR_FREE(ports_info);
    SHR_FREE(master_ports);
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    bcm_gport_t resource_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /*
     * Input validation
     */
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    if (resource->port != port && resource->port != 0 && resource->port != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "resource->port must be equal to port");
    }

    /*
     * 'port' appear twice - in 'port' param and in resource->port.
     * They should be identical, but in case they not we change resource->port
     * to be the same as 'port'. At the end of the function we restore it since
     * we shouldn't change user's params.
     */
    resource_port = resource->port;
    /** Override resource->port (will be restored at end of function) */
    resource->port = port;
    /*
     * Call resource_multi_set with 1 port.
     * All further validations will be done inside.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, 1, resource));
exit:
    resource->port = resource_port;
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    int has_rx_speed = 0, has_tx_speed = 0, rx_speed = 0, tx_speed = 0;
    int flexe_core_port, is_am_transparent;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_resource_verify(unit, port, resource));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    bcm_port_resource_t_init(resource);
    /*
     * Get resource info only if speed has been configured.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
    if (has_rx_speed || has_tx_speed)
    {
        /*
         * Per port type handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(imb_common_port_resource_get(unit, logical_port, resource));
            /*
             * The params 'port' and 'resource->port' should be equal,
             * so if the user gave fabric gport as input - we should
             * return a fabric gport too.
             */
            if (BCM_GPORT_IS_LOCAL_FABRIC(port))
            {
                int link_id;
                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));
                BCM_GPORT_LOCAL_FABRIC_SET(resource->port, link_id);
            }
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE)
                || (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
                    && dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));
                if (flexe_core_port != DNX_ALGO_PORT_INVALID)
                {
                    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
                    {
                        /*
                         * Return base flexe instance ID for FlexE physical port
                         */
                        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get
                                        (unit, flexe_core_port, &(resource->base_flexe_instance)));
                    }
                    else
                    {
                        /*
                         * Check if AM transparent is enabled for L1 ETH port
                         */
                        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_am_transparent_get
                                        (unit, flexe_core_port, &is_am_transparent));
                        if (is_am_transparent)
                        {
                            resource->flags |= BCM_PORT_RESOURCE_AM_TRANSPARENT;
                        }
                    }
                }
            }
            /*
             * Return first logical lane for OTN physical port
             */
            if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_lane_map_otn_logical_lane_get
                                (unit, logical_port, NULL, &resource->logical_otn_lane));
            }
        }
        else
        {
            /*
             * Get Rx and Tx speed separately
             */
            if (has_rx_speed)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
            }
            if (has_tx_speed)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
            }
            resource->speed = tx_speed;
            /*
             * Check the Rx and Tx speed
             */
            if (rx_speed != tx_speed)
            {
                resource->flags |= BCM_PORT_RESOURCE_ASYMMETRICAL;
                resource->rx_speed = rx_speed;
            }
        }
    }
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_resource_verify(unit, port, resource));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /*
     * The params 'port' and 'resource->port' should be equal,
     * so if we receive fabric gport for 'resource->port' we need to
     * translate it to logical port.
     */
    if (BCM_GPORT_IS_LOCAL_FABRIC(resource->port))
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource->port, &resource->port));
    }

    SHR_IF_ERR_EXIT(imb_port_resource_default_get(unit, logical_port, flags, resource));
    /*
     * Validate the SPEED or speed/fec combination for NIF ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH
        (unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT, FRAMER_MGMT, FEEDBACK))
        || DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(imb_port_fec_speed_validate(unit, logical_port, resource->fec_type, resource->speed));
    }

    /*
     * The params 'port' and 'resource->port' should be equal,
     * so if the user gave fabric gport as input - we should
     * return a fabric gport too.
     */
    if (BCM_GPORT_IS_LOCAL_FABRIC(port))
    {
        int link_id;
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));
        BCM_GPORT_LOCAL_FABRIC_SET(resource->port, link_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for the following APIs
 *
 *    dnx_port_remote_fault_status_clear()
 *    dnx_port_remote_fault_status_get()
 *    dnx_port_local_fault_status_clear()
 *    dnx_port_local_fault_status_get()
 *    dnx_port_faults_status_get()
 */
static shr_error_e
dnx_port_link_fault_status_verify(
    int unit,
    bcm_port_t port,
    int *value)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    /** Verify port Type */
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info) &&
        !DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)) &&
        !DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE) &&
        !((DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info)
           || DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
          && dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_embeded_mac_in_busa_busc)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The port doesn't support Local/Remote fault.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get local and remote fault status
 */
static shr_error_e
dnx_port_faults_status_get(
    int unit,
    int port,
    int *local_fault,
    int *remote_fault)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, local_fault));
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, remote_fault));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_faults_status_get(unit, logical_port, local_fault, remote_fault));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault status
 */
static shr_error_e
dnx_port_remote_fault_status_get(
    int unit,
    int port,
    int *value)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_status_get(unit, logical_port, value));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault status
 */
static shr_error_e
dnx_port_local_fault_status_get(
    int unit,
    int port,
    int *value)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_status_get(unit, logical_port, value));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Clear remote fault status
 */
static shr_error_e
dnx_port_remote_fault_status_clear(
    int unit,
    int port)
{
    bcm_port_t logical_port = 0;
    int value;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, &value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_status_clear(unit, logical_port));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Clear local fault status
 */
static shr_error_e
dnx_port_local_fault_status_clear(
    int unit,
    int port)
{
    bcm_port_t logical_port = 0;
    int value;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_status_verify(unit, port, &value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_status_clear(unit, logical_port));
exit:SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for the following APIs
 *
 *    dnx_port_local_fault_enable_get()
 *    dnx_port_local_fault_enable_set()
 *    dnx_port_remote_fault_enable_get()
 *    dnx_port_remote_fault_enable_set()
 *    dnx_port_link_fault_force_set()
 *    dnx_port_link_fault_force_get()
 */
static shr_error_e
dnx_port_link_fault_enable_force_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /** Verify port Type */
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, 1, 1, 1)
        && !DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Wrong port type, only FlexE-MAC and ETH type are supported!\r\n");
    }
    if (is_set)
    {
        SHR_BOOL_VERIFY(*value, _SHR_E_PARAM, "Only values 0 and 1 are supported!\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get local fault enable value
 */
static shr_error_e
dnx_port_local_fault_enable_get(
    int unit,
    int port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, enable, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Set local fault enable value
 */
static shr_error_e
dnx_port_local_fault_enable_set(
    int unit,
    int port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, &enable, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault enable value
 */
static shr_error_e
dnx_port_remote_fault_enable_get(
    int unit,
    int port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, enable, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Set remote fault enable value
 */
static shr_error_e
dnx_port_remote_fault_enable_set(
    int unit,
    int port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, &enable, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - force LF/RF in MAC
 */
static shr_error_e
dnx_port_link_fault_force_set(
    int unit,
    int port,
    bcm_port_control_t type,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, &enable, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_link_fault_force_set(unit, logical_port, type, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get LF/RF force status
 */
static shr_error_e
dnx_port_link_fault_force_get(
    int unit,
    int port,
    bcm_port_control_t type,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_fault_enable_force_verify(unit, port, enable, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_link_fault_force_get(unit, logical_port, type, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for dnx_port_remote_fault_status_clear()/get() and
 *          dnx_port_local_fault_status_clear()/get()API
 */
static shr_error_e
dnx_port_pad_size_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
    {
        /** verify if ethu supporting MAC padding */
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
        if (FALSE == dnx_data_nif.eth.mac_padding_get(unit, ethu_info.imb_type)->is_supported)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MAC Padding is not supported\r\n");
        }

        if ((*value < dnx_data_nif.eth.pad_size_min_get(unit)
             || *value > dnx_data_nif.eth.pad_size_max_get(unit)) && (*value != 0) && (is_set))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong padding size %d for port 0x%x\r\n", *value, port);
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, 1, 1, 1))
    {
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_tx_pading_supported))
        {
            if ((*value < dnx_data_nif.eth.pad_size_min_get(unit) || *value > dnx_data_nif.eth.pad_size_max_get(unit))
                && (*value != 0) && (is_set))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong padding size %d for port 0x%x\r\n", *value, port);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Pading feature is not supported for port %d\r\n", port);
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        if (*value != dnx_data_nif.ilkn.pad_size_get(unit) && (*value != 0) && (is_set))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong padding size %d for port 0x%x\r\n", *value, port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set pad size, TX pad packets to the specified size
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pad_size - pad size (min 64 - max 96)
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
dnx_port_pad_size_set(
    int unit,
    bcm_port_t port,
    int pad_size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_pad_size_verify(unit, port, &pad_size, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_pad_size_set(unit, logical_port, pad_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TX pad size value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] pad_size - pad size (min 64 - max 96)
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
dnx_port_pad_size_get(
    int unit,
    bcm_port_t port,
    int *pad_size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_pad_size_verify(unit, port, pad_size, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_pad_size_get(unit, logical_port, pad_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the lane gport is legal
 *
 * \param [in] unit - chip unit id
 * \param [in] port - lane gport
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_lane_gport_verify(
    int unit,
    bcm_port_t port)
{
    int nof_phys;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    /** verify port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));
    /** verify lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, gport_info.local_port, &nof_phys));
    SHR_RANGE_VERIFY(gport_info.lane, 0, nof_phys - 1, _SHR_E_PARAM, "Lane ID is out of range.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the port type is supported by Prbs function.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prbs_port_verify(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        SHR_IF_ERR_EXIT(dnx_port_lane_gport_verify(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the prbs polynomial value is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] value - polynomial value
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_polynomial_verify(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));
    /** for set API, verify if the poly vaule is correct. */
    if (is_set)
    {
        switch (value)
        {
            case BCM_PORT_PRBS_POLYNOMIAL_X7_X6_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X15_X14_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X23_X18_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X9_X5_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X11_X9_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X58_X31_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X49_X40_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X10_X7_1:
            case BCM_PORT_PRBS_POLYNOMIAL_PAM4_13Q:
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Poly value %d is not supported\r\n", value);
        }
    }

exit:SHR_FUNC_EXIT;
}
/**
 * \brief - Verify if the parameters for prbs data inversion
 *    function is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] invert - enable or disable inversion
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prbs_invert_data_verify(
    int unit,
    bcm_port_t port,
    int type,
    int invert,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));
    /** verify type */
    switch (type)
    {
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prbs invert type %d is not supported\r\n", type);
    }

    /** for set API, verify if the value is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(invert, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the parameters for prbs enable
 *    function is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] value - enable or disable prbs
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_enable_verify(
    int unit,
    bcm_port_t port,
    int type,
    int value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));
    /** verify type */
    switch (type)
    {
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prbs enable type %d is not supported\r\n", type);
    }

    /** for set API, verify if the value is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify if the parameters for 'fabric only' enable controls.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable
 * \param [in] is_set - if this fuction is called by set function
 */
static shr_error_e
dnx_port_fabric_control_enable_verify(
    int unit,
    bcm_port_t port,
    int enable,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    /** for set API, verify if the 'enable' is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Set prbs mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port
 * \param [in] prbs_mode - prbs mode value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prbs_mode_set(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_set(unit, logical_port, prbs_mode));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port
 * \param [out] prbs_mode - prbs mode value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prbs_mode_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t * prbs_mode)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, prbs_mode));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Set prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] value - prbs polynomial value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_polynomial_set(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t value)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_polynomial_verify(unit, port, value, 1 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL, value));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_polynomial_set(unit, logical_port, prbs_mode, value));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [out] value - prbs polynomial value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_polynomial_get(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t * value)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_polynomial_verify(unit, port, *value, 0 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL, value));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_polynomial_get(unit, logical_port, prbs_mode, value));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Whether enable prbs data inversion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] invert - enable or disable data inversion
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_invert_data_set(
    int unit,
    bcm_port_t port,
    int type,
    int invert)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_invert_data_verify(unit, port, type, invert, 1 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxInvertData) ? BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA, invert));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_invert_data_set(unit, logical_port, type, prbs_mode, invert));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the prbs data inversion status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [out] invert - enable or disable data inversion
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prbs_invert_data_get(
    int unit,
    bcm_port_t port,
    int type,
    int *invert)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_invert_data_verify(unit, port, type, *invert, 0 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxInvertData) ? BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA, (uint32 *) invert));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_invert_data_get(unit, logical_port, type, prbs_mode, invert));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable Prbs
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] enable - enable or disable prbs
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_enable_set(
    int unit,
    bcm_port_t port,
    int type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_enable_verify(unit, port, type, enable, 1 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxEnable) ? BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE, enable));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_enable_set(unit, logical_port, type, prbs_mode, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [out] enable - enable or disable prbs
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_enable_get(
    int unit,
    bcm_port_t port,
    int type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_enable_verify(unit, port, type, *enable, 0 /* is set */ ));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxEnable) ? BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE, (uint32 *) enable));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_enable_get(unit, logical_port, type, prbs_mode, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [out] status - prbs status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_prbs_rx_status_get(
    int unit,
    bcm_port_t port,
    int *status)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_prbs_port_verify(unit, port));
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS, (uint32 *) status));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_rx_status_get(unit, logical_port, prbs_mode, status));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable low latency llfc.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable low latency llfc
 */
static shr_error_e
dnx_port_low_latency_llfc_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_low_latency_llfc_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether low latency llfc is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether low latency llfc is enabled or disabled
 */
static shr_error_e
dnx_port_low_latency_llfc_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_low_latency_llfc_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable 'FEC error detect'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'FEC error detect'
 */
static shr_error_e
dnx_port_fec_error_detect_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_fec_error_detect_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether 'FEC error detect' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'Fec error detect' is enabled or disabled
 */
static shr_error_e
dnx_port_fec_error_detect_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_fec_error_detect_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable 'extract LLFC cells congestion indication'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'extract LLFC cells
 *                      congestion indication'
 */
static shr_error_e
dnx_port_llfc_cells_congestion_ind_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_extract_congestion_indication_from_llfc_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether 'extract LLFC cells congestion indication' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'extract LLFC cells congestion indication'
 *                       is enabled or disabled
 */
static shr_error_e
dnx_port_llfc_cells_congestion_ind_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_extract_congestion_indication_from_llfc_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable 'LLFC after FEC'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'LLFC after FEC'
 */
static shr_error_e
dnx_port_llfc_after_fec_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_llfc_after_fec_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether 'LLFC after FEC' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'LLFC after FEC' is enabled or disabled
 */
static shr_error_e
dnx_port_llfc_after_fec_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_llfc_after_fec_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable 'FEC Bypass of Control Cells'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'FEC Bypass of Control Cells'
 */
static shr_error_e
dnx_port_control_cells_fec_bypass_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_control_cells_fec_bypass_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether 'FEC Bypass of Control Cells' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - is 'FEC Bypass of Control Cells' enabled/disabled
 */
static shr_error_e
dnx_port_control_cells_fec_bypass_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_control_cells_fec_bypass_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_stat_oversize_set()/get() API
 */
static shr_error_e
dnx_port_stat_max_size_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    if ((*value < 0 || *value > dnx_data_nif.eth.packet_size_max_get(unit)) && (is_set))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong max size %d for port 0x%x\r\n", *value, port);
    }
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the threshold above which packet will be counted as oversized.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - threshold above which packet will be counted as oversized
 */
static shr_error_e
dnx_port_control_stat_oversize_set(
    int unit,
    bcm_port_t port,
    int size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_stat_max_size_verify(unit, port, &size, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_cntmaxsize_set(unit, logical_port, size));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the threshold above which packet will be counted as oversized.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] size - threshold above which packet will be counted as oversized
 */
static shr_error_e
dnx_port_control_stat_oversize_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_stat_max_size_verify(unit, port, size, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_cntmaxsize_get(unit, logical_port, size));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify function for dnx_port_control_link_recovery_enable_set()/get() API
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable
 * \param [in] is_set - if this fuction is called by set function
 */
static shr_error_e
dnx_port_link_recovery_enable_verify(
    int unit,
    bcm_port_t port,
    int enable,
    int is_set)
{
    bcm_port_t logical_port;
    int first_phy;
    int table_index, pm_index;
    int is_link_recovery_support = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy));
        table_index = first_phy / dnx_data_nif.eth.phy_map_granularity_get(unit);
        pm_index = dnx_data_nif.eth.phy_map_get(unit, table_index)->pm_index;
        is_link_recovery_support = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->link_recovery_support;
    }
    /*
     * Verify link recovery
     */
    if (!is_link_recovery_support)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d - The link recovery is not configurable on this port.\n", port);
    }
    /** for set API, verify if the 'enable' is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/disable link_recovery..
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disabled
 */
static shr_error_e
dnx_port_control_link_recovery_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_recovery_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_link_recovery_enable_set(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/disable link_recovery..
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable or disabled
 */
static shr_error_e
dnx_port_control_link_recovery_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_link_recovery_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_link_recovery_enable_get(unit, logical_port, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_rx_enable_set()/get() API
 */
static shr_error_e
dnx_port_rx_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "Port %d must be of GPORT format. Please use BCM_GPORT_LOCAL_SET for a specific port or BCM_ GPORT_LOCAL_INTERFACE_SET for the entire interface.\r\n",
                     port);
    }

    if (BCM_GPORT_IS_LOCAL(port) && !BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "bcmPortControlRxEnable with BCM_GPORT_LOCAL is for channelized port while RX channels are not supported in this device.\r\n");
    }

    /** NULL checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport_info.local_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/Disable a port's RX direction only.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 */
static shr_error_e
dnx_port_control_rx_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_rx_enable_verify(unit, port, &enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(portmod_port_enable_set
                    (unit, logical_port, PORTMOD_PORT_ENABLE_RX | PORTMOD_PORT_ENABLE_PHY, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve enabling status of a port's RX direction.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 */
static shr_error_e
dnx_port_control_rx_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_rx_enable_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(portmod_port_enable_get
                    (unit, logical_port, PORTMOD_PORT_ENABLE_RX | PORTMOD_PORT_ENABLE_PHY, enable));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_tx_enable_set()/get() API
 */
static shr_error_e
dnx_port_tx_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    if (!BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "Port %d must be in GPORT format. Please use BCM_GPORT_LOCAL_SET for a specific port or BCM_ GPORT_LOCAL_INTERFACE_SET for the entire interface\r\n",
                     port);
    }

    /** NULL checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport_info.local_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/Disable a port's TX direction only.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 */
static shr_error_e
dnx_port_control_tx_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_algo_port_info_s port_info;
    int current_enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_tx_enable_verify(unit, port, &enable));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF
        (unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(FLEXE, OTN_PHY, FRAMER_MGMT, MGMT, FEEDBACK)))
    {
        /** Check if the port is enabled.
         * If it is not return error, TX enable/disable is supported only on enabled ports */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, gport_info.local_port, &current_enable));
        if (current_enable == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Port %d must be enabled. TX enable/disable is supported only on enabled ports!\n", port);
        }
    }
    if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, gport_info.local_port, PORTMOD_PORT_ENABLE_TX, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_tm_port_enable_and_flush_set(unit, gport_info.local_port, enable, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve enabling status of a port's TX direction.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 */
static shr_error_e
dnx_port_control_tx_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_tx_enable_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, gport_info.local_port, PORTMOD_PORT_ENABLE_TX, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_tm_port_enable_get(unit, gport_info.local_port, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_data_rate_get() API
 */
static shr_error_e
dnx_port_data_rate_verify(
    int unit,
    bcm_port_t port,
    int *data_rate)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(data_rate, _SHR_E_PARAM, "data_rate");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    /** get port info and check if it is ILKN */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "bcmPortControlDataRate is supported for ILKN port only.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve a port's data rate.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] data_rate - port data rate
 */
static shr_error_e
dnx_port_control_data_rate_get(
    int unit,
    bcm_port_t port,
    int *data_rate)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_data_rate_verify(unit, port, data_rate));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, data_rate));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for dnx_port_control_start_tx_threshold_set/get() API
 */
static shr_error_e
dnx_port_start_tx_threshold_verify(
    int unit,
    bcm_port_t port,
    int *start_tx)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    /** NULL checks */
    SHR_NULL_CHECK(start_tx, _SHR_E_PARAM, "start_tx");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    /** get port info and check if it is FLEXE MAC */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, 1, 0, 0))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "bcmPortControlStartTxThreshold is supported for FlexE MAC port only.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure start tx threshold.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] start_tx - start tx threshold
 */
static shr_error_e
dnx_port_control_start_tx_threshold_set(
    int unit,
    bcm_port_t port,
    int start_tx)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_start_tx_threshold_verify(unit, port, &start_tx));
    SHR_IF_ERR_EXIT(imb_port_tx_start_thr_set(unit, port, -1, start_tx));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Retrieve a port's Tx start tx threshold.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] start_tx - start tx threshold
 */
static shr_error_e
dnx_port_control_start_tx_threshold_get(
    int unit,
    bcm_port_t port,
    int *start_tx)
{
    SHR_FUNC_INIT_VARS(unit);
    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_start_tx_threshold_verify(unit, port, start_tx));
    SHR_IF_ERR_EXIT(imb_port_tx_start_thr_get(unit, port, start_tx));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify APIs parameters for TXPI APIs.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - configuration value
 */
static shr_error_e
dnx_port_txpi_sdm_verify(
    int unit,
    bcm_port_t port,
    int *val)
{
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;
    int is_ext_txpi_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** Check port type and TXPI mode */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ext_txpi_enable_get(unit, port, &is_ext_txpi_enable));
        if (!is_ext_txpi_enable)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "External TXPI mode is not enabled.\r\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported port type.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Configure TXPI SDM override value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - override value
 */
static shr_error_e
dnx_port_txpi_sdm_override_set(
    int unit,
    bcm_port_t port,
    int val)
{
    portmod_txpi_override_t override;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_txpi_sdm_verify(unit, port, &val));

    override.enable = val ? 1 : 0;
    override.value = val;
    SHR_IF_ERR_EXIT(imb_port_txpi_sdm_override_set(unit, port, &override));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get TXPI SDM override value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] val - override value
 */
static shr_error_e
dnx_port_txpi_sdm_override_get(
    int unit,
    bcm_port_t port,
    int *val)
{
    portmod_txpi_override_t override;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_txpi_sdm_verify(unit, port, val));
    SHR_IF_ERR_EXIT(imb_port_txpi_sdm_override_get(unit, port, &override));
    *val = override.enable ? override.value : 0;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get TXPI SDM output value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] val - SDM output value
 */
static shr_error_e
dnx_port_txpi_sdm_status_get(
    int unit,
    bcm_port_t port,
    int *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_txpi_sdm_verify(unit, port, val));
    SHR_IF_ERR_EXIT(imb_port_txpi_sdm_status_get(unit, port, val));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Verify APIs parameters for dnx_port_txpi_ctrl_select_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - configuration value
 */
static shr_error_e
dnx_port_txpi_ctrl_select_verify(
    int unit,
    bcm_port_t port,
    int val)
{
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** Check port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported port type. The port type must be L1 ETH port!\r\n");
    }
    if (val != port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, val, &port_info));
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_FEEDBACK(unit, port_info))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "The txpi port must be feedback port!\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Configure the TXPI control selection
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - SDM output value
 */
static shr_error_e
dnx_port_txpi_ctrl_select_set(
    int unit,
    bcm_port_t port,
    int val)
{
    int first_phy, lane_id, txpi_lane, map_size;
    bcm_pbmp_t phys;
    uint32 entry_handle_id;
    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_txpi_ctrl_select_verify(unit, port, val));

    map_size = dnx_data_nif.phys.nof_phys_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, lane2serdes));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, val, 0, &first_phy));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ETH_TXPI_CTRL, &entry_handle_id));
    _SHR_PBMP_ITER(phys, lane_id)
    {
        if (lane_id >= dnx_data_nif.phys.nof_phys_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "The lane ID is out of bound.\n");
        }
        /*
         * Set Key
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_IN_CORE, lane2serdes[lane_id].tx_id);
        /*
         * set value fields
         */
        txpi_lane = (port == val) ? lane2serdes[lane_id].tx_id : lane2serdes[first_phy].tx_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXPI_LANE, INST_SINGLE, txpi_lane);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  Verify parameters for port_if_control_set/get functions
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - gport or logical port number.
 * \param [in] control_type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] value - Value of the bit field in port table
 * \param [in] is_set - should be TRUE if calling from set function and FALSE if calling from get function
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * This function does only partial verification for some of the controls.
 *   The rest is done inside the separate function for each control.
 * \see
 *   dnx_port_if_control_get
 */
static shr_error_e
dnx_port_if_control_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t control_type,
    int value,
    int is_set)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    switch (control_type)
    {
        case bcmPortControlLowLatencyLLFCEnable:
        case bcmPortControlFecErrorDetectEnable:
        case bcmPortControlLlfcCellsCongestionIndEnable:
        case bcmPortControlLLFCAfterFecEnable:
        case bcmPortControlControlCellsFecBypassEnable:
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_fec_control_verify(unit, logical_port, control_type));
            }
            break;

        case bcmPortControlPadToSize:
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_fmac_tx_padding_verify(unit, logical_port, value, is_set));
            }
            break;

        case bcmPortControlLinkFaultLocal:
        case bcmPortControlLinkFaultRemote:
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info) && is_set
                && !dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_linkup_indication_map_supported))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "bcmPortControlLinkFaultLocal and bcmPortControlLinkFaultRemote not supported for SET API on Fabric ports \n");
            }
            break;

        case bcmPortControlLinkFaultRemoteEnable:
        case bcmPortControlLinkFaultLocalEnable:
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Reading local and remote faults is always enabled for Fabric ports. \n");
            }
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set various features at the port NIF level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_if_control_get
 */
int
dnx_port_if_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_if_control_verify(unit, port, type, value, TRUE));

    switch (type)
    {
        case bcmPortControlLinkFaultLocal:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_status_clear(unit, port));
            break;
        }
        case bcmPortControlLinkFaultRemote:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_status_clear(unit, port));
            break;
        }
        case bcmPortControlLinkFaultRemoteEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlPadToSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_pad_size_set(unit, port, value));
            break;
        }
        case bcmPortControlPCS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlPCS is no longer supported. Please use bcm_port_resource_set to control FEC\n");
            break;
        }
        case bcmPortControlLinkDownPowerOn:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlLinkDownPowerOn is no longer supported. Please use bcm_port_enable_set instead\n");
            break;
        }
        case bcmPortControlPrbsMode:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_mode_set(unit, port, value));
            break;
        }
        case bcmPortControlPrbsPolynomial:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_polynomial_set(unit, port, value));
            break;
        }
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_invert_data_set(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_enable_set(unit, port, type, value));
            break;
        }
        case bcmPortControlLowLatencyLLFCEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_low_latency_llfc_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlFecErrorDetectEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_fec_error_detect_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLlfcCellsCongestionIndEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_cells_congestion_ind_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLLFCAfterFecEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_after_fec_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlControlCellsFecBypassEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_cells_fec_bypass_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlStatOversize:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_stat_oversize_set(unit, port, value));
            break;
        }
        case bcmPortControlPortmodLinkRecoveryEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_link_recovery_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlRxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_rx_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_tx_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlDataRate:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmPortControlDataRate is supported for get API only.\n");
            break;
        }
        case bcmPortControlStartTxThreshold:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_start_tx_threshold_set(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalForce:
        case bcmPortControlLinkFaultRemoteForce:
        {
            SHR_IF_ERR_EXIT(dnx_port_link_fault_force_set(unit, port, type, value));
            break;
        }
        case bcmPortControlSarToIngressCellMode:
        case bcmPortControlEgressToSarCellMode:
        {
            SHR_IF_ERR_EXIT(dnx_flexe_sar_cell_mode_per_port_set(unit, port, type, value));
            break;
        }
        case bcmPortControlTXPISDMOverride:
        {
            SHR_IF_ERR_EXIT(dnx_port_txpi_sdm_override_set(unit, port, value));
            break;
        }
        case bcmPortControlL1Eth1588RxEnable:
        case bcmPortControlL1Eth1588TxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_l1_eth_1588_enable_set(unit, port, type, value));
            break;
        }
        case bcmPortControlTxpiCtrlSelect:
        {
            SHR_IF_ERR_EXIT(dnx_port_txpi_ctrl_select_set(unit, port, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_control_t type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get various features at the port NIF level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] *value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_if_control_set
 */
int
dnx_port_if_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_if_control_verify(unit, port, type, *value, FALSE));

    switch (type)
    {
        case bcmPortControlLinkFaultLocal:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultRemote:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultRemoteEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlPadToSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_pad_size_get(unit, port, value));
            break;
        }
        case bcmPortControlPCS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlPCS is no longer supported. Please use bcm_port_resource_get to control FEC\n");
            break;
        }
        case bcmPortControlLinkDownPowerOn:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlLinkDownPowerOn is no longer supported. Please use bcm_port_enable_get instead\n");
            break;
        }
        case bcmPortControlPrbsMode:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_mode_get(unit, port, (portmod_prbs_mode_t *) value));
            break;
        }
        case bcmPortControlPrbsPolynomial:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_polynomial_get(unit, port, (bcm_port_prbs_t *) value));
            break;
        }
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_invert_data_get(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_enable_get(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxStatus:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_rx_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLowLatencyLLFCEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_low_latency_llfc_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlFecErrorDetectEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_fec_error_detect_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLlfcCellsCongestionIndEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_cells_congestion_ind_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLLFCAfterFecEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_after_fec_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlControlCellsFecBypassEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_cells_fec_bypass_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlStatOversize:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_stat_oversize_get(unit, port, value));
            break;
        }
        case bcmPortControlPortmodLinkRecoveryEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_link_recovery_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlRxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_rx_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_tx_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlDataRate:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_data_rate_get(unit, port, value));
            break;
        }
        case bcmPortControlStartTxThreshold:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_start_tx_threshold_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalForce:
        case bcmPortControlLinkFaultRemoteForce:
        {
            SHR_IF_ERR_EXIT(dnx_port_link_fault_force_get(unit, port, type, value));
            break;
        }
        case bcmPortControlSarToIngressCellMode:
        case bcmPortControlEgressToSarCellMode:
        {
            SHR_IF_ERR_EXIT(dnx_flexe_sar_cell_mode_per_port_get
                            (unit, port, type, (bcm_port_control_sar_cell_mode_t *) value));
            break;
        }
        case bcmPortControlTXPISDMOverride:
        {
            SHR_IF_ERR_EXIT(dnx_port_txpi_sdm_override_get(unit, port, value));
            break;
        }
        case bcmPortControlTXPISDMStatus:
        {
            SHR_IF_ERR_EXIT(dnx_port_txpi_sdm_status_get(unit, port, value));
            break;
        }
        case bcmPortControlL1Eth1588RxEnable:
        case bcmPortControlL1Eth1588TxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_l1_eth_1588_enable_get(unit, port, type, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "dnx_port_if_control_get type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_fault_get(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    bcm_port_t logical_port = 0;
    int local_fault, remote_fault;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    /** reset flags */
    *flags = 0;
    SHR_IF_ERR_EXIT(dnx_port_faults_status_get(unit, logical_port, &local_fault, &remote_fault));
    if (local_fault)
    {
        *flags |= BCM_PORT_FAULT_LOCAL;
    }
    if (remote_fault)
    {
        *flags |= BCM_PORT_FAULT_REMOTE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_lane_to_serdes_map_get and
 *       bcm_dnx_port_lane_to_serdes_map_set API.
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side.
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [in] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_lane_to_serdes_map_verify(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Null checks */
    SHR_NULL_CHECK(serdes_map, _SHR_E_PARAM, "serdes_map");
    /** Flags */
    SHR_MASK_VERIFY(flags, DNX_PORT_LANE_MAP_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");
    /** Verify the map size */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_size_verify(unit, DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags), map_size));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Lane to Serdes Mapping for NIF and Fabric.
 *         Each lane should be both Rx and Tx mapped, or NOT mapped.
 *
 *         For NiF ports, the lane mapping is allowed within a same octet.
 *         For Fabric ports, the lane mapping is allowed within a same quartet,
 *         The Rx lane mapping should be one to one mapping for Fabric ports.
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side.
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [in] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_lane_to_serdes_map_set(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int lane_id;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_lane_to_serdes_map_verify(unit, flags, map_size, serdes_map));
    SHR_ALLOC(lane2serdes,
              map_size * sizeof(soc_dnxc_lane_map_db_map_t),
              "Lane to serdes mapping struct", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (lane_id = 0; lane_id < map_size; ++lane_id)
    {
        lane2serdes[lane_id].rx_id = serdes_map[lane_id].serdes_rx_id;
        lane2serdes[lane_id].tx_id = serdes_map[lane_id].serdes_tx_id;
    }

    type = DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_set(unit, type, map_size, lane2serdes));
exit:
    SHR_FREE(lane2serdes);
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Lane to Serdes Mapping for NIF and Fabric.
 *
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side.
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [out] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_lane_to_serdes_map_get(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int lane_id;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_lane_to_serdes_map_verify(unit, flags, map_size, serdes_map));
    SHR_ALLOC(lane2serdes,
              map_size * sizeof(soc_dnxc_lane_map_db_map_t),
              "Lane to serdes mapping struct", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    type = DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, type, map_size, lane2serdes));
    for (lane_id = 0; lane_id < map_size; ++lane_id)
    {
        serdes_map[lane_id].serdes_rx_id = lane2serdes[lane_id].rx_id;
        serdes_map[lane_id].serdes_tx_id = lane2serdes[lane_id].tx_id;
    }

exit:SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_ilkn_lane_id_map_set and
 *       bcm_dnx_port_ilkn_lane_id_map_get API.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [in] lanes - list of device lanes ordered by ILKN ID.
 * \param [in] is_set - if the verify function is called by set API
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ilkn_lane_id_map_verify(
    int unit,
    bcm_port_t port,
    int num_of_lanes,
    int *lanes,
    int is_set)
{
    bcm_pbmp_t port_lanes_bmp;
    int nof_phys, i;
    SHR_FUNC_INIT_VARS(unit);
    /** Null checks */
    SHR_NULL_CHECK(lanes, _SHR_E_PARAM, "lanes");
    /** Verify the port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    /** Verify lane numbers */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));
    SHR_VAL_VERIFY(num_of_lanes, nof_phys, _SHR_E_PARAM, "Expect num_of_lanes is equal to the phys in this ILKN port");
    /*
     * Verify the ILKN lane id value for set function
     */
    if (is_set)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &port_lanes_bmp));
        for (i = 0; i < num_of_lanes; ++i)
        {
            /** Verify lane id is not out of range */
            SHR_RANGE_VERIFY(lanes[i], 0, DNX_DATA_MAX_NIF_PHYS_NOF_PHYS - 1, _SHR_E_PARAM, "Lane is out of range.\n");
            /** Verify lane is a member of port lanes */
            if (!BCM_PBMP_MEMBER(port_lanes_bmp, lanes[i]))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "lane %d is not a member OR is duplicated\n", lanes[i]);
            }
            BCM_PBMP_PORT_REMOVE(port_lanes_bmp, lanes[i]);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_cmpfunc(
    void *a,
    void *b)
{
    return (*(int *) a - *(int *) b);
}

/**
 * \brief - This API is used to configure the order of the ILKN lanes.
  * Each index in lanes[] array stands for ILKN lane ID. The values of lanes[] array is the NIF/Fabric lanes mapped to ILKN logical lane.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - support flags
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [in] lanes - array in size of num_of_lanes containing interface lanes.
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
bcm_dnx_port_ilkn_lane_map_set(
    int unit,
    int flags,
    bcm_port_t port,
    int num_of_lanes,
    int *lanes)
{
    int i, j;
    int ilkn_lane_id[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int ilkn_lanes_ordered[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int index[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int result[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    dnx_algo_port_ilkn_lane_info_t ilkn_lane_info;
    int is_over_fabric;
    int loopback;
    dnx_algo_lane_map_type_e type;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_ilkn_lane_id_map_verify(unit, port, num_of_lanes, lanes, 1  /* is_set */
                           ));
    SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_get(unit, port, &loopback));
    if (loopback == BCM_PORT_LOOPBACK_MAC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Setting Lane order with MAC loopback is not Allowed!\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;
    /*
     * map ilkn_lane_id[i]
     */
    for (i = 0; i < num_of_lanes; i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_ilkn_lane_info_get(unit, port, type, lanes[i], &ilkn_lane_info));
        ilkn_lane_id[i] = ilkn_lane_info.ilkn_id;
    }

    /*
     * now convert the array of ilkn lanes to order ilkn lanes
     */
    sal_memcpy(ilkn_lanes_ordered, ilkn_lane_id, (num_of_lanes * sizeof(int)));
    _shr_sort(ilkn_lanes_ordered, num_of_lanes, sizeof(*ilkn_lanes_ordered), dnx_port_cmpfunc);
    j = 0;
    for (i = 0; i < num_of_lanes; i++)
    {
        index[ilkn_lane_id[i]] = j++;
    }

    for (i = 0; i < num_of_lanes; i++)
    {
        result[i] = index[ilkn_lanes_ordered[i]];
    }

    SHR_IF_ERR_EXIT(imb_port_logical_lane_order_set(unit, port, lanes, result, num_of_lanes));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the order of the ILKN lanes for ILKN ports. Each index in lanes[] array stands for ILKN lane ID.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - support flags
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [out] lanes - array in size of num_of_lanes containing interface lanes.
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
bcm_dnx_port_ilkn_lane_map_get(
    int unit,
    int flags,
    bcm_port_t port,
    int num_of_lanes,
    int *lanes)
{
    int i, j;
    int actual_size;
    int phy_iter;
    bcm_pbmp_t device_lanes_bmp;
    int phy_lanes[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int ilkn_lanes_ordered[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int index[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    int order[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    int result_phy[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF] = {
        0
    };
    dnx_algo_port_ilkn_lane_info_t ilkn_lane_info;
    int is_over_fabric;
    dnx_algo_lane_map_type_e type;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_ilkn_lane_id_map_verify(unit, port, num_of_lanes, lanes, 0  /* is_set */
                           ));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &device_lanes_bmp));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;
    i = 0;
    BCM_PBMP_ITER(device_lanes_bmp, phy_iter)
    {
        /*
         * Get the ILKN lane re-mapping configuration from ilkn_lanes_map Soc property
         */
        phy_lanes[i++] = phy_iter;
    }

    /*
     * initialize the index array
     */
    for (i = 0; i < sizeof(index) / sizeof(index[0]); i++)
    {
        index[i] = -1;
    }

    /*
     * convert the phy lane to a corresponding ilkn lane according to ilkn id
     */
    for (i = 0; i < num_of_lanes; i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_ilkn_lane_info_get(unit, port, type, phy_lanes[i], &ilkn_lane_info));
        ilkn_lanes_ordered[i] = ilkn_lane_info.ilkn_id;
    }

    for (i = 0; i < num_of_lanes; i++)
    {
        index[phy_lanes[i]] = ilkn_lanes_ordered[i];
    }

    /*
     * sort ilkn lanes array
     */
    _shr_sort(ilkn_lanes_ordered, num_of_lanes, sizeof(*ilkn_lanes_ordered), dnx_port_cmpfunc);
    for (i = 0; i < num_of_lanes; i++)
    {
        for (j = 0; j < DNX_DATA_MAX_NIF_PHYS_NOF_PHYS; j++)
        {
            if (index[j] == ilkn_lanes_ordered[i])
            {
                break;
            }
        }
        result_phy[i] = j;
    }

    /*
     * retrieve the lanes order from the imb
     */
    SHR_IF_ERR_EXIT(imb_port_logical_lane_order_get(unit, port, num_of_lanes, order, &actual_size));
    /*
     * retreive the lanes according to the original order
     */
    for (i = 0; i < num_of_lanes; i++)
    {
        lanes[order[i]] = result_phy[i];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_phy_control_get/set.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_phy_control_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value,
    int is_set)
{
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);
    /** Null checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "PHY control value");
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    /** verify port. supported port type is lane port or logical local port */
    if (BCM_GPORT_IS_SET(port))
    {
        if (!BCM_PHY_GPORT_IS_LANE(port) && !BCM_GPORT_IS_LOCAL_FABRIC(port))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Port: 0x%x:port type is not supported.\r\n", port);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    }

    SHR_IF_ERR_EXIT(imb_port_phy_control_validate(unit, logical_port, type, is_set));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common PHY control handle function.
 *    It can be shared by PHY control and Port contorl
 *    API.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [out] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static int
dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    /*
     * Coverity note: The verify function dnx_port_phy_control_verify can ensure the local_port here is
     * a valid port.
     */
    /*
     * coverity[overrun-call]
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get
                    (unit, gport_info.local_port, -1, gport_info.lane,
                     gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE, (soc_phy_control_t) type, value));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Common PHY control handle function.
 *    It can be shared by PHY control and Port contorl
 *    API.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    /*
     * Coverity note: The verify function dnx_port_phy_control_verify can ensure the local_port here is
     * a valid port.
     */
    /*
     * coverity[overrun-call]
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_set
                    (unit, gport_info.local_port, -1, gport_info.lane,
                     gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE, (soc_phy_control_t) type, value));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Get the phy contrl values according to
 *    phy control types
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [out] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_phy_control_verify(unit, port, type, value, 0));
    SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, type, value));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Set the phy contrl values according to
 *    phy control types
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_phy_control_verify(unit, port, type, &value, 1));
    SHR_IF_ERR_EXIT(dnx_port_phy_control_set(unit, port, type, value));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_phy_tx_set and
 *       bcm_dnx_port_phy_tx_get API.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_phy_tx_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "PHY tx parameters");

    /** verify port. supported port type is lane port or logical local port */
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        SHR_IF_ERR_EXIT(dnx_port_lane_gport_verify(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_phy_tx_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    bcm_port_resource_t resource;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_phy_tx_verify(unit, port, tx));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, gport_info.local_port, &resource));
    if (resource.link_training == 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: setting TX FIR parameters when link training is enabled is not allowed. First disable link training and then set TX FIR parameters.",
                     gport_info.local_port);
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_serdex_tx_tap_pre3_verify(unit, port, tx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PHY tx parameters, including tx taps,
 *    tx tap mode and signalling mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    int is_init_sequence, orig_enabled;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_phy_tx_set_verify(unit, port, tx));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit,
                                                    port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    is_init_sequence = !dnxc_init_is_init_done_get(unit);

    /*
     * disable port if not in init sequence. In init sequence port shouldn't be enabled yet.
     */
    if (!is_init_sequence)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, gport_info.local_port, &orig_enabled));
        if (orig_enabled)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, gport_info.local_port, 0));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_set(unit, gport_info.local_port, -1, gport_info.lane, 0, tx));

    /*
     * in init sequence port shouldn't be enabled yet.
     */
    if (!is_init_sequence && orig_enabled)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, gport_info.local_port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PHY tx parameters, including tx taps,
 *    tx tap mode and signalling mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_phy_tx_verify(unit, port, tx));

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_get(unit, gport_info.local_port, -1, gport_info.lane, 0, tx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default reassembler HW configuration
 *
 */
static shr_error_e
dnx_port_reassembler_init(
    int unit)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_reassembler))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_REASSEMBLER_CONFIG, &entry_handle_id));
        /** Setting key fields */
        /** Setting value fields */
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_AF_THRESHOLD, INST_SINGLE,
                                         dnx_data_nif.global.reassembler_fifo_threshold_get(unit));
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_AF_THRESHOLD, INST_SINGLE,
                                     dnx_data_nif.global.reassembler_fifo_threshold_get(unit));
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default reassembler HW configuration
 *
 */
static shr_error_e
dnx_port_hw_global_init(
    int unit)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_ilkn_credit_loss_handling_mode))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_GLOBAL_CONFIG, &entry_handle_id));
        /** Setting key fields */
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CREDIT_LOSS_HANDLE_MODE, INST_SINGLE, TRUE);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default HW configuration
 *
 */
static shr_error_e
dnx_port_hw_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_port_reassembler_init(unit));
    SHR_IF_ERR_EXIT(dnx_port_hw_global_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_interrupt_init(
    int unit)
{
    bcm_switch_event_control_t type;
    SHR_FUNC_INIT_VARS(unit);
    type.action = bcmSwitchEventUnmaskAndClearDisable;
    type.index = BCM_CORE_ALL;
    type.event_id = dnx_data_nif.ilkn.port_0_status_intr_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, TRUE));
    type.event_id = dnx_data_nif.ilkn.port_1_status_intr_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, TRUE));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_phy_measure_verify(
    int unit,
    bcm_port_t port,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(phy_measure, _SHR_E_PARAM, "phy_measure");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - PHY Measure for all the IMB ports.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_phy_measure_verify(unit, port, phy_measure));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
    {
        phy_measure->valid = 0;
        SHR_EXIT();
    }
    sal_memset(phy_measure, 0, sizeof(soc_dnxc_port_phy_measure_t));
    SHR_IF_ERR_EXIT(imb_port_phy_measure_get(unit, port, is_rx, phy_measure));
    phy_measure->valid = 1;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - sync the MAC status according to the PHY status.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] link - phy link status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * This API can not be called with a fabric gport
 * \see
 *   * None
 */
int
bcm_dnx_port_update(
    int unit,
    bcm_port_t port,
    int link)
{
    dnx_algo_port_info_s port_info;
    int has_speed;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** facility protection case */
    if (BCM_GPORT_IS_SYSTEM_PORT(port) || BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_failover_facility_port_status_set(unit, port, link));
    }
    else
    {
        SHR_INVOKE_VERIFY_DNXC(dnx_port_logical_verify(unit, port));
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT))
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
            if (has_speed)
            {
                SHR_IF_ERR_EXIT(imb_port_link_up_mac_update(unit, port, link));
            }
            /*
             * Recover the link config for W40 FLR upon link up event
             */
            if (link)
            {
                SHR_IF_ERR_EXIT(dnx_port_advanced_flr_link_config_recover(unit, port, -1));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_resource_speed_config_validate_verify(
    int unit,
    bcm_port_resource_t * resource,
    bcm_pbmp_t * pbmp)
{
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");
    SHR_NULL_CHECK(pbmp, _SHR_E_PARAM, "pbmp");
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, resource->port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource->port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF
        (unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, FRAMER_MGMT, FEEDBACK)))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port %d: This API is supported for NIF ports only.", resource->port);
    }
    /*
     * According to DS, 20G is not supported"
     */
    if (resource->speed == 20000)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d: NIF ports CAN'T support 20000 speed\n", resource->port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validate a port's speed configuration by fetching the affected ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] resource - port resource configuration
 * \param [out] pbmp - port bitmap of ports affected by resource
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
bcm_dnx_port_resource_speed_config_validate(
    int unit,
    bcm_port_resource_t * resource,
    bcm_pbmp_t * pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_resource_speed_config_validate_verify(unit, resource, pbmp));
    SHR_IF_ERR_EXIT(imb_common_port_speed_config_validate(unit, resource, pbmp));
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_pcs_lane_map_set.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] flags - unused currently
 * \param [in] num_map - array size for serdes_map
 * \param [in] serdes_map - lane to serdes map for Rx and Tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_pcs_lane_map_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int num_map,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int pm_nof_phys, i;
    soc_dnxc_lane_map_db_map_t *serdes2lane = NULL;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if the device supports configuring only PCS lane map
     */
    if (!dnx_data_nif.eth.feature_get(unit, dnx_data_nif_eth_pcs_lane_map_reconfig_support))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_pcs_lane_map_set is not supported on this device.\n");
    }
    /*
     * Null checks
     */
    SHR_NULL_CHECK(serdes_map, _SHR_E_PARAM, "serdes_map");
    /*
     * Verify the port is valid
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    /*
     * Verify Flags
     */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");
    /*
     * Verify the num_map, the value should be equal to
     * the lanes of the PM
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    pm_nof_phys = dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->nof_lanes_in_pm;
    SHR_VAL_VERIFY(pm_nof_phys, num_map, _SHR_E_PARAM, "Expect num_map is equal to the phys in the PM.\n");
    /*
     * Verify all the lanes are mapped correctly and there
     * is no double mapping.
     */
    SHR_ALLOC(serdes2lane, num_map * sizeof(soc_dnxc_lane_map_db_map_t),
              "Serdes to lane mapping struct", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (i = 0; i < num_map; ++i)
    {
        serdes2lane[i].rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
        serdes2lane[i].tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    }
    for (i = 0; i < num_map; ++i)
    {
        /*
         * Range checking
         */
        SHR_RANGE_VERIFY(serdes_map[i].serdes_rx_id, 0, pm_nof_phys - 1, _SHR_E_PARAM,
                         "Serdes Rx id is out of range.\n");
        SHR_RANGE_VERIFY(serdes_map[i].serdes_tx_id, 0, pm_nof_phys - 1, _SHR_E_PARAM,
                         "Serdes Tx id is out of range.\n");
        /*
         * Double mapping checking
         */
        if (serdes2lane[serdes_map[i].serdes_rx_id].rx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "lane %d: double mapping: serdes rx id %d is already mapped.\n", i,
                         serdes_map[i].serdes_rx_id);
        }
        else if (serdes2lane[serdes_map[i].serdes_tx_id].tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "lane %d: double mapping: serdes tx id %d is already mapped.\n", i,
                         serdes_map[i].serdes_tx_id);
        }
        serdes2lane[serdes_map[i].serdes_rx_id].rx_id = i;
        serdes2lane[serdes_map[i].serdes_tx_id].tx_id = i;
    }
exit:
    SHR_FREE(serdes2lane);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the PCS lane map without touching
 *    PMD lane map. This API configures the PCS lane map
 *    for entire PM.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] flags - unused currently
 * \param [in] num_map - array size for serdes_map
 * \param [in] serdes_map - lane to serdes map for Rx and Tx
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
bcm_dnx_port_pcs_lane_map_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int num_map,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int i;
    uint32 tx_lane_swap = 0, rx_lane_swap = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_pcs_lane_map_verify(unit, port, flags, num_map, serdes_map));
    for (i = 0; i < num_map; ++i)
    {
        tx_lane_swap |= ((serdes_map[i].serdes_tx_id & 0xf) << (i * 4));
        rx_lane_swap |= ((serdes_map[i].serdes_rx_id & 0xf) << (i * 4));
    }

    SHR_IF_ERR_EXIT(imb_port_pcs_lane_map_set(unit, port, tx_lane_swap, rx_lane_swap));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - ILKN RX link status change interrupt handler
 *
 * \param [in] unit - Relevant unit
 * \param [in] block_instance - ILKN block instance.
 * \param [in] port_in_core - Internal port index in core (0,1)
 */
shr_error_e
dnx_port_ilkn_rx_status_change_interrupt_handler(
    int unit,
    int block_instance,
    uint8 *port_in_core)
{
    int interface_offset;
    bcm_gport_t gport;
    bcm_port_t local_port;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Calculate ILKN offset
     */
    interface_offset = (block_instance * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)) + *port_in_core;
    /*
     * Get ILKN logical port
     */
    rv = dnx_algo_port_interface_offset_logical_get(unit, DNX_ALGO_PORT_TYPE_NIF_ILKN, interface_offset, &gport);
    if (rv == _SHR_E_NOT_FOUND)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Logical port not found\n")));
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /*
     * Call to link status event function
     */
    local_port = BCM_GPORT_LOCAL_GET(gport);
    SHR_IF_ERR_EXIT(imb_port_link_status_change_event(unit, local_port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - ILKN RX lane lock status change interrupt handler
 *
 * \param [in] unit - Relevant unit
 * \param [in] cdu_id - CDU instance index
 * \param [in] lane_in_cdu - index of the lane in cdu
 */
shr_error_e
dnx_port_ilkn_rx_lane_lock_status_change_interrupt_handler(
    int unit,
    int cdu_id,
    uint8 *lane_in_cdu)
{
    int lane;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get lane ID
     */
    lane = dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit) * cdu_id + *lane_in_cdu;

    /*
     * Get logical port based on logical lane
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, lane, 0, 0, &logical_port));

    /*
     * If this is an ILKN PORT as expected - call to lock status change event function, otherwise raise an error
     */
    SHR_IF_ERR_EXIT(imb_port_pm_lock_status_change_event(unit, logical_port));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_post_ber_proj_verify(
    int unit,
    bcm_port_t port,
    bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    SHR_FUNC_INIT_VARS(unit);
    if (BCM_GPORT_IS_SET(port) &&
        !(BCM_PHY_GPORT_IS_LANE(port) || BCM_GPORT_IS_LOCAL_FABRIC(port) || BCM_GPORT_IS_LOCAL(port)))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM,
                          "port should be either logical port or either lane gport or fabric gport%s%s%s", "", "", "");
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_post_ber_proj_verify
                           (unit, port, ber_proj, max_errcnt, errcnt_array, actual_errcnt));
    /*
     * retrieve local port
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    /*
     * call api
     */
    SHR_IF_ERR_EXIT(imb_port_post_ber_proj_get
                    (unit, gport_info.local_port, gport_info.lane, (const bcm_port_ber_proj_params_t *) ber_proj,
                     max_errcnt, errcnt_array, actual_errcnt));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the value for bcm_dnx_port_fast_link_recovery_enable_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] enable - Enable or disable FLR
 * \param [in] timeout - FLR timeout value
 * \param [in] is_set - if this API is called by set API
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_fast_link_recovery_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable,
    bcm_port_flr_timeout_t * timeout,
    int is_set)
{
    int is_flr_fw_support;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    SHR_NULL_CHECK(timeout, _SHR_E_PARAM, "timeout");
    /*
     * Verify the port is valid
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    /*
     * Check if FLR Firmware is supported
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
    if (!is_flr_fw_support)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "FLR firmware is not supported, so this API is irrelevant.\r\n");
    }
    /*
     * Verify port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
        && !DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) && !DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "FLR is only supported on ETH, OTN and FlexE physical interface.\r\n");
    }
    if (is_set)
    {
        /*
         * Verify values
         */
        SHR_BOOL_VERIFY(*enable, _SHR_E_PARAM, "Enable value should be 0 or 1.\r\n");
        SHR_RANGE_VERIFY(*timeout, 0, bcmPortFlrTimeoutCount - 1, _SHR_E_PARAM, "Timeout value is incorrect.\r\n");
        if ((*enable == 1) && (*timeout == bcmPortFlrTimeoutInvaild))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please provide correct timeout values.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable Fast Link recovery on the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] enable - Enable or disable FLR
 * \param [in] timeout - FLR timeout value
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
bcm_dnx_port_fast_link_recovery_enable_set(
    int unit,
    bcm_port_t port,
    int enable,
    bcm_port_flr_timeout_t timeout)
{
    bcm_port_flr_link_config_t link_config;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fast_link_recovery_enable_verify(unit, port, &enable, &timeout, 1));
    /*
     * call api
     */
    SHR_IF_ERR_EXIT(portmod_port_flr_config_set(unit, port, enable, (portmod_flr_timeout_t) timeout));
    /*
     * call linkscan flr api
     */
    SHR_IF_ERR_EXIT(bcm_dnx_linkscan_flr_enable_set(unit, port, enable, timeout));
    /*
     * If advanced FLR is supported, ony use CDR lock to monitor the FLR status,
     * which acts as legacy FLR mode.
     * If OTN is not supported, advanced FLR is not supported.
     */
    if (dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
    {
        sal_memset(&link_config, 0, sizeof(bcm_port_flr_link_config_t));
        /*
         * Keep CDR lock down signal unmasked
         */
        link_config.link_mask = (BCM_PORT_FLR_LOS_DOWN_MASK | BCM_PORT_FLR_CDR_OOL_DOWN_MASK);
        if (!enable)
        {
            /*
             * For disable, mask all the down signals
             */
            link_config.link_mask |= BCM_PORT_FLR_CDR_LOCK_DOWN_MASK;
        }
        SHR_IF_ERR_EXIT(imb_port_flr_link_config_set(unit, port, -1, &link_config));
        /*
         * Configure los_ext_en to enable/disable the los signal from external
         */
        SHR_IF_ERR_EXIT(portmod_port_external_los_enable_set(unit, port, enable));
        /*
         * Configure link mask into DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_link_config_set(unit, port, link_config));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get Fast Link recovery status on the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [out] enable - Enable or disable FLR
 * \param [out] timeout - FLR timeout value
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
bcm_dnx_port_fast_link_recovery_enable_get(
    int unit,
    bcm_port_t port,
    int *enable,
    bcm_port_flr_timeout_t * timeout)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_port_fast_link_recovery_enable_verify(unit, port, enable, timeout, 0));
    /*
     * call api
     */
    SHR_IF_ERR_EXIT(portmod_port_flr_config_get(unit, port, enable, (portmod_flr_timeout_t *) timeout));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See port.h for reference
 */
shr_error_e
dnx_portmod_thread_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (!portmod_was_created(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(portmod_thread_stop(unit));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the value for bcm_dnx_port_fast_link_recovery_link_config_get/set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] flags - flags. Not used.
 * \param [in] config - FLR link mask and timer
 * \param [in] is_set - This API is called by set or get API
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_fast_link_recovery_link_config_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flr_link_config_t * config,
    int is_set)
{
    int is_flr_fw_support;
    dnx_algo_port_info_s port_info;
    uint32 supported_masks, max_timer;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "FLR link config");
    /*
     * Verify if this API is supported
     */
    if (!dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable on this device.\r\n");
    }
    /*
     * Verify the port is valid
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    /*
     * Check if FLR Firmware is supported
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
    if (!is_flr_fw_support)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "FLR firmware is not supported, so this API is irrelevant.\r\n");
    }
    /*
     * Verify port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
        && !DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) && !DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "FLR is only supported on ETH ports, FlexE and OTN physical interface.\r\n");
    }
    /*
     * Verify Flags
     */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    if (is_set)
    {
        /*
         * Verify values
         */
        supported_masks = BCM_PORT_FLR_LOS_DOWN_MASK | BCM_PORT_FLR_CDR_LOCK_DOWN_MASK | BCM_PORT_FLR_CDR_OOL_DOWN_MASK;
        SHR_MASK_VERIFY(config->link_mask, supported_masks, _SHR_E_PARAM, "unexpected masks.\n");

        max_timer =
            (dnx_data_nif.flr.max_tick_unit_get(unit) * dnx_data_nif.flr.max_tick_count_get(unit) *
             dnx_data_nif.flr.timer_granularity_get(unit)) / 1000;
        SHR_MAX_VERIFY(config->link_stable_timer, max_timer, _SHR_E_PARAM,
                       "The configured timer exceeds the MAX timer value.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the FLR link mask and
 *    link stable timer
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] flags - flags. Not used.
 * \param [out] config - FLR link mask and timer
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
bcm_dnx_port_fast_link_recovery_link_config_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flr_link_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_fast_link_recovery_link_config_verify(unit, port, flags, config, 0));
    /*
     * Get the link mask and link stable timer from DB as the original configuration might be
     * modified in HW by interrupt handler.
     */
    sal_memset(config, 0, sizeof(bcm_port_flr_link_config_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_link_config_get(unit, port, config));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set the FLR link mask and
 *    link stable timer
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] flags - flags. Not used.
 * \param [in] config - FLR link mask and timer
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
bcm_dnx_port_fast_link_recovery_link_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flr_link_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_fast_link_recovery_link_config_verify(unit, port, flags, config, 1));
    SHR_IF_ERR_EXIT(imb_port_flr_link_config_set(unit, port, -1, config));
    /*
     * Configure link config into DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_link_config_set(unit, port, *config));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Recover the original link config for
 *    FLR link up and non-FLR link up
 *
 * \param [in] unit - Relevant unit
 * \param [in] logical_port - Logical port ID.
 * \param [in] lane_id - logical lane ID.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_advanced_flr_link_config_recover(
    int unit,
    bcm_port_t logical_port,
    int lane_id)
{
    uint32 all_mask;
    dnx_algo_port_info_s port_info;
    bcm_port_flr_link_config_t link_config;
    int is_flr_fw_support, is_advanced_flr_enabled;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    /*
     * Only relevant for ETH, FlexE phy and OTN ports
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        /*
         * Check if FLR firmware is supported
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, logical_port, &is_flr_fw_support));
        is_advanced_flr_enabled = (is_flr_fw_support
                                   && dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported));
        if (is_advanced_flr_enabled)
        {
            /*
             * Get original link mask configuration
             */
            all_mask = (BCM_PORT_FLR_CDR_LOCK_DOWN_MASK | BCM_PORT_FLR_LOS_DOWN_MASK | BCM_PORT_FLR_CDR_OOL_DOWN_MASK);
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_link_config_get(unit, logical_port, &link_config));
            if ((link_config.link_mask & all_mask) != all_mask)
            {
                /*
                 * Restore the orignal link configuration
                 */
                SHR_IF_ERR_EXIT(imb_port_flr_link_config_set(unit, logical_port, lane_id, &link_config));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - W40 FLR interrupt handler
 *
 * \param [in] unit - Relevant unit
 * \param [in] block_instance - W40 block instance.
 * \param [in] phy_lane - The physical lane ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_w40_flr_interrupt_handler(
    int unit,
    int block_instance,
    int *phy_lane)
{
    int map_size;
    uint32 all_mask;
    bcm_port_t logical_port;
    bcm_port_flr_link_config_t link_config;
    soc_dnxc_lane_map_db_map_t serdes2lane[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get lane map, from physical lane to logical lane
     */
    map_size = dnx_data_nif.phys.nof_phys_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_serdes_to_lane_map_get(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, map_size, serdes2lane));
    /*
     * Get logical port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, serdes2lane[*phy_lane].rx_id, 0, 0, &logical_port));
    /*
     * Get original link mask configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_link_config_get(unit, logical_port, &link_config));
    /*
     * Need to perform corrective action if one of the signals
     * are unmasked
     */
    all_mask = (BCM_PORT_FLR_CDR_LOCK_DOWN_MASK | BCM_PORT_FLR_LOS_DOWN_MASK | BCM_PORT_FLR_CDR_OOL_DOWN_MASK);
    if ((link_config.link_mask & all_mask) != all_mask)
    {
        /*
         * Mask all the sources, the mask configuration will
         * be recovered in FLR link up or non-FLR link up
         */
        link_config.link_mask |= all_mask;
        SHR_IF_ERR_EXIT(imb_port_flr_link_config_set(unit, logical_port, serdes2lane[*phy_lane].rx_id, &link_config));
    }
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
