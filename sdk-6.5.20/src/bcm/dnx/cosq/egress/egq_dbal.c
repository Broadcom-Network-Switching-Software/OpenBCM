/**
* \file egq_dbal.c
*
* This file contains wrapper functions
* for accessing the EGQ DBAL tables.
* 
* Tables are found in files:
*   hl_tm_egq_definition.xml
*/

/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_DBALDNX
/*
 * Includes related to DBAL
 * {
 */
/*
 * }
 */
/*
 * Includes - General
 * {
 */
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/egress/cosq_egq.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>

/*
 * }
 */
/*
 * Includes related to DATA
 * {
 */
/*
 * }
 */
/**
* static functions:
* {
*/

/*
 * }
 */

/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_tcg_cir_sp_en_set(
    int unit,
    bcm_gport_t gport,
    uint32 enable)
{
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    core = gport ? BCM_COSQ_GPORT_CORE_GET(gport) : DBAL_CORE_ALL;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CIR_SP_EN, INST_SINGLE, enable,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_tcg_cir_sp_en_get(
    int unit,
    bcm_gport_t gport,
    uint32 *enable_p)
{
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    core = gport ? BCM_COSQ_GPORT_CORE_GET(gport) : DBAL_CORE_ALL;
    SHR_IF_ERR_EXIT
        (dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                          1, 1,
                          GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                          GEN_DBAL_FIELD32, DBAL_FIELD_CIR_SP_EN, INST_SINGLE, enable_p, GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_emr_fifo_default_set(
    int unit,
    int core,
    uint32 tdm_exist)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_MULTICAST_FIFOS, 1, 12,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     /*
                                      * get EMR fifo depth parameters and configure the HW
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_FIFO_FULL, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->full),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_UNICAST_FIFO_FULL, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->full),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_HIGH_FIFO_FULL, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH, tdm_exist)->full),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_LOW_FIFO_FULL, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW, tdm_exist)->full),
                                     /*
                                      * get EMR fifo almost full threshold parameters and configure the HW
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_FIFO_ALMOST_FULL, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->almost_full),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_UNICAST_FIFO_DISCARD_TH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->almost_full),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_HIGH_FIFO_DISCARD_TH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH,
                                                                   tdm_exist)->almost_full), GEN_DBAL_FIELD32,
                                     DBAL_FIELD_MC_LOW_FIFO_DISCARD_TH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW,
                                                                   tdm_exist)->almost_full),
                                     /*
                                      * get EMR fifo full threshold parameters and configure the HW
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_FIFO_DEPTH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->depth),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_UNICAST_FIFO_DEPTH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->depth),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_HIGH_FIFO_DEPTH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH, tdm_exist)->depth),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_LOW_FIFO_DEPTH, INST_SINGLE,
                                     (uint32) (dnx_data_egr_queuing.
                                               params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW, tdm_exist)->depth),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_cmic_egq_map_set(
    int unit,
    bcm_core_t core,
    int cpu_cos,
    int qpair)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Flow control mapping CMIC to EGQ
     */
    SHR_IF_ERR_EXIT
        (dnx_dbal_gen_set(unit, DBAL_TABLE_FC_RX_CPU_MAP,
                          2, 1,
                          GEN_DBAL_FIELD32, DBAL_FIELD_CPU_CHAN, cpu_cos,
                          GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                          GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, INST_SINGLE, qpair, GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in egr_queuing.h
 */
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_shaping_enable_set(
    int unit,
    uint8 enable)
{
    uint32 fld_val;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    fld_val = enable;
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                         1, 3,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, (uint32) (fld_val),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, (uint32) (fld_val),
                                         GEN_DBAL_FIELD32, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, (uint32) (fld_val),
                                         GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_shaping_enable_get(
    int unit,
    uint8 *enable_p)
{
    uint32 fld_val;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    fld_val = 0;
    core = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, (uint32 *) (&fld_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
    *enable_p = fld_val;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given raw value of counter plus some information regarding it,
 *   get value of counter (and overflow information) depending on
 *   input size plus overflow and gtimer status. 
 * \param [in] unit -
 *   Unit HW id
 * \param [in] is_gtimer_enabled -
 *   gitmer status. 
 * \param [in,out] counter_info -
 *   As input:
 *     Counter information to process. Use 
 *       counter_info->has_ovf
 *         Indicates whether counter has overflow mecanism.
 *       counter_info->counter_size
 *         Indicates number of bits on raw input counter value
 *   As output:
 *     Calculated counter information:
 *       counter_info->count_ovf
 *         Indicates whether counter is in 'overflow' state
 *       counter_info->count_val
 *         Indicates deduced value of counter
 * \return 
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 */
static shr_error_e
dnx_egq_dbal_counter_info_process(
    int unit,
    int is_gtimer_enabled,
    dnx_egq_counter_info_t * counter_info)
{
    uint64 counter_mask;
    uint32 counter_overfloaw_bit;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(counter_info, _SHR_E_PARAM, "counter_info");
    COMPILER_64_ZERO(counter_mask);
    if (counter_info->has_ovf)
    {
        /*
         * Enter if counter has an overflow mechanism.
         */
        /*
         * Set 'counter_overfloaw_bit' as per the MS bit of the counter.
         */
        if (COMPILER_64_BITTEST(counter_info->count_val, (counter_info->counter_size - 1)))
        {
            counter_overfloaw_bit = 1;
        }
        else
        {
            counter_overfloaw_bit = 0;
        }
        /*
         * If 'gtimer' is NOT active then the overflow bit (the MS bit of the counter)
         * is meaningful.
         */
        counter_info->count_ovf = (is_gtimer_enabled ? 0 : counter_overfloaw_bit);
        /*
         * Create a mask with all bits set up to bit (size - 1), not including that bit.
         */
        COMPILER_64_MASK_CREATE(counter_mask, (counter_info->counter_size - 1), 0);
    }
    else
    {
        /*
         * If counter does not have an overflow mechanism, all its bits are meaningful.
         */
        counter_info->count_ovf = 0;
        /*
         * Create a mask with all bits set up to bit 'size', not including that bit.
         */
        COMPILER_64_MASK_CREATE(counter_mask, counter_info->counter_size, 0);
    }
    COMPILER_64_AND(counter_info->count_val, counter_mask);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_rqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter)
{
    int counter_idx;
    int is_gtimer_enabled;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    counter_idx = 0;
    is_gtimer_enabled = 0;
    /*
     * Taking a handle to DBAL table
     */
    table_id = DBAL_TABLE_EGQ_RQP_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RQP_COUNTER, &entry_handle_id));
    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    field_id = DBAL_FIELD_EMR_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx],
                             entry_handle_id,
                             table_id, field_id, "EMR_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_EMR_MC_HIGH_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_MC_HIGH_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_EMR_MC_LOW_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_MC_LOW_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_EMR_DISCARDS_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_DISCARDS_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        field_id = DBAL_FIELD_EMR_TDM_PACKET_COUNTER;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EMR_TDM_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    if ((dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_non_j2b0_counters_are_valid)) &&
        (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist)))
    {
        field_id = DBAL_FIELD_CDC_FABRIC_PACKET_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "CDC_FABRIC_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    if ((dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_non_j2b0_counters_are_valid)) &&
        (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist)))
    {
        field_id = DBAL_FIELD_CDC_IPT_PACKET_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "CDC_IPT_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    field_id = DBAL_FIELD_PRP_PACKET_IN_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_IN_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_PRP_PACKET_GOOD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_GOOD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_PRP_PACKET_GOOD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_GOOD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        field_id = DBAL_FIELD_PRP_PACKET_GOOD_TDM_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "PRP_PACKET_GOOD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    field_id = DBAL_FIELD_PRP_PACKET_DISCARD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_DISCARD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_PRP_PACKET_DISCARD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_DISCARD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        field_id = DBAL_FIELD_PRP_PACKET_DISCARD_TDM_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "PRP_PACKET_DISCARD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    field_id = DBAL_FIELD_PRP_SOP_DISCARD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_SOP_DISCARD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_PRP_SOP_DISCARD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_SOP_DISCARD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        field_id = DBAL_FIELD_PRP_SOP_DISCARD_TDM_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "PRP_SOP_DISCARD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) == TRUE)
    {
        field_id = DBAL_FIELD_PRP_TDM_BYPASS_PACKET_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "PRP_TDM_BYPASS_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
        counter_idx++;
    }
    *nof_counter = counter_idx;
    /*
     * Performing the action: Retrieve from HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_RQP, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_egq_dbal_counter_info_process(unit, is_gtimer_enabled, &counter_info[counter_idx]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_pqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info)
{
    int counter_idx;
    int nof_counter;
    int is_gtimer_enabled;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    counter_idx = 0;
    is_gtimer_enabled = 0;
    /** Taking a handle to DBAL table */
    table_id = DBAL_TABLE_EGQ_PQP_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_COUNTER, &entry_handle_id));
    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Get count value*/
    field_id = DBAL_FIELD_ENQ_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx],
                             entry_handle_id,
                             table_id, field_id, "PQP_ENQ_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_ENQ_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_ENQ_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_MULTICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_MULTICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_DISCARD_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_DISCARD_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_DISCARD_MULTICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_DISCARD_MULTICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_UNICAST_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_UNICAST_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;
    field_id = DBAL_FIELD_DEQ_MULTICAST_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_MULTICAST_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    nof_counter = counter_idx;
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_PQP, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < nof_counter; counter_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_egq_dbal_counter_info_process(unit, is_gtimer_enabled, &counter_info[counter_idx]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_pqp_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_COUNTER, &entry_handle_id));
   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Disable the filters*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_epni_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, &entry_handle_id));
   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** clear the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Disable the filters*/
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_Q_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_IF_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIRR_PRIO_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIRR_ADD_ORIG_HEADER_MASK, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_epni_counter_info_get(
    int unit,
    int core,
    int flag,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter)
{
    int counter_idx;
    int counter_type;
    int is_gtimer_enabled;
    int suffix_len;
    int name_len;
    char *name_suffix = NULL;
    uint32 is_count_bytes;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Taking a handle to DBAL table */
    table_id = DBAL_TABLE_EGQ_EPNI_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, &entry_handle_id));
    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Get the setting of byte or packet */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_BYTES, INST_SINGLE, &is_count_bytes);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    if (is_count_bytes)
    {
        counter_type = DNX_EGQ_COUTNER_TYPE_BYTE;
        name_suffix = "_BYTE";
    }
    else
    {
        counter_type = DNX_EGQ_COUTNER_TYPE_PACKET;
        name_suffix = "_PACKET";
    }
    /*
     * Load length of suffix right after definition of suffix text, for clarity.
     */
    suffix_len = sal_strnlen(name_suffix, DNX_EGQ_COUNTER_NAME_LEN);

    counter_idx = 0;
    is_gtimer_enabled = 0;
    if (flag & DNX_EGQ_EPNI_QUEUE_COUNTER)
    {
        field_id = DBAL_FIELD_Q_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_Q_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_EPNI_IF_COUNTER)
    {
        field_id = DBAL_FIELD_IFC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_IFC_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_EPNI_MIRROR_COUNTER)
    {
        field_id = DBAL_FIELD_MIRR_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_MIRR_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_NON_PROGRAMMABLE_COUNTER)
    {
        field_id = DBAL_FIELD_DISC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_DISC_CNT", counter_type, TRUE);
        counter_idx++;
        field_id = DBAL_FIELD_FRWD_DISC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_FRWD_DISC_CNT", counter_type, TRUE);
        counter_idx++;
    }

    *nof_counter = counter_idx;
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_EPNI, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_egq_dbal_counter_info_process(unit, is_gtimer_enabled, &counter_info[counter_idx]));
        /*
         * Add suffix text to original name
         */
        name_len = sal_strnlen(counter_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN);
        sal_strncpy_s(&(counter_info[counter_idx].name[name_len]), name_suffix, suffix_len + 1);
        if (sal_strnlen(counter_info[counter_idx].name, DNX_EGQ_COUNTER_NAME_LEN) >= (DNX_EGQ_COUNTER_NAME_LEN - 1))
        {
            /*
             * Do not allow text to overflow original space assigned for 'name'
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Size of 'name' on 'dnx_egq_counter_info_t' is larger"
                         " than %d (array size minus ending null). Quit.", (DNX_EGQ_COUNTER_NAME_LEN - 1));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_weights_tbl_get(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(egq_dwm_tbl_data, 0x0, sizeof(*egq_dwm_tbl_data));
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, 2, 2,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) entry_offset,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT, INST_SINGLE,
                                     (uint32 *) (&(egq_dwm_tbl_data->mc_or_mc_low_queue_weight)), GEN_DBAL_FIELD32,
                                     DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT, INST_SINGLE,
                                     (uint32 *) (&(egq_dwm_tbl_data->uc_or_uc_low_queue_weight)),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_weights_tbl_set(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, 2, 2,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) entry_offset,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT, INST_SINGLE,
                                     (uint32) (egq_dwm_tbl_data->mc_or_mc_low_queue_weight),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT, INST_SINGLE,
                                     (uint32) (egq_dwm_tbl_data->uc_or_uc_low_queue_weight), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
int
dnx_egq_dbal_tc_dp_map_tbl_get(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_TC_DP_MAP, 5, 2,
    /** 5 keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SYS_TC, (uint32) (entry->tc),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SYS_DP, (uint32) (entry->dp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_IS_EGRESS_MC, (uint32) (entry->is_egr_mc),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MAP_PROFILE, (uint32) (entry->map_profile),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core_id,
    /** 2 fields */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGRESS_TC, INST_SINGLE, (&(tbl_data->tc)),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CGM_MC_DP, INST_SINGLE, (&(tbl_data->dp)),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
int
dnx_egq_dbal_tc_dp_map_tbl_set(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_TC_DP_MAP, 5, 2,
    /** 5 keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SYS_TC, (uint32) (entry->tc),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_SYS_DP, (uint32) (entry->dp),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_IS_EGRESS_MC, (uint32) (entry->is_egr_mc),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_MAP_PROFILE, (uint32) (entry->map_profile),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core_id,
    /** 2 fields */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EGRESS_TC, INST_SINGLE, (uint32) (tbl_data->tc),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CGM_MC_DP, INST_SINGLE, (uint32) (tbl_data->dp),
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
int
dnx_egq_dbal_cos_map_profile_get(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGRESS_PP_PORT, 2, 1,
    /** 2 keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PP_PORT, (uint32) (internal_port),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core_id,
    /** 1 field  */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_COS_MAP_PROFILE, INST_SINGLE,
                                     (&(egq_ppct_tbl_data->cos_map_profile)), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
int
dnx_egq_dbal_cos_map_profile_set(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGRESS_PP_PORT, 2, 1,
    /** 2 keys */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_PP_PORT, (uint32) (internal_port),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core_id,
    /** 1 field  */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_COS_MAP_PROFILE, INST_SINGLE,
                                     (uint32) (egq_ppct_tbl_data->cos_map_profile), GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_empty_port_max_credit_set(
    int unit,
    int arg)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 2,
                                         /*
                                          * key construction.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         /*
                                          * Setting STOP_COLLECTING field. If set to '0' then no limit is applied to
                                          * collection of credits on OTM port shapers.
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_PORT_STOP_COLLECTING_EN, INST_SINGLE,
                                         (uint32) (arg ? 1 : 0),
                                         /*
                                          * Setting shapers
                                          */
                                         GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_PORT_MAX_CREDIT, INST_SINGLE,
                                         (uint32) (arg), GEN_DBAL_FIELD_LAST_MARK));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_empty_port_max_credit_get(
    int unit,
    int *arg_p)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Getting shapers
     */
    core = 0;
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION, 1, 1,
                                     /*
                                      * key construction.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                     /*
                                      * Getting field.
                                      */
                                     GEN_DBAL_FIELD32, DBAL_FIELD_EMPTY_PORT_MAX_CREDIT, INST_SINGLE, arg_p,
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}
/**
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_gport_scheduler_queue_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int core;
    /**
     *  Strict Priority, if mapped to channelized
     *  Interface. Otherwise - ignored by the HW.
     */
    uint32 nif_prio_fld_val;
    dnx_port_t logical_port;
    int nof_q_pairs, base_q_pair;
    dnx_egr_q_prio_e prio;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_cosq_priority_convert(unit, cosq, &prio));
    SHR_IF_ERR_EXIT(dnx_cosq_egq_port_priority_validate(unit, gport, prio));
    /*
     * Deduce 'priority' from input 'mode' 
     */
    switch (mode)
    {
        case BCM_COSQ_SP0:
        {
            nif_prio_fld_val = 0x1;
            break;
        }
        case BCM_COSQ_SP1:
        {
            nif_prio_fld_val = 0x0;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "unit %d, invalid mode %d. Range is SP0-SP1 (%d-%d)", unit, mode, BCM_COSQ_SP0, BCM_COSQ_SP1);
        }
    }
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_q_pairs));
    /*
     * Set the OFP nif-priority. Each bit correspond to a OFP
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, 2, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + prio),
                                     GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, (uint32) core,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_QP_IS_HP, INST_SINGLE, (uint32) (nif_prio_fld_val),
                                     GEN_DBAL_FIELD_LAST_MARK));
exit:
    SHR_FUNC_EXIT;
}

/** see h file */
shr_error_e
dnx_egq_dbal_fqp_profile_params_set(
    int unit,
    int core,
    int profile_id,
    dnx_fqp_profile_params_t * profile_params)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_FQP_PROFILE_ATTRIBUTES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_id);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_MAX_BYTES_TH, INST_SINGLE,
                                     profile_params->txq_max_bytes);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_IRDY_TH, INST_SINGLE,
                                     profile_params->irdy_thr);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_GAP, INST_SINGLE, profile_params->min_gap);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALLOW_CONS_SEL, INST_SINGLE,
                                     profile_params->serve_consecutive_if);
        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist) == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_TDM_IRDY_SEL, INST_SINGLE,
                                         profile_params->txq_tdm_irdy);
        }
        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_sub_calendar) == TRUE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_PART_OF_SUB_CALENDAR, INST_SINGLE,
                                         profile_params->is_belong_to_sub_calendar);
        }
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See egq_dbal.h
 */
