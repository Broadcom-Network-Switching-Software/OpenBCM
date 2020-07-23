/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 DRV
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/mem.h>

#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_iproc.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>

#include <soc/sand/sand_ser_correction.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe3200/fe3200_drv.h>
#include <soc/dfe/fe3200/fe3200_defs.h>
#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_stack.h>
#include <soc/dfe/fe3200/fe3200_port.h>
#include <soc/dfe/fe3200/fe3200_fabric_flow_control.h>
#include <soc/dfe/fe3200/fe3200_fabric_links.h>
#include <soc/dfe/fe3200/fe3200_fabric_topology.h>

#include <soc/dfe/fe1600/fe1600_config_imp_defs.h>
#include <soc/dfe/fe1600/fe1600_drv.h>

extern char *_build_release;


#define _SOC_FE3200_DRV_SBUS_RING_MAP_0_VAL                 (0x02222227)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_1_VAL                 (0x33333222)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_2_VAL                 (0X55333333)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_3_VAL                 (0x65555555)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_4_VAL                 (0x66666666)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_5_VAL                 (0x55555555)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_6_VAL                 (0x66666665)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_7_VAL                 (0x44444466)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_8_VAL                 (0x10444444)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_9_VAL                 (0x22040065)
#define _SOC_FE3200_DRV_SBUS_RING_MAP_10_VAL                (0x00000033)

#define _SOC_FE3200_DRV_BRDC_FMAC_AC_ID                     (72)
#define _SOC_FE3200_DRV_BRDC_FMAC_BD_ID                     (73)
#define _SOC_FE3200_DRV_BRDC_DCH                            (78)
#define _SOC_FE3200_DRV_BRDC_DCM                            (80)
#define _SOC_FE3200_DRV_BRDC_DCL                            (79)
#define _SOC_FE3200_DRV_BRDC_CCS                            (81)
#define _SOC_FE3200_DRV_BRDC_FSRD                           (76)


#define _SOC_FE3200_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT   (32)
#define _SOC_FE3200_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)
#define _SOC_FE3200_PVT_MON_NOF                             (4)
#define _SOC_FE3200_PVT_FACTOR                              (49103)
#define _SOC_FE3200_PVT_BASE                                (41205000)


#define _SOC_FE3200_WFQ_PIPES_PRIORITY_INIT_VALUE           (0x7)


#define _SOC_FE3200_DRV_FE13_ASYM_FAP_FIRST_LINK_QUARTER_0_PART_2       (104)
#define _SOC_FE3200_DRV_FE13_ASYM_FAP_FIRST_LINK_QUARTER_1_PART_2       (140)
#define _SOC_FE3200_DRV_FE13_ASYM_FAP_RANGE_PART_2                      (4)
#define _SOC_FE3200_DRV_FE13_ASYM_FE2_RANGE                             (32)

STATIC int
soc_fe3200_reset_cmic_iproc_regs(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));
    sal_usleep(10*1000); 
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_device_hard_reset(unit, SOC_DCMN_RESET_ACTION_INOUT_RESET));
    SOCDNX_IF_ERR_EXIT(soc_dcmn_iproc_config_paxb(unit));

    
    soc_endian_config(unit);
    soc_pci_ep_config(unit,0);

    
    SOCDNX_IF_ERR_EXIT(soc_fe3200_drv_rings_map_set(unit));
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_sbus_timeout_set(unit, SOC_DFE_CONFIG(unit).core_clock_speed , SOC_CONTROL(unit)->schanTimeout));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC0_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC1_SCHAN_ERRr(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CMC2_SCHAN_ERRr(unit, 0));

    
    SOCDNX_IF_ERR_EXIT(soc_fe3200_drv_mdio_config_set(unit));
    
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_fe3200_avs_value_get(
            int       unit,
            uint32*      avs_val)
{
    uint32
        reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(avs_val);

    *avs_val = 0;

    SOCDNX_IF_ERR_EXIT(READ_ECI_REG_01BBr(unit, &reg_val));
    *avs_val = soc_reg_field_get(unit, ECI_REG_01BBr, reg_val, FIELD_24_26f);

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_fe3200_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap) 
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (force_blocks_reset_value)
    {
        SOC_REG_ABOVE_64_COPY(reg_above_64, *block_bitmap);
    } else {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    sal_sleep(1); 
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64));
    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_drv_fabric_device_mode_set(int unit, soc_dfe_fabric_device_mode_t fabric_device_mode)
{
    uint32 repeater_mode_get, repeater_mode_set;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_FE_CFG_1r(unit, &reg32_val));

    
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, REP_MODEf, 
                      (fabric_device_mode == soc_dfe_fabric_device_mode_repeater) ? 1 : 0);
    
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, REP_MODE_FC_ENf, 0); 
        
    
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, FE_13_MODEf, 
                      (fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13 || 
                       fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric) ? 1 : 0);
    
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, FAP_80_LINKSf, SOC_DFE_IS_FE13_ASYMMETRIC(unit)? 1:0); 

    
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, MULTISTAGE_MODEf, 
                      (fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe2 ||
                       fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13 ||
                       fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric) ? 1 : 0);
    

    

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_FE_CFG_1r(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_DCH_GLOBAL_GENERAL_FE_CFG_1r(unit, REG_PORT_ANY, &reg32_val));
    repeater_mode_get = soc_reg_field_get(unit, DCH_GLOBAL_GENERAL_FE_CFG_1r, reg32_val, REP_MODEf);
    repeater_mode_set = (fabric_device_mode == soc_dfe_fabric_device_mode_repeater) ? 1 : 0;
    if (repeater_mode_set  != repeater_mode_get && !SAL_BOOT_PLISIM) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("FABRIC_DEVICE_MODE!=REPEATER -  device for repeater only use")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_drv_fabric_load_balancing_set(int unit, soc_dfe_load_balancing_mode_t load_balancing)
{
    uint64 reg_val64;
    uint32 load_balancing_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));

    load_balancing_val = soc_dfe_load_balancing_mode_normal == load_balancing ? 1 : 0;
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, DISABLE_RCG_LOAD_BALANCINGf, 
                            load_balancing_val);

    load_balancing_val = (soc_dfe_load_balancing_mode_destination_unreachable == load_balancing) ? 0 : 1;
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, load_balancing_val);
                            
    if (load_balancing == soc_dfe_load_balancing_mode_normal || SOC_DFE_IS_FE13_ASYMMETRIC(unit))
    {
        
        soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, MAX_DOWN_LINKS_FE_1f, 0);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_DRH_LOAD_BALANCING_LEVEL_CONFIGr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_DRH_LOAD_BALANCING_LEVEL_CONFIGr, &reg_val64, LOAD_BALANCE_LEVELS_IGNOREf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_DRH_LOAD_BALANCING_LEVEL_CONFIGr(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_drv_fabric_multicast_config_set(int unit, soc_dfe_multicast_mode_t mc_mode, int fe_mc_priority_map_enable)
{
    int i;
    uint32 reg_val32;
    uint32 multicast_mode_field_val;
    soc_dfe_multicast_table_mode_t multicast_mode;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;


    for (i=0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_PRIORITY_TRANSLATIONr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, LOW_PRE_MUL_SETf, SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable ? 1 : 0);
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, MID_PRE_MUL_SETf, SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable ? 1 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_PRIORITY_TRANSLATIONr(unit, i, reg_val32));

    }
    if (SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable)
    {
        COMPILER_64_ZERO(reg_val64);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_DCH_USE_MC_CELL_PRIO_BMPr(unit, reg_val64));
    }
    

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_MODE_SELECTIONr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_INDIRECT_LIST_OF_FAPS_MODEf, 
                        soc_dfe_multicast_mode_indirect == mc_mode ? 1 : 0);
    
    
    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_mode_get, (unit, &multicast_mode)));
    switch (multicast_mode)
    {
        case soc_dfe_multicast_table_mode_64k:
            multicast_mode_field_val = 0; 
            break;
        case soc_dfe_multicast_table_mode_128k:
            multicast_mode_field_val = 1; 
            break;
        case soc_dfe_multicast_table_mode_128k_half:
            multicast_mode_field_val = 4; 
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
    }
    soc_reg_field_set(unit, RTP_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_TABLE_MODEf, multicast_mode_field_val);

    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_MODE_SELECTIONr(unit, reg_val32));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_drv_fabric_local_routing_set(int unit, int local_routing_enable)
{
    int i;
	uint32 reg_val32;
	uint64 reg_val64;

	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(READ_RTP_DRH_LOAD_BALANCING_GENERAL_CONFIGr(unit, &reg_val64));
	soc_reg64_field32_set(unit, RTP_DRH_LOAD_BALANCING_GENERAL_CONFIGr, &reg_val64, ENABLE_LOCAL_FE_1_ROUTINGf, local_routing_enable == 1 ? 0xf : 0);
    soc_reg64_field32_set(unit, RTP_DRH_LOAD_BALANCING_GENERAL_CONFIGr, &reg_val64, ENABLE_CTRL_LOCAL_FE_1_ROUTINGf, 0x1); 
	SOCDNX_IF_ERR_EXIT(WRITE_RTP_DRH_LOAD_BALANCING_GENERAL_CONFIGr(unit, reg_val64));

	for (i=0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dcm) ; i++ )
	{
		SOCDNX_IF_ERR_EXIT(READ_DCM_DCM_ENABLERS_REGISTERr(unit, i, &reg_val32));
		soc_reg_field_set(unit, DCM_DCM_ENABLERS_REGISTERr, &reg_val32, LOCAL_ROUT_ENABLEf, local_routing_enable == 1 ? 1:0);
		SOCDNX_IF_ERR_EXIT(WRITE_DCM_DCM_ENABLERS_REGISTERr(unit, i, reg_val32));
	}

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_soc_fe3200_drv_fabric_cell_priority_config_set(int unit, uint32 min_tdm_priority)
{
    uint32 reg32_val;
    uint32 tdm_bmp;
    int dch_instance;
    SOCDNX_INIT_FUNC_DEFS;

    
    for (dch_instance = 0; dch_instance < SOC_DFE_DEFS_GET(unit, nof_instances_dch); dch_instance++)
    {
        tdm_bmp = 0;
        SOCDNX_IF_ERR_EXIT(READ_DCH_TDM_PRIORITYr(unit, dch_instance, &reg32_val));
        if (min_tdm_priority == SOC_DFE_FABRIC_TDM_PRIORITY_NONE) 
        {
            
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x0); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        } else {
            
            SHR_BITSET_RANGE(&tdm_bmp, min_tdm_priority, soc_dfe_fabric_priority_nof - min_tdm_priority);
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x1); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_TDM_PRIORITYr(unit, dch_instance, reg32_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

#define _SOC_FE3200_MAX_LINKS_IN_DCH    (40)

STATIC int
_soc_fe3200_drv_fabric_pipes_config_set(int unit, soc_dcmn_fabric_pipe_map_t fabric_pipe_mapping)
{
    uint32 reg32_val;
    int rv, link;
    int dch_instance;
    int fmac_instance, inner_link;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    

    
    rv = READ_ECI_GLOBAL_GENERAL_FE_CFG_1r(unit,&reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, PIPE_1_ENf, fabric_pipe_mapping.nof_pipes > 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_FE_CFG_1r, &reg32_val, PIPE_2_ENf, fabric_pipe_mapping.nof_pipes > 2);
    rv = WRITE_ECI_GLOBAL_GENERAL_FE_CFG_1r(unit,reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    PBMP_SFI_ITER(unit, link)
    {
        fmac_instance = link / SOC_DFE_DEFS_GET(unit, nof_links_in_mac);
        inner_link = link % SOC_DFE_DEFS_GET(unit, nof_links_in_mac);
        rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link , &reg32_val);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf, fabric_pipe_mapping.nof_pipes - 1);
        rv  = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link , reg32_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    COMPILER_64_SET(reg64_val, 0, 0);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, _SOC_FE3200_MAX_LINKS_IN_DCH, 0);
    }
    rv = WRITE_BRDC_DCH_TWO_PIPES_BMPr(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, _SOC_FE3200_MAX_LINKS_IN_DCH, 0);
    }
    rv = WRITE_BRDC_DCH_THREE_PIPES_BMPr(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    COMPILER_64_SET(reg64_val, 0, 0);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, SOC_DFE_DEFS_GET(unit, nof_links_in_dcl), 0);
    }
    rv = WRITE_BRDC_DCL_TWO_PIPES_BMPr(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, SOC_DFE_DEFS_GET(unit, nof_links_in_dcl), 0);
    }
    rv = WRITE_BRDC_DCL_THREE_PIPES_BMPr(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    for (dch_instance = 0; dch_instance < SOC_DFE_DEFS_GET(unit, nof_instances_dch); dch_instance++)
    {
        rv = READ_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, &reg32_val);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_0_PIPEf, fabric_pipe_mapping.config_uc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_1_PIPEf, fabric_pipe_mapping.config_uc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_2_PIPEf, fabric_pipe_mapping.config_uc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_3_PIPEf, fabric_pipe_mapping.config_uc[3]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_0_PIPEf, fabric_pipe_mapping.config_mc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_1_PIPEf, fabric_pipe_mapping.config_mc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_2_PIPEf, fabric_pipe_mapping.config_mc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_3_PIPEf, fabric_pipe_mapping.config_mc[3]);
        rv = WRITE_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, reg32_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_set_operation_mode(int unit)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    
     rv = _soc_fe3200_drv_fabric_device_mode_set(unit, SOC_DFE_CONFIG(unit).fabric_device_mode);
     SOCDNX_IF_ERR_EXIT(rv);
    

     
     rv = _soc_fe3200_drv_fabric_cell_priority_config_set(unit, SOC_DFE_CONFIG(unit).fabric_tdm_priority_min);
     SOCDNX_IF_ERR_EXIT(rv);

    
     rv = _soc_fe3200_drv_fabric_pipes_config_set(unit, SOC_DFE_FABRIC_PIPES_CONFIG(unit));
     SOCDNX_IF_ERR_EXIT(rv);


     
     rv = _soc_fe3200_drv_fabric_multicast_config_set(unit,  SOC_DFE_CONFIG(unit).fabric_multicast_mode, SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable); 
     SOCDNX_IF_ERR_EXIT(rv);

     
     rv = _soc_fe3200_drv_fabric_load_balancing_set(unit, SOC_DFE_CONFIG(unit).fabric_load_balancing_mode);
     SOCDNX_IF_ERR_EXIT(rv);

	 if (SOC_DFE_IS_FE13(unit))
	 {
		 rv = _soc_fe3200_drv_fabric_local_routing_set(unit, SOC_DFE_CONFIG(unit).fabric_local_routing_enable);
		 SOCDNX_IF_ERR_EXIT(rv);
	 }


