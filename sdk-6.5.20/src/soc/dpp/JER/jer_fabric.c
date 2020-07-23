#include <soc/mcm/memregs.h>
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <shared/bsl.h>
#include "bcm_int/common/debug.h"
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/register.h>

#include <soc/dcmn/error.h>
#include <soc/sand/sand_mem.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_pll_synce.h>





#define SOC_JER_FABRIC_SIMPLE_PACKING_MODID (2045)
#define SOC_JER_FABRIC_SYNCE_NOF_CLKS       (2)
#define SOC_JER_FABRIC_SYNCE_CFG_MASTER_SEL (156)
#define SOC_JER_FABRIC_SYNCE_CFG_SLAVE_SEL  (157)
#define SOC_JER_FABRIC_RX_FC_TH_MAX         (127)

#define JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP 16











typedef enum soc_fabric_pipe_strict_priority_2_type_e
{
    SOC_FABRIC_PIPE_STRICT_PRIORITY_2_NONE = 0x0,
    SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_1 = 0x1,
    SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_2 = 0x2,
    SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_3 = 0x4
} soc_fabric_pipe_strict_priority_2_type_t ;




STATIC CONST int soc_jer_fabric_all_reachable_vector_exclude_regs[] = {
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_0r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_1r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_2r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_3r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_4r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_5r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_6r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_7r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_8r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_9r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_10r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_11r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_12r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_13r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_14r,
    RTP_EXCLUDE_DEST_ID_FOR_MC_LINKS_15r
};

STATIC CONST int soc_jer_fabric_all_reachable_vector_exclude_fields[] = {
    EXCLUDE_DEST_ID_0f,
    EXCLUDE_DEST_ID_1f,
    EXCLUDE_DEST_ID_2f,
    EXCLUDE_DEST_ID_3f,
    EXCLUDE_DEST_ID_4f,
    EXCLUDE_DEST_ID_5f,
    EXCLUDE_DEST_ID_6f,
    EXCLUDE_DEST_ID_7f,
    EXCLUDE_DEST_ID_8f,
    EXCLUDE_DEST_ID_9f,
    EXCLUDE_DEST_ID_10f,
    EXCLUDE_DEST_ID_11f,
    EXCLUDE_DEST_ID_12f,
    EXCLUDE_DEST_ID_13f,
    EXCLUDE_DEST_ID_14f,
    EXCLUDE_DEST_ID_15f
};

STATIC CONST int soc_jer_fabric_all_reachable_vector_exclude_fields_valid[] = {
    VALID_0f,
    VALID_1f,
    VALID_2f,
    VALID_3f,
    VALID_4f,
    VALID_5f,
    VALID_6f,
    VALID_7f,
    VALID_8f,
    VALID_9f,
    VALID_10f,
    VALID_11f,
    VALID_12f,
    VALID_13f,
    VALID_14f,
    VALID_15f
};

STATIC CONST uint32 fda_egq_pipe_2_fifo_mapping[] = {0x0c42211, 0x000a651, 0x1c22111, 0x000a251 , 0x0, 0x0, 0x100a151};








