
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

/* 
 *  include  
 */ 
#include <sal/core/time.h>
#include <sal/core/dpc.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/ARAD/arad_api_dram.h> 
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h> 
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_em_ser.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_ports.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/fabric.h>
#include <bcm/port.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/l2.h>

#include <appl/diag/system.h>

#include <appl/dpp/interrupts/interrupt_handler.h>
#include <appl/dpp/interrupts/interrupt_handler_cb_func.h>
#include <appl/dpp/interrupts/interrupt_handler_corr_act_func.h>

#include <appl/dcmn/rx_los/rx_los.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>

#include <soc/sand/sand_ser_correction.h>


#define ARAD_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE 16384
#define ARAD_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY 8

STATIC arad_interrupt_type arad_inaccessible_mem_ser_interrupts[] =
{
    ARAD_INT_EGQ_ECC_2BERRINT,
    ARAD_INT_EGQ_PARITYERRINT,
    ARAD_INT_EPNI_ECC_2BERRINT,
    ARAD_INT_EPNI_PARITYERRINT,
    ARAD_INT_FDR_PRMECC_2BERRINT,
    ARAD_INT_FDR_SECECC_2BERRINT,
    ARAD_INT_IDR_PARITYERRINT,
    ARAD_INT_IHP_PARITYERRINT,
    ARAD_INT_IRE_ECC_2BERRINT,
    ARAD_INT_IRE_PARITYERRINT,
    ARAD_INT_IRR_ECC_2BERRINT,
    ARAD_INT_MMU_ECC_2BERRINT,
    ARAD_INT_NBI_ECC_2BERRINT,
    ARAD_INT_NBI_PARITYERRINT,
    ARAD_INT_LAST
};

/*
 *  Corrective Action Utility functions
 */


/*
 *  Corrective Action functions    
 */






int
arad_interrupt_handles_corrective_action_force(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "%s(): unit=%d, block_instance=%d, interrupt_id=%d\n"), FUNCTION_NAME(), unit, block_instance, interrupt_id));
#endif

    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handles_corrective_action_ingress_hard_reset(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset!\n")));
#endif

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handles_corrective_action_hard_reset_without_fabric(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    char *msg)
{
    int rc = 0;
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: Hard reset without Fabric!\n")));
#endif

    rc = soc_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DPP_RESET_ACTION_INOUT_RESET);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_interrupt_handles_corrective_action_handle_crc_del_buf_fifo(
                                                int unit,
                                                int block_instance,
                                                arad_interrupt_type en_arad_interrupt,
                                                char* msg)
{
    int rc = 0, i;
    uint32  del_buf_num, del_buf[ARAD_INTERRUPT_IPT_MAX_CRC_DELETED_FIFO_DEPTH];
    arad_dram_buffer_info_t buf_info;
    uint32 max_error_for_buffer;

    SOCDNX_INIT_FUNC_DEFS;

    /* get the deleted buffer list */
    
    rc =  arad_dram_delete_buffer_read_fifo(unit, ARAD_INTERRUPT_IPT_MAX_CRC_DELETED_FIFO_DEPTH/*max_entries*/, del_buf, &del_buf_num);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_dram_crc_del_buffer_max_reclaims_get(unit, & max_error_for_buffer);
    SOCDNX_IF_ERR_EXIT(rc);    
   
    for(i=0; i<del_buf_num; ++i ) { 
  
        /* get info on the bufer */
        rc = arad_dram_buffer_get_info(unit, del_buf[i], &buf_info);
        SOCDNX_IF_ERR_EXIT(rc);
         /* if error counter cross the limit of max error for buffer -  delete the buffer */
        if( buf_info.err_cntr >= max_error_for_buffer) {
             rc =  arad_dram_delete_buffer_action(unit, del_buf[i], 1);
             SOCDNX_IF_ERR_EXIT(rc);
        } else { /* else - release the buffer */
                rc = arad_dram_delete_buffer_action(unit, del_buf[i], 0);
                SOCDNX_IF_ERR_EXIT(rc);
        }
                   
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handles_corrective_action_handle_oamp_event_fifo(int unit, 
                                                                       int block_instance, 
                                                                       arad_interrupt_type en_arad_interrupt, 
                                                                       char* msg)
{
/*    int rc;  */

    SOCDNX_INIT_FUNC_DEFS;


     sal_dpc(soc_ppd_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT), 0, 0, 0);


goto exit;
exit:
    SOCDNX_FUNC_RETURN;
}
 
int 
arad_interrupt_handles_corrective_action_clear_check(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    int *iterations,
    int *delay_us)
{
    bcm_switch_event_control_t event_bcm_switch_event;
    int i;
    uint32 value = 0;
    int rv = BCM_E_NONE;
 
    event_bcm_switch_event.event_id = interrupt_id;
    event_bcm_switch_event.index = block_instance;
    for (i = 0; i < *iterations; i++ ) {
        /*set clear event*/
        event_bcm_switch_event.action = bcmSwitchEventClear;
        rv = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event, 1);
        if(rv != SOC_E_NONE) {
            return rv;
        }

        sal_usleep(*delay_us);

        event_bcm_switch_event.action = bcmSwitchEventRead;
        rv = bcm_switch_event_control_get( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event, &value);
        if(rv != SOC_E_NONE) {
            return rv;
        }
        if (value == 0) {
            break;
        }
    }
    return rv;
}
 
int arad_interrupt_handles_corrective_action_handle_mact_event_fifo(int unit, 
                                                                    int block_instance, 
                                                                    arad_interrupt_type en_arad_interrupt,
                                                                    char* msg)
{
    int rc;
    soc_reg_above_64_val_t pr_reg;
    uint32
      fld_val, reg_value;

    SOCDNX_INIT_FUNC_DEFS;

    /* Read and handle all event-fifo contents */
    while(1)
    {
        /* Verify an event is present in the Event FIFO via its interrupt */
        rc = soc_reg32_get(unit, IHP_MACT_INTERRUPT_REGISTER_TWOr, (REG_PORT_ANY), 0, &reg_value);
        SOCDNX_IF_ERR_EXIT(rc);
        fld_val = soc_reg_field_get(unit, IHP_MACT_INTERRUPT_REGISTER_TWOr, reg_value, MACT_EVENT_READYf);
        /* Break this while if haven't contents in this buffer */
        if (fld_val == 0x0)
        {
            break;
        }
        /* Data collection */
        rc = soc_reg_above_64_get(unit, IHP_MACT_EVENTr, block_instance, 0, pr_reg);
        SOCDNX_IF_ERR_EXIT(rc);  

        /* Process event-FIFO record */
        rc = _bcm_dpp_l2_event_fifo_interrupt_handler(unit, (void*)pr_reg);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
} 


int arad_interrupt_handles_corrective_action_shutdown_unreach_destination(int unit, 
                                                                          int block_instance, 
                                                                          arad_interrupt_type interrupt_id, 
                                                                          uint32 fap_id)
{
    int rc, ndx ,n_flow;
    uint32 *alloced_mem =NULL,
           entry,
           tmp_fap_id,
           flow;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     *  Get the SCH_FLOW_TO_FIP_MAPPING__FFM full table.
     */

    if (soc_mem_dmaable(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, SOC_MEM_BLOCK_ANY(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm))) {
     
        alloced_mem = soc_cm_salloc(unit, 4 * ARAD_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE, "sch_flow2fip_mapping_ffm_dma");

        if(alloced_mem != NULL) {

            rc = soc_mem_read_range(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, MEM_BLOCK_ANY, 0, ARAD_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE - 1 , alloced_mem);
            if(rc != SOC_E_NONE) {
                soc_cm_sfree(unit, alloced_mem);
                alloced_mem = NULL;                
            }
        }

    }

    /* 
     *  Loop on all entries of SCH_FLOW_TO_FIP_MAPPING__FFM
     */
    for(ndx=0; ndx < ARAD_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE; ++ndx) {

        /* 
         * Get fap_id from the SCH_FLOW_TO_FIP_MAPPING__FFM and compare to the fap_id we search
         */
        if(alloced_mem != NULL) {
            sal_memset(&entry,  *(alloced_mem + ndx), sizeof(uint32));   
        } else {
            rc = soc_mem_read(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, MEM_BLOCK_ANY, ndx, &entry);
            SOCDNX_IF_ERR_EXIT(rc);
        }

        tmp_fap_id = soc_mem_field32_get(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, &entry , DEVICE_NUMBERf);

        if(tmp_fap_id == fap_id) {
           
            /* 
             * Each entry in SCH_FLOW_TO_FIP_MAPPING__FFM represent 8 flows 
             */                
            for(n_flow=0; n_flow < ARAD_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY ; n_flow++) {

                flow = ndx * ARAD_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY + n_flow;
            
                    rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_flow_status_set, (unit,SOC_CORE_DEFAULT, flow, ARAD_SCH_FLOW_OFF));
                    if (soc_sand_get_error_code_from_error_word(rc) != SOC_SAND_OK) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to set flow status"))); 
                    }       
                
            }
                       
        }
    }

exit:

    if (alloced_mem != NULL) {
        soc_cm_sfree(unit, alloced_mem);
    }

    SOCDNX_FUNC_RETURN;
}


