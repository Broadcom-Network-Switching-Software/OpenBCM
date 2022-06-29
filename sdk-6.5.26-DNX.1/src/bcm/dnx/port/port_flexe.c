/** \file port_flexe.c
 * 
 *
 * FlexE Port procedures for DNX.
 *
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
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

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
 * TYPEDEF
 * {
 */
/*
 * }
 */
/*
 * DEFINES
 * {
 */
#define DNX_FLEXE_GROUP_ALARMS        (FLEXE_CORE_OH_ALARM_PHY_MAP_MISMATCH | \
                                       FLEXE_CORE_OH_ALARM_GROUP_DESKEW)
#define DNX_FLEXE_PHY_ALARMS          (FLEXE_CORE_OH_ALARM_GID_MISMATCH     | \
                                       FLEXE_CORE_OH_ALARM_PHY_NUM_MISMATCH | \
                                       FLEXE_CORE_OH_ALARM_LOF              | \
                                       FLEXE_CORE_OH_ALARM_LOM              | \
                                       FLEXE_CORE_OH_ALARM_RPF              | \
                                       FLEXE_CORE_OH_ALARM_OH1_ALARM        | \
                                       FLEXE_CORE_OH_ALARM_C_BIT_ALARM      | \
                                       FLEXE_CORE_OH_ALARM_CAL_A_MISMATCH   | \
                                       FLEXE_CORE_OH_ALARM_CAL_B_MISMATCH   | \
                                       FLEXE_CORE_OH_ALARM_CRC              | \
                                       FLEXE_CORE_OH_ALARM_SC_MISMATCH      | \
                                       FLEXE_CORE_OH_ALARM_GID_UNEQUIPPED)

/*
 * }
 */

/**
 * \brief - Configure the RMC logical fifos for
 *    resource_set API. This API will be called for
 *    the following scenarios:
 *    1. Change the FlexE client speed.
 *    2. Configure the client speed for the first time.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - Rx or Tx
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
dnx_port_flexe_logcial_fifo_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int i;
    dnx_algo_port_info_s port_info;
    bcm_port_prio_config_t priority_config;

    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FLEXE_DIRECTION_TX_GET(flags) && !IMB_FLEXE_DIRECTION_RX_GET(flags))
    {
        /*
         * If the resource is only for TX, dont need to configure
         */
        SHR_EXIT();
    }
    /*
     * Get the original priority configuration. If it is the first time
     * to allocate the RMC fifos, set default priority for it.
     */
    SHR_IF_ERR_EXIT(imb_port_priority_config_get(unit, port, &priority_config));
    /*
     * If it is the first time to allocate RMC fifo, set default value for it.
     */
    if (priority_config.nof_priority_groups == 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit, port_info))
        {
            /*
             * If FlexE Client is MAC L1 all traffic for this client should go to RMC scheduled for TDM
             */
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerTDM;
        }
        else
        {
            priority_config.priority_groups[0].sch_priority = bcmPortNifSchedulerLow;
        }
        priority_config.priority_groups[0].source_priority = IMB_PRD_PRIORITY_ALL;
        priority_config.nof_priority_groups = 1;
    }
    else
    {
        /*
         * imb_port_priority_config_get will return the actual nof_entries
         * which each RMC is using. However the nof_entries is not configurable,
         * need to clear these values.
         */
        for (i = 0; i < priority_config.nof_priority_groups; ++i)
        {
            priority_config.priority_groups[i].num_of_entries = 0;
        }
    }
    /*
     * Configure the priority
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_priority_config_set(unit, port, &priority_config));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the arguments for dnx_port_flexe_calendar_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
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
dnx_port_flexe_calendar_set_verify(
    int unit,
    bcm_port_t port)
{
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flexe feature is not available on the device.\r\n");
    }

    if (dnx_data_nif.flexe.nb_tdm_slot_allocation_mode_get(unit) != DNX_FLEXE_ALLOC_MODE_DYNAMIC)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Modify the TDM slots dynamically is not supported on the device.\r\n");
    }
    /*
     * Verify the port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_verify(unit, port, 1));
    /*
     * Verify the port speed and FEC
     */
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));
    if (!
        ((speed_config.num_lane == 2) && (speed_config.speed == 50000)
         && (speed_config.fec != PORTMOD_PORT_PHY_FEC_NONE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Modifying TDM slots is only support for 50G NRZ port with FEC enabled.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE calendar dynamically
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - flags for FlexE APIs
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
dnx_port_flexe_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_calendar_set_verify(unit, port));
    SHR_IF_ERR_EXIT(imb_calendar_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure nof slots in FlexE core for MAC and SAR
 *    settings
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - Rx or Tx flag
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
dnx_port_flexe_core_speed_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int client_channel, has_rx_speed = 0, has_tx_speed = 0;
    int rx_speed = 0, tx_speed = 0, mgmt_channel = -1;
    dnx_algo_port_info_s port_info;
    flexe_core_rate_adpt_info_t rate_adpt_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_framer_mgmt_channel_get(unit, port, &mgmt_channel));
    }

    if (IMB_FLEXE_DIRECTION_RX_GET(flags))
    {
        /*
         * Get rx nof slots
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        if (has_rx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
        }
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            /*
             * Update Rx nof slots
             */
            if (!has_rx_speed)
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_delete
                                (unit, client_channel, mgmt_channel, BCM_PORT_FLEXE_RX, rx_speed));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_add
                                (unit, client_channel, mgmt_channel, BCM_PORT_FLEXE_RX, rx_speed));
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
        {
            /*
             * Update Rx nof slots
             */
            if (!has_rx_speed)
            {
                SHR_IF_ERR_EXIT(flexe_core_sar_client_delete(unit, client_channel, BCM_PORT_FLEXE_RX, rx_speed));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_sar_client_add(unit, client_channel, BCM_PORT_FLEXE_RX, rx_speed));
            }
        }
    }

    if (IMB_FLEXE_DIRECTION_TX_GET(flags))
    {
        /*
         * Get rx nof slots
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        if (has_tx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
        }
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
        {
            /*
             * For MAC, only need to configure the tx slots
             */
            if (!has_tx_speed)
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_delete
                                (unit, client_channel, mgmt_channel, BCM_PORT_FLEXE_TX, tx_speed));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_mac_client_add
                                (unit, client_channel, mgmt_channel, BCM_PORT_FLEXE_TX, tx_speed));
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
        {
            /*
             * Update Tx nof slots
             */
            if (!has_tx_speed)
            {
                SHR_IF_ERR_EXIT(flexe_core_sar_client_delete(unit, client_channel, BCM_PORT_FLEXE_TX, tx_speed));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_sar_client_add(unit, client_channel, BCM_PORT_FLEXE_TX, tx_speed));
            }
        }
        else
        {
            /*
             * Configure rate adpt for L1 ETH ports and FlexE unaware
             */
            flexe_core_rate_adpt_info_t_init(unit, &rate_adpt_info);
            rate_adpt_info.speed = tx_speed;

            if (!has_tx_speed)
            {
                SHR_IF_ERR_EXIT(flexe_core_rateadpt_delete(unit, client_channel, &rate_adpt_info));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_rateadpt_add(unit, client_channel, &rate_adpt_info));
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure speed in TMAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - Rx or Tx flag
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_tmac_speed_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int speed, has_speed;
    portmod_speed_config_t port_speed_config;

    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FLEXE_DIRECTION_TX_GET(flags))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
            port_speed_config.speed = speed;
            SHR_IF_ERR_EXIT(imb_port_speed_config_set(unit, port, &port_speed_config));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - G.hao for FlexE 66b switching
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] speed - port speed
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
dnx_port_flexe_66b_switch_ghao_config_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int nof_dests;
    dnx_algo_port_info_s port_info;
    flexe_core_66bswitch_channel_info_t src_info;
    flexe_core_66bswitch_channel_info_t dest_info[DNX_DATA_MAX_NIF_FRAMER_B66SWITCH_BACKUP_CHANNELS + 1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, port_info.port_type, 0, &src_info.block_id));
    /*
     * Get channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &src_info.channel));
    /*
     * Get flow
     */
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_reverse_get(unit, &src_info, dest_info, &nof_dests));
    if (nof_dests > 0)
    {
         /* coverity[swapped_arguments : FALSE]  */
        SHR_IF_ERR_EXIT(flexe_core_66b_switch_set(unit, dest_info, &src_info, 0, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Prepare the fifo resources
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - flags, Rx or Tx
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
dnx_port_flexe_fifo_resource_prepare(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 tmp_flags = flags;

    SHR_FUNC_INIT_VARS(unit);

    IMB_FLEXE_RESOURCE_PREPARE_SET(tmp_flags);
    SHR_IF_ERR_EXIT(imb_port_fifo_resource_set(unit, port, tmp_flags));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Switch the fifo resources
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - flags, Rx or Tx
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
dnx_port_flexe_fifo_resource_switch(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 tmp_flags = flags;

    SHR_FUNC_INIT_VARS(unit);

    IMB_FLEXE_RESOURCE_SWITCH_SET(tmp_flags);
    SHR_IF_ERR_EXIT(imb_port_fifo_resource_set(unit, port, tmp_flags));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Prepare the shadow calendar
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - flags, Rx or Tx
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
dnx_port_flexe_calendar_prepare(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 tmp_flags = flags;

    SHR_FUNC_INIT_VARS(unit);

    IMB_FLEXE_RESOURCE_PREPARE_SET(tmp_flags);
    SHR_IF_ERR_EXIT(imb_calendar_set(unit, port, tmp_flags));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Switch the calendar
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - flags, Rx or Tx
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
dnx_port_flexe_calendar_switch(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    uint32 tmp_flags = flags;

    SHR_FUNC_INIT_VARS(unit);

    IMB_FLEXE_RESOURCE_SWITCH_SET(tmp_flags);
    SHR_IF_ERR_EXIT(imb_calendar_set(unit, port, tmp_flags));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FlexE client configurations, which
 *    are required to configure after speed is ready
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port id
 * \param [in] flags - Rx/Tx flags
 * \param [out] cal_info - FlexE calendar info
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
dnx_port_flexe_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    dnx_flexe_cal_info_t * cal_info)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * Flexewp configuration for each FlexE phy port and ETH L1 Framer port
     */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        /*
         * Update calendar if needed
         */
        if (!cal_info->flexe_phy_cal_is_updated)
        {
            SHR_IF_ERR_EXIT(imb_calendar_set(unit, port, flags));
            cal_info->flexe_phy_cal_is_updated = TRUE;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        /*
         * Configure TinyMac in IMB
         */
        SHR_IF_ERR_EXIT(dnx_port_flexe_tmac_speed_config_set(unit, port, flags));
        /*
         * Configure FIFO resources in IMB
         */
        SHR_IF_ERR_EXIT(imb_port_fifo_resource_set(unit, port, flags));
        /*
         * Adjust Rx logical fifos in FEU, otherwise the RMC priority
         * and RMC FIFOs are configured in common OFR module.
         */
        if ((dnx_data_nif.flexe.feu_nof_get(unit) > 0) && !DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_PROTECTION(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_logcial_fifo_set(unit, port, flags));
        }
        /*
         * Update calendar if needed
         */
        if (!cal_info->mac_client_cal_is_updated)
        {
            SHR_IF_ERR_EXIT(imb_calendar_set(unit, port, flags));
            cal_info->mac_client_cal_is_updated = TRUE;
        }
    }
    /*
     * Configure nof slots in FlexE core
     */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE)
        || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE)
        || DNX_ALGO_PORT_TYPE_IS_FLEXE_UNAWARE(unit, port_info))
    {
        /*
         * Configure FlexE core phy default settings
         */
        SHR_IF_ERR_EXIT(dnx_port_flexe_core_speed_config_set(unit, port, flags));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_flexe_group_create API
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] nof_pcs - Number of PCS in the group
 * \param [in] phy_info - The FlexE group phy info
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
dnx_port_flexe_group_create_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int nof_pcs,
    bcm_port_flexe_group_phy_info_t * phy_info)
{
    int i = 0, speed, tmp_speed, group_index;
    bcm_pbmp_t logical_phy_id_bmp, phy_ports;
    uint32 supported_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(phy_info, _SHR_E_PARAM, "phy_info");

    /** Check port type */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));

    /** Verify flags */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_eth_use_flexe_group))
    {
        supported_flags |= BCM_PORT_FLEXE_BYPASS;
    }
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "Unsupported flags.\r\n");

    /** For create API, the FlexE group should be invalid */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_free_verify(unit, group_index));

    /** Verify nof pcs */
    if (flags & BCM_PORT_FLEXE_BYPASS)
    {
        SHR_VAL_VERIFY(nof_pcs, 1, _SHR_E_PARAM, "nof_pcs should 1 for bypass mode.\n");
    }
    else
    {
        SHR_RANGE_VERIFY(nof_pcs, 1, dnx_data_nif.flexe.nof_pcs_get(unit), _SHR_E_PARAM, "nof_pcs is out of range.\n");
    }
    /*
     * Verify the slot mode
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_flexe_slot_mode_support))
    {
        SHR_RANGE_VERIFY(phy_info[0].slot_mode, 0, bcmPortFlexePhySlot1G, _SHR_E_PARAM, "Invalid slot mode.\r\n");
        if (((phy_info[0].slot_mode == bcmPortFlexePhySlot1G)
             && (dnx_data_nif.flexe.device_slot_mode_get(unit) == DNX_FLEXE_SLOT_MODE_1P25G_AND_5G))
            || ((phy_info[0].slot_mode == bcmPortFlexePhySlot1P25G)
                && (dnx_data_nif.flexe.device_slot_mode_get(unit) == DNX_FLEXE_SLOT_MODE_1G_AND_5G)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE phy slot mode is not supported on the device.\r\n");
        }
    }
    else
    {
        SHR_RANGE_VERIFY(phy_info[0].slot_mode, 0, bcmPortFlexePhySlot5G, _SHR_E_PARAM, "Invalid slot mode.\r\n");
    }

    /** Verify the speed */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, phy_info[0].port, 0, &speed));

    /** Verify phy_info array value */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_phy_ports_get(unit, &phy_ports));
    BCM_PBMP_CLEAR(logical_phy_id_bmp);
    for (i = 0; i < nof_pcs; i++)
    {
        /** Verify FlexE physical port is valid */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_verify(unit, phy_info[i].port, 1));

        /*
         * Verify all the physical port speed and slot mode are the same if
         * there are more than 1 members
         */
        if (i > 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, phy_info[i].port, 0, &tmp_speed));
            SHR_VAL_VERIFY(speed, tmp_speed, _SHR_E_PARAM,
                           "The speeds of all the FlexE physical ports should be the same.\n");

            SHR_VAL_VERIFY(phy_info[i].slot_mode, phy_info[0].slot_mode, _SHR_E_PARAM,
                           "The slot modes of all the FlexE physical ports should be the same.\n");
        }
        /*
         * Make sure the PHY ports only belong to one group
         */
        if (BCM_PBMP_MEMBER(phy_ports, phy_info[i].port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The PHY port can only belong to one group.\r\n");
        }
        if (flags & BCM_PORT_FLEXE_BYPASS)
        {
            /*
             * For bypass mode, the logical PHY ID is irrelevant.
             */
            SHR_VAL_VERIFY(phy_info[i].logical_phy_id, 0, _SHR_E_PARAM,
                           "The logical PHY ID should be 0 for bypass mode.\n");
        }
        else
        {
            /*
             * Verify FlexE physical port logical PHY ID
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_and_speed_verify
                            (unit, speed, phy_info[i].logical_phy_id));

            /*
             * The logical PHY ID cannot be the same
             */
            if (BCM_PBMP_MEMBER(logical_phy_id_bmp, phy_info[i].logical_phy_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The logical PHY ID cannot be the same.\r\n");
            }
            BCM_PBMP_PORT_ADD(logical_phy_id_bmp, phy_info[i].logical_phy_id);
        }
    }
    /*
     * Verify FlexE group speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_verify(unit, phy_info[0].slot_mode, nof_pcs * speed));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flexe_core_port_info_get(
    int unit,
    bcm_port_t logical_port,
    int is_bypass,
    flexe_core_port_info_t * port_info)
{
    int lane_speed, speed, nof_phys;
    bcm_port_phy_fec_t fec_type;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the lane speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &nof_phys));
    lane_speed = speed / nof_phys;
    /*
     * Get serdes rate mode
     */
    sal_memset(port_info, 0, sizeof(flexe_core_port_info_t));
    switch (lane_speed)
    {
        case 50000:
            port_info->serdes_rate_mode = flexe_core_serdes_rate_mode_53G;
            break;
        case 25000:
            SHR_IF_ERR_EXIT(dnx_algo_port_fec_type_get(unit, logical_port, &fec_type));
            if (fec_type == bcmPortPhyFecRs544)
            {
                port_info->serdes_rate_mode = flexe_core_serdes_rate_mode_26G;
            }
            else
            {
                port_info->serdes_rate_mode = flexe_core_serdes_rate_mode_25G;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "lane speed %d is not supported!\n", lane_speed);
    }
    port_info->speed = speed;
    port_info->is_bypass = is_bypass;

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Create FlexE group with FlexE physical ports
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] nof_pcs - Number of PCS in the group
 * \param [in] phy_info - The FlexE group phy info
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
bcm_dnx_port_flexe_group_create(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int nof_pcs,
    bcm_port_flexe_group_phy_info_t * phy_info)
{
    int i, group_index, is_bypass;
    bcm_pbmp_t phy_ports, flexe_core_port_bmp;
    uint32 group_create_flags = 0;
    int logical_phy_id[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS] = { 0 };
    int flexe_core_port;
    flexe_core_port_info_t port_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_create_verify(unit, gport, flags, nof_pcs, phy_info));

    /** Mark the FlexE group is valid */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_set(unit, group_index, 1));
    /*
     * Mark if the group is bypassed
     */
    is_bypass = (flags & BCM_PORT_FLEXE_BYPASS) ? 1 : 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_is_bypass_set(unit, group_index, is_bypass));
    /*
     * Configure flexe group slot mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_set(unit, group_index, phy_info[0].slot_mode));

    BCM_PBMP_CLEAR(phy_ports);
    BCM_PBMP_CLEAR(flexe_core_port_bmp);
    /*
     * Update SW DB for logical PHY ID and flexe physical ports
     */
    for (i = 0; i < nof_pcs; ++i)
    {
        BCM_PBMP_PORT_ADD(phy_ports, phy_info[i].port);
        /*
         * Store the FlexE logical PHY ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_set(unit, phy_info[i].port, phy_info[i].logical_phy_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_set(unit, group_index, &phy_ports));
    /*
     * Prepare date for FlexE core API
     */
    for (i = (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_half_capability) ? nof_pcs / 2 : 0);
         i < nof_pcs; ++i)
    {
        /*
         * Get the FlexE core port ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_info[i].port, &flexe_core_port));
        BCM_PBMP_PORT_ADD(flexe_core_port_bmp, flexe_core_port);
        logical_phy_id[flexe_core_port] = phy_info[i].logical_phy_id;
    }
    /*
     * Get the FlexE core port info, since all the FlexE core port info are the same,
     * we only get the port_info from the first flexe physical port.
     */
    SHR_IF_ERR_EXIT(dnx_flexe_core_port_info_get(unit, phy_info[0].port, is_bypass, &port_info));
    /*
     * Configure the FlexE core, we always create it for RX and Tx
     */
    group_create_flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(flexe_core_group_phy_add
                    (unit, group_create_flags, group_index, flexe_core_port_bmp, logical_phy_id, &port_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_flexe_group_get API.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] phy_info - The FlexE group phy info
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
dnx_port_flexe_group_get_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_phy_info_t * phy_info)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);
    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }

    /** Null checks */
    SHR_NULL_CHECK(phy_info, _SHR_E_PARAM, "phy_info");

    /** Check port type */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** For get API, the FlexE group should be valid */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create FlexE group with FlexE physical ports
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] max_nof_pcs - Array size for phy_info
 * \param [out] phy_info - The FlexE group phy info
 * \param [out] actual_nof_pcs - The actual phy number
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
bcm_dnx_port_flexe_group_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_nof_pcs,
    bcm_port_flexe_group_phy_info_t * phy_info,
    int *actual_nof_pcs)
{
    int group_index;
    bcm_pbmp_t phy_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_get_verify(unit, gport, flags, phy_info));

    /** Get the FlexE phy ports into the SW DB */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));

    *actual_nof_pcs = 0;
    BCM_PBMP_ITER(phy_ports, logical_port)
    {
        /*
         * Return error if there is no enough space for all the flexe phys
         */
        if (*actual_nof_pcs >= max_nof_pcs)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The size of phy_info array is too small to store all the flexe phys.\r\n");
        }
        phy_info[*actual_nof_pcs].port = logical_port;
        /** Get the FlexE logical PHY ID */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get
                        (unit, logical_port, &phy_info[*actual_nof_pcs].logical_phy_id));
        /** Get slot mode */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &phy_info[*actual_nof_pcs].slot_mode));

        (*actual_nof_pcs)++;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_flexe_group_destroy API.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
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
dnx_port_flexe_group_destroy_verify(
    int unit,
    bcm_gport_t gport)
{
    int group_index, is_occupied;
    bcm_pbmp_t phy_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /*
     * Check port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
    /*
     * For destroy API, the FlexE group should be valid
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 1));
    /*
     * Check if G.hao is in progress
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX));
    /*
     * Check if there is active slots in this group
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));
    _SHR_PBMP_ITER(phy_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_phy_port_is_occupied_get(unit, group_index, logical_port, &is_occupied));
        if (is_occupied)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "there is active time slots in the group.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy the FlexE group. All the FlexE clients in the group should
 *    be cleared before calling this API.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
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
bcm_dnx_port_flexe_group_destroy(
    int unit,
    bcm_gport_t gport)
{
    int group_index, flexe_core_port;
    uint32 flags;
    bcm_port_t logical_port;
    bcm_pbmp_t phy_ports, flexe_core_port_bmp;
    int nof_pcs, count = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_destroy_verify(unit, gport));

    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    /*
     * Clear the logical PHY ID in DB
     */
    BCM_PBMP_CLEAR(flexe_core_port_bmp);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));

    _SHR_PBMP_COUNT(phy_ports, nof_pcs);
    _SHR_PBMP_ITER(phy_ports, logical_port)
    {
        /*
         * For half capacity, need to skip the first half phy ports.
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_half_capability) && (count < (nof_pcs / 2)))
        {
            count++;
            continue;
        }
        /*
         * Get FlexE core port
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));
        BCM_PBMP_PORT_ADD(flexe_core_port_bmp, flexe_core_port);
    }
    /*
     * Configure the FlexE core, we always destroy it for Rx and Tx
     */
    if (!BCM_PBMP_IS_NULL(flexe_core_port_bmp))
    {
        flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX;
        SHR_IF_ERR_EXIT(flexe_core_group_phy_remove(unit, flags, group_index, flexe_core_port_bmp));
    }
    _SHR_PBMP_ITER(phy_ports, logical_port)
    {
        /*
         * Clear logical PHY ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_set(unit, logical_port, -1));
    }
    /*
     * Clear the FlexE group DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_info_db_init(unit, group_index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_flexe_group_phy_add/remove API.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] phy_info - The FlexE group phy info
 * \param [in] is_add - If this API is called by ADD API.
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
dnx_port_flexe_group_modify_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_phy_info_t * phy_info,
    int is_add)
{
    int group_index, speed, speed_to_check;
    int is_occupied, nof_ports, max_logical_phy_id;
    uint8 is_unaligned;
    bcm_pbmp_t group_phy_ports, phy_ports;
    int phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    bcm_port_t logical_port;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }

    /** Null checks */
    SHR_NULL_CHECK(phy_info, _SHR_E_PARAM, "phy_info");

    /** Check port type */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** For modify API, the FlexE group should be valid */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));

    /** Check if G.hao is in progress */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX));

    /** Check if the group is empty */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &group_phy_ports));
    if (_SHR_PBMP_IS_NULL(group_phy_ports))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Empty group! Not allowed to add port to or remove port from it!\r\n");
    }

    /** Verify FlexE physical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_verify(unit, phy_info->port, 0));

    if (is_add)
    {
        /** The port speed should be same as the other members */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_speed_get(unit, group_index, &speed));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, phy_info->port, 0, &speed_to_check));
        SHR_VAL_VERIFY(speed, speed_to_check, _SHR_E_PARAM, "All the physical port speed should be the same.\n");
        /*
         * Verify logical PHY ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_and_speed_verify(unit, speed, phy_info->logical_phy_id));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_phy_ports_get(unit, &phy_ports));
        if (_SHR_PBMP_MEMBER(phy_ports, phy_info->port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The PHY port can only belong to one group.\r\n");
        }
        /** Verify the group speed */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_verify(unit, phy_info->slot_mode, speed));

        SHR_IF_ERR_EXIT(dnx_algo_flexe_cal_slots_is_unaligned_get(unit, group_index, &is_unaligned));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));

        SHR_RANGE_VERIFY(nof_ports, 1, dnx_data_nif.flexe.nof_pcs_get(unit),
                         _SHR_E_PORT, "nof_ports is out of range.\n");
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, phy_ports_arr[nof_ports - 1], &max_logical_phy_id));
        if (is_unaligned || (phy_info->logical_phy_id < max_logical_phy_id))
        {
            _SHR_PBMP_ITER(group_phy_ports, logical_port)
            {
                SHR_IF_ERR_EXIT(dnx_port_flexe_phy_port_is_occupied_get(unit, group_index, logical_port, &is_occupied));
                if (is_occupied)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "there are active time slots in the group. Please clear the slots before adding this FlexE PHY port.\r\n");
                }
            }
        }
        /** Make sure the slot mode is the same */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
        SHR_VAL_VERIFY(phy_info->slot_mode, slot_mode, _SHR_E_PARAM,
                       "The slot mode should be same as the other PHYs in the group.\r\n");
    }
    else
    {
        /** For remove API, make sure the port belongs to this group */
        if (!_SHR_PBMP_MEMBER(group_phy_ports, phy_info->port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The phsical port doesn't belong to this Group.\r\n");
        }
        /** For remove API, make sure there is no active time slots using this physical port */
        SHR_IF_ERR_EXIT(dnx_port_flexe_phy_port_is_occupied_get(unit, group_index, phy_info->port, &is_occupied));
        if (is_occupied)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "there is active time slots using this physical port.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add one new PHY port into FlexE group.
 *    The new logical phy id should not break the original
 *    logical phy order.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] phy_info - The FlexE group phy info
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
bcm_dnx_port_flexe_group_phy_add(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_phy_info_t * phy_info)
{
    int group_index, flexe_core_port;
    bcm_pbmp_t phy_ports, flexe_core_port_bmp;
    uint32 group_add_flags = 0;
    int logical_phy_id[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS] = { 0 };
    flexe_core_port_info_t port_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_modify_verify(unit, gport, flags, phy_info, TRUE));
    /*
     * Store the FlexE phy ports into the SW DB
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));
    _SHR_PBMP_PORT_ADD(phy_ports, phy_info->port);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_set(unit, group_index, &phy_ports));
    /*
     * Store the FlexE logical PHY ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_set(unit, phy_info->port, phy_info->logical_phy_id));
    /*
     * All instance will be rearranged after adding the FlexE PHY,
     * so need to clear the unaligned status
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_cal_slots_is_unaligned_set(unit, group_index, 0));
    /*
     * Get FlexE core port
     */
    BCM_PBMP_CLEAR(flexe_core_port_bmp);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_info->port, &flexe_core_port));
    BCM_PBMP_PORT_ADD(flexe_core_port_bmp, flexe_core_port);
    /*
     * Get the FlexE core port info
     */
    SHR_IF_ERR_EXIT(dnx_flexe_core_port_info_get(unit, phy_info->port, 0, &port_info));
    /*
     * Configure the FlexE core, we always add it for RX and Tx
     */
    group_add_flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX;
    logical_phy_id[flexe_core_port] = phy_info->logical_phy_id;
    SHR_IF_ERR_EXIT(flexe_core_group_phy_add
                    (unit, group_add_flags, group_index, flexe_core_port_bmp, logical_phy_id, &port_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete one PHY port from FlexE group.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] phy_info - The FlexE group phy info
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
bcm_dnx_port_flexe_group_phy_remove(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_phy_info_t * phy_info)
{
    int group_index, flexe_core_port, nof_ports;
    uint32 group_remove_flags = 0;
    bcm_pbmp_t phy_ports, flexe_core_port_bmp;
    int phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    uint8 is_unaligned;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_modify_verify(unit, gport, flags, phy_info, FALSE));

    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_cal_slots_is_unaligned_get(unit, group_index, &is_unaligned));
    /*
     * If the group slots are not aligned, need to check if
     * the current removal will bring the group into unaligned status.
     */
    if (!is_unaligned)
    {
        /*
         * Get FlexE core ports sorted by logical PHY ID
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));
        /*
         * If the removed PHY is not the one with biggest logical PHY ID,
         * we need to reorder the FlexE instatnce when inserting the new FlexE PHY.
         * Timeslots also need to be cleared and re-allocated when inserting the
         * new FlexE PHY. Therefore we need to mark the flexe core cal slots is unaligned.
         * This flag will be used for validity checking when adding new FlexE PHY.
         */

        SHR_RANGE_VERIFY(nof_ports, 1, dnx_data_nif.flexe.nof_pcs_get(unit),
                         _SHR_E_PORT, "nof_ports is out of range.\n");
        SHR_IF_ERR_EXIT(dnx_algo_flexe_cal_slots_is_unaligned_set
                        (unit, group_index, (phy_ports_arr[nof_ports - 1] != phy_info->port)));
    }
    /*
     * Since the phy port is removed, need to adjust the calendar_slots
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_calendar_slots_adjust(unit, group_index, phy_info->port));
    /*
     * Store the FlexE phy ports into the SW DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));
    _SHR_PBMP_PORT_REMOVE(phy_ports, phy_info->port);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_set(unit, group_index, &phy_ports));
    /*
     * Get FlexE core port
     */
    BCM_PBMP_CLEAR(flexe_core_port_bmp);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_info->port, &flexe_core_port));
    BCM_PBMP_PORT_ADD(flexe_core_port_bmp, flexe_core_port);
    /*
     * Configure the FlexE core, we always remove it for Rx and Tx
     */
    group_remove_flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(flexe_core_group_phy_remove(unit, group_remove_flags, group_index, flexe_core_port_bmp));
    /*
     * Clear the FlexE logical PHY ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_set(unit, phy_info->port, -1));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_flexe_group_cal_active_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] active_cal - Active calendar ID, A or B
 * \param [in] is_set - If called by set API
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
dnx_port_flexe_group_cal_active_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t * active_cal,
    int is_set)
{
    int group_index;
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /*
     * Null checks
     */
    SHR_NULL_CHECK(active_cal, _SHR_E_PARAM, "active_cal");
    /*
     * Check port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
    /*
     * For modify API, the FlexE group should be valid
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));

    if (is_set)
    {
        /*
         * We can only configure the active calendar in Mux, meaning Tx.
         */
        supported_flags = BCM_PORT_FLEXE_TX;
        /*
         * If G.hao is supported, Rx flag is supported to switch schedule calendar
         */
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
        {
            supported_flags |= BCM_PORT_FLEXE_RX;
        }
        /*
         * Verify calendar ID
         */
        SHR_RANGE_VERIFY(*active_cal, 0, bcmPortFlexeGroupCalCount - 1, _SHR_E_PARAM, "Calendar ID is out of range.\n");
        /*
         * Check if G.hao is in progress
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, flags));
    }
    else
    {
        supported_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX;
    }
    /*
     * Verify flags
     */
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get active calendar from FlexE mux/demux
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [out] active_cal - Active calendar ID, A or B
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
bcm_dnx_port_flexe_group_cal_active_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t * active_cal)
{
    int group_index;
    bcm_pbmp_t flexe_core_ports;
    bcm_port_t flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_cal_active_verify(unit, gport, flags, active_cal, 0));
    /*
     * Get FlexE core ports
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));
    /*
     * Get Activie calendar in Mux/demux
     */
    _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(flexe_core_active_calendar_get(unit, flexe_core_port, flags, (int *) active_cal));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure active calendar in FlexE mux side
 */
