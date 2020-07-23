/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include <bcm_int/dnx/port/imb/imb_ile.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/flow_control/cosq_fc.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/cmic.h>

#include "imb_utils.h"
#include "imb_ile_internal.h"
#include "imb_ilu_internal.h"
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_NIF_QUEUE_LEVEL_FLOW_CTRL_NOF_CHANNELS  (16)
#define IMB_ILE_FEC_NOF_INSTANCES  (6)

static int imb_ilu_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

/**
 * \brief
 *   Init the ILU scheduler for the ilkn core scheduler.
 *   Set the weight to 0.
 */
static int
imb_ilu_scheduler_config_init(
    int unit,
    uint32 ilkn_core_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        /*
         * Clear weight per ILKN port
         */
        SHR_IF_ERR_EXIT(imb_ilu_port_scheduler_config_hw_init(unit, ilkn_core_id));
    }
    else
    {
        /*
         * Clear weight per ILKN core
         */
        SHR_IF_ERR_EXIT(imb_ilu_core_scheduler_config_hw_set
                        (unit, ilkn_core_id, 0, 0, DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0));
    }

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_hrf_flush_support))
    {
        /*
         * Set Scheduler cnt decrement threshold
         */
        SHR_IF_ERR_EXIT(imb_ilu_sch_cnt_dec_thres_hw_set(unit, ilkn_core_id, 0));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - initialize the ILE required information.
 * allso create a new PM for the ILE unit
 *
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB info required for
 *        initalization.
 * \param [in] imb_specific_info - specific info required to
 *        init the ILE
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
imb_ilu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    portmod_pm_create_info_t pm_info;
    int ilkn_iter;
    const dnx_data_nif_ilkn_ilkn_cores_t *ilkn_core_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core info
     */
    ilkn_core_info = dnx_data_nif.ilkn.ilkn_cores_get(unit, imb_info->inst_id);

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

    for (ilkn_iter = 0; ilkn_iter < dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); ilkn_iter++)
    {
        pm_info.pm_specific_info.os_ilkn.wm_high[ilkn_iter] = dnx_data_nif.ilkn.watermark_high_data_get(unit);
        pm_info.pm_specific_info.os_ilkn.wm_low[ilkn_iter] = dnx_data_nif.ilkn.watermark_low_data_get(unit);

        if (!sw_state_is_warm_boot(unit))
        {
            /*
             * Init all schedulers in the ilkn core
             */
            SHR_IF_ERR_EXIT(imb_ilu_scheduler_config_init(unit, imb_info->inst_id));
        }
    }

    if (!sw_state_is_warm_boot(unit))
    {
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ilu_power_down_set(unit, imb_info->inst_id, TRUE));
        }

    }

    pm_info.type = portmodDispatchTypePmOsILKN_50G;
    pm_info.pm_specific_info.os_ilkn.fec_disable_by_bypass = ilkn_core_info->fec_disable_by_bypass;
    pm_info.pm_specific_info.os_ilkn.core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
    /*
     * ILKN core lane map callback
     */
    pm_info.pm_specific_info.os_ilkn.ilkn_port_lane_map_get = imb_ile_port_ilkn_port_lane_map_get;
    pm_info.pm_specific_info.os_ilkn.ilkn_pm_lane_map_get = imb_ile_port_ilkn_pm_lane_map_get;
    pm_info.pm_specific_info.os_ilkn.ilkn_port_fec_units_set = imb_ile_port_ilkn_fec_units_set;
    pm_info.pm_specific_info.os_ilkn.ilkn_block_index = imb_info->inst_id;

    SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - De-init the ILU.
 *
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB informatin required for de-init
 * \param [in] imb_specific_info - specific ILE information to
 *        de-init.
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
imb_ilu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Nothing to do
     */
    SHR_FUNC_EXIT;
}
/**
 * See .h file
 */
