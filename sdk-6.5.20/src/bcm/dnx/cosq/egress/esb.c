/*
 * \file esb.c
 *
 *  Created on: March 26, 2019
 *      Author: vg889851
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>

/**
 * \brief - Function to set an entry in the ESB MUX calendar
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] slot - Calendar slot ID
 * \param [in] cal_entry - Calendar entry value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_esb_cal_entry_set(
    uint32 unit,
    bcm_core_t core,
    uint32 slot,
    uint32 cal_entry)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Setting the HW calendar
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_QUEUE_MUX, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, slot);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, INST_SINGLE, cal_entry);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to enable hardware to load new calendar settings to internal usage
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_esb_cal_load(
    uint32 unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Enable hardware to load new calendar settings to internal usage when it reaches the end of current calendar.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_CALENDER_SET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SWITCH_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Poll for "DONE" status of the loading of the new calendar.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_CALENDER_SET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_CALENDAR_SWITCH_STATUS, 1));

    /**
     * Disable hardware to load new calendar settings to internal usage when it reaches the end of current calendar.
     * We only need to load new calendar when there is a change.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_CALENDER_SET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SWITCH_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the configuration parameters for the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 * \param [out] esb_queue_id - ESB queue ID
 * \param [out] is_interleave - Indication if port is used for TDM interleave
 * \param [out] esb_intlv_queue_id - additional ESB queue ID for TDM
 * interleave (relevant only if is_interleave==TRUE)
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_esb_port_params_get(
    uint32 unit,
    bcm_port_t logical_port,
    uint32 *esb_queue_id,
    int *is_interleave,
    uint32 *esb_intlv_queue_id)
{
    dnx_algo_port_info_s port_info;
    int nif_if, ilkn_if, ilkn_first_phy;
    int idx, if_id, core;
    bcm_port_t master_port;
    int flexe_mac;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_egress_interleave_get(unit, logical_port, is_interleave));

    *esb_intlv_queue_id = ESB_QUEUE_ID_INVALID;

   /**
    * Determine the appropriate ESB queue ID according to port type and speed.
    */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 0))
    {
        /**
         * In case of NIF ETH interface the ESB queue ID is equal to the NIF interface ID.
         * For the configuration also Egress interface, NIF Unit ID and internal offset will be needed.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_if));
        nif_if = nif_if - core * dnx_data_nif.phys.nof_phys_per_core_get(unit);
        *esb_queue_id = nif_if;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
    {
        /**
         * FlexE MAC excluding L1
         *
         * In case of FlexE interface the ESB queue ID is equal to the FlexE with offset
         * the number of queues for NIF ETH and ILKN TDM dedicated queues.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &flexe_mac));
        *esb_queue_id =
            flexe_mac + dnx_data_esb.general.nof_nif_eth_queues_get(unit) +
            dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0))
    {
        /**
         * In case of NIF ILKN interface the ESB queue ID is equal to the ILKN interface ID with offset
         * the number of queues for NIF ETH, ILKN TDM dedicated queues and FlexE queues.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &ilkn_if));
        *esb_queue_id =
            ilkn_if + dnx_data_esb.general.nof_nif_eth_queues_get(unit) +
            dnx_data_esb.general.nof_flexe_queues_get(unit) + dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit);

       /**
        * In case of NIF ILKN interface that should support TDM interleave, if TDM interleave is enabled and the
        * TDM mode of the port is not TDM invalid an additional ESB queue ID must be allocated.
        * Allocate an additional ESB queue ID. All actions performed on the main queue will be performed on this one as well
        */
        if (dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit) != 0)
        {
           /**
            * In case dedicated queues for TDM interleave exist, the ESB queue ID should be the
            * ILKN interface ID with offset the number of queues for NIF ETH.
            */
            *esb_intlv_queue_id = ilkn_if + dnx_data_esb.general.nof_nif_eth_queues_get(unit);
        }
        else
        {
           /**
            * In case dedicated queues for TDM interleave don't exist, the ESB queue ID should be
            * one of the NIF which reside on the same PM as the ILKN, because they will not be used.
            */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &ilkn_first_phy));
            ilkn_first_phy = ilkn_first_phy - core * dnx_data_nif.phys.nof_phys_per_core_get(unit);
            *esb_intlv_queue_id = ilkn_first_phy;

        }
    }
    else
    {
        for (idx = 0; idx < dnx_data_esb.general.if_esb_params_info_get(unit)->key_size[0]; idx++)
        {
            if (dnx_data_esb.general.if_esb_params_get(unit, idx)->if_id == if_id)
            {
               /**
                * Determine the minimal gap for MUX access according to egress interface ID for special ports
                * In case of any type of special port the ESB queue ID and the minimal gap
                * for MUX access is pre-defined. Recycle interface is special in a way that it has 2 instances.
                * For each of them the values of ESB queue ID and minimal gap are different.
                */
                break;
            }
        }

        if (idx < dnx_data_esb.general.if_esb_params_info_get(unit)->key_size[0])
        {
           /**
            * Get parameters from DNX Data. For special ports a dedicated ESB queue will be returned, in all other
            * cases queue_id will be -1 and needs to be calculated based on port type.
            */
            *esb_queue_id = dnx_data_esb.general.if_esb_params_get(unit, idx)->queue_id;
            if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
            {
                *esb_intlv_queue_id = dnx_data_esb.general.if_esb_params_get(unit, idx)->interleave_queue_id;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF parameters are not found %d\n", logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the configuration parameters that depend on port speed for the Egress Shared Buffers
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 * \param [out] min_gap - Minimal gap for MUX calendar
 * \param [out] is_slow - Indication if port is 10G
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_esb_port_speed_params_get(
    uint32 unit,
    bcm_port_t logical_port,
    uint32 *min_gap,
    uint8 *is_slow)
{
    int rate_in_mbps;
    int has_speed, idx, if_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &if_id));
   /**
    * Determine if the port is considered "slow port".
    */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed == 0)
    {
        rate_in_mbps = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get
                        (unit, logical_port, DNX_ALGO_PORT_SPEED_F_MBPS | DNX_ALGO_PORT_SPEED_F_TX, &rate_in_mbps));
    }
    *is_slow = (rate_in_mbps <= dnx_data_egr_queuing.common_max_val.slow_port_speed_get(unit)) ? 1 : 0;

    for (idx = 0; idx < dnx_data_esb.general.if_esb_params_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_esb.general.if_esb_params_get(unit, idx)->if_id == if_id)
        {
           /**
            * Determine the minimal gap for MUX access according to egress interface ID for special ports
            * In case of any type of special port the ESB queue ID and the minimal gap
            * for MUX access is pre-defined. Recycle interface is special in a way that it has 2 instances.
            * For each of them the values of ESB queue ID and minimal gap are different.
            */
            break;
        }

        if ((rate_in_mbps <= dnx_data_esb.general.if_esb_params_get(unit, idx)->speed)
            && (dnx_data_esb.general.if_esb_params_get(unit, idx)->speed != -1))
        {
           /**
            * Determine the minimal gap for MUX access according to interface speed for all other ports
            */
            break;
        }
    }

    if (idx < dnx_data_esb.general.if_esb_params_info_get(unit)->key_size[0])
    {
       /**
        * Get parameters from DNX Data.
        */
        *min_gap = dnx_data_esb.general.if_esb_params_get(unit, idx)->min_gap;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF parameters are not found %d\n", logical_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_init(
    uint32 unit)
{
    uint32 entry_handle_id;
    uint32 last_buff = 0;
    uint32 fpc_id = 0;
    bcm_core_t core_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Init Egress Shared Buffers block. The bit is not self raised so it after the reset it needs to be raised again
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFF_ALLOC_RSTN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Configure ranges for the Free Pointer Controllers.
     * The total Egress Shared Buffers are divided equally between the controllers.
     * Each controller starts with the appropriate offset from the previous one,
     * so the range for all of them should be 0 to (nof_buffers/nof_fpc -1)
     * The init bit is not self cleared so after the init it needs to be cleared.
     */
    last_buff = dnx_data_esb.general.total_nof_buffers_get(unit) / dnx_data_esb.general.nof_fpc_get(unit) - 1;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_FPC_RANGE_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    for (fpc_id = 0; fpc_id < dnx_data_esb.general.nof_fpc_get(unit); fpc_id++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_ID, fpc_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_BUFF_PTR, INST_SINGLE, last_buff);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /**
     * Init Egress Shared Buffers block. The bit is not self raised so it after the reset it needs to be raised again
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_INIT, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUFF_ALLOC_RSTN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Poll for "DONE" status of the initialization process of
     * Tail Pointer Memory, Head Pointer Memory and End of Burst Memory.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_INIT, entry_handle_id));
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_TPM_INIT_STATUS, 0));
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_HPM_INIT_STATUS, 0));
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_EOBM_INIT_STATUS, 0));
    }

    /**
     * Initialize the TXI logic for all NIF interfaces.
     * This is done so that all TXIs will be initialized to 0 credits.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_PER_NIF_IF_CREDITS_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_INIT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Track all ESB queues by default
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_COUNTER_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_port_add(
    uint32 unit,
    bcm_port_t logical_port)
{
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;
    uint32 esb_intlv_queue_id = 0, esb_queue_id = 0;
    int nif_if, egr_if, ilkn_id;
    int is_interleave, core;
    bcm_port_t master_port;
    int nif_unit_id, internal_offset, nof_channels;
    uint32 current_queue;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, logical_port, &nof_channels));
    if (nof_channels > 1)
    {
        SHR_EXIT();
    }

    /**
     * Get configuration parameters for the specified port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_esb_port_params_get(unit, logical_port, &esb_queue_id, &is_interleave, &esb_intlv_queue_id));

   /**
    * Set configuration to HW.
    */
    current_queue = esb_queue_id;
    while (current_queue != ESB_QUEUE_ID_INVALID)
    {
       /**
        * Activate the chosen queue.
        */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_ADD_QUEUE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_QUEUE_ID, INST_SINGLE, current_queue);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_QUEUE_EN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * Poll for "DONE" status of the process of allocating new buffer for the new Queue added.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_ADD_QUEUE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_ENQ_ADD_QUEUE_DONE, 1));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_ADD_QUEUE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_QUEUE_EN, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * If the current queue is TDM ILKN interleave, additional enabler needs to be set.
         */
        if ((current_queue == esb_intlv_queue_id) && (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0)))
        {
            /*
             * Set ILKN TDM queue ID even if there's no interleaving - this is required due to a HW issue:
             * Design always tries to dequeue from this queue even if interleaving is off
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &ilkn_id));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_ILKN_TDM_QUEUE_EN, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_QUEUE_ID, INST_SINGLE, current_queue);
            if (is_interleave == TRUE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_INTLV_EN, INST_SINGLE, 1);
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /**
         * NIF excluding ELK, STIF, L1 and FlexE Phy || FlexE MAC excluding L1
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 0, 0, 0, 0)
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, master_port, 0, &core, &nif_if));
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, master_port, &egr_if));
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_unit_id_get(unit, logical_port, &nif_unit_id, &internal_offset));

            /**
             * Mapping of Egress interface to ESB queue.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_EGQ_IF_2_Q_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Q_NUM, INST_SINGLE, esb_queue_id);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

           /**
            * Mapping of ESB queue to NIF and Egress interface.
            */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_Q_2_EGQ_IF_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, current_queue);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_IF, INST_SINGLE, egr_if);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, INST_SINGLE, nif_if);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_MACRO, INST_SINGLE, nif_unit_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_PORT, INST_SINGLE, internal_offset);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /**
         * In the case of TDM ILKN interleave, all actions relevant for ILKN queue must also be performed for the ILKN queue.
         */
        if (current_queue != esb_intlv_queue_id)
        {
            current_queue = esb_intlv_queue_id;
        }
        else
        {
            current_queue = ESB_QUEUE_ID_INVALID;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_port_speed_config_add(
    uint32 unit,
    bcm_port_t logical_port)
{
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;
    uint32 esb_intlv_queue_id = 0, esb_queue_id = 0;
    uint8 is_slow = 0;
    int is_interleave, core;
    uint32 min_gap, current_queue;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Get configuration parameters for the specified port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_esb_port_params_get(unit, logical_port, &esb_queue_id, &is_interleave, &esb_intlv_queue_id));
    SHR_IF_ERR_EXIT(dnx_esb_port_speed_params_get(unit, logical_port, &min_gap, &is_slow));

   /**
    * Set configuration to HW.
    */
    current_queue = esb_queue_id;
    while (current_queue != ESB_QUEUE_ID_INVALID)
    {
        /**
         * Set the per Queue configurations in ESB.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_PER_QUEUE_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, current_queue);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_GAP, INST_SINGLE, min_gap);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_IS_SLOW, INST_SINGLE, is_slow);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_IS_ACTIVE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * In the case of TDM ILKN interleave, all actions relevant for ILKN queue must also be performed for the ILKN queue.
         */
        if (current_queue != esb_intlv_queue_id)
        {
            current_queue = esb_intlv_queue_id;
        }
        else
        {
            current_queue = ESB_QUEUE_ID_INVALID;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the nif port id, used to init the credit size
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port
 * \param [out] nif_port_id - nif port id
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_esb_credits_nif_port_get(
    uint32 unit,
    bcm_port_t logical_port,
    uint32 *nif_port_id)
{
    uint32 esb_intlv_queue_id = 0, esb_queue_id = 0;
    int is_interleave;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_esb_port_params_get(unit, logical_port, &esb_queue_id, &is_interleave, &esb_intlv_queue_id));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0))
        && (dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit) != 0))
    {
         /**
          * The index to the TXI logic and IRDY table is the NIF ESB Qs (ETH, FlexE and ILKN).
          * However if ESB Qs that are dedicated for TDM interleave exist ILKN should be set with
          * lower (TDM) IDs since they correspond to the the NIF port Id for ILKN port.
          */
        *nif_port_id = esb_intlv_queue_id;
    }
    else
    {
        *nif_port_id = esb_queue_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_port_txi_config_set(
    uint32 unit,
    bcm_port_t logical_port,
    int is_add)
{
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;
    uint8 is_slow = 0;
    uint32 txi_irdy_th, min_gap, nif_port_id;
    int core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Get configuration parameters for the specified port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    SHR_IF_ERR_EXIT(dnx_esb_port_speed_params_get(unit, logical_port, &min_gap, &is_slow));

    /**
     * Set configuration to HW for
     * NIF excluding ELK, STIF, L1 and FlexE Phy || FlexE MAC excluding L1
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 0, 0, 0, 0) || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
    {
         /**
          * Initialize the TXI logic for NIF interface for both NIF ETH and NIF ILKN.
          * Calculate and set the IRDY threshold
          */
        if (is_add)
        {
            txi_irdy_th =
                (UTILEX_DIV_ROUND_DOWN(dnx_data_esb.general.txi_irdy_th_divident_get(unit), (min_gap + 1)) +
                 1) * dnx_data_esb.general.txi_irdy_th_factor_get(unit) - 1;
        }
        else
        {
            txi_irdy_th = 0;
        }
        SHR_IF_ERR_EXIT(dnx_esb_credits_nif_port_get(unit, logical_port, &nif_port_id));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_PER_NIF_IF_CREDITS_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, nif_port_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_INIT, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_IRDY_THRESH, INST_SINGLE, txi_irdy_th);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
         /**
          * Field is not self-clear, so it needs to be restored back to 0
          */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_INIT, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_port_remove(
    uint32 unit,
    bcm_port_t logical_port)
{
    uint32 entry_handle_id;
    dnx_algo_port_info_s port_info;
    uint32 esb_intlv_queue_id = 0, esb_queue_id = 0;
    int egr_if, ilkn_id, nof_channels;
    int is_interleave, core;
    bcm_port_t master_port;
    uint32 current_queue, nif_esb_q;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**
     * Get configuration parameters for the specified port.
     */
    SHR_IF_ERR_EXIT(dnx_esb_port_params_get(unit, logical_port, &esb_queue_id, &is_interleave, &esb_intlv_queue_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, logical_port, &nof_channels));
    if (nof_channels > 1)
    {
        SHR_EXIT();
    }

   /**
    * Set configuration to HW.
    */
    current_queue = esb_queue_id;
    while (current_queue != ESB_QUEUE_ID_INVALID)
    {
       /**
        * Free buffers from the head of the chosen queue.
        */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_REMOVE_QUEUE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMOVE_QUEUE_ID, INST_SINGLE, current_queue);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMOVE_QUEUE_EN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * Poll for "DONE" status of the process of removing the Queue and freeing its buffers.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_REMOVE_QUEUE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        SHR_IF_ERR_EXIT(dnxcmn_polling
                        (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_REMOVE_QUEUE_DONE, 1));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_REMOVE_QUEUE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMOVE_QUEUE_EN, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * If the current queue is TDM ILKN interleave, additional enabler needs to be set to disabled.
         */
        if ((current_queue == esb_intlv_queue_id) && (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0)))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &ilkn_id));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_ILKN_TDM_QUEUE_EN, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_ID, ilkn_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_QUEUE_ID, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_INTLV_EN, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /**
         * Set the per Queue configurations in ESB to not active.
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_PER_QUEUE_CONFIG, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, current_queue);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_GAP, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_IS_SLOW, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_IS_ACTIVE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /**
         * NIF excluding ELK, STIF, L1 and FlexE Phy || FlexE MAC excluding L1
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 0, 0, 0, 0)
            || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, master_port, &egr_if));

            /**
             * Mapping of Egress interface to ESB queue needs to be cleared.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_EGQ_IF_2_Q_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Q_NUM, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /**
             * Mapping of ESB queue to NIF and Egress interface needs to be cleared.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_Q_2_EGQ_IF_MAP, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, current_queue);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_IF, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_MACRO, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTERNAL_PORT, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0))
                && (dnx_data_esb.general.nof_ilkn_tdm_queues_get(unit) != 0))
            {
                 /**
                  * The index to the TXI logic and IRDY table is the NIF ESB Qs (ETH, FlexE and ILKN).
                  * However if ESB Qs that are dedicated for TDM interleave exist ILKN should be set with
                  * lower (TDM) IDs since they correspond to the the NIF port Id for ILKN port.
                  */
                nif_esb_q = esb_intlv_queue_id;
            }
            else
            {
                nif_esb_q = current_queue;
            }

             /**
              * TXI threshold for NIF interface for both NIF ETH and NIF ILKN must be cleared.
              */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_PER_NIF_IF_CREDITS_CONFIG, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, nif_esb_q);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_INIT, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_IRDY_THRESH, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /**
         * In the case of TDM ILKN interleave, all actions relevant for ILKN queue must also be performed for the ILKN queue.
         */
        if (current_queue != esb_intlv_queue_id)
        {
            current_queue = esb_intlv_queue_id;
        }
        else
        {
            current_queue = ESB_QUEUE_ID_INVALID;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_esb_nif_rate_to_weight(
    int unit,
    uint32 rate,
    uint32 *weight_p,
    uint8 is_l1)
{
    uint8 flexe_supported_and_enabled = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (rate == 0)
    {
        *weight_p = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_supported_and_enabled_get(unit, &flexe_supported_and_enabled));
        if (flexe_supported_and_enabled == TRUE)
        {
            /**
             * If FlexE is used, calculate weight using different calendar
             * resolution than the EGQ calendar. This is done because unlike the EGQ,
             * the ESB also includes L1 ports and finer tuning is needed in order to support them.
             */
            if (is_l1 == TRUE)
            {
               /**
                * L1 FLEXE traffic is treated as TDM so it must not drop any packets - even when there is over subscription.
                * So we need to make sure that non FLEXE ports will be the ones that drop. In order to achieve this, the allocated slots
                * for L1 FLEXE-TO-ETH ESB Q in MUX calendar should be twice the actual required speed.
                */
                rate = UTILEX_DIV_ROUND_UP(rate, dnx_data_esb.general.l1_mux_cal_res_get(unit));
                *weight_p = rate * 2;
            }
            else
            {
                /**
                 * Dividing rate by the MUX calendar resolution and rounding up to power of 2
                 */
                rate = UTILEX_DIV_ROUND_UP(rate, dnx_data_esb.general.tm_egr_queuing_mux_cal_res_get(unit));
                *weight_p = utilex_power_of_2(utilex_log2_round_up(rate));
            }
        }
        else
        {
            /**
             * If FlexE is not used, calculate weight according to the interface rate
             * using the same formula and parameters as in EGQ calendar
             */
            SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_rate_to_weight(unit, rate, weight_p));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_esb_nif_calendar_set(
    int unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    bcm_port_t port;
    int if_rate_mbps;
    bcm_pbmp_t port_bm, port_bm_l1;
    uint32 cal_len_optimal, nof_cal_objects;
    uint32 weights[BCM_PBMP_PORT_MAX], lcm_temp[BCM_PBMP_PORT_MAX];
    dnx_ofp_rates_cal_sch_t *calendar = NULL;
    uint32 max_field_value, dummy_start = 0;
    dnx_algo_port_info_s interface_type;
    int is_interleave, has_speed, is_master_port, ii;
    int max_slow_port_rate = 0, nof_slow_ports = 0;
    int total_flexe_l1_dest_queue_rate = 0, nof_flexe_l1_dest = 0;
    uint32 esb_intlv_queue_id = 0, esb_queue_id = 0, min_gap;
    uint8 is_slow_port = 0;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(lcm_temp, 0, sizeof(lcm_temp));
    sal_memset(weights, 0, sizeof(weights));
    SHR_ALLOC(calendar, sizeof(*calendar), "calendar esb_mux_calendar", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        /**
         * Get all Master logical ports for of type Egr queuing and L1 on specified core. Will be used to iterate and calculate the
         * per port weights.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_id, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING,
                                                   DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &port_bm));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_id, DNX_ALGO_PORT_LOGICALS_TYPE_L1,
                                                   DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &port_bm_l1));
        BCM_PBMP_OR(port_bm, port_bm_l1);

        BCM_PBMP_ITER(port_bm, port)
        {
            /**
             * Get interface type for the current port. ESB configuration is done only for Egress TM ports, FlexE of L1 ports
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &interface_type));
            if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, interface_type)
                && !DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, interface_type))
            {
                continue;
            }

            /**
             * Get interface rate, only interfaces that have speed must be added to the calendar.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
            if (has_speed == 0)
            {
                if_rate_mbps = 0;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &if_rate_mbps));
            }

            /**
             * Weight is calculated per interface.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
            if (!is_master_port)
            {
                continue;
            }

            /**
             * Get the ESP parameters of the port, the index for the weights array is the ESB Q
             */
            SHR_IF_ERR_EXIT(dnx_esb_port_params_get(unit, port, &esb_queue_id, &is_interleave, &esb_intlv_queue_id));
            SHR_IF_ERR_EXIT(dnx_esb_port_speed_params_get(unit, port, &min_gap, &is_slow_port));
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, interface_type))
            {
                /**
                 * Ports that are used for the destination of the FlexE->ETH L1 path are grouped together and are
                 * represented by a single queue in the ESB MUX calendar.
                 */
                total_flexe_l1_dest_queue_rate += if_rate_mbps;
                nof_flexe_l1_dest++;
            }
            else if ((is_slow_port == TRUE)
                     && (dnx_data_esb.
                         general.feature_get(unit, dnx_data_esb_general_slow_port_dedicated_queue_support) == 1))
            {
                /**
                 * Ports under a certain port speed are grouped together and are represented all as one in the MUX calendar
                 * with total rate of max_rate*nof_if
                 */
                max_slow_port_rate = UTILEX_MAX(max_slow_port_rate, if_rate_mbps);
                nof_slow_ports++;
            }
            else
            {
                /**
                 * Calculate weight according to the interface rate
                 */
                SHR_IF_ERR_EXIT(dnx_esb_nif_rate_to_weight(unit, if_rate_mbps, &weights[esb_queue_id], FALSE));
            }
        }

        if (nof_flexe_l1_dest != 0)
        {
            /**
             * Calculate the weight of the dedicated queue for FlexE->ETH L1 path.
             */
            SHR_IF_ERR_EXIT(dnx_esb_nif_rate_to_weight
                            (unit, total_flexe_l1_dest_queue_rate,
                             &weights[dnx_data_esb.general.flexe_l1_dest_dedicated_queue_get(unit)], TRUE));
        }

        if (nof_slow_ports != 0)
        {
            /**
             * Calculate the weight of the dedicated queue for Slow Ports
             */
            SHR_IF_ERR_EXIT(dnx_esb_nif_rate_to_weight
                            (unit, (max_slow_port_rate * nof_slow_ports),
                             &weights[dnx_data_esb.general.slow_port_dedicated_queue_get(unit)], FALSE));
        }

        /**
         * Calculate the calendar parameters
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_cal_params_calc
                        (unit, weights, dnx_data_esb.general.mux_cal_len_get(unit), &cal_len_optimal,
                         &nof_cal_objects, &dummy_start));

        /**
         * Given the optimal calendar length and the
         * corresponding weight (in slots) of each port,
         * build a calendar that will avoid burstiness
         * behavior as much as possible.
         */
        SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit, weights, nof_cal_objects,
                                                                cal_len_optimal,
                                                                dnx_data_esb.general.mux_cal_len_get(unit),
                                                                calendar->slots));

        /**
         * Get field max value to use it as invalid value
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_QUEUE_MUX, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                        (unit, entry_handle_id, DBAL_FIELD_ESB_Q_NUM, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_field_value));

        /**
         * Replace dummies and the rest of calendar with invalid IFs and write the values to HW
         */
        for (ii = 0; ii < dnx_data_esb.general.mux_cal_len_get(unit); ii++)
        {
            if (ii >= cal_len_optimal || calendar->slots[ii] >= dummy_start)
            {
                calendar->slots[ii] = max_field_value;
            }

            /**
             * Set the value of ESB_ESB_QUEUE_MUX
             */
            SHR_IF_ERR_EXIT(dnx_esb_cal_entry_set(unit, core_id, ii, calendar->slots[ii]));
        }

        /**
         * Enable hardware to load new calendar settings to internal usage
         */
        SHR_IF_ERR_EXIT(dnx_esb_cal_load(unit, core_id));
    }
exit:
    SHR_FREE(calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_esb_ilkn_credits_set(
    int unit,
    bcm_port_t port,
    int max_credits)
{
    int if_speed_mbps;
    uint32 entry_handle_id;
    uint32 nif_port_id;
    int credits_size, core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &if_speed_mbps));
    /** calc credit size */
    credits_size = (if_speed_mbps / dnx_data_esb.general.ilkn_credits_rate_resolution_get(unit)) *
        dnx_data_esb.general.ilkn_credits_resolution_get(unit);
    /** there is minimum value of credit size */
    credits_size = UTILEX_MAX(credits_size, dnx_data_esb.general.ilkn_credits_resolution_get(unit));
    /** in any case, do not cross the max value */
    credits_size = UTILEX_MIN(credits_size, max_credits);

    SHR_IF_ERR_EXIT(dnx_esb_credits_nif_port_get(unit, port, &nif_port_id));
    /** set the credit size */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESB_CREDIT_SIZE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_SIZE, INST_SINGLE, credits_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set to one in order to load the credit size, and back to zero */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_ESB_PER_NIF_IF_CREDITS_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_IF, nif_port_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_INIT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
