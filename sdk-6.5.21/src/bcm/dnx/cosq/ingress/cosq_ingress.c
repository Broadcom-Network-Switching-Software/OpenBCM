/** \file cosq_ingress.c
 * 
 *
 * General COSQ Ingress functionality for DNX. \n
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
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_cosq_ingress_access.h>
#include "ingress_congestion.h"
#include "compensation.h"
#include "cosq_ingress.h"
#include "system_red.h"
/*
 * See .h file
 */
shr_error_e
dnx_cosq_ingress_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init swstate db
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.init(unit));

    /*
     * Init Ingress congestion submodule
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_init(unit));

    /*
     * Init Ingress compensation submodule
     */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_compensation_init(unit));

    /*
     * System RED init
     */
    SHR_IF_ERR_EXIT(dnx_system_red_init(unit));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ingress_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    /*
     * Init Ingress congestion submodule
     */
    SHR_IF_ERR_CONT(dnx_ingress_congestion_deinit(unit));

    /*
     * Init Ingress compensation submodule
     */
    SHR_IF_ERR_CONT(dnx_cosq_ingress_compensation_deinit(unit));

    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ingress_port_add_handle(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** nothing to do */

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_cosq_ingress_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be removed */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    /** Do Ingress Congestion part */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_port_remove(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /** for mirror, this is done by port_to_rcy_port_unmap_set */
    if (!DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info))
    {
        /** unmark the port to have unchangable priority */
        SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_set(unit, port, 0));
    }

exit:
    SHR_FUNC_EXIT;

}

/** See .h file */
shr_error_e
dnx_cosq_ingress_dram_bound_traffic_to_dram_restore(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_VOQ_DRAM_QSIZE_TH, INST_SINGLE, 0x10);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_WORDS_QSIZE_TH, INST_SINGLE, 0x18);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_BUFFERS_QSIZE_TH, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_PDS_QSIZE_TH, INST_SINGLE, 0x1);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_BUFFERS_TH, INST_SINGLE, 0x100);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_PDBS_TH, INST_SINGLE, 0x100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_cosq_ingress_dram_bound_traffic_to_dram_stop(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_CGM_DRAM_BOUND_STATE_TH, &entry_handle_id));
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id,
                                               DBAL_FIELD_DRAM_BOUND_VOQ_DRAM_QSIZE_TH, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_WORDS_QSIZE_TH,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_BUFFERS_QSIZE_TH,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_VOQ_SRAM_PDS_QSIZE_TH,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_BUFFERS_TH, INST_SINGLE,
                                 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_TOTAL_FREE_SRAM_PDBS_TH, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_RECOVERY_ELIGIBLE_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get the IRE counter information
 */
shr_error_e
dnx_ingress_ire_counter_info_get(
    int unit,
    int core,
    dnx_ingress_ire_counter_info_t * counter_info)
{
    int counter_size;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get NIF packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_NIF_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->nif_pkt_cnt);

    /** Get RCYH packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_RCYH_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->rcyh_pkt_cnt);

    /** Get RCYL packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_RCYL_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->rcyl_pkt_cnt);

    /** Get CPU packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_CPU_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->cpu_pkt_cnt);

    /** Get OLP packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_OLP_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->olp_pkt_cnt);

    /** Get OAMP packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_OAMP_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->oamp_pkt_cnt);

    /** Get register interface packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->reg_if_pkt_cnt);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the gtimer status of block IRE */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_IRE, core, &is_gtimer_enabled));

    /** Get the overflow status and real counter value for NIF packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_NIF_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->nif_pkt_ovf as per the MS bit of counter_info->nif_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->nif_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->nif_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->nif_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->nif_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for RCYH packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_RCYH_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->rcyh_pkt_ovf as per the MS bit of counter_info->rcyh_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->rcyh_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->rcyh_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->rcyh_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->rcyh_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for SRAM to RCYL packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_RCYL_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->rcyl_pkt_ovf as per the MS bit of counter_info->rcyl_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->rcyl_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->rcyl_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->rcyl_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->rcyl_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for CPU packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_CPU_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->cpu_pkt_ovf as per the MS bit of counter_info->cpu_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->cpu_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->cpu_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->rcyl_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->cpu_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for OLP packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_OLP_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->olp_pkt_ovf as per the MS bit of counter_info->olp_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->olp_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->olp_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->olp_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->olp_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for OAMP packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_OAMP_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->oamp_pkt_ovf as per the MS bit of counter_info->oamp_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->oamp_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->oamp_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->oamp_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->oamp_pkt_cnt, work_reg_64);

    /** Get the overflow status and real counter value for register interface packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGR_REASSEMBLY_IRE_COUNTER,
                               DBAL_FIELD_REGISTER_INTERFACE_PACKET_COUNTER, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->reg_if_pkt_ovf as per the MS bit of counter_info->reg_if_pkt_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->reg_if_pkt_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->reg_if_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->reg_if_pkt_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->reg_if_pkt_cnt, work_reg_64);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get the SQM counter information
 */