int
imb_ilu_inband_ilkn_fc_init(
    int unit,
    const imb_create_info_t * imb_info)
{
    uint32 ilkn_core_id, ilkn_port_id, ilkn_cal_id;
    int tx_cal_len = -1, rx_cal_len = -1, llfc_mode, calendar_mode;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Iterate over all ILKN cores (ILUs) and ILKN Port IDs
     */
    ilkn_core_id = imb_info->inst_id;
    for (ilkn_port_id = 0; ilkn_port_id < dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); ilkn_port_id++)
    {
        /** Calculate the ILKN calendar ID */
        ilkn_cal_id = ilkn_core_id * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) + ilkn_port_id;

        /** Get the LLFC mode - determines which channels will be used for LLFC */
        llfc_mode = dnx_data_fc.inband.port_get(unit, ilkn_cal_id)->llfc_mode;

        /** Get the ILKN calendar mode */
        calendar_mode = dnx_data_fc.inband.port_get(unit, ilkn_cal_id)->mode;

        /** If RX mode is enabled, get calendar length */
        rx_cal_len = dnx_data_fc.inband.calender_length_get(unit, ilkn_cal_id)->rx - 1;

        /** If TX mode is enabled, get calendar length */
        /** TX calendar length should be set as follows:4'd0-> length of 16, 4'd1->32, 4'd3->64, 4'd7->128, 4'd15->256 */
        tx_cal_len = (dnx_data_fc.inband.calender_length_get(unit, ilkn_cal_id)->tx - 1) / 16;

        /** Call internal function to set to HW */
        SHR_IF_ERR_EXIT(imb_ilu_internal_inband_ilkn_fc_init
                        (unit, ilkn_core_id, ilkn_port_id, tx_cal_len, rx_cal_len, llfc_mode, calendar_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ilu_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ILU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /** Set to HW */
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_stop_pm_from_cfc_llfc_enable_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_ilu_fc_reset_set(
    int unit,
    const imb_create_info_t * imb_info,
    uint32 in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Set HW to put in/out of reset the specified ILU */
    SHR_IF_ERR_EXIT(imb_ilu_internal_fc_reset_set(unit, imb_info->inst_id, in_reset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Update reset for High/Low priority in ILKN core
 * See .h file
 */
int
imb_ilu_port_scheduler_priority_update(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update priority reset in NMG, in case the reset is done per
     * ILKN core.
     */
    if (!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        dnx_algo_port_ilkn_access_info_t ilkn_access_info;
        int is_low_priority_exists = FALSE;
        int is_high_priority_exists = FALSE;
        bcm_pbmp_t ilkn_ports;
        bcm_port_t port_i;
        bcm_port_nif_scheduler_t ilkn_port_sch_prio;
        uint32 dbal_low_sch_prio;
        uint32 dbal_high_sch_prio;

        /*
         * Get ILKN access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

        /*
         * Get all ILKN ports in the same core
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, port, &ilkn_ports));

        BCM_PBMP_ITER(ilkn_ports, port_i)
        {
            /*
             * Get ILKN scheduler priority from sw state
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port_i, &ilkn_port_sch_prio));

            if (ilkn_port_sch_prio == bcmPortNifSchedulerLow)
            {
                is_low_priority_exists = TRUE;
            }
            else
            {
                is_high_priority_exists = TRUE;
            }
        }

        /*
         * convert bcm low sch_prio to DBAL low sch_prio
         */
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get
                        (unit, bcmPortNifSchedulerLow, &dbal_low_sch_prio));
        /*
         * Take low priority NMG Rx core out of reset in case low
         * priority exists, or put it in reset in case low priority
         * doesn't exist.
         */
        SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset
                        (unit, ilkn_access_info.ilkn_core, dbal_low_sch_prio, is_low_priority_exists ? FALSE : TRUE));

        /*
         * convert bcm high sch_prio to DBAL high sch_prio
         */
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get
                        (unit, bcmPortNifSchedulerHigh, &dbal_high_sch_prio));
        /*
         * Take high priority NMG Rx core out of reset in case high
         * priority exists, or put it in reset in case high priority
         * doesn't exist.
         */
        SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset
                        (unit, ilkn_access_info.ilkn_core, dbal_high_sch_prio, is_high_priority_exists ? FALSE : TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Put ILU Rx in NMG into out of reset.
 */
static int
imb_ilu_rx_nmg_reset_out_of_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * Get the channel TDM mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;

        ilkn_hrf_type =
            (tdm_mode ==
             DNX_ALGO_PORT_TDM_MODE_BYPASS) ? DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM : DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA;

        /*
         * Take NMG Rx HRF out of reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_hrf_reset
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, FALSE));
    }
    else
    {
        uint32 dbal_sch_prio;
        dnx_algo_port_info_s port_info;
        int is_tdm_or_l1;

        /*
         * Get the channel port type
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        is_tdm_or_l1 = ((tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
                        || (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))) ? 1 : 0;

        if (is_tdm_or_l1)
        {
            /*
             * convert bcm tdm sch_prio to DBAL tdm sch_prio
             */
            SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get
                            (unit, bcmPortNifSchedulerTDM, &dbal_sch_prio));

            /*
             * Take NMG Rx core tdm out of reset
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset(unit, ilkn_access_info.ilkn_core, dbal_sch_prio, FALSE));
        }
        /*
         * Since high/low rx priority can be changed after the port attach
         * sequence, High/Low Rx priority reset will be done in the
         * following function:
         * bcm_dnx_port_control_set(bcmPortControlRxPriority)
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Put ILU Rx in NMG into reset.
 */
static int
imb_ilu_rx_nmg_reset_in_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dnx_algo_port_tdm_mode_e tdm_mode;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * Get the channel TDM mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
    /*
     * Get the channel port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        bcm_pbmp_t channels;
        dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;
        uint32 flags = 0;

        BCM_PBMP_CLEAR(channels);

        if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
        {
            flags = DNX_ALGO_PORT_CHANNELS_F_TDM_BYPASS_ONLY;
            ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM;
        }
        else
        {
            flags = DNX_ALGO_PORT_CHANNELS_F_NON_TDM_BYPASS_ONLY;
            ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA;
        }

        /*
         * Get all channels with same TDM mode
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, flags, &channels));
        /*
         * Don't include self channel
         */
        BCM_PBMP_PORT_REMOVE(channels, port);
        /*
         * Reset HRF if no channels with same TDM mode left on this interface
         */
        if (BCM_PBMP_IS_NULL(channels))
        {
            SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_hrf_reset
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, TRUE));
        }
    }
    else
    {
        bcm_pbmp_t ilkn_core_channels;
        bcm_port_nif_scheduler_t sch_prio;
        uint32 dbal_sch_prio;
        dnx_algo_port_info_s port_info;
        int is_tdm_or_l1;

        BCM_PBMP_CLEAR(ilkn_core_channels);

        /*
         * Get the channel port type
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        is_tdm_or_l1 = ((tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
                        || (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) ? 1 : 0));

        if (is_tdm_or_l1)
        {
            sch_prio = bcmPortNifSchedulerTDM;
        }
        else
        {
            /*
             * Get ILKN scheduler priority from sw state
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &sch_prio));
        }

        /*
         * Get all channels in the ilkn core with same scheduler priority
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_sch_prio_channels_in_core_get(unit, port, sch_prio, &ilkn_core_channels));
        /*
         * Don't include self channel
         */
        BCM_PBMP_PORT_REMOVE(ilkn_core_channels, port);
        /*
         * Reset the appropriate scheduler priority of the ILU if no channels
         * with the same priority were left.
         */
        if (BCM_PBMP_IS_NULL(ilkn_core_channels))
        {
            /*
             * convert bcm sch_prio to DBAL sch_prio
             */
            SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
            /*
             * Put NMG Rx core in reset
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset(unit, ilkn_access_info.ilkn_core, dbal_sch_prio, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset ILU Rx in NMG
 */
static int
imb_ilu_rx_nmg_reset(
    int unit,
    bcm_port_t port,
    int in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (in_reset == IMB_COMMON_IN_RESET)
    {
        SHR_IF_ERR_EXIT(imb_ilu_rx_nmg_reset_in_reset(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_ilu_rx_nmg_reset_out_of_reset(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Per channel connfiguration when adding a port
 */
static int
imb_ilu_port_channel_add(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_tdm_mode_e tdm_mode;
    int is_tdm;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int channel_id;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ILU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel_id));

    /** Set IS_TDM per channel */
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    is_tdm = (((tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS) || (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))) ? 1 : 0);
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_channel_tdm_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, channel_id, is_tdm));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Reset per channel configuration when removing a port
 */
static int
imb_ilu_port_channel_remove(
    int unit,
    bcm_port_t port)
{
    int is_tdm;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    int channel_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ILU access info */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel_id));

    /** Reset IS_TDM per channel */
    is_tdm = 0;
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_channel_tdm_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, channel_id, is_tdm));

exit:
    SHR_FUNC_EXIT;
}

/***
    * brief - configure a new ILU port.
    *
    *\param[in] unit - chip unit id
    * \param[in] port - logical port
    return *int -see _SHR_E *
 */