static int
dnx_port_flexe_group_cal_active_tx_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_flexe_group_cal_t active_cal)
{
    int group_index, max_nof_clients, slot_index, nof_slots, group_speed;
    int calendar_slots[DNX_DATA_MAX_NIF_FLEXE_MAX_NOF_SLOTS];
    int ts_num, client_channel;
    bcm_port_flexe_group_cal_t curr_active_cal;
    bcm_pbmp_t flexe_core_ports, all_channels_in_group;
    bcm_port_t flexe_core_port;
    dnx_algo_flexe_client_ts_config_t *ts_config_db = NULL;
    flexe_core_rate_adpt_info_t rate_adpt_info;
    bcm_port_flexe_phy_slot_mode_t slot_mode;
    uint32 flags = BCM_PORT_FLEXE_TX;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get FlexE core ports
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));
    /*
     * Get current active calendar
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_flexe_group_cal_active_get(unit, gport, flags, &curr_active_cal));
    /*
     * If there is calendar switching, need to update rate adapter accordingly.
     */
    if (curr_active_cal != active_cal)
    {
        /*
         * Get slot mode
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
        /*
         * Alloc space
         */
        max_nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
        SHR_ALLOC_SET_ZERO(ts_config_db, (max_nof_clients * sizeof(dnx_algo_flexe_client_ts_config_t)), "ts_config_db",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        _SHR_PBMP_CLEAR(all_channels_in_group);
        /*
         * Get time slots
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group_index, &group_speed));
        nof_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
        for (slot_index = 0; slot_index < nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_cal_slot_get
                            (unit, group_index, flags, active_cal, slot_index, &calendar_slots[slot_index]));
            if (calendar_slots[slot_index] != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, calendar_slots[slot_index], &client_channel));
                _SHR_PBMP_PORT_ADD(all_channels_in_group, client_channel);
            }
        }
        /*
         * Build the Time slots DB for each original client
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_client_time_slots_db_build
                        (unit, group_index, nof_slots, calendar_slots, max_nof_clients, ts_config_db));
        /*
         * Get all the channels in the current calendar
         */
        for (slot_index = 0; slot_index < nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_cal_slot_get
                            (unit, group_index, flags, curr_active_cal, slot_index, &calendar_slots[slot_index]));
            if (calendar_slots[slot_index] != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, calendar_slots[slot_index], &client_channel));
                _SHR_PBMP_PORT_ADD(all_channels_in_group, client_channel);
            }
        }
        /*
         * Update rate adpater for all the clients in the group:
         *
         * - If there is no slot for the client, always delete it in rate adapter
         * - If there are slots for the client, always add it in the rate adapter.
         */
        flexe_core_rate_adpt_info_t_init(unit, &rate_adpt_info);
        _SHR_PBMP_ITER(all_channels_in_group, client_channel)
        {
            SHR_BITCOUNT_RANGE(ts_config_db[client_channel].ts_mask, ts_num, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
            rate_adpt_info.speed = ts_num * dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;

            if (ts_config_db[client_channel].nof_flexe_core_ports != 0)
            {
                SHR_IF_ERR_EXIT(flexe_core_rateadpt_add(unit, client_channel, &rate_adpt_info));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_rateadpt_delete(unit, client_channel, &rate_adpt_info));
            }
        }
    }
    /*
     * Switch the schedule calendar
     */
    if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
    {
        SHR_IF_ERR_EXIT(flexe_core_schedule_calendar_switch(unit, group_index, flags, active_cal));
    }
    /*
     * Configure active calendar in MUX
     */
    _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(flexe_core_active_calendar_set(unit, flexe_core_port, active_cal));
    }

exit:
    SHR_FREE(ts_config_db);
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure active calendar in FlexE Demux side
 */
