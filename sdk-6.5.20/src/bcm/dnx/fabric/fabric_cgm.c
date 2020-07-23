/** \file fabric_cgm.c
 * 
 *
 * Fabric CGM functions for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/fabric.h>
#include <bcm/fabric.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_cgm.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/** Highest configuration allowed by HW for slow start timer period */
#define DNX_FABRIC_CGM_SLOW_START_TIMER_PERIOD_MAX  (0x11)

/************
 * TYPEDEFS *
 * {        *
 */

/*
 * \brief
 *   Struct to hold dbal info regarding fabric cgm control type
 */
typedef struct dnx_fabric_cgm_control_access_info_s
{
    /** Control type */
    bcm_fabric_cgm_control_type_t control_type;
    /** DBAL Table of control_type */
    dbal_tables_e dbal_table;
    /** DBAL Result of control_type */
    dbal_fields_e dbal_result;
    /** Index type of DBAL keys */
    dnx_fabric_cgm_control_index_type_e index_type;
    /** DBAL Key1 of control_type */
    dbal_fields_e dbal_key1;
    /** DBAL Key2 of control_type */
    dbal_fields_e dbal_key2;
} dnx_fabric_cgm_control_access_info_t;

/*
 * \brief
 *   Fabric CGM threshold Key index range to configure.
 */
typedef struct dnx_fabric_cgm_control_key_range_s
{
    /** Key's first index to configure */
    int start_index;
    /** Key's last index to configure */
    int end_index;
} dnx_fabric_cgm_control_key_range_t;

/*
 * \brief
 *   Fabric CGM threshold Keys info to configure.
 */
typedef struct dnx_fabric_cgm_control_key_info_s
{
    /** Key1 range info */
    dnx_fabric_cgm_control_key_range_t key1;
    /** Key2 range info */
    dnx_fabric_cgm_control_key_range_t key2;
} dnx_fabric_cgm_control_keys_info_t;

/************
 * }        *
 */

/*************
 * Functions *
 * {         *
 */

/** See function implementation */
static shr_error_e dnx_fabric_cgm_control_key_range_get(
    int unit,
    dbal_tables_e dbal_table,
    dbal_fields_e dbal_key,
    int *min_value,
    int *max_value);

/*
 * \brief
 *   Get shaper boundaries according to HW.
 * \see
 *   \ref dnx_algo_rates_rate_to_credit_interval
 */
static shr_error_e
dnx_fabric_cgm_shaper_bounds_get(
    int unit,
    dbal_tables_e dbal_table,
    dnx_algo_rates_shaper_bounds_t * shaper_bounds)
{
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get 'credit' boundaries.
     */
    sal_memset(&field_info, 0, sizeof(field_info));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table, DBAL_FIELD_CALENDAR, FALSE, 0, 0, &field_info));
    shaper_bounds->credit_min = 1;
    shaper_bounds->credit_max = field_info.max_value;

    /*
     * Get 'interval' boundaries.
     */
    sal_memset(&field_info, 0, sizeof(field_info));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table, DBAL_FIELD_DELAY, FALSE, 0, 0, &field_info));
    shaper_bounds->interval_min = 1;
    shaper_bounds->interval_max = field_info.max_value;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Convert bcm_fabric_mc_shaper_t type to ing_transmit_shaper type.
 *   The enums are different for some tables.
 */
static shr_error_e
dnx_fabric_cgm_fmc_shaper_to_ing_transmit_shaper(
    int unit,
    bcm_fabric_mc_shaper_t fmc_shaper,
    dbal_enum_value_field_ing_transmit_shaper_e * ing_transmit_shaper)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (fmc_shaper)
    {
        case bcmFabricMcShaperGfmc:
        {
            *ing_transmit_shaper = DBAL_ENUM_FVAL_ING_TRANSMIT_SHAPER_FMC_DEV3_HP;
            break;
        }
        case bcmFabricMcShaperBfmc:
        {
            *ing_transmit_shaper = DBAL_ENUM_FVAL_ING_TRANSMIT_SHAPER_FMC_DEV3_LP;
            break;
        }
        case bcmFabricMcShaperTotalFmc:
        {
            *ing_transmit_shaper = DBAL_ENUM_FVAL_ING_TRANSMIT_SHAPER_FMC_DEV3_TOTAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown fmc_shaper %d", fmc_shaper);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM RCI module.
 */
static shr_error_e
dnx_fabric_cgm_rci_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 pipe = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_RCI_ENABLE, &entry_handle_id));

    /*
     * Enable RCI for active pipes.
     * Disable RCI for non-active pipes.
     */
    for (pipe = 0; pipe < max_nof_pipes; ++pipe)
    {
        /** Clearing the handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_RCI_ENABLE, entry_handle_id));

        /** Setting key field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe);

        /** Setting result field */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (pipe < nof_pipes) ? 1 : 0);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM GCI congestion severity.
 */
static shr_error_e
dnx_fabric_cgm_gci_backoff_congestion_severity_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 weight = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    weight = 1;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_CONGESTION_SEVERITY_WEIGHTS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONGESTION_SEVERITY_WORSE_WEIGHT, INST_SINGLE,
                                 weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONGESTION_SEVERITY_BETTER_WEIGHT, INST_SINGLE,
                                 weight);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Defaults for GCI random backoff timer max value per timer/level.
 */
static const uint32 dnx_fabric_cgm_gci_backoff_timer_level_max_val[][4] = {
    /* *INDENT-OFF* */
    /* Level0   | Level1    | Level2    | Level3 */
    {  0x1,       0xf,        0x7f,       0x3ff },   /* Timer0 */
    {  0x3,       0x1f,       0xff,       0x7ff },   /* Timer1 */
    {  0x7,       0x3f,       0x1ff,      0xfff }    /* Timer2 */
    /* *INDENT-ON* */
};

/*
 * \brief
 *   Initialize Fabric CGM GCI backoff timer mask.
 */
static shr_error_e
dnx_fabric_cgm_gci_backoff_mask_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    int timer = 0;
    int level = 0;
    int min_level = 0;
    int max_level = 0;
    int min_timer = 0;
    int max_timer = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                    (unit, DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_TIMER_MAX, DBAL_FIELD_BACKOFF_TIMER_ID, &min_timer,
                     &max_timer));
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                    (unit, DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_TIMER_MAX, DBAL_FIELD_LEVEL, &min_level, &max_level));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_TIMER_MAX, &entry_handle_id));
    for (timer = min_timer; timer <= max_timer; ++timer)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BACKOFF_TIMER_ID, timer);

        for (level = min_level; level <= max_level; ++level)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEVEL, level);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_VALUE, INST_SINGLE,
                                         dnx_fabric_cgm_gci_backoff_timer_level_max_val[timer][level]);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM GCI module.
 */
