/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */




#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_cmic.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_dram.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_regs.h>
#include <shared/swstate/sw_state.h>


#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#define SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD (10)


typedef struct soc_jer_reset_status_polling_s {
    soc_reg_t   reg;
    soc_field_t field;
    uint32      expected_val;
} soc_jer_reset_status_polling_t;

soc_field_t jer_ingress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_21f,  BLOCKS_SOFT_INIT_19f,  BLOCKS_SOFT_INIT_20f
    };
soc_field_t jer_egress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_52f,  BLOCKS_SOFT_INIT_46f,  
        BLOCKS_SOFT_INIT_47f, BLOCKS_SOFT_INIT_48f,  
        BLOCKS_SOFT_INIT_49f, 
        INVALIDf, 
        BLOCKS_SOFT_INIT_104f,  
        BLOCKS_SOFT_INIT_105f,  BLOCKS_SOFT_INIT_106f
    };
soc_field_t jer_plus_ingress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_21f,  BLOCKS_SOFT_INIT_19f,  BLOCKS_SOFT_INIT_20f
    };
soc_field_t jer_plus_egress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_56f,  
        BLOCKS_SOFT_INIT_50f,  BLOCKS_SOFT_INIT_51f,  
        BLOCKS_SOFT_INIT_52f,  BLOCKS_SOFT_INIT_53f,  
        INVALIDf, 
        BLOCKS_SOFT_INIT_111f,  BLOCKS_SOFT_INIT_112f,  BLOCKS_SOFT_INIT_113f
    };
soc_field_t qax_egress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_52f,
        BLOCKS_SOFT_INIT_46f,
        BLOCKS_SOFT_INIT_48f,
        INVALIDf, 
        BLOCKS_SOFT_INIT_97f,
        BLOCKS_SOFT_INIT_98f,
        BLOCKS_SOFT_INIT_99f,
        BLOCKS_SOFT_INIT_114f,
        BLOCKS_SOFT_INIT_115f,
        BLOCKS_SOFT_INIT_116f
    };
soc_field_t qux_egress_order_dependent_init_field_list[] = {
        
        BLOCKS_SOFT_INIT_49f,
        BLOCKS_SOFT_INIT_46f,
        BLOCKS_SOFT_INIT_48f,
        INVALIDf, 
        BLOCKS_SOFT_INIT_31f
    };

soc_field_t jer_ingress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_21f,  BLOCKS_SOFT_RESET_19f,  BLOCKS_SOFT_RESET_20f
    };
soc_field_t jer_egress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_52f,  
        BLOCKS_SOFT_RESET_46f,  BLOCKS_SOFT_RESET_47f,  
        BLOCKS_SOFT_RESET_48f,  BLOCKS_SOFT_RESET_49f,  
        BLOCKS_SOFT_RESET_104f,  BLOCKS_SOFT_RESET_105f,  BLOCKS_SOFT_RESET_106f
    };
soc_field_t jer_plus_ingress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_21f,  BLOCKS_SOFT_RESET_19f,  BLOCKS_SOFT_RESET_20f
    };
soc_field_t jer_plus_egress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_56f,  
        BLOCKS_SOFT_RESET_50f,  BLOCKS_SOFT_RESET_51f,  
        BLOCKS_SOFT_RESET_52f,  BLOCKS_SOFT_RESET_53f,  
        BLOCKS_SOFT_RESET_111f,  BLOCKS_SOFT_RESET_112f,  BLOCKS_SOFT_RESET_113f
    };
soc_field_t qax_egress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_52f,
        BLOCKS_SOFT_RESET_46f,
        BLOCKS_SOFT_RESET_48f,
        BLOCKS_SOFT_RESET_97f,
        BLOCKS_SOFT_RESET_98f,
        BLOCKS_SOFT_RESET_99f,
        BLOCKS_SOFT_RESET_114f,
        BLOCKS_SOFT_RESET_115f,
        BLOCKS_SOFT_RESET_116f
    };
soc_field_t qux_egress_order_dependent_reset_field_list[] = {
        
        BLOCKS_SOFT_RESET_49f,
        BLOCKS_SOFT_RESET_46f,
        BLOCKS_SOFT_RESET_48f,
        BLOCKS_SOFT_RESET_31f
    };


