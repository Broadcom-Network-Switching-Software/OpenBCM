/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON DRV
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/mem.h>

#include <soc/dnxc/drv.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnxc/dnxc_iproc.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_ser_correction.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>

#include <soc/dnxf/ramon/ramon_drv.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#include <soc/dnxf/ramon/ramon_stack.h>
#include <soc/dnxf/ramon/ramon_port.h>
#include <soc/dnxf/ramon/ramon_fabric_links.h>
#include <soc/dnxf/ramon/ramon_fabric_topology.h>
#include <soc/dnxf/ramon/ramon_fabric_multicast.h>

#include <soc/dnxc/dnxc_intr.h>

#include <soc/cmicx_miim.h>

extern char *_build_release;


#define _SOC_RAMON_DRV_SBUS_RING_MAP_0_VAL                 (0x02222227)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_1_VAL                 (0x33333222)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_2_VAL                 (0X55333333)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_3_VAL                 (0x65555555)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_4_VAL                 (0x66666666)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_5_VAL                 (0x55555555)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_6_VAL                 (0x66666665)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_7_VAL                 (0x44444466)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_8_VAL                 (0x10444444)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_9_VAL                 (0x22040065)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_10_VAL                (0x00000033)

#define _SOC_RAMON_DRV_BRDC_FMAC_AC_ID                     (72)
#define _SOC_RAMON_DRV_BRDC_FMAC_BD_ID                     (73)
#define _SOC_RAMON_DRV_BRDC_DCH                            (78)
#define _SOC_RAMON_DRV_BRDC_DCM                            (80)
#define _SOC_RAMON_DRV_BRDC_DCL                            (79)
#define _SOC_RAMON_DRV_BRDC_CCH                            (81)
#define _SOC_RAMON_DRV_BRDC_FSRD                           (76)


#define _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT   (32)
#define _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)


#define _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE           (0x7)


#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_LOW  (0x0F)
#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_HIGH (0xF0)

#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_NLR      (0)
#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_LR       (1)



#define SOC_RAMON_DRV_LINK_LOAD_SAT_VAL         (0x7ff)
#define SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_BYTE  (0)     
#define SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_FRAG  (1)     
#define SOC_RAMON_DRV_LINK_LOAD_HYST_LOW_TH     (0)
#define SOC_RAMON_DRV_LINK_LOAD_HYST_HIGH_TH    (0xffff)


#define SOC_RAMON_DRV_FC_AGING_COUNTER_PERIOD     (1000000000)


#define SOC_RAMON_DRV_DFL_AUTO_DOC_NAME_33      (0x6fa)


#define SOC_RAMON_DRV_DCML_BANK_WRITE_ALGORITHM_VALUE               (11)
#define SOC_RAMON_DRV_DCML_NOF_BANK_WRITE_ALGORITHM_REG_FIELDS      (12)


#define SOC_RAMON_DRV_ASYNC_FIFO_AUTO_DOC_NAME_59   (5)
#define SOC_RAMON_DRV_BURST_PERIOD                  (0xd)


#define SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS       (1)

#define SOC_RAMON_DRV_MAX_NOF_REG_NUMELS                    (8)
#define SOC_RAMON_DRV_MAX_NOF_DCH_REGISTERS                 (140)
#define SOC_RAMON_DRV_FIRST_DCH_BLOCK_IDX                   (96)


typedef struct {
    
    soc_reg_t reg;
    
    int reg_numels;
    
    soc_reg_above_64_val_t reg_above_64_val[SOC_RAMON_DRV_MAX_NOF_REG_NUMELS][DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
} soc_ramon_soft_init_restore_t;


STATIC int
soc_ramon_reset_cmic_iproc_regs(int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_device.general.hard_reset_disable_get(unit))
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(soc_dnxc_iproc_config_paxb(unit));
    sal_usleep(10*1000); 
    SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
    SHR_IF_ERR_EXIT(soc_dnxc_iproc_config_paxb(unit));

    
    soc_endian_config(unit);
    soc_pci_ep_config(unit,0);

    
    SHR_IF_ERR_EXIT(soc_ramon_drv_rings_map_set(unit));
    SHR_IF_ERR_EXIT(soc_dnxc_cmic_sbus_timeout_set(unit, dnxf_data_device.general.core_clock_khz_get(unit)  ,
                                                   SOC_CONTROL(unit)->schanTimeout));

    
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH0_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH1_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH2_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH3_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH4_ERRr(unit, 0));

    
    SHR_IF_ERR_EXIT(soc_dnxc_mdio_config_set(unit));
exit:
    SHR_FUNC_EXIT;
}



STATIC int
soc_ramon_drv_dynamic_mem_access_enable(int unit)
{
    int reg_index;
    uint32 reg_val32;

    soc_reg_t dyn_mem_access_reg[SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS] = { RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr };

    SHR_FUNC_INIT_VARS(unit);

    for (reg_index = 0 ; reg_index < SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS ; reg_index++)
    {
        reg_val32 = 0;
        soc_reg_field_set(unit, dyn_mem_access_reg[reg_index], &reg_val32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, dyn_mem_access_reg[reg_index], REG_PORT_ANY, 0, reg_val32));
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap) 
{
    soc_reg_above_64_val_t reg_above_64;
    SHR_FUNC_INIT_VARS(unit);

    if (force_blocks_reset_value && (block_bitmap != NULL))
    {
        SOC_REG_ABOVE_64_COPY(reg_above_64, *block_bitmap);
    } else {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
    }
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    sal_usleep(40000);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64));
    sal_usleep(40000);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    sal_usleep(40000);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_device_mode_set(int unit, soc_dnxf_fabric_device_mode_t fabric_device_mode)
{
    uint32 reg32_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val));
        
    
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, FE_13_MODEf,
                          (fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13) ? 1 : 0);

    
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, MULTISTAGE_MODEf,
                           (fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe2 ||
                           fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13) ? 1 : 0);

    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, reg32_val));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_load_balancing_set(int unit, soc_dnxf_load_balancing_mode_t load_balancing)
{
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 load_balancing_val;
    int i, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    load_balancing_val = soc_dnxf_load_balancing_mode_normal == load_balancing ? 1 : 0;
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, DISABLE_RCG_LOAD_BALANCINGf, load_balancing_val);

    load_balancing_val = (soc_dnxf_load_balancing_mode_destination_unreachable == load_balancing) ? 0 : 1;
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, SCT_SCRUB_ENABLEf, load_balancing_val);
                            
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh; ++i) {
        SHR_IF_ERR_EXIT(soc_reg_get(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, &reg_val64));
        soc_reg64_field32_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, &reg_val64, LOAD_BALANCE_LEVELS_IGNOREf, 0);
        SHR_IF_ERR_EXIT(soc_reg_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, reg_val64));
    }

    for (i = 0; i < nof_qrh; ++i) {
        SHR_IF_ERR_EXIT(soc_reg_get(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, &reg_val64));
        soc_reg64_field32_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, &reg_val64, LOAD_BALANCE_LEVELS_GAPf, 60);
        SHR_IF_ERR_EXIT(soc_reg_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, reg_val64));
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_multicast_config_set(int unit, soc_dnxf_multicast_mode_t mc_mode, int fe_mc_priority_map_enable)
{
    int i, nof_qrh;
    uint32 reg_val32;
    uint32 multicast_mode_field_val;
    soc_dnxf_multicast_table_mode_t multicast_mode;
    uint64 reg_val64;
    soc_reg_above_64_val_t data;
    int fap_modid_offset;
    SHR_FUNC_INIT_VARS(unit);

    for (i=0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit) ; i++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_PRIORITY_TRANSLATIONr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, LOW_PRE_MUL_SETf,
                          dnxf_data_fabric.multicast.priority_map_enable_get(unit) ? 1 : 0);
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, MID_PRE_MUL_SETf,
                          dnxf_data_fabric.multicast.priority_map_enable_get(unit) ? 1 : 0);
        SHR_IF_ERR_EXIT(WRITE_DCH_PRIORITY_TRANSLATIONr(unit, i, reg_val32));

    }
    if (dnxf_data_fabric.multicast.priority_map_enable_get(unit))
    {
        COMPILER_64_ZERO(reg_val64);
        SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_USE_MC_CELL_PRIO_BMPr(unit, reg_val64));
    }
    

    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh; i++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_MULTICAST_MODE_SELECTIONr, i, 0, &reg_val32));
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_INDIRECT_LIST_OF_FAPS_MODEf,
                                soc_dnxf_multicast_mode_indirect == mc_mode ? 1 : 0);
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_LIST_OF_FAPS_USE_MCLBTf,
                                soc_dnxf_multicast_mode_indirect == mc_mode ? 0 : 1);
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_MULTIPLE_PIPESf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_MULTICAST_MODE_SELECTIONr, i, 0, reg_val32));
    }
    
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_mode_get, (unit, &multicast_mode)));
    switch (multicast_mode)
    {
        case soc_dnxf_multicast_table_mode_64k:
            multicast_mode_field_val = 0; 
            break;
        case soc_dnxf_multicast_table_mode_128k:
            multicast_mode_field_val = 1; 
            break;
        case soc_dnxf_multicast_table_mode_128k_half:
            multicast_mode_field_val = 2; 
            break;
        case soc_dnxf_multicast_table_mode_256k:
            multicast_mode_field_val = 3 ; 
            break;
        case soc_dnxf_multicast_table_mode_256k_half:
            multicast_mode_field_val = 4 ; 
            break;
        case soc_dnxf_multicast_table_mode_512k_half:
            multicast_mode_field_val = 5 ;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong mc_table_mode value %d",
                         dnxf_data_fabric.multicast.id_range_get(unit));
    }

    SHR_IF_ERR_EXIT(READ_RTP_MULTICAST_MODE_SELECTIONr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_TABLE_MODEf, multicast_mode_field_val);
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTICAST_MODE_SELECTIONr(unit, reg_val32));

    if (soc_dnxf_multicast_mode_indirect == mc_mode)
    {
        SOC_REG_ABOVE_64_ALLONES(data);
        SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_FLGMm, MEM_BLOCK_ALL, data));
    }
    else
    {
        fap_modid_offset = 0;
        SHR_IF_ERR_EXIT(soc_ramon_fabric_multicast_direct_offset_set(unit, fap_modid_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_local_routing_set(int unit, int local_routing_enable_uc, int local_routing_enable_mc)
{
    int i, nof_qrh, nof_lcm;
    uint32 reg_val32;

    SHR_FUNC_INIT_VARS(unit);

    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh/2; i++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, i, 0, &reg_val32));
        
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_LOCAL_FE_1_ROUTINGf, local_routing_enable_uc? 0xf : 0);
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_CTRL_LOCAL_FE_1_ROUTINGf, 0x1); 
        
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_MC_LOCAL_FE_1_ROUTINGf, local_routing_enable_mc? 0x1 : 0);
        
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, MC_FORCE_LOCAL_ENf, 0x1);
        
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, MC_LOCAL_ONLYf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, i, 0, reg_val32));
    }

    SHR_IF_ERR_EXIT(READ_RTP_LOCAL_ROUTE_CONFIGURATIONSr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_REACH_LOCAL_FE_1_ROUTINGf, local_routing_enable_uc? 0x1 : 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_LOCAL_ROUTE_CONFIGURATIONSr(unit, reg_val32));

    nof_lcm = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
    for (i=0 ; i < nof_lcm/2 ; i++ )
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTML_ENABLERSr, i, 0, &reg_val32));
        soc_reg_field_set(unit, LCM_DTML_ENABLERSr, &reg_val32, LCLRT_ENf, (local_routing_enable_uc || local_routing_enable_mc)? 0x1 : 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTML_ENABLERSr, i, 0, reg_val32));
    }

    
    SHR_IF_ERR_EXIT(READ_LCM_LCM_GCI_ENABLERSr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, LCM_LCM_GCI_ENABLERSr, &reg_val32, GCI_LOCAL_MC_ENf, local_routing_enable_mc? 0x1 : 0);
    for (i=0 ; i < nof_lcm/2 ; i++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_GCI_ENABLERSr(unit, i, reg_val32));
    }

    
    SHR_IF_ERR_EXIT(READ_LCM_LCM_FC_ENABLERSr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, LCM_LCM_FC_ENABLERSr, &reg_val32, RTP_COPIES_FC_LCL_RT_MC_ENf, local_routing_enable_mc? 0x1 : 0);
    for (i=0 ; i < nof_lcm/2 ; i++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_FC_ENABLERSr(unit, i, reg_val32));
    }


exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_cell_priority_config_set(int unit, uint32 min_tdm_priority)
{
    uint32 reg32_val;
    uint32 tdm_bmp;
    int dch_instance;
    SHR_FUNC_INIT_VARS(unit);

    
    for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
    {
        tdm_bmp = 0;
        SHR_IF_ERR_EXIT(READ_DCH_TDM_PRIORITYr(unit, dch_instance, &reg32_val));
        if (min_tdm_priority == SOC_DNXF_FABRIC_TDM_PRIORITY_NONE) 
        {
            
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x0); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        } else {
            
            SHR_BITSET_RANGE(&tdm_bmp, min_tdm_priority, soc_dnxf_fabric_priority_nof - min_tdm_priority);
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x1); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        }
        SHR_IF_ERR_EXIT(WRITE_DCH_TDM_PRIORITYr(unit, dch_instance, reg32_val));
    }


exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_pipes_config_set(int unit)
{
    uint32 reg32_val;
    int rv, link;
    int dch_instance, dcml_instance, lcm_instance;
    int fmac_instance, inner_link;
    int pipe_index, prio_index;
    int cast_max, priority_max;
    uint64 reg64_val;
    soc_field_t dch_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {AUTO_DOC_NAME_18f, AUTO_DOC_NAME_20f, AUTO_DOC_NAME_22f};
    soc_field_t dch_priority_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {AUTO_DOC_NAME_19f, AUTO_DOC_NAME_21f, AUTO_DOC_NAME_23f};
    soc_field_t lcm_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_0_0f, FIELD_3_3f, FIELD_6_6f};
    soc_field_t lcm_priority_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_1_2f, FIELD_4_5f, FIELD_7_8f};
    soc_field_t dcml_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_0_0f, FIELD_1_1f, FIELD_2_2f};
    SHR_FUNC_INIT_VARS(unit);

    

    
    rv = READ_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit,&reg32_val);
    SHR_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, PIPE_1_ENf,
                      dnxf_data_fabric.pipes.nof_pipes_get(unit) > 1);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, PIPE_2_ENf,
                      dnxf_data_fabric.pipes.nof_pipes_get(unit) > 2);
    rv = WRITE_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, reg32_val);
    SHR_IF_ERR_EXIT(rv);

    
    PBMP_SFI_ITER(unit, link)
    {
        fmac_instance = link / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        inner_link = link % dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link , &reg32_val);
        SHR_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf,
                          dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1);
        rv = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, reg32_val);
        SHR_IF_ERR_EXIT(rv);
    }

      COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dch_get(unit), 0);
    }
    rv = WRITE_BRDC_DCH_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dch_get(unit), 0);
    }
    rv = WRITE_BRDC_DCH_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    
    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dcml_get(unit), 0);
    }
    rv = WRITE_BRDC_DCML_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);	
    rv = WRITE_BRDC_LCM_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dcml_get(unit), 0);
    }
    rv = WRITE_BRDC_DCML_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);	
    rv = WRITE_BRDC_LCM_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    
    
    for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
    {
        rv = READ_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, &reg32_val);
        SHR_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[3]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[3]);
        rv = WRITE_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, reg32_val);
        SHR_IF_ERR_EXIT(rv);
    }

    
    for (lcm_instance = 0; lcm_instance < dnxf_data_device.blocks.nof_instances_lcm_get(unit); lcm_instance++)
    {

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_PIPES_SEPARATION_REGISTERr, lcm_instance, 0, &reg32_val));
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[0]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[1]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[2]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[3]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[0]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[1]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[2]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[3]);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_PIPES_SEPARATION_REGISTERr, lcm_instance, 0, reg32_val));
    }

    
    for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
    {

        priority_max = cast_max = 0;

        for (prio_index=(SOC_DNXF_MAX_NUM_OF_PRIORITIES - 1); prio_index >= 0  ; prio_index--)
        {
            if (pipe_index == dnxf_data_fabric.pipes.map_get(unit)->mc[prio_index])
            {
                cast_max = 1;
                priority_max = prio_index;
                break;
            }
            else if (pipe_index == dnxf_data_fabric.pipes.map_get(unit)->uc[prio_index])
            {
                cast_max = 0;
                priority_max = prio_index;
                break;
            }
        }

        for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
        {
            rv = READ_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, &reg32_val);
            SHR_IF_ERR_EXIT(rv);
            soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, dch_cast_fields[pipe_index], cast_max);
            soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, dch_priority_fields[pipe_index], priority_max);
            rv = WRITE_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, reg32_val);
            SHR_IF_ERR_EXIT(rv);
        }

        for (lcm_instance = 0; lcm_instance < dnxf_data_device.blocks.nof_instances_lcm_get(unit); lcm_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DCML_SEPARATION_REGISTERr, lcm_instance, 0, &reg32_val));
            soc_reg_field_set(unit, LCM_DCML_SEPARATION_REGISTERr, &reg32_val, lcm_cast_fields[pipe_index], cast_max);
            soc_reg_field_set(unit, LCM_DCML_SEPARATION_REGISTERr, &reg32_val, lcm_priority_fields[pipe_index], priority_max);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DCML_SEPARATION_REGISTERr, lcm_instance, 0, reg32_val));
        }

        for (dcml_instance = 0; dcml_instance < dnxf_data_device.blocks.nof_instances_dcml_get(unit); dcml_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCML_DCML_SEPARATION_REGISTERr, dcml_instance, 0, &reg32_val));
            soc_reg_field_set(unit, DCML_DCML_SEPARATION_REGISTERr, &reg32_val, dcml_cast_fields[pipe_index], cast_max);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCML_DCML_SEPARATION_REGISTERr, dcml_instance, 0, reg32_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_operation_mode(int unit)
{
    int rv;
    int local_routing_enable_uc = 0, local_routing_enable_mc = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    rv = _soc_ramon_drv_fabric_device_mode_set(unit, dnxf_data_fabric.general.device_mode_get(unit));
    SHR_IF_ERR_EXIT(rv);

     
    rv = _soc_ramon_drv_fabric_cell_priority_config_set(unit, dnxf_data_fabric.pipes.system_tdm_priority_get(unit));
    SHR_IF_ERR_EXIT(rv);

    
    rv = _soc_ramon_drv_fabric_pipes_config_set(unit);
    SHR_IF_ERR_EXIT(rv);

    
    rv = _soc_ramon_drv_fabric_multicast_config_set(unit, dnxf_data_fabric.multicast.mode_get(unit),
                                                    dnxf_data_fabric.multicast.priority_map_enable_get(unit));
    SHR_IF_ERR_EXIT(rv);

    
    rv = _soc_ramon_drv_fabric_load_balancing_set(unit, dnxf_data_fabric.topology.load_balancing_mode_get(unit));
    SHR_IF_ERR_EXIT(rv);

    if (SOC_DNXF_IS_FE13(unit))
    {
        local_routing_enable_uc = dnxf_data_fabric.general.local_routing_enable_uc_get(unit);
        local_routing_enable_mc = dnxf_data_fabric.general.local_routing_enable_mc_get(unit);
        rv = _soc_ramon_drv_fabric_local_routing_set(unit, local_routing_enable_uc, local_routing_enable_mc);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

#define _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS                  (96)

#define _SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH                        (64)
#define _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH                         (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE                   (_SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T2_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH                         (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS + _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH - 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MAX_TOTAL_LINKS               (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS)

#define _SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH                         (64)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS               (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_TOTAL_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE                          (_SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH * _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T4_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH                         (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS + _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T5_LENGTH                        (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T5_WIDTH                         (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T5_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T4_MAX_ACTIVE_LINKS + 1)


#define _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY             (4)

#define _SOC_RAMON_MULTIPLIER_TABLE_DUMP 0


STATIC int
soc_ramon_clean_table_slow(int unit, soc_mem_t mem, soc_reg_above_64_val_t data)
{
    int index, index_max;
    SHR_FUNC_INIT_VARS(unit);

    index_max = soc_mem_index_max(unit, mem);

    for (index = 0; index <= index_max; index++) {
        SHR_IF_ERR_EXIT(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_reset_tables(int unit)
{
    soc_reg_above_64_val_t data;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 total_links, active_links, score, entry;
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val, slsct_val, score_slsct, links_count, sctinc_val, sctinc;
    soc_field_t scrub_en;
    soc_field_t calc_result_field_arr[_SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY] = {CALC_RESULT_0f, CALC_RESULT_1f, CALC_RESULT_2f, CALC_RESULT_3f};
    int link;
    uint64 data64;
    int tbl_index = 0;
    int field_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    scrub_en = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    SOC_REG_ABOVE_64_CLEAR(data);
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, QRH_MCLBTm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, QRH_MNOLm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, QRH_FFLBm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, QRH_MCSFFm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_RCGLBTm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_TOTSFm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_SLSCTm, MEM_BLOCK_ALL, data));
    
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, RTP_SCTINCm, MEM_BLOCK_ALL, data));

    
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_MNOLGm, data));
    
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_FLGMm, data));
    
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_GCILBTm, data));

    
    totsf_val = 0;
    links_count = 1;
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32*)&links_count);

    
    slsct_val = 0;
    score_slsct = 0;
    soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, SCORE_OF_LINKf, &score_slsct);

    
    sctinc_val = 0;
    sctinc = 0;
    soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, SCORE_TO_INCf, &sctinc);
    
    BCM_PBMP_ITER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, link)
    {
        
        if (link > dnxf_data_port.general.nof_links_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,"Link provided %d is bigger than the NOF links in the device - %d", link, dnxf_data_port.general.nof_links_get(unit));
        }

        
        
        
        sal_memset(bmp, 0, sizeof(bmp));
        SHR_BITSET(bmp,link);

        SHR_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
        SHR_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
        SHR_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
        SHR_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
    }

    
    

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
    LOG_CLI((BSL_META_U(unit,
                        "MUL TABLE\n\n")));

    LOG_CLI((BSL_META_U(unit,
                        "%02d || "), 0));
    for(active_links = 1 ; active_links <= _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS ; active_links++) {
        LOG_CLI((BSL_META_U(unit,
                            "%04d | "), active_links));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\n")));
#endif
    for(total_links = 1 ; total_links <= _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS ; total_links++) {

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "%02d || "), total_links));
#endif
        for(active_links = 1 ; active_links <= total_links ; active_links++) {
            
            score = UTILEX_DIV_ROUND_UP((SOC_DNXF_DRV_MULTIPLIER_MAX_LINK_SCORE * active_links), total_links);

            
            
            if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T5_MIN_ACTIVE_LINKS)
            {
                
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE
                    + ((_SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS - total_links) * _SOC_RAMON_DRV_MULTIPLIER_T5_WIDTH)
                    + (_SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS - active_links);
            }
            
            else if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS)
            {
                
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE
                    + ((total_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T4_LENGTH) * _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH
                    + ((active_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH);
            }
            
            else if (total_links >= _SOC_RAMON_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS)
            {
                
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + ((total_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH) * _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH
                    + (active_links - 1);
            }
            
            else if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS)
            {
                
                entry = (_SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH - total_links) * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH
                    + (_SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH  + _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH - active_links);
            }
            
            else
            {
                
                entry = (total_links - 1) * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH + (active_links - 1);
            }

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
            LOG_CLI((BSL_META_U(unit,
                                "%04d | "), entry));
#endif
            COMPILER_64_ZERO(data64);
            tbl_index = entry / _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY;
            field_offset = entry % _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY;

            SHR_IF_ERR_EXIT(READ_RTP_MULTI_TBm(unit, MEM_BLOCK_ANY, tbl_index, &data64));
            soc_mem_field32_set(unit, RTP_MULTI_TBm, &data64, calc_result_field_arr[field_offset], score);
            SHR_IF_ERR_EXIT(WRITE_RTP_MULTI_TBm(unit, MEM_BLOCK_ALL, tbl_index, &data64));
        }
#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
#endif
    }
    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, scrub_en);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_fmac_config(int unit)
{
    uint32 reg_val32, field[1];
    uint64  reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    int i;
    int link, blk, inner_link;
    int fmac_index;
    soc_dnxf_fabric_link_device_mode_t link_mode;
    int nof_links_in_fmac;
    int nof_instances_fmac;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_fmac = dnxf_data_device.blocks.nof_instances_fmac_get(unit);
    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, 0, 0, &reg_val32));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_FILL_RATE_Nf,
                      dnxf_data_port.general.mac_bucket_fill_rate_get(unit));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_UP_TH_Nf, 0x20);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_DN_TH_Nf, 0x10);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, SIG_DET_BKT_RST_ENA_Nf, 0x1);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, ALIGN_LCK_BKT_RST_ENA_Nf, 0x1);
    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, BRDC_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, REG_PORT_ANY, i, reg_val32));
    }

    
    if (SOC_DNXF_IS_FE13(unit))
    {
       PBMP_SFI_ITER(unit, link)
       {
           SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, link, 0, &link_mode)));
           SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

           SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, &reg_val32));
           soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
           if (link_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe1)
           {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE1);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE1);
           } else {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3);
           }
           SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, reg_val32));
       }
    } else {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 0, 0, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE2);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE2);

        for (i = 0 ; i < nof_links_in_fmac ; i++)
        {
            SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, i, reg_val32));
        }

    }
    
    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val64));
        soc_reg64_field_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg_val64 ,AUTO_DOC_NAME_59f, SOC_RAMON_DRV_ASYNC_FIFO_AUTO_DOC_NAME_59);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, i, reg_val64));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, i, reg_val64));
            }
        }
    }

    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, REG_PORT_ANY, i, reg_above64_val));
        soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above64_val, FMAL_N_RX_RETIMER_RS_FEC_FEEDBACK_PROCESSOR_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_RETIMERr(unit, i, reg_above64_val));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_index, i, reg_above64_val));
            }
        }
    }

    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CNTRL_BURST_PERIODf, SOC_RAMON_DRV_BURST_PERIOD);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, i, reg_val32));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, fmac_index, i, reg_val32));
            }
        }
    }

    if(SOC_DNXF_IS_FE13(unit)) {

        PBMP_SFI_ITER(unit, link)
        {

            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, link, 0, &link_mode)));
            if (link_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe1)
            {
                SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

                SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, &reg_val32));
                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_0f);
                SHR_BITSET(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_0f, *field);

                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_1f);
                SHR_BITCLR(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_1f, *field);

                SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, reg_val32));
            }
        }
    }

    
    reg_val32 = 0;
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_RX_ENf, 0xf);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_TX_ENf, 0xf);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    
    for(i=0; i<nof_links_in_fmac; i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

    
    for(i=0; i<nof_links_in_fmac; i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val64));
        soc_reg64_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg_val64 ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, i, reg_val64));
    }

    
    
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_AUTO_DOC_NAME_12r(unit, 0xf));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_fsrd_config(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

        
        SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_ccs_config(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_rtp_config(int unit)
{
    uint32 reg_val32;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 core_clock_speed;
    uint32 rtpwp;
    uint32 gpd_rmgr_global_time = 0;
    uint32 gpd_rmgr_global_in_clocks = 0;
    uint32 gpd_rmgr_global_in_units_clocks = 0;
    uint32 gpd_protect_cnt_th = 0;
    uint32 gpd_protect_cnt_th_in_clocks = 0;
    uint32 gpd_cnt_th = 0;
    uint32 gpd_cnt_th_in_clocks = 0;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);

    core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);

     
    rtpwp = ((dnxf_data_fabric.reachability.watchdog_rate_get(unit)/ 1000) * core_clock_speed ) / 1000 ;

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr, &reg_val64, RTPWPf, rtpwp);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, reg_val64));

    if (SOC_DNXF_IS_FE13(unit)){
        SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
        soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_ENABLE_SLOW_LINK_DOWNf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    }

    SHR_IF_ERR_EXIT(soc_ramon_stk_module_max_fap_set(unit, SOC_RAMON_STK_MAX_MODULE));

    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf, 0);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGR_GLOBALf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));

    
    COMPILER_64_ZERO(reg_val64);

    
    gpd_rmgr_global_time = dnxf_data_fabric.reachability.gpd_gen_rate_full_cycle_get(unit);
    gpd_rmgr_global_in_clocks = ((gpd_rmgr_global_time / 100) * core_clock_speed ) / 10000;
    gpd_rmgr_global_in_units_clocks = gpd_rmgr_global_in_clocks / dnxf_data_fabric.reachability.rmgr_units_get(unit);

    
    gpd_protect_cnt_th = dnxf_data_fabric.gpd.in_time_get(unit);
    gpd_protect_cnt_th_in_clocks = (gpd_protect_cnt_th * core_clock_speed) / 1000;

    
    gpd_cnt_th = dnxf_data_fabric.gpd.out_time_get(unit);
    gpd_cnt_th_in_clocks = gpd_cnt_th * core_clock_speed;

    SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_RMGRf, gpd_rmgr_global_in_units_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_CNT_THf, gpd_protect_cnt_th_in_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_CNT_THf, gpd_cnt_th_in_clocks);
    SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