int
imb_ilu_port_attach(
    int unit,
    bcm_port_t port)
{
    int nof_segments, is_master_port, master_port;
    uint32 burst_min, burst_max, enable_padding;
    int enable_interleaving;
    uint8 is_extended_mem_used = 0;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get whether this port is master port or channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_to_ilkn_master_get(unit, port, &master_port));
    is_master_port = (port == master_port);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * At this point, we assume that ilkn data port uses half of number of segments. The exact number of segments will
     * be later determined by the portmod according to number of lanes and serdes rate. Then extended memory mode will
     * be determined, and all configuration will be set accordingly.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_nof_segments_default_get(unit, port, &nof_segments));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_set(unit, port, nof_segments));
    is_extended_mem_used = 0;

    /** Padding should be configured by port control */
    enable_padding = 0;
    burst_min = dnx_data_nif.ilkn.properties_get(unit, ilkn_access_info.ilkn_id)->burst_min;
    burst_max = dnx_data_nif.ilkn.properties_get(unit, ilkn_access_info.ilkn_id)->burst_max;

    if (is_master_port)
    {
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power up the unit */
            SHR_IF_ERR_EXIT(imb_ilu_port_power_down_set(unit, port, FALSE));
        }

        /*
         * Set ILKN Burst Editor configuration
         */
        SHR_IF_ERR_EXIT(imb_ilu_ibe_config(unit, port, enable_padding, burst_max, burst_min, is_extended_mem_used));
        SHR_IF_ERR_EXIT(imb_ilu_ibe_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        /*
         * Take parser wrap out of reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_parser_wrap_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        /*
         * Take PRD out of reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_reset(unit, port, IMB_COMMON_OUT_OF_RESET));
        /*
         * HRF RX configuration
         */

        SHR_IF_ERR_EXIT(dnx_algo_port_is_ingress_interleave_get(unit, port, &enable_interleaving));

        SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_config_set(unit, port, is_extended_mem_used, enable_interleaving, 1));

        /*
         * HRF TX configuration
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_tx_config_set(unit, port, burst_max, is_extended_mem_used));

        /*
         * HRF RX Segmentation Reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset(unit, port, 1, IMB_COMMON_OUT_OF_RESET));

        /*
         * Set Active Segment for ilkn big port
         */
        if (ilkn_access_info.port_in_core == 0)
        {
            SHR_IF_ERR_EXIT(imb_ilu_active_segment_set(unit, port, is_extended_mem_used ? 0 : 1));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_ilu_active_segment_set(unit, port, 1));
        }
    }
    /*
     * Put ILU Rx in NMG into out of reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_rx_nmg_reset(unit, port, IMB_COMMON_OUT_OF_RESET));
    /*
     * Per channel configuration
     */
    SHR_IF_ERR_EXIT(imb_ilu_port_channel_add(unit, port));
    /*
     * Attach the ILE port
     */
    SHR_IF_ERR_EXIT(imb_ile_port_attach(unit, port, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the ILU. first calls Portmos API
 *        to remove from portmod DB, then configures all ILU
 *        settings.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ilu_port_detach(
    int unit,
    bcm_port_t port)
{
    int nof_channels, ilkn_nof_ports, is_complete = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
    /*
     * Get the relevant ports in this ilkn core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_nof_ports_get(unit, port, &ilkn_nof_ports));

    /*
     * Detach the ILE port
     */
    SHR_IF_ERR_EXIT(imb_ile_port_detach(unit, port));
    /*
     * Reset per channel configuration
     */
    SHR_IF_ERR_EXIT(imb_ilu_port_channel_remove(unit, port));
    /*
     * Put ILU Rx in NMG into reset
     */
    SHR_IF_ERR_EXIT(imb_ilu_rx_nmg_reset(unit, port, IMB_COMMON_IN_RESET));
    /*
     * Last channel in port
     */
    if (nof_channels == 1)
    {
        SHR_IF_ERR_EXIT(imb_port_scheduler_config_set(unit, port, 0));

        /*
         * HRF RX Segmentation Reset (1 for last channel in core, 0 otherwise)
         */
        is_complete = (ilkn_nof_ports == 1) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset(unit, port, is_complete, IMB_COMMON_IN_RESET));

        /*
         * PRD reset (last channel in port )
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_reset(unit, port, IMB_COMMON_IN_RESET));

        /*
         * parser wrap reset (last channel in port )
         */
        SHR_IF_ERR_EXIT(imb_ilu_parser_wrap_reset(unit, port, IMB_COMMON_IN_RESET));

        /*
         * reset ILKN Burst Editor
         */
        SHR_IF_ERR_EXIT(imb_ilu_ibe_reset(unit, port, IMB_COMMON_IN_RESET));

        if (is_complete && dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ilu_port_power_down_set(unit, port, TRUE));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ilkn data port speed configuration
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed_config - port speed configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ilu_port_speed_config_set(
    int unit,
    bcm_port_t port,
    const portmod_speed_config_t * speed_config)
{
    int core_clk, ilkn_burst_short, nof_segments, current_nof_segments;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    core_clk = dnx_data_device.general.core_clock_khz_get(unit);
    ilkn_burst_short = dnx_data_nif.ilkn.properties_get(unit, ilkn_access_info.ilkn_id)->burst_short;
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, port, &current_nof_segments));

    SHR_IF_ERR_EXIT(dnx_algo_port_imb_ilkn_nof_segments_calc
                    (unit, core_clk, speed_config->num_lane, ilkn_burst_short, speed_config->speed, &nof_segments));

    if (nof_segments != current_nof_segments)
    {
        /*
         * verification: nof segments not bigger than maximum, not bigger then half for ilkn second (small) port
         */
        if ((nof_segments > dnx_data_nif.ilkn.segments_max_nof_get(unit))
            || ((ilkn_access_info.port_in_core == 1) && (nof_segments > dnx_data_nif.ilkn.segments_half_nof_get(unit))))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: Number of calculated segments %d is not correct! \n ", port, nof_segments);
        }
        /*
         * Update DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_set(unit, port, nof_segments));

        SHR_IF_ERR_EXIT(imb_ilu_port_nof_segments_set(unit, port, nof_segments));

        SHR_IF_ERR_EXIT(imb_ile_port_nof_segments_set(unit, port, nof_segments));
        
        SHR_IF_ERR_EXIT(imb_ile_port_segments_enable_set(unit, port, nof_segments));
        SHR_IF_ERR_EXIT(portmod_port_ilkn_nof_segments_set(unit, port, (uint32) nof_segments));
    }

    SHR_IF_ERR_EXIT(imb_ile_port_speed_config_set(unit, port, speed_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ilkn port speed configuration
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] speed_config - port speed configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ilu_port_speed_config_get(
    int unit,
    bcm_port_t port,
    portmod_speed_config_t * speed_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ile_port_speed_config_get(unit, port, speed_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get number of High/Low scheduler priority ports.
 *   Get the bandwidth of the Low scheduler priority
 *   ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_included - does port is included in the calculations
 * \param [out] nof_high_sch_prio_ports - number of high scheduler priority ports
 * \param [out] nof_low_sch_prio_ports - number of low scheduler priority ports
 * \param [out] low_sch_prio_bandwidth - bandwidth of low scheduler priority ports
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_ilu_nof_sch_prio_get(
    int unit,
    bcm_port_t port,
    int is_port_included,
    int *nof_high_sch_prio_ports,
    int *nof_low_sch_prio_ports,
    int *low_sch_prio_bandwidth)
{
    bcm_pbmp_t ilkn_ports;
    bcm_port_t port_i;
    bcm_port_nif_scheduler_t sch_prio;
    int nof_low_sch_prio = 0;
    int nof_high_sch_prio = 0;
    int bw_sum = 0;
    int port_speed = 0;
    int nof_channels = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all ILKN ports in the same core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, port, &ilkn_ports));

    if (!is_port_included)
    {
        /** Check if it is the last channel */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
        if (nof_channels == 1)
        {
            /*
             * don't include the current port in the calculation (means port is in process of being removed)
             */
            BCM_PBMP_PORT_REMOVE(ilkn_ports, port);
        }
    }
    BCM_PBMP_ITER(ilkn_ports, port_i)
    {
        /*
         * Get ILKN scheduler priority from sw state
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port_i, &sch_prio));

        if (sch_prio == bcmPortNifSchedulerLow)
        {
            /*
             * Count number of ports with low scheduler priority
             */
            ++nof_low_sch_prio;

            /*
             * Sum the bandwidth of ports with low scheduler priority
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port_i, 0, &port_speed));
            bw_sum += port_speed;
        }
        else
        {
            /*
             * Count number of ports with high scheduler priority.
             *
             * No need to sum the bandwidth since we'll always set the
             * weight to maximum for high scheduler priority.
             */
            ++nof_high_sch_prio;
        }
    }

    *nof_high_sch_prio_ports = nof_high_sch_prio;
    *nof_low_sch_prio_ports = nof_low_sch_prio;
    *low_sch_prio_bandwidth = bw_sum;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ilkn port scheduler
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable port scheduler
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ilu_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    bcm_port_nif_scheduler_t sch_prio;
    int is_max_sch_weight_tdm = 0;
    int is_max_sch_weight_high = 0;
    int nof_weight_bits_low_sch = 0;
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low;
    int port_speed = 0;
    int nof_channels = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        /*
         * The scheduler is per port.
         */

        /*
         * Set weight for TDM in case there is any channel on that port.
         * Only if there are no channels left on the port then clear the
         * weight.
         * For TDM scheduler priorities we set maximum weight.
         */
        /** Check if it is the last channel */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
        is_max_sch_weight_tdm = ((nof_channels == 1) && (enable == 0)) ? 0 : 1;

        /*
         * If scheduler priority is High, we set maximum weight.
         * If scheduler priority is Low, the weight is dependent on the bandwidth
         * of the port.
         */
        if (enable)
        {
            /*
             * Get ILKN scheduler priority from sw state
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &sch_prio));

            /*
             * Per port scheduler there can be only Low or High scheduler, but not both
             */
            if (sch_prio == bcmPortNifSchedulerLow)
            {
                /*
                 * Get number of Low scheduler priority weight bits
                 * according to port speed.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &port_speed));
                SHR_IF_ERR_EXIT(dnx_port_algo_imb_ilu_scheduler_weight_get(unit, port_speed, &nof_weight_bits_low_sch));

                /*
                 * No High scheduler priority weight
                 */
                is_max_sch_weight_high = 0;
            }
            else
            {
                /*
                 * No Low scheduler priority weight
                 */
                nof_weight_bits_low_sch = 0;
                /*
                 * Set High scheduler priority weight to max
                 */
                is_max_sch_weight_high = 1;
            }
        }
        else
        {
            /*
             * No High/Low scheduler priority weight
             */
            nof_weight_bits_low_sch = 0;
            is_max_sch_weight_high = 0;
        }

        /*
         * Convert number of weight bits to DBAL weight
         */
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, nof_weight_bits_low_sch, &dbal_sch_weight_low));

        /*
         * Configure weight per ILKN port
         */
        SHR_IF_ERR_EXIT(imb_ilu_port_scheduler_config_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, is_max_sch_weight_tdm,
                         is_max_sch_weight_high, dbal_sch_weight_low));

    }
    else
    {
        /*
         * The scheduler is per ILU.
         */

        /*
         * If there are High scheduler priorities ports on the ILKN we
         * set maximum weight.
         * If there are Low scheduler priorities ports on the ILKN the
         * weight is dependent on the bandwidth of all of them together.
         * If there is any port on the ILKN then we set TDM scheduler
         * priority to maximum weight.
         */
        int is_port_included = enable;
        int nof_high_sch_prio_ports = 0;
        int nof_low_sch_prio_ports = 0;
        int low_sch_prio_ports_bandwidth = 0;

        /*
         * Get number of High and Low scheduler priority ports.
         * Get bandwidth of Low scheduler priority ports.
         */
        SHR_IF_ERR_EXIT(imb_ilu_nof_sch_prio_get
                        (unit, port, is_port_included, &nof_high_sch_prio_ports, &nof_low_sch_prio_ports,
                         &low_sch_prio_ports_bandwidth));
        /*
         * Convert the Low scheduler priority ports bandwidth to number
         * of weight bits.
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_ilu_scheduler_weight_get
                        (unit, low_sch_prio_ports_bandwidth, &nof_weight_bits_low_sch));
        /*
         * Need to set High scheduler priority in case there are High
         * scheduler priority ports on the ILKN.
         */
        is_max_sch_weight_high = (nof_high_sch_prio_ports > 0) ? 1 : 0;
        /*
         * TDM scheduler priority will always be set, unless we disable
         * and there are no ports left on the ILKN.
         */
        /*
         * Set weight for TDM in case there is any port on that core.
         * Only if there are no ports left on the core then clear the
         * weight.
         * For TDM scheduler priorities we set maximum weight.
         */
        is_max_sch_weight_tdm = ((nof_high_sch_prio_ports + nof_low_sch_prio_ports == 0) && (enable == 0)) ? 0 : 1;

        /*
         * Convert number of weight bits to DBAL weight
         */
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, nof_weight_bits_low_sch, &dbal_sch_weight_low));

        /*
         * Configure weight per ILKN core
         */
        SHR_IF_ERR_EXIT(imb_ilu_core_scheduler_config_hw_set
                        (unit, ilkn_access_info.ilkn_core, is_max_sch_weight_tdm, is_max_sch_weight_high,
                         dbal_sch_weight_low));

        /*
         * Get ILKN scheduler priority from sw state
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &sch_prio));

        /*
         * Set Data HRF priority in HW
         */
        SHR_IF_ERR_EXIT(imb_ilu_internal_port_hrf_scheduler_config_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core,
                         (sch_prio == bcmPortNifSchedulerHigh) ? 1 : 0));

        if (sch_prio == bcmPortNifSchedulerLow)
        {
            /*
             * Get number of Low scheduler priority weight bits
             * according to port speed.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &port_speed));
            SHR_IF_ERR_EXIT(dnx_port_algo_imb_ilu_scheduler_weight_get(unit, port_speed, &nof_weight_bits_low_sch));

            /*
             * Convert number of weight bits to DBAL weight
             */
            SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, nof_weight_bits_low_sch, &dbal_sch_weight_low));

            /*
             * Configure Data HRF weight in Low priority scheduler
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_low_prio_scheduler_config_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, dbal_sch_weight_low));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Flush packets in ILU RX
 */