shr_error_e
dnx_ingress_sqm_counter_info_get(
    int unit,
    int core,
    dnx_ingress_sqm_counter_info_t * counter_info)
{
    int counter_size;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_SQM_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get en-queue packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PKT_ENQ_CTR, INST_SINGLE, &counter_info->pkt_enq_cnt);

    /** Get de-queue packet count */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_PKT_DEQ_CTR, INST_SINGLE, &counter_info->pkt_deq_cnt);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the gtimer status of block IRE */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_SQM, core, &is_gtimer_enabled));

    /** Get the overflow status and real counter value for en-queue packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_SQM_COUNTER, DBAL_FIELD_PKT_ENQ_CTR, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->enq_pkt_ovf as per the MS bit of counter_info->pkt_enq_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->pkt_enq_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->enq_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->pkt_enq_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->pkt_enq_cnt, work_reg_64);

    /** Get the overflow status and real counter value for de-queue packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_SQM_COUNTER, DBAL_FIELD_PKT_DEQ_CTR, 0, 0, 0, &counter_size);
    /*
     * Set counter_info->deq_pkt_ovf as per the MS bit of counter_info->pkt_deq_cnt
     */
    COMPILER_64_COPY(work_reg_64, counter_info->pkt_deq_cnt);
    COMPILER_64_SHR(work_reg_64, (counter_size - 1));
    counter_info->deq_pkt_ovf = is_gtimer_enabled ? 0 : (COMPILER_64_LO(work_reg_64) & 0x1);
    /*
     * Calculate: ((1 << (counter_size - 1)) - 1) and load into work_reg_64
     * Then 'and' counter_info->pkt_deq_cnt with work_reg_64 
     */
    COMPILER_64_SET(work_reg_64, 0, 1);
    COMPILER_64_SHL(work_reg_64, (counter_size - 1));
    COMPILER_64_SUB_32(work_reg_64, 1);
    COMPILER_64_AND(counter_info->pkt_deq_cnt, work_reg_64);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get the IPT counter information
 */
shr_error_e
dnx_ingress_ipt_counter_info_get(
    int unit,
    int core,
    dnx_ingress_ipt_counter_info_t * counter_info)
{
    int counter_size;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IPT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get en-queue packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CGM_S_2D_CMD_PACKET_COUNTER, INST_SINGLE,
                               &counter_info->pkt_cgm_cnt);

    /** Get de-queue packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SPB_PACKET_COUNT, INST_SINGLE,
                               &counter_info->pkt_spb_cnt);

    /** Get de-queue packet count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PACKET_DISCARD, INST_SINGLE,
                               &counter_info->pkt_itpp_disc_cnt);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the gtimer status of block IRE */
    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_IPT, core, &is_gtimer_enabled));

    /** Get the overflow status and real counter value for en-queue packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_IPT_COUNTER,
                               DBAL_FIELD_CGM_S_2D_CMD_PACKET_COUNTER, 0, 0, 0, &counter_size);
    counter_info->cgm_pkt_ovf = is_gtimer_enabled ? 0 : ((counter_info->pkt_cgm_cnt >> (counter_size - 1)) & 0x1);
    counter_info->pkt_cgm_cnt = counter_info->pkt_cgm_cnt & ((1 << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for de-queue packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_IPT_COUNTER,
                               DBAL_FIELD_SPB_PACKET_COUNT, 0, 0, 0, &counter_size);
    counter_info->spb_pkt_ovf = is_gtimer_enabled ? 0 : ((counter_info->pkt_spb_cnt >> (counter_size - 1)) & 0x1);
    counter_info->pkt_spb_cnt = counter_info->pkt_spb_cnt & ((1 << (counter_size - 1)) - 1);

    /** Get the overflow status and real counter value for de-queue packet count */
    dbal_tables_field_size_get(unit, DBAL_TABLE_INGRESS_IPT_COUNTER, DBAL_FIELD_PACKET_DISCARD, 0, 0, 0, &counter_size);
    counter_info->itpp_disc_pkt_ovf =
        is_gtimer_enabled ? 0 : ((counter_info->pkt_itpp_disc_cnt >> (counter_size - 1)) & 0x1);
    counter_info->pkt_itpp_disc_cnt = counter_info->pkt_itpp_disc_cnt & ((1 << (counter_size - 1)) - 1);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief Function to retrieve the correct dbal table id for the 
 *        ingress shapers configurations
 *  
 * \param unit - unit
 * \param gport - ingress receive gport
 * \param table_id - table_id
 * 
 * \return int 
 */