exit:
    SHR_FUNC_EXIT;
}

#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE3_DEF          (0x1200)
#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_MULTI_FE2_DEF    (0x800)
#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF      (0x400)


STATIC int
soc_ramon_drv_dch_fifo_clear(int unit)
{
    uint32 nof_profiles;
    int    profile_index;

    SHR_FUNC_INIT_VARS(unit);
    nof_profiles = dnxf_data_fabric.congestion.nof_profiles_get(unit);

    for (profile_index = 0; profile_index < nof_profiles; profile_index++)
    {
        SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_FIFOS_DEPTH_TYPEr(unit, profile_index, 0));
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dch_fifo_block_pipe_size_config(int unit, int block_index, int pipe_index, int fifo_size)
{
    uint32 reg_val32;
    soc_field_t depth_fields[]      = { FIFO_DEPTH_P_0_TYPE_Nf, FIFO_DEPTH_P_1_TYPE_Nf, FIFO_DEPTH_P_2_TYPE_Nf};
    uint32 nof_profiles;
    int    profile_index;

    SHR_FUNC_INIT_VARS(unit);
    nof_profiles = dnxf_data_fabric.congestion.nof_profiles_get(unit);

    for (profile_index = 0; profile_index < nof_profiles; profile_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, &reg_val32));
        soc_reg_field_set(unit, DCH_FIFOS_DEPTH_TYPEr, &reg_val32, depth_fields[pipe_index], fifo_size);
        SHR_IF_ERR_EXIT(WRITE_DCH_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, reg_val32));
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dch_fifo_size_config(int unit)
{
    uint32 nof_blocks;
    uint32 nof_pipes;
    int    fifo_size;
    int    block_index;
    int    pipe_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_blocks    = dnxf_data_device.blocks.nof_instances_dch_get(unit);
    nof_pipes     = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    
    SHR_IF_ERR_EXIT(soc_ramon_drv_dch_fifo_clear(unit));

    
    for (block_index = 0 ; block_index < (nof_blocks/2) ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            fifo_size = dnxf_data_fabric.fifos.rx_depth_per_pipe_get(unit, pipe_index)->fe1;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dch_fifo_block_pipe_size_config(unit, block_index, pipe_index, fifo_size));
       }
    }

    
    for (block_index = (nof_blocks/2) ; block_index < nof_blocks ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            fifo_size = dnxf_data_fabric.fifos.rx_depth_per_pipe_get(unit, pipe_index)->fe3;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dch_fifo_block_pipe_size_config(unit, block_index, pipe_index, fifo_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtm_fifo_clear(int unit)
{
    uint64 reg_val64;
    uint32 nof_profiles;
    int    profile_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_profiles = dnxf_data_fabric.congestion.nof_profiles_get(unit);

    COMPILER_64_ZERO(reg_val64);
    for (profile_index = 0; profile_index < nof_profiles; profile_index++)
    {
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DTM_FIFOS_DEPTH_TYPEr(unit, profile_index, 0));
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_REGISTER_5r(unit, profile_index, reg_val64));
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtm_fifo_block_pipe_size_config(int unit, int block_index, int pipe_index, int nlr_fifo_size, int lr_fifo_size)
{
    uint32 reg_val32;
    uint64 reg_val64;
    int full_value;
    soc_field_t depth_fields[] = { DTM_FIFO_DEPTH_P_0_TYPE_Nf, DTM_FIFO_DEPTH_P_1_TYPE_Nf,   INVALIDf};
    soc_field_t full_fields[] =  { FIELD_0_12f,                FIELD_16_28f,                 FIELD_32_44f };

    SHR_FUNC_INIT_VARS(unit);

    
    full_value = (nlr_fifo_size == 0) ? 0 : (nlr_fifo_size - dnxf_data_fabric.fifos.mid_full_offset_get(unit));

    if (depth_fields[pipe_index] != INVALIDf)
    {
        SHR_IF_ERR_EXIT(READ_LCM_DTM_FIFOS_DEPTH_TYPEr(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_NLR, &reg_val32));
        soc_reg_field_set(unit, LCM_DTM_FIFOS_DEPTH_TYPEr, &reg_val32, depth_fields[pipe_index], nlr_fifo_size);
        SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFOS_DEPTH_TYPEr(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_NLR, reg_val32));
    }

    SHR_IF_ERR_EXIT(READ_LCM_LCM_REGISTER_5r(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_NLR, &reg_val64));
    soc_reg64_field_set(unit, LCM_LCM_REGISTER_5r, &reg_val64, full_fields[pipe_index], full_value);
    SHR_IF_ERR_EXIT(WRITE_LCM_LCM_REGISTER_5r(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_NLR, reg_val64));

    
    full_value = (lr_fifo_size == 0) ? 0 : (lr_fifo_size - dnxf_data_fabric.fifos.mid_full_offset_get(unit));

    if (depth_fields[pipe_index] != INVALIDf)
    {
        SHR_IF_ERR_EXIT(READ_LCM_DTM_FIFOS_DEPTH_TYPEr(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_LR, &reg_val32));
        soc_reg_field_set(unit, LCM_DTM_FIFOS_DEPTH_TYPEr, &reg_val32, depth_fields[pipe_index], lr_fifo_size);
        SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFOS_DEPTH_TYPEr(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_LR, reg_val32));
    }

    SHR_IF_ERR_EXIT(READ_LCM_LCM_REGISTER_5r(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_LR, &reg_val64));
    soc_reg64_field_set(unit, LCM_LCM_REGISTER_5r, &reg_val64, full_fields[pipe_index], full_value);
    SHR_IF_ERR_EXIT(WRITE_LCM_LCM_REGISTER_5r(unit, block_index, SOC_RAMON_DRV_DTM_FIFO_PROFILE_LR, reg_val64));

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtm_fifo_size_config(int unit)
{
    uint32 nof_blocks;
    uint32 nof_pipes;
    int  nlr_fifo_size, lr_fifo_size;
    int  block_index;
    int  pipe_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_blocks   = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
    nof_pipes    = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    SHR_IF_ERR_EXIT(soc_ramon_drv_dtm_fifo_clear(unit));

    
    for (block_index=0 ; block_index < (nof_blocks/2) ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            nlr_fifo_size = dnxf_data_fabric.fifos.mid_depth_per_pipe_get(unit, pipe_index)->fe3_nlr;
            lr_fifo_size  = dnxf_data_fabric.fifos.mid_depth_per_pipe_get(unit, pipe_index)->fe3_lr;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dtm_fifo_block_pipe_size_config(unit, block_index, pipe_index, nlr_fifo_size, lr_fifo_size));
        }
    }

    
    for (block_index=(nof_blocks/2) ; block_index < nof_blocks ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            nlr_fifo_size = dnxf_data_fabric.fifos.mid_depth_per_pipe_get(unit, pipe_index)->fe1_nlr;
            lr_fifo_size  = dnxf_data_fabric.fifos.mid_depth_per_pipe_get(unit, pipe_index)->fe1_lr;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dtm_fifo_block_pipe_size_config(unit, block_index, pipe_index, nlr_fifo_size, lr_fifo_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtm_fc_aging_counters_config(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DTM_MASK_FC_AGING_PERIODr(unit, SOC_RAMON_DRV_FC_AGING_COUNTER_PERIOD));

    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_MASK_FC_AGING_PERIODr(unit, SOC_RAMON_DRV_FC_AGING_COUNTER_PERIOD));

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtl_fifo_clear(int unit)
{
    uint64 reg_val64;
    uint32 nof_profiles;
    int    profile_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_profiles = dnxf_data_fabric.congestion.nof_profiles_get(unit);

    COMPILER_64_ZERO(reg_val64);
    for (profile_index = 0; profile_index < nof_profiles; profile_index++)
    {
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DTL_FIFOS_DEPTH_TYPEr(unit, profile_index, reg_val64));
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_REGISTER_6r(unit, profile_index, reg_val64));
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCLRT_FIFOS_DEPTH_TYPEr(unit, profile_index, reg_val64));
        SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_REGISTER_7r(unit, profile_index, reg_val64));
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtl_fifo_block_pipe_size_config(int unit, int block_index, int pipe_index, int nlr_fifo_size, int lr_fifo_size)
{
    uint64 reg_val64;
    uint32 nof_profiles;
    int  full_value;
    int  profile_index;
    soc_field_t depth_fields[]             = { DTL_FIFO_DEPTH_P_0_TYPE_Nf,       DTL_FIFO_DEPTH_P_1_TYPE_Nf,       DTL_FIFO_DEPTH_P_2_TYPE_Nf };
    soc_field_t full_fields[]              = { FIELD_0_12f,                      FIELD_13_25f,                     FIELD_26_38f };
    soc_field_t local_route_depth_fields[] = { DTL_LCLRT_FIFO_DEPTH_P_0_TYPE_Nf, DTL_LCLRT_FIFO_DEPTH_P_1_TYPE_Nf, DTL_LCLRT_FIFO_DEPTH_P_2_TYPE_Nf };
    soc_field_t local_route_full_fields[]  = { FIELD_0_12f,                      FIELD_13_25f,                     FIELD_26_38f };

    SHR_FUNC_INIT_VARS(unit);

    nof_profiles = dnxf_data_fabric.congestion.nof_profiles_get(unit);

    for (profile_index = 0; profile_index < nof_profiles; profile_index++)
    {
        
        full_value = (nlr_fifo_size == 0) ? 0 : (nlr_fifo_size - dnxf_data_fabric.fifos.tx_full_offset_get(unit));

        SHR_IF_ERR_EXIT(READ_LCM_DTL_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, &reg_val64));
        soc_reg64_field_set(unit, LCM_DTL_FIFOS_DEPTH_TYPEr, &reg_val64, depth_fields[pipe_index], nlr_fifo_size);
        SHR_IF_ERR_EXIT(WRITE_LCM_DTL_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_LCM_LCM_REGISTER_6r(unit, block_index, profile_index, &reg_val64));
        soc_reg64_field_set(unit, LCM_LCM_REGISTER_6r, &reg_val64, full_fields[pipe_index], full_value);
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_REGISTER_6r(unit, block_index, profile_index, reg_val64));

        
        full_value = (lr_fifo_size == 0) ? 0 : (lr_fifo_size - dnxf_data_fabric.fifos.tx_full_offset_get(unit));

        SHR_IF_ERR_EXIT(READ_LCM_LCLRT_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, &reg_val64));
        soc_reg64_field_set(unit, LCM_LCLRT_FIFOS_DEPTH_TYPEr, &reg_val64, local_route_depth_fields[pipe_index], lr_fifo_size);
        SHR_IF_ERR_EXIT(WRITE_LCM_LCLRT_FIFOS_DEPTH_TYPEr(unit, block_index, profile_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_LCM_LCM_REGISTER_7r(unit, block_index, profile_index, &reg_val64));
        soc_reg64_field_set(unit, LCM_LCM_REGISTER_7r, &reg_val64, local_route_full_fields[pipe_index], full_value);
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_REGISTER_7r(unit, block_index, profile_index, reg_val64));

    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_drv_dtl_fifo_size_config(int unit)
{
    uint32 nof_blocks;
    uint32 nof_pipes;
    int  nlr_fifo_size, lr_fifo_size;
    int  block_index;
    int  pipe_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_blocks    = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
    nof_pipes     = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    
    SHR_IF_ERR_EXIT(soc_ramon_drv_dtl_fifo_clear(unit));

    
    for (block_index=0 ; block_index < (nof_blocks/2) ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            nlr_fifo_size = dnxf_data_fabric.fifos.tx_depth_per_pipe_get(unit, pipe_index)->fe3_nlr;
            lr_fifo_size  = dnxf_data_fabric.fifos.tx_depth_per_pipe_get(unit, pipe_index)->fe3_lr;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dtl_fifo_block_pipe_size_config(unit, block_index, pipe_index, nlr_fifo_size, lr_fifo_size));
        }
    }

    
    for (block_index = (nof_blocks/2) ; block_index < nof_blocks ; block_index++)
    {
        for (pipe_index = 0; pipe_index < nof_pipes; pipe_index++)
        {
            nlr_fifo_size = dnxf_data_fabric.fifos.tx_depth_per_pipe_get(unit, pipe_index)->fe1_nlr;
            lr_fifo_size  = dnxf_data_fabric.fifos.tx_depth_per_pipe_get(unit, pipe_index)->fe1_lr;
            SHR_IF_ERR_EXIT(soc_ramon_drv_dtl_fifo_block_pipe_size_config(unit, block_index, pipe_index, nlr_fifo_size, lr_fifo_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_dch_config(int unit)
{
    uint64 reg_val64;
    uint32 reg_val32;
    int  dch_index, nof_instances_dch;
    int  pipe_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_instances_dch = dnxf_data_device.blocks.nof_instances_dch_get(unit);

    
    for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
    {
        for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++) {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_3_Pr(unit, dch_index, pipe_index, &reg_val32));
            soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_3_Pr, &reg_val32, AUTO_DOC_NAME_12f, 0Xfd);
            SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_3_Pr(unit, dch_index, pipe_index, reg_val32));
        }
    }


    
    for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, &reg_val64, AUTO_DOC_NAME_26f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, dch_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, &reg_val64, AUTO_DOC_NAME_27f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, dch_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, &reg_val64, AUTO_DOC_NAME_28f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, dch_index, reg_val64));
    }
   
    
    for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_PIPES_WEIGHTS_REGISTERr(unit, dch_index, &reg_val32));
        soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_RST_CFGf, _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SHR_IF_ERR_EXIT(WRITE_DCH_PIPES_WEIGHTS_REGISTERr(unit, dch_index, reg_val32));
    }

     
    if (SOC_DNXF_IS_MULTISTAGE_FE2(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            for (dch_index = 0; dch_index < nof_instances_dch; dch_index++)
            {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_MULTI_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }
    }
    else if (SOC_DNXF_IS_SINGLESTAGE_FE2(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            for (dch_index = 0; dch_index < nof_instances_dch; dch_index++)
            {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }
    }
    else if (SOC_DNXF_IS_FE13(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            
            for (dch_index=0 ; dch_index < nof_instances_dch/2; dch_index++) {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }

            
            for (dch_index=nof_instances_dch/2 ; dch_index < nof_instances_dch; dch_index++) {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE3_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }

    }

    soc_ramon_drv_dch_fifo_size_config(unit);
           
exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_dcml_config(int unit)
{
    int dcml_index, nof_instances_dcml, field_index = 0;
    uint32 reg_val32, dcml_link_mask = 0;
    soc_reg_above_64_val_t reg_above64_val;
    soc_field_t bank_write_algorith_reg_fields[] = {
            AUTO_DOC_NAME_2f, AUTO_DOC_NAME_3f, AUTO_DOC_NAME_4f, AUTO_DOC_NAME_5f, AUTO_DOC_NAME_6f, AUTO_DOC_NAME_7f,
            AUTO_DOC_NAME_17f, AUTO_DOC_NAME_18f, AUTO_DOC_NAME_19f, AUTO_DOC_NAME_20f, AUTO_DOC_NAME_21f, AUTO_DOC_NAME_22f};

    SHR_FUNC_INIT_VARS(unit);

    nof_instances_dcml = dnxf_data_device.blocks.nof_instances_dcml_get(unit);

    
    for (dcml_index=0 ; dcml_index < nof_instances_dcml ; dcml_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTL_PIPES_WEIGHTS_REGISTERr, dcml_index, 0, &reg_val32));
        soc_reg_field_set(unit, LCM_DTL_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_RST_CFGf, _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTL_PIPES_WEIGHTS_REGISTERr, dcml_index, 0, reg_val32));
    }

    
    SHR_BITSET_RANGE(&dcml_link_mask, 0, dnxf_data_device.blocks.nof_links_in_dcml_get(unit));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_CPU_FORCE_MAC_LINK_IREADYr(unit, dcml_link_mask));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DISABLE_MACr(unit, dcml_link_mask));

    for (dcml_index=0 ; dcml_index < nof_instances_dcml ; dcml_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCML_AUTO_DOC_NAME_1r(unit, dcml_index, reg_above64_val));
        soc_reg_above_64_field32_set(unit, DCML_AUTO_DOC_NAME_1r, reg_above64_val, AUTO_DOC_NAME_33f, SOC_RAMON_DRV_DFL_AUTO_DOC_NAME_33);

        
        for (field_index = 0 ; field_index < SOC_RAMON_DRV_DCML_NOF_BANK_WRITE_ALGORITHM_REG_FIELDS; field_index++)
        {
            soc_reg_above_64_field32_set(unit, DCML_AUTO_DOC_NAME_1r, reg_above64_val, bank_write_algorith_reg_fields[field_index], SOC_RAMON_DRV_DCML_BANK_WRITE_ALGORITHM_VALUE);
        }

        SHR_IF_ERR_EXIT(WRITE_DCML_AUTO_DOC_NAME_1r(unit, dcml_index, reg_above64_val));
    }

    soc_ramon_drv_dtm_fifo_size_config(unit);
    soc_ramon_drv_dtl_fifo_size_config(unit);
    
    soc_ramon_drv_dtm_fc_aging_counters_config(unit);

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_lcm_config(int unit)
{
    int block_idx, nof_instances_lcm;
    uint32 reg_val32;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 pipes_uc, pipes_mc;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_lcm = dnxf_data_device.blocks.nof_instances_lcm_get(unit);

    
    
    

    
    pipes_uc = 0;
    pipes_mc = 0;

    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 1)
    {
        SHR_BITSET(&pipes_uc, 0);
        SHR_BITSET(&pipes_mc, 0);
    }
    else
    {
        switch (dnxf_data_fabric.pipes.map_get(unit)->type)
        {
        case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 1);
            break;
        case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET_RANGE(&pipes_mc, 1, 2);
            break;
        case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 0);
            break;
        case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 1);
            break;
        case soc_dnxc_fabric_pipe_map_single:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 0);
            break;
        default:
            break;
        };
    }

    for (block_idx=0 ; block_idx < nof_instances_lcm ; block_idx++)
    {
        SHR_IF_ERR_EXIT(READ_LCM_LCM_RCI_ENABLERSr(unit, block_idx, &reg_val64));
        soc_reg64_field_set(unit, LCM_LCM_RCI_ENABLERSr, &reg_val64, PIPE_RCI_ENf, pipes_uc);
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_RCI_ENABLERSr(unit, block_idx, reg_val64));

        SHR_IF_ERR_EXIT(READ_LCM_LCM_GCI_ENABLERSr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LCM_GCI_ENABLERSr, &reg_val32, PIPE_GCI_ENf, pipes_mc);
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_GCI_ENABLERSr(unit, block_idx, reg_val32));
    }

    
    
    

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    for (block_idx=0 ; block_idx < nof_instances_lcm ; block_idx++)
    {
         SHR_IF_ERR_EXIT(WRITE_LCM_DFL_GCI_TH_METHOD_2r(unit, block_idx, reg_above64_val));
         SHR_IF_ERR_EXIT(WRITE_LCM_DFL_RCI_TH_METHOD_2r(unit, block_idx, reg_above64_val));
    }

    
    
    


    if(SOC_DNXF_IS_FE13(unit)) {

        

        for (block_idx=0 ; block_idx < nof_instances_lcm/2 ; block_idx++)
        {
            SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFO_TYPE_BMPr(unit, block_idx, SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_LOW));
        }

        for (block_idx=nof_instances_lcm/2 ; block_idx < nof_instances_lcm ; block_idx++)
        {
            SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFO_TYPE_BMPr(unit, block_idx, SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_HIGH));
        }
    }


    
    
    

    for (block_idx=0 ; block_idx < nof_instances_lcm ; block_idx++)
    {
        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_COUNT_CFGr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_COUNT_CFGr, &reg_val32, LINK_LOAD_SAT_VALf, SOC_RAMON_DRV_LINK_LOAD_SAT_VAL);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_COUNT_CFGr(unit, block_idx, reg_val32));

        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_HYST_TYPEr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_TYPEr, &reg_val32, LINK_LOAD_HYST_TYPEf, SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_BYTE);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_HYST_TYPEr(unit, block_idx, reg_val32));

        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_HYST_THr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_THr, &reg_val32, LINK_LOAD_HYST_LOW_THf, SOC_RAMON_DRV_LINK_LOAD_HYST_LOW_TH);
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_THr, &reg_val32, LINK_LOAD_HYST_HIGH_THf, SOC_RAMON_DRV_LINK_LOAD_HYST_HIGH_TH);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_HYST_THr(unit, block_idx, reg_val32));
    }

    exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_eci_config(int unit)
{
    uint32 reg32_val;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    
    

    rv = READ_ECI_FAP_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val);
    SHR_IF_ERR_EXIT(rv);
    if (dnxf_data_fabric.pipes.system_contains_multiple_pipe_device_get(unit)) {
        soc_reg_field_set(unit, ECI_FAP_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, 2);
    } else {
        soc_reg_field_set(unit, ECI_FAP_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, 0);
    }
    rv = WRITE_ECI_FAP_GLOBAL_GENERAL_CFG_1r(unit, reg32_val);
    SHR_IF_ERR_EXIT(rv);
    
    
    SHR_IF_ERR_EXIT(soc_ramon_drv_pvt_monitor_enable(unit));