static int
imb_ilu_hrf_controller_rx_hrf_flush_and_reset(
    int unit,
    bcm_port_t port,
    dnx_algo_port_ilkn_access_info_t ilkn_access_info,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type)
{

    uint32 nof_bits_per_sec;
    uint32 time_usec;
    uint32 counter;
    uint32 sch_weight_tdm = 0, sch_weight_high = 0;
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low;
    int get_sch_config = 0;
    bcm_port_nif_scheduler_t ilkn_port_sch_prio;
    uint32 dbal_sch_prio;
    int ilkn_nof_ports;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
    {
        SHR_IF_ERR_EXIT(imb_ilu_core_scheduler_config_hw_get(unit,
                                                             ilkn_access_info.ilkn_core,
                                                             &sch_weight_tdm, &sch_weight_high, &dbal_sch_weight_low));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_ilu_port_scheduler_config_hw_get(unit,
                                                             ilkn_access_info.ilkn_core,
                                                             ilkn_access_info.port_in_core,
                                                             &sch_weight_tdm, &sch_weight_high, &dbal_sch_weight_low));
    }
    get_sch_config = 1;

    /**
     * Enable flush sequence
     */
    /*
     * Disable Data to HRF from PM
     */
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_drop_data_hw_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, TRUE));

    /*
     * In case SOP entered the fifo, we need to wait until all the packet
     * will enter.
     * Worst case is maximum packet size on a minimum bandwidth port (1Gbps).
     */
    nof_bits_per_sec = dnx_data_nif.eth.packet_size_max_get(unit) * UTILEX_NOF_BITS_IN_CHAR;
    time_usec = UTILEX_DIV_ROUND_UP(nof_bits_per_sec, 1000);
    sal_usleep(time_usec);

    if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_hrf_flush_support))
    {
        /*
         * Here we have to check if there are entries in  HRF, if empty Do not Flush!
         */
        SHR_IF_ERR_EXIT(imb_ilu_port_rx_hrf_counter_get
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, &counter));
    }
    else
    {
        uint32 max_fifo_level;
        SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_fifo_status_get(unit, ilkn_access_info.ilkn_core,
                                                                 ilkn_access_info.port_in_core, ilkn_hrf_type,
                                                                 &max_fifo_level, &counter));
    }

    if (counter == 0)
    {
        /*
         * note that local scheduler counter is 0 and no data can enter HRF
         * so we can reset HRF without removing from scheduler
         */

        /**
         * Reset the HRF
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type,
                         IMB_COMMON_IN_RESET));
    }
    else
    {

        /*
         * Mask the priority in the NIF scheduler round-robin, to disable RD operation during Flush
         */
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
        {
            SHR_IF_ERR_EXIT(imb_ilu_port_scheduler_config_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core,
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA ? sch_weight_tdm : 0),
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM ? sch_weight_high : 0),
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM ? dbal_sch_weight_low :
                              DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0)));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_ilu_core_scheduler_config_hw_set
                            (unit, ilkn_access_info.ilkn_core,
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA ? sch_weight_tdm : 0),
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM ? sch_weight_high : 0),
                             (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM ? dbal_sch_weight_low :
                              DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0)));
        }

        /*
         * Get number of active interfaces on this ilkn core
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_nof_ports_get(unit, port, &ilkn_nof_ports));

        if (ilkn_hrf_type == DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &ilkn_port_sch_prio));
            SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, ilkn_port_sch_prio, &dbal_sch_prio));
        }
        else
        {
            dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_TDM;
        }

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_hrf_flush_support) && ilkn_nof_ports > 1)
        {
            /** there is additional port on this core - flush must be used */

            /*
             * Enable flush contexts in NMG
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_nmg_flush_context_enable_hw_set
                            (unit, ilkn_access_info.ilkn_core, dbal_sch_prio, TRUE));

            /*
             * Enable Flush mode in NMG
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_nmg_flush_mode_enable_hw_set(unit, TRUE));

            /*
             * Enable flush mode for HRF
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_hrf_flush_mode_enable_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, TRUE));

            /*
             * Start flushing
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_core_flush_enable_hw_set(unit, ilkn_access_info.ilkn_core, TRUE));

            /*
             * Wait until HRF counter is empty
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_hrf_counter_polling
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type));
        }
        else
        {
            /** there is no additional port on this core - direct reset can be used */
            if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
            {
                SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_hrf_reset
                                (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, TRUE));
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset(unit, ilkn_access_info.ilkn_core, dbal_sch_prio,
                                                                   TRUE));

            }
        }

        /**
         * Reset the HRF
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type,
                         IMB_COMMON_IN_RESET));

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_hrf_flush_support) && ilkn_nof_ports > 1)
        {
            /**
             * Disable flush sequence
             */
            /*
             * Stop flushing
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_core_flush_enable_hw_set(unit, ilkn_access_info.ilkn_core, FALSE));

            /*
             * Disable flush mode for HRF
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_hrf_flush_mode_enable_hw_set(unit, ilkn_access_info.ilkn_core,
                                                                                  ilkn_access_info.port_in_core,
                                                                                  ilkn_hrf_type, FALSE));

            /*
             * Disable Flush mode in NMG
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_nmg_flush_mode_enable_hw_set(unit, FALSE));

            /*
             * Disable flush contexts in NMG
             */
            SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_nmg_flush_context_enable_hw_set
                            (unit, ilkn_access_info.ilkn_core, dbal_sch_prio, FALSE));
        }
        else
        {
            if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
            {
                SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_hrf_reset(unit, ilkn_access_info.ilkn_core,
                                                                  ilkn_access_info.port_in_core, ilkn_hrf_type, FALSE));
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_ilu_internal_nmg_rx_core_reset(unit, ilkn_access_info.ilkn_core, dbal_sch_prio,
                                                                   FALSE));

            }
        }

        /*
         * Restore the priority in the NIF scheduler round-robin
         */
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
        {
            SHR_IF_ERR_EXIT(imb_ilu_port_scheduler_config_hw_set(unit,
                                                                 ilkn_access_info.ilkn_core,
                                                                 ilkn_access_info.port_in_core,
                                                                 sch_weight_tdm, sch_weight_high, dbal_sch_weight_low));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_ilu_core_scheduler_config_hw_set(unit,
                                                                 ilkn_access_info.ilkn_core,
                                                                 sch_weight_tdm, sch_weight_high, dbal_sch_weight_low));
        }
    }

    /*
     * Enable Data to HRF from PM
     */
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_drop_data_hw_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, FALSE));

