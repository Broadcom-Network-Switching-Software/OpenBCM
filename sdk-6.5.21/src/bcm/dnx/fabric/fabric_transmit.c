/** \file fabric_transmit.c
 *
 * Functions for handling Fabric transmit adapter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

/*************
 * INCLUDES  *
 */
/* { */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/compiler.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_transmit.h>
/* } */

/*************
 * DEFINES   *
 */
/* { */

/** Default packet size for WFQ */
#define DNX_FABRIC_WFQ_DFLT_PKT_SIZE   (12*1024)

#define DNX_FABRIC_DTQ_SHAPERS_FC_NOF  (6)

#define DNX_FABRIC_RRF_OCB_ONLY_DEPTH  (128)
#define DNX_FABRIC_RRF_SRAM_DEPTH      (4*1024 - DNX_FABRIC_RRF_OCB_ONLY_DEPTH)
#define DNX_FABRIC_RRF_MIXS_DEPTH      (4*1024)
#define DNX_FABRIC_RRF_IRDY            (13)
/** RRF thresholds */
#define DNX_FABRIC_RRF_SRAM_HP_FC_HIGH_TH   (DNX_FABRIC_RRF_SRAM_DEPTH - 46 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_SRAM_HP_FC_LOW_TH    (DNX_FABRIC_RRF_SRAM_DEPTH - 100 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_SRAM_LP_FC_HIGH_TH   (DNX_FABRIC_RRF_SRAM_DEPTH - 66 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_SRAM_LP_FC_LOW_TH    (DNX_FABRIC_RRF_SRAM_DEPTH - 100 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_MIXS_HP_FC_HIGH_TH   (DNX_FABRIC_RRF_MIXS_DEPTH - 46 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_MIXS_HP_FC_LOW_TH    (DNX_FABRIC_RRF_MIXS_DEPTH - 100 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_MIXS_LP_FC_HIGH_TH   (DNX_FABRIC_RRF_MIXS_DEPTH - 66 - DNX_FABRIC_RRF_IRDY)
#define DNX_FABRIC_RRF_MIXS_LP_FC_LOW_TH    (DNX_FABRIC_RRF_MIXS_DEPTH - 100 - DNX_FABRIC_RRF_IRDY)

/* } */

typedef struct
{
    int sram;
    int mixs;
    int mixd;
    int mix_mixs;
    int mix_mixd;
    int s2d;
} dnx_fabric_transmit_pdq_fc_thresholds_t;

/*************
 * FUNCTIONS *
 */
/* { */

