#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC


#include <soc/mem.h>


#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/TMC/tmc_api_framework.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_api_fabric.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/dpp/mbcm.h>

#include <soc/dcmn/fabric.h>

#include <bcm/port.h>

#include <sal/compiler.h>





#define ARAD_FBC_CONNECT_FE_VAL_1        (0x0)
#define ARAD_FBC_CONNECT_FE_MULT_VAL_1   (0x0)

#define ARAD_FBC_CONNECT_FE_VAL_2        (0x0)
#define ARAD_FBC_CONNECT_FE_MULT_VAL_2   (0x0)
#define ARAD_FBC_CONNECT_BACK2BACK_VAL_2 (0x1)
#define ARAD_FBC_CONNECT_MESH_VAL_3      (0x1)
#define ARAD_FBC_CONNECT_MESH_BYPASS_TDM (0x0)

#define ARAD_FABRIC_LINK_FC_ENABLE_MODE_ON_VAL  (1)
#define ARAD_FABRIC_LINK_FC_ENABLE_MODE_OFF_VAL (0)



#define ARAD_FABRIC_IPT_GCI_BACKOFF_CONGESTION_LEVEL_MAX    (0x3ff)


#define ARAD_FABRIC_GCI_BUCKET_MAX                          (0xff)

#define ARAD_FABRIC_LLFC_RX_MAX                             (127)


#define ARAD_FABRIC_RCI_THRESHOLD_MAX                       (127)
#define ARAD_FABRIC_RCI_THRESHOLD_SINGLE_PIPE_DEFAULT       (64)
#define ARAD_FABRIC_RCI_THRESHOLD_DUAL_PIPE_DEFAULT         (32)
#define ARAD_FABRIC_RCI_INC_VAL_MAX                         (0x7f)



#define ARAD_FABRIC_EMPTY_CELL_SIZE_MIN                     (64)
#define ARAD_FABRIC_EMPTY_CELL_SIZE_MAX                     (127)
#define ARAD_FABRIC_EMPTY_CELL_SIZE_REMOTE_LINK_REPEATER    (84)



#define ARAD_MAX_BUCKET_FILL_RATE                           0xB 

#define ARAD_FABRIC_ALDWP_FAP_OFFSET                        (3)












#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)












uint32
  arad_fabric_regs_init(
    SOC_SAND_IN  int       unit
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(arad_fabric_common_regs_init(unit));

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, DEL_CRC_PKTf,  0x1));

exit:
  SOCDNX_FUNC_RETURN;

}



uint32
  arad_fabric_common_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    byte_mode,
    period,
    llfc_enabler_mode,
    fmac_index,
    link_ndx,
    blk_id,
    brst_size_init_val,
    fbr_nof_links,
    fbr_nof_macs,
    fbr_nof_links_in_mac;
  uint32 reg_val;
  int i;

    SOCDNX_INIT_FUNC_DEFS;
  
     
    if (SOC_IS_QMX(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, 16); 
        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, 4);
    }

    fbr_nof_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
      
    for (link_ndx=0; link_ndx < fbr_nof_links; link_ndx++) {
        fmac_index = link_ndx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx,SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac));

        brst_size_init_val = 1;
        byte_mode = 0x1;
        llfc_enabler_mode = 0x3;
        period = SOC_DPP_DEFS_GET(unit, fabric_comma_burst_period);

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr ,  blk_id, fmac_index, FMAL_N_CM_BRST_SIZEf,  brst_size_init_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr ,  blk_id, fmac_index, FMAL_N_CM_BRST_SIZE_LLFCf,  brst_size_init_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr ,  blk_id, fmac_index, FMAL_N_CM_BRST_LLFC_ENABLERf,  llfc_enabler_mode));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr ,  blk_id, fmac_index, FMAL_N_CM_TX_BYTE_MODEf,  byte_mode));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr ,  blk_id, fmac_index, FMAL_N_CM_TX_PERIODf,  period));

        
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, blk_id, 0, &reg_val));
        SOC_DPP_PORT_PARAMS(unit).comma_burst_conf[link_ndx] = reg_val;        
    }


    fbr_nof_macs = SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
    for (blk_id = 0; blk_id < fbr_nof_macs; ++blk_id) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr,  blk_id, 0, FAP_MODEf,  0x1));
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, blk_id, 0, &reg_val));
        SOC_DPP_PORT_PARAMS(unit).control_burst_conf[blk_id] = reg_val;

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr,  blk_id, 0, SIG_DET_BKT_RST_ENAf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr,  blk_id, 0, ALIGN_LCK_BKT_RST_ENAf,  0x1));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, REG_PORT_ANY, 0, ENABLE_MCL_UPDATESf,  0x1));
   
    
    fbr_nof_links_in_mac = SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
    for(i = 0; i < fbr_nof_links_in_mac; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        if (!SOC_IS_FLAIR(unit)) {
            SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val));
        } else {
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
  arad_fabric_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_FABRIC*      fabric
  )
{
  uint32
    link_i;
  ARAD_FABRIC_FC
    fc;  
  uint32
    res = SOC_SAND_OK;
  uint8 is_mesh;
  uint32 fld_val = 0;

    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_INIT);

  
  arad_ARAD_FABRIC_FC_clear(unit, &fc);

  res = arad_fabric_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if(!fabric->segmentation_enable && fabric->dual_pipe_tdm_packet) {
      fld_val = 1;
  } else if(fabric->segmentation_enable) {
      fld_val = 1;
  }  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_FDT_SEGMENTATION_AND_INTERLEAVINGr, REG_PORT_ANY, 0, INTERLEAVING_IPT_TO_IRE_ENf,  fld_val));

  
  is_mesh = SOC_SAND_NUM2BOOL(SOC_DPP_IS_MESH((unit)));
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  if (!is_mesh) {
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, ENABLE_SWITCHING_NETWORKf,  0x1));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  61,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, REG_PORT_ANY, 0, SWITCHING_NETWORK_LFSR_CNTf,  500));
  }

  for (link_i=0; link_i<SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_i)
  {
    fc.enable[link_i] = TRUE;
  }

  if (!SOC_IS_ARDON(unit)) {
      res = arad_fabric_flow_control_init(
              unit,
              ARAD_CONNECTION_DIRECTION_BOTH,
              &fc
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = arad_fabric_scheduler_adaptation_init(
          unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 77, exit);

  
  
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS((unit)))
  {
      uint32 quad_repeater_bitmap;
      uint32 fmac_index;
      uint32 link_internal;
      int is_remote_link_repeater;   

      for (fmac_index = 0; fmac_index < SOC_DPP_DEFS_GET(unit, nof_instances_fmac); fmac_index++)
      {
          quad_repeater_bitmap = 0;
          for (link_internal = 0; link_internal < SOC_ARAD_NOF_LINKS_IN_MAC; link_internal++)
          {
              link_i = link_internal + fmac_index*SOC_ARAD_NOF_LINKS_IN_MAC;
              is_remote_link_repeater = soc_property_port_get(unit, link_i, spn_REPEATER_LINK_ENABLE, 0) ? 1 : 0;
              quad_repeater_bitmap |= is_remote_link_repeater << link_internal; 
          }

          if (quad_repeater_bitmap == 0xf)
          {
              
              res = arad_fabric_empty_cell_size_set(
                        unit,
                        fmac_index,
                        ARAD_FABRIC_EMPTY_CELL_SIZE_REMOTE_LINK_REPEATER
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          } else if (quad_repeater_bitmap == 0x0) {
              
              
          } else {
              
              SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_MIXED_CONFIGURATION_ERR, 40, exit);
          }
      }
  }
#endif 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_init()",0,0);
}


uint32
  arad_fabric_fc_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  )
{
  uint32
    fld_val,
    reg_val,
    tx_field,
    rx_field;
  uint32
    fmac_index = 0,
    link_ndx = 0,
    inner_link = 0;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;
  uint32 res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  


  rx_enable = ARAD_IS_DIRECTION_RX(direction_ndx);
  tx_enable = ARAD_IS_DIRECTION_TX(direction_ndx);

  
  for (link_ndx = 0; link_ndx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_ndx)
  {
    fld_val = SOC_SAND_BOOL2NUM(info->enable[link_ndx]);
    fmac_index = SOC_SAND_DIV_ROUND_DOWN(link_ndx,SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac));
    inner_link = link_ndx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    if (tx_enable)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit,fmac_index,&reg_val));
      tx_field = soc_reg_field_get(unit,FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr,reg_val,LNK_LVL_FC_TX_ENf);
      SOC_SAND_SET_BIT(tx_field,fld_val,inner_link);
      soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val,LNK_LVL_FC_TX_ENf, tx_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit,fmac_index,reg_val));
    }

    if (rx_enable)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit,fmac_index,&reg_val));
      rx_field = soc_reg_field_get(unit,FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr,reg_val,LNK_LVL_FC_RX_ENf);
      SOC_SAND_SET_BIT(rx_field,fld_val,inner_link);
      soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val,LNK_LVL_FC_RX_ENf, rx_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit,fmac_index,reg_val));
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_enable_set_unsafe()",0,0);
}


uint32
  arad_fabric_flow_control_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  )
{
  uint32
    reg_val,
    rx_field;
  uint8
    rx_enable = FALSE;
  uint32 res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  


  rx_enable = ARAD_IS_DIRECTION_RX(direction_ndx);

  
  res = arad_fabric_fc_enable_set_unsafe(
      unit,
      direction_ndx,
      info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (rx_enable) {
      
      rx_field = 0x1;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1040, exit, READ_SCH_RCI_PARAMSr(unit, REG_PORT_ANY,&reg_val));
      soc_reg_field_set(unit, SCH_RCI_PARAMSr, &reg_val,RCI_ENAf, rx_field);
      soc_reg_field_set(unit, SCH_RCI_PARAMSr, &reg_val,DEVICE_RCI_ENAf, rx_field);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1041, exit, WRITE_SCH_RCI_PARAMSr(unit, REG_PORT_ANY,reg_val));

      
      if (SOC_DPP_CONFIG(unit)->arad->init.fabric.dual_pipe_tdm_packet)
      {
          res = arad_fabric_rci_config_set(unit, ARAD_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH, ARAD_FABRIC_RCI_THRESHOLD_DUAL_PIPE_DEFAULT);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1042, exit);
      } else {
          res = arad_fabric_rci_config_set(unit, ARAD_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH, ARAD_FABRIC_RCI_THRESHOLD_SINGLE_PIPE_DEFAULT);
          SOC_SAND_CHECK_FUNC_RESULT(res, 1043, exit);
      }

  }

  
  


  
  
  if (rx_enable)
  {
      
      res = arad_fabric_gci_enable_set(unit, ARAD_FABRIC_GCI_TYPE_RANDOM_BACKOFF, 0);    
 
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1059,  exit, ARAD_REG_ACCESS_ERR, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_backoff_masks_init, (unit)));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1060,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_GCI_BACKOFF_CS_WEIGHTSr, REG_PORT_ANY, 0, CNGST_LVL_WORSE_WEIGHTf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1061,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_GCI_BACKOFF_CS_WEIGHTSr, REG_PORT_ANY, 0, CNGST_LVL_BETTER_WEIGHTf,  0x1));


  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_flow_control_init()",0,0);
}