exit:
    SOCDNX_FUNC_RETURN;
}

#define _SOC_FE3200_DRV_MULTIPLIER_TABLE_MAX_LINKS                  (72)

#define _SOC_FE3200_DRV_MULTIPLIER_T1_LENGTH                        (64)
#define _SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH                         (32)
#define _SOC_FE3200_DRV_MULTIPLIER_T1_MAX_TOTAL_LINKS               (_SOC_FE3200_DRV_MULTIPLIER_T1_LENGTH)
#define _SOC_FE3200_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS              (_SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_FE3200_DRV_MULTIPLIER_T1_AND_T2_SIZE                   (_SOC_FE3200_DRV_MULTIPLIER_T1_LENGTH * _SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_FE3200_DRV_MULTIPLIER_T2_LENGTH                        (32)
#define _SOC_FE3200_DRV_MULTIPLIER_T2_WIDTH                         (32)
#define _SOC_FE3200_DRV_MULTIPLIER_T2_MIN_TOTAL_LINKS               (33)
#define _SOC_FE3200_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS              (_SOC_FE3200_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS + 1)

#define _SOC_FE3200_DRV_MULTIPLIER_T3_LENGTH                        (8)
#define _SOC_FE3200_DRV_MULTIPLIER_T3_WIDTH                         (64)
#define _SOC_FE3200_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS               (_SOC_FE3200_DRV_MULTIPLIER_T1_MAX_TOTAL_LINKS + 1)
#define _SOC_FE3200_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS              (64)
#define _SOC_FE3200_DRV_MULTIPLIER_T3_SIZE                          (_SOC_FE3200_DRV_MULTIPLIER_T3_LENGTH * _SOC_FE3200_DRV_MULTIPLIER_T3_WIDTH)

#define _SOC_FE3200_DRV_MULTIPLIER_T4_LENGTH                        (8)
#define _SOC_FE3200_DRV_MULTIPLIER_T4_WIDTH                         (8)
#define _SOC_FE3200_DRV_MULTIPLIER_T4_MIN_TOTAL_LINKS               (_SOC_FE3200_DRV_MULTIPLIER_T1_MAX_TOTAL_LINKS + 1)
#define _SOC_FE3200_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS              (_SOC_FE3200_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS + 1)

#define _SOC_FE3200_MULTIPLIER_TABLE_DUMP 0


extern int soc_dfe_clean_rtp_table_array(int unit, soc_mem_t mem, soc_reg_above_64_val_t data);