/*
 *  Data collection functions    
 */
 
int arad_interrupt_data_collection_for_mac_rxcrcerrn_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    uint8*  array_links,
    char* special_msg)

{
    int rc;
    int link;
    uint64 value, val64;
    uint32 flags;
    int interval;
    pbmp_t pbmp;
    char str_value[ARAD_INTERRUPT_MESSAGE_FOR_CONVERSION];
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(array_links);
    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* get link number */
    rc = arad_interrupt_fmac_link_get(unit, block_instance ,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);   
    
    /* get soc counter status */
    rc = soc_counter_status(unit, &flags, &interval, &pbmp);
    SOCDNX_IF_ERR_EXIT(rc);

    /* read statistic counter */
    if(interval != 0)
    {
        rc = bcm_stat_sync(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    
    rc = bcm_stat_get(unit, link, snmpBcmRxCrcErrors, &value);
    SOCDNX_IF_ERR_EXIT(rc);

    array_links[link] = ARAD_INT_LINK_SHUTDOWN;
    COMPILER_64_SET(val64, 0, ARAD_INT_CRC_MASK );
    COMPILER_64_AND(value, val64);
    format_uint64(str_value, value);
    sal_sprintf(special_msg, "link=%d, fmal_statistic_output_control_crc=%s", link, str_value);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_mac_wrongsize_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char* special_msg)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);
    sal_sprintf(special_msg, "link=%d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_recurring_action_mac_wrongsize_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char* special_msg,
    uint8*  array_links)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);
    array_links[link] = ARAD_INT_LINK_SHUTDOWN;
    sal_sprintf(special_msg, "link = %d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_mac_oof_int( 
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    char* special_msg)
{
    int rc;
    int link;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_pcs);
    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_port_control_pcs_get( unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link), p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    if(soc_dcmn_port_pcs_8_10!= (*p_pcs))
    {
        sal_sprintf(special_msg, "The receiver frame decoder lost of frame alignment for link %d", link);
    }
    else{
        sal_sprintf(special_msg, "Out of frame interrupt received for link %d for for encoding type 8/10, for this mode interrupt should be masked ",link);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_interrupt_data_collection_for_fdrprmifmbfob(int unit,int block_instance, dcmn_int_corr_act_type *corr_act , soc_reg_t reg, soc_field_t field, char* special_msg)
{
    uint32 err_field_cnt,err_cnt;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = soc_reg32_get(unit, reg, block_instance, 0, &err_cnt );
    SOCDNX_IF_ERR_EXIT(rc);  

    err_field_cnt = soc_reg_field_get(unit, reg, err_cnt, field);
    if (err_field_cnt < 250) {
        sal_sprintf(special_msg, "IFMB FIFO overflow - path.err_field_cnt = 0x%08x", err_field_cnt);
        *corr_act = DCMN_INT_CORR_ACT_NONE;
    } else {
        sal_sprintf(special_msg, "IFMB FIFO overflow - path.err_field_cnt = 0x%08x illeagal size, hard reset", err_field_cnt);
        *corr_act = DCMN_INT_CORR_ACT_HARD_RESET;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_interrupt_data_collection_for_recurring_mac_oof_int( 
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    soc_dcmn_port_pcs_t* p_pcs,
    dcmn_int_corr_act_type* p_corrective_action,
    char* special_msg,
    uint8*  array_links)
{
    int rc;
    int link;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_pcs);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    *p_corrective_action = DCMN_INT_CORR_ACT_NONE ;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);

    rc = arad_port_control_pcs_get( unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link) ,p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    if(soc_dcmn_port_pcs_8_10!= (*p_pcs))
    {
        sal_sprintf(special_msg, "The receiver frame decoder lost of frame alignment for link %d", link);
        array_links[link] = ARAD_INT_LINK_SHUTDOWN;
        *p_corrective_action = DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS;
    }
    else{
        sal_sprintf(special_msg, "Out of frame interrupt received for link %d for for encoding type 8/10, for this mode interrupt should be masked ",link);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_mac_los_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char* special_msg)
{
    int link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);

    sal_sprintf(special_msg, "link=%d ",link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_mac_rxlostofsync_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    soc_port_t *port,
    char* special_msg)
{
    soc_port_t link;
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);

    *port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link);


    sal_sprintf(special_msg, "link=%d, diagnose SerDes ", link);
    
exit:
    SOCDNX_FUNC_RETURN;
}


int 
arad_interrupt_data_collection_for_mac_decerr_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    int is_recurring,
    dcmn_int_corr_act_type* p_corrective_action,
    char* special_msg,
    uint8*  array_links)

{
    int rc;
    int link;
    uint64 value, val64;
    uint32 flags;
    int interval;
    pbmp_t pbmp;
    soc_dcmn_port_pcs_t p_pcs;
    bcm_stat_val_t error_type ;
    char mode_string[ARAD_INTERRUPT_MESSAGE_FOR_CONVERSION];
    char str_value[ARAD_INTERRUPT_MESSAGE_FOR_CONVERSION];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* update default corrective action */
    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
    
    /* get link number */
    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);   
 
    rc = arad_port_control_pcs_get( unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link) ,&p_pcs);
    SOCDNX_IF_ERR_EXIT(rc);
 
    switch(p_pcs) {
        case soc_dcmn_port_pcs_8_9_legacy_fec:
            sal_strcpy(mode_string,"8_9_legacy_fec");
            error_type = snmpBcmRxFecUncorrectable ;
            break;
        case soc_dcmn_port_pcs_64_66_fec:
            sal_strcpy(mode_string,"64_66_fec");
            error_type = snmpBcmRxFecUncorrectable ;
            break;
        case soc_dcmn_port_pcs_64_66_bec:
            sal_strcpy(mode_string,"64_66_bec");
            error_type = snmpBcmRxBecRxFault;
            break;
        case soc_dcmn_port_pcs_8_10:
            sal_strcpy(mode_string,"8_10");
            error_type = snmpBcmRxCodeErrors;
            break;
        default:
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
        
    /* get soc counter status */
    rc = soc_counter_status(unit, &flags, &interval, &pbmp);
    SOCDNX_IF_ERR_EXIT(rc);

    if(interval!= 0){
        /* read statistic counter */
        rc = bcm_stat_sync(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    
    rc = bcm_stat_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link), error_type, &value);
    SOCDNX_IF_ERR_EXIT(rc);

    /* define corrective action  and fill shutdown link structure   */
    COMPILER_64_SET(val64, 0, ARAD_INT_FEC_UNCORRECTABLE_MASK);
    COMPILER_64_AND(value, val64);
    COMPILER_64_SET(val64, 0, ARAD_INT_FEC_UNCORRECTABLE_THRESHOLD);
    if(COMPILER_64_GT(value, val64) && is_recurring) {
        SOCDNX_NULL_CHECK(array_links);
        array_links[link] = ARAD_INT_LINK_SHUTDOWN;
        *p_corrective_action = DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS;
    }
  
    format_uint64(str_value, value);
    sal_sprintf(special_msg, "link=%d, pcs mode=%s, fmal_statistic_output_control=%s", link, mode_string, str_value);
    
exit:
    SOCDNX_FUNC_RETURN;
}


int arad_interrupt_data_collection_for_mac_transmit_err_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char* special_msg,
    dcmn_int_corr_act_type* p_corrective_action,
    uint8* array_links)
{
    int rc;
    int link;
    soc_dcmn_port_pcs_t pcs;
    char mode_string[20];

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(array_links);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

     /* update default corrective action */
    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;

    /* get link number */
    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link );
    SOCDNX_IF_ERR_EXIT(rc);
 
    rc = arad_port_control_pcs_get( unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link) ,&pcs);
    SOCDNX_IF_ERR_EXIT(rc);

    /* perform mode string and select shut down link for 8_10 mode */
    switch(pcs) {
    case soc_dcmn_port_pcs_8_9_legacy_fec:
        sal_strcpy(mode_string,"8_9_legacy_fec");
        break;
    case soc_dcmn_port_pcs_64_66_fec:
        sal_strcpy(mode_string,"64_66_fec");
        break;
    case soc_dcmn_port_pcs_64_66_bec:
        sal_strcpy(mode_string,"64_66_bec");
        break;
    case soc_dcmn_port_pcs_8_10:
        sal_strcpy(mode_string,"8_10");
        *p_corrective_action = DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS;
        break;
    default:
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    sal_sprintf(special_msg, "link=%d, pcs mode=%s", link, mode_string);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_mac_lnklvlagectxbn_int(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    char* special_msg)
{
    uint32 lnk_lvl_flow_control_conf_reg;
    uint32 lnk_lvl_age_prd;
    int link;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = arad_interrupt_fmac_link_get(unit, block_instance,SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, &link);
    SOCDNX_IF_ERR_EXIT(rc);  
    
    rc = soc_reg32_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, block_instance, 0, &lnk_lvl_flow_control_conf_reg );
    SOCDNX_IF_ERR_EXIT(rc);  

    lnk_lvl_age_prd = soc_reg_field_get(unit, BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_CONFIGURATION_REGISTERr, lnk_lvl_flow_control_conf_reg, LNK_LVL_AGE_PRDf);
    sal_sprintf(special_msg,"link = %d, LnkLvlAgePrd = 0x%01x " ,link, lnk_lvl_age_prd );

exit:

    SOCDNX_FUNC_RETURN;
}