int
dnx_ingress_receive_table_id_get(
    int unit,
    bcm_gport_t gport,
    int *table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED(gport))
    {
        *table_id = DBAL_TABLE_INGRESS_SHAPER_RCY_GUARANTEED;
    }
    else if (BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED(gport))
    {
        *table_id = DBAL_TABLE_INGRESS_SHAPER_CPU_GUARANTEED;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport 0x%x provided.\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to validate the inputs of 
 * dnx_cosq_ingress_receive_bandwith_set 
 */
static int
dnx_ingress_receive_bandwith_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    int core;

    SHR_FUNC_INIT_VARS(unit);

    /** Cosq paramter should be always 0 */
    SHR_VAL_VERIFY(cosq, 0, _SHR_E_PARAM, "cosq parameter must be set to 0!");

    /** Validate core */
    core = BCM_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(gport);
    DNXCMN_CORE_VALIDATE(unit, core, TRUE);

    /** No flags are currently supported */
    SHR_VAL_VERIFY(flags, 0, _SHR_E_PARAM, "NO flags are curently supported. flags parameter must be set to 0!");

    /** kbits_sec_min paramter should be always 0. It is unused */
    SHR_VAL_VERIFY(kbits_sec_min, 0, _SHR_E_PARAM, "kbits_sec_min parameter must be set to 0!");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to vlaidate the inputs of 
 * dnx_cosq_ingress_receive_bandwith_get 
 */
static int
dnx_ingress_receive_bandwith_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    /** Cosq paramter should be always 0 */
    SHR_VAL_VERIFY(cosq, 0, _SHR_E_PARAM, "cosq parameter must be set to 0!");

    /** Validate core */
    core = BCM_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(gport);
    DNXCMN_CORE_VALIDATE(unit, core, TRUE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to calculate the best timer cycles and 
 *        grant size for the ingress shaper in order to reach as
 *        close as possible the passed rate. 
 *        Rate is in Mega bytes per second
 *
 * \param [in] unit - unit id.
 * \param [in] rate - desired rate
 * \param [in] max_nof_timer_cycles - maximal value of timer cycles
 * \param [in] max_grant_size_val - maximal value of grant size
 * \param [out] best_grant_size - result grant size
 * \param [out] best_timer_cycles - result timer cycles
 */
shr_error_e
dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc(
    int unit,
    uint32 rate,
    uint32 max_nof_timer_cycles,
    uint32 max_grant_size_val,
    uint32 *best_grant_size,
    uint32 *best_timer_cycles)
{
    uint32 min_delta = 0xffffffff, delta;
    uint32 grant_size, actual_rate;
    int current_timer_cycles;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    /** If about to disable the shaper */
    if (rate == 0)
    {
        /** In HW timer cycles of 0 means grant each clock cycle.
         *  However before timer cycle value is set to HW it is
         *  reduced by 1
         *  (dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_set) */
        *best_grant_size = 0;
        *best_timer_cycles = 1;
    }
    else
    {
        /** Check all possible timer cycles value and find the most
         *  with which value that actual rate will be closer to the
         *  requested */
        /** Set init value */
        *best_grant_size = -1;
        for (current_timer_cycles = 1; current_timer_cycles <= max_nof_timer_cycles; current_timer_cycles++)
        {
            /** Get actual rate.
             *  Calculation for grant_size is:
             *
             *     current_timer_cycles * (rate [Mbits/sec] * 1000)
             *   ----------------------------------------------------
             *                core_clock_freq [kHz]
             */
            COMPILER_64_ZERO(u64_calc);
            COMPILER_64_SET(u64_calc, 0, (current_timer_cycles * rate));
            COMPILER_64_UMUL_32(u64_calc, 1000);
            /** To round to nearest, formula is ((x) + ((y)/2) ) / (y) */
            COMPILER_64_ADD_32(u64_calc, (DNXCMN_CORE_CLOCK_KHZ_GET(unit) / 2));
            COMPILER_64_UDIV_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
            grant_size = COMPILER_64_LO(u64_calc);
            /** Currently in u64_calc we have grant_size, to calculate the actual rate in [Mbits/sec]:
             *
             *          grant_size * core_clock_freq [kHz]
             *   ----------------------------------------------------
             *              current_timer_cycles * 1000
             */
            COMPILER_64_ZERO(u64_calc);
            COMPILER_64_SET(u64_calc, 0, grant_size);
            COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
            /** To round to nearest, formula is ((x) + ((y)/2) ) / (y) */
            COMPILER_64_ADD_32(u64_calc, ((current_timer_cycles * 1000) / 2));
            COMPILER_64_UDIV_32(u64_calc, (current_timer_cycles * 1000));
            actual_rate = COMPILER_64_LO(u64_calc);

            delta = UTILEX_DELTA(rate, actual_rate);
            /** Try to get the minimum delta between the requested and
             *  actual rate */
            if (delta < min_delta)
            {
                /** Check if calculated grant size will fit in the HW field */
                if (grant_size <= max_grant_size_val)
                {
                    min_delta = delta;
                    *best_grant_size = grant_size;
                    *best_timer_cycles = current_timer_cycles;
                }
            }
        }
    }

    /** Check if a valid combination(timer_cycles/grant_size) was
     *  found for the provided rate */
    if (*best_grant_size == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The rate (%d) provided cannot be configured.\n", rate);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to calculate the best timer cycles and 
 *        grant size for the ingress shaper in order to reach as
 *        clese as possible the passed rate. Rate is in Kbits in
 *        second
 */
static shr_error_e
dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_for_dbal_calc(
    int unit,
    int dbal_table_id,
    uint32 rate,
    uint32 *best_grant_size,
    uint32 *best_timer_cycles)
{
    uint32 max_nof_timer_cycles = 0, max_grant_size_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Get max value for timer cycles */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, dbal_table_id, DBAL_FIELD_TIMER_CYCLES, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     &max_nof_timer_cycles));
    /** Get max value for timer cycles */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, dbal_table_id, DBAL_FIELD_GRANT_SIZE, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     &max_grant_size_val));

    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc(unit, rate,
                                                                                     max_nof_timer_cycles,
                                                                                     max_grant_size_val,
                                                                                     best_grant_size,
                                                                                     best_timer_cycles));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See cosq_ingress.h
 */