soc_error_t
  arad_fabric_gci_backoff_masks_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32 gci_backoff_mask_table[SOC_MAX_MEM_WORDS];
  SOCDNX_INIT_FUNC_DEFS;

    
  
  
  SOCDNX_IF_ERR_EXIT(READ_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 0,  gci_backoff_mask_table));
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_0f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_0);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_1f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_1);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_2f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_2);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_3f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_3);
  SOCDNX_IF_ERR_EXIT(WRITE_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 0,  gci_backoff_mask_table));

  
  SOCDNX_IF_ERR_EXIT(READ_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 1,  gci_backoff_mask_table));
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_0f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_0);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_1f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_1);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_2f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_2);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_3f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_3);
  SOCDNX_IF_ERR_EXIT(WRITE_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 1,  gci_backoff_mask_table));

  
  SOCDNX_IF_ERR_EXIT(READ_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 2,  gci_backoff_mask_table));
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_0f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_0);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_1f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_1);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_2f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_2);
  soc_mem_field32_set(unit, IPT_GCI_BACKOFF_MASKm, gci_backoff_mask_table, MASK_3f, ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_3);
  SOCDNX_IF_ERR_EXIT(WRITE_IPT_GCI_BACKOFF_MASKm(unit, MEM_BLOCK_ANY, 2,  gci_backoff_mask_table));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fabric_fc_enable_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_NOF_CONNECTION_DIRECTIONS,
    ARAD_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_enable_verify()",0,0);
}


uint32
  arad_fabric_fc_enable_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT ARAD_FABRIC_FC            *info_rx,
    SOC_SAND_OUT ARAD_FABRIC_FC            *info_tx
  )
{
  uint32
    rx_field[1],
    tx_field[1],
    reg_val;
  uint32
    fmac_index,
    link_idx,
    inner_link;
  ARAD_FABRIC_FC
    info_local_rx,
    info_local_tx;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  arad_ARAD_FABRIC_FC_clear(unit, &info_local_rx);
  arad_ARAD_FABRIC_FC_clear(unit, &info_local_tx);

  

  for (link_idx = 0; link_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); ++link_idx)
  {
    fmac_index = SOC_SAND_DIV_ROUND_DOWN(link_idx,SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac));
    inner_link = link_idx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1130, exit, READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit,fmac_index,&reg_val));
    *rx_field = soc_reg_field_get(unit,FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr,reg_val,LNK_LVL_FC_RX_ENf);
    *tx_field = soc_reg_field_get(unit,FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr,reg_val,LNK_LVL_FC_TX_ENf);
    if(SHR_BITGET(rx_field, inner_link))
    {
      info_rx->enable[link_idx] = SOC_SAND_NUM2BOOL(ARAD_FABRIC_LINK_FC_ENABLE_MODE_ON_VAL); 
    }
    else
    {
      info_rx->enable[link_idx] = SOC_SAND_NUM2BOOL(ARAD_FABRIC_LINK_FC_ENABLE_MODE_OFF_VAL);
    }

    if(SHR_BITGET(tx_field, inner_link))
    {
      info_tx->enable[link_idx] = SOC_SAND_NUM2BOOL(ARAD_FABRIC_LINK_FC_ENABLE_MODE_ON_VAL);
    }
    else
    {
      info_tx->enable[link_idx] = ARAD_FABRIC_LINK_FC_ENABLE_MODE_OFF_VAL;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_enable_get_unsafe()",0,0);
}


uint32
  arad_fabric_fc_shaper_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  )
{
  uint32
    res,
    bytes_cells_mode,
    fmac_index,
    blk_id,
    fc_cells,
    fc_bytes,
    data_bytes,
    data_cells,
    fc_period,
    data_period;
    
        
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_SHAPER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  arad_ARAD_FABRIC_FC_SHAPER_clear(exact_info);


  fmac_index = SOC_SAND_DIV_ROUND_DOWN(link_ndx,SOC_DPP_DEFS_GET(unit, nof_instances_fmac));
  blk_id = link_ndx % SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
  

  switch (shaper_mode->shaper_mode) 
  {
    case (ARAD_FABRIC_SHAPER_BYTES_MODE):
    {
      bytes_cells_mode = 0x1;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_BYTE_MODEf,  bytes_cells_mode)); 
      
      data_bytes = info->data_shaper.bytes;
      data_period = soc_sand_log2_round_up(data_bytes);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIODf,  data_period)); 
      exact_info->data_shaper.bytes = soc_sand_power_of_2(data_period);

      
      fc_bytes = info->fc_shaper.bytes;
      fc_period = soc_sand_log2_round_up(fc_bytes);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIOD_LLFCf,  fc_period));
      exact_info->fc_shaper.bytes = soc_sand_power_of_2(fc_period);
      break;
   }
   case (ARAD_FABRIC_SHAPER_CELLS_MODE): 
   {
     bytes_cells_mode = 0x0;
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_BYTE_MODEf,  bytes_cells_mode)); 

     
     data_cells = info->data_shaper.cells;
     data_period = soc_sand_log2_round_up(data_cells);
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIODf,  data_period)); 
     exact_info->data_shaper.cells = soc_sand_power_of_2(data_period);

     
     fc_cells = info->fc_shaper.cells;
     fc_period = soc_sand_log2_round_up(fc_cells);
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIOD_LLFCf,  fc_period));
     exact_info->fc_shaper.cells = soc_sand_power_of_2(fc_period);
     break;
   }
   default:  
     SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_SHAPER_MODE_OUT_OF_RANGE_ERR, 70, exit);   
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_shaper_set_unsafe()",link_ndx,0);
}


uint32
  arad_fabric_fc_shaper_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_SHAPER_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links),
    ARAD_FBR_LINK_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->data_shaper.bytes, ARAD_FABRIC_SHAPER_BYTES_MAX,
    ARAD_FABRIC_SHAPER_BYTES_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->fc_shaper.bytes, ARAD_FABRIC_SHAPER_BYTES_MAX,
    ARAD_FABRIC_SHAPER_BYTES_OUT_OF_RANGE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->data_shaper.cells, ARAD_FABRIC_SHAPER_CELLS_MAX,
    ARAD_FABRIC_SHAPER_CELLS_OUT_OF_RANGE_ERR, 40, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->fc_shaper.cells, ARAD_FABRIC_SHAPER_CELLS_MAX,
    ARAD_FABRIC_SHAPER_CELLS_OUT_OF_RANGE_ERR, 50, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_shaper_verify()",link_ndx,0);
}   
  

uint32
  arad_fabric_fc_shaper_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         link_ndx,
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER            *info
  )
{
  uint32
    res,
    bytes_cells_mode,
    fmac_index,
    blk_id,
    data_period,
    fc_period;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_FC_SHAPER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_FABRIC_FC_SHAPER_MODE_INFO_clear(shaper_mode);
  arad_ARAD_FABRIC_FC_SHAPER_clear(info);


  fmac_index = SOC_SAND_DIV_ROUND_DOWN(link_ndx,SOC_DPP_DEFS_GET(unit, nof_instances_fmac));
  blk_id = link_ndx % SOC_DPP_DEFS_GET(unit, nof_instances_fmac);
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_BYTE_MODEf, &bytes_cells_mode)); 
  shaper_mode->shaper_mode = bytes_cells_mode;

  switch (bytes_cells_mode) 
  {
    case (ARAD_FABRIC_SHAPER_BYTES_MODE):
    {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIODf, &data_period)); 
      info->data_shaper.bytes = soc_sand_power_of_2(data_period);

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIOD_LLFCf, &fc_period)); 
      info->fc_shaper.bytes = soc_sand_power_of_2(fc_period);
      break;
   }
   case (ARAD_FABRIC_SHAPER_CELLS_MODE): 
   {
     
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIODf, &data_period)); 
     info->data_shaper.cells = soc_sand_power_of_2(data_period);

     
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr,  blk_id,  fmac_index, FMAL_N_CM_TX_PERIOD_LLFCf, &fc_period)); 
     info->fc_shaper.cells = soc_sand_power_of_2(fc_period);
     break;
   }
   default:  
     SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_SHAPER_MODE_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_fc_shaper_get_unsafe()",link_ndx,0);
}



STATIC
int
    _arad_fabric_mesh_value1_get(
       SOC_SAND_IN int unit,
       SOC_SAND_OUT uint32 *value_1,
       SOC_SAND_IN int speed,
       SOC_SAND_IN soc_dcmn_port_pcs_t pcs
       )
{
    struct values_table_entry
    {
        int speed;
        uint32 value[_SHR_PORT_PCS_COUNT];
    };

    struct values_table_entry values_arr[] = {
        {5750,  {53, 34, 104, 104, 40}},
        {6250,  {49, 31, 96, 96, 37}},
        {8125,  {38, 24, 75, 75, 29}},
        {8500,  {37, 23, 72, 72, 28}},
        {10312, {30, 20, 60, 60, 23}},
        {11250, {24, 24, 55, 55, 22}},
        {11500, {28, 28, 54, 54, 21}},
        {12500, {22, 22, 50, 50, 20}}
    };

    int i = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (pcs  > 4 )
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsporrted pcs % \n"), pcs));
    }

    *value_1=0;
    for( i = 0 ; i < sizeof(values_arr)/sizeof(values_arr[0]); i++) {
        if(values_arr[i].speed == speed){
            *value_1 = values_arr[i].value[pcs] * SOC_DPP_CONFIG(unit)->arad->init.core_freq.system_ref_clock /300000;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
    arad_fabric_mesh_topology_values_config(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int speed,
       SOC_SAND_IN soc_dcmn_port_pcs_t pcs
       )
{
    uint32 value_1;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(_arad_fabric_mesh_value1_get(unit, &value_1, speed , pcs));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_REG_010Cr, REG_PORT_ANY, 0, FIELD_0_10f,  value_1));
    if(value_1 != 0){
        value_1++;
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_SYNC_MSG_RX_ADJ_FACTORr, REG_PORT_ANY, 0, SYNC_MSG_RX_ADJ_FACTORf,  value_1));    

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_fabric_scheduler_adaptation_init(
       SOC_SAND_IN int    unit
       )
{
  uint32 res,reg_val32;
  int field_val=0;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (SOC_DPP_CONFIG(unit)->arad->init.fabric.scheduler_adapt_to_links) field_val=1;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_RTP_RTP_ENABLEr_REG32(unit,&reg_val32));
  soc_reg_field_set(unit, RTP_RTP_ENABLEr, &reg_val32,EN_LOCAL_LINK_REDUCTION_MCf, field_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_RTP_RTP_ENABLEr_REG32(unit,reg_val32));



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_scheduler_adaptation_init()",0,0);
}


uint32
  arad_fabric_stand_alone_fap_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  )
{
  uint32
    res,
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_STAND_ALONE_FAP_MODE_SET_UNSAFE);

  fld_val = SOC_SAND_BOOL2NUM(is_single_fap_mode);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, STAN_ALNf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, MULTI_FAP_2f,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_stand_alone_fap_mode_set_unsafe()",0,0);
}


uint32
  arad_fabric_stand_alone_fap_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_STAND_ALONE_FAP_MODE_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_stand_alone_fap_mode_verify()",0,0);
}


uint32
  arad_fabric_stand_alone_fap_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  )
{
  uint32
    res,
    fld_val = 0;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_STAND_ALONE_FAP_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_single_fap_mode);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, STAN_ALNf, &fld_val));
  *is_single_fap_mode = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_stand_alone_fap_mode_get_unsafe()",0,0);
}