int
arad_interrupt_data_collection_for_shadowing(
    int unit, 
    int block_instance, 
    arad_interrupt_type en_arad_interrupt, 
    char* special_msg, 
    dcmn_int_corr_act_type* p_corrective_action,
    dcmn_interrupt_mem_err_info* shadow_correct_info)
{
    int rc;
    uint64 counter;
    uint32 counter32;
    uint32 cntf=0, cnt_overflowf=0, addrf=0, addr_validf=0;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name; 
    int cached_flag;
    uint32 block;
    int32 is_par=0, is_ecc_2b=0;
    ARAD_PP_EM_SER_TYPE em_ser_type = ARAD_PP_EM_SER_TYPE_LAST; 
    soc_mem_t em_ser_mem; 
    uint32 dummy_addr;
    ARAD_PP_EM_SER_BLOCK em_ser_block = ARAD_PP_EM_SER_NOF_BLOCKS;
    soc_pbmp_t port_bm;
    soc_port_t port_i;
    soc_port_if_t interface;
    uint32 offset, num_of_lanes, ilkn_num = 0, ilkn_lane = 0, is_master;
    sand_memory_dc_t type ;
    uint32 i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(shadow_correct_info);

    switch(en_arad_interrupt) {
    case(ARAD_INT_SCH_FDMSPARERROR):
    case(ARAD_INT_SCH_SHDSPARERROR):
    case(ARAD_INT_SCH_FQMPARERROR):
    case(ARAD_INT_SCH_FFMPARERROR):
    case(ARAD_INT_SCH_FGMPARERROR):
    case(ARAD_INT_SCH_FSFPARERROR):
    case(ARAD_INT_SCH_SHCPARERROR):
    case(ARAD_INT_SCH_SCCPARERROR):
    case(ARAD_INT_SCH_SCTPARERROR):
    case(ARAD_INT_SCH_DSMPARERROR):
    case(ARAD_INT_SCH_CAL0PARERROR):
    case(ARAD_INT_SCH_CAL1PARERROR):
    case(ARAD_INT_SCH_CAL2PARERROR):
    case(ARAD_INT_SCH_CAL3PARERROR):
    case(ARAD_INT_SCH_CAL4PARERROR):
    case(ARAD_INT_SCH_DRMPARERROR):
    case(ARAD_INT_SCH_CSSTPARERROR):
    case(ARAD_INT_SCH_PSWPARERROR):
    case(ARAD_INT_SCH_DPNPARERROR):
    case(ARAD_INT_SCH_PSSTPARERROR):
    case(ARAD_INT_SCH_CSDTPARERROR):
    case(ARAD_INT_SCH_FIMPARERROR):
    case(ARAD_INT_SCH_PSDDPARERROR):
    case(ARAD_INT_SCH_PSDTPARERROR):
    case(ARAD_INT_SCH_SEMPARERROR):
    case(ARAD_INT_SCH_SIMPARERROR):
    case(ARAD_INT_SCH_TMCPARERROR):

        rc = READ_SCH_PARITY_ERR_ADDRr(unit, &counter32);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg_field_get(unit, SCH_PARITY_ERR_ADDRr, counter32, PARITY_ERR_ADDR_VALIDf);
        /* get memory address bit */
        addrf = soc_reg_field_get(unit, SCH_PARITY_ERR_ADDRr, counter32, PARITY_ERR_ADDRf);

         rc = READ_SCH_PARITY_ERR_CNTr(unit, REG_PORT_ANY, &counter);
        SOCDNX_IF_ERR_EXIT(rc);
         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, SCH_PARITY_ERR_CNTr, counter, PARITY_ERR_CNT_OVERFLOWf);
         /* get counter value of sch error */
        cntf = soc_reg64_field32_get(unit, SCH_PARITY_ERR_CNTr, counter, PARITY_ERR_CNTf);

        block = ARAD_INTERRUPT_SCH_BLOCK;
        cnt_reg = SCH_PARITY_ERR_ADDRr ; 
             
        break;

    case(ARAD_INT_SCH_DHDECCERROR):
    case(ARAD_INT_SCH_DCDECCERROR):
    case(ARAD_INT_SCH_FLHHRECCERROR):
    case(ARAD_INT_SCH_FLTHRECCERROR):
    case(ARAD_INT_SCH_FLHCLECCERROR):
    case(ARAD_INT_SCH_FLTCLECCERROR):
    case(ARAD_INT_SCH_FLHFQECCERROR):
    case(ARAD_INT_SCH_FLTFQECCERROR):
    case(ARAD_INT_SCH_FDMDECCERROR):
    case(ARAD_INT_SCH_SHDDECCERROR):
    case(ARAD_INT_SCH_FSMECCERROR):
        rc = READ_SCH_ECC_2B_ERR_ADDRr(unit, &counter32);
        SOCDNX_IF_ERR_EXIT(rc);

         /* get address validity bit */
         addr_validf = soc_reg_field_get(unit, SCH_ECC_2B_ERR_ADDRr, counter32, ECC_2B_ERR_ADDR_VALIDf);
         /* get memory address bit */
         addrf = soc_reg_field_get(unit, SCH_ECC_2B_ERR_ADDRr, counter32, ECC_2B_ERR_ADDRf);

         rc = READ_SCH_ECC_2B_ERR_CNTr(unit, REG_PORT_ANY, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, SCH_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNT_OVERFLOWf);
        /* get counter value of sch error */
        cntf = soc_reg64_field32_get(unit, SCH_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNTf);

        block = ARAD_INTERRUPT_SCH_BLOCK;
        cnt_reg = SCH_ECC_2B_ERR_ADDRr;
   
        break;

    case(ARAD_INT_FMAC_ECC_2BERRINT):
        rc = READ_FMAC_ECC_2B_ERR_ADDRr(unit, block_instance, &counter32);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg_field_get(unit, FMAC_ECC_2B_ERR_ADDRr, counter32, ECC_2B_ERR_ADDR_VALIDf);
        /* get memory address bit */
        addrf = soc_reg_field_get(unit, FMAC_ECC_2B_ERR_ADDRr, counter32, ECC_2B_ERR_ADDRf);

        rc = READ_FMAC_ECC_2B_ERR_CNTr(unit, block_instance, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, FMAC_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNT_OVERFLOWf);
         /* get counter value of sch error */
        cntf = soc_reg64_field32_get(unit, FMAC_ECC_2B_ERR_CNTr, counter, ECC_2B_ERR_CNTf);

        block = ARAD_INTERRUPT_FMAC_FIRST_BLOCK + block_instance;
        cnt_reg = FMAC_ECC_2B_ERR_ADDRr;
       
        break;  
    /* parity error */
    case(ARAD_INT_FDT_PARITYERRINT):
        cnt_reg = FDT_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_FDT_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_CFC_PARITYERRINT):
        cnt_reg = CFC_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_CFC_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_IQM_PARITYERRINT):
        cnt_reg = IQM_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IQM_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_IPS_PARITYERRINT):
        cnt_reg = IPS_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IPS_BLOCK;   
        is_par = TRUE;
        break;
  
    case(ARAD_INT_CRPS_PARITYERRINT):
        cnt_reg = CRPS_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_CRPS_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_IHP_PARITYERRINT):
        cnt_reg = IHP_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IHP_BLOCK;
        em_ser_block = ARAD_PP_EM_SER_BLOCK_IHP;
        is_par = TRUE;
        break;

    case(ARAD_INT_IHB_PARITYERRINT):
        cnt_reg = IHB_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IHB_BLOCK;
        em_ser_block = ARAD_PP_EM_SER_BLOCK_IHB;
        is_par = TRUE;
        break;

    case(ARAD_INT_OAMP_PARITYERRINT):
        cnt_reg = OAMP_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_OAMP_BLOCK;
        em_ser_block = ARAD_PP_EM_SER_BLOCK_OAMP;
        is_par = TRUE;
        break;

    case(ARAD_INT_IDR_PARITYERRINT):
        cnt_reg = IDR_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IDR_BLOCK;   
        is_par = TRUE;
        break;

    case(ARAD_INT_EGQ_PARITYERRINT):
        cnt_reg = EGQ_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_EGQ_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_EPNI_PARITYERRINT):
        cnt_reg = EPNI_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_EPNI_BLOCK;
        em_ser_block = ARAD_PP_EM_SER_BLOCK_EPNI;
        is_par = TRUE;
        break;

    case(ARAD_INT_IRR_PARITYERRINT):
        cnt_reg = IRR_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IRR_BLOCK;
        is_par = TRUE;
        break;

     case(ARAD_INT_IRE_PARITYERRINT):
        cnt_reg = IRE_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IRE_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_IPT_PARITYERRINT):
        cnt_reg = IPT_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_IPT_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_OLP_PARITYERRINT):
        cnt_reg = OLP_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_OLP_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_NBI_PARITYERRINT):
        cnt_reg = NBI_PARITY_ERR_CNTr;
        block = ARAD_INTERRUPT_NBI_BLOCK;
        is_par = TRUE;
        break;

    case(ARAD_INT_EGQ_ECC_2BERRINT):
        cnt_reg = EGQ_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_EGQ_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_EPNI_ECC_2BERRINT):
        cnt_reg = EPNI_ECC_2B_ERR_CNTr;   
        block = ARAD_INTERRUPT_EPNI_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_FDT_ECC_2BERRINT):
        cnt_reg = FDT_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_FDT_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_IDR_ECC_2BERRINT):
        cnt_reg = IDR_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IDR_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_IHB_ECC_2BERRINT):
        cnt_reg = IHB_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IHB_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_IHP_ECC_2BERRINT):
        cnt_reg = IHP_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IHP_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_IPS_ECC_2BERRINT):
        cnt_reg = IPS_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IPS_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_IPT_ECC_2BERRINT):
        cnt_reg = IPT_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IPT_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_IQM_ECC_2BERRINT):
        cnt_reg = IQM_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IQM_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_IRE_ECC_2BERRINT):
        cnt_reg = IRE_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IRE_BLOCK;
        is_ecc_2b =  TRUE;
        break;

     case(ARAD_INT_IRR_ECC_2BERRINT):
        cnt_reg = IRR_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_IRR_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_MMU_ECC_2BERRINT):
        cnt_reg = MMU_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_MMU_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_NBI_ECC_2BERRINT):
        cnt_reg = NBI_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_NBI_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_OAMP_ECC_2BERRINT):
        cnt_reg = OAMP_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_OAMP_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_OLP_ECC_2BERRINT):
        cnt_reg = OLP_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_OLP_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    case(ARAD_INT_FCR_ECC_2BERRINT):
        cnt_reg = FCR_ECC_2B_ERR_CNTr;
        block = ARAD_INTERRUPT_FCR_BLOCK;
        is_ecc_2b =  TRUE;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_arad_interrupt));
      }

    type = sand_get_cnt_reg_type(unit, cnt_reg); 

    if (type==SAND_INVALID_DC) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_arad_interrupt));
    }

    if(is_par) {

         rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
         SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_ADDR_VALIDf);

        /* get memory address bit */
        addrf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_ADDRf);

         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_CNT_OVERFLOWf);

        /* get counter value of 2 bit error */
        cntf = soc_reg64_field32_get(unit, cnt_reg, counter, PARITY_ERR_CNTf);
    }    

    if(is_ecc_2b) {

        rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
         SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_ADDR_VALIDf);

        /* get memory address bit */
        addrf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_ADDRf);

        /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_CNT_OVERFLOWf);

        /* get counter value of 2 bit error */
        cntf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_2B_ERR_CNTf);
    }

    if (addr_validf != 0) {

       /*
        *  handle inaccessible memory ECC errors
        */
        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);
        if(mem == INVALIDm) {
            for(i = 0; arad_inaccessible_mem_ser_interrupts[i] != ARAD_INT_LAST; i++) {
                if(en_arad_interrupt == arad_inaccessible_mem_ser_interrupts[i]) {
                    *p_corrective_action = DCMN_INT_CORR_ACT_SOFT_RESET;
                    break;
                }
            }
        }
       /*
        *  handle EGQ BufLink false ECC errors
        */
        if((  (en_arad_interrupt == ARAD_INT_EGQ_PARITYERRINT)  || (en_arad_interrupt == ARAD_INT_EGQ_ECC_2BERRINT) ) 
                && (addrf >= 0xa0000 && addrf <= (0xa0000 + 12288))) {
                
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x",
            cntf, cnt_overflowf, addrf);
                            
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            rc = SOC_E_NONE;
            SOC_EXIT;       
        }
       /*
        *  Handle IRE WDF Parity errors - perform ING soft reset
        */
         if ((en_arad_interrupt == ARAD_INT_IRE_PARITYERRINT) && 
            ((addrf >= 0x1000) && (addrf <= (0x1000 + 2048)))) {
                
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x",
            cntf, cnt_overflowf, addrf);
                            
            *p_corrective_action = DCMN_INT_CORR_ACT_INGRESS_HARD_RESET;
            rc = SOC_E_NONE;
            SOC_EXIT;       
        }
       /*
        *  Handle IDR RSQ_FIFO_CONTROL & RPF_MEMORY ecc_2b error - should not cause soft reset (action == none)
        */
         if ((en_arad_interrupt == ARAD_INT_IDR_ECC_2BERRINT) && 
            (((addrf >= 0x300000) && (addrf < (0x300000 + 4096))) || (((addrf >= 0x3c0000) && (addrf < (0x3c0000 + 4096)))))) {
                
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x", cntf, cnt_overflowf, addrf);
                            
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            rc = SOC_E_NONE;
            SOC_EXIT;       
        }
       /*
        *  Handle NBI lknTx0LbsAdapMem, lknTx1LbsAdapMem & IlknRxAlgnMem[0-23] ecc_2b error - should not cause soft reset (action == none)
        */
         if ((en_arad_interrupt == ARAD_INT_NBI_ECC_2BERRINT) && 
            (((addrf >= 0x120c00) && (addrf < (0x120c00 + 32))) || (((addrf >= 0x121000) && (addrf < (0x121000 + 32)))) || (((addrf >= 0x120000) && (addrf < (0x120000 + 24 * 16)))))) {
                
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x", cntf, cnt_overflowf, addrf);
                            
            if (addrf >= 0x121000) {
                ilkn_num = 1;
            } else if (addrf >= 0x120c00) {
                ilkn_num = 0;
            } else {
                ilkn_lane = (addrf - 0x120000) / 16;
            }
            SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm)));
            SOC_PBMP_ITER(port_bm, port_i){
                SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_is_master_get(unit, port_i, &is_master)));
                if(!is_master){
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_interface_type_get(unit, port_i, &interface)));
                if(interface == SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port_i, 0, &offset));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, &num_of_lanes));
                    if (((addrf >= 0x120c00) && (ilkn_num == offset)) || ((addrf < 0x120c00) && (num_of_lanes > ilkn_lane))) {
                        *p_corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;
                        rc = SOC_E_NONE;
                        SOC_EXIT;       
                    }
                }
            }
        }
       /*
        *  Handle NBI IlknTx0LnExpnMem[0-23] & IlknTx1LnExpnMem[24-35] parity error - should cause soft reset
        */
         if ((en_arad_interrupt == ARAD_INT_NBI_PARITYERRINT) && 
            (((addrf >= 0x120400) && (addrf < (0x120400 + 8 * 24))) || (((addrf >= 0x120500) && (addrf < (0x120500 + 8 * 12)))))) {
                
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x", cntf, cnt_overflowf, addrf);
                            
            if (addrf >= 0x120500) {
                ilkn_num = 1;
                ilkn_lane = (addrf - 0x120500) / 8;
            } else {
                ilkn_num = 0;
                ilkn_lane = (addrf - 0x120400) / 8;
            }
            SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm)));
            SOC_PBMP_ITER(port_bm, port_i){
                SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_is_master_get(unit, port_i, &is_master)));
                if(!is_master){
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_port_sw_db_interface_type_get(unit, port_i, &interface)));
                if(interface == SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port_i, 0, &offset));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, &num_of_lanes));
                    if ((ilkn_num == offset) && (num_of_lanes > ilkn_lane)) {
                        *p_corrective_action = DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC;
                        rc = SOC_E_NONE;
                        SOC_EXIT;       
                    }
                }
            }
        }
        /* NBI_MLF_RX_MEM_A/B, should be treated as ignore */
        if((en_arad_interrupt == ARAD_INT_NBI_PARITYERRINT) && (addrf >= 0x100000 && addrf < 0x102000)) {
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x",
                                     cntf, cnt_overflowf, addrf);
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            rc = SOC_E_NONE;
            SOC_EXIT;
        }
        /* NBI_TX_MEM, should be treated as ignore */
        if((en_arad_interrupt == ARAD_INT_NBI_PARITYERRINT) && (addrf >= 0x104000 && addrf < 0x105000)) {
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x",
                             cntf, cnt_overflowf, addrf);
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            rc = SOC_E_NONE;
            SOC_EXIT;
        }
        /* NBI_{R/T}MON_MEM, should be treated as ignore */
        if((en_arad_interrupt == ARAD_INT_NBI_PARITYERRINT) && (addrf >= 0x110000 && addrf < 0x110040)) {
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x",
                             cntf, cnt_overflowf, addrf);
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            rc = SOC_E_NONE;
            SOC_EXIT;
        }

        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);

        if(mem!= INVALIDm) {
            memory_name = SOC_MEM_NAME(unit, mem);
        } else {
            memory_name = NULL;
        }

        switch(mem) {
        case INVALIDm:
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory is not accessible",
                    cntf, cnt_overflowf, addrf);
            break;     
        /* handle nbi_{r/t}bins_)mem, nbi_{r/t}leng_men,  that are for statics, no need reset. should be treated as ignore */
        case NBI_RBINS_MEMm:
        case NBI_TBINS_MEMm:
        case NBI_RLENG_MEMm:
        case NBI_TLENG_MEMm:
        case NBI_RPKTS_MEMm:
        case NBI_TPKTS_MEMm:
        case NBI_RTYPE_MEMm:
        case NBI_TTYPE_MEMm:
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            /* set corrective action */
            *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
            shadow_correct_info->array_index = numels;
            shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
            shadow_correct_info->min_index = index;
            shadow_correct_info->max_index = index;
            shadow_correct_info->mem = mem;
            break;;
        /*
         *  Handle IPS QDESC - write zero to unused entries
         */
        case IPS_CRBALm:
        case IPS_QDESC_TABLEm:
        case IPS_QDESCm:
            mem = IPS_QDESC_TABLEm;
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            /* set corrective action */
            *p_corrective_action = DCMN_INT_CORR_ACT_IPS_QDESC;
            shadow_correct_info->array_index = numels;
            shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
            shadow_correct_info->min_index = index;
            shadow_correct_info->max_index = index;
            shadow_correct_info->mem = mem;
            break;

        default:
            /* check if it's EM SER */
            if ((block == ARAD_INTERRUPT_IHP_BLOCK)  || 
                (block == ARAD_INTERRUPT_IHB_BLOCK)  ||
                (block == ARAD_INTERRUPT_EPNI_BLOCK) ||
                (block == ARAD_INTERRUPT_OAMP_BLOCK)) {

                rc = arad_pp_em_ser_type_get(unit, addrf, em_ser_block, &em_ser_type, &em_ser_mem, &dummy_addr);
                SOCDNX_IF_ERR_EXIT(rc);
            }
            /* support for shadow memories*/
            rc = interrupt_memory_cached(unit, mem, block_instance, &cached_flag);
            if(rc != SOC_E_NONE) {
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory not cached",
                            cntf, cnt_overflowf, addrf);
            }
            else if(TRUE == cached_flag) {
            
                SOC_MEM_ALIAS_TO_ORIG(unit,mem);
                rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
                SOCDNX_IF_ERR_EXIT(rc);

                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                            cntf, cnt_overflowf, addrf, memory_name, numels, index);
                /* set corrective action */
                *p_corrective_action = DCMN_INT_CORR_ACT_SHADOW;
                shadow_correct_info->array_index = numels;
                shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
                shadow_correct_info->min_index = index;
                shadow_correct_info->max_index = index;
                shadow_correct_info->mem = mem;
                
            } 
            else if (em_ser_type != ARAD_PP_EM_SER_TYPE_LAST) {
                /* EM SER */
                rc = soc_mem_addr_to_array_element_and_index(unit, em_ser_mem, addrf, &numels, &index);
                SOCDNX_IF_ERR_EXIT(rc);

                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                            cntf, cnt_overflowf, addrf, memory_name, numels, index);
                shadow_correct_info->array_index = numels;
                shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, em_ser_mem);
                shadow_correct_info->min_index = index;
                shadow_correct_info->max_index = index;
                shadow_correct_info->mem = em_ser_mem;
                break;
            }
            else {
                rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
                SOCDNX_IF_ERR_EXIT(rc);
                shadow_correct_info->array_index = numels;
                shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
                shadow_correct_info->min_index = index;
                shadow_correct_info->max_index = index;
                shadow_correct_info->mem = mem;
                sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                            cntf, cnt_overflowf, addrf, memory_name, numels, index);
            }
            SOC_MEM_ALIAS_TO_ORIG(unit,shadow_correct_info->mem);
            rc = dcmn_mem_decide_corrective_action(unit,type,shadow_correct_info->mem,block_instance, p_corrective_action, special_msg);
            SOCDNX_IF_ERR_EXIT(rc);

           /*
            *  Handle IRR_MCDB Parity errors - perform recover from the cache and do soft reset
            */
            if (mem == IRR_MCDBm) {
                *p_corrective_action = DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET;
            }

            if (mem ==  IPS_QSZm) {
                *p_corrective_action = DCMN_INT_CORR_ACT_IPS_QSZ_CORRECT;
            }
            if (mem ==  SCH_FLOW_STATUS_MEMORY__FSMm) {
                *p_corrective_action = DCMN_INT_CORR_ACT_HARD_RESET;
            }
        }

    
     } else {
         sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
     }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_data_collection_for_ecc_1b(
    int unit, 
    int block_instance, 
    int en_arad_interrupt, 
    char* special_msg, 
    dcmn_int_corr_act_type* p_corrective_action,
    dcmn_interrupt_mem_err_info* ecc_1b_correct_info)
{
    int rc;
    uint64 counter;
    uint32 counter32;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name; 
    uint32 block;
    int is_2regs=0;
    

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(ecc_1b_correct_info);
    SOCDNX_NULL_CHECK(p_corrective_action);

    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;

    switch(en_arad_interrupt) {
    case(ARAD_INT_SCH_DHDECCERRORFIXED):
    case(ARAD_INT_SCH_DCDECCERRORFIXED):
    case(ARAD_INT_SCH_FLHHRECCERRORFIXED):
    case(ARAD_INT_SCH_FLTCLECCERRORFIXED):
    case(ARAD_INT_SCH_FLTHRECCERRORFIXED):
    case(ARAD_INT_SCH_FLHCLECCERRORFIXED):
    case(ARAD_INT_SCH_FLHFQECCERRORFIXED):
    case(ARAD_INT_SCH_FLTFQECCERRORFIXED):
    case(ARAD_INT_SCH_FDMDECCERRORFIXED):
    case(ARAD_INT_SCH_SHDDECCERRORFIXED):
    case(ARAD_INT_SCH_FSMECCERRORFIXED):
        rc = READ_SCH_ECC_1B_ERR_ADDRr(unit, &counter32);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg_field_get(unit, SCH_ECC_1B_ERR_ADDRr, counter32, ECC_1B_ERR_ADDR_VALIDf);
        /* get memory address bit */
        addrf = soc_reg_field_get(unit, SCH_ECC_1B_ERR_ADDRr, counter32, ECC_1B_ERR_ADDRf);

        rc = READ_SCH_ECC_1B_ERR_CNTr(unit, REG_PORT_ANY, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, SCH_ECC_1B_ERR_CNTr, counter, ECC_1B_ERR_CNT_OVERFLOWf);
         /* get counter value of sch error */
        cntf = soc_reg64_field32_get(unit, SCH_ECC_1B_ERR_CNTr, counter, ECC_1B_ERR_CNTf);

        block = ARAD_INTERRUPT_SCH_BLOCK;

        is_2regs = 1;

        break; 
    
    case(ARAD_INT_FMAC_ECC_1BERRINT):
        rc = READ_FMAC_ECC_1B_ERR_ADDRr(unit, block_instance, &counter32);
        SOCDNX_IF_ERR_EXIT(rc);

        /* get address validity bit */
        addr_validf = soc_reg_field_get(unit, FMAC_ECC_1B_ERR_ADDRr, counter32, ECC_1B_ERR_ADDR_VALIDf);
        /* get memory address bit */
        addrf = soc_reg_field_get(unit, FMAC_ECC_1B_ERR_ADDRr, counter32, ECC_1B_ERR_ADDRf);

        rc = READ_FMAC_ECC_1B_ERR_CNTr(unit, block_instance, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

         /* get counter overflow indication  */
        cnt_overflowf = soc_reg64_field32_get(unit, FMAC_ECC_1B_ERR_CNTr, counter, ECC_1B_ERR_CNT_OVERFLOWf);
         /* get counter value of sch error */
        cntf = soc_reg64_field32_get(unit, FMAC_ECC_1B_ERR_CNTr, counter, ECC_1B_ERR_CNTf);

        block = ARAD_INTERRUPT_FMAC_FIRST_BLOCK + block_instance;
        is_2regs = 1;
        break;  

    case(ARAD_INT_EGQ_ECC_1BERRINT):
        cnt_reg = EGQ_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_EGQ_BLOCK;
        break;

    case(ARAD_INT_EPNI_ECC_1BERRINT):
        cnt_reg = EPNI_ECC_1B_ERR_CNTr;   
        block = ARAD_INTERRUPT_EPNI_BLOCK;
        break;

    case(ARAD_INT_FDT_ECC_1BERRINT):
        cnt_reg = FDT_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_FDT_BLOCK;
        break;

    case(ARAD_INT_IDR_ECC_1BERRINT):
        cnt_reg = IDR_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IDR_BLOCK;
        break;

    case(ARAD_INT_IHB_ECC_1BERRINT):
        cnt_reg = IHB_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IHB_BLOCK;
        break;

    case(ARAD_INT_IHP_ECC_1BERRINT):
        cnt_reg = IHP_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IHP_BLOCK;
        break;

    case(ARAD_INT_IPS_ECC_1BERRINT):
        cnt_reg = IPS_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IPS_BLOCK;
        break;

    case(ARAD_INT_IPT_ECC_1BERRINT):
        cnt_reg = IPT_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IPT_BLOCK;
        break;

    case(ARAD_INT_IQM_ECC_1BERRINT):
        cnt_reg = IQM_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IQM_BLOCK;
        break;

    case(ARAD_INT_IRE_ECC_1BERRINT):
        cnt_reg = IRE_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IRE_BLOCK;
        break;

    case(ARAD_INT_IRR_ECC_1BERRINT):
        cnt_reg = IRR_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_IRR_BLOCK;
        break;

    case(ARAD_INT_MMU_ECC_1BERRINT):
        cnt_reg = MMU_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_MMU_BLOCK;
        break;

    case(ARAD_INT_NBI_ECC_1BERRINT):
        cnt_reg = NBI_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_NBI_BLOCK;
        break;

    case(ARAD_INT_OAMP_ECC_1BERRINT):
        cnt_reg = OAMP_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_OAMP_BLOCK;
        break;

    case(ARAD_INT_OLP_ECC_1BERRINT):
        cnt_reg = OLP_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_OLP_BLOCK;
        break;

    case(ARAD_INT_FCR_ECC_1BERRINT):
        cnt_reg = FCR_ECC_1B_ERR_CNTr;
        block = ARAD_INTERRUPT_FCR_BLOCK;
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));       
        break; 
    }

    if(!is_2regs) {
        rc = soc_reg_get(unit, cnt_reg, REG_PORT_ANY, 0, &counter);
        SOCDNX_IF_ERR_EXIT(rc);

         /* get address validity bit */
         addr_validf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_1B_ERR_ADDR_VALIDf);

         /* get memory address bit */
         addrf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_1B_ERR_ADDRf);

         /* get counter overflow indication  */
         cnt_overflowf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_1B_ERR_CNT_OVERFLOWf);

         /* get counter value of 2 bit error */
         cntf = soc_reg64_field32_get(unit, cnt_reg, counter, ECC_1B_ERR_CNTf);
    }

    if (addr_validf != 0) {
        
        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);

        switch (mem) {
        case INVALIDm:
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory not accessible",
                        cntf, cnt_overflowf, addrf);
        break;

        /*
         *  Handle IPS QDESC - write zero to unused entries
         */
        case IPS_CRBALm:
        case IPS_QDESC_TABLEm:
        case IPS_QDESCm:
            mem = IPS_QDESC_TABLEm;
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            memory_name = SOC_MEM_NAME(unit, mem);
            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            /* set corrective action */
            *p_corrective_action = DCMN_INT_CORR_ACT_IPS_QDESC;
            ecc_1b_correct_info->array_index = numels;
            ecc_1b_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
            ecc_1b_correct_info->min_index = index;
            ecc_1b_correct_info->max_index = index;
            ecc_1b_correct_info->mem = mem;
            break;

        default:    
            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);
                    
             memory_name = SOC_MEM_NAME(unit, mem);
             sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                                cntf, cnt_overflowf, addrf, memory_name, numels, index);

             /* set corrective action */
             ecc_1b_correct_info->array_index = numels;
             ecc_1b_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
             ecc_1b_correct_info->min_index = index;
             ecc_1b_correct_info->max_index = index;
             ecc_1b_correct_info->mem = mem;
             *p_corrective_action = DCMN_INT_CORR_ACT_ECC_1B_FIX;
        }
     } else {
         sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
     }

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_interrupt_data_collection_for_nbi_link_status_change(
    int unit, 
    int block_instance, 
    int en_arad_interrupt,
    char *special_msg,
    dcmn_int_corr_act_type *corrective_action,
    soc_port_t *port)
{
    uint32 phy_port;
    soc_port_t port_i;
    soc_pbmp_t ports_bm;
    int lane;
    SOCDNX_INIT_FUNC_DEFS;


    /*do nothing unless it is first physical_port*/
    *corrective_action = DCMN_INT_CORR_ACT_NONE;
    lane = en_arad_interrupt - ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_0;
    
    *port = -1;

    /*convert physical ports to port bitmap*/
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
    PBMP_ITER(ports_bm, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port));
        if (phy_port - 1 /*1 base to 0 base*/ == lane) {
            *port = port_i;
            *corrective_action = DCMN_INT_CORR_ACT_RX_LOS_HANDLE;
            break;  
        }
    }

    sal_sprintf(special_msg, "diagnose port %d", *port);

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_data_collection_for_ilkn_rx_status_change(
   int unit, 
   int block_instance, 
   int en_arad_interrupt, 
   char* special_msg, 
   dcmn_int_corr_act_type *corrective_action,
   soc_port_t *port)
{
    soc_port_if_t interface_type;
    soc_pbmp_t ports_bm;
    soc_port_t port_i;
    uint32 offset;

    SOCDNX_INIT_FUNC_DEFS;

	*corrective_action=DCMN_INT_CORR_ACT_NONE;
	*port=-1;

	SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
	PBMP_ITER(ports_bm,port_i)
	{
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i,&interface_type));
        if(interface_type != SOC_PORT_IF_ILKN)
		{
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port_i, 0, &offset));
        if (((offset == 0)  && (en_arad_interrupt == ARAD_INT_NBI_ILKNRXPORT0STATUSCHANGEINT)) ||
            ((offset == 1) && (en_arad_interrupt == ARAD_INT_NBI_ILKNRXPORT1STATUSCHANGEINT)))
        {
			*port=port_i;
			*corrective_action = DCMN_INT_CORR_ACT_RX_LOS_HANDLE;
            break;  
		}

	}
	if(*port == -1){
        sal_sprintf(special_msg, "port not found"); 
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }

	sal_sprintf(special_msg, "diagnose port %d", *port);