static shr_error_e
dnx_fabric_cgm_gci_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_cgm_gci_backoff_congestion_severity_init(unit));

    SHR_IF_ERR_EXIT(dnx_fabric_cgm_gci_backoff_mask_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize Fabric CGM LCI module.
 */
static shr_error_e
dnx_fabric_cgm_fabric_shaper_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_FABRIC_SHAPER_ENABLE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SHAPER_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM Slow Start module.
 */
static shr_error_e
dnx_fabric_cgm_slow_start_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    dbal_enum_value_field_ing_transmit_shaper_e ing_transmit_shaper = 0;
    uint32 shaper_interval = 0;
    uint32 shaper_credit = 0;
    uint32 shaper_rate = 0;
    uint32 interval_resolution = 2;
    uint32 credit_resolution = 1;
    dnx_algo_rates_shaper_bounds_t shaper_bounds;
    bcm_fabric_mc_shaper_t fmc_shaper = 0;
    uint32 phase = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&shaper_bounds, 0, sizeof(shaper_bounds));

    /*
     * Max rate is dependent on core clock and bus size.
     * Take extra 5% to be on the safe side.
     * shaper_rate is in KBytes/Sec.
     */
    shaper_rate =
        dnx_data_device.general.core_clock_khz_get(unit) * (dnx_data_device.general.bus_size_in_bits_get(unit) /
                                                            SAL_CHAR_BIT) / 100 * 105;
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_shaper_bounds_get
                    (unit, DBAL_TABLE_FABRIC_CGM_ING_TRANSMIT_PER_CORE_SHAPERS_CONFIG, &shaper_bounds));
    SHR_IF_ERR_EXIT(dnx_algo_rates_rate_to_credit_interval
                    (unit, shaper_rate, &shaper_bounds, interval_resolution, credit_resolution, &shaper_interval,
                     &shaper_credit));

    /*
     * Enable only FMC shapers.
     * Set them to maximum bandwidth.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_FABRIC_CGM_ING_TRANSMIT_PER_CORE_SHAPERS_CONFIG, &entry_handle_id));
    for (fmc_shaper = 0; fmc_shaper < bcmFabricMcShaperNof; ++fmc_shaper)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_cgm_fmc_shaper_to_ing_transmit_shaper(unit, fmc_shaper, &ing_transmit_shaper));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ING_TRANSMIT_SHAPER, ing_transmit_shaper);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, shaper_interval);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, shaper_credit);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, 0x7ffff);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DCR_FACTOR, INST_SINGLE, 0x3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 0x1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Set Slow Start timer period to maximum.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG, entry_handle_id));
    for (fmc_shaper = 0; fmc_shaper < bcmFabricMcShaperNof; ++fmc_shaper)
    {
        for (phase = 0; phase < bcmFabricPhaseNof; ++phase)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMC_SHAPER_ID, fmc_shaper);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHASE, phase);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_PERIOD, INST_SINGLE,
                                         DNX_FABRIC_CGM_SLOW_START_TIMER_PERIOD_MAX);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set Fabric Link & Egress FIFOs size.
 */
static shr_error_e
dnx_fabric_cgm_llfc_fifo_size_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 pipe = 0;
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 base_address = 0;
    dbal_table_field_info_t field_info;
    uint32 link_fifo_base_address_max_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_info, 0, sizeof(field_info));

    /*
     * Get max address of FIFOs.
     * Will be used to set size of pipe's FIFO to 0.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_FABRIC_CGM_LINK_FIFO_BASE_ADDRESS, DBAL_FIELD_BASE_ADDRESS, FALSE, 0, 0,
                     &field_info));
    link_fifo_base_address_max_value = field_info.max_value;

    /*
     * Set size of each pipe by setting the base address of the next pipe.
     * Pipe0 starts from address 0, so need to confgure only pipe1 and pipe2.
     * Set base address of unused pipes to start on max address, thus disabling it.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_LINK_FIFO_BASE_ADDRESS, &entry_handle_id));

    for (pipe = 1; pipe < max_nof_pipes; ++pipe)
    {
        base_address =
            (pipe <
             nof_pipes) ? (link_fifo_base_address_max_value / nof_pipes * pipe) : (link_fifo_base_address_max_value);
        /** base_address must be even number */
        base_address = base_address / 2 * 2;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_LINK_FIFO_BASE_ADDRESS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_ADDRESS, INST_SINGLE, base_address);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Use default values if fabric egress fifos are static
     * Configure fabric egress fifos base addresses only when fifos
     * size is dynamic. For static fifos size use default values.
     */
    if (!dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_egress_fifo_static_size))
    {
        uint32 egress_fifo_base_address_max_value = 0;

        /*
         * Get max address of FIFOs.
         * Will be used to set size of pipe's FIFO to 0.
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_BASE_ADDRESS, DBAL_FIELD_BASE_ADDRESS, FALSE, 0, 0,
                         &field_info));
        egress_fifo_base_address_max_value = field_info.max_value;

        /*
         * Set size of each pipe by setting the base address of the next pipe.
         * Pipe0 starts from address 0, so need to confgure only pipe1 and pipe2.
         * Set base address of unused pipes to start on max address, thus disabling it.
         */
        for (pipe = 1; pipe < max_nof_pipes; ++pipe)
        {
            base_address =
                (pipe <
                 nof_pipes) ? (egress_fifo_base_address_max_value / nof_pipes *
                               pipe) : (egress_fifo_base_address_max_value);
            /** base_address must be even number */
            base_address = base_address / 2 * 2;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_BASE_ADDRESS, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE, pipe);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_ADDRESS, INST_SINGLE, base_address);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM LLFC mapping.
 */