uint32
  arad_fabric_connect_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  )
{
  soc_port_t port;
  soc_dcmn_port_pcs_t pcs;
  int speed;
  uint32
    res,
    value_1,
    value_2,
    value_3;
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_CONNECT_MODE_SET_UNSAFE);

  switch(fabric_mode)
  {
  case ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP:
  case ARAD_FABRIC_CONNECT_MODE_FE:

    value_1 = ARAD_FBC_CONNECT_FE_VAL_1;
    value_2 = ARAD_FBC_CONNECT_FE_VAL_2;
    break;

  case ARAD_FABRIC_CONNECT_MODE_MULT_STAGE_FE:

    value_1 = ARAD_FBC_CONNECT_FE_MULT_VAL_1;
    value_2 = ARAD_FBC_CONNECT_FE_MULT_VAL_2;
    break;
  case ARAD_FABRIC_CONNECT_MODE_MESH:
    value_3 = (SOC_DPP_CONFIG(unit)->tdm.is_bypass) ? ARAD_FBC_CONNECT_MESH_BYPASS_TDM : ARAD_FBC_CONNECT_MESH_VAL_3;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, MESH_MODEf,  value_3));
  
  case ARAD_FABRIC_CONNECT_MODE_BACK2BACK:
    
    value_1 = 0;
    PBMP_SFI_ITER(unit, port)
    {
      speed = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, 0);
      if(speed != -1)
      {
        
        if(speed == 0)
        {
            res = arad_port_speed_max(unit, port, &speed);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res , 50 , exit);
        }
        res = arad_port_control_pcs_get(unit, port, &pcs);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit);
        res = _arad_fabric_mesh_value1_get(unit, &value_1 , speed, pcs);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit);
        break;
      }
    }
    value_2 = ARAD_FBC_CONNECT_BACK2BACK_VAL_2;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_REG_010Cr, REG_PORT_ANY, 0, FIELD_0_10f,  value_1));
    if(value_1 != 0)
    {
      value_1 += 1;
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 10, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_SYNC_MSG_RX_ADJ_FACTORr, REG_PORT_ANY, 0, SYNC_MSG_RX_ADJ_FACTORf,  value_1));
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, MULTI_FAPf,  value_2));

  if (
      (fabric_mode == ARAD_FABRIC_CONNECT_MODE_FE) ||
      (fabric_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP) ||
      (fabric_mode == ARAD_FABRIC_CONNECT_MODE_MULT_STAGE_FE)
    )
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,WRITE_MESH_TOPOLOGY_INITr(unit,  0xd2d));
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,WRITE_MESH_TOPOLOGY_INITr(unit,  0xd00));
  }
  value_1 = (fabric_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP)?1:0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,WRITE_IPT_FORCE_LOCAL_OR_FABRICr(unit,  value_1));

  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_connect_mode_set_unsafe()",0,0);
}


uint32
  arad_fabric_connect_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_CONNECT_MODE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
      fabric_mode,  ARAD_FABRIC_NOF_CONNECT_MODES - 1,
      ARAD_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_connect_mode_verify()",0,0);
}


soc_error_t
    arad_link_power_set(
        int unit, 
        soc_port_t port, 
        uint32 flags, 
        soc_dcmn_port_power_t power
        )
{
    ARAD_LINK_STATE_INFO info;
    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_LINK_STATE_INFO_clear(&info);
    if (power == soc_dcmn_port_power_on) {
        info.on_off = ARAD_LINK_STATE_ON;
    } else {
        info.on_off = ARAD_LINK_STATE_OFF;
    }

    info.serdes_also = TRUE;

    SOCDNX_IF_ERR_EXIT(arad_link_on_off_set(unit, port, &info));

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_link_on_off_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_LINK_STATE_INFO   *info
  )
{
    uint32
        reg_val,
        pwr_up_val,
        res,
        rx_field[1], 
        tx_field[1],
        high_64bits,
        low_64bits;
    uint32
        blk_id,
        inner_link,
        reg_idx,
        fld_idx;
    uint64
        reg_64val;
    int 
        link_ndx;
    SOCDNX_INIT_FUNC_DEFS;
  
    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    reg_idx = ARAD_REG_IDX_GET(link_ndx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = ARAD_FLD_IDX_GET(link_ndx, SOC_SAND_REG_SIZE_BITS);

    pwr_up_val = (info->on_off == ARAD_LINK_STATE_ON)?0x1:0x0;
  
    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac));
    inner_link = SOC_IS_ARDON(unit) ? link_ndx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac) : link_ndx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
  
    if (info->on_off == ARAD_LINK_STATE_ON)
    {  
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, &reg_val));
        *rx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
        *tx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
  
        SHR_BITCLR(rx_field, inner_link);
        SHR_BITCLR(tx_field, inner_link);
  
        
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *tx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val)); 
      
        
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKSr_REG64(unit, &reg_64val));
        high_64bits = COMPILER_64_HI(reg_64val);
        low_64bits = COMPILER_64_LO(reg_64val);
        if(reg_idx == 0) {
            SOC_SAND_SET_BIT(low_64bits, pwr_up_val, fld_idx);
        } else {
            SOC_SAND_SET_BIT(high_64bits, pwr_up_val, fld_idx);
        }
        COMPILER_64_SET(reg_64val, high_64bits, low_64bits);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALLOWED_LINKSr_REG64(unit,  reg_64val));
    
        
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *rx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val));

        
        if (info->serdes_also == TRUE) {
            MIIM_LOCK(unit);
            res = soc_phyctrl_enable_set(unit, port, TRUE);
            MIIM_UNLOCK(unit);
            SOCDNX_IF_ERR_EXIT(res);    
        }
    } else { 

        
        if (info->serdes_also == TRUE)
        {
            MIIM_LOCK(unit);
            res = soc_phyctrl_enable_set(unit, port, FALSE);
            MIIM_UNLOCK(unit);
            SOCDNX_IF_ERR_EXIT(res);
        }

        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, &reg_val));
        *rx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
        *tx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);

        SHR_BITSET(rx_field, inner_link);
        SHR_BITSET(tx_field, inner_link);
  
        
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *rx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val));
    
        
        SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKSr_REG64(unit, &reg_64val));
        high_64bits = COMPILER_64_HI(reg_64val);
        low_64bits = COMPILER_64_LO(reg_64val);
        if(reg_idx == 0) {
            SOC_SAND_SET_BIT(low_64bits, pwr_up_val, fld_idx);
        } else {
            SOC_SAND_SET_BIT(high_64bits, pwr_up_val, fld_idx);
        }
        COMPILER_64_SET(reg_64val, high_64bits, low_64bits);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALLOWED_LINKSr_REG64(unit,  reg_64val));

        
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *tx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_link_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  ARAD_LINK_STATE_INFO     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_LINK_ON_OFF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links),
    ARAD_FBR_LINK_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->on_off, ARAD_LINK_NOF_STATES,
    ARAD_FBR_LINK_ON_OFF_STATE_OUT_OF_RANGE_ERR, 20, exit
  );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_link_on_off_verify()",link_ndx,0);
}

soc_error_t
    arad_link_power_get(
        int unit, 
        soc_port_t port, 
        soc_dcmn_port_power_t* power
        )
{
    ARAD_LINK_STATE_INFO info;
    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_LINK_STATE_INFO_clear(&info);

    SOCDNX_IF_ERR_EXIT(arad_link_on_off_get(unit, port, &info)); 
    *power = (info.on_off == SOC_TMC_LINK_STATE_ON ? soc_dcmn_port_power_on : soc_dcmn_port_power_off);

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_link_on_off_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_OUT ARAD_LINK_STATE_INFO   *info
  )
{
    uint32
        reg_val,
        res,
        rx_field[1], 
        tx_field[1];
    int32
        srd_pwr_state;
    uint32
        blk_id,
        inner_link;
    int 
        link_ndx;
    SOCDNX_INIT_FUNC_DEFS;
  
    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);

    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac));
    inner_link = link_ndx % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, &reg_val));

    
    *rx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    *tx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);

    arad_ARAD_LINK_STATE_INFO_clear(info);
    if(SHR_BITGET(rx_field, inner_link) || SHR_BITGET(tx_field, inner_link)) {
        info->on_off = ARAD_LINK_STATE_OFF;
    } else {
        info->on_off = ARAD_LINK_STATE_ON;
    }

    
    MIIM_LOCK(unit);
    res = soc_phyctrl_enable_get(unit,port, &srd_pwr_state);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(res);    

    if (
      ((info->on_off == ARAD_LINK_STATE_ON) && (srd_pwr_state == ARAD_SRD_POWER_STATE_UP)) ||
      ((info->on_off == ARAD_LINK_STATE_OFF) && (srd_pwr_state == ARAD_SRD_POWER_STATE_DOWN))
     ) {
        info->serdes_also = TRUE;
    } else {
        info->serdes_also = FALSE;
    }

exit:
    SOCDNX_FUNC_RETURN
}


uint32
arad_fabric_port_speed_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   port,
    SOC_SAND_IN int                     speed
    )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_PORT_SPEED_SET);

    MIIM_LOCK(unit);
    res = soc_phyctrl_speed_set(unit, port, speed);
    MIIM_UNLOCK(unit);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
    if(SOC_DPP_IS_MESH(unit))
    {
      soc_dcmn_port_pcs_t pcs;

      res = arad_port_control_pcs_get(unit, port, &pcs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);
      res = arad_fabric_mesh_topology_values_config(unit, speed, pcs);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_port_speed_set()",port, speed);

}


int
arad_fabric_port_speed_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port,
    SOC_SAND_OUT int                    *speed
    )
{
    uint32 res;

    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    res = soc_phyctrl_speed_get(unit, port, speed);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;

}

uint32
arad_fabric_priority_bits_mapping_to_fdt_index_get(
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_OUT uint32                              *index
  )
{
    uint32
        is_hp = 0;

    SOCDNX_INIT_FUNC_DEFS;
    *index = 0;

    if (flags & SOC_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) {
        is_hp = 1;
    } 
    *index  |=  ((is_hp     << ARAD_FBC_PRIORITY_NDX_IS_HP_OFFSET)  & ARAD_FBC_PRIORITY_NDX_IS_HP_MASK  )|
                ((tc        << ARAD_FBC_PRIORITY_NDX_TC_OFFSET)     & ARAD_FBC_PRIORITY_NDX_TC_MASK     )|
                ((dp        << ARAD_FBC_PRIORITY_NDX_DP_OFFSET)     & ARAD_FBC_PRIORITY_NDX_DP_MASK     )
                ;

  SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_fabric_priority_set(
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_IN  int                                 fabric_priority
  )
{
    uint32 index,
           offset,
           val;
    uint32 fabric_priority_table[SOC_MAX_MEM_WORDS];
    int res;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe) {
        if (fabric_priority >= ARAD_FBC_PRIORITY_NOF-1 ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric priority out of range")));
        }
    } else {
        if (fabric_priority >= ARAD_FBC_PRIORITY_NOF-2 ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric priority out of range")));
        }
    }
    
    res = arad_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index);
    SOCDNX_IF_ERR_EXIT(res);
    offset = index*ARAD_FBC_PRIORITY_LENGTH ;


    res = READ_IPT_PRIORITY_BITS_MAP_2_FDTm(unit, MEM_BLOCK_ANY, 0, fabric_priority_table);
    SOCDNX_IF_ERR_EXIT(res);
    val = fabric_priority;
    SHR_BITCOPY_RANGE(fabric_priority_table, offset, &val, 0, ARAD_FBC_PRIORITY_LENGTH);
    res = WRITE_IPT_PRIORITY_BITS_MAP_2_FDTm(unit, MEM_BLOCK_ALL, 0, fabric_priority_table);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_fabric_priority_get(
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_OUT int                                 *fabric_priority
  )
{
    uint32 res,
           index,
           offset,
           val = 0;
    uint32 fabric_priority_table[SOC_MAX_MEM_WORDS];
    SOCDNX_INIT_FUNC_DEFS;

    
    res = arad_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index);
    SOCDNX_IF_ERR_EXIT(res);
    offset = index*ARAD_FBC_PRIORITY_LENGTH ;

    
    res = READ_IPT_PRIORITY_BITS_MAP_2_FDTm(unit, MEM_BLOCK_ANY, 0, fabric_priority_table);
    SOCDNX_IF_ERR_EXIT(res);
    SHR_BITCOPY_RANGE(&val, 0, fabric_priority_table, offset, ARAD_FBC_PRIORITY_LENGTH); 
    *fabric_priority = val;

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_fabric_topology_status_connectivity_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  int
    source_lvl;
  bcm_port_t
    link_id, link_index;
  uint32
    reg_val,
    is_active;
  uint64
    mask;
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TOPOLOGY_STATUS_CONNECTIVITY_GET_UNSAFE);

  if (soc_feature(unit, soc_feature_no_fabric)) {
    SOC_EXIT;
  }
  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);
  SOC_SAND_ERR_IF_ABOVE_MAX(
    link_index_max, SOC_DPP_DEFS_GET(unit, nof_fabric_links) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) - 1,
    ARAD_FBR_LINK_ID_OUT_OF_RANGE_ERR, 10, exit
  );
  arad_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(connectivity_map);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1480, exit, READ_RTP_LINK_ACTIVE_MASKr_REG64(unit, &mask));

  COMPILER_64_NOT(mask);

  
  for(link_index = link_index_min; link_index <= link_index_max; ++link_index)
  {

    link_id = link_index;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1490, exit, READ_FCR_CONNECTIVITY_MAP_REGISTERSr(unit, link_id, &reg_val));
    is_active = COMPILER_64_BITTEST(mask, link_id);
    if(is_active)
    {
      connectivity_map->link_info[link_index].far_unit = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_ID_Nf);

      source_lvl = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_LEVEL_Nf);