exit:
    SOCDNX_FUNC_RETURN;

}

int arad_interrupt_data_collection_for_recurring_drc_err(int unit, int block_instance, int en_arad_interrupt, char* special_msg, dcmn_int_corr_act_type* p_corrective_action)
{    
    int rc;  
    uint32 err_cnt, err_field_cnt;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);

   rc = soc_reg32_get(unit, MMU_INTERRUPT_REGISTERr, block_instance, 0, &err_cnt);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    err_field_cnt = soc_reg_field_get(unit, MMU_INTERRUPT_REGISTERr, err_cnt, DRAM_OPP_CRC_ERR_INTf);

    if(!err_field_cnt) {

        rc = soc_reg32_get(unit,  IPT_INTERRUPT_REGISTERr , block_instance, 0, &err_cnt);
        SOCDNX_IF_ERR_EXIT(rc);  

        err_field_cnt = soc_reg_field_get(unit, IPT_INTERRUPT_REGISTERr , err_cnt, CRC_ERR_PKTf);
    }

    if(err_field_cnt) {

        *p_corrective_action = DCMN_INT_CORR_ACT_CONFIG_DRAM;
         sal_sprintf(special_msg, "problem in PHY ");
    }else {
        sal_strcpy(special_msg, "");
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_data_collection_for_fctunrchdestevent(int unit, 
                                                                  int block_instance, 
                                                                  arad_interrupt_type interrupt_id, 
                                                                  uint32* fap_id,
                                                                  uint32* cell_type,
                                                                  dcmn_int_corr_act_type* corrective_action)
{
 
    int rc;
    uint32 unr_dst, unreach_dest;

    SOCDNX_INIT_FUNC_DEFS;

    *corrective_action = DCMN_INT_CORR_ACT_NONE;    

    /* 
     * Get the fap ID of the unreachable destination 
     */
    rc = soc_reg32_get(unit, FCT_UNREACHABLE_DESTINATIONr, block_instance, 0, &unr_dst);
    SOCDNX_IF_ERR_EXIT(rc);  
   
    unreach_dest = soc_reg_field_get(unit, FCT_UNREACHABLE_DESTINATIONr, unr_dst, UNRCH_DESTf);

    /* Fap id is on bits 0-10 */
    *fap_id = 0x0;
    SHR_BITCOPY_RANGE(fap_id , 0, &unreach_dest, 0, 11); 
    /* Get the sale type */ 
    *cell_type = 0x0;
    SHR_BITCOPY_RANGE(cell_type , 0, &unreach_dest, 11, 3);

    /* 
     * Validate fap_id 
     */
    if( SOC_IS_ARAD_B1_AND_BELOW(unit)) {
    
        if ( soc_reg_field_get(unit, FCT_UNREACHABLE_DESTINATIONr, unr_dst, UNRCH_DEST_EVTf) && *cell_type ==  ARAD_INTERRUPTS_FCT_UNREACHABLE_DEST_CELL_TYPE_CREDIT) {
            *corrective_action = DCMN_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    uint8* array_links,
    char* msg)
{
    int rc ;
    int link_number, port;
        
    SOCDNX_INIT_FUNC_DEFS;

    PBMP_SFI_ITER(unit, port){
        link_number = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        if(ARAD_INT_LINK_SHUTDOWN == array_links[link_number]) {
            rc = bcm_port_enable_set(unit, link_number + SOC_TMC_NOF_FAP_PORTS_ARAD, ARAD_INT_LINK_SHUTDOWN);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}

int 
arad_interrupt_handles_corrective_action_ips_qdesc(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{
    int rc;
    uint32 qdesc_data_entry[2]= {0,0}; /* size of IPS_QDESC_TABLE is 5 bytes - 2 DW needed */ 
    int current_index;
    uint32 soc_sand_rc;
    ARAD_IPQ_QUARTET_MAP_INFO queue2flow;
    uint32 old_flag;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(shadow_correct_info_p);
 
    for (current_index = shadow_correct_info_p->min_index ; current_index <= shadow_correct_info_p->max_index ; current_index++) {
        /* test if entry unused */
        soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_queue_to_flow_mapping_get,(unit, block_instance, SOC_TMC_IPQ_Q_TO_QRTT_ID(current_index), &queue2flow));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            rc = translate_sand_success_failure(soc_sand_rc);
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                                  "unit %d, error in retreiving queue (%d) flow mapping, soc_sand error 0x%x, error 0x%x\n"), unit, current_index, soc_sand_rc, rc));
            SOCDNX_IF_ERR_EXIT(rc);
            BCM_EXIT;
        }
        if (queue2flow.flow_quartet_index != ARAD_IPQ_INVALID_FLOW_QUARTET) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(READ_IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY,&old_flag));
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
        rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index, shadow_correct_info_p->copyno, current_index, qdesc_data_entry);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, old_flag));
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* in order for the arad+ to stop traffic when nof_links < minimum configured */
/* when a link changes its status from up to down, RTP tables needs to be updated */