STATIC int
soc_fe3200_reset_tables(int unit)
{
    soc_reg_above_64_val_t data;
    uint32 total_links, active_links, score[1], entry, ecc[1], mem_row_bit_width;
    uint32 bmp[5];
    uint32 totsf_val, slsct_val, score_slsct, links_count, sctinc_val, sctinc;
    soc_field_t scrub_en;
    uint64 reg_val64;
    int link, array_index;
    uint32 table_entry[5] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    scrub_en = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, SCT_SCRUB_ENABLEf); 
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, 0);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    SOC_REG_ABOVE_64_CLEAR(data);
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MCLBTPm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MNOLPm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_RCGLBTm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_TOTSFm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_SLSCTm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_SCTINCm, data));

    totsf_val = 0;
    links_count = 1;
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, LINK_NUMf, (uint32*)&links_count);

    slsct_val = 0;
    score_slsct = 0;
    soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, LINK_NUMf, &score_slsct);

    sctinc_val = 0;
    sctinc = 0;
    soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, LINK_NUMf, &sctinc);
    
    for(link = 0 ; link < SOC_DFE_DEFS_GET(unit, nof_links) ; link++) {
        
        bmp[0] = bmp[1] = bmp[2] = bmp[3] = bmp[4] = 0;
        SHR_BITSET(bmp,link);

        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
        for (array_index = 0; array_index < SOC_DFE_DEFS_GET(unit, nof_rtp_mclbtp_instances); array_index++) {
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_MNOLPm(unit, array_index, MEM_BLOCK_ALL, link, table_entry));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_MCSFFPm(unit, array_index, MEM_BLOCK_ALL, link, table_entry));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_MCLBTPm(unit, array_index, MEM_BLOCK_ALL, link, table_entry));
        }
    }

    for(link = 0 ; link <= soc_mem_index_max(unit, RTP_FFLBPm); link++) {
        for (array_index = 0; array_index < SOC_MEM_NUMELS(unit,RTP_FFLBPm); array_index++) {
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_FFLBPm(unit, array_index, MEM_BLOCK_ALL, link, table_entry));
        }
    }

    
    
    
    SOC_REG_ABOVE_64_CLEAR(data);
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MULTI_TBm, data));

    
#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
    LOG_CLI((BSL_META_U(unit,
                        "MUL TABLE\n\n")));
#endif

#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
    LOG_CLI((BSL_META_U(unit,
                        "%02d || "), 0));
#endif
    for(active_links = 1 ; active_links <= 72 ; active_links++) {
#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "%04d | "), active_links));
#endif
    }
#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
    LOG_CLI((BSL_META_U(unit,
                        "\n\n")));
#endif
    for(total_links = 1 ; total_links <= _SOC_FE3200_DRV_MULTIPLIER_TABLE_MAX_LINKS ; total_links++) {

#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "%02d || "), total_links));
#endif
        for(active_links = 1 ; active_links <= total_links ; active_links++) {
            *score = (SOC_DFE_DRV_MULTIPLIER_MAX_LINK_SCORE * active_links) / total_links;
            if ((SOC_DFE_DRV_MULTIPLIER_MAX_LINK_SCORE * active_links) % total_links != 0) {
                (*score)++;
            }
            if (total_links >= _SOC_FE3200_DRV_MULTIPLIER_T4_MIN_TOTAL_LINKS && active_links >= _SOC_FE3200_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS) 
            {
                
                entry = _SOC_FE3200_DRV_MULTIPLIER_T1_AND_T2_SIZE + _SOC_FE3200_DRV_MULTIPLIER_T3_SIZE
                    + ((total_links - 1) % _SOC_FE3200_DRV_MULTIPLIER_T4_LENGTH)*_SOC_FE3200_DRV_MULTIPLIER_T4_WIDTH
                    + (active_links - 1) % _SOC_FE3200_DRV_MULTIPLIER_T4_WIDTH;
                        
            } else if (total_links >= _SOC_FE3200_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS && active_links <= _SOC_FE3200_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS)
            {
                
                entry = _SOC_FE3200_DRV_MULTIPLIER_T1_AND_T2_SIZE 
                    + ((total_links - 1) % _SOC_FE3200_DRV_MULTIPLIER_T3_LENGTH)*_SOC_FE3200_DRV_MULTIPLIER_T3_WIDTH
                    + active_links - 1;
            } else if (active_links <= _SOC_FE3200_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS)
            {
                
                entry = (total_links - 1)*_SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH + active_links - 1;
            } else {
                
                entry = (_SOC_FE3200_DRV_MULTIPLIER_T1_LENGTH - total_links)* _SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH
                    + _SOC_FE3200_DRV_MULTIPLIER_T1_WIDTH  + _SOC_FE3200_DRV_MULTIPLIER_T2_WIDTH - active_links;
            }
#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
            LOG_CLI((BSL_META_U(unit,
                                "%04d | "), entry));
#endif
            mem_row_bit_width = soc_mem_entry_bits(unit, RTP_MULTI_TBm) - soc_mem_field_length(unit, RTP_MULTI_TBm, ECCf);
            SOCDNX_IF_ERR_EXIT(calc_ecc(unit, mem_row_bit_width, score, ecc));
            soc_mem_field32_set(unit, RTP_MULTI_TBm, score, ECCf, *ecc);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_TBm(unit, MEM_BLOCK_ALL, entry, score));
        }
#if _SOC_FE3200_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
#endif
    }
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, scrub_en);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_set_fmac_config(int unit)
{
    uint32 reg_val32, field[1];
    int i;
    int link, blk, inner_link;
    soc_dfe_fabric_link_device_mode_t link_mode;
    int nof_links_in_mac;
    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_FILL_RATEf, SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_UP_THf, 0x20);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_DN_THf, 0x10);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, SIG_DET_BKT_RST_ENAf, 0x1);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, ALIGN_LCK_BKT_RST_ENAf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, reg_val32));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_LEAKY_BUCKET_CONTROL_REGISTERr(unit, reg_val32));


    
    if (SOC_DFE_IS_FE13(unit))
    {
       PBMP_SFI_ITER(unit, link)
       {
           SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, link, 0, &link_mode)));
           SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

           SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, &reg_val32));
           soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
           if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
           {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE1);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE1);
           } else {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3);
           }
           SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, reg_val32));
       }
    } else {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 0, 0, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        if (SOC_DFE_IS_REPEATER(unit)) {
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_REPEATER);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_REPEATER);
        } else {
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE2);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_FE3200_PORT_COMMA_BURST_SIZE_FE2);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 0, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 1, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 2, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 3, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 0, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 1, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 2, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 3, reg_val32));
       
    }
    
    if(SOC_DFE_IS_FE13(unit)) {

        PBMP_SFI_ITER(unit, link)
        {

            SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, link, 0, &link_mode)));
            if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
            {
                SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

                SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, &reg_val32));
                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_0f);
                SHR_BITSET(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_0f, *field);

                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_1f);
                SHR_BITCLR(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_1f, *field);

                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, reg_val32));
            }
        }
    } else if (SOC_DFE_IS_REPEATER(unit))
    {
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_LINK_TOPO_MODE_REG_0r(unit,0));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_LINK_TOPO_MODE_REG_0r(unit,0));
    }

    
    reg_val32 = 0;
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_RX_ENf, 0xf);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_TX_ENf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    
    for(i=0; i<nof_links_in_mac; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

    
    for(i=0; i<nof_links_in_mac; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg_val32 ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, i, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_TX_GENERAL_CONFIGURATIONr(unit, i, reg_val32));
    }

    
    
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_REG_0102r(unit, 0xf));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_REG_0102r(unit, 0xf));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_set_fsrd_config(int unit)
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) || SOC_DFE_IS_FE2(unit))
    {
        if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_26 != -1)
        {
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, 8, 2, &reg_val32)); 
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val32, SRD_QUAD_N_LCREF_ENf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, 8, 2, reg_val32));       
        }
        if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_35 != -1)
        {
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, 11, 2, &reg_val32)); 
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val32, SRD_QUAD_N_LCREF_ENf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, 11, 2, reg_val32));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_set_ccs_config(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_fe3200_set_rtp_config(int unit)
{
    uint32 reg_val32;
    uint32 core_clock_speed;
    uint32 rtpwp;
    uint32 wp_at_core_clock_steps;
    SOCDNX_INIT_FUNC_DEFS;
    
    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed;

     
    wp_at_core_clock_steps = ((SOC_DFE_IMP_DEFS_GET(unit, rtp_reachabilty_watchdog_rate)/ 1000) * core_clock_speed ) / 1000 ;
    rtpwp = wp_at_core_clock_steps/4096;
    rtpwp = (rtpwp * 4096 < wp_at_core_clock_steps) ? rtpwp+1 : rtpwp; 

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr_REG32(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr, &reg_val32, RTPWPf, rtpwp);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr_REG32(unit, reg_val32));

    if (SOC_DFE_IS_FE13(unit)){
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
        soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_ENABLE_SLOW_LINK_DOWNf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    }


    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_fap_set(unit, SOC_FE3200_STK_MAX_MODULE));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_attach_links_to_default_fifo(int unit)
{
    int i, all_links[SOC_FE3200_NOF_LINKS];

    SOCDNX_INIT_FUNC_DEFS

    for (i = 0 ; i < SOC_DFE_DEFS_GET(unit, nof_links); i++)
    {
        all_links[i] = i;
    }

    SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_links_link_type_set(unit, bcmFabricPipeAll, soc_dfe_fabric_link_fifo_type_index_0, 1, 1, 1, 1, SOC_DFE_DEFS_GET(unit, nof_links), all_links));