int soc_jer_reset_blocks_poll_init_finish(int unit)
{
    soc_jer_reset_status_polling_t jer_polling_common_list[] = {
            { IRE_RESET_STATUS_REGISTERr, CTXT_STATUS_INIT_DONEf, 1 },
            { IRE_RESET_STATUS_REGISTERr, CTXT_MAP_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, CONTEXT_STATUS_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, CHUNK_STATUS_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, WORD_INDEX_FIFO_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, FREE_PCB_FIFO_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, FPF_0_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, FPF_1_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, IS_FPF_0_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, IS_FPF_1_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, DESTINATION_TABLE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, LAG_MAPPING_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, LAG_TO_LAG_RANGE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, MCDB_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, STACK_FEC_RESOLVE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, STACK_TRUNK_RESOLVE_INIT_DONEf, 1 },
            { PPDB_A_OEMA_RESET_STATUS_REGISTERr, OEMA_KEYT_RESET_DONEf, 1 },
            { PPDB_A_OEMB_RESET_STATUS_REGISTERr, OEMB_KEYT_RESET_DONEf, 1 },
            { PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, LARGE_EM_KEYT_RESET_DONEf, 1 },
            { EDB_GLEM_RESET_STATUS_REGISTERr, GLEM_KEYT_RESET_DONEf, 1 },
            { EDB_ESEM_RESET_STATUS_REGISTERr, ESEM_KEYT_RESET_DONEf, 1 },
            { OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REMOTE_MEP_EXACT_MATCH_KEYT_RESET_DONEf, 1 }
        };
    soc_jer_reset_status_polling_t jer_polling_per_core_list[] = {
            { IQM_IQM_INITr, IQC_INITf, 0 },
            { IQM_IQM_INITr, STE_INITf, 0 },
            { IQM_IQM_INITr, PDM_INITf, 0 },
            { IHB_ISEM_RESET_STATUS_REGISTERr, ISEM_KEYT_RESET_DONEf, 1 },
            { IPS_IPS_GENERAL_CONFIGURATIONSr, IPS_INIT_TRIGGERf, 0 },
            { EGQ_EGQ_BLOCK_INIT_STATUSr, EGQ_BLOCK_INITf, 0 },
        };
    soc_jer_reset_status_polling_t jer_plus_polling_common_list[] = {
            { IRE_RESET_STATUS_REGISTERr, CTXT_STATUS_INIT_DONEf, 1 },
            { IRE_RESET_STATUS_REGISTERr, CTXT_MAP_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, CONTEXT_STATUS_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, CHUNK_STATUS_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, WORD_INDEX_FIFO_INIT_DONEf, 1 },
            { IDR_RESET_STATUS_REGISTERr, FREE_PCB_FIFO_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, FPF_0_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, FPF_1_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, IS_FPF_0_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, IS_FPF_1_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, DESTINATION_TABLE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, LAG_MAPPING_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, LAG_TO_LAG_RANGE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, MCDB_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, STACK_FEC_RESOLVE_INIT_DONEf, 1 },
            { IRR_RESET_STATUS_REGISTERr, STACK_TRUNK_RESOLVE_INIT_DONEf, 1 },
            { PPDB_A_OEMA_RESET_STATUS_REGISTERr, OEMA_KEYT_RESET_DONEf, 1 },
            { PPDB_A_OEMB_RESET_STATUS_REGISTERr, OEMB_KEYT_RESET_DONEf, 1 },
            { PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, LARGE_EM_KEYT_RESET_DONEf, 1 },
            { EDB_GLEM_RESET_STATUS_REGISTERr, GLEM_KEYT_RESET_DONEf, 1 },
            { EDB_ESEM_0_RESET_STATUS_REGISTERr, ESEM_0_KEYT_RESET_DONEf, 1 },
            { EDB_ESEM_1_RESET_STATUS_REGISTERr, ESEM_1_KEYT_RESET_DONEf, 1 },
            { OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REMOTE_MEP_EXACT_MATCH_KEYT_RESET_DONEf, 1 }
        };
    soc_jer_reset_status_polling_t qax_polling_common_list[] = {
            { IRE_RESET_STATUS_REGISTERr, CTXT_STATUS_INIT_DONEf, 1 },
            { SPB_RESET_STATUS_REGISTERr, PEC_INIT_STATUSf, 0 },
            { SPB_RESET_STATUS_REGISTERr, FBC_RCN_STATUS_INITf, 0 },
            { SPB_RESET_STATUS_REGISTERr, FBC_PTR_STATUS_INITf, 0 },
            { SPB_RESET_STATUS_REGISTERr, DBG_COUNTER_STATUS_INITf, 0 },
            { SPB_RESET_STATUS_REGISTERr, BUFF_LINK_STATUS_INITf, 0 },
            { IPS_IPS_GENERAL_CONFIGURATIONSr, IPS_INIT_TRIGGERf, 0 },
            { EGQ_EGQ_BLOCK_INIT_STATUSr, EGQ_BLOCK_INITf, 0 },
            { PPDB_A_ISEM_RESET_STATUS_REGISTERr, ISEM_KEYT_RESET_DONEf, 1 },
            { PPDB_A_OEMA_RESET_STATUS_REGISTERr, OEMA_KEYT_RESET_DONEf, 1 },
            { PPDB_A_OEMB_RESET_STATUS_REGISTERr, OEMB_KEYT_RESET_DONEf, 1 },
            { PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, LARGE_EM_KEYT_RESET_DONEf, 1 },
            { PPDB_B_ISEM_RESET_STATUS_REGISTERr, ISEM_KEYT_RESET_DONEf, 1 },
            { EDB_GLEM_RESET_STATUS_REGISTERr, GLEM_KEYT_RESET_DONEf, 1 },
            { EDB_ESEM_RESET_STATUS_REGISTERr, ESEM_KEYT_RESET_DONEf, 1 },
            { OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REMOTE_MEP_EXACT_MATCH_KEYT_RESET_DONEf, 1 }
        };
    soc_jer_reset_status_polling_t *polling_common_list = NULL;
    int polling_common_list_size = 0;
    soc_jer_reset_status_polling_t *polling_per_core_list = NULL;
    int polling_per_core_list_size = 0;
    int core;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_FLAIR(unit)) {
        
    }
    else if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        polling_common_list = jer_plus_polling_common_list;
        polling_common_list_size = sizeof(jer_plus_polling_common_list)/sizeof(jer_plus_polling_common_list[0]);
        
        polling_per_core_list = jer_polling_per_core_list;
        polling_per_core_list_size = sizeof(jer_polling_per_core_list)/sizeof(jer_polling_per_core_list[0]);
    }
    else if (SOC_IS_QAX(unit)) {
        
        polling_common_list = qax_polling_common_list;
        polling_common_list_size = sizeof(qax_polling_common_list)/sizeof(qax_polling_common_list[0]);
        polling_per_core_list = NULL;
        polling_per_core_list_size = 0;
    }
    else if (SOC_IS_JERICHO(unit)) {
        polling_common_list = jer_polling_common_list;
        polling_common_list_size = sizeof(jer_polling_common_list)/sizeof(jer_polling_common_list[0]);
        polling_per_core_list = jer_polling_per_core_list;
        polling_per_core_list_size = sizeof(jer_polling_per_core_list)/sizeof(jer_polling_per_core_list[0]);
    }
    
    for(i = 0; i < polling_common_list_size; i++) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, 
            polling_common_list[i].reg, REG_PORT_ANY, 0, 
            polling_common_list[i].field, polling_common_list[i].expected_val));
    }

    for (core=0; core<SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; core++) {
        for(i = 0; i < polling_per_core_list_size; i++) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, 
                polling_per_core_list[i].reg, core, 0, 
                polling_per_core_list[i].field, polling_per_core_list[i].expected_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;  
}