soc_error_t
  jer_fabric_gci_backoff_masks_init(
    SOC_SAND_IN  int                 unit
  )
{

  soc_reg_t gci_backoff_mask_lut_1_reg = (SOC_IS_QAX(unit) ? PTS_GCI_BACKOFF_MASK_LUT_1r : IPT_GCI_BACKOFF_MASK_LUT_1r);
  soc_reg_t gci_backoff_mask_lut_2_reg = (SOC_IS_QAX(unit) ? PTS_GCI_BACKOFF_MASK_LUT_2r : IPT_GCI_BACKOFF_MASK_LUT_2r);
  soc_reg_t gci_backoff_mask_lut_3_reg = (SOC_IS_QAX(unit) ? PTS_GCI_BACKOFF_MASK_LUT_3r : IPT_GCI_BACKOFF_MASK_LUT_3r);

  SOCDNX_INIT_FUNC_DEFS;

  
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_1_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_0_1f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_0));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_1_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_1_1f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_1));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_1_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_2_1f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_2));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_1_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_3_1f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_3));


  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_2_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_0_2f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_0));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_2_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_1_2f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_1));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_2_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_2_2f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_2));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_2_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_3_2f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_3));

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_3_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_0_3f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_0));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_3_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_1_3f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_1));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_3_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_2_3f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_2));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, gci_backoff_mask_lut_3_reg,  REG_PORT_ANY, 0, GCI_BACKOFF_MASK_3_3f,  ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_3));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_interleaving_init(
    SOC_SAND_IN  int                 unit
  )
{
    uint32 reg_val;
    uint64 reg64_val;
    int is_mesh, is_mesh_mc, context_field_val = 0x3F;
    int tdm_pipe_field_val;
    soc_dcmn_fabric_pipe_map_type_t map_type;
    SOCDNX_INIT_FUNC_DEFS;

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);
    map_type = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.mapping_type;

    if (!SOC_IS_QAX(unit)) { 
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val));
        if (is_mesh_mc || !is_mesh) { 
            soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg_val, FMC_ENf, 1);
        }
        else { 
            soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg_val, FMC_ENf, 0);    
        }
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_SEGMENTATION_AND_LLFC_MAPPINGr(unit, &reg_val)); 
    soc_reg_field_set(unit, FDT_FDT_SEGMENTATION_AND_LLFC_MAPPINGr, &reg_val, SEGMENT_PKTf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_SEGMENTATION_AND_LLFC_MAPPINGr(unit, reg_val));

    
    switch (map_type) {
    case soc_dcmn_fabric_pipe_map_triple_uc_mc_tdm:
        tdm_pipe_field_val = 0x4; 
        break;
    case soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm:
        tdm_pipe_field_val = 0x2; 
        break;
    
    default:
        tdm_pipe_field_val = 0x0; 
    }
    SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_CONTEXTS_FABRIC_MAPPINGr(unit, &reg_val)); 
    soc_reg_field_set(unit, FDT_FDT_CONTEXTS_FABRIC_MAPPINGr, &reg_val, FABRIC_TDM_PIPE_SELECTf, tdm_pipe_field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_CONTEXTS_FABRIC_MAPPINGr(unit, reg_val));

    
    if (!is_mesh_mc ) {
        SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_CONTEXTS_INTERLEAVINGr(unit, &reg64_val)); 
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_0_ENf, context_field_val); 
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_1_ENf, context_field_val);
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_2_ENf, context_field_val);
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_3_ENf, context_field_val);
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_4_ENf, context_field_val);
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_5_ENf, context_field_val);
        soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IRE_ENf, context_field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_CONTEXTS_INTERLEAVINGr(unit, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_flow_control_init(
    SOC_SAND_IN  int              unit
  )
{
    uint32 link_i, reg_val;
    soc_gport_t gport = 0;
    int core, i;
    ARAD_FABRIC_FC fc;
    soc_dpp_fabric_rci_config_t    rci_config;
    int is_mesh_hybrid = 0, nof_fabric_pms;
    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_FABRIC_FC_clear(unit, &fc);

    
    for (link_i = 0; link_i < SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_i) {
       
       fc.enable[link_i] = TRUE;
    }
    SOCDNX_SAND_IF_ERR_EXIT(arad_fabric_fc_enable_set_unsafe(unit, ARAD_CONNECTION_DIRECTION_BOTH, &fc));

    
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[0] = 0xc8;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[1] = 0x12c;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[2] = 0x190;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[3] = 0x1f4;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[4] = 0x258;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[5] = 0x2bc;
    rci_config.soc_dpp_fabric_rci_device_level_thresholds[6] = 0x320;

    if (SOC_IS_QAX(unit)) {
        is_mesh_hybrid = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_enable", 0);
        if (is_mesh_hybrid) {
            nof_fabric_pms = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_nof_fabric_quads", SOC_QAX_MESH_HYBRID_PM_FABRIC);
            for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
                rci_config.soc_dpp_fabric_rci_device_level_thresholds[i] =  rci_config.soc_dpp_fabric_rci_device_level_thresholds[i] * nof_fabric_pms / SOC_QAX_PM_FABRIC;
            }
        }
    }

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[0] = 0xc8;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[1] = 0x12c;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[2] = 0x190;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[3] = 0x1f4;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[4] = 0x258;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[5] = 0x2bc;
        rci_config.soc_dpp_fabric_rci_core_level_thresholds[6] = 0x320;
    } else { 
        for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
            rci_config.soc_dpp_fabric_rci_core_level_thresholds[i] = 0x0; 
        }
    }
    rci_config.soc_dpp_fabric_rci_severity_factors[0] = SOC_DPP_IMP_DEFS_GET(unit, rci_severity_factor_1); 
    rci_config.soc_dpp_fabric_rci_severity_factors[1] = SOC_DPP_IMP_DEFS_GET(unit, rci_severity_factor_2);
    rci_config.soc_dpp_fabric_rci_severity_factors[2] = SOC_DPP_IMP_DEFS_GET(unit, rci_severity_factor_3);

    rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue = 0x1f4;
    rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue = 0x64;

    
    rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links = SOC_DPP_IMP_DEFS_GET(unit, rci_threshold_nof_congested_links);
    
    rci_config.soc_dpp_fabric_rci_high_score_congested_links = 0x3e8;

    if (!(SOC_IS_FLAIR(unit) && SAL_BOOT_PLISIM)) {
        SOCDNX_SAND_IF_ERR_EXIT(soc_jer_fabric_rci_thresholds_config_set(unit, rci_config));

        SOCDNX_SAND_IF_ERR_EXIT(soc_jer_fabric_link_thresholds_pipe_set(unit, socDppFabricPipeAll, socDppFabricLinkRxRciLvl1FC, 0x40));
        SOCDNX_SAND_IF_ERR_EXIT(soc_jer_fabric_link_thresholds_pipe_set(unit, socDppFabricPipeAll, socDppFabricLinkRxRciLvl2FC, 0x40));
        SOCDNX_SAND_IF_ERR_EXIT(soc_jer_fabric_link_thresholds_pipe_set(unit, socDppFabricPipeAll, socDppFabricLinkRxRciLvl3FC, 0x40));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_jer_fabric_link_thresholds_pipe_set(unit, socDppFabricPipeAll, socDppFabricLinkRxFifoLLFC, 0x50));

    
    if (SOC_IS_JERICHO_AND_BELOW(unit)){ 
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_cosq_gport_rci_threshold_set, (unit, gport, 0x50, soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast)));
    }

    if (!SOC_IS_FLAIR(unit)) {
        
        for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); core ++) {
            SOCDNX_IF_ERR_EXIT(READ_SCH_RCI_PARAMSr(unit, core, &reg_val)); 
            soc_reg_field_set(unit, SCH_RCI_PARAMSr, &reg_val, RCI_ENAf, 1);
            soc_reg_field_set(unit, SCH_RCI_PARAMSr, &reg_val, DEVICE_RCI_ENAf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_RCI_PARAMSr(unit, core, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_scheduler_adaptation_init(
    SOC_SAND_IN int    unit
  )
{
    int core_i;
    uint32 reg_val32;
    int link_to_core_mode = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (link_to_core_mode == SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_DEDICATED) {
        SOCDNX_IF_ERR_EXIT(READ_SCH_LINKS_MASKr(unit, 0, 1, &reg_val32));
        soc_reg_field_set(unit, SCH_LINKS_MASKr, &reg_val32, LINKS_MASK_Nf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_LINKS_MASKr(unit, 0, 1, reg_val32));

        SOCDNX_IF_ERR_EXIT(READ_SCH_LINKS_MASKr(unit, 1, 0, &reg_val32));
        soc_reg_field_set(unit, SCH_LINKS_MASKr, &reg_val32, LINKS_MASK_Nf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_LINKS_MASKr(unit, 1, 0, reg_val32));
    } else if (link_to_core_mode == SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED) {
        
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_i) {
            SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_CREDIT_SHARE_CONFIGURATIONr(unit, core_i, &reg_val32));
            if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
                soc_reg_field_set(unit, SCH_DVS_CREDIT_SHARE_CONFIGURATIONr, &reg_val32, ENABLE_DVS_CREDITS_SHAREf, 1);
            } else {
                soc_reg_field_set(unit, SCH_DVS_CREDIT_SHARE_CONFIGURATIONr, &reg_val32, ENABLE_DVS_CREDITS_SHAREf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_DVS_CREDIT_SHARE_CONFIGURATIONr(unit, core_i, reg_val32));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_STATE_FOR_SC_HAND_FDTr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINK_REDUCTION_MCf, 0);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_0_PIPEID_Af, 1);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_0_PIPEID_Bf, 0);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_1_PIPEID_Af, 0); 
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_1_PIPEID_Bf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LINK_STATE_FOR_SC_HAND_FDTr(unit, reg_val32));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_fabric_regs_init(
    SOC_SAND_IN  int         unit
  )
{
    uint32 reg_val;
    uint64 reg64;
    uint32 reg32;
    int fabric_priority;
    int fabric_links_core_map_mode;
    int is_mesh, is_mesh_mc, is_stand_alone;
    int i, nof_instances_fmac;
    int field_val=0;
    soc_reg_above_64_val_t reg_above_64;
    soc_dcmn_fabric_pipe_map_type_t fabric_pipe_map_type;
    uint32 nof_fabric_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
    uint32 nof_sch_active_links = SOC_DPP_DEFS_GET(unit, nof_sch_active_links);
    uint32 sch_number_of_links=0;
    int quad, blk_ins;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(arad_fabric_common_regs_init(unit));

    
    SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_ENABLER_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, FIELD_24_24f, 1);
    soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, FIELD_25_25f, 1);
    soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, FIELD_2_2f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, reg_val));

    if (SOC_IS_JERICHO_PLUS(unit)){

        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_4_11f, 0xfd);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));

        SOCDNX_IF_ERR_EXIT(READ_FDR_REG_0100_1r(unit, &reg32));
        soc_reg_field_set(unit, FDR_REG_0100_1r, &reg32, FIELD_0_18f, 0x800);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_REG_0100_1r(unit, reg32));

        SOCDNX_IF_ERR_EXIT(READ_FDR_REG_0100_2r(unit, &reg32));
        soc_reg_field_set(unit, FDR_REG_0100_2r, &reg32, FIELD_0_18f, 0x800);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_REG_0100_2r(unit, reg32));

        SOCDNX_IF_ERR_EXIT(READ_FDR_REG_0100_3r(unit, &reg32));
        soc_reg_field_set(unit, FDR_REG_0100_3r, &reg32, FIELD_0_18f, 0x800);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_REG_0100_3r(unit, reg32));
    } else { 
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64));
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_0_18f, 0x800);
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_23_30f, 0xfd);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));
    }

    
    fabric_priority = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;
    SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_ENABLERSr(unit, &reg_val));
    soc_reg_field_set(unit, FDA_FDA_ENABLERSr, &reg_val, TDM_HEADER_PRIORITYf, fabric_priority);
    SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_ENABLERSr(unit, reg_val));

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MC_TRAVERSE_RATEr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MC_TRAVERSE_RATEr, &reg_val, MC_TRAVERSE_RATEf, 0x1800);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MC_TRAVERSE_RATEr(unit, reg_val));

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        SOCDNX_IF_ERR_EXIT(READ_FDA_REG_0153r(unit, &reg_val));
        soc_reg_field_set(unit, FDA_REG_0153r, &reg_val, FIELD_0_0f, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_REG_0153r(unit, reg_val));
    }

    
    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) { 
        fabric_links_core_map_mode = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode;
        if (fabric_links_core_map_mode == SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_DEDICATED) {
            SOCDNX_IF_ERR_EXIT(READ_RTP_RTP_ENABLEr_REG32(unit, &reg_val));
            soc_reg_field_set(unit, RTP_RTP_ENABLEr, &reg_val, FABRIC_18_18f, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RTP_ENABLEr_REG32(unit, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_FDR_FABRIC_18_18r(unit, &reg_val));
            soc_reg_field_set(unit, FDR_FABRIC_18_18r, &reg_val, FABRIC_18_18f, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FDR_FABRIC_18_18r(unit, reg_val));
        }
    }else{ 
        SOCDNX_IF_ERR_EXIT(READ_RTP_RTP_ENABLEr_REG32(unit, &reg_val));
        soc_reg_field_set(unit, RTP_RTP_ENABLEr, &reg_val, FABRIC_18_18f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RTP_ENABLEr_REG32(unit, reg_val));
    }

    
    if (!SOC_DPP_IS_MESH(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_ENABLER_REGISTERr(unit, &reg_val));
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, MIN_NOF_LINKS_ENf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, reg_val));
    } else {
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val));
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val, MULTI_FAPf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val));
    }

    if (SOC_IS_JERICHO_PLUS(unit)) 
    {
        soc_reg_t regs[3];
        regs[0] = SOC_IS_JERICHO_PLUS_ONLY(unit)? FDR_REG_012A_1r : FDR_REG_011D_1r;
        regs[1] = SOC_IS_JERICHO_PLUS_ONLY(unit)? FDR_REG_012A_2r : FDR_REG_011D_2r;
        regs[2] = SOC_IS_JERICHO_PLUS_ONLY(unit)? FDR_REG_012A_3r : FDR_REG_011D_3r;

        if(SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system)
        {
            soc_field_t fields [1] = {FIELD_0_0f};
            uint32 values [1] = {0x0};
            uint32 field_count = 1;
            for (i = 0; i < 3; ++i) {
                SOCDNX_IF_ERR_EXIT(soc_reg_fields32_modify(unit, regs[i], REG_PORT_ANY, field_count, fields, values));
            }
        }else {
            soc_field_t fields [2] = {FIELD_0_0f, FIELD_4_22f};
            uint32 values [2] = {0x1, 0x1800};
            uint32 field_count = 2;
            for (i = 0; i < 3; ++i) {
                SOCDNX_IF_ERR_EXIT(soc_reg_fields32_modify(unit, regs[i], REG_PORT_ANY, field_count, fields, values));
            }
        }
    } else {
        SOCDNX_IF_ERR_EXIT(READ_FDR_REG_011Er(unit, &reg_val));
        if(SOC_DPP_CONFIG(unit)->arad->init.fabric.is_128_in_system) {
            soc_reg_field_set(unit, FDR_REG_011Er, &reg_val, FIELD_0_0f, 0x0);
        } else {
            soc_reg_field_set(unit, FDR_REG_011Er, &reg_val, FIELD_0_0f, 0x1);
            soc_reg_field_set(unit, FDR_REG_011Er, &reg_val, FIELD_4_22f, 0x1800);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_REG_011Er(unit, reg_val));
    }
    
    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);
    is_stand_alone = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP);

    if (!SOC_IS_QAX(unit)) { 
        if (is_mesh && is_mesh_mc) {
            SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg_val));
            soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg_val, FMC_ENf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg_val));
        }
    }

    nof_instances_fmac = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    if ((!is_mesh && !is_stand_alone) || soc_feature(unit, soc_feature_packet_tdm_marking))
    {
        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(READ_BRDC_FMAC_LINK_TOPO_MODE_REG_0r(unit, &reg_val));
            soc_reg_field_set(unit, BRDC_FMAC_LINK_TOPO_MODE_REG_0r, &reg_val, LINK_TOPO_MODE_1f, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_TOPO_MODE_REG_0r(unit, reg_val));
        } else {
            for ( i = 0; i < nof_instances_fmac; ++i) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, i, &reg_val));
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val, LINK_TOPO_MODE_1f, 0x0);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, i, reg_val));
            }
        }
    }

    
    if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit)) {
        reg_val = 1;
        SOCDNX_IF_ERR_EXIT(WRITE_IPST_FMC_TOP_SHAPER_CONFIGr(unit, reg_val));
    }

    
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_INITr(unit,  0xd2d));


    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_REG_0102r(unit, 0xf));
    } else {
        for ( i = 0; i < nof_instances_fmac; ++i) {
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_REG_0102r(unit, i, 0xf));
        }
    }

    
    for ( i = 0; i < nof_instances_fmac; ++i) {
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_REG_0102r(unit, REG_PORT_ANY, 0xf));

        SOCDNX_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, i, &reg_val)); 
        soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg_val, TX_WIDE_INTERFACEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, i, reg_val));
    }

    if (!(SOC_IS_FLAIR(unit) && SAL_BOOT_PLISIM)) {
        
        
        SOCDNX_IF_ERR_EXIT(READ_FCR_EFMS_CONFIGURATIONr(unit, reg_above_64));
        soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, reg_above_64, EFMS_AGING_TIMERf, 0x4);
        soc_reg_above_64_field32_set(unit, FCR_EFMS_CONFIGURATIONr, reg_above_64, EFMS_BYPASS_ENABLEf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FCR_EFMS_CONFIGURATIONr(unit, reg_above_64));
    }
    if (!SOC_IS_FLAIR(unit)) {
        
        
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, i) {
            SOCDNX_IF_ERR_EXIT(READ_IPS_FMS_PARAMETERSr(unit, i, &reg_val));
            soc_reg_field_set(unit, IPS_FMS_PARAMETERSr, &reg_val, FMS_ENABLE_BYPf, 0x1);
            SOCDNX_IF_ERR_EXIT(WRITE_IPS_FMS_PARAMETERSr(unit, i, reg_val));
        }
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) { 

        for (i=0 ; i < (nof_fabric_links + 1); i++)
        {
            sch_number_of_links = (i * nof_sch_active_links) / nof_fabric_links;
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_SCH_LINK_UP_CALCr(unit, i, sch_number_of_links));
        }
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FDT_REG_0244r(unit, &reg_val));
        field_val = ((SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH)
              || (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP)) ? 0x0 : 0x1;
        soc_reg_field_set(unit, FDT_REG_0244r, &reg_val, FIELD_0_0f, field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_REG_0244r(unit, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_FDT_REG_0242r(unit, &reg_val));
        field_val = ((SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH)
              || (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP)) ? 0x0 : 0x1;
        soc_reg_field_set(unit, FDT_REG_0242r, &reg_val, FIELD_0_0f, field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_REG_0242r(unit, reg_val));
    }

    
    fabric_pipe_map_type = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.mapping_type;
    if ((fabric_pipe_map_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc) && SOC_IS_JERICHO_AND_BELOW(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDA_FAB_18_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64));
        soc_reg_above_64_field32_set(unit, FDA_FAB_18_SCH_WFQ_CFGr, reg_above_64, FAB_18_SCH_WFQ_BACKPRESSURE_ENABLEf, 0x7);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDA_FAB_18_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDA_FAB_36_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64));
        soc_reg_above_64_field32_set(unit, FDA_FAB_36_SCH_WFQ_CFGr, reg_above_64, FAB_36_SCH_WFQ_BACKPRESSURE_ENABLEf, 0x7);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDA_FAB_36_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64));
    }

    if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, &reg32));
        soc_reg_field_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, &reg32, IPT_WFQ_ENf, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY,  0, reg32));

        
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, RTP_RTP_ENABLEr, REG_PORT_ANY, 0, &reg32));
        soc_reg_field_set(unit, RTP_RTP_ENABLEr, &reg32, RTPRTf, 0x40);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, RTP_RTP_ENABLEr, REG_PORT_ANY,  0, reg32));
    }

    
    if (SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_RESCAL_STATUSr(unit, &reg_val));
        field_val = soc_reg_field_get(unit, ECI_RESCAL_STATUSr, reg_val, PONf);

        for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
            for(quad=0 ; quad<SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd) ; quad++) {
                SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, &reg_val));
                soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RESCAL_OVERRIDE_ENf, 1);
                soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RESCAL_OVERRIDE_VALf, field_val);
                SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  jer_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  )
{
    SHR_BITDCLNAME (fdt_data, 137);
    SHR_BITDCLNAME (iqm_data, 39);
    uint32 iqm_modid = 0, device_inx = modid, offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    SHR_BITCOPY_RANGE(&iqm_modid, 0, &device_inx, 4, 8); 
    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, modid, fdt_data));
    SOCDNX_IF_ERR_EXIT(READ_IQM_PACK_MODEm(unit, MEM_BLOCK_ANY, iqm_modid, iqm_data));
    SHR_BITCOPY_RANGE(fdt_data, 6, &pcp_mode, 0, 2);

     
    offset = (modid % 16) * 2;
    SHR_BITCOPY_RANGE(iqm_data, offset, &pcp_mode, 0, 2);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ALL, modid, fdt_data)); 
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_PACK_MODEm(unit, MEM_BLOCK_ALL, iqm_modid, iqm_data)); 

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_fabric_pcp_dest_mode_config_get(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_OUT uint32          *pcp_mode
  )
{
    SHR_BITDCLNAME (data, 137);
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, modid, data)); 
    
    SHR_BITCOPY_RANGE(pcp_mode, 0, data, 6, 2);

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_fabric_repeater_config_init(
    SOC_SAND_IN  int  unit
  )
{
    soc_port_t port;
    int repeater_link_enable;
    SOCDNX_INIT_FUNC_DEFS;

    
    PBMP_SFI_ITER(unit, port) {
        repeater_link_enable = soc_property_port_get(unit, port, spn_REPEATER_LINK_ENABLE, 0);
        if (repeater_link_enable) {
            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_link_repeater_enable_set(unit, port, 1, SOC_DPP_DEFS_GET(unit, repeater_default_empty_cell_size)));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_fabric_pcp_init(
    SOC_SAND_IN  int          unit
  )
{
    uint32 regval, nof_instances_fmac, nof_links_fmac;
    uint64 reg64_val;
    soc_reg_above_64_val_t big_regval;
    ARAD_INIT_FABRIC *fabric = NULL;
    int i, j, core;
    SOCDNX_INIT_FUNC_DEFS;

    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric);
    nof_instances_fmac = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    nof_links_fmac = SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    if (fabric->fabric_pcp_enable) {
   
        
        SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_ENABLER_REGISTERr(unit, &regval));
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &regval, PCP_ENABLEDf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, regval));

        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_2r(unit, &reg64_val));
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, PCP_ENABLEDf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_2r(unit, reg64_val));

        
        for (i = 0; i < nof_instances_fmac; ++i) {
            for (j = 0; j < nof_links_fmac; ++j) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, i, j, &regval)); 
                soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &regval, FMAL_N_PCP_ENABLEDf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, i, j, regval));
            }
        }


        
        SOCDNX_IF_ERR_EXIT(READ_FDT_SIMPLE_PACKING_CONFIGURATIONr_REG32(unit, &regval));
        soc_reg_field_set(unit, FDT_SIMPLE_PACKING_CONFIGURATIONr, &regval, SIMPLE_PACKING_SOURCE_ID_0f, SOC_JER_FABRIC_SIMPLE_PACKING_MODID);
        soc_reg_field_set(unit, FDT_SIMPLE_PACKING_CONFIGURATIONr, &regval, SIMPLE_PACKING_SOURCE_ID_1f, SOC_JER_FABRIC_SIMPLE_PACKING_MODID);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_SIMPLE_PACKING_CONFIGURATIONr_REG32(unit, regval));

        
        
        if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit)){
            SOCDNX_IF_ERR_EXIT(READ_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, big_regval));
            soc_reg_field_set(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, big_regval, EN_LATENCY_DROPf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, big_regval));
        }
    } else {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
        {
            if (SOC_IS_QUX(unit)) {
                SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_CONFIGr(unit, core, &regval));
                soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &regval, PCP_DISf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_CONFIGr(unit, core, regval));
            } else if (!SOC_IS_FLAIR(unit)){
                SOCDNX_IF_ERR_EXIT(READ_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core, &regval));
                soc_reg_field_set(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, &regval, PCP_DISf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_DEBUG_CONFIGr(unit, core, regval));
            }
        }

    }


exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_fabric_pipes_init(
    SOC_SAND_IN  int  unit
  )
{
    uint32 reg_val, pipe_num;
    uint32 nof_pipes, nof_instances_fmac, nof_links_fmac;
    uint32 fabric_priority = 0;
    uint32 prio_table_entry;
    uint32 strict_prio_enable, strict_priority_pipe_num;
    uint64 reg64_val;
    soc_dcmn_fabric_pipe_map_t fabric_pipe_map;
    soc_dcmn_fabric_pipe_map_type_t map_type;
    soc_fabric_pipe_strict_priority_2_type_t strict_priority_2_val;

    int i, j;
    uint32 is_mc;
    SOCDNX_INIT_FUNC_DEFS;

    fabric_pipe_map = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config;
    nof_pipes = fabric_pipe_map.nof_pipes;
    map_type = fabric_pipe_map.mapping_type;

    
    nof_instances_fmac = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    nof_links_fmac = SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    SOCDNX_IF_ERR_EXIT(READ_FDT_FDT_ENABLER_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, PARALLEL_DATA_PATHf, nof_pipes - 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_2r(unit, &reg64_val));
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, PARALLEL_DATA_PATHf, nof_pipes - 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_2r(unit, reg64_val));

    if (!SOC_IS_JERICHO_PLUS(unit)){
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_ENABLERSr(unit, &reg_val));
        soc_reg_field_set(unit, FDA_FDA_ENABLERSr, &reg_val, PARALLEL_DATA_PATHf, nof_pipes - 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_ENABLERSr(unit, reg_val));
    }
    for ( i = 0; i < nof_instances_fmac; ++i) {
        for ( j = 0; j < nof_links_fmac; ++j) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, i, j, &reg_val)); 
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_PARALLEL_DATA_PATHf, nof_pipes - 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, i, j, reg_val));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_2r(unit, &reg64_val));
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, UC_PRI_0_PIPEf, fabric_pipe_map.config_uc[0]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, UC_PRI_1_PIPEf, fabric_pipe_map.config_uc[1]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, UC_PRI_2_PIPEf, fabric_pipe_map.config_uc[2]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, UC_PRI_3_PIPEf, fabric_pipe_map.config_uc[3]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, MC_PRI_0_PIPEf, fabric_pipe_map.config_mc[0]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, MC_PRI_1_PIPEf, fabric_pipe_map.config_mc[1]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, MC_PRI_2_PIPEf, fabric_pipe_map.config_mc[2]);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_2r, &reg64_val, MC_PRI_3_PIPEf, fabric_pipe_map.config_mc[3]);
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_2r(unit, reg64_val));


    if (SOC_DPP_IS_MESH(unit)){
        
        strict_priority_2_val = SOC_FABRIC_PIPE_STRICT_PRIORITY_2_NONE; 
    }else{
         switch (map_type) {
         case soc_dcmn_fabric_pipe_map_triple_uc_mc_tdm:
             strict_priority_2_val = SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_3; 
             break;

         case soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm:
             strict_priority_2_val = SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_2; 
             break;

         case soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
         case soc_dcmn_fabric_pipe_map_dual_uc_mc:
         case soc_dcmn_fabric_pipe_map_single:
             strict_priority_2_val = SOC_FABRIC_PIPE_STRICT_PRIORITY_2_NONE; 
             break;

         case soc_dcmn_fabric_pipe_map_triple_custom:
         case soc_dcmn_fabric_pipe_map_dual_custom:
         default:
             SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("custom mode is not supported")));
             break;
         }
    }

    if (SOC_IS_QAX(unit)){

        
        SOCDNX_IF_ERR_EXIT(READ_FDA_FAB_PIPE_STRICT_PRIOr(unit, &reg_val));
        soc_reg_field_set(unit, FDA_FAB_PIPE_STRICT_PRIOr, &reg_val, FAB_PIPE_STRICT_PRIOf, strict_priority_2_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FAB_PIPE_STRICT_PRIOr(unit, reg_val));

    }else if (!SOC_IS_FLAIR(unit)){ 

        for (i = 0; i < SOC_JER_FABRIC_PRIORITY_NDX_NOF; ++i) {
            SOCDNX_IF_ERR_EXIT(READ_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ANY, i, &prio_table_entry));
            fabric_priority = soc_mem_field32_get(unit, IPT_PRIORITY_BITS_MAPPING_2_FDTm, &prio_table_entry, PRIORITY_BITS_MAPPING_2_FDTf);
            is_mc = (i & SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_MASK) >> SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_OFFSET;
            pipe_num = is_mc ? fabric_pipe_map.config_mc[fabric_priority] : fabric_pipe_map.config_uc[fabric_priority];
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_IPT_0_DEST_PIPE_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &pipe_num));
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_IPT_1_DEST_PIPE_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &pipe_num));
        }

        if (SOC_IS_JERICHO_PLUS_ONLY(unit))
        {
            
            strict_prio_enable = (strict_priority_2_val == SOC_FABRIC_PIPE_STRICT_PRIORITY_2_NONE) ? 0 : 1;

            switch (strict_priority_2_val)
            {

            case SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_1:
                strict_priority_pipe_num = 0;
                break;

            case SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_2:
                strict_priority_pipe_num = 1;
                break;

            case SOC_FABRIC_PIPE_STRICT_PRIORITY_2_PIPE_3:
                strict_priority_pipe_num = 2;
                break;

            case SOC_FABRIC_PIPE_STRICT_PRIORITY_2_NONE:
            default:
                strict_priority_pipe_num = 0;
            }

            SOCDNX_IF_ERR_EXIT(READ_FDA_FAB_24_SCH_PIPE_STRICT_PRIOr(unit, &reg_val));
            soc_reg_field_set(unit, FDA_FAB_24_SCH_PIPE_STRICT_PRIOr, &reg_val, FAB_24_SCH_PIPE_STRICT_PRIO_ENf, strict_prio_enable);
            soc_reg_field_set(unit, FDA_FAB_24_SCH_PIPE_STRICT_PRIOr, &reg_val, FAB_24_SCH_PIPE_STRICT_PRIOf, strict_priority_pipe_num);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FAB_24_SCH_PIPE_STRICT_PRIOr(unit, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_FDA_FAB_48_SCH_PIPE_STRICT_PRIOr(unit, &reg_val));
            soc_reg_field_set(unit, FDA_FAB_48_SCH_PIPE_STRICT_PRIOr, &reg_val, FAB_48_SCH_PIPE_STRICT_PRIO_ENf, strict_prio_enable);
            soc_reg_field_set(unit, FDA_FAB_48_SCH_PIPE_STRICT_PRIOr, &reg_val, FAB_48_SCH_PIPE_STRICT_PRIOf, strict_priority_pipe_num);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FAB_48_SCH_PIPE_STRICT_PRIOr(unit, reg_val));

        }
    }

    
    if (!SOC_IS_QAX(unit)){
        for (i = 0; i < 2; ++i ) {
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, fda_egq_pipe_2_fifo_mapping[fabric_pipe_map.mapping_type]));
        }
    }
    


exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_load_balance_init(
    SOC_SAND_IN  int            unit
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_IS_MESH(unit)) {
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, ENABLE_SWITCHING_NETWORKf,  0x1));
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, REG_PORT_ANY, 0, SWITCHING_NETWORK_SEEDf, sal_time_usecs()));
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, REG_PORT_ANY, 0, SWITCHING_NETWORK_LFSR_CNTf, 1000));
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, REG_PORT_ANY, 0, SWITCHING_NETWORK_LFSR_CNT_MODEf, 1));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_sync_e_init(
    SOC_SAND_IN int           unit
  )
{
    int master_port, slave_port, divider;
    SOCDNX_INIT_FUNC_DEFS;

    master_port = soc_property_suffix_num_get(unit, 0, spn_SYNC_ETH_CLK_TO_PORT_ID_CLK, "", 0xffffffff);

    if (master_port != 0xffffffff && IS_SFI_PORT(unit, master_port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_sync_e_link_set(unit, 1, master_port));

        divider = soc_property_suffix_num_get(unit, 0, spn_SYNC_ETH_CLK_DIVIDER, "clk", 0x0);
        if (divider && (divider >= SOC_JER_FABRIC_SYNC_E_MIN_DIVIDER) && (divider <= SOC_JER_FABRIC_SYNC_E_MAX_DIVIDER)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_sync_e_divider_set(unit, 1, divider));
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, 1, 0));
    }

    slave_port  = soc_property_suffix_num_get(unit, 1, spn_SYNC_ETH_CLK_TO_PORT_ID_CLK, "", 0xffffffff);

    if (slave_port != 0xffffffff && IS_SFI_PORT(unit, slave_port)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_sync_e_link_set(unit, 0, slave_port));

        
        divider = soc_property_suffix_num_get(unit, 1, spn_SYNC_ETH_CLK_DIVIDER, "clk", 0x0);
        if (divider && (divider >= SOC_JER_FABRIC_SYNC_E_MIN_DIVIDER) && (divider <= SOC_JER_FABRIC_SYNC_E_MAX_DIVIDER)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_sync_e_divider_set(unit, 0, divider));
        }
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, 0, 0));
    }

exit:
    SOCDNX_FUNC_RETURN;

}


uint32
  soc_jer_fabric_low_power_init(
    SOC_SAND_IN int           unit
  )

{
    int rc, is_first_link, i, fsrd_block_, fmac_block_, fmac_inner_link;
    int blk;
    soc_port_t port;
    soc_info_t          *si;

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    SOC_PBMP_ITER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port) {

        rc = soc_jer_port_first_link_in_fsrd_get(unit, port , &is_first_link, 0);
        SOCDNX_IF_ERR_EXIT(rc);

        if (is_first_link == 1) 
        {
            fsrd_block_ = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port)/SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_block_, &fmac_inner_link));

            for (i = fmac_block_; i < fmac_block_ +  SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd) ; i++)
            {
                blk = si->fmac_block[fmac_block_];
                si->block_valid[blk] = 0;
            }

            if (!SOC_IS_QAX(unit) && !SOC_IS_FLAIR(unit)) {

                blk = si->fsrd_block[fsrd_block_];
                si->block_valid[blk] = 0;

                rc = soc_jer_port_update_fsrd_block(unit, port, 0);
                SOCDNX_IF_ERR_EXIT(rc);

            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC int
soc_jer_fabric_sw_state_allowed_links_init(int unit)
{
    soc_reg_above_64_val_t soc_reg_above_64_val;
    int nof_bits = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field_read(unit, RTP_ALLOWED_LINKSr, REG_PORT_ANY, 0, ALLOWED_LINKSf, soc_reg_above_64_val));

    nof_bits = soc_reg_field_length(unit, RTP_ALLOWED_LINKSr, ALLOWED_LINKSf);

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.alloc_bitmap(unit, nof_bits));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.bit_range_clear(unit, 0, nof_bits));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.bit_range_or(unit, 0, nof_bits, soc_reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_fabric_init(
    SOC_SAND_IN  int unit  
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_regs_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_pcp_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_pipes_init(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_interleaving_init(unit));

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_load_balance_init(unit));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_flow_control_init(unit));

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) { 
        
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_scheduler_adaptation_init(unit));
    }

     
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_repeater_config_init(unit));

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_init(unit));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_low_power_init(unit));

    
    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sw_state_allowed_links_init(unit));
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type  
  )
{
    soc_field_t field;
    uint32 reg32;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_ingress) {
        
        if ((1)) {
            switch (pipe) {
            case 0:
                field = FAB_18_SCH_WFQ_WEIGHT_PIPE_1f;
                break;
            case 1:
                field = FAB_18_SCH_WFQ_WEIGHT_PIPE_2f;
                break;
            case 2:
                field = FAB_18_SCH_WFQ_WEIGHT_PIPE_3f;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit ,pipe));
                break;
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDA_FAB_18_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
            soc_reg_above_64_field32_set(unit, FDA_FAB_18_SCH_WFQ_CFGr, reg_above_64_val, field, weight);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDA_FAB_18_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
        }
        switch (pipe) {
        case 0:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_1f;
            break;
        case 1:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_2f;
            break;
        case 2:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_3f;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDA_FAB_36_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FDA_FAB_36_SCH_WFQ_CFGr, reg_above_64_val, field, weight);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDA_FAB_36_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
    }

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_egress) {
        
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FDT_IPT_0_WFQ_CONFIGr, reg_above_64_val, field, weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_0_WFQ_CONFIGr(unit, reg_above_64_val));

        switch (pipe) {
        case 0:
            field = IPT_1_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_1_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_1_WFQ_CTX_2_WEIGHTf;
            break;
        
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDT_IPT_1_WFQ_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FDT_IPT_1_WFQ_CONFIGr, reg_above_64_val, field, weight);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, FDT_IPT_1_WFQ_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, &reg32));
        soc_reg_field_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, &reg32, IPT_WFQ_ENf, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY,  0, reg32));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_jer_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type  
  )
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    if (weight == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid address %#x"), unit ,weight));
    }
    if (fabric_pipe_type == soc_dpp_cosq_gport_fabric_pipe_egress) {       
        switch (pipe) {
        case 0:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_1f;
            break;
        case 1:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_2f;
            break;
        case 2:
            field = FAB_36_SCH_WFQ_WEIGHT_PIPE_3f;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit, pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, FDA_FAB_36_SCH_WFQ_CFGr, REG_PORT_ANY, 0, reg_above_64_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FAB_36_SCH_WFQ_CFGr, reg_above_64_val, field);
    } else {    
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, reg_above_64_val));
        *weight = soc_reg_above_64_field32_get(unit, FDT_IPT_0_WFQ_CONFIGr, reg_above_64_val, field);
    }

exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_fabric_link_thresholds_pipe_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_IN int                                  value
  )
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    if (value > SOC_JER_FABRIC_RX_FC_TH_MAX || value < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("threshold value outside allowed range"))); 
    }

    switch (type) {
    case socDppFabricLinkRxFifoLLFC:
        if (pipe == socDppFabricPipeAll) {
            SOCDNX_IF_ERR_EXIT(READ_FDR_LINK_LEVEL_FLOW_CONTROLr(unit, &reg_val));
            soc_reg_field_set(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, &reg_val, LNK_LVL_FC_THf, value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDR_LINK_LEVEL_FLOW_CONTROLr(unit, reg_val));
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("LLFC is only supported in All-Pipes mode")));
        }
        break;
    case socDppFabricLinkRxRciLvl1FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) { 
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_1f, value);
        }
        if (pipe == socDppFabricPipe1 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_2f, value);
        }
        if (pipe == socDppFabricPipe2 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_3f, value);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        break;
    case socDppFabricLinkRxRciLvl2FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_1f, value);
        }
        if (pipe == socDppFabricPipe1 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_2f, value);
        }
        if (pipe == socDppFabricPipe2 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_3f, value);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        break;
    case socDppFabricLinkRxRciLvl3FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_1f, value);
        }
        if (pipe == socDppFabricPipe1 || pipe == socDppFabricPipeAll) { 
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_2f, value);
        }
        if (pipe == socDppFabricPipe2 || pipe == socDppFabricPipeAll) {
            soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_3f, value);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("threshold type is not supported")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_link_thresholds_pipe_get(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_OUT int                                 *value
  )
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    switch (type) {
    case socDppFabricLinkRxFifoLLFC:
        if (pipe == socDppFabricPipeAll) {
            SOCDNX_IF_ERR_EXIT(READ_FDR_LINK_LEVEL_FLOW_CONTROLr(unit, &reg_val));
            *value = soc_reg_field_get(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, reg_val, LNK_LVL_FC_THf);
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("LLFC is only supported in All-Pipes mode")));
        }
        break; 
    case socDppFabricLinkRxRciLvl1FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64)); 
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_1f);
        }
        else if (pipe == socDppFabricPipe1) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_2f);
        }
        else if (pipe == socDppFabricPipe2) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_1_P_3f);
        }
        break;
    case socDppFabricLinkRxRciLvl2FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64)); 
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_1f);
        }
        else if (pipe == socDppFabricPipe1) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_2f);
        }
        else if (pipe == socDppFabricPipe2) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_2_P_3f);
        }
        break;
    case socDppFabricLinkRxRciLvl3FC:
        SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val_above_64)); 
        if (pipe == socDppFabricPipe0 || pipe == socDppFabricPipeAll) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_1f);
        }
        else if (pipe == socDppFabricPipe1) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_2f);
        }
        else if (pipe == socDppFabricPipe2) {
            *value = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, 
                                       reg_val_above_64, LOCAL_FIFO_RCI_LEVEL_TH_3_P_3f);
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("threshold type is not supported")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    int i;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val)); 
        soc_reg_field_set(unit, FDA_FDA_MESHMC_FC_ENr, &reg_val, MESHMC_FC_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_MESHMC_FC_ENr(unit, reg_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, &reg_val, EGQ_N_FABFIF_0_PIPE_FCf, enable);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, &reg_val, EGQ_N_FABFIF_1_PIPE_FCf, enable);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, &reg_val, EGQ_N_FABFIF_2_PIPE_FCf, enable);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, i, reg_val));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val)); 
        *enable = soc_reg_field_get(unit, FDA_FDA_MESHMC_FC_ENr, reg_val, MESHMC_FC_ENf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, 0, &reg_val));
        *enable = soc_reg_field_get(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, reg_val, EGQ_N_FABFIF_0_PIPE_FCf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, 0, &reg_val));
        *enable = soc_reg_field_get(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, reg_val, EGQ_N_FABFIF_1_PIPE_FCf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr(unit, 0, &reg_val));
        *enable = soc_reg_field_get(unit, FDA_FDA_EGQ_PIPE_TO_FIFO_MAPPINGr, reg_val, EGQ_N_FABFIF_2_PIPE_FCf);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type)
{
    int i;
    soc_reg_above_64_val_t above_64_reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
            soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_FABFIF_0f, weight);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
            soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_FABFIF_1f, weight);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
            soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_MESHMCf, weight);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, i, above_64_reg_val));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_jer_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type 
  )
{
    soc_reg_above_64_val_t above_64_reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(weight);
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_FABFIF_0f);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_FABFIF_1f);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_MESHMCf);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_jer_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    int i;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABRIC_RCIr(unit, i, &reg64_val));

            if (SOC_IS_JERICHO_PLUS_ONLY(unit))
            {
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_THR_LEVEL_1f, threshold_val);
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_THR_LEVEL_2f, threshold_val);
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_THR_LEVEL_3f, threshold_val);
            }
            else
            {
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_THRf, threshold_val);
            }

            if (threshold_val == 0) {
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_SELf, 0);
            } else {
                soc_reg64_field32_set(unit, FDA_FDA_EGQ_FABRIC_RCIr, &reg64_val, EGQ_N_FABRIC_RCI_SELf, 1);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABRIC_RCIr(unit, i, reg64_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_IPT_DATA_QUEUE_EGQ_THRESHOLDr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, IPT_DATA_QUEUE_EGQ_THRESHOLDr, reg_above_64_val, DTQ_EGQ_0_GEN_RCI_THf, threshold_val);
        soc_reg_above_64_field32_set(unit, IPT_DATA_QUEUE_EGQ_THRESHOLDr, reg_above_64_val, DTQ_EGQ_1_GEN_RCI_THf, threshold_val);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_DATA_QUEUE_EGQ_THRESHOLDr(unit, reg_above_64_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("access to registers should be fixed for Jericho plus at places we used _REG(32|64) access routines")));
    }


    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABRIC_RCIr(unit, 0, &reg64_val));
        *threshold_val = soc_reg64_field32_get(unit, FDA_FDA_EGQ_FABRIC_RCIr, reg64_val, EGQ_N_FABRIC_RCI_THRf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_IPT_DATA_QUEUE_EGQ_THRESHOLDr(unit, reg_above_64_val));
        *threshold_val = soc_reg_above_64_field32_get(unit, IPT_DATA_QUEUE_EGQ_THRESHOLDr, reg_above_64_val, DTQ_EGQ_0_GEN_RCI_THf);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    int i;
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        if (threshold->dp == 0) {
            field = EGQ_N_FABFIF_0_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_FABFIF_0_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_FABFIF_0_PRIO_2_DROP_THRf;
        }
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, &reg_val, field, threshold->value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        if (threshold->dp == 0) {
            field = EGQ_N_FABFIF_1_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_FABFIF_1_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_FABFIF_1_PRIO_2_DROP_THRf;
        }
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, &reg_val, field, threshold->value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_N_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, &reg_val, field, threshold->value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, reg_val));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        if (threshold->dp == 0) {
            field = EGQ_N_FABFIF_0_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_FABFIF_0_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_FABFIF_0_PRIO_2_DROP_THRf;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr(unit, 0, &reg_val));
        threshold->value = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_0_PRIO_DROP_THRr, reg_val, field);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
        if (threshold->dp == 0) {
            field = EGQ_N_FABFIF_1_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_FABFIF_1_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_FABFIF_1_PRIO_2_DROP_THRf;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr(unit, 0, &reg_val));
        threshold->value = soc_reg_field_get(unit, FDA_FDA_EGQ_FABFIF_1_PRIO_DROP_THRr, reg_val, field);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_N_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_N_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_N_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, 0, &reg_val));
        threshold->value = soc_reg_field_get(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, reg_val, field);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_priority_bits_mapping_to_fdt_index_get(
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  uint32                     tc, 
    SOC_SAND_IN  uint32                     dp,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_OUT uint32                     *index
  )
{
    uint32 is_mc  = 0, is_hp = 0, is_tdm = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *index = 0;

    is_hp  = (flags & SOC_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0 ;
    is_mc  = (flags & SOC_FABRIC_PRIORITY_MULTICAST)       ? 1 : 0 ;
    is_tdm = (flags & SOC_FABRIC_PRIORITY_TDM)             ? 1 : 0 ;
    
    *index  |=  ((is_hp     << SOC_JER_FABRIC_PRIORITY_NDX_IS_HP_OFFSET)  & SOC_JER_FABRIC_PRIORITY_NDX_IS_HP_MASK  )|
                ((tc        << SOC_JER_FABRIC_PRIORITY_NDX_TC_OFFSET)     & SOC_JER_FABRIC_PRIORITY_NDX_TC_MASK     )|
                ((dp        << SOC_JER_FABRIC_PRIORITY_NDX_DP_OFFSET)     & SOC_JER_FABRIC_PRIORITY_NDX_DP_MASK     )|
                ((is_mc     << SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_OFFSET)  & SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_MASK  )|
                ((is_tdm    << SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_OFFSET) & SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_MASK );


    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_fabric_priority_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32             tc, 
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  )
{
    uint32 index, is_tdm = 0;
    uint32 tdm_min_prio;
    SOCDNX_INIT_FUNC_DEFS;

    tdm_min_prio = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;
    is_tdm = (flags & SOC_FABRIC_PRIORITY_TDM);
    
    if (fabric_priority >= SOC_JER_FABRIC_PRIORITY_NOF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric priority out of range")));
    }
    if (tdm_min_prio == SOC_DPP_FABRIC_TDM_PRIORITY_NONE) {
        if (is_tdm) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("TDM unsupported in system")));
        }
    } else {
        if (((is_tdm) && (fabric_priority < tdm_min_prio)) || 
            ((!is_tdm) && (fabric_priority >= tdm_min_prio))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric priority configuration not supported")));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IPT_PRIORITY_BITS_MAPPING_2_FDTm, 
                                                          0, 0, MEM_BLOCK_ALL, index, index, &fabric_priority));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_fabric_priority_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32             tc, 
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  )
{
    uint32 index, val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    
    SOCDNX_IF_ERR_EXIT(READ_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ANY, index, &val));
    *fabric_priority = val;

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_minimal_links_to_dest_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_IN int             num_of_links
  )
{
    int i, modid_max;
    uint32 min_links = num_of_links;
    SHR_BITDCLNAME (fdt_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    modid_max = SOC_DPP_DEFS_GET(unit, modid_max_valid);
    if (module_id == SOC_MODID_ALL) {
        for (i = 0; i < modid_max; ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, i, fdt_data));
            SHR_BITCOPY_RANGE(fdt_data, 0, &min_links, 0, 6);
            SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ALL, i, fdt_data));
        }
    } else {
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, module_id, fdt_data));
        SHR_BITCOPY_RANGE(fdt_data, 0, &min_links, 0, 6);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ALL, module_id, fdt_data));
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_minimal_links_to_dest_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_OUT int            *num_of_links
  )
{
    int modid;
    uint32 min_links = 0;
    SHR_BITDCLNAME (fdt_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    modid = (module_id == SOC_MODID_ALL) ? 0 : module_id;
    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, modid, fdt_data));
    SHR_BITCOPY_RANGE(&min_links, 0, fdt_data, 0, 6);

    *num_of_links = min_links;

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_minimal_links_all_reachable_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN int             num_of_links
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, &reg_val, ALRC_MIN_NUM_OF_LINKSf, num_of_links);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_minimal_links_all_reachable_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_OUT int            *num_of_links
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    *num_of_links = soc_reg_field_get(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, reg_val, ALRC_MIN_NUM_OF_LINKSf);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_fabric_stack_module_all_reachable_ignore_id_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_IN  int                enable
  )
{
    int i, first_empty = -1;
    uint32 reg_val;
    soc_module_t id_field;
    int valid_field = 0;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < SOC_JER_FABRIC_STK_MAX_IGNORED_FAP_IDS ; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], REG_PORT_ANY, 0, &reg_val)); 
        valid_field = soc_reg_field_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], 
                                     reg_val, soc_jer_fabric_all_reachable_vector_exclude_fields_valid[i]);
        id_field = soc_reg_field_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], 
                                  reg_val, soc_jer_fabric_all_reachable_vector_exclude_fields[i]);

        if (valid_field == 1) { 
            if (id_field == module){ 
                if (enable == 0) {  
                    soc_reg_field_set(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], &reg_val, 
                                      soc_jer_fabric_all_reachable_vector_exclude_fields_valid[i], 0);
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], REG_PORT_ANY, 0, reg_val));
                    SOC_EXIT;
                }
                else { 
                    SOC_EXIT; 
                }
            }
        }
        else if (first_empty == -1 && enable == 1) { 
            first_empty = i;
        }
    }
    if (enable == 0){ 
        SOC_EXIT;
    }
    if (first_empty == -1){ 
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't ignore this FAP ID - 16 FAP IDs are already ignored \n")));
    }
    else {
        soc_reg_field_set(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[first_empty], &reg_val, 
                          soc_jer_fabric_all_reachable_vector_exclude_fields[first_empty], module);
        soc_reg_field_set(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[first_empty], &reg_val, 
                          soc_jer_fabric_all_reachable_vector_exclude_fields_valid[first_empty], 1);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[first_empty], REG_PORT_ANY, 0, reg_val));
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_all_reachable_ignore_id_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_OUT int                *enable
  )
{
    int i;
    uint32 reg_val;
    soc_module_t modid;
    int is_valid = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *enable = 0;

    for (i = 0; i < SOC_JER_FABRIC_STK_MAX_IGNORED_FAP_IDS ; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], REG_PORT_ANY, 0, &reg_val));
        is_valid = soc_reg_field_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], 
                                     reg_val, soc_jer_fabric_all_reachable_vector_exclude_fields_valid[i]);
        modid = soc_reg_field_get(unit, soc_jer_fabric_all_reachable_vector_exclude_regs[i], 
                                  reg_val, soc_jer_fabric_all_reachable_vector_exclude_fields[i]);
        if (module == modid) {
            *enable = is_valid;
            break;
        }
    }
    
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_max_all_reachable_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  )
{
    int field_val;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    field_val = max_module / SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, &reg_val, MAX_ALRC_BASE_INDEXf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_max_all_reachable_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT soc_module_t       *max_module
  )
{
    int field_val;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    field_val = soc_reg_field_get(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, reg_val, MAX_ALRC_BASE_INDEXf);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

    *max_module = field_val * SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_max_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  )
{
    int field_val;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    field_val = max_module / SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MAXIMUM_BASE_INDEXr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MAXIMUM_BASE_INDEXr, &reg_val, MAX_BIf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MAXIMUM_BASE_INDEXr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_max_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT soc_module_t       *max_module
  )
{
    int field_val;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_RTP_MAXIMUM_BASE_INDEXr(unit, &reg_val));
    field_val = soc_reg_field_get(unit, RTP_MAXIMUM_BASE_INDEXr, reg_val, MAX_BIf);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MAXIMUM_BASE_INDEXr(unit, reg_val));

    *max_module = field_val * SOC_JER_FABRIC_STK_FAP_GROUP_SIZE;

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_stack_module_devide_by_32_verify(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_module_t        max_module
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (max_module % SOC_JER_FABRIC_STK_FAP_GROUP_SIZE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("max_module should devide by 32")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


int
  soc_jer_fabric_mode_validate(
    SOC_SAND_IN int                           unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_CONNECT_MODE   *fabric_connect_mode
  )
{
    int is_mesh_hybrid = 0, nof_hybrid_mesh_fabric_pms = 0;
    SOCDNX_INIT_FUNC_DEFS;

    is_mesh_hybrid = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_enable", 0);
    if (is_mesh_hybrid &&
        (!SOC_IS_QAX(unit) || SOC_IS_QUX(unit) || (*fabric_connect_mode != SOC_TMC_FABRIC_CONNECT_MODE_MESH)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mesh hybrid mode is supported only for QAX in fabric connect mode MESH")));
    }

    nof_hybrid_mesh_fabric_pms = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_nof_fabric_quads", 0);
    if (!is_mesh_hybrid && nof_hybrid_mesh_fabric_pms != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mesh_hybrid_nof_fabric_quads is supported only mesh hybrid mode. First set mesh_hybrid_enable soc_property to 1.")));
    }
    nof_hybrid_mesh_fabric_pms = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mesh_hybrid_nof_fabric_quads", SOC_QAX_PM_FABRIC);
    if (is_mesh_hybrid && (nof_hybrid_mesh_fabric_pms > SOC_QAX_PM_FABRIC || nof_hybrid_mesh_fabric_pms == 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid value for mesh_hybrid_nof_fabric_quads. Provided %d and valid values are between 1 and %d."), nof_hybrid_mesh_fabric_pms, SOC_QAX_PM_FABRIC));
    }

    switch (*fabric_connect_mode) {
    case SOC_TMC_FABRIC_CONNECT_MODE_FE:
        if(SOC_IS_QAX(unit) && !SOC_IS_KALIA(unit)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric connect mode FE is not supported for QAX")));
        }
        break;
    case SOC_TMC_FABRIC_CONNECT_MODE_MESH:
        if (soc_feature(unit, soc_feature_packet_tdm_marking)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric connect mode MESH is not supported for TDM MARKING")));
        }
        break;
    case SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fabric connect mode")));
    }
exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_port_to_fmac(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_port_t                     port,
    SOC_SAND_OUT  int                            *fmac_index,
    SOC_SAND_OUT  int                            *fmac_inner_link

  )
{

    int fabric_port_logical_base, rv = 0;

    fabric_port_logical_base = SOC_DPP_DEFS_GET(unit, first_fabric_link_id);

    *fmac_index = (SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) - fabric_port_logical_base) / SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac); 
    *fmac_inner_link = (SOC_DPP_FABRIC_PORT_TO_LINK(unit, port) - fabric_port_logical_base) % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    return rv;
}

uint32
soc_jer_fabric_link_topology_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id, 
    SOC_SAND_IN  int                            links_count,
    SOC_SAND_IN  soc_port_t                     *links_array
  )
{
    int i;
    uint64 reg64_val, link_arr_bmp, link_delay_bmp, link_arr_clr;
    uint32 local_core0_id, group_ctx;
    soc_module_t dest_fap_id_arr[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    int fap_id_count, offset;
    uint32 reg32_val, field[1];
    uint32 mesh_field;
    int fmac_index, fmac_inner_link;
    SHR_BITDCLNAME (dqcq_map, 32);
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(link_arr_bmp);
    for (i = 0; i < links_count; ++i) {
        COMPILER_64_BITSET(link_arr_bmp, SOC_DPP_FABRIC_PORT_TO_LINK(unit, links_array[i])); 
    }

    
    SOCDNX_IF_ERR_EXIT(READ_FDT_MESH_MC_RELICATIONr_REG64(unit, local_dest_id, &reg64_val));
    if (SOC_IS_QAX(unit)) {
        soc_reg64_field_set(unit, FDT_MESH_MC_RELICATIONr, &reg64_val, MESH_MC_BITMAPf, link_arr_bmp);
    } else {
        soc_reg64_field_set(unit, FDT_MESH_MC_RELICATIONr, &reg64_val, MESH_MC_N_BITMAPf, link_arr_bmp);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_MESH_MC_RELICATIONr_REG64(unit, local_dest_id, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_modid_group_get(unit, local_dest_id, 2, dest_fap_id_arr, &fap_id_count));
    if (!SOC_IS_QAX(unit)) 
    {
        group_ctx = local_dest_id + 2;
        for (i = 0; i < fap_id_count; ++i) {
            offset = (dest_fap_id_arr[i] % 16) * SOC_JER_FABRIC_GROUP_CTX_LENGTH; 
            if (offset < 32) {
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, &reg64_val));
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_1r, reg64_val, DEV_ID_TO_DQCQ_MAP_LSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64_val, DEV_ID_TO_DQCQ_MAP_LSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64_val));
            } else {
                offset -= 32;
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, &reg64_val));
                *dqcq_map = soc_reg64_field32_get(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, reg64_val, DEV_ID_TO_DQCQ_MAP_MSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64_val, DEV_ID_TO_DQCQ_MAP_MSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64_val));
            }
        }
    }
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &local_core0_id)));
    if (dest_fap_id_arr[0] > local_core0_id) {
        
        mesh_field = SOC_IS_JERICHO_PLUS_ONLY(unit) ? FIELD_0_47f : FIELD_0_35f;
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_010Fr(unit, &reg64_val));
        link_delay_bmp = soc_reg64_field_get(unit, MESH_TOPOLOGY_REG_010Fr, reg64_val, mesh_field);
        COMPILER_64_NOT(link_arr_bmp);
        COMPILER_64_AND(link_delay_bmp, link_arr_bmp);
        soc_reg64_field_set(unit, MESH_TOPOLOGY_REG_010Fr, &reg64_val, mesh_field, link_delay_bmp);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_010Fr(unit, reg64_val));

        
        COMPILER_64_ZERO(link_arr_clr);
        if (COMPILER_64_EQ(link_delay_bmp, link_arr_clr)) {
            SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_INITr(unit,  0xd0d)); 
        }
    } else {
         for (i = 0; i < links_count; ++i) {

            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, links_array[i], &fmac_index, &fmac_inner_link));

            SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, &reg32_val));

            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_0f);
            SHR_BITSET(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_0f, *field);

            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_1f);
            SHR_BITCLR(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_1f, *field);

            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, reg32_val));
        }
    }
