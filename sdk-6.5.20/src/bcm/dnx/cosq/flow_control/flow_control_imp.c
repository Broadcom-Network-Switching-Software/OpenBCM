/** \file src/bcm/dnx/cosq/flow_control/flow_control_imp.c
 * 
 *
 * Flow control callback functions for DNX.\n
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/flow_control_access.h>
#include <soc/portmod/portmod.h>
#include <bcm_int/dnx/cosq/flow_control/cosq_fc.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include "bcm_int/dnx/cosq/flow_control/flow_control_imp.h"
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * Defines
 * {
 */

#define DNX_COE_VID_OFFSET  6

/**
 * \brief - Number of STE VSQs per core. From VSQs B only categories 2 and 3 are used to generate Flow Control
 */
#define NOF_STE_VSQS_PER_CORE (dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS)->nof + \
                              (dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_CNCTN_CLS)->nof/2) + \
                               dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY)->nof)

/**
 * \brief - Choose the appropriate calendar table according to the input Gport
 */
#define GPORT_REC_CAL_TABLE_GET(gport) (BCM_GPORT_IS_CONGESTION(gport) ? (BCM_GPORT_IS_CONGESTION_COE(gport) ? DBAL_TABLE_FC_REC_COE_CAL_MAP : DBAL_TABLE_FC_REC_OOB_CAL_MAP) : DBAL_TABLE_FC_REC_INB_ILKN_CAL_MAP);
#define GPORT_GEN_CAL_TABLE_GET(gport) (BCM_GPORT_IS_CONGESTION(gport) ? DBAL_TABLE_FC_GEN_OOB_CAL_MAP : DBAL_TABLE_FC_GEN_INB_ILKN_CAL_MAP);

/*
 * }
 */

/*
 * Functions
 * {
 */

/**
 * See .h file
 */