static shr_error_e
dnx_fabric_dtq_fmc_hp_fc_enable_mask_get(
    int unit,
    uint32 *fabric_0_fc_enable,
    uint32 *fabric_1_fc_enable,
    uint32 *fabric_2_fc_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        *fabric_2_fc_enable = TRUE;
    }
    else
    {
        switch (dnx_data_fabric.dtqs.nof_active_dtqs_get(unit))
        {
            case 3:
            {
                *fabric_1_fc_enable = TRUE;
                break;
            }
            case 2:
            {
                *fabric_1_fc_enable = TRUE;
                break;
            }
            case 1:
            {
                *fabric_0_fc_enable = TRUE;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of active DTQs is not supported");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_dtq_fmc_lp_fc_enable_mask_get(
    int unit,
    uint32 *fabric_0_fc_enable,
    uint32 *fabric_1_fc_enable,
    uint32 *fabric_2_fc_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        /*
         * In case of Mesh mode, the DEV3 PDQ traffic
         * flows towards Fabric2 DTQ
         */
        *fabric_2_fc_enable = TRUE;
    }
    else
    {
        switch (dnx_data_fabric.dtqs.nof_active_dtqs_get(unit))
        {
            case 3:
            {
                *fabric_2_fc_enable = TRUE;
                break;
            }
            case 2:
            {
                *fabric_1_fc_enable = TRUE;
                break;
            }
            case 1:
            {
                *fabric_0_fc_enable = TRUE;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of active DTQs is not supported");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_fabric_transmit_pdqs_correspondence_to_dtqs_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_context_e context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * For each PDQ, configure which DTQ-FC will cause it to stop.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_DTQ_FC_ENABLE, &entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        dbal_enum_value_field_fabric_dtq_fc_e dtq_fc;
        /*
         * First set to ignore all Flow Controls, and then enable the
         * appropriate Flow Control.
         */
        uint32 local_0_fc_enable = 0;
        uint32 local_1_fc_enable = 0;
        uint32 fabric_0_fc_enable = 0;
        uint32 fabric_1_fc_enable = 0;
        uint32 fabric_2_fc_enable = 0;

        switch (context)
        {
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_HP:
            {
                /** Local0 PDQ traffic flows towards Local0 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP;
                local_0_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_LP:
            {
                /** Local0 PDQ traffic flows towards Local0 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP;
                local_0_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP:
            {
                if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
                {
                    continue;
                }

                /** Local1 PDQ traffic flows towards Local1 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP;
                local_1_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP:
            {
                if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
                {
                    continue;
                }

                /** Local1 PDQ traffic flows towards Local1 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP;
                local_1_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                /** FUC/DEV1 PDQ traffic flows towards Local0 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP;
                fabric_0_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_LP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                /** FUC/DEV1 PDQ traffic flows towards Local0 DTQ */
                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP;
                fabric_0_fc_enable = TRUE;
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP;

                /** Relevant only in Mesh mode */
                if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
                {
                    /** DEV2 PDQ traffic flows towards Local1 DTQ */
                    fabric_1_fc_enable = TRUE;
                }
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP;

                /** Relevant only in Mesh mode */
                if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
                {
                    /** DEV2 PDQ traffic flows towards Local1 DTQ */
                    fabric_1_fc_enable = TRUE;
                }
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_HP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP;
                SHR_IF_ERR_EXIT(dnx_fabric_dtq_fmc_hp_fc_enable_mask_get
                                (unit, &fabric_0_fc_enable, &fabric_1_fc_enable, &fabric_2_fc_enable));
                break;
            }
            case DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP:
            {
                if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
                {
                    continue;
                }

                dtq_fc = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP;
                SHR_IF_ERR_EXIT(dnx_fabric_dtq_fmc_lp_fc_enable_mask_get
                                (unit, &fabric_0_fc_enable, &fabric_1_fc_enable, &fabric_2_fc_enable));
                break;
            }
                /*
                 * Coverity explanation: we really can't reach 'default', since all
                 * cases in the enum are covered, but the instructions are that
                 * switch-case statements should always have 'default' case.
                 */
                 /* coverity[dead_error_begin:FALSE]  */
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown context %d", context);
            }
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_FC, dtq_fc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_0_DTQ_FC_ENABLE, INST_SINGLE,
                                     local_0_fc_enable);
        if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_1_DTQ_FC_ENABLE, INST_SINGLE,
                                         local_1_fc_enable);
        }
        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_0_DTQ_FC_ENABLE, INST_SINGLE,
                                         fabric_0_fc_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_1_DTQ_FC_ENABLE, INST_SINGLE,
                                         fabric_1_fc_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_2_DTQ_FC_ENABLE, INST_SINGLE,
                                         fabric_2_fc_enable);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set sram and mix pdqs size
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_size_sram_mix_set(
    int unit,
    uint32 pdq_size)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_context_e context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_CONFIG, &entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP))
        {
            continue;
        }

        /** Fabric contexts are not relevant for device without fabric */
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
            (context >= DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP))
        {
            continue;
        }

        /** Configure PDQ size */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_SIZE, INST_SINGLE, pdq_size);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIX_SIZE, INST_SINGLE, pdq_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set ocb pdq size
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_size_ocb_set(
    int unit,
    uint32 pdq_size)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_ocb_context_e ocb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_OCB_CONFIG, &entry_handle_id));
    for (ocb_context = 0; ocb_context < DBAL_NOF_ENUM_FABRIC_PDQ_OCB_CONTEXT_VALUES; ++ocb_context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
            && (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1))
        {
            continue;
        }

        /** Fabric contexts are not relevant for device without fabric */
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
            (ocb_context >= DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1))
        {
            continue;
        }

        /** Configure PDQ size */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_OCB_CONTEXT, ocb_context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_OCB_SIZE, INST_SINGLE, pdq_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set dram delete pdq size
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_size_dram_delete_set(
    int unit,
    uint32 pdq_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure DRAM delete PDQ size
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_DRAM_DELETE_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_DRAM_DELETE_SIZE, INST_SINGLE, pdq_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set s2d pdq size
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_size_s2d_set(
    int unit,
    uint32 s2d_pdq_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure SRAM to DRAM PDQ size.
     * 2 elements - HP & LP.
     * Different memory than the other configurable PDQs.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_SRAM_TO_DRAM_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_TO_DRAM_SIZE, INST_SINGLE, s2d_pdq_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set PDQs size
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_size_init(
    int unit,
    uint32 pdq_size,
    uint32 s2d_pdq_size)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure SRAM + MIX PDQs size
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_size_sram_mix_set(unit, pdq_size));

    /*
     * Configure OCB PDQs size
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_size_ocb_set(unit, pdq_size));

    /*
     * Configure DRAM delete PDQ size
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_size_dram_delete_set(unit, pdq_size));

    /*
     * Configure SRAM to DRAM PDQ size
     * Different memory than the other configurable PDQs.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_size_s2d_set(unit, s2d_pdq_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure the thresholds for PDQs to DQCQs flow control.
 *   The thresholds are predefined by this function, so we have
 *   a fixed number of threshold profiles. Later, each FC will
 *   select which index of these thresholds it want to use.
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
static shr_error_e
dnx_fabric_transmit_pdqs_to_dqcq_fc_th_set(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e key_id,
    dbal_fields_e result_id,
    int pdq_size)
{
    uint32 entry_handle_id;
    uint32 index = 0;
    uint32 max_index = 0;
    uint32 threshold = 0;
    uint32 thresholds_gap = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get maximum index */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_id, key_id, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_index));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    thresholds_gap = pdq_size / (max_index + 1);
    for (index = 0; index <= max_index; ++index)
    {
        threshold = thresholds_gap * index;
        dbal_entry_key_field32_set(unit, entry_handle_id, key_id, index);
        dbal_entry_value_field32_set(unit, entry_handle_id, result_id, INST_SINGLE, threshold);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select PDQs to DQCQs FC thresholds index
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
static shr_error_e
dnx_fabric_transmit_pdq_to_dqcq_fc_th_select(
    int unit,
    int pdq_priority,
    int dqcq_priority,
    int min_low_dqcq_priority,
    dnx_fabric_transmit_pdq_fc_thresholds_t * fc_thresh)
{

    int is_dqcq_low;
    const dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *fc_th_select = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear structure */
    sal_memset(fc_thresh, 0, sizeof(dnx_fabric_transmit_pdq_fc_thresholds_t));

    /*
     * in PDQ:
     * 0 - High Priority
     * 1 - Low Priority
     */
    is_dqcq_low = (dqcq_priority < min_low_dqcq_priority ? FALSE : TRUE);
    if (is_dqcq_low == pdq_priority)
    {
        /** both high or both low */
        fc_th_select = dnx_data_fabric.transmit.pdq_to_dqcq_fc_th_select_get(unit, dqcq_priority);
        fc_thresh->sram = fc_th_select->sram;
        fc_thresh->mixs = fc_th_select->mixs;
        fc_thresh->mixd = fc_th_select->mixd;
        fc_thresh->mix_mixs = fc_th_select->mix_mixs;
        fc_thresh->mix_mixd = fc_th_select->mix_mixd;
        fc_thresh->s2d = fc_th_select->s2d;
    }
    else
    {
        fc_thresh->sram = 0;
        fc_thresh->mixs = 0;
        fc_thresh->mixd = 0;
        fc_thresh->mix_mixs = 0;
        fc_thresh->mix_mixd = 0;
        fc_thresh->s2d = 0;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select PDQs to DQCQs FC thresholds index
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
static shr_error_e
dnx_fabric_transmit_pdqs_to_dqcq_fc_th_select_all(
    int unit,
    int min_low_dqcq_priority)
{
    uint32 entry_handle_id;
    uint32 pdq_priority = 0;
    uint32 dqcq_priority = 0;
    dbal_enum_value_field_fabric_pdq_context_e context;

    dnx_fabric_transmit_pdq_fc_thresholds_t fc_thresh;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_TO_DQCQ_FC_SELECT, &entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP))
        {
            continue;
        }

        /** Fabric contexts are not relevant for device without fabric */
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
            (context >= DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP))
        {
            continue;
        }

        /*
         * 0 - High Priority
         * 1 - Low Priority
         */
        pdq_priority = context % 2;

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        for (dqcq_priority = 0; dqcq_priority < dnx_data_iqs.dqcq.nof_priorities_get(unit); ++dqcq_priority)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_to_dqcq_fc_th_select(unit, pdq_priority, dqcq_priority,
                                                                         min_low_dqcq_priority, &fc_thresh));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, dqcq_priority);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.sram);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXS_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.mixs);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXD_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.mixd);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIX_MIXS_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.mix_mixs);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIX_MIXD_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.mix_mixd);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Select PDQs to DQCQs Sram to Dram FC thresholds index
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
static shr_error_e
dnx_fabric_transmit_pdqs_to_dqcq_sram_to_dram_fc_th_select_all(
    int unit,
    int min_low_dqcq_priority)
{
    uint32 entry_handle_id;
    uint32 pdq_priority = 0;
    uint32 dqcq_priority = 0;
    dbal_enum_value_field_fabric_pdq_s2d_context_e s2d_context;

    dnx_fabric_transmit_pdq_fc_thresholds_t fc_thresh;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (s2d_context = 0; s2d_context < DBAL_NOF_ENUM_FABRIC_PDQ_S2D_CONTEXT_VALUES; ++s2d_context)
    {
        /*
         * 0 - High Priority
         * 1 - Low Priority
         */
        pdq_priority = s2d_context % 2;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_TO_DQCQ_SRAM_TO_DRAM_FC_SELECT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_S2D_CONTEXT, s2d_context);
        for (dqcq_priority = 0; dqcq_priority < dnx_data_iqs.dqcq.nof_priorities_get(unit); ++dqcq_priority)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_to_dqcq_fc_th_select(unit, pdq_priority, dqcq_priority,
                                                                         min_low_dqcq_priority, &fc_thresh));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, dqcq_priority);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_TO_DRAM_FC_TH_SELECT, INST_SINGLE,
                                         fc_thresh.s2d);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure PDQs to DQCQs FC thresholds using the 'select' meachanism.
 *   Each FC sets to one of predefined FC thresholds.
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
static shr_error_e
dnx_fabric_transmit_pdqs_to_dqcq_fc_th_init(
    int unit,
    uint32 pdq_size,
    uint32 s2d_pdq_size)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set PDQs to DQCQs FC thresholds
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_th_set
                    (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_TO_DQCQ_FC_TH, DBAL_FIELD_INDEX, DBAL_FIELD_THRESHOLD,
                     pdq_size));

    /*
     * Select PDQs to DQCQs FC thresholds index
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_th_select_all(unit, 0));

    /*
     * Set PDQs to DQCQs SRAM to DRAM FC thresholds
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_th_set
                    (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_TO_DQCQ_SRAM_TO_DRAM_FC_TH, DBAL_FIELD_INDEX,
                     DBAL_FIELD_THRESHOLD, s2d_pdq_size));

    /*
     * Select PDQs to DQCQs SRAM to DRAM FC thresholds index
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_sram_to_dram_fc_th_select_all(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure PDQs to DQCQs FC thresholds using the 'select' mechanism.
 *   Each FC sets to one of predefined FC thresholds.
 * \param [in] unit -
 *   The unit number.
 * \param [in] min_low_dqcq_priority -
 *   Maximal low priority in DQCQ
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
shr_error_e
dnx_fabric_transmit_dqcq_priority_update(
    int unit,
    int min_low_dqcq_priority)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Select PDQs to DQCQs FC thresholds index
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_th_select_all(unit, min_low_dqcq_priority));

    /*
     * Select PDQs to DQCQs SRAM to DRAM FC thresholds index
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_sram_to_dram_fc_th_select_all(unit, min_low_dqcq_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure direct PDQs to DQCQs FC thresholds
 * \remark
 *   Shouldn't be used when 8 priorities dqcq is supported
 */