exit:
  SOCDNX_FUNC_RETURN;

}

uint32
soc_jer_fabric_link_topology_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id, 
    SOC_SAND_IN  int                            max_links_count,
    SOC_SAND_OUT int                            *links_count, 
    SOC_SAND_OUT soc_port_t                     *links_array
  )
{
    int i;
    uint64 reg64_val, bmp;
    int nof_fab_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id);
    soc_port_t link_arr[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    SOCDNX_INIT_FUNC_DEFS;

    *links_count = 0;

    SOCDNX_IF_ERR_EXIT(READ_FDT_MESH_MC_RELICATIONr_REG64(unit, local_dest_id, &reg64_val));
    if (SOC_IS_QAX(unit)) {
        bmp = soc_reg64_field_get(unit, FDT_MESH_MC_RELICATIONr, reg64_val, MESH_MC_BITMAPf); 
    } else {
        bmp = soc_reg64_field_get(unit, FDT_MESH_MC_RELICATIONr, reg64_val, MESH_MC_N_BITMAPf); 
    }

    
    for (i = 0; i < nof_fab_links; ++i) {
        if (COMPILER_64_BITTEST(bmp, i)){
            link_arr[*links_count] = SOC_DPP_FABRIC_LINK_TO_PORT(unit, i);
            ++(*links_count);
        }
    }

    
    if (max_links_count < *links_count) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("links_array is too short to contain all links")));
    } else { 
        for (i = 0; i < *links_count; ++i) {
            links_array[i] = link_arr[i];
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_link_topology_unset(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id
  )
{
    int i, links_count;
    uint64 reg64_val, bmp_val, mesh_mc_bmp_val = COMPILER_64_INIT(0xf,0xffffffff);
    uint32 local_core0_id, group_ctx;
    int nof_fab_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
    int fap_id_count, offset;
    uint32 reg32_val, field[1];
    uint32 mesh_field;
    int fmac_index, fmac_inner_link;
    SHR_BITDCLNAME (dqcq_map, 32);
    soc_module_t links_arr[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    soc_module_t dest_fap_id_arr[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_link_topology_get(unit, local_dest_id, nof_fab_links, &links_count, links_arr));

    
    SOCDNX_IF_ERR_EXIT(READ_FDT_MESH_MC_RELICATIONr_REG64(unit, local_dest_id, &reg64_val));
    if (SOC_IS_QAX(unit)) {
        soc_reg64_field_set(unit, FDT_MESH_MC_RELICATIONr, &reg64_val, MESH_MC_BITMAPf, mesh_mc_bmp_val);
    } else {
        soc_reg64_field_set(unit, FDT_MESH_MC_RELICATIONr, &reg64_val, MESH_MC_N_BITMAPf, mesh_mc_bmp_val);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_MESH_MC_RELICATIONr_REG64(unit, local_dest_id, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_modid_group_get(unit, local_dest_id, 2, dest_fap_id_arr, &fap_id_count));
    if (!SOC_IS_QAX(unit)) 
    {
        group_ctx = 0;
        for (i = 0; i < fap_id_count; ++i) {
            offset = (dest_fap_id_arr[i] % 16) * SOC_JER_FABRIC_GROUP_CTX_LENGTH; 
            if (offset < 32) {
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, &reg64_val));
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_1r, reg64_val, DEV_ID_TO_DQCQ_MAP_LSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64_val, DEV_ID_TO_DQCQ_MAP_LSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64_val));
            } else {
                offset -= 32;
                SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, &reg64_val));
                *dqcq_map = soc_reg64_field32_get(unit,ECI_GLOBAL_DQCQ_MAP_CFG_2r, reg64_val, DEV_ID_TO_DQCQ_MAP_MSBf);
                SHR_BITCOPY_RANGE(dqcq_map, offset, &group_ctx, 0, SOC_JER_FABRIC_GROUP_CTX_LENGTH);
                soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64_val, DEV_ID_TO_DQCQ_MAP_MSBf, *dqcq_map);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64_val));
            }
        }
    }

    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &local_core0_id)));
    if (dest_fap_id_arr[0] > local_core0_id) {

        mesh_field = SOC_IS_JERICHO_PLUS_ONLY(unit) ? FIELD_0_47f : FIELD_0_35f;
        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_010Fr(unit, &reg64_val));
        bmp_val = soc_reg64_field_get(unit, MESH_TOPOLOGY_REG_010Fr, reg64_val, mesh_field);
        for (i = 0; i < links_count; ++i) {
            COMPILER_64_BITCLR(bmp_val, SOC_DPP_FABRIC_PORT_TO_LINK(unit, links_arr[i]));
        }
        soc_reg64_field_set(unit, MESH_TOPOLOGY_REG_010Fr, &reg64_val, mesh_field, bmp_val);
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_010Fr(unit, reg64_val));
    } else {
         for (i = 0; i < links_count; ++i) {
            SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, links_arr[i], &fmac_index, &fmac_inner_link));

            SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, &reg32_val));

            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_0f);
            SHR_BITCLR(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_0f, *field);

            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_1f);
            SHR_BITSET(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_1f, *field);

            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, reg32_val));
        }
    }
exit:
  SOCDNX_FUNC_RETURN;

}

soc_error_t
soc_jer_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  )
{
    int index, offset, i;
    uint32 mc_rep = 0;
    soc_module_t local_dest;
    uint32 my_core0_fap_id, my_core1_fap_id;
    SHR_BITDCLNAME (table_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    index = mc_id / SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE;
    offset = (mc_id % SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE) * SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &my_core0_fap_id)));
    my_core1_fap_id = my_core0_fap_id + 1;

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));
    for (i = 0; i < destid_count; ++i) {
        if (destid_array[i] == my_core0_fap_id){
            mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT;
        } else if (destid_array[i] == my_core1_fap_id){
            mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT;
        } else { 
            local_dest = SOC_DPP_FABRIC_GROUP_MODID_UNSET(destid_array[i]);
            if (local_dest == 0 || local_dest == 1) {
                mc_rep |= (local_dest == 0) ? SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT :
                                             SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT ;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("dest %d is invalid"),destid_array[i]));
            }
                        
        }
    }
    SHR_BITCOPY_RANGE(table_data, offset, &mc_rep, 0, SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_multicast_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count_max,
    SOC_SAND_OUT int                            *destid_count,
    SOC_SAND_OUT soc_module_t                   *destid_array
  )
{
    int index, offset, i, zero = 0, one = 1;
    uint32 mc_rep = 0;
    uint32 my_core0_fap_id, my_core1_fap_id;
    soc_module_t *pdest = destid_array;
    SHR_BITDCLNAME (table_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    *destid_count = 0;
    index = mc_id / SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE;
    offset = (mc_id % SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE) * SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &my_core0_fap_id)));
    my_core1_fap_id = my_core0_fap_id + 1;

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));
    SHR_BITCOPY_RANGE(&mc_rep, 0, table_data, offset, SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH);

    for (i = 1; i < SOC_JER_FABRIC_MESH_MC_REPLICATION_MASK; i *= 2) {
        if (mc_rep & i) {
            *destid_count += 1;
        }
    }
    if (destid_count_max < *destid_count) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("destid_array is too short to contain all dest id's")));
    } else {
        if (mc_rep & SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT){
            *pdest = SOC_DPP_FABRIC_GROUP_MODID_SET(zero);
            ++pdest;
        }
        if (mc_rep & SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT){
            *pdest = SOC_DPP_FABRIC_GROUP_MODID_SET(one);
            ++pdest;
        }
        if (mc_rep & SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT){
            *pdest = my_core0_fap_id;
            ++pdest;
        }
        if (mc_rep & SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT){
            *pdest = my_core1_fap_id;
            ++pdest;
        }
    }
    
exit:
  SOCDNX_FUNC_RETURN;

}

uint32
soc_jer_fabric_modid_group_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest,
    SOC_SAND_IN  int                            modid_count,
    SOC_SAND_IN  soc_module_t                   *modid_array
  )
{
    int i;
    int (*set_count_func)(int unit, int count);
    int (*set_arr_entry_func)(int unit, int idx, int value);
    SOCDNX_INIT_FUNC_DEFS;

    switch (local_dest) {
    case 0:
        set_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_0_count.set;
        set_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_0.set;
        break;
    case 1:
        set_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_1_count.set;
        set_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_1.set;
        break;
    case 2:
        set_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_2_count.set;
        set_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_2.set;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("destination id is invalid")));
        break;
    }
    SOCDNX_IF_ERR_EXIT(set_count_func(unit, modid_count));

    for (i = 0; i < modid_count; ++i) {
        SOCDNX_IF_ERR_EXIT(set_arr_entry_func(unit, i, modid_array[i]));
    }

exit:
  SOCDNX_FUNC_RETURN;

}

uint32
soc_jer_fabric_modid_group_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest,
    SOC_SAND_IN  int                            modid_max_count,
    SOC_SAND_OUT soc_module_t                   *modid_array,
    SOC_SAND_OUT int                            *modid_count
  )
{
    int i, id_count;
    int (*get_count_func)(int unit, int *count);
    int (*get_arr_entry_func)(int unit, int idx, int *value);
    SOCDNX_INIT_FUNC_DEFS;

    switch (local_dest) {
    case 0:
        get_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_0_count.get;
        get_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_0.get;
        break;
    case 1:
        get_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_1_count.get;
        get_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_1.get;
        break;
    case 2:
        get_count_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_2_count.get;
        get_arr_entry_func = sw_state_access[unit].dpp.soc.arad.tm.jer_modid_group_map.dest_2.get;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("destination id is invalid")));
        break;
    }
    SOCDNX_IF_ERR_EXIT(get_count_func(unit, &id_count));

    if (modid_max_count < id_count) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("modid_array is too short to contain all FAP id's")));
    }
    *modid_count = id_count;
    for (i = 0; i < id_count; ++i) {
        SOCDNX_IF_ERR_EXIT(get_arr_entry_func(unit, i, &modid_array[i]));
    }
exit:
  SOCDNX_FUNC_RETURN;

}

uint32 
soc_jer_fabric_local_dest_id_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id
  )
{
    int is_mesh, is_mesh_mc;
    SOCDNX_INIT_FUNC_DEFS;

    if (local_dest_id < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid dest_id")));
    }
    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);

    if (is_mesh_mc && (local_dest_id >= SOC_DPP_DEFS_GET(unit, fabric_max_dest_mesh_mc_mode))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("in Mesh MC mode there can only be %d destinations"), SOC_DPP_DEFS_GET((unit), fabric_max_dest_mesh_mc_mode)));
    } else if (is_mesh && (local_dest_id >= SOC_DPP_DEFS_GET(unit, fabric_max_dest_mesh_mode))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("in Mesh mode there can only be %d destinations"), SOC_DPP_DEFS_GET((unit), fabric_max_dest_mesh_mode)));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_fabric_rci_thresholds_config_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_dpp_fabric_rci_config_t    rci_config)
{
    int i;
    soc_reg_above_64_val_t reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val));
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, GCI_RCI_CNT_MODE_ENf, 1);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_1_FACTORf, rci_config.soc_dpp_fabric_rci_severity_factors[0]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_2_FACTORf, rci_config.soc_dpp_fabric_rci_severity_factors[1]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_3_FACTORf, rci_config.soc_dpp_fabric_rci_severity_factors[2]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_HIGH_SEV_MIN_NOF_LINKSf, rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_HIGH_SEV_SCOREf, rci_config.soc_dpp_fabric_rci_high_score_congested_links);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_IPT_FIFO_SCOREf, rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue);

    if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_FDA_FIFO_SCORE_LEVEL_1f, rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_FDA_FIFO_SCORE_LEVEL_2f, rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_FDA_FIFO_SCORE_LEVEL_3f, rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue);
    }
    else
    {
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_FDA_FIFO_SCOREf, rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue); 
    }

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_1f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[0]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_2f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[1]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_3f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[2]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_4f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[3]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_5f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[4]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_6f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[5]);
        soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_7f, rci_config.soc_dpp_fabric_rci_core_level_thresholds[6]);
    }else{
        
        for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
            if (rci_config.soc_dpp_fabric_rci_core_level_thresholds[i] != 0x0){
                LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit," For single core device, rci_core_level_thresholds doesn't exist, therefore, the convention is to use value of 0\n")));
            }
        }
    }
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_1_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[0]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_2_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[1]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_3_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[2]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_4_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[3]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_5_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[4]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_6_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[5]);
    soc_reg_above_64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_7_DEVf, rci_config.soc_dpp_fabric_rci_device_level_thresholds[6]);
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val));
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_fabric_rci_thresholds_config_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_OUT soc_dpp_fabric_rci_config_t    *rci_config)
{
    int i;
    soc_reg_above_64_val_t reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_3r(unit, reg_val));
    rci_config->soc_dpp_fabric_rci_severity_factors[0] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_1_FACTORf);
    rci_config->soc_dpp_fabric_rci_severity_factors[1] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_2_FACTORf);
    rci_config->soc_dpp_fabric_rci_severity_factors[2] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_LEVEL_3_FACTORf);
    rci_config->soc_dpp_fabric_rci_threshold_num_of_congested_links = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_HIGH_SEV_MIN_NOF_LINKSf);
    rci_config->soc_dpp_fabric_rci_high_score_congested_links = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_HIGH_SEV_SCOREf);
    rci_config->soc_dpp_fabric_rci_high_score_fabric_rx_local_queue = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_IPT_FIFO_SCOREf);
    rci_config->soc_dpp_fabric_rci_high_score_fabric_rx_queue = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_FDA_FIFO_SCOREf);

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[0] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_1f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[1] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_2f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[2] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_3f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[3] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_4f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[4] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_5f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[5] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_6f);
        rci_config->soc_dpp_fabric_rci_core_level_thresholds[6] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_7f);
    }else{
        
        for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
            rci_config->soc_dpp_fabric_rci_core_level_thresholds[i] = 0x0;
        }
    }

    rci_config->soc_dpp_fabric_rci_device_level_thresholds[0] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_1_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[1] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_2_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[2] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_3_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[3] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_4_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[4] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_5_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[5] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_6_DEVf);
    rci_config->soc_dpp_fabric_rci_device_level_thresholds[6] = soc_reg_above_64_field32_get(unit, FDR_FDR_ENABLERS_REGISTER_3r, reg_val, RCI_SCORE_LEVEL_TH_7_DEVf);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_link_repeater_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int enable,
    SOC_SAND_IN  int empty_cell_size)
{
    int nof_links_in_mac,
        fmac_instance,
        inner_link, inner_link_enable,
        include_empties;
    uint32 reg32_val;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_instance, &inner_link));

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, fmac_instance,  inner_link, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val, FIELD_21_28f, empty_cell_size);
        soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val, FIELD_29_29f, enable);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, fmac_instance,  inner_link,  reg64_val));
    } else {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_TX_LLFC_CELL_SIZEf, empty_cell_size);
        soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, reg32_val));
    }

    
    if (enable) {
       include_empties = 1;
    } else {
        include_empties = 0;
        
        for (inner_link = 0; inner_link < nof_links_in_mac; inner_link++) {
            if(SOC_IS_QAX(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, fmac_instance,  inner_link, &reg64_val));
                inner_link_enable = soc_reg64_field32_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg64_val, FIELD_29_29f);
            } else {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
                inner_link_enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf);
            }
            if (inner_link_enable) {
               include_empties = 1;
               break;
            }
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_instance, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_LLFC_CELLS_GENf, include_empties); 
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_instance, reg64_val));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_link_repeater_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int *enable,
    SOC_SAND_OUT int *empty_cell_size)
{
    int fmac_instance,
        inner_link;
    uint32 reg32_val;

    SOCDNX_INIT_FUNC_DEFS;



    if (SOC_IS_QAX(unit))
    {
      LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,"access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
      SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);

    }


    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_instance, &inner_link));

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
    *empty_cell_size = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZEf); 
    *enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