int arad_interrupt_handles_corrective_action_rtp_link_mask_change( 
   int unit){  
        
    uint64 rtp_link_active_mask_val;
    uint32 mask_lo=0,mask_hi=0;
    soc_reg_above_64_val_t rtp_mem_val, rtp_mem_mask;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_ACTIVE_MASKr_REG64(unit,&rtp_link_active_mask_val));
    COMPILER_64_NOT(rtp_link_active_mask_val);
    
    SOC_REG_ABOVE_64_CLEAR(rtp_mem_mask);
    COMPILER_64_TO_32_LO(mask_lo, rtp_link_active_mask_val);
    COMPILER_64_TO_32_HI(mask_hi, rtp_link_active_mask_val);

    if (SOC_IS_ARADPLUS(unit))
    {
        SHR_BITCOPY_RANGE(rtp_mem_mask,0,&mask_lo,0,BYTES2BITS(sizeof(uint32))); 
        SHR_BITCOPY_RANGE(rtp_mem_mask,BYTES2BITS(sizeof(uint32)),&mask_hi,0,SOC_DPP_DEFS_GET(unit, nof_fabric_links)-BYTES2BITS(sizeof(uint32)));
        SHR_BITCOPY_RANGE(rtp_mem_mask,SOC_DPP_DEFS_GET(unit, nof_fabric_links),&mask_lo,0,BYTES2BITS(sizeof(uint32))); 
        SHR_BITCOPY_RANGE(rtp_mem_mask,BYTES2BITS(sizeof(uint32))+SOC_DPP_DEFS_GET(unit, nof_fabric_links),&mask_hi,0,SOC_DPP_DEFS_GET(unit, nof_fabric_links)-BYTES2BITS(sizeof(uint32)));

        for (i=0; i<soc_mem_index_count(unit,RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm);i++)
        {
            
            SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm(unit,MEM_BLOCK_ANY,i,rtp_mem_val)); 
            SOC_REG_ABOVE_64_AND(rtp_mem_val, rtp_mem_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm(unit,MEM_BLOCK_ANY,i,rtp_mem_val));
        }
        for (i=0; i<soc_mem_index_count(unit,RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm);i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm(unit,MEM_BLOCK_ANY,i,rtp_mem_val)); 
            SOC_REG_ABOVE_64_AND(rtp_mem_val, rtp_mem_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm(unit,MEM_BLOCK_ANY,i,rtp_mem_val));

        }
        for (i=0; i<soc_mem_index_count(unit,RTP_UNICAST_DISTRIBUTION_MEMORYm);i++)
        {
            SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_DISTRIBUTION_MEMORYm(unit,MEM_BLOCK_ANY,i,rtp_mem_val)); 
            SOC_REG_ABOVE_64_AND(rtp_mem_val, rtp_mem_mask);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_DISTRIBUTION_MEMORYm(unit,MEM_BLOCK_ANY,i,rtp_mem_val));
        }
    }