static int
dnx_port_flexe_group_cal_active_rx_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_flexe_group_cal_t active_cal)
{
    int group_index;
    uint32 flags = BCM_PORT_FLEXE_RX;

    SHR_FUNC_INIT_VARS(unit);

    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(flexe_core_schedule_calendar_switch(unit, group_index, flags, active_cal));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure active calendar in FlexE mux/demux
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags, rx or tx
 * \param [in] active_cal - Active calendar ID, A or B
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
bcm_dnx_port_flexe_group_cal_active_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t active_cal)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_cal_active_verify(unit, gport, flags, &active_cal, 1));

    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_group_cal_active_tx_set(unit, gport, active_cal));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_group_cal_active_rx_set(unit, gport, active_cal));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_flexe_group_cal_slots_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags, RX and Tx
 * \param [in] calendar_id - Calendar ID
 * \param [in] nof_slots - Number of slots
 * \param [in] calendar_slots - calendar slots array.
 * \param [in] is_set - If called by set API
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
dnx_port_flexe_group_cal_slots_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots,
    int is_set)
{
    int group_index, slot_index, min_client_id, max_client_id;
    int group_speed, max_slots, is_bypass, count, tmp_group_index;
    uint32 supported_flags, tmp_flags = 0;
    dnx_algo_port_info_s port_info;
    bcm_port_flexe_phy_slot_mode_t slot_mode;
    uint32 directions[] = { BCM_PORT_FLEXE_TX, BCM_PORT_FLEXE_RX };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /*
     * Null checks
     */
    SHR_NULL_CHECK(calendar_slots, _SHR_E_PARAM, "calendar_slots");
    /*
     * Check port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
    /*
     * For modify API, the FlexE group should be valid
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 1));
    /*
     * Verify calendar ID
     */
    SHR_RANGE_VERIFY(calendar_id, 0, bcmPortFlexeGroupCalCount - 1, _SHR_E_PARAM, "Calendar ID is out of range.\n");

    /*
     * Get the current active calendar using SOC API?
     */
    /*
     * Verify for set and get APIs
     */
    if (is_set)
    {
        /*
         * Verify slots number
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group_index, &group_speed));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
        max_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
        SHR_VAL_VERIFY(nof_slots, max_slots, _SHR_E_PARAM, "nof slots is incorrect.\n");
        /*
         * Get group bypass indication
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_is_bypass_get(unit, group_index, &is_bypass));
        if (flags & BCM_PORT_FLEXE_OVERHEAD)
        {
            /*
             * For overhead, we can only set the client ID for Tx.
             */
            supported_flags = BCM_PORT_FLEXE_OVERHEAD | BCM_PORT_FLEXE_TX;
            SHR_IF_ERR_EXIT(dnx_algo_flexe_channel_to_client_id_map_verify
                            (unit, group_index, calendar_id, nof_slots, calendar_slots));
            if (is_bypass)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Bypass group doesn't support overhead configuration.\r\n");
            }
        }
        else
        {
            supported_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX;
        }
        /*
         * Verify flags
         */
        SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");
        /*
         * Check if G.hao is in progress
         */
        tmp_flags = flags;
        if (!DNX_ALGO_PORT_FLEXE_TX_GET(tmp_flags) && !DNX_ALGO_PORT_FLEXE_RX_GET(tmp_flags))
        {
            DNX_ALGO_PORT_FLEXE_TX_SET(tmp_flags);
            DNX_ALGO_PORT_FLEXE_RX_SET(tmp_flags);
        }
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, tmp_flags));
        /*
         * Verify the slots array
         */
        for (slot_index = 0; slot_index < nof_slots; ++slot_index)
        {
            if (calendar_slots[slot_index] != 0)
            {
                if (flags & BCM_PORT_FLEXE_OVERHEAD)
                {
                    /*
                     * For overhead configuration, need to verify the client ID range.
                     */
                    min_client_id = dnx_data_nif.flexe.min_client_id_get(unit);
                    max_client_id = dnx_data_nif.flexe.max_client_id_get(unit);
                    SHR_RANGE_VERIFY(calendar_slots[slot_index], min_client_id, max_client_id, _SHR_E_PARAM,
                                     "client id is out of range.\n");
                }
                else
                {
                    /*
                     * For non-overhead case, need to verify the port is BusA client.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, calendar_slots[slot_index], &port_info));
                    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
                    {
                        SHR_ERR_EXIT(_SHR_E_PORT, "The port type should be FlexE client.\r\n");
                    }
                    /*
                     * Verify if the slots have been configured in other groups
                     */
                    for (count = 0; count < COUNTOF(directions); ++count)
                    {
                        if (tmp_flags & directions[count])
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_flexe_client_to_group_index_get
                                            (unit, calendar_slots[slot_index], directions[count], &tmp_group_index));
                            if ((tmp_group_index != -1) && (tmp_group_index != group_index))
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG, "The FlexE client has been added into other groups!\n");
                            }
                        }
                    }
                }
            }
            if (is_bypass && (slot_index >= 1))
            {
                /*
                 * For bypass mode, all the slots should be allocted to the same client
                 */
                SHR_VAL_VERIFY(calendar_slots[slot_index], calendar_slots[slot_index - 1], _SHR_E_PARAM,
                               "All the slots should be assigned to the same FlexE client for bypass mode.\n");
            }
        }
    }
    else
    {
        supported_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_OVERHEAD;
        /*
         * Verify flags
         */
        SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the client ID in overhead
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] flags - flags, RX and Tx
 * \param [in] calendar_id - Calendar ID
 * \param [in] nof_slots - Max number of slots
 * \param [out] calendar_slots - calendar slots array.
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
dnx_port_flexe_group_cal_slots_oh_get(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots)
{
    int nof_ports, i;
    bcm_port_t phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int flexe_core_port_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));

    for (i = 0; i < nof_ports; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_ports_arr[i], &flexe_core_port_arr[i]));
    }
    SHR_IF_ERR_EXIT(flexe_core_oh_client_id_get
                    (unit, calendar_id, flags, nof_ports, flexe_core_port_arr, nof_slots, calendar_slots));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the calendar slots for Mux/demux and Overhead
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags, RX and Tx
 * \param [in] calendar_id - Calendar ID
 * \param [in] max_nof_slots - Max number of slots
 * \param [out] calendar_slots - calendar slots array.
 * \param [out] actual_nof_slots - Actual number of slots.
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
bcm_dnx_port_flexe_group_cal_slots_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int max_nof_slots,
    int *calendar_slots,
    int *actual_nof_slots)
{
    int is_overhead, group_index, ts_index, group_speed;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_cal_slots_verify
                           (unit, gport, flags, calendar_id, max_nof_slots, calendar_slots, 0));
    /*
     * Init the calendar slots array
     */
    for (ts_index = 0; ts_index < max_nof_slots; ++ts_index)
    {
        calendar_slots[ts_index] = 0;
    }

    is_overhead = (flags & BCM_PORT_FLEXE_OVERHEAD);
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    /*
     * Get actual nof slots
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group_index, &group_speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    *actual_nof_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
    if (max_nof_slots < *actual_nof_slots)
    {
        *actual_nof_slots = max_nof_slots;
    }

    if (!is_overhead)
    {
        /*
         * For non-overhead case, we get the slots array for SW DB.
         */
        for (ts_index = 0; ts_index < *actual_nof_slots; ++ts_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_cal_slot_get
                            (unit, group_index, flags, calendar_id, ts_index, &calendar_slots[ts_index]));
        }
    }
    else
    {
        /*
         * For overhead case, we need to get the Rx overhead from register, as the Rx
         * overhead is from link partner. To simplify the codes, the Tx overhead is also
         * from register.
         */
        SHR_IF_ERR_EXIT(dnx_port_flexe_group_cal_slots_oh_get
                        (unit, group_index, flags, calendar_id, *actual_nof_slots, calendar_slots));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the calendar slots for Mux/demux
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] flags - flags, RX and Tx
 * \param [in] calendar_id - Calendar ID
 * \param [in] nof_slots - Max number of slots
 * \param [in] calendar_slots - calendar slots array.
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
dnx_port_flexe_group_cal_slots_set(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots)
{
    int ori_calendar_slots[DNX_DATA_MAX_NIF_FLEXE_MAX_NOF_SLOTS];
    int max_nof_clients, client_channel, slot_index, count, ts_num, is_bypass;
    bcm_gport_t gport;
    bcm_port_flexe_group_cal_t active_cal;
    dnx_algo_flexe_client_ts_config_t *ts_config_db = NULL;
    dnx_algo_flexe_client_ts_config_t *orig_ts_config_db = NULL;
    uint32 slots_direction[] = { BCM_PORT_FLEXE_TX, BCM_PORT_FLEXE_RX };
    bcm_pbmp_t changed_clients;
    flexe_core_rate_adpt_info_t rate_adpt_info;
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * If either RX or TX flags are not set, set both
     */
    if (!DNX_ALGO_PORT_FLEXE_TX_GET(flags) && !DNX_ALGO_PORT_FLEXE_RX_GET(flags))
    {
        DNX_ALGO_PORT_FLEXE_TX_SET(flags);
        DNX_ALGO_PORT_FLEXE_RX_SET(flags);
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_is_bypass_get(unit, group_index, &is_bypass));
    if (DNX_ALGO_PORT_FLEXE_TX_GET(flags) && !is_bypass)
    {
        /*
         * Get the Tx active calendar, need to use current active
         * cal to decide if we need to configure rate adapter.
         */
        BCM_FLEXE_GROUP_GPORT_SET(gport, group_index);
        SHR_IF_ERR_EXIT(bcm_dnx_port_flexe_group_cal_active_get(unit, gport, BCM_PORT_FLEXE_TX, &active_cal));
    }
    /*
     * Alloc space
     */
    max_nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    SHR_ALLOC_SET_ZERO(ts_config_db, (max_nof_clients * sizeof(dnx_algo_flexe_client_ts_config_t)), "ts_config_db",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(orig_ts_config_db, (max_nof_clients * sizeof(dnx_algo_flexe_client_ts_config_t)),
                       "orig_ts_config_db", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /*
     * Build the Time slots new DB for each new client
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_client_time_slots_db_build
                    (unit, group_index, nof_slots, calendar_slots, max_nof_clients, ts_config_db));

    flexe_core_rate_adpt_info_t_init(unit, &rate_adpt_info);
    for (count = 0; count < sizeof(slots_direction) / sizeof(uint32); ++count)
    {
        if (!(flags & slots_direction[count]))
        {
            /*
             * If the direction is not configured
             */
            continue;
        }
        /*
         * Get original calendar configuration
         */
        for (slot_index = 0; slot_index < nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_cal_slot_get
                            (unit, group_index, slots_direction[count], calendar_id, slot_index,
                             &ori_calendar_slots[slot_index]));
        }
        /*
         * Build the Time slots DB for each original client
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_client_time_slots_db_build
                        (unit, group_index, nof_slots, ori_calendar_slots, max_nof_clients, orig_ts_config_db));
        /*
         * Find the clients whose slots are changed
         */
        _SHR_PBMP_CLEAR(changed_clients);
        for (client_channel = 0; client_channel < max_nof_clients; ++client_channel)
        {
            if (sal_memcmp
                (&orig_ts_config_db[client_channel], &ts_config_db[client_channel],
                 sizeof(dnx_algo_flexe_client_ts_config_t)))
            {
                _SHR_PBMP_PORT_ADD(changed_clients, client_channel);
            }
        }
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
        _SHR_PBMP_ITER(changed_clients, client_channel)
        {
            if (orig_ts_config_db[client_channel].nof_flexe_core_ports != 0)
            {
                /*
                 * Delete the original time slots in calendar
                 */
                SHR_IF_ERR_EXIT(flexe_core_client_delete(unit, client_channel, slots_direction[count], calendar_id,
                                                         orig_ts_config_db[client_channel].flexe_core_port_array,
                                                         orig_ts_config_db[client_channel].ts_mask,
                                                         orig_ts_config_db[client_channel].nof_flexe_core_ports));
                /*
                 * Need to update the rate adpater is configuring the active calendar
                 * Always update the rate adpater for bypass mode
                 */
                if (DNX_ALGO_PORT_FLEXE_TX_GET(slots_direction[count]) && ((calendar_id == active_cal) || is_bypass))
                {
                    SHR_BITCOUNT_RANGE(orig_ts_config_db[client_channel].ts_mask, ts_num, 0,
                                       BCM_PORT_FLEXE_MAX_NOF_SLOTS);
                    rate_adpt_info.speed =
                        ts_num * dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
                    SHR_IF_ERR_EXIT(flexe_core_rateadpt_delete(unit, client_channel, &rate_adpt_info));
                }
            }
        }
        _SHR_PBMP_ITER(changed_clients, client_channel)
        {
            if (ts_config_db[client_channel].nof_flexe_core_ports != 0)
            {
                /*
                 * Add new time stlos in calendar
                 */
                SHR_IF_ERR_EXIT(flexe_core_client_add(unit, client_channel, slots_direction[count], calendar_id,
                                                      ts_config_db[client_channel].flexe_core_port_array,
                                                      ts_config_db[client_channel].ts_mask,
                                                      ts_config_db[client_channel].nof_flexe_core_ports));
                /*
                 * Need to update the rate adpater is configuring the active calendar
                 * Always update the rate adpater for bypass mode
                 */
                if (DNX_ALGO_PORT_FLEXE_TX_GET(slots_direction[count]) && ((calendar_id == active_cal) || is_bypass))
                {
                    SHR_BITCOUNT_RANGE(ts_config_db[client_channel].ts_mask, ts_num, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
                    rate_adpt_info.speed =
                        ts_num * dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
                    SHR_IF_ERR_EXIT(flexe_core_rateadpt_add(unit, client_channel, &rate_adpt_info));
                }
                /*
                 * Update slot mode in DB
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_client_slot_mode_set
                                (unit, client_channel, (slots_direction[count] == BCM_PORT_FLEXE_RX), slot_mode));
            }
        }
        /*
         * Switch the schedule calendar
         */
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
        {
            if (_SHR_PBMP_NOT_NULL(changed_clients) && DNX_ALGO_PORT_FLEXE_TX_GET(slots_direction[count])
                && (calendar_id == active_cal))
            {
                SHR_IF_ERR_EXIT(flexe_core_schedule_calendar_switch
                                (unit, group_index, slots_direction[count], calendar_id));
            }
        }
    }
    /*
     * Update the SW DB
     */
    for (slot_index = 0; slot_index < nof_slots; ++slot_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_cal_slot_set
                        (unit, group_index, flags, calendar_id, slot_index, calendar_slots[slot_index]));
    }
    /*
     * For bypass mode, all the slots should be the same, so we can
     * configure bypass mode for the first element.
     */
    if (calendar_slots[0] != 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_is_bypass_set(unit, calendar_slots[0], is_bypass));
    }
exit:
    SHR_FREE(ts_config_db);
    SHR_FREE(orig_ts_config_db);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the client ID in overhead
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] calendar_id - Calendar ID
 * \param [in] nof_slots - Max number of slots
 * \param [in] calendar_slots - calendar slots array.
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
dnx_port_flexe_group_cal_slots_oh_set(
    int unit,
    int group_index,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots)
{
    int nof_ports, i;
    bcm_port_t phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int flexe_core_port_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));

    for (i = 0; i < nof_ports; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_ports_arr[i], &flexe_core_port_arr[i]));
    }
    SHR_IF_ERR_EXIT(flexe_core_oh_client_id_set
                    (unit, calendar_id, nof_ports, flexe_core_port_arr, nof_slots, calendar_slots));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the calendar slots for Mux/demux and Overhead
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags, RX and Tx
 * \param [in] calendar_id - Calendar ID
 * \param [in] nof_slots - Number of slots
 * \param [in] calendar_slots - calendar slots array.
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
bcm_dnx_port_flexe_group_cal_slots_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots)
{
    int is_config_oh, group_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_cal_slots_verify
                           (unit, gport, flags, calendar_id, nof_slots, calendar_slots, 1));

    is_config_oh = (flags & BCM_PORT_FLEXE_OVERHEAD);
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);

    if (!is_config_oh)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_group_cal_slots_set
                        (unit, group_index, flags, calendar_id, nof_slots, calendar_slots));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_group_cal_slots_oh_set
                        (unit, group_index, calendar_id, nof_slots, calendar_slots));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_flexe_oh_get/set API.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] type - FlexE overhead type
 * \param [in] val - Overhead value
 * \param [in] is_set - If this API is called by SET API
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
dnx_port_flexe_oh_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_type_t type,
    int *val,
    int is_set)
{
    uint32 supported_flags;
    int group_index, flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify flags */
    if (is_set)
    {
        supported_flags = BCM_PORT_FLEXE_TX;
    }
    else
    {
        supported_flags = BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX;
    }
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify port type ,OH type and val*/
    if ((type == bcmPortFlexeOhGroupID) || (type == bcmPortFlexeOhCalInUse) ||
        (type == bcmPortFlexeOhCalRequest) || (type == bcmPortFlexeOhCalAck) || (type == bcmPortFlexeOhSyncConfig))
    {
        /** Verify port type */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));

        /** Verify val */
        if (is_set)
        {
            if (type == bcmPortFlexeOhGroupID)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_group_id_free_verify(unit, *val));
            }
            else if (type == bcmPortFlexeOhSyncConfig)
            {
                SHR_RANGE_VERIFY(*val, 0, 1, _SHR_E_PARAM, "The expected SC value should be 0 or 1.\n");
            }
            else
            {
                SHR_RANGE_VERIFY(*val, 0, bcmPortFlexeGroupCalCount - 1, _SHR_E_PARAM,
                                 "Calendar ID is out of range.\n");
            }
            if ((type != bcmPortFlexeOhCalInUse) && (type != bcmPortFlexeOhCalRequest)
                && (type != bcmPortFlexeOhCalAck))
            {
                /*
                 * Check if G.hao is in progress
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, flags));
            }
        }
    }
    else if (type == bcmPortFlexeOhLogicalPhyID)
    {
        /** Verify port type */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_verify(unit, gport, 0));

        /** Verify val */
        if (is_set)
        {
            /*
             * Verify the new logical PHY id. The new logical PHY ID should not break
             * the original logical PHY order.
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_modify_verify(unit, gport, *val));
            /*
             * Get group index
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, gport, &flexe_core_port));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_group_index_get(unit, flexe_core_port, &group_index));
            if (group_index != -1)
            {
                /*
                 * Check if G.hao is in progress
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_active_verify(unit, group_index, flags));
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported FlexE oh type.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE overhead.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] type - FlexE overhead type
 * \param [in] val - Overhead value
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
bcm_dnx_port_flexe_oh_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_type_t type,
    int *val)
{
    int group_index, flexe_core_port, speed;
    bcm_pbmp_t flexe_core_ports;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oh_verify(unit, gport, flags, type, val, FALSE));

    if ((type == bcmPortFlexeOhGroupID) || (type == bcmPortFlexeOhCalInUse) ||
        (type == bcmPortFlexeOhCalAck) || (type == bcmPortFlexeOhCalRequest) || (type == bcmPortFlexeOhSyncConfig))
    {
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));

        _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port)
        {
            if (type == bcmPortFlexeOhGroupID)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_group_id_get(unit, flexe_core_port, flags, val));
            }
            else if (type == bcmPortFlexeOhCalInUse)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_c_bit_get(unit, flexe_core_port, flags, val));
            }
            else if (type == bcmPortFlexeOhCalRequest)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_cr_bit_get(unit, flexe_core_port, flags, val));
            }
            else if (type == bcmPortFlexeOhCalAck)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_ca_bit_get(unit, flexe_core_port, flags, val));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_sc_bit_get(unit, flexe_core_port, flags, val));
            }
            break;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, gport, &flexe_core_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, gport, 0, &speed));
        SHR_IF_ERR_EXIT(flexe_core_oh_logical_phy_id_get(unit, flexe_core_port, speed, flags, val));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE overhead.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport
 * \param [in] flags - flags
 * \param [in] type - FlexE overhead type
 * \param [out] val - Overhead value
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
bcm_dnx_port_flexe_oh_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_type_t type,
    int val)
{
    int original_val, speed;
    int group_index, flexe_core_port;
    bcm_pbmp_t flexe_core_ports;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oh_verify(unit, gport, flags, type, &val, TRUE));

    if ((type == bcmPortFlexeOhGroupID) || (type == bcmPortFlexeOhCalInUse) ||
        (type == bcmPortFlexeOhCalAck) || (type == bcmPortFlexeOhCalRequest) || (type == bcmPortFlexeOhSyncConfig))
    {
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));

        _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port)
        {
            if (type == bcmPortFlexeOhGroupID)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_group_index_to_id_get(unit, group_index, &original_val));
                if (original_val == val)
                {
                    SHR_EXIT();
                }
                else
                {
                    SHR_IF_ERR_EXIT(flexe_core_oh_group_id_set(unit, flexe_core_port, val));
                }
            }
            else if (type == bcmPortFlexeOhCalInUse)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_c_bit_set(unit, flexe_core_port, val));
            }
            else if (type == bcmPortFlexeOhCalRequest)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_cr_bit_set(unit, flexe_core_port, val));
            }
            else if (type == bcmPortFlexeOhCalAck)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_ca_bit_set(unit, flexe_core_port, val));
            }
            else
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_sc_bit_set(unit, flexe_core_port, val));
            }
        }
        /*
         * Update DB
         */
        if (type == bcmPortFlexeOhGroupID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_index_to_id_set(unit, group_index, val));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, gport, &original_val));
        if (original_val == val)
        {
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, gport, &flexe_core_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, gport, 0, &speed));
            SHR_IF_ERR_EXIT(flexe_core_oh_logical_phy_id_set(unit, flexe_core_port, speed, val));
            /*
             * Update DB
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_set(unit, gport, val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_verify(
    int unit,
    uint32 flags,
    bcm_port_t src_port,
    int channel,
    bcm_port_t dst_port,
    dnx_algo_flexe_flow_type_e * flow_type)
{
    bcm_port_t src_local_port;
    dnx_algo_port_info_s src_port_info, dst_port_info;
    dnx_flexe_mode_e flexe_mode = DNX_FLEXE_MODE_DISABLED;
    int max_channel;
    char src_port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    char dst_port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unknown flags.\n");
    /*
     * Verify the gport
     */
    if (BCM_GPORT_IS_SET(src_port))
    {
        if (BCM_GPORT_IS_NIF_RX_PRIORITY_LOW(src_port) || BCM_GPORT_IS_NIF_RX_PRIORITY_HIGH(src_port))
        {
            src_local_port = BCM_GPORT_NIF_RX_PRIORITY_GET(src_port);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported Gport Type.\n");
        }
    }
    else
    {
        src_local_port = src_port;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, src_local_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, dst_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_local_port, &src_port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_port, &dst_port_info));
    /*
     * Get flexe mode
     */
    flexe_mode = dnx_data_nif.flexe.flexe_mode_get(unit);

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, src_port_info) &&
        DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, src_port_info) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, dst_port_info, TRUE, FALSE, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2MAC;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, src_port_info) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, dst_port_info, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2SAR;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, src_port_info, TRUE, FALSE, TRUE) &&
             DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_MAC2CLIENT;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, src_port_info, TRUE, FALSE, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, dst_port_info, TRUE, FALSE, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_MAC2MAC;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, src_port_info, TRUE, FALSE, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, dst_port_info, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_MAC2SAR;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, dst_port_info, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2SAR;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit, src_port_info) &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, dst_port_info, TRUE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_MAC2ETH;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, src_port_info, FALSE, FALSE, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, dst_port_info, 0) && (DNX_FLEXE_MODE_DISTRIBUTED == flexe_mode))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_MAC2ILKN;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2CLIENT;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit, dst_port_info) && (DNX_FLEXE_MODE_DISTRIBUTED == flexe_mode))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2MAC;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, dst_port_info, FALSE, TRUE, FALSE) &&
             !dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_l1_support))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2MAC;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE)
             && DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, dst_port_info))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2ILKN;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, src_port_info, TRUE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit, dst_port_info))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_ETH2MAC;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_ILKN_L1(unit, src_port_info)
             && DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, dst_port_info, FALSE))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_ILKN2SAR;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, src_port_info, TRUE) &&
             DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, dst_port_info, FALSE) &&
             dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_ETH2SAR;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, src_port_info, FALSE) &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, dst_port_info, TRUE) &&
             dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_SAR2ETH;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, src_port_info, TRUE) &&
             DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, dst_port_info) &&
             dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_ETH2CLIENT;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, src_port_info) &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, dst_port_info, TRUE) &&
             dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2ETH;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, src_port_info, TRUE) &&
             DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, dst_port_info, TRUE) &&
             dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        *flow_type = DNX_ALGO_FLEXE_FLOW_ETH2ETH;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, src_port_info.port_type, src_port_info_str));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, dst_port_info.port_type, dst_port_info_str));
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Setting flow from port type %s to port type %s is not supported for FlexE %s mode.\n",
                         src_port_info_str, dst_port_info_str,
                         (DNX_FLEXE_MODE_CENTRALIZED == flexe_mode ? "CENTRALIZED" : "DISTRIBUTED"));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Setting flow from port type %s to port type %s is not supported.\n",
                         src_port_info_str, dst_port_info_str);
        }
    }
    /*
     * Per Rx priority configuration is only supported on datapath BusB->ILKN
     */
    if ((*flow_type != DNX_ALGO_FLEXE_FLOW_MAC2ILKN) && BCM_GPORT_IS_NIF_RX_PRIORITY(src_port))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This flow doesn't support per RX priority configuration.\n");
    }
    /*
     * Veriy channel
     */
    switch (*flow_type)
    {
        case DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT:
        case DNX_ALGO_FLEXE_FLOW_MAC2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_SAR2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_ETH2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2MAC:
        case DNX_ALGO_FLEXE_FLOW_MAC2MAC:
        case DNX_ALGO_FLEXE_FLOW_SAR2MAC:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2SAR:
        case DNX_ALGO_FLEXE_FLOW_MAC2SAR:
        case DNX_ALGO_FLEXE_FLOW_SAR2SAR:
        case DNX_ALGO_FLEXE_FLOW_ETH2SAR:
        case DNX_ALGO_FLEXE_FLOW_SAR2ETH:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2ETH:
        case DNX_ALGO_FLEXE_FLOW_ETH2ETH:
        {
            max_channel = dnx_data_nif.framer.b66switch_backup_channels_get(unit);
            break;
        }
        default:
            max_channel = 0;
            break;
    }
    SHR_RANGE_VERIFY(channel, 0, max_channel, _SHR_E_PARAM, "channel=%d is out of range! Max channel is %d.\n", channel,
                     max_channel);
    if (((*flow_type == DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT) ||
         (*flow_type == DNX_ALGO_FLEXE_FLOW_MAC2ETH) || (*flow_type == DNX_ALGO_FLEXE_FLOW_ETH2MAC) ||
         (*flow_type == DNX_ALGO_FLEXE_FLOW_SAR2ILKN) || (*flow_type == DNX_ALGO_FLEXE_FLOW_ILKN2SAR)) &&
        dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_disable_l1))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "L1 switching is not supported on this device.\n");
    }