soc_jer_fabric_cell_cpu_data_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT uint32                  *cell_buffer
  )
{
    int i;
    uint32 reg_val, field_val = 0;
    soc_reg_above_64_val_t data;
    const int interrupt_fields[] = {P_1_CPUDATACELLFNE_A_0f, P_1_CPUDATACELLFNE_A_1f, P_1_CPUDATACELLFNE_B_0f, P_1_CPUDATACELLFNE_B_1f};
    const int data_registers[] = {FDR_P_1_CPU_DATA_CELL_Ar, FDR_P_1_CPU_DATA_CELL_Br, FDR_P_1_CPU_DATA_CELL_Cr, FDR_P_1_CPU_DATA_CELL_Dr};
                                 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cell_buffer);

    
    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_INTERRUPT_REGISTER_1r(unit, &reg_val)); 
    for (i = 0; i < SOC_DPP_DEFS_GET(unit, fabric_sr_cell_received_nof_sides); ++i) {
        field_val = soc_reg_field_get(unit, FDR_FDR_INTERRUPT_REGISTER_1r, reg_val, interrupt_fields[i]);
        if (field_val) {
            break;
        }
    }
    if (!field_val) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EMPTY, (_BSL_SOC_MSG("All FIFO's are empty, nothing to read")));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, data_registers[i], REG_PORT_ANY, 0, data));
    sal_memcpy(cell_buffer, data, SOC_JER_FABRIC_DATA_CELL_RECEIVED_SIZE_IN_BYTES);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_fabric_queues_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t    *queues_info
  )
{
    uint32 reg32_val;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_FIFOS_MAX_OCC_1r(unit, &reg32_val));
    queues_info->soc_dpp_fabric_dtq_data_max_occ_val.info = soc_reg_field_get(unit, IPT_FIFOS_MAX_OCC_1r, reg32_val, FDTD_MAX_OCf);
    queues_info->soc_dpp_fabric_dtq_data_max_occ_ctx.info = soc_reg_field_get(unit, IPT_FIFOS_MAX_OCC_1r, reg32_val, FDTD_MAX_OC_QNUMf);
    queues_info->soc_dpp_fabric_dtq_ctrl_max_occ_val.info = soc_reg_field_get(unit, IPT_FIFOS_MAX_OCC_1r, reg32_val, FDTC_MAX_OCf);
    queues_info->soc_dpp_fabric_dtq_ctrl_max_occ_ctx.info = soc_reg_field_get(unit, IPT_FIFOS_MAX_OCC_1r, reg32_val, FDTC_MAX_OC_QNUMf);
    
    queues_info->soc_dpp_fabric_dtq_data_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_dtq_data_max_occ_ctx.is_valid = 1;
    queues_info->soc_dpp_fabric_dtq_ctrl_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_dtq_ctrl_max_occ_ctx.is_valid = 1;

    
    
    SOCDNX_IF_ERR_EXIT(READ_IPT_FIFOS_MAX_OCC_0r(unit, &reg64_val));
    queues_info->soc_dpp_fabric_dblf0_max_occ_val.info = soc_reg64_field32_get(unit, IPT_FIFOS_MAX_OCC_0r, reg64_val, DBLF_0_MAX_OCf);
    queues_info->soc_dpp_fabric_dblf0_max_occ_ctx.info = soc_reg64_field32_get(unit, IPT_FIFOS_MAX_OCC_0r, reg64_val, DBLF_0_MAX_OC_QNUMf);
    queues_info->soc_dpp_fabric_dblf1_max_occ_val.info = soc_reg64_field32_get(unit, IPT_FIFOS_MAX_OCC_0r, reg64_val, DBLF_1_MAX_OCf);
    queues_info->soc_dpp_fabric_dblf1_max_occ_ctx.info = soc_reg64_field32_get(unit, IPT_FIFOS_MAX_OCC_0r, reg64_val, DBLF_1_MAX_OC_QNUMf);

    queues_info->soc_dpp_fabric_dblf0_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_dblf0_max_occ_ctx.is_valid = 1;
    queues_info->soc_dpp_fabric_dblf1_max_occ_val.is_valid = 1;
    queues_info->soc_dpp_fabric_dblf1_max_occ_ctx.is_valid = 1;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
  soc_jer_fabric_mesh_topology_get(
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_OUT soc_dcmn_fabric_mesh_topology_diag_t    *mesh_topology_diag
  )
{
    soc_reg_t reg_list[] = {MESH_TOPOLOGY_REG_01A0r, MESH_TOPOLOGY_REG_01A1r, MESH_TOPOLOGY_REG_01A2r, MESH_TOPOLOGY_REG_01A3r, MESH_TOPOLOGY_REG_01A4r, MESH_TOPOLOGY_REG_01A5r, 
                            MESH_TOPOLOGY_REG_01A6r, MESH_TOPOLOGY_REG_01A7r, MESH_TOPOLOGY_REG_01A8r, MESH_TOPOLOGY_REG_01A9r, MESH_TOPOLOGY_REG_01AAr, MESH_TOPOLOGY_REG_01ABr, 
                            MESH_TOPOLOGY_REG_01ACr, MESH_TOPOLOGY_REG_01ADr, MESH_TOPOLOGY_REG_01AEr, MESH_TOPOLOGY_REG_01AFr, MESH_TOPOLOGY_REG_01B0r, MESH_TOPOLOGY_REG_01B1r};
    uint32 reg32_val;
    uint64 reg64_val;
    soc_reg_t reg;
    int i, valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_INTERRUPTSr(unit, &reg32_val));
    mesh_topology_diag->indication_1 = soc_reg_field_get(unit, MESH_TOPOLOGY_MESH_INTERRUPTSr, reg32_val, INTERRUPT_1f);
    mesh_topology_diag->indication_2 = soc_reg_field_get(unit, MESH_TOPOLOGY_MESH_INTERRUPTSr, reg32_val, INTERRUPT_2f);

    for (i = 0; i < sizeof(reg_list) / sizeof(soc_reg_t); ++i) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_15_15f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_0_14f);
        } else {
            mesh_topology_diag->type2_list[i*2] = -1;
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg_list[i], REG_PORT_ANY, 0, &reg32_val));
        valid = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_31_31f);
        if (valid) {
            mesh_topology_diag->type2_list[i*2 + 1] = soc_reg_field_get(unit, reg_list[i], reg32_val, FIELD_16_30f);
        } else {
            mesh_topology_diag->type2_list[i*2 + 1] = -1;
        }
    }
    mesh_topology_diag->link_list_count = i*2;

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr(unit, &reg32_val));
    mesh_topology_diag->rx_control_cells_type1 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_1f);
    mesh_topology_diag->rx_control_cells_type2 = soc_reg_field_get(unit, MESH_TOPOLOGY_FAP_DETECT_CTRL_CELLS_CNTr, reg32_val, RCV_CTL_2f);

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_STATUSESr(unit, &reg32_val));
    mesh_topology_diag->indication_3 = soc_reg_field_get(unit, MESH_TOPOLOGY_MESH_STATUSESr, reg32_val, MESH_STATUS_4f);
    mesh_topology_diag->indication_4 = soc_reg_field_get(unit, MESH_TOPOLOGY_MESH_STATUSESr, reg32_val, MESH_STATUS_0f);
    mesh_topology_diag->control_cells_type3 = soc_reg_field_get(unit, MESH_TOPOLOGY_MESH_STATUSESr, reg32_val, MESH_STATUS_2f);

    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_STATUS_2r(unit, &reg64_val));
    mesh_topology_diag->status_1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_STATUS_2r, reg64_val, STATUS_2f);
    mesh_topology_diag->status_1_id1 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_STATUS_2r, reg64_val, FIELD_32_38f);
    mesh_topology_diag->status_1_id2 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_STATUS_2r, reg64_val, FIELD_39_49f);

    
    reg = ( SOC_IS_JERICHO_PLUS_ONLY(unit) ? MESH_TOPOLOGY_REG_01BFr : MESH_TOPOLOGY_REG_01B9r );
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg64_val));
    mesh_topology_diag->status_2 = soc_reg64_field32_get(unit, reg, reg64_val, FIELD_0_31f);
    mesh_topology_diag->status_2_id1 = soc_reg64_field32_get(unit, reg, reg64_val, FIELD_32_38f);
    mesh_topology_diag->status_2_id2 = soc_reg64_field32_get(unit, reg, reg64_val, FIELD_39_49f);

exit:
    SOCDNX_FUNC_RETURN;
}

#define MESH_STATUS_TH 256

uint32 
  soc_jer_fabric_mesh_check(
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_IN uint8                                    stand_alone,
    SOC_SAND_OUT uint8                                   *success
  )
{
    uint32 threshold, pos_watermark, neg_watermark;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, MESH_TOPOLOGY_STATUS_2r, REG_PORT_ANY, 0, STATUS_2f, &pos_watermark));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO_PLUS_A0(unit)?MESH_TOPOLOGY_REG_01BFr:MESH_TOPOLOGY_REG_01B9r), REG_PORT_ANY, 0, FIELD_0_31f, &neg_watermark));

    threshold = MESH_STATUS_TH;

    if ( ((pos_watermark < threshold) && (neg_watermark < threshold)) || (stand_alone == TRUE))
    {
      *success = TRUE;
    }
    else
    {
    *success = FALSE; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_fabric_rx_fifo_status_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_OUT soc_dpp_fabric_rx_fifo_diag_t     *rx_fifo_diag
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_QAX(unit)){
        SOCDNX_IF_ERR_EXIT(READ_FDA_EGQ_FAB_FIFOS_WMr(unit, 0, &reg_val));
        rx_fifo_diag->soc_dpp_fabric_egq0_pipe1_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_0_FIFO_WMf);
        rx_fifo_diag->soc_dpp_fabric_egq0_pipe2_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_1_FIFO_WMf);
        rx_fifo_diag->soc_dpp_fabric_egq0_pipe3_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_2_FIFO_WMf);
        SOCDNX_IF_ERR_EXIT(READ_FDA_EGQ_FAB_FIFOS_WMr(unit, 1, &reg_val));
        rx_fifo_diag->soc_dpp_fabric_egq1_pipe1_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_0_FIFO_WMf);
        rx_fifo_diag->soc_dpp_fabric_egq1_pipe2_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_1_FIFO_WMf);
        rx_fifo_diag->soc_dpp_fabric_egq1_pipe3_watermark = soc_reg_field_get(unit, FDA_EGQ_FAB_FIFOS_WMr, reg_val, EGQ_N_FAB_2_FIFO_WMf);
    }else{
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Fabric FIFO watermarks are not relevant in QAX")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
  soc_jer_fabric_port_sync_e_divider_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             is_master,
    SOC_SAND_OUT int             *divider)
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 reg32_val, phase1_div, phase0_div;
    soc_field_t field;
    int fsrd_block;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        field = (SOC_IS_JERICHO_PLUS_ONLY(unit))? SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SEL_MASTERf : SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SELf;
        SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
        fsrd_block = soc_reg_above_64_field32_get(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above64_val, field);

        SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg32_val));
        *divider = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg32_val, SYNC_E_CLK_DIV_PHASE_1f) + 1; 
    } else {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, (is_master ? 0 : 1), &reg32_val));
        phase0_div = soc_reg_field_get(unit, NBIH_SYNC_ETH_CFGr, reg32_val, SYNC_ETH_DIVIDER_PHASE_ZERO_Nf);
        phase1_div = soc_reg_field_get(unit, NBIH_SYNC_ETH_CFGr, reg32_val, SYNC_ETH_DIVIDER_PHASE_ONE_Nf);
        *divider = phase0_div + phase1_div + 2;
    }
exit:
    SOCDNX_FUNC_RETURN; 
}