exit:
    SHR_FUNC_EXIT;
}

int
soc_ramon_reset_device(int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    
    rc =soc_ramon_reset_cmic_iproc_regs(unit); 
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_pll_config_set(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_blocks_reset(unit, 0  , NULL);
    SHR_IF_ERR_EXIT(rc);

    
    if (dnxf_data_device.access.bist_enable_get(unit))
    {
        rc = soc_bist_all_ramon(unit, dnxf_data_device.access.bist_enable_get(unit) - 1);
        SHR_IF_ERR_EXIT(rc);
        test_was_run_and_suceeded[unit/32] |= ((uint32)1) << (unit % 32);
        

        
        SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));

        
        rc =soc_ramon_reset_cmic_iproc_regs(unit); 
        SHR_IF_ERR_EXIT(rc);

        
        rc = soc_ramon_drv_pll_config_set(unit);
        SHR_IF_ERR_EXIT(rc);
        
        
        rc =soc_ramon_drv_blocks_reset(unit, 0  , NULL);
        SHR_IF_ERR_EXIT(rc);
    }


    
    rc = soc_ramon_drv_sbus_broadcast_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_dynamic_mem_access_enable(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_soft_init(unit, SOC_DNXC_RESET_ACTION_IN_RESET);
    SHR_IF_ERR_EXIT(rc);


    
    rc = soc_ramon_reset_tables(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_operation_mode(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_fmac_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_fsrd_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_ccs_config(unit);
    SHR_IF_ERR_EXIT(rc); 

    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_set_mesh_topology_config ,(unit));
    SHR_IF_ERR_EXIT(rc);    

    
    rc = soc_ramon_set_rtp_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_dch_config(unit);
    SHR_IF_ERR_EXIT(rc);    

    
    rc = soc_ramon_set_dcml_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_lcm_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_set_eci_config(unit);
    SHR_IF_ERR_EXIT(rc);

         
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_set,(unit, soc_dnxf_fabric_priority_0));
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_soft_init(unit, SOC_DNXC_RESET_ACTION_OUT_RESET);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_dnxc_interrupts_init(unit);
    SHR_IF_ERR_EXIT(rc);
    
    rc = soc_dnxc_ser_init(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_drv_pll_config_set(int unit)
{

    soc_reg_above_64_val_t puc;
    uint32 puc_lcpll_in_sel=0, is_def_core_clock_ovrd = 0, core_clock_pll_n_divider = 0, core_clock_pll_m_divider = 0;
    uint32 pll_status_val32, pll_locked;
    int lcpll, config_index;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t pll_config_above_64_val;
    int lcpll_in, lcpll_out;
    soc_reg_t   pll_config_reg[]        = {ECI_MISC_PLL_0_CONFIGr,   ECI_MISC_PLL_1_CONFIGr,   ECI_MISC_PLL_2_CONFIGr,   ECI_MISC_PLL_3_CONFIGr};
    soc_field_t pll_config_field[]      = {MISC_PLL_0_CONFIGf,       MISC_PLL_1_CONFIGf,       MISC_PLL_2_CONFIGf,       MISC_PLL_3_CONFIGf};
    soc_field_t pll_reset_field[]       = {MISC_PLL_0_RESET_Bf,      MISC_PLL_1_RESET_Bf,      MISC_PLL_2_RESET_Bf,      MISC_PLL_3_RESET_Bf};
    soc_field_t pll_post_reset_field[]  = {MISC_PLL_0_POST_RESET_Bf, MISC_PLL_1_POST_RESET_Bf, MISC_PLL_2_POST_RESET_Bf, MISC_PLL_3_POST_RESET_Bf};
    soc_field_t pll_sw_ovrd_field[]     = {MISC_PLL_0_SW_OVERRIDEf,  MISC_PLL_1_SW_OVERRIDEf,  MISC_PLL_2_SW_OVERRIDEf,  MISC_PLL_3_SW_OVERRIDEf};

    soc_reg_t   pll_stat_reg[]          = {ECI_MISC_PLL_0_STATUSr,   ECI_MISC_PLL_1_STATUSr,   ECI_MISC_PLL_2_STATUSr,   ECI_MISC_PLL_3_STATUSr};
    soc_field_t pll_locked_field[]      = {MISC_PLL_0_LOCKEDf,       MISC_PLL_1_LOCKEDf,       MISC_PLL_2_LOCKEDf,       MISC_PLL_3_LOCKEDf};

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT( soc_reg_above_64_get(unit, ECI_POWERUP_CONFIGr, REG_PORT_ANY, 0, puc) );
    SHR_BITCOPY_RANGE(&puc_lcpll_in_sel, 0, puc, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL_NOF_BITS);

     
    SHR_BITCOPY_RANGE(&is_def_core_clock_ovrd, 0, puc, SOC_RAMON_PORT_PUC_DEF_CORE_CLOCK_OVRD_SEL, SOC_RAMON_PORT_PUC_DEF_CORE_CLOCK_OVRD_NOF_BITS);
    if (is_def_core_clock_ovrd)
    {
        SHR_BITCOPY_RANGE(&core_clock_pll_n_divider, 0, puc, SOC_RAMON_PORT_PUC_CORE_PLL_N_DIVIDER_SEL, SOC_RAMON_PORT_PUC_CORE_PLL_N_DIVIDER_NOF_BITS);
        SHR_BITCOPY_RANGE(&core_clock_pll_m_divider, 0, puc, SOC_RAMON_PORT_PUC_CORE_PLL_M_DIVIDER_SEL, SOC_RAMON_PORT_PUC_CORE_PLL_M_DIVIDER_NOF_BITS);
        SHR_ERR_EXIT(_SHR_E_CONFIG,"Illegal core clock provided to the device! Core Clock N divider %u, Core clock M divider %u", core_clock_pll_n_divider, core_clock_pll_m_divider);
    }

    if ( !(puc_lcpll_in_sel ==  SOC_RAMON_PORT_PUC_LCPLL_IN_DISABLE_POWERUP) && !SAL_BOOT_PLISIM )
    {
        
        LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit, "PLL was configured at Power Up. serdes_fabric_clk_freq_in / serdes_fabric_clk_freq_out SoC properties are disregarded!\n")));
        SHR_EXIT();
    }

    for(lcpll=0 ; lcpll<dnxf_data_port.pll.nof_lcpll_get(unit); lcpll++) 
    {
        lcpll_in = dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_in;
        lcpll_out = dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_out;

        if (dnxf_data_port.pll.config_get(unit,lcpll_in,lcpll_out)->valid == 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,"Illegal reference clock configuration IN=%d, OUT=%d", lcpll_in, lcpll_out);
        }

        SOC_REG_ABOVE_64_CLEAR(pll_config_above_64_val);

        for (config_index=0 ; config_index < dnxf_data_port.pll.nof_config_words_get(unit) ; config_index++)
        {
            SOC_REG_ABOVE_64_WORD_SET(pll_config_above_64_val, dnxf_data_port.pll.config_get(unit,lcpll_in,lcpll_out)->data[config_index], config_index);
        }

        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

        
        soc_reg_above_64_field_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_config_field[lcpll], pll_config_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_sw_ovrd_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_reset_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        sal_usleep(100);

        
        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, pll_stat_reg[lcpll], REG_PORT_ANY, 0, &pll_status_val32));
            pll_locked = soc_reg_field_get(unit, pll_stat_reg[lcpll], pll_status_val32, pll_locked_field[lcpll]);
            if (pll_locked != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INIT,"Failed to init LCPLL %d", lcpll);
            }
        }

        
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_post_reset_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

    }