static shr_error_e
dnx_fabric_transmit_pdqs_to_dqcq_fc_direct_th_init(
    int unit,
    uint32 s2d_pdq_size)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_context_e context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure SRAM + MIX PDQs FC towards the IPS.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_FC_CONFIG, &entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP))
        {
            continue;
        }

        /** Configure Flow Control thresholds toward the IPS */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_sram_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXS_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mixs_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXD_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mixd_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIX_MIXS_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mix_mixs_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIX_MIXD_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mix_mixd_fc_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Configure SRAM to DRAM queue.
     * Configure Flow Control thresholds toward the IPS.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_SRAM_TO_DRAM_FC_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_TO_DRAM_FC_TH, INST_SINGLE,
                                 dnx_data_fabric.transmit.pdq_s2d_fc_th_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure PDQs EIR FC leaky bucket mechanism
 * \remark
 *   Can be used only when EIR FC leaky bucket is supported
 */
static shr_error_e
dnx_fabric_transmit_pdq_eir_fc_leaky_bucket_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_context_e context;
    const dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *eir_fc =
        dnx_data_fabric.transmit.eir_fc_leaky_bucket_get(unit);
    const dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *eir_fc_delta_select =
        dnx_data_fabric.transmit.eir_fc_leaky_bucket_delta_select_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_EIR_FC_LEAKY_BUCKET_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_FULL_THRESHOLD, INST_SINGLE,
                                 eir_fc->almost_full_th);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_EMPTY_THRESHOLD, INST_SINGLE,
                                 eir_fc->almost_empty_th);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_SIZE, INST_SINGLE, eir_fc->bucket_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUCKET_FC_THRESHOLD, INST_SINGLE,
                                 eir_fc->bucket_fc_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PERIOD, INST_SINGLE, eir_fc->period);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOKENS_DELTA0, INST_SINGLE, eir_fc->tokens_delta0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOKENS_DELTA1, INST_SINGLE, eir_fc->tokens_delta1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOKENS_DELTA2, INST_SINGLE, eir_fc->tokens_delta2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOKENS_DELTA3, INST_SINGLE, eir_fc->tokens_delta3);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                    (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_EIR_FC_LEAKY_BUCKET_THRESHOLDS, entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP))
        {
            continue;
        }

        /** Fabric contexts are not relevant for device without fabric */
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
            (context >= DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP))
        {
            continue;
        }

        /** different almost full/empty thresholds that are being configured per pdq context */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_FULL_THRESHOLD, INST_SINGLE,
                                     eir_fc->almost_full_th);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALMOST_EMPTY_THRESHOLD, INST_SINGLE,
                                     eir_fc->almost_empty_th);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                    (unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_EIR_FC_LEAKY_BUCKET_DELTA_SELECT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_IN_BETWEEN, INST_SINGLE,
                                 eir_fc_delta_select->fifo_in_between);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_PARTIALLY_ALMOST_FULL, INST_SINGLE,
                                 eir_fc_delta_select->fifo_partially_almost_full);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_ALMOST_FULL, INST_SINGLE,
                                 eir_fc_delta_select->fifo_almost_full);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_PARTIALLY_ALMOST_EMPTY, INST_SINGLE,
                                 eir_fc_delta_select->fifo_partially_almost_empty);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_NOT_STABLE, INST_SINGLE,
                                 eir_fc_delta_select->fifo_not_stable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_ALMOST_EMPTY, INST_SINGLE,
                                 eir_fc_delta_select->fifo_almost_empty);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure SRAM + MIX PDQs EIR FC thresholds
 * \remark
 *   Shouldn't be used when EIR FC leacky bucket is supported
 */