STATIC int jer_sch_reset(int unit)
{
    uint32
        mc_conf_0_fld_val,
        mc_conf_1_fld_val,
        ingr_shp_en_fld_val, 
        timeout_val,
        backup_msg_en;
    uint32
        tbl_data[ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE] = {0};
    int sch_nof_instances = 0;
    int i;
    int block_type;

    SOCDNX_INIT_FUNC_DEFS;

    block_type = SOC_BLK_SCH;
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nof_block_instance, (unit, &block_type, &sch_nof_instances)));
    for (i = 0; i < sch_nof_instances; i++) {
        

        


        SOCDNX_IF_ERR_EXIT(READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, i, &mc_conf_0_fld_val));
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, i, &mc_conf_1_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, i, 0, MULTICAST_GFMC_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r, i, 0, MULTICAST_BFMC_1_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, i, 0, MULTICAST_BFMC_2_ENABLEf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r, i, 0, MULTICAST_BFMC_3_ENABLEf,  0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf, &ingr_shp_en_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf,  0x0));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf, &backup_msg_en));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf,  0x0));

          
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_ECC_INTERRUPT_REGISTERr(unit, i, 0xffffffff));

        soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, tbl_data, SCH_INITf, 0x0);

        
        SOCDNX_IF_ERR_EXIT(READ_CMIC_SBUS_TIMEOUTr(unit, &timeout_val));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0xffffffff));

        
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, i, 0x0, tbl_data));

        
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_TIMEOUTr(unit, timeout_val));

        
        

        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r(unit, i,  mc_conf_0_fld_val));
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r(unit, i,  mc_conf_1_fld_val));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, i, 0, INGRESS_SHAPING_ENABLEf,  ingr_shp_en_fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, SCH_SMP_BACK_UP_MESSAGESr, i, 0, BACKUP_MSG_ENABLEf,  backup_msg_en));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_reset_nif_txi_oor(int unit) 
{
    soc_reg_above_64_val_t reg_above64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CREATE_MASK(reg_above64_val, 128, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, SOC_CORE_ALL, reg_above64_val));

    sal_usleep(1);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_INIT_FQP_TXI_NIFr(unit, SOC_CORE_ALL, reg_above64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_soft_init_order_dependent_list_get(
    int            unit,
    soc_field_t    **order_depndend_list,
    uint32         *order_depndend_list_size,
    int            is_ingress)
{
    int rv = SOC_E_NONE;


    if (SOC_IS_FLAIR(unit)) {
        *order_depndend_list = NULL;
        *order_depndend_list_size = 0;
    }
    else if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        if (is_ingress) {
            *order_depndend_list = jer_plus_ingress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(jer_plus_ingress_order_dependent_init_field_list)/sizeof(jer_plus_ingress_order_dependent_init_field_list[0]);
        }
        else {
            *order_depndend_list = jer_plus_egress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(jer_plus_egress_order_dependent_init_field_list)/sizeof(jer_plus_egress_order_dependent_init_field_list[0]);
        }
    } 
    else if (SOC_IS_QUX(unit)) 
    {
        if (is_ingress) {
            *order_depndend_list = NULL;
            *order_depndend_list_size = 0;
        }
        else {
            *order_depndend_list = qux_egress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(qux_egress_order_dependent_init_field_list)/sizeof(qux_egress_order_dependent_init_field_list[0]);
        }
    } 
    else if (SOC_IS_QAX(unit)) 
    {
        if (is_ingress) {
            *order_depndend_list = NULL;
            *order_depndend_list_size = 0;
        }
        else {
            *order_depndend_list = qax_egress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(qax_egress_order_dependent_init_field_list)/sizeof(qax_egress_order_dependent_init_field_list[0]);
        }
    } 
    else if (SOC_IS_JERICHO(unit))
    {
        if (is_ingress) {
            *order_depndend_list = jer_ingress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(jer_ingress_order_dependent_init_field_list)/sizeof(jer_ingress_order_dependent_init_field_list[0]);
        }
        else {
            *order_depndend_list = jer_egress_order_dependent_init_field_list;
            *order_depndend_list_size = sizeof(jer_egress_order_dependent_init_field_list)/sizeof(jer_egress_order_dependent_init_field_list[0]);
        }
    }
    else {
        rv = SOC_E_NOT_FOUND;
    }

    return rv;
}

STATIC int soc_jer_soft_reset_order_dependent_list_get(
    int            unit,
    soc_field_t    **order_depndend_list,
    uint32         *order_depndend_list_size,
    int            is_ingress)
{
    int rv = SOC_E_NONE;


    if (SOC_IS_FLAIR(unit)) {
        *order_depndend_list = NULL;
        *order_depndend_list_size = 0;
    }
    else if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        if (is_ingress) {
            *order_depndend_list = jer_plus_ingress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(jer_plus_ingress_order_dependent_reset_field_list)/sizeof(jer_plus_ingress_order_dependent_reset_field_list[0]);
        }
        else {
            *order_depndend_list = jer_plus_egress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(jer_plus_egress_order_dependent_reset_field_list)/sizeof(jer_plus_egress_order_dependent_reset_field_list[0]);
        }
    } 
    else if (SOC_IS_QUX(unit)) 
    {
        if (is_ingress) {
            *order_depndend_list = NULL;
            *order_depndend_list_size = 0;
        }
        else {
            *order_depndend_list = qux_egress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(qux_egress_order_dependent_reset_field_list)/sizeof(qux_egress_order_dependent_reset_field_list[0]);
        }
    } 
    else if (SOC_IS_QAX(unit)) 
    {
        if (is_ingress) {
            *order_depndend_list = NULL;
            *order_depndend_list_size = 0;
        }
        else {
            *order_depndend_list = qax_egress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(qax_egress_order_dependent_reset_field_list)/sizeof(qax_egress_order_dependent_reset_field_list[0]);
        }
    } 
    else if (SOC_IS_JERICHO(unit))
    {
        if (is_ingress) {
            *order_depndend_list = jer_ingress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(jer_ingress_order_dependent_reset_field_list)/sizeof(jer_ingress_order_dependent_reset_field_list[0]);
        }
        else {
            *order_depndend_list = jer_egress_order_dependent_reset_field_list;
            *order_depndend_list_size = sizeof(jer_egress_order_dependent_reset_field_list)/sizeof(jer_egress_order_dependent_reset_field_list[0]);
        }
    }
    else {
        rv = SOC_E_NOT_FOUND;
    }

    return rv;
}