exit:
    SHR_FUNC_EXIT; 
}

int
soc_ramon_drv_pvt_monitor_enable(int unit)
{
    uint64 reg64_val;
    soc_reg_t pvt_monitors[] = {ECI_PVT_MON_A_CONTROL_REGr, ECI_PVT_MON_B_CONTROL_REGr, ECI_PVT_MON_C_CONTROL_REGr, ECI_PVT_MON_D_CONTROL_REGr};
    int pvt_index;
    SHR_FUNC_INIT_VARS(unit);

    
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_BITSET(reg64_val, _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_BITSET(reg64_val, _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

exit:
    SHR_FUNC_EXIT; 
}


int
soc_ramon_drv_rings_map_set(int unit)
{
	SHR_FUNC_INIT_VARS(unit);

	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x73333027));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x44443333));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x22204444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x55555555));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x72532244));

	exit:
	SHR_FUNC_EXIT;
}

int
soc_ramon_drv_sbus_broadcast_config(int unit)
{
    int block_idx;
    int is_enabled;
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_fmac_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan));
    }
    
    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_fsrd_get(unit); block_idx++)
    {
        
        is_enabled = si->block_valid[block_idx];
        if (is_enabled)
        {
            SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan));
        }
    }

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_dch_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCH_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_DCH_BLOCK(unit)).schan));
    }

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCML_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_DCML_BLOCK(unit)).schan));
    }

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_cch_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_SBUS_BROADCAST_IDr, block_idx, 0, SOC_BLOCK_INFO(unit,BRDC_CCH_BLOCK(unit)).schan));
    }

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_lcm_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_LCM_BLOCK(unit)).schan));
    }

    
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_qrh_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_QRH_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit, BRDC_QRH_BLOCK(unit)).schan));
    }

