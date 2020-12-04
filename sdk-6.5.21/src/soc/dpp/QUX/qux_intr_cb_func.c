/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for QUX interrupts.
 */
 

#include <shared/bsl.h>
#include <soc/mcm/allenum.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dpp/mbcm.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>

#include <soc/dpp/QUX/qux_intr.h>
#include <soc/dpp/QUX/qux_intr_cb_func.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_em_ser.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <bcm_int/dpp/l2.h>

#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>
#include <soc/sand/sand_intr_corr_act_func.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR




int soc_interrupt_clear_fid_limit_reached_event(int unit, int block_instance, int interrupt_id);


int qux_intr_handle_generic_none(int unit, int block_instance, qux_interrupt_type en_qux_interrupt,char *msg)
{
    int rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;


    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"%d %s\n"), en_qux_interrupt, SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_qux_interrupt].name));

    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int qux_intr_recurring_action_handle_generic_none(int unit, int block_instance, qux_interrupt_type en_qux_interrupt, char *msg)
{
    int rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
 int
 qux_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;
    char info_msg[240];
    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_HARD_RESET, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DCMN_RESET_ACTION_INOUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);


exit:


    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    int rc = 0;

    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:


    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_reprogram_resource(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     int rc = 0;
     char info_msg[240];

    SOCDNX_INIT_FUNC_DEFS;

    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_scrub_shadow_write(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     int rc = 0;
     dcmn_int_corr_act_type p_corrective_action;
     dcmn_interrupt_mem_err_info shadow_correct_info;
     char info_msg[240];
     SOCDNX_INIT_FUNC_DEFS;
     rc = dcmn_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,NULL);
     SOCDNX_IF_ERR_EXIT(rc);
exit:
    SOCDNX_FUNC_RETURN;
 }

STATIC
 int