uint32 
  soc_jer_fabric_port_sync_e_divider_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             is_master,
    SOC_SAND_IN  int             divider)
{
    uint32 reg32_val;
    int phase1_divider, i;
    SOCDNX_INIT_FUNC_DEFS;

    if ((divider < SOC_JER_FABRIC_SYNC_E_MIN_DIVIDER) || (divider > SOC_JER_FABRIC_SYNC_E_MAX_DIVIDER)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid divider for syncE configuration, must be between 2 to 16")));
    }
    if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        phase1_divider = divider - 1;
        
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); ++i){
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, i, &reg32_val));
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg32_val, SYNC_E_CLK_DIV_PHASE_1f, phase1_divider);
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg32_val, SYNC_E_CLK_DIV_PHASE_0f, phase1_divider/2);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, i, reg32_val));
        }
    } else {
        
        SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, (is_master ? 0 : 1), &reg32_val));
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg32_val, SYNC_ETH_CLOCK_DIV_Nf, 2);
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg32_val, SYNC_ETH_DIVIDER_PHASE_ZERO_Nf, ((divider + 1) / 2) - 1);
        soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg32_val, SYNC_ETH_DIVIDER_PHASE_ONE_Nf, (divider / 2) - 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_SYNC_ETH_CFGr(unit, (is_master ? 0 : 1), reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

uint32
  soc_jer_fabric_port_sync_e_link_set(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_IN  int                                port
  )
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 reg32_val;
    soc_field_t field;
    int fsrd_block = 0, inner_fsrd_port = 0, dummy = 1;
    SOCDNX_INIT_FUNC_DEFS;

    if ((port < 0) || (port >= SOC_INFO(unit).fabric_logical_port_base + SOC_DPP_DEFS_GET(unit, nof_fabric_links)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid port %d for syncE configuration"), port));
    }
    if (!IS_SFI_PORT(unit, port))
    {
        SOC_EXIT;
    }

    if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        fsrd_block = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        fsrd_block /= SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

        inner_fsrd_port = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        inner_fsrd_port %= SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg32_val));
    }

    if (is_master) {
        
        if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            SOCDNX_IF_ERR_EXIT(jer_plus_pll_set(unit, 0, dummy, dummy, dummy, 1));
        } else {
            SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, JER_PLL_TYPE_SYNCE_0, dummy, dummy, dummy, 1));
        }

        if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
            
            SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
            field = SOC_IS_JERICHO_PLUS_ONLY(unit)? SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SEL_MASTERf : SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SELf;
            soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above64_val, field, fsrd_block);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));

                
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg32_val, MASTER_CLK_SELf, inner_fsrd_port);
        } else {
            SOCDNX_IF_ERR_EXIT(soc_jer_port_synce_clk_sel_set(unit, 0, port));
        }
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, 1, 1));
    } else {
        
        if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            SOCDNX_IF_ERR_EXIT(jer_plus_pll_set(unit, 1, dummy, dummy, dummy, 1));
        } else {
            SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, JER_PLL_TYPE_SYNCE_1, dummy, dummy, dummy, 1));
        }

        if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
            
            SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));
            field = SOC_IS_JERICHO_PLUS_ONLY(unit)? SYNCE_SLAVE_PLL_CFG_REFRENCE_FABRIC_SEL_SLAVEf : SYNCE_SLAVE_PLL_CFG_REFRENCE_FABRIC_SELf;
            soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, reg_above64_val, field, fsrd_block);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));

            
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg32_val, SLAVE_CLK_SELf, inner_fsrd_port);
        } else {
            SOCDNX_IF_ERR_EXIT(soc_jer_port_synce_clk_sel_set(unit, 1, port));
        }

        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, 0, 1));
    }
    if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, fsrd_block, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_port_sync_e_link_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *port
  )
{
    soc_reg_above_64_val_t reg_above64_val;
    uint32 reg32_val, phy_lane;
    soc_field_t field;
    int fsrd_block, inner_fsrd_port;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_QAX_WITH_FABRIC_ENABLED(unit)) {
        if (is_master) {
            SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
            field = SOC_IS_JERICHO_PLUS_ONLY(unit)? SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SEL_MASTERf : SYNCE_MASTER_PLL_CFG_REFRENCE_FABRIC_SELf;
            fsrd_block = soc_reg_above_64_field32_get(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above64_val, field);

            SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg32_val));
            inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg32_val, MASTER_CLK_SELf);
        } else {
            SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));
            field = SOC_IS_JERICHO_PLUS_ONLY(unit)? SYNCE_SLAVE_PLL_CFG_REFRENCE_FABRIC_SEL_SLAVEf : SYNCE_SLAVE_PLL_CFG_REFRENCE_FABRIC_SELf;
            fsrd_block = soc_reg_above_64_field32_get(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, reg_above64_val, field);

            SOCDNX_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg32_val));
            inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg32_val, SLAVE_CLK_SELf);
        }

        *port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, fsrd_block * SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd) + inner_fsrd_port);
    } else {
        SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, (is_master ? 0 : 1), &reg32_val));
        phy_lane = soc_reg_field_get(unit, NBIH_SYNC_ETH_CFGr, reg32_val, SYNC_ETH_VALID_SEL_Nf);
        *port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, phy_lane);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_sync_e_enable_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                is_master,
    SOC_SAND_IN  int                                to_enable
  )
{
    soc_reg_above_64_val_t reg_above64_val;
    int set_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    set_val = to_enable? 0 : 1;

    if(is_master)
    {
        field = (SOC_IS_JERICHO_PLUS_ONLY(unit))? SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SEL_MASTERf : SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SELf;
        SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
        soc_reg_above_64_field32_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above64_val, field, set_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
    }
    else
    {
        field = (SOC_IS_JERICHO_PLUS_ONLY(unit))? SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SEL_SLAVEf : SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SELf;
        SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));
        soc_reg_above_64_field32_set(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, reg_above64_val, field, set_val);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_fabric_sync_e_enable_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *is_fabric_synce
  )
{
    soc_reg_above_64_val_t reg_above64_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    if(is_master)
    {
        field = (SOC_IS_JERICHO_PLUS_ONLY(unit))? SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SEL_MASTERf : SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SELf;
        SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_MASTER_PLL_CONFIGr(unit, reg_above64_val));
        *is_fabric_synce = soc_reg_above_64_field32_get(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above64_val, field);
    }
    else
    {
        field = (SOC_IS_JERICHO_PLUS_ONLY(unit))? SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SEL_SLAVEf : SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SELf;
        SOCDNX_IF_ERR_EXIT(READ_ECI_SYNCE_SLAVE_PLL_CONFIGr(unit, reg_above64_val));
        *is_fabric_synce = soc_reg_above_64_field32_get(unit, ECI_SYNCE_SLAVE_PLL_CONFIGr, reg_above64_val, field);
    }

    *is_fabric_synce = (*is_fabric_synce)? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_jer_fabric_force_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  )
{
    int modid;
    uint32  sys_fap_id;
    uint32  fap_id_plus_1;
    uint64  reg64;
    uint32  reg32_val;
    uint8 is_traffic_enabled;
    uint8 is_ctrl_cells_enabled;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_SAND_IF_ERR_EXIT(jer_mgmt_enable_traffic_get(unit, &is_traffic_enabled));
    SOC_SAND_IF_ERR_EXIT(arad_mgmt_all_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled));

    if (is_traffic_enabled || is_ctrl_cells_enabled) {
        LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit,
                                        " Warning: fabric force should not be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n")));
    }

     

    if (force == socDppFabricForceFabric) {
        
        sys_fap_id = 0xFFFFFFFF;
        fap_id_plus_1 = 0xFFFFFFFF;
        COMPILER_64_ZERO(reg64); 
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));

        
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
    } else if (force == socDppFabricForceLocal0) {
        
        sys_fap_id = 0;
        fap_id_plus_1 = 0xFFFFFFFF;
        COMPILER_64_ZERO(reg64); 
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0xFFFFFFFF);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));

        
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
    } else if (force == socDppFabricForceLocal1) {
        
        sys_fap_id = 0xFFFFFFFF;
        fap_id_plus_1 = 0;
        COMPILER_64_ZERO(reg64); 
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0xFFFFFFFF);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));

        
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
    } else if (force == socDppFabricForceRestore) {
        
        SOCDNX_IF_ERR_EXIT(bcm_stk_my_modid_get(unit,&modid));
        sys_fap_id = modid;
        fap_id_plus_1 = sys_fap_id + 1;
        if(SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
            fap_id_plus_1 = 0xFFFFFFFF;
        } 
        COMPILER_64_ZERO(reg64); 
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MAP_LSBf, sys_fap_id | (sys_fap_id << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_1r, &reg64, DEV_ID_TO_DQCQ_MASKf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_1r(unit, reg64));
        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MAP_MSBf, fap_id_plus_1 | (fap_id_plus_1 << JER_FAPID_BIT_OFFSET_IN_DQCQ_MAP));
        soc_reg64_field32_set(unit, ECI_GLOBAL_DQCQ_MAP_CFG_2r, &reg64, DEV_ID_TO_DQCQ_MASK_1f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DQCQ_MAP_CFG_2r(unit, reg64));

        
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));
    } else {      
        cli_out("Option not supported\n");
        cli_out("Supported options are fabric, core0, core1, default\n");
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }


exit:
  SOCDNX_FUNC_RETURN;
}


int
  soc_jer_fabric_efms_enable_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             modid,
    SOC_SAND_IN  int                val
  )
{

    int    err = 0;
    uint32 reg_val = 0;
    uint32 reg_field_val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("This function is not supported")));
    }

    
    
    err = READ_IPS_FMSBYPm(unit, IPS_BLOCK(unit,0), ((uint32)modid)/16 , &reg_val);
    SOCDNX_IF_ERR_EXIT(err);
    
    reg_field_val = soc_mem_field32_get(unit, IPS_FMSBYPm, &reg_val, FMS_BYP_BMPf);
    
    if (val > 0) 
    {
        SOC_SAND_SET_BIT(reg_field_val, 0x1, ((uint32)modid)%16);
    }
    else
    {
        SOC_SAND_SET_BIT(reg_field_val, 0x0, ((uint32)modid)%16);
    }
    soc_mem_field32_set(unit, IPS_FMSBYPm, &reg_val, FMS_BYP_BMPf, reg_field_val);
    
    err = WRITE_IPS_FMSBYPm(unit, IPS_BLOCK(unit,SOC_CORE_ALL), ((uint32)modid)/16 , &reg_val);
    SOCDNX_IF_ERR_EXIT(err);

    
    
    err = READ_FCR_EFMS_SOURCE_PIPEm(unit, FCR_BLOCK(unit), ((uint32)modid)/8, &reg_val);
    SOCDNX_IF_ERR_EXIT(err);
    
    reg_field_val = soc_mem_field32_get(unit, FCR_EFMS_SOURCE_PIPEm, &reg_val, DATAf);
    
    if (val > 0) 
    {
         SOC_SAND_SET_BIT(reg_field_val, 0x0, ((uint32)modid)%8);
    }
    else
    {
         SOC_SAND_SET_BIT(reg_field_val, 0x1, ((uint32)modid)%8);
    }
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, &reg_val, DATAf, reg_field_val);

    
    err = WRITE_FCR_EFMS_SOURCE_PIPEm(unit, FCR_BLOCK(unit), ((uint32)modid)/8, &reg_val);
    SOCDNX_IF_ERR_EXIT(err);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  soc_jer_fabric_efms_enable_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             modid,
    SOC_SAND_OUT  int               *val
  )
{
    int    err = 0;
    uint32 fcr_boolval = 0, ips_boolval = 0;
    uint32 reg_val = 0;
    uint32 reg_field_val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("This function is not supported")));
    }

    
    
    err = READ_IPS_FMSBYPm(unit, IPS_BLOCK(unit,0), ((uint32)modid)/16 , &reg_val);
    SOCDNX_IF_ERR_EXIT(err);

    
    reg_field_val = soc_mem_field32_get(unit, IPS_FMSBYPm, &reg_val, FMS_BYP_BMPf);
    
    ips_boolval = SOC_SAND_GET_BIT(reg_field_val, ((uint32)modid)%16);

    
    
    err = READ_FCR_EFMS_SOURCE_PIPEm(unit, FCR_BLOCK(unit), ((uint32)modid)/8, &reg_val);
    SOCDNX_IF_ERR_EXIT(err);
    
    reg_field_val = soc_mem_field32_get(unit, FCR_EFMS_SOURCE_PIPEm, &reg_val, DATAf);
    
    fcr_boolval = SOC_SAND_GET_BIT(reg_field_val, ((uint32)modid)%8);


    if (ips_boolval == fcr_boolval) 
    {
        SOCDNX_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    *val = (fcr_boolval? 0:1);

exit:
  SOCDNX_FUNC_RETURN;
}



soc_error_t
  soc_jer_fabric_link_config_ovrd(
    int                unit
  )
{

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QMX(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, 16);
        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, 4);
        SOC_DPP_DEFS_SET(unit, first_fabric_link_id, 8);
        SOC_DPP_DEFS_SET(unit, nof_sch_active_links, 16);
    }


  SOCDNX_FUNC_RETURN;
}