exit:
    SHR_FUNC_EXIT;

}

static int
dnx_port_flexe_flow_bus2bus_set(
    int unit,
    int src_channel,
    int multicast_id,
    int dst_channel,
    int is_add)
{
    flexe_core_66bswitch_channel_info_t src_info, dest_info;

    SHR_FUNC_INIT_VARS(unit);

    src_info.channel = src_channel;
    dest_info.channel = dst_channel;
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_set(unit, &src_info, &dest_info, multicast_id, is_add));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Map ESB QUEUE to EPNI interface
 *
 */
static int
dnx_flexe_esb_queue_to_egq_interface_map_set(
    int unit,
    int lane_in_core,
    int ethu_id,
    int lane_in_ethu,
    int is_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_Q_2_EGQ_IF_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, lane_in_core);

    if (is_add)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_MACRO, INST_SINGLE, ethu_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_PORT, INST_SINGLE, lane_in_ethu);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, INST_SINGLE, lane_in_core);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_PORT_MACRO, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_PORT, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_tmac2eth_set(
    int unit,
    int src_channel,
    dnx_algo_port_rmc_info_t * rmc,
    int dst_channel,
    bcm_core_t core_id,
    int ethu_id,
    int lane_in_ethu,
    int is_add)
{
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Configure the ESB to EGQ interface mapping at first to avoid
     * flexe traffic is sent to PM0, LANE0
     */
    if (is_add)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_esb_queue_to_egq_interface_map_set(unit, dst_channel, ethu_id, lane_in_ethu, 1));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc->rmc_id);
    /*
     * Configure dest channel to invalid value when removing the flow
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, DBAL_FIELD_PORT_OR_CHANNEL, 0, 0, 0, &field_info));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OR_CHANNEL, INST_SINGLE,
                                 (is_add ? dst_channel : ((1 << field_info.field_nof_bits) - 1)));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, entry_handle_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, dst_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_ALARM_SOURCE_CLIENT, INST_SINGLE,
                                 (is_add ? src_channel : 0x7f));

    /*
     * Configure FEU credit mask
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L1_CREDIT_MASK, INST_SINGLE, (is_add ? 0 : 1));
    /*
     * De-reset credit counter, low reset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RSTN, INST_SINGLE, (is_add ? 1 : 0));
    /*
     * Credit min threshold = 4, by DE suggestion
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD_MIN, INST_SINGLE, (is_add ? 4 : 0));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, entry_handle_id));
    if (!is_add)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_esb_queue_to_egq_interface_map_set(unit, dst_channel, ethu_id, lane_in_ethu, 0));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_eth2tmac_set(
    int unit,
    int src_channel,
    int dst_channel,
    int is_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure TMC<->CLIENT mapping
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, src_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TARGET_CLIENT, INST_SINGLE,
                                 (is_add ? dst_channel : 0x7f));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, entry_handle_id));

    /*
     * Configure TX alarms
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_RX_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, dst_channel);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ALARM_TARGET_TMC, INST_SINGLE,
                                 (is_add ? src_channel : 0x3f));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_tmac2ilkn_set(
    int unit,
    dnx_algo_port_rmc_info_t * rmc,
    int ilu_port_in_core,
    int ilu_channel,
    int is_add)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_feu_rmc_traffic_type_e traffic_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc->rmc_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT, INST_SINGLE,
                                 (is_add ? ilu_port_in_core : 0));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OR_CHANNEL, INST_SINGLE,
                                 (is_add ? ilu_channel : 0));

    if (is_add)
    {
        if (bcmPortNifSchedulerLow == rmc->sch_priority)
        {
            traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_LP;
        }
        else if (bcmPortNifSchedulerHigh == rmc->sch_priority)
        {
            traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_HP;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "RMC scheduler priority %d is not supported for tmac2ilkn flow.\n",
                         rmc->sch_priority);
        }
    }
    else
    {
        traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_IRE_LP;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAFFIC_TYPE, INST_SINGLE, traffic_type);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_sar2ilkn_set(
    int unit,
    int sar_channel,
    int port_id_in_core,
    int ilu_channel,
    int is_add)
{
    flexe_core_sar_channel_map_info_t map_info;

    SHR_FUNC_INIT_VARS(unit);

    flexe_core_sar_channel_map_info_t_init(unit, &map_info);
    /*
     * The ILKN destination info is composed of ILKN port id + ILKN channel
     */
    map_info.sar_channel = sar_channel;
    map_info.peer_channel =
        ((port_id_in_core << (utilex_log2_round_up(dnx_data_port.general.max_nof_channels_get(unit)))) | ilu_channel);
    SHR_IF_ERR_EXIT(flexe_core_sar_channel_map_set(unit, 2 /*TX*/, &map_info, is_add));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_port_flexe_flow_ilkn2sar_set(
    int unit,
    int port_id_in_core,
    int ilu_channel,
    int sar_channel,
    int is_add)
{
    flexe_core_sar_channel_map_info_t map_info;

    SHR_FUNC_INIT_VARS(unit);

    flexe_core_sar_channel_map_info_t_init(unit, &map_info);
    /*
     * The ILKN source info is composed of ILKN port id + ILKN channel
     */
    map_info.sar_channel = sar_channel;
    map_info.peer_channel =
        ((port_id_in_core << (utilex_log2_round_up(dnx_data_port.general.max_nof_channels_get(unit)))) | ilu_channel);
    SHR_IF_ERR_EXIT(flexe_core_sar_channel_map_set(unit, 1 /*RX*/, &map_info, is_add));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure start tx for for mismatch rate.
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] dst_port - destination port for this flow
 * \param [in] enable - enable or disable start tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_flow_mismatch_rate_config_set(
    int unit,
    bcm_port_t src_port,
    bcm_port_t dst_port,
    int enable)
{
    int rx_speed, tx_speed;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get Rx and Tx speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, src_port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, dst_port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
    if (rx_speed != tx_speed)
    {
        SHR_IF_ERR_EXIT(imb_port_l1_mismatch_rate_rx_config_set(unit, src_port, tx_speed, enable));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Internal function for configuring FlexE flow in Framer Gen1.
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] mc_channel - multicast channel
 * \param [in] dst_port - destination port for this flow
 * \param [in] flow_type - flexe flow type
 * \param [in] is_add - set or clear the flexe flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_flow_set(
    int unit,
    bcm_port_t src_port,
    int mc_channel,
    bcm_port_t dst_port,
    dnx_algo_flexe_flow_type_e flow_type,
    int is_add)
{
    int src_channel, dst_channel;
    bcm_port_t src_local_port;
    dnx_algo_port_info_s src_port_info, dst_port_info;
    int i, nof_priority_groups, rmc_is_found;
    dnx_algo_port_rmc_info_t rmc;
    dnx_algo_port_ilkn_access_info_t ilkn_info;
    dnx_algo_port_ethu_access_info_t ethu_info;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_NIF_RX_PRIORITY(src_port))
    {
        src_local_port = BCM_GPORT_NIF_RX_PRIORITY_GET(src_port);
    }
    else
    {
        src_local_port = src_port;
    }

    switch (flow_type)
    {
        case DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2MAC:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2SAR:
        case DNX_ALGO_FLEXE_FLOW_MAC2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_MAC2MAC:
        case DNX_ALGO_FLEXE_FLOW_MAC2SAR:
        case DNX_ALGO_FLEXE_FLOW_SAR2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_SAR2MAC:
        case DNX_ALGO_FLEXE_FLOW_SAR2SAR:
        {
            /*
             * get ports internal channels
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, src_local_port, &src_channel));
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, dst_port, &dst_channel));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_local_port, &src_port_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dst_port, &dst_port_info));

            /*
             * convert to type-based channel id, needed by flexe core
             */
            src_channel = DNX_ALGO_FLEXE_CLIENT_CHANNEL_TO_CORE_CHANNEL_GET(unit, src_channel, src_port_info);
            dst_channel = DNX_ALGO_FLEXE_CLIENT_CHANNEL_TO_CORE_CHANNEL_GET(unit, dst_channel, dst_port_info);

            /*
             * configure b66 switch
             */
            SHR_IF_ERR_EXIT(dnx_port_flexe_flow_bus2bus_set(unit, src_channel, mc_channel, dst_channel, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_MAC2ETH:
        {
            /*
             * get NIF port info
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, dst_port, &ethu_info));
            /*
             * get FLEXE MAC channel
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, src_local_port, &src_channel));

            /*
             * get FlexE RMC info, must be TDM priority
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, src_local_port, 0, 0, &rmc));

            if ((rmc.rmc_id == DNX_ALGO_PORT_INVALID) || (rmc.sch_priority != bcmPortNifSchedulerTDM))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "The RMC of FlexE port %d is invalid or non-TDM priority.\n",
                             src_local_port);
            }
            if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_l1_mismatch_rate_support)
                && !DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, src_channel))
            {
                SHR_IF_ERR_EXIT(dnx_port_flexe_flow_mismatch_rate_config_set(unit, src_local_port, dst_port, is_add));
            }
            SHR_IF_ERR_EXIT(dnx_port_flexe_flow_tmac2eth_set
                            (unit, src_channel, &rmc, ethu_info.lane_in_core, ethu_info.core, ethu_info.ethu_id,
                             ethu_info.first_lane_in_port, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_ETH2MAC:
        {
            /*
             * get NIF port TMC index(= first PHY)
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, src_local_port, 0, &src_channel));
            /*
             * get FLEXE MAC channel
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, dst_port, &dst_channel));

            if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_l1_mismatch_rate_support)
                && !DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, dst_channel))
            {
                SHR_IF_ERR_EXIT(dnx_port_flexe_flow_mismatch_rate_config_set(unit, src_local_port, dst_port, is_add));
            }
            SHR_IF_ERR_EXIT(dnx_port_flexe_flow_eth2tmac_set(unit, src_channel, dst_channel, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_SAR2ILKN:
        {
            /*
             * get SAR channel
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, src_local_port, &src_channel));
            /*
             * get access info and channel for ILKN port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, dst_port, &ilkn_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_out_channel_get(unit, dst_port, &dst_channel));

            SHR_IF_ERR_EXIT(dnx_port_flexe_flow_sar2ilkn_set
                            (unit, src_channel, ilkn_info.port_in_core, dst_channel, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_ILKN2SAR:
        {
            /*
             * get access info and channel for ILKN port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, src_local_port, &ilkn_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_in_channel_get(unit, src_local_port, &src_channel));
            /*
             * get SAR channel
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, dst_port, &dst_channel));

            SHR_IF_ERR_EXIT(dnx_port_flexe_flow_ilkn2sar_set
                            (unit, ilkn_info.port_in_core, src_channel, dst_channel, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_MAC2ILKN:
        {
            /*
             * get access info and channel for ILKN port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, dst_port, &ilkn_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_out_channel_get(unit, dst_port, &dst_channel));

            /*
             * FlexE might have more than one RMC
             */
            nof_priority_groups = dnx_data_nif.mac_client.priority_groups_nof_get(unit);
            rmc_is_found = 0;
            for (i = 0; i < nof_priority_groups; ++i)
            {
                /*
                 * get FlexE RMC info
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, src_local_port, 0, i, &rmc));

                if (rmc.rmc_id != DNX_ALGO_PORT_INVALID)
                {
                    if ((BCM_GPORT_IS_NIF_RX_PRIORITY_LOW(src_port) && (bcmPortNifSchedulerLow == rmc.sch_priority)) ||
                        (BCM_GPORT_IS_NIF_RX_PRIORITY_HIGH(src_port) && (bcmPortNifSchedulerHigh == rmc.sch_priority))
                        || !BCM_GPORT_IS_NIF_RX_PRIORITY(src_port))
                    {
                        SHR_IF_ERR_EXIT(dnx_port_flexe_flow_tmac2ilkn_set
                                        (unit, &rmc, ilkn_info.port_in_core, dst_channel, is_add));
                    }
                    rmc_is_found = 1;
                }
            }
            if (!rmc_is_found)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "The RMC of FlexE port %d is invalid.\n", src_local_port);
            }
            break;
        }

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown flow_type %d\n", flow_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Simple 66b switching configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] channel - main or backup path
 * \param [in] dst_port - destination port for this flow
 * \param [in] is_add - set or clear the flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_gen2_simple_66bswitch_flow_set(
    int unit,
    int src_port,
    int channel,
    int dst_port,
    int is_add)
{
    flexe_core_66bswitch_channel_info_t src_info, dst_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get block ID and channel for src and dest ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, src_port, BCM_PORT_FLEXE_RX, 0, &src_info.block_id, &src_info.channel));
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, dst_port, BCM_PORT_FLEXE_TX, channel, &dst_info.block_id, &dst_info.channel));
    /*
     * Configure 66b switching
     */
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_set(unit, &src_info, &dst_info, 0, is_add));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Update SAR channel map info
 *
 * \param [in] unit - chip unit id
 * \param [in] sar_port - sar port ID
 * \param [in] peer_channel - The peer channel ID
 * \param [in] is_rx - is Rx or Tx
 * \param [in] is_eth - The SAR connects to ETH or flexe client
 * \param [in] is_add - set or clear the flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_gen2_sar_channel_map_set(
    int unit,
    int sar_port,
    int peer_channel,
    int is_rx,
    int is_eth,
    int is_add)
{
    int orig_is_eth;
    uint32 flags = 0;
    flexe_core_sar_channel_map_info_t map_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure channel map, only relevant for add context
     */
    if (is_add)
    {
        /*
         * Get original channel map info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_is_eth_get(unit, sar_port, is_rx, &orig_is_eth));
        /*
         * If the map info is not changed, no need to re-configure SAR,
         * as the re-config needs to remove original configuration.
         */
        if (orig_is_eth != is_eth)
        {
            /*
             * Update DB
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_sar_is_eth_set(unit, sar_port, is_rx, is_eth));
            flexe_core_sar_channel_map_info_t_init(unit, &map_info);
            /*
             * Get virtual FlexE channel
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, sar_port, &map_info.sar_channel));
            map_info.peer_channel = peer_channel;
            flags = is_rx ? BCM_PORT_FLEXE_RX : BCM_PORT_FLEXE_TX;
            SHR_IF_ERR_EXIT(flexe_core_sar_channel_map_set(unit, flags, &map_info, is_add));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure flow between SAR and 66b switching blocks
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] channel - main or backup path
 * \param [in] dst_port - destination port for this flow
 * \param [in] flow_type - flow type
 * \param [in] is_add - set or clear the flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_gen2_sar_66bswitch_flow_set(
    int unit,
    int src_port,
    int channel,
    int dst_port,
    dnx_algo_flexe_flow_type_e flow_type,
    int is_add)
{
    int is_eth = 0;
    flexe_core_66bswitch_channel_info_t src_info, dst_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get block ID and channel for src and dest ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, src_port, BCM_PORT_FLEXE_RX, 0, &src_info.block_id, &src_info.channel));
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, dst_port, BCM_PORT_FLEXE_TX, channel, &dst_info.block_id, &dst_info.channel));

    if ((flow_type == DNX_ALGO_FLEXE_FLOW_CLIENT2SAR) || (flow_type == DNX_ALGO_FLEXE_FLOW_MAC2SAR) ||
        (flow_type == DNX_ALGO_FLEXE_FLOW_ETH2SAR) || (flow_type == DNX_ALGO_FLEXE_FLOW_SAR2SAR))
    {
        /*
         * Configure channel map
         */
        if ((flow_type == DNX_ALGO_FLEXE_FLOW_MAC2SAR) || (flow_type == DNX_ALGO_FLEXE_FLOW_ETH2SAR))
        {
            is_eth = 1;
        }
        SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_sar_channel_map_set(unit, dst_port, dst_info.channel, 1, is_eth, is_add));
    }
    if ((flow_type == DNX_ALGO_FLEXE_FLOW_SAR2CLIENT) || (flow_type == DNX_ALGO_FLEXE_FLOW_SAR2MAC) ||
        (flow_type == DNX_ALGO_FLEXE_FLOW_SAR2ETH) || (flow_type == DNX_ALGO_FLEXE_FLOW_SAR2SAR))
    {
        /*
         * Configure channel map
         */
        if ((flow_type == DNX_ALGO_FLEXE_FLOW_SAR2MAC) || (flow_type == DNX_ALGO_FLEXE_FLOW_SAR2ETH))
        {
            is_eth = 1;
        }
        SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_sar_channel_map_set(unit, src_port, src_info.channel, 0, is_eth, is_add));
    }
    /*
     * Configure 66b switching
     */
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_set(unit, &src_info, &dst_info, 0, is_add));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the the flexe flow. For flow add, verify if the
 *    path has been occupied. For flow remove, verify if the path
 *    exists.
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] channel - multicast channel
 * \param [in] dest_port - destination port for this flow
 * \param [in] is_add - set or clear the flexe flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_gen2_flow_set_verify(
    int unit,
    bcm_port_t src_port,
    int channel,
    bcm_port_t dest_port,
    int is_add)
{
    int nof_dests;
    int tmp_block_id, i, index;
    int dest_block_id, dest_channel;
    int mac2_block_id = dnx_data_nif.framer.mac_info_get(unit, 1)->block_id;
    dnx_algo_port_info_s dest_port_info;
    flexe_core_66bswitch_channel_info_t src_info;
    flexe_core_66bswitch_channel_info_t dest_info[DNX_DATA_MAX_NIF_FRAMER_B66SWITCH_BACKUP_CHANNELS + 1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get block ID and channel ID for framer flow
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, src_port, BCM_PORT_FLEXE_RX, 0, &src_info.block_id, &src_info.channel));
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, dest_port, BCM_PORT_FLEXE_TX, channel, &dest_block_id, &dest_channel));
    /*
     * Verify if 1588 is enable on both src and dest ports
     */
    if ((mac2_block_id == src_info.block_id) && (mac2_block_id == dest_block_id))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "The FlexE flow cannot be configured with 1588 enabled on both Src and Dest ports.\n");
    }
    /*
     * There is no backup channel on MAC2
     */
    if ((mac2_block_id == dest_block_id) && channel >= 1)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "There is no backup path if 1588 is enabled on the dest port.\n");
    }
    /*
     * The flow from MAC2 to BusB is not supported
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, dest_port, &dest_port_info));
    if ((mac2_block_id == src_info.block_id)
        && DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, dest_port_info, FALSE, FALSE, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flow from 1588 enabled port to Framer MAC client.\n");
    }
    /*
     * Get existing flow
     */
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_get(unit, &src_info, dest_info, &nof_dests));

    for (i = 0; i < nof_dests; ++i)
    {
        if (is_add)
        {
            /*
             * Check if the path has been configured
             */
            for (index = 0; index < dnx_data_nif.framer.b66switch_info_get(unit)->key_size[0]; index++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, index, channel, &tmp_block_id));

                if (dest_info[i].block_id == tmp_block_id)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "The path for source port %d, channel %d has been configured.\n",
                                 src_port, channel);
                }
            }
            /*
             * Check MAC info
             */
            for (index = 0; index < dnx_data_nif.framer.mac_info_info_get(unit)->key_size[0]; index++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_framer_inner_mac_block_id_get(unit, index, channel, &tmp_block_id));

                if (dest_info[i].block_id == tmp_block_id)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "The path for source port %d, channel %d has been configured.\n",
                                 src_port, channel);
                }
            }
        }
        else
        {
            if ((dest_info[i].block_id == dest_block_id) && (dest_info[i].channel == dest_channel))
            {
                break;
            }
        }
    }
    if (!is_add && (i == nof_dests))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find flow for source port %d, dest port %d.\n", src_port, dest_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal function for configuring FlexE flow in Framer Gen2.
 *
 * \param [in] unit - chip unit id
 * \param [in] src_port - source port for this flow
 * \param [in] channel - multicast channel
 * \param [in] dst_port - destination port for this flow
 * \param [in] flow_type - flexe flow type
 * \param [in] is_add - set or clear the flexe flow
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_gen2_flow_set(
    int unit,
    bcm_port_t src_port,
    int channel,
    bcm_port_t dst_port,
    dnx_algo_flexe_flow_type_e flow_type,
    int is_add)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Additional verifications for GEN2 flow
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_gen2_flow_set_verify(unit, src_port, channel, dst_port, is_add));

    switch (flow_type)
    {
        case DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2MAC:
        case DNX_ALGO_FLEXE_FLOW_MAC2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_MAC2MAC:
        case DNX_ALGO_FLEXE_FLOW_ETH2ETH:
        case DNX_ALGO_FLEXE_FLOW_ETH2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_CLIENT2ETH:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_simple_66bswitch_flow_set(unit, src_port, channel, dst_port, is_add));
            break;
        }
        case DNX_ALGO_FLEXE_FLOW_CLIENT2SAR:
        case DNX_ALGO_FLEXE_FLOW_MAC2SAR:
        case DNX_ALGO_FLEXE_FLOW_ETH2SAR:
        case DNX_ALGO_FLEXE_FLOW_SAR2CLIENT:
        case DNX_ALGO_FLEXE_FLOW_SAR2MAC:
        case DNX_ALGO_FLEXE_FLOW_SAR2ETH:
        case DNX_ALGO_FLEXE_FLOW_SAR2SAR:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_sar_66bswitch_flow_set
                            (unit, src_port, channel, dst_port, flow_type, is_add));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown flow_type %d\n", flow_type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to set port to port flow for FlexE application.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] flags - reserved
 *   \param [in] src_port - source port for a flow
 *   \param [in] channel - multicast replication channel ID, range:0~2. for unicast use 0 always.
 *   \param [in] dst_port - destination port for a flow
 *
 * \remark
 *      src_port / dst_port port can be
 *          - flexe client
 *          - flexe sar
 *          - flexe mac
 *          - NIF L1 port, created with BCM_PORT_ADD_CROSS_CONNECT
 *          - ILKN L1 port, created with BCM_PORT_ADD_CROSS_CONNECT
 *          - ILKN channelized port (valid for dst_port only, and src_port must be flexe_mac in this case)
 * \see
 *   * None
 */
