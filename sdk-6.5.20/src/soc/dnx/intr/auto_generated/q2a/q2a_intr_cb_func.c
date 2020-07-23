/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for Q2A interrupts.
 */


#include <shared/bsl.h>
#include <soc/mcm/allenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>

#include <soc/dnx/drv.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr.h>

#include <soc/sand/sand_intr_corr_act_func.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR



static int
q2a_intr_decide_action(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg,
    uint32 *param)
{
    dnxc_int_corr_act_type action = DNXC_INT_CORR_ACT_NONE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "msg");
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");
    SHR_NULL_CHECK(param, _SHR_E_PARAM, "param");

    *param = 0;
    switch (en_interrupt)
    {
        case Q2A_INT_OAMP_PENDING_EVENT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO;
            break;

        case Q2A_INT_OAMP_STAT_PENDING_EVENT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO;
            break;

        case Q2A_INT_DDP_DELETE_BDB_FIFO_FULL:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_FULL;
            break;

        case Q2A_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY;
            break;

        case Q2A_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
        case Q2A_INT_CLU_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_PFC_DEADLOCK_BREAKING_MECHANISM;
            break;

        case Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT:
        case Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT:
            action = DNXC_INT_CORR_ACT_ILKN_RX_PORT_STATUS_CHANGE;
            break;

        case Q2A_INT_TCAM_TCAM_QUERY_FAILURE_VALID:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID;
            break;
        case Q2A_INT_TCAM_TCAM_MOVE_ECC_VALID:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_TCAM_MOVE_ECC_VALID;
            break;

        case Q2A_INT_CGM_CONGESTION_PB_VSQ_PG_CONGESTION_FIFO_ORDY:
        case Q2A_INT_CGM_CONGESTION_PB_VSQ_LLFC_CONGESTION_FIFO_ORDY:
        case Q2A_INT_CGM_CONGESTION_VOQ_CONGESTION_FIFO_ORDY:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_NONE;
            break;

        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_0:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_1:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_2:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_3:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_4:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_5:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_6:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_7:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_8:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_9:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_10:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_11:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_12:
        case Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_13:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR;
            break;

        case Q2A_INT_MCP_ENG_DB_A_BANK_0_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_A_BANK_1_ACCESS_ERR_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            *param = en_interrupt - Q2A_INT_MCP_ENG_DB_A_BANK_0_ACCESS_ERR_INT;
            action = DNXC_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT;
            break;

        case Q2A_INT_MCP_ENG_DB_C_BANK_0_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_1_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_2_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_3_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_4_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_5_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_6_ACCESS_ERR_INT:
        case Q2A_INT_MCP_ENG_DB_C_BANK_7_ACCESS_ERR_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            *param = en_interrupt - Q2A_INT_MCP_ENG_DB_C_BANK_0_ACCESS_ERR_INT;
            action = DNXC_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT;
            break;
        case Q2A_INT_ETPPB_BTC_HEADER_SIZE_EXCEED:
        case Q2A_INT_ETPPB_BTC_NOF_INSTRUCTIONS_EXCEED:
        case Q2A_INT_ETPPB_BTC_HEADER_SIZE_NOT_BYTE_ALLIGNED:
        case Q2A_INT_EPNI_ETPP_DEC_ABOVE_TH_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_NONE;
            break;
        case Q2A_INT_IPPD_INVALID_DESTINATION_VALID:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID;
            break;
        default:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING;
            break;
    }
    *p_corrective_action = action;
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_none(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = sand_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
STATIC int
q2a_hard_reset(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_HARD_RESET, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_HARD_RESET,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_soft_init(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_SOFT_INIT, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_SOFT_INIT,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
STATIC int
q2a_soft_reset(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_SOFT_RESET, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_SOFT_RESET,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_pon_reset(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type action;
    SHR_FUNC_INIT_VARS(unit);

    action = DNXC_INT_CORR_ACT_PON_RESET;

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, action, msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, action, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_reload_board(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type action;
    SHR_FUNC_INIT_VARS(unit);

    switch (en_interrupt)
    {
        
        case Q2A_INT_CGM_VOQ_STATE_INTERNAL_CACHE_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_HDRM_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_0_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_1_OC_ERR_INT:
        case Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_WORDS_ERR_INT:
        case Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_WORDS_ERR_INT:
        case Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_WORDS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_BUFFERS_ERR_INT:
        case Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_PDS_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_GRNTD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_SHRD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_GRNTD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_SHRD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_GRNTD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_SHRD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_GRNTD_OC_ERR_INT:
        case Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_SHRD_OC_ERR_INT:
            
            action = DNXC_INT_CORR_ACT_SOFT_RESET;
            break;
        
        case Q2A_INT_IPS_QDESC_INTERNAL_CACHE_ERROR:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_UNF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_OVF_INT:
        case Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_UNF_INT:
            
            action = DNXC_INT_CORR_ACT_SOFT_RESET;
            break;

        default:
            action = DNXC_INT_CORR_ACT_HARD_RESET;
            break;
    }
    
    rc = sand_interrupt_print_info(unit, block_instance, en_interrupt, 0, action, msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, action, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_scrub_read_write_back(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = sand_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_special_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 param;
    SHR_FUNC_INIT_VARS(unit);

    
    rc = q2a_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg, &param);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, p_corrective_action,
                                                  &param, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_pfc_deadlock_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    uint8 is_cdu = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 param;
    SHR_FUNC_INIT_VARS(unit);

    
    rc = q2a_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg, &param);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    switch (en_interrupt)
    {
        case Q2A_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
            is_cdu = TRUE;
            break;
        case Q2A_INT_CLU_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
            is_cdu = FALSE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interrupt Id: %d\n", en_interrupt);
            break;

    }
    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, p_corrective_action,
                                                  (void *) &is_cdu, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_ilkn_link_status_change_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    uint8 port_in_core = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    bcm_switch_event_control_t type;
    uint32 param;
    SHR_FUNC_INIT_VARS(unit);

    
    type.index = block_instance;
    type.event_id = en_interrupt;
    type.action = bcmSwitchEventClear;
    rc = bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, TRUE);
    SHR_IF_ERR_EXIT(rc);

    
    rc = q2a_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg, &param);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    switch (en_interrupt)
    {
        case Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT:
            port_in_core = 0;
            break;
        case Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT:
            port_in_core = 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interrupt Id: %d\n", en_interrupt);
            break;

    }
    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, p_corrective_action,
                                                  (void *) &port_in_core, NULL);
    SHR_IF_ERR_EXIT(rc);

    
    type.action = bcmSwitchEventMask;
    rc = bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, FALSE);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_reprogram_resource(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    
    int rc = 0;
    SHR_FUNC_INIT_VARS(unit);
    

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE, msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg,
                                                  DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_retrain_link(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    uint32 rc;
    SHR_FUNC_INIT_VARS(unit);

    

    rc = sand_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_parity_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance, en_interrupt, info_msg,
                                                      &p_corrective_action, &shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, p_corrective_action,
                                                  (void *) &shadow_correct_info, info_msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_parity_recurr(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
STATIC int
q2a_intr_ecc_1b_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance, en_interrupt, info_msg,
                                                      &p_corrective_action, &shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, p_corrective_action,
                                                  (void *) &shadow_correct_info, info_msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
STATIC int
q2a_intr_ecc_1b_recurr(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_ecc_2b_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    SHR_FUNC_INIT_VARS(unit);

    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance, en_interrupt, info_msg,
                                                      &p_corrective_action, &shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, p_corrective_action,
                                                  (void *) &shadow_correct_info, info_msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_ecc_2b_recurr(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_tcam_ecc_1b_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    sand_intr_tcam_location tcam_location;
    dnxc_int_corr_act_type corrective_action;
    char info_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    corrective_action = DNXC_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB;
    rc = dnxc_interrupt_data_collection_for_tcam_protection_error(unit, block_instance, info_msg, &tcam_location);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, corrective_action,
                                                  (void *) &tcam_location, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_tcam_ecc_2b_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    sand_intr_tcam_location tcam_location;
    dnxc_int_corr_act_type corrective_action;
    char info_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    corrective_action = DNXC_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB;
    rc = dnxc_interrupt_data_collection_for_tcam_protection_error(unit, block_instance, info_msg, &tcam_location);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, corrective_action,
                                                  (void *) &tcam_location, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
q2a_intr_kaps_tecc_handler(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type action;
    int is_ecc_2bits = 0, is_rpb_a = 0, index = 0;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    sand_intr_tcam_location tcam_location;

    SHR_FUNC_INIT_VARS(unit);

    action = DNXC_INT_CORR_ACT_KAPS_TECC;
    switch (en_interrupt)
    {
        case Q2A_INT_KAPS_TECC_A_0_1B_ERROR:
            is_ecc_2bits = 0;
            is_rpb_a = 1;
            index = 0;
            break;
        case Q2A_INT_KAPS_TECC_A_0_2B_ERROR:
            is_ecc_2bits = 1;
            is_rpb_a = 1;
            index = 0;
            break;
        case Q2A_INT_KAPS_TECC_B_0_1B_ERROR:
            is_ecc_2bits = 0;
            is_rpb_a = 0;
            index = 0;
            break;
        case Q2A_INT_KAPS_TECC_B_0_2B_ERROR:
            is_ecc_2bits = 1;
            is_rpb_a = 0;
            index = 0;
            break;
        case Q2A_INT_KAPS_TECC_A_1_1B_ERROR:
            is_ecc_2bits = 0;
            is_rpb_a = 1;
            index = 1;
            break;
        case Q2A_INT_KAPS_TECC_A_1_2B_ERROR:
            is_ecc_2bits = 1;
            is_rpb_a = 1;
            index = 1;
            break;
        case Q2A_INT_KAPS_TECC_B_1_1B_ERROR:
            is_ecc_2bits = 0;
            is_rpb_a = 0;
            index = 1;
            break;
        case Q2A_INT_KAPS_TECC_B_1_2B_ERROR:
            is_ecc_2bits = 1;
            is_rpb_a = 0;
            index = 1;
            break;
        default:
            action = DNXC_INT_CORR_ACT_NONE;
            break;
    }

    rc = dnx_interrupt_data_collection_for_kaps_tcam_error(unit, block_instance, is_ecc_2bits,
                                                           is_rpb_a, index, special_msg, &tcam_location);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, action, special_msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static int
q2a_intr_clear_MactLelaFidExceedLimit(
    int unit,
    int block_instance,
    int en_interrupt)
{
    int rc;
    uint32 reg_value;
    int lastFid;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_reg32_get(unit, MACT_LARGE_EM_LELA_EXCEED_LIMIT_FIDr, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    lastFid = soc_reg_field_get(unit, MACT_LARGE_EM_LELA_EXCEED_LIMIT_FIDr, reg_value, LARGE_EM_LELA_EXCEED_LIMIT_FIDf);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flag);
    SHR_IF_ERR_EXIT(rc);

    
    if (SHR_BITGET(&flag, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Block %d, the last FID which Exceeded Lela FID Limit %d\n"),
                                    block_instance, lastFid));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
q2a_intr_clear_MactLelbFidExceedLimit(
    int unit,
    int block_instance,
    int en_interrupt)
{
    int rc;
    uint32 reg_value;
    int lastFid;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_reg32_get(unit, MACT_LARGE_EM_LELB_EXCEED_LIMIT_FIDr, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    lastFid = soc_reg_field_get(unit, MACT_LARGE_EM_LELB_EXCEED_LIMIT_FIDr, reg_value, LARGE_EM_LELB_EXCEED_LIMIT_FIDf);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flag);
    SHR_IF_ERR_EXIT(rc);

    
    if (SHR_BITGET(&flag, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Block %d, the last FID which Exceeded Lelb FID Limit %d\n"),
                                    block_instance, lastFid));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
q2a_intr_clear_MactMngmntReqFidExceedLimitAllowed(
    int unit,
    int block_instance,
    int en_interrupt)
{
    int rc;
    uint32 reg_value;
    int lastFid;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_reg32_get(unit, MACT_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_ALLOWED_FIDr, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    lastFid =
        soc_reg_field_get(unit, MACT_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_ALLOWED_FIDr, reg_value,
                          LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_ALLOWED_FIDf);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flag);
    SHR_IF_ERR_EXIT(rc);

    
    if (SHR_BITGET(&flag, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit, "Block %d, the last FID which Exceeded Mngmnt req Fid Limit allowed %d\n"),
                   block_instance, lastFid));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
q2a_intr_clear_MactMngmntReqFidExceedLimit(
    int unit,
    int block_instance,
    int en_interrupt)
{
    int rc;
    uint32 reg_value;
    int lastFid;
    uint32 flag;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_reg32_get(unit, MACT_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDr, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    lastFid =
        soc_reg_field_get(unit, MACT_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDr, reg_value,
                          LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDf);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flag);
    SHR_IF_ERR_EXIT(rc);

    
    if (SHR_BITGET(&flag, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Block %d, the last FID which Exceeded Mngmnt req Fid Limit %d\n"),
                                    block_instance, lastFid));
    }

exit:
    SHR_FUNC_EXIT;
}

void
q2a_interrupt_cb_init(
    int unit)
{
    
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_ERROR_FREE_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_3, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_4, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_5, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_6, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_7, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_8, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_9, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_10, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_11, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_12, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_13, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_14, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OCB_FBC_BANK_ERROR_FBC_BANK_15, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TIMESTAMP_1588_FIFO_NOT_EMPTY_PM_0_INT, 0, 0, q2a_special_handling,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TIMESTAMP_1588_FIFO_NOT_EMPTY_PM_1_INT, 0, 0, q2a_special_handling,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TIMESTAMP_1588_FIFO_NOT_EMPTY_PM_2_INT, 0, 0, q2a_special_handling,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_0_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_1_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_2_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_3_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_4_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_5_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_6_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_7_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_8_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_9_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_10_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_RX_11_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_0_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_1_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_2_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_3_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_4_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_5_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_6_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_7_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_8_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_9_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_10_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_11_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_0_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_1_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_2_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_3_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_4_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_5_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_6_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_7_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_8_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_9_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_10_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_11_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_0_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_1_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_2_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_3_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_4_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_5_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_6_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_7_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_8_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_9_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_10_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLUP_TX_11_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_special_handling,
                              q2a_none,
                              "This interrupt means that the PM configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF ILKN ILU");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OLP_ERROR_EGRESS_PIPE_CFG, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OLP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OLP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_LMM_OVF_INT, 0, 0, q2a_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_HPM_OVF_INT, 0, 0, q2a_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_HPM_DROP_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_LMM_DROP_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPRE_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ERROR_TIMEOUT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ERROR_REASSEMBLY_CONTEXT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ERROR_BYTE_NUM, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ERROR_PKT_CRC, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ERROR_S_2D_PKT_CRC, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_BUFF, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_NO_BUFF, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_PACKET_REJECT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_OVERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_UNDERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_DELETE_FIFO_FULL, 0, 0, q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_LCD_FIFO_FULL, 0, 0, q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_TC_MAPPING_MISS_CONFIG, 0, 0, q2a_pon_reset, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_MC_MAX_REPLICATION_OVERFLOW_INT, 0, 0, q2a_pon_reset, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_EBTR_ILLEGAL_VALUE, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_INVALID_OTM_INT, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PQP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSAR_GENERAL_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSAR_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSAR_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDHB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDHB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_BYPASS_BTC_READ_WITHOUT_SOP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_APP_AND_PIPE_COLLISION, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_TM_FIELDS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_NETWORK_HEADERS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_SYSTEM_HEADERS_CONTAINER_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_MPLS_BOS_PARSING_DATA_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PSG_HEADER_SIZE_ERR_INT, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_HEADER_SIZE_ERR_INT, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_0, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_1, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_2, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_3, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_0, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_1, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_2, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_3, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_4, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_5, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_6, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_7, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPPA_PP_DSP_FOR_VISIBILITY_AND_IS_TDM_FIFO_ALMOST_FULL_HIGH_INT, 0,
                              0, q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPP_PRP_PRP_ACCEPTABLE_FRAME_TYPE_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPP_PRP_PRP_GLEM_ERROR_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPP_PRP_PRP_SYSTEM_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPP_PRP_PRP_PORT_ATTRIBUTES_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPA_ETPP_PRP_PRP_NETWORK_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_FPC_0_FREE_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_FPC_1_FREE_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_FPC_2_FREE_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_FPC_3_FREE_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_BDM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_RX_HRF_BURST_MERGE_ERROR_INT_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_RX_HRF_BURST_MERGE_ERROR_INT_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_RX_HRF_BURST_MERGE_ERROR_INT_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_RX_HRF_BURST_MERGE_ERROR_INT_3, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILU_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_INVALID_OTM_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_DSS_STACKING_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_EXCLUDE_SRC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_LAG_MULTICAST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_UNKNOWN_DA_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_SPLIT_HORIZON_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_GLEM_PP_TRAP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_GLEM_NON_PP_TRAP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_TTL_SCOPE_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_MTU_VIOLATION_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_TTL_EQUALS_ONE_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_OPTIONS_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_TTL_EQUALS_ZERO_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_LOOPBACK_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_HOP_BY_HOP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_LINK_LOCAL_DST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_IPV6_SITE_LOCAL_DST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_APP_AND_PIPE_COLLISION, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TDM_WRONG_PORT_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_SEQUENCE_NUMBER_AND_FLAGS_ARE_ZERO_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_SEQUENCE_NUMBER_IS_ZERO_AND_FIN_OR_URG_OR_PSH_IS_SET_INT,
                              0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_SYN_AND_FIN_ARE_SET_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_SRC_PORT_EQUALS_DST_PORT_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_FRAGMENT_WITH_INCOMPLETE_TCP_HEADER_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TCP_FRAGMENT_WITH_OFFSET_LESS_THAN_8_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_UDP_SRC_PORT_EQUALS_DST_PORT_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TM_FIELDS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_NETWORK_HEADERS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_SYSTEM_HEADERS_CONTAINER_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_ERPP_ERPP_DISCARD_2_PP_DSP_FOR_VISIBILITY_AND_IS_TDM_FIFO_ALMOST_FULL_HIGH_INT, 0,
                              0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_MPLS_BOS_PARSING_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PSG_HEADER_SIZE_ERR_INT, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_HEADER_SIZE_ERR_INT, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_TM_DATA_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_GLEM_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PRP_PER_PORT_INFO_FIFO_ALMOST_FULL_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_NETWORK_HEADER_FIFO_ALMOST_FULL_LOW_INT, 0,
                              0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_NETWORK_HEADER_FIFO_ALMOST_FULL_HIGH_INT,
                              0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PRP_SAVE_NETWORK_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PMF_INITIAL_ACTIONS_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_0, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_1, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_2, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_3, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_0, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_1, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_2, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_3, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_4, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_5, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_6, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_7, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ERPP_ERPP_DISCARD_2_DISCARD_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RRF_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_DQCF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_DQCF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_DQCF_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_D_BLF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_D_BLF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_PBF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_PBF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_PBF_MISCONFIG, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_PBF_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_DQCF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_DQCF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_DQCF_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_D_BLF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_D_BLF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_D_BLF_MISCONFIG, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_PBF_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_PBF_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_PBF_MISCONFIG, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_ERROR_S_2D_PBF_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_COMP_CNT_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_COMP_CNT_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_FIFO_SCS_UNDERFLOW, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_FTMH_IS_NOT_STAMPPED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_FTMH_PSIZE_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_PSIZE_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_EXPECTED_ITPP_DELTA_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPT_ITE_ERR_NEGATIVE_DELTA, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_EVENTOR_EVENTOR_INTERRUPT_BIT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_EVENTOR_EVENTOR_RX_CMIC_ERROR_INTERRUPT_BIT, 0, 0,
                              q2a_reprogram_resource, NULL, "cmic descriptor or source data should be fixed");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_EVENTOR_EVENTOR_TX_CMIC_ERROR_INTERRUPT_BIT, 0, 0,
                              q2a_reprogram_resource, NULL, "cmic descriptor or source data should be fixed");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EVNT_EVENTOR_EVENTOR_TX_BINNING_WRP_INTERRUPT_BIT, 0, 0,
                              q2a_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_PROTECTION_ERROR_1_BIT_ECC, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_PROTECTION_ERROR_2_BIT_ECC, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_QUERY_FAILURE_VALID, 0, 0, q2a_special_handling, NULL,
                              "Configuration needs to be fixed. Additional information on the reason this interrupt was raised can be found at the MDB.TcamQueryFailure register. Interrupt handler issued a log message.");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_MOVE_ECC_VALID, 0, 0, q2a_special_handling, NULL,
                              "Corrupted Tcam Bank entry was discovered during TCM Move operation. Additional information on The Bank and line on which the issue was encountered can be found at the MDB.TcamMoveEccFailure register. Interrupt handler issued a log message.");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_0_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_1_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_2_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_3_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_4_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPPD_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DRAM_WRITE_CRC_DETECTED_ERROR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DRAM_READ_CRC_DETECTED_ERROR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_WRITE_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_WRITE_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_READ_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_READ_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_WRITE_OUT_SYNCHRONIZER_OVERFLOW_0, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_PASSED_SYNCHRONIZER_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_COHERENCY_PASSED_SYNCHRONIZER_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_BANK_ORDER_FIFO_OVERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_BANK_ORDER_FIFO_UNDERFLOW_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_DSI_WDS_OVERFLOW, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_RDR_RDR_INFO_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_RDR_RDR_INFO_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_RDR_RDS_OVERFLOW, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_RDR_LAST_SLICE_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_RDR_LAST_SLICE_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_BIST_REQUEST_CONTENTION, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_TEMPERATURE_READOUT_ABOVE_ABS, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_TEMPERATURE_READOUT_ABOVE_ABS_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_TSM_REFRESH_ACC_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_READ_DATA_QUEUE_BYTE_0_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_READ_DATA_QUEUE_BYTE_1_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_READ_EDC_QUEUE_BYTE_0_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_READ_EDC_QUEUE_BYTE_1_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_COMMAND_CONTENTION, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_ECC_RD_CONTENTION, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_EDC_IS_RD_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_EDC_IS_RD_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_EDC_RD_IS_TRAINING_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_EDC_RD_IS_TRAINING_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_RD_IS_TRAINING_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_RD_IS_TRAINING_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_PIPELINES_MUX_READ_DATA_SELECTOR_FIFO_OVERFLOW, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_PIPELINES_MUX_READ_DATA_SELECTOR_FIFO_UNDERFLOW, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_RD_CRC_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_RD_CRC_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_WR_CRC_FIFO_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DCC_PIPELINES_WR_CRC_FIFO_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_COHERENCY_WRITE_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_COHERENCY_WRITE_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_COHERENCY_READ_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_COHERENCY_READ_UNDERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_WRITE_REQUEST_RXI_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_READ_REQUEST_RXI_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_READ_DATA_TXI_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TDU_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_0_RX_FRM_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_1_RX_FRM_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_0_RX_DIP_2_ERR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_1_RX_DIP_2_ERR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_0_RX_WD_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_SPI_OOB_1_RX_WD_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CFC_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPC_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPC_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPC_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FASIC_GENERAL_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FASIC_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FASIC_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPF_VTC_OUTLIF_OVERLOAD, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPF_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPF_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPF_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_HEAD_UPDT_IN_LAST_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_HEAD_UPDT_IN_EMPTY_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_DEQ_CMD_TO_EMPTY_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_QDM_INTERNAL_CACHE_ERROR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_ALLOCATED_PDM_PDB_RANGE_ERR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_ALLOCATED_WHEN_PDB_FIFO_EMPTY_ERR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_RELEASED_WHEN_PDB_FIFO_FULL_ERR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_PD_FIFO_OVF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_PD_FIFO_UNF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_BUNDLE_FIFO_OVF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_BUNDLE_FIFO_UNF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_PD_FIFO_OVF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_PD_FIFO_UNF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_BUNDLE_FIFO_OVF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_BUNDLE_FIFO_UNF_INT, 0, 0, q2a_retrain_link,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_0, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_1, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_2, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_3, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_4, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_5, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_6, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_7, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_8, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_9, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_10, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_11, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_12, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_13, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_QUALIFIER_SIZE_ERROR_14, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_APP_AND_PIPE_COLLISION, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPE_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSCL_GENERAL_INT_0, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSCL_GENERAL_INT_1, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSCL_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FSCL_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling, q2a_none,
                              "This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_WRONG_WORD_FROM_MAC_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_FE_PORTID_COLLISION_ERR_INT, 0, 0, q2a_none, q2a_none, "Ignore ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_MISSING_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_DOUBLE_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_NUM_DROPPED_EOPS_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_NUM_DROPPED_EOPS_75P_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Read the counter indicated by NbiThrownBurstsCounters0 75pInterruptRegister.");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_ALIGNER_FIFO_MAC_OVF_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_0_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_1_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_2_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_3_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_4_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_5_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_6_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_RX_7_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the  Rx aligner Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_0_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_1_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_2_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_3_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_4_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_5_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_6_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_7_DATA_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  overflow in the Tx aligner  Data Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_0_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_1_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_2_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_3_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_4_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_5_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_6_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_7_CREDIT_ASYNC_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an overflow in the Tx aligner  Credit Async fifo");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_0_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_1_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_2_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_3_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_4_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_5_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_6_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_TX_7_DATA_ASYNC_FIFO_UNDERFLOW_INT, 0, 0, q2a_none, q2a_none,
                              "This interrupt means that there was an  underflow in the Tx aligner  Data Async fifo   ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_0_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx0_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_1_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx1_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_2_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx2_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_3_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx3_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_4_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx4_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_5_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx5_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_6_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx6_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_7_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx7_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_8_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx8_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_9_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx9_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_10_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx10_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_11_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx11_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_12_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx12_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_13_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx13_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_14_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx14_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_15_INT, 0, 0,
                              q2a_special_handling, NULL, "Read the counter indicated by rx15_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPPC_BIER_BIT_MASK_FIFO_ALMOST_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPPC_BIER_BIT_MASK_FIFO_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPPC_ENCAPSULATION_1_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_TERM_PIPE_FIFO_1_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_VSD_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_ESEM_1_RESULT_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_ESEM_2_RESULT_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_EXEM_RESULT_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_OEM_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_TERM_PIPE_FIFO_2_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_CRPS_RESULT_FIFO_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_MRPS_RESULT_FIFO_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_TERM_SPLIT_HORIZON_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_PROTECTION_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_LATENCY_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPC_ETPP_TERM_ACTION_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_MICRO_BURST_STATU_RDY, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_USR_CNT_DEC_VAL_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_ERR_FLOW_ID_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_ERR_QUEUE_NUMBER_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_DRAM_BOUND_RECOVERY_STATUS_ERR_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VOQ_STATE_INTERNAL_CACHE_ERR_INT, 0, 0, q2a_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_DEL_CTR_RDY, 0, 0, q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_DEL_CTR_DROP_ERR_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_GRNTD_OC_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_SHRD_OC_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_GRNTD_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_SHRD_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_GRNTD_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_SHRD_OC_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_GRNTD_OC_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_SHRD_OC_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_HDRM_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_0_OC_ERR_INT, 0,
                              0, q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_1_OC_ERR_INT, 0,
                              0, q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_0_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_1_OC_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_WORDS_ERR_INT, 0, 0, q2a_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_BUFFERS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_PDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_WORDS_ERR_INT, 0, 0, q2a_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_BUFFERS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_PDS_ERR_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_PG_CONGESTION_FIFO_ORDY, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_LLFC_CONGESTION_FIFO_ORDY, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_VOQ_CONGESTION_FIFO_ORDY, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_GLBL_SRAM_BUFFERS_FC_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_GLBL_SRAM_PDBS_FC_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_GLBL_DRAM_BDBS_FC_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_SRAM_BUFFERS_POOL_0_FC_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_SRAM_BUFFERS_POOL_1_FC_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_SRAM_PDS_POOL_0_FC_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_SRAM_PDS_POOL_1_FC_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_WORDS_POOL_0_FC_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_CONGESTION_PB_VSQ_WORDS_POOL_1_FC_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_GLBL_SRAM_BUFFERS_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_GLBL_SRAM_PDBS_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_GLBL_DRAM_BDBS_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_BUFFERS_POOL_0_FC_LP_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_BUFFERS_POOL_1_FC_LP_INT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_PDS_POOL_0_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_PDS_POOL_1_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_WORDS_POOL_0_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CGM_LOW_PRIORITY_PB_VSQ_WORDS_POOL_1_FC_LP_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_PENDING_EVENT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_STAT_PENDING_EVENT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_RX_STATS_DONE, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_RFC_6374_PKT_DROPPED, 0, 0, q2a_special_handling, q2a_none,
                              "RFC6374 was dropped due to ignore configurations");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_TXO_MESSAGE_SENT, 0, 0, q2a_special_handling, q2a_none,
                              "OAMP TXO sent a message");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_TXO_REQ_FIFO_EMPTY, 0, 0, q2a_special_handling, q2a_none,
                              "TCO request FIFO is empty and ready");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_MDB_0_ERR, 0, 0, q2a_special_handling, q2a_none,
                              "RMEP interface 0 returned an error - check MDB memories");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_MDB_1_ERR, 0, 0, q2a_special_handling, q2a_none,
                              "RMEP interface 1 returned an error - check MDB memories");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_RXP_FIFO_WRITE_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_OAMP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FQP_TXQ_OVF_INT, 0, 0, q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FQP_TXQ_READ_CONJESTED_INT, 0, 0, q2a_pon_reset, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FQP_TXQ_WRITE_CONJESTED_INT, 0, 0, q2a_pon_reset, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FQP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FQP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_RES_1, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_ACT_FLOW_BAD_PARAMS, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SHP_FLOW_BAD_PARAMS, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_RESTART_FLOW_EVENT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SMP_THROW_SCL_MSG, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SMP_FULL_LEVEL_1, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SMP_FULL_LEVEL_2, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_AGGR_FIFO_AF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_DVS_FIFO_AF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_UP_CREDIT_FIFO_AF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_RETURNED_CREDIT_FIFO_AF, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_MCAST_CREDIT_FIFO_AF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_ING_SHAPE_CREDIT_FIFO_AF, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_FCT_FIFO_OVF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SCL_GROUP_CHANGED, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_RES_2, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SMP_BAD_MSG, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_SMP_FABRIC_MSGS_FIFO_FULL, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SCH_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_PORT_MTU_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_STP_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_MEMBERSHIP_TRAP_INT, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_TRAP_ORIGINAL_FTMH_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_TRAP_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, q2a_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_ENCAPSULATION_2_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_ENCAPSULATION_3_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_ENCAPSULATION_4_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_ETPPB_ENCAPSULATION_5_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              q2a_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_BTC_HEADER_SIZE_EXCEED, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_BTC_NOF_INSTRUCTIONS_EXCEED, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_BTC_HEADER_SIZE_NOT_BYTE_ALLIGNED, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ETPPB_BTC_POP_BEFORE_FIFO_READY, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_ING_ERR_MAX_REPLICATION, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_ING_ERR_REPLICATION_EMPTY, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_EGR_ERR_MAX_REPLICATION, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_EGR_ERR_REPLICATION_EMPTY, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_ING_BMP_DIRECT_ACCESS, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_EGR_BMP_DIRECT_ACCESS, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MTM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IQM_PDQ_SEQ_NUM_INTERNAL_CACHE_ERROR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IQM_QM_RPRT_FIFO_OVF, 0, 0, q2a_retrain_link, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IQM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IQM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_TX_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_WRONG_WORD_FROM_TMAC_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_TX_MISSING_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_TX_DOUBLE_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_ASYNCFIFO_FULL_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_INSTRUMENTATION_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_SAR_INSTRUMENTATION_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_NUM_DROPPED_EOPS_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_RX_NUM_DROPPED_EOPS_75P_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_ALIGNER_FIFO_MAC_0_OVF_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_ALIGNER_FIFO_MAC_1_OVF_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_SAR_RX_OVERFLOW, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_SAR_TX_OVERFLOW, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_TX_INPUT_STAGE_IRE_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_TX_INPUT_STAGE_ESB_OVERFLOW, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_0_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_1_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_2_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_3_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_4_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_5_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_6_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_7_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_8_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_9_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_10_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_11_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_12_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_13_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_14_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_15_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_16_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_17_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_18_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_19_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_20_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_21_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_22_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_23_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_24_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_25_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_26_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_27_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_28_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_29_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_30_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_31_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_32_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_33_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_34_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_35_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_36_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_37_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_38_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_39_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_40_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_41_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_42_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_43_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_44_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_45_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_46_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_47_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_48_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_49_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_50_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_51_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_52_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_53_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_54_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_55_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_56_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_57_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_58_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_59_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_60_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_61_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_62_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_63_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_64_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_65_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_66_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_67_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_68_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_69_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_70_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_71_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_72_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_73_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_74_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_75_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_76_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_77_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_78_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_79_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_80_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_81_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_82_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_83_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_84_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_85_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_86_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_87_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_88_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_89_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_90_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_91_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_92_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_93_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_94_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_95_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_96_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_97_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_98_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_99_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_100_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_101_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_102_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_103_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_104_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_105_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_106_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_107_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_108_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_109_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_110_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_111_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_112_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_113_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_114_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_115_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_116_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_117_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_118_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_119_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_120_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_121_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_122_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_123_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_124_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_125_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_126_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FEU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_127_INT, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ERROR_UNEXPECTED_SOP, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ERROR_UNEXPECTED_MOP, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ERROR_BAD_REASSEMBLY_CONTEXT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ERROR_REASSEMBLY_CONTEXT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ALGN_ERR_STRIP_SIZE, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_CTXT_MAP_INVALID_RD_ADDR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_NIF_BT_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_SAT_BT_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_IPG_BT_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_CPU_BT_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_DROP, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_WSL_DROP, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_TDM_SIZE_ERR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_PACKET_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_SOP_WORD_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_PACKET_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_SOP_WORD_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_BCOUNT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_MISSING_EOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_MISSING_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_BCOUNT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_MISSING_EOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_MISSING_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_BCOUNT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_MISSING_EOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_MISSING_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_DATA_ARRIVED, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_BCOUNT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_MISSING_EOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_MISSING_SOP, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_DATA_ARRIVED, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_BCOUNT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_MISSING_EOP, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_MISSING_SOP, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_REGI_ERR_DATA_ARRIVED, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_INTERNAL_INTERFACE_ERROR_REGI_ERR_PACKET_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_NIF_ERROR_NIF_ERR_SOP_LESS_THAN_144_BYTES, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_NIF_ERROR_NIF_ERR_DATA_ARRIVED, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IRE_NIF_ERROR_NIF_ERR_PACKET_SIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_0_1B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_0_2B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_0_1B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_0_2B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_1_1B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_1_2B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_1_1B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_1_2B_ERROR, 0, 0, q2a_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_SQM_DQCQ_ERR, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_DQM_DQCQ_ERR, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_QUEUE_ENTERED_DEL, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_CRDT_LOST, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_CRBAL_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_PUSH_QUEUE_ACTIVE, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_LOST_FSM_EVENT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_CRDT_FLOW_ID_ERR_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_QDESC_INTERNAL_CACHE_ERROR, 0, 0, q2a_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_OVF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_UNF_INT, 0, 0,
                              q2a_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_ILLEGAL_CONSTRUCTED_HEADER_SIZE, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_INVALID_DESTINATION_VALID, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_NIF_PHYSICAL_ERR, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_FLP_FIFO_FULL, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_FLP_LOOKUP_TIMEOUT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_NIF_RX_FIFO_OVF, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_LOOKUP_REPLY_OVF, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_BAD_CHANNEL_NUM, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_INFO_REPLY_OVF, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_BAD_LKP_TYPE, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_NIF_TX_FIFO_FULL, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_RX_BROKEN_RECORD, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_INFO_REPLY_VALID, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_INFO_REPLY_ERR, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_LOOKUP_REPLY_VALID, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_CPU_LOOKUP_REPLY_ERR, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_SIP_TRANSPLANT_DETECTION_INT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_MC_EXPLICIT_RPF_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_MC_SIP_BASED_RPF_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_UC_LOOSE_RPF_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_UC_STRICT_RPF_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_SAME_INTERACE_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_FACILITY_INVALID_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_OUTLIF_OVER_FLOW_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_PACKET_IS_APPLET, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EDB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EDB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PDM_FREE_ERROR, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PDM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PDM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_0, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_1, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_2, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_0, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_1, 0, 0, q2a_none,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_0, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_1, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_2, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_MAC_ECC_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_SB_RX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_SB_TX_ASYNC_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_0_RX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_1_RX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_0_TX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_1_TX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_0_TX_FIFO_EMPTY, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_NB_1_TX_FIFO_EMPTY, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_SB_RX_CLIENT_FIFO_OVERFLOW, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_SB_RX_CLIENT_FIFO_UNDERFLOW, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_TINY_MAC_REGISTERS_NACK, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_TINY_MAC_MEMORY_NACK, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_CREDIT_CNT_OVERFLOW, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_RX_FLEXE_DOMAIN_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_TX_MSBUS_0_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_TX_MSBUS_1_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_SB_TX_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_FLEXEWP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              q2a_pon_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, q2a_pon_reset,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_ERROR_LEARN_REQUEST_OVER_STRONGER, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_WARNING_LEARN_OVER_EXISTING, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_ERROR_DELETE_NON_EXIST, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_ERROR_TRANSPLANT_REQUEST_OVER_STRONGER, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_ERROR_REFRESH_REQUEST_OVER_STRONGER, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_WARNING_TRANSPLANT_NON_EXIST, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_WARNING_REFRESH_NON_EXIST, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_ALLOWED, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_EVENT_READY, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_REPLY_READY, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_REPLY_FIFO_REPLY_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_FID_COUNTER_OVERFLOW, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_LARGE_EM_DB_EXCEED_LIMIT_ALLOWED, 0, 0,
                              q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_EMP_NON_LARGE_EM_EVENT_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELA_FID_EXCEED_LIMIT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELA_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELB_FID_EXCEED_LIMIT, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELB_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, q2a_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MACT_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_INC_ABOVE_TH_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_DEC_ABOVE_TH_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_TOTAL_INC_ABOVE_TH_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_TOTAL_INC_ABOVE_TH_WITH_EBTR_INT, 0, 0, q2a_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_TOTAL_DEC_ABOVE_TH_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_TAIL_ABOVE_INC_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_EBTR_ERR_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_TOTAL_DEC_ABOVE_PKT_SIZE_INT, 0, 0, q2a_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_CROP_TWO_NULL_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_PKT_INC_ABOVE_MAX_PKT_SIZE_INT, 0, 0, q2a_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_ILLEGAL_CROP_TYPE_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_MIRR_ADDITIONAL_INFO_ABOVE_TH_INT, 0, 0, q2a_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ETPP_MIRR_UNKNOWN_PRIORITY_INT, 0, 0, q2a_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ALIGNER_TRANSMIT_SIZE_ABOVE_TH_INT, 0, 0, q2a_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPNI_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_PACKET_OVERSIZE, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_DRAM_BUNDLE_OVERSIZE, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_SRAM_16B_CNT_OVRF, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_OMAC_CNT_OVRF, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_UNPACK_PACKET_SIZE_ERROR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_UNPACK_PACKET_SIZE_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ERROR_UNPACK_BUNDLE_OVERFLOW_ERROR, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_DELETE_BDB_FIFO_FULL, 0, 0, q2a_special_handling, q2a_none,
                              "Clear the deleted bdbs fifo by reading DeleteBdbFifoHead");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_PKUP_RX_CFIF_UNDERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_PKUP_RX_CFIF_OVERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_PKP_OUT_FIF_OVERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_BEC_PTR_FIFO_OVERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_BEC_ENQ_FIFO_OVERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_BEC_WAIT_FIFO_OVERFLOW, 0, 0, q2a_hard_reset, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_1B_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_2B_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_CRC_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_EXT_MEM_ERR_PKUP_PACKET_CRC_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_EXT_MEM_ERR_BTC_TDU_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED, 0, 0, q2a_special_handling,
                              q2a_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_FTMH_IS_NOT_STAMPPED, 0, 0, q2a_special_handling, q2a_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX, 0, 0, q2a_special_handling, q2a_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE, 0, 0, q2a_special_handling,
                              q2a_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_FTMH_PSIZE_MISMATCH, 0, 0, q2a_special_handling, q2a_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_PSIZE_MISMATCH, 0, 0, q2a_special_handling, q2a_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_EXPECTED_ITPP_DELTA_MISMATCH, 0, 0, q2a_special_handling,
                              q2a_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_ITE_ERR_NEGATIVE_DELTA, 0, 0, q2a_special_handling, q2a_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PEM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_PEM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_0, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_1, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_2, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_3, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_4, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_5, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_6, 0, 0, q2a_special_handling,
                              q2a_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_0, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_1, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_2, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_3, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_4, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_5, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_6, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_7, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_8, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_9, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_10, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_11, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_12, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_13, 0, 0,
                              q2a_special_handling, q2a_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_0_FULLNESS_DMA_0_NOT_EMPTY_EVENT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_0_FULLNESS_DMA_0_ALMOST_FULL_EVENT, 0, 0, q2a_special_handling,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_0_FULLNESS_DMA_0_FULL_EVENT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_1_FULLNESS_DMA_1_NOT_EMPTY_EVENT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_1_FULLNESS_DMA_1_ALMOST_FULL_EVENT, 0, 0, q2a_special_handling,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_DMA_1_FULLNESS_DMA_1_FULL_EVENT, 0, 0, q2a_special_handling, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_0, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_1, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_2, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_3, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_4, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_5, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_6, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_7, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_8, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_9, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_10, 0,
                              0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_11, 0,
                              0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_12, 0,
                              0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_13, 0,
                              0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_0,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_1,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_2,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_3,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_4,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_5,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_6,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_7,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_8,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_9,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_10,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_11,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_12,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              Q2A_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_13,
                              0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPA_VTD_OUTLIF_OVERLOAD, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPA_VTE_OUTLIF_OVERLOAD, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPA_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPA_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPA_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ERR_ITPP_PSIZE_TYPE_0_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ERR_ITPP_PSIZE_TYPE_1_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ERR_ITPP_PSIZE_TYPE_2_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ERR_ITPP_PSIZE_TYPE_3_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ERR_ITPP_PSIZE_TYPE_4_MISMATCH, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ITPP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_INGRESS_ENG_RX_FIFO_FULL_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGRESS_ENG_RX_FIFO_FULL_ERR_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGRESS_ENG_TX_FIFO_READ_WHEN_EMPTY_ERR_INT, 0, 0, q2a_hard_reset,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_WRAP_ING_GLBL_ENG_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_WRAP_ING_ENG_0_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_WRAP_ING_ENG_1_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_WRAP_ING_ENG_2_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGR_ENG_WRAP_EGR_ENG_0_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGR_ENG_WRAP_EGR_ENG_1_WRAP_ERR_INT, 0, 0, q2a_reprogram_resource,
                              q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_GLBL_ENG_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_0_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_1_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_2_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGR_ENG_OOP_FIFO_NOT_EMPTY_EGR_ENG_0_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MRPS_EGR_ENG_OOP_FIFO_NOT_EMPTY_EGR_ENG_1_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_EMPTY_BUFF_ALLOC_INT, 0, 0, q2a_hard_reset, q2a_special_handling,
                              "Check for misconfiguration in min gap and credits in fqp txi");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_NO_EOB_INT, 0, 0, q2a_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_TOO_MANY_UNITS_INT, 0, 0, q2a_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_FLEXE_FIFO_OVERFLOW, 0, 0, q2a_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ESB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDHA_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDHA_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_QP_ACC_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_QP_DEQ_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_TCG_ACC_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_TCG_DEQ_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_OTM_ACC_HP_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_OTM_DEQ_HP_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_OTM_ACC_LP_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_OTM_DEQ_LP_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_CAL_ACC_HP_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_CAL_DEQ_HP_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_CAL_ACC_LP_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_CAL_DEQ_LP_OVERFLOW, 0, 0, q2a_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_MC_UC_WFQ_ACC_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_MC_UC_WFQ_DEQ_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_TCG_WFQ_ACC_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_TCG_WFQ_DEQ_OVERFLOW, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_EPS_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_FLP_PACKETS_REJECTED_BY_EGW_INT, 0, 0, q2a_special_handling, NULL,
                              "KBP link is down or busy for some reason");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_FLP_PACKETS_REJECTED_BY_EGW_I_OPCODE, 0, 0, q2a_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_LEL_BURST_FIFO_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_EGRESS_OPPORTUNISTIC_FIFO_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_OPPORTUNISTIC_LOOKUP_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_LEL_ERR_DATA_VALID, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_LEL_CONTROL_FIFO_DROP, 0, 0, q2a_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_LEL_ONE_LEL_TXI_DROP, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_NMG_ERR_BITS_FROM_EGQ_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_NMG_WRONG_EGQ_WORD_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_NMG_SYNC_ETH_0_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_NMG_SYNC_ETH_1_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_TX_FIFO_OVERFLOW_INT, 0, 0, q2a_special_handling, q2a_none,
                              "This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_RX_FIFO_OVERFLOW_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_WRONG_WORD_FROM_PM_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_TX_MISSING_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_TX_DOUBLE_SOB_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_RX_NUM_DROPPED_EOPS_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_RX_NUM_DROPPED_EOPS_75P_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Read the counter indicated by NbiThrownBurstsCounters0 75pInterruptRegister.");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0, 0,
                              q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_0_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx0_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_1_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx1_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_2_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx2_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_3_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx3_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_4_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx4_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_5_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx5_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_6_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx6_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_7_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx7_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_8_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx8_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_9_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx9_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_10_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx10_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_11_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx11_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_12_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx12_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_13_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx13_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_14_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx14_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_15_INT, 0, 0,
                              q2a_special_handling, q2a_none, "Read the counter indicated by rx15_pkt_drop_counter ");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_HEAD_UPDT_IN_LAST_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_HEAD_UPDT_IN_EMPTY_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_DEQ_CMD_TO_EMPTY_ERR_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_ALLOCATED_BDM_BDB_RANGE_ERR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_ALLOCATED_WHEN_BDB_FIFO_EMPTY_ERR, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_TX_FIFO_ERR_TX_BD_FIFO_OVF_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_TX_FIFO_ERR_TX_BD_FIFO_UNF_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_TX_FIFO_ERR_TX_BB_FIFO_OVF_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DQM_TX_FIFO_ERR_TX_BB_FIFO_UNF_INT, 0, 0, q2a_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_TDM_PACKET_SIZE_INT, 0, 0, q2a_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_ALL_DATA_BUFFERS_ALLOCATED_INT, 0, 0, q2a_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_UC_FIFO_FULL_INT, 0, 0, q2a_soft_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_TDM_FIFO_FULL_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_MCH_FIFO_FULL_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_MCL_FIFO_FULL_INT, 0, 0, q2a_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_UC_PKT_PORT_FF, 0, 0, q2a_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_ILLEGAL_PKT_SIZE_INT, 0, 0, q2a_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_RQP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_A_BANK_0_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_A_Bank0_Source");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_A_BANK_1_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_A_Bank1_Source");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_0_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank0_Source and the unexpected source is under EngDb_C_Bank0_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_1_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank1_Source and the unexpected source is under EngDb_C_Bank1_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_2_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank2_Source and the unexpected source is under EngDb_C_Bank2_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_3_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank3_Source and the unexpected source is under EngDb_C_Bank3_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_4_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank4_Source and the unexpected source is under EngDb_C_Bank4_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_5_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank5_Source and the unexpected source is under EngDb_C_Bank5_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_6_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank6_Source and the unexpected source is under EngDb_C_Bank6_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ENG_DB_C_BANK_7_ACCESS_ERR_INT, 0, 0, q2a_special_handling, q2a_none,
                              "Check which source is defined under EngDb_C_Bank7_Source and the unexpected source is under EngDb_C_Bank7_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MCP_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_RESERVED_1, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_UC_PLL_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_CORE_PLL_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_FAB_0_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_FAB_1_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_NIF_0_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_NIF_1_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_BS_PLL_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_TS_PLL_LOCKED_LOST, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_NS_TIMER_ASYNC_FIFO_FULL, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_NTP_TIMER_ASYNC_FIFO_FULL, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_IEEE_1588_TIMER_ASYNC_FIFO_FULL, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECI_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_RX_0_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_RX_0_SECOND_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_TX_0_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_TX_0_SECOND_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_RX_1_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_RX_1_SECOND_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_TX_1_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_CORE_TX_1_SECOND_INT, 0, 0, q2a_none, q2a_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_RX_ELK_OVF_INT, 0, 0, q2a_none, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ISEM_1_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ISEM_2_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ISEM_3_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_LEM_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_IOEM_0_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_IOEM_1_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_MC_ID_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_GLEM_0_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_GLEM_1_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EOEM_0_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EOEM_1_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ESEM_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_1_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_2_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_3_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_4_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_RMEP_EM_CUCKOO_FAIL, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_LEM_EMP_PULSE_SCAN_DONE, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_3_EMP_PULSE_SCAN_DONE, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_EXEM_4_EMP_PULSE_SCAN_DONE, 0, 0, q2a_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ECC_PARITY_ERR_INT, 0, 0, q2a_intr_parity_handler,
                              q2a_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ECC_ECC_1B_ERR_INT, 0, 0, q2a_intr_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_MDB_ECC_ECC_2B_ERR_INT, 0, 0, q2a_intr_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "Check Configuration");

    
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_DROPPED_UC_PD_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ECGM_DROPPED_UC_DB_INT, 0, 0, q2a_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT, 0, 0,
                              q2a_ilkn_link_status_change_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT, 0, 0,
                              q2a_ilkn_link_status_change_handling, q2a_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CDU_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, q2a_pfc_deadlock_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_CLU_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, q2a_pfc_deadlock_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY, 0, 0, q2a_special_handling, q2a_none,
                              "Defective buffer was sent to Quarantine");

    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_PROTECTION_ERROR_1_BIT_ECC, 0, 0, q2a_intr_tcam_ecc_1b_handler,
                              q2a_intr_ecc_1b_recurr, "TCAM ECC 1bit protection error");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_TCAM_TCAM_PROTECTION_ERROR_2_BIT_ECC, 0, 0, q2a_intr_tcam_ecc_2b_handler,
                              q2a_intr_ecc_2b_recurr, "TCAM ECC 2bit protection error");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_IPPD_INVALID_DESTINATION_VALID, 0, 0, q2a_special_handling, NULL,
                              "Invalid destination detected");
    
    dnxc_intr_add_clear_func(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELA_FID_EXCEED_LIMIT,
                             q2a_intr_clear_MactLelaFidExceedLimit);
    dnxc_intr_add_clear_func(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_LELB_FID_EXCEED_LIMIT,
                             q2a_intr_clear_MactLelbFidExceedLimit);
    dnxc_intr_add_clear_func(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_ALLOWED,
                             q2a_intr_clear_MactMngmntReqFidExceedLimitAllowed);
    dnxc_intr_add_clear_func(unit, Q2A_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT,
                             q2a_intr_clear_MactMngmntReqFidExceedLimit);

    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_0_1B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_0_2B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_0_1B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_0_2B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_1_1B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_A_1_2B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_1_1B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
    dnxc_intr_add_handler_ext(unit, Q2A_INT_KAPS_TECC_B_1_2B_ERROR, 0, 0, q2a_intr_kaps_tecc_handler, q2a_none,
                              "Kaps TECC");
}