STATIC int jer_soft_reset(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_DBG_RST_DOMAIN    rst_domain)
{
    int                     rv;
    int                     field_index;
    int                     ingress_field_list_size;
    int                     egress_field_list_size;
    uint32                  reg_val=0;
    uint32                  nbil0_orig = 0, nbil1_orig = 0, nbih_orig = 0, nif_orig = 0;
    uint32                  res = SOC_SAND_OK;
    uint64                  reg64;
    uint8                   is_traffic_enabled_orig;
    uint8                   is_ingr;
    uint8                   is_egr;
    uint8                   is_ctrl_cells_enabled_orig;
    uint8                   is_fabric = 0;
    uint8                   is_schan_locked = 0;
    soc_reg_above_64_val_t  soft_init_reg_val;
    soc_reg_above_64_val_t  soft_init_reg_val_orig;
    soc_reg_above_64_val_t  fld_above64_val;
    soc_reg_above_64_val_t  reg_above64_val;
    soc_reg_above_64_val_t  queue_is_disable_orig[SOC_DPP_DEFS_MAX(NOF_CORES)];
    soc_reg_above_64_val_t  queue_is_disable;
    soc_reg_above_64_val_t tdm_context_drop;

    soc_field_t             *ingress_order_dependent_field_list = NULL;
    uint32                  ingress_order_dependent_field_list_size = 0;
    soc_field_t             *egress_order_dependent_field_list = NULL;
    uint32                  egress_order_dependent_field_list_size = 0;

    soc_field_t             *ingress_field_list;
    soc_field_t             *egress_field_list;
    SHR_BIT_DCL_CLR_NAME(drc_monitor_enable_bitmap, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));
    int enabled;
    int drc_ndx;
    int core;
    uint32 pfc_mask[SOC_DPP_DEFS_MAX(NOF_PM4X25) + SOC_DPP_DEFS_MAX(NOF_PM4X2P5) + SOC_DPP_DEFS_MAX(NOF_PM4X10)];
    uint32 pm_idx;
    uint64 llfc_mask64, reg_val64;
    soc_reg_above_64_val_t llfc_mask_above_64, reg_val_above_64;
    uint32 nof_pm4x25    = SOC_DPP_DEFS_GET(unit, nof_pm4x25);
    uint32 nof_pm4x2p5    = SOC_DPP_DEFS_GET(unit, nof_pm4x2p5);
    uint32 nof_pm4x10       = SOC_DPP_DEFS_GET(unit, nof_pm4x10);
    uint32 nof_port_macros = nof_pm4x25 + nof_pm4x2p5 + nof_pm4x10;
    jer_mgmt_enable_traffic_config_t enable_traffic_config_orig, disable_traffic_config;

    soc_field_t jer_ingress_field_list[] = {
            BLOCKS_SOFT_INIT_1f,  BLOCKS_SOFT_INIT_2f,  BLOCKS_SOFT_INIT_3f,  BLOCKS_SOFT_INIT_4f,  
            BLOCKS_SOFT_INIT_5f,  BLOCKS_SOFT_INIT_6f,  BLOCKS_SOFT_INIT_8f,  BLOCKS_SOFT_INIT_9f, 
            BLOCKS_SOFT_INIT_10f, BLOCKS_SOFT_INIT_22f, BLOCKS_SOFT_INIT_23f, BLOCKS_SOFT_INIT_24f, 
            BLOCKS_SOFT_INIT_25f, BLOCKS_SOFT_INIT_26f, BLOCKS_SOFT_INIT_55f, BLOCKS_SOFT_INIT_56f, 
            BLOCKS_SOFT_INIT_57f, BLOCKS_SOFT_INIT_58f, BLOCKS_SOFT_INIT_59f, BLOCKS_SOFT_INIT_60f, 
            BLOCKS_SOFT_INIT_61f, BLOCKS_SOFT_INIT_62f, BLOCKS_SOFT_INIT_63f, BLOCKS_SOFT_INIT_64f
        };
    soc_field_t jer_plus_ingress_field_list[] = {
            BLOCKS_SOFT_INIT_1f,  BLOCKS_SOFT_INIT_2f,  BLOCKS_SOFT_INIT_3f,  BLOCKS_SOFT_INIT_4f,  
            BLOCKS_SOFT_INIT_5f,  BLOCKS_SOFT_INIT_6f,  BLOCKS_SOFT_INIT_8f,  BLOCKS_SOFT_INIT_9f, 
            BLOCKS_SOFT_INIT_10f, BLOCKS_SOFT_INIT_22f, BLOCKS_SOFT_INIT_23f, BLOCKS_SOFT_INIT_24f, 
            BLOCKS_SOFT_INIT_25f, BLOCKS_SOFT_INIT_26f, BLOCKS_SOFT_INIT_59f, BLOCKS_SOFT_INIT_60f, 
            BLOCKS_SOFT_INIT_61f, BLOCKS_SOFT_INIT_62f, BLOCKS_SOFT_INIT_63f, BLOCKS_SOFT_INIT_64f, 
            BLOCKS_SOFT_INIT_65f, BLOCKS_SOFT_INIT_70f, BLOCKS_SOFT_INIT_71f
         };
    soc_field_t qax_ingress_field_list[] = {
            BLOCKS_SOFT_INIT_23f,  BLOCKS_SOFT_INIT_1f,   BLOCKS_SOFT_INIT_2f,   BLOCKS_SOFT_INIT_9f,  
            BLOCKS_SOFT_INIT_22f,  BLOCKS_SOFT_INIT_24f,  BLOCKS_SOFT_INIT_25f,  BLOCKS_SOFT_INIT_55f, 
            BLOCKS_SOFT_INIT_56f,  BLOCKS_SOFT_INIT_57f,  BLOCKS_SOFT_INIT_58f,  BLOCKS_SOFT_INIT_60f, 
            BLOCKS_SOFT_INIT_63f,  BLOCKS_SOFT_INIT_64f,  BLOCKS_SOFT_INIT_108f, BLOCKS_SOFT_INIT_10f, 
            BLOCKS_SOFT_INIT_19f,  BLOCKS_SOFT_INIT_20f,  BLOCKS_SOFT_INIT_3f,   BLOCKS_SOFT_INIT_4f,
            BLOCKS_SOFT_INIT_29f,  BLOCKS_SOFT_INIT_30f,  BLOCKS_SOFT_INIT_34f,  BLOCKS_SOFT_INIT_35f,
            BLOCKS_SOFT_INIT_36f,  BLOCKS_SOFT_INIT_37f,  BLOCKS_SOFT_INIT_59f,
            BLOCKS_SOFT_INIT_107f, BLOCKS_SOFT_INIT_109f, BLOCKS_SOFT_INIT_110f, 
            BLOCKS_SOFT_INIT_111f
        };
    soc_field_t qux_ingress_field_list[] = {
            BLOCKS_SOFT_INIT_23f,  BLOCKS_SOFT_INIT_1f,   BLOCKS_SOFT_INIT_2f,   BLOCKS_SOFT_INIT_9f,  
            BLOCKS_SOFT_INIT_22f,  BLOCKS_SOFT_INIT_24f,  BLOCKS_SOFT_INIT_25f,  BLOCKS_SOFT_INIT_47f, 
            BLOCKS_SOFT_INIT_41f,  BLOCKS_SOFT_INIT_42f,  BLOCKS_SOFT_INIT_43f,  BLOCKS_SOFT_INIT_45f, 
            BLOCKS_SOFT_INIT_28f,  BLOCKS_SOFT_INIT_29f,  BLOCKS_SOFT_INIT_16f,  BLOCKS_SOFT_INIT_10f, 
            BLOCKS_SOFT_INIT_19f,  BLOCKS_SOFT_INIT_20f,  BLOCKS_SOFT_INIT_3f,   BLOCKS_SOFT_INIT_4f,
            BLOCKS_SOFT_INIT_44f,  BLOCKS_SOFT_INIT_27f,  BLOCKS_SOFT_INIT_15f,  BLOCKS_SOFT_INIT_17f,
            BLOCKS_SOFT_INIT_18f,  BLOCKS_SOFT_INIT_26f,  BLOCKS_SOFT_INIT_12f,  BLOCKS_SOFT_INIT_13f,
            BLOCKS_SOFT_INIT_14f
        };
    soc_field_t jer_egress_field_list[] = { 
            BLOCKS_SOFT_INIT_104f, BLOCKS_SOFT_INIT_105f, BLOCKS_SOFT_INIT_106f, 
            BLOCKS_SOFT_INIT_50f, BLOCKS_SOFT_INIT_51f,  BLOCKS_SOFT_INIT_55f, BLOCKS_SOFT_INIT_56f,  
            BLOCKS_SOFT_INIT_57f
        };
    soc_field_t jer_plus_egress_field_list[] = {
            BLOCKS_SOFT_INIT_111f, BLOCKS_SOFT_INIT_112f, BLOCKS_SOFT_INIT_113f, 
            BLOCKS_SOFT_INIT_54f, BLOCKS_SOFT_INIT_55f,  BLOCKS_SOFT_INIT_59f, BLOCKS_SOFT_INIT_60f,  
            BLOCKS_SOFT_INIT_61f
        };
    soc_field_t qax_egress_field_list[] = { 
            BLOCKS_SOFT_INIT_55f, BLOCKS_SOFT_INIT_56f, 
            BLOCKS_SOFT_INIT_57f, BLOCKS_SOFT_INIT_112f
        };
    soc_field_t qux_egress_field_list[] = { 
            BLOCKS_SOFT_INIT_47f, BLOCKS_SOFT_INIT_41f, BLOCKS_SOFT_INIT_42f, 
            BLOCKS_SOFT_INIT_21f
        };


    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        ingress_field_list      = jer_plus_ingress_field_list;
        ingress_field_list_size = sizeof(jer_plus_ingress_field_list)/sizeof(jer_plus_ingress_field_list[0]);
        egress_field_list       = jer_plus_egress_field_list;
        egress_field_list_size  = sizeof(jer_plus_egress_field_list)/sizeof(jer_plus_egress_field_list[0]);
    } 
    else if (SOC_IS_QUX(unit)) 
    {
        ingress_field_list      = qux_ingress_field_list;
        ingress_field_list_size = sizeof(qux_ingress_field_list)/sizeof(qux_ingress_field_list[0]);
        egress_field_list       = qux_egress_field_list;
        egress_field_list_size  = sizeof(qux_egress_field_list)/sizeof(qux_egress_field_list[0]);
    } 
    else if (SOC_IS_QAX(unit)) 
    {
        ingress_field_list      = qax_ingress_field_list;
        ingress_field_list_size = sizeof(qax_ingress_field_list)/sizeof(qax_ingress_field_list[0]);
        egress_field_list       = qax_egress_field_list;
        egress_field_list_size  = sizeof(qax_egress_field_list)/sizeof(qax_egress_field_list[0]);
    } 
    else 
    {
        ingress_field_list      = jer_ingress_field_list;
        ingress_field_list_size = sizeof(jer_ingress_field_list)/sizeof(jer_ingress_field_list[0]);
        egress_field_list       = jer_egress_field_list;
        egress_field_list_size  = sizeof(jer_egress_field_list)/sizeof(jer_egress_field_list[0]);
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_soft_init_order_dependent_list_get(unit, &ingress_order_dependent_field_list, &ingress_order_dependent_field_list_size, 1));
    SOCDNX_IF_ERR_EXIT(soc_jer_soft_init_order_dependent_list_get(unit, &egress_order_dependent_field_list, &egress_order_dependent_field_list_size, 0));

    is_ingr = (rst_domain == ARAD_DBG_RST_DOMAIN_INGR) || (rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || (rst_domain == ARAD_DBG_RST_DOMAIN_FULL) || (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC);
    is_egr  = (rst_domain == ARAD_DBG_RST_DOMAIN_EGR)  || (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC)  || (rst_domain == ARAD_DBG_RST_DOMAIN_FULL) || (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC);
    if (!SOC_IS_QUX(unit)) {
        
        is_fabric = (rst_domain == ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC) || (rst_domain == ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC) || (rst_domain == ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC);
    }
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Start. is_ingr=%d, is_egr=%d, is_fabric=%d\n"), FUNCTION_NAME(), is_ingr, is_egr, is_fabric));

    
    
    
    if (is_egr) {
       for (core = 0 ; core < SOC_DPP_DEFS_GET(unit, nof_cores) ; core++) {
           if (!SOC_IS_QAX(unit)) { 
               SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, queue_is_disable_orig[core]));
           }
           else {
               SOCDNX_IF_ERR_EXIT(READ_ECGM_CGM_QUEUE_IS_DISABLEDr(unit, queue_is_disable_orig[core]));   
           }
       }
       SOC_REG_ABOVE_64_CREATE_MASK(queue_is_disable, 512, 0);
       if (!SOC_IS_QAX(unit)) { 
           SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_QUEUE_IS_DISABLEDr(unit, SOC_CORE_ALL, queue_is_disable));
       }
       else {
           SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CGM_QUEUE_IS_DISABLEDr(unit, queue_is_disable));
       }
     

       sal_usleep(100);
       rv = soc_dpp_polling(unit, 100*ARAD_TIMEOUT, ARAD_MIN_POLLS, ECI_GP_STATUS_5r, REG_PORT_ANY, 0, PIR_EGQ_0_FIFO_NOT_EMPTY_STICKYf, 0x0);
       if (SOC_FAILURE(rv))
       {
           LOG_WARN(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Warning, Could not validate that Data Path is clean.\n"), FUNCTION_NAME()));
       }
       sal_usleep(100);
       rv = soc_dpp_polling(unit, 100*ARAD_TIMEOUT, ARAD_MIN_POLLS, ECI_GP_STATUS_5r, REG_PORT_ANY, 0, PIR_EGQ_1_FIFO_NOT_EMPTY_STICKYf, 0x0); 
       if (SOC_FAILURE(rv))
       {
           LOG_WARN(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Warning, Could not validate that Data Path is clean.\n"), FUNCTION_NAME()));
       }       
    }
    
    
    
    
    res = jer_mgmt_enable_traffic_advanced_get(unit, &enable_traffic_config_orig, &is_traffic_enabled_orig);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Disable Traffic. is_traffic_enabled_orig=%d\n"), FUNCTION_NAME(), is_traffic_enabled_orig));

    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_0_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_1_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_0_prio_2_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_0_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_1_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_1_prio_2_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_0_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_1_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;
    disable_traffic_config.fda_fabfifos_prio_drop_thresholds.fda_fabfif_2_prio_2_drop_th = SOC_JER_RESET_FDA_FABFIFO_LOW_DROP_THRESHOLD;

    
    if (!SOC_IS_QUX(unit))
    {
        SOCDNX_IF_ERR_EXIT(READ_IRE_TDM_CONTEXT_DROPr(unit, tdm_context_drop));
        SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_TDM_CONTEXT_DROPr(unit, reg_above64_val));
    }

    res = jer_mgmt_enable_traffic_advanced_set(unit, &disable_traffic_config, FALSE);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

    

    if (is_fabric && !soc_feature(unit, soc_feature_no_fabric))
    {
        
        res = arad_mgmt_all_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled_orig);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

        
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, FALSE, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    }

    
    
    
     if (is_egr) {
        sal_usleep(100);
        if (!SOC_IS_QAX(unit)) 
        {
            rv = soc_dpp_polling(unit, 100*ARAD_TIMEOUT, ARAD_MIN_POLLS, CGM_TOTAL_PACKET_DESCRIPTORS_COUNTERr, 
            REG_PORT_ANY, 0, NUMBER_OF_ALLOCATED_PACKET_DESCRIPTORSf, 0x0);
            if (SOC_FAILURE(rv)) 
            {
                LOG_WARN(BSL_LS_SOC_INIT, 
                (BSL_META_U(unit, "%s(): Warning, Could not validate that Data Path is clean.\n"), FUNCTION_NAME()));
            }
        }
        else {
            rv = soc_dpp_polling(unit, 100*ARAD_TIMEOUT, ARAD_MIN_POLLS, ECGM_TOTAL_PACKET_DESCRIPTORS_COUNTERr, REG_PORT_ANY, 0, NUMBER_OF_ALLOCATED_PACKET_DESCRIPTORSf, 0x0);
            if (SOC_FAILURE(rv)) 
            {
                LOG_WARN(BSL_LS_SOC_INIT, 
                (BSL_META_U(unit, "%s(): Warning, Could not validate that Data Path is clean.\n"), FUNCTION_NAME()));
            }
        }
        sal_usleep(100);
    }

    
    is_schan_locked = 1;
    SCHAN_LOCK(unit);

    
    
    
    
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_NIF_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, &llfc_mask64));
        COMPILER_64_SET(reg_val64, 0, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NIF_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, reg_val64));

        for (pm_idx = 0 ; pm_idx < nof_port_macros ; pm_idx++) {
            SOCDNX_IF_ERR_EXIT(READ_NIF_FC_PFC_QMLF_MASKr(unit, pm_idx, &(pfc_mask[pm_idx])));
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_FC_PFC_QMLF_MASKr(unit, pm_idx, 0));
        }
    } else if (SOC_IS_QAX(unit)) {
        SOC_REG_ABOVE_64_CLEAR(llfc_mask_above_64);
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SOCDNX_IF_ERR_EXIT(READ_NBIH_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, llfc_mask_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, reg_val_above_64));

        for (pm_idx = 0 ; pm_idx < nof_port_macros ; pm_idx++) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_FC_PFC_QMLF_MASKr(unit, pm_idx, &(pfc_mask[pm_idx])));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_FC_PFC_QMLF_MASKr(unit, pm_idx, 0));
        }
    }
    

    
    
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit,soft_init_reg_val));
    sal_memcpy(soft_init_reg_val_orig, soft_init_reg_val, sizeof(soc_reg_above_64_val_t));

    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Read original configuration. soft_init_reg_val_orig=0x%x,0x%x,0x%x,0x%x\n"), FUNCTION_NAME(), soft_init_reg_val_orig[0], soft_init_reg_val_orig[1], soft_init_reg_val_orig[2], soft_init_reg_val_orig[3]));
    
    
    
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): IN-RESET\n"), FUNCTION_NAME()));

    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
    SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);


    if (is_ingr) 
    {
        SHR_BIT_ITER(SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_bitmap, SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max), drc_ndx) {
            
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_cdr_monitor_enable_get(unit, drc_ndx, &enabled));
            if (enabled) {
                SHR_BITSET(drc_monitor_enable_bitmap, drc_ndx);
                SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 0));
            }
        }

        
        for (field_index=0; field_index<ingress_order_dependent_field_list_size; field_index++)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, ingress_order_dependent_field_list[field_index], fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        }
    }

    if (is_fabric) {
        
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_34f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_35f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_36f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_37f, fld_above64_val);

        if (!SOC_IS_QAX(unit)) { 
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_38f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_39f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_40f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_41f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_42f, fld_above64_val);

            if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_43f, fld_above64_val);
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_44f, fld_above64_val);
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_45f, fld_above64_val);
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
    }

    if (is_ingr)
    {
        for (field_index = 0; field_index < ingress_field_list_size; ++field_index)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, ingress_field_list[field_index], fld_above64_val);
        }
        if (is_fabric) 
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_27f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_28f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_33f, fld_above64_val);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        SOCDNX_IF_ERR_EXIT(soc_jer_dram_init_drc_soft_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info, 1));

         
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x40));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x0));
    }

    if (is_egr) 
    {
        
        if (!SOC_IS_QUX(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_NBIH_ADDITIONAL_RESETSr(unit, &reg_val));
            nbih_orig = reg_val;
            soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, EIF_RSTNf, 0);
            soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
            soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
            if (!SOC_IS_QAX(unit)) 
            {
                soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0); 
                soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0);
                soc_reg_field_set(unit, NBIH_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ADDITIONAL_RESETSr(unit,  reg_val));
    
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 0, &reg_val));
            nbil0_orig = reg_val;
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0);
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
            if (!SOC_IS_QAX(unit)) 
            {
                soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0); 
                soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 0, reg_val));
    
            SOCDNX_IF_ERR_EXIT(READ_NBIL_ADDITIONAL_RESETSr(unit, 1, &reg_val));
            nbil1_orig = reg_val;
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0);
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_RX_RSTNf, 0);
            soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_0_TX_RSTNf, 0);
            if (!SOC_IS_QAX(unit)) 
            {
                soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_RX_RSTNf, 0); 
                soc_reg_field_set(unit, NBIL_ADDITIONAL_RESETSr, &reg_val, ELK_1_TX_RSTNf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 1, reg_val));
        } else {
            SOCDNX_IF_ERR_EXIT(READ_NIF_ADDITIONAL_RESETSr(unit, &reg_val));
            nif_orig = reg_val;
            soc_reg_field_set(unit, NIF_ADDITIONAL_RESETSr, &reg_val, EIF_RSTNf, 0);
            soc_reg_field_set(unit, NIF_ADDITIONAL_RESETSr, &reg_val, SIF_RSTNf, 0); 
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_ADDITIONAL_RESETSr(unit,  reg_val));
        }

        
        for (field_index=0; field_index<egress_order_dependent_field_list_size; field_index++) {
            if (egress_order_dependent_field_list[field_index] == INVALIDf) {
                continue;
            }
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, egress_order_dependent_field_list[field_index], fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        }

        for (field_index = 0; field_index < egress_field_list_size; ++field_index)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, egress_field_list[field_index], fld_above64_val);
        }
        if (is_fabric) 
        {
            if (SOC_IS_JERICHO_PLUS(unit)) 
            {
                SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_12_12f, 0x1);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_13_13f, 0x1);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_14_14f, 0x1);
                SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));                
            }
            else
            {             
                SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_31_31f, 0x1);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_32_32f, 0x1);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_33_33f, 0x1);
                SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));
            }

            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_31f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_32f, fld_above64_val);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x1));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x1));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x1));
        }
        
        rv = jer_sch_reset(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    
    
    sal_usleep(500);
	
    
    
    
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): OUT-OF-RESET.\n"), FUNCTION_NAME()));

    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

    if (is_ingr) 
    {
        
        for (field_index=0; field_index<ingress_order_dependent_field_list_size; field_index++)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, ingress_order_dependent_field_list[field_index], fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        }
    }

    if (is_fabric) {
        
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_34f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_35f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_36f, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_37f, fld_above64_val);

        if (!SOC_IS_QAX(unit)) { 
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_38f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_39f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_40f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_41f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_42f, fld_above64_val);

            if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_43f, fld_above64_val);
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_44f, fld_above64_val);
                soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_45f, fld_above64_val);
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
    }

    if (is_ingr)
    {
        for (field_index = 0; field_index < ingress_field_list_size; ++field_index)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, ingress_field_list[field_index], fld_above64_val);
        }
        if (is_fabric) 
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_27f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_28f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_33f, fld_above64_val);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));

        
        SOCDNX_IF_ERR_EXIT(soc_jer_dram_init_drc_soft_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info, 0));

        if (SOC_IS_QAX(unit)) 
        {
            
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, SQM_SQM_INITr, REG_PORT_ANY, 0, PDM_INIT_ONf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, SQM_SQM_INITr, REG_PORT_ANY, 0, QDM_INIT_ONf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0));
        }
        else
        {
            
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, IQC_INITf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, STE_INITf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 0, 0, PDM_INITf, 0));
            if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1 )
            {
                
                SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, PDM_INITf, 0));
                SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, IQC_INITf, 0));
                SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, 1, 0, STE_INITf, 0));
            }
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, 0, 0, IPS_INIT_TRIGGERf, 0));
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, 1, 0, IPS_INIT_TRIGGERf, 0));

            
            SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_autogen_set(unit));
        }
        SHR_BIT_ITER(SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_bitmap, SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max), drc_ndx) {
            
            if (SHR_BITGET(drc_monitor_enable_bitmap, drc_ndx)) {
                SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 1));
            }
        }
    }

    if(is_egr) 
    {
        
        for (field_index=0; field_index<egress_order_dependent_field_list_size; field_index++) {

            if (egress_order_dependent_field_list[field_index] == INVALIDf) {
                
                SOCDNX_IF_ERR_EXIT(soc_jer_reset_nif_txi_oor(unit));
                continue;
            }
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, egress_order_dependent_field_list[field_index], fld_above64_val);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        }



        for (field_index = 0; field_index < egress_field_list_size; ++field_index)
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, egress_field_list[field_index], fld_above64_val);
        }
        if (is_fabric) 
        {
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_31f, fld_above64_val);
            soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, soft_init_reg_val, BLOCKS_SOFT_INIT_32f, fld_above64_val);

            if (SOC_IS_JERICHO_PLUS(unit)) 
            {
                SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_12_12f, 0);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_13_13f, 0);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_14_14f, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));                
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_31_31f, 0);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_32_32f, 0);
                soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_33_33f, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val));
        sal_usleep(1);

        
        if (!SOC_IS_QUX(unit)) {
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_ADDITIONAL_RESETSr(unit, nbih_orig));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 0, nbil0_orig));
            SOCDNX_IF_ERR_EXIT(WRITE_NBIL_ADDITIONAL_RESETSr(unit, 1, nbil1_orig));
        } else {
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_ADDITIONAL_RESETSr(unit, nif_orig)); 
        }

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_0_RXI_RESET_Nf, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_1_RXI_RESET_Nf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));
                
        SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_0_RXI_RESET_Nf, fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_EGQ_1_RXI_RESET_Nf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INIT_VALUEf, fld_above64_val);
        SOC_REG_ABOVE_64_CREATE_MASK(fld_above64_val, 1, 0);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INITf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));
        SOC_REG_ABOVE_64_CLEAR(fld_above64_val);
        soc_reg_above_64_field_set(unit, ECI_GP_CONTROL_9r, reg_above64_val, PIR_TXI_CREDITS_INITf, fld_above64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above64_val));

        
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x1));

        
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_FL_STSf,  0x0));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_CRD_FCRf,  0x0));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, REG_PORT_ANY, 0, DIS_SRf,  0x0));
        }
         
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_CMICMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_CMICMf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OLPr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OLPf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_OAMr, SOC_CORE_ALL, 0, INIT_FQP_TXI_OAMf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_RCYr, SOC_CORE_ALL, 0, INIT_FQP_TXI_RCYf,  0x1));

        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_INIT_FQP_TXI_IPSECr, SOC_CORE_ALL, 0, INIT_FQP_TXI_IPSECf,  0x1));
        }

        if (SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
            
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_LBG_CREDIT_INITr, SOC_CORE_ALL, LBG_CREDIT_INITf,  0x1fff));
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, EGQ_LBG_CREDIT_INITr, SOC_CORE_ALL, LBG_CREDIT_INITf,  0x0));
        }

        
        rv = soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0);
        if (SOC_FAILURE(rv)) 
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Error Validate out-of-reset done indications: EGQ_EGQ_BLOCK_INIT_STATUSr, EGQ_BLOCK_INITf.\n"), FUNCTION_NAME()));
        }

        
        for (core = 0 ; core < SOC_DPP_DEFS_GET(unit, nof_cores) ; core++) {
            if (!SOC_IS_QAX(unit)) { 
                SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, queue_is_disable_orig[core]));
            }
            else {
                SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CGM_QUEUE_IS_DISABLEDr(unit, queue_is_disable_orig[core]));
            }
        }
    }

    
    
    
    
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, soft_init_reg_val_orig));

    
    SCHAN_UNLOCK(unit);
    is_schan_locked = 0;

    
    
    
    if (is_fabric && !soc_feature(unit, soc_feature_no_fabric))
    {
        res = arad_mgmt_all_ctrl_cells_enable_set_unsafe(unit, is_ctrl_cells_enabled_orig, ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET);
        SOCDNX_IF_ERR_EXIT(handle_sand_result(res));
    }

    
    
    
    
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_NIF_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, llfc_mask64));
        for (pm_idx = 0 ; pm_idx < nof_port_macros ; pm_idx++) {
            SOCDNX_IF_ERR_EXIT(WRITE_NIF_FC_PFC_QMLF_MASKr(unit, pm_idx, pfc_mask[pm_idx]));
        }
    } else if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_FC_TX_GEN_LLFC_FROM_QMLF_MASKr(unit, llfc_mask_above_64));
        for (pm_idx = 0 ; pm_idx < nof_port_macros ; pm_idx++) {
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_FC_PFC_QMLF_MASKr(unit, pm_idx, pfc_mask[pm_idx]));
        }
    }

    
    
    

    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): Restore traffic.\n"), FUNCTION_NAME()));
    res = jer_mgmt_enable_traffic_advanced_set(unit, &enable_traffic_config_orig, is_traffic_enabled_orig);
    SOCDNX_IF_ERR_EXIT(handle_sand_result(res));

    
    if (!SOC_IS_QUX(unit))
    {
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_TDM_CONTEXT_DROPr(unit, tdm_context_drop));
    }

    
    
    
    if (!SOC_IS_QAX(unit)) 
    {
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL,  0xffffffff)); 
    }