int
bcm_dnx_port_flexe_flow_set(
    int unit,
    uint32 flags,
    bcm_port_t src_port,
    int channel,
    bcm_port_t dst_port)
{
    dnx_algo_flexe_flow_type_e flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * verify ports and resolve the flow type
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_flow_verify(unit, flags, src_port, channel, dst_port, &flow_type));
    /*
     * Call different APIs for different Framer gen
     */
    if (dnx_data_nif.framer.gen_get(unit) == DNX_ALGO_FLEXE_GEN_2)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_flow_set(unit, src_port, channel, dst_port, flow_type, TRUE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_flow_set(unit, src_port, channel, dst_port, flow_type, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to clear port to port flow for FlexE application.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] src_port - source port for a flow
 *   \param [in] channel - multicast replication channel ID, range:0~2. for unicast use 0 always.
 *   \param [in] dst_port - destination port for a flow
 *
 * \remark
 *      src_port / dst_port port can be
 *          - flexe client
 *          - flexe sar
 *          - flexe mac
 *          - NIF L1 port, created with BCM_PORT_ADD_CROSS_CONNECT
 *          - ILKN L1 port, created with BCM_PORT_ADD_CROSS_CONNECT
 *          - ILKN channelized port (valid for dst_port only, and src_port must be flexe_mac in this case)
 * \see
 *   * None
 */
int
bcm_dnx_port_flexe_flow_clear(
    int unit,
    bcm_port_t src_port,
    int channel,
    bcm_port_t dst_port)
{
    dnx_algo_flexe_flow_type_e flow_type = DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * verify ports and resolve the flow type
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_flow_verify(unit, 0, src_port, channel, dst_port, &flow_type));
    /*
     * Call different APIs for different Framer gen
     */
    if (dnx_data_nif.framer.gen_get(unit) == DNX_ALGO_FLEXE_GEN_2)
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_gen2_flow_set(unit, src_port, channel, dst_port, flow_type, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_flow_set(unit, src_port, channel, dst_port, flow_type, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify API for bcm_dnx_port_flexe_oh_alarm_get.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport or physical port
 * \param [in] flags - flags
 * \param [in] alarms - FlexE overhead alarms
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
dnx_port_flexe_oh_alarm_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_alarm_t * alarms)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(alarms, _SHR_E_PARAM, "alarms");

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify gport type */
    if (BCM_GPORT_IS_FLEXE_GROUP(gport))
    {
        /** Verify FlexE group gport */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));
    }
    else
    {
        /** Verify FlexE physical port */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_verify(unit, gport, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE overhead alarms.
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group index gport or physical port
 * \param [in] flags - flags
 * \param [out] alarms - FlexE overhead alarms
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
bcm_dnx_port_flexe_oh_alarm_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_oh_alarm_t * alarms)
{
    int group_index, flexe_core_port;
    uint16 alarm_status = 0, tmp_status = 0;
    bcm_pbmp_t flexe_core_ports;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oh_alarm_verify(unit, gport, flags, alarms));
    /*
     * Clear alarms
     */
    sal_memset(alarms, 0, sizeof(bcm_port_flexe_oh_alarm_t));

    if (BCM_GPORT_IS_FLEXE_GROUP(gport))
    {
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_ports_get(unit, group_index, &flexe_core_ports));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, gport, &flexe_core_port));
        _SHR_PBMP_CLEAR(flexe_core_ports);
        _SHR_PBMP_PORT_ADD(flexe_core_ports, flexe_core_port);
    }
    /*
     * Get the final status
     */
    _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port)
    {
        SHR_IF_ERR_EXIT(flexe_core_oh_alarm_status_get(unit, flexe_core_port, &tmp_status));
        alarm_status |= tmp_status;
    }
    /*
     * Parse the alarms
     */
    if (BCM_GPORT_IS_FLEXE_GROUP(gport))
    {
        alarms->alarm_active = (alarm_status & DNX_FLEXE_GROUP_ALARMS) ? 1 : 0;
        /*
         * The following alarms are based on FlexE Group
         */
        if (alarms->alarm_active)
        {
            if (alarm_status & FLEXE_CORE_OH_ALARM_PHY_MAP_MISMATCH)
            {
                alarms->phymap_mismatch = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_GROUP_DESKEW)
            {
                alarms->group_deskew_alarm = 1;
            }
        }
    }
    else
    {
        alarms->alarm_active = (alarm_status & DNX_FLEXE_PHY_ALARMS) ? 1 : 0;
        if (alarms->alarm_active)
        {
            /*
             * The following alarms are based on FlexE PHY
             */
            if (alarm_status & FLEXE_CORE_OH_ALARM_GID_MISMATCH)
            {
                alarms->group_id_mismatch = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_PHY_NUM_MISMATCH)
            {
                alarms->phy_id_mismatch = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_LOF)
            {
                alarms->lost_of_frame = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_LOM)
            {
                alarms->lost_of_multiframe = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_RPF)
            {
                alarms->rpf_alarm = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_OH1_ALARM)
            {
                alarms->oh_block_1_alarm = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_C_BIT_ALARM)
            {
                alarms->c_bit_alarm = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_CRC)
            {
                alarms->crc_error = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_SC_MISMATCH)
            {
                alarms->sc_mismatch = 1;
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_GID_UNEQUIPPED)
            {
                alarms->unequipped_alarm = 1;
            }
            /*
             * Get FlexE core port to get the mismatched slots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, gport, &flexe_core_port));
            if (alarm_status & FLEXE_CORE_OH_ALARM_CAL_A_MISMATCH)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_cal_mismatch_ts_get(unit, flexe_core_port, &alarms->cal_a_mismatch));
            }
            if (alarm_status & FLEXE_CORE_OH_ALARM_CAL_B_MISMATCH)
            {
                SHR_IF_ERR_EXIT(flexe_core_oh_cal_mismatch_ts_get(unit, flexe_core_port, &alarms->cal_b_mismatch));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify API for bcm_dnx_port_flexe_oam_alarm_get.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] alarms - FlexE oam alarms
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
dnx_port_flexe_oam_alarm_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_alarm_t * alarms)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_oam_over_flexe))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "OAM over FlexE is not supported on this device.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(alarms, _SHR_E_PARAM, "alarms");

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify port type */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_client_verify(unit, port, 0));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE OAM alarms.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [out] alarms - FlexE oam alarms
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
bcm_dnx_port_flexe_oam_alarm_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_alarm_t * alarms)
{
    uint16 alarm_status;
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oam_alarm_verify(unit, port, flags, alarms));
    /*
     * Clear alarms
     */
    sal_memset(alarms, 0, sizeof(bcm_port_flexe_oam_alarm_t));
    /*
     * Get channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    /*
     * Get alarm status
     */
    SHR_IF_ERR_EXIT(flexe_core_oam_alarm_status_get(unit, client_channel, &alarm_status));
    /*
     * Parse the alarms
     */
    if (alarm_status != 0)
    {
        alarms->alarm_active = 1;

        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_CSF_LPI)
        {
            alarms->rx_base_csf_lpi = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_CS_LF)
        {
            alarms->rx_base_cs_lf = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_CS_RF)
        {
            alarms->rx_base_cs_rf = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_BASE_OAM_LOS)
        {
            alarms->base_oam_los = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_SDBIP)
        {
            alarms->rx_sdbip = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_BASE_CRC)
        {
            alarms->rx_base_crc = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_BASE_RDI)
        {
            alarms->rx_base_rdi = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_BASE_PERIOD_MISMATCH)
        {
            alarms->rx_base_period_mismatch = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_SDREI)
        {
            alarms->sdrei = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_SFBIP)
        {
            alarms->sfbip = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_SFREI)
        {
            alarms->sfrei = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_LF)
        {
            alarms->local_fault = 1;
        }
        if (alarm_status & FLEXE_CORE_OAM_ALARM_RX_RF)
        {
            alarms->remote_fault = 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify API for bcm_dnx_port_flexe_oam_control_get/set.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] type - OAM control type
 * \param [in] val - control val
 * \param [in] is_set - If this API is called by set API
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
dnx_port_flexe_oam_control_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_control_type_t type,
    uint32 *val,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_oam_over_flexe))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "OAM over FlexE is not supported on this device.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify type */
    SHR_RANGE_VERIFY(type, 0, bcmPortFlexeOamControlCount - 1, _SHR_E_PARAM, "type is not supported.\n");

    /** Verify gport type */
    if ((type == bcmPortSarOamControlSar2IlknBypassEnable) || (type == bcmPortSarOamControlIlkn2SarBypassEnable))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_sar_client_verify(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_client_verify(unit, port, 0));
    }
    /** Verify value */
    if (is_set)
    {
        switch (type)
        {
            case bcmPortFlexeOamControlRxBasePeriod:
            case bcmPortFlexeOamControlTxBasePeriod:
            {
                if (*val > bcmPortFlexeOamBasePeriod512K)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The value is not supported.\n");
                }
                break;
            }
            case bcmPortFlexeOamControlTxBaseEnable:
            case bcmPortFlexeOamControlRxBypassEnable:
            case bcmPortFlexeOamControlTxBypassEnable:
            case bcmPortFlexeOamControlInsertLocalFault:
            case bcmPortFlexeOamControlInsertRemoteFault:
            case bcmPortSarOamControlIlkn2SarBypassEnable:
            case bcmPortSarOamControlSar2IlknBypassEnable:
            case bcmPortFlexeOamControlInsertRdi:
            {
                if (*val > 1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The value is not supported.\n");
                }
                break;
            }
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE OAM control.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] type - OAM control type
 * \param [out] val - control val
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
bcm_dnx_port_flexe_oam_control_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_control_type_t type,
    uint32 *val)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oam_control_verify(unit, port, flags, type, val, 0));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    switch (type)
    {
        case bcmPortFlexeOamControlRxBasePeriod:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortFlexeOamControlTxBasePeriod:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_period_get(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlTxBaseEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_insert_enable_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlRxBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_bypass_enable_get(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortFlexeOamControlTxBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_bypass_enable_get(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlInsertLocalFault:
            SHR_IF_ERR_EXIT(flexe_core_oam_local_fault_insert_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlInsertRemoteFault:
            SHR_IF_ERR_EXIT(flexe_core_oam_remote_fault_insert_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8BlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_block_num_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8SetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_trigger_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8ClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_clear_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8BlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_block_num_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8SetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_trigger_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8ClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_clear_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiBlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_block_num_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiSetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_trigger_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_clear_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiBlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_block_num_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiSetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_trigger_thr_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_clear_thr_get(unit, client_channel, val));
            break;
        case bcmPortSarOamControlIlkn2SarBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_sar_oam_bypass_enable_get(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortSarOamControlSar2IlknBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_sar_oam_bypass_enable_get(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlInsertRdi:
            SHR_IF_ERR_EXIT(flexe_core_oam_rdi_insert_get(unit, client_channel, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported control type.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FlexE OAM control.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] type - OAM control type
 * \param [in] val - control val
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
bcm_dnx_port_flexe_oam_control_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_control_type_t type,
    uint32 val)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oam_control_verify(unit, port, flags, type, &val, 1));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    switch (type)
    {
        case bcmPortFlexeOamControlRxBasePeriod:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_period_set(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortFlexeOamControlTxBasePeriod:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_period_set(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlTxBaseEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_insert_enable_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlRxBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_bypass_enable_set(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortFlexeOamControlTxBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_oam_bypass_enable_set(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlInsertLocalFault:
            SHR_IF_ERR_EXIT(flexe_core_oam_local_fault_insert_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlInsertRemoteFault:
            SHR_IF_ERR_EXIT(flexe_core_oam_remote_fault_insert_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8BlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_block_num_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8SetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_trigger_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBip8ClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_alm_clear_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8BlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_block_num_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8SetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_trigger_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBip8ClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bip8_alm_clear_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiBlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_block_num_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiSetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_trigger_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSdBeiClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sd_bei_alm_clear_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiBlockNum:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_block_num_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiSetThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_trigger_thr_set(unit, client_channel, val));
            break;
        case bcmPortFlexeOamControlSfBeiClearThreshold:
            SHR_IF_ERR_EXIT(flexe_core_oam_sf_bei_alm_clear_thr_set(unit, client_channel, val));
            break;
        case bcmPortSarOamControlIlkn2SarBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_sar_oam_bypass_enable_set(unit, client_channel, BCM_PORT_FLEXE_RX, val));
            break;
        case bcmPortSarOamControlSar2IlknBypassEnable:
            SHR_IF_ERR_EXIT(flexe_core_sar_oam_bypass_enable_set(unit, client_channel, BCM_PORT_FLEXE_TX, val));
            break;
        case bcmPortFlexeOamControlInsertRdi:
            SHR_IF_ERR_EXIT(flexe_core_oam_rdi_insert_set(unit, client_channel, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported control type.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify API for bcm_dnx_port_flexe_oam_stat_get.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] stat - stat type
 * \param [in] val - counter value
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
dnx_port_flexe_oam_stat_verify(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_stat_t stat,
    uint64 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_oam_over_flexe))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "OAM over FlexE is not supported on this device.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify gport type */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_client_verify(unit, port, 0));

    /** Verify type */
    SHR_RANGE_VERIFY(stat, 0, bcmPortFlexeBaseOamPacketCount, _SHR_E_PARAM, "stat type is not supported.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE OAM stat.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - FlexE client logical port
 * \param [in] flags - flags
 * \param [in] stat - stat type
 * \param [out] val - counter value
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
bcm_dnx_port_flexe_oam_stat_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_flexe_oam_stat_t stat,
    uint64 *val)
{
    int client_channel;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_oam_stat_verify(unit, port, flags, stat, val));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    switch (stat)
    {
        case bcmbcmPortFlexeOamStatBip8:
            SHR_IF_ERR_EXIT(flexe_core_oam_bip8_counter_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamStatBei:
            SHR_IF_ERR_EXIT(flexe_core_oam_bei_counter_get(unit, client_channel, val));
            break;
        case bcmPortFlexeOamPacketCount:
            SHR_IF_ERR_EXIT(flexe_core_oam_pkt_count_get(unit, client_channel, val));
            break;
        case bcmPortFlexeBaseOamPacketCount:
            SHR_IF_ERR_EXIT(flexe_core_oam_base_pkt_count_get(unit, client_channel, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported stat type.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Select the ILU tx source, from FEU or ESB
 *    If FlexE mode is distributed, the Tx source for
 *    ILKN core 1 is from FEU
 *
 * \param [in] unit - chip unit id
 * \param [in] source - tx source, egq or feu
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
dnx_flexe_ilu_tx_src_config(
    int unit,
    dbal_enum_value_field_ilu_tx_src_e source)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_TX_SRC_CONFIG, &entry_handle_id));
    /*
     * set key fields, only ILKN core 1 is used for FlexE
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, 1);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOURCE, INST_SINGLE, source);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the TDM loopback in IRE.
 *    If FlexE mode is enabled, the TDM is loopbacked
 *    in IRE and sent to FEU.
 *
 * \param [in] unit - chip unit id
 * \param [in] enable - enable or disable
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
dnx_flexe_tdm_ire_loopback_config(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_TDM_IRE_LPBK_CONFIG, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ILU slicing mode.
 *
 * \param [in] unit - chip unit id
 * \param [in] mode - slicing mode, enhanced or naive
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
dnx_flexe_ilu_slicing_mode_config(
    int unit,
    dbal_enum_value_field_ilu_slicing_mode_e mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILU_IBE_CONFIG, &entry_handle_id));
    /*
     * set key fields, only ILKN core 1 is used for FlexE
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_CORE_ID, 1);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLICING_MODE, INST_SINGLE, mode);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the ILU credit mask.
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_id - ilkn index
 * \param [in] value - mask value, 0 or 1
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
dnx_flexe_ilu_credit_mask_set(
    int unit,
    int ilkn_id,
    int value)
{
    int ilkn_nif_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure the credit mask
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_EGQ_CREDIT_MASK, &entry_handle_id));
    /*
     * set key fields
     */
    ilkn_nif_id = dnx_data_nif.phys.nof_phys_get(unit) + ilkn_id;
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_ID, ilkn_nif_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_CREDIT_MASK, INST_SINGLE, value);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure default LSS target for L2 path.
 *
 * \param [in] unit - chip unit id
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
dnx_flexe_lss_default_set(
    int unit)
{
    uint32 entry_handle_id;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_RX_CTRL, &entry_handle_id));
    for (i = 0; i < dnx_data_nif.mac_client.nof_clients_get(unit); ++i)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, i);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ALARM_TARGET_TMC, INST_SINGLE, 0x3f);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_L1_RX_CTRL, entry_handle_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));
    for (i = 0; i < dnx_data_nif.phys.nof_phys_get(unit); ++i)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, i);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_ALARM_SOURCE_CLIENT, INST_SINGLE, 0x7f);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TARGET_CLIENT, INST_SINGLE, 0x7f);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Power down the blocks inside Framer core.
 *
 * \param [in] unit - chip unit id
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
dnx_framer_power_down_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_POWER_DOWN, &entry_handle_id));
    /*
     * Power down OTN blocks if OTN is not supported
     */
    if (!dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OTN_POWER_DOWN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Power down common blocks if Framer is not supported
     */
    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMON_POWER_DOWN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Power down the blocks inside Framer Wrapper.
 *
 * \param [in] unit - chip unit id
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
dnx_framer_wrapper_power_down_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_WRAPPER_POWER_DOWN, &entry_handle_id));
    /*
     * Power down W40 if OTN is not supported
     */
    if (!dnx_data_nif.otn.feature_get(unit, dnx_data_nif_otn_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_W40_POWER_DOWN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Power down WA/WBC blocks if Framer is not supported
     */
    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WBC_POWER_DOWN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WA_POWER_DOWN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Power down blockes related to WBC if they are not used
         */
        if (!dnx_data_nif.wbc.feature_get(unit, dnx_data_nif_wbc_is_used))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WBC_POWER_DOWN, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init modules related to Framer.
 *
 * \param [in] unit - chip unit id
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
dnx_framer_init(
    int unit)
{
    int tdm_ire_lpbk_en = 0;
    dbal_enum_value_field_ilu_slicing_mode_e slicing_mode;
    dbal_enum_value_field_ilu_tx_src_e tx_source;
    int distributed_ilu_if_id_base, i;
    uint8 is_active;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Power down the unused blocks
     */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_power_down_supported))
    {
        SHR_IF_ERR_EXIT(dnx_framer_power_down_set(unit));
        SHR_IF_ERR_EXIT(dnx_framer_wrapper_power_down_set(unit));
    }
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
        {
            /*
             * If there is clock dependency, need to active the FlexE core blocks
             * when the clock is active
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &is_active));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_blocks_enable_set(unit, is_active));
        }

        if (!sw_state_is_warm_boot(unit) &&
            dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support))
        {
            if (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISABLED)
            {
                tdm_ire_lpbk_en = 0;
                slicing_mode = DBAL_ENUM_FVAL_ILU_SLICING_MODE_ENHANCED;
                tx_source = DBAL_ENUM_FVAL_ILU_TX_SRC_EGQ;
            }
            else if (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISTRIBUTED)
            {
                tdm_ire_lpbk_en = 1;
                slicing_mode = DBAL_ENUM_FVAL_ILU_SLICING_MODE_NAIVE;
                tx_source = DBAL_ENUM_FVAL_ILU_TX_SRC_FEU;

                /** Mask EGQ credit for ILKN #1 since it is connecting to FEU in distributed mode */
                distributed_ilu_if_id_base =
                    dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) * dnx_data_nif.flexe.distributed_ilu_id_get(unit);
                for (i = distributed_ilu_if_id_base;
                     i < distributed_ilu_if_id_base + dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); ++i)
                {
                    SHR_IF_ERR_EXIT(dnx_flexe_ilu_credit_mask_set(unit, i, 1));
                }
            }
            else
            {
                /*
                 * Centralized mode
                 */
                tdm_ire_lpbk_en = 1;
                slicing_mode = DBAL_ENUM_FVAL_ILU_SLICING_MODE_ENHANCED;
                tx_source = DBAL_ENUM_FVAL_ILU_TX_SRC_EGQ;
            }

            SHR_IF_ERR_EXIT(dnx_flexe_ilu_tx_src_config(unit, tx_source));
            SHR_IF_ERR_EXIT(dnx_flexe_ilu_slicing_mode_config(unit, slicing_mode));
            SHR_IF_ERR_EXIT(dnx_flexe_tdm_ire_loopback_config(unit, tdm_ire_lpbk_en));
            SHR_IF_ERR_EXIT(dnx_flexe_lss_default_set(unit));
        }
        if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
        {
            SHR_IF_ERR_EXIT(flexe_core_init(unit));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Deinit Framer blocks.
 *
 * \param [in] unit - chip unit id
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
dnx_framer_deinit(
    int unit)
{
    uint8 is_active;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
    {
        if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_active_get(unit, &is_active));
            if (is_active)
            {
                SHR_IF_ERR_EXIT(flexe_core_deinit(unit));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the OAM alarm collection time step
 *
 * \param [in] unit - chip unit id
 * \param [in] step - timer step
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
dnx_flexe_oam_alarm_collection_timer_step_set(
    int unit,
    int step)
{
    SHR_FUNC_INIT_VARS(unit);

    /** The register value is 0-based. The FlexE driver will verify the value */
    SHR_IF_ERR_EXIT(flexe_core_oam_alarm_collection_timer_step_set(unit, step - 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure number of steps for OAM alarm
 *    collection period
 *
 * \param [in] unit - chip unit id
 * \param [in] step_count - step count
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
dnx_flexe_oam_alarm_collection_step_count_set(
    int unit,
    int step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** The register value is 0-based. The FlexE driver will verify the value */
    SHR_IF_ERR_EXIT(flexe_core_oam_alarm_collection_step_count_set(unit, step_count - 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the OAM alarm collection time step
 *
 * \param [in] unit - chip unit id
 * \param [out] step - timer step
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
dnx_flexe_oam_alarm_collection_timer_step_get(
    int unit,
    int *step)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_core_oam_alarm_collection_timer_step_get(unit, step));
    *step = *step + 1;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get number of steps for OAM alarm
 *    collection period
 *
 * \param [in] unit - chip unit id
 * \param [out] step_count - step count
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
dnx_flexe_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_core_oam_alarm_collection_step_count_get(unit, step_count));
    *step_count = *step_count + 1;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for dnx_flexe_sar_cell_mode_set/get API.
 *
 * \param [in] unit - chip unit id
 * \param [in] cell_mode - FlexE SAR cell mode
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_sar_cell_mode_verify(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t * cell_mode,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }

    /** Check if per channel SAR cell mode is supported */
    if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_per_channel_cell_mode))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Please configure SAR cell mode per channel by bcm_port_control_set.\r\n");
    }

    /** Check if FlexE mode is distributed */
    if (dnx_data_nif.flexe.flexe_mode_get(unit) != DNX_FLEXE_MODE_DISTRIBUTED)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "The SAR cell mode is configurable only when FlexE mode is distributed.\r\n");
    }

    /** Null checks */
    SHR_NULL_CHECK(cell_mode, _SHR_E_PARAM, "cell_mode");

    /** Verify the cell mode */
    if (is_set)
    {
        SHR_RANGE_VERIFY(*cell_mode, 0, bcmSwitchFlexeSarCellModeCount - 1, _SHR_E_PARAM,
                         "cell mode %d is not supported.\n", *cell_mode);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the ILKN burst in FEU
 *
 */
static int
dnx_flexe_ilkn_burst_config_set(
    int unit,
    int burst_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_ILKN_BURST_CTRL, &entry_handle_id));
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_TX_BURST_SIZE, INST_SINGLE, burst_size - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_MIN_BURST_SIZE, INST_SINGLE, burst_size - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_EN, INST_SINGLE, 1);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the SAR Cell mode:
 *    28*66b or 29*66b
 *
 * \param [in] unit - chip unit id
 * \param [in] cell_mode - SAR cell mode
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
dnx_flexe_sar_cell_mode_set(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t cell_mode)
{
    int burst_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_sar_cell_mode_verify(unit, &cell_mode, 1));

    /** Configure SAR cell mode in FlexE core */
    SHR_IF_ERR_EXIT(flexe_core_sar_cell_mode_set(unit, cell_mode));

    /** Configure the ILKN burst size in FEU */
    if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_feu_ilkn_burst_size_support))
    {
        burst_size = dnx_data_nif.sar_client.sar_cell_mode_to_size_get(unit, cell_mode)->packet_size;
        SHR_IF_ERR_EXIT(dnx_flexe_ilkn_burst_config_set(unit, burst_size));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the SAR Cell mode:
 *    28*66b or 29*66b
 *
 * \param [in] unit - chip unit id
 * \param [out] cell_mode - SAR cell mode
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
dnx_flexe_sar_cell_mode_get(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t * cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_sar_cell_mode_verify(unit, cell_mode, 0));

    /** Configure SAR cell mode in FlexE core */
    SHR_IF_ERR_EXIT(flexe_core_sar_cell_mode_get(unit, (int *) cell_mode));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify function for dnx_flexe_sar_cell_mode_per_port_set/get API.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - SAR logical port id
 * \param [in] cell_mode - FlexE SAR cell mode
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_sar_cell_mode_per_port_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_sar_cell_mode_t * cell_mode,
    int is_set)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }

    /** Check if per channel SAR cell mode is supported */
    if (!dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_per_channel_cell_mode))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Per channel SAR cell mode is not support.\r\n");
    }

    /** Null checks */
    SHR_NULL_CHECK(cell_mode, _SHR_E_PARAM, "cell_mode");

    /** Check port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Only SAR port is supported.\r\n");
    }

    /** Verify the cell mode */
    if (is_set)
    {
        SHR_RANGE_VERIFY(*cell_mode, 0, bcmPortSarCellCount - 1, _SHR_E_PARAM, "cell mode %d is not supported.\n",
                         *cell_mode);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure per port SAR Cell mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - SAR logical port id
 * \param [in] type - port control type
 * \param [in] cell_mode - SAR cell mode
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
dnx_flexe_sar_cell_mode_per_port_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    bcm_port_control_sar_cell_mode_t cell_mode)
{
    int is_rx, client_channel, has_speed;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_sar_cell_mode_per_port_verify(unit, port, &cell_mode, 1));

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    /*
     * Update DB
     */
    is_rx = (type == bcmPortControlSarToIngressCellMode);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_cell_mode_set(unit, client_channel, is_rx, cell_mode));
    /*
     * Check if speed is configured
     */
    flags = is_rx ? DNX_ALGO_PORT_SPEED_F_RX : DNX_ALGO_PORT_SPEED_F_TX;
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, flags, &has_speed));
    if (has_speed)
    {
        /*
         * Configure SAR cell mode in FlexE core
         */
        SHR_IF_ERR_EXIT(flexe_core_sar_cell_mode_per_channel_set(unit, client_channel, is_rx, cell_mode));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get per port SAR Cell mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - SAR logical port id
 * \param [in] type - port control type
 * \param [out] cell_mode - SAR cell mode
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
dnx_flexe_sar_cell_mode_per_port_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    bcm_port_control_sar_cell_mode_t * cell_mode)
{
    int is_rx, client_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_sar_cell_mode_per_port_verify(unit, port, cell_mode, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    is_rx = (type == bcmPortControlSarToIngressCellMode);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_sar_cell_mode_get(unit, client_channel, is_rx, (int *) cell_mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify Flexe oh/oam MAC set/get APIs.
 *
 * \param [in] unit - chip unit id
 * \param [in] val - MAC hi or lo
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_oh_oam_mac_verify(
    int unit,
    int *val,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify the cell mode */
    if (is_set)
    {
        SHR_MASK_VERIFY(*val, 0xFFFFFF, _SHR_E_PARAM, "MAC value is too big.\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify Flexe 1588 header.
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [in] val - MAC hi or lo
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_1588_header_verify(
    int unit,
    bcm_switch_control_t type,
    int *val,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify the cell mode */
    if (is_set)
    {
        switch (type)
        {
            case bcmSwitchFlexe1588RxDestMacOui:
            case bcmSwitchFlexe1588RxDestMacNonOui:
            case bcmSwitchFlexe1588RxSrcMacOui:
            case bcmSwitchFlexe1588RxSrcMacNonOui:
            case bcmSwitchFlexe1588TxDestMacOui:
            case bcmSwitchFlexe1588TxDestMacNonOui:
            case bcmSwitchFlexe1588TxSrcMacOui:
            case bcmSwitchFlexe1588TxSrcMacNonOui:
            case bcmSwitchFlexe1588OverEthRxUnicastDestMacOui:
            case bcmSwitchFlexe1588OverEthRxUnicastDestMacNonOui:
            case bcmSwitchFlexe1588OverEthTxUnicastDestMacOui:
            case bcmSwitchFlexe1588OverEthTxUnicastDestMacNonOui:
            {
                SHR_MASK_VERIFY(*val, 0xFFFFFF, _SHR_E_PARAM, "MAC value is too big.\n");
                break;
            }
            case bcmSwitchFlexe1588OverIpv4RxUnicastDestIpHigh:
            case bcmSwitchFlexe1588OverIpv4RxUnicastDestIpLow:
            case bcmSwitchFlexe1588OverIpv4TxUnicastDestIpHigh:
            case bcmSwitchFlexe1588OverIpv4TxUnicastDestIpLow:
            {
                SHR_MASK_VERIFY(*val, 0xFFFF, _SHR_E_PARAM, "IP value is too big.\n");
                break;
            }
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert switch control types to flags for 1588 packets.
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control types
 * \param [out] flags - flags
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
dnx_flexe_1588_header_switch_control_to_flags_get(
    int unit,
    bcm_switch_control_t type,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmSwitchFlexe1588RxDestMacOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_DEST_MAC_HI;
    }
    else if (type == bcmSwitchFlexe1588RxDestMacNonOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_DEST_MAC_LO;
    }
    else if (type == bcmSwitchFlexe1588RxSrcMacOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_SRC_MAC_HI;
    }
    else if (type == bcmSwitchFlexe1588RxSrcMacNonOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_SRC_MAC_LO;
    }
    else if (type == bcmSwitchFlexe1588TxDestMacOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_DEST_MAC_HI;
    }
    else if (type == bcmSwitchFlexe1588TxDestMacNonOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_DEST_MAC_LO;
    }
    else if (type == bcmSwitchFlexe1588TxSrcMacOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_SRC_MAC_HI;
    }
    else if (type == bcmSwitchFlexe1588TxSrcMacNonOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_SRC_MAC_LO;
    }
    else if (type == bcmSwitchFlexe1588OverEthRxUnicastDestMacOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI;
    }
    else if (type == bcmSwitchFlexe1588OverEthRxUnicastDestMacNonOui)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO;
    }
    else if (type == bcmSwitchFlexe1588OverEthTxUnicastDestMacOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_OVER_ETH_UNI_DA_HI;
    }
    else if (type == bcmSwitchFlexe1588OverEthTxUnicastDestMacNonOui)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_OVER_ETH_UNI_DA_LO;
    }
    else if (type == bcmSwitchFlexe1588OverIpv4RxUnicastDestIpHigh)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI;
    }
    else if (type == bcmSwitchFlexe1588OverIpv4RxUnicastDestIpLow)
    {
        *flags = FLEXE_CORE_1588_RX | FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_LO;
    }
    else if (type == bcmSwitchFlexe1588OverIpv4TxUnicastDestIpHigh)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_HI;
    }
    else if (type == bcmSwitchFlexe1588OverIpv4TxUnicastDestIpLow)
    {
        *flags = FLEXE_CORE_1588_TX | FLEXE_CORE_1588_OVER_IPV4_UNI_DIP_LO;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported switch control type.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set FlexE 1588 packet header
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [in] val - value
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
dnx_flexe_1588_pkt_header_set(
    int unit,
    bcm_switch_control_t type,
    int val)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_1588_header_verify(unit, type, &val, 1));

    SHR_IF_ERR_EXIT(dnx_flexe_1588_header_switch_control_to_flags_get(unit, type, &flags));
    SHR_IF_ERR_EXIT(flexe_core_1588_pkt_header_set(unit, flags, val));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE 1588 packet header
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [out] val - value
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
dnx_flexe_1588_pkt_header_get(
    int unit,
    bcm_switch_control_t type,
    int *val)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_1588_header_verify(unit, type, val, 0));

    SHR_IF_ERR_EXIT(dnx_flexe_1588_header_switch_control_to_flags_get(unit, type, &flags));
    SHR_IF_ERR_EXIT(flexe_core_1588_pkt_header_get(unit, flags, (uint32 *) val));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Convert switch control types to flags for oh/oam packets.
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control types
 * \param [out] flags - flags
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
dnx_flexe_oh_oam_mac_switch_control_to_flags_get(
    int unit,
    bcm_switch_control_t type,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmSwitchFlexeOhOamPktDestMacOui)
    {
        *flags = FLEXE_CORE_OH_OAM_DEST_MAC_HI;
    }
    else if (type == bcmSwitchFlexeOhOamPktDestMacNonOui)
    {
        *flags = FLEXE_CORE_OH_OAM_DEST_MAC_LO;
    }
    else if (type == bcmSwitchFlexeOhOamPktSrcMacOui)
    {
        *flags = FLEXE_CORE_OH_OAM_SRC_MAC_HI;
    }
    else if (type == bcmSwitchFlexeOhOamPktSrcMacNonOui)
    {
        *flags = FLEXE_CORE_OH_OAM_SRC_MAC_LO;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported switch control type.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set FlexE OH/OAM packet MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [in] val - value
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
dnx_flexe_oh_oam_pkt_mac_set(
    int unit,
    bcm_switch_control_t type,
    int val)
{
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_oh_oam_mac_verify(unit, &val, 1));

    SHR_IF_ERR_EXIT(dnx_flexe_oh_oam_mac_switch_control_to_flags_get(unit, type, &flags));
    SHR_IF_ERR_EXIT(flexe_core_oh_oam_pkt_mac_set(unit, flags, val));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get FlexE OH/OAM packet MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [out] val - value
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
dnx_flexe_oh_oam_pkt_mac_get(
    int unit,
    bcm_switch_control_t type,
    int *val)
{
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_oh_oam_mac_verify(unit, val, 0));

    SHR_IF_ERR_EXIT(dnx_flexe_oh_oam_mac_switch_control_to_flags_get(unit, type, &flags));
    SHR_IF_ERR_EXIT(flexe_core_oh_oam_pkt_mac_get(unit, flags, (uint32 *) val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameter for dnx_flexe_bas_oam_to_ext_processor_set/get.
 *
 * \param [in] unit - chip unit id
 * \param [in] val - MAC hi or lo
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_bas_oam_to_ext_processor_verify(
    int unit,
    int *val,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** Verify the value */
    if (is_set)
    {
        SHR_BOOL_VERIFY(*val, _SHR_E_PARAM, "Parameter error when enabling FlexE BAS OAM to ext\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/disable flexe bas oam to external
 *          processor
 *
 * \param [in] unit - chip unit id
 * \param [in] enable - enable or disable
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
dnx_flexe_bas_oam_to_ext_processor_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_bas_oam_to_ext_processor_verify(unit, &enable, 1));
    SHR_IF_ERR_EXIT(flexe_core_bas_oam_to_ext_processor_set(unit, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - get flexe bas oam to external
 *          processor enable status
 *
 * \param [in] unit - chip unit id
 * \param [out] enable - enable or disable
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
dnx_flexe_bas_oam_to_ext_processor_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_bas_oam_to_ext_processor_verify(unit, enable, 0));
    SHR_IF_ERR_EXIT(flexe_core_bas_oam_to_ext_processor_get(unit, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the parameters for
 *    dnx_port_flexe_sar_jitter_tolerance_level_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - configured values
 * \param [in] is_set - if verify API is called by _set API
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_sar_jitter_tolerance_level_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    int max_tolerance_level;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /** Check if it is sar client */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The port type should be SAR client.\r\n");
    }

    /** Verify the value */
    if (is_set)
    {
        max_tolerance_level = dnx_data_nif.sar_client.sar_jitter_tolerance_max_level_get(unit);
        SHR_RANGE_VERIFY(*value, 0, max_tolerance_level, _SHR_E_PARAM, "The tolerance level is out of range.\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the tolerance level for SAR client
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - configured values
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_sar_jitter_tolerance_level_set(
    int unit,
    bcm_port_t port,
    int value)
{
    int client_channel;
    flexe_core_sar_jitter_fifo_level_t fifo_level;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_sar_jitter_tolerance_level_verify(unit, port, &value, 1));

    SHR_IF_ERR_EXIT(flexe_core_sar_jitter_fifo_level_t_init(unit, &fifo_level));
    /*
     * For each tolerance level, there is default high/low level
     */
    fifo_level.high_level = dnx_data_nif.sar_client.sar_jitter_fifo_levels_get(unit, value)->high_level;
    fifo_level.low_level = dnx_data_nif.sar_client.sar_jitter_fifo_levels_get(unit, value)->low_level;
    fifo_level.tolerance_level = value;
    /*
     * Configure levels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(flexe_core_sar_jitter_fifo_level_set(unit, client_channel, &fifo_level));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the jitter tolerance level for SAR client
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - configured values
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_sar_jitter_tolerance_level_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    int client_channel;
    flexe_core_sar_jitter_fifo_level_t fifo_level;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_sar_jitter_tolerance_level_verify(unit, port, value, 0));
    /*
     * Get levels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(flexe_core_sar_jitter_fifo_level_get(unit, client_channel, &fifo_level));
    *value = fifo_level.tolerance_level;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the parameters for
 *    dnx_port_flexe_deskew_alm_trigger_lf_set/get
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - configured values
 * \param [in] is_set - if verify API is called by _set API
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_deskew_alm_trigger_lf_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /** Check if it is flexe group gport */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, port));

    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(port);
    /** Check if the flexe group is valid */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));

    /** Verify the value */
    if (is_set)
    {
        SHR_BOOL_VERIFY(*value, _SHR_E_PARAM, "The configure value should be either 0 or 1.\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/disable LF insertion under FlexE
 *    group deskew alarm
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - configured values
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_deskew_alm_trigger_lf_set(
    int unit,
    bcm_port_t port,
    int value)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_deskew_alm_trigger_lf_verify(unit, port, &value, 1));
    /*
     * Configure value
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(port);
    SHR_IF_ERR_EXIT(flexe_core_deskew_alarm_trigger_lf_set(unit, group_index, value));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get enable status for LF insertion under FlexE
 *    group deskew alarm
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - configured values
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_deskew_alm_trigger_lf_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_deskew_alm_trigger_lf_verify(unit, port, value, 0));
    /*
     * Get value
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(port);
    SHR_IF_ERR_EXIT(flexe_core_deskew_alarm_trigger_lf_get(unit, group_index, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the port control for FlexE/MAC/SAR clients
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - port control type
 * \param [in] value - configured values
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
dnx_port_flexe_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlSarJitterToleranceLevel:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_sar_jitter_tolerance_level_set(unit, port, value));
            break;
        }
        case bcmPortControlFlexeDeskewAlmTriggerLocalFault:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_deskew_alm_trigger_lf_set(unit, port, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_control_t type %d is not supported\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the port control for FlexE/MAC/SAR clients
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - port control type
 * \param [out] value - value
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
dnx_port_flexe_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlSarJitterToleranceLevel:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_sar_jitter_tolerance_level_get(unit, port, value));
            break;
        }
        case bcmPortControlFlexeDeskewAlmTriggerLocalFault:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_deskew_alm_trigger_lf_get(unit, port, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_control_t type %d is not supported\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify parameter for dnx_flexe_oam_switch_control_set/get.
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [in] value - value
 * \param [in] is_set - If this API is called by SET API
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
dnx_flexe_oam_switch_control_verify(
    int unit,
    bcm_switch_control_t type,
    int *value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    /** Null checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /** Verify the value */
    switch (type)
    {
        case bcmSwitchFlexeOamBasBipRecalculationEnable:
        case bcmSwitchFlexeOamBasPeriodMismatchTriggerLosAlarm:
        {
            if (is_set)
            {
                SHR_BOOL_VERIFY(*value, _SHR_E_PARAM, "Value should be 0 or 1.\n");
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_switch_control_t type %d is not supported\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set switch control for FlexE OAM
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [in] value - value
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
dnx_flexe_oam_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_oam_switch_control_verify(unit, type, &value, 1));

    switch (type)
    {
        case bcmSwitchFlexeOamBasBipRecalculationEnable:
        {
            SHR_IF_ERR_EXIT(flexe_core_oam_bas_bip_recal_enable_set(unit, value));
            break;
        }
        case bcmSwitchFlexeOamBasPeriodMismatchTriggerLosAlarm:
        {
            SHR_IF_ERR_EXIT(flexe_core_oam_bas_period_mismatch_trigger_los_alarm_set(unit, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_switch_control_t type %d is not supported\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get switch control for FlexE OAM
 *
 * \param [in] unit - chip unit id
 * \param [in] type - switch control type
 * \param [out] value - value
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
dnx_flexe_oam_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flexe_oam_switch_control_verify(unit, type, value, 0));

    switch (type)
    {
        case bcmSwitchFlexeOamBasBipRecalculationEnable:
        {
            SHR_IF_ERR_EXIT(flexe_core_oam_bas_bip_recal_enable_get(unit, value));
            break;
        }
        case bcmSwitchFlexeOamBasPeriodMismatchTriggerLosAlarm:
        {
            SHR_IF_ERR_EXIT(flexe_core_oam_bas_period_mismatch_trigger_los_alarm_get(unit, value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_switch_control_t type %d is not supported\n", type);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify parameters for bcm_dnx_port_framer_channel_free/reserve
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] module_id - Framer module ID
 * \param [in] channel - channel ID
 * \param [in] is_free - If the API is called by Free API.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_framer_channel_verify(
    int unit,
    uint32 flags,
    bcm_port_framer_module_t module_id,
    int *channel,
    int is_free)
{
    int nof_channels;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if FlexE mode is enable */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    SHR_NULL_CHECK(channel, _SHR_E_PARAM, "channel is NULL.\r\n");

    /** Verify flags, Rx and Tx flags are not supported currently */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify values */
    switch (module_id)
    {
        case bcmPortFramerModuleNormalFlexe:
        case bcmPortFramerModuleVirtualFlexe:
        {
            if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_connect_to_66bswitch))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "This module doesn't support exposing channels.\n");
            }
            break;
        }
        case bcmPortFramerModuleMac1:
        case bcmPortFramerModuleMac2:
        {
            if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "This module doesn't support exposing channels.\n");
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported module ID.\n");
        }
    }
    if (is_free)
    {
        nof_channels = dnx_data_nif.flexe.nof_clients_get(unit);
        SHR_RANGE_VERIFY(*channel, 0, nof_channels - 1, _SHR_E_PARAM, "channel ID is out of range.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Free the channel in specific Framer module.
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] module_id - Framer module ID
 * \param [in] channel - channel ID to free
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
bcm_dnx_port_framer_channel_free(
    int unit,
    uint32 flags,
    bcm_port_framer_module_t module_id,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_framer_channel_verify(unit, flags, module_id, &channel, 1));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_shared_channel_free(unit, module_id, channel));
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Reserve one channel in specific Framer module.
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - flags
 * \param [in] module_id - Framer module ID
 * \param [out] channel - channel ID to reserve
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
bcm_dnx_port_framer_channel_reserve(
    int unit,
    uint32 flags,
    bcm_port_framer_module_t module_id,
    int *channel)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_framer_channel_verify(unit, flags, module_id, channel, 0));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_shared_channel_alloc(unit, module_id, channel));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Called by dnx_port_remove_verify() to ensure
 *          no time slot occupied by the client to be removed
 */
static shr_error_e
dnx_port_flexe_client_remove_slots_verify(
    int unit,
    bcm_port_t logical_port)
{
    int group_index, count;
    uint32 directions[] = { BCM_PORT_FLEXE_TX, BCM_PORT_FLEXE_RX };

    SHR_FUNC_INIT_VARS(unit);

    for (count = 0; count < COUNTOF(directions); ++count)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_client_to_group_index_get(unit, logical_port, directions[count], &group_index));
        if (group_index != -1)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Please call bcm_port_flexe_group_cal_slots_set() to remove the time slot occupied by the client(%d) first!\n",
                         logical_port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Called by dnx_port_remove_verify() to ensure
 *          no flexe flow configured by the client to be removed
 *          Called by dnx_port_l1_eth_1588_enable_verify() to ensure
 *          no flexe flow configured when enabling 1588 on L1 ETH ports
 */
static shr_error_e
dnx_port_framer_port_flow_exist_verify(
    int unit,
    bcm_port_t logical_port,
    uint32 flags)
{
    int nof_dests;
    flexe_core_66bswitch_channel_info_t src_info;
    flexe_core_66bswitch_channel_info_t dest_info[DNX_DATA_MAX_NIF_FRAMER_B66SWITCH_BACKUP_CHANNELS + 1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify flow
     */
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                        (unit, logical_port, BCM_PORT_FLEXE_RX, 0, &src_info.block_id, &src_info.channel));
        SHR_IF_ERR_EXIT(flexe_core_66b_switch_get(unit, &src_info, dest_info, &nof_dests));
        if (nof_dests > 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "The port %d has configured flexe flow. Please clear the flow!\n", logical_port);
        }
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                        (unit, logical_port, BCM_PORT_FLEXE_TX, 0, &src_info.block_id, &src_info.channel));
        SHR_IF_ERR_EXIT(flexe_core_66b_switch_reverse_get(unit, &src_info, dest_info, &nof_dests));
        if (nof_dests > 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "The port %d has configured flexe flow. Please clear the flow!\n", logical_port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Called by dnx_port_remove_verify() to ensure
 *          no flexe group is using the phy port to be removed
 */
static shr_error_e
dnx_port_flexe_phy_remove_verify(
    int unit,
    bcm_port_t logical_port)
{
    bcm_pbmp_t phy_ports;
    int group, valid;
    SHR_FUNC_INIT_VARS(unit);

    for (group = 0; group < dnx_data_nif.flexe.nof_groups_get(unit); ++group)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, group, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group, &phy_ports));
            if (_SHR_PBMP_MEMBER(phy_ports, logical_port))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "The port(%d) is occupied by the flexe group(%d), cannot remove it!\n", logical_port,
                             group);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify Framer port when removing port.
 */
int
dnx_port_framer_port_remove_verify(
    int unit,
    bcm_port_t logical_port)
{
    uint32 flags = 0;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_CLIENT(unit, port_info, FALSE)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE))
    {
        /** Check if the flow exists when removing ports */
        if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            flags = (BCM_PORT_FLEXE_RX | BCM_PORT_FLEXE_TX);
            SHR_IF_ERR_EXIT(dnx_port_framer_port_flow_exist_verify(unit, logical_port, flags));
        }
        /** Check if there's time slot allocation for the flexe client to be removed */
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_client_remove_slots_verify(unit, logical_port));
        }
    }

    /** Check if there's flexe group occupying the phy port to be removed */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_port_flexe_phy_remove_verify(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the parameters for
 *    bcm_dnx_port_flexe_group_ghao_action_set
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group gport
 * \param [in] flags - flags, rx or tx
 * \param [in] action - G.hao actions
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_group_ghao_action_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_ghao_action_t action)
{
    int group_index;
    uint32 supported_flags;
    bcm_port_flexe_group_cal_t active_cal;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\r\n");
    }
    if (!dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "G.hao is not supported on the device.\r\n");
    }
    /*
     * Check port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_port_verify(unit, gport));
    /*
     * The FlexE group should be valid
     */
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_valid_verify(unit, group_index, 0));
    /*
     * Verify flags
     */
    supported_flags = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");
    /*
     * Verify actions
     */
    SHR_RANGE_VERIFY(action, 0, bcmPortFlexeGHaoNofActions - 1, _SHR_E_PARAM, "Action is out of range.\r\n");
    /*
     * Verify if new action is valid
     */
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_flexe_group_cal_active_get(unit, gport, BCM_PORT_FLEXE_TX, &active_cal));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_action_verify(unit, group_index, 0, active_cal, action));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_flexe_group_cal_active_get(unit, gport, BCM_PORT_FLEXE_RX, &active_cal));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_action_verify(unit, group_index, 1, active_cal, action));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the calendar switch action,
 *    from B to A or from A to B
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] flags - flags, rx or tx
 * \param [in] action - G.hao actions
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_group_ghao_cal_switch_action_set(
    int unit,
    bcm_gport_t group_index,
    uint32 flags,
    bcm_port_flexe_group_ghao_action_t action)
{
    int client_channel, speed, is_rx, count;
    uint32 slots_direction[] = { BCM_PORT_FLEXE_TX, BCM_PORT_FLEXE_RX };
    bcm_port_flexe_group_cal_t target_cal_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Preform G.hao for each direction
     */
    target_cal_id = (action == bcmPortFlexeGHaoCalAToB) ? bcmPortFlexeGroupCalB : bcmPortFlexeGroupCalA;
    for (count = 0; count < sizeof(slots_direction) / sizeof(uint32); ++count)
    {
        if (!(flags & slots_direction[count]))
        {
            /*
             * If the direction is not configured
             */
            continue;
        }
        is_rx = (slots_direction[count] == BCM_PORT_FLEXE_RX) ? TRUE : FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_flexe_cal_compare
                        (unit, group_index, is_rx, target_cal_id, &client_channel, &speed));
        SHR_IF_ERR_EXIT(flexe_core_flexe_client_ghao_cal_switch
                        (unit, group_index, target_cal_id, client_channel, slots_direction[count], speed));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Complete the G.hao speed switching
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] flags - flags, rx or tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_group_ghao_complete(
    int unit,
    bcm_gport_t group_index,
    uint32 flags)
{
    int client_channel, speed, is_rx, count;
    uint32 slots_direction[] = { BCM_PORT_FLEXE_TX, BCM_PORT_FLEXE_RX };
    bcm_port_flexe_group_ghao_action_t curr_action;
    bcm_port_flexe_group_cal_t target_cal_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Preform G.hao for each direction
     */
    for (count = 0; count < sizeof(slots_direction) / sizeof(uint32); ++count)
    {
        if (!(flags & slots_direction[count]))
        {
            /*
             * If the direction is not configured
             */
            continue;
        }
        is_rx = (slots_direction[count] == BCM_PORT_FLEXE_RX) ? TRUE : FALSE;
        /*
         * Get current action
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_action_get(unit, group_index, is_rx, &curr_action));
        target_cal_id = (curr_action == bcmPortFlexeGHaoCalAToB) ? bcmPortFlexeGroupCalB : bcmPortFlexeGroupCalA;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_flexe_cal_compare
                        (unit, group_index, is_rx, target_cal_id, &client_channel, &speed));
        SHR_IF_ERR_EXIT(flexe_core_flexe_client_ghao_complete
                        (unit, group_index, client_channel, slots_direction[count], speed));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Stop the G.hao speed switching, this
 *    API is only relevant for demux, for exception
 *    handling
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] flags - flags, rx or tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_flexe_group_ghao_stop(
    int unit,
    bcm_gport_t group_index,
    uint32 flags)
{
    bcm_port_flexe_group_ghao_action_t curr_action;
    bcm_port_flexe_group_cal_t target_cal_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Stop is only relevant for Rx
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_action_get(unit, group_index, 1, &curr_action));
    /*
     * Revert the orignal calendar config
     */
    target_cal_id = (curr_action == bcmPortFlexeGHaoCalAToB) ? bcmPortFlexeGroupCalA : bcmPortFlexeGroupCalB;
    SHR_IF_ERR_EXIT(flexe_core_flexe_client_ghao_stop(unit, group_index, target_cal_id, flags));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the G.hao action for demux and mux
 *
 * \param [in] unit - chip unit id
 * \param [in] gport - FlexE group gport
 * \param [in] flags - flags, rx or tx
 * \param [in] action - G.hao actions
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
bcm_dnx_port_flexe_group_ghao_action_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_port_flexe_group_ghao_action_t action)
{
    int group_index;
    bcm_port_flexe_group_ghao_action_t action_tmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_flexe_group_ghao_action_verify(unit, gport, flags, action));
    /*
     * If either RX or TX flags are not set, set both
     */
    if (!DNX_ALGO_PORT_FLEXE_TX_GET(flags) && !DNX_ALGO_PORT_FLEXE_RX_GET(flags))
    {
        DNX_ALGO_PORT_FLEXE_TX_SET(flags);
        DNX_ALGO_PORT_FLEXE_RX_SET(flags);
    }
    group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
    switch (action)
    {
        case bcmPortFlexeGHaoCalAToB:
        case bcmPortFlexeGHaoCalBToA:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_group_ghao_cal_switch_action_set(unit, group_index, flags, action));
            break;
        }
        case bcmPortFlexeGHaoComplete:
        {
            SHR_IF_ERR_EXIT(dnx_port_flexe_group_ghao_complete(unit, group_index, flags));
            break;
        }
        default:
        {
            /*
             * bcmPortFlexeGHaoStop
             */
            SHR_IF_ERR_EXIT(dnx_port_flexe_group_ghao_stop(unit, group_index, flags));
            break;
        }
    }
    /*
     * Update DB
     * If the action is "stop", mark the action as initial status - complete
     */
    action_tmp = (action == bcmPortFlexeGHaoStop) ? bcmPortFlexeGHaoComplete : action;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_ghao_action_set(unit, group_index, flags, action_tmp));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify parameters for
 *    dnx_port_l1_eth_1588_enable_get/set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] type - port control type
 * \param [in] enable - enable or disable
 * \param [in] is_set - set or get
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_l1_eth_1588_enable_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable,
    int is_set)
{
    uint32 flags;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if FlexE mode is enable
     */
    if (!DNX_ALGO_FLEXE_IS_ENABLED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is unavailable when FlexE mode is disabled.\n");
    }
    /*
     * Null check
     */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    /*
     * Check port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!(DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE) && DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "L1 ETH 1588 is only supported on the FRAMER L1 ETH ports.\n");
    }
    /*
     * Verify the type
     */
    switch (type)
    {
        case bcmPortControlL1Eth1588RxEnable:
        {
            flags = BCM_PORT_FLEXE_RX;
            break;
        }
        case bcmPortControlL1Eth1588TxEnable:
        {
            flags = BCM_PORT_FLEXE_TX;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported control type.\n");
        }
    }
    if (is_set)
    {
        /*
         * Verify the value
         */
        SHR_BOOL_VERIFY(*enable, _SHR_E_PARAM, "The configuration value should be 0 or 1.\n");
        /*
         * Verify if the flow is configured
         */
        SHR_IF_ERR_EXIT(dnx_port_framer_port_flow_exist_verify(unit, port, flags));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get enable status for 1588 on L1 ETH ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] type - port control type
 * \param [out] enable - enable or disable
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
dnx_port_l1_eth_1588_enable_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_l1_eth_1588_enable_verify(unit, port, type, enable, 0));

    flags = (type == bcmPortControlL1Eth1588RxEnable) ? BCM_PORT_FLEXE_RX : BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(imb_port_l1_eth_1588_enable_get(unit, port, flags, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/Disable 1588 on L1 ETH ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port ID
 * \param [in] type - port control type
 * \param [in] enable - enable or disable
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
dnx_port_l1_eth_1588_enable_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int enable)
{
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_l1_eth_1588_enable_verify(unit, port, type, &enable, 1));

    flags = (type == bcmPortControlL1Eth1588RxEnable) ? BCM_PORT_FLEXE_RX : BCM_PORT_FLEXE_TX;
    SHR_IF_ERR_EXIT(imb_port_l1_eth_1588_enable_set(unit, port, flags, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify parameters for
 *    dnx_port_framer_stat_channel_create
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_framer_stat_channel_create_verify(
    int unit,
    bcm_port_t logical_port)
{
    int nof_dests;
    int index, tmp_block_id;
    flexe_core_66bswitch_channel_info_t src_info;
    flexe_core_66bswitch_channel_info_t dest_info[DNX_DATA_MAX_NIF_FRAMER_B66SWITCH_BACKUP_CHANNELS + 1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if there is free channels to monitor
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_flow_port_info_get
                    (unit, logical_port, 0, 0, &src_info.block_id, &src_info.channel));
    /*
     * Get 66b switching info
     */
    SHR_IF_ERR_EXIT(flexe_core_66b_switch_get(unit, &src_info, dest_info, &nof_dests));

    if (nof_dests >= (dnx_data_nif.framer.b66switch_backup_channels_get(unit) + 1))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "There is no free path for port %d to monitor.\n", logical_port);
    }
    /*
     * Check if the backup channel is used
     */
    if (nof_dests == 1)
    {
        for (index = 0; index < dnx_data_nif.framer.b66switch_info_get(unit)->key_size[0]; index++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_framer_66bsw_block_id_get(unit, index, 1, &tmp_block_id));
            if (dest_info[0].block_id == tmp_block_id)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "The backup channel has been used. Cannot create monitor path.\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Create stat channel for FlexE client
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port ID
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
dnx_port_framer_stat_channel_create(
    int unit,
    bcm_port_t logical_port)
{
    int stat_channel, flexe_channel;
    int channel_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_framer_stat_channel_create_verify(unit, logical_port));
    /*
     * Try to allocate one stat channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_shared_channel_alloc(unit, bcmPortFramerModuleMac1, &stat_channel));
    channel_allocated = 1;
    /*
     * Configure the Framer
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &flexe_channel));
    SHR_IF_ERR_EXIT(flexe_core_stat_channel_config_set(unit, flexe_channel, stat_channel, 1));
    /*
     * Update DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_stat_channel_set(unit, logical_port, stat_channel));
exit:
    if (channel_allocated && SHR_FAILURE(_func_rv))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_shared_channel_free(unit, bcmPortFramerModuleMac1, stat_channel));
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - Destroy stat channel for FlexE client
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port ID
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
dnx_port_framer_stat_channel_destroy(
    int unit,
    bcm_port_t logical_port)
{
    int stat_channel, flexe_channel;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get stat channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_stat_channel_get(unit, logical_port, &stat_channel));
    /*
     * Configure the Framer
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &flexe_channel));
    SHR_IF_ERR_EXIT(flexe_core_stat_channel_config_set(unit, flexe_channel, stat_channel, 0));
    /*
     * Update DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_shared_channel_free(unit, bcmPortFramerModuleMac1, stat_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_stat_channel_set(unit, logical_port, -1));
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