static shr_error_e
dnx_fabric_cgm_llfc_map_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 ing_transmit_context = 0;
    uint32 pipe = 0;
    uint32 enable = 0;
    uint32 is_active_pipe = 0;
    uint32 is_tdm_pipe = 0;
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 max_nof_contexts = dnx_data_fabric.pipes.max_nof_contexts_get(unit);
    uint32 max_nof_subcontexts = dnx_data_fabric.pipes.max_nof_subcontexts_get(unit);
    uint32 nof_subcontexts_per_context = max_nof_subcontexts / max_nof_contexts;
    int tdm_pipe = dnx_data_fabric.pipes.map_get(unit)->tdm_pipe;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Map LLFC between context and pipe.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_LLFC_ING_TRANSMIT_CONTEXT_MAP, &entry_handle_id));
    for (ing_transmit_context = 0; ing_transmit_context < max_nof_subcontexts; ++ing_transmit_context)
    {
        uint32 is_tdm_packet_context = 0;
        uint32 dtq_id = ing_transmit_context / nof_subcontexts_per_context;

        if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET)
        {
            /** Is packet TDM context */
            uint32 ocb_context = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, dtq_id, 1)->index;
            if (ing_transmit_context == ocb_context)
            {
                is_tdm_packet_context = 1;
            }
        }

        for (pipe = 0; pipe < max_nof_pipes; ++pipe)
        {
            uint32 is_pipe_connected_to_context = 1;

            /** Is pipe active */
            is_active_pipe = (pipe < nof_pipes) ? 1 : 0;

            /** Is TDM pipe */
            is_tdm_pipe = (pipe == tdm_pipe) ? 1 : 0;

            /** Is pipe connected to context */
            if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
            {
                if (pipe != dtq_id)
                {
                    is_pipe_connected_to_context = 0;
                }
            }
            else if (dnx_data_fabric.mesh.multicast_enable_get(unit))
            {
                int uc_pipe = dnx_data_fabric.pipes.map_get(unit)->uc[0];
                int mc_pipe = dnx_data_fabric.pipes.map_get(unit)->mc[0];
                int mc_context = max_nof_contexts - 1;

                if (((pipe == mc_pipe) && (dtq_id != mc_context)) || ((pipe == uc_pipe) && (dtq_id == mc_context)))
                {
                    is_pipe_connected_to_context = 0;
                }
            }

            /*
             * The FC should be enabled when the pipe is active, the pipe is connected to the context,
             * the pipe is not TDM pipe, and the context is not Packet-TDM context.
             */
            enable = is_active_pipe && is_pipe_connected_to_context && !is_tdm_pipe && !is_tdm_packet_context;

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SUBCONTEXT_ID, ing_transmit_context);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PIPE_ID, pipe);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM LLFC links enable.
 */
static shr_error_e
dnx_fabric_cgm_llfc_links_enable_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 link = 0;
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    int fmac = 0;
    int enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable LLFC for all active fabric links
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_LLFC_ENABLE, &entry_handle_id));
    for (link = 0; link < nof_links; ++link)
    {
        fmac = link / nof_links_in_fmac;

        SHR_IF_ERR_EXIT(dnx_fabric_if_fmac_block_enable_get(unit, fmac, &enable));
        if (enable)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK, link);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM LLFC module.
 */
static shr_error_e
dnx_fabric_cgm_llfc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set FDR FIFOs size
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_fifo_size_init(unit));

    /*
     * Map LLFC between context and pipe
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_map_init(unit));

    /*
     * Enable LLFC per link
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_links_enable_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM internal flow control thresholds for egress fifo.
 */
static shr_error_e
dnx_fabric_cgm_internal_flow_controls_egress_fifo_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 egress_fifo_fc_threshold = dnx_data_fabric.cgm.egress_fifo_fc_threshold_get(unit);
    uint32 pipe = 0;
    uint32 egress_fifo_base_address_max_value = 0;
    dbal_table_field_info_t field_info;
    uint32 threshold = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_info, 0, sizeof(field_info));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_BASE_ADDRESS, DBAL_FIELD_BASE_ADDRESS, FALSE, 0, 0,
                     &field_info));
    egress_fifo_base_address_max_value = field_info.max_value;

    for (pipe = 0; pipe < max_nof_pipes; ++pipe)
    {
        /*
         * Set threshold to be egress_fifo_fc_threshold below the fifo size, or to be max for unused pipes
         */
        threshold =
            (pipe <
             nof_pipes) ? (egress_fifo_base_address_max_value / nof_pipes -
                           egress_fifo_fc_threshold) : (egress_fifo_base_address_max_value);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_FC_THRESHOLDS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM internal flow control egress FIFO FC aging period.
 */
static shr_error_e
dnx_fabric_cgm_internal_flow_controls_egress_fifo_fc_aging_period_init(
    int unit)
{
    uint32 entry_handle_id = 0;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_info, 0, sizeof(field_info));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_FC_AGING_PERIOD, DBAL_FIELD_AGING_PERIOD, FALSE, 0, 0,
                     &field_info));

    /*
     * Set aging priod to maximum.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_FC_AGING_PERIOD, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGING_PERIOD, INST_SINGLE, field_info.max_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize Fabric CGM Drop.
 */
static shr_error_e
dnx_fabric_cgm_internal_flow_controls_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure fabric egress fifos flow controls only when fifos
     * size is dynamic. For static fifos size use default values.
     */
    if (!dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_egress_fifo_static_size))
    {
        /*
         * Egress FIFO internal FC init
         */
        SHR_IF_ERR_EXIT(dnx_fabric_cgm_internal_flow_controls_egress_fifo_init(unit));
    }

    /*
     * Egress FIFO FC aging period
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_internal_flow_controls_egress_fifo_fc_aging_period_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_fabric_cgm_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init RCI
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_rci_init(unit));

    /*
     * Init GCI
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_gci_init(unit));

    /*
     * Init LCI
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_fabric_shaper_init(unit));

    /*
     * Init Slow Start
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_slow_start_init(unit));

    /*
     * Init LLFC
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_init(unit));

    /*
     * Init internal flow controls
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_internal_flow_controls_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Write Fabric CGM Threshold to HW via DBAL.
 */
static shr_error_e
dnx_fabric_cgm_control_hw_set(
    int unit,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 key1,
    uint32 key2,
    uint32 value)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, access_info->dbal_table, &entry_handle_id));

    /** Setting key fields */
    if (access_info->dbal_key1 != DBAL_FIELD_EMPTY)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, access_info->dbal_key1, key1);

        if (access_info->dbal_key2 != DBAL_FIELD_EMPTY)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, access_info->dbal_key2, key2);
        }
    }

    /** Setting result field */
    dbal_entry_value_field32_set(unit, entry_handle_id, access_info->dbal_result, INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get Fabric CGM Threshold to HW via DBAL.
 */
static shr_error_e
dnx_fabric_cgm_control_hw_get(
    int unit,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 key1,
    uint32 key2,
    uint32 *value)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, access_info->dbal_table, &entry_handle_id));

    /** Setting key fields */
    if (access_info->dbal_key1 != DBAL_FIELD_EMPTY)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, access_info->dbal_key1, key1);

        if (access_info->dbal_key2 != DBAL_FIELD_EMPTY)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, access_info->dbal_key2, key2);
        }
    }

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, access_info->dbal_result, INST_SINGLE, value);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get the DBAL info corresponds to control_type.
 */