int
dnx_cosq_ingress_receive_bandwith_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 timer_cycles = 0, grant_size = 0, rate_mbytes_sec = 0;
    int table_id, core;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate inputs */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_receive_bandwith_set_validate
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));

    /** Get dbal table id relevant for the gport type provided */
    SHR_IF_ERR_EXIT(dnx_ingress_receive_table_id_get(unit, gport, &table_id));

    /** Convert Kbps to MBps */
    rate_mbytes_sec = UTILEX_DIV_ROUND(UTILEX_TRANSLATE_BITS_TO_BYTES(kbits_sec_max), 1000);

    /** Calculate best grant size and timer cycles */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_for_dbal_calc
                    (unit, table_id, rate_mbytes_sec, &grant_size, &timer_cycles));

    /** Get the core to be configured */
    core = BCM_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(gport);

    /** Set actual ingress rate */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_set
                    (unit, table_id, core, grant_size, timer_cycles));

exit:
    SHR_FUNC_EXIT;
}

/** Calculate actual rate from grant size and timer cycles */
static int
dnx_cosq_ingress_receive_shaper_actual_rate_calc(
    int unit,
    uint32 grant_size,
    uint32 timer_cycles,
    uint32 *actual_rate)
{
    uint64 u64_calc;
    SHR_FUNC_INIT_VARS(unit);

    /** Calculate rate*/
    if (grant_size == 0)
    {
        *actual_rate = 0;
    }
    else
    {
        if (timer_cycles == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Timer cycles value cannot be 0.\n");
        }
        /** To calculate the actual rate in [Mbits/sec]:
         *
         *          grant_size * core_clock_freq [kHz]
         *   ----------------------------------------------------
         *              timer_cycles * 1000
         */
        COMPILER_64_ZERO(u64_calc);
        COMPILER_64_SET(u64_calc, 0, grant_size);
        COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
        /** To round to nearest, formula is ((x) + ((y)/2) ) / (y) */
        COMPILER_64_ADD_32(u64_calc, ((timer_cycles * 1000) / 2));
        COMPILER_64_UDIV_32(u64_calc, (timer_cycles * 1000));
        *actual_rate = COMPILER_64_LO(u64_calc);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * 
 * See cosq_ingress.h
 */
int
dnx_cosq_ingress_receive_bandwith_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    uint32 timer_cycles = 0, grant_size = 0, mega_bytes_sec_max = 0;
    int table_id, core;
    SHR_FUNC_INIT_VARS(unit);

    /** Validate inputs */
    SHR_INVOKE_VERIFY_DNX(dnx_ingress_receive_bandwith_get_validate
                          (unit, gport, cosq, kbits_sec_min, kbits_sec_max, flags));

    /** Get dbal table id relevant for the gport type provided */
    SHR_IF_ERR_EXIT(dnx_ingress_receive_table_id_get(unit, gport, &table_id));

    /** Get the core */
    core = BCM_COSQ_GPORT_INGRESS_RECEIVE_CORE_GET(gport);

    /** Get actual ingress rate grant size and timer cycles   */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_get
                    (unit, table_id, core, &grant_size, &timer_cycles));

    /** Min is always 0*/
    *kbits_sec_min = 0;

    /** Calculate rate in Mega Bytes per second */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_actual_rate_calc
                    (unit, grant_size, timer_cycles, &mega_bytes_sec_max));
    /** Grant size is for MBps. Translate to Kbps */
    *kbits_sec_max = mega_bytes_sec_max * UTILEX_NOF_BITS_IN_BYTE * 1000;