qux_scrub_read_write_back(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    int index;
    dcmn_int_corr_act_type corrective_action = DCMN_INT_CORR_ACT_NONE;
    char special_msg[DCMN_INTERRUPT_SPECIAL_MSG_SIZE] = {0};
    uint32 regVal, regVal_2;
    int invalidate_n, corrected_n;
    int invalidated_entry=0, corrected_entry=0;

    SOCDNX_INIT_FUNC_DEFS;

    
    switch (en_interrupt) {
        case QUX_INT_KAPS_KAPS_ERROR_REGISTER_MA_ERROR:
            break;

        case QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0:
        case QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1:
        {
            if (en_interrupt == QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0) {
                index = 0;
            } else {
                index = 1;
            }
            
            rc = soc_reg32_get(unit, KAPS_RPB_TCAM_SCAN_ERRORr, block_instance, index, &regVal);
            SOCDNX_IF_ERR_EXIT(rc);
            rc = soc_reg32_get(unit, KAPS_RPB_TCAM_SCAN_STATUSr, block_instance, index, &regVal_2);
            SOCDNX_IF_ERR_EXIT(rc);

            invalidate_n = soc_reg_field_get(unit, KAPS_RPB_TCAM_SCAN_ERRORr, regVal, RPB_TCAM_ENTRY_INVALIDATED_Nf);
            corrected_n = soc_reg_field_get(unit, KAPS_RPB_TCAM_SCAN_ERRORr, regVal, RPB_TCAM_ENTRY_CORRECTED_Nf);
            if (invalidate_n) {
                
                invalidated_entry = soc_reg_field_get(unit, KAPS_RPB_TCAM_SCAN_STATUSr, regVal_2, RPB_TCAM_INVALIDATED_OFFSET_Nf);
            }
            if (corrected_n) {
                corrected_entry = soc_reg_field_get(unit, KAPS_RPB_TCAM_SCAN_STATUSr, regVal_2, RPB_TCAM_CORRECTED_OFFSET_Nf);
            }
            sal_snprintf(special_msg, DCMN_INTERRUPT_SPECIAL_MSG_SIZE, "Kaps Tcam Ecc error on bank %d, entry offset %d", index, invalidate_n ? invalidated_entry : corrected_entry);
            break;
        }

        default:
            break;
    }

    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int qux_event_drc_phy_cdr_above_th_handler(int unit, int block_instance, uint32 en_interrupt, char* msg)
{
    jer_interrupt_type drc_phy_cdr_above_th_arr[] = { QUX_INT_DRCA_PHY_CDR_ABOVE_TH};
    int drc_phy_cdr_above_th_arr_size;
    int drc_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION, msg));

    drc_phy_cdr_above_th_arr_size = sizeof(drc_phy_cdr_above_th_arr) / sizeof(drc_phy_cdr_above_th_arr[0]);

    
    for (drc_ndx = 0; drc_ndx < drc_phy_cdr_above_th_arr_size; ++drc_ndx)
    {
        if (drc_phy_cdr_above_th_arr[drc_ndx] == en_interrupt)
        {
            
            SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_dynamic_calibration_handle_cdr_interrupt(unit, drc_ndx));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qux_interrupt_handle_tcamprotectionerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
{
    int rc;
    dcmn_int_corr_act_type corrective_action;
    ARAD_TCAM_LOCATION location;

    SOCDNX_INIT_FUNC_DEFS;

    corrective_action = DCMN_INT_CORR_ACT_NONE;

    
    rc = dcmn_interrupt_data_collection_for_tcamprotectionerror(unit, block_instance, en_arad_interrupt, &location, &corrective_action);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_arad_interrupt, 0, DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance, en_arad_interrupt, msg, corrective_action, (void*)(&location), NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC
 int
 qux_special_handling_normal_0(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_special_handling_recur_1(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_special_handling_normal_2(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;
    dcmn_int_corr_act_type p_corrective_action;
    dcmn_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    
    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_special_handling_recur_3(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_special_handling_normal_4(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;
    dcmn_int_corr_act_type p_corrective_action;
    dcmn_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];
    
    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 qux_special_handling_recur_5(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }

STATIC
 int
 qux_special_handling_normal_7(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
    

    int rc = 0;


    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }


STATIC dcmn_intr_action_t qux_intr_action[] =
{
    {qux_intr_handle_generic_none, DCMN_INT_CORR_ACT_NONE},
    {qux_intr_recurring_action_handle_generic_none, DCMN_INT_CORR_ACT_NONE},
    {qux_hard_reset, DCMN_INT_CORR_ACT_HARD_RESET},
    {qux_none, DCMN_INT_CORR_ACT_NONE},
    {qux_reprogram_resource, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE},
    {qux_scrub_shadow_write, DCMN_INT_CORR_ACT_SHADOW},
    {qux_scrub_read_write_back, DCMN_INT_CORR_ACT_NONE},
    {qux_event_drc_phy_cdr_above_th_handler, DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION},
    {qux_special_handling_normal_0, DCMN_INT_CORR_ACT_NONE},
    {qux_special_handling_recur_1,  DCMN_INT_CORR_ACT_NONE},
    {qux_special_handling_normal_2, DCMN_INT_CORR_ACT_SHADOW},
    {qux_special_handling_recur_3, DCMN_INT_CORR_ACT_NONE},
    {qux_special_handling_normal_4, DCMN_INT_CORR_ACT_SHADOW},
    {qux_special_handling_recur_5, DCMN_INT_CORR_ACT_NONE},
    {qux_special_handling_normal_7, DCMN_INT_CORR_ACT_NONE},
    {qux_interrupt_handle_tcamprotectionerror, DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB},
    {NULL, DCMN_INT_CORR_ACT_NONE}
};



void qux_interrupt_cb_init(int unit)
{
    sand_intr_action_info_set(unit, qux_intr_action);

   
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_S_DQCQ_DEPTH_OVF, 0,0,qux_special_handling_normal_0,NULL,"fix the configuration in the faulted dqcq depth registers and resume traffic.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_D_DQCQ_DEPTH_OVF, 0,0,qux_special_handling_normal_0,NULL,"fix the configuration in the faulted dqcq depth registers and resume traffic.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_EMPTY_S_DQCQ_WRITE, 0,0,qux_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_EMPTY_D_DQCQ_WRITE, 0,0,qux_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_QUEUE_ENTERED_DEL, 0,0,qux_none,NULL,"Monitor queue number (0x0149)");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_CREDIT_LOST, 0,0,qux_none,NULL,"QUX_INT_IPS_CREDIT_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_CREDIT_OVERFLOW, 0,0,qux_none,NULL,"QUX_INT_IPS_CREDIT_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_S_DQCQ_OVERFLOW, 0,0,qux_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_D_DQCQ_OVERFLOW, 0,0,qux_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_PUSH_QUEUE_ACTIVE, 0,0,qux_none,NULL,"QUX_INT_IPS_PUSH_QUEUE_ACTIVE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_CR_FLW_ID_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IPS_CR_FLW_ID_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPS_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_SMP_FULL_LEVEL_1, 0,0,qux_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_SMP_FULL_LEVEL_2, 0,0,qux_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_REBOUND_FIFO_CR_LOSS, 0,0,qux_reprogram_resource,NULL,"QUX_INT_SCH_REBOUND_FIFO_CR_LOSS");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SCH_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps0OvfCntrsCnt (0x0245) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x221). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_0_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_0_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_0_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_0_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps1OvfCntrsCnt (0x0246) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x222). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_1_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_1_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_1_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_1_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps2OvfCntrsCnt (0x0247) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x223). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_2_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_2_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_2_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_2_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps3OvfCntrsCnt (0x0248) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x224). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_3_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_3_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_3_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_3_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps4OvfCntrsCnt (0x0249) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x225). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_4_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_4_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_4_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_4_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps5OvfCntrsCnt (0x024A) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x226). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_5_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_5_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_5_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_5_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps6OvfCntrsCnt (0x024B) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x227). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_6_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_6_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_6_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_6_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps7OvfCntrsCnt (0x024C) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x228). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_7_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_7_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_7_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_7_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps8OvfCntrsCnt (0x024D) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x229). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_8_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_8_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_8_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_8_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps9OvfCntrsCnt (0x024E) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22A). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_9_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_9_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_9_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_9_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps10OvfCntrsCnt (0x024F) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22B). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_10_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_10_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_10_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_10_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps11vfCntrsCnt (0x0250) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22C). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_11_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_11_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_11_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_11_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps12OvfCntrsCnt (0x0251) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22D). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_12_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_12_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_12_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_12_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps13OvfCntrsCnt (0x0252) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22E). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_13_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_13_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_13_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_13_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps14OvfCntrsCnt (0x0253) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22F). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_14_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_14_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_14_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_14_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps15OvfCntrsCnt (0x0254) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x230). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_15_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_15_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_15_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_15_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_CNT_OVF, 0,0,qux_special_handling_normal_0,NULL,"The sw process should read Crps16OvfCntrsCnt (0x0255) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x231). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_INVLD_PTR_ACC, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_16_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_PRE_READ_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_PRE_READ_FIFO_NOT_EMPTY, 0,0,qux_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_DIRECT_RD_WHEN_WAITING, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_16_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ENGINE_CRPS_16_CNT_ECC_ERR, 0,0,qux_none,NULL,"QUX_INT_CRPS_ENGINE_CRPS_16_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_EPNI_A_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check EPNI configuration for command EPNI-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_EPNI_B_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check EPNI configuration for command EPNI-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_EGQ_A_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check EGQ configuration for command EGQ-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_EGQ_B_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check EGQ configuration for command EGQ-B Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_IRPP_A_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check IRPP configuration for command IRPP-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_IRPP_B_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check IRPP configuration for command IRPP-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_CGM_A_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check CGM configuration for command CGM-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_CGM_B_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check CGM configuration for command CGM-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_INVLID_ACCESS_EGQ_TM_INVLD_AD_ACC, 0,0,qux_special_handling_normal_0,NULL,"Check EGQ TM configuration for command EGQ TM.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_A_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check EPNIA  DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_B_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check EPNIB DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_A_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check EGQA DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_B_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check EGQB DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_A_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check IRPPA DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_B_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check IRPPB DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_CGM_A_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check CGMA DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_CGM_B_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check CGMB DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_TM_OFFSET_WAS_FILT, 0,0,qux_special_handling_normal_0,NULL,"Check EGQTM DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_0_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine0.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_1_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine1.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_2_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine2.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_3_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine3.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_4_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine4.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_5_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine5.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_6_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine6.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_7_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine7.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_8_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine8.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_9_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine9.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_10_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine10.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_11_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine11.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_12_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine12.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_13_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine13.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_14_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine14.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ACCESS_COLLISION_CRPS_15_WE_COLLIDE, 0,0,qux_special_handling_normal_0,NULL,"Check which sources tried to access engine15.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_CRPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CRPS_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDA_WRAP, 0,0,qux_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDB_WRAP, 0,0,qux_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDA_OOP_FIFO_NOT_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDB_OOP_FIFO_NOT_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDA_OOP_FIFO_NEW_ENTRY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_MCDB_OOP_FIFO_NEW_ENTRY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IMP_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IMP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDA_WRAP, 0,0,qux_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDB_WRAP, 0,0,qux_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDA_OOP_FIFO_NOT_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDB_OOP_FIFO_NOT_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDA_OOP_FIFO_NEW_ENTRY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_MCDB_OOP_FIFO_NEW_ENTRY, 0,0,qux_special_handling_normal_0,NULL,"Read oop fifo");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IEP_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IEP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_INT_UCFIFO_AF, 0,0,qux_none,NULL,"QUX_INT_EGQ_INT_UCFIFO_AF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_VLAN_EMPTY_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_VLAN_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_DROPPED_UC_PD_INT, 0,0,qux_special_handling_normal_0,NULL,"Check tresholds of of packet descriptors per Port / TC / Queue / Interface or Total. The CGM registers Unicast Packet Descriptors Drop Counter, Congestion Tracking Unicast PDMax Value and Unicast Packet Descriptors Counter should be read to analaze the status.    Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_DROPPED_UC_DB_INT, 0,0,qux_special_handling_normal_0,NULL,"Check tresholds of of data buffers per Port / TC / QUE / Interface or Total.The CGM registers Unicast Data Buffers Drop Counter (RQP / PQP), Congestion Tracking Unicast DBMax Value and Unicast Data Buffers Counter should be read to analaze the status.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_UC_PKT_PORT_FF, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EGQ_UC_PKT_PORT_FF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_DELETE_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"Check that the configuration of the EGQ does not result in too many discards. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ILLEGAL_PKT_SIZE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ILLEGAL_PKT_SIZE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_TC_MAPPING_MISS_CONFIG, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EGQ_TC_MAPPING_MISS_CONFIG");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ALL_DATA_BUFFERS_ALLOCATED_INT, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EGQ_ALL_DATA_BUFFERS_ALLOCATED_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_MC_MAX_REPLICATION_OVERFLOW_INT, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EGQ_MC_MAX_REPLICATION_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_MID_REP_DROP, 0,0,qux_none,NULL,"QUX_INT_EGQ_MID_REP_DROP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_TDM_HP_MC_DROP_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_TDM_HP_MC_DROP_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_TDM_LP_MC_DROP_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_TDM_LP_MC_DROP_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_MCDB_MIS_CONFIG, 0,0,qux_none,NULL,"QUX_INT_EGQ_MCDB_MIS_CONFIG");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_INVALID_OTM_INT, 0,0,qux_special_handling_normal_0,NULL,"Verify the ingress configuration of the device the packet was recived from.   Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0,0,qux_none,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ERPP_DISCARD_2_TDM_WRONG_PORT_INT, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EGQ_ERPP_DISCARD_2_TDM_WRONG_PORT_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EGQ_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_MIRR_OVF_INT, 0,0,qux_none,NULL,"QUX_INT_EPNI_MIRR_OVF_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_TRAP_OVF_INT, 0,0,qux_none,NULL,"QUX_INT_EPNI_TRAP_OVF_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_APP_AND_PIPE_COLLISION, 0,0,qux_none,NULL,"QUX_INT_EPNI_APP_AND_PIPE_COLLISION");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_EEI_TYPE_ERR, 0,0,qux_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_PHP_ERR, 0,0,qux_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_PHP_NEXT_IP_PROTOCOL_ERR, 0,0,qux_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_ERR, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_SNOOP, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_ERR, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_SNOOP, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_ERR, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_SNOOP, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_ERR, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_SNOOP, 0,0,qux_reprogram_resource,NULL,"QUX_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_NEW_DA_ERR, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_NEW_DA_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_NEW_NATIVE_DA_ERR, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_NEW_NATIVE_DA_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_STP_STATE_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_STP_STATE_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_ACCEPTABLE_FRAME_TYPE_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_ACCEPTABLE_FRAME_TYPE_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_EES_ACTION_DROP_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_EES_ACTION_DROP_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_MTU_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_MTU_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_EES_LAST_ACTION_NOT_AC, 0,0,qux_special_handling_normal_0,NULL,"PPConfiguration error, SW Proccess should alert configuration error");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_SPLIT_HORIZON_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_SPLIT_HORIZON_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_PROTECTION_PATH_DENY, 0,0,qux_none,NULL,"QUX_INT_EPNI_PP_PROTECTION_PATH_DENY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_PP_GLEM_DENY, 0,0,qux_special_handling_normal_0,NULL,"PPConfiguration error, SW Proccess should alert configuration error");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EPNI_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_CAM_TABLE_FULL, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_TABLE_COHERENCY, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_WARNING_INSERTED_EXISTING, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_EDB_ESEM_ONE_ESEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_EDB_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OLP_ERROR_EGRESS_PIPE_CFG, 0,0,qux_special_handling_normal_0,NULL,"If set then One of the pipes was configured to expect the traffic but the other pipe actually received it.\nCleared upon read.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OLP_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OLP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OLP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_PENDING_EVENT, 0,0,qux_special_handling_normal_0,NULL,"Pop message from event fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_STAT_PENDING_EVENT, 0,0,qux_special_handling_normal_0,NULL,"Pop message from stat fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_RX_STATS_DONE, 0,0,qux_special_handling_normal_0,NULL,"read SAT statistcics entry from the registers");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_RFC_6374_PKT_DROPPED, 0,0,qux_special_handling_normal_0,NULL,"QUX_INT_OAMP_RFC_6374_PKT_DROPPED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to the fail source specified in 0x0265, RmapemManagementUnitFailure");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_OAMP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_SPI_OOB_RX_FRM_ERR, 0,0,qux_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_SPI_OOB_RX_DIP_2_ERR, 0,0,qux_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_SPI_OOB_RX_WD_ERR, 0,0,qux_special_handling_normal_0,NULL,"the spi watch dog expired, and indicates that the interface is not active (down). Should verify the wd period config and also see if the board maybe noisy or defected. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CFC_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_BACA_BG_0_OVERFLOW_INT, 10,1,qux_none,qux_hard_reset,"QUX_INT_MMU_BACA_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_BACA_BG_1_OVERFLOW_INT, 10,1,qux_none,qux_hard_reset,"QUX_INT_MMU_BACA_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_BACA_BG_2_OVERFLOW_INT, 10,1,qux_none,qux_hard_reset,"QUX_INT_MMU_BACA_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_BACA_BG_3_OVERFLOW_INT, 10,1,qux_none,qux_hard_reset,"QUX_INT_MMU_BACA_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_MMU_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_MMU_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_CQF_OVER_FLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_RD_CRC_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_WR_CRC_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_READ_DATA_FIFO_OVERFLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_READ_DATA_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_DRAM_WR_CRC_ERR, 0,0,qux_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_DRAM_RD_CRC_ERR, 0,0,qux_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_DRAM_CMD_PARITY_ERR, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_DWF_OVER_FLOW, 0,0,qux_hard_reset,NULL,"QUX_INT_DRCA_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_TEMP_SENS_ABOVE_TH, 0,0,qux_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_PHY_CDR_ABOVE_TH, 0,0,qux_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_PHY_CDR_CYCLE_ABOVE_TH, 0,0,qux_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_PHY_CALIB_REST_ERROR, 0,0,qux_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_LB_VECTOR_INDEX_ILLEGAL_CALC_INT, 0,0,qux_special_handling_normal_0,NULL,"fix LB configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_LB_ECMP_LAG_USE_SAME_HASH_INT, 0,0,qux_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_FEC_ENTRY_ACCESSED_INT, 0,0,qux_none,NULL,"QUX_INT_IHB_FEC_ENTRY_ACCESSED_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0,0,qux_special_handling_normal_0,NULL,"fix configuration. ");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_INVALID_DESTINATION_VALID, 0,0,qux_none,NULL,"QUX_INT_IHB_INVALID_DESTINATION_VALID");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_KAPS_DIRECT_LOOKUP_ERR, 0,0,qux_special_handling_normal_0,NULL,"Read Kaps Register Address: 0x0000001b/1a\nBit0 = ECC Error\nBit1 = Address range Error\nif Address range Error need to reconfigure PMF direct access address");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHB_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_MAX_HEADER_STACK_EXCEEDED, 0,0,qux_none,NULL,"QUX_INT_IHP_MAX_HEADER_STACK_EXCEEDED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED, 0,0,qux_none,NULL,"QUX_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_VT_LIF_BANK_CONTENTION, 0,0,qux_none,NULL,"QUX_INT_IHP_VT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_TT_LIF_BANK_CONTENTION, 0,0,qux_none,NULL,"QUX_INT_IHP_TT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_MPLS_LABEL_TERMINATION_ERROR, 0,0,qux_none,NULL,"QUX_INT_IHP_MPLS_LABEL_TERMINATION_ERROR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT, 0,0,qux_none,NULL,"QUX_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_OAM_BFD_MISCONFIG_INT, 0,0,qux_none,NULL,"QUX_INT_IHP_OAM_BFD_MISCONFIG_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_KAPS_DB_LOOKUP_ERR, 0,0,qux_none,NULL,"QUX_INT_IHP_KAPS_DB_LOOKUP_ERR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_APP_AND_PIPE_COLLISION, 0,0,qux_none,NULL,"QUX_INT_IHP_APP_AND_PIPE_COLLISION");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IHP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_TCAM_PROTECTION_ERROR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_TCAM_QUERY_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_CAM_TABLE_FULL, 0,0,qux_special_handling_normal_0,NULL,"defrag should be initiated. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY, 0,0,qux_hard_reset,NULL,"QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to specified in 0x0405, OemaManagementUnitFailure. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_CAM_TABLE_FULL, 0,0,qux_special_handling_normal_0,NULL,"defrag should be initiated.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY, 0,0,qux_hard_reset,NULL,"QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to specified in 0x0485, OembManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_ISEM_ONE_ISEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to specified in 0x0305, IsemManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_PPDB_A_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to specified in 0x0265, MactManagementUnitFailure. You should print an error to the screen and check your configuration");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_EVENT_READY, 0,0,qux_special_handling_normal_0,NULL,"read mact_event if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning). Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_EVENT_FIFO_HIGH_THRESHOLD_REACHED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_REPLY_READY, 0,0,qux_special_handling_normal_0,NULL,"read mact_reply if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning).  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE, 0,0,qux_special_handling_normal_0,NULL,"fix configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_COUNTER_OVERFLOW, 0,0,qux_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_LEL_ERR_DATA_VALID, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_LEL_ERR_DATA_VALID");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_EVENT_FID_OR_LIF_EXCEED_LIMIT, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_EVENT_FID_OR_LIF_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_ISEM_ONE_ISEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,qux_special_handling_normal_0,NULL,"Act according to specified in 0x0305, IsemManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_B_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED, 0,0,qux_none,NULL,"QUX_INT_PPDB_B_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,qux_special_handling_normal_0,NULL,"Check configuration of contexts in the device: This interrupt implies that an incoming reassembly context was equal to the value configured in the field BadReassemblyContext of the IRE Static Configuration register. One of the context mappings erroneously points to this reassembly context. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_IPSEC_ERR_DATA_ARRIVED, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_IPSEC_ERR_DATA_ARRIVED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_IPSEC_ERR_WORD_SIZE, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_IPSEC_ERR_WORD_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_DATA_ARRIVED, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_DATA_ARRIVED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_WORD_SIZE, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_WORD_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_TDM_PACKET_SIZE, 0,0,qux_reprogram_resource,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_SAT_ERR_TDM_PACKET_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_CPYDAT, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_CPYDAT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_BYTE_EN, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_BYTE_EN");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_MISSING_EOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_MISSING_EOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_MISSING_SOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_CPU_ERR_MISSING_SOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_BYTE_EN, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_BYTE_EN");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_MISSING_EOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_MISSING_EOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_MISSING_SOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OLP_ERR_MISSING_SOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_BYTE_EN, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_BYTE_EN");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_MISSING_EOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_MISSING_EOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_MISSING_SOB, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_OAMP_ERR_MISSING_SOB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_64_BYTES_PACK, 0,0,qux_special_handling_normal_0,NULL,"Check how the code configued RegisterInterfacePacketControl: Check if the RegiPktErr field of that register was set to true.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_DATA_ARRIVED, 0,0,qux_special_handling_normal_0,NULL,"Check how the code configured RegisterInterfacePacketControl: Check if the packet sent was smaller than 32 bytes.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_PACKET_SIZE, 0,0,qux_reprogram_resource,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_PACKET_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_NIF_ERR_DATA_ARRIVED, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_NIF_ERR_DATA_ARRIVED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_EXTERNAL_IF_ERROR_NIF_ERR_PACKET_SIZE, 0,0,qux_none,NULL,"QUX_INT_IRE_EXTERNAL_IF_ERROR_NIF_ERR_PACKET_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IRE_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IRE_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ERROR_REASSEMBLY_CONTEXT, 0,0,qux_special_handling_normal_0,NULL,"Check configuratgion of reassembly contect mapping in IRE block");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ERROR_FBC_FPC, 0,0,qux_hard_reset,NULL,"QUX_INT_SPB_ERROR_FBC_FPC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ERROR_PKT_CRC, 0,0,qux_none,NULL,"QUX_INT_SPB_ERROR_PKT_CRC");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ERROR_SPACER_OVFL, 0,0,qux_hard_reset,NULL,"QUX_INT_SPB_ERROR_SPACER_OVFL");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_SPB_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_BUFF, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_BUFF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_NO_BUFF, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_NO_BUFF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_SOP, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_SOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_PACKET_REJECT, 0,0,qux_none,NULL,"QUX_INT_SPB_REASSEMBLY_REASSEMBLY_ERROR_PACKET_REJECT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ERR_MAX_REPLICATION, 0,0,qux_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ERR_REPLICATION_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ERR_FLOW_ID_IS_OVER_16K, 0,0,qux_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_TAR_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TAR_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,qux_hard_reset,NULL,"QUX_INT_ECGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_PNIMI_001, 0,0,qux_none,NULL,"QUX_INT_ECI_PNIMI_001");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_UC_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_UC_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_CORE_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_CORE_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_TS_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_TS_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_BS_0_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_BS_0_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_BS_1_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_BS_1_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_MISC_PLL_3_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_MISC_PLL_3_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_NIF_PML_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_NIF_PML_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_NIF_PMX_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_NIF_PMX_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_MISC_PLL_6_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_MISC_PLL_6_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_SYNCE_MASTER_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_SYNCE_MASTER_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_SYNCE_SLAVE_PLL_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_SYNCE_SLAVE_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_MISC_PLL_9_LOCKED_LOST, 0,0,qux_none,NULL,"QUX_INT_ECI_MISC_PLL_9_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_PVT_MON_TRACKER_INT, 0,0,qux_none,NULL,"QUX_INT_ECI_PVT_MON_TRACKER_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_ECI_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_ECC_ECC_1B_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_ECI_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ECI_ECC_ECC_2B_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_ECI_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_KAPS_ERROR_REGISTER_MA_ERROR, 0,0,qux_scrub_read_write_back,NULL,"QUX_INT_KAPS_KAPS_ERROR_REGISTER_MA_ERROR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0, 0,0,qux_scrub_read_write_back,NULL,"QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0");
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1, 0,0,qux_scrub_read_write_back,NULL,"QUX_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPSEC_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IPSEC_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPSEC_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IPSEC_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TXQ_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_TXQ_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TXQ_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_TXQ_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SQM_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_SQM_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SQM_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_SQM_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DQM_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_DQM_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DQM_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DQM_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_PACKET_OVERSIZE, 0,0,qux_reprogram_resource,NULL,"QUX_INT_DDP_ERROR_PACKET_OVERSIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_DRAM_BUNDLE_OVERSIZE, 0,0,qux_reprogram_resource,NULL,"QUX_INT_DDP_ERROR_DRAM_BUNDLE_OVERSIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_SRAM_16B_CNT_OVRF, 0,0,qux_reprogram_resource,NULL,"QUX_INT_DDP_ERROR_SRAM_16B_CNT_OVRF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_OMAC_CNT_OVRF, 0,0,qux_reprogram_resource,NULL,"QUX_INT_DDP_ERROR_OMAC_CNT_OVRF");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_UNPACK_PACKET_SIZE_ERROR, 0,0,qux_hard_reset,NULL,"QUX_INT_DDP_ERROR_UNPACK_PACKET_SIZE_ERROR");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_UNPACK_PACKET_SIZE_MISMATCH, 0,0,qux_hard_reset,NULL,"QUX_INT_DDP_ERROR_UNPACK_PACKET_SIZE_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ERROR_FBC_FATAL, 0,0,qux_hard_reset,NULL,"QUX_INT_DDP_ERROR_FBC_FATAL");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_DELETE_BUFF_FIFO_FULL, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO and consider each buffer in it whether it may be returned to the free buffers pool.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_DELETE_BUFF_FIFO_NOT_EMPTY, 0,0,qux_special_handling_normal_0,NULL,"the sw process should clear the FIFO and consider each buffer in it whether it may be returned to the free buffers pool.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_DDP_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_EXT_MEM_ERR_FBC_ECC_1B_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_DDP_EXT_MEM_ERR_FBC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_1B_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_CRC_ERR_INT, 0,0,qux_hard_reset,NULL,"QUX_INT_DDP_EXT_MEM_ERR_PKUP_CPYDAT_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_DDP_EXT_MEM_ERR_PKUP_PACKET_CRC_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_DDP_EXT_MEM_ERR_PKUP_PACKET_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_FTMH_PKT_SIZE_IS_NOT_STAMPPED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_FTMH_IS_NOT_STAMPPED, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_FTMH_IS_NOT_STAMPPED");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_BYTES_TO_ADD_ABOVE_MAX");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_BYTES_TO_REMOVE_ABOVE_PSIZE");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_FTMH_PSIZE_MISMATCH, 0,0,qux_hard_reset,NULL,"QUX_INT_ITE_ERR_FTMH_PSIZE_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_PSIZE_MISMATCH, 0,0,qux_hard_reset,NULL,"QUX_INT_ITE_ERR_PSIZE_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_0_MISMATCH, 0,0,qux_none,NULL,"QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_0_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_1_MISMATCH, 0,0,qux_none,NULL,"QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_1_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_2_MISMATCH, 0,0,qux_none,NULL,"QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_2_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_3_MISMATCH, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_3_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_4_MISMATCH, 0,0,qux_none,NULL,"QUX_INT_ITE_ERR_ITPP_PSIZE_TYPE_4_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ERR_ITPP_EEI_PSIZE_MISMATCH, 0,0,qux_reprogram_resource,NULL,"QUX_INT_ITE_ERR_ITPP_EEI_PSIZE_MISMATCH");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_ITE_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ITE_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CGM_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_CGM_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CGM_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_CGM_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ERROR_FPC_FREE, 0,0,qux_special_handling_normal_7,NULL,"find missconfiguratoin. Reconfigure and reset the device.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ERROR_OUT_OF_BUFFER, 0,0,qux_none,NULL,"QUX_INT_ILB_ERROR_OUT_OF_BUFFER");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ERROR_NO_EOP, 0,0,qux_none,NULL,"QUX_INT_ILB_ERROR_NO_EOP");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ERROR_SHARED_COUNTER_OVERFLOW, 0,0,qux_special_handling_normal_7,NULL,"find missconfiguratoin. Reconfigure and reset the device.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ERROR_SHARED_COUNTER_UNDERFLOW, 0,0,qux_special_handling_normal_7,NULL,"find missconfiguratoin. Reconfigure and reset the device.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_ILB_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_0, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_0");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_1, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_1");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_2, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_2");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_3, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_3");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_4, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_4");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_5, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_5");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_6, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_6");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_7, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_7");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_8, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_8");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_9, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_9");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_10, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_10");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_11, 0,0,qux_none,NULL,"QUX_INT_ILB_MISSING_FRAGMENT_ERROR_MISSING_FRAGMENT_ERROR_11");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_0, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_0");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_1, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_1");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_2, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_2");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_3, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_3");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_4, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_4");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_5, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_5");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_6, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_6");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_7, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_7");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_8, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_8");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_9, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_9");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_10, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_10");
    dcmn_intr_add_handler_ext(unit, QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_11, 0,0,qux_none,NULL,"QUX_INT_ILB_FLUSH_ERROR_FLUSH_ERROR_11");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IDB_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_IDB_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IDB_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_IDB_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PTS_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_PTS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PTS_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PTS_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PEM_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_PEM_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PEM_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_1,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PEM_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_2,qux_special_handling_recur_3,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");


    
    dcmn_intr_add_handler_ext(unit, QUX_INT_DRCA_PHY_CDR_ABOVE_TH,                              0,      0,  qux_event_drc_phy_cdr_above_th_handler,     NULL, "DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION");
    dcmn_intr_add_handler(unit, QUX_INT_OAMP_PENDING_EVENT,                                 0,      0,  jer_interrupt_handle_oamppendingevent,      NULL);
    dcmn_intr_add_handler(unit, QUX_INT_OAMP_STAT_PENDING_EVENT,                            0,      0,  jer_interrupt_handle_oamppendingstatevent,  NULL);
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_ECC_PARITY_ERR_INT, 0,0,qux_none,NULL,"QUX_INT_KAPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_ECC_ECC_1B_ERR_INT, 1000,100,qux_scrub_shadow_write,qux_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_KAPS_ECC_ECC_2B_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PEM_ECC_PARITY_ERR_INT, 1000,100,qux_special_handling_normal_4,qux_special_handling_recur_5,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, QUX_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,qux_interrupt_handle_tcamprotectionerror,NULL,"QUX_INT_PPDB_A_TCAM_PROTECTION_ERROR");

    dcmn_intr_add_clear_ext(unit, QUX_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, soc_interrupt_clear_fid_limit_reached_event);     
}

#undef _ERR_MSG_MODULE_NAME
