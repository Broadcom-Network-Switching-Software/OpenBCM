/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement CallBacks function for RAMON interrupts.
 */
 

#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/intr.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/ramon/ramon_intr.h>
#include <soc/dnxf/ramon/ramon_intr_cb_func.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR



int ramon_intr_handle_generic_none(int unit, int block_instance, ramon_interrupt_type en_ramon_interrupt,char *msg)
{
    return 0;
}

int ramon_intr_recurring_action_handle_generic_none(int unit, int block_instance, ramon_interrupt_type en_ramon_interrupt, char *msg)
{
    return 0;
}
int ramon_none(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    
    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, NULL);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
int ramon_special_handling(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
int ramon_reprogram_resource(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE, msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
int ramon_shutdown_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    uint32 rc;
    SHR_FUNC_INIT_VARS(unit);;

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
int ramon_retrain_link(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}
int ramon_intr_parity_handler(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
int ramon_intr_parity_recurr(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
int ramon_intr_ecc_1b_handler(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}
int ramon_intr_ecc_1b_recurr(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int ramon_intr_ecc_2b_handler(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc = 0;
    dnxc_int_corr_act_type p_corrective_action;
    dnxc_interrupt_mem_err_info shadow_correct_info;
    char info_msg[240];

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_data_collection_for_shadowing(unit, block_instance,  en_interrupt, info_msg, &p_corrective_action,&shadow_correct_info);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, p_corrective_action, info_msg);
    SHR_IF_ERR_EXIT(rc);
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance,en_interrupt, info_msg, p_corrective_action , (void *) &shadow_correct_info,info_msg);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int ramon_intr_ecc_2b_recurr(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, msg);
    SHR_IF_ERR_EXIT(rc);

exit:
     SHR_FUNC_EXIT;
}

int ramon_hard_reset(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc = 0;

    SHR_FUNC_INIT_VARS(unit);
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_HARD_RESET, NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, msg, DNXC_INT_CORR_ACT_HARD_RESET, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}


int ramon_interrupt_handle_CCH_CCP0IllCellInt(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 reg32_val;
    uint32 valid, type, source_id, input_link;

    SHR_FUNC_INIT_VARS(unit);

    
    rc = soc_reg32_get(unit, CCH_CCP_0_ILLEGAL_CELLS_DATAr, block_instance, 0, &reg32_val);
    SHR_IF_ERR_EXIT(rc);

    valid = soc_reg_field_get(unit, CCH_CCP_0_ILLEGAL_CELLS_DATAr, reg32_val, CCP_0_ILLCELL_VALIDf);
    type = soc_reg_field_get(unit, CCH_CCP_0_ILLEGAL_CELLS_DATAr, reg32_val, CCP_0_ILLCELL_TYPEf);
    source_id = soc_reg_field_get(unit, CCH_CCP_0_ILLEGAL_CELLS_DATAr, reg32_val, CCP_0_ILLCELL_SOURCE_IDf);
    input_link = soc_reg_field_get(unit, CCH_CCP_0_ILLEGAL_CELLS_DATAr, reg32_val, CCP_0_ILLCELL_INPUT_LINKf);

    sal_sprintf(special_msg, "Read illegal cells data, valid is %d, type is %d, source_id is %d, input_link is %d",valid, type, source_id, input_link);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_CCH_CCP1IllCellInt(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    uint32 reg32_val;
    uint32 valid, type, source_id, input_link;

    SHR_FUNC_INIT_VARS(unit);

    
    rc = soc_reg32_get(unit, CCH_CCP_1_ILLEGAL_CELLS_DATAr, block_instance, 0, &reg32_val);
    SHR_IF_ERR_EXIT(rc);

    valid = soc_reg_field_get(unit, CCH_CCP_1_ILLEGAL_CELLS_DATAr, reg32_val, CCP_1_ILLCELL_VALIDf);
    type = soc_reg_field_get(unit, CCH_CCP_1_ILLEGAL_CELLS_DATAr, reg32_val, CCP_1_ILLCELL_TYPEf);
    source_id = soc_reg_field_get(unit, CCH_CCP_1_ILLEGAL_CELLS_DATAr, reg32_val, CCP_1_ILLCELL_SOURCE_IDf);
    input_link = soc_reg_field_get(unit, CCH_CCP_1_ILLEGAL_CELLS_DATAr, reg32_val, CCP_1_ILLCELL_INPUT_LINKf);

    sal_sprintf(special_msg, "Read illegal cells data, valid is %d, type is %d, source_id is %d, input_link is %d",valid, type, source_id, input_link);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_DCH_TypeErrInt(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_sprintf(special_msg, "please check the link");

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_fmac_link_get(
    int unit,
    int fmac_instance,
    int bit_in_field,
    int *p_link)
{
    int nof_links_in_fmac = 0;

    SHR_FUNC_INIT_VARS(unit);
    *p_link = -1;

    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    *p_link = fmac_instance * nof_links_in_fmac  + bit_in_field ;
    if (!(*p_link >= 0 && *p_link < dnxf_data_port.general.nof_links_get(unit))) {
        SHR_EXIT_WITH_LOG(SOC_E_FAIL, "Failed in dnxc_interrupt_fmac_link_get %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_fsrd_link_get(int unit,
                                int block_instance,
                                int bit_in_field,
                                int *p_link)
{
    int nof_links_in_fsrd = 0;

    SHR_FUNC_INIT_VARS(unit);
    *p_link = -1;

    nof_links_in_fsrd = dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    *p_link = block_instance * nof_links_in_fsrd + bit_in_field ;
    if (!(*p_link >= 0 && *p_link < dnxf_data_port.general.nof_links_get(unit))) {
        SHR_EXIT_WITH_LOG(SOC_E_FAIL, "Failed in dnxc_interrupt_fsrd_link_get %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fmac_fmac_LOS_Int(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    
    rc = ramon_interrupt_fsrd_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fsrd_fsrd_RxLockChanged(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    
    rc = ramon_interrupt_fsrd_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fmac_fmac_RxLostOfSync(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port, link_partner = -1;
    int fec_enable = 0;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    if (dnxf_data_port.retimer.feature_get(unit, dnxf_data_port_retimer_is_supported))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_retimer_connect_get,(unit, link, &link_partner));
        if (!SHR_FAILURE(rc) && link_partner != -1) {
            
            rc = portmod_port_pass_through_fec_enable_get(unit, link, &fec_enable);
            if (!SHR_FAILURE(rc) && !fec_enable) {
                uint32 flags;

                
                sal_sprintf(special_msg, "link=%d, port=%d, link is retimer and FEC off. Interrupt is meaningless, mask it.", link, port);

                rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
                SHR_IF_ERR_EXIT(rc);
                flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
                rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
                SHR_IF_ERR_EXIT(rc);
            }
        }
    }
    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fmac_fmac_OOF_Int(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port, link_partner = -1;
    int fec_enable = 0;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    int  retimer_fec_off = 0;
    int  encoding_64_66 = 0;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    if (dnxf_data_port.retimer.feature_get(unit, dnxf_data_port_retimer_is_supported))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_retimer_connect_get,(unit, link, &link_partner));
        if (!SHR_FAILURE(rc) && link_partner != -1) {
            
            rc = portmod_port_pass_through_fec_enable_get(unit, link, &fec_enable);
            if (!SHR_FAILURE(rc) && !fec_enable) {
                retimer_fec_off = 1;
                
                sal_sprintf(special_msg, "link=%d, port=%d, link is retimer and FEC off. Interrupt is meaningless, mask it.", link, port);
            }
        }
    }

    if (!retimer_fec_off) {
        
       bcm_port_resource_t resource;
       rc = soc_dnxc_port_resource_get(unit, link, &resource);
       if (!SHR_FAILURE(rc) && resource.fec_type == bcmPortPhyFecNone) {
           
           encoding_64_66 = 1;
           sal_sprintf(special_msg, "link=%d, port=%d, encoding type is pure 64_66. Interrupt is meaningless, mask it.", link, port);
       }
    }
    if (retimer_fec_off || encoding_64_66) {
        
        rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
        SHR_IF_ERR_EXIT(rc);
        flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
        rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
        SHR_IF_ERR_EXIT(rc);
    }
    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

int ramon_interrupt_handle_fmac_fmac_DecErr_Int(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
    sal_sprintf(special_msg, "link=%d, port=%d, need tune SerDes", link, port);

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}


int ramon_interrupt_handle_fmac_tx_ilkn_underrun(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port, link_partner = -1;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    int  retimer_mode = 0;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;

    
    if (dnxf_data_port.retimer.feature_get(unit, dnxf_data_port_retimer_is_supported))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_retimer_connect_get,(unit, link, &link_partner));
        if (!SHR_FAILURE(rc) && link_partner != -1) {
            
            retimer_mode = 1;
        }
    }

    if (retimer_mode) {
        
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE-1, "fmac_tx_ilkn_underrun link=%d, port=%d, link is retimer mode.", link, port);
    } else {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE-1, "fmac_tx_ilkn_underrun link=%d, port=%d, link is not retimer. Interrupt is meaningless, mask it.", link, port);
        rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
        SHR_IF_ERR_EXIT(rc);
        flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
        rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
        SHR_IF_ERR_EXIT(rc);
    }

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}



int ramon_interrupt_handle_fmac_tx_ilkn_overflow(int unit, int block_instance, uint32 en_interrupt, char *msg)
{
    int rc;
    soc_port_t link, port, link_partner = -1;
    char special_msg[DNXC_INTERRUPT_SPECIAL_MSG_SIZE];
    int  retimer_mode = 0;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    
    
    rc = ramon_interrupt_fmac_link_get(unit, block_instance,
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                     &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;

    
    if (dnxf_data_port.retimer.feature_get(unit, dnxf_data_port_retimer_is_supported))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_retimer_connect_get,(unit, link, &link_partner));
        if (!SHR_FAILURE(rc) && link_partner != -1) {
            
            retimer_mode = 1;
        }
    }

    if (retimer_mode) {
        
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE-1, "fmac_tx_ilkn_overflow link=%d, port=%d, link is retimer mode.", link, port);
    } else {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE-1, "fmac_tx_ilkn_overflow link=%d, port=%d, link is not retimer. Interrupt is meaningless, mask it.", link, port);
        rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
        SHR_IF_ERR_EXIT(rc);
        flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
        rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
        SHR_IF_ERR_EXIT(rc);
    }

    
    rc = dnxc_interrupt_print_info(unit, block_instance, en_interrupt, 0, DNXC_INT_CORR_ACT_NONE, special_msg);
    SHR_IF_ERR_EXIT(rc);

    
    rc = dnxc_interrupt_handles_corrective_action(unit, block_instance, en_interrupt, special_msg, DNXC_INT_CORR_ACT_NONE, NULL, NULL);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

void ramon_interrupt_cb_init(int unit)
{
    
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_0_SRC_DV_CNG_LINK_INT, 0, 0, ramon_none, ramon_none, "NA");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_1_SRC_DV_CNG_LINK_INT, 0, 0, ramon_none, ramon_none, "NA");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_0_ILL_CELL_INT, 100, 1, ramon_special_handling, ramon_shutdown_link, "Read IllegalCellsData register to clear the FIFO. If it still happens, it is probably due to high BER - need to shutdown the link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_1_ILL_CELL_INT, 100, 1, ramon_special_handling, ramon_shutdown_link, "Read IllegalCellsData register to clear the FIFO. If it still happens, it is probably due to high BER - need to shutdown the link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_0_CHF_OVF_INT, 10, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_0_CLF_OVF_INT, 10, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_1_CHF_OVF_INT, 10, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_1_CLF_OVF_INT, 10, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CPU_CAPT_CELL_FNE_INT, 0, 0, ramon_special_handling, ramon_none, "Read FIFO to clear it");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_UNRCH_DST_INT, 10000, 1, ramon_special_handling, ramon_retrain_link, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_AUTO_DOC_NAME_1, 100, 1, ramon_none, ramon_special_handling, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_DTL_DRP_CPU_CELL_INT, 0, 0, ramon_special_handling, ramon_none, "Reduce CPU captured data rate, or increase FIFO readout rate.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_DTL_DRP_LCLRT_CPU_CELL_INT, 0, 0, ramon_special_handling, ramon_none, "Reduce CPU captured data rate, or increase FIFO readout rate.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_ADMIT_DENY_DFL_FULL_INT, 0, 0, ramon_reprogram_resource, ramon_none, "DFL being full indicates the system is not properly configured - traffic arriving to DCML is more than DCML can carry.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_LCM_ADMIT_DENY_DUE_SHARED_MEM_INT, 0, 0, ramon_special_handling, ramon_none, "Check admit tests threasholds");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_LCM_ADMIT_DENY_DUE_DTM_INT, 0, 0, ramon_none, ramon_none, "Check admit tests thresholds.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_LCM_ADMIT_DENY_DUE_LATENCY_INT, 0, 0, ramon_special_handling, ramon_none, "Check latency check thresholds.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_LCM_LCM_CNT_OVERFLOW_INT, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_LCM_LCM_LCM_CNT_UNDERFLOW_INT, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_GENERAL_LINK_INTEGRITY_CHANGED_INT, 0, 0, ramon_special_handling, ramon_none, "Check for link errors. If link is OK, check RTP configurations at both sides (RMGR and RTPWP)");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_GENERAL_UNICAST_TABLE_CHANGED_INT, 0, 0, ramon_none, ramon_none, "Might indicate errors on device partner, or topology changes.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_GENERAL_MCID_OUT_OF_RANGE_INT, 2, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore (could be link error flipping MC ID bits). Otherwise check MC table configuration.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_GENERAL_GPD_MODE_DETECT, 0, 0, ramon_none, ramon_none, "RAMON_INT_RTP_GENERAL_GPD_MODE_DETECT");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_RTP_GENERAL_MASK_GCI_ON, 0, 0, ramon_none, ramon_none, "RAMON_INT_RTP_GENERAL_MASK_GCI_ON");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_0, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_1, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_2, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_3, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_4, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_5, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_6, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_7, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_8, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_9, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_10, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_AUTO_DOC_NAME_11, 100, 1, ramon_none, ramon_special_handling, "If happens once in a while - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_MCT_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_TRANSACTION_REG_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_GEN_DATA_CELL_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_GEN_CONTROL_CELL_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_CAPTURE_DATA_CELL_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_CAPTURE_CONTROL_CELL_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_OCCG_TEST_MODE_CMD_FINISH_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_QUERY_EMPTY_MULTICAST_ID_INT, 2, 1, ramon_none, ramon_special_handling, "If happens once - ignore (could be link error flipped MCID). Otherwise, check multicast table configuration. This interrupt indicates traffic was sent to uninitilized MC ID.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_QUERY_EMPTY_LINK_MAP_INT, 2, 1, ramon_none, ramon_special_handling, "If happens once - ignore (could be link error flipped destination ID bits). Otherwise - check destination ID mapping at the source FAP. It could also be related to link errors, causing the destination FAP to be unreachable.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 2, 1, ramon_none, ramon_special_handling, "If happens once - ignore (could be link error flipping destination ID bits). Otherwise, check MaxBaseIndex configuration.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 2, 1, ramon_none, ramon_special_handling, "If happens once - ignore (could be link error flipping destination ID bits). Otherwise, check UpdateBaseIndex configuration.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_QUERY_UNREACHABLE_MULTICAST_INT, 0, 0, ramon_special_handling, ramon_none, "Check MC configuration (MCT, MCLBT), and also link status.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_DRH_DRH_MNOL_DROP_INT, 1000, 1, ramon_none, ramon_reprogram_resource, "If happens constanstly, it indicates some config error (check MNOL table). Otherwise it incdicates drops caused by device projected to be removed from the system.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_CRH_CRH_QUERY_EMPTY_LINK_MAP_INT, 10, 1, ramon_none, ramon_special_handling, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_CRH_CRH_QUERY_DESTINATION_ABOVE_MAX_BASE_INDEX_INT, 2, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_QRH_CRH_CRH_QUERY_DESTINATION_ABOVE_UPDATE_BASE_INDEX_INT, 2, 1, ramon_none, ramon_reprogram_resource, "If happens once - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_DFL_CPU_CELL_FIFO_NE_INT, 0, 0, ramon_none, ramon_none, "Read from CPU FIFO.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMA_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMB_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMC_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMD_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDME_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMF_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMG_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_CDMH_LP_FIF_OVF_INT, 0, 0, ramon_special_handling, ramon_none, "Need to allocate more buffers in DFL to CPU and/or lowering the traffic arriving to the CPU port and/or speeding up the CPU FIFO readout.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_0, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_1, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_2, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_3, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_4, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_5, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_6, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_7, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_8, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_9, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_10, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_11, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_12, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_13, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_14, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_15, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_16, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_17, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_18, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_FREE_ERROR_FPC_FREE_ERROR_19, 0, 0, ramon_hard_reset, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_0, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_1, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_2, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_3, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_4, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_5, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_6, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_7, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_8, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_9, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_10, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_11, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_12, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_13, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_14, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_15, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_16, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_17, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_18, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCML_FPC_ALLOC_ERROR_FPC_ALLOC_ERROR_19, 0, 0, ramon_reprogram_resource, ramon_none, "Need to change LCM thresholds to make sure DFL is never full.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_P_0_DESCCNTO, 0, 0, ramon_none, ramon_none, "RAMON_INT_DCH_DCH_P_0_DESCCNTO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_P_1_DESCCNTO, 0, 0, ramon_none, ramon_none, "RAMON_INT_DCH_DCH_P_1_DESCCNTO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_P_2_DESCCNTO, 0, 0, ramon_none, ramon_none, "RAMON_INT_DCH_DCH_P_2_DESCCNTO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_IFMFO_P_0_INT, 0, 0, ramon_special_handling, ramon_none, "Check link level flow control thresholds.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_IFMFO_P_1_INT, 0, 0, ramon_special_handling, ramon_none, "Check link level flow control thresholds.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_IFMFO_P_2_INT, 0, 0, ramon_special_handling, ramon_none, "Check link level flow control thresholds.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_CPUDATACELLFNE_0_INT, 0, 0, ramon_none, ramon_none, "Read CPU cells FIFO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_CPUDATACELLFNE_1_INT, 0, 0, ramon_none, ramon_none, "Read CPU cells FIFO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_CPUDATACELLFNE_2_INT, 0, 0, ramon_none, ramon_none, "Read CPU cells FIFO");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_UNREACH_DEST_EV_INT, 0, 0, ramon_special_handling, ramon_none, "If unreachanble destination occurs continously, requires to further debug system, understand if it is a real destination that exists in the system, if this is known on all other FE, check if is known in RTP main database and only one of the copies is missing, in which case the recovery can be simple");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ERROR_FILTER_INT, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_TYPE_ERR_INT, 0, 0, ramon_special_handling, ramon_none, "This interrupt indicates cell corruption. If happns constantly - need to check link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_P_0, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_0_INT, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_ERROR_P_0, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_OUT_OF_SYNC_INT_P_0, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_DCH_OUT_OF_SYNC_INT_P_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_2_P_0, 0, 0, ramon_special_handling, ramon_none, "Check filter settings.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_P_1, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_1_INT, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_ERROR_P_1, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_OUT_OF_SYNC_INT_P_1, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_DCH_OUT_OF_SYNC_INT_P_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_2_P_1, 0, 0, ramon_special_handling, ramon_none, "Check filter settings.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_P_2, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_2_INT, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_ERROR_P_2, 0, 0, ramon_special_handling, ramon_none, "If Alto occurs, handle according to Alto. Might also be related to some misconfiuration in the system, need to collect dumps of all devices in system and analyze them.  ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_OUT_OF_SYNC_INT_P_2, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_DCH_OUT_OF_SYNC_INT_P_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_2_P_2, 0, 0, ramon_special_handling, ramon_none, "Check filter settings.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DCH_UN_EXP_CELL_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_DCH_DCH_UN_EXP_CELL_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_DROP_FILTER_INT, 0, 0, ramon_none, ramon_none, "RAMON_INT_DCH_DROP_FILTER_INT");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_4, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_5, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_6, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_7, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_4, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_5, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_6, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_7, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_4, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_4");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_5, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_5");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_6, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_6");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_7, 0, 0, ramon_none, ramon_none, "RAMON_INT_FSRD_QUAD_0_AUTO_DOC_NAME_0_7");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_PLL_0_LOCK_CHANGED, 0, 0, ramon_reprogram_resource, ramon_none, "Check MAS PLL settings, Blackhawk PLL settings, and reference clock");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_PLL_1_LOCK_CHANGED, 0, 0, ramon_reprogram_resource, ramon_none, "Check MAS PLL settings, Blackhawk PLL settings, and reference clock");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_0_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_0_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_0_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_0_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_0_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_0_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_0_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_0_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_0, 0, 0, ramon_special_handling, ramon_none, "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_1, 0, 0, ramon_special_handling, ramon_none, "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_2, 0, 0, ramon_special_handling, ramon_none, "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_RX_RRR_CRL_OVF_DROP_INT_3, 0, 0, ramon_special_handling, ramon_none, "Reduce control cell rate in MAC TX of link partner (should be 1 cell each 8 links of the receiving MAC)");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_2_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_2_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_2_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_2_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_2_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_2_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_2_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_2_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_3_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_3_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_3_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_3_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_3_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_3_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_3_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_3_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_4_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_4_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_4_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_4_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_4_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_4_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_4_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_4_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_5_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_5_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_5_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_5_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_5_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_5_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_5_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_5_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_6_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_6_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_6_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_6_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_6_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_6_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_6_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_6_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_7_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_7_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_7_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_7_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_7_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_7_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_7_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_7_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_8_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_8_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_8_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_8_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_8_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_8_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_8_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_8_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_9_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_9_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_9_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_9_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_9_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_9_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_AUTO_DOC_NAME_9_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_AUTO_DOC_NAME_9_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_0_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_0, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_1, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_2, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_3, 0, 0, ramon_special_handling, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_ECC_PARITY_ERR_INT, 0, 0, ramon_intr_parity_handler, ramon_intr_parity_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_0, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_1, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_2, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_RX_CRC_ERR_N_INT_3, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_WRONG_SIZE_INT_0, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_WRONG_SIZE_INT_1, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_WRONG_SIZE_INT_2, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_1_WRONG_SIZE_INT_3, 10000000, 1, ramon_none, ramon_shutdown_link, "Low  rate errors - ignore");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_AGE_N_INT_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_3_LNKLVL_HALT_N_INT_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 0, 0, ramon_special_handling, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_5_TRANSMIT_ERR_INT_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_0, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_1, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_2, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_AGE_CTX_BN_INT_3, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_6_LNKLVL_HALT_CTX_BN_INT_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_0, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_1, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_2, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_AGE_CTX_CN_INT_3, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_0, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_1, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_2, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_7_LNKLVL_HALT_CTX_CN_INT_3, 0, 0, ramon_special_handling, ramon_none, "If aging happens, it means that the system is over-subscribed for too long. It maybe due to link error, load balancing error or other configuration issues.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_0, 0, 0, ramon_none, ramon_none, "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_1, 0, 0, ramon_none, ramon_none, "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_2, 0, 0, ramon_none, ramon_none, "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_CRC_INT_3, 0, 0, ramon_none, ramon_none, "This indicates 2b ECC error somewhere along the payload datapath");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_0, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_2, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_3, 0, 0, ramon_none, ramon_none, "RAMON_INT_FMAC_FMAC_8_AUTO_DOC_NAME_11_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3, 0, 0, ramon_hard_reset, ramon_none, "RAMON_INT_FMAC_FMAC_8_TX_FDRC_IF_FAULT_INT_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_AUTO_DOC_NAME_1, 0, 0, ramon_none, ramon_none, "RAMON_INT_ECI_AUTO_DOC_NAME_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_UC_PLL_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_CORE_PLL_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_MISC_PLL_0_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_MISC_PLL_1_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_MISC_PLL_2_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_MISC_PLL_3_LOCKED_LOST, 0, 0, ramon_reprogram_resource, ramon_none, "Check PLL reference clock and configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_0, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_1, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_2, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_3, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_4, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_INTERRUPT_REGISTER_FIELD_5, 0, 0, ramon_none, ramon_none, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_ECC_ECC_1B_ERR_INT, 0, 0, ramon_intr_ecc_1b_handler, ramon_intr_ecc_1b_recurr, "Check Configuration");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_ECI_ECC_ECC_2B_ERR_INT, 0, 0, ramon_intr_ecc_2b_handler, ramon_intr_ecc_2b_recurr, "Check Configuration");

    

    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_0_ILL_CELL_INT, 100, 1, ramon_interrupt_handle_CCH_CCP0IllCellInt, ramon_shutdown_link, "Read IllegalCellsData register to clear the FIFO. If it still happens, it is probably due to high BER - need to shutdown the link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_CCH_CCP_1_ILL_CELL_INT, 100, 1, ramon_interrupt_handle_CCH_CCP1IllCellInt, ramon_shutdown_link, "Read IllegalCellsData register to clear the FIFO. If it still happens, it is probably due to high BER - need to shutdown the link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_0_INT, 10, 1, ramon_special_handling, ramon_hard_reset, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_1_INT, 10, 1, ramon_special_handling, ramon_hard_reset, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_ALTO_P_2_INT, 10, 1, ramon_special_handling, ramon_hard_reset, "If Alto occurs once - no error. If Alto occurs consistently - need to check which links are involved. If only one link has recurring Alto (3 occurrences within 600 seconds) - power down the link. If multiple links have Alto repeatedly (10 occurrences within 1 second) - suggest to collect dumps from the entire system and hard reset the device ");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_DCH_TYPE_ERR_INT, 100, 1, ramon_none, ramon_interrupt_handle_DCH_TypeErrInt, "This interrupt indicates cell corruption. If happns constantly - need to check link.");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_0, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_1, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_2, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_3, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_4, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_5, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_6, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_SYNC_STATUS_CHANGED_7, 0, 0, ramon_interrupt_handle_fsrd_fsrd_SyncStatusChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_0, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_1, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_2, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_3, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_4, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_5, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_6, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FSRD_QUAD_0_FSRD_N_RX_LOCK_CHANGED_7, 0, 0, ramon_interrupt_handle_fsrd_fsrd_RxLockChanged, ramon_none, "Tune SerDes"); 
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_0, 0, 0, ramon_interrupt_handle_fmac_fmac_LOS_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_1, 0, 0, ramon_interrupt_handle_fmac_fmac_LOS_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_2, 0, 0, ramon_interrupt_handle_fmac_fmac_LOS_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_LOS_INT_3, 0, 0, ramon_interrupt_handle_fmac_fmac_LOS_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_0, 0, 0, ramon_interrupt_handle_fmac_fmac_RxLostOfSync, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_1, 0, 0, ramon_interrupt_handle_fmac_fmac_RxLostOfSync, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_2, 0, 0, ramon_interrupt_handle_fmac_fmac_RxLostOfSync, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_2_RX_LOST_OF_SYNC_3, 0, 0, ramon_interrupt_handle_fmac_fmac_RxLostOfSync, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_0, 0, 0, ramon_interrupt_handle_fmac_fmac_OOF_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_1, 0, 0, ramon_interrupt_handle_fmac_fmac_OOF_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_2, 0, 0, ramon_interrupt_handle_fmac_fmac_OOF_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_OOF_INT_3, 0, 0, ramon_interrupt_handle_fmac_fmac_OOF_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_0, 0, 0, ramon_interrupt_handle_fmac_fmac_DecErr_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_1, 0, 0, ramon_interrupt_handle_fmac_fmac_DecErr_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_2, 0, 0, ramon_interrupt_handle_fmac_fmac_DecErr_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_FMAC_4_DEC_ERR_INT_3, 0, 0, ramon_interrupt_handle_fmac_fmac_DecErr_Int, ramon_none, "Tune SerDes");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_0, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_overflow, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_1, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_overflow, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_2, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_overflow, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_3, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_overflow, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_1_3");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_0, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_underrun, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_0");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_1, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_underrun, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_1");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_2, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_underrun, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_2");
    dnxc_intr_add_handler_ext(unit, RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_3, 0, 0, ramon_interrupt_handle_fmac_tx_ilkn_underrun, ramon_none, "RAMON_INT_FMAC_INTERRUPT_REGISTER_FIELD_2_3");



}


#undef BSL_LOG_MODULE