static shr_error_e
dnx_fabric_cgm_control_access_info_get(
    int unit,
    bcm_fabric_cgm_control_type_t control_type,
    dnx_fabric_cgm_control_access_info_t * access_info)
{
    uint32 is_control_type_valid = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (control_type >= bcmFabricCgmNof)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown control type %d", control_type);
    }

    /*
     * Get access info from dnx data.
     */
    is_control_type_valid = dnx_data_fabric.cgm.access_map_get(unit, control_type)->is_valid;
    if (!is_control_type_valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s", dnx_data_fabric.cgm.access_map_get(unit, control_type)->err_msg);
    }

    /*
     * Get DBAL info from DNX DATA
     */
    access_info->control_type = control_type;
    access_info->dbal_table = dnx_data_fabric.cgm.access_map_get(unit, control_type)->dbal_table;
    access_info->dbal_key1 = dnx_data_fabric.cgm.access_map_get(unit, control_type)->dbal_key1;
    access_info->dbal_key2 = dnx_data_fabric.cgm.access_map_get(unit, control_type)->dbal_key2;
    access_info->dbal_result = dnx_data_fabric.cgm.access_map_get(unit, control_type)->dbal_result;
    access_info->index_type = dnx_data_fabric.cgm.access_map_get(unit, control_type)->index_type;

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get key values range.
 */
static shr_error_e
dnx_fabric_cgm_control_key_range_get(
    int unit,
    dbal_tables_e dbal_table,
    dbal_fields_e dbal_key,
    int *min_value,
    int *max_value)
{
    dbal_table_field_info_t key_field_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&key_field_info, 0, sizeof(key_field_info));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table, dbal_key, TRUE, 0, 0, &key_field_info));

    *min_value = key_field_info.min_value;
    *max_value = key_field_info.max_value;

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Verify value of a key.
 */