static shr_error_e
dnx_fabric_transmit_pdq_eir_fc_set(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_context_e context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_EIR_FC_CONFIG, &entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP
             || context == DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP))
        {
            continue;
        }

        /** Configure EIR Flow Control thresholds */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_EIR_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_sram_eir_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXS_EIR_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mixs_eir_fc_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_MIXD_EIR_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_mixd_eir_fc_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure OCB PDQs EIR FC thresholds
 * \remark
 *   Shouldn't be used when EIR FC leacky bucket is supported
 */
static shr_error_e
dnx_fabric_transmit_pdq_eir_fc_ocb_set(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_ocb_context_e ocb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_OCB_EIR_FC_CONFIG, &entry_handle_id));
    for (ocb_context = 0; ocb_context < DBAL_NOF_ENUM_FABRIC_PDQ_OCB_CONTEXT_VALUES; ++ocb_context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
            && (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1))
        {
            continue;
        }

        /** Configure OCB EIR Flow Control thresholds */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_OCB_CONTEXT, ocb_context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_OCB_EIR_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_ocb_eir_fc_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure PDQs EIR FC thresholds
 * \remark
 *   Shouldn't be used when EIR FC leacky bucket is supported
 */
static shr_error_e
dnx_fabric_transmit_pdq_eir_fc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set EIR FC for SRAM + MIX PDQs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_eir_fc_set(unit));

    /*
     * Set EIR FC for OCB PDQs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_eir_fc_ocb_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure OCB PDQ FC thresholds
 */