#ifdef PLISIM
      if (SAL_BOOT_PLISIM) {
          source_lvl = 0x3; 
      }
#endif
      

      if((source_lvl & 0x2) == 0)
      {
        connectivity_map->link_info[link_index].far_dev_type = ARAD_FAR_DEVICE_TYPE_FAP;
      }
      else if (source_lvl == 0x2)
      {
        connectivity_map->link_info[link_index].far_dev_type = ARAD_FAR_DEVICE_TYPE_FE3;
      }
      else if((source_lvl & 0x3) == 0x3)
      {
        connectivity_map->link_info[link_index].far_dev_type = ARAD_FAR_DEVICE_TYPE_FE2;
      }
      else
      {
        connectivity_map->link_info[link_index].far_dev_type = ARAD_FAR_DEVICE_TYPE_FE1;
      }

      connectivity_map->link_info[link_index].far_link_id = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_LINK_Nf);
      connectivity_map->link_info[link_index].is_logically_connected=1; 
    }
    else
    {
      connectivity_map->link_info[link_index].far_link_id = DCMN_FABRIC_LINK_NO_CONNECTIVITY;
    }      
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_topology_status_connectivity_get_unsafe()",0,0);
}


soc_error_t 
arad_fabric_reachability_status_get(int unit, int moduleid, int links_max, uint32 *links_array, int *links_count)
{
    int i, offset, port;
    soc_reg_above_64_val_t rtp_reg_val;
    uint64 link_active_mask;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_REG_ABOVE_64_CLEAR(rtp_reg_val);
    if(moduleid < 0 || moduleid > 2048) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("module id invalid")));
    }
    SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm(unit, MEM_BLOCK_ANY, moduleid/2, rtp_reg_val));
    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_ACTIVE_MASKr_REG64(unit, &link_active_mask));
    
    *links_count = 0;

    
    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        offset = SOC_DPP_DEFS_GET(unit, nof_fabric_links)*(moduleid%2);
    } else {
        offset = 36*(moduleid%2);
    }

    PBMP_SFI_ITER(unit, port)
    {
        i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        
        if(SHR_BITGET(rtp_reg_val,i + offset) && !COMPILER_64_BITTEST(link_active_mask,i)) 
        {
            if(*links_count >= links_max) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("links_array is too small")));
            }
            
            links_array[*links_count] = i;
            (*links_count)++;
        }
    } 
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    int i, rc, port;
    SOCDNX_INIT_FUNC_DEFS;

    (*links_array_count) = 0;

    PBMP_SFI_ITER(unit, port) {
        i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        if ((*links_array_count) > links_array_max_size) {
            break;
        }
        rc = arad_fabric_link_status_get(unit, i, &(link_status[(*links_array_count)]), &(errored_token_count[(*links_array_count)]));
        SOCDNX_IF_ERR_EXIT(rc);
        (*links_array_count)++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_fabric_link_status_clear(int unit, soc_port_t link)
{
    int blk_id, inner_link;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    blk_id = (link - SOC_DPP_DEFS_GET(unit, first_fabric_link_id))/4;
    inner_link = link % 4;

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, RX_CRC_ERR_N_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, WRONG_SIZE_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val));
   
    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, &reg_val, DEC_ERR_INTf, 1 << inner_link);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,reg_val));


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_link_status_get(int unit, soc_port_t link_id, uint32 *link_status, uint32 *errored_token_count)
{
    uint32 reg_val, field_val[1], sig_acc = 0;
    int blk_id, reg_select;
    soc_port_t inner_lnk;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;
    
    *link_status = 0;
    
    blk_id = (link_id - SOC_DPP_DEFS_GET(unit, first_fabric_link_id))/4;
    reg_select = link_id % 4;
   
    
    if (reg_select >= 0 && reg_select <= 3) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKETr(unit,blk_id, reg_select, &reg_val));
        *errored_token_count = soc_reg_field_get(unit, FMAC_LEAKY_BUCKETr, reg_val, MACR_N_LKY_BKT_VALUEf);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't find register for link %d"),link_id));
    }
    
  
  
  
    inner_lnk = link_id % 4;
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, RX_CRC_ERR_N_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
        *link_status |= DCMN_FABRIC_LINK_STATUS_CRC_ERROR;
    
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, WRONG_SIZE_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
        *link_status |= DCMN_FABRIC_LINK_STATUS_SIZE_ERROR;  
       
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
    if(SHR_BITGET(field_val, inner_lnk))
       *link_status |= DCMN_FABRIC_LINK_STATUS_MISALIGN;  
      
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, reg_val, DEC_ERR_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
       *link_status |= DCMN_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;  
      
    
    if (SOC_USE_PORTCTRL(unit))
    {
#if defined(PORTMOD_SUPPORT)
        rv = soc_dcmn_port_rx_locked_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_id), &sig_acc);
        SOCDNX_IF_ERR_EXIT(rv);
#endif

    } 
    else
    {
        MIIM_LOCK(unit); 
        if (!SOC_IS_ARDON(unit)) {
            rv = soc_phyctrl_control_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_id), SOC_PHY_CONTROL_RX_SIGNAL_DETECT, &sig_acc); 
        } else {
            rv = soc_phyctrl_control_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_id), SOC_PHY_CONTROL_RX_SEQ_DONE, &sig_acc); 
        }
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    if(!sig_acc) {
        *link_status |= DCMN_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }
    
    if(*errored_token_count < 63) {
       *link_status |= DCMN_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }

    
    rv = arad_fabric_link_status_clear(unit, link_id);
    SOCDNX_IF_ERR_EXIT(rv);
    
exit:
    SOCDNX_FUNC_RETURN;
  
}

soc_error_t
arad_fabric_force_signal_detect_set(int unit, int mac_instance)
{
   uint32 force_signal_detect, base_link, i;
   soc_dcmn_loopback_mode_t loopback;
   uint32 reg_val;
   soc_dcmn_port_pcs_t pcs;

   SOCDNX_INIT_FUNC_DEFS;

   force_signal_detect = 0;
   base_link = SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac)*mac_instance;

   for(i=0 ; i<SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac) ; i++) {
       SOCDNX_IF_ERR_EXIT(arad_fabric_loopback_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, base_link+i), &loopback));
       if(soc_dcmn_loopback_mode_mac_pcs == loopback || soc_dcmn_loopback_mode_mac_outer == loopback || soc_dcmn_loopback_mode_mac_async_fifo == loopback) {
           force_signal_detect = 1;
           break;
       }

       SOCDNX_IF_ERR_EXIT(arad_port_control_pcs_get(unit,SOC_DPP_FABRIC_LINK_TO_PORT(unit, base_link+i) , &pcs));
       if(soc_dcmn_port_pcs_64_66_fec == pcs || soc_dcmn_port_pcs_64_66_bec == pcs) {
           force_signal_detect = 1;
           break;
       }

   }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, mac_instance, &reg_val));
    soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg_val, FORCE_SIGNAL_DETECTf, force_signal_detect);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, mac_instance, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