exit:
    SOCDNX_FUNC_RETURN;

}

#define SOC_FE3200_DCH_REG_01BE_FIELD_4_24_FE3_DEF       (0x1200)
#define SOC_FE3200_DCH_REG_01BE_FIELD_4_24_FE1_FE2_DEF   (0x400)

STATIC int
soc_fe3200_set_dch_config(int unit)
{
    uint64 reg_val64;
    uint32 reg_val32;
    int  i, nof_instances_dch; 
    soc_dcmn_fabric_pipe_t pipes[3];
    soc_dfe_drv_dch_default_thresholds_t dch_thresholds_default_values;

    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    
    for (i=0; i < 3 ; i++)   
    {
        SOC_DCMN_FABRIC_PIPE_INIT(pipes[i]);
        SOC_DCMN_FABRIC_PIPE_SET(&(pipes[i]), i);
    }

    soc_fe3200_init_dch_thresholds_config(unit, &dch_thresholds_default_values);

    for (i=0; i < 3 ; i++)  
    {
        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dch_thresholds_default_values.fifo_size[i] ));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dch_thresholds_default_values.fifo_size[i] ));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_llfc_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dch_thresholds_default_values.llfc_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_llfc_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dch_thresholds_default_values.llfc_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dch_thresholds_default_values.mc_low_prio_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dch_thresholds_default_values.mc_low_prio_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dch_thresholds_default_values.full_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_rx_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dch_thresholds_default_values.full_threshold[i]));
    }

    
    for (i=0 ; i < nof_instances_dch ; i++) {     
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_2r(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_2r, &reg_val32, FIELD_0_7f, 0Xfd);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_2r(unit, i, reg_val32));
    }


    
    for (i=0 ; i < nof_instances_dch ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, &reg_val64, FIELD_36_36f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, i, reg_val64));

        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, &reg_val64, FIELD_36_36f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, i, reg_val64));

        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, i, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, &reg_val64, FIELD_36_36f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, i, reg_val64));
    }
   
    
    for (i=0 ; i < nof_instances_dch ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, CONFIG_0f, _SOC_FE3200_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
    }

    for (i=0 ; i < nof_instances_dch ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCH_PRIORITY_TRANSLATIONr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, FIELD_4_5f, 3);
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, FIELD_6_7f, 3);
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, FIELD_8_9f, 3);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_PRIORITY_TRANSLATIONr(unit, i, reg_val32));
    }

    
    if (SOC_DFE_IS_REPEATER(unit)) {
        for (i=0 ; i < nof_instances_dch; i++) {    
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_01BEr(unit, i, &reg_val32));
            soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_0_0f, 0X0); 
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_01BEr(unit, i, reg_val32));
        }

    } else if (SOC_DFE_IS_FE2(unit)) {
        for (i=0 ; i < nof_instances_dch; i++) {    
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_01BEr(unit, i, &reg_val32));
            soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_0_0f, 0X1); 
            soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_4_22f, SOC_FE3200_DCH_REG_01BE_FIELD_4_24_FE1_FE2_DEF);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_01BEr(unit, i, reg_val32));
        }
    } else if (SOC_DFE_IS_FE13(unit)) {
        
        for (i=0 ; i < nof_instances_dch/2; i++) {     
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_01BEr(unit, i, &reg_val32));
            soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_0_0f, 0X1); 
            soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_4_22f, SOC_FE3200_DCH_REG_01BE_FIELD_4_24_FE1_FE2_DEF);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_01BEr(unit, i, reg_val32));
        }

        
        if (SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system)
        {
            for (i=nof_instances_dch/2 ; i < nof_instances_dch; i++) {    
                SOCDNX_IF_ERR_EXIT(READ_DCH_REG_01BEr(unit, i, &reg_val32));
                soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_0_0f, 0X0); 
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_01BEr(unit, i, reg_val32));
            }
        } else {
            for (i=nof_instances_dch/2 ; i < nof_instances_dch; i++) {    
                SOCDNX_IF_ERR_EXIT(READ_DCH_REG_01BEr(unit, i, &reg_val32));
                soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_REG_01BEr, &reg_val32, FIELD_4_22f, SOC_FE3200_DCH_REG_01BE_FIELD_4_24_FE3_DEF);
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_01BEr(unit, i, reg_val32));
            }
        }
    }

           
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_fe3200_set_dcm_config(int unit)
{
    int i, nof_instances_dcm; 
    uint32 reg_val32;
    soc_dcmn_fabric_pipe_t pipes[3];
    soc_dfe_drv_dcm_default_thresholds_t dcm_thresholds_default_values;
    SOCDNX_INIT_FUNC_DEFS;

    for (i=0; i < 3 ; i++)   
    {
        SOC_DCMN_FABRIC_PIPE_INIT(pipes[i]);
        SOC_DCMN_FABRIC_PIPE_SET(&(pipes[i]), i);
    }

    nof_instances_dcm = SOC_DFE_DEFS_GET(unit, nof_instances_dcm);

    soc_fe3200_init_dcm_thresholds_config(unit, &dcm_thresholds_default_values);


    for (i=0 ; i < SOC_DFE_MAX_NOF_PIPES; i++) 
    {
       

        
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.fifo_size[i], 0));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.fifo_size[i], 0));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 
                                                                                        -1 , 1 , 1, dcm_thresholds_default_values.almost_full_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], -1 , 1 , 1, dcm_thresholds_default_values.almost_full_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio0Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_0_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio0Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_0_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio1Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_1_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio1Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_1_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio2Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_2_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio2Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_2_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio3Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_3_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio3Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.prio_3_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl1FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_low_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl1FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_low_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl2FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_med_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl2FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_med_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl3FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_high_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_gci_threshold_set(unit, bcmFabricLinkMidGciLvl3FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.gci_high_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl1FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_low_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl1FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_low_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl2FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_med_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl2FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_med_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl3FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_high_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_rci_threshold_set(unit, bcmFabricLinkMidRciLvl3FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.rci_high_threshold[i]));

        
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1 , 1, dcm_thresholds_default_values.full_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.full_threshold[i]));
        
        
    }

    
    if (SOC_DFE_IS_FE13(unit) && SOC_DFE_CONFIG(unit).fabric_local_routing_enable)
    {
        
        
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 0 , 0, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 0 , 1, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 1 , 0, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 1 , 1, soc_dfe_fabric_link_fifo_type_index_1));
        
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 2 , 2, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 2 , 3, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 3 , 2, soc_dfe_fabric_link_fifo_type_index_1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_type_set(unit, 3 , 3, soc_dfe_fabric_link_fifo_type_index_1));

        for (i=0 ; i < SOC_DFE_MAX_NOF_PIPES; i++) 
        {
            
            
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, dcm_thresholds_default_values.local_switch_fifo_size[i], 0));

            
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 
                                                                                            0 , 0 , 1, dcm_thresholds_default_values.local_switch_almost_full_0_threshold[i]));
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 
                                                                                            1 , 0 , 1, dcm_thresholds_default_values.local_switch_almost_full_1_threshold[i]));

             
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio0Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, SOC_FE3200_DCM_MAX_THRESHOLD));
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio1Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, SOC_FE3200_DCM_MAX_THRESHOLD));
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio2Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, SOC_FE3200_DCM_MAX_THRESHOLD));
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_prio_drop_threshold_set(unit, bcmFabricLinkMidPrio3Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, SOC_FE3200_DCM_MAX_THRESHOLD));

            
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_mid_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1 , 1, SOC_FE3200_DCM_MAX_THRESHOLD));
        }
        

    }

    
    for (i=0 ; i < nof_instances_dcm ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCM_PIPES_WEIGHTS_REGISTERr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCM_PIPES_WEIGHTS_REGISTERr, &reg_val32, CONFIG_0f, _SOC_FE3200_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SOCDNX_IF_ERR_EXIT(WRITE_DCM_PIPES_WEIGHTS_REGISTERr(unit, i, reg_val32));
    }
exit:
    SOCDNX_FUNC_RETURN;
}





