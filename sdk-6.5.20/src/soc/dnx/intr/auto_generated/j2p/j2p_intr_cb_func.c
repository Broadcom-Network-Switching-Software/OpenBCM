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
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr.h>

#include <soc/sand/sand_intr_corr_act_func.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR




static int
j2p_intr_decide_action(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    dnxc_int_corr_act_type action = DNXC_INT_CORR_ACT_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "msg");
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    switch (en_interrupt)
    {
        case J2P_INT_OAMP_PENDING_EVENT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO;
            break;

        case J2P_INT_OAMP_STAT_PENDING_EVENT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO;
            break;

        case J2P_INT_DDP_DELETE_BDB_FIFO_FULL:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_FULL;
            break;

        case J2P_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY;
            break;

        case J2P_INT_HBC_ECC_ECC_1B_ERR_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HBC_ECC_1B_FIX;
            break;

        case J2P_INT_HBC_ECC_ECC_2B_ERR_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_HBC_ECC_2B_FIX;
            break;

        case J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
        case J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_PFC_DEADLOCK_BREAKING_MECHANISM;
            break;

        case J2P_INT_ETPPB_BTC_HEADER_SIZE_EXCEED:
        case J2P_INT_ETPPB_BTC_NOF_INSTRUCTIONS_EXCEED:
        case J2P_INT_ETPPB_BTC_HEADER_SIZE_NOT_BYTE_ALLIGNED:
        case J2P_INT_EPNI_ETPP_SOP_DEC_ABOVE_TH_INT:
        case J2P_INT_EPNI_ETPP_EOP_DEC_ABOVE_TH_INT:
        case J2P_INT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH_INT:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_NONE;
            break;
        case J2P_INT_IPPD_INVALID_DESTINATION_VALID:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID;
            break;
        case J2P_INT_MSS_MACSEC_MACSEC_INTERRUPT_REGISTER:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_MACSEC_SA_EXPIRY;
            break;
        default:
            sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "%s", msg);
            action = DNXC_INT_CORR_ACT_NONE;
            break;
    }
    *p_corrective_action = action;
exit:
    SHR_FUNC_EXIT;
}


int
j2p_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc;

    SHR_FUNC_INIT_VARS(unit);

    

    
    rc = sand_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, NULL);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_NONE, NULL,
                                                  NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;;
}