static shr_error_e
dnx_fabric_cgm_control_key_verify(
    int unit,
    dbal_tables_e dbal_table,
    dbal_fields_e dbal_key,
    int key_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (key_value != -1)
    {
        int min_value = -1;
        int max_value = -1;

        SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get(unit, dbal_table, dbal_key, &min_value, &max_value));

        SHR_RANGE_VERIFY(key_value, min_value, max_value, _SHR_E_PARAM, "key %u is out of range.", key_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get keys info from control_id and dbal info.
 *   In case index==-1, keys range will span from minimum index value
 *   to its maximum value.
 */
static shr_error_e
dnx_fabric_cgm_control_keys_info_get(
    int unit,
    bcm_fabric_cgm_control_id_t control_id,
    dnx_fabric_cgm_control_access_info_t * access_info,
    dnx_fabric_cgm_control_keys_info_t * keys_info)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_info->index_type)
    {
        /** Index is composed of 'pipe' key and 'level' key */
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL:
        {
            int pipe = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_PIPE_GET(control_id);
            int level = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_LEVEL_GET(control_id);
            uint32 min_pipe = 0;
            uint32 max_pipe = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;
            int min_level = -1;
            int max_level = -1;

            /** Get level's min & max values */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                            (unit, access_info->dbal_table, access_info->dbal_key2, &min_level, &max_level));

            /*
             * Set key's range.
             * When threshold==MASK it means to configure all key's indexes.
             * Otherwise, configure only the specific key.
             */
            keys_info->key1.start_index = (pipe == bcmFabricPipeAll) ? min_pipe : pipe;
            keys_info->key1.end_index = (pipe == bcmFabricPipeAll) ? max_pipe : pipe;
            keys_info->key2.start_index = (level == bcmFabricLevelAll) ? min_level : level;
            keys_info->key2.end_index = (level == bcmFabricLevelAll) ? max_level : level;
            break;
        }
        /** Index is composed of 2 keys */
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SLOW_START_PHASE:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_PRIORITY:
        {
            int key1 = -1;
            int key2 = -1;
            int min_key1 = -1;
            int max_key1 = -1;
            int min_key2 = -1;
            int max_key2 = -1;

            /** Index is composed of 'fmc_shaper' key and 'phase' key */
            if (_SHR_FABRIC_TH_INDEX_IS_FMC_SLOW_START_PHASE(control_id))
            {
                key1 = _SHR_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_FMC_GET(control_id);
                key2 = _SHR_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SLOW_START_PHASE_GET(control_id);
            }
            /** Index is composed of 'cast' key and 'prio' key */
            else if (_SHR_FABRIC_TH_INDEX_IS_CAST_PRIO(control_id))
            {
                key1 = _SHR_FABRIC_TH_INDEX_CAST_PRIO_CAST_GET(control_id);
                key2 = _SHR_FABRIC_TH_INDEX_CAST_PRIO_PRIO_GET(control_id);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Control_id %u is not does not match index_type %d", control_id,
                             access_info->index_type);
            }

            /** Get key1's min & max values */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                            (unit, access_info->dbal_table, access_info->dbal_key1, &min_key1, &max_key1));

            /** Get key2's min & max values */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                            (unit, access_info->dbal_table, access_info->dbal_key2, &min_key2, &max_key2));

            /*
             * Set key's range.
             * When threshold==MASK it means to configure all key's indexes.
             * Otherwise, configure only the specific key.
             */
            keys_info->key1.start_index = (key1 == -1) ? min_key1 : key1;
            keys_info->key1.end_index = (key1 == -1) ? max_key1 : key1;
            keys_info->key2.start_index = (key2 == -1) ? min_key2 : key2;
            keys_info->key2.end_index = (key2 == -1) ? max_key2 : key2;
            break;
        }
        /** Index is composed of single key */
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL_SEPARATE:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SHAPER:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE:
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PRIORITY:
        {
            int min_range = -1;
            int max_range = -1;

            /** Get level's min & max values */
            if (access_info->index_type == DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE)
            {
                /** Max pipe is dependent on device configuration */
                min_range = 0;
                max_range = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_range_get
                                (unit, access_info->dbal_table, access_info->dbal_key1, &min_range, &max_range));
            }

            /*
             * Set key's range.
             * When threshold==-1 it means to configure all key's indexes.
             * Otherwise, configure only the specific key.
             */
            keys_info->key1.start_index = (control_id == -1) ? min_range : control_id;
            keys_info->key1.end_index = (control_id == -1) ? max_range : control_id;
            keys_info->key2.start_index = 0;
            keys_info->key2.end_index = 0;
            break;
        }
        /** Index is is not composed of any key */
        case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE:
        {
            keys_info->key1.start_index = 0;
            keys_info->key1.end_index = 0;
            keys_info->key2.start_index = 0;
            keys_info->key2.end_index = 0;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown index type %d", access_info->index_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Verify control_id param of bcm_dnx_fabric_cgm_control_set is valid.
 * \param [in] control_id -
 *   The control id to validate.
 * \param [in] access_info -
 *   The DBAL info which holds info about control_id expected type and boundaries.
 */
static shr_error_e
dnx_fabric_cgm_control_control_id_verify(
    int unit,
    bcm_fabric_cgm_control_id_t control_id,
    dnx_fabric_cgm_control_access_info_t * access_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify control_id in case it composed out of 2 keys.
     */
    if (access_info->dbal_key1 != DBAL_FIELD_EMPTY && access_info->dbal_key2 != DBAL_FIELD_EMPTY)
    {
        switch (access_info->index_type)
        {
            /** Index is composed of 'pipe' key and 'level' key */
            case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL:
            {
                if (_SHR_FABRIC_TH_INDEX_IS_PIPE_LEVEL(control_id))
                {
                    int pipe = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_PIPE_GET(control_id);
                    int level = _SHR_FABRIC_TH_INDEX_PIPE_LEVEL_LEVEL_GET(control_id);

                    /** Validate pipe */
                    if (pipe != bcmFabricPipeAll)
                    {
                        uint32 min_pipe = 0;
                        uint32 max_pipe = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;

                        SHR_RANGE_VERIFY(pipe, min_pipe, max_pipe, _SHR_E_PARAM, "pipe %u is out of range.", pipe);
                    }

                    /** Validate level */
                    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_verify
                                    (unit, access_info->dbal_table, access_info->dbal_key2, level));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Control id %u for type %d must be constructed using BCM_FABRIC_TH_INDEX_PIPE_LEVEL_TH",
                                 control_id, access_info->control_type);
                }
                break;
            }
            /** Index is composed of 2 keys */
            case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SLOW_START_PHASE:
            case DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_PRIORITY:
            {
                int key1 = -1;
                int key2 = -1;

                /** Index is composed of 'fmc_shaper' key and 'slow_start_phase' key */
                if (_SHR_FABRIC_TH_INDEX_IS_FMC_SLOW_START_PHASE(control_id))
                {
                    key1 = _SHR_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_FMC_GET(control_id);
                    key2 = _SHR_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SLOW_START_PHASE_GET(control_id);
                }
                /** Index is composed of 'cast' key and 'priority' key */
                else if (_SHR_FABRIC_TH_INDEX_IS_CAST_PRIO(control_id))
                {
                    key1 = _SHR_FABRIC_TH_INDEX_CAST_PRIO_CAST_GET(control_id);
                    key2 = _SHR_FABRIC_TH_INDEX_CAST_PRIO_PRIO_GET(control_id);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Control id %u for type %d must be constructed using %s",
                                 control_id, access_info->control_type,
                                 _SHR_FABRIC_TH_INDEX_IS_CAST_PRIO(control_id) ? "BCM_FABRIC_TH_INDEX_CAST_PRIO_SET" :
                                 "BCM_FABRIC_TH_INDEX_FMC_SLOW_START_PHASE_SET");
                }

                /** Validate key1 */
                SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_verify
                                (unit, access_info->dbal_table, access_info->dbal_key1, key1));

                /** Validate key2 */
                SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_verify
                                (unit, access_info->dbal_table, access_info->dbal_key2, key2));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown index type %d", access_info->index_type);
            }
        }
    }
    /*
     * Verify control_id in case it composed out of 1 key.
     */
    else if (access_info->dbal_key1 != DBAL_FIELD_EMPTY)
    {
        if (access_info->index_type == DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE)
        {
            if (control_id != bcmFabricPipeAll)
            {
                uint32 max_pipe = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;
                SHR_MAX_VERIFY(control_id, max_pipe, _SHR_E_PARAM, "pipe %u is out of range.", control_id);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_key_verify
                            (unit, access_info->dbal_table, access_info->dbal_key1, control_id));
        }
    }
    /*
     * Verify control_id in case it doesn't have keys.
     */
    else
    {
        if (control_id != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "control_id must be -1 for control_type %d.", access_info->control_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Verify 'value' param of bcm_dnx_fabric_cgm_control_set is valid.
 * \param [in] control_type -
 *   The control type which the value refers to.
 * \param [in] value -
 *   The value to validate.
 * \param [in] access_info -
 *   DBAL info on the field user want to set its value.
 * \param [out] exact_value -
 *   Exact value in case value==-1.
 */
static shr_error_e
dnx_fabric_cgm_control_value_verify(
    int unit,
    int value,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 *exact_value)
{
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case of special configuration, value validation will be done
     * in the relevant special config function.
     */
    if (access_info->dbal_result == DBAL_FIELD_EMPTY)
    {
        *exact_value = value;
        SHR_EXIT();
    }

    sal_memset(&field_info, 0, sizeof(field_info));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, access_info->dbal_table, access_info->dbal_result, FALSE, 0, 0, &field_info));

    if (field_info.nof_instances > 1)
    {
        SHR_RANGE_VERIFY(value, 0, field_info.nof_instances - 1, _SHR_E_PARAM, "value %d is out of range.", value);
        *exact_value = value;
    }
    else if (value != -1)
    {
        SHR_RANGE_VERIFY(value, field_info.min_value, field_info.max_value, _SHR_E_PARAM,
                         "value %d is out of range.", value);
        *exact_value = value;
    }
    else
    {
        /*
         * In case value==-1 need to set value to be the maximum value the field can get
         */
        *exact_value = field_info.max_value;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Verify all params of bcm_dnx_fabric_cgm_control_set are valid.
 * \param [out] access_info -
 *   The DBAL info needed for dnx_fabric_cgm_control_hw_set.
 * \param [out] keys_info -
 *   Info about DBAL keys start & end indexes, needed for configuring
 *   the right keys entries.
 * \param [out] exact_value -
 *   Exact value in case value==-1.
 *   Relevant only for 'set' function.
 */
static shr_error_e
dnx_fabric_cgm_control_verify(
    int unit,
    uint32 flags,
    bcm_fabric_cgm_control_id_t control_id,
    int value,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 *exact_value)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify flags
     */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unrecognized flags");

    /*
     * Verify control_id.
     * Get keys info extracted from control_id.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_control_id_verify(unit, control_id, access_info));

    if (exact_value != NULL)
    {
        /*
         * Verify value
         */
        SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_value_verify(unit, value, access_info, exact_value));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get the full rate of the FMC shaper.
 */
static shr_error_e
dnx_fabric_cgm_control_slow_start_fmc_shaper_hw_get(
    int unit,
    bcm_fabric_mc_shaper_t fmc_shaper,
    uint32 *shaper_rate)
{
    uint32 entry_handle_id = 0;
    uint32 shaper_interval = 0;
    uint32 shaper_credit = 0;
    dbal_enum_value_field_ing_transmit_shaper_e ing_transmit_shaper = 0;
    uint32 interval_resolution = 2;
    uint32 credit_resolution = 1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert FMC_SHAPER to ING_TRANSMIT_SHAPER
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_fmc_shaper_to_ing_transmit_shaper(unit, fmc_shaper, &ing_transmit_shaper));

    /*
     * Get 'credit' and 'interval' from HW.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_FABRIC_CGM_ING_TRANSMIT_PER_CORE_SHAPERS_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ING_TRANSMIT_SHAPER, ing_transmit_shaper);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, &shaper_interval);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, &shaper_credit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Convert 'credit' and 'interval' to 'rate'.
     */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate
                    (unit, credit_resolution, interval_resolution, shaper_interval, shaper_credit, shaper_rate));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set Slow Start HW configurations for FMC shaper phase.
 *   FMC shapers has their fixed rate.
 *   Slow Start lets configure smaller rate, as a percentage of the full
 *   shaper's rate.
 *   This can be done in 2 phases. When shaper starts getting traffic it
 *   will have max rate according to rate_percent of the first phase. After
 *   predefined time it will move to the second phase which has higher
 *   rate_percent, and only after a predefined time will pass it will move
 *   to its full shaper rate.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmc_shaper -
 *   The FMC shaper to configure.
 * \param [in] phase -
 *   Which phase the rate_percent will apply.
 *   Slow start is done in 2 phases. Each phase has its own rate.
 * \param [in] rate_percent -
 *   The rate percentage from the full shaper's rate to be applied for
 *   these fmc_shaper and phase.
 */