shr_error_e
dnx_egq_dbal_fqp_profile_id_set(
    int unit,
    int core,
    int egr_if,
    int profile_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_PROFILE_ID, INST_SINGLE, profile_id);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /** see h file */
shr_error_e
dnx_egq_dbal_fqp_profile_id_get(
    int unit,
    int core,
    int egr_if,
    int *profile_id)
{
    uint32 entry_handle_id;
    uint32 profile_idx;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FQP_PROFILE_ID, INST_SINGLE, &profile_idx);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *profile_id = profile_idx;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /** see h file */
shr_error_e
dnx_egq_dbal_fqp_profile_params_get(
    int unit,
    int core,
    int egr_if,
    int profile_id,
    dnx_fqp_profile_params_t * profile_params)
{
    uint32 entry_handle_id;
    uint32 txq_max_bytes = 0, irdy_thr = 0, txq_tdm_irdy = 0, is_belong_to_sub_calendar = 0;
    uint32 fqp_min_gap = 0, serve_consecutive_if = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        /** get the profile params */
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_FQP_PROFILE_ATTRIBUTES, &entry_handle_id));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_INDEX, profile_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TXQ_MAX_BYTES_TH, INST_SINGLE, &txq_max_bytes);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TXI_IRDY_TH, INST_SINGLE, &irdy_thr);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_GAP, INST_SINGLE, &fqp_min_gap);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALLOW_CONS_SEL, INST_SINGLE,
                                   &serve_consecutive_if);
        if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_txq_tdm_irdy_sel_exist) == 1)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TXQ_TDM_IRDY_SEL, INST_SINGLE, &txq_tdm_irdy);
        }

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IFC_PART_OF_SUB_CALENDAR, INST_SINGLE,
                                   &is_belong_to_sub_calendar);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        profile_params->txq_max_bytes = txq_max_bytes;
        profile_params->irdy_thr = irdy_thr;
        profile_params->txq_tdm_irdy = txq_tdm_irdy;
        profile_params->min_gap = fqp_min_gap;
        profile_params->serve_consecutive_if = serve_consecutive_if;
        profile_params->is_belong_to_sub_calendar = is_belong_to_sub_calendar;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /** see h file */
shr_error_e
dnx_egq_dbal_pqp_almost_empty_thresholds_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_ALMOST_EMPTY, &entry_handle_id));
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_ALMOST_EMPTY_TH, INST_SINGLE,
                                 dnx_data_egr_queuing.params.pqp_queue_almost_empty_th_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ALMOST_EMPTY_TH, INST_SINGLE,
                                 dnx_data_egr_queuing.params.pqp_port_almost_empty_th_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_ALMOST_EMPTY_TH, INST_SINGLE,
                                 dnx_data_egr_queuing.params.pqp_ifc_almost_empty_th_get(unit));
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