arad_fabric_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback)
{
    soc_port_t inner_port;
    int blk_id, link_id;
    uint32 reg_val, field_val[1];
    int lane0,lane1,lane2,lane3;
    ARAD_LINK_STATE_INFO     info;
    soc_dcmn_port_pcs_t pcs;
    soc_dcmn_loopback_mode_t prev_loopback;
    SOC_TMC_LINK_STATE on_off;
    uint32 rx_field[1], tx_field[1];
    int locked;

    SOCDNX_INIT_FUNC_DEFS;
  
    link_id = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    locked = 0;

    
    SOCDNX_IF_ERR_EXIT(arad_link_on_off_get(unit, port, &info));
    on_off = info.on_off;
    if(info.on_off == SOC_TMC_LINK_STATE_ON) {
        info.on_off = SOC_TMC_LINK_STATE_OFF;
        info.serdes_also = TRUE;
        SOCDNX_IF_ERR_EXIT(arad_link_on_off_set(unit, port, &info));
    }
    
    blk_id = link_id/4;
    inner_port = link_id % 4;

    
    if(soc_dcmn_loopback_mode_mac_pcs == loopback) {
        SOCDNX_IF_ERR_EXIT(arad_port_control_pcs_get(unit, port, &pcs));
        if(soc_dcmn_port_pcs_64_66_fec != pcs) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,(_BSL_SOCDNX_MSG("MAC PCS llopback is valid only for 64/66 FEC encoding")));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(arad_fabric_loopback_get(unit, port, &prev_loopback));

    if(prev_loopback != loopback) {

        
        if(soc_dcmn_loopback_mode_mac_async_fifo == loopback || soc_dcmn_loopback_mode_mac_async_fifo == prev_loopback) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, blk_id, &reg_val));
            *field_val = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
            if(soc_dcmn_loopback_mode_mac_async_fifo == loopback) {
                SHR_BITSET(field_val, inner_port);
            } else {
                SHR_BITCLR(field_val, inner_port);
            }
            soc_reg_field_set(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, &reg_val, LCL_LPBK_ONf, *field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, blk_id, reg_val));
        }
        
        
        if(soc_dcmn_loopback_mode_mac_outer == loopback || soc_dcmn_loopback_mode_mac_outer == prev_loopback) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_CORE_40B_LOOPBACKf, soc_dcmn_loopback_mode_mac_outer == loopback ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, reg_val));

            if(soc_dcmn_loopback_mode_mac_outer == loopback) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val));
                SOC_DPP_PORT_PARAMS(unit).comma_burst_conf[port]=reg_val;
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, blk_id, inner_port, 0x00a0e513));

                SOCDNX_IF_ERR_EXIT(READ_FMAC_CONTROL_CELL_BURST_REGISTERr(unit, blk_id, &reg_val));
                SOC_DPP_PORT_PARAMS(unit).control_burst_conf[blk_id]=reg_val;
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_CONTROL_CELL_BURST_REGISTERr(unit, blk_id, 0x1801c00d));
            } else {
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, blk_id, inner_port, SOC_DPP_PORT_PARAMS(unit).comma_burst_conf[port]));

                
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, 0, &reg_val));
                lane0 = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_40B_LOOPBACKf);
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, 1, &reg_val));
                lane1 = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_40B_LOOPBACKf);
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, 2, &reg_val));
                lane2 = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_40B_LOOPBACKf);
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, 3, &reg_val));
                lane3 = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_40B_LOOPBACKf);
                if(!lane0 && !lane1 && !lane2 && !lane3) {
                    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_CONTROL_CELL_BURST_REGISTERr(unit, blk_id, SOC_DPP_PORT_PARAMS(unit).control_burst_conf[blk_id]));
                }
            }
        }

        if(soc_dcmn_loopback_mode_mac_pcs == loopback || soc_dcmn_loopback_mode_mac_pcs == prev_loopback) {
            
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_CONFIGURATIONr, &reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf, soc_dcmn_loopback_mode_mac_pcs == loopback ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_CONFIGURATIONr(unit, blk_id, inner_port, reg_val));
        }
        
        MIIM_LOCK(unit);
        locked = 1;
        
        
        if(soc_dcmn_loopback_mode_phy_gloop == loopback || soc_dcmn_loopback_mode_phy_gloop == prev_loopback) {
            if (!SOC_IS_ARDON(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_loopback_set(unit, port, soc_dcmn_loopback_mode_phy_gloop == loopback ? 1 : 0, 1)); 
            } else {
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_LOOPBACK_PMD, soc_dcmn_loopback_mode_phy_gloop == loopback ? 1 : 0));
            }
        }

        
        if(soc_dcmn_loopback_mode_phy_rloop == loopback || soc_dcmn_loopback_mode_phy_rloop == prev_loopback) {
            if (!SOC_IS_ARDON(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS, soc_dcmn_loopback_mode_phy_rloop == loopback ? 1 : 0));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_LOOPBACK_REMOTE, soc_dcmn_loopback_mode_phy_rloop == loopback ? 1 : 0));
            }
        }
        
        locked = 0;
        MIIM_UNLOCK(unit); 
        
        SOCDNX_IF_ERR_EXIT(arad_fabric_force_signal_detect_set(unit, blk_id));

    }

    if(on_off == SOC_TMC_LINK_STATE_ON) {
        info.on_off = SOC_TMC_LINK_STATE_ON;
        info.serdes_also = TRUE;
        SOCDNX_IF_ERR_EXIT(arad_link_on_off_set(unit, port, &info));
    }

    
    if(soc_dcmn_loopback_mode_mac_outer == loopback){
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, &reg_val));
        *rx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
        *tx_field = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);

        SHR_BITSET(rx_field, inner_port);
        SHR_BITSET(tx_field, inner_port);
  
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *rx_field);
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *tx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val));

        sal_usleep(10);

        SHR_BITCLR(rx_field, inner_port);
        SHR_BITCLR(tx_field, inner_port);
  
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *rx_field);
        soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *tx_field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, blk_id, reg_val));
    }

exit:
  if (locked) {
      MIIM_UNLOCK(unit);
  }
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback)
{
    uint32 inner_port;
    int blk_id;
    uint32 serdes_xrloop, serdes_xgloop;
    int serdes_gloop;
    uint32 reg_val, field_val[1];
    soc_error_t rc;
    int locked, link_id;
    SOCDNX_INIT_FUNC_DEFS;

    locked = 0;
  
    link_id = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link_id/4;
    inner_port = link_id % 4;

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, blk_id, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
    if(SHR_BITGET(field_val, inner_port)) {
        *loopback = soc_dcmn_loopback_mode_mac_async_fifo;
    } else {        
        
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val));
        *field_val = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_40B_LOOPBACKf);
        if(*field_val) {
            *loopback = soc_dcmn_loopback_mode_mac_outer;
        } else {
            
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val));
            *field_val = soc_reg_field_get(unit, FMAC_KPCS_CONFIGURATIONr, reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf);
            if(*field_val) {
                *loopback = soc_dcmn_loopback_mode_mac_pcs;
            } else {
                MIIM_LOCK(unit);
                locked = 1;
                
                if (!SOC_IS_ARDON(unit)) {
                    rc = soc_phyctrl_loopback_get(unit, port, &serdes_gloop); 
                } else {
                    rc = soc_phyctrl_control_get(unit, port, BCM_PORT_PHY_CONTROL_LOOPBACK_PMD, &serdes_xgloop);

                    serdes_gloop = serdes_xgloop ? 1 : 0;
                }
                locked = 0;
                MIIM_UNLOCK(unit);
                if(SOC_FAILURE(rc)) {
                    SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed in soc_phyctrl_loopback_get, error %s"),soc_errmsg(rc)));   
                } else if(serdes_gloop) {
                    *loopback = soc_dcmn_loopback_mode_phy_gloop;
                } else {
                    MIIM_LOCK(unit);
                    locked = 1;
                    
                    if (!SOC_IS_ARDON(unit)) {
                        rc = soc_phyctrl_control_get(unit, port, BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS, &serdes_xrloop);
                    } else {
                        rc = soc_phyctrl_control_get(unit, port, BCM_PORT_PHY_CONTROL_LOOPBACK_REMOTE, &serdes_xrloop);
                    }
                    locked = 0;
                    MIIM_UNLOCK(unit);
                    if(SOC_FAILURE(rc)) {
                        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed in soc_phyctrl_control_get(SOC_PHY_CONTROL_LOOPBACK_REMOTE), error %s"),soc_errmsg(rc))); 
                    } else if(serdes_xrloop) {
                        *loopback = soc_dcmn_loopback_mode_phy_rloop;
                    } else {

                        
                         *loopback = soc_dcmn_loopback_mode_none;
                    }
                }
            }
        }
    }

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_link_control_strip_crc_set(int unit, soc_port_t port, int strip_crc)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 general_config;
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, &general_config));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &general_config, FMAL_N_ENABLE_CELL_CRCf, strip_crc ? 0 : 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, general_config));
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
arad_link_control_strip_crc_get(int unit, soc_port_t port, int* strip_crc)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 general_config;
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_port, &general_config));
    *strip_crc = !soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, general_config, FMAL_N_ENABLE_CELL_CRCf);
 
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
arad_link_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 reg_val, field_val[1];
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    if(enable) {
        SHR_BITCLR(field_val,inner_port);
    } else {
        SHR_BITSET(field_val,inner_port);
    }

    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf,*field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,reg_val));

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
arad_link_control_tx_enable_set(int unit, soc_port_t port, int enable)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 reg_val, field_val[1];
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    MIIM_LOCK(unit);
  
    if (!enable) {
        
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_TX_LANE_SQUELCH, 1));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    if(enable) {
        SHR_BITCLR(field_val,inner_port);
    } else {
        SHR_BITSET(field_val,inner_port);
    }
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf,*field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,reg_val));
    if (enable) {
        
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_TX_LANE_SQUELCH, 0));
    }

exit:
  MIIM_UNLOCK(unit);
  SOCDNX_FUNC_RETURN; 
}


soc_error_t 
arad_link_control_rx_enable_get(int unit, soc_port_t port, int* enable)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 reg_val, field_val[1];
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    *enable = SHR_BITGET(field_val,inner_port) ? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_link_control_tx_enable_get(int unit, soc_port_t port, int* enable)
{
    uint32 inner_port;
    int blk_id, link;
    uint32 reg_val, field_val[1];
    SOCDNX_INIT_FUNC_DEFS;

    link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = link/4;
    inner_port = link % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    *enable = SHR_BITGET(field_val,inner_port) ? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_link_port_fault_get(int unit, bcm_port_t link_id, uint32* flags)
{
  uint64 reg_64val; 
  uint32 arr_64bits[2];
  SOCDNX_INIT_FUNC_DEFS;

  (*flags) = 0;
  SOCDNX_IF_ERR_EXIT(READ_RTP_LOCALLY_GENERATED_ACLr_REG64(unit, &reg_64val)); 
  arr_64bits[1] = COMPILER_64_HI(reg_64val);
  arr_64bits[0] = COMPILER_64_LO(reg_64val);

  if(!SHR_BITGET(arr_64bits,link_id)) {
      (*flags) |= BCM_PORT_FAULT_LOCAL;
  }

  SOCDNX_IF_ERR_EXIT(READ_RTP_ACL_RECEIVEDr_REG64(unit, &reg_64val)); 
  arr_64bits[1] = COMPILER_64_HI(reg_64val);
  arr_64bits[0] = COMPILER_64_LO(reg_64val);
  if(!SHR_BITGET(arr_64bits,link_id)) {
      (*flags) |= BCM_PORT_FAULT_REMOTE;
  }  

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_link_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate)
{
  SOCDNX_INIT_FUNC_DEFS;

  if(bucket_fill_rate > ARAD_MAX_BUCKET_FILL_RATE) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("BUCKET_FILL_RATE: %d is out-of-ranget"), bucket_fill_rate));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_links_llf_control_source_set(int unit, soc_port_t link, soc_dcmn_fabric_control_source_t val) 
{
  soc_port_t inner_lnk;
  uint32 blk_id, reg_val;
  uint32 field_val_rx[1], field_val_tx[1];
  SOCDNX_INIT_FUNC_DEFS;

  blk_id = link/4;
  inner_lnk = link % 4;

  
  SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, &reg_val));
  *field_val_rx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
  *field_val_tx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
  if(soc_dcmn_fabric_control_source_none == val) {
    SHR_BITCLR(field_val_rx, inner_lnk);
    SHR_BITCLR(field_val_tx, inner_lnk);
  } else {
    SHR_BITSET(field_val_rx, inner_lnk);
    SHR_BITSET(field_val_tx, inner_lnk);
  }
  soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_RX_ENf, *field_val_rx);
  soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_TX_ENf, *field_val_tx);
  SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, reg_val));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_links_llf_control_source_get(int unit, soc_port_t link, soc_dcmn_fabric_control_source_t* val) 
{
  soc_port_t inner_lnk;
  uint32 blk_id, reg_val;
  uint32 field_val_rx[1], field_val_tx[1];
  SOCDNX_INIT_FUNC_DEFS;

  blk_id = link/4;
  inner_lnk = link % 4;

  
  SOCDNX_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, &reg_val));
  *field_val_rx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
  *field_val_tx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
  if(!SHR_BITGET(field_val_rx, inner_lnk) && !SHR_BITGET(field_val_tx, inner_lnk)) {
    *val = soc_dcmn_fabric_control_source_none;
  } else {
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBALEr(unit, &reg_val));
    if(soc_reg_field_get(unit, ECI_GLOBALEr, reg_val, PARALLEL_DATA_PATHf)) {
      *val = soc_dcmn_fabric_control_source_both;
    } else {
      *val = soc_dcmn_fabric_control_source_primary;
    }

  }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_links_isolate_set(int unit, soc_port_t link, soc_dcmn_isolation_status_t val)
{
  uint64 reachability_allowed_bm;
  uint64 specific_link_bm;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr_REG64(unit,&reachability_allowed_bm));

  COMPILER_64_SET(specific_link_bm,0,1);
  COMPILER_64_SHL(specific_link_bm,link);

  if(soc_dcmn_isolation_status_active == val)
  {
    
    COMPILER_64_OR(reachability_allowed_bm,specific_link_bm); 
  }
  else
  {
    
    COMPILER_64_NOT(specific_link_bm);
    COMPILER_64_AND(reachability_allowed_bm, specific_link_bm);
  }

  SOCDNX_IF_ERR_EXIT(WRITE_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr_REG64(unit,reachability_allowed_bm));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_links_isolate_get(int unit, soc_port_t link, soc_dcmn_isolation_status_t* val)
{
  uint64 reachability_allowed_bm;
  uint64 specific_link_bm;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(READ_RTP_ALLOWED_LINKS_FOR_REACHABILITY_MESSAGESr_REG64(unit,&reachability_allowed_bm));

  COMPILER_64_SET(specific_link_bm,0,1);
  COMPILER_64_SHL(specific_link_bm,link);

  COMPILER_64_AND(specific_link_bm,reachability_allowed_bm);
  if(!COMPILER_64_IS_ZERO(specific_link_bm))
    *val = soc_dcmn_isolation_status_active;
  else
    *val = soc_dcmn_isolation_status_isolated;

exit:
  SOCDNX_FUNC_RETURN; 
}