static shr_error_e
dnx_fabric_cgm_control_slow_start_hw_set(
    int unit,
    bcm_fabric_mc_shaper_t fmc_shaper,
    uint32 phase,
    uint32 rate_percent)
{
    uint32 entry_handle_id = 0;
    uint32 shaper_interval = 0;
    uint32 shaper_credit = 0;
    uint32 full_rate = 0;
    uint32 slow_start_shaper_rate = 0;
    uint32 interval_resolution = 2;
    uint32 credit_resolution = 1;
    dnx_algo_rates_shaper_bounds_t shaper_bounds;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(rate_percent, 100, _SHR_E_PARAM, "rate_percent %u must be between 0-100.", rate_percent);

    /*
     * Get the full rate of the shaper
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_slow_start_fmc_shaper_hw_get(unit, fmc_shaper, &full_rate));

    /*
     * Set user's rate percent of this shaper
     */
    slow_start_shaper_rate = full_rate / 100 * rate_percent;

    /*
     * Convert shaper's rate to 'credit' and 'interval'
     */
    sal_memset(&shaper_bounds, 0, sizeof(shaper_bounds));
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_shaper_bounds_get(unit, DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG, &shaper_bounds));
    SHR_IF_ERR_EXIT(dnx_algo_rates_rate_to_credit_interval
                    (unit, slow_start_shaper_rate, &shaper_bounds, interval_resolution, credit_resolution,
                     &shaper_interval, &shaper_credit));

    /*
     * Set 'credit' and 'interval' into HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMC_SHAPER_ID, fmc_shaper);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHASE, phase);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, shaper_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, shaper_credit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_PERCENT, INST_SINGLE, rate_percent);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get Slow Start HW configurations of FMC shaper phase.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmc_shaper -
 *   The required FMC shaper.
 * \param [in] phase -
 *   The required phase.
 *   Slow start is done in 2 phases. Each phase has its own rate.
 * \param [out] rate_percent -
 *   The rate percentage from the full shaper's rate of these fmc_shaper and phase.
 */
static shr_error_e
dnx_fabric_cgm_control_slow_start_hw_get(
    int unit,
    bcm_fabric_mc_shaper_t fmc_shaper,
    uint32 phase,
    uint32 *rate_percent)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get rate_percent
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMC_SHAPER_ID, fmc_shaper);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHASE, phase);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_PERCENT, INST_SINGLE, rate_percent);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set egress drop tdm latency threshold.
 * \param [in] unit -
 *   The unit number.
 * \param [in] nsec -
 *   Threshold to configure in nano seconds.
 */
static shr_error_e
dnx_fabric_cgm_control_egress_drop_tdm_latency_th_hw_set(
    int unit,
    uint32 nsec)
{
    uint32 entry_handle_id = 0;
    uint32 clocks_resolution = dnx_data_fabric.cgm.egress_drop_th_clocks_resolution_get(unit);
    uint32 clocks = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert nsec to system clocks
     */
    time.time_units = DNXCMN_TIME_UNIT_NSEC;
    COMPILER_64_SET(time.time, 0, nsec);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_system_ref_clock_cycles_get(unit, &time, &clocks));

    /*
     * Set threshold in HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_TDM_LATENCY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE,
                                 UTILEX_DIV_ROUND_UP(clocks, clocks_resolution));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get egress drop tdm latency threshold.
 * \param [in] unit -
 *   The unit number.
 * \param [out] nsec -
 *   Configured threshold in nano seconds.
 */
static shr_error_e
dnx_fabric_cgm_control_egress_drop_tdm_latency_th_hw_get(
    int unit,
    uint32 *nsec)
{
    uint32 entry_handle_id = 0;
    uint32 clocks_resolution = dnx_data_fabric.cgm.egress_drop_th_clocks_resolution_get(unit);
    uint32 threshold = 0;
    uint32 clocks = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get threshold from HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_TDM_LATENCY, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, &threshold);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Convert system clocks to nsec
     */
    clocks = threshold * clocks_resolution;
    SHR_IF_ERR_EXIT(dnxcmn_system_ref_clock_cycles_to_time_get(unit, clocks, DNXCMN_TIME_UNIT_NSEC, &time));
    *nsec = COMPILER_64_LO(time.time);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the priority which will be considered as the maximum low priority
 *   for multicast low priority egress drop.
 * \param [in] unit -
 *   The unit number.
 * \param [in] max_low_priority -
 *   Priority which is the maximum priority consider as low priority.
 */