exit:
    if (get_sch_config && SHR_GET_CURRENT_ERR() != BCM_E_NONE)
    {
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_scheduler_context_per_port_support))
        {
            SHR_IF_ERR_CONT(imb_ilu_port_scheduler_config_hw_set(unit,
                                                                 ilkn_access_info.ilkn_core,
                                                                 ilkn_access_info.port_in_core,
                                                                 sch_weight_tdm, sch_weight_high, dbal_sch_weight_low));
        }
        else
        {
            SHR_IF_ERR_CONT(imb_ilu_core_scheduler_config_hw_set(unit,
                                                                 ilkn_access_info.ilkn_core,
                                                                 sch_weight_tdm, sch_weight_high, dbal_sch_weight_low));
        }
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Flush packets in ILU RX
 */
static int
imb_ilu_hrf_controller_rx_flush_and_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * Run flush sequence for each of the HRFs of the port
     */
    for (ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA; ilkn_hrf_type < DBAL_NOF_ENUM_ILKN_HRF_TYPE_VALUES;
         ++ilkn_hrf_type)
    {
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hrf_flush_and_reset(unit, port, ilkn_access_info, ilkn_hrf_type));
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_ilu_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    dnx_algo_port_info_s port_info;
    int nof_active_port;
    bcm_port_t port_i;
    bcm_pbmp_t pbmp_same_if;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &pbmp_same_if));

    if (enable)
    {

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, TRUE));
        }

        /*
         * NMG Tx reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_tx_nmg_reset(unit, port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_ilu_tx_nmg_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        /*
         * HRF RX Segmentation Reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset(unit, port, 1, IMB_COMMON_OUT_OF_RESET));

        /*
         * Enable ILE
         */
        SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, port, flags, enable));

        /*
         * Take TX HRF out-of-reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_tx_reset(unit, port, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_tx_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        /*
         * Take RX HRF out-of-reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_reset(unit, port, IMB_COMMON_OUT_OF_RESET));

        sal_usleep(10);
        /*
         * override the egress credits
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, port));
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            uint8 is_extended_memory_used;
            int max_credits = dnx_data_nif.ilkn.tx_hrf_credits_get(unit);
            SHR_IF_ERR_EXIT(imb_ilu_is_extended_memory_used(unit, port, &is_extended_memory_used));
            if (is_extended_memory_used)
            {
                max_credits *= 2;
            }
            SHR_IF_ERR_EXIT(dnx_esb_ilkn_credits_set(unit, port, max_credits));
        }

        /** Enable Qpairs */
        _SHR_PBMP_ITER(pbmp_same_if, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_i, enable, FALSE));
        }
    }
    else
    {
        /** Disable Qpairs */
        _SHR_PBMP_ITER(pbmp_same_if, port_i)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port_i, enable, FALSE));
        }

        /*
         * Flush (if availbale) and reset the Rx HRF controller
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_flush_and_reset(unit, port));

        /*
         * Reset TX HRF
         */
        SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_tx_reset(unit, port, IMB_COMMON_IN_RESET));

        DNX_ERR_RECOVERY_SUPPRESS(unit);
        /*
         * HRF RX Segmentation Reset
         */
        SHR_IF_ERR_EXIT(imb_ilu_port_nof_active_ilkn_ports_in_core_get(unit, port, &nof_active_port));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);

        SHR_IF_ERR_EXIT(imb_ilu_hrf_rx_segmentation_reset
                        (unit, port, (nof_active_port == 0) ? 1 : 0, IMB_COMMON_IN_RESET));

        /*
         * Disable ILE
         */
        SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, port, flags, enable));

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, FALSE));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable status of the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - returned enable status
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
imb_ilu_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    int port_enable = 0, core_enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** set initial value */
    *enable = 0;

    /*
     * Get the indication from Portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &port_enable));

    /*
     * Get the indication from ILE core
     */
    SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_get(unit, port, &core_enable));

    *enable = (port_enable && core_enable);

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get ILKN current bandwidth
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication whether to include /
 *        exclude specified port in the BW calculation
 * \param [out] bandwidth - BW calculated
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
imb_ilu_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth)
{
    bcm_pbmp_t ilkn_ports;
    bcm_port_t port_i;
    int bw_sum, port_speed;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all ILKN ports in the same core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_ports_in_core_get(unit, port, &ilkn_ports));

    bw_sum = 0;
    if (!is_port_include)
    {
        /*
         * don't include the current port in the calculation (means port is in process of being removed)
         */
        BCM_PBMP_PORT_REMOVE(ilkn_ports, port);
    }
    BCM_PBMP_ITER(ilkn_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port_i, 0, &port_speed));
        bw_sum += port_speed;
    }
    *bandwidth = bw_sum;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - port interface rate
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
imb_ilu_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int idx, start_thr = -1;
    int if_rate;
    int num_of_lanes;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * Calculate interface rate
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &num_of_lanes));
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_serdes_rate_to_data_rate_conv(unit, port, speed, num_of_lanes, &if_rate));

    /*
     * Lookup TX threshold in table.
     */
    for (idx = 0; idx < dnx_data_nif.ilkn.start_tx_threshold_table_info_get(unit)->key_size[0]; idx++)
    {
        if (if_rate <= dnx_data_nif.ilkn.start_tx_threshold_table_get(unit, idx)->speed)
        {
            start_thr = dnx_data_nif.ilkn.start_tx_threshold_table_get(unit, idx)->start_thr;
            break;
        }
    }

    if (start_thr < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported rate %d (port %d)\n", if_rate, port);
    }

    SHR_IF_ERR_EXIT(imb_ilu_tx_start_thr_hw_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, start_thr));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable packet padding. The only supported
 *    padding size for ILKN is 60B. Value "0" means disable
 *    padding.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - padding size
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
imb_ilu_port_pad_size_set(
    int unit,
    bcm_port_t port,
    int value)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(imb_ilu_internal_padding_enable_hw_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, (value != 0)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the padding size for ILKN
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - padding size
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
imb_ilu_port_pad_size_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    uint32 enable;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(imb_ilu_internal_padding_enable_hw_get
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, &enable));

    *value = (enable) ? dnx_data_nif.ilkn.pad_size_get(unit) : 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get RX HRF status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - nif schedule priority
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
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
imb_ilu_port_rx_fifo_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint32 *max_occupancy,
    uint32 *fifo_level)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    /*
     * Get ILKN HRF Type
     */
    if (sch_priority == bcmPortNifSchedulerTDM)
    {
        ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM;
    }
    else
    {
        ilkn_hrf_type = DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA;
    }
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_rx_fifo_status_get
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type, max_occupancy,
                     fifo_level));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ILKN TX HRF status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] pm_credits - Not used for ILU
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
imb_ilu_port_tx_fifo_status_get(
    int unit,
    bcm_port_t port,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *pm_credits)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(imb_ilu_internal_port_tx_fifo_status_get
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, max_occupancy, fifo_level));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
imb_ilu_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 en_soft_stage_eth, en_soft_stage_tm;
    uint32 is_eth_port = 0, is_tm_port = 0;
    uint32 port_type = 0;
    uint32 outer_tag_size = 0;
    int tc_offset = 0;
    int dp_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    if (enable_mode == bcmCosqIngressPortDropDisable)
    {
        /*
         * Disable Hard stage
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_hard_stage_enable_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, 0));

        /*
         * Disable Soft Stage
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_soft_stage_enable_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, 0, 0));

    }
    else
    {
        /*
         * set TC/DP offsets for ITMH/FTMH
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_itmh_offsets_get(unit, ilkn_access_info.ilkn_core, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_ilu_prd_itmh_offsets_hw_set(unit, ilkn_access_info.ilkn_core, tc_offset, dp_offset));

        SHR_IF_ERR_EXIT(dnx_cosq_prd_ftmh_offsets_get(unit, ilkn_access_info.ilkn_core, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_ilu_prd_ftmh_offsets_hw_set(unit, ilkn_access_info.ilkn_core, tc_offset, dp_offset));

        /*
         * Get is port extender
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_outer_tag_size_get(unit, port, &outer_tag_size));
        SHR_IF_ERR_EXIT(imb_ilu_prd_outer_tag_size_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, outer_tag_size));

        /*
         * Set Ether type sizes for fixed Ether types
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_ether_type_size_set(unit, port));

        /*
         * Set prd port type
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));
        SHR_IF_ERR_EXIT(imb_ilu_prd_port_type_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, port_type));

        /*
         * Enable soft stage
         */
        en_soft_stage_eth = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_eth_port)) ? 1 : 0;
        en_soft_stage_tm = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_tm_port)) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_ilu_prd_soft_stage_enable_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, en_soft_stage_eth,
                         en_soft_stage_tm));

        /*
         * Enable hard stage
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_hard_stage_enable_hw_set
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, 1));
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
imb_ilu_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 is_eth_port = 0, is_tm_port = 0, port_type, en_hard_stage, en_soft_stage_eth, en_soft_stage_tm;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ilu_prd_hard_stage_enable_hw_get
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, (uint32 *) &en_hard_stage));

    if (en_hard_stage)
    {
        /*
         * The expectation is that eth soft stage will only be set for eth ports, and tm soft stage will only be set
         * for tm ports. but we check for the port type just in case
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));

        SHR_IF_ERR_EXIT(imb_ilu_prd_soft_stage_enable_hw_get
                        (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, &en_soft_stage_eth,
                         &en_soft_stage_tm));

        if ((en_soft_stage_eth && is_eth_port) || (en_soft_stage_tm && is_tm_port))
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;
        }
        else
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardStage;
        }
    }
    else
    {
        *enable_mode = bcmCosqIngressPortDropDisable;
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
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
imb_ilu_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_ip_dscp_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_mpls_exp_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_inner_tag_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_outer_tag_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_ilu_prd_default_priority_hw_set
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
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
imb_ilu_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_ip_dscp_hw_get
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_mpls_exp_hw_get
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_inner_tag_hw_get
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_ilu_prd_ignore_outer_tag_hw_get
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_ilu_prd_default_priority_hw_get
                            (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
 * \param [in] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ilu_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_tm_tc_dp_hw_set(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_ip_dscp_hw_set(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_eth_pcp_dei_hw_set(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_mpls_exp_hw_set(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        priority tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [out] priority - returned priority value
 * \param [out] is_tdm - TDM signal
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ilu_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    *is_tdm = 0;
    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_tm_tc_dp_hw_get(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_ip_dscp_hw_get(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_eth_pcp_dei_hw_get(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_ilu_prd_map_mpls_exp_hw_get(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD threshold value per priority per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority for the threshold
 * \param [in] threshold - threshold value
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
imb_ilu_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    int hrf_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_hrf_size_get(unit, port, &hrf_size));

    if (threshold > hrf_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size %d\n",
                     threshold, priority, port, hrf_size);
    }
    SHR_IF_ERR_EXIT(imb_ilu_prd_threshold_hw_set(unit, port, priority, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Threshold value for the given priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to get threshold
 * \param [in] threshold - returned threshold
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
imb_ilu_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_threshold_hw_get(unit, port, priority, threshold));
exit:
    SHR_FUNC_EXIT;
}

int
imb_ilu_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    *threshold = dnx_data_nif.prd.hrf_threshold_max_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_ilu_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ilu_prd_tpid_hw_set
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_ilu_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ilu_prd_tpid_hw_get
                    (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD drop counter per sch_priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - scheduler priority. the PRD
 *        counter is per HRF. Only HRF=0 is used (Data HRF)
 * \param [out] count - counter value
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
imb_ilu_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint64 *count)
{
    uint32 is_tdm;
    bcm_port_nif_scheduler_t hrf_sch_priority;
    SHR_FUNC_INIT_VARS(unit);
    switch (sch_priority)
    {
        case bcmPortNifSchedulerLow:
        case bcmPortNifSchedulerHigh:
            is_tdm = 0;
            break;
        case bcmPortNifSchedulerTDM:
            is_tdm = 1;
            break;
        default:
            is_tdm = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_priority_get(unit, port, &hrf_sch_priority));
    if (sch_priority == hrf_sch_priority)
    {
        SHR_IF_ERR_EXIT(imb_ilu_prd_drop_count_hw_get(unit, port, is_tdm, count));
    }
    else
    {
        COMPILER_64_SET(*count, 0, 0);
    }
exit:
    SHR_FUNC_EXIT;

}

int
imb_ilu_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_ether_type_size_hw_set(unit, port, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

int
imb_ilu_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_ether_type_size_hw_get(unit, port, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

int
imb_ilu_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_key_offset_base_hw_set(unit, port, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}
int
imb_ilu_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_key_offset_base_hw_get(unit, port, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}
int
imb_ilu_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_key_offset_hw_set(unit, port, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;

}

int
imb_ilu_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_key_offset_hw_get(unit, port, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set an entry to the TCAM table in the PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM
 * \param [in] entry_info - info of the TCAM entry
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
imb_ilu_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_hw_set(unit, port, key_index, entry_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get an entry from the PRD soft stage TCAM table
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM table
 * \param [in] entry_info - returned entry info.
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
imb_ilu_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_tcam_entry_hw_get(unit, port, key_index, entry_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set control frame properties to be recognized by the
 *        PRD. if a control packet is recognized (each packet is
 *        compared against all control planes defined for the
 *        ILU), it gets the highest priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        configure (0-3)
 * \param [in] control_frame_config - control frame properties
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
imb_ilu_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_control_frame_hw_set(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Control frame properties recognized by the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get
 * \param [out] control_frame_config - returned control frame
 *        properties
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
imb_ilu_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_control_frame_hw_get(unit, port, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set MPLS special label properties. the special
 *        labels are compared globaly against all the MPLS ports
 *        of the same ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to set (0-3)
 * \param [in] label_config - special label configuration.
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
imb_ilu_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_mpls_special_label_hw_set(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get MPLS special label properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to get (0-3)
 * \param [out] label_config - label properties
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
imb_ilu_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_mpls_special_label_hw_get(unit, port, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set configurable ether type to be recognized by the
 *        PRD Parser
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the new ether type (1-6)
 * \param [in] ether_type_val - ether type value
 *
 * \return
 *   int  -see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ilu_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_custom_ether_type_hw_set(unit, port, ether_type_code - 1, ether_type_val));

    if (ether_type_code == ether_type_code_max)
    {
        /*
         * The last configurable ether type (with ether type code 7) is saved for TM port types. the way to save it is
         * to always configure ether type code 6 and ether type code 7 to the same value. so if the user want to
         * configure ether type code 6, we copy the same value to ether type code 7
         */
        SHR_IF_ERR_EXIT(imb_ilu_prd_custom_ether_type_hw_set(unit, port, ether_type_code, ether_type_val));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get ether type mapped to a specific configurable
 *        ether type  code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - configurable ether type code to
 *        get (1-6)
 * \param [in] ether_type_val - ether type value mapped to the
 *        given ether type code.
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
imb_ilu_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_prd_custom_ether_type_hw_get(unit, port, ether_type_code - 1, ether_type_val));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Link status change event handler
 *          If Alignment is stable enable RX data, otherwise disable RX data
 *
 * \param [in] unit - chip unit id.
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
/*Action on link status change*/
int
imb_ilu_port_link_status_change_event(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    uint32 data;
    int credits_lost = 0;
    /*
     * Note, we currently have ILKN data on single core devices only and
     * ilkn_access_info does not provide information about (device) core
     */
    int core = 0;
    soc_reg_above_64_val_t data_above_64;
    int hrf_offset[] = { 0, 2 };
    int ire_credit_reset_field[] = { FIELD_4_4f, FIELD_5_5f };
    int is_tdm;
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.scheduler.feature_get(unit, dnx_data_nif_scheduler_decrement_credit_on_read_enable))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

        for (is_tdm = 1; is_tdm >= 0; is_tdm--)
        {
            /** check underflow on data HRF */
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, ILU_HRF_RX_CONTROLLER_DEBUGr, ilkn_access_info.ilkn_core,
                                          ilkn_access_info.port_in_core + hrf_offset[is_tdm], &data));

            credits_lost = soc_reg_field_get(unit, ILU_HRF_RX_CONTROLLER_DEBUGr, data, HRF_RX_UNDERFLOW_ERR_HRF_Nf);
            if (credits_lost)
            {
                /** 1. disable RX data */
                SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));

                ilkn_hrf_type = (is_tdm ? DBAL_ENUM_FVAL_ILKN_HRF_TYPE_TDM : DBAL_ENUM_FVAL_ILKN_HRF_TYPE_DATA);
                /** 2. Flush transaction counter and reset HRF */
                SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hrf_flush_and_reset(unit, port,
                                                                              ilkn_access_info, ilkn_hrf_type));

                /** 3. Stop NMG RX scheduler */

                /*
                 * we don't read NMG_BIST_RX_SHAPER as SDK never changes it (except this code)
                 * and we can assume that it is in its reset value
                 */
                SOC_REG_ABOVE_64_CLEAR(data_above_64);
                soc_reg_above_64_field32_set(unit, NMG_BIST_RX_SHAPERr, data_above_64, BIST_RX_SHAPER_PIPE_0_CLEARf, 1);
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, NMG_BIST_RX_SHAPERr, core, 0, data_above_64));

                /** 4. reset IRE NIF credits */

                /*
                 * we don't read IRE_AUTO_DOC_NAME_2 as SDK never changes it (except this code)
                 * and we can assume that it is in its reset value
                 */
                data = 0;
                soc_reg_field_set(unit, IRE_AUTO_DOC_NAME_2r, &data, ire_credit_reset_field[is_tdm], 0x1);
                SHR_IF_ERR_EXIT(soc_reg32_set(unit, IRE_AUTO_DOC_NAME_2r, core, 0, data));

                data = 0;
                SHR_IF_ERR_EXIT(soc_reg32_set(unit, IRE_AUTO_DOC_NAME_2r, core, 0, data));

                /** 5. Resume NMG RX scheduler */
                SOC_REG_ABOVE_64_CLEAR(data_above_64);
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, NMG_BIST_RX_SHAPERr, core, 0, data_above_64));

                /*
                 * 6. Take RX HRF out-of-reset
                 */
                SHR_IF_ERR_EXIT(imb_ilu_hrf_controller_rx_hw_set
                                (unit, ilkn_access_info.ilkn_core, ilkn_access_info.port_in_core, ilkn_hrf_type,
                                 IMB_COMMON_OUT_OF_RESET));
            }
        }
    }
    SHR_IF_ERR_EXIT(imb_ile_port_link_status_change_event(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down of ILU
 */
static int
imb_ilu_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ilu_power_down_set(unit, ilkn_access_info.ilkn_core, power_down));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