static shr_error_e
dnx_fabric_transmit_pdq_ocb_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pdq_ocb_context_e ocb_context;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_OCB_FC_CONFIG, &entry_handle_id));
    for (ocb_context = 0; ocb_context < DBAL_NOF_ENUM_FABRIC_PDQ_OCB_CONTEXT_VALUES; ++ocb_context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if ((dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
            && (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core) &&
            (ocb_context == DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1))
        {
            continue;
        }

        /** Local1 contexts are not relevant for single core device */
        if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
            (ocb_context >= DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1))
        {
            continue;
        }

        /** Configure Flow Control thresholds toward the IPS */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_OCB_CONTEXT, ocb_context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_OCB_FC_TH, INST_SINGLE,
                                     dnx_data_fabric.transmit.pdq_ocb_fc_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure DRAM delete FC thresholds
 */
static shr_error_e
dnx_fabric_transmit_pdq_dram_delete_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure DRAM delete PDQ flow control thresholds towards the IPS
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_DRAM_DELETE_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_DRAM_DELETE_FC_TH, INST_SINGLE,
                                 dnx_data_fabric.transmit.pdq_dram_delete_fc_th_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure SRAM delete FC thresholds
 */
static shr_error_e
dnx_fabric_transmit_pdq_sram_delete_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure SRAM delete PDQ flow control thresholds towards the IPS
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_SRAM_DELETE_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PDQ_SRAM_DELETE_FC_TH, INST_SINGLE,
                                 dnx_data_fabric.transmit.pdq_sram_delete_fc_th_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   PDQ configurations.
 */
static shr_error_e
dnx_fabric_transmit_pdqs_contexts_init(
    int unit)
{
    uint32 pdq_size = 0;
    uint32 s2d_pdq_size = dnx_data_fabric.transmit.s2d_pdq_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        /** The '-1' is to make sure we don't overflow the memory */
        pdq_size = dnx_data_fabric.transmit.pdqs_total_size_get(unit) / dnx_data_fabric.transmit.nof_pdqs_get(unit) - 1;
    }
    else
    {
        pdq_size =
            dnx_data_fabric.transmit.pdqs_total_size_get(unit) / (dnx_data_fabric.transmit.nof_pdqs_get(unit) - 2);
    }

    /*
     * Configure PDQs size
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_size_init(unit, pdq_size, s2d_pdq_size));

    /*
     * Configure PDQs to DQCQs FC thresholds
     */
    if (dnx_data_iqs.dqcq.feature_get(unit, dnx_data_iqs_dqcq_8_priorities))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_th_init(unit, pdq_size, s2d_pdq_size));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_to_dqcq_fc_direct_th_init(unit, s2d_pdq_size));
    }

    /*
     * Configure EIR FC thresholds
     */
    if (dnx_data_fabric.transmit.feature_get(unit, dnx_data_fabric_transmit_eir_fc_leaky_backet))
    {
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_eir_fc_leaky_bucket_init(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_eir_fc_init(unit));
    }

    /*
     * Configure OCB PDQ FC thresholds
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_ocb_fc_init(unit));

    /*
     * Configure Dram delete PDQ FC thresholds
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_dram_delete_fc_init(unit));

    /*
     * Configure Sram delete PDQ FC thresholds
     */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_sram_delete_fc_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   DTQ-FC thresholds configurations.
 */