static shr_error_e
dnx_fabric_cgm_control_egress_drop_mc_low_prio_select_hw_set(
    int unit,
    uint32 max_low_priority)
{
    uint32 entry_handle_id = 0;
    uint32 nof_priorities = dnx_data_fabric.cell.nof_priorities_get(unit);
    uint32 priority = 0;
    uint32 is_low_priority = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(max_low_priority, nof_priorities - 1, _SHR_E_PARAM, "max_low_priority %u must be between 0-%u.",
                   max_low_priority, nof_priorities - 1);

    /*
     * Set threshold in HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC, &entry_handle_id));
    for (priority = 0; priority < nof_priorities; ++priority)
    {
        is_low_priority = (priority <= max_low_priority) ? 1 : 0;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOW_PRIORITY, priority, is_low_priority);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the priority which is considered as the maximum low priority
 *   for multicast low priority egress drop.
 * \param [in] unit -
 *   The unit number.
 * \param [out] max_low_priority -
 *   Priority which is the maximum priority consider as low priority.
 */
static shr_error_e
dnx_fabric_cgm_control_egress_drop_mc_low_prio_max_hw_get(
    int unit,
    uint32 *max_low_priority)
{
    uint32 entry_handle_id = 0;
    uint32 nof_priorities = dnx_data_fabric.cell.nof_priorities_get(unit);
    uint32 priority = 0;
    uint32 is_low_priority = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *max_low_priority = 0;

    /*
     * Set threshold in HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC, &entry_handle_id));
    for (priority = 0; priority < nof_priorities; ++priority)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC, entry_handle_id));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_LOW_PRIORITY, priority, &is_low_priority);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (is_low_priority)
        {
            *max_low_priority = priority;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set the rate for the fabric static shaper
 * \param [in] unit -
 *   The unit number.
 * \param [in] shaper_rate_gbps -
 *   The shaper rate in Gbps [Gbits/sec]
 */
static shr_error_e
dnx_fabric_cgm_control_static_shaper_rate_hw_set(
    int unit,
    uint32 shaper_rate_gbps)
{
    uint32 entry_handle_id = 0;
    uint32 shaper_interval = 0;
    uint32 shaper_credit = 0;
    uint32 interval_resolution = 1;
    uint32 credit_resolution = dnx_data_fabric.cgm.static_shaper_credit_resolution_bytes_get(unit);
    dnx_algo_rates_shaper_bounds_t shaper_bounds;
    uint32 shaper_rate_kbyte = 0;
    uint32 max_shaper_rate_gpbs = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get shaper rate bounds
     */
    sal_memset(&shaper_bounds, 0, sizeof(shaper_bounds));
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_shaper_bounds_get(unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, &shaper_bounds));

    /*
     * Validate user's rate is in shaper rate bounds.
     * rate_bps = core_clk * 256 * credit / interval
     */
    max_shaper_rate_gpbs =
        dnx_data_device.general.core_clock_khz_get(unit) * credit_resolution * SAL_CHAR_BIT / 1000000 *
        shaper_bounds.credit_max / shaper_bounds.interval_min;
    SHR_MAX_VERIFY(shaper_rate_gbps, max_shaper_rate_gpbs, _SHR_E_PARAM,
                   "rate %u can't be higher than max value %u [Gpbs]", shaper_rate_gbps, max_shaper_rate_gpbs);

    /*
     * Convert shaper rate from Gbps to KBps
     * KBps = Gbps * 1M / 8
     */
    shaper_rate_kbyte = ((shaper_rate_gbps * 1000) / SAL_CHAR_BIT) * 1000;

    /*
     * Convert shaper's rate to 'credit' and 'interval'
     */
    SHR_IF_ERR_EXIT(dnx_algo_rates_rate_to_credit_interval
                    (unit, shaper_rate_kbyte, &shaper_bounds, interval_resolution, credit_resolution, &shaper_interval,
                     &shaper_credit));

    /*
     * Set 'credit' and 'interval' into HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, &entry_handle_id));
    /** on HW, interval is 0 based */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, shaper_interval - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, shaper_credit);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_static_shaper_fix_factor))
    {
        uint32 found_shaper_rate_kbyte;
        uint32 found_shaper_rate_gpbs;

        /*
         * Calculate what is the rate we found:
         */
        SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate
                        (unit, credit_resolution, interval_resolution, shaper_interval, shaper_credit,
                         &found_shaper_rate_kbyte));

        /*
         * Convert shaper rate from KBps to Gbps
         * Gbps = KBps * 8 / 1M
         */
        found_shaper_rate_gpbs = ((found_shaper_rate_kbyte / 1000) * SAL_CHAR_BIT) / 1000;

        /** Fix the rate by a factor, if we didn't reached the required rate */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, entry_handle_id));
        if (found_shaper_rate_gpbs != shaper_rate_gbps)
        {
            int fix_factor_field_size;
            int fix_factor_signed;
            uint32 fix_factor_value;

            /*
             * total_fabric_rate = fabric_max_bandwidth * (1-fix_factor/1024)
             * => fix_factor = 1024 - 1024 * (total_fabric_rate / fabric_max_bandwidth)
             */
            fix_factor_signed = 1024 - UTILEX_DIV_ROUND_UP(1024 * shaper_rate_gbps, found_shaper_rate_gpbs);
            /** fix_factor_signed must be between -1024 and 1023 */
            fix_factor_signed = UTILEX_RANGE_BOUND(fix_factor_signed, -1024, 1023);

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, DBAL_FIELD_FIX_FACTOR_VALUE, FALSE, 0, 0,
                             &fix_factor_field_size));
            fix_factor_value = UTILEX_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(fix_factor_signed, fix_factor_field_size);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIX_FACTOR_ENABLE, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIX_FACTOR_VALUE, INST_SINGLE,
                                         fix_factor_value);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIX_FACTOR_ENABLE, INST_SINGLE, FALSE);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get the rate for the fabric static shaper
 * \param [in] unit -
 *   The unit number.
 * \param [out] shaper_rate_gbps -
 *   The shaper rate in Gbps [Gbits/sec]
 */
static shr_error_e
dnx_fabric_cgm_control_static_shaper_rate_hw_get(
    int unit,
    uint32 *shaper_rate_gbps)
{
    uint32 entry_handle_id = 0;
    uint32 shaper_interval = 0;
    uint32 shaper_credit = 0;
    uint32 interval_resolution = 1;
    uint32 credit_resolution = dnx_data_fabric.cgm.static_shaper_credit_resolution_bytes_get(unit);
    uint32 shaper_rate_kbyte = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get 'credit' and 'interval' from HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, &shaper_interval);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, &shaper_credit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Convert 'credit' and 'interval' to shaper's rate.
     * on HW, interval is 0 based.
     */
    SHR_IF_ERR_EXIT(dnx_algo_rates_credit_interval_to_rate
                    (unit, credit_resolution, interval_resolution, shaper_interval + 1, shaper_credit,
                     &shaper_rate_kbyte));

    /*
     * Convert shaper rate from KBps to Gbps
     * Gbps = KBps * 8 / 1M
     */
    *shaper_rate_gbps = ((shaper_rate_kbyte / 1000) * SAL_CHAR_BIT) / 1000;

    if (dnx_data_fabric.cgm.feature_get(unit, dnx_data_fabric_cgm_static_shaper_fix_factor))
    {
        uint32 fix_factor_enabled;
        uint32 fix_factor_value;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, entry_handle_id));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIX_FACTOR_ENABLE, INST_SINGLE,
                                   &fix_factor_enabled);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIX_FACTOR_VALUE, INST_SINGLE, &fix_factor_value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (fix_factor_enabled)
        {
            int fix_factor_field_size;
            int fix_factor_signed;

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER, DBAL_FIELD_FIX_FACTOR_VALUE, FALSE, 0, 0,
                             &fix_factor_field_size));
            fix_factor_signed = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(fix_factor_value, fix_factor_field_size);
            /*
             * total_fabric_rate = fabric_max_bandwidth * (1-fix_factor/1024)
             */
            *shaper_rate_gbps = *shaper_rate_gbps - UTILEX_DIV_ROUND_UP(*shaper_rate_gbps * fix_factor_signed, 1024);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Call special config functions according to control type
 */