int
j2p_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg)
{
    int rc;

    SHR_FUNC_INIT_VARS(unit);

    

    
    rc = sand_interrupt_print_info(unit, block_instance, en_jer_interrupt, 1, SAND_INT_CORR_ACT_NONE, NULL);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_jer_interrupt, msg, DNXC_INT_CORR_ACT_NONE,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_none(
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
j2p_hard_reset(
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
j2p_reload_board(
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
j2p_null(
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
j2p_special_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    rc = j2p_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, p_corrective_action,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_pfc_deadlock_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    uint8 is_cdu = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    
    rc = j2p_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    switch (en_interrupt)
    {
        case J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
            is_cdu = TRUE;
            break;
        case J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT:
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
j2p_macsec_interrupt_handling(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action = DNXC_INT_CORR_ACT_NONE;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    
    rc = j2p_intr_decide_action(unit, block_instance, en_interrupt, msg, &p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, p_corrective_action,
                                                  NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_reprogram_resource(
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
j2p_retrain_link(
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
j2p_intr_parity_handler(
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
j2p_intr_parity_recurr(
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
j2p_intr_ecc_1b_handler(
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
j2p_intr_ecc_1b_recurr(
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
j2p_intr_ecc_2b_handler(
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
j2p_intr_ecc_2b_recurr(
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
j2p_intr_tcam_ecc_1b_handler(
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
j2p_intr_tcam_ecc_2b_handler(
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

    corrective_action = DNXC_INT_CORR_ACT_TCAM_ECC2BITS_HANDLE_DONE;
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, info_msg, corrective_action,
                                                  NULL, NULL);

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_shutdown_link(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    uint32 rc;
    SHR_FUNC_INIT_VARS(unit);

    

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg,
                                                  DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_soft_init(
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
j2p_scrub_read_write_back(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
j2p_scrub_value_zero(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *msg)
{
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, SAND_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

void
j2p_interrupt_cb_init(
    int unit)
{
    
    
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OLP_ERROR_EGRESS_PIPE_CFG, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OLP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OLP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_AUTO_DOC_NAME_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_PRIO_0_DROP_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_PRIO_1_DROP_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_PRIO_2_DROP_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_PRIO_3_DROP_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_0_MESHMC_0_OVF_DROP_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_1_MESHMC_0_OVF_DROP_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_0_MESHMC_1_OVF_DROP_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_1_MESHMC_1_OVF_DROP_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_0_TDM_OVF_DROP, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_EGQ_1_TDM_OVF_DROP, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_OFM_MEM_CONTENTION_INT_CORE_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_OFM_MEM_CONTENTION_INT_CORE_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDA_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_TX_FIFO_OVERFLOW_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_RX_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_WRONG_WORD_FROM_MAC_0_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_TX_MISSING_SOB_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_TX_DOUBLE_SOB_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_RX_NUM_DROPPED_EOPS_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_RX_NUM_DROPPED_EOPS_75P_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_ALIGNER_FIFO_OVF_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_0_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_1_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_2_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_3_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_4_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_5_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_6_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_7_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_8_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_9_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_10_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_11_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_12_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_13_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_14_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_15_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_LMM_OVF_INT, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_HPM_OVF_INT, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_RCYM_OVF_INT, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_HPM_DROP_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_LMM_DROP_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_EPRE_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_EPRE_FIFO_OVERFLOW_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPRE_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ERROR_TIMEOUT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ERROR_REASSEMBLY_CONTEXT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ERROR_BYTE_NUM, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ERROR_PKT_CRC, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ERROR_S_2D_PKT_CRC, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_BUFF, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_NO_BUFF, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_PACKET_REJECT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_0, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_1, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_OVERFLOW_2, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_0, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_1, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2F_RPC_SROT_FIFO_UNDERFLOW_2, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_OVERFLOW_0, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_OVERFLOW_1, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_UNDERFLOW_0, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2D_RPC_SROT_FIFO_UNDERFLOW_1, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_PSF_CTRL_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_PSF_CTRL_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_PSF_DATA_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_PSF_DATA_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_WBR_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_WBR_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2_F_TCF_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2_F_TCF_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2_D_TCF_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_S_2_D_TCF_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_F_REL_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_F_REL_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_F_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_F_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_D_REL_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_D_REL_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_D_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SPB_FIFO_ERROR_BRC_S_2_D_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_DELETE_FIFO_FULL, 0, 0, j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_LCD_FIFO_FULL, 0, 0, j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_TC_MAPPING_MISS_CONFIG, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_MC_MAX_REPLICATION_OVERFLOW_INT, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_EBTR_ILLEGAL_VALUE, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_INVALID_OTM_INT, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_ECGM_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_ECGM_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_DEL_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_DEL_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_DEL_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_DEL_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_DEL_FBC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_DEL_FBC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_DEL_FBC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_DEL_FBC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_LCD_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_LCD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_LCD_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_LCD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_LCD_FBC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_TDM_LCD_FBC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_LCD_FBC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PQP_FIFO_STATUS_NONTDM_LCD_FBC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDHB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDHB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_FPC_0_FREE_ERROR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_FPC_1_FREE_ERROR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_FPC_2_FREE_ERROR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_FPC_3_FREE_ERROR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_BDM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_INTERRUPT_REGISTER_FIELD_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_INTERRUPT_REGISTER_FIELD_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_INTERRUPT_REGISTER_FIELD_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_INTERRUPT_REGISTER_FIELD_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDTL_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_DATA_FIFO_OVERFLOW_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_DATA_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_CTRL_FIFO_OVERFLOW_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_CTRL_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_BYPASS_BTC_READ_WITHOUT_SOP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_APP_AND_PIPE_COLLISION, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_TM_FIELDS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_NETWORK_HEADERS_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_SYSTEM_HEADERS_CONTAINER_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_MPLS_BOS_PARSING_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PSG_HEADER_SIZE_ERR_INT, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_HEADER_SIZE_ERR_INT, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_0, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_1, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_2, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PSG_QUALIFIER_SIZE_ERR_INT_3, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_0, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_1, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_2, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_3, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_4, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_5, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_6, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PES_QUALIFIER_SIZE_ERR_INT_7, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPPA_PP_DSP_FOR_VISIBILITY_AND_IS_TDM_FIFO_ALMOST_FULL_HIGH_INT, 0,
                              0, j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPP_PRP_PRP_ACCEPTABLE_FRAME_TYPE_INT, 0, 0, j2p_special_handling,
                              NULL, "Wrong_Eth_Type");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPP_PRP_PRP_GLEM_ERROR_INT, 0, 0, j2p_special_handling, NULL,
                              "Check_MDB_Err");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPP_PRP_PRP_SYSTEM_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_special_handling, NULL, "HW_Need_To_Check");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPP_PRP_PRP_PORT_ATTRIBUTES_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_special_handling, NULL, "HW_Need_To_Check");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPA_ETPP_PRP_PRP_NETWORK_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_special_handling, NULL, "HW_Need_To_Check");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_INVALID_OTM_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_DSS_STACKING_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_EXCLUDE_SRC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_LAG_MULTICAST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_UNKNOWN_DA_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_SPLIT_HORIZON_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_GLEM_PP_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_GLEM_NON_PP_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_TTL_SCOPE_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_MTU_VIOLATION_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_TTL_EQUALS_ONE_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_OPTIONS_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_TTL_EQUALS_ZERO_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_LOOPBACK_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_HOP_BY_HOP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_LINK_LOCAL_DST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_IPV6_SITE_LOCAL_DST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_APP_AND_PIPE_COLLISION, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TDM_WRONG_PORT_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TCP_SEQUENCE_NUMBER_AND_FLAGS_ARE_ZERO_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_ERPP_ERPP_DISCARD_2_TCP_SEQUENCE_NUMBER_IS_ZERO_AND_FIN_OR_URG_OR_PSH_IS_SET_INT,
                              0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TCP_SYN_AND_FIN_ARE_SET_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TCP_SRC_PORT_EQUALS_DST_PORT_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TCP_FRAGMENT_WITH_INCOMPLETE_TCP_HEADER_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TCP_FRAGMENT_WITH_OFFSET_LESS_THAN_8_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_UDP_SRC_PORT_EQUALS_DST_PORT_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TM_FIELDS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_NETWORK_HEADERS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_SYSTEM_HEADERS_CONTAINER_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_ERPP_ERPP_DISCARD_2_PP_DSP_FOR_VISIBILITY_AND_IS_TDM_FIFO_ALMOST_FULL_HIGH_INT, 0,
                              0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_MPLS_BOS_PARSING_DATA_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PSG_HEADER_SIZE_ERR_INT, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_HEADER_SIZE_ERR_INT, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_TM_DATA_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_GLEM_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PRP_PER_PORT_INFO_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_NETWORK_HEADER_FIFO_ALMOST_FULL_LOW_INT, 0,
                              0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PRP_WAIT_FOR_NETWORK_HEADER_FIFO_ALMOST_FULL_HIGH_INT,
                              0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PRP_SAVE_NETWORK_HEADER_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PMF_INITIAL_ACTIONS_FIFO_ALMOST_FULL_INT, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_0, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_1, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_2, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PSG_QUALIFIER_SIZE_ERR_INT_3, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_0, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_1, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_2, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_3, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_4, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_5, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_6, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_PES_QUALIFIER_SIZE_ERR_INT_7, 0, 0,
                              j2p_reprogram_resource, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ERPP_ERPP_DISCARD_2_DISCARD_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_OVERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_UNDERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_0, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_1, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_MISCONFIG_2, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RRF_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_CRDT_OVERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_UNDERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_RDF_SRAM_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_DQCF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_DQCF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_DQCF_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_D_BLF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_D_BLF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_PBF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_PBF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_PBF_MISCONFIG, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_PBF_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_DQCF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_DQCF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_DQCF_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_D_BLF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_D_BLF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_D_BLF_MISCONFIG, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_PBF_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_PBF_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_PBF_MISCONFIG, 0, 0, j2p_none, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_ERROR_S_2D_PBF_OVERSIZE, 0, 0, j2p_special_handling, j2p_none,
                              "Configuration error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_COMP_CNT_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_COMP_CNT_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_SCS_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_OVERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_OVERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_OVERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_OVERFLOW_3, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_UNDERFLOW_0, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_UNDERFLOW_1, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_UNDERFLOW_2, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_EOB_FIFO_UNDERFLOW_3, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_DRAM_FIFO_OVERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_FIFO_RDF_DRAM_FIFO_UNDERFLOW, 0, 0, j2p_reload_board, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED, 0, 0, j2p_special_handling,
                              j2p_none, "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_FTMH_IS_NOT_STAMPPED, 0, 0, j2p_special_handling, j2p_none,
                              "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX, 0, 0, j2p_special_handling, j2p_none,
                              "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE, 0, 0, j2p_special_handling,
                              j2p_none, "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_FTMH_PSIZE_MISMATCH, 0, 0, j2p_special_handling, j2p_none,
                              "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_PSIZE_MISMATCH, 0, 0, j2p_special_handling, j2p_none,
                              "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_EXPECTED_ITPP_DELTA_MISMATCH, 0, 0, j2p_special_handling,
                              j2p_none, "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_ERR_NEGATIVE_DELTA, 0, 0, j2p_special_handling, j2p_none,
                              "Check for ITPP errors");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_BYPASS_FIFO_OVERFLOW, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_BYPASS_FIFO_UNDERFLOW, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_WDF_OVERFLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_WDF_UNDERFLOW, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_WDF_CPYDAT_OVERFLOW, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPT_ITE_FIFO_WDF_CPYDAT_UNDERFLOW, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_EVENTOR_EVENTOR_INTERRUPT_BIT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_EVENTOR_EVENTOR_RX_CMIC_ERROR_INTERRUPT_BIT, 0, 0,
                              j2p_reprogram_resource, NULL, "cmic descriptor or source data should be fixed");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_EVENTOR_EVENTOR_TX_CMIC_ERROR_INTERRUPT_BIT, 0, 0,
                              j2p_reprogram_resource, NULL, "cmic descriptor or source data should be fixed");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EVNT_EVENTOR_EVENTOR_TX_BINNING_WRP_INTERRUPT_BIT, 0, 0,
                              j2p_scrub_value_zero, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_PROTECTION_ERROR_1_BIT_ECC, 0, 0, j2p_reprogram_resource,
                              j2p_none,
                              "One bit ECC corruption was detected and HW is taking actions to correct it. Please check the TcamProtectionError register for the Address and Bank in which the corruption was found by the protection Operation.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_PROTECTION_ERROR_2_BIT_ECC, 0, 0, j2p_reprogram_resource,
                              j2p_none,
                              "Two bit ECC corruption was detected. HW will not invalidate the corrupted entry. It will raise an interupt and will wait on the user to handle the 2bit ECC as desired. When SW is done the TcamProtection2bitEccHandlingDone register needs to be reset so HW can continue with the Protection scan. Please check TcamProtectionError register for the Address and Bank in which the corruption was found by the Protection Operation.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_QUERY_FAILURE_VALID, 0, 0, j2p_special_handling, j2p_none,
                              "Configuration needs to be fixed. Additional information on the reason this interrupt was raised can be found at the TcamQueryFailure register. Interrupt handler issued a log message.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_MOVE_ECC_VALID, 0, 0, j2p_special_handling, j2p_none,
                              "Corrupted Tcam Bank entry was discovered during TCM Move operation. Additional information on The Bank and line on which the issue was encountered can be found at the TcamMoveEccFailure register. Interrupt handler issued a log message.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_4, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_5, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_6, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_7, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_8, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_9, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_10, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_10, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPUDATACELLFNE_MAC_11, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_CPU_DATA_CELL_FIFO_OFLW_INT_MAC_11, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_FDR_MAC_0_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_FDR_MAC_0_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_FDR_MAC_0_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_IFMFO_MAC_0, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_ALTO_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_ERR_FILTER_DROP_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_DROP_FILTER_DROP_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_0_P_1_DROP_LOW_PRIO_MC_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_FDR_MAC_0_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_FDR_MAC_0_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_FDR_MAC_0_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_IFMFO_MAC_0, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_ALTO_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_ERR_FILTER_DROP_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_DROP_FILTER_DROP_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_0_P_2_DROP_LOW_PRIO_MC_INT_MAC_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_FDR_MAC_1_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_FDR_MAC_1_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_FDR_MAC_1_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_IFMFO_MAC_1, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_ALTO_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_ERR_FILTER_DROP_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_DROP_FILTER_DROP_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_1_P_1_DROP_LOW_PRIO_MC_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_FDR_MAC_1_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_FDR_MAC_1_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_FDR_MAC_1_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_IFMFO_MAC_1, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_ALTO_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_ERR_FILTER_DROP_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_DROP_FILTER_DROP_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_1_P_2_DROP_LOW_PRIO_MC_INT_MAC_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_FDR_MAC_2_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_FDR_MAC_2_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_FDR_MAC_2_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_IFMFO_MAC_2, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_ALTO_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_ERR_FILTER_DROP_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_DROP_FILTER_DROP_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_2_P_1_DROP_LOW_PRIO_MC_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_FDR_MAC_2_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_FDR_MAC_2_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_FDR_MAC_2_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_IFMFO_MAC_2, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_ALTO_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_ERR_FILTER_DROP_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_DROP_FILTER_DROP_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_2_P_2_DROP_LOW_PRIO_MC_INT_MAC_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_FDR_MAC_3_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_FDR_MAC_3_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_FDR_MAC_3_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_IFMFO_MAC_3, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_ALTO_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_ERR_FILTER_DROP_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_DROP_FILTER_DROP_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_3_P_1_DROP_LOW_PRIO_MC_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_FDR_MAC_3_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_FDR_MAC_3_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_FDR_MAC_3_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_IFMFO_MAC_3, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_ALTO_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_ERR_FILTER_DROP_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_DROP_FILTER_DROP_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_3_P_2_DROP_LOW_PRIO_MC_INT_MAC_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_FDR_MAC_4_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_FDR_MAC_4_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_FDR_MAC_4_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_IFMFO_MAC_4, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_ALTO_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_ERR_FILTER_DROP_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_DROP_FILTER_DROP_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_4_P_1_DROP_LOW_PRIO_MC_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_FDR_MAC_4_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_FDR_MAC_4_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_FDR_MAC_4_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_IFMFO_MAC_4, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_ALTO_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_ERR_FILTER_DROP_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_DROP_FILTER_DROP_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_4_P_2_DROP_LOW_PRIO_MC_INT_MAC_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_FDR_MAC_5_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_FDR_MAC_5_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_FDR_MAC_5_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_IFMFO_MAC_5, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_ALTO_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_ERR_FILTER_DROP_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_DROP_FILTER_DROP_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_5_P_1_DROP_LOW_PRIO_MC_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_FDR_MAC_5_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_FDR_MAC_5_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_FDR_MAC_5_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_IFMFO_MAC_5, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_ALTO_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_ERR_FILTER_DROP_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_DROP_FILTER_DROP_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_5_P_2_DROP_LOW_PRIO_MC_INT_MAC_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_FDR_MAC_6_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_FDR_MAC_6_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_FDR_MAC_6_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_IFMFO_MAC_6, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_ALTO_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_ERR_FILTER_DROP_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_DROP_FILTER_DROP_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_6_P_1_DROP_LOW_PRIO_MC_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_FDR_MAC_6_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_FDR_MAC_6_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_FDR_MAC_6_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_IFMFO_MAC_6, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_ALTO_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_ERR_FILTER_DROP_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_DROP_FILTER_DROP_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_6_P_2_DROP_LOW_PRIO_MC_INT_MAC_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_FDR_MAC_7_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_FDR_MAC_7_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_FDR_MAC_7_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_IFMFO_MAC_7, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_ALTO_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_ERR_FILTER_DROP_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_DROP_FILTER_DROP_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_7_P_1_DROP_LOW_PRIO_MC_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_FDR_MAC_7_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_FDR_MAC_7_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_FDR_MAC_7_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_IFMFO_MAC_7, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_ALTO_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_ERR_FILTER_DROP_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_DROP_FILTER_DROP_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_7_P_2_DROP_LOW_PRIO_MC_INT_MAC_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_FDR_MAC_8_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_FDR_MAC_8_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_FDR_MAC_8_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_IFMFO_MAC_8, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_ALTO_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_ERR_FILTER_DROP_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_DROP_FILTER_DROP_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_8_P_1_DROP_LOW_PRIO_MC_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_FDR_MAC_8_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_FDR_MAC_8_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_FDR_MAC_8_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_IFMFO_MAC_8, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_ALTO_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_ERR_FILTER_DROP_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_DROP_FILTER_DROP_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_8_P_2_DROP_LOW_PRIO_MC_INT_MAC_8, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_FDR_MAC_9_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_FDR_MAC_9_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_FDR_MAC_9_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_IFMFO_MAC_9, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_ALTO_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_ERR_FILTER_DROP_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_DROP_FILTER_DROP_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_9_P_1_DROP_LOW_PRIO_MC_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_FDR_MAC_9_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_FDR_MAC_9_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_FDR_MAC_9_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_IFMFO_MAC_9, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_ALTO_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_ERR_FILTER_DROP_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_DROP_FILTER_DROP_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_9_P_2_DROP_LOW_PRIO_MC_INT_MAC_9, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_FDR_MAC_10_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_FDR_MAC_10_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_FDR_MAC_10_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_IFMFO_MAC_10, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_ALTO_MAC_10, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_ERR_FILTER_DROP_INT_MAC_10, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_DROP_FILTER_DROP_INT_MAC_10, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_10_P_1_DROP_LOW_PRIO_MC_INT_MAC_10, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_FDR_MAC_10_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_FDR_MAC_10_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_FDR_MAC_10_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_IFMFO_MAC_10, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_ALTO_MAC_10, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_ERR_FILTER_DROP_INT_MAC_10, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_DROP_FILTER_DROP_INT_MAC_10, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_10_P_2_DROP_LOW_PRIO_MC_INT_MAC_10, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_FDR_MAC_11_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_FDR_MAC_11_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_FDR_MAC_11_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_IFMFO_MAC_11, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_ALTO_MAC_11, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_ERR_FILTER_DROP_INT_MAC_11, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_DROP_FILTER_DROP_INT_MAC_11, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_1_MAC_11_P_1_DROP_LOW_PRIO_MC_INT_MAC_11, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_FDR_MAC_11_UN_EXP_CELL_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_FDR_MAC_11_UN_EXP_CELL_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_FDR_MAC_11_UN_EXP_CELL_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_IFMFO_MAC_11, 0, 0, j2p_special_handling, j2p_none,
                              "Check LLFC thresholds");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_ALTO_MAC_11, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_ERR_FILTER_DROP_INT_MAC_11, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_DROP_FILTER_DROP_INT_MAC_11, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_P_2_MAC_11_P_2_DROP_LOW_PRIO_MC_INT_MAC_11, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_IN_BAND_FIFO_FULL, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Reduce inband cells rate in FDT.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_1_OFM_FIFO_TRAFFIC_CLASS_DROP_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_2_OFM_FIFO_TRAFFIC_CLASS_DROP_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_1_OFM_FIFO_OVFLW_DROP_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check LLFC thresholds.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_2_OFM_FIFO_OVFLW_DROP_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check LLFC thresholds.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_1_FDR_OUT_UN_EXP_CELL, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDR_FDR_GENERAL_P_2_FDR_OUT_UN_EXP_CELL, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_0_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_0_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_0_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_0_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_2, 0, 0, j2p_special_handling, j2p_none,
                              "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_3, 0, 0, j2p_special_handling, j2p_none,
                              "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_2_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_2_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_2_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_2_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_3_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_3_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_3_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_3_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_4_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_4_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_4_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_4_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_5_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_5_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_5_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_5_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_6_0, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_6_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_6_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_AUTO_DOC_NAME_6_3, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low    rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low    rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low    rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low    rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 10000000, 1, j2p_none, j2p_shutdown_link,
                              "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_LOS_INT_0, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_LOS_INT_1, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_LOS_INT_2, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_LOS_INT_3, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_OOF_INT_0, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_OOF_INT_1, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_OOF_INT_2, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_OOF_INT_3, 0, 0, j2p_special_handling, j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_5_CORRECTABLE_ERROR_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_5_CORRECTABLE_ERROR_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_5_CORRECTABLE_ERROR_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_5_CORRECTABLE_ERROR_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_2, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_3, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_2, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_3, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_2, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_3, 0, 0, j2p_special_handling, j2p_none,
                              "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_0, 0, 0, j2p_none, j2p_none,
                              "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_1, 0, 0, j2p_none, j2p_none,
                              "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_2, 0, 0, j2p_none, j2p_none,
                              "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_3, 0, 0, j2p_none, j2p_none,
                              "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_OVERFLOW_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_OVERFLOW_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_OVERFLOW_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_OVERFLOW_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_UNDERRUN_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_UNDERRUN_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_UNDERRUN_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_PAYLOAD_FIFO_UNDERRUN_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_OVERFLOW_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_OVERFLOW_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_OVERFLOW_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_OVERFLOW_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_UNDERRUN_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_UNDERRUN_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_UNDERRUN_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_HEADER_FIFO_UNDERRUN_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_OVERFLOW_INT_0, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_OVERFLOW_INT_1, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_OVERFLOW_INT_2, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_OVERFLOW_INT_3, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_UNDERRUN_INT_0, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_UNDERRUN_INT_1, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_UNDERRUN_INT_2, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_SIZE_PREFETCH_FIFO_UNDERRUN_INT_3, 0, 0,
                              j2p_special_handling, j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_OVERFLOW_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_OVERFLOW_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_OVERFLOW_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_OVERFLOW_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_UNDERRUN_INT_0, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_UNDERRUN_INT_1, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_UNDERRUN_INT_2, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FMAC_FMAC_9_TX_CTRL_FIFO_UNDERRUN_INT_3, 0, 0, j2p_special_handling,
                              j2p_none, "fmal link reset.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_0_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_1_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_2_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_3_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ERR_ITPP_PSIZE_TYPE_4_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPPD_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_COHERENCY_WRITE_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_COHERENCY_WRITE_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_COHERENCY_READ_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_COHERENCY_READ_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_WRITE_REQUEST_RXI_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_READ_REQUEST_RXI_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_READ_DATA_TXI_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_CHANNEL_ORDER_FIFO_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_CHANNEL_ORDER_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TDU_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_UNRCH_DEST_INT_0, 0, 0, j2p_special_handling, j2p_none,
                              "Check if there is any link error that cause some other device to be disconnected from the fabric. Otherwise, check that destination port is mapped correctly to the destination ID.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_UNRCH_DEST_INT_1, 0, 0, j2p_special_handling, j2p_none,
                              "Check if there is any link error that cause some other device to be disconnected from the fabric. Otherwise, check that destination port is mapped correctly to the destination ID.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_ILEGAL_IRE_PACKET_SIZE_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check IRE filters. Those type of packets should not be arriving to FDT.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_AUTO_DOC_NAME_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_CORE_0_DTQ_OVF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_CORE_1_DTQ_OVF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_CORE_0_DTQ_UNDF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_CORE_1_DTQ_UNDF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_INBAND_ACK_OVF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_FDT_INBAND_ACK_UNDF_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FDT_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CFC_SPI_OOB_RX_FRM_ERR, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CFC_SPI_OOB_RX_DIP_2_ERR, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CFC_SPI_OOB_RX_WD_ERR, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CFC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CFC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPC_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RTP_LINK_MASK_CHANGE, 0, 0, j2p_special_handling, j2p_none,
                              "Check RMGR / RTPWP settings in both device and device partner. If configuration is OK - look for physical link error indication and retrain link if needed.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RTP_TABLE_CHANGE, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RTP_DISCONNECT_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RTP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RTP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPF_VTC_OUTLIF_OVERLOAD, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPF_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPF_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPF_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_HEAD_UPDT_IN_LAST_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_HEAD_UPDT_IN_EMPTY_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_DEQ_CMD_TO_EMPTY_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_QDM_INTERNAL_CACHE_ERROR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_ALLOCATED_PDM_PDB_RANGE_ERR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_ALLOCATED_WHEN_PDB_FIFO_EMPTY_ERR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_RELEASED_WHEN_PDB_FIFO_FULL_ERR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_PD_FIFO_OVF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_PD_FIFO_UNF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_BUNDLE_FIFO_OVF_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_DRAM_TX_BUNDLE_FIFO_UNF_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_PD_FIFO_OVF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_PD_FIFO_UNF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_BUNDLE_FIFO_OVF_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_TX_FIFO_ERR_TO_FABRIC_TX_BUNDLE_FIFO_UNF_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_2_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_2_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_3_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_3_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_4_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_4_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_5_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_5_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_6_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_6_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_7_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_7_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_8_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_DEQ_FIFO_8_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_2_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SQM_FIFO_ERR_PDM_FIFO_2_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_0, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_1, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_2, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_3, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_4, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_5, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_6, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_7, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_8, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_9, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_10, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_11, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_12, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_13, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_QUALIFIER_SIZE_ERROR_14, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_APP_AND_PIPE_COLLISION, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPE_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_HBM_ECC_DETECTED_ERROR, 0, 0, j2p_special_handling, j2p_none,
                              "mutlibit error on hbm data - should be handled in ddp (quarantine buffer or hard reset)");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_HBM_ECC_CORRECTED_ERROR, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_WRITE_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_WRITE_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_READ_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_READ_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_WRITE_OUT_SYNCHRONIZER_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_WRITE_OUT_SYNCHRONIZER_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_PASSED_SYNCHRONIZER_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_COHERENCY_PASSED_SYNCHRONIZER_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_BANK_ORDER_FIFO_OVERFLOW_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_BANK_ORDER_FIFO_OVERFLOW_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_BANK_ORDER_FIFO_UNDERFLOW_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_BANK_ORDER_FIFO_UNDERFLOW_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WDS_OVERFLOW, 0, 0, j2p_soft_init, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_REQUEST_ASYNC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_REQUEST_ASYNC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_REQUEST_ASYNC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_REQUEST_ASYNC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_USED_POINTERS_SYNCHRONIZER_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_USED_POINTERS_SYNCHRONIZER_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_USED_POINTERS_PREFETCH_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_USED_POINTERS_PREFETCH_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WDS_OVERFLOW_SYNCHRONIZER_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WDS_OVERFLOW_SYNCHRONIZER_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_CREDIT_SYNCHRONIZER_FIFO_OVERFLOW_0, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_CREDIT_SYNCHRONIZER_FIFO_UNDERFLOW_0, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_CREDIT_SYNCHRONIZER_FIFO_OVERFLOW_1, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_CREDIT_SYNCHRONIZER_FIFO_UNDERFLOW_1, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WDS_WRITE_TO_FULL_ADDR_SYNCHRONIZER_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WDS_WRITE_TO_FULL_ADDR_SYNCHRONIZER_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_CREDIT_SYNCHRONIZER_OVERFLOW_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_CREDIT_SYNCHRONIZER_OVERFLOW_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_CREDIT_SYNCHRONIZER_UNDERFLOW_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_CREDIT_SYNCHRONIZER_UNDERFLOW_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_REQUEST_PREFETCH_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_WRITE_REQUEST_PREFETCH_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_REQUEST_PREFETCH_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_DSI_READ_REQUEST_PREFETCH_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_REASSEMBLY_INFO_FIFO_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_REASSEMBLY_INFO_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_SLICE_INFO_FIFO_OVERFLOW_DS_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_SLICE_INFO_FIFO_OVERFLOW_DS_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_SLICE_INFO_FIFO_UNDERFLOW_DS_1, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_SLICE_INFO_FIFO_UNDERFLOW_DS_0, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_RDS_OVERFLOW, 0, 0, j2p_soft_init, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_PSEUDO_CHANNEL_ORDER_FIFO_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_RDR_PSEUDO_CHANNEL_ORDER_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_0_UI_0_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_0_UI_1_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_1_UI_0_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_1_UI_1_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_2_UI_0_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_2_UI_1_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_3_UI_0_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_3_UI_1_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_0_UI_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_0_UI_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_1_UI_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_1_UI_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_2_UI_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_2_UI_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_3_UI_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_QUEUE_DWORD_3_UI_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_ROW_COMMAND_CONTENTION, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_COLUMN_COMMAND_CONTENTION, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_ECC_INFO_QUEUE_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_ECC_INFO_QUEUE_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_SELECTOR_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_PIPELINES_READ_DATA_SELECTOR_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MSD_MACSEC_MACSEC_INTERRUPT_REGISTER, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_RESERVED_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_USR_CNT_DEC_VAL_ERR_INT, 0, 0, j2p_retrain_link, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_ERR_FLOW_ID_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_ERR_QUEUE_NUMBER_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_DRAM_BOUND_RECOVERY_STATUS_ERR_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VOQ_STATE_INTERNAL_CACHE_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_DEL_CTR_RDY, 0, 0, j2p_special_handling, NULL,
                              "Empty Delete counter FIFO by reading DelCtr register until reply invalid queue. After clear interrupt");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_DEL_CTR_DROP_ERR_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_GRNTD_OC_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_WORDS_SHRD_OC_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_GRNTD_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_BUFFERS_SHRD_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_GRNTD_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_WORDS_SHRD_OC_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_GRNTD_OC_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_VOQ_OC_ERR_VOQ_SRAM_PDS_SHRD_OC_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_WORDS_HDRM_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_0_OC_ERR_INT, 0,
                              0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_BUFFERS_HDRM_EXT_POOL_1_OC_ERR_INT, 0,
                              0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_GRNTD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_SHRD_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_0_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_TOTAL_PB_VSQ_OC_ERR_PB_VSQ_SRAM_PDS_HDRM_EXT_POOL_1_OC_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_WORDS_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_BUFFERS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VOQ_CTR_ERR_VOQ_CTR_ERR_SRAM_PDS_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQA_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQB_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQC_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_QB_VSQ_CTR_ERR_VSQD_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQE_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_WORDS_ERR_INT, 0, 0, j2p_reload_board,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_BUFFERS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_VSQ_CTR_ERR_PB_VSQ_CTR_ERR_VSQF_SRAM_PDS_ERR_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_PG_CONGESTION_FIFO_ORDY, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_LLFC_CONGESTION_FIFO_ORDY, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_VOQ_CONGESTION_FIFO_ORDY, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_GLBL_SRAM_BUFFERS_FC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_GLBL_SRAM_PDBS_FC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_GLBL_DRAM_BDBS_FC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_SRAM_BUFFERS_POOL_0_FC_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_SRAM_BUFFERS_POOL_1_FC_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_SRAM_PDS_POOL_0_FC_INT, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_SRAM_PDS_POOL_1_FC_INT, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_WORDS_POOL_0_FC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_CONGESTION_PB_VSQ_WORDS_POOL_1_FC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_GLBL_SRAM_BUFFERS_FC_LP_INT, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_GLBL_SRAM_PDBS_FC_LP_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_GLBL_DRAM_BDBS_FC_LP_INT, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_BUFFERS_POOL_0_FC_LP_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_BUFFERS_POOL_1_FC_LP_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_PDS_POOL_0_FC_LP_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_SRAM_PDS_POOL_1_FC_LP_INT, 0, 0,
                              j2p_special_handling, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_WORDS_POOL_0_FC_LP_INT, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_LOW_PRIORITY_PB_VSQ_WORDS_POOL_1_FC_LP_INT, 0, 0, j2p_special_handling,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DEQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DEQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DEQ_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DEQ_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_2_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_2_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_3_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_3_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_4_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_4_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_5_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_ENQ_FIFO_5_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_2_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_2_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_3_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_3_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_4_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_4_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_5_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_5_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_6_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_6_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_7_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_7_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_8_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_8_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_9_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_TAR_FIFO_9_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DCTR_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CGM_FIFO_ERR_DCTR_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPPC_BIER_BIT_MASK_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPPC_BIER_BIT_MASK_FIFO_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPPC_ENCAPSULATION_1_DATA_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_TERM_PIPE_FIFO_1_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_VSD_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_ESEM_1_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_ESEM_2_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_EXEM_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_OEM_RESULT_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_TERM_PIPE_FIFO_2_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_CRPS_RESULT_FIFO_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_MRPS_RESULT_FIFO_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_TERM_SPLIT_HORIZON_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_PROTECTION_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_LATENCY_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_ACTION_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPC_ETPP_TERM_PORT_RATE_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDPM_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0, 0,
                              j2p_retrain_link, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_PENDING_EVENT, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_STAT_PENDING_EVENT, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_RX_STATS_DONE, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_RFC_6374_PKT_DROPPED, 0, 0, j2p_special_handling, j2p_none,
                              "RFC6374 was dropped due to ignore configurations");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_TXO_MESSAGE_SENT, 0, 0, j2p_special_handling, j2p_none,
                              "OAMP TXO sent a message");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_TXO_REQ_FIFO_EMPTY, 0, 0, j2p_special_handling, j2p_none,
                              "TCO request FIFO is empty and ready");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_MDB_0_ERR, 0, 0, j2p_special_handling, j2p_none,
                              "RMEP interface 0 returned an error - check MDB memories");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_MDB_1_ERR, 0, 0, j2p_special_handling, j2p_none,
                              "RMEP interface 1 returned an error - check MDB memories");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_RXP_FIFO_WRITE_FULL, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_SAT_TX_PKT_CONTAINER_FIFO_ALMOST_FULL_INT, 0, 0, j2p_special_handling,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OAMP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_TXQ_OVF_INT, 0, 0, j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_TXQ_READ_CONJESTED_INT, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_TXQ_WRITE_CONJESTED_INT, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_DBF_PIPE_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_DBF_PIPE_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_MCDB_PIPE_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_MCDB_PIPE_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_FREE_FBC_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_FREE_FBC_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FQP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_SRC_DV_CNG_LINK_EV, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_CPU_CNT_CELL_FNE, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_LOCAL_ROUT_FS_OVF, 0, 0, j2p_special_handling, j2p_none,
                              "Check status / configuration of SCH, that might lead to backpressure to FCR.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_LOCAL_ROUTRC_OVF, 0, 0, j2p_special_handling, j2p_none,
                              "Check status / configuration of IPS, that might lead to backpressure to FCR.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_REACH_FIFO_OVF, 0, 0, j2p_special_handling, j2p_none,
                              "Check RMGR settings in device partner.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_FLOW_FIFO_OVF, 0, 0, j2p_special_handling, j2p_none,
                              "Check status / configuration of SCH, that might lead to backpressure to FCR.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_CREDIT_FIFO_OVF, 0, 0, j2p_special_handling, j2p_none,
                              "Check status / configuration of IPS, that might lead to backpressure to FCR.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_AUTO_DOC_NAME_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_IPS_0_FIFO_OVF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_IPS_0_FIFO_UNDER_FLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_IPS_1_FIFO_OVF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_IPS_1_FIFO_UNDER_FLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_SCH_FIFO_OVF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_SCH_FIFO_UNDER_FLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCR_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_RES_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_ACT_FLOW_BAD_PARAMS, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SHP_FLOW_BAD_PARAMS, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_RESTART_FLOW_EVENT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SMP_THROW_SCL_MSG, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SMP_FULL_LEVEL_1, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SMP_FULL_LEVEL_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_AGGR_FIFO_AF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_DVS_FIFO_AF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_UP_CREDIT_FIFO_AF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_RETURNED_CREDIT_FIFO_AF, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_MCAST_CREDIT_FIFO_AF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_ING_SHAPE_CREDIT_FIFO_AF, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_FCT_FIFO_OVF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SCL_GROUP_CHANGED, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_RES_2, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SMP_BAD_MSG, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_SMP_FABRIC_MSGS_FIFO_FULL, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_FIFO_OVERFLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_FIFO_UNDERFLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SCH_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_PORT_MTU_TRAP_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_STP_TRAP_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_MEMBERSHIP_TRAP_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_TRAP_ORIGINAL_FTMH_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_TRAP_BYPASS_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_ENCAPSULATION_2_DATA_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_ENCAPSULATION_3_DATA_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_ENCAPSULATION_4_DATA_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_ETPPB_ENCAPSULATION_5_DATA_FIFO_ALMOST_FULL_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_BTC_HEADER_SIZE_EXCEED, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_BTC_NOF_INSTRUCTIONS_EXCEED, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_BTC_HEADER_SIZE_NOT_BYTE_ALLIGNED, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ETPPB_BTC_POP_BEFORE_FIFO_READY, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_0_ERR_MAX_REPLICATION, 0, 0, j2p_none, j2p_none,
                              "Ing0_ErrMaxReplicationMcid. Error configuration would lead to more replications than expected.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_0_ERR_REPLICATION_EMPTY, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Ing0_ErrReplicationEmptyMcid. Error configuration would lead to empty replication");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_1_ERR_MAX_REPLICATION, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Ing1_ErrMaxReplicationMcid. Error configuration would lead to more replications than expected.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_1_ERR_REPLICATION_EMPTY, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Ing1_ErrReplicationEmptyMcid. Error configuration would lead to empty replication");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_0_ERR_MAX_REPLICATION, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Egr0_ErrMaxReplicationMcid. Error configuration would lead to more replications than expected.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_0_ERR_REPLICATION_EMPTY, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Egr0_ErrReplicationEmptyMcid. Need to config the MCDB again for the Egress replications");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_1_ERR_MAX_REPLICATION, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Egr1_ErrMaxReplicationMcid. Error configuration would lead to more replications than expected.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_1_ERR_REPLICATION_EMPTY, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Egr1_ErrReplicationEmptyMcid. Need to config the MCDB again for the Egress replications");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_0_BMP_DIRECT_ACCESS, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Error configuration would lead to Error in the bitmap access");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ING_1_BMP_DIRECT_ACCESS, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Error configuration would lead to Error in the bitmap access");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_0_BMP_DIRECT_ACCESS, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Error configuration would lead to Error in the bitmap access.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_EGR_1_BMP_DIRECT_ACCESS, 0, 0, j2p_reprogram_resource, j2p_none,
                              "Error configuration would lead to Error in the bitmap access.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_ING_0_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_ING_0_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_ING_1_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_ING_1_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EGR_0_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EGR_0_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EGR_1_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EGR_1_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EQP_0_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EQP_0_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EQP_1_RADDR_REQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_EQP_1_RADDR_REQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_DATA_FIFO_A_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_DATA_FIFO_A_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_ORDY_FIFO_A_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_ORDY_FIFO_A_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_DATA_FIFO_B_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_DATA_FIFO_B_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_ORDY_FIFO_B_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_READ_ORDY_FIFO_B_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_ING_0_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_ING_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_ING_1_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_ING_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EGR_0_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EGR_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EGR_1_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EGR_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EQP_0_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EQP_0_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EQP_1_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_NUM_FIFO_EQP_1_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_SRC_FIFO_A_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_SRC_FIFO_A_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_SRC_FIFO_B_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_0_REQ_BNK_SRC_FIFO_B_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_MCDB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_MCDB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_SORTER_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_UC_SORTER_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCH_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_MCL_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_PEND_RD_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_0_PEND_RD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_MCDB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_MCDB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_SORTER_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_UC_SORTER_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCH_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_MCL_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_PEND_RD_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_1_EGR_1_PEND_RD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCH_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_MCL_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_PEND_RD_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_0_PEND_RD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCH_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_PEND_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_PEND_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_REP_DESC_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_REP_DESC_FIFO_UNDERFLOW, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_REP_ARB_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_MCL_REP_ARB_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_PEND_RD_FIFO_OVERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MTM_MTM_FIFO_STATUS_2_ING_1_PEND_RD_FIFO_UNDERFLOW, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_PDQ_SEQ_NUM_INTERNAL_CACHE_ERROR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_QM_RPRT_FIFO_OVF, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_BTX_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_BTX_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_BTX_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_BTX_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_QMRP_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_QMRP_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_QSTATE_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IQM_FIFO_ERR_QSTATE_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_4, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_5, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_6, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_7, 0, 0, j2p_special_handling,
                              j2p_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_0, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_1, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_2, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_3, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_4, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_5, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_6, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_7, 0, 0, j2p_special_handling, j2p_none,
                              "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_6, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_7, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FSRD_QUAD_0_FSRD_N_PLL_0_LOCK_CHANGED, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check MAS PLL settings, Blackhawk PLL settings, and reference clock");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ERROR_UNEXPECTED_SOP, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ERROR_UNEXPECTED_MOP, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ERROR_BAD_REASSEMBLY_CONTEXT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ERROR_REASSEMBLY_CONTEXT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_TDM_CTXT_MAP_INVALID_RD_ADDR, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_TDM_DROP, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_TDM_ERR, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_TDM_SIZE_ERR, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_PACKET_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYH_ERR_SOP_WORD_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_PACKET_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_RCY_INTERFACE_ERROR_RCYL_ERR_SOP_WORD_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CRPS_ERR_DATA_ARRIVED, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CRPS_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CRPS_ERR_MISSING_EOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CRPS_ERR_MISSING_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_MISSING_EOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_SAT_ERR_MISSING_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_MISSING_EOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_CPU_ERR_MISSING_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_MISSING_EOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OLP_ERR_MISSING_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_DATA_ARRIVED, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_MISSING_EOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_OAMP_ERR_MISSING_SOP, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_DATA_ARRIVED, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_BCOUNT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_MISSING_EOP, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_EVENTOR_ERR_MISSING_SOP, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_REGI_ERR_DATA_ARRIVED, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_INTERNAL_INTERFACE_ERROR_REGI_ERR_PACKET_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_NIF_ERROR_NIF_ERR_SOP_LESS_THAN_384_BYTES, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_NIF_ERROR_NIF_ERR_DATA_ARRIVED, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_NIF_ERROR_NIF_ERR_PACKET_SIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_ACF_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_ACF_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_HPF_DATA_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_HPF_DATA_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_PEF_CTRL_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_PEF_CTRL_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_PEF_DATA_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_PEF_DATA_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_CTRL_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_CTRL_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_DATA_0_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_DATA_0_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_DATA_1_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IRE_FIFO_ERROR_WDF_DATA_1_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_A_0_1B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_A_0_2B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_B_0_1B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_B_0_2B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_A_1_1B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_A_1_2B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_B_1_1B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_TECC_B_1_2B_ERROR, 0, 0, j2p_scrub_read_write_back, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_KAPS_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_SQM_DQCQ_ERR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_DQM_DQCQ_ERR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_QUEUE_ENTERED_DEL, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_CRDT_LOST, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_CRBAL_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_PUSH_QUEUE_ACTIVE, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_LOST_FSM_EVENT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_CRDT_FLOW_ID_ERR_INT, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_QDESC_0_INTERNAL_CACHE_ERROR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_QDESC_1_INTERNAL_CACHE_ERROR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_SQM_DQCQ_INTERNAL_CACHE_ERROR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_A_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_B_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_C_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_SQM_ACTIVE_QUEUE_COUNT_D_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_A_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_B_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_C_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_OVF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_ACTIVE_QUEUE_COUNT_ERR_DQM_ACTIVE_QUEUE_COUNT_D_UNF_INT, 0, 0,
                              j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_CRDT_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_CRDT_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_CRDT_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_CRDT_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_FMS_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_FMS_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_FSMRQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPS_FIFO_ERR_FSMRQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_ILLEGAL_CONSTRUCTED_HEADER_SIZE, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_INVALID_DESTINATION_VALID, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_PHYSICAL_ERR_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX_0, 0, 0, j2p_null, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX_0, 0, 0, j2p_null, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FLP_FIFO_FULL_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FLP_LOOKUP_TIMEOUT_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_RX_FIFO_OVF_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_OVF_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_BAD_CHANNEL_NUM_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_OVF_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_BAD_LKP_TYPE_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_TX_FIFO_FULL_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_RX_BROKEN_RECORD_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_VALID_0, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_ERR_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_VALID_0, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_ERR_0, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_PHYSICAL_ERR_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX_1, 0, 0, j2p_null, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX_1, 0, 0, j2p_null, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FLP_FIFO_FULL_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FLP_LOOKUP_TIMEOUT_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_RX_FIFO_OVF_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_OVF_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_BAD_CHANNEL_NUM_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_OVF_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_BAD_LKP_TYPE_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_NIF_TX_FIFO_FULL_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_RX_BROKEN_RECORD_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_VALID_1, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_INFO_REPLY_ERR_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_VALID_1, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_CPU_LOOKUP_REPLY_ERR_1, 0, 0, j2p_null, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SIP_TRANSPLANT_DETECTION_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_MC_EXPLICIT_RPF_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_MC_SIP_BASED_RPF_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_UC_LOOSE_RPF_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_UC_STRICT_RPF_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_SAME_INTERACE_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FACILITY_INVALID_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_OUTLIF_OVER_FLOW_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_PACKET_IS_APPLET, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_PARSING_START_OFFSET_OVF, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_FTMH_BIER_STRING_OFFSET_OVF, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_IEEE_1588_OFFSET_OVF, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_OAM_OFFSET_OVF, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EDB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EDB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_0, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_1, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_2, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_3, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_4, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_BILLING_RX_FIFO_ERR_ST_BILL_FIFO_OVF_INT_5, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_0, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_1, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_2, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_RPRT_QSIZE_RX_FIFO_ERR_ST_QSIZE_FIFO_OVF_INT_3, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_0, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_1, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_3, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_4, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_SIF_ST_BILL_OPCODE_ERR_ST_BILL_OPCODE_INT_5, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PDM_FREE_ERROR, 0, 0, j2p_reload_board, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PDM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PDM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_DROPPED_UC_PD_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_DROPPED_UC_DB_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0, 0,
                              j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0, 0, j2p_soft_init,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_ERR_LEARN_REQUEST_OVER_STRONGER, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_WARNING_LEARN_OVER_EXISTING, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_ERR_DELETE_NON_EXIST, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_ERR_TRANSPLANT_REQUEST_OVER_STRONGER, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_ERR_REFRESH_REQUEST_OVER_STRONGER, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_WARNING_TRANSPLANT_NON_EXIST, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_WARNING_REFRESH_NON_EXIST, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_ALLOWED, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_EVENT_READY, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_REPLY_READY, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_REPLY_FIFO_REPLY_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_FID_COUNTER_OVERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MNGMNT_REQ_LARGE_EM_DB_EXCEED_LIMIT_ALLOWED, 0, 0,
                              j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_EMP_NON_LARGE_EM_EVENT_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_LELA_FID_EXCEED_LIMIT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_LELA_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_LELB_FID_EXCEED_LIMIT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_LELB_LARGE_EM_DB_EXCEED_LIMIT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MRQ_FIFO_STATUS_OVERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_MRQ_FIFO_STATUS_UNDERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_EVENT_FIFO_STATUS_OVERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_EVENT_FIFO_STATUS_UNDERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_REPLY_FIFO_STATUS_OVERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_LARGE_EM_LARGE_EM_REPLY_FIFO_STATUS_UNDERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MACT_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_SAT_OVF_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in min gap in esb");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_OLP_OVF_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in min gap in esb");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_OAM_OVF_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in min gap in esb");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_EVENTOR_OVF_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in min gap in esb");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_CPU_OVF_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in min gap in esb");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_INC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_SOP_DEC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_EOP_DEC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TOTAL_INC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_SOP_INC_DELTA_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TOTAL_INC_ABOVE_TH_WITH_EBD_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TOTAL_DEC_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TAIL_ABOVE_INC_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_EBD_ERR_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TOTAL_DEC_ABOVE_PKT_SIZE_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_CROP_TWO_NULL_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_PKT_INC_ABOVE_MAX_PKT_SIZE_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_ILLEGAL_CROP_TYPE_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_MIRR_ADDITIONAL_INFO_ABOVE_TH_INT, 0, 0, j2p_special_handling,
                              NULL, "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ETPP_TAIL_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ALIGNER_TRANSMIT_SIZE_ABOVE_TH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_PKT_SIZE_EOP_MISMATCH_INT, 0, 0, j2p_special_handling, NULL,
                              "Check for misconfiguration in ETPP");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_FIFO_UNDERFLOW_INT, 0, 0, j2p_special_handling, NULL,
                              "Check which fifo underflowed in FifoUnderflowStatus ");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_FIFO_OVERFLOW_INT, 0, 0, j2p_special_handling, NULL,
                              "Check which fifo overflowed in FifoOverflowStatus ");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPNI_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_PACKET_OVERSIZE, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_DRAM_BUNDLE_OVERSIZE, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_SRAM_16B_CNT_OVRF, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_OMAC_CNT_OVRF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_UNPACK_PACKET_SIZE_ERROR, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_UNPACK_PACKET_SIZE_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ERROR_UNPACK_BUNDLE_OVERFLOW_ERROR, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_DELETE_BDB_FIFO_FULL, 0, 0, j2p_special_handling, j2p_none,
                              "Clear the deleted bdbs fifo by reading DeleteBdbFifoHead");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_1B_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_2B_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_CRC_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_PKUP_PACKET_CRC_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_BTC_TDU_ECC_1B_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_BTC_TDU_ECC_2B_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_EXT_MEM_ERR_PKUP_LAST_BUFF_CRC_ERR_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_CFIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_CFIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKP_OUT_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_PTR_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_PTR_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_WAIT_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_WAIT_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BDBC_RESULT_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BDBC_RESULT_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BDBC_SUB_REQ_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BDBC_SUB_REQ_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_ENQ_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_BEC_ENQ_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_DBDF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_DBDF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKP_RX_CFIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKP_RX_CFIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKP_RX_DFIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKP_RX_DFIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_WCF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_WCF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_BDBC_PEND_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_BDBC_PEND_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_END_OF_BUFFER_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_END_OF_BUFFER_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_QUARANTINE_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_QUARANTINE_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_DFIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_DFIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_METADATA_FIF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_FIFO_PKUP_RX_METADATA_FIF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED, 0, 0, j2p_special_handling,
                              j2p_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_FTMH_IS_NOT_STAMPPED, 0, 0, j2p_special_handling, j2p_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX, 0, 0, j2p_special_handling, j2p_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE, 0, 0, j2p_special_handling,
                              j2p_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_FTMH_PSIZE_MISMATCH, 0, 0, j2p_special_handling, j2p_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_PSIZE_MISMATCH, 0, 0, j2p_special_handling, j2p_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_EXPECTED_ITPP_DELTA_MISMATCH, 0, 0, j2p_special_handling,
                              j2p_none, "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_ERR_NEGATIVE_DELTA, 0, 0, j2p_special_handling, j2p_none,
                              "Error is due to wrong configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_BYPASS_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_BYPASS_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_WDF_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_WDF_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_WDF_CPYDAT_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_ITE_FIFO_WDF_CPYDAT_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_BDBC_TX_FIF_OVF, 0, 0, j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_BDBC_RX_SUB_REQ_FIF_OVF, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PEM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_PEM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_0, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_1, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_2, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_3, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_4, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_5, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_LM_COLLISION_ERROR_LM_COLLISION_ERROR_6, 0, 0, j2p_special_handling,
                              j2p_none, "Check which source tried to access the relevant bit that toggeled");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_0, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_1, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_2, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_3, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_4, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_5, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_6, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_7, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_8, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_9, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_10, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_11, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_12, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_13, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_14, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_15, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_16, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_17, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_18, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_19, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_20, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_21, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_22, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_23, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_24, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_25, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_26, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_27, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_28, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_29, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_30, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_31, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_32, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_ERROR_COUNTER_OVF_ERROR_COUNTER_OVF_ERROR_33, 0, 0,
                              j2p_special_handling, j2p_none,
                              "The sw process should read Engine[n]CntOvfAddress (0x0122) in order to retrieve the overflow address");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_0_FULLNESS_DMA_0_NOT_EMPTY_EVENT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_0_FULLNESS_DMA_0_ALMOST_FULL_EVENT, 0, 0, j2p_special_handling,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_0_FULLNESS_DMA_0_FULL_EVENT, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_1_FULLNESS_DMA_1_NOT_EMPTY_EVENT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_1_FULLNESS_DMA_1_ALMOST_FULL_EVENT, 0, 0, j2p_special_handling,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_DMA_1_FULLNESS_DMA_1_FULL_EVENT, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_0, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_1, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_2, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_3, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_4, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_5, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_6, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_7, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_8, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_9, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_10, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_11, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_12, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_13, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_14, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_15, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_16, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_17, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_18, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_19, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_20, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_21, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_22, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_23, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_24, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_25, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_26, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_27, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_28, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_29, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_30, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_31, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_32, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_CONDITIONAL_TRIGGER_OCCURED_CONDITIONAL_TRIGGER_OCCURED_ENGINE_33, 0,
                              0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_0,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_1,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_2,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_3,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_4,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_5,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_6,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_7,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_8,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_9,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_10,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_11,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_12,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_13,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_14,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_15,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_16,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_17,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_18,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_19,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_20,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_21,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_22,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_23,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_24,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_25,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_26,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_27,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_28,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_29,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_30,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_31,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_32,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_CONDITIONAL_TRIGGER_WHILE_FIFO_IS_FULL_ENGINE_33,
                              0, 0, j2p_special_handling, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_0,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_1,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_2,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_3,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_4,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_5,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_6,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_7,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_8,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_9,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_10,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_11,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_12,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_13,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_14,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_15,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_16,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_17,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_18,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_19,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_20,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_21,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_22,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_23,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_24,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_25,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_26,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_27,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_28,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_29,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_30,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_31,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_32,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit,
                              J2P_INT_CRPS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_DUAL_CNTR_SCAN_EVICTION_TO_IRE_CONFLICTS_ENGINE_33,
                              0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_0_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_1_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_2_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_3_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_4_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_5_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_6_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_AB_TYPE_FIFO_7_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_0_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_1_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_RECORD_PACKING_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_0_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_1_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_2_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_3_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_4_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_5_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_6_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_7_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_8_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_9_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_10_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_11_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_12_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_13_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_14_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_15_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_16_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_17_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_18_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_19_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_20_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_21_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_22_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_23_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_24_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_25_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_26_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_27_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_28_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_29_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_30_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_31_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_32_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_OVERFLOW_DMA_ENG_33_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_0_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_1_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_2_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_3_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_4_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_5_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_6_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_AB_TYPE_FIFO_7_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_0_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_1_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_RECORD_PACKING_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_0_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_1_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_2_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_3_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_4_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_5_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_6_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_7_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_8_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_9_FIFO_UNDERFLOW_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_10_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_11_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_12_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_13_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_14_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_15_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_16_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_17_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_18_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_19_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_20_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_21_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_22_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_23_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_24_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_25_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_26_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_27_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_28_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_29_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_30_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_31_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_32_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CRPS_FIFOS_UNDERFLOW_DMA_ENG_33_FIFO_UNDERFLOW_INT, 0, 0, j2p_none,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_TX_FIFO_OVERFLOW_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_RX_FIFO_OVERFLOW_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_WRONG_WORD_FROM_MAC_0_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_TX_MISSING_SOB_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_TX_DOUBLE_SOB_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_RX_NUM_DROPPED_EOPS_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_RX_NUM_DROPPED_EOPS_75P_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_ALIGNER_FIFO_OVF_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_0_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_1_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_2_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_3_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_4_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_5_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_6_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_7_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_8_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_9_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_10_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_11_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_12_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_13_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_14_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_PORT_15_INT, 0, 0,
                              j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MESH_TOPOLOGY_MESH_INTERRUPTS_INTERRUPT_1, 0, 0, j2p_special_handling,
                              j2p_none, "Check if system got disconnected from the fabric.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MESH_TOPOLOGY_MESH_INTERRUPTS_INTERRUPT_2, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPA_VTD_OUTLIF_OVERLOAD, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPA_VTE_OUTLIF_OVERLOAD, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPA_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPA_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPA_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ERR_ITPP_PSIZE_TYPE_0_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ERR_ITPP_PSIZE_TYPE_1_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ERR_ITPP_PSIZE_TYPE_2_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ERR_ITPP_PSIZE_TYPE_3_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ERR_ITPP_PSIZE_TYPE_4_MISMATCH, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ITPP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_WRAP_ING_GLBL_ENG_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_WRAP_ING_ENG_0_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_WRAP_ING_ENG_1_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_WRAP_ING_ENG_2_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_EGR_ENG_WRAP_EGR_ENG_0_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_EGR_ENG_WRAP_EGR_ENG_1_WRAP_ERR_INT, 0, 0, j2p_reprogram_resource,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_GLBL_ENG_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading IngGlblEngOopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_0_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading IngEng0_OopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_1_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading IngEng1_OopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_ING_ENG_OOP_FIFO_NOT_EMPTY_ING_ENG_2_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading IngEng2_OopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_EGR_ENG_OOP_FIFO_NOT_EMPTY_EGR_ENG_0_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading EgrEng0_OopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MRPS_EGR_ENG_OOP_FIFO_NOT_EMPTY_EGR_ENG_1_OOP_FIFO_NOT_EMPTY_INT, 0, 0,
                              j2p_special_handling, j2p_none,
                              "For clearing this interrupt need to read all FIFO entries by reading EgrEng1_OopFifoData");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_UNRCH_DEST_EVENT, 0, 0, j2p_special_handling, j2p_none,
                              "Check that the destination device is configured correctly. If so, check fabric connectivity to see if device is connected properly.");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_20, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_21, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_DATAFIFO_OVF, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_DATAFIFO_UNDER_FLOW, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_22, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_23, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_24, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_AUTO_DOC_NAME_25, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_FCT_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MSS_MACSEC_MACSEC_INTERRUPT_REGISTER, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_EMPTY_BUFF_ALLOC_INT, 0, 0, j2p_hard_reset, j2p_special_handling,
                              "Check for misconfiguration in min gap and credits in fqp txi");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_NO_EOB_INT, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_TOO_MANY_UNITS_INT, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_DEQ_WAS_CANCELED, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_DEQ_WAS_CANCELED_EMPTY_Q, 0, 0, j2p_hard_reset, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_ENQ_TO_NONE_VALID_Q, 0, 0, j2p_hard_reset, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ESB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDHA_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDHA_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ERROR_FREE_INT, 0, 0, j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ERROR_FBM_0_BUFF_STAT_OVF_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ERROR_FBM_1_BUFF_STAT_OVF_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ERROR_OCBM_BUFF_STAT_OVF_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_0, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_1, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_2, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_3, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_4, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_5, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_6, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_7, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_8, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_9, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_10, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_11, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_12, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_13, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_14, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_15, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_16, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_17, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_18, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_19, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_20, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_21, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_22, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_23, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_24, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_25, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_26, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_27, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_28, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_29, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_30, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_FBC_BANK_ERROR_FBC_BANK_31, 0, 0, j2p_special_handling, j2p_none,
                              "Ingress soft init");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RSUT_WR_RXI_CTRL_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RSUT_WR_RXI_CTRL_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_0_RD_2D_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_0_RD_2D_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_0_DEL_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_0_DEL_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_1_RD_2D_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_1_RD_2D_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_1_DEL_FREE_FIFO_OVERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_FBM_RING_1_DEL_FREE_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_0_S_2F_RD_REQ_ADDR_FIFO_OVERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_0_S_2F_RD_REQ_ADDR_FIFO_UNDERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_0_S_2D_RD_REQ_ADDR_FIFO_OVERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_0_S_2D_RD_REQ_ADDR_FIFO_UNDERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_1_S_2F_RD_REQ_ADDR_FIFO_OVERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_1_S_2F_RD_REQ_ADDR_FIFO_UNDERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_1_S_2D_RD_REQ_ADDR_FIFO_OVERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_OCBM_OCBM_FIFO_RRM_RING_1_S_2D_RD_REQ_ADDR_FIFO_UNDERFLOW, 0, 0,
                              j2p_hard_reset, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_QP_ACC_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_QP_DEQ_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_TCG_ACC_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_TCG_DEQ_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_OTM_ACC_HP_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_OTM_DEQ_HP_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_OTM_ACC_LP_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_OTM_DEQ_LP_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_CAL_ACC_HP_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_CAL_DEQ_HP_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_CAL_ACC_LP_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_CAL_DEQ_LP_OVERFLOW, 0, 0, j2p_reprogram_resource, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_MC_UC_WFQ_ACC_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_MC_UC_WFQ_DEQ_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_TCG_WFQ_ACC_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_TCG_WFQ_DEQ_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_QP_ACC_DEQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_QP_ACC_DEQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_TCG_ACC_DEQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_TCG_ACC_DEQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTMH_ACC_DEQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTMH_ACC_DEQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTML_ACC_DEQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTML_ACC_DEQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTM_CALH_ENQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTM_CALH_ENQ_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTM_CALL_ENQ_FIFO_OVERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_EPS_EPS_FIFO_STATUS_OTM_CALL_ENQ_FIFO_UNDERFLOW, 0, 0, j2p_soft_init, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_ERR_BITS_FROM_EGQ_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_WRONG_EGQ_WORD_INT, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_SYNC_ETH_0_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_SYNC_ETH_1_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_NMG_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_FLP_PACKETS_REJECTED_BY_EGW_INT, 0, 0, j2p_special_handling, NULL,
                              "KBP link is down or busy for some reason");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_FLP_PACKETS_REJECTED_BY_EGW_I_OPCODE, 0, 0, j2p_reprogram_resource,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_LEL_BURST_FIFO_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_EGRESS_OPPORTUNISTIC_FIFO_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_OPPORTUNISTIC_LOOKUP_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_LEL_ERR_DATA_VALID, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_LEL_CONTROL_FIFO_DROP, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_LEL_ONE_LEL_TXI_DROP, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_HEAD_UPDT_IN_LAST_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_HEAD_UPDT_IN_EMPTY_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_DEQ_CMD_TO_EMPTY_ERR_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_ALLOCATED_BDM_BDB_RANGE_ERR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_ALLOCATED_WHEN_BDB_FIFO_EMPTY_ERR, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_TX_FIFO_ERR_TX_BD_FIFO_OVF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_TX_FIFO_ERR_TX_BD_FIFO_UNF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_TX_FIFO_ERR_TX_BB_FIFO_OVF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_TX_FIFO_ERR_TX_BB_FIFO_UNF_INT, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_ENQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_ENQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_2_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_2_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_3_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_3_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_4_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_4_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_5_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_5_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_6_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_6_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_7_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_7_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_8_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_DEQ_FIFO_8_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_BDM_FIFO_0_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_BDM_FIFO_0_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_BDM_FIFO_1_OVF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DQM_FIFO_ERR_BDM_FIFO_1_UNF, 0, 0, j2p_reload_board, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_A_BANK_0_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_A_Bank0_Source");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_A_BANK_1_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_A_Bank1_Source");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_0_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank0_Source and the unexpected source is under EngDb_C_Bank0_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_1_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank1_Source and the unexpected source is under EngDb_C_Bank1_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_2_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank2_Source and the unexpected source is under EngDb_C_Bank2_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_3_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank3_Source and the unexpected source is under EngDb_C_Bank3_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_4_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank4_Source and the unexpected source is under EngDb_C_Bank4_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_5_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank5_Source and the unexpected source is under EngDb_C_Bank5_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_6_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank6_Source and the unexpected source is under EngDb_C_Bank6_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_7_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank7_Source and the unexpected source is under EngDb_C_Bank7_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_8_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank8_Source and the unexpected source is under EngDb_C_Bank8_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_9_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank9_Source and the unexpected source is under EngDb_C_Bank9_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_10_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank10_Source and the unexpected source is under EngDb_C_Bank10_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_11_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank11_Source and the unexpected source is under EngDb_C_Bank11_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_12_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank12_Source and the unexpected source is under EngDb_C_Bank12_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_13_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank13_Source and the unexpected source is under EngDb_C_Bank13_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_14_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank14_Source and the unexpected source is under EngDb_C_Bank14_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_15_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank15_Source and the unexpected source is under EngDb_C_Bank15_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_16_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank16_Source and the unexpected source is under EngDb_C_Bank16_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_17_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank17_Source and the unexpected source is under EngDb_C_Bank17_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_18_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank18_Source and the unexpected source is under EngDb_C_Bank18_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_19_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank19_Source and the unexpected source is under EngDb_C_Bank19_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_20_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank20_Source and the unexpected source is under EngDb_C_Bank20_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_21_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank21_Source and the unexpected source is under EngDb_C_Bank21_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_22_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank22_Source and the unexpected source is under EngDb_C_Bank22_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_23_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank23_Source and the unexpected source is under EngDb_C_Bank23_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_24_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank24_Source and the unexpected source is under EngDb_C_Bank24_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_25_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank25_Source and the unexpected source is under EngDb_C_Bank25_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_26_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank26_Source and the unexpected source is under EngDb_C_Bank26_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_27_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank27_Source and the unexpected source is under EngDb_C_Bank27_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_28_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank28_Source and the unexpected source is under EngDb_C_Bank28_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_29_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank29_Source and the unexpected source is under EngDb_C_Bank29_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_30_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank30_Source and the unexpected source is under EngDb_C_Bank30_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_31_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank31_Source and the unexpected source is under EngDb_C_Bank31_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_32_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank32_Source and the unexpected source is under EngDb_C_Bank32_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_33_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank33_Source and the unexpected source is under EngDb_C_Bank33_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_34_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank34_Source and the unexpected source is under EngDb_C_Bank34_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ENG_DB_C_BANK_35_ACCESS_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "Check which source is defined under EngDb_C_Bank35_Source and the unexpected source is under EngDb_C_Bank35_AccessErrSource");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MCP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PKT_REAS_INT_VEC, 0, 0, j2p_soft_init, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_TDM_PACKET_SIZE_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ALL_DATA_BUFFERS_ALLOCATED_INT, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_DBF_USED_SAME_BANK_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_UC_FIFO_FULL_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_TDM_FIFO_FULL_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_MCH_FIFO_FULL_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_MCL_FIFO_FULL_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_UC_PKT_PORT_FF, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ILLEGAL_PKT_SIZE_INT, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ILLEGAL_BIER_OFFSET, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_FDA_ORIGIN_ERR, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_FDA_SOURCE_ERR, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_RCM_FIFO_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_RCM_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_STUCK_FIFO_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_STUCK_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_CTRL_FIFO_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_CTRL_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_HDR_FIFO_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_HDR_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_EMR_FPC_FIFO_OVERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_EMR_FPC_FIFO_UNDERFLOW, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_PKT_SIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_FRAG_NUM_ZERO_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_PCP_LOC_NOT_IN_CELL_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_NO_EOP_ON_EOC_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_FDR_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_PAIR_FRAG_NUM_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_CELL_0_LAST_EOP_PSIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_SINGLE_CELL_SRC_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_SOP_CELL_SIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CDC_SOP_CELL_OVERSIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_MISSING_SOP_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_MISSING_EOP_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_MISSING_CELL_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_TDM_MISMATCH_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_PKT_SIZE_MISMATCH_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_MOP_CELL_SIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CUP_CELL_0_LAST_EOP_PSIZE_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_CRC_ERR, 0, 0, j2p_none, NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_DISCARD_ERR, 0, 0, j2p_none,
                              NULL, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_RCM_DYNAMIC_MISSING_SOP_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_RQP_PACKET_REASSEMBLY_RCM_STATIC_MISS_CONFIG_ERR, 0, 0, j2p_none, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HRC_WRITE_REQUEST_TXI_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HRC_READ_REQUEST_TXI_OVERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HRC_READ_DATA_RXI_UNDERFLOW, 0, 0, j2p_soft_init, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HRC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HRC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_UC_PLL_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_UC_PLL_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_CORE_PLL_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_CORE_PLL_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_FAB_0_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_FAB_0_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_FAB_1_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_FAB_1_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NIF_0_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NIF_0_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NIF_1_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NIF_1_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_BS_PLL_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_BS_PLL_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_TS_PLL_LOCKED_LOST, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_TS_PLL_LOSS_REFCLK, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NS_TIMER_ASYNC_FIFO_FULL, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NS_TIMER_ASYNC_FIFO_OVERFLOW, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NS_TIMER_ASYNC_FIFO_UNDERFLOW, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NTP_TIMER_ASYNC_FIFO_FULL, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NTP_TIMER_ASYNC_FIFO_OVERFLOW, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_NTP_TIMER_ASYNC_FIFO_UNDERFLOW, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_IEEE_1588_TIMER_ASYNC_FIFO_FULL, 0, 0, j2p_special_handling, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_IEEE_1588_TIMER_ASYNC_FIFO_OVERFLOW, 0, 0, j2p_special_handling,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_IEEE_1588_TIMER_ASYNC_FIFO_UNDERFLOW, 0, 0, j2p_special_handling,
                              j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_SB_FIFO_OVERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_SB_FIFO_UNDERFLOW, 0, 0, j2p_hard_reset, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ECI_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_RX_0_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_RX_0_SECOND_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_TX_0_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_TX_0_SECOND_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_RX_1_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_RX_1_SECOND_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_TX_1_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ILKN_CORE_TX_1_SECOND_INT, 0, 0, j2p_none, j2p_none,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_RX_ELK_OVF_INT, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_ILE_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBMC_HBM_CATTRIP, 0, 0, j2p_hard_reset, j2p_none,
                              "hbm must be shut down immediately");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBMC_HBM_TEMP_CHANGE, 0, 0, j2p_none, j2p_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ISEM_1_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ISEM_2_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ISEM_3_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_LEM_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_IOEM_EM_CUCKOO_FAIL_0, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_IOEM_EM_CUCKOO_FAIL_1, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_MC_ID_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_GLEM_EM_CUCKOO_FAIL_0, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_GLEM_EM_CUCKOO_FAIL_1, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EOEM_EM_CUCKOO_FAIL_0, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EOEM_EM_CUCKOO_FAIL_1, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ESEM_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_1_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_2_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_3_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_4_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_RMEP_EM_CUCKOO_FAIL, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_LEM_EMP_PULSE_SCAN_DONE, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_3_EMP_PULSE_SCAN_DONE, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_EXEM_4_EMP_PULSE_SCAN_DONE, 0, 0, j2p_special_handling, NULL,
                              "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ECC_PARITY_ERR_INT, 0, 0, j2p_intr_parity_handler,
                              j2p_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ECC_ECC_1B_ERR_INT, 0, 0, j2p_intr_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MDB_ECC_ECC_2B_ERR_INT, 0, 0, j2p_intr_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "Check Configuration");

    
    dnxc_intr_add_handler_ext(unit, J2P_INT_IPPD_INVALID_DESTINATION_VALID, 0, 0, j2p_special_handling, NULL,
                              "Invalid destination detected");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_PROTECTION_ERROR_1_BIT_ECC, 0, 0, j2p_intr_tcam_ecc_1b_handler,
                              j2p_intr_ecc_1b_recurr, "TCAM ECC 1bit protection error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_TCAM_TCAM_PROTECTION_ERROR_2_BIT_ECC, 0, 0, j2p_intr_tcam_ecc_2b_handler,
                              j2p_intr_ecc_2b_recurr, "TCAM ECC 2bit protection error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDB_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0,
                              j2p_pfc_deadlock_handling, NULL, "PFC Deadlock detected");
    dnxc_intr_add_handler_ext(unit, J2P_INT_CDBM_PFC_DEADLOCK_BREAKING_MECHANISM_INT, 0, 0,
                              j2p_pfc_deadlock_handling, NULL, "PFC Deadlock detected");
    dnxc_intr_add_handler_ext(unit, J2P_INT_MSS_MACSEC_MACSEC_INTERRUPT_REGISTER, 0, 0, j2p_macsec_interrupt_handling,
                              NULL, "MACSEC interrupt detected");
    dnxc_intr_add_handler_ext(unit, J2P_INT_DDP_DELETE_BDB_FIFO_NOT_EMPTY, 0, 0, j2p_special_handling, j2p_none,
                              "Defective buffer was sent to Quarantine");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_ECC_ECC_1B_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "HBC ECC 1bit error");
    dnxc_intr_add_handler_ext(unit, J2P_INT_HBC_ECC_ECC_2B_ERR_INT, 0, 0, j2p_special_handling, j2p_none,
                              "HBC ECC 2bit error");

}