exit:
    SHR_FUNC_EXIT;
}

static int 
soc_ramon_drv_soft_init_dch_reg_info_get(
    int unit,
    soc_ramon_soft_init_restore_t * dch_soft_reset_info,
    int * nof_relevant_dch_registers)
{
    soc_reg_info_t *reginfo;
    soc_reg_t reg;
    int dch_register_idx = 0;
    int instance_idx, numel_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {

        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        if (!SOC_REG_IS_ENABLED(unit, reg))
            continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        if (!SOC_BLOCK_IS(reginfo->block, SOC_BLK_DCH))
            continue;

        if (reginfo->flags & (SOC_REG_FLAG_RO | SOC_REG_FLAG_WO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL))
            continue;

        
        dch_soft_reset_info[dch_register_idx].reg = reg;

        

        if (reginfo->numels > SOC_RAMON_DRV_MAX_NOF_REG_NUMELS )
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ramon SW reset WA ERROR - nof relevant DCH blocks numels %d bigger than maximum expected", reginfo->numels);
        }
        dch_soft_reset_info[dch_register_idx].reg_numels = reginfo->numels;

         
        for (numel_idx = 0; numel_idx < dch_soft_reset_info[dch_register_idx].reg_numels; numel_idx++)
        {
            for (instance_idx = 0; instance_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); instance_idx++ )
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, dch_soft_reset_info[dch_register_idx].reg, instance_idx, numel_idx, dch_soft_reset_info[dch_register_idx].reg_above_64_val[numel_idx][instance_idx]));
            }
        }

        dch_register_idx++;
    }

    *nof_relevant_dch_registers = dch_register_idx;

    if (*nof_relevant_dch_registers > SOC_RAMON_DRV_MAX_NOF_DCH_REGISTERS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ramon SW reset WA ERROR - nof relevant DCH registers %d bigger than maximum expected", *nof_relevant_dch_registers);
    }

