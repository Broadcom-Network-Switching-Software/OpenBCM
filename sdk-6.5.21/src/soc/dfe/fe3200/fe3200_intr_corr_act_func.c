/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement Correction action functions for fe3200 interrupts.
 */


#include <shared/bsl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_defs.h>

#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>
#include <soc/sand/sand_ser_correction.h>
#include <soc/sand/sand_intr_corr_act_func.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR

#define FE3200_INTERRUPTS_SCH_FLOW_TO_FIP_MAPPING_FFM_SIZE 16384
#define FE3200_INTERRUPTS_NOF_FLOWS_PER_FFM_ENTRY 8



int
fe3200_interrupt_data_collection_for_shadowing(
    int unit,
    int block_instance,
    fe3200_interrupt_type en_interrupt,
    char* special_msg,
    dcmn_int_corr_act_type* p_corrective_action,
    dcmn_interrupt_mem_err_info* shadow_correct_info)
{
    int rc;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char* memory_name;
    uint32 block;
    sand_memory_dc_t type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(special_msg);
    SOCDNX_NULL_CHECK(p_corrective_action);
    SOCDNX_NULL_CHECK(shadow_correct_info);
    *p_corrective_action = DCMN_INT_CORR_ACT_NONE;

    switch(en_interrupt) {

    case FE3200_INT_DCL_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCL_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_CCS_ECC_ECC_2B_ERR_INT:

                cnt_reg = CCS_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_RTP_ECC_ECC_2B_ERR_INT:

                cnt_reg = RTP_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCH_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCH_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_ECI_ECC_ECC_2B_ERR_INT:

                cnt_reg = ECI_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCM_ECC_ECC_2B_ERR_INT:

                cnt_reg = DCM_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_FSRD_ECC_ECC_2B_ERR_INT:

                cnt_reg = FSRD_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_FMAC_ECC_ECC_2B_ERR_INT:

                cnt_reg = FMAC_ECC_2B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC2B_DC;
                break;
    case FE3200_INT_DCL_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCL_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_CCS_ECC_ECC_1B_ERR_INT:

                cnt_reg = CCS_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_RTP_ECC_ECC_1B_ERR_INT:

                cnt_reg = RTP_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_DCH_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCH_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_ECI_ECC_ECC_1B_ERR_INT:

                cnt_reg = ECI_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_FMAC_ECC_ECC_1B_ERR_INT:

                cnt_reg = FMAC_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_FSRD_ECC_ECC_1B_ERR_INT:

                cnt_reg = FSRD_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_DCM_ECC_ECC_1B_ERR_INT:

                cnt_reg = DCM_ECC_1B_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_ECC1B_DC;
                break;
    case FE3200_INT_CCS_CRP_PARITY_ERR_INT:

                cnt_reg = CCS_CRP_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_PARITY_DC;
                break;
    case FE3200_INT_FMAC_ECC_PARITY_ERR_INT:

                cnt_reg = FMAC_PARITY_ERR_CNTr;
                SOCDNX_IF_ERR_EXIT(soc_get_reg_first_block_id(unit,cnt_reg,&block));
                type =  SAND_ECC_PARITY_DC;
                break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Unavail action for interrupt %d\n"),en_interrupt));
      }



    rc = sand_get_cnt_reg_values(unit, type, cnt_reg,block_instance,&cntf, &cnt_overflowf, &addrf,&addr_validf);
    SOCDNX_IF_ERR_EXIT(rc);




    if (addr_validf != 0) {

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
        default:

            rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
            SOCDNX_IF_ERR_EXIT(rc);

            sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                        cntf, cnt_overflowf, addrf, memory_name, numels, index);
            
            if (mem == RTP_SLSCTm) {
                *p_corrective_action = DCMN_INT_CORR_ACT_RTP_SLSCT;
            } else {
                rc = dcmn_mem_decide_corrective_action(unit, type, mem, block_instance,p_corrective_action, special_msg);
            }

            SOCDNX_IF_ERR_EXIT(rc);
            shadow_correct_info->array_index = numels;
            shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
            shadow_correct_info->min_index = index;
            shadow_correct_info->max_index = index;
            shadow_correct_info->mem = mem;
        }

     } else {
         sal_sprintf(special_msg, "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
     }

exit:
    SOCDNX_FUNC_RETURN;
}

int
fe3200_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char* msg)
{
    uint32 port = block_instance*4 + SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].bit_in_field;
    uint32 rc;
    SOCDNX_INIT_FUNC_DEFS;


    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_enable_set, (unit, port, 0));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}


int
fe3200_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    fe3200_interrupt_type interrupt_id,
    char *msg,
    dcmn_int_corr_act_type corr_act,
    void *param1,
    void *param2)
{
    int rc;
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device")));
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);

    
    if(((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE)) {
        corr_act = DCMN_INT_CORR_ACT_NONE;
    }

    switch(corr_act) {
    case DCMN_INT_CORR_ACT_NONE:
        rc = dcmn_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;

    case DCMN_INT_CORR_ACT_SOFT_RESET:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_SOFT_RESET_BLOCKS_FABRIC);
        } else {
            rc = dcmn_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            SOCDNX_IF_ERR_EXIT(rc);
        }
        break;
    case DCMN_INT_CORR_ACT_HARD_RESET:
        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupt_id, block_instance, ASIC_HARD_RESET);
        } else {
            rc = dcmn_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
            SOCDNX_IF_ERR_EXIT(rc);
        }
        break;
    case DCMN_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
        rc = fe3200_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_PRINT:
        rc = dcmn_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id,(char*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_SHADOW:
        rc = dcmn_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    case DCMN_INT_CORR_ACT_ECC_1B_FIX:
        rc = dcmn_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
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
    case DCMN_INT_CORR_ACT_RTP_SLSCT:
        rc = dcmn_interrupt_handles_corrective_action_for_rtp_slsct(unit, block_instance, interrupt_id,(dcmn_interrupt_mem_err_info*)param1, msg);
        SOCDNX_IF_ERR_EXIT(rc);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("UnKnown corrective action")));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