shr_error_e
dnx_fc_cfc_global_enable_set(
    int unit,
    uint8 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set CFC Global Enabler for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CFC_ENABLERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_cfc_global_enable_get(
    int unit,
    uint8 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get CFC Global Enabler for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CFC_ENABLERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_pfc_refresh_set(
    int unit,
    bcm_port_t port,
    int value)
{
    portmod_pfc_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if (value == 0)
    {                /** disable PFC refresh */
        control.refresh_timer = -1;
    }
    else
    {
        control.refresh_timer = value;
    }
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_pfc_refresh_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    portmod_pfc_control_t control;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &control));
    if (control.refresh_timer == -1)
    {                                 /** PFC refresh is disabled*/
        *value = 0;
    }
    else
    {
        *value = control.refresh_timer;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_inband_mode_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 *llfc_enabled,
    uint32 *pfc_enabled)
{
    portmod_pfc_control_t pfc_control;
    portmod_pause_control_t pause_control;
    SHR_FUNC_INIT_VARS(unit);

        /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &pfc_control));
    SHR_IF_ERR_EXIT(portmod_port_pause_control_get(unit, port, &pause_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        *pfc_enabled = pfc_control.tx_enable;
        *llfc_enabled = pause_control.tx_enable;
    }
    else
    {
        *pfc_enabled = pfc_control.rx_enable;
        *llfc_enabled = pause_control.rx_enable;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_inband_mode_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_fc_direction_type_t fc_direction,
    uint32 llfc_enabled,
    uint32 pfc_enabled)
{
    portmod_pfc_control_t pfc_control;
    portmod_pause_control_t pause_control;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    if (pfc_enabled == 1 && llfc_enabled == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode can not be both PFC and LLFC");
    }

    /** Enable / Disable - LL */
    SHR_IF_ERR_EXIT(portmod_port_pause_control_get(unit, port, &pause_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        pause_control.tx_enable = llfc_enabled;
    }
    else
    {
        pause_control.rx_enable = llfc_enabled;
    }
    SHR_IF_ERR_EXIT(portmod_port_pause_control_set(unit, port, &pause_control));

    /** Enable / Disable - PFC*/
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_get(unit, port, &pfc_control));
    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        pfc_control.tx_enable = pfc_enabled;
    }
    else
    {
        pfc_control.rx_enable = pfc_enabled;
    }
    SHR_IF_ERR_EXIT(portmod_port_pfc_control_set(unit, port, &pfc_control));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_port_mac_sa_set(
    int unit,
    int port,
    bcm_mac_t mac_sa)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_tx_mac_sa_set(unit, port, mac_sa));
    SHR_IF_ERR_EXIT(portmod_port_rx_mac_sa_set(unit, port, mac_sa));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_port_mac_sa_get(
    int unit,
    int port,
    bcm_mac_t mac_sa)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_rx_mac_sa_get(unit, port, mac_sa));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_pfc_status_get(
    int unit,
    bcm_port_t port,
    int *pfc_bitmap)
{
    uint32 entry_handle_id;
    uint32 *status = NULL;
    int qpair, base_qpair, max_qpair;
    uint32 select, select_prev, bit_idx, alloc_size;
    uint8 pfc_idx = 0, enabled = 0;
    int nof_presented;
    int num_priorities;
    bcm_core_t core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pfc_bitmap, _SHR_E_PARAM, "pfc_bitmap");

    /*
     * Make sure the PFC bitmap is cleared
     */
    sal_memset(pfc_bitmap, 0, sizeof(int));

    /*
     * Get core, base Q-pair and number of priorities from the port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qpair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

    /*
     * Get number of bits presented at once from the DBAL table to calculate
     * the size of the "status" variable and the value of the "select"
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_FC_STATUS, DBAL_FIELD_PFC_STATUS, FALSE, 0, 0, &nof_presented));

    /*
     * Get the appropriate size and allocate the needed memory for the status
     */
    if ((nof_presented % SHR_BITWID) == 0)
    {
        alloc_size = nof_presented / SHR_BITWID;
    }
    else
    {
        alloc_size = (nof_presented / SHR_BITWID) + 1;
    }
    SHR_ALLOC_SET_ZERO(status, sizeof(uint32) * (alloc_size), "status", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Set the select_prev to initial value to ensure first iteration will read the HW
     */
    select_prev = 0xFFFF;

    /*
     * Calculate the max Q-pair that needs to be checked
     */
    max_qpair = base_qpair + (num_priorities - 1);
    for (qpair = base_qpair; qpair <= max_qpair; qpair++)
    {
        /*
         * Calculate the "select" for the current Q-pair
         */
        select = qpair / nof_presented;

        /*
         * Read the status from HW again only is the select is changed.
         * This means that the current ID is not in the range of the current select
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_STATUS_SELECT, &entry_handle_id));
        if (select != select_prev)
        {
            /*
             * Set the appropriate select for the bits to be presented according to the port's Q-pairs
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_STATUS_SELECT, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_STATUS_SEL, INST_SINGLE, select);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * Get the current status for the port's core
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_STATUS, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PFC_STATUS, INST_SINGLE, status);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * Registers for EGQ PFC status and NIF PFC status are both 'sticky' and will clear on read only.
             * Changing the STATUS_SEL for each of them will result with an OR between two different ranges of the PFC vector.
             * This is wrong behavior and can be avoided by reading the registers twice for the same select value in order to clear them.
             */
            if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_status_regs_double_read))
            {
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            }

            /*
             * Set the select_prev
             */
            select_prev = select;
        }

        /*
         * Get the bit corresponding of the requested Q-pair
         */
        bit_idx = qpair % nof_presented;
        enabled = SHR_BITGET(status, bit_idx) ? 1 : 0;

        /*
         * If the relevant bit was up, set the indication for the current
         * priority in the PFC bitmap
         */
        if (enabled == TRUE)
        {
            pfc_idx = qpair - base_qpair;
            SHR_BITSET(pfc_bitmap, pfc_idx);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to set the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] core - core ID
 *   \param [in] is_delete - indicates if we want to add or delete path
 *   \param [in] cosq_min - start COSQ from which to start setting the bits in the generic bitmap
 *   \param [in] cosq_max - end COSQ up to which to set the bits in the generic bitmap
 *   \param [in] bitmap_index_old - index of one of the 32 PFC Generic Bitmaps that was previously used
 *   \param [in] bitmap_index_new - index of one of the 32 PFC Generic Bitmaps that will be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_pfc_generic_bmp_hw_set(
    int unit,
    bcm_core_t core,
    int is_delete,
    int cosq_min,
    int cosq_max,
    int bitmap_index_old,
    int bitmap_index_new)
{
    uint32 entry_handle_id;
    uint32 qpair, enabled = 0;
    bcm_core_t core_iter;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** If a new bitmap index was allocated, iterate over the generic bitmap specified with
     *  bitmap_index_old to get the Qpairs with enabled FC then copy them to the bitmap specified with bitmap_index_new */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    if (bitmap_index_old != bitmap_index_new)
    {
        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_iter)
        {
            for (qpair = 0; qpair < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); qpair++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_old);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (enabled)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_new);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enabled);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
            }
        }
    }

    /** Set the appropriate bits for Q-pairs of the port in the generic bitmap */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index_new);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    for (qpair = cosq_min; qpair <= cosq_max; qpair++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GENERIC_BITMAP, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the targets from the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] bitmap_index - index of one of the 32 PFC Generic Bitmaps
 *   \param [in] core_in - core
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object array
 *   \param [out] target_count - updated size(valid entries) of target array
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_pfc_generic_bmp_targets_get(
    int unit,
    uint32 bitmap_index,
    bcm_core_t core_in,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 qpair, enabled;
    bcm_port_t port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Itterate over the generic bitmap specified with bitmap_index to get the Qpairs with enabled FC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GENERIC_BITMAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BMP_IDX, bitmap_index);
    DNXCMN_CORES_ITER(unit, core_in, core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (qpair = 0; qpair < dnx_data_egr_queuing.params.nof_q_pairs_get(unit); qpair++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, qpair);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (enabled)
            {
                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                /** From the Qpairs with enabled FC get the associated Port+Cosq*/
                SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get
                                (unit, qpair, core, &port, &target[*target_count].cosq));
                BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                *target_count += 1;
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the cosq_nim, cosq_max and is_bitmap needed to set the PFC Generic Bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] target - holds information for the targeted reaction point
 *   \param [in] is_cal - indicate if the reception is from Calendar(OOB, COE, Inband ILKN) or PFC
 *   \param [out] cosq_min - start COSQ from which to start setting the bits in the generic bitmap
 *   \param [out] cosq_max - end COSQ up to which to set the bits in the generic bitmap
 *   \param [out] is_bitmap - indicates if bitmap should be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_rec_is_bmp_get(
    int unit,
    bcm_cosq_fc_endpoint_t * target,
    uint8 is_cal,
    int *cosq_min,
    int *cosq_max,
    uint8 *is_bitmap)
{
    bcm_port_t port = 0;
    int base_q_pair;
    int num_priorities;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Get the priorities and base Q-pair*/
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

    if (target->cosq >= num_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
    }

    /** When this flag is specified we always use PFC Generic Bitmap */
    if (target->flags & BCM_COSQ_FC_PORT_OVER_PFC)
    {
        if (target->cosq == -1)
        {
                        /**  Reaction point is Port */
            *cosq_min = base_q_pair;
            *cosq_max = base_q_pair + (num_priorities - 1);
        }
        else
        {
            /**  Reaction point is Port+COSQ using PFC Generic Bitmap */
            /** Inheritance is enabled */
            if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
            {
                /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
                if (target->flags & BCM_COSQ_FC_INHERIT_UP)
                {
                    *cosq_min = base_q_pair + target->cosq;
                    *cosq_max = base_q_pair + (num_priorities - 1);
                }
                else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
                {
                    *cosq_min = base_q_pair;
                    *cosq_max = base_q_pair + target->cosq;
                }
            }
            else
            {
                /** No Inheritance */
                *cosq_min = base_q_pair + target->cosq;
                *cosq_max = base_q_pair + target->cosq;
            }
        }

        *is_bitmap = TRUE;

    }
    else
    {
        if (target->cosq == -1)
        {
            if (is_cal)
            {
                                /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                *is_bitmap = FALSE;
            }
            else
            {
                if (num_priorities == 1)
                {
                                        /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                    *is_bitmap = FALSE;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid Flags, for Port reaction point with port priorities different than 1 need to set BCM_COSQ_FC_PORT_OVER_PFC.");
                }
            }
        }
        else
        {
                        /**  Reaction point is Port+COSQ with no BCM_COSQ_FC_PORT_OVER_PFC*/
                        /** Inheritance is enabled */
            if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
            {
                                /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
                if (target->flags & BCM_COSQ_FC_INHERIT_UP)
                {
                    *cosq_min = base_q_pair + target->cosq;
                    *cosq_max = base_q_pair + (num_priorities - 1);
                }
                else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
                {
                    *cosq_min = base_q_pair;
                    *cosq_max = base_q_pair + target->cosq;
                }

                *is_bitmap = TRUE;

            }
            else
            {
                                /** No Inheritance */
                *is_bitmap = FALSE;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to check if bitmap is already allocated and return its index.
 *  If it is not allocated, allocate a new one
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] source - holds information for the source of the FC
 *   \param [in] target - holds information for the target of the FC
 *   \param [in] is_cal - indicate if the reception is from Calendar(OOB, COE, Inband ILKN) or PFC
 *   \param [out] bitmap_index_old - index of one of the 32 PFC Generic Bitmaps that was previously used
 *   \param [out] bitmap_index_new - index of one of the 32 PFC Generic Bitmaps that will be used
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_rec_bmp_index_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target,
    uint8 is_cal,
    int *bitmap_index_old,
    int *bitmap_index_new)
{
    dnx_pfc_generic_bmp_t new_data;
    dnx_pfc_generic_bmp_t old_data;
    int qpair;
    int base_q_pair;
    int num_priorities;
    int ref_count;
    uint32 entry_handle_id;
    bcm_port_t src_port = 0;
    bcm_port_t tar_port = 0;
    bcm_port_t port_i = 0;
    bcm_port_t master_port;
    int nif_port, ilkn_port;
    int cal_if = 0;
    bcm_core_t core;
    uint32 map_sel;
    uint32 map_val;
    uint8 is_first, is_last;
    uint8 is_bitmap;
    int channelized;
    int cosq_min = 0;
    int cosq_max = 0;
    bcm_pbmp_t channels;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Prepare the bitmap variable */
    sal_memset(&new_data, 0, sizeof(dnx_pfc_generic_bmp_t));

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    if (is_cal)
    {
        /** In case of calendar determine the source Calendar(OOB, COE, Inband ILKN) interface */
        if (BCM_GPORT_IS_CONGESTION(source->port))
        {
            cal_if = BCM_GPORT_CONGESTION_GET(source->port);
        }
        else
        {
            /** Determine the target ILKN interface */
            ilkn_port =
                BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->port) :
                BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
        }
        /** Choose the appropriate calendar table according to the input Gport */
        cal_table = GPORT_REC_CAL_TABLE_GET(source->port);
        /** Core is necessary for the allocation of the resource manager. */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, tar_port, &core));

        /** Check if bitmap is already selected for this port */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, &map_sel);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, cal_table, entry_handle_id));
    }
    else
    {

        /** Determine the source port - Return the local port and then find the NIF port mapped to it */
        src_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
        nif_port = nif_port - core * nof_phys_per_core;

        /** Check if bitmap is already selected for this port in the PFC table*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, &map_sel);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_PFC_MAP, entry_handle_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, tar_port, &channelized));
    /** Interface is channelized */
    if (channelized && is_cal)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, tar_port, 0, &channels));
        /** For each channel set the appropriate bits in the FC generic bitmap */
        _SHR_PBMP_ITER(channels, port_i)
        {
            /** Get the base Q-pair and priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &num_priorities));

            cosq_min = base_q_pair;
            cosq_max = base_q_pair + (num_priorities - 1);

            /** Create the PFC Generic Bitmap */
            for (qpair = cosq_min; qpair <= cosq_max; qpair++)
            {
                SHR_BITSET(&new_data.qpair_bits[core][0], qpair);
            }
        }
    }
    else
    {
        /** Get the relevant cosq_min and cosq_max */
        SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, is_cal, &cosq_min, &cosq_max, &is_bitmap));
        /** Create the PFC Generic Bitmap */
        for (qpair = cosq_min; qpair <= cosq_max; qpair++)
        {
            SHR_BITSET(&new_data.qpair_bits[core][0], qpair);
        }
    }

    if ((map_sel == DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP) || (map_sel == DNX_RX_CAL_DEST_PFC_GENERIC_BMP))
    {
        *bitmap_index_old = map_val;
        /** Get the old profile data */
        SHR_IF_ERR_EXIT(flow_control_db.
                        fc_generic_bmp.profile_data_get(unit, *bitmap_index_old, &ref_count, &old_data));

        /** Combine the old profile data and the new data*/
        for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_or
                            (&new_data.qpair_bits[core][0], &old_data.qpair_bits[core][0],
                             (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS / (sizeof(uint32) * 8))));
        }

        /** Exchange the existing profile with a new one*/
        SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, 0, &new_data, (int) map_val, NULL,
                                                                bitmap_index_new, &is_first, &is_last));
    }
    else
    {
        /** Exchange the default profile with a new one*/
        SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, 0, &new_data, bitmap_index_default, NULL,
                                                                bitmap_index_new, &is_first, &is_last));
        *bitmap_index_old = *bitmap_index_new;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the VSQ attributes, the source select and FC index in context of Calendar(OOB, COE), when provided a VSQ Gport.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] vsq_gport - VSQ gport, whose attributes we want to get
 *   \param [out] src_sel - source select for the Calendar(OOB, COE, Inband ILKN)
 *   \param [out] fc_index - flow control index for the Calendar(OOB, COE, Inband ILKN)
 *   \param [out] traffic_class - traffic class
 *   \param [out] connection_class - connection class
 *   \param [out] category - category
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gport_to_fc_index(
    int unit,
    bcm_gport_t vsq_gport,
    uint8 *src_sel,
    uint32 *fc_index,
    int *traffic_class,
    int *connection_class,
    int *category)
{
    bcm_core_t core;
    uint32 vsq_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ type from the port */
    if (BCM_COSQ_GPORT_IS_VSQ_PP(vsq_gport))
    {
        *src_sel = DNX_TX_CAL_SRC_VSQ_D;
        *fc_index = BCM_COSQ_GPORT_VSQ_PP_GET(vsq_gport);
    }
    else
    {
        *src_sel = DNX_TX_CAL_SRC_VSQ_A_B_C;

        /** Determine the traffic class, connection class, core and category */
        *traffic_class = BCM_COSQ_GPORT_VSQ_TC_GET(vsq_gport);
        core = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(vsq_gport);
        *category = BCM_COSQ_GPORT_VSQ_CT_GET(vsq_gport);
        *connection_class = BCM_COSQ_GPORT_VSQ_CC_GET(vsq_gport);

        /** Calculate the VSQ index */
        if (*connection_class != -1)
        {
            /** VSQ C */
            if (((*category) != 2) && ((*category) != 3))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only Categories 2 and 3 of VSQs C can generate Flow Control");
            }
            vsq_index =
                (*category - 2) * dnx_data_ingr_congestion.vsq.connection_class_nof_get(unit) + *connection_class;
            *fc_index =
                NOF_STE_VSQS_PER_CORE * core + vsq_index + dnx_data_ingr_congestion.vsq.info_get(unit,
                                                                                                 DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY)->nof
                + dnx_data_ingr_congestion.vsq.info_get(unit, DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY_TRAFFIC_CLS)->nof;
        }
        else if ((*traffic_class) != -1)
        {
            /** VSQ B */
            vsq_index = (*category) * BCM_COS_COUNT + (*traffic_class);
            *fc_index =
                NOF_STE_VSQS_PER_CORE * core + vsq_index + dnx_data_ingr_congestion.vsq.info_get(unit,
                                                                                                 DNX_INGRESS_CONGESTION_VSQ_GROUP_CTGRY)->nof;
        }
        else
        {
            /** VSQ A */
            vsq_index = *category;
            *fc_index = NOF_STE_VSQS_PER_CORE * core + vsq_index;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] port - CPU port which is the source of the FC
 *   \param [in] is_delete - indicates if we want to add or delete path
 *   \param [in] cosq_min - start COSQ from which to start setting the bits in the mask
 *   \param [in] cosq_max - end COSQ up to which to set the bits in the mask
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_cmic_egq_mask_set(
    int unit,
    bcm_port_t port,
    int is_delete,
    int cosq_min,
    int cosq_max)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    int cosq = 0;
    int channel;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

    /** Set the mask of generated CMIC vector */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    for (cosq = cosq_min; cosq <= cosq_max; cosq++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel + cosq);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !is_delete);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function to get the Global Resource type, priority and FC index.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit ID
 *   \param [in] endpoint - source or target endpoint holding the user input information for the Global resource
 *   \param [out] resource - Type of the Global Resource
 *   \param [out] glb_res_prio - Global Recourse priority (High or Low)
 *   \param [out] fc_index - flow control index for the Calendar(OOB, COE, Inband ILKN)
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_glb_res_prio_fc_index_get(
    int unit,
    bcm_cosq_fc_endpoint_t * endpoint,
    dbal_enum_value_field_glb_rcs_e * resource,
    uint32 *glb_res_prio,
    int *fc_index)
{
    bcm_cosq_vsq_info_t vsq_info;
    SHR_FUNC_INIT_VARS(unit);

    /** To keep backward compatibility, because in legacy devices 0 represented high priority and 1 represented low priority */
    *glb_res_prio = ((endpoint->priority == 0) || (endpoint->priority == BCM_COSQ_HIGH_PRIORITY)) ? 1 : 0;

    /** Determine the Global recourse */
    if (endpoint->flags & BCM_COSQ_FC_BDB)
    {
        if (endpoint->cosq == 0)
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_DRAM0;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_DRAM_POOL_0_HIGH : DNX_GLB_RCS_IDX_DRAM_POOL_0_LOW;
        }
        else if (endpoint->cosq == 1)
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_DRAM1;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_DRAM_POOL_1_HIGH : DNX_GLB_RCS_IDX_DRAM_POOL_1_LOW;
        }
        else
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_DRAM;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_DRAM_HIGH : DNX_GLB_RCS_IDX_DRAM_LOW;
        }
    }
    else if (endpoint->flags & BCM_COSQ_FC_IS_OCB_ONLY)
    {
        if (endpoint->cosq == 0)
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_SRAM0;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_SRAM_POOL_0_HIGH : DNX_GLB_RCS_IDX_SRAM_POOL_0_LOW;
        }
        else if (endpoint->cosq == 1)
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_SRAM1;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_SRAM_POOL_1_HIGH : DNX_GLB_RCS_IDX_SRAM_POOL_1_LOW;
        }
        else
        {
            *resource = DBAL_ENUM_FVAL_GLB_RCS_SRAM;
            *fc_index = *glb_res_prio ? DNX_GLB_RCS_IDX_SRAM_HIGH : DNX_GLB_RCS_IDX_SRAM_LOW;
        }
    }
    else if (endpoint->flags & BCM_COSQ_FC_HEADROOM)
    {
        *resource = DBAL_ENUM_FVAL_GLB_RCS_HDRM;
        *fc_index = DNX_GLB_RCS_IDX_HDRM;
        /** Only high priority is supported for HDRM */
        *glb_res_prio = 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Flags, need to specify global resource");
    }

    /** Determine the source port core */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, endpoint->port, &vsq_info));
    /** Calculate FC index according to core */
    *fc_index = (vsq_info.core_id == 0) ? *fc_index : (*fc_index + DNX_GLB_RCS_COUNT);

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** When configuring reception from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if (src_port != tar_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configured reception from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

    /** If port is CPU we need to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
    {
        /** Map all Q-pairs of the port to receive FC from CPU */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, tar_port, &num_priorities));
        cosq_min = 0;
        cosq_max = num_priorities - 1;

        /** Set the CMIC to EGQ mask */
        SHR_IF_ERR_EXIT(dnx_fc_cmic_egq_mask_set(unit, src_port, is_delete, cosq_min, cosq_max));
    }
    else
    {
        /** Check which mode is enabled */
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, tar_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        if (pfc_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
        }
        llfc_enabled = (is_delete ? FALSE : TRUE);
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlReception, llfc_enabled, FALSE));

        /** Call IMB API to stop transmission on all lanes of the port according LLFC signal received from CFC */
        SHR_IF_ERR_EXIT(imb_port_stop_pm_from_cfc_llfc_enable_set(unit, tar_port, !(is_delete)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    bcm_port_t tar_port = 0;
    bcm_port_t src_port = 0;
    int cosq;
    int cosq_min = 0;
    int cosq_max = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target interface */
    tar_port = BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** When configuring reception from PFC to stop NIF port, port parameter must match */
    if (src_port != tar_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configured reception from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, src_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }
    pfc_enabled = (is_delete ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, src_port, bcmCosqFlowControlReception, FALSE, pfc_enabled));

    if (source->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
    {
       /** Inheritance is enabled, calculate COSQs*/
        if (source->flags & BCM_COSQ_FC_INHERIT_UP)
        {
            cosq_min = source->cosq;
            cosq_max = BCM_COS_COUNT - 1;
        }
        else if (source->flags & BCM_COSQ_FC_INHERIT_DOWN)
        {
            cosq_min = 0;
            cosq_max = source->cosq;
        }
    }
    else
    {
       /** No Inheritance */
        cosq_min = cosq_max = source->cosq;
    }

   /** Set the HW to enable stopping of the interface based on the specified PFCs */
    for (cosq = cosq_min; cosq <= cosq_max; cosq++)
    {
        SHR_IF_ERR_EXIT(imb_port_stop_pm_from_pfc_enable_set(unit, src_port, cosq, !(is_delete)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t tar_port = 0;
    bcm_port_t src_port = 0;
    bcm_core_t core;
    int base_q_pair;
    uint32 map_sel;
    uint32 map_val;
    int bitmap_index_new = 0;
    int bitmap_index_old = 0;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    int nif_port;
    int master_port;
    int first_affected_nif_port;
    uint8 is_bitmap = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    dnx_algo_port_info_s port_info;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the target port */
    tar_port = BCM_GPORT_LOCAL_GET(target->port);

    /** Determine the source port */
    /** Return the local port and then find the NIF port mapped to it */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** When configuring reception from PFC, port and cosq parameters must match or flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if ((src_port != tar_port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid Flags, need to specify BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT, or ports for source and destination need to be the same");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, tar_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, tar_port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));

    /** If port is CPU we need to set the CMIC to EGQ mapping, enable CFC to react to CMIC FC */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
    {
        if (source->cosq >= num_priorities)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, source port has only %d priorities.", num_priorities);
        }
        /** Target is Port+COSQ */
        /** Inheritance is enabled */
        if (target->flags & (BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
        {
            /** Determine which Q-pairs need to be stopped according to the Inheritance flag */
            if (target->flags & BCM_COSQ_FC_INHERIT_UP)
            {
                cosq_min = source->cosq;
                cosq_max = num_priorities - 1;
            }
            else if (target->flags & BCM_COSQ_FC_INHERIT_DOWN)
            {
                cosq_min = 0;
                cosq_max = source->cosq;
            }
        }
        else
        {
            /** No Inheritance */
            cosq_min = cosq_max = source->cosq;
        }

        /** Set the CMIC to EGQ mask */
        SHR_IF_ERR_EXIT(dnx_fc_cmic_egq_mask_set(unit, src_port, is_delete, cosq_min, cosq_max));
    }
    else
    {
         /** Check which mode is enabled */
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, tar_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        if (llfc_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
        }
        pfc_enabled = (is_delete ? FALSE : TRUE);
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlReception, FALSE, pfc_enabled));

        /** Check if Generic PFC Bitmap needs to be used and get the relevant cosq_min and cosq_max */
        SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, FALSE, &cosq_min, &cosq_max, &is_bitmap));

        if (is_bitmap)
        {
            /** Get the PFC Generic Bitmap index */
            SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get
                            (unit, source, target, FALSE, &bitmap_index_old, &bitmap_index_new));

                        /** If the entry is for delete, free the bitmap index in the resource manager */
            if (is_delete)
            {
                sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
                SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                                        &default_data, bitmap_index_new, NULL,
                                                                        &bitmap_index_default, &tmp_is_first,
                                                                        &tmp_is_last));
            }

                        /** If called from Add API set the BMP, if called from Delete API only delete if it is the last reference in the template manager */
            if ((!is_delete) || (is_delete && tmp_is_last))
            {
                                /** Set the PFC Generic Bitmap  */
                SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                                (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
            }

            /** Set the values to be written in the FC_REC_PFC_MAP */
            map_val = (is_delete ? 0 : bitmap_index_new);
            map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP);
        }
        else
        {
            if (target->cosq == -1)
            {
                /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
                if (num_priorities == 1)
                {
                    /** Set the values to be written in the FC_REC_PFC_MAP */
                    map_val = base_q_pair;
                    map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_PFC);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid Flags, for Port reaction point with port priorities different than 1 need to set BCM_COSQ_FC_PORT_OVER_PFC.");
                }
            }
            else
            {
                if (target->cosq < num_priorities)
                {
                    /**  Reaction point is Port+COSQ, no inheritance */
                    map_val = (is_delete ? 0 : (base_q_pair + target->cosq));
                    map_sel = (is_delete ? 0 : DNX_REC_PFC_MAP_SEL_PFC);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
                }
            }
        }

        /** Set FC_REC_PFC_MAP with the appropriate values */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
        /** Calculate the NIF port (per core) */
        nif_port = nif_port - core * nof_phys_per_core;

        /** Allocate handle to the table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));

        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_rec_cdu_shift))
        {
            /** The last two CDUs of each core have one bit shift left so the bitmap configuration is changed.
              * The implications here are that the mapping at CFC also needs to be shifted to correspond to the mapping of
              * the CDU bitmaps set on init. To do that, for the last two CDUs of each core when we want to map priority 0
              * we need to set it to the next nif_port. For example if we want to set prio 0 on NIF 36, we need to set the
              * field corresponding to NIF 37. For the other priorities the mapping is standard */

            first_affected_nif_port = nof_phys_per_core - (2 * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit));
            if ((nif_port >= first_affected_nif_port) && (source->cosq == 0))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT,
                                           ((nif_port ==
                                             (nof_phys_per_core - 1)) ? first_affected_nif_port : (nif_port + 1)));
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
            }
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, map_sel);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VALID, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_interface_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t port_i;
    bcm_core_t core;
    int base_q_pair;
    uint32 dest_sel;
    uint32 map_val;
    int bitmap_index_new;
    int bitmap_index_old;
    int cal_if;
    int cosq_min = 0;
    int cosq_max = 0;
    bcm_pbmp_t channels;
    int num_priorities;
    bcm_port_t master_port;
    int nif_port, ilkn_port;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set the variable for the new bitmap to 0 */
    sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));

    /** Determine the source Calendar(OOB, COE, Inband ILKN) interface */
    if (BCM_GPORT_IS_CONGESTION(source->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(source->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Return the port associated with that interface */
    port = BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Get the core */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    /** Interface is not Ethernet port use Generic bitmap type */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, FALSE))
    {
        /** Get the PFC Generic Bitmap index */
        SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get(unit, source, target, TRUE, &bitmap_index_old, &bitmap_index_new));

        /** If the entry is for delete, free the allocated bitmap index from the resource manager */
        if (is_delete)
        {
            SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                                    &default_data, bitmap_index_new, NULL,
                                                                    &bitmap_index_default, &tmp_is_first,
                                                                    &tmp_is_last));
        }

        /** Set the values to be written in the Calendar(OOB, COE, Inband ILKN) */
        map_val = (is_delete ? 0 : bitmap_index_new);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC_GENERIC_BMP);

        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &channels));
        /** For each channel set the appropriate bits in the FC generic bitmap */
        _SHR_PBMP_ITER(channels, port_i)
        {
            /** Get the base Q-pair and priorities */
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port_i, &base_q_pair));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port_i, &num_priorities));

            cosq_min = base_q_pair;
            cosq_max = base_q_pair + (num_priorities - 1);

            /** If called from Add API set the BMP, if called from Delete API only delete if it is the last reference in the template manager */
            if ((!is_delete) || (is_delete && tmp_is_last))
            {
                /** Set the PFC Generic Bitmap  */
                SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                                (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
            }
        }
    }
    else
    {
        /** Get the NIF interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

        /** Call IMB API to stop transmission on all lanes of the port according LLFC signal received from CFC */
        SHR_IF_ERR_EXIT(imb_port_stop_pm_from_cfc_llfc_enable_set(unit, master_port, !(is_delete)));

        /** Set the values to be written in the Calendar(OOB, COE, Inband ILKN) */
        map_val = (is_delete ? 0 : nif_port);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_NIF);
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_REC_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE, Inband ILKN) calendar mapping with the appropriate destination and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_port_cosq_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_core_t core;
    int base_q_pair;
    uint32 dest_sel;
    uint32 map_val;
    int cal_if, ilkn_port;
    int bitmap_index_new;
    int bitmap_index_old;
    int cosq_min = 0;
    int cosq_max = 0;
    int num_priorities;
    uint8 is_bitmap = 0;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_first;
    dnx_pfc_generic_bmp_t default_data;
    int bitmap_index_default = GENERIC_BMP_DEFAULT_PROFILE_ID;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source Calendar(OOB, COE, Inband ILKN) interface */
    if (BCM_GPORT_IS_CONGESTION(source->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(source->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Determine the target port */
    port = BCM_GPORT_LOCAL_GET(target->port);

    /** Get the base Q-pair, core and priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

    /** Check if Generic PFC Bitmap needs to be used and get the relevant cosq_min and cosq_max */
    SHR_IF_ERR_EXIT(dnx_fc_rec_is_bmp_get(unit, target, TRUE, &cosq_min, &cosq_max, &is_bitmap));

    if (is_bitmap)
    {
        /** Get the PFC Generic Bitmap index */
        SHR_IF_ERR_EXIT(dnx_fc_rec_bmp_index_get(unit, source, target, TRUE, &bitmap_index_old, &bitmap_index_new));

        /** If the entry is for delete, free the bitmap index in the resource manager */
        if (is_delete)
        {
            sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
            SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.exchange(unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID,
                                                                    &default_data, bitmap_index_new, NULL,
                                                                    &bitmap_index_default, &tmp_is_first,
                                                                    &tmp_is_last));
        }

        /** If called from Add API set the BMP, if called from Delete API only delete if it is the last reference in the template manager */
        if ((!is_delete) || (is_delete && tmp_is_last))
        {
            /** Set the PFC Generic Bitmap  */
            SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_hw_set
                            (unit, core, is_delete, cosq_min, cosq_max, bitmap_index_old, bitmap_index_new));
        }

        /** Set the values to be written in the Calendar(OOB, COE, Inband ILKN) */
        map_val = (is_delete ? 0 : bitmap_index_new);
        dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC_GENERIC_BMP);
    }
    else
    {
        if (target->cosq == -1)
        {
            /**  Reaction point is Port, but the BCM_COSQ_FC_PORT_OVER_PFC flag is not specified */
            /** Select destination according to the port priorities */
            switch (num_priorities)
            {
                case 1:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC);
                    break;
                case 2:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_2_PRIO);
                    break;
                case 4:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_4_PRIO);
                    break;
                case 8:
                    dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PORT_8_PRIO);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid number of port priorities");
            }
            map_val = (is_delete ? 0 : (base_q_pair + dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core));
        }
        else
        {
            if (target->cosq < num_priorities)
            {
                /**  Reaction point is Port+COSQ, no inheritance */
                /** Set the values to be written in the calendar DBAL table */
                map_val =
                    (is_delete ? 0
                     : (base_q_pair + dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core + target->cosq));
                dest_sel = (is_delete ? DNX_RX_CAL_DEST_CONST : DNX_RX_CAL_DEST_PFC);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
            }
        }
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_REC_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE, Inband ILKN) calendar mapping with the appropriate source and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, map_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_core_t core;
    int base_q_pair;
    int if_id;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 glb_res_prio;
    dbal_enum_value_field_glb_rcs_e resource;
    uint32 nof_phys_per_core;
    int fc_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    nif_port = nif_port - core * nof_phys_per_core;

    /** Determine the FC index , resource type and priority according to the source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_res_prio_fc_index_get(unit, source, &resource, &glb_res_prio, &fc_index));

    /** Check which mode is enabled and set inband mode to LLFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }

    /** Set the CFC to enable the generation from global resource to LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_LLFC_EN, &entry_handle_id));
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_res_lp_to_llfc))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, glb_res_prio);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resource);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_core_t core;
    int base_q_pair;
    int if_id;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 glb_res_prio;
    int num_priorities;
    dbal_enum_value_field_glb_rcs_e resource;
    uint32 nof_phys_per_core;
    int fc_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    nif_port = nif_port - core * nof_phys_per_core;

    /** Determine the FC index , resource type and priority according to the source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_res_prio_fc_index_get(unit, source, &resource, &glb_res_prio, &fc_index));

    /** Check which mode is enabled and set inband mode to PFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** Set the HW to enable the generation from global resource to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_PFC_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resource);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, glb_res_prio);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, target->cosq);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    int cal_if = 0;
    int fc_index;
    int ilkn_port = 0;
    uint32 glb_res_prio;
    dbal_enum_value_field_glb_rcs_e resource;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    if (BCM_GPORT_IS_CONGESTION(target->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(target->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(target->
                               port) ? BCM_GPORT_LOCAL_GET(target->port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Determine the FC index , resource type and priority according to the source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_res_prio_fc_index_get(unit, source, &resource, &glb_res_prio, &fc_index));

    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_rsc_oob_en)
        && BCM_GPORT_IS_CONGESTION(target->port))
    {
        /** Set the HW to enable the generation from global resource to OOB calendar */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS, resource);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, glb_res_prio);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, cal_if);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, entry_handle_id));
    }

    if (!BCM_GPORT_IS_CONGESTION(target->port)
        && dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_flexe_flow_control))
    {
        /** Map CGM signal for ILKN Inband flow-control */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CGM_2_ILKN_OR_FLEXE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_INB, INST_SINGLE, (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_GEN_CAL_TABLE_GET(target->port);

    /** Set mapping of the calendar to the appropriate source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_TX_CAL_SRC_CONST : DNX_TX_CAL_SRC_GLB_RSC));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    bcm_cosq_vsq_info_t vsq_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LLFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if (src_port != tar_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, tar_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Generation from VSQ LLFC to target ILKN port is not allowed.");
    }

    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, tar_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }
    llfc_enabled = (is_delete ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlGeneration, llfc_enabled, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 cal_if;
    uint32 fc_index;
    int src_port_vsq_id;
    int core_id;
    int ilkn_port = 0;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    if (BCM_GPORT_IS_CONGESTION(target->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(target->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(target->
                               port) ? BCM_GPORT_LOCAL_GET(target->port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Determine the FC index according to the source port */
    src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(source->port);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(source->port);
    fc_index = src_port_vsq_id + dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit) * core_id;

    if (!BCM_GPORT_IS_CONGESTION(target->port)
        && dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_flexe_flow_control))
    {
        /** Map CGM signal for ILKN Inband flow-control */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CGM_2_ILKN_OR_FLEXE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_INB, INST_SINGLE, (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_GEN_CAL_TABLE_GET(target->port);

    /** Set mapping of the calendar to the LLFC VSQ source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_TX_CAL_SRC_CONST : DNX_TX_CAL_SRC_LLFC_VSQ));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t tar_port;
    bcm_port_t src_port;
    bcm_cosq_vsq_info_t vsq_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the target port */
    tar_port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        tar_port = src_port;
    }
    else if ((src_port != tar_port) || (target->cosq != source->cosq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port and cosq parameter must match, or IMPLICIT flag should be used");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, tar_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Generation from VSQ PFC to target ILKN port is not allowed.");
    }

    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, tar_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }
    pfc_enabled = (is_delete ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_set(unit, tar_port, bcmCosqFlowControlGeneration, FALSE, pfc_enabled));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 cal_if;
    bcm_port_t src_port;
    bcm_cosq_vsq_info_t vsq_info;
    uint32 fc_index;
    int pg_base_vsq_id;
    int core_id;
    int ilkn_port = 0;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    if (BCM_GPORT_IS_CONGESTION(target->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(target->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(target->
                               port) ? BCM_GPORT_LOCAL_GET(target->port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** Determine the FC index from the source port */
    pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(source->port);
    core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(source->port);
    fc_index = pg_base_vsq_id + dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COS_COUNT * core_id + source->cosq;

    if (!BCM_GPORT_IS_CONGESTION(target->port)
        && dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_flexe_flow_control))
    {
        /** Map CGM signal for ILKN Inband flow-control */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CGM_2_ILKN_OR_FLEXE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_INB, INST_SINGLE, (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_GEN_CAL_TABLE_GET(target->port);

    /** Set mapping of the calendar to the PFC VSQ source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_TX_CAL_SRC_CONST : DNX_TX_CAL_SRC_PFC_VSQ));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    uint32 cat2_prio, cat2_prio_min = 0, cat2_prio_max = 0;
    bcm_core_t core;
    int base_q_pair;
    int if_id, num_priorities;
    int nif_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    nif_port = nif_port - core * nof_phys_per_core;

    /** Check which mode is enabled and set inband mode to PFC*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** Determine the Category 2 priority */
    cat2_prio = BCM_COSQ_GPORT_VSQ_TC_GET(source->port);

    /** No Inheritance */
    cat2_prio_min = cat2_prio;
    cat2_prio_max = cat2_prio;

    /** Inheritance */
    /** Determine which Category 2 priorities need to be stopped according to the Inheritance flag */
    if (source->flags & BCM_COSQ_FC_INHERIT_UP)
    {
        cat2_prio_max = (num_priorities - 1);
    }
    else if (source->flags & BCM_COSQ_FC_INHERIT_DOWN)
    {
        cat2_prio_min = 0;
    }

    /** Enable generation of Flow control from Category 2 and Traffic class to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_PFC_EN, &entry_handle_id));
    for (cat2_prio = cat2_prio_min; cat2_prio <= cat2_prio_max; cat2_prio++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAT2_PRIO, cat2_prio);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, target->cosq);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, !(is_delete));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_d_pfc_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 index, is_pfc;
    int num_priorities, vsq_d_index = 0, base_q_pair;
    bcm_port_t port, master_port, nif_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ D index */
    vsq_d_index = BCM_COSQ_GPORT_VSQ_PP_GET(source->port);

    /** Determine the target port */
    /** Return the local port and then find the NIF port mapped to it */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    if (target->cosq >= 0)
    {
        /** Target is PFC */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
        if (target->cosq >= num_priorities)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ, target port has only %d priorities.", num_priorities);
        }

        index = target->cosq + base_q_pair;
        is_pfc = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));

        /** Set the select and index values */
        index = nif_port;
        is_pfc = FALSE;
    }

    /** Set mapping of VSQ D to LLFC or PFC target */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_VSQD_PFC_LLFC_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQD_INDEX, vsq_d_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, INST_SINGLE, (is_delete ? 0 : index));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE, (is_delete ? 0 : is_pfc));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE, (is_delete ? 0 : 1));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_a_d_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    uint32 entry_handle_id_cat2_ena;
    int cal_if = 0;
    uint8 src_sel;
    int traffic_class;
    int connection_class;
    uint32 fc_index;
    int category;
    int ilkn_port = 0;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target OOB interface */
    if (BCM_GPORT_IS_CONGESTION(target->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(target->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(target->
                               port) ? BCM_GPORT_LOCAL_GET(target->port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Determine the VSQ properties, FC index and source select from the VSQ Gport */
    SHR_IF_ERR_EXIT(dnx_fc_vsq_gport_to_fc_index
                    (unit, source->port, &src_sel, &fc_index, &traffic_class, &connection_class, &category));

    if (((category == 2) && (traffic_class != -1))
        && dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_cat2_oob_en)
        && BCM_GPORT_IS_CONGESTION(target->port))
    {
        /** Special case for Category 2 + TC - Enable generation from Cat2 TC to OOB calendar */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_OOB_EN, &entry_handle_id_cat2_ena));
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_OOB_IF, cal_if);
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_CAL_IDX, target->calender_index);
        dbal_entry_key_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_CAT2_PRIO, traffic_class);
        dbal_entry_value_field32_set(unit, entry_handle_id_cat2_ena, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_cat2_ena, DBAL_COMMIT));
    }

    if (!BCM_GPORT_IS_CONGESTION(target->port)
        && dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_flexe_flow_control))
    {
        /** Map CGM signal for ILKN Inband flow-control */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_CGM_2_ILKN_OR_FLEXE, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ILKN_INB, INST_SINGLE, (is_delete ? 0 : 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_GEN_CAL_TABLE_GET(target->port);

    /** Set mapping of the calendar to the VSQ (A to D) source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_TX_CAL_SRC_CONST : src_sel));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : fc_index));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_llfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t port, src_port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target interface */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        port = src_port;
    }
    else if (src_port != port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }
    /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (pfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is PFC, use bcm_port_control_set to set the inband mode to LLFC.");
    }

    /** Call IMB API to map port's RMCs to generate LLFC */
    SHR_IF_ERR_EXIT(imb_port_gen_llfc_from_rmc_enable_set(unit, port, !(is_delete)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_pfc_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t port, src_port;
    int num_priorities;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the target interface */
    port =
        BCM_GPORT_IS_LOCAL(target->port) ? BCM_GPORT_LOCAL_GET(target->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));

        /** When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used */
    if (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        port = src_port;
    }
    else if (src_port != port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "When configuring generation from LLFC, port parameter must match, or IMPLICIT flag should be used");
    }

        /** Check which mode is enabled */
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
    if (llfc_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband mode is LLFC, use bcm_port_control_set to set the inband mode to PFC.");
    }

    /** Call IMB API to set HW to enable all RMCs of the port to produce PFC for the specified priority */
    SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_rmc_enable_set(unit, port, target->cosq, !(is_delete)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_cal_map(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    uint32 entry_handle_id;
    bcm_port_t port = 0;
    int cal_if;
    bcm_port_t master_port;
    int nif_port;
    int llfc_index;
    int ilkn_port = 0;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the source interface */
    port = BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /** Determine the target OOB interface */
    if (BCM_GPORT_IS_CONGESTION(target->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(target->port);
    }
    else
    {
        /** Determine the target ILKN interface */
        ilkn_port =
            BCM_GPORT_IS_LOCAL(target->
                               port) ? BCM_GPORT_LOCAL_GET(target->port) : BCM_GPORT_LOCAL_INTERFACE_GET(target->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Get information for the port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
    /** Link-Level FC (from the NIF-RX-MLF) that can be mapped to calendar are NIF ports + CMIC ports or ILKN ports depending on device.
      * This API allows mapping of the NIF ports, but the correct offset needs to be calculated. For ports on core 0 the
      * Calendar LLFC index is equal to the ID of the NIF port, as the CMIC/ILKN ports are the last. However for ports on core one
      * a calculation must be made, because originally they have offset equal to NOF_PHYS_PER_CORE. This offset must be
      * removed and the new offset (nof_cal_llfcs) must be added. */
    if (nif_port >= nof_phys_per_core)
    {
        llfc_index = (nif_port - nof_phys_per_core) + dnx_data_fc.general.nof_cal_llfcs_get(unit);
    }
    else
    {
        llfc_index = nif_port;
    }

    /** Call IMB API to map port's RMCs to generate LLFC */
    SHR_IF_ERR_EXIT(imb_port_gen_llfc_from_rmc_enable_set(unit, port, !(is_delete)));

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_GEN_CAL_TABLE_GET(target->port);

    /** Set mapping of the calendar to the Almost full NIF source and index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, target->calender_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL,
                                 INST_SINGLE, (is_delete ? DNX_TX_CAL_SRC_CONST : DNX_TX_CAL_SRC_NIF));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, (is_delete ? 0 : llfc_index));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function to verify the parameters for Calendar endpoint
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_generation - indicates weather the function is called for generation or reception path
 *   \param [in] endpoint - holds information for the source or target endpoint of the Flow control
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_cal_endpoint_verify(
    int unit,
    uint8 is_generation,
    bcm_cosq_fc_endpoint_t * endpoint)
{
    int cal_if_id_max = 0;
    int cal_if_id = 0;
    int cal_index_max = 0;
    int ilkn_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify endpoint flags */
    if (endpoint->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags %d for calendar endpoint!", endpoint->flags);
    }

    /** Verify endpoint COSQ */
    if (endpoint->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ %d  for calendar endpoint, must be -1!", endpoint->cosq);
    }

    if (!BCM_GPORT_IS_CONGESTION(endpoint->port) && (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inband ILKN calendar not supported!");
    }

    /** Verify endpoint calendar interface ID */
    cal_if_id_max =
        (BCM_GPORT_IS_CONGESTION(endpoint->port)
         ? ((BCM_GPORT_IS_CONGESTION_COE(endpoint->port) ? (dnx_data_fc.coe.nof_coe_cal_instances_get(unit) - 1)
             : (dnx_data_fc.oob.nof_oob_ids_get(unit) -
                1))) : (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) - 1));
    if (BCM_GPORT_IS_CONGESTION(endpoint->port))
    {
        cal_if_id = BCM_GPORT_CONGESTION_GET(endpoint->port);
    }
    else
    {
        /** Determine the endpoint ILKN interface */
        ilkn_port = BCM_GPORT_LOCAL_GET(endpoint->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, &cal_if_id));
    }
    if (cal_if_id > cal_if_id_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid calendar interface ID %d for calendar endpoint, max is %d!", cal_if_id,
                     cal_if_id_max);
    }

    /** Verify endpoint calender_index */
    if (is_generation == TRUE)
    {
        cal_index_max =
            (BCM_GPORT_IS_CONGESTION(endpoint->port) ? (dnx_data_fc.oob.calender_length_get(unit, cal_if_id)->tx - 1)
             : (dnx_data_fc.inband.calender_length_get(unit, cal_if_id)->tx - 1));
    }
    else
    {
        cal_index_max =
            (BCM_GPORT_IS_CONGESTION(endpoint->port)
             ? ((BCM_GPORT_IS_CONGESTION_COE(endpoint->port)
                 ? (dnx_data_fc.coe.calender_length_get(unit, cal_if_id)->len -
                    1) : (dnx_data_fc.oob.calender_length_get(unit,
                                                              cal_if_id)->rx -
                          1))) : (dnx_data_fc.inband.calender_length_get(unit, cal_if_id)->rx - 1));
    }
    if ((endpoint->calender_index < 0) || (endpoint->calender_index > cal_index_max))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid endpoint calender index %d for calendar endpoint, max is %d!",
                     endpoint->calender_index, cal_index_max);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function to verify the parameters for Global Resource endpoint
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_llfc_gen - indicates weather the function is called for generation of LLFC
 *   \param [in] endpoint - holds information for the source or target endpoint of the Flow control
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_glb_rsc_endpoint_verify(
    int unit,
    uint8 is_llfc_gen,
    bcm_cosq_fc_endpoint_t * endpoint)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify endpoint flags */
    if (endpoint->flags & ~(BCM_COSQ_FC_BDB | BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_IS_OCB_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags %d for Global Resource endpoint endpoint!", endpoint->flags);
    }

    /** Verify endpoint priority */
    if ((endpoint->priority != 0) && (endpoint->priority != BCM_COSQ_HIGH_PRIORITY) && (endpoint->priority != 1)
        && (endpoint->priority != BCM_COSQ_LOW_PRIORITY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority %d for Global Resource endpoint!", endpoint->priority);
    }

    if (!dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_res_lp_to_llfc)
        && ((endpoint->priority == 1) || (endpoint->priority == BCM_COSQ_LOW_PRIORITY)) && (is_llfc_gen == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid priority %d for Global Resource endpoint - mapping of Low Priority Global Resources to LLFC target not supported.",
                     endpoint->priority);
    }

    /** Verify endpoint COSQ */
    if ((endpoint->cosq < -1) || (endpoint->cosq > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid COSQ %d for Global Resource endpoint!", endpoint->cosq);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source calender_index */
    if (source->calender_index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source calender_index */
    if (source->calender_index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
    }

    /** Verify source flags */
    if (source->flags & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_pfc_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source calender_index */
    if (source->calender_index != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
    }

    /** Verify target flags */
    if (target->cosq != BCM_COS_INVALID)
    {
        if (target->flags
            & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN
                | BCM_COSQ_FC_PORT_OVER_PFC | BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }
    else
    {
        if (target->flags & ~(BCM_COSQ_FC_PORT_OVER_PFC | BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_interface_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify Calendar source */
    SHR_IF_ERR_EXIT(dnx_fc_cal_endpoint_verify(unit, FALSE, source));

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_port_cosq_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify Calendar source */
    SHR_IF_ERR_EXIT(dnx_fc_cal_endpoint_verify(unit, FALSE, source));

    /** Verify target flags */
    if (target->cosq != BCM_COS_INVALID)
    {
        if (target->flags & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN | BCM_COSQ_FC_PORT_OVER_PFC))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }
    else
    {
        if (target->flags & ~BCM_COSQ_FC_PORT_OVER_PFC)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify Global Resource source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_rsc_endpoint_verify(unit, TRUE, source));

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify Global Resource source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_rsc_endpoint_verify(unit, FALSE, source));

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_glb_res_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify Global Resource source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_rsc_endpoint_verify(unit, FALSE, source));

    /** Verify Calendar target */
    SHR_IF_ERR_EXIT(dnx_fc_cal_endpoint_verify(unit, TRUE, target));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_to_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify Calendar target */
    SHR_IF_ERR_EXIT(dnx_fc_cal_endpoint_verify(unit, TRUE, target));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < BCM_COS_MIN) || (source->cosq > BCM_COS_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_d_pfc_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify feature is supported */
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_vsqd_to_llfc_pfc) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Generation from PP VSQ to PFC/LLFC target is not supported for this device.");
    }

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify the VSQ type of the source port */
    if (!BCM_COSQ_GPORT_IS_VSQ_PP(source->port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source port VSQ type, must be PP VSQ.");
    }

    /** Verify target COSQ */
    if (((target->cosq < BCM_COS_MIN) || (target->cosq > BCM_COS_MAX)) && (target->cosq != BCM_COS_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_cal_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if ((source->cosq < BCM_COS_MIN) || (source->cosq > BCM_COS_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
    }

    /** Verify Calendar target */
    SHR_IF_ERR_EXIT(dnx_fc_cal_endpoint_verify(unit, TRUE, target));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if ((source->flags != 0) && (source->flags & ~(BCM_COSQ_FC_INHERIT_UP | BCM_COSQ_FC_INHERIT_DOWN)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if (target->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_pfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_llfc_verify(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify source flags */
    if (source->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
    }

    /** Verify source COSQ */
    if (source->cosq != BCM_COS_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
    }

    /** Verify target flags */
    if ((target->flags != 0) && (target->flags & ~BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid target flags.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function to get the generation calendar targets
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] src_sel - indicates the source of the generation
 *   \param [in] source - holds information for the source endpoint of the Flow control
 *   \param [in] target_max - max number of targets
 *   \param [out] target - holds returned information for the target endpoint of the Flow control
 *   \param [out] target_count - actual number of targets returned
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_gen_to_cal_targets_get(
    int unit,
    uint8 src_sel,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 entry_handle_id_oob_en;
    uint32 src_sel_get;
    bcm_port_t port, master_port;
    bcm_port_t nif_port, src_port;
    uint32 fc_index_get;
    uint32 fc_index;
    uint32 enabled_oob, enabler_valid = 0;
    int cal_index, cal_if_cur;
    dbal_enum_value_field_glb_rcs_e resource;
    uint32 glb_res_prio;
    int traffic_class;
    int connection_class;
    int category;
    uint32 nof_phys_per_core;
    int pg_base_vsq_id, src_port_vsq_id;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the FC index according to the source */
    if (src_sel == DNX_TX_CAL_SRC_GLB_RSC)
    {
        SHR_IF_ERR_EXIT(dnx_fc_glb_res_prio_fc_index_get(unit, source, &resource, &glb_res_prio, (int *) &fc_index));
        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_rsc_oob_en))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_OOB_EN, &entry_handle_id_oob_en));
            dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_GLB_RCS, resource);
            dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_GLB_PRIO, glb_res_prio);
            /** Enable valid indication - for the case of Global resource source and glb_rsc_oob_en feature */
            enabler_valid = 1;
        }
    }
    else if (src_sel == DNX_TX_CAL_SRC_PFC_VSQ)
    {
        pg_base_vsq_id = BCM_COSQ_GPORT_VSQ_PG_GET(source->port);
        core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(source->port);
        fc_index = pg_base_vsq_id + dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit) * core_id;
        fc_index =
            pg_base_vsq_id + dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COS_COUNT * core_id + source->cosq;
    }
    else if (src_sel == DNX_TX_CAL_SRC_LLFC_VSQ)
    {
        src_port_vsq_id = BCM_COSQ_GPORT_VSQ_SRC_PORT_GET(source->port);
        core_id = BCM_COSQ_GPORT_VSQ_CORE_ID_GET(source->port);
        fc_index = src_port_vsq_id + dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit) * core_id;
    }
    else if (src_sel == DNX_TX_CAL_SRC_NIF)
    {
        src_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
        /** Link-Level FC (from the NIF-RX-MLF) that can be mapped to OOB are NIF ports + 4 CMIC ports.
          * This API allows mapping of the NIF ports, but the correct offset needs to be calculated. For ports on core 0 the
          * OOB LLFC index is equal to the ID of the NIF port, as the CMIC ports are the last 4. However for ports on core one
          * a calculation must be made, because originally they have offset equal to NOF_PHYS_PER_CORE. This offset must be
          * removed and the new offset (nof_oob_llfcs = NIF ports + 4 CMIC port) must be added. */
        if (nif_port >= nof_phys_per_core)
        {
            fc_index = (nif_port - nof_phys_per_core) + dnx_data_fc.general.nof_cal_llfcs_get(unit);
        }
        else
        {
            fc_index = nif_port;
        }
    }
    else
    {
        /** Determine the VSQ properties, FC index and source select from the VSQ Gport */
        SHR_IF_ERR_EXIT(dnx_fc_vsq_gport_to_fc_index
                        (unit, source->port, &src_sel, &fc_index, &traffic_class, &connection_class, &category));

        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_cat2_oob_en) && (category == 2)
            && (traffic_class != -1))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_OOB_EN, &entry_handle_id_oob_en));
            /** Enable valid indication - for the case of Cat2 VSQ source and cat2_oob_en feature */
            enabler_valid = 1;
        }
    }

    /** Get OOB calendar targets*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_OOB_CAL_MAP, &entry_handle_id));
    for (cal_if_cur = 0; cal_if_cur < dnx_data_fc.oob.nof_oob_ids_get(unit); cal_if_cur++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if_cur);
        for (cal_index = 0; cal_index < dnx_data_fc.oob.calender_length_get(unit, cal_if_cur)->tx; cal_index++)
        {
            if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_rsc_oob_en)
                && (src_sel == DNX_TX_CAL_SRC_GLB_RSC))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_OOB_IF, cal_if_cur);
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAL_IDX, cal_index);
                dbal_value_field32_request(unit, entry_handle_id_oob_en, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_oob);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob_en, DBAL_COMMIT));
            }
            if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_cat2_oob_en)
                && (src_sel == DNX_TX_CAL_SRC_VSQ_A_B_C) && (category == 2) && (traffic_class != -1))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_OOB_IF, cal_if_cur);
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAL_IDX, cal_index);
                dbal_entry_key_field32_set(unit, entry_handle_id_oob_en, DBAL_FIELD_CAT2_PRIO, traffic_class);
                dbal_value_field32_request(unit, entry_handle_id_oob_en, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_oob);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_oob_en, DBAL_COMMIT));
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, cal_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel_get);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index_get);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if ((src_sel_get == src_sel) && (fc_index_get == fc_index))
            {
                if ((enabled_oob == 1) || (enabler_valid == 0))
                {
                    if (*target_count >= target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    target[*target_count].calender_index = cal_index;
                    BCM_GPORT_CONGESTION_SET(port, cal_if_cur);
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }

    if (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) != 0)
    {
        /** Get Inband ILKN calendar targets*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_INB_ILKN_CAL_MAP, entry_handle_id));
        for (cal_if_cur = 0; cal_if_cur < dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit); cal_if_cur++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if_cur);
            for (cal_index = 0; cal_index < dnx_data_fc.inband.calender_length_get(unit, cal_if_cur)->tx; cal_index++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, cal_index);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, &src_sel_get);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, INST_SINGLE, &fc_index_get);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                if ((src_sel_get == src_sel) && (fc_index_get == fc_index))
                {
                    if (*target_count >= target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_logical_get
                                    (unit, DNX_ALGO_PORT_TYPE_NIF_ILKN, cal_if_cur, &port));
                    target[*target_count].calender_index = cal_index;
                    target[*target_count].port = port;
                    target[*target_count].cosq = -1;
                    *target_count += 1;
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_llfc_pfc_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    bcm_port_t src_port = 0;
    bcm_port_t port = 0;
    bcm_port_t master_port;
    bcm_gport_t if_gport;
    bcm_core_t core;
    int if_id = 0;
    int channel;
    int nif_port;
    int first_affected_nif_port;
    dnx_algo_port_info_s port_info;
    uint32 map_sel, map_val;
    uint32 valid, llfc_enabled, pfc_enabled;
    uint32 enabled = 0;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the source port */

    /** Return the local port and then find the NIF port mapped to it */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, src_port, 0, &master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, master_port, 0, &nif_port));
    }

    /** Get information for the ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, src_port, &if_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &channel));
    nif_port = nif_port - core * nof_phys_per_core;

    if (source->cosq == -1)
    {
        /** If port is CPU we need to get the CIMC to EGQ mapping */
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            /** Get the mask of generated CMIC vector */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /** We only need to check the first channel of the port to confirm it is mapped */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel);
            /** Getting value fields */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &llfc_enabled);
            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        }
        else
        {
            /** Get inband mode */
            SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                            (unit, src_port, bcmCosqFlowControlReception, &llfc_enabled, &pfc_enabled));
        }

        if (llfc_enabled)
        {
            /** Source is LLFC, target is interface */
            target[*target_count].port = source->port;
            *target_count += 1;
        }
    }
    else
    {
        /** Source is PFC */

        /** If port is CPU we need to get the CIMC to EGQ mapping */
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            /** Get the mask of generated CMIC vector */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MASK, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /** We only need to check the first channel of the port to confirm it is mapped */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, channel + source->cosq);
            /** Getting value fields */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &pfc_enabled);
            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (pfc_enabled)
            {
                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                target[*target_count].port = source->port;
                target[*target_count].cosq = source->cosq;
                *target_count += 1;
            }
        }
        else
        {
            /** Get FC_REC_PFC_MAP from the entry corresponding to the source to get Port+COSQ targets*/
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_REC_PFC_MAP, &entry_handle_id));
            if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pfc_rec_cdu_shift))
            {
                /** The last two CDUs of each core have one bit shift left so the bitmap configuration is changed.
                  * The implications here are that the mapping at CFC also needs to be shifted to correspond to the mapping of
                  * the CDU bitmaps set on init. To do that, for the last two CDUs of each core when we want to map priority 0
                  * we need to set it to the next nif_port. For example if we want to set prio 0 on NIF 36, we need to set the
                  * field corresponding to NIF 37. For the other priorities the mapping is standard */

                first_affected_nif_port = nof_phys_per_core - (2 * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit));
                if ((nif_port >= first_affected_nif_port) && (source->cosq == 0))
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT,
                                               ((nif_port ==
                                                 (nof_phys_per_core - 1)) ? first_affected_nif_port : (nif_port + 1)));
                }
                else
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
                }
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, nif_port);
            }
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, source->cosq);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_SEL, INST_SINGLE, &map_sel);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VALID, INST_SINGLE, &valid);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (valid)
            {
                if (map_sel == DNX_REC_PFC_MAP_SEL_PFC)
                {
                    /** Reaction point is Port+Cosq, no inheritance */
                    if (*target_count >= target_max)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                     "The target count is more than the specified maximum number of targets.");
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get
                                    (unit, map_val, core, &port, &target[*target_count].cosq));
                    BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                    *target_count += 1;
                }
                else if (map_sel == DNX_REC_PFC_MAP_SEL_GENERIC_PFC_BMP)
                {
                    /** Reaction point is using Generic PFC bitmap, which means it is Port or Port+Cosq with inheritance */
                    SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_targets_get
                                    (unit, map_val, core, target_max, target, target_count));
                }
            }

            /** Get Interface targets*/
            SHR_IF_ERR_EXIT(imb_port_stop_pm_from_pfc_enable_get(unit, src_port, source->cosq, &enabled));
            if (enabled)
            {
                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                BCM_GPORT_LOCAL_INTERFACE_SET(if_gport, source->port);
                target[*target_count].port = if_gport;
                target[*target_count].cosq = -1;
                *target_count += 1;
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_rec_cal_react_point_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint32 dest_sel, map_val;
    int cal_if;
    bcm_port_t port;
    bcm_core_t core;
    int q_pair;
    int ilkn_port = 0;
    dbal_tables_e cal_table = DBAL_TABLE_EMPTY;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the source Calendar interface */
    if (BCM_GPORT_IS_CONGESTION(source->port))
    {
        cal_if = BCM_GPORT_CONGESTION_GET(source->port);
    }
    else
    {
        ilkn_port =
            BCM_GPORT_IS_LOCAL(source->
                               port) ? BCM_GPORT_LOCAL_GET(source->port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, (int *) &cal_if));
    }

    /** Choose the appropriate calendar table according to the input Gport */
    cal_table = GPORT_REC_CAL_TABLE_GET(source->port);

    /** Set the Calendar(OOB, COE, Inband ILKN) calendar mapping with the appropriate destination and value */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, cal_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, source->calender_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAP_VAL, INST_SINGLE, &map_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, &dest_sel);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (dest_sel == DNX_RX_CAL_DEST_PFC_GENERIC_BMP)
    {
        /*
         * Reaction point is Interface, Port+Cosq using a Generic Bitmap
         */
        SHR_IF_ERR_EXIT(dnx_fc_pfc_generic_bmp_targets_get
                        (unit, map_val, _SHR_CORE_ALL, target_max, target, target_count));
    }
    else if ((dest_sel == DNX_RX_CAL_DEST_PFC) || (dest_sel == DNX_RX_CAL_DEST_PORT_2_PRIO)
             || (dest_sel == DNX_RX_CAL_DEST_PORT_4_PRIO) || (dest_sel == DNX_RX_CAL_DEST_PORT_8_PRIO))
    {
        /*
         * Reaction point is Port
         */
        if (*target_count >= target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }

        core = (map_val >= dnx_data_egr_queuing.params.nof_q_pairs_get(unit)) ? 1 : 0;
        q_pair = map_val - (dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * core);
        SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get(unit, q_pair, core, &port, &target[*target_count].cosq));
        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
        if (dest_sel != DNX_RX_CAL_DEST_PFC)
        {
            target[*target_count].cosq = -1;
        }
        *target_count += 1;
    }
    else if (dest_sel == DNX_RX_CAL_DEST_NIF)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, map_val, 0, 0, &port));
        if (*target_count >= target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }
        BCM_GPORT_LOCAL_INTERFACE_SET(target[*target_count].port, port);
        target[*target_count].cosq = -1;
        *target_count += 1;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_fc_gen_glb_res_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id_llfc;
    uint32 entry_handle_id_pfc;
    bcm_port_t port = 0;
    bcm_core_t core;
    int if_id = 0;
    int nif_port = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 enabled_llfc, enabled_pfc;
    uint32 tar_prio = 0;
    uint32 glb_res_prio;
    int fc_index;
    dbal_enum_value_field_glb_rcs_e resource;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

   /** Determine the FC index , resource type and priority according to the source */
    SHR_IF_ERR_EXIT(dnx_fc_glb_res_prio_fc_index_get(unit, source, &resource, &glb_res_prio, &fc_index));

    /** Allocate handle for global resource to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_PFC_EN, &entry_handle_id_pfc));
    dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_GLB_RCS, resource);
    dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_GLB_PRIO, glb_res_prio);

     /** Allocate handle for global resource to LLFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_GLB_RCS_LLFC_EN, &entry_handle_id_llfc));
    dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_GLB_RCS, resource);
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        for (if_id = 0; if_id < nof_phys_per_core; if_id++)
        {
            /** Get targets from global resource to LLFC */
            dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_NIF_PORT, if_id);
            if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_glb_res_lp_to_llfc))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_llfc, DBAL_FIELD_GLB_PRIO, glb_res_prio);
            }
            dbal_value_field32_request(unit, entry_handle_id_llfc, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_llfc);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_llfc, DBAL_COMMIT));
            if (enabled_llfc)
            {
                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                nif_port = if_id + core * nof_phys_per_core;
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));

                target[*target_count].cosq = -1;
                BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                *target_count += 1;
            }

            /** Get targets from global resource to PFC */
            dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_NIF_PORT, if_id);
            for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id_pfc, DBAL_FIELD_NIF_PRIORITY, tar_prio);
                dbal_value_field32_request(unit, entry_handle_id_pfc, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled_pfc);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_pfc, DBAL_COMMIT));
                if (enabled_pfc)
                {
                    nif_port = if_id + core * nof_phys_per_core;
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                    (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
                    if (pfc_enabled)
                    {
                        if (*target_count >= target_max)
                        {
                            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                         "The target count is more than the specified maximum number of targets.");
                        }
                        target[*target_count].cosq = tar_prio;
                        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                        *target_count += 1;
                    }
                }
            }
        }
    }

    /** Get targets from global resource to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get
                    (unit, DNX_TX_CAL_SRC_GLB_RSC, source, target_max, target, target_count));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_llfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_port_t src_port, port;
    bcm_cosq_vsq_info_t vsq_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;

    /** Get targets from LLFC VSQ to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get
                    (unit, DNX_TX_CAL_SRC_LLFC_VSQ, source, target_max, target, target_count));

    /*
     * Source is LLFC, get the inband mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
        if (llfc_enabled)
        {
            /*
             * Target is LLFC
             */
            BCM_GPORT_LOCAL_SET(port, src_port);
            target[*target_count].port = port;
            target[*target_count].cosq = -1;
            *target_count += 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_pfc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    bcm_port_t port, src_port;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    bcm_cosq_vsq_info_t vsq_info;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the source port */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_vsq_info_get(unit, source->port, &vsq_info));
    src_port = vsq_info.src_port;
    if (src_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PFC VSQ not enabled for source port 0x%X!", source->port);
    }

    /** Get targets from PFC VSQ to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get
                    (unit, DNX_TX_CAL_SRC_PFC_VSQ, source, target_max, target, target_count));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        /** Source is PFC VSQ, get the inband mode */
        SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                        (unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
        if (pfc_enabled)
        {
            /** Target is PFC */
            if (*target_count >= target_max)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
            }
            BCM_GPORT_LOCAL_SET(port, src_port);
            target[*target_count].port = port;
            target[*target_count].cosq = source->cosq;
            *target_count += 1;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_vsq_a_d_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    uint8 src_sel;
    uint32 index, is_pfc, valid;
    uint32 vsq_d_index;
    int cosq;
    bcm_port_t nif_port = 0, port;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Determine the VSQ type from the port */
    if (BCM_COSQ_GPORT_IS_VSQ_PP(source->port))
    {
        /** Set appropriate src_sel for the calendar targets */
        src_sel = DNX_TX_CAL_SRC_VSQ_D;

        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_vsqd_to_llfc_pfc))
        {
            /** Determine the VSQ D index to get the PFC and LLFC targets*/
            vsq_d_index = BCM_COSQ_GPORT_VSQ_PP_GET(source->port);

            /** Get mapping of VSQ D to LLFC or PFC target */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_VSQD_PFC_LLFC_MAP, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQD_INDEX, vsq_d_index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDEX, INST_SINGLE, &index);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE, &is_pfc);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE, &valid);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (valid == TRUE)
            {
                if (is_pfc == TRUE)
                {
                    /** Target is PFC */
                    SHR_IF_ERR_EXIT(dnx_algo_port_q_pair_port_cosq_get(unit, index, _SHR_CORE_ALL, &port, &cosq));
                }
                else
                {
                    /** Target is LLFC */
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                    cosq = BCM_COS_INVALID;
                }

                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                target[*target_count].cosq = cosq;
                BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                *target_count += 1;
            }
        }
    }
    else
    {
        /** Set appropriate src_sel for the calendar targets */
        src_sel = DNX_TX_CAL_SRC_VSQ_A_B_C;
    }

    /** Get targets from A,B,C,D VSQs to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get(unit, src_sel, source, target_max, target, target_count));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_c2_tc_vsq_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    int if_id;
    int nif_port = 0;
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 traffic_class;
    bcm_port_t port;
    uint32 tar_prio, enabled;
    uint32 nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_phys_per_core = dnx_data_nif.phys.nof_phys_per_core_get(unit);

    /** Determine the traffic class */
    traffic_class = BCM_COSQ_GPORT_VSQ_TC_GET(source->port);

    /** Get the PFC targets from Category 2 TC VSQ */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GEN_CAT2_PFC_EN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAT2_PRIO, traffic_class);
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (if_id = 0; if_id < nof_phys_per_core; if_id++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT, if_id);
            for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, tar_prio);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (enabled)
                {
                    nif_port = if_id + core * nof_phys_per_core;
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, nif_port, 0, 0, &port));
                    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get
                                    (unit, port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));
                    if (pfc_enabled)
                    {
                        if (*target_count >= target_max)
                        {
                            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                         "The target count is more than the specified maximum number of targets.");
                        }
                        target[*target_count].cosq = tar_prio;
                        BCM_GPORT_LOCAL_SET(target[*target_count].port, port);
                        *target_count += 1;
                    }
                }
            }
        }
    }

    /** Get targets from Category 2 TC VSQ to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get
                    (unit, DNX_TX_CAL_SRC_VSQ_A_B_C, source, target_max, target, target_count));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_gen_nif_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    uint32 pfc_enabled;
    uint32 llfc_enabled;
    uint32 enabled;
    bcm_port_t src_port;
    int tar_prio;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the source interface */
    src_port =
        BCM_GPORT_IS_LOCAL(source->port) ? BCM_GPORT_LOCAL_GET(source->
                                                               port) : BCM_GPORT_LOCAL_INTERFACE_GET(source->port);

    /**Get the inband mode*/
    SHR_IF_ERR_EXIT(dnx_fc_inband_mode_get(unit, src_port, bcmCosqFlowControlGeneration, &llfc_enabled, &pfc_enabled));

    if (llfc_enabled)
    {
        /** Get LLFC targets */
        if (*target_count >= target_max)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The target count is more than the specified maximum number of targets.");
        }
        target[*target_count].port = source->port;
        target[*target_count].cosq = -1;
        *target_count += 1;
    }

    if (pfc_enabled)
    {
        /** Get LLFC targets, check if thresholds are enabled to produce PFC */
        for (tar_prio = 0; tar_prio < BCM_COS_COUNT; tar_prio++)
        {
            /** Call IMB API to get the Enable field of the RMCs of the Port to check if they were enabled to produce PFC for the specified priority */
            SHR_IF_ERR_EXIT(imb_port_gen_pfc_from_rmc_enable_get(unit, src_port, tar_prio, &enabled));

            if (enabled)
            {
                if (*target_count >= target_max)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "The target count is more than the specified maximum number of targets.");
                }
                target[*target_count].port = source->port;
                target[*target_count].cosq = tar_prio;
                *target_count += 1;
            }
        }
    }

    /** Get targets from Almost Full NIF to calendars (OOB, Inband ILKN) */
    SHR_IF_ERR_EXIT(dnx_fc_gen_to_cal_targets_get(unit, DNX_TX_CAL_SRC_NIF, source, target_max, target, target_count));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_cosq_gport_eth_port_threshold_set_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    /*
     * Validate gport
     */
    if (!BCM_GPORT_IS_LOCAL(gport) && !BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, Unsupported gport(0x%x) for BCM_COSQ_THRESHOLD_ETH_PORT_LLFC/PFC",
                     unit, gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_cosq_gport_eth_port_threshold_set_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_threshold_t * threshold,
    uint8 is_set)
{
    bcm_port_t port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_gport_eth_port_threshold_set_get_verify(unit, gport, threshold, is_set));

    /** Get the logical port */
    port = BCM_GPORT_IS_LOCAL(gport) ? BCM_GPORT_LOCAL_GET(gport) : BCM_GPORT_LOCAL_INTERFACE_GET(gport);

    /** Call IMB APIs to set/get the threshold for all RMCs of the port */
    if (is_set)
    {
        SHR_IF_ERR_EXIT(imb_port_fc_rx_qmlf_threshold_set(unit, port, threshold->flags, (uint32) threshold->value));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_port_fc_rx_qmlf_threshold_get(unit, port, threshold->flags, (uint32 *) &threshold->value));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function get the appropriate DBAL table and fields from  the COSQ resource format
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] cosq_resource - COSQ resource
 *   \param [out] table - DBAL table
 *   \param [out] set_thresh_field - DBAL field for SET threshold
 *   \param [out] clear_thresh_field - DBAL field for CLEAR threshold
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_fc_resource_to_dbal_info(
    int unit,
    dnx_cosq_threshold_index_type_t index_type,
    bcm_cosq_resource_t cosq_resource,
    dbal_tables_e * table,
    dbal_fields_e * set_thresh_field,
    dbal_fields_e * clear_thresh_field)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (cosq_resource)
    {
        case bcmResourceBytes:
            *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
            *set_thresh_field = DBAL_FIELD_DRAM_WORDS_PER_POOL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_DRAM_WORDS_PER_POOL_TH_CLR;
            break;
        case bcmResourceDramBundleDescriptorsBuffers:
            *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
            *set_thresh_field = DBAL_FIELD_DRAM_BDBS_TOTAL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_DRAM_BDBS_TOTAL_TH_CLR;
            break;
        case bcmResourceOcbBuffers:
            if (index_type == DNX_COSQ_THRESHOLD_INDEX_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_BUFFERS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_BUFFERS_TOTAL_TH_CLR;
            }
            else if (index_type == DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_BUFFERS_PER_POOL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_BUFFERS_PER_POOL_TH_CLR;
            }
            else
            {
                *table = DBAL_TABLE_FC_GLB_RCS_HDRM_THS;
                *set_thresh_field = DBAL_FIELD_HDRM_BUFFERS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_HDRM_BUFFERS_TOTAL_TH_CLR;
            }
            break;
        case bcmResourceOcbPacketDescriptors:
            if (index_type == DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO)
            {
                *table = DBAL_TABLE_FC_GLB_RCS_PER_POOL_THS;
                *set_thresh_field = DBAL_FIELD_SRAM_PDS_PER_POOL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_SRAM_PDS_PER_POOL_TH_CLR;
            }
            else
            {
                *table = DBAL_TABLE_FC_GLB_RCS_HDRM_THS;
                *set_thresh_field = DBAL_FIELD_HDRM_PDS_TOTAL_TH_SET;
                *clear_thresh_field = DBAL_FIELD_HDRM_PDS_TOTAL_TH_CLR;
            }
            break;
        case bcmResourceOcbPacketDescriptorsBuffers:
            *table = DBAL_TABLE_FC_GLB_RCS_TOTAL_THS;
            *set_thresh_field = DBAL_FIELD_SRAM_PDBS_TOTAL_TH_SET;
            *clear_thresh_field = DBAL_FIELD_SRAM_PDBS_TOTAL_TH_CLR;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", cosq_resource);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to verify the arguments of dnx_cosq_vsq_gl_total_fc_threshold_set_get.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_total_fc_threshold_verify(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    int threshold_set, threshold_clear, is_dram_present;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE /** allow_all */ );

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");
    }

    if ((index_type == DNX_COSQ_THRESHOLD_INDEX_PRIO) && ((priority != 0) && (priority != 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority value, must be Low:1 or High:0 %d", priority);
    }

    if ((index_type == DNX_COSQ_THRESHOLD_INDEX_INVALID)
        && ((thresh_info->resource_type != bcmResourceOcbPacketDescriptors)
            && (thresh_info->resource_type != bcmResourceOcbBuffers)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid index value, must be Global resource priority");
    }

    switch (thresh_info->resource_type)
    {

        case bcmResourceDramBundleDescriptorsBuffers:
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
        case bcmResourceOcbPacketDescriptorsBuffers:
            /** expected resources */

            if (is_set)
            {
                /** set API only*/

                /** Get the appropriate DBAL table and fields */
                SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                                (unit, index_type, thresh_info->resource_type, &table, &set_thresh_field,
                                 &clear_thresh_field));

                /** Check "set" value */
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                                (unit, table, set_thresh_field, FALSE, 0, 0, &threshold_set));
                if (threshold->thresh_set > threshold_set)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Threshold value to set Flow Control(%d) is bigger than the maximum allowed(%d).\n",
                                 threshold->thresh_set, threshold_set);
                }

                /** Check "clear" value */
                SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                                (unit, table, clear_thresh_field, FALSE, 0, 0, &threshold_clear));
                if (threshold->thresh_clear > threshold_clear)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Threshold value to clear Flow Control(%d) is bigger than the maximum allowed(%d).\n",
                                 threshold->thresh_clear, threshold_clear);
                }

                /** BDBs thresholds are irrelevant in systems without DRAM */
                is_dram_present = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
                if (!is_dram_present && thresh_info->resource_type == bcmResourceDramBundleDescriptorsBuffers)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "DRAM BDBs threshold cannot be set on a system without DRAM\n");
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to set the HW for the FC thresholds of total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] cosq_resource - COSQ resource
 *   \param [in,out] set_threshold - threshold value to start generating FC
 *   \param [in,out] clear_threshold - threshold value to stop generating FC
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_vsq_gl_total_fc_threshold_hw_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_resource_t cosq_resource,
    uint32 *set_threshold,
    uint32 *clear_threshold,
    uint8 is_set)
{
    uint32 entry_handle_id;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the appropriate DBAL table and fields */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                    (unit, index_type, cosq_resource, &table, &set_thresh_field, &clear_thresh_field));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (table != DBAL_TABLE_FC_GLB_RCS_HDRM_THS)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_PRIO, priority);
    }

    if (is_set)
    {
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, set_thresh_field, INST_SINGLE, *set_threshold);
        dbal_entry_value_field32_set(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, *clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Getting value fields */
        dbal_value_field32_request(unit, entry_handle_id, set_thresh_field, INST_SINGLE, set_threshold);
        dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function set or get the thresholds for Flow control generation from total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gl_total_fc_threshold_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    bcm_cosq_static_threshold_t threshold_internal;
    uint32 granularity;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ranularity for the internal representation encoding of the thresholds */
    granularity =
        ((thresh_info->resource_type ==
          bcmResourceBytes) ? dnx_data_ingr_congestion.
         info.bytes_threshold_granularity_get(unit) : dnx_data_ingr_congestion.info.threshold_granularity_get(unit));

    /** Convert threshold to internal representation according to granularity */
    if (is_set)
    {
        threshold_internal.thresh_set = threshold->thresh_set / granularity;
        threshold_internal.thresh_clear = threshold->thresh_clear / granularity;
    }

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_vsq_gl_total_fc_threshold_verify(unit, core_id, index_type,
                                                                    priority, thresh_info, &threshold_internal,
                                                                    is_set));
    /** Set/get HW */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_total_fc_threshold_hw_set_get(unit, core_id, index_type, priority,
                                                                  thresh_info->resource_type,
                                                                  (uint32 *) &threshold_internal.thresh_set,
                                                                  (uint32 *) &threshold_internal.thresh_clear, is_set));

    /** Convert internal representation to threshold according to granularity */
    if (!is_set)
    {
        threshold->thresh_set = threshold_internal.thresh_set * granularity;
        threshold->thresh_clear = threshold_internal.thresh_clear * granularity;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to verify the arguments of dnx_cosq_vsq_gl_total_fc_threshold_set_get.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_cosq_vsq_gl_per_pool_fc_threshold_verify(
    int unit,
    int core_id,
    int pool_id,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    dnx_ingress_congestion_resource_type_e resource_type;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE /** allow_all */ );

    if (thresh_info->is_free_resource != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This threshold is configured for free resources, is_free_resource must be set to 1");
    }

    if ((priority != 0) && (priority != 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority value, must be Low:1 or High:0 %d", priority);

    }

    if (pool_id < 0 || pool_id >= dnx_data_ingr_congestion.vsq.pool_nof_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Pool ID value %d", pool_id);
    }

    switch (thresh_info->resource_type)
    {

        case bcmResourceBytes:
        case bcmResourceOcbBuffers:
        case bcmResourceOcbPacketDescriptors:
            /** expected resources */
            if (is_set)
            {
                /** set API */
                SHR_IF_ERR_EXIT(dnx_ingress_congestion_cosq_resource_to_ingress_resource(unit,
                                                                                         thresh_info->resource_type,
                                                                                         &resource_type));

                if (threshold->thresh_set > dnx_data_ingr_congestion.info.resource_get(unit, resource_type)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Set threshold(%d) higher than maximum allowed(%d)",
                                 threshold->thresh_set, dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                                   resource_type)->max);

                }
                if (threshold->thresh_clear > dnx_data_ingr_congestion.info.resource_get(unit, resource_type)->max)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Clear threshold(%d) higher than maximum allowed(%d)",
                                 threshold->thresh_clear, dnx_data_ingr_congestion.info.resource_get(unit,
                                                                                                     resource_type)->max);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected resource type %d", thresh_info->resource_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to set the HW for the FC thresholds of total global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] cosq_resource - COSQ resource
 *   \param [in,out] set_threshold - threshold value to start generating FC
 *   \param [in,out] clear_threshold - threshold value to stop generating FC
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_vsq_gl_per_pool_fc_threshold_hw_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int pool_id,
    int priority,
    bcm_cosq_resource_t cosq_resource,
    uint32 *set_threshold,
    uint32 *clear_threshold,
    uint8 is_set)
{
    uint32 entry_handle_id;
    dbal_tables_e table;
    dbal_fields_e set_thresh_field;
    dbal_fields_e clear_thresh_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the appropriate DBAL table and fields */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_fc_resource_to_dbal_info
                    (unit, index_type, cosq_resource, &table, &set_thresh_field, &clear_thresh_field));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_PRIO, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RCS_POOL, pool_id);

    if (is_set)
    {
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, set_thresh_field, INST_SINGLE, *set_threshold);
        dbal_entry_value_field32_set(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, *clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /** Getting value fields */
        dbal_value_field32_request(unit, entry_handle_id, set_thresh_field, INST_SINGLE, set_threshold);
        dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field, INST_SINGLE, clear_threshold);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Function set or get the thresholds for Flow control generation from per pool global resources.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - Core ID
 *   \param [in] index_type - indicates the type of the index according to which priority is determined
 *   \param [in] pool_id - Pool ID
 *   \param [in] priority - global resource priority - high or low
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in,out] threshold - threshold value
 *   \param [in] is_set -indicates weather the function was called from the set or get API
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    int pool_id,
    int priority,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold,
    uint8 is_set)
{
    bcm_cosq_static_threshold_t threshold_internal;
    uint32 granularity;
    SHR_FUNC_INIT_VARS(unit);

    /** Determine the ranularity for the internal representation encoding of the thresholds */
    granularity =
        ((thresh_info->resource_type ==
          bcmResourceBytes) ? dnx_data_ingr_congestion.
         info.bytes_threshold_granularity_get(unit) : dnx_data_ingr_congestion.info.threshold_granularity_get(unit));

    /** Convert threshold to internal representation according to granularity */
    if (is_set)
    {
        threshold_internal.thresh_set = threshold->thresh_set / granularity;
        threshold_internal.thresh_clear = threshold->thresh_clear / granularity;
    }

    /** Verify API arguments */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_vsq_gl_per_pool_fc_threshold_verify(unit, core_id, pool_id,
                                                                       priority, thresh_info, &threshold_internal,
                                                                       is_set));

    /** Set/get HW */
    SHR_IF_ERR_EXIT(dnx_cosq_vsq_gl_per_pool_fc_threshold_hw_set_get(unit, core_id, index_type,
                                                                     pool_id, priority,
                                                                     thresh_info->resource_type,
                                                                     (uint32 *) &threshold_internal.thresh_set,
                                                                     (uint32 *) &threshold_internal.thresh_clear,
                                                                     is_set));

    /** Convert internal representation to threshold according to granularity */
    if (!is_set)
    {
        threshold->thresh_set = threshold_internal.thresh_set * granularity;
        threshold->thresh_clear = threshold_internal.thresh_clear * granularity;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_glb_rsc_threshold_set(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_PRIO:
        case DNX_COSQ_THRESHOLD_INDEX_INVALID:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_total_fc_threshold_set_get(unit, core_id, index_type,
                                                                     index_info->priority,
                                                                     thresh_info, threshold, TRUE));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(unit, core_id, index_type,
                                                                        index_info->pool_id,
                                                                        index_info->priority,
                                                                        thresh_info, threshold, TRUE));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid index type %d", unit, index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fc_glb_rsc_threshold_get(
    int unit,
    int core_id,
    dnx_cosq_threshold_index_type_t index_type,
    dnx_cosq_threshold_index_info_t * index_info,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (index_type)
    {
        case DNX_COSQ_THRESHOLD_INDEX_PRIO:
        case DNX_COSQ_THRESHOLD_INDEX_INVALID:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_total_fc_threshold_set_get(unit, core_id,
                                                                     index_type,
                                                                     index_info->priority,
                                                                     thresh_info, threshold, FALSE));
            break;
        case DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO:
            SHR_IF_ERR_EXIT(dnx_fc_vsq_gl_per_pool_fc_threshold_set_get(unit, core_id, index_type,
                                                                        index_info->pool_id,
                                                                        index_info->priority,
                                                                        thresh_info, threshold, FALSE));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unit %d, Invalid index type %d", unit, index_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header file for description.
 */