exit:
    SHR_FUNC_EXIT;
}

static int 
soc_ramon_drv_soft_init_dch_reg_info_set(
    int unit,
    soc_ramon_soft_init_restore_t * dch_soft_reset_info,
    int nof_relevant_dch_registers)
{
    int dch_register_idx = 0, instance_idx, numel_idx;

    SHR_FUNC_INIT_VARS(unit);
     

    for (dch_register_idx = 0; dch_register_idx < nof_relevant_dch_registers; dch_register_idx++)
    {
        for (numel_idx = 0; numel_idx < dch_soft_reset_info[dch_register_idx].reg_numels; numel_idx++)
        {
            for (instance_idx = 0; instance_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); instance_idx++ )
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, dch_soft_reset_info[dch_register_idx].reg , instance_idx, numel_idx, dch_soft_reset_info[dch_register_idx].reg_above_64_val[numel_idx][instance_idx]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}



int 
soc_ramon_drv_soft_init(int unit, uint32 soft_reset_mode_flags)
{
    soc_reg_above_64_val_t reg_above_64;

    int nof_relevant_dch_registers = 0;
    soc_ramon_soft_init_restore_t * dch_soft_reset_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_CONTROL(unit)->soc_flags & SOC_F_INITED)
    {
        if (dnxf_data_device.blocks.feature_get(unit, dnxf_data_device_blocks_dch_reset_restore_support))
        {
            
            SHR_ALLOC_SET_ZERO(dch_soft_reset_info,
                           sizeof(soc_ramon_soft_init_restore_t) * SOC_RAMON_DRV_MAX_NOF_DCH_REGISTERS,
                           "soc_ramon_soft_init_restore_t",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            if (NULL == dch_soft_reset_info)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "alloc dch_soft_reset_info FAILED!");
            }
            SHR_IF_ERR_EXIT(soc_ramon_drv_soft_init_dch_reg_info_get(unit, dch_soft_reset_info, &nof_relevant_dch_registers)); 
        }
    }

    if (soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_IN_RESET || soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_INOUT_RESET)
    {

        if (SOC_CONTROL(unit)->soc_flags & SOC_F_INITED)
        {
            
            if (dnxf_data_device.blocks.feature_get(unit, dnxf_data_device_blocks_dch_reset_restore_support))
            {
                SOC_REG_ABOVE_64_CREATE_MASK(reg_above_64, dnxf_data_device.blocks.nof_instances_dch_get(unit), SOC_RAMON_DRV_FIRST_DCH_BLOCK_IDX);
                SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
            }
        }

        
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));

    }

    if (soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_OUT_RESET || soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_INOUT_RESET)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        if (SOC_CONTROL(unit)->soc_flags & SOC_F_INITED)
        {
            if (dnxf_data_device.blocks.feature_get(unit, dnxf_data_device_blocks_dch_reset_restore_support))
            {
                
                SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));

                SHR_IF_ERR_EXIT(soc_ramon_drv_soft_init_dch_reg_info_set(unit, dch_soft_reset_info, nof_relevant_dch_registers));
            }
        }

        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
    }

