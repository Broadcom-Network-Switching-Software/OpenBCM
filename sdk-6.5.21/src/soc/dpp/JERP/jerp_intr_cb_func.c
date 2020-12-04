/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for JER interrupts.
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

#include <soc/dpp/JERP/jerp_intr.h>
#include <soc/dpp/JERP/jerp_intr_cb_func.h>
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


int jerp_intr_handle_generic_none(int unit, int block_instance, jerp_interrupt_type en_jerp_interrupt,char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    
    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_jerp_interrupt, 0, DCMN_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance, en_jerp_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int jerp_intr_recurring_action_handle_generic_none(int unit, int block_instance, jerp_interrupt_type en_jerp_interrupt, char *msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    
    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_jerp_interrupt, 1, DCMN_INT_CORR_ACT_NONE, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance, en_jerp_interrupt, msg, DCMN_INT_CORR_ACT_NONE, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
 int
 jerp_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
 jerp_hard_reset_w_fabric(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {

    int rc = 0;
    char info_msg[240];
    SOCDNX_INIT_FUNC_DEFS;
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SOFT_RESET, info_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, DCMN_INT_CORR_ACT_SOFT_RESET , NULL,NULL);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jerp_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
 jerp_reprogram_resource(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
 jerp_scrub_shadow_write(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
     if (!soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_asic_debug", 0)) {
         rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,NULL);
         SOCDNX_IF_ERR_EXIT(rc);
     }

exit:
    SOCDNX_FUNC_RETURN;
 }
STATIC
 int
 jerp_shutdown_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
 {
     char info_msg[240];
     int rc = 0;
     SOCDNX_INIT_FUNC_DEFS;
     rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, info_msg);
     SOCDNX_IF_ERR_EXIT(rc);
     rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS , NULL,info_msg);
     SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
 }


STATIC
int jerp_interrupt_handle_tcamprotectionerror(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg)
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
int jerp_interrupt_handle_kapstcamerror(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    int index = 0;
    dcmn_int_corr_act_type corrective_action = DCMN_INT_CORR_ACT_NONE;
    char special_msg[DCMN_INTERRUPT_SPECIAL_MSG_SIZE] = {0};
    uint32 regVal, regVal_2;
    int invalidate_n, corrected_n;
    int invalidated_entry=0, corrected_entry=0;

    SOCDNX_INIT_FUNC_DEFS;

    
    switch (en_interrupt) {
        case JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0:
            index = 0;
            break;
        case JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1:
            index = 1;
            break;
        case JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_2:
            index = 2;
            break;
        case JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_3:
            index = 3;
            break;
        default:
            break;
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

    
    rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, corrective_action, special_msg);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = dcmn_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, corrective_action, NULL, NULL);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int
jerp_special_handling_normal_0(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_1(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
     if (!soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_asic_debug", 0)) {
         rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
         SOCDNX_IF_ERR_EXIT(rc);
     }
 exit:
     SOCDNX_FUNC_RETURN;
}
STATIC
int
jerp_special_handling_recur_2(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_recur_3(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_4(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
     if (!soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_asic_debug", 0)) {
         rc = dcmn_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
         SOCDNX_IF_ERR_EXIT(rc);
     }
 exit:
     SOCDNX_FUNC_RETURN;
}
STATIC
int
jerp_special_handling_recur_5(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_recur_6(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_7(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_8(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_9(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_10(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_11(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_recur_12(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_13(int unit, int block_instance, uint32 en_interrupt, char *msg)
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
jerp_special_handling_normal_14(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
   

   int rc = 0;

   SOCDNX_INIT_FUNC_DEFS;
   rc = dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_NONE, msg);
   SOCDNX_IF_ERR_EXIT(rc);


exit:
   SOCDNX_FUNC_RETURN;
}

int jerp_event_drc_rd_wr_crc_handler(int unit, int block_instance, uint32 en_interrupt, char* msg)
{
    
    jer_interrupt_type drc_interrupt_arr[] = {JERP_INT_DRCA_DRAM_WR_CRC_ERR, JERP_INT_DRCB_DRAM_WR_CRC_ERR, JERP_INT_DRCC_DRAM_WR_CRC_ERR, JERP_INT_DRCD_DRAM_WR_CRC_ERR,
                                              JERP_INT_DRCE_DRAM_WR_CRC_ERR, JERP_INT_DRCF_DRAM_WR_CRC_ERR, JERP_INT_DRCG_DRAM_WR_CRC_ERR, JERP_INT_DRCH_DRAM_WR_CRC_ERR,
                                              JERP_INT_DRCA_DRAM_RD_CRC_ERR, JERP_INT_DRCB_DRAM_RD_CRC_ERR, JERP_INT_DRCC_DRAM_RD_CRC_ERR, JERP_INT_DRCD_DRAM_RD_CRC_ERR,
                                              JERP_INT_DRCE_DRAM_RD_CRC_ERR, JERP_INT_DRCF_DRAM_RD_CRC_ERR, JERP_INT_DRCG_DRAM_RD_CRC_ERR, JERP_INT_DRCH_DRAM_RD_CRC_ERR };
    
    int is_enabled[] =                       {0, 0, 0, 0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0, 0, 0, 0 };
    int drc_interrupt_arr_size;
    int drc_ndx = 0;
    int interrupt_index;
    int max_nof_drc_interfaces = 0;
    int interrupt_occured;
    soc_interrupt_db_t interrupt_info;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_IF_ERR_EXIT( dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_PRINT, NULL));    

    
    if (SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_type != SHMOO_COMBO28_DRAM_TYPE_GDDR5)
    {
        SOC_EXIT;
    }
    
    
    drc_interrupt_arr_size = sizeof(drc_interrupt_arr) / sizeof(drc_interrupt_arr[0]);

    
    max_nof_drc_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
    
    
    
    for (interrupt_index = 0; interrupt_index < drc_interrupt_arr_size; ++interrupt_index)
    {
        if (drc_interrupt_arr[interrupt_index] == en_interrupt)
        {
            drc_ndx = interrupt_index % max_nof_drc_interfaces;
            break;
        }
    }
    
    
    for (interrupt_index = drc_ndx; interrupt_index < drc_interrupt_arr_size; interrupt_index += max_nof_drc_interfaces)
    {
        
        if (drc_interrupt_arr[interrupt_index] == en_interrupt)
        {
            continue;
        }
        LOG_VERBOSE (BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s(): clearing and masking interrupt %d"), FUNCTION_NAME(), drc_interrupt_arr[interrupt_index]));
        
        SOCDNX_IF_ERR_EXIT( soc_interrupt_info_get(unit, drc_interrupt_arr[interrupt_index], &interrupt_info));
        
        SOCDNX_IF_ERR_EXIT( soc_interrupt_is_enabled(unit, 0, &interrupt_info, &(is_enabled[interrupt_index])));
        
        if (is_enabled[interrupt_index])
        {
            SOCDNX_IF_ERR_EXIT( soc_interrupt_disable(unit, 0, &interrupt_info));
        }
        
        SOCDNX_IF_ERR_EXIT( soc_interrupt_get(unit, 0, &interrupt_info, &interrupt_occured));
        if (interrupt_occured)
        {
            SOCDNX_IF_ERR_EXIT( dcmn_interrupt_print_info(unit, block_instance, drc_interrupt_arr[interrupt_index], 0, DCMN_INT_CORR_ACT_PRINT, "interrupt cleared without action as part of handling another interrupt"));
        }
        
        SOCDNX_IF_ERR_EXIT( soc_interrupt_clear_on_write(unit, block_instance, drc_interrupt_arr[interrupt_index]));
    }
    
    
    LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s(): activating corrective action sequence on DRC index %d"), FUNCTION_NAME(), drc_ndx));
    SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_rd_wr_crc_handle_interrupt(unit, drc_ndx));
    
        
    
    for (interrupt_index = drc_ndx; interrupt_index < drc_interrupt_arr_size; interrupt_index += max_nof_drc_interfaces)
    {
        
        if (drc_interrupt_arr[interrupt_index] == en_interrupt)
        {
            continue;
        }
        
        LOG_VERBOSE(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s(): unmasking interrupt %d"), FUNCTION_NAME(), drc_interrupt_arr[interrupt_index]));
        
        SOCDNX_IF_ERR_EXIT( soc_interrupt_info_get(unit, drc_interrupt_arr[interrupt_index], &interrupt_info));
        
        if (is_enabled[interrupt_index])
        {
            SOCDNX_IF_ERR_EXIT( soc_interrupt_enable(unit, 0, &interrupt_info));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int jerp_event_drc_phy_cdr_above_th_handler(int unit, int block_instance, uint32 en_interrupt, char* msg)
{
    jerp_interrupt_type drc_phy_cdr_above_th_arr[] = { JERP_INT_DRCA_PHY_CDR_ABOVE_TH, JERP_INT_DRCB_PHY_CDR_ABOVE_TH, JERP_INT_DRCC_PHY_CDR_ABOVE_TH, JERP_INT_DRCD_PHY_CDR_ABOVE_TH,
                                                       JERP_INT_DRCE_PHY_CDR_ABOVE_TH, JERP_INT_DRCF_PHY_CDR_ABOVE_TH, JERP_INT_DRCG_PHY_CDR_ABOVE_TH, JERP_INT_DRCH_PHY_CDR_ABOVE_TH };
    int drc_phy_cdr_above_th_arr_size;
    int drc_ndx;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT( dcmn_interrupt_print_info(unit, block_instance, en_interrupt, 0, DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION, NULL));

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

STATIC dcmn_intr_action_t jerp_intr_action[] =
{
    {jerp_intr_handle_generic_none, DCMN_INT_CORR_ACT_NONE},
    {jerp_intr_recurring_action_handle_generic_none, DCMN_INT_CORR_ACT_NONE},
    {jerp_hard_reset, DCMN_INT_CORR_ACT_HARD_RESET},
    {jerp_hard_reset_w_fabric, DCMN_INT_CORR_ACT_SOFT_RESET},
    {jerp_none, DCMN_INT_CORR_ACT_NONE},
    {jerp_reprogram_resource, DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE},
    {jerp_scrub_shadow_write, DCMN_INT_CORR_ACT_SHADOW},
    {jerp_shutdown_link, DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS},
    {jerp_event_drc_phy_cdr_above_th_handler, DCMN_INT_CORR_ACT_DYNAMIC_CALIBRATION},
    {jerp_special_handling_normal_0, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_1, DCMN_INT_CORR_ACT_SHADOW},
    {jerp_special_handling_recur_2, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_recur_3, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_4, DCMN_INT_CORR_ACT_SHADOW},
    {jerp_special_handling_recur_5, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_recur_6, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_7, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_8, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_9, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_10, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_11, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_recur_12, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_13, DCMN_INT_CORR_ACT_NONE},
    {jerp_special_handling_normal_14, DCMN_INT_CORR_ACT_NONE},
    {jerp_interrupt_handle_tcamprotectionerror, DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB},
    {NULL, DCMN_INT_CORR_ACT_NONE}
};

void jerp_interrupt_cb_init(int unit)
{
    sand_intr_action_info_set(unit, jerp_intr_action);
    
    
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_CNM_DRAM_PKT_TOO_SMALL_ENCP, 0,0,jerp_none,NULL,"JERP_INT_IPT_CNM_DRAM_PKT_TOO_SMALL_ENCP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_CNM_PACKET_DELETED, 0,0,jerp_none,NULL,"JERP_INT_IPT_CNM_PACKET_DELETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_0, 0,0,jerp_none,NULL,"JERP_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_1, 0,0,jerp_none,NULL,"JERP_INT_IPT_OCB_BUFFER_IN_DRAM_RANGE_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_LARGE_LATENCY_PKT_DROP, 0,0,jerp_none,NULL,"JERP_INT_IPT_LARGE_LATENCY_PKT_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_OCB_ONLY_DATA_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_OCB_MIX_0_DATA_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_OCB_MIX_1_DATA_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"bad configuration, should re-tune the threshold to stop (prophet counter)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPT_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_DQCQ_DEPTH_OVF, 0,0,jerp_special_handling_normal_0,NULL,"fix the configuration in the faulted dqcq depth registers and resume traffic.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_EMPTY_DQCQ_WRITE, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_QUEUE_ENTERED_DEL, 0,0,jerp_none,NULL,"Monitor queue number (0x0149)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_CREDIT_LOST, 0,0,jerp_none,NULL,"JERP_INT_IPS_CREDIT_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_CREDIT_OVERFLOW, 0,0,jerp_none,NULL,"JERP_INT_IPS_CREDIT_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_DEQ_COMMAND_TIMEOUT, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_DQCQ_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_PUSH_QUEUE_ACTIVE, 0,0,jerp_none,NULL,"JERP_INT_IPS_PUSH_QUEUE_ACTIVE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_ENQ_BLOCK_OVERFLOW, 1000,1,jerp_none,jerp_special_handling_recur_6,"JERP_INT_IPS_ENQ_BLOCK_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_CR_FLW_ID_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IPS_CR_FLW_ID_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPS_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_SMP_FULL_LEVEL_1, 0,0,jerp_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_SMP_FULL_LEVEL_2, 0,0,jerp_reprogram_resource,NULL,"Fix the IPS configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_REBOUND_FIFO_CR_LOSS, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_SCH_REBOUND_FIFO_CR_LOSS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_SCH_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 1,1000000,jerp_shutdown_link,jerp_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 1,1000000,jerp_shutdown_link,jerp_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 1,1000000,jerp_shutdown_link,jerp_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 1,1000000,jerp_shutdown_link,jerp_none,"Read CRC error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 1,1000000,jerp_shutdown_link,jerp_none,"JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 1,1000000,jerp_shutdown_link,jerp_none,"JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 1,1000000,jerp_shutdown_link,jerp_none,"JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 1,1000000,jerp_shutdown_link,jerp_none,"JERP_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_LOS_INT_0, 0,0,jerp_special_handling_normal_7,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_LOS_INT_1, 0,0,jerp_special_handling_normal_7,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_LOS_INT_2, 0,0,jerp_special_handling_normal_7,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_LOS_INT_3, 0,0,jerp_special_handling_normal_7,NULL,"Tune SerDes If not resolved, shutdown link ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_OOF_INT_0, 1,100,jerp_special_handling_normal_8,jerp_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_OOF_INT_1, 1,100,jerp_special_handling_normal_8,jerp_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_OOF_INT_2, 1,100,jerp_special_handling_normal_8,jerp_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_OOF_INT_3, 1,100,jerp_special_handling_normal_8,jerp_none,"if pcs mode is 9/10 or pcs mode is 64/66 KR{  shutdown link} else { ignore}");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 1,1000000,jerp_special_handling_normal_9,jerp_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 1,1000000,jerp_special_handling_normal_9,jerp_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 1,1000000,jerp_special_handling_normal_9,jerp_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 1,1000000,jerp_special_handling_normal_9,jerp_none,"If pcs mode is KR{ read int_info_csrs_prop1 registers } set int_info_csrs_prp2 registers to count errors on the relevant lane.  Read error counter to get exact rate, and shutdown link when threshold is reached.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0,0,jerp_special_handling_normal_10,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0,0,jerp_special_handling_normal_10,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0,0,jerp_special_handling_normal_10,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0,0,jerp_special_handling_normal_10,NULL,"Handling depands on PCS mode: 8/10: Shutdown link else: None");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_0, 0,0,jerp_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_1, 0,0,jerp_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_2, 0,0,jerp_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_5_RX_CTRL_OVERFLOW_INT_3, 0,0,jerp_special_handling_normal_0,NULL,"Configure registers 0x0062 at link partner.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0,0,jerp_hard_reset,NULL,"JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0,0,jerp_hard_reset,NULL,"JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0,0,jerp_hard_reset,NULL,"JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0,0,jerp_hard_reset,NULL,"JERP_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_FMAC_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FMAC_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_1_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_0, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_1, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_2, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_2_FSRD_N_SYNC_STATUS_CHANGED_3, 0,0,jerp_special_handling_normal_0,NULL,"Tune SerDes");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_FSRD_QUAD_0_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_FSRD_QUAD_1_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_FSRD_QUAD_2_FSRD_N_TXPLL_LOCK_CHANGED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_FSRD_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FSRD_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps0OvfCntrsCnt (0x0245) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x221). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_0_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_0_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_0_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_0_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps1OvfCntrsCnt (0x0246) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x222). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_1_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_1_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_1_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_1_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps2OvfCntrsCnt (0x0247) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x223). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_2_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_2_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_2_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_2_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps3OvfCntrsCnt (0x0248) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x224). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_3_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_3_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_3_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_3_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps4OvfCntrsCnt (0x0249) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x225). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_4_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_4_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_4_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_4_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps5OvfCntrsCnt (0x024A) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x226). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_5_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_5_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_5_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_5_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps6OvfCntrsCnt (0x024B) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x227). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_6_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_6_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_6_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_6_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps7OvfCntrsCnt (0x024C) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x228). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_7_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_7_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_7_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_7_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps8OvfCntrsCnt (0x024D) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x229). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_8_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_8_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_8_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_8_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps9OvfCntrsCnt (0x024E) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22A). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_9_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_9_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_9_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_9_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps10OvfCntrsCnt (0x024F) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22B). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_10_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_10_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_10_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_10_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps11vfCntrsCnt (0x0250) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22C). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_11_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_11_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_11_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_11_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps12OvfCntrsCnt (0x0251) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22D). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_12_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_12_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_12_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_12_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps13OvfCntrsCnt (0x0252) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22E). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_13_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_13_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_13_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_13_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps14OvfCntrsCnt (0x0253) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x22F). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_14_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_14_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_14_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_14_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps15OvfCntrsCnt (0x0254) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x230). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_15_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_15_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_15_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_15_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps16OvfCntrsCnt (0x0255) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x231). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_16_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_16_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_16_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_16_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_CNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"The sw process should read Crps17OvfCntrsCnt (0x0256) . If the value is 1 then the counter value may still be restored (as the counter overflowed only once) according to the address and data in CrpsCntOvrfSts (0x232). If greater than 1 then the entire database is unreliable.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_INVLD_PTR_ACC, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_17_INVLD_PTR_ACC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_PRE_READ_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"the sw process should clear the FIFO (by reading counters values) to allow more counters to be written to it.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_PRE_READ_FIFO_NOT_EMPTY, 0,0,jerp_none,NULL,"the driver needs to access this FIFO and read it.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_DIRECT_RD_WHEN_WAITING, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_17_DIRECT_RD_WHEN_WAITING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ENGINE_CRPS_17_CNT_ECC_ERR, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ENGINE_CRPS_17_CNT_ECC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI0 configuration for command EPNI0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EPNI_0_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI0 configuration for command EPNI0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI1 configuration for command EPNI1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EPNI_1_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI1 configuration for command EPNI1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0 configuration for command EGQ0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0 configuration for command EGQ0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1 configuration for command EGQ1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1 configuration for command EGQ1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP0 configuration for command IRPP0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IRPP_0_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP0 configuration for command IRPP0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP1 configuration for command IRPP1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IRPP_1_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP1 configuration for command IRPP1-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM0 configuration for command IQM0-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IQM_0_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM0 configuration for command IQM0-B.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_A_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM1 configuration for command IQM1-A.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_IQM_1_B_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM1 configuration for command IQM1-B. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_0_TM_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0TM configuration for command EGQ0TM.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_INVLID_ACCESS_EGQ_1_TM_INVLD_AD_ACC, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1TM configuration for command EGQ1TM. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI0  DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_0_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EPNI_1_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EPNI1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_0_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IRPP_1_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IRPP1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_0_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM0 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_A_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM1 DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_IQM_1_B_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check IQM1 DoNotCountOffset configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_0_TM_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ0TM DoNotCountOffset configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_SRC_CMD_WAS_FILTERED_EGQ_1_TM_OFFSET_WAS_FILT, 0,0,jerp_special_handling_normal_0,NULL,"Check EGQ1TM DoNotCountOffset configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_CRPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CRPS_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MRPS_MCDA_WRAP, 0,0,jerp_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MRPS_MCDB_WRAP, 0,0,jerp_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MRPS_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_MRPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MRPS_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MRPS_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MTRPS_EM_MCDA_WRAP, 0,0,jerp_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MTRPS_EM_MCDB_WRAP, 0,0,jerp_special_handling_normal_0,NULL,"Possible configuration issue.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MTRPS_EM_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_MTRPS_EM_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MTRPS_EM_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MTRPS_EM_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_TX_FIFO_OVERFLOW_INT, 0,0,jerp_special_handling_normal_0,NULL,"This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF\nCheck also HrfTx<n>NumCreditsToEgq.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_RX_FIFO_OVERFLOW_INT, 0,0,jerp_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_WRONG_WORD_FROM_PM_INT, 0,0,jerp_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_ERR_BITS_FROM_EGQ_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_ERR_BITS_FROM_EGQ_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_WRONG_EGQ_WORD_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_NBIH_WRONG_EGQ_WORD_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_ILLEGAL_PORT_MAP_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_NBIH_ILLEGAL_PORT_MAP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_EGQ_0_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_NBIH_EGQ_0_TRAFFIC_TO_INACTIVE_PORT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_EGQ_1_TRAFFIC_TO_INACTIVE_PORT_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_NBIH_EGQ_1_TRAFFIC_TO_INACTIVE_PORT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_TX_MISSING_SOB_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_TX_MISSING_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_TX_DOUBLE_SOB_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_TX_DOUBLE_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_RX_ELK_0_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_RX_ELK_0_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_RX_ELK_1_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_RX_ELK_1_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_SYNC_ETH_0_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_SYNC_ETH_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_SYNC_ETH_1_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_SYNC_ETH_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_4");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_5");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_6");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jerp_none,NULL,"JERP_INT_NBIH_HRF_FORCE_MERGE_ERROR_INT_7");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jerp_none,NULL,"JERP_INT_NBIH_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_2_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_2_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_3_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIH_NBI_PKT_DROP_COUNTERS_75P_PKT_DROP_COUNTER_75P_RX_HRF_3_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIH_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_TX_FIFO_OVERFLOW_INT, 0,0,jerp_special_handling_normal_0,NULL,"This interrupt means that the EGQ configuration of max number of entries in the Tx FIFO does not match the number of credits send from NIF\nCheck also HrfTx<n>NumCreditsToEgq.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_RX_FIFO_OVERFLOW_INT, 0,0,jerp_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_WRONG_WORD_FROM_PM_INT, 0,0,jerp_none,NULL,"y");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_ILLEGAL_INPUT_FROM_GPORT_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_ILLEGAL_INPUT_FROM_GPORT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_TX_MISSING_SOB_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_TX_MISSING_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_TX_DOUBLE_SOB_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_TX_DOUBLE_SOB_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_RX_ELK_0_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_RX_ELK_0_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_RX_ELK_1_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_RX_ELK_1_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT, 0,0,jerp_none,NULL,"JERP_INT_NBIL_TIMESTAMP_1588_FIFO_NOT_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_0, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_1, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_2, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_3, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_4, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_4");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_5, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_5");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_6, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_6");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_7, 0,0,jerp_none,NULL,"JERP_INT_NBIL_HRF_FORCE_MERGE_ERROR_INT_7");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_4");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_5");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_6");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_7");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_8");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_9");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_10");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_11");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_12");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_13");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_14");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_15");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_16");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_17");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_18");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_19");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_20");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_21");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_22");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_23");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_24, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_24");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_25, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_25");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_26, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_26");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_27, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_27");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_28, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_28");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_29, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_29");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_30, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_30");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_31, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_31");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_32, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_32");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_33, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_33");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_34, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_34");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_35, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_35");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_36, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_36");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_37, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_37");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_38, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_38");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_39, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_39");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_40, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_40");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_41, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_41");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_42, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_42");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_43, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_43");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_44, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_44");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_45, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_45");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_46, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_46");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_47, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_47");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_48, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_48");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_49, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_49");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_50, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_50");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_51, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_51");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_52, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_52");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_53, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_53");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_54, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_54");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_55, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_55");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_56, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_56");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_57, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_57");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_58, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_58");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_59, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_59");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_60, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_60");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_61, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_61");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_62, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_62");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_63, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_63");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_64, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_64");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_65, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_65");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_66, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_66");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_67, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_67");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_68, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_68");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_69, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_69");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_70, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_70");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_71, 0,0,jerp_none,NULL,"JERP_INT_NBIL_LINK_STATUS_CHANGE_LINK_STATUS_CHANGE_INTERRUPT_REGISTER_71");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_NBIL_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_INT_UCFIFO_AF, 0,0,jerp_none,NULL,"JERP_INT_EGQ_INT_UCFIFO_AF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_VLAN_EMPTY_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_VLAN_EMPTY_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_DROPPED_UC_PD_INT, 0,0,jerp_special_handling_normal_0,NULL,"Check tresholds of of packet descriptors per Port / TC / Queue / Interface or Total. The CGM registers Unicast Packet Descriptors Drop Counter, Congestion Tracking Unicast PDMax Value and Unicast Packet Descriptors Counter should be read to analaze the status.    Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_DROPPED_UC_DB_INT, 0,0,jerp_special_handling_normal_0,NULL,"Check tresholds of of data buffers per Port / TC / QUE / Interface or Total.The CGM registers Unicast Data Buffers Drop Counter (RQP / PQP), Congestion Tracking Unicast DBMax Value and Unicast Data Buffers Counter should be read to analaze the status.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_UC_PKT_PORT_FF, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EGQ_UC_PKT_PORT_FF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_DELETE_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"Check that the configuration of the EGQ does not result in too many discards. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ILLEGAL_PKT_SIZE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ILLEGAL_PKT_SIZE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_TC_MAPPING_MISS_CONFIG, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EGQ_TC_MAPPING_MISS_CONFIG");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ALL_DATA_BUFFERS_ALLOCATED_INT, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EGQ_ALL_DATA_BUFFERS_ALLOCATED_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_MC_MAX_REPLICATION_OVERFLOW_INT, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EGQ_MC_MAX_REPLICATION_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_MID_REP_DROP, 0,0,jerp_none,NULL,"JERP_INT_EGQ_MID_REP_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_TDM_HP_MC_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_TDM_HP_MC_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_TDM_LP_MC_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_TDM_LP_MC_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_TDM_PACKET_SIZE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_TDM_PACKET_SIZE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_BOUNCE_BACK_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_INVALID_OTM_INT, 0,0,jerp_special_handling_normal_0,NULL,"Verify the ingress configuration of the device the packet was recived from.   Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_DSS_STACKING_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_EXCLUDE_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_LAG_MULTICAST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_VLAN_MEMBERSHIP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_UNACCEPTABLE_FRAME_TYPE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_SRC_EQUAL_DEST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_UNKNOWN_DA_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_SPLIT_HORIZON_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_PRIVATE_VLAN_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_TTL_SCOPE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_MTU_VIOLATION_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_TRILL_TTL_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_TRILL_SAME_INTERFACE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_CNM_INTERCEPT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_VERSION_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_CHECKSUM_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_HEADER_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_TOTAL_LENGTH_ERROR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ONE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_OPTIONS_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_TTL_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_SIP_EQUALS_DIP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_DIP_EQUALS_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV4_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_SIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_DST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_IPV6_UNSPECIFIED_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LOOPBACK_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_HOP_BY_HOP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_DST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_LINK_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_SITE_LOCAL_SRC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_COMPATIBLE_DST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_IPV4_MAPPED_DST_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT, 0,0,jerp_none,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_IPV6_DIP_IS_MC_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ERPP_DISCARD_2_TDM_WRONG_PORT_INT, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EGQ_ERPP_DISCARD_2_TDM_WRONG_PORT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_PKT_SIZE_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_FRAG_NUM_ZERO_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_PCP_LOC_NOT_IN_CELL_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_NO_EOP_ON_EOC_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_FDR_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_NON_PCP_SRC_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_SINGLE_CELL_SRC_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_SOP_CELL_SIZE_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CDC_SOP_CELL_OVERSIZE_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_SOP_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_EOP_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_MISSING_CELL_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_TDM_MISMATCH_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_PKT_SIZE_MISMATCH_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CUP_MOP_CELL_SIZE_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_CRC_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_RCM_FAP_ID_NOT_VALID, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_RCM_ALL_CONTEXTS_TAKEN_DISCARD_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_RCM_DYNAMIC_MISSING_SOP_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_PACKET_REASSEMBLY_RCM_STATIC_MISS_CONFIG_ERR, 1000,1,jerp_none,jerp_special_handling_recur_6,"Verify that Fabric links are not experiencing any fault");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EGQ_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_MIRR_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_EPNI_MIRR_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_TRAP_OVF_INT, 0,0,jerp_none,NULL,"JERP_INT_EPNI_TRAP_OVF_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_EEI_TYPE_ERR, 0,0,jerp_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_PHP_ERR, 0,0,jerp_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_PHP_NEXT_IP_PROTOCOL_ERR, 0,0,jerp_special_handling_normal_0,NULL,"Verify the configuration of ingress PP generation of system headers.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_ERR, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_SNOOP, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_FIRST_ENC_MPLS_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_ERR, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_SNOOP, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_IP_TUNNEL_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_ERR, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_SNOOP, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_TRILL_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_ERR, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_SNOOP, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_EPNI_PP_SECOND_ENC_MPLS_HEADER_CODE_SNOOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_NEW_DA_ERR, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_NEW_DA_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_STP_STATE_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_STP_STATE_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_ACCEPTABLE_FRAME_TYPE_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_ACCEPTABLE_FRAME_TYPE_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_EES_ACTION_DROP_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_EES_ACTION_DROP_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_MTU_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_MTU_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_EES_LAST_ACTION_NOT_AC, 0,0,jerp_special_handling_normal_0,NULL,"PP?Configuration error, SW Proccess should alert configuration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_SPLIT_HORIZON_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_SPLIT_HORIZON_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_PROTECTION_PATH_DENY, 0,0,jerp_none,NULL,"JERP_INT_EPNI_PP_PROTECTION_PATH_DENY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_PP_GLEM_DENY, 0,0,jerp_special_handling_normal_0,NULL,"PP?Configuration error, SW Proccess should alert configuration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EPNI_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_CAM_TABLE_FULL, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_CAM_TABLE_FULL, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_TABLE_COHERENCY, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_TABLE_COHERENCY, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_DELETE_UNKNOWN_KEY, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_DELETE_UNKNOWN_KEY, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_WARNING_INSERTED_EXISTING, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_WARNING_INSERTED_EXISTING, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_0_ONE_ESEM_0_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_0_ONE_ESEM_0_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ESEM_1_ONE_ESEM_1_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_EDB_ESEM_1_ONE_ESEM_1_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_CAM_TABLE_FULL");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_UNIT_FAILURE_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_EDB_GLEM_ONE_GLEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_0_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_1_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_2_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_2_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_3_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_3_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_4_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_0_POINTER_CONFLICT_EEDB_0_POINTER_CONFLICT_4_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_0_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_1_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_2_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_2_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_3_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_3_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_4_INT, 0,0,jerp_none,NULL,"JERP_INT_EDB_EEDB_1_POINTER_CONFLICT_EEDB_1_POINTER_CONFLICT_4_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OLP_ERROR_EGRESS_PIPE_CFG, 0,0,jerp_special_handling_normal_0,NULL,"If set then One of the pipes was configured to expect the traffic but the other pipe actually received it.\nCleared upon read.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OLP_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OLP_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OLP_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_PENDING_EVENT, 0,0,jerp_special_handling_normal_0,NULL,"Pop message from event fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_STAT_PENDING_EVENT, 0,0,jerp_special_handling_normal_0,NULL,"Pop message from stat fifo by  reading  from register InterruptMessage (address 0x0140) till read data is 0xFFFFFF.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_RX_STATS_DONE, 0,0,jerp_special_handling_normal_0,NULL,"read SAT statistcics entry from the registers");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_RFC_6374_PKT_DROPPED, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_OAMP_RFC_6374_PKT_DROPPED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"Act according to the fail source specified in 0x0265, RmapemManagementUnitFailure");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_OAMP_REMOTE_MEP_EXACT_MATCH_ONE_RMAPEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OAMP_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_RX_PORT_0_STATUS_CHANGE_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PMH_ILKN_RX_PORT_0_STATUS_CHANGE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_RX_PORT_1_STATUS_CHANGE_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PMH_ILKN_RX_PORT_1_STATUS_CHANGE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_RX_PORT_1_STATUS_CHANGE_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PML_ILKN_RX_PORT_1_STATUS_CHANGE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_RX_PORT_0_STATUS_CHANGE_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PML_ILKN_RX_PORT_0_STATUS_CHANGE_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PMH_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PMH_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PML_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT, 0,0,jerp_none,NULL,"JERP_INT_ILKN_PML_ILKN_LINK_PARTNER_STATUS_CHANGE_ILKN_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_RX_0_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_RX_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_RX_1_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_RX_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_RX_1_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_RX_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_RX_0_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_RX_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_RX_0_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_RX_0_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_RX_1_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_RX_1_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_RX_1_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_RX_1_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_RX_0_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_RX_0_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_TX_0_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_TX_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_TX_1_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_TX_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_TX_1_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_TX_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_TX_0_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_TX_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_TX_1_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_TX_1_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ILKN_CORE_TX_0_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PMH_ILKN_CORE_TX_0_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_TX_0_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_TX_0_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ILKN_CORE_TX_1_SECOND_INT, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_ILKN_PML_ILKN_CORE_TX_1_SECOND_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ECC_ECC_2B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PMH_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ILKN_PML_ECC_ECC_2B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_MACA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_MACB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_IFMAFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_IFMBFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_A_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_ALTO_A, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRA_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_1_P_1_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRA_FILTER_DROP_INT_A, 0,0,jerp_none,NULL,"JERP_INT_FDR_FDR_1_P_1_FDRA_FILTER_DROP_INT_A");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRA_FILTER_DROP_INT_B, 0,0,jerp_none,NULL,"JERP_INT_FDR_FDR_1_P_1_FDRA_FILTER_DROP_INT_B");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_MACC_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_MACD_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_IFMAFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_IFMBFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_CPUDATACELLFNE_B_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_ALTO_B, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_1_P_1_FDRB_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_1_P_1_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_FDRA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_MACA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_MACB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_IFMAFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_IFMBFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_A_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_ALTO_A, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_FDRA_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_2_P_2_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_FDRB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_MACC_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_MACD_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_IFMAFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_IFMBFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_CPUDATACELLFNE_B_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_ALTO_B, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_2_P_2_FDRB_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_2_P_2_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_FDRA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_MACA_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_MACB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_IFMAFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_IFMBFO_A, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_A_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_ALTO_A, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_FDRA_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_3_P_3_FDRA_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_FDRB_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.   Interrupt handler prints a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_MACC_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_MACD_UN_EXP_CELL, 0,0,jerp_special_handling_normal_0,NULL,"Check thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_IFMAFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_IFMBFO_B, 1,1000000,jerp_special_handling_normal_0,jerp_special_handling_recur_6,"Adjust link level flow conrol thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_0, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_CPUDATACELLFNE_B_1, 0,0,jerp_special_handling_normal_0,NULL,"it indicates that at least one cell towards CPU got dropped. No action is required other than to log this error. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_ALTO_B, 10,10,jerp_special_handling_normal_11,jerp_special_handling_recur_12,"If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_3_P_3_FDRB_OUT_OF_SYNC, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_3_P_3_FDRB_OUT_OF_SYNC");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_1_ECC_1B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_FDR_FDR_4_P_1_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_4_P_1_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_2_ECC_1B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_FDR_FDR_4_P_2_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_4_P_2_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_3_ECC_1B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_FDR_FDR_4_P_3_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT, 0,0,jerp_hard_reset_w_fabric,NULL,"JERP_INT_FDR_FDR_4_P_3_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_IN_BAND_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"Reduce FDT inband shaper rate.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDR_FDR_4_FDR_UN_EXP_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"If Alto occurs, handle according to Alto. Might also be related to some misconfiguration in the system, need to collect dumps of all devices in system and analyze them.    Interrupt handler logs a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_PRIO_0_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_PRIO_0_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_PRIO_1_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_PRIO_1_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_PRIO_2_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_PRIO_2_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_0_FAB_0_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_0_FAB_0_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_0_FAB_1_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_0_FAB_1_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_0_FAB_2_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_0_FAB_2_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_1_FAB_0_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_1_FAB_0_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_1_FAB_1_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_1_FAB_1_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_1_FAB_2_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_1_FAB_2_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_0_MESHMC_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_0_MESHMC_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_1_MESHMC_OVF_DROP_INT, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_1_MESHMC_OVF_DROP_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_0_TDM_OVF_DROP, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_0_TDM_OVF_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_EGQ_1_TDM_OVF_DROP, 0,0,jerp_none,NULL,"JERP_INT_FDA_EGQ_1_TDM_OVF_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDA_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_SRC_DV_CNG_LINK_EV, 0,0,jerp_none,NULL,"JERP_INT_FCR_SRC_DV_CNG_LINK_EV");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_CPU_CNT_CELL_FNE, 0,0,jerp_special_handling_normal_0,NULL,"Read captured cell(s) from register ControlCellFIFOBuffer");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_LOCAL_ROUT_FS_OVF, 0,0,jerp_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_LOCAL_ROUTRC_OVF, 0,0,jerp_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_REACH_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"Check rechability message Generation Rate (RMGR) configuration of link partner. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_FLOW_FIFO_OVF, 0,0,jerp_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_CREDIT_FIFO_OVF, 0,0,jerp_none,NULL,"Nothing, this should not happen.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_MAC_PARITY_DROP, 0,0,jerp_none,NULL,"Nothing, this should not happen (unconnected in the design)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FCR_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_LINK_MASK_CHANGE, 0,0,jerp_special_handling_normal_13,NULL,"Driver should hold shadow of Link Active Mask register in order to understand what was changed.\nIf link goes up then this is not an error. \nIf link goes down: \nIf MAC link status is down then this is the problem\nelse, if local ACL is down then there is a problem with local Rx MAC\nelse, if remote ACL is down then there is an problem with remote Rx MAC\nelse, check if remote device was isolated\nelse, review local RTPWD and remote RMGR settings");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_TABLE_CHANGE, 0,0,jerp_special_handling_normal_0,NULL,"Check fabric rechability tables.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_DISCONNECT_INT, 0,0,jerp_none,NULL,"JERP_INT_RTP_DISCONNECT_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_RTP_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_UNRCH_DEST_EVT, 0,0,jerp_none,NULL,"Problem in rtp topology table or multicast bitmap. \nLink / destination device might be down.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_TDM_CRC_DROP, 0,0,jerp_none,NULL,"JERP_INT_FDT_TDM_CRC_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_ILEGAL_IRE_PACKET_SIZE_INT, 0,0,jerp_special_handling_normal_0,NULL,"Reprogram IRE filter (cell size should be 65B-256B).  Interrupt handler logs a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT, 0,0,jerp_none,NULL,"JERP_INT_FDT_IN_BAND_LAST_READ_CNT_ZERO_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_BURST_TOO_LARGE, 0,0,jerp_special_handling_normal_0,NULL,"Reprogram IQM burst size settings.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_SAT_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_SAT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_SAT_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_SAT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_SAT_2, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_SAT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_UDP_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_UDP_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_UDP_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_UDP_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_0_UDP_2, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_0_UDP_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_SAT_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_SAT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_SAT_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_SAT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_SAT_2, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_SAT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_UDP_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_UDP_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_UDP_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_UDP_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_1_UDP_2, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_1_UDP_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_CORE_SAT_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_CORE_SAT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_CORE_SAT_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_CORE_SAT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_CORE_UDP_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_CORE_UDP_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQ_CORE_UDP_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQ_CORE_UDP_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_0, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_0");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_1, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_1");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_2, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_2");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_3, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_3");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_4, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_4");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_5, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_5");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_6, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_6");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_7, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_7");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_WFQI_SAT_8, 0,0,jerp_reprogram_resource,NULL,"JERP_INT_FDT_WFQI_SAT_8");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_FDT_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_CR_ARB_DROP_IPS_0_INT, 0,0,jerp_none,NULL,"JERP_INT_IPST_CR_ARB_DROP_IPS_0_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_CR_ARB_DROP_IPS_1_INT, 0,0,jerp_none,NULL,"JERP_INT_IPST_CR_ARB_DROP_IPS_1_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_CR_MSG_ERR_QNUM_INT, 0,0,jerp_none,NULL,"JERP_INT_IPST_CR_MSG_ERR_QNUM_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IPST_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IPST_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_LOCK_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface did not lock, should verify that calendar length and number of multiplications in a frame match on both devices. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_LOCK_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface did not lock, should verify that calendar length and number of multiplications in a frame match on both devices. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_OUT_OF_FRM, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_OUT_OF_FRM, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_DIP_2_ALARM, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_DIP_2_ALARM, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Interrupt handler  log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_FRM_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_FRM_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong size of frame. Should verify the calendar length and number of multiplication in a frame. Also, the interface on the board might be noisy. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_DIP_2_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_DIP_2_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the interface locked, but the CFC received a wrong DIP2 at the end of the calendar. should verify that calendar length match on both devices. Also, the interface on the board might be noisy. . Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_0_WD_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the spi watch dog expired, and indicates that the interface is not active (down). Should verify the wd period config and also see if the board maybe noisy or defected. Not relevant if the SpiOob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SPI_OOB_RX_1_WD_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the spi watch dog expired, and indicates that the interface is not active (down). Should verify the wd period config and also see if the board maybe noisy or defected. Not relevant if the SpiOob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SCH_OOB_RX_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the sch oob interface received a crc error. Should check the other device and the interface itself. Not relevant if the SCH-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_SCH_OOB_RX_WD_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the sch oob interface did not receive a correct crc for the duration of the watch dog period. Should check the other device and the interface. Not relevant if the SCH-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_HCFC_OOB_RX_0_HEADER_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the hcfc oob interface received a msg type or fwd field that does not match the configuration. Should check the config and the other device config. Not relevant if the Hcfc-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_HCFC_OOB_RX_1_HEADER_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the hcfc oob interface received a msg type or fwd field that does not match the configuration. Should check the config and the other device config. Not relevant if the Hcfc-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_HCFC_OOB_RX_0_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the hcfc oob interface received a crc error in one of the msgs. Should check the other device and the interface itself. Not relevant if the Hcfc-Oob is not in use.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_HCFC_OOB_RX_1_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"the hcfc oob interface received a crc error in one of the msgs. Should check the other device and the interface itself. Not relevant if the Hcfc-Oob is not in use. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CFC_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FREE_BDB_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FREE_BDB_UNF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_DEQ_COM_EMPTY_Q, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FULL_USCNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init , check mc table in irr");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_MINI_USCNT_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init , check other table in irr");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_UPDT_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_TXDSCR_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_TXDSCR_BURST_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_PD_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ISP_DUP_ERR, 0,0,jerp_special_handling_normal_0,NULL,"check configurations in ingress");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_RST_USCNT_ERR, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_PKT_ENQ_RSRC_ERR, 0,0,jerp_none,NULL,"JERP_INT_IQM_PKT_ENQ_RSRC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_PKT_ENQ_MC_ERR, 0,0,jerp_none,NULL,"JERP_INT_IQM_PKT_ENQ_MC_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FREE_BDB_PROT_ERR, 0,0,jerp_special_handling_normal_0,NULL,"check counter (0x202) , chcek the free bdb discard threshold, ingress soft init");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_PKT_ENQ_QNVALID_ERR, 0,0,jerp_none,NULL,"JERP_INT_IQM_PKT_ENQ_QNVALID_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_QROLL_OVER, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init\ncheck soft error indication on bdb ll and  pdm tables");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_VSQ_ROLL_OVER, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init\ncheck soft error indication on bdb ll and  pdm tables");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_DRAM_DYN_SIZE_ROLL_OVER, 0,0,jerp_special_handling_normal_0,NULL,"check guarnteed bytes config\ncheck discard over dynamic size config");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_CNM_PKT_RJCT, 0,0,jerp_special_handling_normal_0,NULL,"check CNM shaper and CNM thresholds");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_DEQ_STOP_PDM_ERR, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ENQ_QNUM_OVF, 0,0,jerp_special_handling_normal_0,NULL,"check other module condifuration (irr/ ihp)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ING_MC_2_ISP_ERR, 0,0,jerp_special_handling_normal_0,NULL,"check other module condifuration (irr mc table, isp queues )");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_DOUBLE_CD_ERR, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FR_FLMC_DB_ROLL_OVER, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init, check buffer configurations, check soft errors in other modules (all the dbuff number path)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_FR_MNMC_DB_ROLL_OVER, 0,0,jerp_special_handling_normal_0,NULL,"ingress soft init, check buffer configurations, check soft errors in other modules (all the dbuff number path)");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQM_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_0_ST_ING_ENQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_0_ST_ING_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_0_ST_ING_DEQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_0_ST_ING_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_EPNI_0_ST_EGR_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_EPNI_0_ST_EGR_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_0_ST_ENQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_0_ST_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_0_ST_DEQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_0_ST_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_1_ST_ING_ENQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_1_ST_ING_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_1_ST_ING_DEQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_1_ST_ING_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_EPNI_1_ST_EGR_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_EPNI_1_ST_EGR_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_1_ST_ENQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_1_ST_ENQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_1_ST_DEQ_RPT_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_1_ST_DEQ_RPT_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_0_SCRB_RPT_FF_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_0_SCRB_RPT_FF_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_IQM_1_SCRB_RPT_FF_OVF, 0,0,jerp_none,NULL,"JERP_INT_IQMT_IQM_1_SCRB_RPT_FF_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IQMT_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_DRAM_OPP_CRC_ERR_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_DRAM_OPP_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACA_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACA_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACA_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACA_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACA_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACA_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACA_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACA_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACB_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACB_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACB_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACB_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACB_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACB_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACB_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACB_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACC_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACC_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACC_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACC_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACC_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACC_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACC_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACC_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACD_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACD_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACD_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACD_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACD_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACD_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACD_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACD_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACE_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACE_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACE_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACE_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACE_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACE_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACE_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACE_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACF_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACF_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACF_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACF_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACF_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACF_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACF_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACF_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACG_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACG_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACG_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACG_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACG_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACG_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACG_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACG_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACH_BG_0_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACH_BG_0_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACH_BG_1_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACH_BG_1_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACH_BG_2_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACH_BG_2_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACH_BG_3_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACH_BG_3_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACA_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACA_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACB_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACB_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACC_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACC_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACD_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACD_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACE_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACE_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACF_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACF_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACG_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACG_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_BACH_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT, 10,1,jerp_none,jerp_hard_reset,"JERP_INT_MMU_BACH_PRE_RDF_BUFFER_FIFO_OVERFLOW_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_MMU_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_MMU_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_CRC_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_OCB_CRC_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_WR_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jerp_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_WR_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jerp_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_RD_DBUFF_POINTER_NOT_IN_ANY_RANGE_INT, 0,0,jerp_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_RD_DBUFF_POINTER_MATCHED_MORE_THAN_ONE_RANGE_INT, 0,0,jerp_reprogram_resource,NULL,"Fix the OCB configuration.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_OCB_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_CQF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_CQF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_SHADDOW_RD_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_SHADDOW_RD_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_SHADDOW_WR_ADDR_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_SHADDOW_WR_ADDR_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_RD_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_RD_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_WR_CRC_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_WR_CRC_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_RD_RETRANSMIT_SHALLOW_FIFO_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_DRAM_WR_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_DRAM_RD_CRC_ERR, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_DRAM_CMD_PARITY_ERR, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_DRAM_CMD_PARITY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCA_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCB_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCC_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCD_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCE_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCF_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCG_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_DWF_OVER_FLOW, 0,0,jerp_hard_reset,NULL,"JERP_INT_DRCH_DWF_OVER_FLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_TEMP_SENS_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_PHY_CDR_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_PHY_CDR_CYCLE_ABOVE_TH, 0,0,jerp_special_handling_normal_0,NULL,"TBD");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_PHY_CALIB_REST_ERROR, 0,0,jerp_special_handling_normal_0,NULL,"FATAL calibration error");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCA_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCB_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCC_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCD_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCE_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCF_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCG_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_DRCH_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_NIF_PHYSICAL_ERR, 0,0,jerp_special_handling_normal_0,NULL,"need to check H/W. might be issue with ELK connection.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_SEQ_RX_BIGER_SEQ_EXP_AND_SMALLER_SEQ_TX, 0,0,jerp_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_SEQ_RX_SMALLER_SEQ_EXP_OR_BIGGER_EQ_SEQ_TX, 0,0,jerp_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_FLP_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"this means that ELK link rate can not sustain FLP requests rate. Can be configuration error, or system setup error.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_FLP_LOOKUP_TIMEOUT, 0,0,jerp_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, issue with ELK. Could be too much info replies generated by ELK Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_NIF_RX_FIFO_OVF, 0,0,jerp_special_handling_normal_0,NULL,"worst case: ELK alignment is wrong, meaning NOP are inserted in packet start.\nCould be too much info replies generated by ELK. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_LB_VECTOR_INDEX_ILLEGAL_CALC_INT, 0,0,jerp_special_handling_normal_0,NULL,"fix LB configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_LB_ECMP_LAG_USE_SAME_HASH_INT, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_FEC_ENTRY_ACCESSED_INT, 0,0,jerp_none,NULL,"JERP_INT_IHB_FEC_ENTRY_ACCESSED_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ILLEGAL_BYTES_TO_REMOVE_VALUE, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration. ");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_INVALID_DESTINATION_VALID, 0,0,jerp_none,NULL,"JERP_INT_IHB_INVALID_DESTINATION_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_LOOKUP_REPLY_OVF, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_LOOKUP_REPLY_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_BAD_CHANNEL_NUM, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_INFO_REPLY_OVF, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_INFO_REPLY_OVF");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_BAD_LKP_TYPE, 0,0,jerp_special_handling_normal_14,NULL,"fix configuration, and reset to IPP. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_BAD_LRN_TYPE, 0,0,jerp_special_handling_normal_14,NULL,"fix configuration, and reset to IPP.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_NIF_TX_FIFO_FULL, 0,0,jerp_special_handling_normal_0,NULL,"this means that ELK link rate can not sustain FLP requests rate. Can be configuration error, or system setup error.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_RX_BROKEN_RECORD, 0,0,jerp_special_handling_normal_0,NULL,"if asserted with NifPhysical error- treat as NifPhysicalError. Otherwise, means packet loss in ELK and should be treated in ELK.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_INFO_REPLY_VALID, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_INFO_REPLY_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_INFO_REPLY_ERR, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_INFO_REPLY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_LOOKUP_REPLY_VALID, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_LOOKUP_REPLY_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_CPU_LOOKUP_REPLY_ERR, 0,0,jerp_none,NULL,"JERP_INT_IHB_CPU_LOOKUP_REPLY_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_KAPS_DIRECT_LOOKUP_ERR, 0,0,jerp_special_handling_normal_0,NULL,"Read Kaps Register Address: 0x0000001b/1a\nBit0 = ECC Error\nBit1 = Address range Error\nif Address range Error need to reconfigure PMF direct access address");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_MAX_HEADER_STACK_EXCEEDED, 0,0,jerp_none,NULL,"JERP_INT_IHP_MAX_HEADER_STACK_EXCEEDED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED, 0,0,jerp_none,NULL,"JERP_INT_IHP_ILLEGAL_ADDRESS_ENCOUNTERED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_VT_LIF_BANK_CONTENTION, 0,0,jerp_none,NULL,"JERP_INT_IHP_VT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_TT_LIF_BANK_CONTENTION, 0,0,jerp_none,NULL,"JERP_INT_IHP_TT_LIF_BANK_CONTENTION");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_MPLS_LABEL_TERMINATION_ERROR, 0,0,jerp_none,NULL,"JERP_INT_IHP_MPLS_LABEL_TERMINATION_ERROR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT, 0,0,jerp_none,NULL,"JERP_INT_IHP_RPF_DEST_NOT_FEC_PTR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_OAM_BFD_MISCONFIG_INT, 0,0,jerp_none,NULL,"JERP_INT_IHP_OAM_BFD_MISCONFIG_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_KAPS_DB_LOOKUP_ERR, 0,0,jerp_none,NULL,"JERP_INT_IHP_KAPS_DB_LOOKUP_ERR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_ECC_PARITY_ERR_INT, 1000,100,jerp_special_handling_normal_1,jerp_special_handling_recur_2,"identify the memory according to the field ParityErrAddr in reg Parity_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHP_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_TCAM_PROTECTION_ERROR");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_TCAM_QUERY_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_CAM_TABLE_FULL, 0,0,jerp_special_handling_normal_0,NULL,"defrag should be initiated. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY, 0,0,jerp_hard_reset,NULL,"JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMA_ONE_OEMA_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMA_ONE_OEMA_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"Act according to specified in 0x0405, OemaManagementUnitFailure. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMA_ONE_OEMA_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_CAM_TABLE_FULL, 0,0,jerp_special_handling_normal_0,NULL,"defrag should be initiated.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY, 0,0,jerp_hard_reset,NULL,"JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_TABLE_COHERENCY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_DELETE_UNKNOWN_KEY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMB_ONE_OEMB_ERROR_REACHED_MAX_ENTRY_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMB_ONE_OEMB_WARNING_INSERTED_EXISTING");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"Act according to specified in 0x0485, OembManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_OEMB_ONE_OEMB_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_A_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"You should print an error to the screen and check your configuration");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_ONE_MACT_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_STATIC_ALLOWED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_READY, 0,0,jerp_special_handling_normal_0,NULL,"read mact_event if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning). Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FIFO_HIGH_THRESHOLD_REACHED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_REPLY_READY, 0,0,jerp_special_handling_normal_0,NULL,"read mact_reply if needed. This interrupt should be handled as part of the learning mechanism application (CPU based learning).  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AGE_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FLU_REACHED_END_INDEX");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_AMSG_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FMSG_DROP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_FCNT_COUNTER_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"fix configuration. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_FCNT_MTM_CONSECUTIVE_OPS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_LEL_ERR_DATA_VALID, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_LEL_ERR_DATA_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT_CPU_ALLOWED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FID_OR_LIF_EXCEED_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MASTER_EVENT_FID_OR_LIF_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_READY, 0,0,jerp_special_handling_normal_0,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_READY");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FIFO_HIGH_THRESHOLD_REACHED, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FIFO_HIGH_THRESHOLD_REACHED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FID_OR_LIF_EXCEED_LIMIT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_EVENT_FID_OR_LIF_EXCEED_LIMIT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_LEL_ERR_DATA_VALID, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_LEL_ERR_DATA_VALID");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_SRC_OR_LL_LOOKUP_ON_WRONG_CYCLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_FCNT_COUNTER_OVERFLOW, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_SLAVE_FCNT_COUNTER_OVERFLOW");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_PPDB_B_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_B_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_PIPE_0_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jerp_special_handling_normal_0,NULL,"Check configuration of contexts in the device: This interrupt implies that an incoming reassembly context was equal to the value configured in the field BadReassemblyContext of the IRE Static Configuration register. One of the context mappings erroneously points to this reassembly context. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_PIPE_1_ERROR_BAD_REASSEMBLY_CONTEXT, 0,0,jerp_special_handling_normal_0,NULL,"Check configuration of contexts in the device: This interrupt implies that an incoming reassembly context was equal to the value configured in the field BadReassemblyContext of the IRE Static Configuration register. One of the context mappings erroneously points to this reassembly context. Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_DATA_ARRIVED, 0,0,jerp_special_handling_normal_0,NULL,"Check how the code configued RegisterInterfacePacketControl: Check if the RegiPktErr field of that register was set to true.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_EXTERNAL_IF_ERROR_REGI_ERR_PACKET_SIZE, 0,0,jerp_special_handling_normal_0,NULL,"Check how the code configured RegisterInterfacePacketControl: Check if the packet sent was smaller than 32 bytes.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IRE_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRE_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_MINI_MULTICAST_0_RECYCLE, 0,0,jerp_hard_reset,NULL,"should not happen by design. \nIt indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_FULL_MULTICAST_0_RECYCLE, 0,0,jerp_hard_reset,NULL,"should not happen by design. \nIt indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_OCB_0_RECYCLE, 0,0,jerp_hard_reset,NULL,"Check configuratgion of OCB bank buffer ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_MINI_MULTICAST_1_RECYCLE, 0,0,jerp_hard_reset,NULL,"should not happen by design. \nIt indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_FULL_MULTICAST_1_RECYCLE, 0,0,jerp_hard_reset,NULL,"should not happen by design. \nIt indicates that flow control from IDR to IQM did not work.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_OCB_1_RECYCLE, 0,0,jerp_hard_reset,NULL,"Check configuratgion of OCB bank buffer ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_MINI_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_FULL_MULTICAST_0_EXTERNAL_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_MINI_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_FULL_MULTICAST_1_EXTERNAL_OVERFLOW, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of External FBC ranges");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_RECEIVED_PIPE, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_ETHERNET_METER_PTR, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of Ethernet Metering in PP block");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_REASSEMBLY_CONTEXT, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of reassembly contect mapping in IRE block");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_SENT_PIPE_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ERROR_SENT_PIPE_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of contect pipe mapping");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_MMU_ECC_1B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IDR_MMU_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_MMU_ECC_2B_ERR_INT, 0,0,jerp_hard_reset,NULL,"JERP_INT_IDR_MMU_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IDR_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MINIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_ORIGINAL_SIZE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_SIZE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_DP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_MAXIMUM_DP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_PCB, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_PCB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_SOP_PCB, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_SOP_PCB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_DP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_NO_DP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_EOP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_EOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_MOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_UNEXPECTED_SOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_MOP");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_IDLE, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_GENERAL_IDLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_DRAM_REJECT, 0,0,jerp_none,NULL,"JERP_INT_IDR_REASSEMBLY_REASSEMBLY_ERROR_DRAM_REJECT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_MAX_REPLICATION_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_MAX_REPLICATION_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_IS_MAX_REPLICATION_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_IS_MAX_REPLICATION_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB and IRR_MAX_REPLICATIONS");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_REPLICATION_EMPTY_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_REPLICATION_EMPTY_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_IS_REPLICATION_EMPTY_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_IS_REPLICATION_EMPTY_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_MCDB");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_FLOW_ID_IS_OVER_96K_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_QUEUE_IS_OVER_96K_0, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ERR_QUEUE_IS_OVER_96K_1, 0,0,jerp_special_handling_normal_0,NULL,"Check configuratgion of IRR_DESTINATION_TABLE");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_IRR_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IRR_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_TOTAL_DB_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_TOTAL_DB_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_TOTAL_PD_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_TOTAL_PD_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_PD_SP_0_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_PD_SP_1_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_DB_SP_0_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_RSVD_DB_SP_1_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_0_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_1_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_2_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_3_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_4_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_5_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_6_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_7_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_8_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_9_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_10_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_11_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_12_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_13_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_14_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_MC_PD_SP_TC_15_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_PDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_PDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_QDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_QDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_PQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_PQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_QQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_QQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_FQSM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_FQSM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_FDCM_UNDERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_CGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT, 0,0,jerp_hard_reset,NULL,"JERP_INT_CGM_CGM_REP_AROUND_FDCM_OVERFLOW_REP_AROUND_INTERRUPT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_PNIMI_001, 0,0,jerp_none,NULL,"JERP_INT_ECI_PNIMI_001");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_UC_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_UC_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_CORE_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_CORE_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_TS_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_TS_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_BS_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_BS_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_FAB_0_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_FAB_0_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_FAB_1_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_FAB_1_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_NIF_PMH_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_NIF_PMH_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_NIF_PML_0_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_NIF_PML_0_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_NIF_PML_1_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_NIF_PML_1_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_SYNCE_MASTER_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_SYNCE_MASTER_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_SYNCE_SLAVE_PLL_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_SYNCE_SLAVE_PLL_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_MISC_PLL_9_LOCKED_LOST, 0,0,jerp_none,NULL,"JERP_INT_ECI_MISC_PLL_9_LOCKED_LOST");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_PVT_MON_TRACKER_INT, 0,0,jerp_none,NULL,"JERP_INT_ECI_PVT_MON_TRACKER_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_ECI_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_ECC_ECC_1B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_ECI_ECC_ECC_1B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_ECI_ECC_ECC_2B_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_ECI_ECC_ECC_2B_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ISEM_ONE_ISEM_MANAGEMENT_UNIT_FAILURE_VALID, 0,0,jerp_special_handling_normal_0,NULL,"Act according to specified in 0x0305, IsemManagementUnitFailure.  Interrupt handler log a message.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_IHB_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED, 0,0,jerp_none,NULL,"JERP_INT_IHB_ISEM_ONE_ISEM_MANAGEMENT_COMPLETED");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_KAPS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_BBS_ECC_PARITY_ERR_INT, 0,0,jerp_none,NULL,"JERP_INT_KAPS_BBS_ECC_PARITY_ERR_INT");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_BBS_ECC_ECC_1B_ERR_INT, 1000,100,jerp_scrub_shadow_write,jerp_special_handling_recur_3,"if memory was not cached, ignore.");
    dcmn_intr_add_handler_ext(unit, JERP_INT_KAPS_BBS_ECC_ECC_2B_ERR_INT, 1000,100,jerp_special_handling_normal_4,jerp_special_handling_recur_5,"identify the memory according to the field Ecc 2bErrAddr in reg Ecc_2b_Err_Cnt. according to the memory severity, either init device (or core), fix line , ignore or perform action that applies for the specific memory.");

    

    dcmn_intr_add_handler(unit, JERP_INT_OAMP_PENDING_EVENT,                                 0,      0,  jer_interrupt_handle_oamppendingevent,      NULL);
    dcmn_intr_add_handler(unit, JERP_INT_OAMP_STAT_PENDING_EVENT,                            0,      0,  jer_interrupt_handle_oamppendingstatevent,  NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCA_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCB_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCC_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCD_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCE_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCF_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCG_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCH_PHY_CDR_ABOVE_TH,                              0,      0,  jerp_event_drc_phy_cdr_above_th_handler,     NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCA_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCB_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCC_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCD_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCE_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCF_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCG_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCH_DRAM_RD_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCA_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCB_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCC_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCD_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCE_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCF_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCG_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);
    dcmn_intr_add_handler(unit, JERP_INT_DRCH_DRAM_WR_CRC_ERR,                               0,      0,  jerp_event_drc_rd_wr_crc_handler,            NULL);    
    dcmn_intr_add_handler_ext(unit, JERP_INT_PPDB_A_TCAM_PROTECTION_ERROR, 0,0,jerp_interrupt_handle_tcamprotectionerror,NULL,"JERP_INT_PPDB_A_TCAM_PROTECTION_ERROR");

    dcmn_intr_add_handler(unit, JERP_INT_IPT_CRC_ERR_PKT,                     0,    0,  dcmn_interrupt_handle_IPTCrcErrPkt_Int, jerp_hard_reset);
    dcmn_intr_add_handler(unit, JERP_INT_IPT_CRC_DELETED_BUFFERS_FIFO_FULL,   0,    0,  dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoFull_Int, NULL);
    dcmn_intr_add_handler(unit, JERP_INT_IPT_CRC_DELETED_BUFFERS_FIFO_NOT_EMPTY, 0, 0,  dcmn_interrupt_handle_IPTCrcDeletedBuffersFifoNotEmpty_Int, NULL);

    dcmn_intr_add_clear_ext(unit, JERP_INT_PPDB_B_LARGE_EM_TWO_LARGE_EM_MNGMNT_REQ_FID_EXCEED_LIMIT, soc_interrupt_clear_fid_limit_reached_event);
    dcmn_intr_add_handler(unit, JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_0, 0, 0,  jerp_interrupt_handle_kapstcamerror, NULL);
    dcmn_intr_add_handler(unit, JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_1, 0, 0,  jerp_interrupt_handle_kapstcamerror, NULL);
    dcmn_intr_add_handler(unit, JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_2, 0, 0,  jerp_interrupt_handle_kapstcamerror, NULL);
    dcmn_intr_add_handler(unit, JERP_INT_KAPS_KAPS_ERROR_REGISTER_TECC_ERROR_3, 0, 0,  jerp_interrupt_handle_kapstcamerror, NULL);

}

#undef _ERR_MSG_MODULE_NAME