exit:
    if (is_schan_locked) 
    {
        SCHAN_UNLOCK(unit);
    }   
    SOCDNX_FUNC_RETURN;
}


STATIC int soc_jer_reset_sbus_reset(int unit, int is_in_reset)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;
    if((SOC_IS_JERICHO(unit)) && (!SOC_IS_QAX(unit))) 
    {
             
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_21f, is_in_reset);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
             
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_19f, is_in_reset);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SBUS_RESETr, reg_above_64_val, BLOCKS_SBUS_RESET_20f, is_in_reset);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    }
    
    if (is_in_reset) {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int soc_jer_reset_blocks_reset(int unit, int is_in_reset)
{
    int disable_hard_reset;
    uint32 reg_val;
    uint32 field_val;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t field;
    soc_field_t *ingress_order_dependent_field_list = NULL;
    uint32 ingress_order_dependent_field_list_size = 0;
    soc_field_t *egress_order_dependent_field_list = NULL;
    uint32 egress_order_dependent_field_list_size = 0;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_soft_reset_order_dependent_list_get(unit, &ingress_order_dependent_field_list, &ingress_order_dependent_field_list_size, 1));
    SOCDNX_IF_ERR_EXIT(soc_jer_soft_reset_order_dependent_list_get(unit, &egress_order_dependent_field_list, &egress_order_dependent_field_list_size, 0));

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

    for (i=0; i<ingress_order_dependent_field_list_size; i++) {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, ingress_order_dependent_field_list[i], is_in_reset);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

    for (i=0; i<egress_order_dependent_field_list_size; i++) {
        if (egress_order_dependent_field_list[i] == INVALIDf) {
            continue;
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, reg_above_64_val, egress_order_dependent_field_list[i], is_in_reset);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    }

    
    if (is_in_reset) {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    } else {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

    
    if (SOC_REG_IS_VALID(unit,ECI_GP_CONTROL_9r)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_above_64_val, PDM_RSTNf, is_in_reset ? 0 : 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_above_64_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_ECIC_BLOCKS_RESETr(unit, &reg_val));
    field = SOC_IS_JERICHO_PLUS_A0(unit) || SOC_IS_QUX(unit) ? PNIMI_002f : FIELD_0_0f;
    soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val,field , is_in_reset);
    if (soc_reg_field_valid(unit, ECI_ECIC_BLOCKS_RESETr, TIME_SYNC_RESETf)) {
        soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val, TIME_SYNC_RESETf, is_in_reset);
    }
    if (SOC_IS_FLAIR(unit)) {
        
        field_val = (is_in_reset == 0)?1:0;
        soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val, FGL_ING_RESET_Nf, field_val);
        soc_reg_field_set(unit, ECI_ECIC_BLOCKS_RESETr, &reg_val, FGL_EGR_RESET_Nf, field_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_ECIC_BLOCKS_RESETr(unit, reg_val));

    disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
    LOG_VERBOSE(BSL_LS_SOC_INIT, (BSL_META_U(unit, "%s(): disable_hard_reset = %d\n"), FUNCTION_NAME(), disable_hard_reset));
    if (disable_hard_reset == 1) {
        if (is_in_reset == 1) {
            
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x40));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_TXBUF_IPINTF_INTERFACE_CREDITSr(unit, 0x0));

            
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x1));
            SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RXBUF_EPINTF_RELEASE_ALL_CREDITSr(unit, 0x0));
        }
    }

    if (!SOC_IS_FLAIR(unit) && (is_in_reset == 0x0)) {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_poll_init_finish(unit));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_reset_blocks_soft_init(int unit, int reset_action)
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t *ingress_order_dependent_field_list = NULL;
    uint32 ingress_order_dependent_field_list_size = 0;
    soc_field_t *egress_order_dependent_field_list = NULL;
    uint32 egress_order_dependent_field_list_size = 0;
    int i;
    
    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) { 
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    }

    sal_usleep(100);

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_soft_init_order_dependent_list_get(unit, &ingress_order_dependent_field_list, &ingress_order_dependent_field_list_size, 1));
        SOCDNX_IF_ERR_EXIT(soc_jer_soft_init_order_dependent_list_get(unit, &egress_order_dependent_field_list, &egress_order_dependent_field_list_size, 0));

        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        
        for (i=0; i<ingress_order_dependent_field_list_size; i++) {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, ingress_order_dependent_field_list[i], 0);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        }
        
        for (i=0; i<egress_order_dependent_field_list_size; i++) {
            if (egress_order_dependent_field_list[i] == INVALIDf) {
                continue;
            }
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, egress_order_dependent_field_list[i], 0);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        }
        
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

        
        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_poll_init_finish(unit));
        }
        
        if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_SYS_CFG_RESERVED_1r, REG_PORT_ANY, FIELD_0_0f, 0x1));
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IRE_SYS_CFG_RESERVED_1r, REG_PORT_ANY, FIELD_0_0f, 0x0));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_device_blocks_reset(int unit, int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((reset_action == SOC_DPP_RESET_ACTION_IN_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) { 

        
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_sbus_reset(unit, 1));
        
        
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_reset(unit, 1));
    }

    if ((reset_action == SOC_DPP_RESET_ACTION_OUT_RESET) || (reset_action == SOC_DPP_RESET_ACTION_INOUT_RESET)) {

        
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_sbus_reset(unit, 0));
        SOCDNX_IF_ERR_EXIT(jer_pll_init(unit));
        
        SOCDNX_IF_ERR_EXIT(soc_jer_reset_blocks_reset(unit, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_init_reg_access(
    int unit,
    int reset_action)
{
    SOCDNX_INIT_FUNC_DEFS;

 

     
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        int sw_state_size = soc_property_get(unit, spn_SW_STATE_MAX_SIZE,
                  SHR_SW_STATE_MAX_DATA_SIZE_IN_BYTES);

        soc_jer_specific_info_config_derived(unit);
        SOC_DPP_CONFIG(unit)->emulation_system = soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0);
        soc_dpp_implementation_defines_init(unit);
        SOCDNX_IF_ERR_RETURN(shr_sw_state_init(unit, 0x0, socSwStateDataBlockRegularInit, sw_state_size));
        SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_ports(unit));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_init_reset(unit, reset_action));
    
    SOCDNX_IF_ERR_EXIT(soc_jer_device_blocks_reset(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));

    
    if ((!SOC_IS_QAX(unit)) && (!SOC_IS_FLAIR(unit))) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit)); 
    }

    

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_arad_init_empty_scache(unit);
#endif

    
    if ((SOC_CONTROL(unit)->soc_flags & SOC_F_INITED) == 0) {
        SOC_INFO(unit).fabric_logical_port_base = soc_property_get(unit, spn_FABRIC_LOGICAL_PORT_BASE, \
            SOC_DPP_FABRIC_LOGICAL_PORT_BASE_DEFAULT);
        SOCDNX_IF_ERR_EXIT(soc_jer_ports_config(unit));
    }

    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_device_reset(int unit, int mode, int action)
{
    uint32 enable;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Jericho function. invalid Device")));
    }

    switch (mode) {
    case SOC_DPP_RESET_MODE_HARD_RESET:
        SOCDNX_IF_ERR_RETURN(soc_dcmn_cmic_device_hard_reset(unit, action));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_RESET:
        SOCDNX_IF_ERR_RETURN(soc_jer_device_blocks_reset(unit, action));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_FULL));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_INGR));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_EGR));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_FULL_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_INGR_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET:
        SOCDNX_IF_ERR_RETURN(jer_soft_reset(unit, ARAD_DBG_RST_DOMAIN_EGR_AND_FABRIC));
        break;
    case SOC_DPP_RESET_MODE_INIT_RESET:
        SOCDNX_IF_ERR_RETURN(soc_dpp_jericho_init(unit, action));
        break;
    case SOC_DPP_RESET_MODE_REG_ACCESS:
        SOCDNX_IF_ERR_RETURN(soc_jer_init_reg_access(unit, action));
        break;
    case SOC_DPP_RESET_MODE_ENABLE_TRAFFIC:
        enable = (action == SOC_DPP_RESET_ACTION_IN_RESET) ? FALSE : TRUE;
        SOCDNX_IF_ERR_RETURN(handle_sand_result(jer_mgmt_enable_traffic_set(unit, enable)));
        break;
    case SOC_DPP_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT:
        SOCDNX_IF_ERR_RETURN(soc_jer_reset_blocks_soft_init(unit, action));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