static shr_error_e
dnx_fabric_transmit_dtq_thresholds_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_dtq_fc_e dtq_fc;
    uint32 dtq_fc_fabric_thresholds[] = { 100, 80, 60, 40 };
    dbal_enum_value_field_fabric_dtq_ocb_fc_e dtq_ocb_fc;
    uint32 dtq_ocb_fc_thresholds[] = { 40, 20 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configurations for Local DTQ FC Thresholds
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_DTQ_FC_THRESHOLDS, &entry_handle_id));
    for (dtq_fc = 0; dtq_fc < DBAL_NOF_ENUM_FABRIC_DTQ_FC_VALUES; ++dtq_fc)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOCAL, 1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_FC, dtq_fc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE,
                                     dnx_data_fabric.transmit.dtq_fc_local_thresholds_get(unit, dtq_fc)->threshold);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Configurations for Fabric DTQ FC Thresholds
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_DTQ_FC_THRESHOLDS, entry_handle_id));
        for (dtq_fc = 0; dtq_fc < DBAL_NOF_ENUM_FABRIC_DTQ_FC_VALUES; ++dtq_fc)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOCAL, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_FC, dtq_fc);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE,
                                         dtq_fc_fabric_thresholds[dtq_fc]);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_DTQ_OCB_FC_THRESHOLDS, entry_handle_id));
        for (dtq_ocb_fc = 0; dtq_ocb_fc < DBAL_NOF_ENUM_FABRIC_DTQ_OCB_FC_VALUES; ++dtq_ocb_fc)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_OCB_FC, dtq_ocb_fc);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE,
                                         dtq_ocb_fc_thresholds[dtq_ocb_fc]);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set PDQs WFQs configurations.
 */
