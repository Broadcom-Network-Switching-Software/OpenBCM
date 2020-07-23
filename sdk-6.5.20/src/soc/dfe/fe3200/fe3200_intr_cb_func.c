/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for FE3200 interrupts.
 */
 

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/dcmn/dcmn_intr_handler.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>

#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR




STATIC
 int
 fe3200_scrub_shadow_write(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    dcmn_int_corr_act_type p_corrective_action;
    dcmn_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];

    SOCDNX_INIT_FUNC_DEFS;
    rc = fe3200_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

 }

 int
 fe3200_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DCMN_INT_CORR_ACT_SOFT_RESET, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }

STATIC
 int
 fe3200_shutdown_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;

    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }


int fe3200_intr_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt,char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    

    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe3200_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_fe3200_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int fe3200_intr_recurring_action_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt, char *msg)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    

    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_fe3200_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = fe3200_interrupt_handles_corrective_action(unit, block_instance, en_fe3200_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}
STATIC
 int
 fe3200_special_handling_normal_0(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_1(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_recur_2(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_3(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_recur_4(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_5(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_6(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_7(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 fe3200_special_handling_normal_8(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }


int soc_interrupt_clear_src_chg_lnk_event(int unit, int block_instance, int interrupt_id)
{
    int rc = SOC_E_NONE;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;
    uint64  data;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,(BSL_META_U(unit, "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,(BSL_META_U(unit, "Null parameter\n")));
        return SOC_E_PARAM;
    }

    
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,(BSL_META_U(unit, "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

    inter = &(interrupts[interrupt_id]);
    rc = soc_reg64_get(unit, DCL_CONNECTIVITY_LINKS_EVENTSr, block_instance, inter->reg_index, &data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,(BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}


void fe3200_interrupt_cb_init(int unit)
{
    
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_P_0_DESCCNTO, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_DCH_P_0_DESCCNTO");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_P_1_DESCCNTO, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_DCH_P_1_DESCCNTO");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_P_2_DESCCNTO, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_DCH_P_2_DESCCNTO");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_IFMFO_P_0_INT, 10,1,fe3200_special_handling_normal_0,fe3200_hard_reset,"If FifosDepthType has an illegal size - do hard_reset.\nIf there are more than 10 legal occurrences of overflow per second, do a hard_reset.\nNote:\nFifo depth size rules:\n- Minimal size of 8 in a used pipe\n- Total size of all pipes (per type) smaller or equal to 384\n- Size must be dividable by 4");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_IFMFO_P_1_INT, 10,1,fe3200_special_handling_normal_0,fe3200_hard_reset,"If FifosDepthType has an illegal size - do hard_reset.\nIf there are more than 10 legal occurrences of overflow per second, do a hard_reset.\nNote:\nFifo depth size rules:\n- Minimal size of 8 in a used pipe\n- Total size of all pipes (per type) smaller or equal to 384\n- Size must be dividable by 4");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_IFMFO_P_2_INT, 10,1,fe3200_special_handling_normal_0,fe3200_hard_reset,"If FifosDepthType has an illegal size - do hard_reset.\nIf there are more than 10 legal occurrences of overflow per second, do a hard_reset.\nNote:\nFifo depth size rules:\n- Minimal size of 8 in a used pipe\n- Total size of all pipes (per type) smaller or equal to 384\n- Size must be dividable by 4");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_CPUDATACELLFNE_A_INT, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_DCH_CPUDATACELLFNE_A_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_CPUDATACELLFNE_B_INT, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_DCH_CPUDATACELLFNE_B_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_CPUDATACELLFNE_C_INT, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_DCH_CPUDATACELLFNE_C_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_CPUDATACELLFNE_D_INT, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_DCH_CPUDATACELLFNE_D_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_UNREACH_DEST_EV_INT, 10000,100,fe3200_none,fe3200_special_handling_recur_2,"If unreachanble destination occurs continously, requires to further debug system, understand if it is a real destination that exists in the system, if this is known on all other FE, check if is known in RTP main database and only one of the copies is missing, in which case the recovery can be simple");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_0, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_ALTO_P_0_INT, 0,0,fe3200_special_handling_normal_3,NULL,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_0, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_0, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_OUT_OF_SYNC_INT_P_0");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_1, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_ALTO_P_1_INT, 0,0,fe3200_special_handling_normal_3,NULL,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_1, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_1, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_OUT_OF_SYNC_INT_P_1");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_CELL_P_2, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_ALTO_P_2_INT, 0,0,fe3200_special_handling_normal_3,NULL,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_DCH_UN_EXP_ERROR_P_2, 0,0,fe3200_special_handling_normal_1,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_OUT_OF_SYNC_INT_P_2, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_OUT_OF_SYNC_INT_P_2");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_DCH_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCH_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCH_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_0_SRC_DV_CNG_LINK_INT, 1000,100,fe3200_none,fe3200_special_handling_recur_4,"If occurs continously, need to debug the whole system, to understand if this is only occuring on on FE or on all, try to isolate if this is a problem if the peer side or the local FE. If only occurs on one link - powering it down to bypass it is a valid approach. Consider hard reset to the FE if only apears on this FE, but on more than 1 link, to see if problem has dissapeared. ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_1_SRC_DV_CNG_LINK_INT, 1000,100,fe3200_none,fe3200_special_handling_recur_4,"If occurs continously, need to debug the whole system, to understand if this is only occuring on on FE or on all, try to isolate if this is a problem if the peer side or the local FE. If only occurs on one link - powering it down to bypass it is a valid approach. Consider hard reset to the FE if only apears on this FE, but on more than 1 link, to see if problem has dissapeared. ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_0_CHF_OVF_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_0_CLF_OVF_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_1_CHF_OVF_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_CCP_1_CLF_OVF_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_DCL_P_0_TAG_PAR_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_DCL_P_1_TAG_PAR_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_DCL_P_2_TAG_PAR_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_MACA_DATA_CRC_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_MACB_DATA_CRC_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_MACC_DATA_CRC_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_SOV_ERR_P_0_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_SOV_ERR_P_1_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_SOV_ERR_P_2_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_PCP_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"If occurs continously, assume some logical corruption and hard reset might recover it");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_DCL_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCL_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCL_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_0_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCM_OUT_OF_SYNC_P_0_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_1_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCM_OUT_OF_SYNC_P_1_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCM_OUT_OF_SYNC_P_2_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCM_OUT_OF_SYNC_P_2_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCM_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_DCM_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCM_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCM_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_0_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_1_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_0_LB_CNT_UNDERFLOW_P_2_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_0_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_1_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_1_LB_CNT_UNDERFLOW_P_2_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_0_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_1_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_2_LB_CNT_UNDERFLOW_P_2_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_0_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_0_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_1_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_1_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_2_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_DCMC_DCQ_3_LB_CNT_UNDERFLOW_P_2_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_CPU_CAPT_CELL_FNE_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Read captured cell(s) from register specified at int_info_csrs_prop1");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_UNRCH_DST_INT, 0,0,fe3200_special_handling_normal_1,NULL,"If ParityErrInt is set { Handle the parrity error and ignore this  interrupt } else if 2bits ecc error in RTP { handle RTP errors and ignore this interrupt } else { There is a problem with the local or remote reachbility tables} ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_GSYNC_DSCRD_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Read register scecified at int_info_csrs_prop1");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_CRP_PARITY_ERR_INT, 1,1000000,fe3200_hard_reset,fe3200_none,"FE3200_INT_CCS_CRP_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_CCS_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_CCS_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_CCS_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_EMPTY_MULTICAST_ID_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check maximum device ID at the FE device (MaxBaseIndex field). Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check maximum device ID at the FE device (MaxBaseIndex field). Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check maximum device ID at the FE device (MaxBaseIndex field). Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check maximum device ID at the FE device (MaxBaseIndex field). Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_0_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_1_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_2_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_DRHP_3_DRHP_N_QUERY_UNREACHABLE_MULTICAST_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check MC group configuration in the sending device. Check MC group destination assigment at the sending device.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_EMPTY_LINK_MAP_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at MaxBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at MaxBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at MaxBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at MaxBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_0_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_1_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_2_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_CRH_3_CRH_N_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 0,0,fe3200_special_handling_normal_1,NULL,"Check if the value configured at UpdateBaseIndex field is correct. Check destination assigment in the sending device");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_GENERAL_LINK_INTEGRITY_CHANGED_INT, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_RTP_GENERAL_LINK_INTEGRITY_CHANGED_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_scrub_shadow_write,fe3200_hard_reset,"FE3200_INT_RTP_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_RTP_ECC_ECC_2B_ERR_INT, 0,0,fe3200_scrub_shadow_write,NULL,"FE3200_INT_RTP_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 1,1000000,fe3200_shutdown_link,fe3200_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 1,1000000,fe3200_shutdown_link,fe3200_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 1,1000000,fe3200_shutdown_link,fe3200_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 1,1000000,fe3200_shutdown_link,fe3200_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 1,1000000,fe3200_shutdown_link,fe3200_none,"FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 1,1000000,fe3200_shutdown_link,fe3200_none,"FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 1,1000000,fe3200_shutdown_link,fe3200_none,"FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 1,1000000,fe3200_shutdown_link,fe3200_none,"FE3200_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_0, 0,0,fe3200_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_1, 0,0,fe3200_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_2, 0,0,fe3200_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_LOS_INT_3, 0,0,fe3200_special_handling_normal_5,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_0, 1,100,fe3200_special_handling_normal_6,fe3200_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_1, 1,100,fe3200_special_handling_normal_6,fe3200_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_2, 1,100,fe3200_special_handling_normal_6,fe3200_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_OOF_INT_3, 1,100,fe3200_special_handling_normal_6,fe3200_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 1,1000000,fe3200_special_handling_normal_7,fe3200_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 1,1000000,fe3200_special_handling_normal_7,fe3200_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 1,1000000,fe3200_special_handling_normal_7,fe3200_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 1,1000000,fe3200_special_handling_normal_7,fe3200_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0,0,fe3200_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0,0,fe3200_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0,0,fe3200_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0,0,fe3200_special_handling_normal_8,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_0, 0,0,fe3200_special_handling_normal_1,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_1, 0,0,fe3200_special_handling_normal_1,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_2, 0,0,fe3200_special_handling_normal_1,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_3, 0,0,fe3200_special_handling_normal_1,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,fe3200_special_handling_normal_1,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,fe3200_special_handling_normal_1,NULL,"FE3200_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT, 1000,100,fe3200_none,fe3200_hard_reset,"FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT, 0,0,fe3200_hard_reset,NULL,"FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT");
    

    dcmn_intr_add_clear_ext(unit, FE3200_INT_DCL_CCP_0_SRC_DV_CNG_LINK_INT, soc_interrupt_clear_src_chg_lnk_event);
    dcmn_intr_add_clear_ext(unit, FE3200_INT_DCL_CCP_1_SRC_DV_CNG_LINK_INT, soc_interrupt_clear_src_chg_lnk_event);
}

#undef _ERR_MSG_MODULE_NAME