STATIC int
soc_fe3200_set_dcl_config(int unit)
{
    int i, nof_instances_dcl; 
    uint32 reg_val32;
    soc_dcmn_fabric_pipe_t pipes[3];
    soc_dfe_drv_dcl_default_thresholds_t dcl_thresholds_default_values;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_dcl = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);

    for (i=0; i < 3 ; i++)   
    {
        SOC_DCMN_FABRIC_PIPE_INIT(pipes[i]);
        SOC_DCMN_FABRIC_PIPE_SET(&(pipes[i]), i);
    }
    soc_fe3200_init_dcl_thresholds_config(unit, &dcl_thresholds_default_values);


    for (i=0; i < 3; i++)
    {
        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.fifo_size[i], 0));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_fifo_size_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.fifo_size[i], 0));


        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.llfc_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_bypass_llfc_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.llfc_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.almost_full_threshold[i], 1));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_almost_full_threshold_set(unit, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.almost_full_threshold[i], 1));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl1FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.gci_low_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl1FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.gci_low_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl2FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.gci_med_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl2FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.gci_med_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl3FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.gci_high_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_gci_threshold_set(unit, bcmFabricLinkTxGciLvl3FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.gci_high_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl1FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.rci_low_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl1FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.rci_low_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl2FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.rci_med_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl2FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.rci_med_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl3FC, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.rci_high_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_rci_threshold_set(unit, bcmFabricLinkTxRciLvl3FC, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.rci_high_threshold[i]));

        

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio0Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.prio_0_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio0Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.prio_0_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio1Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.prio_1_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio1Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.prio_1_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio2Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.prio_2_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio2Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.prio_2_threshold[i]));

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio3Drop, soc_dfe_fabric_link_fifo_type_index_0, pipes[i], 1, 1, dcl_thresholds_default_values.prio_3_threshold[i]));
        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_flow_control_tx_drop_threshold_set(unit, bcmFabricLinkTxPrio3Drop, soc_dfe_fabric_link_fifo_type_index_1, pipes[i], 1, 1, dcl_thresholds_default_values.prio_3_threshold[i]));
    }

    
    for (i=0 ; i < nof_instances_dcl ; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_DCL_SHAPER_AND_WFQ_CFGr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCL_SHAPER_AND_WFQ_CFGr, &reg_val32, CONFIG_0f, _SOC_FE3200_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_SHAPER_AND_WFQ_CFGr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_set_eci_config(int unit)
{
    uint32 reg32_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;
    
    

    rv = READ_ECI_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if(SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system) {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, FIELD_1_2f, 1);
    } else if (SOC_DFE_CONFIG(unit).system_contains_multiple_pipe_device) {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, FIELD_1_2f, 2);
    } else {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32_val, FIELD_1_2f, 0);
    }
    rv = WRITE_ECI_GLOBAL_GENERAL_CFG_1r(unit, reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    
    
    SOCDNX_IF_ERR_EXIT(soc_fe3200_drv_pvt_monitor_enable(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_reset_device(int unit)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    
    rc =soc_fe3200_reset_cmic_iproc_regs(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_blocks_reset(unit, 0  , NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_pll_config_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    if (SOC_DFE_CONFIG(unit).run_mbist) {
         rc = soc_fe3200_bist_all(unit, SOC_DFE_CONFIG(unit).run_mbist - 1);
        SOCDNX_IF_ERR_EXIT(rc);
        
       
        rc =soc_fe3200_drv_blocks_reset(unit, 0  , NULL);
        SOCDNX_IF_ERR_EXIT(rc);

        

        
        rc =soc_fe3200_reset_cmic_iproc_regs(unit);
        SOCDNX_IF_ERR_EXIT(rc);

        
        rc = soc_fe3200_drv_blocks_reset(unit, 0  , NULL);
        SOCDNX_IF_ERR_EXIT(rc);

        
        rc = soc_fe3200_drv_pll_config_set(unit);
        SOCDNX_IF_ERR_EXIT(rc);

    }

    
    rc = soc_fe3200_interrupts_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_sbus_broadcast_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_soft_init(unit, SOC_DCMN_RESET_ACTION_IN_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_interrupts_disable(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_operation_mode(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    

    
    rc = soc_fe3200_reset_tables(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_fmac_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_fsrd_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_ccs_config(unit);
    SOCDNX_IF_ERR_EXIT(rc); 

    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_set_mesh_topology_config ,(unit));
    SOCDNX_IF_ERR_EXIT(rc);    

    
    rc = soc_fe3200_set_rtp_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    

    rc = soc_fe3200_attach_links_to_default_fifo(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_dch_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);    

    
    rc = soc_fe3200_set_dcm_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_dcl_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_set_eci_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

         
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set,(unit, soc_dfe_fabric_priority_0));
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_soft_init(unit, SOC_DCMN_RESET_ACTION_OUT_RESET);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe3200_drv_pll_config_set(int unit)
{
    int lcpll;
    uint32 reg32_val;
    soc_reg_above_64_val_t reg_above_64_val;
    int lcpll_in, lcpll_out;
    soc_reg_t pll_config_reg[] = {ECI_MISC_PLL_0_CONFIGr, ECI_MISC_PLL_1_CONFIGr, ECI_MISC_PLL_2_CONFIGr, ECI_MISC_PLL_3_CONFIGr};
    SOCDNX_INIT_FUNC_DEFS;


    
    SOCDNX_IF_ERR_EXIT(READ_ECI_POWERUP_CONFIGr_REG32(unit, &reg32_val));

    if (SHR_BITGET(&reg32_val, SOC_FE3200_PORT_ECI_POWER_UP_CONFIG_STATIC_PLL_BIT) && !SAL_BOOT_PLISIM)
    {
        
        SOC_EXIT;
    }


    for(lcpll=0 ; lcpll<SOC_DFE_DEFS_GET(unit, nof_lcpll) ; lcpll++) 
    {
        lcpll_in = SOC_DFE_CONFIG(unit).fabric_port_lcpll_in[lcpll];
        lcpll_out = SOC_DFE_CONFIG(unit).fabric_port_lcpll_out[lcpll];
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        switch(lcpll_out) {
            case soc_dcmn_init_serdes_ref_clock_125:
                SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_OUT_125_MHZ_WORD_0, 0);
                break;
            case soc_dcmn_init_serdes_ref_clock_156_25:
                SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_OUT_156_25_MHZ_WORD_0, 0);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("lcpll_out: %d is out-of-ranget (use 0=125MHz, 1=156.25MHz"), lcpll_out));
                break;
        }

        switch(lcpll_in) {
            case soc_dcmn_init_serdes_ref_clock_125:
                SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_IN_125_MHZ_WORD_1, 1);
                break;
            case soc_dcmn_init_serdes_ref_clock_156_25:
                SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_IN_156_25_MHZ_WORD_1, 1);
                break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("lcpll_out: %d is out-of-ranget (use 0=125MHz, 1=156.25MHz"), lcpll_out));
            break;
        }

        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_2, 2);
        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_3, 3);
        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_4, 4);
        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE1, 5);
        

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        
        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE2, 5);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));
        
        
        SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE3, 5);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_fe3200_drv_mdio_config_set(int unit)
{
    int dividend, divisor;
    int mdio_int_freq, mdio_delay;
    uint32 core_clock_speed_mhz;
    SOCDNX_INIT_FUNC_DEFS;
    
    

    
    if (SOC_DFE_CONFIG(unit).mdio_int_dividend == -1) 
    {
        
        dividend =  SOC_DFE_IMP_DEFS_GET(unit, mdio_int_dividend_default);
        SOC_DFE_CONFIG(unit).mdio_int_dividend = dividend;
    } else {
        dividend = SOC_DFE_CONFIG(unit).mdio_int_dividend;
    }

    if (SOC_DFE_CONFIG(unit).mdio_int_divisor == -1) 
    {
        
        mdio_int_freq = SOC_DFE_IMP_DEFS_GET(unit, mdio_int_freq_default);
        core_clock_speed_mhz = SOC_DFE_CONFIG(unit).core_clock_speed / 1000;

        divisor = core_clock_speed_mhz * dividend / (2* mdio_int_freq);
        SOC_DFE_CONFIG(unit).mdio_int_divisor = divisor;
    } else {
        divisor = SOC_DFE_CONFIG(unit).mdio_int_divisor;
    }
    


    mdio_delay = SOC_DFE_IMP_DEFS_GET(unit, mdio_int_out_delay_default);

    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_mdio_config(unit,dividend,divisor,mdio_delay));


exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_fe3200_drv_pvt_monitor_enable(int unit)
{
    uint64 reg64_val;
    soc_reg_t pvt_monitors[] = {ECI_PVT_MON_A_CONTROL_REGr, ECI_PVT_MON_B_CONTROL_REGr, ECI_PVT_MON_C_CONTROL_REGr, ECI_PVT_MON_D_CONTROL_REGr};
    int pvt_index;
    SOCDNX_INIT_FUNC_DEFS;

    
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_BITSET(reg64_val, _SOC_FE3200_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    
    COMPILER_64_BITSET(reg64_val, _SOC_FE3200_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_fe3200_drv_rings_map_set(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_0_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_1_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_2_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_3_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_4_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_5_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_6_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_7_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_8_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_9_VAL));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit,_SOC_FE3200_DRV_SBUS_RING_MAP_10_VAL));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_sbus_broadcast_config(int unit)
{
    int i;
    int nof_blocks;
    int broadcast_id;
    int disabled;

    SOCDNX_INIT_FUNC_DEFS;

    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_mac);
    for (i=0;i< nof_blocks;i++)
    {
        SOCDNX_IF_ERR_EXIT(soc_fe3200_drv_mac_broadcast_id_get(unit,i,&broadcast_id));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i,broadcast_id));

    }
    
    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd);
    for (i=0;i<nof_blocks;i++)
    {
        
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_quad_disabled, (unit, i * SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd), &disabled)));
        if (!disabled)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, _SOC_FE3200_DRV_BRDC_FSRD));
        }

    }

    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    for (i=0;i<nof_blocks;i++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_SBUS_BROADCAST_IDr(unit, i, _SOC_FE3200_DRV_BRDC_DCH));
    }

    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);
    for (i=0;i<nof_blocks;i++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_SBUS_BROADCAST_IDr(unit, i, _SOC_FE3200_DRV_BRDC_DCL));
    }

    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_dcm);
    for (i=0;i<nof_blocks;i++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_DCM_SBUS_BROADCAST_IDr(unit, i, _SOC_FE3200_DRV_BRDC_DCM)); 

    }

    
    nof_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_ccs);
    for (i=0;i<nof_blocks;i++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_SBUS_BROADCAST_IDr(unit, i, _SOC_FE3200_DRV_BRDC_CCS));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_mac_broadcast_id_get(int unit, int block_num,int *broadcast_id)
{
    
    int nof_mac_blocks;
    SOCDNX_INIT_FUNC_DEFS;

    nof_mac_blocks = SOC_DFE_DEFS_GET(unit, nof_instances_mac);

    if (block_num<0 || block_num >= nof_mac_blocks)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL , (_BSL_SOCDNX_MSG("Invalid mac block number")));
        
    }

    if (block_num<=8 || (block_num>=18 && block_num<=26)) 
    {
        *broadcast_id=_SOC_FE3200_DRV_BRDC_FMAC_AC_ID;
    }
    else
    {
        *broadcast_id=_SOC_FE3200_DRV_BRDC_FMAC_BD_ID;
    }




exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_soc_properties_validate(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}



int 
soc_fe3200_drv_soft_init(int unit, uint32 soft_reset_mode_flags)
{
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (soft_reset_mode_flags == SOC_DCMN_RESET_ACTION_IN_RESET || soft_reset_mode_flags == SOC_DCMN_RESET_ACTION_INOUT_RESET)
    {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
    }

    if (soft_reset_mode_flags == SOC_DCMN_RESET_ACTION_OUT_RESET || soft_reset_mode_flags == SOC_DCMN_RESET_ACTION_INOUT_RESET)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe3200_drv_test_reg_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch (reg) 
    {

       case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
       case MESH_TOPOLOGY_RESERVED_MTCPr:

       case ECI_BLOCKS_POWER_DOWNr:
       case CMIC_CPS_RESETr:
       case ECI_BLOCKS_SOFT_RESETr:
       case ECI_BLOCKS_SOFT_INITr:
       case ECI_BLOCKS_SBUS_RESETr:
       case FMAC_ASYNC_FIFO_CONFIGURATIONr:
       
       case ECI_INDIRECT_COMMANDr:
       case FSRD_INDIRECT_COMMANDr:
       case RTP_INDIRECT_COMMANDr:
       case DCL_INDIRECT_COMMANDr:
       
       case FMAC_SBUS_LAST_IN_CHAINr:
       case FSRD_SBUS_LAST_IN_CHAINr:
       case DCH_SBUS_LAST_IN_CHAINr:
       case DCM_SBUS_LAST_IN_CHAINr:
       case DCMC_SBUS_LAST_IN_CHAINr:
       case DCL_SBUS_LAST_IN_CHAINr:
       case CCS_SBUS_LAST_IN_CHAINr:
       case RTP_SBUS_LAST_IN_CHAINr:
       case MESH_TOPOLOGY_SBUS_LAST_IN_CHAINr:
       case ECI_SBUS_LAST_IN_CHAINr:
       case OCCG_REG_0087r:
       
       case DCL_INDIRECT_COMMAND_WR_DATAr:
       case ECI_INDIRECT_COMMAND_WR_DATAr:
       case FSRD_INDIRECT_COMMAND_WR_DATAr:
       case RTP_INDIRECT_COMMAND_WR_DATAr:
       
			 		
        
       case ECI_REG_0172r:
       case ECI_REG_016Cr:
       case ECI_PRM_PLL_CONTROL_STATUSr:
       
           *is_filtered = 1;
       default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}


int
soc_fe3200_drv_test_reg_default_val_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    
    switch(reg) {
     
      case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
      case MESH_TOPOLOGY_RESERVED_MTCPr:

       
       case ECI_BLOCKS_POWER_DOWNr:
       case CMIC_CPS_RESETr:
       case ECI_BLOCKS_SOFT_RESETr:
       case ECI_BLOCKS_SOFT_INITr:
       case ECI_BLOCKS_SBUS_RESETr:
       
       
       case ECI_INDIRECT_COMMANDr:
       case FSRD_INDIRECT_COMMANDr:
       case RTP_INDIRECT_COMMANDr:
       case DCL_INDIRECT_COMMANDr:

       
       case FMAC_ASYNC_FIFO_CONFIGURATIONr:

       
       case FMAC_SBUS_LAST_IN_CHAINr:
       case FSRD_SBUS_LAST_IN_CHAINr:
       case DCH_SBUS_LAST_IN_CHAINr:
       case DCM_SBUS_LAST_IN_CHAINr:
       case DCMC_SBUS_LAST_IN_CHAINr:
       case DCL_SBUS_LAST_IN_CHAINr:
       case CCS_SBUS_LAST_IN_CHAINr:
       case RTP_SBUS_LAST_IN_CHAINr:
       case MESH_TOPOLOGY_SBUS_LAST_IN_CHAINr:
       case ECI_SBUS_LAST_IN_CHAINr:
       case OCCG_REG_0087r:

       
       case FMAC_SBUS_BROADCAST_IDr:
       	
       
       case DCL_INDIRECT_COMMAND_WR_DATAr:
       case ECI_INDIRECT_COMMAND_WR_DATAr:
       case FSRD_INDIRECT_COMMAND_WR_DATAr:
       case RTP_INDIRECT_COMMAND_WR_DATAr:
       
			 		
        
       case ECI_REG_0172r:
       case ECI_REG_016Cr:
       case ECI_PRM_PLL_CONTROL_STATUSr:
       	
            *is_filtered = 1; 
        default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}


int
soc_fe3200_drv_test_mem_filter(int unit, soc_mem_t mem, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch(mem) 
    {
        case RTP_RMHMTm:
        case RTP_CUCTm:
        case RTP_DUCTPm:
        case RTP_MEM_1100000m:

        
        case FSRD_FSRD_WL_EXT_MEMm:
        case RTP_MULTI_CAST_TABLE_UPDATEm:
            *is_filtered = 1;
            break;
        default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}




int
soc_fe3200_drv_sw_ver_set(int unit)
{
    int         ver_val[3] = {0,0,0};
    uint32      regval, i, prev_regval;
    char        *ver;
    char        *cur_number_ptr;
    int         rc;
    int         wb, issu, bit_ndx ;
    uint32      prev_ver_val[3] = {0,0,0};

    SOCDNX_INIT_FUNC_DEFS;
   
    
    regval = 0;
    ver = _build_release;

    
    cur_number_ptr = sal_strstr(ver, "JR2");
    if(cur_number_ptr != NULL) {
        SOC_EXIT;
    }

    cur_number_ptr = sal_strchr(ver, '-');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid version format.")));

    }
    ++cur_number_ptr;
    ver_val[0] = _shr_ctoi (cur_number_ptr);
    cur_number_ptr = sal_strchr(cur_number_ptr, '.');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid version format.")));

    }
    ++cur_number_ptr;
    ver_val[1] = _shr_ctoi (cur_number_ptr);
    cur_number_ptr = sal_strchr(cur_number_ptr, '.');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid version format.")));
    }
    ++cur_number_ptr;
    ver_val[2] = _shr_ctoi (cur_number_ptr);

    SOCDNX_IF_ERR_EXIT(READ_ECI_SW_VERSIONr(unit, &prev_regval));
    wb = 0;
    issu = 0;
  
    if (SOC_WARM_BOOT(unit)) {
        wb = 1;
        bit_ndx = 28;

        for (i=0; i<3; i++) {
            
            prev_ver_val[i]= ( prev_regval >> (bit_ndx - i*4)) & 0xf;

            if (prev_ver_val[i] != ver_val[i]) {
                  issu = 1;
            }
            regval = (regval | (0xf & prev_ver_val[i])) << 4;
        }

    } else {
        for (i=0; i<3; i++) {
            regval = (regval | (0xf & ver_val[i])) << 4;
        }
    }


     
     for (i=0; i<3; i++) {
         if (issu) {
             regval = (regval | (0xf & ver_val[i]));
         }
         regval = regval << 4;
     }

     
     regval = (regval | (0xf & wb)) << 4;

     
     regval = (regval | (0xf & issu));


     SOC_DFE_ALLOW_WARMBOOT_WRITE(WRITE_ECI_SW_VERSIONr(unit, regval), rc);
     SOCDNX_IF_ERR_EXIT(rc);