soc_error_t 
arad_fabric_links_cell_interleaving_set(int unit, soc_port_t link, int val)
{
  uint32 interleaving_bm_32[1];
  uint32 blk_id;
  soc_port_t inner_lnk;
  SOCDNX_INIT_FUNC_DEFS;

  blk_id = (int)link/4;
  inner_lnk = link % 4;

  SOCDNX_IF_ERR_EXIT(READ_FMAC_CNTRL_INTRLVD_MODE_REGr(unit, blk_id, interleaving_bm_32));

  if(val)
  {
    
    SHR_BITSET(interleaving_bm_32, inner_lnk);
  }
  else
  {
    
    SHR_BITCLR(interleaving_bm_32, inner_lnk);
  }

  SOCDNX_IF_ERR_EXIT(WRITE_FMAC_CNTRL_INTRLVD_MODE_REGr(unit,blk_id, *interleaving_bm_32));

exit:
  SOCDNX_FUNC_RETURN;

}


soc_error_t 
arad_fabric_links_cell_interleaving_get(int unit, soc_port_t link, int* val)
{
  uint32 interleaving_bm_32[1];
  uint32 blk_id;
  soc_port_t inner_lnk;
  SOCDNX_INIT_FUNC_DEFS;

  blk_id = (int)link/4;
  inner_lnk = link % 4; 

  SOCDNX_IF_ERR_EXIT(READ_FMAC_CNTRL_INTRLVD_MODE_REGr(unit, blk_id, interleaving_bm_32));

  if(SHR_BITGET(interleaving_bm_32, inner_lnk))
  {
    *val = 1;
  }
  else
  {
    *val = 0;
  }   

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_fabric_links_nof_links_get(int unit, int* nof_links)
{
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(nof_links);

  *nof_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_fabric_cell_format_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CELL_FORMAT  *info
  )
{
  uint32
    fld_val = 0,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FABRIC_CELL_FORMAT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);


  fld_val = SOC_SAND_BOOL2NUM(info->segmentation_enable);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_FDT_SEGMENTATION_AND_INTERLEAVINGr, REG_PORT_ANY, 0, SEGMENT_PKTf,  fld_val));

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_cell_format_set_unsafe()",0,0);
}


int
  arad_fabric_link_up_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT int                 *up
  )
{
    uint32 
        link,
        is_down=0,
        reg_val,
        blk_id,
        inner_link,
        fld_val[1];
    soc_error_t
        rv;
    ARAD_LINK_STATE_INFO     info;

    SOCDNX_INIT_FUNC_DEFS;
  
    
    rv = arad_link_on_off_get(unit, port, &info);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if(info.on_off == SOC_TMC_LINK_STATE_ON) {
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_ARAD_NOF_LINKS_IN_MAC;
        inner_link = link % SOC_ARAD_NOF_LINKS_IN_MAC;
        reg_val = 0;
        *fld_val = (1 << inner_link);
        soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *fld_val);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val),rv);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val);
        SOCDNX_IF_ERR_EXIT(rv);
        *fld_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
        is_down = SHR_BITGET(fld_val, inner_link);
        *up = (is_down == 0);
    } else {
        *up = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fabric_nof_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                    *nof_links
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(nof_links);

    *nof_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fabric_nof_links_get()",0,0);
}



uint32
arad_fabric_wcmod_ucode_load(int unit ,int port, uint8 *arr, int arr_len) {
    int j, n;
    int max_ndx;
    int rv;
    uint32 reg_val32;
    int port_local, blk;
    uint8  phy_id;
    uint16 phy_wr_data, phy_rd_data;
    uint32 phy_data;
    soc_reg_above_64_val_t wr_data;
    soc_timeout_t to;
    int clause = 22; 
    soc_pbmp_t pbmp_sfi;

    
    uint32 xgswl_microcommand_a = 0x12;
    uint32 xgswl_microcommand3_a = 0x1c;
    uint32 xgswl_microblk_download_status = 0x15;
    uint32 xgswl_microblk_a = 0xffc;
    uint32 xgswl_blockaddress_a  = 0x1f;  
    uint32 xgswl_xgxsblk0_a = 0x800;
    uint32 xgswl_xgxscontrol_a = 0x10; 

    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    
    
    phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
    phy_data = 0x81f0;
    phy_wr_data = (uint16) (phy_data & 0xffff);
    rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_blockaddress_a, phy_wr_data);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dcmn_miim_read(unit, clause, phy_id, 0x10, &phy_rd_data);
    SOCDNX_IF_ERR_EXIT(rv);

    if(phy_rd_data != 0) {
        SOC_EXIT;
    }

    max_ndx = 32*1024;
    
    for (port_local = 0; port_local < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); port_local++) {
        SOCDNX_IF_ERR_EXIT(READ_FSRD_WC_UC_MEM_ACCESSr(unit, port_local, &reg_val32));
        soc_reg_field_set(unit, FSRD_WC_UC_MEM_ACCESSr, &reg_val32, CLK_DIVf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_ACCESSr(unit, port_local, reg_val32));

        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, port_local, 0xf));
    }

    
    
    BCM_PBMP_CLEAR(pbmp_sfi);
    BCM_PBMP_ASSIGN(pbmp_sfi, PBMP_SFI_ALL(unit));
    
    for (port_local=0; port_local<SOC_DPP_DEFS_GET(unit, nof_fabric_links); port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {

        if (!BCM_PBMP_MEMBER(pbmp_sfi, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local))) {
            continue;
        }
        
        phy_id = PORT_TO_PHY_ADDR_INT(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local));
        
        
        phy_data = xgswl_xgxsblk0_a << 4;
        phy_wr_data = (uint16) (phy_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_blockaddress_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_xgxscontrol_a, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_rd_data &= 0xdfff;
        phy_wr_data = (uint16) (phy_rd_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_xgxscontrol_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        phy_data = xgswl_microblk_a << 4;
        phy_wr_data = (uint16) (phy_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_blockaddress_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_data = 0x8000;
        phy_wr_data = (uint16) (phy_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_microcommand_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

    }

    
    for (port_local=0; port_local<SOC_DPP_DEFS_GET(unit, nof_fabric_links); port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {

        if (!BCM_PBMP_MEMBER(pbmp_sfi, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local))) {
            continue;
        }

        phy_id = PORT_TO_PHY_ADDR_INT(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local));
        
        soc_timeout_init(&to, 100000 , 100);   
        
        while(1)
        {
            rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_microblk_download_status, &phy_rd_data);
            SOCDNX_IF_ERR_EXIT(rv);

            if(phy_rd_data & 0x8000 ) {            
                break;
            }
            
            if (soc_timeout_check(&to)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("WL Initialize program memory - timeout"))); 
            }
        }
    }
    for (port_local=0; port_local<SOC_DPP_DEFS_GET(unit, nof_fabric_links); port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {

        if (!BCM_PBMP_MEMBER(pbmp_sfi, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local))) {
            continue;
        }

        phy_id = PORT_TO_PHY_ADDR_INT(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local));

        
        rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_microcommand3_a, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_wr_data = (uint16) ((phy_rd_data & 0xfffc) | 0x0003);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_microcommand3_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_data = arr_len;
        phy_wr_data = (uint16) (phy_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, 0x10, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

    }

    for (port_local = 0; port_local < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); port_local++) {
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, port_local, 0xf));
    }
    
    
    for (j=0; j<max_ndx; j+=16) {
         
        SOC_REG_ABOVE_64_CLEAR(wr_data);
         for (n=0; n<16; n++) {
             if (j+n<arr_len) {
                 wr_data[(15- n)/4] |= (arr[j+n] & 0xff) <<  ((15-n) % 4)*8;
             }
        }

        SOC_BLOCK_ITER(unit, blk, SOC_BLK_FSRD) {
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, FSRD_FSRD_WL_EXT_MEMm, blk, j/16, wr_data));
        }
    }

    for (port_local = 0; port_local < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); port_local++) {
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, port_local, 0x0));
    }

    for (port_local=0; port_local<SOC_DPP_DEFS_GET(unit, nof_fabric_links); port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {
        
        if (!BCM_PBMP_MEMBER(pbmp_sfi, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local))) {
            continue;
        }
        
        phy_id = PORT_TO_PHY_ADDR_INT(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit,port_local));
        
        
        phy_data = xgswl_microblk_a << 4;
        phy_wr_data = (uint16) (phy_data & 0xffff);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_blockaddress_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_microcommand3_a, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_wr_data = (uint16) (phy_rd_data & 0xfffc);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_microcommand3_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_microcommand_a, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_wr_data = (uint16) ((phy_rd_data & 0xE7FF) | 0x0800);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_microcommand_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = soc_dcmn_miim_read(unit, clause, phy_id, xgswl_microcommand_a, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);

        phy_wr_data = (uint16) ((phy_rd_data & 0xFFEF) | 0x0010);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, xgswl_microcommand_a, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);

    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}



uint32 arad_fabric_gci_enable_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN  ARAD_FABRIC_GCI_TYPE                    type,
    SOC_SAND_OUT int                                    *enable
  )
{
    uint32
        res = SOC_SAND_OK;
    uint32 value;
    int i;
    soc_reg_t registers_gci_map_masking[7];
    soc_reg_t fields_gci_map_masking[7];
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    switch (type)
    {
       case ARAD_FABRIC_GCI_TYPE_LEAKY_BUCKET:
           
           registers_gci_map_masking[0] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[0] = GCI_LB_2_IPT_DTQ_MC_FC_MAPf;
           
           registers_gci_map_masking[1] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[1] = GCI_LB_2_IPT_GFMC_FC_MAPf;
           
           registers_gci_map_masking[2] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[2] = GCI_LB_2_IPT_BFMC_FC_MAPf;
           
           registers_gci_map_masking[3] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[3] = GCI_LB_2_IPS_GFMC_FC_MAPf;
           
           registers_gci_map_masking[4] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[4] = GCI_LB_2_IPS_BFMC_0_FC_MAPf;
           
           registers_gci_map_masking[5] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[5] = GCI_LB_2_IPS_BFMC_1_FC_MAPf;
           
           registers_gci_map_masking[6] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[6] = GCI_LB_2_IPS_BFMC_2_FC_MAPf;
           break;
        case ARAD_FABRIC_GCI_TYPE_RANDOM_BACKOFF:
           
           registers_gci_map_masking[0] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[0] = GCI_BCKOF_2_IPT_DTQ_MC_FC_MAPf;
           
           registers_gci_map_masking[1] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[1] = GCI_BCKOF_2_IPT_GFMC_FC_MAPf;
           
           registers_gci_map_masking[2] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[2] = GCI_BCKOF_2_IPT_BFMC_FC_MAPf;
           
           registers_gci_map_masking[3] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[3] = GCI_BCKOF_2_IPS_GFMC_FC_MAPf;
           
           registers_gci_map_masking[4] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[4] = GCI_BCKOF_2_IPS_BFMC_0_FC_MAPf;
           
           registers_gci_map_masking[5] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[5] = GCI_BCKOF_2_IPS_BFMC_1_FC_MAPf;
           
           registers_gci_map_masking[6] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[6] = GCI_BCKOF_2_IPS_BFMC_2_FC_MAPf;
           break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 70, exit);   
            break;
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1000,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, registers_gci_map_masking[0], REG_PORT_ANY, 0, fields_gci_map_masking[0], &value));
    *enable = value ? 1 : 0;

    
    for (i=0; i<7; i++)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  i,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, registers_gci_map_masking[i], REG_PORT_ANY, 0, fields_gci_map_masking[i], &value));
        if ((value ? 1 : 0) != *enable)
        {
            
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100 + i, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_gci_enable_get()", 0, 0);
}