#ifdef BCM_FLAIR_SUPPORT
soc_error_t
  soc_jer_fabric_fgl_egress_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_egress_config_t    *fgl_egress_config)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGRESS_CONFIGr(unit, &reg_val));
    fgl_egress_config->egress_drop_threshold = soc_reg_field_get(unit, ECI_FGL_EGRESS_CONFIGr, reg_val, EGRESS_DROP_THRESHOLDf);
    fgl_egress_config->egress_almost_full_threshold = soc_reg_field_get(unit, ECI_FGL_EGRESS_CONFIGr, reg_val, EGRESS_ALMOST_FULL_THRESHOLDf);
    fgl_egress_config->egress_ftmh_mode = (soc_reg_field_get(unit, ECI_FGL_EGRESS_CONFIGr, reg_val, EGRESS_FTMH_MODEf) == 1) ? NINE_BYTE_STANDARD_FTMH : FOUR_BYTE_TDM_FTMH;
    fgl_egress_config->egress_packet_crc = soc_reg_field_get(unit, ECI_FGL_EGRESS_CONFIGr, reg_val, EGRESS_PACKET_CRCf);
    fgl_egress_config->egress_packet_crc_discard = soc_reg_field_get(unit, ECI_FGL_EGRESS_CONFIGr, reg_val, EGRESS_PACKET_CRC_DISCARDf);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_egress_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_egress_config_t    fgl_egress_config)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    
    if ((fgl_egress_config.egress_ftmh_mode != NINE_BYTE_STANDARD_FTMH) && (fgl_egress_config.egress_ftmh_mode != FOUR_BYTE_TDM_FTMH)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid egress FTMH mode %d"), fgl_egress_config.egress_ftmh_mode));
    }
    if ((fgl_egress_config.egress_packet_crc != 0) && (fgl_egress_config.egress_packet_crc != 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid egress packet CRC configuration %d"), fgl_egress_config.egress_packet_crc));
    }
    if ((fgl_egress_config.egress_packet_crc_discard!= 0) && (fgl_egress_config.egress_packet_crc_discard != 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid egress packet CRC discard configuration %d"), fgl_egress_config.egress_packet_crc_discard));
    }
    if ((fgl_egress_config.egress_packet_crc == 0) && (fgl_egress_config.egress_packet_crc_discard != 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("mismatch egress packet CRC configuration %d and egress packet CRC discard configuration %d"), fgl_egress_config.egress_packet_crc, fgl_egress_config.egress_packet_crc_discard));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGRESS_CONFIGr(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FGL_EGRESS_CONFIGr, &reg_val, EGRESS_DROP_THRESHOLDf, fgl_egress_config.egress_drop_threshold);
    soc_reg_field_set(unit, ECI_FGL_EGRESS_CONFIGr, &reg_val, EGRESS_ALMOST_FULL_THRESHOLDf, fgl_egress_config.egress_almost_full_threshold);
    soc_reg_field_set(unit, ECI_FGL_EGRESS_CONFIGr, &reg_val, EGRESS_FTMH_MODEf,fgl_egress_config.egress_ftmh_mode);
    soc_reg_field_set(unit, ECI_FGL_EGRESS_CONFIGr, &reg_val, EGRESS_PACKET_CRCf, fgl_egress_config.egress_packet_crc);
    soc_reg_field_set(unit, ECI_FGL_EGRESS_CONFIGr, &reg_val, EGRESS_PACKET_CRC_DISCARDf, fgl_egress_config.egress_packet_crc_discard);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGRESS_CONFIGr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_jer_fabric_fgl_ingress_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_ingress_config_t    *fgl_ingress_config)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_INGRESS_CONFIGr(unit, &reg_val));
    fgl_ingress_config->tdm_min_size = soc_reg_field_get(unit, ECI_FGL_INGRESS_CONFIGr, reg_val, TDM_MIN_SIZEf);
    fgl_ingress_config->tdm_max_size = soc_reg_field_get(unit, ECI_FGL_INGRESS_CONFIGr, reg_val, TDM_MAX_SIZEf);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_ingress_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_ingress_config_t    fgl_ingress_config)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    if (fgl_ingress_config.tdm_min_size < SOC_JER_FABRIC_FGL_INGRESS_TDM_MIN_SIZE){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid TDM min size for FGL ingress, must be larger or equal than 65")));
    }
    if (fgl_ingress_config.tdm_max_size > SOC_JER_FABRIC_FGL_INGRESS_TDM_MAX_SIZE){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid TDM max size for FGL ingress, must be smaller or equal than 256")));
    }
    if (fgl_ingress_config.tdm_max_size < fgl_ingress_config.tdm_min_size){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TDM max size should be larger or equal than TDM min size")));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_INGRESS_CONFIGr(unit, &reg_val));

    soc_reg_field_set(unit, ECI_FGL_INGRESS_CONFIGr, &reg_val, TDM_MIN_SIZEf, (uint32)fgl_ingress_config.tdm_min_size);
    soc_reg_field_set(unit, ECI_FGL_INGRESS_CONFIGr, &reg_val, TDM_MAX_SIZEf, (uint32)fgl_ingress_config.tdm_max_size);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_INGRESS_CONFIGr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_loopback_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_loopback_config_t    *fgl_loopback_config)
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_64;
    int i;
    int offset;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_LOOPBACK_CONTROLr(unit, &reg_val));
    fgl_loopback_config->egr_to_ing_lpck = soc_reg_field_get(unit, ECI_FGL_LOOPBACK_CONTROLr, reg_val, EGR_TO_ING_LPCKf);
    fgl_loopback_config->ing_to_egr_lpck = soc_reg_field_get(unit, ECI_FGL_LOOPBACK_CONTROLr, reg_val, ING_TO_EGR_LPCKf);
    fgl_loopback_config->egr_to_ing_lpck_mode = (soc_reg_field_get(unit, ECI_FGL_LOOPBACK_CONTROLr, reg_val, EGR_TO_ING_LPCK_MODEf)==0) ? ALL_CHANNELS: ONLY_SELECTED_CHANNELS;

    SOC_REG_ABOVE_64_CLEAR(reg_val_64);
    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGR_2_ING_LOOPBACK_CONTROLr(unit, reg_val_64));

    offset = 0;
    for (i = 0; i < (sizeof(fgl_loopback_config->egr_to_ing_lpck_framer_en) / sizeof(uint32)); i++) {
        SHR_BITCOPY_RANGE(&fgl_loopback_config->egr_to_ing_lpck_framer_en[i], 0, reg_val_64, (i*32), 32);
        offset += 32;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_loopback_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_loopback_config_t    fgl_loopback_config)
{
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_64;
    int i;
    int offset;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_LOOPBACK_CONTROLr(unit, &reg_val));
    soc_reg_field_set(unit, ECI_FGL_LOOPBACK_CONTROLr, &reg_val, EGR_TO_ING_LPCKf, (uint32)fgl_loopback_config.egr_to_ing_lpck);
    soc_reg_field_set(unit, ECI_FGL_LOOPBACK_CONTROLr, &reg_val, ING_TO_EGR_LPCKf, (uint32)fgl_loopback_config.ing_to_egr_lpck);
    soc_reg_field_set(unit, ECI_FGL_LOOPBACK_CONTROLr, &reg_val, EGR_TO_ING_LPCK_MODEf, (uint32)((fgl_loopback_config.egr_to_ing_lpck_mode==ALL_CHANNELS) ? 0 : 1));
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_LOOPBACK_CONTROLr(unit, reg_val));
    SOC_REG_ABOVE_64_CLEAR(reg_val_64);
    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGR_2_ING_LOOPBACK_CONTROLr(unit, reg_val_64));

    offset = 0;
    for (i = 0; i < (sizeof(fgl_loopback_config.egr_to_ing_lpck_framer_en) / sizeof(uint32)); i++) {
        SHR_BITCOPY_RANGE( reg_val_64, (i*32), &fgl_loopback_config.egr_to_ing_lpck_framer_en[i], 0, 32);
        offset += 32;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGR_2_ING_LOOPBACK_CONTROLr(unit, reg_val_64));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_ingress_tdm_context_drop_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_ingress_tdm_context_drop_t    *tdm_context_drop)
{
    soc_reg_above_64_val_t reg_val_64;
    int i;
    int offset;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOC_REG_ABOVE_64_CLEAR(reg_val_64);
    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_INGRESS_TDM_CONTEXT_DROPr(unit, reg_val_64));

    offset = 0;
    for (i = 0; i < (sizeof(tdm_context_drop->bitmap) / sizeof(uint32)); i++) {
        SHR_BITCOPY_RANGE(&tdm_context_drop->bitmap[i], 0, reg_val_64, (i*32), 32);
        offset += 32;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_fgl_ingress_tdm_context_drop_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_ingress_tdm_context_drop_t    tdm_context_drop)
{
    soc_reg_above_64_val_t reg_val_64;
    int i;
    int offset;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOC_REG_ABOVE_64_CLEAR(reg_val_64);
    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_INGRESS_TDM_CONTEXT_DROPr(unit, reg_val_64));

    offset = 0;
    for (i = 0; i < (sizeof(tdm_context_drop.bitmap) / sizeof(uint32)); i++) {
        SHR_BITCOPY_RANGE(reg_val_64, (i*32), &tdm_context_drop.bitmap[i], 0, 32);
        offset += 32;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_INGRESS_TDM_CONTEXT_DROPr(unit, reg_val_64));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_egr_mc_mapping_mode_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN int    egr_mc_mapping_mode)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    if ((egr_mc_mapping_mode != FABRIC_FGL_EGR_MC_MAPPING_MODE_MC_ID) &&
        (egr_mc_mapping_mode != FABRIC_FGL_EGR_MC_MAPPING_MODE_EGR_MCT_MAPPING)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid EGR MC mapping mode")));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGR_MC_CONTROLr(unit, &reg_val));

    soc_reg_field_set(unit, ECI_FGL_EGR_MC_CONTROLr, &reg_val, FGL_EGR_MC_MAPPING_MODEf, (uint32)egr_mc_mapping_mode);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FGL_EGR_MC_CONTROLr(unit, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_egr_mc_mapping_mode_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT int    *egr_mc_mapping_mode)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("this feature is unavailable for current device."))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_FGL_EGR_MC_CONTROLr(unit, &reg_val));
    *egr_mc_mapping_mode = soc_reg_field_get(unit, ECI_FGL_EGR_MC_CONTROLr, reg_val, FGL_EGR_MC_MAPPING_MODEf);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_tdm_config_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_tdm_config_t    *eci_egr_tdm_config)
{
    SHR_BITDCLNAME (data, 126);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_TDM_CONFIG_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_TDM_CONFIG_ENTRY_NUMBER));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_TDM_CONFIGm(unit, MEM_BLOCK_ANY, index, data)); 

    eci_egr_tdm_config->mode = soc_mem_field32_get(
            unit,
            ECI_TDM_CONFIGm,
            data,
            MODEf );

    eci_egr_tdm_config->cpu   = soc_mem_field32_get(
            unit,
            ECI_TDM_CONFIGm,
            data,
            CPUf );

    soc_mem_field_get(
                    unit,
                    ECI_TDM_CONFIGm,
                    data,
                    HEADERf,
                eci_egr_tdm_config->header);

    eci_egr_tdm_config->link_mask_ptr = soc_mem_field32_get(
                    unit,
                    ECI_TDM_CONFIGm,
                    data,
                    LINK_MASK_PTRf );

    eci_egr_tdm_config->add_packet_crc  = soc_mem_field32_get(
                    unit,
                    ECI_TDM_CONFIGm,
                    data,
                    ADD_PACKET_CRCf );

    eci_egr_tdm_config->mc_replication  = soc_mem_field32_get(
                        unit,
                        ECI_TDM_CONFIGm,
                        data,
                        MC_REPLICATIONf );

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_tdm_config_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_tdm_config_t    eci_egr_tdm_config)
{
    int rv = 0;
    SHR_BITDCLNAME (data, 126);
    SOCDNX_INIT_FUNC_DEFS

    soc_mem_field32_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        MODEf,
        eci_egr_tdm_config.mode );

    soc_mem_field32_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        CPUf,
        eci_egr_tdm_config.cpu );

    soc_mem_field_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        HEADERf,
        (uint32*)&(eci_egr_tdm_config.header[0]));

    soc_mem_field32_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        LINK_MASK_PTRf,
        eci_egr_tdm_config.link_mask_ptr );

    soc_mem_field32_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        ADD_PACKET_CRCf,
        eci_egr_tdm_config.add_packet_crc);

    soc_mem_field32_set(
        unit,
        ECI_TDM_CONFIGm,
        data,
        MC_REPLICATIONf,
        eci_egr_tdm_config.mc_replication);

    rv = soc_mem_write(
            unit,
            ECI_TDM_CONFIGm,
            MEM_BLOCK_ANY,
            index,
            data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_port_map_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_port_map_t    *egr_port_map)
{
    SHR_BITDCLNAME (data, 13);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_PORT_MAP_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_PORT_MAP_ENTRY_NUMBER));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_EGR_PORT_MAPm(unit, MEM_BLOCK_ANY, index, data)); 

    egr_port_map->channel_id = soc_mem_field32_get(
            unit,
            ECI_EGR_PORT_MAPm,
            data,
            CHANNEL_IDf );

    egr_port_map->interface_id= soc_mem_field32_get(
            unit,
            ECI_EGR_PORT_MAPm,
            data,
            INTERFACE_IDf );

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_port_map_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_port_map_t    egr_port_map)
{
    int rv = 0;
    SHR_BITDCLNAME (data, 13);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_PORT_MAP_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_PORT_MAP_ENTRY_NUMBER));
    }

    soc_mem_field32_set(
        unit,
        ECI_EGR_PORT_MAPm,
        data,
        CHANNEL_IDf,
        egr_port_map.channel_id);

    soc_mem_field32_set(
        unit,
        ECI_EGR_PORT_MAPm,
        data,
        INTERFACE_IDf,
        egr_port_map.interface_id);

    rv = soc_mem_write(
            unit,
            ECI_EGR_PORT_MAPm,
            MEM_BLOCK_ANY,
            index,
            data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_mct_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_mct_t    *eci_egr_mct)
{
    SHR_BITDCLNAME (data, 39);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_MCT_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_MCT_ENTRY_NUMBER));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_EGR_MCTm(unit, MEM_BLOCK_ANY, index, data)); 

    eci_egr_mct->channel_id_0_valid = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_0_VALIDf );

    eci_egr_mct->channel_id_0 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_0f );

    eci_egr_mct->channel_id_1_valid = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_1_VALIDf );

    eci_egr_mct->channel_id_1 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_1f );

    eci_egr_mct->channel_id_2_valid = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_2_VALIDf );

    eci_egr_mct->channel_id_2 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_2f );

    eci_egr_mct->channel_id_3_valid = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_3_VALIDf );

    eci_egr_mct->channel_id_3 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCTm,
            data,
            CHANNEL_ID_3f );

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_mct_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_mct_t    eci_egr_mct)
{
    int rv = 0;
    SHR_BITDCLNAME (data, 39);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_MCT_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_MCT_ENTRY_NUMBER));
    }

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_0_VALIDf,
        eci_egr_mct.channel_id_0_valid);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_0f,
        eci_egr_mct.channel_id_0);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_1_VALIDf,
        eci_egr_mct.channel_id_1_valid);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_1f,
        eci_egr_mct.channel_id_1);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_2_VALIDf,
        eci_egr_mct.channel_id_2_valid);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_2f,
        eci_egr_mct.channel_id_2);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_3_VALIDf,
        eci_egr_mct.channel_id_3_valid);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCTm,
        data,
        CHANNEL_ID_3f,
        eci_egr_mct.channel_id_3);

    rv = soc_mem_write(
            unit,
            ECI_EGR_MCTm,
            MEM_BLOCK_ANY,
            index,
            data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_mct_mapping_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_mct_mapping_t    *eci_egr_mct_mapping)
{
    SHR_BITDCLNAME (data, 72);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_MCT_MAPPING_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_MCT_MAPPING_ENTRY_NUMBER));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_EGR_MCT_MAPPINGm(unit, MEM_BLOCK_ANY, index, data)); 

    eci_egr_mct_mapping->mc_map_0 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_0f );

    eci_egr_mct_mapping->mc_map_1 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_1f );

    eci_egr_mct_mapping->mc_map_2 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_2f );

    eci_egr_mct_mapping->mc_map_3 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_3f );

    eci_egr_mct_mapping->mc_map_4 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_4f );

    eci_egr_mct_mapping->mc_map_5 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_5f );

    eci_egr_mct_mapping->mc_map_6 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_6f );

    eci_egr_mct_mapping->mc_map_7 = soc_mem_field32_get(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            data,
            MC_MAP_7f );

    exit:
        SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_jer_fabric_eci_egr_mct_mapping_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_mct_mapping_t    eci_egr_mct_mapping)
{
    int rv = 0;
    SHR_BITDCLNAME (data, 39);
    SOCDNX_INIT_FUNC_DEFS

    if (index >= FABRIC_ECI_EGR_MCT_MAPPING_ENTRY_NUMBER){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid table index, must be smaller than %d.\n"), FABRIC_ECI_EGR_MCT_MAPPING_ENTRY_NUMBER));
    }

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_0f,
        eci_egr_mct_mapping.mc_map_0);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_1f,
        eci_egr_mct_mapping.mc_map_1);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_2f,
        eci_egr_mct_mapping.mc_map_2);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_3f,
        eci_egr_mct_mapping.mc_map_3);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_4f,
        eci_egr_mct_mapping.mc_map_4);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_5f,
        eci_egr_mct_mapping.mc_map_5);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_6f,
        eci_egr_mct_mapping.mc_map_6);

    soc_mem_field32_set(
        unit,
        ECI_EGR_MCT_MAPPINGm,
        data,
        MC_MAP_7f,
        eci_egr_mct_mapping.mc_map_7);

    rv = soc_mem_write(
            unit,
            ECI_EGR_MCT_MAPPINGm,
            MEM_BLOCK_ANY,
            index,
            data
          );
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
        SOCDNX_FUNC_RETURN;
}
#endif 

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