exit:
    SHR_FREE(dch_soft_reset_info);
    SHR_FUNC_EXIT;
}

int soc_ramon_drv_reg_access_only_reset(int unit)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    
    rc =soc_ramon_reset_cmic_iproc_regs(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_blocks_reset(unit, 0  , NULL);
    SHR_IF_ERR_EXIT(rc);

    rc = soc_ramon_drv_pll_config_set(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_sbus_broadcast_config(unit);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_ramon_drv_dynamic_mem_access_enable(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
soc_ramon_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances) 
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_block_instances, _SHR_E_PARAM, "nof_block_instances");
    SHR_NULL_CHECK(block_types, _SHR_E_PARAM, "block_types");

    switch(block_types[0]) {
        case SOC_BLK_FMAC:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_fmac_get(unit); 
            break;
        case SOC_BLK_FSRD:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);
            break;
        case SOC_BLK_DCH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dch_get(unit);
            break;
        case SOC_BLK_CCH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_cch_get(unit);
            break;
        case SOC_BLK_DCL:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_RTP:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_rtp_get(unit);
            break;
        case SOC_BLK_OCCG:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_occg_get(unit);
            break;
        case SOC_BLK_ECI:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_eci_get(unit);
            break;
        case SOC_BLK_DCM:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_CMIC:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_cmic_get(unit);
            break;
        case SOC_BLK_DCML:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_MCT:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_mct_get(unit);
            break;
        case SOC_BLK_QRH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
            break;
        case SOC_BLK_LCM:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
            break;
        case SOC_BLK_MESH_TOPOLOGY:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_mesh_topology_get(unit);
            break;
        case SOC_BLK_BRDC_FMACH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fmach_get(unit);
            break;
        case SOC_BLK_BRDC_FMACL:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fmacl_get(unit);
            break;
        case SOC_BLK_BRDC_FSRD:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fsrd_get(unit);
            break;
            
        case SOC_BLK_BRDC_CCH:
        case SOC_BLK_BRDC_DCML:
        case SOC_BLK_BRDC_LCM:
        case SOC_BLK_BRDC_QRH:
            *nof_block_instances = 1;
            break;
        default:
            *nof_block_instances = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}



int
soc_ramon_drv_mbist(int unit, int skip_errors)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SOC_DNXF_DRV_INIT_LOG(unit, "Memory Bist");
    rc = soc_bist_all_ramon(unit, skip_errors);
    SHR_IF_ERR_EXIT(rc);
        
exit:
    SHR_FUNC_EXIT;   
}

int
soc_ramon_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp)
{   
    int first_link = 0,range = 0;
    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(*pbmp);

    switch (block_type)
    {
       case SOC_BLK_DCH:
       case SOC_BLK_DCML:
           first_link = dnxf_data_device.blocks.nof_links_in_dcq_get(unit) * blk_instance;
           range = dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
           break;
       default:
           SHR_ERR_EXIT(_SHR_E_INTERNAL, "block (%d) - block pbmp is not supported", block_type);
           break;
    }

    SOC_PBMP_PORTS_RANGE_ADD(*pbmp, first_link, range);
    SOC_PBMP_AND(*pbmp, PBMP_SFI_ALL(unit));

exit:
    SHR_FUNC_EXIT;
}


int soc_ramon_tbl_is_dynamic(int unit, soc_mem_t mem) {

    if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem)) {
        return TRUE;
    }

    switch (mem)
    {
        case RTP_MULTI_CAST_TABLE_UPDATEm:
            return TRUE;
        case RTP_SLSCTm:
            if (soc_dnxf_load_balancing_mode_destination_unreachable !=
                dnxf_data_fabric.topology.load_balancing_mode_get(unit))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }

        default:
            return FALSE;
    }
}

int
soc_ramon_drv_block_valid_get(int unit, int blktype, int blockid, char *valid)
{

    int enabled = 1;
    soc_pbmp_t pbmp_supported;
    soc_pbmp_t pbmp_fsrd_tmp;

    SHR_FUNC_INIT_VARS(unit);
    SOC_PBMP_CLEAR(pbmp_fsrd_tmp);
    pbmp_supported = dnxf_data_port.general.supported_phys_get(unit)->pbmp;

    if (blktype == SOC_BLK_FSRD) {
        SOC_PBMP_PORTS_RANGE_ADD(pbmp_fsrd_tmp, blockid * dnxf_data_device.blocks.nof_links_in_fsrd_get(unit), dnxf_data_device.blocks.nof_links_in_fsrd_get(unit));
        SOC_PBMP_AND(pbmp_fsrd_tmp, pbmp_supported);
        if (SOC_PBMP_IS_NULL(pbmp_fsrd_tmp))
        {
            enabled = 0;
        }

        *valid = ((enabled == 1) ? 1 : 0);
    } else {
        *valid  = 1;
    }

    SHR_FUNC_EXIT; 
}

int
soc_ramon_drv_link_to_block_mapping(int unit, int link, int* block_id,int* inner_link, int block_type)
{   
    int nof_links_in_blk;

    SHR_FUNC_INIT_VARS(unit);

    switch (block_type)
    {
       case SOC_BLK_DCH:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_dch_get(unit);
           break;

       case SOC_BLK_DCML:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_dcml_get(unit);
           break;

       case SOC_BLK_LCM:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_lcm_get(unit);
           break;

       case SOC_BLK_QRH:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_qrh_get(unit);
           break;

       case SOC_BLK_FMAC:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
           break;

       default:
           SHR_ERR_EXIT(_SHR_E_INTERNAL, "block (%d) - link to block mapiing is not supported", block_type);
           break;
    }

   *block_id=INT_DEVIDE(link,nof_links_in_blk);
   *inner_link=link % nof_links_in_blk;

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_set_mesh_topology_config(int unit) {

    uint32 reg_val32, intg, frac; 
    uint64 frac64;
    soc_reg_t mesh_topology_reg_0107;
    int gt_size = -1;
    SHR_FUNC_INIT_VARS(unit);
    
    mesh_topology_reg_0107 = MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r;


    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, FIELD_27_27f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_2r, &reg_val32, FIELD_4_17f, 2049);    
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_INITr(unit, &reg_val32));
    if(SOC_DNXF_IS_FE13(unit)) {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, INITf, 10);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_1f, 2);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 0xe);
    } else {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 1);
    }
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_INITr(unit, reg_val32));

    intg =
        dnxf_data_device.general.system_ref_core_clock_khz_get(unit) /
        dnxf_data_device.general.core_clock_khz_get(unit);
    COMPILER_64_SET(frac64, 0, dnxf_data_device.general.system_ref_core_clock_khz_get(unit));
    COMPILER_64_SUB_32(frac64, dnxf_data_device.general.core_clock_khz_get(unit) * intg);
    COMPILER_64_SHL(frac64, 19);
    SHR_IF_ERR_EXIT(soc_dnxf_compiler_64_div_32(frac64, dnxf_data_device.general.core_clock_khz_get(unit), &frac));

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, &reg_val32));

    soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_1f, frac);
    soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_2f, intg);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, reg_val32));

    
    gt_size = soc_property_suffix_num_get(unit,0,spn_CUSTOM_FEATURE, "mesh_topology_size", -1);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    if (gt_size == -1)
    {
        if (dnxf_data_fabric.pipes.system_contains_multiple_pipe_device_get(unit))
        {
            soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 2);
        }
        else
        {
            soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 0);
        }
    } else {
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, gt_size);
    }
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_1f, 15);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_011Br(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_0f, 5); 

    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_1f, 0xc);

    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_2f,
                      dnxf_data_device.custom_features.mesh_topology_fast_get(unit) ? 1 : 0);

    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_011Br(unit, reg_val32));

    sal_usleep(20);

    
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