static shr_error_e
dnx_fabric_transmit_pdq_wfq_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 wfq_index = 0;
    uint32 max_wfq = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_WFQ_WEIGHT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT_FIRST, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT_SECOND, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set default packet size for CLOS WFQ
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_WFQ_CONFIG, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get
                    (unit, entry_handle_id, DBAL_FIELD_WFQ_INDEX, 1, DBAL_PREDEF_VAL_MAX_VALUE, &max_wfq));
    for (wfq_index = 0; wfq_index <= max_wfq; ++wfq_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_WFQ_INDEX, wfq_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PACKET_SIZE, INST_SINGLE,
                                     DNX_FABRIC_WFQ_DFLT_PKT_SIZE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /*
         * Set default packet size for Mesh WFQ
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_WFQ_MESH_CONFIG, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PACKET_SIZE, INST_SINGLE,
                                     DNX_FABRIC_WFQ_DFLT_PKT_SIZE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure which DTQ-FC signal will stop the read from the Read Fifos (RDF).
 */
static shr_error_e
dnx_fabric_transmit_rdf_correspondence_to_dtqs_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_dtq_fc_e dtq_fc;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The Read Fifos FC is by default DBAL_ENUM_FVAL_FABRIC_DTQ_FC_RDF
     * for all DTQs.
     * In device configurations where there are less DTQs, need to disable the
     * FC in the unused DTQs.
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
    {
        switch (dnx_data_fabric.dtqs.nof_active_dtqs_get(unit))
        {
            case 3:
            {
                /*
                 * All DTQs are active. No need to disable any FC.
                 */
                break;
            }
            case 2:
            {
                /*
                 * Disable FC from unused Fabric2 DTQ.
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_FABRIC_TRANSMIT_RDF_DTQ_FC_ENABLE, &entry_handle_id));
                for (dtq_fc = 0; dtq_fc < DBAL_NOF_ENUM_FABRIC_DTQ_FC_VALUES; ++dtq_fc)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_FC, dtq_fc);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_2_DTQ_FC_ENABLE, INST_SINGLE,
                                                 0);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }

                break;
            }
            case 1:
            {
                /*
                 * Disable FC from unused Fabric1 & Fabric2 DTQs.
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                (unit, DBAL_TABLE_FABRIC_TRANSMIT_RDF_DTQ_FC_ENABLE, &entry_handle_id));
                for (dtq_fc = 0; dtq_fc < DBAL_NOF_ENUM_FABRIC_DTQ_FC_VALUES; ++dtq_fc)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_FC, dtq_fc);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_1_DTQ_FC_ENABLE, INST_SINGLE,
                                                 0);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_2_DTQ_FC_ENABLE, INST_SINGLE,
                                                 0);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }

                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure PDQs shapers.
 */
static shr_error_e
dnx_fabric_transmit_pdq_shapers_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 compensation = 0;
    uint32 is_pcp_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Shapers general configurations
     */
    /** Calculate cell header compensation */
    is_pcp_enabled = dnx_data_fabric.cell.pcp_enable_get(unit);
    switch (dnx_data_fabric.pipes.nof_pipes_get(unit))
    {
        case 1:
        {
            compensation = is_pcp_enabled ? 11 : 10;
            break;
        }
        case 2:
        {
            compensation = is_pcp_enabled ? 13 : 12;
            break;
        }
        case 3:
        {
            compensation = is_pcp_enabled ? 16 : 15;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric number of pipes is not supported");
            break;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_SHAPERS_CONFIG, &entry_handle_id));
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_SHAPERS_NOF_ACTIVE_LINKS_ENABLE,
                                     INST_SINGLE, 1);
    }
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ZERO_ACTIVE_LINKS_RATE_ENABLE, INST_SINGLE, 1);
        if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_ACTIVE_LINKS_MC_SHPER_ENABLE,
                                         INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_FMC_NUM_LINKS_MAX_ENABLE, INST_SINGLE,
                                         1);
        }
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_COMPENSATION, INST_SINGLE, compensation);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure PDQs global shapers.
 */
static shr_error_e
dnx_fabric_transmit_pdq_global_shapers_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_dtq_global_shaper_e dtq_global_shaper;
    dbal_enum_value_field_fabric_pdq_context_e context;
    uint32 shaper_delay = 9;
    uint32 shaper_max_burst = 80000;
    uint32 shaper_cal = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    shaper_cal = (dnx_data_device.general.bus_size_in_bits_get(unit) * 105 * (shaper_delay + 1)) / (8 * 100);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_GLOBAL_SHAPER_CONFIG, &entry_handle_id));
    for (dtq_global_shaper = 0; dtq_global_shaper < DBAL_NOF_ENUM_FABRIC_DTQ_GLOBAL_SHAPER_VALUES; ++dtq_global_shaper)
    {
        /*
         * Config only local shapers.
         * Fabric shapers should be disabled (default).
         */
        if (dtq_global_shaper == DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0
            || dtq_global_shaper == DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_DTQ_GLOBAL_SHAPER, dtq_global_shaper);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR, INST_SINGLE, shaper_cal);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELAY, INST_SINGLE, shaper_delay);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, shaper_max_burst);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_IS_SEGMENTED, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_BUNDLE_ENABLE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * The following field is disabled, but we put it here in case we want to activate
             * this feature in the future:
             * soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_FC_ENABLEf, 0);
             */
        }
    }

    /*
     * Enable PDQs Global Shaper FC.
     * Enable bit per context.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_PDQ_GLOBAL_SHAPER_FC_ENABLE, entry_handle_id));
    for (context = 0; context < DBAL_NOF_ENUM_FABRIC_PDQ_CONTEXT_VALUES; ++context)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PDQ_CONTEXT, context);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure Read Request FIFOs (RRF).
 */