#define ARAD_FABRIC_GCI_LB_2_IPT_DTQ_DEFAULT_VALUE          (0xf)
#define ARAD_FABRIC_GCI_LB_2_IPT_GFMC_DEFAULT_VALUE         (0x8)
#define ARAD_FABRIC_GCI_LB_2_IPT_BFMC_DEFAULT_VALUE         (0xf)
#define ARAD_FABRIC_GCI_LB_2_IPS_GFMC_DEFAULT_VALUE         (0x8)
#define ARAD_FABRIC_GCI_LB_2_IPS_BFMC_0_DEFAULT_VALUE       (0xf)
#define ARAD_FABRIC_GCI_LB_2_IPS_BFMC_1_DEFAULT_VALUE       (0xe)
#define ARAD_FABRIC_GCI_LB_2_IPS_BFMC_2_DEFAULT_VALUE       (0xc)

#define ARAD_FABRIC_GCI_BCKOF_2_IPT_DTQ_DEFAULT_VALUE       (0x7)
#define ARAD_FABRIC_GCI_BCKOF_2_IPT_GFMC_DEFAULT_VALUE      (0x4)
#define ARAD_FABRIC_GCI_BCKOF_2_IPT_BFMC_DEFAULT_VALUE      (0x7)
#define ARAD_FABRIC_GCI_BCKOF_2_IPS_GFMC_DEFAULT_VALUE      (0x4)
#define ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_0_DEFAULT_VALUE    (0x7)
#define ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_1_DEFAULT_VALUE    (0x7)
#define ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_2_DEFAULT_VALUE    (0x6)

uint32 arad_fabric_gci_enable_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN ARAD_FABRIC_GCI_TYPE                    type,
    SOC_SAND_IN int                                     enable
  )
{
    uint32
        res = SOC_SAND_OK;
    int i;
    soc_reg_t registers_gci_map_masking[7];
    soc_reg_t fields_gci_map_masking[7];
    soc_reg_t values_gci_map_masking[7];
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);



    
    switch (type)
    {
       case ARAD_FABRIC_GCI_TYPE_LEAKY_BUCKET:
           
           registers_gci_map_masking[0] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[0] = GCI_LB_2_IPT_DTQ_MC_FC_MAPf;
           values_gci_map_masking[0] = enable ? ARAD_FABRIC_GCI_LB_2_IPT_DTQ_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[1] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[1] = GCI_LB_2_IPT_GFMC_FC_MAPf;
           values_gci_map_masking[1] = enable ? ARAD_FABRIC_GCI_LB_2_IPT_GFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[2] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[2] = GCI_LB_2_IPT_BFMC_FC_MAPf;
           values_gci_map_masking[2] = enable ? ARAD_FABRIC_GCI_LB_2_IPT_BFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[3] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[3] = GCI_LB_2_IPS_GFMC_FC_MAPf;
           values_gci_map_masking[3] = enable ? ARAD_FABRIC_GCI_LB_2_IPS_GFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[4] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[4] = GCI_LB_2_IPS_BFMC_0_FC_MAPf;
           values_gci_map_masking[4] = enable ? ARAD_FABRIC_GCI_LB_2_IPS_BFMC_0_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[5] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[5] = GCI_LB_2_IPS_BFMC_1_FC_MAPf;
           values_gci_map_masking[5] = enable ? ARAD_FABRIC_GCI_LB_2_IPS_BFMC_1_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[6] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[6] = GCI_LB_2_IPS_BFMC_2_FC_MAPf;
           values_gci_map_masking[6] = enable ? ARAD_FABRIC_GCI_LB_2_IPS_BFMC_2_DEFAULT_VALUE : 0;
           break;
        case ARAD_FABRIC_GCI_TYPE_RANDOM_BACKOFF:
           
           registers_gci_map_masking[0] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[0] = GCI_BCKOF_2_IPT_DTQ_MC_FC_MAPf;
           values_gci_map_masking[0] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPT_DTQ_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[1] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[1] = GCI_BCKOF_2_IPT_GFMC_FC_MAPf;
           values_gci_map_masking[1] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPT_GFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[2] = (SOC_IS_QAX(unit) ? PTS_IPT_INTRNL_FMC_FC_MAPr : IPT_IPT_INTRNL_FMC_FC_MAPr);
           fields_gci_map_masking[2] = GCI_BCKOF_2_IPT_BFMC_FC_MAPf;
           values_gci_map_masking[2] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPT_BFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[3] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[3] = GCI_BCKOF_2_IPS_GFMC_FC_MAPf;
           values_gci_map_masking[3] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPS_GFMC_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[4] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[4] = GCI_BCKOF_2_IPS_BFMC_0_FC_MAPf;
           values_gci_map_masking[4] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_0_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[5] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[5] = GCI_BCKOF_2_IPS_BFMC_1_FC_MAPf;
           values_gci_map_masking[5] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_1_DEFAULT_VALUE : 0;
           
           registers_gci_map_masking[6] = (SOC_IS_QAX(unit) ? PTS_IPT_FMC_IPS_FC_MAPr : IPT_IPT_FMC_IPS_FC_MAP_1r);
           fields_gci_map_masking[6] = GCI_BCKOF_2_IPS_BFMC_2_FC_MAPf;
           values_gci_map_masking[6] = enable ? ARAD_FABRIC_GCI_BCKOF_2_IPS_BFMC_2_DEFAULT_VALUE : 0;
           break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 70, exit);   
            break;
    }

    
    for (i=0; i<7; i++)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  i,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, registers_gci_map_masking[i], REG_PORT_ANY, 0, fields_gci_map_masking[i],  values_gci_map_masking[i]));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_gci_enable_set()", 0, 0);
}

uint32 arad_fabric_gci_config_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN ARAD_FABRIC_GCI_CONFIG_TYPE             type,
    SOC_SAND_OUT int                                    *value
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_t gci_backoff_range_thresholds_reg = (SOC_IS_QAX(unit) ? PTS_GCI_BACKOFF_RANGE_THRESHOLDSr : IPT_GCI_BACKOFF_RANGE_THRESHOLDSr);
    soc_reg_t gci_leaky_bucket_configuration_register_0_reg = (SOC_IS_QAX(unit) ? PTS_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_0r : IPT_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_0r);
    soc_reg_t gci_leaky_bucket_configuration_register_2_reg = (SOC_IS_QAX(unit) ? PTS_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_2r : IPT_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_2r);
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    switch (type)
    {
        
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, gci_backoff_range_thresholds_reg, REG_PORT_ANY, 0, CNGST_LVL_THRESH_0f, (uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, gci_backoff_range_thresholds_reg, REG_PORT_ANY, 0, CNGST_LVL_THRESH_1f, (uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, gci_backoff_range_thresholds_reg, REG_PORT_ANY, 0, CNGST_LVL_THRESH_2f, (uint32*)value));
            break;

        
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_1f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_2f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_3f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_4f,(uint32*)value));
            break;

         
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_1f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_2f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_3f,(uint32*)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_4f,(uint32*)value));
            break;


       default:
            SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_GCI_CONFIG_TYPE_OUT_OF_RANGE, 70, exit);   
            break;
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_gci_config_get()", 0, 0);
}

uint32 arad_fabric_gci_config_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN ARAD_FABRIC_GCI_CONFIG_TYPE            type,
    SOC_SAND_IN int                                    value
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_t gci_backoff_range_thresholds_reg = (SOC_IS_QAX(unit) ? PTS_GCI_BACKOFF_RANGE_THRESHOLDSr : IPT_GCI_BACKOFF_RANGE_THRESHOLDSr);
    soc_reg_t gci_leaky_bucket_configuration_register_0_reg = (SOC_IS_QAX(unit) ? PTS_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_0r : IPT_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_0r);
    soc_reg_t gci_leaky_bucket_configuration_register_2_reg = (SOC_IS_QAX(unit) ? PTS_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_2r : IPT_GCI_LEAKY_BUCKET_CONFIGURATION_REGISTER_2r);
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    switch (type)
    {
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0:
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1:
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2:
            SOC_SAND_ERR_IF_ABOVE_MAX(
            value, ARAD_FABRIC_IPT_GCI_BACKOFF_CONGESTION_LEVEL_MAX,
            SOC_TMC_INPUT_OUT_OF_RANGE, 100, exit
            );
            break;

       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL:
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL:
           SOC_SAND_ERR_IF_ABOVE_MAX(
            value, ARAD_FABRIC_GCI_BUCKET_MAX,
            SOC_TMC_INPUT_OUT_OF_RANGE, 101, exit
           );
            break;

       default:
            SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_GCI_CONFIG_TYPE_OUT_OF_RANGE, 70, exit);   
            break;
    }

    
    switch (type)
    {
       case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_backoff_range_thresholds_reg,REG_PORT_ANY,0, CNGST_LVL_THRESH_0f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_backoff_range_thresholds_reg,REG_PORT_ANY,0, CNGST_LVL_THRESH_1f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_backoff_range_thresholds_reg,REG_PORT_ANY,0, CNGST_LVL_THRESH_2f, (uint32)value));
            break;

        
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  4,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_1f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_2f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_3f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_2_reg,REG_PORT_ANY,0, LKY_BKT_CNG_TH_4f, (uint32)value));
            break;

         
       case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_1f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_2f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_3f, (uint32)value));
            break;
        case ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,gci_leaky_bucket_configuration_register_0_reg,REG_PORT_ANY,0, LKY_BKT_MAX_CNT_4f, (uint32)value));
            break;
        
        
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_FABRIC_GCI_CONFIG_TYPE_OUT_OF_RANGE, 70, exit);   
            break;
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_gci_config_set()", 0, 0);
}


uint32 arad_fabric_llfc_threshold_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT int                                    *value
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *value=0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,FDR_LINK_LEVEL_FLOW_CONTROLr,REG_PORT_ANY,0, LNK_LVL_FC_THf,(uint32*)value));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_llfc_threshold_get()", 0, 0);
}

uint32 arad_fabric_llfc_threshold_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN int                                    value
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_MAX(
       value, ARAD_FABRIC_LLFC_RX_MAX,
       SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit
     );

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, REG_PORT_ANY, 0, LNK_LVL_FC_THf,  value));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_llfc_threshold_set()", 0, 0);
}