static shr_error_e
dnx_fabric_cgm_control_special_config_set(
    int unit,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 key1,
    uint32 key2,
    uint32 exact_value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_info->control_type)
    {
        case bcmFabricCgmSlowStartRateShaperPhaseParam:
        {
            /** Slow Start set */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_slow_start_hw_set(unit, key1, key2, exact_value));

            break;
        }
        case bcmFabricCgmEgressDropTdmLatencyTh:
        {
            /** Egress drop TDM latency threshold set */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_egress_drop_tdm_latency_th_hw_set(unit, exact_value));
            break;
        }
        case bcmFabricCgmEgressDropMcLowPrioSelect:
        {
            /** Egress drop MC low priority maximum low priority set */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_egress_drop_mc_low_prio_select_hw_set(unit, exact_value));
            break;
        }
        case bcmFabricCgmLciShaperStaticRateInGbps:
        {
            /** Static Shaper rate set */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_static_shaper_rate_hw_set(unit, exact_value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown control_type %d", access_info->control_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure Fabric CGM various thresholds.
 */
int
bcm_dnx_fabric_cgm_control_set(
    int unit,
    uint32 flags,
    bcm_fabric_cgm_control_type_t control_type,
    bcm_fabric_cgm_control_id_t control_id,
    int value)
{
    uint32 key1 = 0;
    uint32 key2 = 0;
    dnx_fabric_cgm_control_access_info_t access_info;
    dnx_fabric_cgm_control_keys_info_t keys_info;
    uint32 exact_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    sal_memset(&access_info, 0, sizeof(access_info));
    sal_memset(&keys_info, 0, sizeof(keys_info));

    /*
     * Validate control_type.
     * Get DBAL info of control_type.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_access_info_get(unit, control_type, &access_info));

    /*
     * Validate input params.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_verify(unit, flags, control_id, value, &access_info, &exact_value));

    /*
     * Get keys info from control_id.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_keys_info_get(unit, control_id, &access_info, &keys_info));

    /*
     * Configure the threshold in HW.
     * Do it for all required keys.
     */
    for (key1 = keys_info.key1.start_index; key1 <= keys_info.key1.end_index; ++key1)
    {
        for (key2 = keys_info.key2.start_index; key2 <= keys_info.key2.end_index; ++key2)
        {
            /**
             * Special configuration HW set.
             * Used for control types that their value doesn't maps
             * directly to a field.
             */
            if (access_info.dbal_result == DBAL_FIELD_EMPTY)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_special_config_set(unit, &access_info, key1, key2, exact_value));
            }
            /** Common HW set */
            else
            {
                SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_hw_set(unit, &access_info, key1, key2, exact_value));
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Call special config functions according to control type
 */
static shr_error_e
dnx_fabric_cgm_control_special_config_get(
    int unit,
    dnx_fabric_cgm_control_access_info_t * access_info,
    uint32 key1_start_index,
    uint32 key2_start_index,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (access_info->control_type)
    {
        case bcmFabricCgmSlowStartRateShaperPhaseParam:
        {
            /** Slow Start get */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_slow_start_hw_get
                            (unit, key1_start_index, key2_start_index, (uint32 *) value));
            break;
        }
        case bcmFabricCgmEgressDropTdmLatencyTh:
        {
            /** Egress drop TDM latency threshold get */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_egress_drop_tdm_latency_th_hw_get(unit, (uint32 *) value));
            break;
        }
        case bcmFabricCgmEgressDropMcLowPrioSelect:
        {
            /** Egress drop MC low priority maximum low priority get */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_egress_drop_mc_low_prio_max_hw_get(unit, (uint32 *) value));
            break;
        }
        case bcmFabricCgmLciShaperStaticRateInGbps:
        {
            /** Static Shaper rate get */
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_static_shaper_rate_hw_get(unit, (uint32 *) value));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown control_type %d", access_info->control_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get configuration of Fabric CGM various thresholds.
 */
int
bcm_dnx_fabric_cgm_control_get(
    int unit,
    uint32 flags,
    bcm_fabric_cgm_control_type_t control_type,
    bcm_fabric_cgm_control_id_t control_id,
    int *value)
{
    dnx_fabric_cgm_control_access_info_t access_info;
    dnx_fabric_cgm_control_keys_info_t keys_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    sal_memset(&access_info, 0, sizeof(access_info));
    sal_memset(&keys_info, 0, sizeof(keys_info));

    /*
     * Validate control_type.
     * Get DBAL info of control_type.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_access_info_get(unit, control_type, &access_info));

    /*
     * Validate input params.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_verify(unit, flags, control_id, 0, &access_info, NULL));

    /*
     * Get keys info from control_id.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_keys_info_get(unit, control_id, &access_info, &keys_info));

    /**
     * Get the threshold from HW.
     *
     * Special configuration HW get.
     * Used for control types that their value doesn't maps
     * directly to a field.
     */
    if (access_info.dbal_result == DBAL_FIELD_EMPTY)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_special_config_get
                        (unit, &access_info, keys_info.key1.start_index, keys_info.key2.start_index, (uint32 *) value));
    }
    /**
     * Common HW get
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_cgm_control_hw_get
                        (unit, &access_info, keys_info.key1.start_index, keys_info.key2.start_index, (uint32 *) value));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_cgm_llfc_enable_hw_set(
    int unit,
    bcm_port_t link,
    int enable)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_LLFC_ENABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_cgm_llfc_enable_hw_get(
    int unit,
    bcm_port_t link,
    int *enable)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CGM_LLFC_ENABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK, link);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*************
 * }         *
 */