exit:
    SOCDNX_FUNC_RETURN;


}

int arad_interrupt_handles_corrective_action_reprogram_resource(
    int unit, 
    int block_instance, 
    int interrupt_id,
    void *param1, 
    char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: reprogram resource should be made!\n")));
#endif

    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_handles_corrective_action_config_dram(
    int unit, 
    int block_instance, 
    int interrupt_id,
    char *msg)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef DEBUG_COMMENTS_APLL_INTERRUPT_HANDLE
    LOG_INFO(BSL_LS_BCM_INTR,
             (BSL_META_U(unit,
                         "Decision: reprogram resource should be made!\n")));
#endif

    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_fsrd_link_get(int unit, 
                                int block_instance, 
                                int bit_in_field, 
                                int reg_index, 
                                int *p_link)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_link);

    *p_link = block_instance * ARAD_INT_BIT_PER_FSRD_QUAD_INT_TYPE * ARAD_INT_REGS_PER_FSRD_QUAD_INT_TYPE + reg_index * ARAD_INT_BIT_PER_FSRD_QUAD_INT_TYPE  + bit_in_field ;
    if (!(*p_link >= 0 && *p_link < SOC_DPP_DEFS_GET(unit, nof_fabric_links))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed in arad_interrupt_fsrd_link_get")));
    }