exit:
    SHR_FUNC_EXIT;
}
/**
 * 
 * \brief - Function to configure the device overall ingress 
 *        receive rate. The rate is given in Mega packets per
 *        second
 * 
 * \param unit 
 * \param rate_mpps - the rate in mega packets per second
 * 
 * \return int 
 */
int
dnx_cosq_ingress_receive_packet_rate_limit_set(
    int unit,
    int rate_mpps)
{
    uint32 timer_cycles = 0, grant_size = 0;
    uint32 rate_mpps_per_core;
    SHR_FUNC_INIT_VARS(unit);

    /** The rate provided is for the whole device. Before writing
     *  to HW the rate should be split evenly per core */
    rate_mpps_per_core = UTILEX_DIV_ROUND(rate_mpps, dnx_data_device.general.nof_cores_get(unit));

    /** Calculate best grant size and timer cycles */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_for_dbal_calc
                    (unit, DBAL_TABLE_INGRESS_SHAPER_PACKET_RATE, rate_mpps_per_core, &grant_size, &timer_cycles));

    /** Set actual ingress rate */
    /** Configure ALL cores with the calculated value */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_set
                    (unit, DBAL_TABLE_INGRESS_SHAPER_PACKET_RATE, BCM_CORE_ALL, grant_size, timer_cycles));

exit:
    SHR_FUNC_EXIT;
}

/**
 * 
 * See cosq_ingress.h
 */
int
dnx_cosq_ingress_receive_packet_rate_limit_get(
    int unit,
    int *rate_mpps)
{
    uint32 timer_cycles = 0, grant_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rate_mpps, _SHR_E_PARAM, "rate_mpps");

    /** Get actual ingress rate grant size and timer cycles for 1
     *  core. Function assumes that the core rate destributed
     *  evenly between all cores */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_get
                    (unit, DBAL_TABLE_INGRESS_SHAPER_PACKET_RATE, 0, &grant_size, &timer_cycles));

    /** Calculate mega packets per second */
    SHR_IF_ERR_EXIT(dnx_cosq_ingress_receive_shaper_actual_rate_calc
                    (unit, grant_size, timer_cycles, (uint32 *) rate_mpps));

    /** The Rate value is splitted evenly between all cores */
    *rate_mpps *= dnx_data_device.general.nof_cores_get(unit);

exit:
    SHR_FUNC_EXIT;
}