void
dnx_fc_generic_bmp_profile_template_print_cb(
    int unit,
    const void *data)
{
    bcm_core_t core = 0;
    dnx_pfc_generic_bmp_t *pfc_generic_bmp = (dnx_pfc_generic_bmp_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Core", core, "PFC Generic Bitmaps for Core",
                                       NULL);
        {
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();
             /* coverity[overrun-buffer-val:FALSE]  */
            SW_STATE_PRETTY_PRINT_ADD_ARR(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Bitmap",
                                          pfc_generic_bmp->qpair_bits[core], NULL, "0x%x",
                                          (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS / (sizeof(uint32) * 8)));
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();
        }
    }
    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief -
 * To init Flow Control related memories and registers.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 fld_val;
    uint32 interface_number, coe_chan_id;
    int imb_id;
    uint32 nof_imbs;
    imb_dispatch_type_t type;
    dnx_pfc_generic_bmp_t default_data;
    dnx_algo_template_create_data_t fc_generic_bmp;
    uint32 nof_gen_bmps, prio;
    uint32 max_ref = 0;
    uint32 coe_tick_mhz = 0;
    uint64 u64_calc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_control_db.init(unit));

        /** Get the number of Generic PFC Bitmaps in order to create a resource in the resource manager for allocating indexes */
    nof_gen_bmps = dnx_data_fc.general.nof_gen_bmps_get(unit);
    /*
     * Calculate the max references for the template - theoretically it is possible that the entire OOB calendar
     * (both interfaces) and all PFCs will be mapped to only one bitmap, so max_ref must be the size of the calendars + all PFCs.
     */
    for (interface_number = 0; interface_number < dnx_data_fc.oob.nof_oob_ids_get(unit); interface_number++)
    {
        max_ref += dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx;
    }
    max_ref += dnx_data_egr_queuing.params.nof_q_pairs_get(unit);

        /** Create template manager insance - handle: RESOURCE_FC_GENERIC_BMP for the index of generic PFC bitmaps */
    sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
    sal_memset(&fc_generic_bmp, 0, sizeof(dnx_algo_template_create_data_t));
    fc_generic_bmp.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    fc_generic_bmp.first_profile = 0;
    /*
     * The max Bitmap index is 31, which means we have 32 in total Generic Bitmaps in HW (0-31).
     * Number of profiles is 33 and not 32 because the profile with index 32 is used as a default profile and does not correspond
     * to a HW Generic Bitmap. This is done so that we don't take one of the indexes existing in HW for deault profile and with
     * that lower the number of usable bitmaps.
     */
    fc_generic_bmp.nof_profiles = nof_gen_bmps + 1;
    fc_generic_bmp.default_profile = nof_gen_bmps;
    fc_generic_bmp.data_size = sizeof(dnx_pfc_generic_bmp_t);
    fc_generic_bmp.default_data = &default_data;
    fc_generic_bmp.max_references = max_ref;
    sal_strncpy(fc_generic_bmp.name, RESOURCE_FC_GENERIC_BMP, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.create(unit, &fc_generic_bmp, NULL));

    /*
     * Configure the NIF Flow Control Interface to process indications from CFC.
     * If set, the Flow Control Interface will be held in reset, so set to 0 to get it out of reset.
     */
    SHR_IF_ERR_EXIT(imb_fc_reset_all(unit, FALSE));

        /** Set the CDUs to enable the generation of LLFC based on Global Resources */
    SHR_IF_ERR_EXIT(imb_llfc_from_glb_rsc_enable_all(unit, TRUE));

    /*
     * Get number of IMB instances in use
     */
    SHR_IF_ERR_EXIT(imb_nof_get(unit, &nof_imbs));

        /** Per ETHU map the received PFC - each priority received by NIF should be mapped to a priority that will be sent to EGQ in order to stop a Q-pair */
    for (imb_id = 0; imb_id < nof_imbs; imb_id++)
    {
            /** Chec type, mapping can be set only for CDU type IMBs */
        SHR_IF_ERR_EXIT(imb_id_type_get(unit, imb_id, &type));
        if (type != imbDispatchTypeImb_cdu)
        {
            continue;
        }

            /** Unmap the priorities of the PFC signal received from RX Port Macro to clear any pre-existing mapping */
        SHR_IF_ERR_EXIT(imb_pfc_rec_priority_unmap(unit, imb_id));
        for (prio = 0; prio < BCM_COS_COUNT; prio++)
        {
                /** Map the priorities of the PFC signal received from RX Port Macro, default is 1 to 1 mapping */
            SHR_IF_ERR_EXIT(imb_pfc_rec_priority_map(unit, imb_id, prio, prio));
        }
    }

        /** Global resources masking, bit 0 - LP mask, bit 1 - HP mask */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GLB_RSC_MASKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_POOL_0_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_POOL_0_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_POOL_1_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_POOL_1_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** HDRAM does not have low and high priority */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HDRM_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Enable Category 2 & Traffic class mapping to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_CAT2_TC_MAP_PFC_ENA, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** CFC Enablers for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_CFC_ENABLERS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_RCL_PFC_HP_CFG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_RCL_PFC_LP_CFG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_EGQ_RCL_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_oob_fast_llfc))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_NIF_LLFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_NIF_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_VSQ_TO_NIF_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_VSQ_TO_NIF_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_additional_enablers))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_PP_VSQ_TO_NIF_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_PP_VSQ_TO_NIF_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_TX_LLFC_STOP_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_TX_GEN_LLFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_CHAIN_TX_GEN_PFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_TX_PFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    }
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_inb_llfc))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ILU_TX_LLFC_STOP_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ILU_TX_GEN_LLFC_EN, INST_SINGLE, 1);
    }
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_if_2_cfc_enablers))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ETH_RX_LLFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ETH_RX_PFC_EN, INST_SINGLE, 1);
        /** Mask for enabling CGM2CFC VSQ's interface: Bit 0 - VSQA , Bit 1 - VSQB ,... ,Bit 6 - VSQF */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CGM_VSQS_RX_EN, INST_SINGLE, 0x3F);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) != 0)
    {
            /** Enablers for ILKN Inband flow-control */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_CFC_ILKN_INBAND_ENABLERS, entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_EN, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

        /** OOB configurations - take into reset */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_OOB_PARAMETERS, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_RSTN, INST_SINGLE, 0);
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_oob_tx_reset))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_RSTN, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure OOB Type: ILKN/SPI
     */
    for (interface_number = 0; interface_number < dnx_data_fc.oob.nof_oob_ids_get(unit); interface_number++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, interface_number);
        fld_val = (dnx_data_fc.oob.port_get(unit, interface_number)->type == DNX_OOB_TYPE_ILKN ? 1 : 0);

        if ((dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_RX_ENABLE) ||
            (dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_MODE, INST_SINGLE, fld_val);
                /** If type is ILKN */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == DNX_OOB_TYPE_ILKN)
            {
                if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_polarity_set))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_FC_POLARITY, INST_SINGLE,
                                                 1);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_FC_POLARITY_INVERT,
                                             INST_SINGLE, dnx_data_fc.oob.indication_invert_get(unit,
                                                                                                interface_number)->ilkn_rx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx - 1));
                if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_oob_en))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_EN, INST_SINGLE, 1);
                }
                    /** OOB ILKN RX - take out of reset */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_RSTN, INST_SINGLE, 1);
            }

                /** If type is SPI */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == DNX_OOB_TYPE_SPI)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->spi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_CAL_M, INST_SINGLE,
                                             dnx_data_fc.oob.calender_rep_count_get(unit, interface_number)->rx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_EN, INST_SINGLE, 1);
            }
        }

        if ((dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_ENABLE) ||
            (dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_MODE, INST_SINGLE, fld_val);
                /** If type is ILKN */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == DNX_OOB_TYPE_ILKN)
            {
                if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_polarity_set))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_FC_POLARITY, INST_SINGLE,
                                                 1);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_FC_POLARITY_INVERT,
                                             INST_SINGLE, dnx_data_fc.oob.indication_invert_get(unit,
                                                                                                interface_number)->ilkn_tx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->tx - 1));
                if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_ilkn_oob_en))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_EN, INST_SINGLE, 1);
                }
                    /** OOB ILKN TX - take out of reset */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_RSTN, INST_SINGLE, 1);
            }

                /** If type is SPI */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == DNX_OOB_TYPE_SPI)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->spi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->tx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_CAL_M, INST_SINGLE,
                                             dnx_data_fc.oob.calender_rep_count_get(unit, interface_number)->tx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_EN, INST_SINGLE, 1);
            }

                /** OOB TX CLK Speed ratio, phase and rough phase*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_SPEED, INST_SINGLE,
                                         dnx_data_fc.oob.tx_freq_get(unit, interface_number)->ratio);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_PHASE, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_ROUGH_PHASE, INST_SINGLE, 1);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

        /** OOB configurations - take out of reset */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_RSTN, INST_SINGLE, 1);
    if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_oob_tx_reset))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_RSTN, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** COE settings - enable, set mode, set tick, Control opcode, MAC address, data and VID offsets, Ether type */
    for (interface_number = 0; interface_number < dnx_data_fc.coe.nof_coe_cal_instances_get(unit); interface_number++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_COE_PARAMETERS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_IF, interface_number);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_RX_EN, INST_SINGLE, 1);
        if (dnx_data_fc.general.feature_get(unit, dnx_data_fc_general_pp_inb_enable))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_INB_ENB, INST_SINGLE, 1);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_COE_MODE, INST_SINGLE,
                                     dnx_data_fc.coe.calendar_mode_get(unit)->mode);
        /** Calculate COE tick in MHz */
        COMPILER_64_ZERO(u64_calc);
        COMPILER_64_SET(u64_calc, 0, dnx_data_device.general.core_clock_khz_get(unit));
        COMPILER_64_UMUL_32(u64_calc, dnx_data_fc.coe.calendar_pause_resolution_get(unit));
        COMPILER_64_UDIV_32(u64_calc, 1000);
        coe_tick_mhz = COMPILER_64_LO(u64_calc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_COE_TICK_MHZ, INST_SINGLE, coe_tick_mhz);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_CONTROL_OPCODE, INST_SINGLE,
                                     dnx_data_fc.coe.control_opcode_get(unit));
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_COE_MAC_ADDRESS, INST_SINGLE,
                                        (uint8 *) dnx_data_fc.coe.mac_address_get(unit)->mac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_ETHER_TYPE, INST_SINGLE,
                                     dnx_data_fc.coe.ethertype_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_DATA_OFFSET, INST_SINGLE,
                                     dnx_data_fc.coe.data_offset_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_VID_OFFSET, INST_SINGLE,
                                     dnx_data_fc.coe.coe_vid_offset_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_VID_MASK, INST_SINGLE,
                                     dnx_data_fc.coe.coe_vid_mask_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_VID_SHIFT_RIGHT, INST_SINGLE,
                                     dnx_data_fc.coe.coe_vid_right_shift_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_RX_CAL_LEN, INST_SINGLE,
                                     (dnx_data_fc.coe.calender_length_get(unit, interface_number)->len - 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Map VID to Channel ID 1x1  - mapping VLAN IDs from 0 to COE calendar length to the corresponding COE channels */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_COE_VID_CHANNEL_ID_MAP, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_IF, interface_number);
        for (coe_chan_id = 0; coe_chan_id < dnx_data_fc.coe.calender_length_get(unit, interface_number)->len;
             coe_chan_id++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, coe_chan_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_CHANNEL_ID, INST_SINGLE, coe_chan_id);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

        /** Set the initial values for the Map Select in OOB TX calendar, for the value of FC_INDEX we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_OOB_CAL_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, DNX_TX_CAL_SRC_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Set the initial values for the Map Select in OOB RX calendar, for the value of MAP_VAL we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_OOB_CAL_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, DNX_RX_CAL_DEST_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) != 0)
    {
            /** ILU flow control settings init */
        SHR_IF_ERR_EXIT(imb_inband_ilkn_fc_init_all(unit));

            /** Inband ILKN calendar settings */
        for (interface_number = 0; interface_number < dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit);
             interface_number++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_INB_ILKN_PARAMETERS, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, interface_number);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CAL_LEN, INST_SINGLE,
                                         (dnx_data_fc.inband.calender_length_get(unit, interface_number)->tx - 1));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

            /** Set the initial values for the Map Select in Inband ILKN TX calendar, for the value of FC_INDEX we are relying on the reset value which is 0 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_INB_ILKN_CAL_MAP, entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, DNX_TX_CAL_SRC_CONST);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /** Set the initial values for the Map Select in Inband ILKN RX calendar, for the value of MAP_VAL we are relying on the reset value which is 0 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_INB_ILKN_CAL_MAP, entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, DNX_RX_CAL_DEST_CONST);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

        /** Set the initial values for the Map Select in COE RX calendar, for the value of MAP_VAL we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_COE_CAL_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, DNX_RX_CAL_DEST_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