uint32 arad_fabric_minimal_links_to_dest_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_module_t                         module_id,
    SOC_SAND_IN int                                  minimum_links
   )
{
   SOCDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_88660_A0
   if (SOC_IS_ARDON(unit)) {
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, RTP_MIN_NUM_OF_LINKS_PER_FAPr, REG_PORT_ANY, 0, MIN_NUM_OF_LINKS_PER_FAPf,  minimum_links)); 
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, RTP_MIN_NUM_OF_LINKS_PER_FAPr, REG_PORT_ANY, 0, MIN_NUM_OF_LINKS_PER_FAP_ENf,  minimum_links? 1:0));
   } else {
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, RTP_SPARE_REGISTER_2r, REG_PORT_ANY, 0, ECO_MIN_NOF_LINKSf,  minimum_links)); 
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, RTP_SPARE_REGISTER_2r, REG_PORT_ANY, 0, ECO_ENABLEDf,  minimum_links? 1:0));
   }
#endif

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 arad_fabric_minimal_links_to_dest_get(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN soc_module_t                        module_id,
    SOC_SAND_OUT int                                *minimum_links
    )
{
  uint32
        res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
#ifdef BCM_88660_A0
  if (SOC_IS_ARDON(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, RTP_MIN_NUM_OF_LINKS_PER_FAPr, REG_PORT_ANY, 0, MIN_NUM_OF_LINKS_PER_FAPf, (uint32*)minimum_links));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 1, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, RTP_SPARE_REGISTER_2r, REG_PORT_ANY, 0, ECO_MIN_NOF_LINKSf, (uint32*)minimum_links));
  }
#else
  *minimum_links = 0;
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_minimal_links_to_dest_get()",0,0);
    
}

uint32 arad_fabric_rci_enable_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT soc_dcmn_fabric_control_source_t       *value
  )
{
    uint32
        res = SOC_SAND_OK;
    uint32 fap_rci_en;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *value=0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_RCI_PARAMSr, SOC_CORE_ALL, 0, DEVICE_RCI_ENAf, &fap_rci_en));

    
    if (fap_rci_en)
    {
        *value = soc_dcmn_fabric_control_source_both;
    } else {
        *value = soc_dcmn_fabric_control_source_none;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_rci_enable_get()", 0, 0);
}

uint32 arad_fabric_rci_enable_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN soc_dcmn_fabric_control_source_t        value
  )
{
    int fap_rci_en = 0;
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   
    switch (value)
    {
       case soc_dcmn_fabric_control_source_none:
           fap_rci_en = 0;
           break;
       case soc_dcmn_fabric_control_source_both:
           fap_rci_en = 1;
           break;
       default:
           
           SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
           break;

    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_RCI_PARAMSr, SOC_CORE_ALL, 0, DEVICE_RCI_ENAf,  fap_rci_en));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_rci_enable_set()", 0, 0);
}

uint32 arad_fabric_rci_config_get ( 
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_FABRIC_RCI_CONFIG_TYPE            rci_config_type,  
    SOC_SAND_OUT int                                    *value
  )
{
    uint32
        res = SOC_SAND_OK;
    int value_low, value_high;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *value=0;
    switch (rci_config_type)
    {
       
       case SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH:
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FDR_LOCAL_FIFO_RCI_LEVELr, REG_PORT_ANY, 0, RCI_LOW_LEVELf, (uint32*)&value_low));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FDR_LOCAL_FIFO_RCI_LEVELr, REG_PORT_ANY, 0, RCI_HIGH_LEVELf, (uint32*)&value_high));

        if (value_low != value_high)
        {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
        *value = value_low;
        break;

       
       case SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE:
           SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_RCI_PARAMSr, SOC_CORE_ALL, 0, RCI_INC_VALf, (uint32*)value));
           break;

       default:
           SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 100, exit);   
           break;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_rci_config_get()", 0, 0);
}

uint32 arad_fabric_rci_config_set ( 
    SOC_SAND_IN  int                                unit,  
    SOC_SAND_IN  ARAD_FABRIC_RCI_CONFIG_TYPE           rci_config_type,
    SOC_SAND_IN int                                    value
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (rci_config_type)
    {
       
       case SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH:
           SOC_SAND_ERR_IF_ABOVE_MAX(
               value, ARAD_FABRIC_RCI_THRESHOLD_MAX,
               SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit
           );

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_LOCAL_FIFO_RCI_LEVELr, REG_PORT_ANY, 0, RCI_LOW_LEVELf,  value));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_LOCAL_FIFO_RCI_LEVELr, REG_PORT_ANY, 0, RCI_HIGH_LEVELf,  value));
            break;

       
       case SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE:
           SOC_SAND_ERR_IF_ABOVE_MAX(
               value, ARAD_FABRIC_RCI_INC_VAL_MAX,
               SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit
           );
           SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_RCI_PARAMSr, SOC_CORE_ALL, 0, RCI_INC_VALf,  value));
           break;

       default:
           SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 100, exit);   
           break;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_rci_config_set()", 0, 0);
}

#ifdef BCM_88660_A0

uint32
arad_fabric_empty_cell_size_set (
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 fmac_index,  
   SOC_SAND_IN  uint32                                 cell_size
   )
{
    uint32
        res = SOC_SAND_OK;
    uint32 cell_size_hw_format;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    
    if (SOC_IS_ARAD_B1_AND_BELOW(unit))
    {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 1, exit);
    }

    
    SOC_SAND_ERR_IF_BELOW_MIN (
        cell_size, ARAD_FABRIC_EMPTY_CELL_SIZE_MIN,
        SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit
    );
    SOC_SAND_ERR_IF_ABOVE_MAX (
        cell_size, ARAD_FABRIC_EMPTY_CELL_SIZE_MAX,
        SOC_TMC_INPUT_OUT_OF_RANGE, 20, exit
    );

    
    cell_size_hw_format = cell_size - ARAD_FABRIC_EMPTY_CELL_SIZE_MIN;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_SPARE_REGISTER_2r,  fmac_index,  0, EMPTY_CELL_SIZEf,  cell_size_hw_format)); 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_SPARE_REGISTER_2r,  fmac_index,  0, OVERRIDE_EMPTY_CELL_SIZEf,  0x1)); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fabric_empty_cell_size_set()", 0, 0);

}
#endif 




int 
arad_fabric_aldwp_config(
   SOC_SAND_IN  int                                 unit)
{
    uint32 max_cell_size, aldwp, highest_aldwp;
    uint32 core_clock_speed;
    int speed;
    soc_dcmn_port_pcs_t encoding;
    soc_port_t port_index;
    uint64 reg64_val;
    uint32 rv;
    int enable;
    soc_field_t aldwp_field;

    SOCDNX_INIT_FUNC_DEFS;

    

    

    


    
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.is_fe600) {
        max_cell_size = ARAD_FABRIC_VSC128_MAX_CELL_SIZE;
    } else {
        max_cell_size = SOC_DPP_DEFS_GET(unit, fabric_vsc256_max_cell_size);
    }
    core_clock_speed = arad_chip_kilo_ticks_per_sec_get(unit);


    highest_aldwp = 0;
    for (port_index = SOC_DPP_FABRIC_LINK_TO_PORT(unit, 0); 
          port_index < SOC_DPP_FABRIC_LINK_TO_PORT(unit, SOC_DPP_DEFS_GET(unit, nof_fabric_links)); 
          port_index++)
    {

        if (!SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port_index))
        {
            
            continue;
        }

        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_enable_get, (unit, port_index, 0, &enable));
        SOCDNX_IF_ERR_EXIT(rv);
        if (!enable) {
            
            continue;
        }
        
        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_speed_get, (unit, port_index, &speed));
        SOCDNX_IF_ERR_EXIT(rv);
        if (speed == 0) {
            
            continue;
        }


        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_control_pcs_get, (unit, port_index, &encoding));
        SOCDNX_IF_ERR_EXIT(rv);

        if (encoding == soc_dcmn_port_pcs_8_10) {
            
            speed = (speed * ARAD_PORT_PCS_8_10_EFFECTIVE_RATE_PERCENT) / 100;
        }

        
        aldwp = (3* max_cell_size * core_clock_speed) / (speed / 8 ); 
        aldwp = aldwp /(1024*64)  + ((aldwp % (1024*64) != 0) ? 1 : 0) ; 

        
        highest_aldwp = (aldwp > highest_aldwp ? aldwp : highest_aldwp);    
    }

    if (highest_aldwp != 0 ) {
        highest_aldwp += ARAD_FABRIC_ALDWP_FAP_OFFSET;
    }


    
    if (highest_aldwp == 0)
    {
        
        SOC_EXIT;
    } else if (highest_aldwp < ARAD_FABRIC_ALDWP_MIN)
    {
        
        
        highest_aldwp = ARAD_FABRIC_ALDWP_MIN;
    } else if (highest_aldwp > ARAD_FABRIC_ALDWP_MAX)
    {
        highest_aldwp = ARAD_FABRIC_ALDWP_MAX;
    }

    
    SOCDNX_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64_val));
    aldwp_field = ( (SOC_IS_QAX(unit) || SOC_IS_JERICHO_PLUS_A0(unit)) ? FIELD_0_3f : FIELD_19_22f);
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r , &reg64_val, aldwp_field, highest_aldwp);
    SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
arad_fabric_link_tx_traffic_disable_set(int unit, soc_port_t link, int disable)
{
    int rv;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_RTP_ALLOWED_LINKSr_REG64(unit, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (disable)
    {
        COMPILER_64_BITCLR(reg64_val, link);
    } else {
        COMPILER_64_BITSET(reg64_val, link);
    }
    rv = WRITE_RTP_ALLOWED_LINKSr_REG64(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = READ_RTP_MULTICAST_LINK_UPr_REG64(unit, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (disable)
    {
        COMPILER_64_BITCLR(reg64_val, link);
    } else {
        COMPILER_64_BITSET(reg64_val, link);
    }
    rv = WRITE_RTP_MULTICAST_LINK_UPr_REG64(unit, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
arad_fabric_link_tx_traffic_disable_get(int unit, soc_port_t link, int *disable)
{
    int rv;
    uint64 reg64_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_RTP_ALLOWED_LINKSr_REG64(unit, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    
    *disable = COMPILER_64_BITTEST(reg64_val, link) ? 0 : 1;

exit:
    SOCDNX_FUNC_RETURN;
}





uint32 
  arad_fabric_mesh_check(
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_IN uint8                                    stand_alone,
    SOC_SAND_OUT uint8                                   *success
  )
{
    uint32 buffer_1, buffer_2;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, MESH_TOPOLOGY_MESH_STATUSESr, REG_PORT_ANY, 0, MESH_STATUS_2f, &buffer_1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, MESH_TOPOLOGY_THRESHOLDSr, REG_PORT_ANY, 0, THRESHOLD_0f, &buffer_2));

    if ((buffer_1 < buffer_2) || (stand_alone == TRUE))
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

soc_error_t 
arad_fabric_prbs_polynomial_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode){ 
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Polynomial set isn't supported for MAC PRBS\n")));
    }else { 
        switch(value){
        case SOC_PHY_PRBS_POLYNOMIAL_X7_X6_1:
        case SOC_PHY_PRBS_POLYNOMIAL_X15_X14_1:
        case SOC_PHY_PRBS_POLYNOMIAL_X23_X18_1:
        case SOC_PHY_PRBS_POLYNOMIAL_X31_X28_1:
            MIIM_LOCK(unit);
            rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_PRBS_POLYNOMIAL, value);
            MIIM_UNLOCK(unit);
            SOCDNX_IF_ERR_EXIT(rv);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid value %d"), value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_fabric_prbs_polynomial_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode){ 
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Polynomial get isn't supported for MAC PRBS\n")));
    }else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_PRBS_POLYNOMIAL, (uint32*)value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_brdc_fsrd_blk_id_set(int unit){

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 