exit:
    SOCDNX_FUNC_RETURN;  
}

int arad_interrupt_handles_corrective_action_em_ser(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    uint32* address,
    char* msg)
{
    int rc;
    ARAD_PP_EM_SER_BLOCK block;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(address);

    switch (interrupt_id) {
        case ARAD_INT_IHP_PARITYERRINT:
            block = ARAD_PP_EM_SER_BLOCK_IHP;
            break;
 
        case ARAD_INT_IHB_PARITYERRINT:
            block = ARAD_PP_EM_SER_BLOCK_IHB;
            break;
 
        case ARAD_INT_EPNI_PARITYERRINT:
            block = ARAD_PP_EM_SER_BLOCK_EPNI;
            break;
 
        case ARAD_INT_OAMP_PARITYERRINT:
            block = ARAD_PP_EM_SER_BLOCK_OAMP;
            break;
 
        default:
            block = ARAD_PP_EM_SER_NOF_BLOCKS;
            break;
    }
 
    if (SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        rc = arad_pp_em_ser(unit,address, block);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Corrective Action main function */
int
arad_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    arad_interrupt_type interrupt_id,
    char *msg,
    dcmn_int_corr_act_type corr_act,
    void *param1, 
    void *param2)
{
    int rc;
    uint32 flags;
    soc_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    BCMDNX_IF_ERR_EXIT(rc);
    
    if(SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = DCMN_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case DCMN_INT_CORR_ACT_NONE:
        rc = dcmn_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_FORCE:
        rc = arad_interrupt_handles_corrective_action_force(unit, block_instance, interrupt_id, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_SOFT_RESET:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_SOFT_RESET_BLOCKS_FABRIC);
        } else {
            rc = dcmn_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case DCMN_INT_CORR_ACT_HARD_RESET:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_HARD_RESET);
        } else {
            rc = dcmn_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case DCMN_INT_CORR_ACT_INGRESS_HARD_RESET:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_SOFT_RESET_BLOCKS_FABRIC);
        } else {
            rc = arad_interrupt_handles_corrective_action_ingress_hard_reset(unit, block_instance, interrupt_id, msg);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
        
    case DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = arad_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id,(uint8*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_SHADOW:
        rc = dcmn_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
        rc = dcmn_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_SOFT_RESET_BLOCKS_FABRIC);
        } else {
            rc = dcmn_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    case DCMN_INT_CORR_ACT_PRINT:
        rc = dcmn_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_REPROGRAM_RESOURCE:
        rc = arad_interrupt_handles_corrective_action_reprogram_resource(unit, block_instance, interrupt_id,(void*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_CONFIG_DRAM:
        rc = arad_interrupt_handles_corrective_action_reprogram_resource(unit, block_instance, interrupt_id,(void*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_ECC_1B_FIX:
        rc = dcmn_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id, (dcmn_interrupt_mem_err_info * )param1, msg); 
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO:
        rc = arad_interrupt_handles_corrective_action_handle_crc_del_buf_fifo(unit, block_instance, interrupt_id, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
        rc = arad_interrupt_handles_corrective_action_handle_oamp_event_fifo(unit, block_instance, interrupt_id, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_CLEAR_CHECK:
        rc = arad_interrupt_handles_corrective_action_clear_check(unit, block_instance, interrupt_id,(int*)param1, (int*)param2);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_RX_LOS_HANDLE:
        /* get link number */
        SOCDNX_NULL_CHECK(param1);
        port = *((soc_port_t *)param1);
        
        if (rx_loss_appl_enable(unit)) {
            rc = rx_los_handle(unit, port);
            SOCDNX_IF_ERR_EXIT(rc);
        }
        break;
    case DCMN_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION:
        rc = arad_interrupt_handles_corrective_action_shutdown_unreach_destination(unit, block_instance, interrupt_id, *((uint32*)param1));
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO:
        rc = arad_interrupt_handles_corrective_action_handle_mact_event_fifo(unit, block_instance, interrupt_id, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break; 

    case DCMN_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_SOFT_RESET_BLOCKS);
        } else {
            rc = arad_interrupt_handles_corrective_action_hard_reset_without_fabric(unit, block_instance, interrupt_id, msg);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    case DCMN_INT_CORR_ACT_RTP_LINK_MASK_CHANGE:
        rc=arad_interrupt_handles_corrective_action_rtp_link_mask_change(unit);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB:
        rc=dcmn_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(unit, block_instance, interrupt_id,(ARAD_TCAM_LOCATION*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_IPS_QDESC:
        rc = arad_interrupt_handles_corrective_action_ips_qdesc(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY:
    case DCMN_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY:
    case DCMN_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY:
    case DCMN_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY:
        rc=arad_interrupt_handles_corrective_action_em_ser(unit, block_instance, interrupt_id,(uint32*)param1, msg);
        BCMDNX_IF_ERR_EXIT(rc);
        break;		
    case DCMN_INT_CORR_ACT_IPS_QSZ_CORRECT:
        rc = dcmn_interrupt_handles_corrective_action_for_ips_qsz(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_BCM_MSG("UnKnown corrective action")));
  }
   
exit:
  SOCDNX_FUNC_RETURN;
}

int 
arad_interrupt_data_collection_for_fsrd_syncstatuschanged( 
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    soc_port_t *link,
    char* special_msg)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    
    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    /* get link number */
    rc = arad_interrupt_fsrd_link_get(unit, block_instance, 
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].bit_in_field, 
                                     SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_arad_interrupt].reg_index, link);
    SOCDNX_IF_ERR_EXIT(rc);
        
    sal_sprintf(special_msg, "link=%d, diagnose SerDes", *link);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_data_collection_for_port_mib(
    int unit, 
    int block_instance, 
    arad_interrupt_type en_arad_interrupt, 
    char* special_msg,
    dcmn_int_corr_act_type* p_corrective_action)
{
    soc_reg_t status_reg;
    uint32 multiple_errf, double_bit_errf, status_reg_data;
    soc_reg_above_64_val_t reset = {-1};
    int rc;

    SOCDNX_INIT_FUNC_DEFS;
    
    /* read port status according to interrupt*/
    switch(en_arad_interrupt) {

        case ARAD_INT_PORT_MIB_TSC0_MEM_ERR:
            status_reg = PORT_MIB_TSC_0_ECC_STATUSr;
            break;

        case ARAD_INT_PORT_MIB_TSC1_MEM_ERR:
            status_reg = PORT_MIB_TSC_1_ECC_STATUSr;
            break;

        case ARAD_INT_PORT_MIB_RSC0_MEM_ERR:
            status_reg = PORT_MIB_RSC_0_ECC_STATUSr;
            break;

        case ARAD_INT_PORT_MIB_RSC1_MEM_ERR:
            status_reg = PORT_MIB_RSC_1_ECC_STATUSr;
            break;
    
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail interrupt for collection data for port_mib")));
    }
     
    rc = soc_reg32_get(unit, status_reg, block_instance, 0, &status_reg_data);
    SOCDNX_IF_ERR_EXIT(rc); 
    
    /* cheack if the error is multiple or double bit */
    multiple_errf = soc_reg_field_get(unit, status_reg, status_reg_data, MULTIPLE_ERRf);  
    double_bit_errf = soc_reg_field_get(unit, status_reg, status_reg_data, DOUBLE_BIT_ERRf);
    
    sal_sprintf(special_msg, "Port MIB ECC status: multiple_err=%d, double_bit_err=%d", multiple_errf, double_bit_errf);  
    
    /* if error multiple corr act is hard_reset */
    if(multiple_errf || double_bit_errf) {
        *p_corrective_action = DCMN_INT_CORR_ACT_HARD_RESET;

         rc = soc_reg_above_64_set(unit, PORT_MIB_RESETr, block_instance, 0, reset); 
         SOCDNX_IF_ERR_EXIT(rc);                    
         reset[0] = 0;
         rc = soc_reg_above_64_set(unit, PORT_MIB_RESETr, block_instance, 0, reset); 
         SOCDNX_IF_ERR_EXIT(rc);                                         
         
    } else {
        *p_corrective_action = DCMN_INT_CORR_ACT_NONE;
    }
 
    exit:
    SOCDNX_FUNC_RETURN;
}

int arad_interrupt_fmac_link_get(
    int unit,
    int fmac_block_instance,
    int bit_in_field,
    int *p_link    )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_link);

    *p_link = fmac_block_instance * ARAD_INT_BIT_PER_MAC_INT_TYPE  + bit_in_field ;
    if (!(*p_link >= 0 && *p_link < SOC_DPP_DEFS_GET(unit, nof_fabric_links))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed in arad_interrupt_fmac_link_get")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
*  common utilities for the same interrupts
*/

int arad_interrupt_data_collection_for_recuring_action_fdr_alto(
    int unit,
    int block_instance,
    arad_interrupt_type en_arad_interrupt,
    dcmn_int_corr_act_type * p_corrective_action,
    uint8*  array_links,
    char* special_msg)
{

    int rc  ;
    uint32 data;
    uint32 first_link=0;
    int link_number, str_pos;
    int link_is_up;
    char temp_buff[10];
    char shutdown_links_num[200];
 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(array_links);
    SOCDNX_NULL_CHECK(special_msg);

    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;

    switch (en_arad_interrupt) {
        case ARAD_INT_FDR_PRMALTOA:
            /*read Alto BmpP*/
            rc = READ_FDR_LINK_UP_STATUS_FROM_FDRA_PRIMARYr(unit, &data);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case ARAD_INT_FDR_PRMALTOB:
            rc = READ_FDR_LINK_UP_STATUS_FROM_FDRB_PRIMARYr(unit, &data);
            SOCDNX_IF_ERR_EXIT(rc);
            first_link = 18;
            break;
        case ARAD_INT_FDR_SECALTOA:
            /*read Alto BmpP*/
            rc = READ_FDR_LINK_UP_STATUS_FROM_FDRA_SECONDARYr(unit, &data);
            SOCDNX_IF_ERR_EXIT(rc);
            break;
        case ARAD_INT_FDR_SECALTOB:
            rc = READ_FDR_LINK_UP_STATUS_FROM_FDRB_SECONDARYr(unit, &data);
            SOCDNX_IF_ERR_EXIT(rc);
            first_link = 18;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt ")));
    }    
  
    /* get problem links */
    str_pos = 0;
    for(link_number = 0; link_number < ARAD_INT_FDR_NUM_OF_LINKS_IN_LINK_UP_STATUS_REG; link_number++) {
        link_is_up = SHR_BITGET(&data, link_number);
        if(!link_is_up) {
            *p_corrective_action = DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS ;
            /* add link to shutdown table*/
            array_links[first_link + link_number] = ARAD_INT_LINK_SHUTDOWN;
            /* add corrupted link to message */
            sal_sprintf(temp_buff, "%d ", link_number);
            sal_strcpy((&shutdown_links_num[str_pos]),temp_buff);
            str_pos+=sal_strlen(temp_buff);
            sal_strcpy((&shutdown_links_num[str_pos])," ");
            str_pos++;
        }/*if link status changed*/
    } /*end for*/

    sal_sprintf(special_msg, "corrupted links: %s",shutdown_links_num);

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