exit:
     SOCDNX_FUNC_RETURN;;

}

int
soc_fe3200_drv_asymmetrical_quad_get(int unit, int link, int *asymmetrical_quad)
{
    int nof_quad;
    SOCDNX_INIT_FUNC_DEFS;

    nof_quad = INT_DEVIDE(link, SOC_FE3200_NOF_LINKS_IN_QUAD);

    if (nof_quad == SOC_FE3200_ASYMMETRICAL_FE13_QUAD_0)
    {
        *asymmetrical_quad = 0;
    }
    else if (nof_quad == SOC_FE3200_ASYMMETRICAL_FE13_QUAD_1)
    {
        *asymmetrical_quad = 1;
    }
    else
    {
        *asymmetrical_quad = -1;
    }

    SOCDNX_FUNC_RETURN;
}

int soc_fe3200_drv_reg_access_only_reset(int unit)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    
    rc =soc_fe3200_reset_cmic_iproc_regs(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_blocks_reset(unit, 0  , NULL);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_fe3200_drv_sbus_broadcast_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe3200_drv_test_brdc_blk_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch(reg) 
    {   
        
        case BRDC_FMAC_AC_ASYNC_FIFO_CONFIGURATIONr:
        case BRDC_FMAC_AC_SBUS_BROADCAST_IDr:
        case BRDC_FMAC_AC_SBUS_LAST_IN_CHAINr:
        case BRDC_FMAC_AC_GTIMER_TRIGGERr:
        
        case BRDC_FMAC_BD_ASYNC_FIFO_CONFIGURATIONr:
        case BRDC_FMAC_BD_SBUS_BROADCAST_IDr:
        case BRDC_FMAC_BD_SBUS_LAST_IN_CHAINr:
        case BRDC_FMAC_BD_GTIMER_TRIGGERr:
        
        case BRDC_FSRD_SBUS_BROADCAST_IDr:
        case BRDC_FSRD_SBUS_LAST_IN_CHAINr:
        case BRDC_FSRD_GTIMER_TRIGGERr:
        case BRDC_FSRD_ERROR_INITIATION_DATAr:
        case BRDC_FSRD_INDIRECT_COMMANDr:
        case BRDC_FSRD_INDIRECT_COMMAND_WR_DATAr:
        case BRDC_FSRD_INDIRECT_FORCE_BUBBLEr:

        case BRDC_FSRD_RESERVED_PCMI_0_Tr:
        case BRDC_FSRD_RESERVED_PCMI_1_Tr:
        case BRDC_FSRD_RESERVED_PCMI_2_Tr:
        case BRDC_FSRD_RESERVED_PCMI_4_Tr:


        
        case BRDC_DCH_SBUS_BROADCAST_IDr:
        case BRDC_DCH_SBUS_LAST_IN_CHAINr:
        case BRDC_DCH_GTIMER_TRIGGERr:
        case BRDC_DCH_DCH_ERROR_INITIATION_DATAr:
        case BRDC_DCH_ERROR_INITIATION_DATAr:
        case BRDC_DCH_GEN_ERR_MEMr:
        
        case BRDC_DCH_DCH_ENABLERS_REGISTER_1r:
        case BRDC_DCH_PRIORITY_TRANSLATIONr:
        case BRDC_DCH_RESERVED_PCMI_2_Tr:
        case BRDC_DCH_RESERVED_PCMI_4_Tr:
        
        case BRDC_DCM_SBUS_BROADCAST_IDr:
        case BRDC_DCM_SBUS_LAST_IN_CHAINr:
        case BRDC_DCM_GTIMER_TRIGGERr:
        case BRDC_DCM_ERROR_INITIATION_DATAr:

        case BRDC_DCM_RESERVED_PCMI_0_Tr:
        case BRDC_DCM_RESERVED_PCMI_1_Tr:
        case BRDC_DCM_RESERVED_PCMI_2_Tr:
        case BRDC_DCM_RESERVED_PCMI_4_Tr:


        
        case BRDC_DCL_SBUS_BROADCAST_IDr:
        case BRDC_DCL_SBUS_LAST_IN_CHAINr:
        case BRDC_DCL_GTIMER_TRIGGERr:
        case BRDC_DCL_ERROR_INITIATION_DATAr:
        case BRDC_DCL_INDIRECT_COMMANDr:
        case BRDC_DCL_RESERVED_PCMI_0_Tr:
        case BRDC_DCL_RESERVED_PCMI_1_Tr:

        case BRDC_DCL_TRANSMIT_DATA_CELL_TRIGGERr:
        case BRDC_DCL_INDIRECT_FORCE_BUBBLEr:
        
        case BRDC_CCS_SBUS_BROADCAST_IDr:
        case BRDC_CCS_SBUS_LAST_IN_CHAINr:
        case BRDC_CCS_GTIMER_TRIGGERr:
        case BRDC_CCS_CPU_SOURCE_CELL_TRIGGERr:
        case BRDC_CCS_ERROR_INITIATION_DATAr:
                *is_filtered = 1;
                break;
        default:
            break;
    }



    SOCDNX_FUNC_RETURN; 
}

int 
soc_fe3200_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size)
{
    int instance;
    int i;
    SOCDNX_INIT_FUNC_DEFS;
    *actual_size = 0;

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMAC_AC;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 4; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        for (instance = SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 2; instance < 3 * SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 4; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMAC_BD;
        for (i = 0, instance = SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 4; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 2; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        for (instance = 3 * SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 4; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (!(dcmn_device_block_for_feature(unit,DCMN_FABRIC_12_QUADS_FEATURE)) &&
        !(dcmn_device_block_for_feature(unit,DCMN_FABRIC_24_QUADS_FEATURE)) &&
        !(dcmn_device_block_for_feature(unit,DCMN_FABRIC_18_QUADS_FEATURE))) 
    {
        if (max_size > *actual_size)
        {
            brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FSRD;
            for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
            {
                brdc_info[*actual_size].blk_ids[i] = FSRD_BLOCK(unit, instance); 
            }
            brdc_info[*actual_size].blk_ids[i] = -1;

            (*actual_size)++;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
        }

    }
    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCH;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCH_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCM;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCM_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCL;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCL_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_CCS;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = CCS_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int 
soc_fe3200_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    int i;
    uint32 reg32_val;
    int peak, curr;
    soc_reg_t temp_reg[] = {ECI_PVT_MON_A_THERMAL_DATAr, ECI_PVT_MON_B_THERMAL_DATAr, ECI_PVT_MON_C_THERMAL_DATAr, ECI_PVT_MON_D_THERMAL_DATAr};
    soc_field_t curr_field[] = {THERMAL_DATA_Af, THERMAL_DATA_Bf, THERMAL_DATA_Cf, THERMAL_DATA_Df};
    soc_field_t peak_field[] = {PEAK_THERMAL_DATA_Af, PEAK_THERMAL_DATA_Bf, PEAK_THERMAL_DATA_Cf, PEAK_THERMAL_DATA_Df};
    
    SOCDNX_INIT_FUNC_DEFS;

    if (temperature_max < _SOC_FE3200_PVT_MON_NOF)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array size should be equal or bigger from %d.\n"), _SOC_FE3200_PVT_MON_NOF));
    }

    for (i = 0; i < _SOC_FE3200_PVT_MON_NOF; i++)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, temp_reg[i], REG_PORT_ANY, 0, &reg32_val));

        curr = soc_reg_field_get(unit, temp_reg[i], reg32_val, curr_field[i]);
        
        temperature_array[i].curr =  (_SOC_FE3200_PVT_BASE - curr * _SOC_FE3200_PVT_FACTOR) / 10000;

        peak = soc_reg_field_get(unit, temp_reg[i], reg32_val, peak_field[i]);
        
        temperature_array[i].peak = (_SOC_FE3200_PVT_BASE - peak * _SOC_FE3200_PVT_FACTOR) / 10000;
    }

    *temperature_count = _SOC_FE3200_PVT_MON_NOF;

exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_fe3200_drv_fe13_graceful_shutdown_set(int unit, soc_pbmp_t active_links, soc_pbmp_t unisolated_links, int shutdown) 
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_fe3200_drv_graceful_shutdown_set(unit, active_links, shutdown, unisolated_links, 1 ));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_graceful_shutdown_set(int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device) 
{
    int rv;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;


    if (shutdown)
    {
        if (SOC_DFE_IS_FE13(unit))
        {
            
            rv = soc_fe3200_drv_fe13_isolate_set(unit, unisolated_links, 1);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            
            rv = soc_fe3200_fabric_topology_isolate_set(unit, soc_dcmn_isolation_status_isolated); 
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dcmn_port_control_rx_enable_set(unit, port, SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE, 0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(20000); 

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dcmn_port_enable_set(unit, port, 0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(50000); 

    } else { 

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dcmn_port_enable_set(unit, port, 1);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_dcmn_port_control_rx_enable_set(unit, port, SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE, 1);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(1000000); 

        if (SOC_DFE_IS_FE13(unit))
        {
            
            rv = soc_fe3200_drv_fe13_isolate_set(unit, unisolated_links, 0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            
            if (!isolate_device) {
                rv = soc_fe3200_fabric_topology_isolate_set(unit, soc_dcmn_isolation_status_active); 
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_fe13_links_bitmap_get(int unit, soc_reg_above_64_val_t *all_links_bitmap, soc_reg_above_64_val_t *fap_links_bitmap, soc_reg_above_64_val_t *fe2_links_bitmap)
{
    soc_port_t port;
    int rv;
    soc_dfe_fabric_link_device_mode_t link_device_mode;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(*all_links_bitmap);
    SOC_REG_ABOVE_64_CLEAR(*fap_links_bitmap);
    SOC_REG_ABOVE_64_CLEAR(*fe2_links_bitmap);
    for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links); port++)
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_link_device_mode_get, (unit, port, 1, &link_device_mode));
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITSET(*all_links_bitmap, port);
        if (link_device_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
        {
            
            SHR_BITSET(*fap_links_bitmap, port);
        } else {
            
            SHR_BITSET(*fe2_links_bitmap, port);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_drv_fe13_isolate_set(int unit, soc_pbmp_t unisolated_links_pbmp, int isolate) 
{
    int rv;
    int ccs_index;
    soc_reg_above_64_val_t reg_above_64_val, all_links_bitmap, fap_links_bitmap, fe2_links_bitmap, unisolated_links;
    uint32 reg32, rtp_stop_en;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_fe3200_drv_fe13_links_bitmap_get(unit, &all_links_bitmap, &fap_links_bitmap, &fe2_links_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(unisolated_links);
    
    SOC_PBMP_ITER(unisolated_links_pbmp, port)
    {
        
        SHR_BITSET(unisolated_links, port);
    }

    if (isolate)
    {
        
        rv = READ_RTP_RESERVED_10r(unit, &reg32);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rtp_stop_en = soc_reg_field_get(unit, RTP_RESERVED_10r, reg32, FIELD_0_7f);

        soc_reg_field_set(unit, RTP_RESERVED_10r, &reg32, FIELD_0_7f, 0x0);
        rv = WRITE_RTP_RESERVED_10r(unit, reg32);
        SOCDNX_IF_ERR_EXIT(rv);

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));
        SOC_REG_ABOVE_64_AND(reg_above_64_val, fe2_links_bitmap);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_CCS_REG_0102r(unit, ccs_index, reg_above_64_val));
            SOC_REG_ABOVE_64_AND(reg_above_64_val, fe2_links_bitmap);
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0102r(unit, ccs_index, reg_above_64_val));
        }       
        
        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0102r(unit, ccs_index, reg_above_64_val));
        }       
        
        sal_usleep(30000); 

        
        rv = READ_RTP_RESERVED_10r(unit, &reg32);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, RTP_RESERVED_10r, &reg32, FIELD_0_7f, rtp_stop_en);
        rv = WRITE_RTP_RESERVED_10r(unit, reg32);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(1000000); 

        
    } else { 

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_topology_mesh_topology_reset(unit));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_COPY(reg_above_64_val, fe2_links_bitmap);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0102r(unit, ccs_index, reg_above_64_val));
        } 
       
        sal_usleep(30000); 

        SOC_REG_ABOVE_64_COPY(reg_above_64_val, fe2_links_bitmap);
        SOC_REG_ABOVE_64_AND(reg_above_64_val, unisolated_links);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_COPY(reg_above_64_val, all_links_bitmap);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0102r(unit, ccs_index, reg_above_64_val));
        }
        
        sal_usleep(30000); 

        SOC_REG_ABOVE_64_COPY(reg_above_64_val, all_links_bitmap);
        SOC_REG_ABOVE_64_AND(reg_above_64_val, unisolated_links);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe3200_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE3200_ONLY(unit);

    SOCDNX_NULL_CHECK(nof_block_instances);
    SOCDNX_NULL_CHECK(block_types);

    switch(block_types[0]) {
        case SOC_BLK_FMAC:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_mac); 
            break;
        case SOC_BLK_FSRD:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd);
            break;
        case SOC_BLK_DCH:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
            break;
        case SOC_BLK_CCS:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_CCS;
            break;
        case SOC_BLK_DCL:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);
            break;
        case SOC_BLK_RTP:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_RTP;
            break;
        case SOC_BLK_OCCG:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_OCCG;
            break;
        case SOC_BLK_ECI:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_ECI;
            break;
        case SOC_BLK_DCM:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_DCM;
            break;
        case SOC_BLK_DCMC:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_DCMC;
            break;
        case SOC_BLK_CMIC:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_CMIC;
            break;
        case SOC_BLK_MESH_TOPOLOGY:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_MESH_TOPOLOGY;
            break;
        case SOC_BLK_OTPC:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_OTPC;
            break;
        case SOC_BLK_BRDC_FMACH:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_brdc_fmach); 
            break;
        case SOC_BLK_BRDC_FMACL:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_BRDC_FMACL;
            break;
        case SOC_BLK_BRDC_FSRD:
            *nof_block_instances = SOC_FE3200_NOF_INSTANCES_BRDC_FSRD;
            break;

        default:
            *nof_block_instances = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_fe3200_drv_mbist(int unit, int skip_errors)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DFE_DRV_INIT_LOG(unit, "Memory Bist");
    rc = soc_fe3200_bist_all(unit, skip_errors);
    SOCDNX_IF_ERR_EXIT(rc);
        
exit:
    SOCDNX_FUNC_RETURN;   
}

int
soc_fe3200_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp)
{   
    int first_link = 0,range = 0;
    int first_link_part_2 = 0, range_part_2 = 0;


    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*pbmp);

    switch (block_type)
    {
       case SOC_BLK_DCH:
       case SOC_BLK_DCM:
           first_link = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) * blk_instance;
           if (SOC_DFE_IS_FE13_ASYMMETRIC(unit))
           {
               if (blk_instance < SOC_DFE_DEFS_GET(unit, nof_instances_dch) / 2)
               {
                   range = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);
                   first_link_part_2 = blk_instance ? _SOC_FE3200_DRV_FE13_ASYM_FAP_FIRST_LINK_QUARTER_1_PART_2 : _SOC_FE3200_DRV_FE13_ASYM_FAP_FIRST_LINK_QUARTER_0_PART_2;
                   range_part_2 = _SOC_FE3200_DRV_FE13_ASYM_FAP_RANGE_PART_2;
               } else {
                   range = _SOC_FE3200_DRV_FE13_ASYM_FE2_RANGE;
               }
           } else {
               range = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);
           }
           
           break;
       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("block (%d) - block pbmp is not supported"), block_type));
           break;
    }

    SOC_PBMP_PORTS_RANGE_ADD(*pbmp, first_link, range);
    SOC_PBMP_PORTS_RANGE_ADD(*pbmp, first_link_part_2, range_part_2);
    SOC_PBMP_AND(*pbmp, PBMP_SFI_ALL(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_fe3200_tbl_is_dynamic(int unit, soc_mem_t mem) {

    

    if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem)) {
        return TRUE;
    }

    switch (mem) {
    case RTP_DUCTPm:
    case RTP_CUCTm:
    case RTP_RMHMTm:
    case RTP_MULTI_CAST_TABLE_UPDATEm:
        if(!SOC_DFE_IS_REPEATER(unit)) {
            return TRUE;
        } else {
            return FALSE;
        }
    case RTP_SLSCTm:
        if (soc_dfe_load_balancing_mode_destination_unreachable != SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
            return TRUE;
        } else {
            return FALSE;
        }

    default:
        return FALSE;
    }
}

int
soc_fe3200_drv_block_valid_get(int unit, int blktype, int blockid, char *valid)
{
    int rv;
    int disabled = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (blktype == SOC_BLK_FSRD) {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_quad_disabled, (unit, blockid * SOC_DFE_DEFS_GET(unit, nof_quads_in_fsrd), &disabled));
        SOCDNX_IF_ERR_EXIT(rv);
        *valid = ((disabled == 0) ? 1 : 0);
    } else if (blktype == SOC_BLK_BRDC_FSRD &&
               (SOC_IS_BCM88773(unit) || SOC_IS_BCM88774(unit))) {
        *valid = 0;
    } else {
        *valid  = 1;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


#undef _ERR_MSG_MODULE_NAME