static shr_error_e
dnx_fabric_transmit_rrf_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set Flow Control hysteresis thresholds from RRF to PDQs.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_RRF_TO_PDQ_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_HP_FC_HIGH_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_SRAM_HP_FC_HIGH_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_HP_FC_LOW_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_SRAM_HP_FC_LOW_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_LP_FC_HIGH_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_SRAM_LP_FC_HIGH_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_LP_FC_LOW_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_SRAM_LP_FC_LOW_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OCBO_FC_HIGH_TH, INST_SINGLE, 70);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OCBO_FC_LOW_TH, INST_SINGLE, 70);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXS_HP_FC_HIGH_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_MIXS_HP_FC_HIGH_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXS_HP_FC_LOW_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_MIXS_HP_FC_LOW_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXS_LP_FC_HIGH_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_MIXS_LP_FC_HIGH_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXS_LP_FC_LOW_TH, INST_SINGLE,
                                 DNX_FABRIC_RRF_MIXS_LP_FC_LOW_TH);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXD_HP_FC_HIGH_TH, INST_SINGLE, 90);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXD_HP_FC_LOW_TH, INST_SINGLE, 70);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXD_LP_FC_HIGH_TH, INST_SINGLE, 80);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIXD_LP_FC_LOW_TH, INST_SINGLE, 60);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_fabric_dtqs_contexts_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 mixed_dtq_size = 0;
    uint32 nof_active_ocb_dtqs = 0;
    uint32 nof_active_mixed_dtqs = 0;
    uint32 nof_active_dtqs = dnx_data_fabric.dtqs.nof_active_dtqs_get(unit);
    uint32 nof_active_subdtqs = nof_active_dtqs * 2;
    uint32 subdtq = 0;
    uint32 dtq_start_addr = 0;
    uint32 max_nof_subdtqs = dnx_data_fabric.dtqs.max_nof_subdtqs_get(unit);
    uint32 dtqs_max_size = dnx_data_fabric.dtqs.max_size_get(unit);
    uint32 dtqs_ocb_only_size = dnx_data_fabric.dtqs.ocb_only_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_active_ocb_dtqs = nof_active_dtqs;
    nof_active_mixed_dtqs = nof_active_dtqs;
    mixed_dtq_size = (dtqs_max_size - (nof_active_ocb_dtqs * dtqs_ocb_only_size)) / nof_active_mixed_dtqs;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_DTQ_CONFIG, &entry_handle_id));
    dtq_start_addr = 0;
    for (subdtq = 0; subdtq < max_nof_subdtqs; ++subdtq)
    {
        uint32 enable = (subdtq < nof_active_subdtqs) ? 1 : 0;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SUBDTQ_ID, subdtq);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_ADDRESS, INST_SINGLE,
                                     enable ? dtq_start_addr : 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        if (enable)
        {
            /** The Sub-DTQs are ordered by MIXED/OCB alternatively, where the first Sub-DTQ is MIXED */
            dtq_start_addr += (subdtq % 2) ? dtqs_ocb_only_size : mixed_dtq_size;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_TRANSMIT_DTQ_CONFIG_END, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_DTQ_END_ADDRESS, INST_SINGLE,
                                 dtqs_max_size - 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init configurations regarding IPT flow control debug
 */
static shr_error_e
dnx_fabric_transmit_flow_control_debug_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_FLOW_CONTROL_DEBUG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_STICKY, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init GCI configurations
 */
static shr_error_e
dnx_fabric_transmit_gci_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Enable GCI and count all incoming GCI indications 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TRANSMIT_GCI_ENABLE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_SELECT, INST_SINGLE, 0x4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_transmit_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /** Enable GCI */
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_gci_init(unit));
    }

    /** Configure DTQs FC thresholds */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_dtq_thresholds_init(unit));

    /** Configure PDQs contexts */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_contexts_init(unit));

    /** Set which DTQ-FC affect each PDQ */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdqs_correspondence_to_dtqs_fc_init(unit));

    /** Set which DTQ-FC affect RDFs */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_rdf_correspondence_to_dtqs_fc_init(unit));

    /** Configure PDQs shapers */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_shapers_init(unit));

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /** Configure PDQs global shapers */
        SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_global_shapers_init(unit));
    }

    /** Configure PDQs WFQs */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_pdq_wfq_init(unit));

    /** Configure RRF */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_rrf_init(unit));

    /** Configure flow control debug settings */
    SHR_IF_ERR_EXIT(dnx_fabric_transmit_flow_control_debug_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE
