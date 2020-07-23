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

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT



#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/mem.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/TMC/tmc_api_framework.h>
#include <soc/dpp/PPC/ppc_api_port.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/JER/jer_egr_queuing.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_action_cmd.h>
#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_defs.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>

#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>

#include <soc/dpp/JER/jer_ports.h>

#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/trunk_sw_db.h>


#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/pon.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/ARAD/arad_drv.h>








#define ARAD_PORTS_IF_ITERATIONS_MAX (ARAD_NOF_WARP_CORES * ARAD_NOF_IF_CHANNELS_MAX + 10)





#define ARAD_PORTS_FAP_PORT_INVALID ARAD_MAX_FAP_PORT_ID


#define  ARAD_NOF_ILKN_IF_CHANNELS_MAX 80


#define  ARAD_NOF_CPU_IF_CHANNELS_MAX 64


#define  ARAD_PORT_NIF_CTXT_MAP_MAX 512


#define ARAD_PORTS_CH_UNKNOWN 0xffff


#define ARAD_PORTS_REASSEMBLY_CONTEXT_ERP (ARAD_PORTS_NOF_REASSEMBLY_CONTEXT-2) 
#define ARAD_PORTS_REASSEMBLY_CONTEXT_OLP (ARAD_PORTS_NOF_REASSEMBLY_CONTEXT-3)
#define ARAD_PORTS_REASSEMBLY_CONTEXT_OAMP (ARAD_PORTS_NOF_REASSEMBLY_CONTEXT-4)
#define ARAD_PORTS_REASSEMBLY_CONTEXT_LAST (ARAD_PORTS_REASSEMBLY_CONTEXT_OAMP)



#define ARAD_EGQ_TXI_TH_ID_10GB       0


#define ARAD_EGQ_TXI_TH_ID_20GB       1


#define ARAD_EGQ_TXI_TH_ID_40GB       2


#define ARAD_EGQ_TXI_TH_ID_100GB      3


#define ARAD_EGQ_TXI_TH_ID_200GB      4


#define ARAD_EGQ_TXI_TH_ID_CPU        5


#define ARAD_EGQ_TXI_TH_ID_OLP        5


#define ARAD_EGQ_TXI_TH_ID_OAMP       5


#define ARAD_EGQ_TXI_TH_ID_RCY        6 


#define ARAD_EGQ_TXI_TH_ID_ILKN_RETRANSMIT        7


#define ARAD_EGQ_TXI_TH_TM_INTERNAL_PKT           2


#define ARAD_EGQ_TXI_TH_ID_OFF        1

#define ARAD_EGQ_TXI_SEL_REG_FLD_SIZE_BITS    3



#define ARAD_EGQ_FREQ_RDY_TH_ID_10GB  5


#define ARAD_EGQ_FREQ_RDY_TH_ID_20GB  10 


#define ARAD_EGQ_FREQ_RDY_TH_ID_40GB  19


#define ARAD_EGQ_FREQ_RDY_TH_ID_100GB 47


#define ARAD_EGQ_FREQ_RDY_TH_ID_200GB 64


#define ARAD_EGQ_FREQ_RDY_TH_ID_OFF   0x20

#define ARAD_EGQ_FREQ_RDY_SEL_REG_FLD_SIZE_BITS    8




#define ARAD_PORT_NDX_MAX(unit)                                  (ARAD_NOF_LOCAL_PORTS(unit)-1)
#define ARAD_PORTS_FIRST_HEADER_SIZE_MAX                         (63)
#define ARAD_PORT_PP_PORT_INFO_FC_TYPE_MAX                       (ARAD_PORTS_NOF_FC_TYPES-1)

#define ARAD_PORTS_XGS_HG_MODID_ID_MAX                           (255)
#define ARAD_PORTS_XGS_HG_PORT_ID_MAX                            (63)

#define ARAD_PORTS_VENDOR_PP_SOURCE_BOARD_MAX                    (247)
#define ARAD_PORTS_VENDOR_PP_SOURCE_PORT_MAX                     (63)


#define ARAD_PORTS_IF_ITERATIONS_MAX (ARAD_NOF_WARP_CORES * ARAD_NOF_IF_CHANNELS_MAX + 10)

#define ARAD_PORTS_FEM_ACTION_ID_DFLT                            (0)
#define ARAD_PORTS_FEM_ACTION_ID_SRC_PORT_EXT                    (1)


#define ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1  (1)
#define ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_PON             (2)
#define ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_XGS_MAC_EXT     (ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_PON)
#define ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_VENDOR_PP       (ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_XGS_MAC_EXT)
#define ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_COE             (3)

#define ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR                       (0)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1    (1)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH2    (ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_PON               (2)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_XGS_MAC_EXT       (ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_PON)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_VENDOR_PP         (ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_XGS_MAC_EXT)
#define ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_COE               (3)

#define ARAD_PORTS_FEM_PROFILE_SRC_PORT_EXT                      (1)
#define ARAD_PORTS_FEM_PROFILE_SRC_PORT_INJECTED                 (2) 

#define ARAD_PORT_PTCH_1_SIZE_IN_BYTES                           (3) 
#define ARAD_PORT_PTCH_2_SIZE_IN_BYTES                           (2) 

#define ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT                       (3)
#define ARAD_PORTS_FEM_BIT_SELECT_MSB_PP_PORT_PROFILE_INJECTED   (31)




#ifdef BCM_88660_A0 
#define ARAD_PORT_LAG_LB_RNG_MAX_VAL(unit) \
    (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) ? 127 : 255
#else 
#define ARAD_PORT_LAG_LB_RNG_MAX_VAL(unit) 255    
#endif  \

#ifdef BCM_88660_A0 
#define ARAD_PORT_LAG_LB_RNG_SIZE(unit) \
    (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) ? 128 : 256
#else 
#define ARAD_PORT_LAG_LB_RNG_SIZE(unit) 256    
#endif  \



#define SOC_DCMN_PORT_PCS_8_9_LEGACY_FEC_VAL (0)
#define SOC_DCMN_PORT_PCS_8_10_VAL           (1) 
#define SOC_DCMN_PORT_PCS_64_66_FEC_VAL      (2)          
#define SOC_DCMN_PORT_PCS_64_66_BEC_VAL      (3)  

#define ARAD_PORTS_FEM_PP_PORT_PTCH_SIZE    (8)

#define ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT (4)
#define ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL       (3)

#define ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)  (SOC_IS_JERICHO(unit) ? (4) : (3))
#define ARAD_PORTS_FEM_PP_PORT_PON_CONST_SIZE (11)
#define ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE (7)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR_PP_PORT_SIZE (6)
#define ARAD_PORTS_FEM_PP_PORT_COE_PORT_SIZE (6)

#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_SIZE (6)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SIZE (4)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_SIZE (4)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_MSB (ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_SIZE-1)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_MSB (ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_MSB+ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SIZE)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_MSB (ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_MSB+ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_SIZE)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_UC_CTRL_TYPE 2
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SUB_TYPE 2
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_3_MC_TYPE 1

#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_FEM_LSB (0)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_FEM_LSB (24)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_FEM_LSB (31)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_HEADER_0_OFFSET (0)
#define ARAD_PORTS_FEM_PP_PORT_VENDOR2_HEADER_1_OFFSET (3)

#define ARAD_PORTS_FEM_PON_PP_PORT_OFFSET (2)
#define ARAD_PORTS_FEM_VENDOR_CUSTOM_PP_PORT_OFFSET (6)
#define ARAD_PORTS_FEM_COE_PORT_OFFSET (2)

#define ARAD_PORTS_FEM_PP_PORT_GPON_TID_PORT_SIZE (2)



#define ARAD_PORTS_FEM_XGS_MAC_EXT_PP_PORT_OFFSET (4)


#define ARAD_PORTS_VIRTUAL_PORT_PP_PORT_CONST_MAP_VAL (SOC_IS_JERICHO(unit)? 0x7F00: 0x3F00)
#define ARAD_PORTS_VIRTUAL_PORT_PP_PORT_CONST_INJECTED_TM_MAP_VAL (SOC_IS_JERICHO(unit)? 0x7E00: 0x3E00)


#define ARAD_PORTS_VIRTUAL_PORT_ENCODED(local_port) (local_port + ARAD_PORTS_VIRTUAL_PORT_PP_PORT_CONST_MAP_VAL)

#define ARAD_PORTS_VIRTUAL_PORT_INJECTED_TM_ENCODED(local_port) (local_port + ARAD_PORTS_VIRTUAL_PORT_PP_PORT_CONST_INJECTED_TM_MAP_VAL)

#define ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID(unit) ((2047-(256>>ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)))+1)


#define ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID_MODE0 (2047)
#define ARAD_PORTS_VIRTUAL_PORT_PON_LIMITED_PORT(unit)  ARAD_PORT_MAX_PON_PORT(unit)
#define ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID_IN_LIMITED_PORT ARAD_PORT_MAX_TUNNEL_ID_IN_MAX_PON_PORT 
#define ARAD_PORTS_VIRTUAL_PORT_PON_NOF_ENTRIES_MODE0(unit, local_port) \
    (local_port == ARAD_PORTS_VIRTUAL_PORT_PON_LIMITED_PORT(unit) ? (ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID_IN_LIMITED_PORT+1):(ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID_MODE0+1))

#define ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(local_port,tunnel_id) ((local_port << ARAD_PORTS_FEM_PP_PORT_PON_CONST_SIZE) + tunnel_id)


#define ARAD_PORTS_SWAP_MAX_OFFSET_VAL      (7)


#define ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit,local_port,tunnel_id) (((tunnel_id) << ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)) + (local_port))

#define ARAD_PORTS_PON_PORT_NDX_MAX(unit)    (_BCM_PPD_NOF_PON_PHY_PORT(unit) - 1)
#define ARAD_PORTS_PON_TUNNEL_ID_MAX         (4095)
#define ARAD_PORTS_PON_IN_PP_NDX_MAX         (127)


#define ARAD_PORTS_VIRTUAL_PORT_COE_ENCODED(is_custom_pp_enabled,local_port,vid) \
((is_custom_pp_enabled)? ((1 << 13)+((local_port) << 6)+ (vid)) : (((local_port) << 6)+ (vid)))



#define ARAD_PORTS_XGS_TM_SYS_PORT_ENCODED(system_port) ((system_port & 0x7F) + (((system_port >> 7) & 0x1FF) << 8))



  
#define ARAD_PORT_IS_XGS_MAC_EXT_PORT(local_port,is_xgs_mac_ext) \
    { \
      uint32 ___flags, ___is_valid; \
      SOCDNX_IF_ERR_RETURN(soc_port_sw_db_is_valid_port_get(unit, local_port, &___is_valid)); \
      if(___is_valid) { \
        SOCDNX_IF_ERR_RETURN(soc_port_sw_db_flags_get(unit, local_port, &___flags)); \
        is_xgs_mac_ext = SOC_PORT_IS_XGS_MAC_EXT_INTERFACE(___flags); \
      } else {\
        is_xgs_mac_ext = 0; \
      }\
    }


#define ARAD_PORTS_VIRTUAL_PORT_XGS_MAC_EXT_ENCODED(ptc_port,hg_port) (((ptc_port & 0xFF) << ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE) + hg_port)
  
 
#define ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE(hg_modid,hg_port) ((hg_modid << 8) + hg_port)

#define ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE_MODID_GET(val) (val >> 8)
#define ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE_PORT_GET(val) (val & 0xFF)



#define ARAD_PORT_PCT_TYPE_CPU       (0x0)
#define ARAD_PORT_PCT_TYPE_RAW       (0x1)
#define ARAD_PORT_PCT_TYPE_TM        (0x2)
#define ARAD_PORT_PCT_TYPE_ETH       (0x3)









typedef struct
{
  
  uint32 ptc_port;

  
  uint32 hg_remote_port;
  
  uint32 hg_remote_modid;

} ARAD_PORTS_XGS_MAC_EXT_INFO;





STATIC uint32 
arad_ports_multicast_id_offset_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core_id, 
    SOC_SAND_IN  uint32       port_ndx,
    SOC_SAND_IN  uint32       multicast_offset
    ) ;

static uint32 is_pon_dpp_support=0;


uint16 arad_virtual_port_xgs_mac_ext_encode(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint16 ptc_port,
    SOC_SAND_IN uint16 hg_port)
{
    if (SOC_IS_JERICHO(unit)) {
        return (((ptc_port & 0xFF) << ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE) + hg_port);
    } else {
        return (((ptc_port & 0x7F) << ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE) + hg_port);
    }
}


uint32
  arad_ports_fap_and_nif_type_match_verify(
    SOC_SAND_IN ARAD_INTERFACE_ID if_id,
    SOC_SAND_IN ARAD_FAP_PORT_ID  fap_port_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_PORTS_FAP_AND_NIF_TYPE_MATCH_VERIFY);

  if (ARAD_IS_OLP_IF_ID(if_id))
  {
    if (!(ARAD_IS_OLP_FAP_PORT_ID(fap_port_id)))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 20, exit);
    }
  }
  else if (ARAD_IS_OAMP_IF_ID(if_id))
  {
    if (!(ARAD_IS_OAMP_FAP_PORT_ID(fap_port_id)))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 30, exit);
    }
  }
  else if(!ARAD_IS_NIF_ID(if_id) && !ARAD_IS_RCY_IF_ID(if_id) && !ARAD_IS_CPU_IF_ID(if_id) && !ARAD_IS_NONE_IF_ID(if_id))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_fap_and_nif_type_match_verify()",0,0);
}



STATIC uint32 
  _arad_ports_is_smooth_division_used(SOC_SAND_IN  int unit)
{
  return SOC_DPP_CONFIG(unit)->arad->init.ports.lag_mode == SOC_TMC_PORT_LAG_MODE_1K_16;
}


STATIC uint32
  arad_ports_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;  
  uint32
    data,
    nrdy_th,
    ifc,
    map_ps_to_ifc;    
  ARAD_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_REGS_INIT);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, BAD_REASSEMBLY_CONTEXT_VALIDf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, BAD_REASSEMBLY_CONTEXTf,  ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OLP_PORT_TERMINATION_CONTEXTf,  ARAD_PORTS_IF_UNMAPPED_INDICATION));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OLP_REASSEMBLY_CONTEXTf,  ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED));  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OAMP_PORT_TERMINATION_CONTEXTf,  ARAD_PORTS_IF_UNMAPPED_INDICATION));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OAMP_REASSEMBLY_CONTEXTf,  ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED));  

  
  

  data = 0;

  nif_ctxt_map_data.port_termination_context = ARAD_PORTS_IF_UNMAPPED_INDICATION;
  nif_ctxt_map_data.reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED;
  soc_IRE_NIF_CTXT_MAPm_field32_set(unit, &data, REASSEMBLY_CONTEXTf, nif_ctxt_map_data.reassembly_context);
  soc_IRE_NIF_CTXT_MAPm_field32_set(unit, &data, PORT_TERMINATION_CONTEXTf, nif_ctxt_map_data.port_termination_context);

  
  res = arad_fill_table_with_entry(unit, IRE_NIF_CTXT_MAPm, MEM_BLOCK_ANY, &data);
  
  
  for (ifc = 0; ifc < ARAD_EGQ_NOF_IFCS; ifc++)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_MAP_PS_TO_IFCm(unit, MEM_BLOCK_ANY, ifc, &data));    
    map_ps_to_ifc = 0x0;
    soc_EGQ_MAP_PS_TO_IFCm_field32_set(unit, &data, MAP_PS_TO_IFCf, map_ps_to_ifc);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_EGQ_MAP_PS_TO_IFCm(unit, MEM_BLOCK_ANY, ifc, &data));
  }  

   

  
  
  data = 0;  
  nrdy_th = 17;  
  soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_0f, nrdy_th);
  nrdy_th = 33;  
  soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_1f, nrdy_th);
  nrdy_th = 65;  
  soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_2f, nrdy_th);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, WRITE_EGQ_NRDY_TH_0_2r(unit, REG_PORT_ANY, data));

  
  data = 0;
  nrdy_th = 129; 
  soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_3f, nrdy_th);
  nrdy_th = 129; 
  soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_4f, nrdy_th);
  nrdy_th = 6;   
  soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_5f, nrdy_th);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1520, exit, WRITE_EGQ_NRDY_TH_3_5r(unit, REG_PORT_ANY, data));
  
  
  data = 0;
  nrdy_th = 52;  
  soc_reg_field_set(unit, EGQ_NRDY_TH_6_7r, &data, NRDY_TH_6f, nrdy_th);
  nrdy_th = 193;
  soc_reg_field_set(unit, EGQ_NRDY_TH_6_7r, &data, NRDY_TH_7f, nrdy_th);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1540, exit, WRITE_EGQ_NRDY_TH_6_7r(unit, data));

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_regs_init()",0,0);
}

uint32
  arad_ports_init_interfaces_erp_setting(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN ARAD_INIT_PORTS   *info
  )
{
  uint32
    res,
    free_nif_interface,
    if_id_internal,
    is_master,
    phy_port,
    cmic_count;
  uint8
    erp_exist;
  ARAD_EGQ_PPCT_TBL_DATA
    ppct_data;
  soc_pbmp_t
    phy_ports_in_use,
    port_bm;
  soc_port_t
    port_i;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE 
      ilkn_tdm_dedicated_queuing;
  soc_port_if_t
      interface;
  soc_pbmp_t
    pbmp;
  uint32    base_q_pair;
  int       erp_lane, core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {      
      
      erp_exist = FALSE;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));
      SOC_PBMP_ITER(pbmp,port_i)
      {
          if (port_i == SOC_INFO(unit).erp_port[core])
          {
              erp_exist = TRUE;
              break;
          }
      }
      
      if (erp_exist) {
          free_nif_interface = ARAD_IF_ID_NONE;

          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              
              ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
              SOCDNX_IF_ERR_RETURN(soc_arad_str_prop_parse_ucode_port_erp_lane(unit, &erp_lane));

              
              if (erp_lane > 0 && erp_lane <= ARAD_EGQ_NOF_IFCS) 
              {
                  free_nif_interface = erp_lane;
              }
              else
              
              {
                  SOC_PBMP_CLEAR(phy_ports_in_use);
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 800, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));
                  SOC_PBMP_ITER(port_bm, port_i){
                      SOC_SAND_SOC_IF_ERROR_RETURN(res, 810, exit, soc_port_sw_db_is_master_get(unit, port_i, &is_master));
                      if(!is_master){
                          continue;
                      }

                      SOC_SAND_SOC_IF_ERROR_RETURN(res, 820,  exit, soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port ));
                      SOC_PBMP_PORT_ADD(phy_ports_in_use, phy_port); 

                      if(ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
                          SOC_SAND_SOC_IF_ERROR_RETURN(res, 830,  exit, soc_port_sw_db_interface_type_get(unit, port_i, &interface));
                          if(interface == SOC_PORT_IF_ILKN) {
                              SOC_PBMP_PORT_ADD(phy_ports_in_use, phy_port+1); 
                          }
                      }

                  }

                  
                  for (if_id_internal = 0; if_id_internal < ARAD_EGQ_NOF_NIFS_IFCS; ++if_id_internal)
                  {
                      if(!SOC_PBMP_MEMBER(phy_ports_in_use, if_id_internal+1)) {
                          free_nif_interface = if_id_internal;
                          break;
                      }
                  }

                  
                  
                  if (free_nif_interface == ARAD_IF_ID_NONE && !(NUM_OLP_PORT(unit) > 0)) 
                  {
                      free_nif_interface = ARAD_EGQ_IFC_OLP;
                  }

                  if (free_nif_interface == ARAD_IF_ID_NONE && !(NUM_OAMP_PORT(unit) > 0)) {
                      free_nif_interface = ARAD_EGQ_IFC_OAMP;
                  }

                  if (free_nif_interface == ARAD_IF_ID_NONE && !(NUM_RCY_PORT(unit) > 0)) {
                      free_nif_interface = ARAD_EGQ_IFC_RCY;
                  }
                  
                  if (free_nif_interface == ARAD_IF_ID_NONE) {
                      cmic_count = 0;
                      SOC_PBMP_COUNT(SOC_INFO(unit).cmic_bitmap,cmic_count);
                      if (cmic_count <= 0) {
                          free_nif_interface = ARAD_EGQ_IFC_CPU;
                      }
                  }

                  if (free_nif_interface == ARAD_IF_ID_NONE) 
                  {
                    SOC_SAND_SET_ERROR_CODE(ARAD_PORT_UNOCCUPIED_INTERFACE_FOR_ERP_ERR, 40, exit);
                  }
                       
              }
              
              if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                  
                  if (free_nif_interface == ARAD_EGQ_IFC_DEF_VAL) {

                    base_q_pair = ARAD_FAP_EGRESS_REPLICATION_BASE_Q_PAIR;
                    res = arad_egq_ppct_tbl_get_unsafe(
                        unit,
                        core,
                        base_q_pair,
                        &(ppct_data)
                      );
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

                    ppct_data.cgm_interface = 1;

                    res = arad_egq_ppct_tbl_set_unsafe( 
                        unit,
                        core,
                        base_q_pair,
                        &(ppct_data)
                      );
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 155, exit);
                  }
              }
          } else { 
              res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port_i, &free_nif_interface);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 156, exit);
          }

          res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.set(unit, core, free_nif_interface);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 157, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_init_interfaces_erp_setting()",0,0);
}


int
  arad_ports_fap_port_id_cud_extension_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID   port_id
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) {
        if(port_id > ARAD_MAX_FAP_PORT_ID_EGR_MC_17BIT_CUD) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_FAP_PORT_ID_INVALID_ERR")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_ports_init_interfaces_context_map(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN ARAD_INIT_PORTS   *info
  )
{
  uint32
    res,
    if_id_internal,    
    data,
    ctxt_map_base,
    base_ctxt_mapping = 0,
    max_chan_num;
  uint32
    nof_channels_per_interface,
    is_master,
    first_phy_port;
  soc_pbmp_t
    port_bm;
  soc_port_t
    port_i;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_INIT_INTERFACES_CONTEXT_MAP);

  
  data = 0;
  res = arad_fill_table_with_entry(unit, IRE_NIF_PORT_MAPm, MEM_BLOCK_ANY, &data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 800, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));
  SOC_PBMP_ITER(port_bm, port_i){

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 900, exit, soc_port_sw_db_is_master_get(unit, port_i, &is_master));
      if(!is_master){
          continue;
      }

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, soc_port_sw_db_max_channel_num_get(unit, port_i, &max_chan_num));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, soc_port_sw_db_first_phy_port_get(unit, port_i, &first_phy_port));
      if_id_internal = first_phy_port-1;

       
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = base_ctxt_mapping;
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));    

      
      
      nof_channels_per_interface = max_chan_num + 1;
      base_ctxt_mapping += nof_channels_per_interface;

      
      SOC_SAND_ERR_IF_ABOVE_MAX(base_ctxt_mapping-1, ARAD_PORT_NIF_CTXT_MAP_MAX, ARAD_PORTS_NIF_CTXT_MAP_OUT_OF_RANGE_ERR, 30+if_id_internal, exit);      

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_init_interfaces_context_map()",0,0);
}

uint32
  arad_ports_init_interfaces_dynamic_context_map(
    SOC_SAND_IN int         unit
  )
{
  uint32
    res,
    if_id_internal,    
    data,
    ctxt_map_base;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_INIT_INTERFACES_CONTEXT_MAP);

  
  data = 0;
  res = arad_fill_table_with_entry(unit, IRE_NIF_PORT_MAPm, MEM_BLOCK_ANY, &data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  for (if_id_internal = SOC_TMC_IF_ID_0; if_id_internal < SOC_TMC_IF_ID_8; if_id_internal++) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = if_id_internal;
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));
  }

  
  for (if_id_internal = SOC_TMC_IF_ID_8; if_id_internal < SOC_TMC_IF_ID_12; if_id_internal++) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = 250 + (if_id_internal - SOC_TMC_IF_ID_8);
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));
  }

  
  for (if_id_internal = SOC_TMC_IF_ID_12; if_id_internal < SOC_TMC_IF_ID_16; if_id_internal++) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = 508 + (if_id_internal - SOC_TMC_IF_ID_12);
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));
  }

  
  for (if_id_internal = SOC_TMC_IF_ID_16; if_id_internal < SOC_TMC_IF_ID_24; if_id_internal++) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = 258 + (if_id_internal - SOC_TMC_IF_ID_16);
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));
  }
  
  
  for (if_id_internal = SOC_TMC_IF_ID_24; if_id_internal < SOC_TMC_IF_ID_28; if_id_internal++) 
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));      
      ctxt_map_base = 254 + (if_id_internal - SOC_TMC_IF_ID_24);
      soc_IRE_NIF_PORT_MAPm_field32_set(unit, &data, CTXT_MAP_BASE_ADDRESSf, ctxt_map_base);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));
  }   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_init_interfaces_context_map()",0,0);
}

uint32
  arad_ports_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      virtual_port,
      loop_ndx,
      max_tunnel_id,
    tunnel_id,
    reassembly_context,
      tbl_data_32b,
    tbl_data_64b[2] = {0},
      fld_val,
    fem_bit_ndx,
    fem_map_ndx,
    fem_pgm_dflt;
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA
    parser_program_pointer_fem_bit_select_table;
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA
    parser_program_pointer_fem_map_index_table;
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA
    parser_program_pointer_fem_field_select_map;
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
    src_system_port_fem_bit_select_table;
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
    src_system_port_fem_map_index_table;
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
    src_system_port_fem_field_select_map;
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
    virtual_port_fem_bit_select_table;
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
    virtual_port_fem_map_index_table;
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
    virtual_port_fem_field_select_map;  
  
  uint8
    pp_enable,
    oamp_enable;
  ARAD_EGQ_PCT_TBL_DATA 
    pct_tbl_data;
  soc_pbmp_t 
    pon_port_bm,
    vendor_pp_port_bm,
    coe_port_bm;
  soc_port_t
      port,
      local_port;
  uint32
    base_q_pair,
    nof_pairs,
    curr_q_pair;
  soc_pbmp_t 
    pbmp;
  ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_port2sys_data;
  soc_field_t
      pfq_0_fem_field_select[ARAD_PORTS_FEM_PFQ_0_SIZE] = 
        {PFQ_0_FEM_FIELD_SELECT_0f, PFQ_0_FEM_FIELD_SELECT_1f, PFQ_0_FEM_FIELD_SELECT_2f};
      
  uint32 is_valid, flags, offset;
  uint32 multicast_offset, port_i, tm_port, pp_port;
  int core = SOC_CORE_INVALID;
  uint32 if_pon_pp_port_map_bypass;
  uint32 pon_tunnel_id_alloc_mode;
  char *propval;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_INIT);

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }

  SOC_PBMP_CLEAR(pon_port_bm);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));

  SOC_PBMP_CLEAR(vendor_pp_port_bm);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 6, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_VENDOR_PP_PORT, &vendor_pp_port_bm));

  SOC_PBMP_CLEAR(coe_port_bm);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 7, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_COE_PORT, &coe_port_bm));

  
  if (!SOC_IS_JERICHO(unit)) {

      res = arad_ports_regs_init(
              unit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);  


      
      for (local_port = 0; local_port < ARAD_PORTS_REASSEMBLY_CONTEXT_LAST; ++local_port)
      {
        
        reassembly_context = local_port;
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.set(unit, local_port, reassembly_context);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);    
      }

      reassembly_context = ARAD_PORTS_NOF_REASSEMBLY_CONTEXT-1;
      for (local_port = ARAD_PORTS_REASSEMBLY_CONTEXT_LAST ; local_port < ARAD_NOF_FAP_PORTS; ++local_port)
      {
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.set(unit,local_port,reassembly_context);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }

      
      pp_enable = SOC_DPP_PP_ENABLE(unit);
      if (pp_enable && (SOC_INFO(unit).olp_port[0]>=0))
      {
        reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_OLP;
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.set(unit,ARAD_OLP_PORT_ID,reassembly_context);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }
      
      
      oamp_enable = SOC_DPP_CONFIG(unit)->pp.oamp_enable;
      if (oamp_enable)
      {
        reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_OAMP;
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.set(unit,ARAD_OAMP_PORT_ID,reassembly_context);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }

      
      if (SOC_INFO(unit).erp_port[0]>=0)
      {
        reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_ERP;
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.set(unit,ARAD_ERP_PORT_ID,reassembly_context);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      }
  }
  else {
      

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, BAD_REASSEMBLY_CONTEXT_VALIDf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, BAD_REASSEMBLY_CONTEXTf,  ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED));

      
  fld_val = 0;  
  soc_reg_field_set(unit, IRE_CPU_CHANNEL_CONFIGURATIONr, &fld_val, CPU_CHANNEL_SELECTf, 2);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, WRITE_IRE_CPU_CHANNEL_CONFIGURATIONr(unit, fld_val));
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

  SOC_PBMP_ITER(pbmp, port_i) {
      res = soc_port_sw_db_flags_get(unit, port_i, &flags);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100 ,exit);
      if (!SOC_PORT_IS_ELK_INTERFACE(flags)) {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 97, exit, soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
          res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.multicast_offset.get(unit, port_i, &multicast_offset);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100 ,exit);
          res = arad_ports_multicast_id_offset_set(unit, core, tm_port , multicast_offset);
          SOC_SAND_CHECK_FUNC_RESULT(res, 100 ,exit);
      }
  }

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      for (pp_port = 0; pp_port < MAX_PORTS_IN_CORE ; ++pp_port)
      {
          res = soc_port_sw_db_pp_is_valid_get(unit, core, pp_port, &is_valid);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

          if (is_valid) {
              res = soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          } else {
              port = pp_port;
          }

          res = READ_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

          
          if (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && PBMP_MEMBER(vendor_pp_port_bm,port)) {
              fld_val = 0;
          } else {
              fld_val = 0x1; 
          }
          soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUE_TO_USEf, fld_val);

          
          fld_val = ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR;

          
          if (PBMP_MEMBER(pon_port_bm,port)) {
              fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_PON;
          }

          
          if (PBMP_MEMBER(vendor_pp_port_bm,port)) {
              fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_VENDOR_PP;
          }

          
          if ((SOC_DPP_CONFIG(unit)->pp.port_extender_map_lc_exists == 2) && PBMP_MEMBER(coe_port_bm,port)) {
              fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_COE;
          }

          soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, PROFILEf, fld_val);

          
          if (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && PBMP_MEMBER(vendor_pp_port_bm,port)) {
              fld_val = ARAD_PORTS_FEM_PP_PORT_VENDOR2_HEADER_1_OFFSET;
          } else {
              fld_val = pp_port;
          }
          soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUEf, fld_val);

          
          fld_val = 0;

          
          if (PBMP_MEMBER(pon_port_bm,port)) {
              
              if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0))) {
                  fld_val = ARAD_PORTS_FEM_PON_PP_PORT_OFFSET;
              }
          }
          
          
          if (PBMP_MEMBER(vendor_pp_port_bm,port)) {
              fld_val = SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()?ARAD_PORTS_FEM_PP_PORT_VENDOR2_HEADER_0_OFFSET:ARAD_PORTS_FEM_VENDOR_CUSTOM_PP_PORT_OFFSET;
          }
          
          
          if ((SOC_DPP_CONFIG(unit)->pp.port_extender_map_lc_exists == 2) && PBMP_MEMBER(coe_port_bm,port)) {
              fld_val = ARAD_PORTS_FEM_COE_PORT_OFFSET;
          }
          
          soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, OFFSETf, fld_val);
          
          res = WRITE_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 27, exit);

          if(is_valid) {
              res = soc_port_sw_db_flags_get(unit, port, &flags);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101, exit);

              
              
              if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))) {
                  res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core, pp_port, &base_q_pair);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 29, exit);
                  res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core, pp_port, &nof_pairs);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

                  
                  for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) 
                  {
                      res = arad_egq_pct_tbl_get_unsafe(unit, core, curr_q_pair, &pct_tbl_data);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

                      pct_tbl_data.prog_editor_profile  = 5; 

                      
                      if (PBMP_MEMBER(coe_port_bm,port)) {
                          pct_tbl_data.prog_editor_value = ARAD_EGR_PROG_EDITOR_PORT_EXTENDER_HEADER_SIZE; 
                          
                      }
                      res = arad_egq_pct_tbl_set_unsafe(unit, core, curr_q_pair, &pct_tbl_data);
                      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
                  }
              }
          }
      }
  }
  
  
    
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {    
      for (loop_ndx = 0; loop_ndx < 2; ++loop_ndx)
      {
      
      if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable) && (loop_ndx == 1)) {
          continue;
      }
  
      for (pp_port = 0; pp_port < MAX_PORTS_IN_CORE ; ++pp_port)
      {
          virtual_port = (loop_ndx == 1)? ARAD_PORTS_VIRTUAL_PORT_INJECTED_TM_ENCODED(pp_port): ARAD_PORTS_VIRTUAL_PORT_ENCODED(pp_port);
          res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), virtual_port, &tbl_data_32b);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

          res = soc_port_sw_db_pp_is_valid_get(unit, core, pp_port, &is_valid);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
          
          
          fld_val = 0;
          if (is_valid) {
              fld_val = pp_port;
          }

          soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, fld_val);

          
          if (is_valid) {
              res = soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &local_port);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 229, exit);
              res = soc_port_sw_db_local_to_tm_port_get(unit, local_port, &tm_port, &core);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
              fld_val = tm_port;
          } else {
              fld_val = pp_port;
          }

          soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, fld_val);

          res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), virtual_port, &tbl_data_32b);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 27, exit);
      }
      }
  }

  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core){
      for (pp_port = 0; pp_port < MAX_PORTS_IN_CORE; ++pp_port)
      {

          res = soc_port_sw_db_pp_is_valid_get(unit, core, pp_port, &is_valid);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

          if (is_valid) {
              res = soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
          } else {
              port = pp_port;
          }

          
          res = arad_ihp_tm_port_sys_port_config_tbl_get_unsafe(
                  unit,
                  core,
                  pp_port,
                  &ihp_port2sys_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
          ihp_port2sys_data.system_port_value_to_use = 0x2; 

          if (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && PBMP_MEMBER(vendor_pp_port_bm, port)) {
            ihp_port2sys_data.system_port_offset1 = 2;
            ihp_port2sys_data.system_port_value = 0;
            ihp_port2sys_data.system_port_profile = ARAD_PORTS_FEM_PROFILE_SRC_PORT_EXT;
          }
          res = arad_ihp_tm_port_sys_port_config_tbl_set_unsafe(
                  unit,
                  core,
                  pp_port,
                  &ihp_port2sys_data
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
      }
  }

  
  
  if_pon_pp_port_map_bypass = _BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit);
  pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);
  propval = soc_property_get_str(unit, spn_PON_APPLICATION_SUPPORT_ENABLED);
  if (propval) {      
      if (sal_strcmp(propval, "TRUE") == 0) {
          is_pon_dpp_support = 1;
      } else {
          is_pon_dpp_support = 0;
      }
  }

  if (if_pon_pp_port_map_bypass == 0) {
      PBMP_ITER(pon_port_bm, local_port) {
          if (pon_tunnel_id_alloc_mode == 0) {
              max_tunnel_id = ARAD_PORTS_VIRTUAL_PORT_PON_NOF_ENTRIES_MODE0(unit, local_port);
              for (tunnel_id = 0; tunnel_id < max_tunnel_id; tunnel_id++)
              {
                  res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, (ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(local_port,tunnel_id)), &tbl_data_32b);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

                  
                  fld_val = 0;
                  soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, fld_val);

                  
                  fld_val = local_port;
                  soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, fld_val);

                  res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, (ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(local_port, tunnel_id)), &tbl_data_32b);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 27, exit);
              }
          } else if (pon_tunnel_id_alloc_mode == 1) {
              for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID(unit); tunnel_id++) {
                  res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, (ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, local_port, tunnel_id)), &tbl_data_32b);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

                  
                  fld_val = 0;
                  soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, fld_val);

                  
                  fld_val = local_port;
                  soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, fld_val);
                  res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, (ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, local_port, tunnel_id)), &tbl_data_32b);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 27, exit);
              }
          }

      }
  }

  
  fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR;

  parser_program_pointer_fem_bit_select_table.parser_program_pointer_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &parser_program_pointer_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tbl_data_32b = 0;
  soc_IHP_PFQ_0_FEM_BIT_SELECT_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_BIT_SELECTf, ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT);
  res = WRITE_IHP_PFQ_0_FEM_BIT_SELECT_TABLEm(unit, MEM_BLOCK_ANY, fem_pgm_dflt, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);

  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
  res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &virtual_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_DFLT;
  parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_data = 0;
  src_system_port_fem_map_index_table.src_system_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_DFLT;
  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_DFLT;
  virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
  tbl_data_32b = 0;
  soc_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_MAP_DATAf, 0);
  soc_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_MAP_INDEXf, ARAD_PORTS_FEM_ACTION_ID_DFLT);

  for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    
    res = arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_set_unsafe(
            unit,
            ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &parser_program_pointer_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
    res = arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR,
            fem_map_ndx,
            &virtual_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

    res = WRITE_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm(unit, MEM_BLOCK_ANY, 
               fem_map_ndx + (ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS), &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);
  }

  
  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE; ++fem_bit_ndx)
  {
    parser_program_pointer_fem_field_select_map.parser_program_pointer_fem_field_select[fem_bit_ndx] = fem_bit_ndx;
  }
  res = arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_set_unsafe(
          unit,
          ARAD_PORTS_FEM_ACTION_ID_DFLT,
          &parser_program_pointer_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tbl_data_32b = 0;
  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_PFQ_0_SIZE; ++fem_bit_ndx)
  {
    soc_IHP_PFQ_0_FEM_FIELD_SELECT_MAPm_field32_set(unit, &tbl_data_32b, pfq_0_fem_field_select[fem_bit_ndx], fem_bit_ndx);
  }
  res = WRITE_IHP_PFQ_0_FEM_FIELD_SELECT_MAPm(unit, MEM_BLOCK_ANY, ARAD_PORTS_FEM_ACTION_ID_DFLT, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_SYSTEM_PORT_SIZE; ++fem_bit_ndx)
  {
    src_system_port_fem_field_select_map.src_system_port_fem_field_select[fem_bit_ndx] = fem_bit_ndx;
  }
  res = arad_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          ARAD_PORTS_FEM_ACTION_ID_DFLT,
          &src_system_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx)
  {
    
    virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
      (fem_bit_ndx < (ARAD_PORTS_FEM_PP_PORT_PTCH_SIZE))? fem_bit_ndx : ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT) + 1);
  }
  res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          ARAD_PORTS_FEM_ACTION_ID_DFLT,
          &virtual_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  
  fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_SRC_PORT_EXT;

  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = 31;
  res = arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    src_system_port_fem_map_index_table.src_system_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_SRC_PORT_EXT;
    res = arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
  }

  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_SYSTEM_PORT_SIZE; ++fem_bit_ndx)
  {
    src_system_port_fem_field_select_map.src_system_port_fem_field_select[fem_bit_ndx] = 16 + fem_bit_ndx;
  }
  res = arad_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
          unit,
          ARAD_PORTS_FEM_ACTION_ID_SRC_PORT_EXT,
          &src_system_port_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);

  
  fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1;

  parser_program_pointer_fem_bit_select_table.parser_program_pointer_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_PP_PORT_PROFILE_INJECTED;
  res = arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &parser_program_pointer_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  tbl_data_32b = 0;
  soc_IHP_PFQ_0_FEM_BIT_SELECT_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_BIT_SELECTf, ARAD_PORTS_FEM_BIT_SELECT_MSB_PP_PORT_PROFILE_INJECTED);
  res = WRITE_IHP_PFQ_0_FEM_BIT_SELECT_TABLEm(unit, MEM_BLOCK_ANY, fem_pgm_dflt, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);

  
  tbl_data_32b = 0;
  soc_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_MAP_DATAf, 0);
  soc_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm_field32_set(unit, &tbl_data_32b, PFQ_0_FEM_MAP_INDEXf, ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1);

  for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    if ((fem_map_ndx >> 3) == 0)
    {
      
      parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_data = ARAD_PARSER_PROG_TM_ADDR_START;
      parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1;
    }
    else 
    {
      
      parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_data = ARAD_PARSER_PROG_ETH_ADDR_START;
      parser_program_pointer_fem_map_index_table.parser_program_pointer_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_DFLT;
    }

    
    res = arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            fem_map_ndx,
            &parser_program_pointer_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

    res = WRITE_IHP_PFQ_0_FEM_MAP_INDEX_TABLEm(unit, MEM_BLOCK_ANY, 
               fem_map_ndx + (fem_pgm_dflt << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS), &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 83, exit);
  }

  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE; ++fem_bit_ndx)
  {
    
    parser_program_pointer_fem_field_select_map.parser_program_pointer_fem_field_select[fem_bit_ndx] = fem_bit_ndx; 
    SOC_SAND_SET_BIT(parser_program_pointer_fem_field_select_map.parser_program_pointer_fem_field_select[fem_bit_ndx], 0x1, ARAD_PMF_FEM_MAP_DATA_ENCODED_BIT);
  }
  res = arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_set_unsafe(
          unit,
          ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1,
          &parser_program_pointer_fem_field_select_map
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);

  tbl_data_32b = 0;
  for (fem_bit_ndx = 0; fem_bit_ndx < ARAD_PORTS_FEM_PFQ_0_SIZE; ++fem_bit_ndx)
  {
    soc_IHP_PFQ_0_FEM_FIELD_SELECT_MAPm_field32_set(unit, &tbl_data_32b, pfq_0_fem_field_select[fem_bit_ndx], fem_bit_ndx + 28);
  }
  res = WRITE_IHP_PFQ_0_FEM_FIELD_SELECT_MAPm(unit, MEM_BLOCK_ANY, ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);


  
  if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable || is_pon_dpp_support) {
    if (if_pon_pp_port_map_bypass == 0) {
        
        fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_PON;

        
        virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
        res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
                unit,
                fem_pgm_dflt,
                &virtual_port_fem_bit_select_table
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
        for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
        {
          virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_PON;
          res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
                  unit,
                  fem_pgm_dflt,
                  fem_map_ndx,
                  &virtual_port_fem_map_index_table
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
        }  

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
            for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx) {
               
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] =
                    (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)) ? (fem_bit_ndx) :
                    (fem_bit_ndx >= ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit) && 
                     fem_bit_ndx < (ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit) + ARAD_PORTS_FEM_PP_PORT_GPON_TID_PORT_SIZE)) ?  
                        (16 - ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit) + fem_bit_ndx) : 0x30;
	    }
        }
        else if (pon_tunnel_id_alloc_mode == 0) {
            for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx) {    
              
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                    (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_PON_CONST_SIZE) ? (16 + fem_bit_ndx) : (fem_bit_ndx - ARAD_PORTS_FEM_PP_PORT_PON_CONST_SIZE);
            }
        } else if (pon_tunnel_id_alloc_mode == 1) {
            for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx) {    
              
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                    (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)) ? 
                        (fem_bit_ndx) : ((SOC_IS_QAX(unit) ? 12 : 13) + fem_bit_ndx);            
            }
        } else {

        }

        res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
                unit,
                ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_PON,
                &virtual_port_fem_field_select_map
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 94, exit);
    }
    
  }
  else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port) {
      
      
      fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_VENDOR_PP;
      
      
      virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
      res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
              unit,
              fem_pgm_dflt,
              &virtual_port_fem_bit_select_table
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
      
      virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
      for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
      {
        virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_VENDOR_PP;
        res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
                unit,
                fem_pgm_dflt,
                fem_map_ndx,
                &virtual_port_fem_map_index_table
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);
      }  
      
      for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx)
      {    
        if (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()) {
            
            if (SOC_IS_JERICHO(unit) && (fem_bit_ndx > ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_MSB)) {
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT));
                continue;
            }

            virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                (fem_bit_ndx <= ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_MSB) ? (fem_bit_ndx+ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_FEM_LSB)
                : (fem_bit_ndx+ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_FEM_LSB-(ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_MSB+1));
        } else {
            
            if (SOC_IS_JERICHO(unit) && (fem_bit_ndx > 13) && (fem_bit_ndx < SOC_DPP_GET_JERICHO(VIRTUAL_PORT_NOF_BITS))) {
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT));
                continue;
            }

        
             virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_VENDOR_PP_PORT_SIZE) ? (16 + (fem_bit_ndx + 8)) : (16 + (fem_bit_ndx - 6));
        }
      }
      
      res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
              unit,
              ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_VENDOR_PP,
              &virtual_port_fem_field_select_map
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 97, exit);
      
  } else {
    
    
    fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_XGS_MAC_EXT;

    
    virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
    res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            &virtual_port_fem_bit_select_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
    for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
    {
      virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_XGS_MAC_EXT;
      res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
              unit,
              fem_pgm_dflt,
              fem_map_ndx,
              &virtual_port_fem_map_index_table
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
    }  

    for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx)
    {    
      
       virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
          (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE) ? (16 + fem_bit_ndx) : (fem_bit_ndx - ARAD_PORTS_FEM_PP_PORT_XGS_MAC_EXT_CONST_SIZE);
    }

    res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
            unit,
            ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_XGS_MAC_EXT,
            &virtual_port_fem_field_select_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 94, exit);    
    
  }

  
  if (SOC_DPP_CONFIG(unit)->pp.port_extender_map_lc_exists == 2) {
      
      fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_COE;
      
      
      virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
      res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
              unit,
              fem_pgm_dflt,
              &virtual_port_fem_bit_select_table
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);
      
      virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
      for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
      {
        virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_COE;
        res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
                unit,
                fem_pgm_dflt,
                fem_map_ndx,
                &virtual_port_fem_map_index_table
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 97, exit);
      }  
      
      for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx)
      {    
        
         if (SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port && !SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()) {
            virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_COE_PORT_SIZE) ? (16 + fem_bit_ndx) : (fem_bit_ndx - ARAD_PORTS_FEM_PP_PORT_COE_PORT_SIZE);
            if (fem_bit_ndx == (SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits)-1)) {
                virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT) + 1);
            }
         } else {
             virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
                 (fem_bit_ndx < ARAD_PORTS_FEM_PP_PORT_COE_PORT_SIZE) ? (16 + fem_bit_ndx) : (fem_bit_ndx - ARAD_PORTS_FEM_PP_PORT_COE_PORT_SIZE);
         }
      }
      
      res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
              unit,
              ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_COE,
              &virtual_port_fem_field_select_map
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 98, exit);
  }
  

  
  fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_SRC_PORT_INJECTED;

  src_system_port_fem_bit_select_table.src_system_port_fem_bit_select = 31;
  res = arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
          unit,
          fem_pgm_dflt,
          &src_system_port_fem_bit_select_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

  src_system_port_fem_map_index_table.src_system_port_fem_map_data = 0;
  src_system_port_fem_map_index_table.src_system_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_SRC_PORT_EXT;
  for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
  {
    res = arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            fem_map_ndx,
            &src_system_port_fem_map_index_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
  }
  
  
    fem_pgm_dflt = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1;
    virtual_port_fem_bit_select_table.virtual_port_fem_bit_select = ARAD_PORTS_FEM_BIT_SELECT_MSB_DFLT;
    res = arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
            unit,
            fem_pgm_dflt,
            &virtual_port_fem_bit_select_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    virtual_port_fem_map_index_table.virtual_port_fem_map_data = 0;
    for (fem_map_ndx = 0; fem_map_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++fem_map_ndx)
    {
      virtual_port_fem_map_index_table.virtual_port_fem_map_index = ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1;
      res = arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
              unit,
              fem_pgm_dflt,
              fem_map_ndx,
              &virtual_port_fem_map_index_table
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
    }  

    for (fem_bit_ndx = 0; fem_bit_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++fem_bit_ndx)
    {
      
      
      if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable || is_pon_dpp_support) {
          virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
              (fem_bit_ndx < (ARAD_PORTS_FEM_PP_PORT_PTCH_SIZE))? fem_bit_ndx: ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT) + 1);
      } else {
        
        virtual_port_fem_field_select_map.virtual_port_fem_field_select[fem_bit_ndx] = 
            (fem_bit_ndx < (ARAD_PORTS_FEM_PP_PORT_PTCH_SIZE))? fem_bit_ndx: 
            ((fem_bit_ndx == ARAD_PORTS_FEM_PP_PORT_PTCH_SIZE)? 31: ((ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_VAL << ARAD_PORTS_FEM_MAP_DATA_ENCODED_CONST_START_BIT) + 1));
      }

    }
    res = arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
            unit,
            ARAD_PORTS_FEM_ACTION_ID_PP_PORT_PROFILE_INJECTED_PTCH1,
            &virtual_port_fem_field_select_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 94, exit);

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        for (offset = 0; offset < SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES); offset++) {
            res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.set(unit, core, offset, ARAD_PORT_INVALID_RCY_PORT);
            SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);
        }
    }

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        for (offset = 0; offset < SOC_DPP_MAX_NOF_CHANNELS; offset++) {
            res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.set(unit, core, offset, ARAD_PORT_INVALID_EGQ_INTERFACE);
            SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_init()",0,0);
}


uint32
  arad_ports_lag_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_LAG_MODE        lag_mode
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_MODE_SET);

  
 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, LAG_MODEf,  lag_mode));

   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, SOC_CORE_ALL, 0, ENABLE_LAG_FILTER_MCf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_LAG_FILTER_ENABLEr, SOC_CORE_ALL, 0, ENABLE_LAG_FILTER_UCf,  0x0));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_mode_set_unsafe()",0,0);
}

uint32
  arad_ports_lag_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PORT_LAG_MODE        *lag_mode,
    SOC_SAND_OUT uint32                 *sys_lag_port_id_nof_bits
  )
{
  uint32
    res,
    fld_val=0;
  soc_reg_t reg = SOC_IS_JERICHO(unit) ? ECI_GLOBAL_GENERAL_CFG_3r : ECI_GLOBALFr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_MODE_GET);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, LAG_MODEf, &fld_val)); 

  
  *lag_mode = fld_val;

  
  *sys_lag_port_id_nof_bits = (10 - (*lag_mode));

 exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_mode_get_unsafe()",0,0);
}


uint32
  arad_ports_lag_nof_lag_groups_get_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res,
    sys_lag_port_id_nof_bits;
  ARAD_PORT_LAG_MODE        
    lag_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_NOF_LAG_GROUPS_GET_UNSAFE);

  res = arad_ports_lag_mode_get_unsafe(
          unit,
          &lag_mode,
          &sys_lag_port_id_nof_bits);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  return (1 << sys_lag_port_id_nof_bits);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_nof_lag_groups_get_unsafe()",0,0);
}

uint32
  arad_ports_lag_nof_lag_entries_get_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res,
    sys_lag_port_id_nof_bits;
  ARAD_PORT_LAG_MODE        
    lag_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_NOF_LAG_ENTRIES_GET_UNSAFE);

  res = arad_ports_lag_mode_get_unsafe(
          unit,
          &lag_mode,
          &sys_lag_port_id_nof_bits);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  return (1 << (14 - sys_lag_port_id_nof_bits));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_nof_lag_entries_get_unsafe()",0,0);
}


uint32
  arad_ports_logical_sys_id_build(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  )
{
  uint32
    res,
    sys_lag_port_id_nof_bits,
    constructed_val = 0;
  ARAD_REG_FIELD
    is_lag_fld,
    lag_id_res_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;
  ARAD_PORT_LAG_MODE        
    lag_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LOGICAL_SYS_ID_BUILD);

  
  is_lag_fld.lsb = 15;
  is_lag_fld.msb = 15;

  if (is_lag_not_phys)
  {
      res = arad_ports_lag_mode_get_unsafe(unit, &lag_mode, &sys_lag_port_id_nof_bits);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      lag_id_res_fld.lsb = 14;
      lag_id_res_fld.msb = 14;

      lag_member_id_fld.lsb = sys_lag_port_id_nof_bits;
      lag_member_id_fld.msb = 13;

      lag_id_fld.lsb = 0;
      lag_id_fld.msb = sys_lag_port_id_nof_bits - 1;

    constructed_val |= ARAD_FLD_IN_PLACE_OLD(0x1, is_lag_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(0x0, lag_id_res_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(lag_id, lag_id_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(lag_member_id, lag_member_id_fld);
  }
  else
  {
      phys_port_id_fld.lsb = 0;
      phys_port_id_fld.msb = 14;

    constructed_val |= ARAD_FLD_IN_PLACE_OLD(0x0, is_lag_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(sys_phys_port_id, phys_port_id_fld);
  }

  *sys_logic_port_id = constructed_val;

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_logical_sys_id_build()",0,0);
}


uint32
  arad_ports_logical_sys_id_parse(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32  sys_logic_port_id,
    SOC_SAND_OUT uint8 *is_lag_not_phys,
    SOC_SAND_OUT uint32  *lag_id,
    SOC_SAND_OUT uint32  *lag_member_id,
    SOC_SAND_OUT uint32  *sys_phys_port_id
  )
{
  uint32
    res = SOC_SAND_OK,
    sys_lag_port_id_nof_bits,
    parsed_val = sys_logic_port_id;
  ARAD_REG_FIELD
    is_lag_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;
  ARAD_PORT_LAG_MODE        
    lag_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LOGICAL_SYS_ID_PARSE);

  
  is_lag_fld.lsb = 15;
  is_lag_fld.msb = 15;

  *is_lag_not_phys = SOC_SAND_NUM2BOOL(ARAD_FLD_FROM_PLACE_OLD(parsed_val, is_lag_fld));
  if (*is_lag_not_phys)
  {    
      res = arad_ports_lag_mode_get_unsafe(
              unit,
              &lag_mode,
              &sys_lag_port_id_nof_bits);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      lag_member_id_fld.lsb = sys_lag_port_id_nof_bits;
      lag_member_id_fld.msb = 13;

      lag_id_fld.lsb = 0;
      lag_id_fld.msb = sys_lag_port_id_nof_bits - 1;

      
      *lag_id        = ARAD_FLD_FROM_PLACE_OLD(parsed_val, lag_id_fld);
      *lag_member_id = ARAD_FLD_FROM_PLACE_OLD(parsed_val, lag_member_id_fld);
  }
  else
  {
      phys_port_id_fld.lsb = 0;
      phys_port_id_fld.msb = 14;

    *sys_phys_port_id = ARAD_FLD_FROM_PLACE_OLD(parsed_val, phys_port_id_fld);
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_logical_sys_id_parse()",0,0);
}

STATIC int
  arad_ports_local_to_xgs_mac_extender_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       tm_port,
    SOC_SAND_IN  int                          core,
    SOC_SAND_OUT ARAD_PORTS_XGS_MAC_EXT_INFO  *xgs_mac_ext_info
  )
{
    uint32
        base_q_pair;
    ARAD_EGQ_PCT_TBL_DATA
        pct_data;
    soc_port_t
        port,
        master_port;
    int
        dummy = 0;
    soc_error_t
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, master_port, &xgs_mac_ext_info->ptc_port, &dummy));

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(
          unit,
          core,
          base_q_pair,
          &pct_data
        ));
      
    xgs_mac_ext_info->hg_remote_modid = ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE_MODID_GET(pct_data.prog_editor_value);
    xgs_mac_ext_info->hg_remote_port  = ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE_PORT_GET(pct_data.prog_editor_value);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  arad_ports_local_port_prge_var_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_port_t                   port,
    SOC_SAND_OUT uint32                       *prge_var
  )
{
    ARAD_PP_EPNI_PP_PCT_TBL_DATA
        pp_pct_tbl_data;
    int core;
    uint32 pp_port;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port,  &pp_pct_tbl_data));

    *prge_var = pp_pct_tbl_data.prge_var;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_port_src_sys_port_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core,
    SOC_SAND_IN  uint32                             tm_port_id,
    SOC_SAND_IN  uint32                             sys_port
  )
{
    uint32
        virtual_port,
        loop_ndx,
        tbl_data_32b = 0,
        tunnel_id = 0,
        prge_var = 0,
        entry_index = 0,
        res;
    uint8
        is_xgs_mac_ext;
    ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
        sys_port_config_tbl_data;
    ARAD_PORTS_XGS_MAC_EXT_INFO
        xgs_mac_ext_info;
    soc_port_t 
        master_port = 0,
        local_port;
    uint16 xgs_mac_ext_encode;
    int master_port_core;
    uint32 master_tm_port;
    uint32 pon_tunnel_id_alloc_mode;
    uint32 flags;
      
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SET_IN_SRC_SYS_PORT_SET_UNSAFE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port_id, &local_port));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_flags_get(unit, local_port, &flags));

    if (!(SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && (flags & SOC_PORT_FLAGS_VENDOR_PP_PORT))) { 
        res = arad_ihp_tm_port_sys_port_config_tbl_get_unsafe(unit, core, tm_port_id, &sys_port_config_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        sys_port_config_tbl_data.system_port_value = sys_port;
        res = arad_ihp_tm_port_sys_port_config_tbl_set_unsafe(unit, core, tm_port_id, &sys_port_config_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }

  
    
    
    for (loop_ndx = 0; loop_ndx < 2; ++loop_ndx)
    {
        if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable) && (loop_ndx == 1)) {
            continue;
        }

        virtual_port = (loop_ndx == 1)? ARAD_PORTS_VIRTUAL_PORT_INJECTED_TM_ENCODED(tm_port_id): ARAD_PORTS_VIRTUAL_PORT_ENCODED(tm_port_id);
        res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), virtual_port, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);
        
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
        res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), virtual_port, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);
    }

    pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);
    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit) == 0) {
        if (flags & SOC_PORT_FLAGS_PON_INTERFACE) {
            if (pon_tunnel_id_alloc_mode == 0) {
                for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_NOF_ENTRIES_MODE0(unit, local_port); tunnel_id++) {
                    res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(local_port,tunnel_id), &tbl_data_32b);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
                    soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
                    res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(local_port,tunnel_id), &tbl_data_32b);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 141, exit);
                }
            } else if (pon_tunnel_id_alloc_mode == 1) {
                
                for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID(unit); tunnel_id++) {
                    res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, local_port, tunnel_id), &tbl_data_32b);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 142, exit);
                    
                    soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
                    res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, local_port, tunnel_id), &tbl_data_32b);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 144, exit);
                }
            }
        }
    }

    ARAD_PORT_IS_XGS_MAC_EXT_PORT(local_port,is_xgs_mac_ext);
    if (is_xgs_mac_ext) 
    {     
        
        res = arad_ports_local_to_xgs_mac_extender_info_get(unit, tm_port_id, core, &xgs_mac_ext_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 720, exit);
        
        xgs_mac_ext_encode = arad_virtual_port_xgs_mac_ext_encode(unit, xgs_mac_ext_info.ptc_port, xgs_mac_ext_info.hg_remote_port);
        res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 142, exit);
        
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
        res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 144, exit);
    }

    if (!SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && (flags & SOC_PORT_FLAGS_VENDOR_PP_PORT)) {
        res = arad_ports_local_port_prge_var_get(unit, local_port, &prge_var);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 155, exit);

        res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), prge_var, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);
        
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
        res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), prge_var, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 170, exit);
    }

    if (flags & SOC_PORT_FLAGS_COE_PORT) {
        
        res = arad_ports_local_port_prge_var_get(unit, local_port, &prge_var);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 190, exit);

        
        prge_var = 0xfff & prge_var;
        
        res = soc_port_sw_db_master_channel_get(unit, local_port, &master_port); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 200, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 201, exit, soc_port_sw_db_local_to_tm_port_get(unit, master_port, &master_tm_port, &master_port_core));

        entry_index = ARAD_PORTS_VIRTUAL_PORT_COE_ENCODED(SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port, master_tm_port, prge_var);
        res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), entry_index, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 210, exit);
        
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
        res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), entry_index, &tbl_data_32b);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 220, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_src_sys_port_set_unsafe()",tm_port_id,sys_port);
}


uint32
  arad_port_src_sys_port_get_unsafe(
    SOC_SAND_IN     int                                 unit,
    SOC_SAND_IN     uint32                              core,
    SOC_SAND_IN     uint32                              tm_port_id,
    SOC_SAND_OUT    uint32                              *sys_port
  )
{
    uint32 res;
    ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA sys_port_config_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SET_IN_SRC_SYS_PORT_GET_UNSAFE);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, arad_ihp_tm_port_sys_port_config_tbl_get_unsafe( unit, core, tm_port_id, &sys_port_config_tbl_data));
    *sys_port = sys_port_config_tbl_data.system_port_value;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_src_sys_port_get_unsafe()",tm_port_id,*sys_port);

}


uint32
  arad_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;
  uint32
    sys_fap_id_self = 0,
    egq_sys_phys_port_ndx,
    mine_data[2],
    is_self_port,
    memlock = 0;
  ARAD_PORT_PP_PORT_INFO
    pp_port_info;
  ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA 
      dsp_tbl_data;
  ARAD_SYSPORT master_sysport;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  ARAD_PORT_PP_PORT_INFO_clear(&pp_port_info);

  ARAD_DEVICE_CHECK(unit, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

  res = arad_sw_db_modport2sysport_get(unit, mapped_fap_id, mapped_fap_port_id, &master_sysport);
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  
  if (mapped_fap_id >= sys_fap_id_self && mapped_fap_id < sys_fap_id_self + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)
  {
    uint32 core_id = mapped_fap_id - sys_fap_id_self;
    
    egq_sys_phys_port_ndx = sys_phys_port_ndx;

    if (SOC_DPP_CONFIG(unit)->arad->xgs_compatability_tm_system_port_encoding == SOC_DPP_XGS_TM_8_MODID_7_PORT) 
    {
      
      
      res = arad_port_pp_port_get_unsafe(
              unit, 
              core_id,
              mapped_fap_port_id, 
              &pp_port_info
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

      if (pp_port_info.header_type_out == ARAD_PORT_HEADER_TYPE_XGS_DiffServ ||
          pp_port_info.header_type_out == ARAD_PORT_HEADER_TYPE_XGS_HQoS ) 
      {
          
          egq_sys_phys_port_ndx = ARAD_PORTS_XGS_TM_SYS_PORT_ENCODED(sys_phys_port_ndx);          
      }
    }

    
    if ( (!ARAD_IS_HQOS_MAPPING_ENABLE(unit)) || (master_sysport == ARAD_SYS_PHYS_PORT_INVALID(unit)) ) {

        
        MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
        memlock = 1;

        res = arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id, mapped_fap_port_id,  &dsp_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1130, exit);

        dsp_tbl_data.dst_system_port = egq_sys_phys_port_ndx;

        res = arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id, mapped_fap_port_id, &dsp_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1140, exit);

        memlock = 0;
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);

        
        res = arad_port_src_sys_port_set_unsafe(unit, core_id, mapped_fap_port_id, sys_phys_port_ndx);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    is_self_port = TRUE;
  }
  else
  {
    is_self_port = FALSE;
  }

  if (!SOC_IS_ARDON(unit)) {
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm(unit,MEM_BLOCK_ALL,sys_phys_port_ndx/32,mine_data));
    SHR_BITCOPY_RANGE(mine_data,sys_phys_port_ndx%32,&is_self_port,0,1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 65, exit, WRITE_PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm(unit,MEM_BLOCK_ALL,sys_phys_port_ndx/32,mine_data));
  }

  if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)) { 
    ARAD_MOD_PORT_TBL_DATA ips_lookup_data;
    
    ips_lookup_data.dest_dev = mapped_fap_id;
    ips_lookup_data.dest_port = mapped_fap_port_id;

    res = arad_indirect_sysport_to_modport_tbl_set_unsafe(unit, sys_phys_port_ndx, &ips_lookup_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  

  res = arad_sw_db_sysport2modport_set(unit, sys_phys_port_ndx, mapped_fap_id, mapped_fap_port_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

  if (ARAD_IS_HQOS_MAPPING_ENABLE(unit)) {
      
      if (master_sysport == ARAD_SYS_PHYS_PORT_INVALID(unit)) {
          res = arad_sw_db_modport2sysport_set(unit, mapped_fap_id, mapped_fap_port_id, sys_phys_port_ndx);
          SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      }
  } else { 
      res = arad_sw_db_modport2sysport_set(unit, mapped_fap_id, mapped_fap_port_id, sys_phys_port_ndx);
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  if(memlock) {
      MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_set_unsafe()",0,0);
}


uint32
  arad_sys_virtual_port_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32 res;
  ARAD_SYSPORT master_sysport;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);
  


  if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)) { 
    ARAD_MOD_PORT_TBL_DATA ips_lookup_data;
    ips_lookup_data.dest_dev = mapped_fap_id;
    ips_lookup_data.dest_port = mapped_fap_port_id;
    res = arad_indirect_sysport_to_modport_tbl_set_unsafe(unit, sys_phys_port_ndx, &ips_lookup_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  } 

    

    
    res = arad_sw_db_modport2sysport_get(unit, mapped_fap_id, mapped_fap_port_id, &master_sysport);
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

    if ( (!ARAD_IS_HQOS_MAPPING_ENABLE(unit)) || (master_sysport == ARAD_SYS_PHYS_PORT_INVALID(unit)) ) {
        res = arad_sw_db_modport2sysport_set(unit, mapped_fap_id, mapped_fap_port_id, sys_phys_port_ndx);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    res = arad_sw_db_sysport2modport_set(unit, sys_phys_port_ndx, mapped_fap_id, mapped_fap_port_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_virtual_port_to_local_port_map_set_unsafe()",0,0);
}


int
  arad_sys_phys_to_local_port_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res,
    nof_ports;

  SOCDNX_INIT_FUNC_DEFS;
  
  nof_ports = (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) ? ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT : ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID);
  if (sys_phys_port_ndx >= nof_ports) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Parameter out of range, range is 0 - %d"), (nof_ports - 1)));
  }

  if (mapped_fap_id > ARAD_MAX_DEVICE_ID) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("The parameter 'mapped_fap_id' is out of range. \n\r The range is: 0 - 8.\n\r ")));
  }

  res = arad_fap_port_id_verify(unit, mapped_fap_port_id);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_ports_fap_port_id_cud_extension_verify(unit, mapped_fap_port_id);
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN
}


uint32
  arad_sys_phys_to_local_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id 
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_id);
  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_port_id);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_phys_port_ndx, ARAD_NOF_SYS_PHYS_PORTS_GET(unit) - 1,
    ARAD_SYSTEM_PHYSICAL_PORT_OUT_OF_RANGE_ERR, 10, exit
  );

  ARAD_DEVICE_CHECK(unit, exit);
  if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)) { 
    ARAD_MOD_PORT_TBL_DATA ips_lookup_data;
    

    res = arad_indirect_sysport_to_modport_tbl_get_unsafe(unit, sys_phys_port_ndx, &ips_lookup_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *mapped_fap_id  = (uint32)ips_lookup_data.dest_dev;
    *mapped_fap_port_id = (uint32)ips_lookup_data.dest_port;
  } else { 
    res = arad_sw_db_sysport2modport_get(unit, sys_phys_port_ndx, mapped_fap_id, mapped_fap_port_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_get_unsafe()",0,0);
}


uint32
  arad_local_to_sys_phys_port_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;
  ARAD_SYSPORT sysport;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

  ARAD_DEVICE_CHECK(unit, exit);

  res = arad_fap_port_id_verify(unit, fap_local_port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fap_ndx, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  res = arad_sw_db_modport2sysport_get(unit, fap_ndx, fap_local_port_ndx, &sysport);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  *sys_phys_port_id = sysport;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_local_to_sys_phys_port_map_get_unsafe()",0,0);
}



uint32
  arad_modport_to_sys_phys_port_map_get_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  fap_id,            
    SOC_SAND_IN  uint32  tm_port,           
    SOC_SAND_OUT uint32  *sys_phys_port_id  
  )
{
    uint32 res;
    ARAD_SYSPORT sysport;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);
    ARAD_DEVICE_CHECK(unit, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(fap_id, ARAD_MAX_DEVICE_ID, ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit);

    res = arad_sw_db_modport2sysport_get(unit, fap_id, tm_port, &sysport);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *sys_phys_port_id = sysport;
  
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_modport_to_sys_phys_port_map_get_unsafe()", fap_id, tm_port);
}



int
  arad_port_control_pcs_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_port_t      port,
    SOC_SAND_IN soc_dcmn_port_pcs_t   pcs
  )
{
  
    uint32
        inner_link,
        blk_id;  
    uint32 
        general_config,  
        rx_fec_config,
        tx_fec_config,
        bec_config,
        tx_ack_mask_period,
        encoding_type, 
        syndrom;
    uint64 
        async_fifo_config,
        slow_read_rate;
    ARAD_LINK_STATE
        link_state;
    ARAD_LINK_STATE_INFO
        link_state_info;
    int disable_fec=0;
    int set_high_latency=0;
    int disable_error_marking=0;
    int rv, link_ndx;
    int speed;
    SOCDNX_INIT_FUNC_DEFS;

    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
    inner_link = link_ndx % 4;

    
    rv = arad_link_on_off_get(unit, port, &link_state_info);
    SOCDNX_IF_ERR_EXIT(rv);

    link_state = link_state_info.on_off;

    if(link_state == ARAD_LINK_STATE_ON) {
        link_state_info.on_off = ARAD_LINK_STATE_OFF;
        rv = arad_link_on_off_set(unit, port, &link_state_info);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    switch(pcs)
    {
        case soc_dcmn_port_pcs_8_9_legacy_fec:
            encoding_type = SOC_DCMN_PORT_PCS_8_9_LEGACY_FEC_VAL;
            COMPILER_64_SET(slow_read_rate, 0, SOC_SAND_NUM2BOOL(TRUE));
            syndrom = SOC_SAND_NUM2BOOL(FALSE);
            tx_ack_mask_period = 8;
            set_high_latency=1;
            disable_error_marking=1;
            break;
        case soc_dcmn_port_pcs_8_10:
            encoding_type = SOC_DCMN_PORT_PCS_8_10_VAL;
            COMPILER_64_SET(slow_read_rate, 0, SOC_SAND_NUM2BOOL(TRUE));
            syndrom = SOC_SAND_NUM2BOOL(FALSE);
            tx_ack_mask_period = 8;
            set_high_latency=1;
            disable_error_marking=1;
            break;
        case soc_dcmn_port_pcs_64_66_fec:
            encoding_type = SOC_DCMN_PORT_PCS_64_66_FEC_VAL;
            COMPILER_64_SET(slow_read_rate, 0, SOC_SAND_NUM2BOOL(FALSE));
            syndrom = SOC_SAND_NUM2BOOL(FALSE);
            tx_ack_mask_period = 8;
            break;
        case soc_dcmn_port_pcs_64_66_bec:
            encoding_type = SOC_DCMN_PORT_PCS_64_66_BEC_VAL;
            COMPILER_64_SET(slow_read_rate, 0, SOC_SAND_NUM2BOOL(FALSE));
            syndrom = SOC_SAND_NUM2BOOL(TRUE);
            tx_ack_mask_period = 8;
            disable_error_marking=1;
            break;
        case soc_dcmn_port_pcs_64_66:
            encoding_type = SOC_DCMN_PORT_PCS_64_66_FEC_VAL;
            COMPILER_64_SET(slow_read_rate, 0, SOC_SAND_NUM2BOOL(FALSE));
            syndrom = SOC_SAND_NUM2BOOL(FALSE);
            tx_ack_mask_period = 8;
            disable_fec=1;
            set_high_latency=1;
            disable_error_marking=1;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG_STR("invalid pcs %d"), pcs));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_link, &general_config));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, blk_id, inner_link, &async_fifo_config));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, blk_id, inner_link, &rx_fec_config));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_TX_CONFIGURATIONr(unit, blk_id, inner_link, &tx_fec_config));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_BEC_CONFIGURATIONr(unit, blk_id, inner_link, &bec_config));

    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &general_config, FMAL_N_ENCODING_TYPEf, encoding_type);
    soc_reg64_field_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &async_fifo_config, FMAL_N_RX_SLOW_READ_RATEf, slow_read_rate);
    soc_reg_field_set(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, &rx_fec_config, FPS_N_RX_FEC_SYNDROM_SELf, syndrom);
    soc_reg_field_set(unit, FMAC_FPS_TX_CONFIGURATIONr, &tx_fec_config, FPS_N_TX_FEC_SYNDROM_SELf, syndrom);
    soc_reg_field_set(unit, FMAC_BEC_CONFIGURATIONr, &bec_config, BEC_N_TX_SAME_ACK_MASK_PERIODf, tx_ack_mask_period);
    soc_reg_field_set(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, &rx_fec_config, FPS_N_RX_FEC_FEC_ENf, disable_fec? 0:1);
    soc_reg_field_set(unit, FMAC_FPS_TX_CONFIGURATIONr, &tx_fec_config,FPS_N_TX_FEC_ENf, disable_fec? 0:1);
    if (set_high_latency) {
        soc_reg_field_set(unit,FMAC_FMAL_GENERAL_CONFIGURATIONr,&general_config,FMAL_N_LOW_LATENCY_LLFCf,0);
    }
    if (disable_error_marking) {
        soc_reg_field_set(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, &rx_fec_config,FPS_N_RX_FEC_ERR_MARK_ALLf,0);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_link, general_config));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, blk_id, inner_link, async_fifo_config));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, blk_id, inner_link, rx_fec_config));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_TX_CONFIGURATIONr(unit, blk_id, inner_link, tx_fec_config));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_BEC_CONFIGURATIONr(unit, blk_id, inner_link, bec_config));

    
    if ((link_ndx%SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac)) == 0) {
        SOCDNX_IF_ERR_EXIT(arad_fabric_force_signal_detect_set(unit, blk_id));
    }

    
    if(link_state == ARAD_LINK_STATE_ON) {
        link_state_info.on_off = ARAD_LINK_STATE_ON;
        SOCDNX_IF_ERR_EXIT(arad_link_on_off_set(unit, port, &link_state_info));
    }

    if(SOC_DPP_IS_MESH((unit))) {
        SOCDNX_IF_ERR_EXIT(arad_fabric_port_speed_get(unit, port, &speed));
        SOCDNX_IF_ERR_EXIT(arad_fabric_mesh_topology_values_config(unit, speed, pcs));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_port_control_pcs_set_verify(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32     link_ndx,
    SOC_SAND_IN soc_dcmn_port_pcs_t pcs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_CONTROL_PCS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
  link_ndx, SOC_DPP_DEFS_GET(unit, nof_fabric_links)-1,
  ARAD_PORT_CONTROL_PCS_LINK_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    pcs, _SHR_PORT_PCS_COUNT-1,
    ARAD_PORT_CONTROL_PCS_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_control_pcs_set_verify()",0,0);
}



soc_error_t
  arad_port_control_low_latency_set(
     SOC_SAND_IN int         unit,
     SOC_SAND_IN soc_port_t  port,
     SOC_SAND_IN int         value
     )
{
      uint32 blk_id,
             inner_link,
             reg_val;
      soc_dcmn_port_pcs_t pcs;
      int link_ndx, rv;

      SOCDNX_INIT_FUNC_DEFS;

      link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
      blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
      inner_link = link_ndx % 4;
  
      rv = arad_port_control_pcs_get(unit, port, &pcs);
      SOCDNX_IF_ERR_EXIT(rv);

      switch (pcs) 
      {
          case soc_dcmn_port_pcs_64_66_fec:
          case soc_dcmn_port_pcs_64_66_bec:
              break;

          case soc_dcmn_port_pcs_8_9_legacy_fec:
          case soc_dcmn_port_pcs_8_10:
          case soc_dcmn_port_pcs_64_66:
          default:
              SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Operation not supported for current PCS"))); 
              break;
      }

     SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_link, &reg_val)); 
     soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_LOW_LATENCY_LLFCf, value? 1:0);
     SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_link, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
    arad_port_control_fec_error_detect_set(
       SOC_SAND_IN int          unit,
       SOC_SAND_IN soc_port_t   port,
       SOC_SAND_IN int          value
     )
{
    uint32 blk_id,
        inner_link,
        reg_val;
    soc_dcmn_port_pcs_t pcs;
    int link_ndx, rv;

    SOCDNX_INIT_FUNC_DEFS;

    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
    inner_link = link_ndx % 4;
  
    rv = arad_port_control_pcs_get(unit,SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_ndx),&pcs);
    SOCDNX_IF_ERR_EXIT(rv);

    switch (pcs)
    {
        case soc_dcmn_port_pcs_64_66_fec:
            break;

        case soc_dcmn_port_pcs_8_9_legacy_fec:
        case soc_dcmn_port_pcs_8_10:
        case soc_dcmn_port_pcs_64_66_bec:
        case soc_dcmn_port_pcs_64_66:
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Operation not supported for current PCS"))); 
            break;
    }

     SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, blk_id, inner_link, &reg_val)); 
     soc_reg_field_set(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, &reg_val, FPS_N_RX_FEC_ERR_MARK_ALLf, value? 1:0);
     SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, blk_id, inner_link, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  arad_port_control_low_latency_get(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    port,
     SOC_SAND_OUT int*          value
     )
{
    uint32 blk_id,
         inner_link,
         reg_val;
    soc_dcmn_port_pcs_t pcs;
    int link_ndx, rv;
  
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_port_control_pcs_get(unit, port, &pcs);
    SOCDNX_IF_ERR_EXIT(rv);

    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
    inner_link = link_ndx % 4;

    if (! (pcs == soc_dcmn_port_pcs_64_66_fec || pcs == soc_dcmn_port_pcs_64_66_bec) ) {
        *value=0;
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Operation not supported for current PCS"))); 
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr (unit, blk_id, inner_link, &reg_val));
    *value=soc_reg_field_get(unit,FMAC_FMAL_GENERAL_CONFIGURATIONr,reg_val,FMAL_N_LOW_LATENCY_LLFCf);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  arad_port_control_fec_error_detect_get(
     SOC_SAND_IN  int           unit,
     SOC_SAND_IN  soc_port_t    port,
     SOC_SAND_OUT int*          value
     )
{
     uint32 blk_id,
         inner_link,
         reg_val;
     int rv, link_ndx;
     soc_dcmn_port_pcs_t pcs;
  
     SOCDNX_INIT_FUNC_DEFS;

     link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
     blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
     inner_link = link_ndx % 4;

     rv = arad_port_control_pcs_get(unit,SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_ndx),&pcs);
     SOCDNX_IF_ERR_EXIT(rv);

     if (! (pcs == soc_dcmn_port_pcs_64_66_fec) ) {
         *value=0;
         SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Operation not supported for current PCS"))); 
     }

     SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr (unit, blk_id, inner_link, &reg_val));
     *value=soc_reg_field_get(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,reg_val,FPS_N_RX_FEC_ERR_MARK_ALLf);

exit:
    SOCDNX_FUNC_RETURN;
}




int
  arad_port_control_pcs_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  soc_port_t         port,
    SOC_SAND_OUT soc_dcmn_port_pcs_t *pcs
  )
{
    uint32
        inner_link,
        blk_id; 
    uint32 
        general_config,   
        encoding_type,
        fmac_config,
        rx_fec_en;
    int link_ndx;
  
    SOCDNX_INIT_FUNC_DEFS;

    link_ndx = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    blk_id = SOC_SAND_DIV_ROUND_DOWN(link_ndx, 4);
    inner_link = link_ndx % 4;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk_id, inner_link, &general_config));
    encoding_type = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, general_config, FMAL_N_ENCODING_TYPEf);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, blk_id, inner_link,&fmac_config));
    rx_fec_en = soc_reg_field_get(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr,fmac_config,FPS_N_RX_FEC_FEC_ENf);

    switch(encoding_type)
    {
        case SOC_DCMN_PORT_PCS_8_9_LEGACY_FEC_VAL: 
            *pcs = soc_dcmn_port_pcs_8_9_legacy_fec;
            break;
        case SOC_DCMN_PORT_PCS_8_10_VAL: 
            *pcs = soc_dcmn_port_pcs_8_10;
            break;
        case SOC_DCMN_PORT_PCS_64_66_FEC_VAL:
            if (rx_fec_en) {
                *pcs = soc_dcmn_port_pcs_64_66_fec;
            } else {
                *pcs = soc_dcmn_port_pcs_64_66;
            }
            break;
        case SOC_DCMN_PORT_PCS_64_66_BEC_VAL: 
            *pcs = soc_dcmn_port_pcs_64_66_bec;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ENCODING_TYPE_OUT_OF_RANGE")));     
    }
 
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32 
  arad_ports_expected_chan_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  ARAD_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint8           is_channelized_interface,
    SOC_SAND_IN  ARAD_INTERFACE_ID   nif_id,
    SOC_SAND_IN  uint32           if_id_internal,
    SOC_SAND_OUT uint32           *channel,
    SOC_SAND_OUT uint32           *channel_step    
  )
{
  uint32
    ch_id = ARAD_PORTS_CH_UNKNOWN,
    ch_step = 1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_EXPECTED_CHAN_GET);

  switch(nif_type)
  {
  case ARAD_NIF_TYPE_RXAUI:
  case ARAD_NIF_TYPE_XAUI:
  case ARAD_NIF_TYPE_ILKN: 
  case ARAD_NIF_TYPE_10GBASE_R: 
  case ARAD_NIF_TYPE_100G_CGE:
  case ARAD_NIF_TYPE_40G_XLGE:
  case ARAD_NIF_TYPE_TM_INTERNAL_PKT:
    if (is_channelized_interface)
    {
      ch_id = ARAD_PORTS_CH_UNKNOWN;
    }
    else
    {
      ch_id = 0;
    }
    break;    
  case ARAD_NIF_TYPE_SGMII:
    if(if_id_internal == ARAD_NIF_INVALID_VAL_INTERN)
    {
      ch_id = ARAD_PORTS_CH_UNKNOWN;
    }
    else
    {
      ch_id = 0;
    }    
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_TYPE_ERR, 30, exit);
    break;
  }

  *channel = ch_id;
  *channel_step = ch_step;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_expected_chan_get()",0,0);
}


uint32
  arad_port_ingr_map_write_val_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  uint8            is_mapped,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO   *map_info
  )
{
  uint32
    offset = 0,
    data,
    reassebly_mapping_val = 0,
    port_mapping_val = 0,    
    to_map_val,
    to_iqm_itmpm = 0,
    to_iqm_itmpm_fld,
    ctxt_map_base,
    res,
    is_master,
    tm_port,
    nof_channels,
    flags;
  uint32
    skip_ctx_map = FALSE;
  ARAD_IRE_NIF_CTXT_MAP_TBL_DATA
    nif_ctxt_map_data;
  ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA
    nif2ctxt_data;
  ARAD_IRE_RCY_CTXT_MAP_TBL_DATA
    rcy_ctxt_map_data;
  ARAD_IRE_CPU_CTXT_MAP_TBL_DATA
    cpu_ctxt_map_data;
  ARAD_NIF_TYPE
    nif_type;
  uint8
    is_channelized_interface = FALSE;
  uint32
    if_id_internal;
  uint32
    reg_idx,
    fld_idx,
    wc_id,
    ch_id = map_info->channel_id,
    ch_step,
    reassembly_context;
  soc_port_t port, port_match_channel, next_master;
  int
    core=0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_INGR_MAP_WRITE_VAL);
  
    
  
  res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, port_ndx, &reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit,soc_port_sw_db_tm_to_local_port_get(unit, core, port_ndx, &port));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit,soc_port_sw_db_flags_get(unit, port, &flags));
  if (SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)) {
      skip_ctx_map = TRUE;
  }

  if (!is_mapped) 
  {
      
      if (map_info->if_id != SOC_TMC_IF_ID_CPU && map_info->if_id != SOC_TMC_IF_ID_OLP && 
          map_info->if_id != SOC_TMC_IF_ID_RCY && map_info->if_id != SOC_TMC_IF_ID_ERP && 
          map_info->if_id != SOC_TMC_IF_ID_OAMP) {
         
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,soc_port_sw_db_port_with_channel_get(unit, port, ch_id, &port_match_channel));

         if (port_match_channel == port_ndx || port_match_channel == SOC_MAX_NUM_PORTS) {
             port_mapping_val = ARAD_PORTS_IF_UNMAPPED_INDICATION;
             reassebly_mapping_val = ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED;
         } else {
             skip_ctx_map = TRUE;
         }
      } else {
          port_mapping_val = ARAD_PORTS_IF_UNMAPPED_INDICATION;
          reassebly_mapping_val = ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED;
      }
  }
  else
  {
      reassebly_mapping_val = reassembly_context;
      port_mapping_val = port_ndx;
  }
  
  if (ARAD_NIF_IS_ARAD_ID(map_info->if_id))
  {
    if_id_internal = arad_nif2intern_id(unit, map_info->if_id);

    nif_type = arad_nif_id2type(map_info->if_id);

    
    to_iqm_itmpm_fld = is_mapped;
    soc_mem_field_set(unit, IQM_ITMPMm, &to_iqm_itmpm, NIF_PORT_VALIDf, &to_iqm_itmpm_fld);
    to_iqm_itmpm_fld = is_mapped ? if_id_internal:0;
    soc_mem_field_set(unit, IQM_ITMPMm, &to_iqm_itmpm, NIF_PORTf, &to_iqm_itmpm_fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1610, exit, WRITE_IQM_ITMPMm(unit, MEM_BLOCK_ANY, reassembly_context, &to_iqm_itmpm));
    
    res = arad_nif_is_channelized(
                                 unit,
                                 port,
                                 &is_channelized_interface
                               );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(is_channelized_interface)
    {
      ch_id = map_info->channel_id;
    }
    else
    {
      res = arad_ports_expected_chan_get(
              unit,
              nif_type,
              is_channelized_interface,
              map_info->if_id,
              if_id_internal,
              &ch_id,
              &ch_step
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if(ch_id == ARAD_PORTS_CH_UNKNOWN)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_NON_CH_IF_ERR, 35, exit);
      }
    }

    
    wc_id = ARAD_NIF2WC_GLBL_ID(if_id_internal);

    res = arad_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
            unit,
            wc_id,
            &(nif2ctxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    reg_idx = ARAD_REG_IDX_GET(reassembly_context, SOC_SAND_REG_SIZE_BITS);
    fld_idx = ARAD_FLD_IDX_GET(reassembly_context, SOC_SAND_REG_SIZE_BITS);
    to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
    if(!is_channelized_interface)
    {
      to_map_val = 0x0; 
    }
    SOC_SAND_SET_BIT(nif2ctxt_data.contexts_bit_mapping[reg_idx], to_map_val, fld_idx);

    res = arad_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
            unit,
            wc_id,
            &(nif2ctxt_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    nif_ctxt_map_data.reassembly_context = reassebly_mapping_val;
    nif_ctxt_map_data.port_termination_context = port_mapping_val;

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));    
    ctxt_map_base = soc_IRE_NIF_PORT_MAPm_field32_get(unit, &data, CTXT_MAP_BASE_ADDRESSf);
    
    
    if (is_channelized_interface && !(nif_type == ARAD_NIF_TYPE_ILKN)) 
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, soc_port_sw_db_is_master_get(unit, port_ndx, &is_master));
        if(is_master){
            offset = ctxt_map_base; 
            if (!is_mapped) { 
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,soc_port_sw_db_tm_to_local_port_get(unit, core, port_ndx, &port));
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit,soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));
                if (nof_channels > 1) {
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 56, exit,soc_port_sw_db_next_master_get(unit, port, &next_master));
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 57, exit,soc_port_sw_db_local_to_tm_port_get(unit, next_master, &tm_port, &core));
                    res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, tm_port, &reassebly_mapping_val);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 57, exit);
                    port_mapping_val = tm_port;
                    
                    nif_ctxt_map_data.reassembly_context = reassebly_mapping_val;
                    nif_ctxt_map_data.port_termination_context = port_mapping_val;
                } else {
                    skip_ctx_map = TRUE;
                }
            }
        } else {
            skip_ctx_map = TRUE;
        }
    }
    else
    {
        offset = ctxt_map_base + ch_id;
        offset = offset % ARAD_PORT_NIF_CTXT_MAP_MAX;
    }

    if(!skip_ctx_map)
    {
        res = arad_ire_nif_ctxt_map_tbl_set_unsafe(
                unit,
                offset,
                &(nif_ctxt_map_data)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
  else if(ARAD_IS_CPU_IF_ID(map_info->if_id))
  {
    
    cpu_ctxt_map_data.port_termination_context = port_mapping_val;
    cpu_ctxt_map_data.reassembly_context = reassebly_mapping_val;
    offset = ch_id;

    res = arad_ire_cpu_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(cpu_ctxt_map_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);    
  }
  else if (ARAD_IS_OLP_IF_ID(map_info->if_id))
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OLP_PORT_TERMINATION_CONTEXTf,  port_mapping_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  82,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OLP_REASSEMBLY_CONTEXTf,  reassebly_mapping_val));
  }
  else if (ARAD_IS_OAMP_IF_ID(map_info->if_id))
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OAMP_PORT_TERMINATION_CONTEXTf,  port_mapping_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  82,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, REG_PORT_ANY, 0, OAMP_REASSEMBLY_CONTEXTf,  reassebly_mapping_val));
  }
  else if (ARAD_IS_RCY_IF_ID(map_info->if_id))
  {
        
    rcy_ctxt_map_data.port_termination_context = port_mapping_val;
    rcy_ctxt_map_data.reassembly_context = reassebly_mapping_val;
    offset = ch_id;

    res = arad_ire_rcy_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(rcy_ctxt_map_data)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }
  else if (ARAD_IS_ERP_IF_ID(map_info->if_id))
  {
    
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_TYPE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_ingr_map_write_val_unsafe()",port_ndx,map_info->if_id);
}


uint32
  arad_port_ingr_reassembly_context_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_port_t  port,
    SOC_SAND_OUT uint32     *port_termination_context,
    SOC_SAND_OUT uint32     *reassembly_context
  )
{
    uint32
        offset = 0,
        data,
        ctxt_map_base;
    ARAD_IRE_NIF_CTXT_MAP_TBL_DATA
        nif_ctxt_map_data;
    ARAD_IRE_RCY_CTXT_MAP_TBL_DATA
        rcy_ctxt_map_data;
    ARAD_IRE_CPU_CTXT_MAP_TBL_DATA
        cpu_ctxt_map_data;

    soc_port_if_t   nif_type;
    uint32          ch_id, reg_val;
    uint32          first_phy_port;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &nif_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &ch_id));

    if (nif_type == SOC_PORT_IF_ERP) {
        
        *port_termination_context = 0xffffffff;
        *reassembly_context = 0xffffffff;
    }
    else if (nif_type == SOC_PORT_IF_OLP)
    {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_FAP_PORT_CONFIGURATIONr(unit, &reg_val));
        *port_termination_context = soc_reg_field_get(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, reg_val, OLP_PORT_TERMINATION_CONTEXTf);
        *reassembly_context = soc_reg_field_get(unit, IRE_OLP_FAP_PORT_CONFIGURATIONr, reg_val, OLP_REASSEMBLY_CONTEXTf);
    }
    else if (nif_type == SOC_PORT_IF_OAMP)
    {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_FAP_PORT_CONFIGURATIONr(unit, &reg_val));
        *port_termination_context = soc_reg_field_get(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, reg_val, OAMP_PORT_TERMINATION_CONTEXTf);
        *reassembly_context = soc_reg_field_get(unit, IRE_OAMP_FAP_PORT_CONFIGURATIONr, reg_val, OAMP_REASSEMBLY_CONTEXTf);
    }
    else if (nif_type == SOC_PORT_IF_CPU)
    {
        offset = ch_id;

        SOCDNX_SAND_IF_ERR_EXIT(
            arad_ire_cpu_ctxt_map_tbl_get_unsafe(
                unit,
                offset,
                &(cpu_ctxt_map_data)
            )
        );

        *port_termination_context = cpu_ctxt_map_data.port_termination_context;
        *reassembly_context = cpu_ctxt_map_data.reassembly_context;
    }
    else if (nif_type == SOC_PORT_IF_RCY)
    {
      offset = ch_id;

      SOCDNX_SAND_IF_ERR_EXIT(
          arad_ire_rcy_ctxt_map_tbl_get_unsafe(
              unit,
              offset,
              &(rcy_ctxt_map_data)
          )
      );

      *port_termination_context = rcy_ctxt_map_data.port_termination_context;
      *reassembly_context = rcy_ctxt_map_data.reassembly_context;
    } else
    {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port ));
        --first_phy_port; 
        SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, first_phy_port, &data));
        ctxt_map_base = soc_IRE_NIF_PORT_MAPm_field32_get(unit, &data, CTXT_MAP_BASE_ADDRESSf);

        
        if (nif_type == SOC_PORT_IF_ILKN) {
            offset = ctxt_map_base + ch_id;
            offset = offset % ARAD_PORT_NIF_CTXT_MAP_MAX;
        } else {
            offset = ctxt_map_base;
            offset = offset % ARAD_PORT_NIF_CTXT_MAP_MAX;
        }

        SOCDNX_SAND_IF_ERR_EXIT(
            arad_ire_nif_ctxt_map_tbl_get_unsafe(
                unit,
                offset,
                &(nif_ctxt_map_data)
            )
        );

        *port_termination_context = nif_ctxt_map_data.port_termination_context;
        *reassembly_context = nif_ctxt_map_data.reassembly_context;
    }

exit:
    SOCDNX_FUNC_RETURN
}


STATIC uint32
    arad_port_egr_egq_nif_cancel_en_regs_set(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 if_rate_mbps,
        SOC_SAND_IN uint32 ifc
    )
{
    uint32
        res,
        reg_val,
        fld_val[1];
        
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_EGQ_NIF_CANCEL_EN_REGS_SET)
    if(ifc < ARAD_EGQ_NOF_NIFS_IFCS) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_EGQ_NIF_CANCEL_EN_REGISTER_1r(unit, &reg_val));
        *fld_val = soc_reg_field_get(unit, EGQ_NIF_CANCEL_EN_REGISTER_1r, reg_val, NIF_CANCEL_EN_1f);
    
        if(if_rate_mbps <= 48000) {
            SHR_BITSET(fld_val, ifc);
        } else {
            SHR_BITCLR(fld_val, ifc);
        }
        soc_reg_field_set(unit, EGQ_NIF_CANCEL_EN_REGISTER_1r, &reg_val, NIF_CANCEL_EN_1f, *fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_NIF_CANCEL_EN_REGISTER_1r(unit, reg_val));
    
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EGQ_NIF_CANCEL_EN_REGISTER_2r(unit, &reg_val));
        *fld_val = soc_reg_field_get(unit, EGQ_NIF_CANCEL_EN_REGISTER_2r, reg_val, NIF_CANCEL_EN_2f);
        if(if_rate_mbps <= 48000) {
            SHR_BITSET(fld_val, ifc);
        } else {
            SHR_BITCLR(fld_val, ifc);
        }
        soc_reg_field_set(unit, EGQ_NIF_CANCEL_EN_REGISTER_2r, &reg_val, NIF_CANCEL_EN_2f, *fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EGQ_NIF_CANCEL_EN_REGISTER_2r(unit, reg_val));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_EGQ_NIF_CANCEL_EN_REGISTER_3r(unit, &reg_val));
        *fld_val = soc_reg_field_get(unit, EGQ_NIF_CANCEL_EN_REGISTER_3r, reg_val, NIF_CANCEL_EN_3f);
        if(if_rate_mbps <= 23000) {
            SHR_BITSET(fld_val, ifc);
        } else {
            SHR_BITCLR(fld_val, ifc);
        }
        soc_reg_field_set(unit, EGQ_NIF_CANCEL_EN_REGISTER_3r, &reg_val, NIF_CANCEL_EN_3f, *fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EGQ_NIF_CANCEL_EN_REGISTER_3r(unit, reg_val));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_EGQ_NIF_CANCEL_EN_REGISTER_4r(unit, &reg_val));
        *fld_val = soc_reg_field_get(unit, EGQ_NIF_CANCEL_EN_REGISTER_4r, reg_val, NIF_CANCEL_EN_4f);
        SHR_BITCLR(fld_val, ifc);
        soc_reg_field_set(unit, EGQ_NIF_CANCEL_EN_REGISTER_4r, &reg_val, NIF_CANCEL_EN_4f, *fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_EGQ_NIF_CANCEL_EN_REGISTER_4r(unit, reg_val));
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_egq_nif_cancel_en_regs_set()",0,0);
}


STATIC uint32
  arad_port_egr_map_write_val(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  int            core_id,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  uint8            is_mapped,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO   *map_info
  )
{
  uint32
    to_map_val,
    txi_sel_val,
    freq_rdy_sel_val,
    res,
    fld_idx;
  soc_error_t
    rv;
  uint32
    offset,
    data,
    map_ps_to_ifc,
    nof_channels,
    tm_port_i,
    skip_ps_mapping;
  soc_reg_above_64_val_t th_data, th_field_val;
  uint32
    ch_id = map_info->channel_id,
    base_q_pair,
    base_q_pair_i,
    nof_q_pairs,
    q_pair_num;
  uint32
    if_id_internal,
    ifc = 0,
    ps,
    ps_i,
    fqp_txi_context_id = ARAD_EGQ_NOF_IFCS,
    found,
    if_rate_mbps;
  uint8
    is_channelized_interface = FALSE;
  ARAD_NIF_TYPE
    nif_type;
  ARAD_EGQ_PCT_TBL_DATA
    pct_data;
  ARAD_EGQ_PPCT_TBL_DATA
    ppct_data;
  soc_port_t 
    local_port, port_i;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
  int core_i;
  soc_pbmp_t   pbmp;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_MAP_WRITE_VAL);

  SOC_REG_ABOVE_64_CLEAR(th_data);
  SOC_REG_ABOVE_64_CLEAR(th_field_val);

  to_map_val = SOC_SAND_BOOL2NUM(is_mapped);

  
  rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, port_ndx, &base_q_pair);
  if(SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 1, exit);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5 ,exit, soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_id, port_ndx, &nof_q_pairs));

  rv = soc_port_sw_db_tm_to_local_port_get(unit, core_id, port_ndx, &local_port);
  if(SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 2, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 8 ,exit, soc_port_sw_db_is_valid_port_get(unit, local_port, &found));
  if(found) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_interface_rate_get(unit, local_port, &if_rate_mbps));
  }

  if (!found || if_rate_mbps == 0) { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_OFF;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_OFF;
  } else if (if_rate_mbps <= 12500) { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_10GB;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_10GB;
  } else if (if_rate_mbps <= 23000) { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_20GB;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_20GB;
  } else if (if_rate_mbps <= 48000) { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_40GB;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_40GB;
  } else if (if_rate_mbps <= 100000) { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_100GB;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_100GB;
  } else { 
    freq_rdy_sel_val = ARAD_EGQ_FREQ_RDY_TH_ID_200GB;
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_200GB;
  }

  if (ARAD_NIF_IS_ARAD_ID(map_info->if_id))
  {
    if_id_internal = arad_nif2intern_id(unit, map_info->if_id);
    fqp_txi_context_id = if_id_internal;

    nif_type = arad_nif_id2type(map_info->if_id);
    if (nif_type == ARAD_NIF_TYPE_NONE)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 20, exit);
    }

     res = arad_nif_is_channelized(
                                 unit,
                                 local_port,
                                 &is_channelized_interface
                               );
     SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)

    if (nif_type == ARAD_NIF_TYPE_ILKN) {
        ARAD_PORTS_ILKN_CONFIG *ilkn_config;
        
        if (!SOC_UNIT_NUM_VALID(unit)) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
        }

       SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_port_sw_db_protocol_offset_get(unit, port_ndx, 0, &offset));
       ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];

        if ((ilkn_config->retransmit.enable_tx) && (ilkn_config->retransmit.buffer_size_entries > 0x80)) { 
            uint32 data, actual_credits_to_cpu, nrdy_th;
            txi_sel_val = ARAD_EGQ_TXI_TH_ID_ILKN_RETRANSMIT;
            
            actual_credits_to_cpu = 0x180  -  ilkn_config->retransmit.buffer_size_entries;
            nrdy_th = actual_credits_to_cpu/2 + 1;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 1541, exit, READ_EGQ_NRDY_TH_6_7r(unit, &data));
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_7r, &data, NRDY_TH_7f, nrdy_th);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 1540, exit, WRITE_EGQ_NRDY_TH_6_7r(unit, data));
        } else {
            txi_sel_val = ARAD_EGQ_TXI_TH_ID_100GB;
        }
        
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        if((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (!IS_TDM_PORT(unit,port_ndx)))
        {
            if_id_internal += 1;
            fqp_txi_context_id += 1;
        }
    } else

    switch(nif_type)
    {
    case ARAD_NIF_TYPE_XAUI:
    case ARAD_NIF_TYPE_RXAUI:
    case ARAD_NIF_TYPE_10GBASE_R:
    case ARAD_NIF_TYPE_ILKN:
    case ARAD_NIF_TYPE_100G_CGE:
    case ARAD_NIF_TYPE_40G_XLGE:
    case ARAD_NIF_TYPE_TM_INTERNAL_PKT:
      ch_id = map_info->channel_id;      
      break;    
    case ARAD_NIF_TYPE_SGMII:
      ch_id = 0;      
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_TYPE_ERR, 30, exit);
      break;
    }
  
      
    ifc = if_id_internal;

    if (ARAD_IS_TM_INTERNAL_PKT_IF_ID(map_info->if_id)) {
      txi_sel_val = ARAD_EGQ_TXI_TH_TM_INTERNAL_PKT;
    }
  }
  else if(ARAD_IS_ECI_IF_ID(map_info->if_id))
  {
        
    if (ARAD_IS_CPU_IF_ID(map_info->if_id))
    {
            
      txi_sel_val = ARAD_EGQ_TXI_TH_ID_CPU;
      fqp_txi_context_id = ARAD_EGQ_IFC_CPU;
      ifc = ARAD_EGQ_IFC_CPU;
    }
    else if (ARAD_IS_OLP_FAP_PORT_ID(port_ndx))
    {
      
      txi_sel_val = ARAD_EGQ_TXI_TH_ID_OLP;
      fqp_txi_context_id = ARAD_EGQ_IFC_OLP;
      ifc = ARAD_EGQ_IFC_OLP;
    }
    else if (ARAD_IS_OAMP_FAP_PORT_ID(port_ndx))
    {
      
      txi_sel_val = ARAD_EGQ_TXI_TH_ID_OAMP;
      fqp_txi_context_id = ARAD_EGQ_IFC_OAMP;
      ifc = ARAD_EGQ_IFC_OAMP;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_INVALID_ECI_PORT_IDX_ERR, 100, exit);
    }    
  }
  else if (ARAD_IS_RCY_IF_ID(map_info->if_id))
  {
    
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_RCY;
    fqp_txi_context_id = ARAD_EGQ_IFC_RCY;
    ifc = ARAD_EGQ_IFC_RCY;
  }
  else if (ARAD_IS_ERP_IF_ID(map_info->if_id))
  {
    
    txi_sel_val = ARAD_EGQ_TXI_TH_ID_RCY;
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.get(unit, core_id, &ifc);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
    fqp_txi_context_id = ifc;    
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_TYPE_ERR, 140, exit);
  }

  

  
    
  
  
  for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
  {    
    offset = base_q_pair+q_pair_num;
    res = arad_egq_pct_tbl_get_unsafe(
            unit,
            core_id,
            offset,
            &(pct_data)
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);
  
    pct_data.port_ch_num = ch_id;

    
    if (ARAD_IS_CPU_IF_ID(map_info->if_id)) 
    {
      pct_data.port_ch_num = (offset % ARAD_NOF_CPU_IF_CHANNELS_MAX);
    }

    res = arad_egq_pct_tbl_set_unsafe(
            unit,
            core_id,
            offset,
            &(pct_data)
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);
  }  

  

  
  offset = base_q_pair;
  res = arad_egq_ppct_tbl_get_unsafe(
          unit,
          core_id,
          offset,
          &(ppct_data)
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 210, exit);

  ppct_data.cgm_interface = (is_mapped) ? ifc:ARAD_EGQ_IFC_DEF_VAL;

  res = arad_egq_ppct_tbl_set_unsafe(
          unit,
          SOC_CORE_ALL,
          offset,
          &(ppct_data)
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 220, exit);

  
  
  
  ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
  skip_ps_mapping = FALSE;

  if (!is_mapped) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 221, exit, soc_port_sw_db_ports_to_same_interface_get(unit, local_port, &pbmp));
      SOC_PBMP_ITER(pbmp, port_i) {

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 223, exit, soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 223, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, tm_port_i, &base_q_pair_i));
          ps_i = ARAD_BASE_PORT_TC2PS(base_q_pair_i);

          if ((local_port != port_i) && (ps == ps_i)) {
              skip_ps_mapping = TRUE;
              break;
          }
      }
  }
  
  if (!skip_ps_mapping) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_EGQ_MAP_PS_TO_IFCm(unit, MEM_BLOCK_ANY, ifc, &data));    
      map_ps_to_ifc = soc_EGQ_MAP_PS_TO_IFCm_field32_get(unit,&data, MAP_PS_TO_IFCf);
      
      fld_idx = ARAD_FLD_IDX_GET(ps, SOC_SAND_REG_SIZE_BITS);    
      to_map_val = SOC_SAND_BOOL2NUM(is_mapped);
      SOC_SAND_SET_BIT(map_ps_to_ifc, to_map_val, fld_idx);

      soc_EGQ_MAP_PS_TO_IFCm_field32_set(unit, &data, MAP_PS_TO_IFCf, map_ps_to_ifc);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, WRITE_EGQ_MAP_PS_TO_IFCm(unit, MEM_BLOCK_ANY, ifc, &data));
  }

  
  
  
  if (is_mapped)
  {
    res = arad_nif_channelize_arbiter_set_unsafe(
            unit,
            port_ndx,
            map_info->if_id,
            base_q_pair
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  }
  else
  {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 256, exit, soc_port_sw_db_num_of_channels_get(unit, local_port, &nof_channels));
      if (nof_channels == 1) {
        res = arad_nif_channelize_arbiter_delete_unsafe(
                unit,
                port_ndx
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
      }
  }
 
  SOC_REG_ABOVE_64_CLEAR(th_data);
  SOC_REG_ABOVE_64_CLEAR(th_field_val);

  if (fqp_txi_context_id ==  ARAD_EGQ_NOF_IFCS) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 265, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, READ_EGQ_NRDY_TH_SELr(unit,th_data));  
  soc_reg_above_64_field_get(unit, EGQ_NRDY_TH_SELr, th_data, NRDY_TH_SELf,th_field_val);
   
  
  SHR_BITCOPY_RANGE(th_field_val,ARAD_EGQ_TXI_SEL_REG_FLD_SIZE_BITS*fqp_txi_context_id,&txi_sel_val,0,ARAD_EGQ_TXI_SEL_REG_FLD_SIZE_BITS);
   
  
  soc_reg_above_64_field_set(unit, EGQ_NRDY_TH_SELr, th_data, NRDY_TH_SELf,th_field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, WRITE_EGQ_NRDY_TH_SELr(unit,th_data));

  
  SOC_REG_ABOVE_64_CLEAR(th_data);
  SOC_REG_ABOVE_64_CLEAR(th_field_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, READ_EGQ_FRAGMENTATION_QUEUES_READY_WORDS_THRESHOLDr(unit,th_data));
  soc_reg_above_64_field_get(unit, EGQ_FRAGMENTATION_QUEUES_READY_WORDS_THRESHOLDr, th_data, FQ_RDY_THf, th_field_val);

  SHR_BITCOPY_RANGE(th_field_val, ARAD_EGQ_FREQ_RDY_SEL_REG_FLD_SIZE_BITS*ifc, &freq_rdy_sel_val, 0, ARAD_EGQ_FREQ_RDY_SEL_REG_FLD_SIZE_BITS);

  soc_reg_above_64_field_set(unit, EGQ_FRAGMENTATION_QUEUES_READY_WORDS_THRESHOLDr, th_data, FQ_RDY_THf, th_field_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, WRITE_EGQ_FRAGMENTATION_QUEUES_READY_WORDS_THRESHOLDr(unit,th_data));

  if (ARAD_NIF_IS_ARAD_ID(map_info->if_id)) {
    res = arad_port_egr_egq_nif_cancel_en_regs_set(
            unit,
            found ? if_rate_mbps : 0,
            ifc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
  }
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_map_write_val()",port_ndx,0);
}


STATIC
  uint32
    arad_port2if_symmetric_map_new_unmap_old(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  int                 core_id,
      SOC_SAND_IN  uint32                 port_ndx,
      SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
      SOC_SAND_IN  uint8                  is_init
    )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_already_mapped = FALSE;
  ARAD_PORT2IF_MAPPING_INFO
    curr_info;
  uint32
    iter_count = 0,
    flags;
  soc_port_t
    local_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT2IF_EGR_MAP_NEW_UNMAP_OLD);

  
  is_already_mapped = FALSE;
  do
  {
    res = arad_port_to_interface_map_get(
            unit,
            core_id,
            port_ndx,
            &curr_info.if_id,
            &curr_info.channel_id
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if ((curr_info.if_id == info->if_id) &&
       (curr_info.channel_id == info->channel_id) &&
       (is_init))
    {
      
      is_already_mapped = TRUE;
    }
    else
    {
      if (curr_info.if_id != ARAD_IF_ID_NONE)
      {
        
        res = arad_port_ingr_map_write_val_unsafe(
                unit,
                port_ndx,
                FALSE,
                &curr_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 64, exit, soc_port_sw_db_tm_to_local_port_get(unit, core_id, port_ndx, &local_port));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 65, exit, soc_port_sw_db_flags_get(unit, local_port, &flags));
        if(!(SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))) {
            res = arad_port_egr_map_write_val(
                    unit,
                    core_id,
                    port_ndx,
                    FALSE,
                    &curr_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        }
      }
    }

    if (iter_count >= ARAD_PORTS_IF_ITERATIONS_MAX)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR, 70, exit);
    }
    iter_count++;
  } while((!is_already_mapped) && (curr_info.if_id != ARAD_IF_ID_NONE));

  if (((!is_already_mapped) && (info->if_id != ARAD_IF_ID_NONE))||
      !(is_init)) 
  {

    
    res = arad_port_ingr_map_write_val_unsafe(
            unit,
            port_ndx,
            TRUE,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 114, exit, soc_port_sw_db_tm_to_local_port_get(unit, core_id, port_ndx, &local_port));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, soc_port_sw_db_flags_get(unit, local_port, &flags));
    if(!(SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))) {
        
        res = arad_port_egr_map_write_val(
                unit,
                core_id,
                port_ndx,
                TRUE,
                info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port2if_symmetric_map_new_unmap_old()",0,0);
}

uint32
  arad_port_to_dynamic_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8                    is_init
  )
{
  uint32
    res;
  soc_port_t
    local_port;

  uint8
    in_enable,
    out_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  in_enable = ARAD_PORT_IS_INCOMING(direction_ndx);
  out_enable = ARAD_PORT_IS_OUTGOING(direction_ndx);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit,soc_port_sw_db_tm_to_local_port_get(unit,core_id,port_ndx,&local_port));

  if (in_enable != out_enable && local_port != SOC_INFO(unit).erp_port[0]) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PORT_SYMMETRIC_ASSUMPTION_ERR, 5, exit);
  }
  else
  {
    res = arad_port2if_symmetric_map_new_unmap_old(
      unit,
      core_id,
      port_ndx,
      info,
      is_init
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  }
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_dynamic_interface_map_set_unsafe()",port_ndx,0);
}

uint32
  arad_port_to_dynamic_interface_unmap_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  )
{
    uint32
        res,
        flags,
        first_phy_port;
    soc_port_t
        local_port;
    ARAD_PORT2IF_MAPPING_INFO info;
    ARAD_NIF_TYPE           nif_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET_UNSAFE);

    arad_ARAD_PORT2IF_MAPPING_INFO_clear(&info);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_tm_to_local_port_get(unit, core_id, tm_port, &local_port));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_port_sw_db_channel_get(unit, local_port, &(info.channel_id)));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, soc_port_sw_db_first_phy_port_get(unit, local_port, &first_phy_port));
    res = arad_port_to_nif_type(unit, tm_port, &nif_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
    info.if_id = arad_nif_intern2nif_id(unit, nif_type, first_phy_port-1);

    if (direction_ndx == SOC_TMC_PORT_DIRECTION_INCOMING || direction_ndx == SOC_TMC_PORT_DIRECTION_BOTH) {
        res = arad_port_ingr_map_write_val_unsafe(
                unit,
                tm_port,
                FALSE,
                &info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    if (direction_ndx == SOC_TMC_PORT_DIRECTION_OUTGOING || direction_ndx == SOC_TMC_PORT_DIRECTION_BOTH) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_port_sw_db_flags_get(unit, local_port, &flags));
        if(!(SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))) {
            res = arad_port_egr_map_write_val(
                    unit,
                    core_id,
                    tm_port,
                    FALSE,
                    &info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_dynamic_interface_unmap_set_unsafe()",tm_port,0);
}


uint32
  arad_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8               is_init
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_INTERFACE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_to_dynamic_interface_map_set_unsafe(
            unit,
            core_id,
            port_ndx,
            direction_ndx,
            info,
            is_init
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_interface_map_set_unsafe()",port_ndx,0);
}

uint32
  arad_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_INTERFACE_MAP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = arad_fap_port_id_verify(unit, port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_PORT_NOF_DIRECTIONS-1,
    ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  if (ARAD_NIF_IS_ARAD_ID(info->if_id))
  {
    res = arad_nif_id_verify(info->if_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = arad_interface_id_verify(unit, info->if_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_ports_fap_and_nif_type_match_verify(
            info->if_id,
            port_ndx
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

  if (ARAD_NIF_IS_TYPE_ID(ILKN, info->if_id))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->channel_id, ARAD_NOF_ILKN_IF_CHANNELS_MAX-1,
      ARAD_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 47, exit
    );
  }
  else if (ARAD_IS_CPU_IF_ID(info->if_id))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->channel_id, ARAD_NOF_CPU_IF_CHANNELS_MAX-1,
      ARAD_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 47, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->channel_id, ARAD_NOF_IF_CHANNELS_MAX-1,
      ARAD_IF_CHANNEL_ID_OUT_OF_RANGE_ERR, 50, exit
    );
  }

  
  if ((info->channel_id != 0) &&
      ((((direction_ndx == ARAD_PORT_DIRECTION_INCOMING || direction_ndx == ARAD_PORT_DIRECTION_BOTH) && ARAD_IS_CPU_IF_ID(info->if_id)) ||
       (ARAD_IS_OLP_IF_ID(info->if_id)) ||
       (ARAD_IS_OAMP_IF_ID(info->if_id)) ||
       (ARAD_NIF_IS_TYPE_ID(SGMII, info->if_id)) ||
       (ARAD_NIF_IS_TYPE_ID(10GBASE_R, info->if_id)))))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_NON_ZERO_CHANNEL_FOR_UNCHANNELIZED_IF_TYPE_ERR, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_interface_map_verify()",port_ndx,0);
}


int
    arad_port_to_interface_map_get(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  int                  core_id,
      SOC_SAND_IN  uint32               tm_port,
      SOC_SAND_OUT ARAD_INTERFACE_ID    *if_id,
      SOC_SAND_OUT uint32               *channel_id
    )
{

    uint32
        res,
        is_mapped,
        to_iqm_itmpm;
    uint8
        found;
    uint32
        base_q_pair,
        reassembly_context;
    ARAD_EGQ_PCT_TBL_DATA
        pct_data;  
    ARAD_EGQ_PPCT_TBL_DATA
        ppct_data;  
    ARAD_NIF_TYPE
        nif_type;
    ARAD_INTERFACE_ID
        erp_interface_id,
        if_id_internal;

    SOCDNX_INIT_FUNC_DEFS;

    *if_id = ARAD_IF_ID_NONE;
    if(channel_id) {
        *channel_id = 0;
    }
  
    found = FALSE;

    
    res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, tm_port, &reassembly_context);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);

    
    SOCDNX_IF_ERR_EXIT(READ_IQM_ITMPMm(unit, IQM_BLOCK(unit,core_id), reassembly_context, &to_iqm_itmpm));
    is_mapped = soc_IQM_ITMPMm_field32_get(unit, &to_iqm_itmpm, NIF_PORT_VALIDf);
    if (is_mapped)
    {
        
        found = TRUE;

        
        if_id_internal = soc_IQM_ITMPMm_field32_get(unit, &to_iqm_itmpm, NIF_PORTf);
        res = arad_port_to_nif_type(unit, tm_port, &nif_type);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        *if_id = arad_nif_intern2nif_id(unit, nif_type, if_id_internal);
    }
  
    if (!found)
    {
            
             
        res = arad_egq_ppct_tbl_get_unsafe(
            unit,
            core_id,
            base_q_pair,
            &(ppct_data)
          );
        SOCDNX_IF_ERR_EXIT(res);

        
        if (base_q_pair == ARAD_FAP_EGRESS_REPLICATION_BASE_Q_PAIR) {
            
            res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.erp_interface_id.get(unit, core_id, &erp_interface_id);
            SOCDNX_IF_ERR_EXIT(res);
            if ((erp_interface_id != ARAD_IF_ID_NONE) && (ppct_data.cgm_interface == erp_interface_id))
            {          
                found = TRUE;
                *if_id = ARAD_IF_ID_ERP;
                if(channel_id) {
                    *channel_id = 0;
                }
            }     
        }

         
        if (!found) {
            switch(ppct_data.cgm_interface)
            {
                case ARAD_EGQ_IFC_RCY:
                    found = TRUE;
                    *if_id = ARAD_IF_ID_RCY;
                    break;
                case ARAD_EGQ_IFC_CPU:
                    found = TRUE;
                    *if_id = ARAD_IF_ID_CPU;
                    break;
                case ARAD_EGQ_IFC_OLP:
                    found = TRUE;
                    *if_id = ARAD_IF_ID_OLP;
                    break;
                case ARAD_EGQ_IFC_OAMP:
                    found = TRUE;
                    *if_id = ARAD_IF_ID_OAMP;
                    break;
                default:
                    
                    break;               
            }     
        }
  }
  
  if (found && channel_id)
  {
      
      
      res = arad_egq_pct_tbl_get_unsafe(
            unit,
            core_id,
            base_q_pair,
            &(pct_data)
          );
      SOCDNX_IF_ERR_EXIT(res);
      *channel_id = pct_data.port_ch_num;
  }   

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_ports_lag_order_preserve_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, ARAD_MAX_LAG_GROUP_ID,
    ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_verify()",0,0);
}


uint32
  arad_ports_lag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info
  )
{
  uint8
    in_enable,
    out_enable;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  in_enable = ARAD_PORT_IS_INCOMING(direction_ndx);
  out_enable = ARAD_PORT_IS_OUTGOING(direction_ndx);


  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_PORT_NOF_DIRECTIONS-1,
    ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, ARAD_MAX_LAG_GROUP_ID,
    ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

  if (out_enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nof_entries, ARAD_PORTS_LAG_OUT_MEMBERS_MAX,
      ARAD_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 30, exit
    );
  }

  if (in_enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nof_entries, ARAD_PORTS_LAG_IN_MEMBERS_MAX,
      ARAD_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR, 35, exit
    );
  }

  for (idx = 0; idx < info->nof_entries; idx++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->members[idx].member_id, ARAD_PORTS_LAG_MEMBER_ID_MAX,
      ARAD_PORT_LAG_MEMBER_ID_OUT_OF_RANGE_ERR, 36, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_verify()",lag_ndx,0);
}

void
  arad_ports_lag_mem_id_mark_invalid(
    SOC_SAND_INOUT SOC_PPC_LAG_INFO      *info
  )
{
  uint32
    ind;

  if (info != NULL)
  {
    for (ind=0; ind<ARAD_PORTS_LAG_MEMBERS_MAX; ++ind)
    {
      info->members[ind].member_id = ARAD_FAP_PORT_ID_INVALID;
    }
  }
}

STATIC
    int
      arad_port_parser_program_pointer_get_unsafe(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  ARAD_PORT_HEADER_TYPE  header_type,
        SOC_SAND_OUT uint32                 *port_type_val,
        SOC_SAND_OUT uint8                  *skip_config
      )
{
    SOCDNX_INIT_FUNC_DEFS;

    *skip_config = FALSE;
    *port_type_val = 0;
    switch(header_type) {
        case ARAD_PORT_HEADER_TYPE_ETH:
        case ARAD_PORT_HEADER_TYPE_INJECTED_PP: 
        case ARAD_PORT_HEADER_TYPE_INJECTED_2_PP: 
        case ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT:
        case ARAD_PORT_HEADER_TYPE_RCH_0:
        case ARAD_PORT_HEADER_TYPE_RCH_1:
            *port_type_val = ARAD_PARSER_PROG_ETH_ADDR_START;
            break;

        case ARAD_PORT_HEADER_TYPE_TDM_RAW:
        case ARAD_PORT_HEADER_TYPE_RAW:
        case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
        case ARAD_PORT_HEADER_TYPE_DSA_RAW:
        case ARAD_PORT_HEADER_TYPE_RAW_DSA:
        case ARAD_PORT_HEADER_TYPE_XGS_DiffServ:
        case ARAD_PORT_HEADER_TYPE_XGS_HQoS:    
        case ARAD_PORT_HEADER_TYPE_PROG:
          *port_type_val = ARAD_PARSER_PROG_RAW_ADDR_START;
          break;

        case ARAD_PORT_HEADER_TYPE_TM:
        case ARAD_PORT_HEADER_TYPE_INJECTED: 
        case ARAD_PORT_HEADER_TYPE_INJECTED_2: 
            *port_type_val = ARAD_PARSER_PROG_TM_ADDR_START;
            break;

        case ARAD_PORT_HEADER_TYPE_STACKING:
        case ARAD_PORT_HEADER_TYPE_TDM:
        case ARAD_PORT_HEADER_TYPE_TDM_PMM:
            *port_type_val = ARAD_PARSER_PROG_FTMH_ADDR_START;
            break;

        case ARAD_PORT_HEADER_TYPE_CPU:
            *skip_config = TRUE;
            break;
        case ARAD_PORT_HEADER_TYPE_MPLS_RAW:
            *port_type_val = ARAD_PARSER_PROG_RAW_MPLS_ADDR_START;
            break; 

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid header type %d"), header_type));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
uint8
  arad_port_header_type_is_injected_tm(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return ((header_type == ARAD_PORT_HEADER_TYPE_INJECTED) || 
              (header_type == ARAD_PORT_HEADER_TYPE_INJECTED_2))? 
                    TRUE:FALSE;
}

STATIC
uint8
  arad_port_header_type_is_injected_eth(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return ((header_type == ARAD_PORT_HEADER_TYPE_INJECTED_PP) || 
              (header_type == ARAD_PORT_HEADER_TYPE_INJECTED_2_PP))? 
                    TRUE:FALSE;
}

STATIC
uint8
  arad_port_header_type_is_injected_2(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return ((header_type == ARAD_PORT_HEADER_TYPE_INJECTED_2) || 
              (header_type == ARAD_PORT_HEADER_TYPE_INJECTED_2_PP))? 
                    TRUE:FALSE;
}

STATIC
uint8
  arad_port_header_type_is_injected_1(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return ((header_type == ARAD_PORT_HEADER_TYPE_INJECTED) || 
              (header_type == ARAD_PORT_HEADER_TYPE_INJECTED_PP))? 
                    TRUE:FALSE;
}

STATIC
uint8
  arad_port_header_type_is_injected_any(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return ((arad_port_header_type_is_injected_1(header_type)) || 
              (arad_port_header_type_is_injected_2(header_type)))? 
                    TRUE:FALSE;
}

STATIC
uint8
  arad_port_header_type_is_xgs_mac_ext(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{

      return (header_type == ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT) ? 
                    TRUE:FALSE;
}

STATIC int
soc_dpp_pp_port_is_pon_port(int unit, int pp_port) {
    int port_is_pon;
    int pon_port;

    if (!SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
        port_is_pon = FALSE;
    }
    else if (_BCM_PPD_IS_PON_PP_PORT(pp_port, unit)) {
        pon_port = _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, pp_port);
        if (IS_PON_PORT(unit, pon_port)) {
            port_is_pon = TRUE;
        }
        else {
            port_is_pon = FALSE;
        }
    }
    else {
        port_is_pon = FALSE;
    }

    return port_is_pon;
}


int
  arad_port_header_type_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE  header_type
  )
{
    uint32
        fld_val,
        tbl_data_32b,
        tbl_data_64b[2],
        port_type_val = 0,
        prge_prog_select = 5; 
    ARAD_EGR_PROG_TM_PORT_PROFILE
        eg_tm_profile;
    uint8
        in_enable,
        out_enable,
        skip_config = FALSE;
    uint32
        nof_q_pairs,
        base_q_pair,
        curr_q_pair,
        pp_port,
        flags;
    ARAD_EGQ_PCT_TBL_DATA
        egq_pct_data;
    ARAD_EGQ_PPCT_TBL_DATA
        egq_ppct_data;
    ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA
        tm_port_parser_program_pointer;
    ARAD_PORT_HEADER_TYPE
        previous_headers[2];
    ARAD_MGMT_TDM_MODE
        tdm_mode;
    ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
        ihp_port2sys_data;
    ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
      vtt_in_pp_port_config_tbl_data;
    soc_port_t 
        local_port, port;
    uint8
        is_reflector_mac_and_ip_swap_port = FALSE;
    uint8
        is_reflector_mac_swap_port = FALSE;
    soc_reg_above_64_val_t 
        field_above_64,
        in_port_tbl_data;
    int core_id;
    soc_pbmp_t 
      vendor_pp_port_bm;
    soc_error_t
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    in_enable = ARAD_PORT_IS_INCOMING(direction_ndx);
    out_enable = ARAD_PORT_IS_OUTGOING(direction_ndx);

    tdm_mode = arad_sw_db_tdm_mode_get(unit);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core_id));

    if (in_enable)
    {
        SOCDNX_IF_ERR_EXIT(arad_port_parser_program_pointer_get_unsafe(
            unit,
            header_type,
            &port_type_val,
            &skip_config
        ));

        if (!skip_config)
        {
            
            SOCDNX_IF_ERR_EXIT(arad_port_header_type_get_unsafe(
                unit,
                core_id,
                tm_port,
                &(previous_headers[ARAD_PORT_DIRECTION_INCOMING]),
                &(previous_headers[ARAD_PORT_DIRECTION_OUTGOING])
            ));

            
            SOCDNX_IF_ERR_EXIT(arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_get_unsafe(unit, core, pp_port, &tm_port_parser_program_pointer));

            
            
            SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_get(unit, core, tm_port, &fld_val));

            tm_port_parser_program_pointer.parser_program_pointer_offset = 
                arad_port_header_type_is_injected_1(header_type)? (fld_val - ARAD_PORT_PTCH_1_SIZE_IN_BYTES) :
                    (arad_port_header_type_is_injected_2(header_type)? (fld_val - ARAD_PORT_PTCH_2_SIZE_IN_BYTES) : 0);
            
            if(SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG((unit))->pp.oam_statistics && arad_port_header_type_is_injected_2(header_type)) 
            {
			    if(( port == ARAD_OAMP_PORT_ID) || ((SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores != 1) && ( port == ARAD_OAMP_PORT_ID + 1)))
                tm_port_parser_program_pointer.parser_program_pointer_offset -= 2;
            }
            tm_port_parser_program_pointer.parser_program_pointer_value = port_type_val;
            tm_port_parser_program_pointer.parser_program_pointer_profile = 
                (arad_port_header_type_is_injected_any(header_type))? 
                    ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1:ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR;
            tm_port_parser_program_pointer.parser_program_pointer_value_to_use = 0x2; 

            SOCDNX_IF_ERR_EXIT(arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_set_unsafe(unit, core, pp_port, &tm_port_parser_program_pointer));

            
            tbl_data_32b = 0;
            soc_IHP_PTC_PFQ_0_CONFIGm_field32_set(unit, &tbl_data_32b, OFFSETf, tm_port_parser_program_pointer.parser_program_pointer_offset);
            soc_IHP_PTC_PFQ_0_CONFIGm_field32_set(unit, &tbl_data_32b, VALUEf, 0); 
            soc_IHP_PTC_PFQ_0_CONFIGm_field32_set(unit, &tbl_data_32b, PROFILEf, tm_port_parser_program_pointer.parser_program_pointer_profile);
            soc_IHP_PTC_PFQ_0_CONFIGm_field32_set(unit, &tbl_data_32b, VALUE_TO_USEf, 0x1); 
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_PTC_PFQ_0_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));

            
            if ((arad_port_header_type_is_injected_any(header_type)) || arad_port_header_type_is_xgs_mac_ext(header_type)) {
                SOCDNX_IF_ERR_EXIT(READ_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b));

                
                if (arad_port_header_type_is_injected_2(header_type)) {
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, PROFILEf, ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH2);
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, OFFSETf, tm_port_parser_program_pointer.parser_program_pointer_offset);
                    
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUE_TO_USEf, 0x0);
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUEf, tm_port_parser_program_pointer.parser_program_pointer_offset);
                } else if (arad_port_header_type_is_xgs_mac_ext(header_type)) {
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, PROFILEf, ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_XGS_MAC_EXT);
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, OFFSETf, ARAD_PORTS_FEM_XGS_MAC_EXT_PP_PORT_OFFSET);                
                } else {
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, PROFILEf, ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH1);
                    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, OFFSETf, tm_port_parser_program_pointer.parser_program_pointer_offset);
                }

                SOCDNX_IF_ERR_EXIT(WRITE_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b));
            } 

            SOC_PBMP_CLEAR(vendor_pp_port_bm);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_VENDOR_PP_PORT, &vendor_pp_port_bm));

            if (!(SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2() && PBMP_MEMBER(vendor_pp_port_bm,port))) {
                SOCDNX_IF_ERR_EXIT(arad_ihp_tm_port_sys_port_config_tbl_get_unsafe(unit, core, pp_port, &ihp_port2sys_data));

                
                ihp_port2sys_data.system_port_profile = (arad_port_header_type_is_injected_1(header_type)) ? 
                    ARAD_PORTS_FEM_PROFILE_SRC_PORT_INJECTED : ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR;
                ihp_port2sys_data.system_port_offset1 =  (arad_port_header_type_is_injected_1(header_type))? 
                    (1 + (fld_val - ARAD_PORT_PTCH_1_SIZE_IN_BYTES)):4; 

                SOCDNX_IF_ERR_EXIT(arad_ihp_tm_port_sys_port_config_tbl_set_unsafe(unit, core, pp_port, &ihp_port2sys_data));
            }
 


            
            if ((header_type == SOC_TMC_PORT_HEADER_TYPE_TM) || (arad_port_header_type_is_injected_any(header_type))
            ) {
                SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, IHB_PTC_KEY_GEN_VARm, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));

                SOC_REG_ABOVE_64_CLEAR(field_above_64);
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &local_port));
                fld_val = 0xC0 + soc_property_port_get((unit), local_port, spn_POST_HEADERS_SIZE,  FALSE);
                SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_PTC_KEY_BYTES_TO_RMV_LSB, &fld_val, 0, ARAD_PORT_PTC_KEY_BYTES_TO_RMV_LENGTH);

                soc_IHB_PTC_KEY_GEN_VARm_field_set(unit, in_port_tbl_data, PTC_KEY_GEN_VARf, field_above_64);
                SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IHB_PTC_KEY_GEN_VARm, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));
            }

            if ((header_type == SOC_TMC_PORT_HEADER_TYPE_RCH_0) || (header_type == SOC_TMC_PORT_HEADER_TYPE_RCH_1)) {
                
                rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
                  unit,
                  core,
                  pp_port,
                  &vtt_in_pp_port_config_tbl_data
                );
                SOCDNX_SAND_IF_ERR_RETURN(rv);

                vtt_in_pp_port_config_tbl_data.vlan_translation_profile = (header_type == SOC_TMC_PORT_HEADER_TYPE_RCH_0) 
                                                                           ? SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH0
                                                                           : SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH1;
                vtt_in_pp_port_config_tbl_data.termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_GENERALIZED_RCH;                

                rv = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
                  unit,
                  core,
                  pp_port,
                  &vtt_in_pp_port_config_tbl_data
                );
                SOCDNX_SAND_IF_ERR_RETURN(rv);
            }

            if (!SOC_IS_JERICHO(unit)) {
                
                if ((header_type == ARAD_PORT_HEADER_TYPE_TDM) && (previous_headers[ARAD_PORT_DIRECTION_INCOMING] != ARAD_PORT_HEADER_TYPE_TDM))
                {
                    
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_tdm_ifp_set,(unit, port, TRUE)));
                }
                else if ((header_type != ARAD_PORT_HEADER_TYPE_TDM)
                         && (previous_headers[ARAD_PORT_DIRECTION_INCOMING] == ARAD_PORT_HEADER_TYPE_TDM))
                {
                    
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_tdm_ifp_set,(unit, port, FALSE)));
                }
            }
        }
    }

    if (out_enable)
    {
        skip_config = FALSE;
        switch(header_type) {
            case ARAD_PORT_HEADER_TYPE_ETH:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                prge_prog_select = ARAD_PRGE_TM_SELECT_NONE;
#ifdef BCM_88660_A0
                if (SOC_IS_ARADPLUS(unit) && soc_dpp_pp_port_is_pon_port(unit, pp_port)) {
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_ETH_WITH_LLID_TAG, &prge_prog_select));
                }
#endif
                {
                    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "user_header_always_remove", 0) == 1) {
                        uint32 tm_select_eth_remove_system_headers = 0;

                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_ETH_REMOVE_SYSTEM_HEADERS, &tm_select_eth_remove_system_headers));
						
						prge_prog_select |= tm_select_eth_remove_system_headers;
                    }
                }
                break;
            case ARAD_PORT_HEADER_TYPE_INJECTED_PP: 
            case ARAD_PORT_HEADER_TYPE_INJECTED_2_PP: 
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                break;

            case ARAD_PORT_HEADER_TYPE_RAW:
            case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
                port_type_val = ARAD_PORT_PCT_TYPE_RAW;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_RAW;
                SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_SYSTEM_HEADER, &prge_prog_select));
                break;

            case ARAD_PORT_HEADER_TYPE_DSA_RAW:
                port_type_val = ARAD_PORT_PCT_TYPE_RAW;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_DSA_TX;
                SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_DSA_TX, &prge_prog_select));
                 break;

            case ARAD_PORT_HEADER_TYPE_RAW_DSA:
                port_type_val = ARAD_PORT_PCT_TYPE_RAW;
                if (SOC_DPP_CONFIG(unit)->pp.dsa_tag_custom_enable == 0) {
                    eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_RAW;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_SYSTEM_HEADER, &prge_prog_select));
                } else {
                    eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_DSA_RX;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_DSA_RX, &prge_prog_select));
                }
                 break;
 
            case ARAD_PORT_HEADER_TYPE_INJECTED_2: 
            case ARAD_PORT_HEADER_TYPE_TM:
                port_type_val = ARAD_PORT_PCT_TYPE_TM;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_OTMH_BASE, &prge_prog_select));
                break;

            case ARAD_PORT_HEADER_TYPE_PROG:
                skip_config = TRUE;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_RAW;
                break;

            case ARAD_PORT_HEADER_TYPE_CPU:
                port_type_val = ARAD_PORT_PCT_TYPE_CPU;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_CPU;
                prge_prog_select = ARAD_PRGE_TM_SELECT_NONE; 
                break;

            case ARAD_PORT_HEADER_TYPE_STACKING:
                port_type_val = ARAD_PORT_PCT_TYPE_TM;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_STACK1;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_STACKING, &prge_prog_select));                     
                break;

            case ARAD_PORT_HEADER_TYPE_TDM_RAW:
            case ARAD_PORT_HEADER_TYPE_TDM:
                if (header_type == ARAD_PORT_HEADER_TYPE_TDM) 
                {
                    if (!SOC_DPP_CONFIG(unit)->tdm.is_bypass) 
                    {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed !tdm.is_bypass")));
                    }
                } else  
                {
                    if (!SOC_DPP_CONFIG(unit)->tdm.is_packet) 
                    {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed !tdm.is_packet")));
                    }
                }
                port_type_val = ARAD_PORT_PCT_TYPE_TM;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_TDM1;
                    if ((tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT) || (SOC_IS_JERICHO_PLUS(unit) && !(SOC_DPP_CONFIG(unit)->pp.port_extender_map_lc_exists) 
                        && (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA))) { 
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_TDM_OPT_FTMH, &prge_prog_select));
                    } else {
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_SYSTEM_HEADER, &prge_prog_select));
                    }
                break;

            case ARAD_PORT_HEADER_TYPE_TDM_PMM:
                port_type_val = ARAD_PORT_PCT_TYPE_TM;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_TDM1;
                if (arad_sw_db_is_petrab_in_system_get(unit)) {
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TDM_PMM_HEADER, &prge_prog_select));
                }
                else {
                    prge_prog_select = ARAD_PRGE_TM_SELECT_NONE;
                    if (SOC_IS_JERICHO(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tdm_eg_editing_is_cud_stamping", 0) == 1)) {
                        
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TDM_STAMP_CUD_OVER_OUTER_FTMH, &prge_prog_select));
                    }
                    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tdm_eg_editing_is_cud_stamping_type1", 0) == 1)) {
                        
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TDM_STAMP_CUD, &prge_prog_select));
                    }
                }
                break;

            case ARAD_PORT_HEADER_TYPE_INJECTED:
                port_type_val = ARAD_PORT_PCT_TYPE_CPU;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_INJECTED;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TDM_STAMP_CUD, &prge_prog_select));                  
                break;

            case ARAD_PORT_HEADER_TYPE_XGS_DiffServ:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                prge_prog_select = ARAD_PRGE_TM_SELECT_NONE;                   
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_RAW; 
                break;

            case ARAD_PORT_HEADER_TYPE_XGS_HQoS:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_XGS_PE_FROM_FRC_LITE, &prge_prog_select));              
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_RAW; 
                break;

            case ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                prge_prog_select = ARAD_PRGE_TM_SELECT_NONE;                   
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH; 
                break;

            case ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_L2_REMOTE_CPU, &prge_prog_select));                 
                break;
            case ARAD_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_L2_ENCAP_EXTERNAL_CPU, &prge_prog_select));                 
                break;

            case ARAD_PORT_HEADER_TYPE_UDH_ETH:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                break;
            case ARAD_PORT_HEADER_TYPE_COE:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                prge_prog_select=ARAD_PRGE_TM_SELECT_NONE;
                break;

            case ARAD_PORT_HEADER_TYPE_MPLS_RAW:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_ETH;
                SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_RAW_MPLS, &prge_prog_select));
                break;
            case ARAD_PORT_HEADER_TYPE_RCH_0:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_GENERALIZED_RCY_HEADER_0;
                SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_GENERALIZED_RCY_HEADER_0, &prge_prog_select));
                break;
            case ARAD_PORT_HEADER_TYPE_RCH_1:
                port_type_val = ARAD_PORT_PCT_TYPE_ETH;
                eg_tm_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_GENERALIZED_RCY_HEADER_1;
                SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_GENERALIZED_RCY_HEADER_1, &prge_prog_select));
                break;

            default:
                prge_prog_select = ARAD_PRGE_TM_SELECT_NONE; 
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR")));
        }

        if (!skip_config)
        {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
                SOCDNX_IF_ERR_EXIT(rv);
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));

                
                SOCDNX_IF_ERR_EXIT(arad_port_header_type_get_unsafe(
                    unit,
                    core_id,
                    tm_port,
                    &(previous_headers[ARAD_PORT_DIRECTION_INCOMING]),
                    &(previous_headers[ARAD_PORT_DIRECTION_OUTGOING])
                  ));

                if ((soc_property_port_num_get(unit, port, spn_RFC2544_REFLECTOR_MAC_AND_IP_SWAP_PORT, 0) == 1) ||
                    (soc_property_get(unit, spn_RFC2544_REFLECTOR_MAC_AND_IP_SWAP_PORT, 0) == port)) {
                    is_reflector_mac_and_ip_swap_port = TRUE;
                }
                if ((soc_property_port_num_get(unit, port, spn_RFC2544_REFLECTOR_MAC_SWAP_PORT, 0) == 1) ||
                    (soc_property_get(unit, spn_RFC2544_REFLECTOR_MAC_SWAP_PORT, 0) == port)) {
                    is_reflector_mac_swap_port = TRUE;
                }
                
                for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_q_pairs; curr_q_pair++) 
                {
                    SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, curr_q_pair,  &egq_pct_data));
                    egq_pct_data.port_type = port_type_val;
                    egq_pct_data.prog_editor_profile = prge_prog_select;
      
                    if(soc_property_port_suffix_num_get(unit, port, -1, spn_CUSTOM_FEATURE, "add_size_header", 0))
                    {
                            SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_ADD_PACKET_SIZE_HEADER, &egq_pct_data.prog_editor_profile));
                    }

                    if (is_reflector_mac_and_ip_swap_port && port) {
                        if (egq_pct_data.port_type != ARAD_PORT_PCT_TYPE_ETH) {
                            
                            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Reflector port must be defined as Ethernet port")));
                        }
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REFLECTOR_IP_AND_ETH, &egq_pct_data.prog_editor_profile));
                    } else if (is_reflector_mac_swap_port && port) {
                        if (egq_pct_data.port_type != ARAD_PORT_PCT_TYPE_ETH) {
                            
                            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Reflector port must be defined as Ethernet port")));
                        }
                        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REFLECTOR_ETH, &egq_pct_data.prog_editor_profile));
                    }

                    if (soc_property_port_suffix_num_get(unit, port,-1, spn_CUSTOM_FEATURE,"vendor_custom_tm_port", FALSE)==1  && port) {
                              SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TM_MODE_HW_PROCESSING, &egq_pct_data.prog_editor_profile));
                    } else if (soc_property_port_suffix_num_get(unit, port, -1,spn_CUSTOM_FEATURE,"vendor_custom_pp_port", FALSE)==1 && port) {
                        if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0)) {
                              SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_PP_MODE_HW_PROCESSING, &egq_pct_data.prog_editor_profile));
                        }
                    }

                    SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_set_unsafe(unit, core, curr_q_pair, &egq_pct_data));

                    if (SOC_DPP_CONFIG((unit))->tm.queue_level_interface_enable) 
                    {
                        
                        soc_port_if_t interface;
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

                        
                        if (interface == SOC_PORT_IF_ILKN) 
                        {
                            int curr_mc_q_pair;

                            curr_mc_q_pair = ARAD_EGR_QUEUING_MC_QUEUE_OFFSET(curr_q_pair); 
          
                            SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, curr_mc_q_pair, &egq_pct_data));
                            egq_pct_data.port_type = port_type_val;
                            egq_pct_data.prog_editor_profile = prge_prog_select;
                            SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_set_unsafe(unit, core, curr_mc_q_pair, &egq_pct_data));
                        }
                    }
                }        

                
                SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_get_unsafe(unit, core_id, base_q_pair, &egq_ppct_data));
                egq_ppct_data.port_type = port_type_val;
                SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_set_unsafe(unit, core_id, base_q_pair, &egq_ppct_data));

                
                if (header_type != ARAD_PORT_HEADER_TYPE_TM)
                {
                    
                    if (header_type != ARAD_PORT_HEADER_TYPE_PROG)
                    {
                        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.set(unit,tm_port,eg_tm_profile));
                    }
                }
                              
                if (header_type == ARAD_PORT_HEADER_TYPE_TDM ||
                    header_type == ARAD_PORT_HEADER_TYPE_TDM_RAW ||
                    header_type == ARAD_PORT_HEADER_TYPE_TDM_PMM) {
                    
                    SOCDNX_IF_ERR_EXIT(arad_tdm_ofp_set_unsafe(unit, port, TRUE));
                } else if ((header_type != ARAD_PORT_HEADER_TYPE_TDM &&
                            header_type != ARAD_PORT_HEADER_TYPE_TDM_RAW &&
                            header_type != ARAD_PORT_HEADER_TYPE_TDM_PMM) &&
                           (previous_headers[ARAD_PORT_DIRECTION_OUTGOING] == ARAD_PORT_HEADER_TYPE_TDM ||
                            previous_headers[ARAD_PORT_DIRECTION_OUTGOING] == ARAD_PORT_HEADER_TYPE_TDM_RAW ||
                            previous_headers[ARAD_PORT_DIRECTION_OUTGOING] == ARAD_PORT_HEADER_TYPE_TDM_PMM)) {
                    
                    SOCDNX_IF_ERR_EXIT(
                        arad_tdm_ofp_set_unsafe(unit, port, FALSE));
                }             

                if(header_type == ARAD_PORT_HEADER_TYPE_STACKING) {
                    SOCDNX_IF_ERR_EXIT(
                        arad_pp_lag_hashing_port_lb_profile_set(unit, core, pp_port,  1));
                } else {
                    SOCDNX_IF_ERR_EXIT(
                        arad_pp_lag_hashing_port_lb_profile_set(unit, core, pp_port,  0));
                }

                
                if (header_type == ARAD_PORT_HEADER_TYPE_ETH ||
                    header_type == ARAD_PORT_HEADER_TYPE_MPLS_RAW) {
                    
                    SOCDNX_IF_ERR_EXIT(
                        arad_tdm_ofp_set_unsafe(unit, port, FALSE));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_port_header_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_HEADER_TYPE_VERIFY);

  res = arad_fap_port_id_verify(unit, port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_PORT_NOF_DIRECTIONS-1,
    ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    header_type, ARAD_PORT_NOF_HEADER_TYPES-1,
    ARAD_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_header_type_verify()",0,0);
}

int
  arad_port_parse_header_type_unsafe(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN soc_port_t  port,
    SOC_SAND_IN uint32      port_parser_program_pointer,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE * header_type_in
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    switch(port_parser_program_pointer) {
        case ARAD_PARSER_PROG_RAW_ADDR_START:
            *header_type_in = ARAD_PORT_HEADER_TYPE_RAW;
            break;
        case ARAD_PARSER_PROG_TM_ADDR_START:
            *header_type_in = ARAD_PORT_HEADER_TYPE_TM;
            break;
        case ARAD_PARSER_PROG_ETH_ADDR_START:
            *header_type_in = ARAD_PORT_HEADER_TYPE_ETH;
            break;
        case ARAD_PARSER_PROG_RAW_MPLS_ADDR_START:
            *header_type_in = ARAD_PORT_HEADER_TYPE_MPLS_RAW;
            break;
        case ARAD_PARSER_PROG_FTMH_ADDR_START:
            *header_type_in = (IS_TDM_PORT(unit,port) == TRUE) ? ARAD_PORT_HEADER_TYPE_TDM : ARAD_PORT_HEADER_TYPE_STACKING;
            break;
        default:
            *header_type_in = ARAD_PORT_HEADER_TYPE_NONE;
    }

    SOCDNX_FUNC_RETURN;
}

int
  arad_port_header_type_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE  *header_type_incoming,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE  *header_type_outgoing
  )
{
    uint32                 
        pp_port;
    int 
        core_id;
    uint32
        port_type_val = 0;
    ARAD_EGQ_PPCT_TBL_DATA
        egq_ppct_data;
    ARAD_PORT_HEADER_TYPE
        header_type_in = ARAD_PORT_HEADER_TYPE_NONE,
        header_type_out = ARAD_PORT_HEADER_TYPE_NONE;
    uint32
        base_q_pair;    
    ARAD_EGR_PROG_TM_PORT_PROFILE
        port_profile;
    ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA
        tm_port_parser_program_pointer;
    soc_port_t port;
    soc_error_t
        rv;
  
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core_id));

    
    SOCDNX_IF_ERR_EXIT(arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_get_unsafe(
        unit,
        core,
        pp_port,
        &tm_port_parser_program_pointer
        ));

    port_type_val = tm_port_parser_program_pointer.parser_program_pointer_value;

    SOCDNX_IF_ERR_EXIT(arad_port_parse_header_type_unsafe(
        unit,
        port,
        port_type_val,
        &header_type_in
        ));

    
    if (tm_port_parser_program_pointer.parser_program_pointer_profile == ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_INJECTED_PTCH2)
    {
        
        header_type_in = ((header_type_in != ARAD_PORT_HEADER_TYPE_ETH) && (header_type_in != ARAD_PORT_HEADER_TYPE_MPLS_RAW))?
            ARAD_PORT_HEADER_TYPE_INJECTED_2:ARAD_PORT_HEADER_TYPE_INJECTED_2_PP;
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_get_unsafe(unit, core_id, base_q_pair, &egq_ppct_data));

    port_type_val = egq_ppct_data.port_type;

    switch(port_type_val) {
        case ARAD_PORT_PCT_TYPE_CPU:
            
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.get(unit, tm_port, &port_profile));

            switch(port_profile)
            {
                case ARAD_EGR_PROG_TM_PORT_PROFILE_CPU:
                    header_type_out = ARAD_PORT_HEADER_TYPE_CPU;
                    break;

                case ARAD_EGR_PROG_TM_PORT_PROFILE_STACK1:
                    header_type_out = ARAD_PORT_HEADER_TYPE_STACKING;
                    break;

                case ARAD_EGR_PROG_TM_PORT_PROFILE_TDM1:
                    header_type_out = ARAD_PORT_HEADER_TYPE_TDM;
                    break;

                case ARAD_EGR_PROG_TM_PORT_PROFILE_INJECTED:
                    header_type_out = ARAD_PORT_HEADER_TYPE_INJECTED;
                    break;

                default:
                    header_type_out = ARAD_PORT_HEADER_TYPE_NONE;
            }
            break;

        case ARAD_PORT_PCT_TYPE_RAW:
            header_type_out = ARAD_PORT_HEADER_TYPE_RAW;
            break;

        case ARAD_PORT_PCT_TYPE_TM:
            header_type_out = ARAD_PORT_HEADER_TYPE_TM;
            break;

        case ARAD_PORT_PCT_TYPE_ETH:
            header_type_out = ARAD_PORT_HEADER_TYPE_ETH;
            break;

            

        default:
            header_type_out = ARAD_PORT_HEADER_TYPE_NONE;
    }


    *header_type_incoming = header_type_in;
    *header_type_outgoing = header_type_out;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_ports_ftmh_extension_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
     ext_option, ARAD_PORTS_FTMH_NOF_EXT_OUTLIFS,
      ARAD_FTMH_EXTENSION_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_verify()",0,0);
}

uint32
  arad_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res,
    field_val;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_SET_UNSAFE);

  field_val = (uint32)ext_option;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, FTMH_EXTf,  field_val));    
  
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_set_unsafe()",0,0);
}

uint32
  arad_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res,
    field_val;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_GET_UNSAFE);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, FTMH_EXTf, &field_val));    

  *ext_option = (ARAD_PORTS_FTMH_EXT_OUTLIF)field_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_get_unsafe()",0,0);
}

int
  arad_ports_otmh_extension_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     tm_port,
    SOC_SAND_IN  ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
    ARAD_PORT_HEADER_TYPE
        header_type_outgoing,
        header_type_incoming;
    uint32
        tm_port_profile;
    ARAD_EGQ_PCT_TBL_DATA
        pct_tbl_data;
    uint32
        base_q_pair,
        nof_pairs,
        curr_q_pair, 
        prog_editor_value=0,
        update_prog_editor_value = 0;
    uint32 
        is_double_tag = 0,
        is_otmh_enlarge = 0;
    uint32 prge_prog_pointer;
    uint32 branch_ptr_cud_sp, branch_ptr_cud_mc;
    uint32 addr_no_jump, addr_cud_sp, addr_cud_mc;
    soc_port_t   port;
    soc_error_t
        rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));

    
    SOCDNX_IF_ERR_EXIT(arad_port_header_type_get_unsafe(
          unit,
          core,
          tm_port,
          &header_type_incoming,
          &header_type_outgoing
        ));

    if (header_type_outgoing != ARAD_PORT_HEADER_TYPE_TM)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_PORTS_NOT_TM_PORT_ERR")));
    }

    is_otmh_enlarge = (info->outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE);

    is_double_tag = (info->outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG);
    if (!is_double_tag && !is_otmh_enlarge ) 
    {   
        update_prog_editor_value = 1;
        tm_port_profile = ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD(info->src_ext_en, info->dest_ext_en, info->outlif_ext_en);

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.set(unit,tm_port,tm_port_profile));

        SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_program_pointer_get(unit, ARAD_EGR_PROG_EDITOR_PROG_OTMH, &prge_prog_pointer));
        addr_no_jump = ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR(prge_prog_pointer, 0);

        SOC_SAND_IF_ERR_EXIT(arad_egr_prog_editor_branch_pointer_allocate(unit, ARAD_EGR_PROG_EDITOR_BRANCH_BASE_OTMH_CUD_AND_SOURCE_PORT, &branch_ptr_cud_sp));
        addr_cud_sp = ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR(branch_ptr_cud_sp, 0);

        SOC_SAND_IF_ERR_EXIT(arad_egr_prog_editor_branch_pointer_allocate(unit, ARAD_EGR_PROG_EDITOR_BRANCH_BASE_OTMH_CUD_MC, &branch_ptr_cud_mc));
        addr_cud_mc = ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR(branch_ptr_cud_mc, 0);

        
        switch (tm_port_profile) {
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV: 
                
                prog_editor_value   = addr_no_jump | (addr_no_jump << 12); 
                break;
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC:  
                
                prog_editor_value = addr_cud_mc | (addr_no_jump << 12); 
                break;
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW: 
                
                prog_editor_value = addr_cud_sp | (addr_no_jump << 12); 
                break;
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV: 
                
                prog_editor_value   = addr_no_jump | (addr_cud_sp << 12); 
                break;
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC:
                
                prog_editor_value = addr_cud_mc | (addr_cud_sp << 12); 
                break;
            case ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW:
                
                prog_editor_value = addr_cud_sp | (addr_cud_sp << 12); 
                break;
            default:
                update_prog_editor_value = 0; 
        }
    }

    if (update_prog_editor_value || is_double_tag || is_otmh_enlarge)
    {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_pairs));

        
        for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {

            SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, curr_q_pair, &pct_tbl_data));
        
            
            if (is_double_tag) {
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_PP_RCY_PORT, &pct_tbl_data.prog_editor_profile));
            } else if(is_otmh_enlarge) {
                    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_OTMH_WITH_CUD_EXT, &pct_tbl_data.prog_editor_profile));
            } else {
                pct_tbl_data.prog_editor_value = prog_editor_value;
            }

            SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_set_unsafe(unit, core, curr_q_pair, &pct_tbl_data));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_ports_otmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;
  ARAD_PORT_HEADER_TYPE
    header_type_outgoing,
    header_type_incoming;
  ARAD_EGR_PROG_TM_PORT_PROFILE
    port_profile;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_OTMH_EXTENSION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  
  res = arad_port_header_type_get_unsafe(
          unit,
          core_id,
          tm_port,
          &header_type_incoming,
          &header_type_outgoing
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.ports_prog_editor_profile.get(
            unit,
            tm_port,
            &port_profile
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  info->outlif_ext_en = (port_profile  - ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV)
    % ARAD_PORTS_FTMH_NOF_EXT_OUTLIFS;

  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_SRC_GET(port_profile, info->src_ext_en);
  ARAD_EGR_PROG_TM_PORT_PROFILE_TM_DEST_GET(port_profile, info->dest_ext_en);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_otmh_extension_get_unsafe()",0,0);
}

uint32
  arad_if2wc_id(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID if_id,
    SOC_SAND_OUT uint32          *wc_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_OFP_MAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(wc_ndx);

  *wc_ndx = ARAD_IF2WC_NDX(if_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error arad_if2wc_id soc_pa_if2mal_id()",if_id,0);
}



uint32
  arad_port_egr_hdr_credit_discount_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  soc_reg_t
    uc_reg,
    mc_reg;
  soc_field_t
    uc_fld = 0,
    mc_fld = 0;
  uint32
    res,
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  uc_reg = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ? 
    EGQ_UNICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Ar:EGQ_UNICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Br;

  mc_reg = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ? 
    EGQ_MULTICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Ar:EGQ_MULTICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Br;

  switch(port_hdr_type_ndx)
  {
  case ARAD_PORT_HEADER_TYPE_RAW:
  case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
  case ARAD_PORT_HEADER_TYPE_DSA_RAW:
  case ARAD_PORT_HEADER_TYPE_RAW_DSA:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_RAW_TYPE_Af:HDR_ADJUST_UC_RAW_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_RAW_TYPE_Af:HDR_ADJUST_MC_RAW_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_CPU:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_CPU_TYPE_Af:HDR_ADJUST_UC_CPU_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_CPU_TYPE_Af:HDR_ADJUST_MC_CPU_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_ETH:
  case ARAD_PORT_HEADER_TYPE_MPLS_RAW:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_ETH_TYPE_Af:HDR_ADJUST_UC_ETH_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_ETH_TYPE_Af:HDR_ADJUST_MC_ETH_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_TM:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_TM_TYPE_Af:HDR_ADJUST_UC_TM_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_TM_TYPE_Af:HDR_ADJUST_MC_TM_TYPE_Bf;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_HEADER_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  value = 0;
  value |= SOC_SAND_SET_FLD_IN_PLACE((uint32)soc_sand_abs(info->uc_credit_discount), 0, SOC_SAND_BITS_MASK(6, 0));
  value |= SOC_SAND_SET_FLD_IN_PLACE(info->uc_credit_discount < 0 ? 1 : 0, 7, SOC_SAND_BITS_MASK(7, 7));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, uc_reg, core, 0, uc_fld,  value));

  value = 0;
  value |= SOC_SAND_SET_FLD_IN_PLACE((uint32)soc_sand_abs(info->mc_credit_discount), 0, SOC_SAND_BITS_MASK(6, 0));
  value |= SOC_SAND_SET_FLD_IN_PLACE(info->mc_credit_discount < 0 ? 1 : 0, 7, SOC_SAND_BITS_MASK(7, 7));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, mc_reg, core, 0, mc_fld,  value));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_set_unsafe()",port_hdr_type_ndx,cr_discnt_type_ndx);
}



uint32
  arad_port_egr_hdr_credit_discount_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_hdr_type_ndx, ARAD_PORT_NOF_HEADER_TYPES-1,
    ARAD_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type_ndx, ARAD_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_verify()",port_hdr_type_ndx,cr_discnt_type_ndx);
}



uint32
  arad_port_egr_hdr_credit_discount_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  soc_reg_t
    uc_reg,
    mc_reg;
  soc_field_t
    uc_fld = 0,
    mc_fld = 0;
  uint32
    res,
    value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_hdr_type_ndx, ARAD_PORT_NOF_HEADER_TYPES-1,
    ARAD_PORT_TYPE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type_ndx, ARAD_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

  uc_reg = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ? 
    EGQ_UNICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Ar:EGQ_UNICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Br;

  mc_reg = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ? 
    EGQ_MULTICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Ar:EGQ_MULTICAST_EGRESS_PACKET_HEADER_COMPENSATION_FOR_TYPE_Br;

  switch(port_hdr_type_ndx)
  {
  case ARAD_PORT_HEADER_TYPE_RAW:
  case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
  case ARAD_PORT_HEADER_TYPE_DSA_RAW:
  case ARAD_PORT_HEADER_TYPE_RAW_DSA:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_RAW_TYPE_Af:HDR_ADJUST_UC_RAW_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_RAW_TYPE_Af:HDR_ADJUST_MC_RAW_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_CPU:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_CPU_TYPE_Af:HDR_ADJUST_UC_CPU_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_CPU_TYPE_Af:HDR_ADJUST_MC_CPU_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_ETH:
  case ARAD_PORT_HEADER_TYPE_MPLS_RAW:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_ETH_TYPE_Af:HDR_ADJUST_UC_ETH_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_ETH_TYPE_Af:HDR_ADJUST_MC_ETH_TYPE_Bf;
    break;
  case ARAD_PORT_HEADER_TYPE_TM:
    uc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_UC_TM_TYPE_Af:HDR_ADJUST_UC_TM_TYPE_Bf;

    mc_fld = (cr_discnt_type_ndx == ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A) ?
      HDR_ADJUST_MC_TM_TYPE_Af:HDR_ADJUST_MC_TM_TYPE_Bf;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_HEADER_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, uc_reg, REG_PORT_ANY, 0, uc_fld, &value));  
  info->uc_credit_discount = SOC_SAND_GET_FLD_FROM_PLACE(value,  0, SOC_SAND_BITS_MASK(6, 0));
  info->uc_credit_discount *= SOC_SAND_GET_FLD_FROM_PLACE(value, 7, SOC_SAND_BITS_MASK(7, 7)) ? -1 : 1;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, mc_reg, REG_PORT_ANY, 0, mc_fld, &value));  
  info->mc_credit_discount = 0;
  info->mc_credit_discount = SOC_SAND_GET_FLD_FROM_PLACE(value,  0, SOC_SAND_BITS_MASK(6, 0));
  info->mc_credit_discount *= SOC_SAND_GET_FLD_FROM_PLACE(value, 7, SOC_SAND_BITS_MASK(7, 7)) ? -1 : 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_get_unsafe()",port_hdr_type_ndx,cr_discnt_type_ndx);
}



uint32
  arad_port_egr_hdr_credit_discount_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_VERIFY);

  res = arad_fap_port_id_verify(unit, port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_discnt_type, ARAD_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES-1,
    ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_verify()",0,0);
}


int
  arad_port_pp_port_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     pp_port,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  )
{
    uint32
        tbl_data_32b,
        ihb_pinfo_pmf_mem[SOC_DPP_IMP_DEFS_MAX(IHB_PINFO_PMF_NOF_LONGS)],
        is_hg,    
        fld_val,
        port_encoded,
        value,
        prge_var;
    uint8
        in_port_key_gen_var_write,
        skip_config = FALSE;
    uint32
        prge_prog_select,
        port_type_val = 0;  
    soc_reg_above_64_val_t 
        field_above_64,
        in_port_tbl_data;
    soc_mem_t
        mem;
    soc_field_t
        field;
    ARAD_PP_EPNI_PP_PCT_TBL_DATA
        pp_pct_tbl_data, pp_pct_tbl_data_tmp;
    ARAD_MGMT_TDM_MODE
        tdm_mode;
    soc_port_t local_port;
    soc_port_if_t itf_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(success);

    
    if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_NONE) {
        *success = SOC_SAND_SUCCESS;
        SOC_EXIT;
    }

    tdm_mode = arad_sw_db_tdm_mode_get(unit);

     
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    in_port_key_gen_var_write = FALSE;
    if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ || 
        info->header_type == SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS)
    {
        if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ) {
            fld_val = 0x1;
        } else if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS) {
            fld_val = 0x0;
        }
        SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_INPORT_KEY_DIFFSERV_LSB, &fld_val, 0, ARAD_PORT_INPORT_KEY_DIFFSERV_LENGTH);
        fld_val = ~fld_val;
        SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_INPORT_KEY_NOT_DIFFSERV_LSB, &fld_val, 0, ARAD_PORT_INPORT_KEY_NOT_DIFFSERV_LENGTH);
        fld_val = 1; 
        SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_INPORT_KEY_RECYCLE_PORT_LSB, &fld_val, 0, ARAD_PORT_INPORT_KEY_RECYCLE_PORT_LENGTH);
        in_port_key_gen_var_write = TRUE;
    }
    else if ((info->header_type != SOC_TMC_PORT_HEADER_TYPE_ETH)
           && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW)   
           && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)
           && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_STACKING)
           && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_TDM)
           && (!arad_port_header_type_is_injected_eth(info->header_type))) {
        
        fld_val = ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_VALUE;
        SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_LSB, &fld_val, 0, ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE2_LENGTH);
        
        SHR_BITCOPY_RANGE(field_above_64, ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_LSB, &(info->fc_type), 0, ARAD_PORT_INPORT_KEY_SYST_HDR_PROFILE_LENGTH);
        fld_val = 0;

        in_port_key_gen_var_write = TRUE;
    }

    if (in_port_key_gen_var_write) {
        
        if(SOC_IS_JERICHO(unit)) {
            mem = IHB_PINFO_PMFm;
            field = KEY_GEN_VARf;
        } else {
            mem = IHB_IN_PORT_KEY_GEN_VARm;
            field = IN_PORT_KEY_GEN_VARf;
        }

        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));
        soc_mem_field_set(unit, mem, in_port_tbl_data, field, field_above_64);
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));
    }


    
    if ((info->header_type == SOC_TMC_PORT_HEADER_TYPE_TM) || arad_port_header_type_is_injected_tm(info->header_type)) {
        fld_val = ARAD_PMF_PORT_PROFILE_TM;
    }
    else if ((info->header_type == ARAD_PORT_HEADER_TYPE_XGS_DiffServ) || (info->header_type == ARAD_PORT_HEADER_TYPE_XGS_HQoS)) {
        fld_val = ARAD_PMF_PORT_PROFILE_XGS_TM;
    }
    else if ((info->header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_STACKING)) {
        fld_val = ARAD_PMF_PORT_PROFILE_FTMH;
    }
    else if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_PROG) {
        fld_val = ARAD_PMF_PORT_PROFILE_MH;
    } else if ((info->header_type == SOC_TMC_PORT_HEADER_TYPE_RAW) 
           || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_DSA_RAW)
           || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_RAW_DSA)
           || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_TDM_RAW)
           || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_CPU))
    {
        fld_val = ARAD_PMF_PORT_PROFILE_RAW;
    } 
    else if (info->header_type == SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW) {
        fld_val = ARAD_PMF_PORT_PROFILE_MIRROR_RAW;
    }


    if ((info->header_type != SOC_TMC_PORT_HEADER_TYPE_ETH) 
        && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW)
        && (info->header_type != SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT) 
        && (!arad_port_header_type_is_injected_eth(info->header_type))) {
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, IHB_PINFO_PMFm, IHB_BLOCK(unit, core), pp_port, ihb_pinfo_pmf_mem));
        soc_mem_field32_set(unit, IHB_PINFO_PMFm, &ihb_pinfo_pmf_mem, PROGRAM_SELECTION_PROFILEf, fld_val);
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IHB_PINFO_PMFm, IHB_BLOCK(unit, core), pp_port, ihb_pinfo_pmf_mem));
    }

    
    SOCDNX_IF_ERR_EXIT(arad_port_parser_program_pointer_get_unsafe(
              unit,
              info->header_type,
              &port_type_val,
              &skip_config
          ));

    if (!skip_config) {
        SOCDNX_IF_ERR_EXIT(READ_IHP_PP_PORT_INFOm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));
        
        soc_IHP_PP_PORT_INFOm_field32_set(unit, &tbl_data_32b, DEFAULT_PARSER_PROGRAM_POINTERf, port_type_val);
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_PP_PORT_INFOm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_IHP_PP_PORT_INFOm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));

    
    fld_val = ((info->header_type == SOC_TMC_PORT_HEADER_TYPE_ETH)
            || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT)
            || (info->header_type == SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW) 
            || arad_port_header_type_is_injected_eth(info->header_type))? 0:1;
    soc_IHP_PP_PORT_INFOm_field32_set(unit, &tbl_data_32b, PARSER_CUSTOM_MACRO_SELECTf, fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_PP_PORT_INFOm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));

    
    if (((info->header_type == SOC_TMC_PORT_HEADER_TYPE_TM) || (arad_port_header_type_is_injected_tm(info->header_type))) && (!(SOC_DPP_CONFIG(unit)->pp.pon_application_enable))) {
        for (port_encoded = 0; port_encoded < ARAD_NOF_FAP_PORTS; ++port_encoded)
        {
            SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_INJECTED_TM_ENCODED(port_encoded), &tbl_data_32b));
            soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, pp_port);
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_INJECTED_TM_ENCODED(port_encoded), &tbl_data_32b));
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port, &pp_pct_tbl_data));

    prge_var = 0;
    switch(info->header_type_out) {
        case ARAD_PORT_HEADER_TYPE_ETH:
        case ARAD_PORT_HEADER_TYPE_MPLS_RAW:   
        case ARAD_PORT_HEADER_TYPE_INJECTED_PP:
        case ARAD_PORT_HEADER_TYPE_INJECTED_2_PP: 
            prge_prog_select = ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_SPAN; 
            if (info->flags & ARAD_PORTS_PP_PORT_RCY_OVERLAY_PTCH) {
                prge_prog_select = ARAD_PRGE_PP_SELECT_RCY_OVERLAY;
            }
            break;

        case ARAD_PORT_HEADER_TYPE_UDH_ETH: 
            prge_prog_select = ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_WITH_USER_HEADERS; 
            break;
        case ARAD_PORT_HEADER_TYPE_XGS_DiffServ:
        case ARAD_PORT_HEADER_TYPE_XGS_HQoS: 
            prge_prog_select = ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FRC_LITE; 
            break;

        case ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT:
            prge_prog_select = ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_SPAN; 
            break;

        case ARAD_PORT_HEADER_TYPE_STACKING:
            prge_prog_select = ARAD_PRGE_PP_SELECT_TDM_STAMP_CUD; 
            break;

    case ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU:
            prge_prog_select = ARAD_PRGE_PP_SELECT_L2_REMOTE_CPU;
            break;

        case ARAD_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU:
            prge_prog_select = ARAD_PRGE_PP_SELECT_L2_ENCAP_EXTERNAL_CPU;
            break;

        case ARAD_PORT_HEADER_TYPE_TDM:
        case ARAD_PORT_HEADER_TYPE_TDM_RAW:
        case ARAD_PORT_HEADER_TYPE_TDM_PMM:
            
            prge_prog_select = (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT) ? ARAD_PRGE_PP_SELECT_REMOVE_TDM_OPT_FTMH:ARAD_PRGE_PP_SELECT_REMOVE_SYSTEM_HEADER;
            break;        
        case ARAD_PORT_HEADER_TYPE_TM:
            if(soc_property_suffix_num_get(unit, pp_port, spn_CUSTOM_FEATURE, "otmh_keep_pph_", 0) == 1) {
                prge_prog_select = ARAD_PRGE_PP_SELECT_OTMH_BASE_KEEP_PPH;
            } else {
                prge_prog_select = ARAD_PRGE_PP_SELECT_NONE;   
            }
            break;
        case ARAD_PORT_HEADER_TYPE_CPU:  
        case ARAD_PORT_HEADER_TYPE_NONE:
        case ARAD_PORT_HEADER_TYPE_RAW:
        case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
        case ARAD_PORT_HEADER_TYPE_DSA_RAW:
        case ARAD_PORT_HEADER_TYPE_RAW_DSA:
        case ARAD_PORT_HEADER_TYPE_PROG:   
        case ARAD_PORT_HEADER_TYPE_INJECTED:
        case ARAD_PORT_HEADER_TYPE_INJECTED_2:
        case ARAD_PORT_NOF_HEADER_TYPES:
            
            if (soc_dpp_pp_port_is_pon_port(unit, pp_port)) {
                prge_var = 0;
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &local_port));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_get(unit, local_port, &is_hg));
                prge_var = is_hg? 1 : 0;
            }
            prge_prog_select = ARAD_PRGE_PP_SELECT_NONE;   
            break;
        case ARAD_PORT_HEADER_TYPE_COE: 
            prge_prog_select = ARAD_PRGE_PP_SELECT_PP_COPY_HEADER; 
            break;
        case ARAD_PORT_HEADER_TYPE_RCH_0:
        case ARAD_PORT_HEADER_TYPE_RCH_1:
            prge_prog_select = ARAD_PRGE_PP_SELECT_NONE;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR")));
    }

    
    pp_pct_tbl_data.disable_filter = 0;
    switch(info->header_type_out) {
        case ARAD_PORT_HEADER_TYPE_ETH:
        case ARAD_PORT_HEADER_TYPE_MPLS_RAW: 
        case ARAD_PORT_HEADER_TYPE_UDH_ETH:
        case ARAD_PORT_HEADER_TYPE_INJECTED_PP:
        case ARAD_PORT_HEADER_TYPE_INJECTED_2_PP:       
        case ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT:
        case ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU:
        case ARAD_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU:
        case ARAD_PORT_HEADER_TYPE_COE: 
        case ARAD_PORT_HEADER_TYPE_RCH_0: 
        case ARAD_PORT_HEADER_TYPE_RCH_1: 
            
            break;
        case ARAD_PORT_HEADER_TYPE_XGS_DiffServ:
        case ARAD_PORT_HEADER_TYPE_XGS_HQoS: 
        case ARAD_PORT_HEADER_TYPE_STACKING:
        case ARAD_PORT_HEADER_TYPE_TDM:
        case ARAD_PORT_HEADER_TYPE_TDM_RAW:
        case ARAD_PORT_HEADER_TYPE_TDM_PMM:
        case ARAD_PORT_HEADER_TYPE_CPU:  
        case ARAD_PORT_HEADER_TYPE_NONE:
        case ARAD_PORT_HEADER_TYPE_RAW:
        case ARAD_PORT_HEADER_TYPE_MIRROR_RAW: 
        case ARAD_PORT_HEADER_TYPE_DSA_RAW:
        case ARAD_PORT_HEADER_TYPE_RAW_DSA:
        case ARAD_PORT_HEADER_TYPE_TM:
        case ARAD_PORT_HEADER_TYPE_PROG:   
        case ARAD_PORT_HEADER_TYPE_INJECTED:
        case ARAD_PORT_HEADER_TYPE_INJECTED_2:
        case ARAD_PORT_NOF_HEADER_TYPES:
            pp_pct_tbl_data.disable_filter = 1; 
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR")));
    }  

    if (pp_port == ARAD_OLP_PORT_ID &&
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "olp_petra", 0) != 0) {
        prge_prog_select = ARAD_PRGE_PP_SELECT_PP_OLP_PORT;
    }

    SOCDNX_IF_ERR_EXIT(dcmn_property_get(unit,spn_NUM_OAMP_PORTS, 0,&value));
    if (pp_port == ARAD_OAMP_PORT_ID &&
        ((value == 1) || soc_property_get(unit, spn_SAT_ENABLE, 0))) {
        prge_prog_select = ARAD_PRGE_PP_SELECT_PP_OAM_PORT;
    }

     
    if (SOC_DPP_CONFIG(unit)->pp.test1 == 1 || SOC_DPP_CONFIG(unit)->pp.test2 == 1 || SOC_DPP_CONFIG(unit)->pp.mim_bsa_lsb_from_ssp) {
        if (info->flags & ARAD_PORTS_TEST1_PORT_PP_PORT) {

                prge_prog_select = ARAD_PRGE_PP_SELECT_EoE_PROGRAM;
        }
    } 

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
        if (info->flags & ARAD_PORTS_MIM_SPB_PORT_PP_PORT) {
                prge_prog_select = ARAD_PRGE_PP_SELECT_MIM_SPB;
        }
    }

    
    if (((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN) && info->flags & ARAD_PORTS_PRESERVING_DSCP_PORT_PP_PORT) {
        prge_prog_select = ARAD_PRGE_PP_SELECT_PRESERVING_DSCP;
    }

    if(SOC_IS_QAX(unit))           
    {
        if(SOC_DPP_CONFIG(unit)->qax->ipsec)  
        {
            soc_port_t  ipsec_port;
            IPSEC_ACCESS.ipsec_port.get(unit, &ipsec_port);

            if (pp_port == ipsec_port)        
            {
                
                prge_prog_select = ARAD_PRGE_PP_SELECT_IPSEC_ENC_DEC;
            }
        }
    }

     
    if (SOC_DPP_DEFS_GET(unit, nof_cores) == 2) {
       if (core == 0) {
          if (SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up_core_0 == pp_port) {
             prge_prog_select = ARAD_PRGE_PP_SELECT_ETH_OAM_LOOPBACK_UP;
          }
       }else{
          if (SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up_core_1 == pp_port) {
             prge_prog_select = ARAD_PRGE_PP_SELECT_ETH_OAM_LOOPBACK_UP;
          }
       }
    }else{
       if (SOC_DPP_CONFIG(unit)->pp.oam_rcy_port_up == pp_port) {
           prge_prog_select = ARAD_PRGE_PP_SELECT_ETH_OAM_LOOPBACK_UP;
       }
    }
    if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.bfd_sbfd_enable != 0)) {
        if (SOC_DPP_CONFIG(unit)->pp.sbfd_rcy_port == pp_port) {
            prge_prog_select = ARAD_PRGE_PP_SELECT_SBFD_REFLECTOR;
        }
    }
    pp_pct_tbl_data.prge_profile = prge_prog_select;
    if (((SOC_DPP_CONFIG(unit))->pp.routing_preserve_dscp & SOC_DPP_DSCP_PRESERVE_PORT_CTRL_EN) && info->flags & ARAD_PORTS_PRESERVING_DSCP_PORT_PP_PORT) {
        
        SOCDNX_IF_ERR_EXIT(arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port, &pp_pct_tbl_data_tmp)); 
        pp_pct_tbl_data.prge_var = pp_pct_tbl_data_tmp.prge_var;
    } else {
        pp_pct_tbl_data.prge_var = prge_var;
    }


    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &local_port));
    if(soc_property_port_suffix_num_get(unit, local_port, -1, spn_CUSTOM_FEATURE, "add_size_header", 0))
    {
        pp_pct_tbl_data.prge_profile = 0;
    }

    if (soc_property_port_suffix_num_match_port_get(unit,local_port, -1, spn_CUSTOM_FEATURE, "dedicated_recycle_port", 0) &&  pp_port != 0) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, local_port, &itf_type));
        if (itf_type == SOC_PORT_IF_RCY) {
            pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_INTO_BACKCONE_WITH_TTL_PORT;
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core, pp_port, &pp_pct_tbl_data)); 

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_port_pp_port_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_PP_PORT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, ARAD_PP_PORT_NDX_MAX, ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_PORT_PP_PORT_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_set_verify()", pp_port_ndx, 0);
}


int
  arad_port_pp_port_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  uint32                     pp_port,
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO     *info
  )
{
    uint32
        tbl_data_32b,
        port_type_val; 
    ARAD_PP_EPNI_PP_PCT_TBL_DATA
        pp_pct_tbl_data;
    soc_port_t port;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    ARAD_PORT_PP_PORT_INFO_clear(info);
  
    SOCDNX_IF_ERR_EXIT(READ_IHP_PP_PORT_INFOm(unit, IHP_BLOCK(unit, core), pp_port, &tbl_data_32b));

    
    port_type_val = soc_IHP_PP_PORT_INFOm_field32_get(unit, &tbl_data_32b, DEFAULT_PARSER_PROGRAM_POINTERf);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port));
    SOCDNX_IF_ERR_EXIT(arad_port_parse_header_type_unsafe(
          unit,
          port,
          port_type_val,
          &(info->header_type)
          ));

    SOCDNX_IF_ERR_EXIT(arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port, &pp_pct_tbl_data));
 
    switch(pp_pct_tbl_data.prge_profile) {
        case ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_SPAN:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_ETH;
            break;
        case ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FRC_LITE:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_XGS_DiffServ;
            break;
        case ARAD_PRGE_PP_SELECT_XGS_PE_FROM_FTMH_SPAN:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT;
            break;
        case ARAD_PRGE_PP_SELECT_NONE:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_NONE;
            break;
        case ARAD_PRGE_PP_SELECT_PP_COPY_HEADER_WITH_USER_HEADERS:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_UDH_ETH;
            break;
        case ARAD_PRGE_PP_SELECT_RCY_OVERLAY:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_ETH;
            info->flags |= ARAD_PORTS_PP_PORT_RCY_OVERLAY_PTCH;
            break;
        case ARAD_PRGE_PP_SELECT_PRESERVING_DSCP:
            
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &info->header_type_out));
            break;
        case ARAD_PRGE_PP_SELECT_CH_CONN_TO_FPGA:            
            info->header_type_out = ARAD_PORT_HEADER_TYPE_ETH;            
            break;
        default:
            info->header_type_out = ARAD_PORT_HEADER_TYPE_NONE;
            break;  
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_port_pp_port_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_PP_PORT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pp_port_ndx, ARAD_PP_PORT_NDX_MAX, ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_get_verify()", pp_port_ndx, 0);
}


int
  arad_port_to_pp_port_map_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION        direction_ndx
  )
{
    uint32
        tunnel_id = 0,
        tbl_data = 0,
        memlock = 0,
        pp_port,
        tm_port;
    uint8
        is_xgs_mac_ext;  
    soc_pbmp_t 
        pon_port_bm;
    ARAD_PORTS_XGS_MAC_EXT_INFO
        xgs_mac_ext_info;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA
        dsp_tbl_data;
    int
        core;
    uint16 xgs_mac_ext_encode;
    uint32 pon_tunnel_id_alloc_mode;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pon_port_bm);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);

    if ((direction_ndx == ARAD_PORT_DIRECTION_INCOMING)
        || (direction_ndx == ARAD_PORT_DIRECTION_BOTH)) {
        
        SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_ENCODED(tm_port), &tbl_data));
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data, IN_PP_PORTf, pp_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_ENCODED(tm_port), &tbl_data));

        if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit) == 0) {
            if ((SOC_PBMP_MEMBER(pon_port_bm, port))) {
                if (pon_tunnel_id_alloc_mode == 0) {
                    for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_NOF_ENTRIES_MODE0(unit, port); tunnel_id++) {
                        SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port, tunnel_id), &tbl_data));
                        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data, IN_PP_PORTf, pp_port);
                        SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port, tunnel_id), &tbl_data));
                    }
                } else if (pon_tunnel_id_alloc_mode == 1) {
                    for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID(unit); tunnel_id++) {
                        SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port, tunnel_id), &tbl_data));
                        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data, IN_PP_PORTf, pp_port);
                        SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port, tunnel_id), &tbl_data));
                    }
                }
            }
        }
        ARAD_PORT_IS_XGS_MAC_EXT_PORT(tm_port, is_xgs_mac_ext)
        if (is_xgs_mac_ext) 
        {           
            
            SOCDNX_IF_ERR_EXIT(arad_ports_local_to_xgs_mac_extender_info_get(unit, tm_port, core, &xgs_mac_ext_info));

            xgs_mac_ext_encode = arad_virtual_port_xgs_mac_ext_encode(unit, xgs_mac_ext_info.ptc_port, xgs_mac_ext_info.hg_remote_port);

            SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data));
            soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data, IN_PP_PORTf, pp_port);
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data));
        }
    }

    if ((direction_ndx == ARAD_PORT_DIRECTION_OUTGOING)
        || (direction_ndx == ARAD_PORT_DIRECTION_BOTH)) {
        

        MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
        memlock = 1;

        SOCDNX_IF_ERR_EXIT(arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core, tm_port,  &dsp_tbl_data));
        dsp_tbl_data.out_pp_port = pp_port;
        SOCDNX_IF_ERR_EXIT(arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core,tm_port,  &dsp_tbl_data));

        memlock = 0;
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }

exit:
    if(memlock) {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOCDNX_FUNC_RETURN;
}

int
  arad_port_to_pp_port_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  )
{
    ARAD_PORT_HEADER_TYPE
        header_type_outgoing,
        header_type_incoming;
    ARAD_PORT_PP_PORT_INFO
        pp_port_info;
    uint32
        pp_port,
        tm_port;
    int
        core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    SOCDNX_IF_ERR_EXIT(arad_ports_fap_port_id_cud_extension_verify(unit, tm_port));

    
    SOCDNX_IF_ERR_EXIT(arad_port_header_type_get_unsafe(
          unit,
          core,
          tm_port,
          &header_type_incoming,
          &header_type_outgoing
        ));


    ARAD_PORT_PP_PORT_INFO_clear(&pp_port_info);
    SOCDNX_IF_ERR_EXIT(arad_port_pp_port_get_unsafe(
          unit,
          core,
          pp_port,
          &pp_port_info
        ));

    
    if ((!arad_port_header_type_is_injected_any(header_type_incoming)) && (pp_port != ARAD_OLP_PORT_ID))
    {
        if (pp_port_info.header_type != header_type_incoming)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_PORTS_PROFILE_IS_HEADER_TYPE_INCONSISTENT_ERR")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_port_to_pp_port_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_PP_PORT_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PORT_NDX_MAX(unit), ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_to_pp_port_map_get_verify()", port_ndx, 0);
}


uint32
  arad_port_to_pp_port_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  )
{
  uint32
      dsp_ptr_map[2] = {0, 0},
    tbl_data,
    pp_port;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_PP_PORT_MAP_GET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2080, exit, READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, ARAD_PORTS_VIRTUAL_PORT_ENCODED(port_ndx), &tbl_data));
  pp_port = soc_IHP_VIRTUAL_PORT_TABLEm_field32_get(unit, &tbl_data, IN_PP_PORTf);
  *pp_port_in = pp_port;

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2090, exit, READ_EGQ_DSP_PTR_MAPm(unit, EGQ_BLOCK(unit, core_id), port_ndx, dsp_ptr_map));
  pp_port = soc_EGQ_DSP_PTR_MAPm_field32_get(unit, dsp_ptr_map, OUT_PP_PORTf);
  *pp_port_out = pp_port;

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_to_pp_port_map_get_unsafe()", port_ndx, 0);
}


uint32 
  arad_port_direct_lb_key_set( 
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 local_port,
    SOC_SAND_IN uint32 min_lb_key,
    SOC_SAND_IN uint32 set_min,
    SOC_SAND_IN uint32 max_lb_key,
    SOC_SAND_IN uint32 set_max
   )
{
  int 
      res;
  uint32 
    base_q_pair = 0,
    nof_pairs,
    curr_q_pair;
  ARAD_EGQ_PPCT_TBL_DATA
      egq_pp_ppct_data;
#ifdef BCM_88660_A0
  uint32 use_table_2 = 0x0, field_val; 
  ARAD_PER_PORT_LB_RANGE_TBL_DATA egq_per_port_lb_range_tbl_data;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, local_port, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit); 
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_id, local_port, &nof_pairs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit); 

#ifdef BCM_88660_A0
    
    if (SOC_IS_ARADPLUS(unit)) {
        if(SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) {
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf, &field_val));
             
            if (field_val == 0) {
                use_table_2 = 1;
            }

       }        
    } 
#endif 

  
  for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
  {
#ifdef BCM_88660_A0

    if(use_table_2 == 1) {
       
         res = arad_egq_per_port_lb_range_tbl_get_unsafe(
                    unit,
                    curr_q_pair,
                    &egq_per_port_lb_range_tbl_data
                  ); 
                  
         if (0x1 == set_min) {
             egq_per_port_lb_range_tbl_data.lb_key_min = min_lb_key | 0x80; 
         }

         if (0x1 == set_max) {
             egq_per_port_lb_range_tbl_data.lb_key_max = max_lb_key | 0x80; 
         }

        res = arad_egq_per_port_lb_range_tbl_set_unsafe(
                    unit,
                    curr_q_pair,
                    &egq_per_port_lb_range_tbl_data
                  );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

    } else 
#endif 
    {
        res = arad_egq_ppct_tbl_get_unsafe(
                        unit,
                        core_id,
                        curr_q_pair,
                        &egq_pp_ppct_data
                      );

        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        
        if (0x1 == set_min) {
            egq_pp_ppct_data.lb_key_min = min_lb_key;
        }

        if (0x1 == set_max) {
            egq_pp_ppct_data.lb_key_max = max_lb_key;
        }

        res = arad_egq_ppct_tbl_set_unsafe(
                    unit,
                    SOC_CORE_ALL,
                    curr_q_pair,
                    &egq_pp_ppct_data
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_set()", 0, 0);
  
}

uint32 
  arad_port_direct_lb_key_min_set_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 min_lb_key
   )
{
  int 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core_id, local_port, min_lb_key,1,0,0)));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_min_set_unsafe()", 0, 0);
}

uint32 
  arad_port_direct_lb_key_max_set_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 max_lb_key
   )
{
  int 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core_id, local_port,0,0,max_lb_key,1)));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_max_set_unsafe()", 0, 0);
}

STATIC uint32
    _arad_ports_lag_members_get_num_of_replication(int unit, uint32 lag_ndx, uint32 member_id, uint32 sys_port, uint32 lag_size, uint32* nof_rep)
{
  uint32
    res,
    i,
    sys_phys_port_id;
  ARAD_IRR_LAG_MAPPING_TBL_DATA
    irr_lag_mapping_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *nof_rep = 0;

    for(i=0; i<lag_size; ++i) 
    {
        res = arad_irr_lag_mapping_tbl_get_unsafe(unit, lag_ndx, i, &irr_lag_mapping_tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
        sys_phys_port_id = irr_lag_mapping_tbl_data.destination & ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
        
        if(sys_phys_port_id == sys_port) 
        {
            if(i < member_id) 
            {
                 
                break;    
            } else {
                ++(*nof_rep); 
            }
        }
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_members_get_num_of_replication()",lag_ndx,member_id);
}

uint32 arad_ports_lag_members_ranges_clear_unsafe(
        SOC_SAND_IN  int        unit,
        SOC_SAND_IN  uint32     lag_ndx
    )
{
    int first_replication_ndx = -1;
    int core = SOC_CORE_INVALID;

    uint32      sys_fap_id_self;
    uint32      nof_members;
    uint32      current_member_ndx;
    uint32      fap_id_current_member;
    uint32      port_id_current_member;
    
    SOC_PPC_LAG_INFO    *lag_info = NULL;
    SOC_PPC_LAG_MEMBER  *current_member;


    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    lag_info = sal_alloc(sizeof(SOC_PPC_LAG_INFO), "lag_info");
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, lag_ndx, lag_info));
    nof_members = lag_info->nof_entries;
    
    for(current_member_ndx = 0; current_member_ndx < nof_members; ++current_member_ndx)
    {
        current_member = &lag_info->members[current_member_ndx];

        
        SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_first_replication_index(unit, lag_ndx, current_member, &first_replication_ndx));
        
        
        if((current_member_ndx != first_replication_ndx) || (current_member->flags & (SOC_PPC_LAG_MEMBER_EGRESS_DISABLE | SOC_PPC_LAG_MEMBER_INGRESS_DISABLE))){
            continue;
        }

        
        SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, current_member->sys_port, &fap_id_current_member, &port_id_current_member));

        
        if ( SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_current_member, sys_fap_id_self) == 0){
            continue;
        }

        
        core = fap_id_current_member - sys_fap_id_self; 
        
        SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core, port_id_current_member, 0, 1 , 0xff, 1 )));
    }

exit:
    sal_free_safe(lag_info);
    SOCDNX_FUNC_RETURN;
}

uint32 arad_ports_lag_members_ranges_set_unsafe(
        SOC_SAND_IN  int        unit,
        SOC_SAND_IN  uint32     lag_ndx,
        SOC_SAND_IN  uint32     lag_size,
        SOC_SAND_IN  uint32     sys_port,
        SOC_SAND_IN  uint32     remove
    )
{
    int core = SOC_CORE_INVALID;
    uint32 res;
    uint32 fap_id;
    uint32 port_id = 0;
    uint32 lag_info_idx;
    uint32 sys_port_curr;
    uint32 member_id;
    uint32 sys_fap_id_self;
    uint32 lag_rng_size=0;
    uint32 remain=0;
    uint32 lb_key_max;
    uint32 curr_rng=0;
    uint32 mem_rep;
    uint32 lb_range_size;
    ARAD_IRR_LAG_MAPPING_TBL_DATA irr_lag_mapping_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    lb_range_size = ARAD_PORT_LAG_LB_RNG_SIZE(unit);  
  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    if (lag_size)
    {
        lag_rng_size = lb_range_size / lag_size;
        remain = lb_range_size - lag_rng_size *lag_size;
        for (lag_info_idx = 0; lag_info_idx < lag_size; lag_info_idx++)
        {
            member_id = (lag_size - lag_info_idx) - 1;
            res = arad_irr_lag_mapping_tbl_get_unsafe( unit, lag_ndx, member_id, &irr_lag_mapping_tbl_data );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            sys_port_curr = irr_lag_mapping_tbl_data.destination & ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
            
            res = _arad_ports_lag_members_get_num_of_replication(unit, lag_ndx, member_id, sys_port_curr, lag_size, &mem_rep);
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
            
            if(mem_rep == 0) {
                continue;
            }

            res = arad_sys_phys_to_local_port_map_get_unsafe( unit, sys_port_curr, &fap_id, &port_id );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id, sys_fap_id_self) == 1)
            {
                lb_key_max = curr_rng + (lag_rng_size)*mem_rep -1;
                
                if (remain)
                {
                    if(remain > mem_rep) {
                        lb_key_max += mem_rep;
                        remain -= mem_rep;
                    } else {
                        lb_key_max += remain;
                        remain = 0;
                    }
                }
                core = fap_id - sys_fap_id_self;

                res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core, port_id, curr_rng, 1 , lb_key_max, 1 )));
                SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
                curr_rng = lb_key_max + 1;
            } 
            else
            {
                curr_rng += (lag_rng_size * mem_rep);
                if (remain)
                {
                    if(remain > mem_rep) {
                        curr_rng += mem_rep;
                        remain -= mem_rep;
                    } else {
                        curr_rng += remain;
                        remain = 0;
                    }
                }
            }
        }
    }
  
    if (remove)
    {
        res = arad_sys_phys_to_local_port_map_get_unsafe( unit, sys_port, &fap_id, &port_id );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id, sys_fap_id_self) == 1)
        {
            core = fap_id - sys_fap_id_self; 
            
            res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core, port_id, 0, 1 , 0xff, 1 )));
            SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_members_ranges_set_unsafe()",lag_ndx,0);
}


int arad_ports_lag_check_if_member_is_only_lag_member_in_fap(
    int unit, 
    SOC_SAND_IN SOC_PPC_LAG_INFO *lag_info, 
    SOC_SAND_IN ARAD_PORTS_LAG_MEMBER *checked_lag_member, 
    int* member_is_only_lag_member_in_fap)
{
    int lag_info_idx;
    int nof_members_in_lag;
    uint32 fap_id;
    uint32 port_id;
    uint32 current_sys_port;
    uint32 refernce_sys_port;
    uint32 sys_fap_id_self;

    SOCDNX_INIT_FUNC_DEFS;

    nof_members_in_lag = lag_info->nof_entries;
    refernce_sys_port = checked_lag_member->sys_port;
    *member_is_only_lag_member_in_fap = TRUE;

    
    SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    for (lag_info_idx = 0; lag_info_idx < nof_members_in_lag; lag_info_idx++)
    {
        current_sys_port = lag_info->members[lag_info_idx].sys_port;
        if (current_sys_port == refernce_sys_port)
        {
            
            continue;
        }

        SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, current_sys_port, &fap_id, &port_id));
        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id, sys_fap_id_self) == 1) 
        {
            *member_is_only_lag_member_in_fap = FALSE;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int arad_ports_irr_lag_to_lag_range_tbl_data_range_fix_for_hw(int unit, uint32 lag_mode, int actual_range)
{
    if ( ((lag_mode == SOC_PPC_LAG_LB_TYPE_ROUND_ROBIN) || (_arad_ports_is_smooth_division_used(unit)) || (SOC_IS_ARADPLUS(unit))) && (actual_range != 0) )
    {
        return (actual_range - 1); 
    } else {
        return actual_range; 
    }
}



int arad_ports_lag_member_add_unsafe(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  lag_ndx,
    SOC_SAND_IN ARAD_PORTS_LAG_MEMBER   *add_lag_member
    )
{

    int nof_replications;
    int last_replicated_member_index;
    int core_id_add_lag_member;
    int nof_enabled_members;
    uint32 fap_id_add_lag_member;    
    uint32 port_id_add_lag_member;
    uint32 sys_fap_id_self;
    uint32 dummy_sys_phys_id = 0;
    uint32 sys_logic_port_id;
    uint32 memlock = 0;
    uint32 lag_is_mine_data[2];    
    SOC_PPC_LAG_INFO lag_info;
    SOC_PPC_LAG_MEMBER lag_member_sw_db_format;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA egq_dsp_ptr_map_tbl_data;
    ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA irr_lag_to_lag_range_tbl_data;
    ARAD_IRR_LAG_MAPPING_TBL_DATA irr_lag_mapping_tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    lag_member_sw_db_format.sys_port    = add_lag_member->sys_port;
    lag_member_sw_db_format.member_id   = add_lag_member->member_id;
    lag_member_sw_db_format.flags       = add_lag_member->flags;

    
    SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, add_lag_member->sys_port, &fap_id_add_lag_member, &port_id_add_lag_member));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, lag_ndx, &lag_info));
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_nof_replications(unit, lag_ndx, &lag_member_sw_db_format, &nof_replications, &last_replicated_member_index));
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_nof_enabled_members(unit, lag_ndx, &nof_enabled_members));
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_add(unit, lag_ndx, &lag_member_sw_db_format));

    if( nof_replications == 0 ) 
    {
        
        if( SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_add_lag_member, sys_fap_id_self) == 1 )
        {
            SOCDNX_SAND_IF_ERR_EXIT( arad_ports_logical_sys_id_build( unit, TRUE, lag_ndx, lag_info.nof_entries, dummy_sys_phys_id, &sys_logic_port_id));
            core_id_add_lag_member = fap_id_add_lag_member - sys_fap_id_self;
            
            
            MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
            memlock = 1;
            SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id_add_lag_member, port_id_add_lag_member, &egq_dsp_ptr_map_tbl_data));
            egq_dsp_ptr_map_tbl_data.dst_system_port = sys_logic_port_id;
            SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id_add_lag_member, port_id_add_lag_member, &egq_dsp_ptr_map_tbl_data));
            memlock = 0;
            MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);

            
            SOCDNX_SAND_IF_ERR_EXIT( arad_port_src_sys_port_set_unsafe(unit, core_id_add_lag_member, port_id_add_lag_member, sys_logic_port_id));

            
            if (!SOC_IS_ARDON(unit)) 
            {
                SOCDNX_SAND_IF_ERR_EXIT( READ_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
                SHR_BITSET_RANGE(lag_is_mine_data, (lag_ndx % 32), 1);
                SOCDNX_SAND_IF_ERR_EXIT( WRITE_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
            }
        }
    }

    if( (add_lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE) == 0x0 )
    {
        
        
        irr_lag_mapping_tbl_data.destination = add_lag_member->sys_port | INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;

        
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_set_unsafe(unit, lag_ndx, nof_enabled_members, &irr_lag_mapping_tbl_data));

        
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_get_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));
        ++nof_enabled_members;
        irr_lag_to_lag_range_tbl_data.range = arad_ports_irr_lag_to_lag_range_tbl_data_range_fix_for_hw(unit, lag_info.lb_type, nof_enabled_members);
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_set_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));

        
        if(nof_enabled_members == 1)
        {
            SOCDNX_SAND_IF_ERR_EXIT( sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.set(unit, lag_ndx, 0x1));
        }

        SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_members_ranges_set_unsafe(unit, lag_ndx, nof_enabled_members, add_lag_member->sys_port, FALSE));
    }

exit:
    if (memlock)  {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOCDNX_FUNC_RETURN;
}




int arad_ports_lag_sys_port_remove_unsafe( 
    SOC_SAND_IN int                     unit, 
    SOC_SAND_IN uint32                  lag_ndx, 
    SOC_SAND_IN ARAD_PORTS_LAG_MEMBER   *removed_lag_member
    )
{
    int nof_replications;
    int last_replicated_member_index;
    int core_id_removed_lag_member;
    int core_id_moved_lag_member;
    int nof_enabled_members;
    int removed_member_has_disable_flag;
    int need_to_disable_not_to_full_remove;
    int member_is_only_lag_member_in_fap;
    int is_stateful;
    int moved_lag_member_index;
    int removed_member_index = 0;
    int moved_lag_member_last_replication_index;
    int moved_lag_member_nof_replications;
    int moved_lag_member_first_replication_index;
    int i;
    uint32 fap_id_removed_lag_member;    
    uint32 fap_id_moved_lag_member;
    uint32 port_id_removed_lag_member;
    uint32 port_id_moved_lag_member;
    uint32 sys_fap_id_self;
    uint32 dummy_sys_phys_id = 0;
    uint32 sys_logic_port_id;
    uint32 memlock = 0;
    uint32 lag_is_mine_data[2];    
    SOC_PPC_LAG_INFO lag_info;
    SOC_PPC_LAG_MEMBER lag_member_sw_db_format;
    SOC_PPC_LAG_MEMBER *moved_lag_member;
    ARAD_SYSPORT org_sys_port;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA egq_dsp_ptr_map_tbl_data;
    ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA irr_lag_to_lag_range_tbl_data;
    ARAD_IRR_LAG_MAPPING_TBL_DATA irr_lag_mapping_tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    lag_member_sw_db_format.sys_port = removed_lag_member->sys_port;
    lag_member_sw_db_format.member_id = removed_lag_member->member_id;
    lag_member_sw_db_format.flags = removed_lag_member->flags;

    
    SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, removed_lag_member->sys_port, &fap_id_removed_lag_member, &port_id_removed_lag_member));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, lag_ndx, &lag_info));
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_nof_replications(unit, lag_ndx, &lag_member_sw_db_format, &nof_replications, &last_replicated_member_index));
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_nof_enabled_members(unit, lag_ndx, &nof_enabled_members));

    
    
    if(nof_replications == 0)
    {
        
        LOG_INFO(BSL_LS_SOC_TRUNK, (BSL_META_U(unit, "%s: Member (0x%u) not found in lag(id=%d) - nothing to do\n"), FUNCTION_NAME(), removed_lag_member->sys_port, lag_ndx));
        SOC_EXIT;
    }

    removed_member_has_disable_flag = (lag_info.members[last_replicated_member_index].flags & (SOC_PPC_LAG_MEMBER_EGRESS_DISABLE | SOC_PPC_LAG_MEMBER_INGRESS_DISABLE));
    need_to_disable_not_to_full_remove = (removed_lag_member->flags & SOC_PPC_LAG_MEMBER_EGRESS_DISABLE);

    
    if( need_to_disable_not_to_full_remove )
    {
        
        if(nof_replications > 1)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS, (_BSL_SOC_MSG("%s: Failed.  EGRESS_DISABLEd remove of members is allowed only when one replication in lag(id=%d)\n"), FUNCTION_NAME(), lag_ndx));
        } else if( removed_member_has_disable_flag ) {
            
            LOG_INFO(BSL_LS_SOC_TRUNK, (BSL_META_U(unit, "%s: Member (0x%u) found only once lag(id=%d) and is already disabled - nothing to do\n"), FUNCTION_NAME(), removed_lag_member->sys_port, lag_ndx));
            SOC_EXIT;
        }
    }

    
    is_stateful = lag_info.is_stateful;

    
    
    if( ! removed_member_has_disable_flag )
    {

        
        
        
        moved_lag_member_index = nof_enabled_members - 1;
        
        for (i = 0; i < nof_enabled_members; ++i) { 
            SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_get_unsafe(unit, lag_ndx, i, &irr_lag_mapping_tbl_data)); 
            
            irr_lag_mapping_tbl_data.destination &= ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
            
            if (irr_lag_mapping_tbl_data.destination == removed_lag_member->sys_port) { 
                removed_member_index = i; 
                break; 
            }
        }

        
        if (i == nof_enabled_members) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOC_MSG("%s: Failed.  can't find lag member in IRR_LAG_MAPPING tbl\n"), FUNCTION_NAME()));
        }

        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_get_unsafe(unit, lag_ndx, moved_lag_member_index, &irr_lag_mapping_tbl_data));
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_set_unsafe(unit, lag_ndx, removed_member_index, &irr_lag_mapping_tbl_data));

        
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_get_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));
        --nof_enabled_members;
        irr_lag_to_lag_range_tbl_data.range = arad_ports_irr_lag_to_lag_range_tbl_data_range_fix_for_hw(unit, lag_info.lb_type, nof_enabled_members);
        SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_set_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));

        
        
        if (!is_stateful || (SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "extra_hash_enable", 0))) 
        {
            
            sal_usleep(10);
            
            irr_lag_mapping_tbl_data.destination = ARAD_SYS_PHYS_PORT_INVALID(unit) | INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
            SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_set_unsafe(unit, lag_ndx, moved_lag_member_index, &irr_lag_mapping_tbl_data));
        }

        if(nof_enabled_members == 0)
        {
            SOCDNX_SAND_IF_ERR_EXIT( sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.set(unit, lag_ndx, 0x0));
        }

        SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_members_ranges_set_unsafe(unit, lag_ndx, nof_enabled_members, removed_lag_member->sys_port, nof_replications == 1 ? TRUE : FALSE));
    }

    
    if (need_to_disable_not_to_full_remove == 0)
    {
        
        if (nof_replications == 1) 
        {
            
            if ( SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_removed_lag_member, sys_fap_id_self) == 1)
            {
                
                SOCDNX_SAND_IF_ERR_EXIT( arad_sw_db_modport2sysport_get(unit, fap_id_removed_lag_member, port_id_removed_lag_member, &org_sys_port));
                core_id_removed_lag_member = fap_id_removed_lag_member - sys_fap_id_self;
    
                
                SOCDNX_SAND_IF_ERR_EXIT( arad_port_src_sys_port_set_unsafe(unit, core_id_removed_lag_member, port_id_removed_lag_member, org_sys_port));
    
                
                MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
                memlock = 1;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id_removed_lag_member, port_id_removed_lag_member, &egq_dsp_ptr_map_tbl_data));
                egq_dsp_ptr_map_tbl_data.dst_system_port = org_sys_port;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id_removed_lag_member, port_id_removed_lag_member, &egq_dsp_ptr_map_tbl_data));
                memlock = 0;
                MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    
                if (!SOC_IS_ARDON(unit)) 
                {
                    SOCDNX_IF_ERR_EXIT( arad_ports_lag_check_if_member_is_only_lag_member_in_fap(unit, &lag_info, removed_lag_member, &member_is_only_lag_member_in_fap));
                    if(member_is_only_lag_member_in_fap == TRUE)
                    {
                        SOCDNX_SAND_IF_ERR_EXIT( READ_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
                        SHR_BITCLR_RANGE(lag_is_mine_data, (lag_ndx % 32), 1);
                        SOCDNX_SAND_IF_ERR_EXIT( WRITE_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
                    }
                }
            }
        }    

        
        
        
        
        
        removed_member_index   = last_replicated_member_index;
        moved_lag_member_index = lag_info.nof_entries - 1;
        if( removed_member_index != moved_lag_member_index)
        {
            moved_lag_member = &(lag_info.members[moved_lag_member_index]);
            SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_nof_replications(unit, lag_ndx, moved_lag_member, &moved_lag_member_nof_replications, &moved_lag_member_last_replication_index));
            SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_first_replication_index(unit, lag_ndx, moved_lag_member, &moved_lag_member_first_replication_index));
            
            if(( moved_lag_member_nof_replications == 1) || ( (moved_lag_member_first_replication_index != -1) && (moved_lag_member_first_replication_index > removed_member_index) ))
            {
                
                SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, moved_lag_member->sys_port, &fap_id_moved_lag_member, &port_id_moved_lag_member));
                
                if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_moved_lag_member, sys_fap_id_self) == 1)
                {                    
                    SOCDNX_SAND_IF_ERR_EXIT( arad_ports_logical_sys_id_build( unit, TRUE, lag_ndx, removed_member_index, dummy_sys_phys_id, &sys_logic_port_id));
                    core_id_moved_lag_member = fap_id_moved_lag_member - sys_fap_id_self;

                    
                    SOCDNX_SAND_IF_ERR_EXIT( arad_port_src_sys_port_set_unsafe(unit, core_id_moved_lag_member, port_id_moved_lag_member, sys_logic_port_id));

                    
                    MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
                    memlock = 1;
                    SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id_moved_lag_member, port_id_moved_lag_member, &egq_dsp_ptr_map_tbl_data));
                    egq_dsp_ptr_map_tbl_data.dst_system_port = sys_logic_port_id;
                    SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id_moved_lag_member, port_id_moved_lag_member, &egq_dsp_ptr_map_tbl_data));
                    memlock = 0;
                    MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
                }
            }
        }
    }

    SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_fix_next_member_pionter(unit, lag_ndx));

    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_remove(unit, lag_ndx, &lag_member_sw_db_format));

exit:
    if (memlock)  {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOCDNX_FUNC_RETURN; 
}


 
uint32  
    arad_ports_is_port_lag_member_unsafe( 
        SOC_SAND_IN  int                                 unit, 
        SOC_SAND_IN  int                                 core_id,
        SOC_SAND_IN  uint32                                 port_id, 
        SOC_SAND_OUT uint8                                 *is_in_lag, 
        SOC_SAND_OUT uint32                                 *lag_id) 
{         
         
  
    uint32  
            res, 
            lag_member_id, 
            sys_phys_port_id, 
            dsp_ptr_map[2] = {0, 0}; 
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
  
    SOC_SAND_ERR_IF_ABOVE_MAX(port_id, ARAD_PORT_NDX_MAX(unit), ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit); 
  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_EGQ_DSP_PTR_MAPm(unit, EGQ_BLOCK(unit, core_id), port_id, dsp_ptr_map));  
  
       
     res = arad_ports_logical_sys_id_parse( 
                unit,  
                soc_EGQ_DSP_PTR_MAPm_field32_get(unit, dsp_ptr_map, DST_SYSTEM_PORTf), 
                is_in_lag, 
                lag_id, 
                &(lag_member_id), 
                &sys_phys_port_id); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); 
  
exit: 
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_is_port_lag_member_unsafe()",unit,port_id); 
  
} 


int arad_ports_lag_fix_next_member_pionter(int unit, uint32 lag_ndx)
{
    int array_mem_idx;
    int max_idx;
    uint32 orig_val;
    uint32 mem32_val;
    uint32 field_val = 0;
    soc_reg_t dynamic_memory_access_reg;
    soc_mem_t mem = INVALIDm;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    dynamic_memory_access_reg = SOC_IS_QAX(unit) ? TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr : IRR_ENABLE_DYNAMIC_MEMORY_ACCESSr;
    SOCDNX_IF_ERR_EXIT( soc_reg32_get(unit, dynamic_memory_access_reg, REG_PORT_ANY, 0, &orig_val));
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, dynamic_memory_access_reg, REG_PORT_ANY, 0, 1));

    
    mem = SOC_IS_QAX(unit) ? TAR_LAG_NEXT_MEMBERm : IRR_LAG_NEXT_MEMBERm;
    max_idx = SOC_MEM_IS_ARRAY(unit, mem) ? SOC_MEM_NUMELS(unit, mem) : 1;
    for (array_mem_idx = 0; array_mem_idx < max_idx; ++array_mem_idx)
    {
        SOCDNX_IF_ERR_EXIT( soc_mem_array_read(unit, mem, array_mem_idx, MEM_BLOCK_ANY, lag_ndx, &mem32_val));
        soc_mem_field_set(unit, mem, &mem32_val, OFFSETf, &field_val);
        SOCDNX_IF_ERR_EXIT( soc_mem_array_write(unit, mem, array_mem_idx, MEM_BLOCK_ALL, lag_ndx, &mem32_val));
    }

    
    SOCDNX_IF_ERR_EXIT( soc_reg32_set(unit, dynamic_memory_access_reg, REG_PORT_ANY, 0, orig_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int arad_ports_lag_check_if_member_in_lag(int unit, CONST SOC_PPC_LAG_INFO *lag_info, CONST SOC_PPC_LAG_MEMBER *lag_member, int *member_found_in_lag)
{
    int nof_members_in_lag = 0;
    int current_member_index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(lag_info);
    SOCDNX_NULL_CHECK(lag_member);
    SOCDNX_NULL_CHECK(member_found_in_lag);

    *member_found_in_lag = 0;
    nof_members_in_lag = lag_info->nof_entries;
    for (current_member_index = 0; current_member_index < nof_members_in_lag; ++current_member_index) 
    {
        if (lag_info->members[current_member_index].sys_port == lag_member->sys_port) 
        {
            *member_found_in_lag = 1;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_ports_lag_set_unsafe(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  uint32                 lag_ndx,
        SOC_SAND_IN  SOC_PPC_LAG_INFO    *new_lag_info
    )
{
    int current_member_index;
    int current_member_first_replication_index;
    int nof_members_in_lag;
    int core_id_current_lag_member;
    int port_is_mine = FALSE;
    int nof_disabled_members = 0;
    int nof_enabled_members = 0;
    int old_member_found_in_new_lag = 0;
    uint32 fap_id_current_lag_member;
    uint32 port_id_current_lag_member;
    uint32 sys_fap_id_self;
    uint32 dummy_sys_phys_id = 0;
    uint32 sys_logic_port_id;
    uint32 memlock = 0;
    uint32 lag_is_mine_data[2];    
    SOC_PPC_LAG_INFO old_lag_info;
    const SOC_PPC_LAG_MEMBER *current_lag_member;
    ARAD_SYSPORT org_sys_port;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA egq_dsp_ptr_map_tbl_data;
    ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA irr_lag_to_lag_range_tbl_data;
    ARAD_IRR_LAG_MAPPING_TBL_DATA irr_lag_mapping_tbl_data;
    uint32 nof_lag_entries = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT( MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get(unit, lag_ndx, &old_lag_info));
    nof_members_in_lag = old_lag_info.nof_entries;

    
    SOCDNX_IF_ERR_EXIT( arad_ports_lag_members_ranges_clear_unsafe(unit, lag_ndx));

    for(current_member_index = 0; current_member_index < nof_members_in_lag; ++current_member_index)
    {
        current_lag_member = &(old_lag_info.members[current_member_index]);
        
        SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, current_lag_member->sys_port, &fap_id_current_lag_member, &port_id_current_lag_member));
        
        if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_current_lag_member, sys_fap_id_self) == 1)
        {                    
            
            SOCDNX_SAND_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_first_replication_index(unit, lag_ndx, current_lag_member, &current_member_first_replication_index));
            
            if(current_member_first_replication_index == current_member_index)
            {                
                
                SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_check_if_member_in_lag(unit, new_lag_info, current_lag_member, &old_member_found_in_new_lag));
                if (old_member_found_in_new_lag) 
                {
                    continue;
                }

                
                SOCDNX_SAND_IF_ERR_EXIT( arad_sw_db_modport2sysport_get(unit, fap_id_current_lag_member, port_id_current_lag_member, &org_sys_port));
                core_id_current_lag_member = fap_id_current_lag_member - sys_fap_id_self;

                
                SOCDNX_SAND_IF_ERR_EXIT( arad_port_src_sys_port_set_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, org_sys_port));

                
                MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
                memlock = 1;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, &egq_dsp_ptr_map_tbl_data));
                egq_dsp_ptr_map_tbl_data.dst_system_port = org_sys_port;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, &egq_dsp_ptr_map_tbl_data));
                memlock = 0;
                MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
            }
        }
    }

    
    SOCDNX_IF_ERR_EXIT( soc_dpp_trunk_sw_db_set(unit, lag_ndx, new_lag_info));

    
    nof_lag_entries = arad_ports_lag_nof_lag_entries_get_unsafe(unit);
    for(current_member_index = 0; current_member_index < nof_lag_entries; ++current_member_index)
    {
        irr_lag_mapping_tbl_data.destination = 0;
        SOCDNX_SAND_IF_ERR_EXIT(arad_irr_lag_mapping_tbl_set_unsafe(unit, lag_ndx, current_member_index, &irr_lag_mapping_tbl_data));
    }

    nof_members_in_lag = new_lag_info->nof_entries;
    for(current_member_index = 0; current_member_index < nof_members_in_lag; ++current_member_index)
    {
        current_lag_member = &(new_lag_info->members[current_member_index]);
        
        SOCDNX_SAND_IF_ERR_EXIT( arad_sys_phys_to_local_port_map_get_unsafe(unit, current_lag_member->sys_port, &fap_id_current_lag_member, &port_id_current_lag_member));
        
        if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id_current_lag_member, sys_fap_id_self) == 1)
        {                    
            
            SOCDNX_SAND_IF_ERR_EXIT( soc_dpp_trunk_sw_db_get_first_replication_index(unit, lag_ndx, current_lag_member, &current_member_first_replication_index));
            
            if(current_member_first_replication_index == current_member_index)
            {                
                SOCDNX_SAND_IF_ERR_EXIT( arad_ports_logical_sys_id_build( unit, TRUE, lag_ndx, current_member_index, dummy_sys_phys_id, &sys_logic_port_id));
                core_id_current_lag_member = fap_id_current_lag_member - sys_fap_id_self;

                
                SOCDNX_SAND_IF_ERR_EXIT( arad_port_src_sys_port_set_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, sys_logic_port_id));

                
                MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
                memlock = 1;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, &egq_dsp_ptr_map_tbl_data));
                egq_dsp_ptr_map_tbl_data.dst_system_port = sys_logic_port_id;
                SOCDNX_IF_ERR_EXIT( arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit, core_id_current_lag_member, port_id_current_lag_member, &egq_dsp_ptr_map_tbl_data));
                memlock = 0;
                MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);

                
                port_is_mine = TRUE;
            }
        }

        if( (current_lag_member->flags & SOC_PPC_LAG_MEMBER_INGRESS_DISABLE) == 0x0 )
        {
            
            irr_lag_mapping_tbl_data.destination = current_lag_member->sys_port | INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;

            
            SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_mapping_tbl_set_unsafe(unit, lag_ndx, current_member_index - nof_disabled_members, &irr_lag_mapping_tbl_data));
        } else {
            ++nof_disabled_members;
        }
    }
    
    
    if (!SOC_IS_ARDON(unit)) 
    {
        SOCDNX_SAND_IF_ERR_EXIT( READ_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
        if(port_is_mine)
        {
            SHR_BITSET_RANGE(lag_is_mine_data, (lag_ndx % 32), 1);
        } else {
            SHR_BITCLR_RANGE(lag_is_mine_data, (lag_ndx % 32), 1);
        }
        SOCDNX_SAND_IF_ERR_EXIT( WRITE_PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm(unit, MEM_BLOCK_ALL, lag_ndx/32, lag_is_mine_data));
    }

    
    SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_get_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));
    nof_enabled_members = nof_members_in_lag - nof_disabled_members;
    irr_lag_to_lag_range_tbl_data.range = arad_ports_irr_lag_to_lag_range_tbl_data_range_fix_for_hw(unit, new_lag_info->lb_type, nof_members_in_lag - nof_disabled_members);
    SOCDNX_SAND_IF_ERR_EXIT( arad_irr_lag_to_lag_range_tbl_set_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data));

    
    SOCDNX_SAND_IF_ERR_EXIT( sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.set(unit, lag_ndx, nof_enabled_members ? 1 : 0));

    SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_fix_next_member_pionter(unit, lag_ndx));

    
    SOCDNX_SAND_IF_ERR_EXIT( arad_ports_lag_members_ranges_set_unsafe(unit, lag_ndx, nof_enabled_members, 0x0, FALSE));

exit:
    if (memlock)  {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_ports_lag_get_unsafe(
        SOC_SAND_IN  int                      unit,
        SOC_SAND_IN  uint32                   lag_ndx,
        SOC_SAND_OUT SOC_PPC_LAG_INFO      *in_info,
        SOC_SAND_OUT SOC_PPC_LAG_INFO      *out_info
    )
{
    uint8
        is_lag,
        in_use;
    uint32
        res,
        fap_id,
        port_id = 0,
        lag_info_idx,
        in_nof_entries = 0,
        port_id_curr,
        sys_fap_id_self,
        cur_sys_port_id,
        nof_members,
        lag_id,
        lag_member_id,
        sys_phys_port_id,
        nof_active_cores,
        core;
    ARAD_SYSPORT
        sys_port_curr;
    ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA
        irr_lag_to_lag_range_tbl_data;
    ARAD_IRR_LAG_MAPPING_TBL_DATA
        irr_lag_mapping_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_GET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(in_info);
    SOC_SAND_CHECK_NULL_INPUT(out_info);

    SOC_SAND_ERR_IF_ABOVE_MAX( lag_ndx, ARAD_NOF_LAG_GROUPS-1, ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR, 10, exit);

    
    SOC_PPC_LAG_INFO_clear(in_info);
    arad_ports_lag_mem_id_mark_invalid(in_info);
    SOC_PPC_LAG_INFO_clear(out_info);
    arad_ports_lag_mem_id_mark_invalid(out_info);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

    
    res = sw_state_access[unit].dpp.soc.arad.tm.lag.in_use.get(unit, lag_ndx, &in_use);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 75, exit);

    if (in_use)
    {
        res = arad_irr_lag_to_lag_range_tbl_get_unsafe(unit, lag_ndx, &irr_lag_to_lag_range_tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        
        if ((irr_lag_to_lag_range_tbl_data.mode == 1)   ||
            (_arad_ports_is_smooth_division_used(unit)) ||
            (SOC_IS_ARADPLUS(unit)))
        {
            nof_members = irr_lag_to_lag_range_tbl_data.range + 1; 
        } else {
            nof_members = irr_lag_to_lag_range_tbl_data.range ;
        }

        for (lag_info_idx = 0; lag_info_idx < nof_members; lag_info_idx++)
        {
            res = arad_irr_lag_mapping_tbl_get_unsafe(unit, lag_ndx, lag_info_idx, &irr_lag_mapping_tbl_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            sys_port_curr = irr_lag_mapping_tbl_data.destination & ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
            out_info->members[lag_info_idx].sys_port = sys_port_curr;

            res = arad_sys_phys_to_local_port_map_get_unsafe(unit, sys_port_curr, &fap_id, &port_id );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

            out_info->members[lag_info_idx].member_id = lag_info_idx;
        }
    } else
    {
        nof_members = 0;
    }

    out_info->nof_entries = nof_members;

    
    nof_active_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    for (core = 0; core < nof_active_cores; ++core) 
    {
        for (port_id = 0; port_id < SOC_TMC_NOF_FAP_PORTS_PER_CORE; port_id++) 
        {
            
            
            res = arad_port_src_sys_port_get_unsafe(unit, core, port_id, &cur_sys_port_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            
            res = arad_ports_logical_sys_id_parse(unit, cur_sys_port_id, &is_lag, &lag_id, &lag_member_id, &sys_phys_port_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            if ((is_lag) && (lag_id == lag_ndx))
            {
                
                 res = arad_sw_db_modport2sysport_get(unit, sys_fap_id_self + core, port_id, &sys_port_curr);
                SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

                in_info->members[in_nof_entries].sys_port = sys_port_curr;

                res = arad_sys_phys_to_local_port_map_get_unsafe(unit, sys_port_curr, &fap_id, &port_id_curr);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

                if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id, sys_fap_id_self) == 1)
                {
                    in_info->members[in_nof_entries].member_id = lag_member_id;
                }
                in_nof_entries++;
            }
        } 
    }
    in_info->nof_entries = in_nof_entries;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_get_unsafe()",0,0);
}

uint32
  arad_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32
    res;
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_lag_to_lag_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_SET_UNSAFE);

   

   
    res = arad_irr_lag_to_lag_range_tbl_get_unsafe(
            unit,
            lag_ndx,
            &irr_lag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    irr_lag_to_lag_range_tbl_data.mode =
      SOC_SAND_BOOL2NUM(is_order_preserving);

    res = arad_irr_lag_to_lag_range_tbl_set_unsafe(
            unit,
            lag_ndx,
            &irr_lag_to_lag_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_set_unsafe()",0,0);
}


uint32
  arad_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32
    res;
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_lag_to_lag_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

  res = arad_irr_lag_to_lag_range_tbl_get_unsafe(
          unit,
          lag_ndx,
          &irr_lag_to_lag_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  *is_order_preserving =
    SOC_SAND_NUM2BOOL(irr_lag_to_lag_range_tbl_data.mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_get_unsafe()",0,0);
}

uint32 arad_ports_lag_lb_key_range_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info)
{
    uint32
        res,
        sys_fap_id_self,
        lag_info_idx,
        fap_id,
        port_id,
        lag_size,
        curr_rng = 0x0,
        lag_rng_size, 
        remain,
        lb_key_max,
        lb_range_size;
    int core = SOC_CORE_INVALID;
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    ARAD_DEVICE_CHECK(unit, exit);

    lb_range_size = ARAD_PORT_LAG_LB_RNG_SIZE(unit);

    lag_size = info->nof_entries;
    lag_rng_size = lb_range_size/lag_size;
    remain = lb_range_size%lag_size;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));
    
    for (lag_info_idx = 0; lag_info_idx < lag_size; lag_info_idx++) {

        res = arad_sys_phys_to_local_port_map_get_unsafe(unit, info->members[lag_info_idx].sys_port, &fap_id, &port_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, fap_id, sys_fap_id_self) == 1) { 
            core = fap_id - sys_fap_id_self;
            lb_key_max = (curr_rng + (lag_rng_size - 1));

            
          if (remain) {
              lb_key_max += 1;
              --remain;
          }
            res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core, port_id, curr_rng, 1 , lb_key_max, 1 )));
            SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
            curr_rng = lb_key_max + 1;
      }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_lb_key_range_set_unsafe()",0,0);
}


uint32
  arad_port_egr_hdr_credit_discount_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE    cr_discnt_type
  )
{
  uint32
    res;
  ARAD_EGQ_PCT_TBL_DATA
    data;
  uint32
    base_q_pair,
    nof_pairs,
    curr_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  
  for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
  {
    res = arad_egq_pct_tbl_get_unsafe(
            unit,
            core,
            curr_q_pair,
            &data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20 + curr_q_pair, exit);

    data.cr_adjust_type = cr_discnt_type;

    res = arad_egq_pct_tbl_set_unsafe(
            unit,
            core,
            curr_q_pair,
            &data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30 + curr_q_pair, exit);
    if (SOC_DPP_CONFIG((unit))->tm.queue_level_interface_enable) 
    {
      soc_port_t 
        port;
      soc_port_if_t 
        interface;
      
      res = soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1110, exit);

      res = soc_port_sw_db_interface_type_get(unit, port, &interface);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1111, exit);

      
      if (interface == SOC_PORT_IF_ILKN) 
      {
        int curr_mc_q_pair;

        curr_mc_q_pair = ARAD_EGR_QUEUING_MC_QUEUE_OFFSET(curr_q_pair); 
      
        res = arad_egq_pct_tbl_get_unsafe(
                unit,
                core,
                curr_mc_q_pair,
                &data
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

        data.cr_adjust_type = cr_discnt_type;

        res = arad_egq_pct_tbl_set_unsafe(
                unit,
                core,
                curr_mc_q_pair,
                &data
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 
      }
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_set_unsafe()",tm_port,0);
}


uint32
  arad_port_egr_hdr_credit_discount_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;
  ARAD_EGQ_PCT_TBL_DATA
    data;
  uint32
    base_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

  res = arad_fap_port_id_verify(unit, tm_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  
  res = arad_egq_pct_tbl_get_unsafe(
          unit,
          core,
          base_q_pair,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  *cr_discnt_type = data.cr_adjust_type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_get_unsafe()",tm_port,0);
}


uint32 arad_port_stacking_info_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd)
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PORT_NDX_MAX(unit), ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_stacking, 1, SOC_SAND_GEN_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(peer_tmd, ARAD_NOF_TM_DOMAIN_IN_SYSTEM - 1, SOC_SAND_GEN_ERR, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_stacking_info_verify()", local_port_ndx, peer_tmd);
}

uint32 arad_port_stacking_info_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd)
{
    uint32
        res;
    ARAD_EGQ_PPCT_TBL_DATA
        ppct_tbl_data;
    uint32
        base_q_pair;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, local_port_ndx,  &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_egq_ppct_tbl_get_unsafe(unit, core_id, base_q_pair, &ppct_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    ppct_tbl_data.is_stacking_port =  is_stacking;
    ppct_tbl_data.peer_tm_domain_id =  peer_tmd;

    res = arad_egq_ppct_tbl_set_unsafe(unit, core_id, base_q_pair, &ppct_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    
    if (is_stacking) {
        res = arad_port_stacking_route_history_bitmap_set_unsafe(unit, core_id, local_port_ndx, SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK1, 0x0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_set_unsafe()",local_port_ndx,peer_tmd);
}

uint32 arad_port_stacking_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                              *is_stacking,
    SOC_SAND_OUT  uint32                              *peer_tmd)
{
    uint32
        res,
        base_q_pair;
    ARAD_EGQ_PPCT_TBL_DATA
        ppct_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(is_stacking);
    SOC_SAND_CHECK_NULL_INPUT(peer_tmd);

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, local_port_ndx, &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    res = arad_egq_ppct_tbl_get_unsafe(unit, core_id, base_q_pair, &ppct_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    *is_stacking = ppct_tbl_data.is_stacking_port;
    *peer_tmd = ppct_tbl_data.peer_tm_domain_id;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_get_unsafe()",local_port_ndx,0);
}


uint32 arad_port_stacking_route_history_bitmap_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                           tm_port,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap)
{
    uint32
        res;
    ARAD_EGQ_PPCT_TBL_DATA
        ppct_tbl_data;
    ARAD_PORT_HEADER_TYPE
        header_type_outgoing,
        header_type_incoming;
   ARAD_EGQ_PCT_TBL_DATA
        pct_tbl_data;
    uint32
        base_q_pair,
        nof_pairs,
        curr_q_pair, 
        prog_editor_value;
    uint32 prge_prog_pointer;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, tm_port, &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_id, tm_port, &nof_pairs);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

    
    res = arad_egq_ppct_tbl_get_unsafe(unit, core_id,  base_q_pair, &ppct_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (ppct_tbl_data.is_stacking_port == 0x0) {
         SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);    
    }

    
    res = arad_port_header_type_get_unsafe(
        unit,
        core_id,
        tm_port,
        &header_type_incoming,
        &header_type_outgoing
      );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    if (header_type_outgoing != ARAD_PORT_HEADER_TYPE_TM)
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_NOT_TM_PORT_ERR, 30, exit);
    }

    
    if (tm_port_profile_stack == SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK1) {
        
        res = arad_egr_prog_editor_program_pointer_get(unit, ARAD_EGR_PROG_EDITOR_PROG_STACKING, &prge_prog_pointer);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    } else if (tm_port_profile_stack == SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK2) {
        
        res = arad_egr_prog_editor_branch_pointer_allocate(unit, ARAD_EGR_PROG_EDITOR_BRANCH_STACKING, &prge_prog_pointer);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    } else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
    }

    prog_editor_value = ARAD_EGR_PROG_EDITOR_PTR_TO_BRANCH_ADDR(prge_prog_pointer, 0);

    
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {

        res = arad_egq_pct_tbl_get_unsafe(
              unit,
              core_id,
              curr_q_pair,
              &pct_tbl_data
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit); 

        pct_tbl_data.prog_editor_value = prog_editor_value;

        res = arad_egq_pct_tbl_set_unsafe(
              unit,
              core_id,
              curr_q_pair,
              &pct_tbl_data
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit); 
    }
    
    
    res = arad_egr_prog_editor_stacking_lfems_set(unit, tm_port_profile_stack, bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_route_history_bitmap_set_unsafe()",tm_port,0);
}


uint32
  ARAD_PORT_PP_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO *info
  )
{


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_header_size, ARAD_PORTS_FIRST_HEADER_SIZE_MAX, ARAD_PORTS_FIRST_HEADER_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fc_type, ARAD_PORT_PP_PORT_INFO_FC_TYPE_MAX, ARAD_PORTS_FC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, ARAD_PORT_NOF_HEADER_TYPES-1, ARAD_PORTS_HEADER_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mirror_profile, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PORT_PP_PORT_INFO_verify()",0,0);
}


soc_error_t 
arad_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int blk_id, inner_port, link, rv;
    uint32 reg_val32;
    soc_dcmn_port_pcs_t  pcs;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode) { 
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        inner_port = link % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

        rv = arad_port_control_pcs_get(unit, port, &pcs);
        SOCDNX_IF_ERR_EXIT(rv);

        switch(pcs) {
            case soc_dcmn_port_pcs_64_66_bec:
            case soc_dcmn_port_pcs_64_66_fec:
            case soc_dcmn_port_pcs_64_66:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val32, KPCS_N_TST_TX_ENf, value ? 1 : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, blk_id, inner_port, reg_val32));
                break;

            case soc_dcmn_port_pcs_8_10:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                soc_reg_field_set(unit, FMAC_TEST_CONFIGURATIONr, &reg_val32, EDS_N_TX_TST_ENABLEf, value ? 1 : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, reg_val32));
                break;

            case soc_dcmn_port_pcs_8_9_legacy_fec:
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("MAC PRBS isn't supported for PCS %d"), pcs));
                break;
        }
    } else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_PRBS_TX_ENABLE, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    int blk_id, inner_port, link, rv;
    uint32 reg_val32;
    soc_dcmn_port_pcs_t  pcs;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode) { 
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        inner_port = link % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

        SOCDNX_IF_ERR_EXIT(arad_port_control_pcs_get(unit, port, &pcs));
        switch(pcs) {
            case soc_dcmn_port_pcs_64_66_bec:
            case soc_dcmn_port_pcs_64_66_fec:
            case soc_dcmn_port_pcs_64_66:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                *value = soc_reg_field_get(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, reg_val32, KPCS_N_TST_TX_ENf);
                break;

            case soc_dcmn_port_pcs_8_10:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                *value = soc_reg_field_get(unit, FMAC_TEST_CONFIGURATIONr, reg_val32, EDS_N_TX_TST_ENABLEf);
                break;

            case soc_dcmn_port_pcs_8_9_legacy_fec:
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("MAC PRBS isn't supported for PCS %d"), pcs));
                break;
        }
    } else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port,SOC_PHY_CONTROL_PRBS_TX_ENABLE, (uint32*)value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int blk_id, inner_port, rv, link;
    uint32 reg_val32;
    soc_dcmn_port_pcs_t  pcs;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode) { 
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        inner_port = link % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

        rv = arad_port_control_pcs_get(unit, port, &pcs);
        SOCDNX_IF_ERR_EXIT(rv);

        switch(pcs) {
            case soc_dcmn_port_pcs_64_66_bec:
            case soc_dcmn_port_pcs_64_66_fec:
            case soc_dcmn_port_pcs_64_66:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val32, KPCS_N_TST_RX_ENf, value ? 1 : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, blk_id, inner_port, reg_val32));
                break;

            case soc_dcmn_port_pcs_8_10:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                soc_reg_field_set(unit, FMAC_TEST_CONFIGURATIONr, &reg_val32, EDS_N_RX_TST_ENABLEf, value ? 1 : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, reg_val32));
                break;

            case soc_dcmn_port_pcs_8_9_legacy_fec:
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("MAC PRBS isn't supported for PCS %d"), pcs));
                break;
        }
    } else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_PRBS_RX_ENABLE, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    int blk_id, inner_port, rv, link;
    uint32 reg_val32;
    soc_dcmn_port_pcs_t  pcs;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode) { 
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        inner_port = link % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

        rv = arad_port_control_pcs_get(unit, port, &pcs);
        SOCDNX_IF_ERR_EXIT(rv);

        switch(pcs) {
            case soc_dcmn_port_pcs_64_66_bec:
            case soc_dcmn_port_pcs_64_66_fec:
            case soc_dcmn_port_pcs_64_66:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                *value = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val32, KPCS_N_TST_RX_ENf);
                break;

            case soc_dcmn_port_pcs_8_10:
                SOCDNX_IF_ERR_EXIT(READ_FMAC_TEST_CONFIGURATIONr(unit, blk_id, inner_port, &reg_val32));
                *value = soc_reg_field_get(unit, FMAC_TEST_CONFIGURATIONr, reg_val32, EDS_N_RX_TST_ENABLEf);
                break;

            case soc_dcmn_port_pcs_8_9_legacy_fec:
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("MAC PRBS isn't supported for PCS %d"), pcs));
                break;
        }
    } else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_PRBS_RX_ENABLE, (uint32*)value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    int blk_id, inner_port, rv, link;
    uint32 reg_val32, reg_val32_counter, reg_val32_sync;
    soc_dcmn_port_pcs_t pcs;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode) { 
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        blk_id = link/SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        inner_port = link % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

        rv = arad_port_control_pcs_get(unit, port, &pcs);
        SOCDNX_IF_ERR_EXIT(rv);

        switch(pcs) {
            case soc_dcmn_port_pcs_64_66_bec:
            case soc_dcmn_port_pcs_64_66_fec:
            case soc_dcmn_port_pcs_64_66:
                
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_STATUSr(unit, blk_id, inner_port, &reg_val32_sync));
                SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_STATUSr(unit, blk_id, inner_port, &reg_val32_counter));
                if (!soc_reg_field_get(unit, FMAC_FPS_RX_STATUSr, reg_val32_sync, FPS_N_RX_SYNC_STATUSf)) {
                    *value = -1;
                } else { 
                    
                    *value = (int)reg_val32_counter;
                }
                break;

            case soc_dcmn_port_pcs_8_10:
                
                SOCDNX_IF_ERR_EXIT(READ_FMAC_TEST_STATUSr(unit, blk_id, inner_port, &reg_val32));
                if (!soc_reg_field_get(unit, FMAC_TEST_STATUSr, reg_val32, TST_N_SYNCEDf)) {
                    *value = -1;
                }  else {
                    
                    *value = (int)soc_reg_field_get(unit, FMAC_TEST_STATUSr, reg_val32, TST_N_ERR_CNTf);
                }
                break;

            case soc_dcmn_port_pcs_8_9_legacy_fec:
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("MAC PRBS isn't supported for PCS %d"), pcs));
                break;
        }
    } else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_PRBS_RX_STATUS, (uint32*)value);
        MIIM_UNLOCK(unit);    
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode){ 
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("PRBS TX invert data set isn't supported for MAC PRBS\n")));
    }else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
arad_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode){ 
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("PRBS TX invert data get isn't supported for MAC PRBS\n")));
    }else { 
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA, (uint32*)value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
arad_port_speed_max(int unit, soc_port_t port, int *speed)
{
  SOCDNX_INIT_FUNC_DEFS;

  if(soc_property_get(unit, spn_SYSTEM_IS_FE600_IN_SYSTEM,  FALSE)) {
    *speed = 6250;
  } else {
    *speed = 12500;
  }

  SOCDNX_FUNC_RETURN;
}
  

uint32 alloc_reassembly_context_and_recycle_channel_unsafe(int unit, int core_id, uint32 out_pp_port, uint32 *channel, uint32 *reassembly_context)
{
  uint32 res;
  ARAD_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing, new_header_type_incoming;
  ARAD_PORT_PP_PORT_INFO in_info, out_info;
  SOC_SAND_SUCCESS_FAILURE success;
  soc_dpp_config_pp_t *dpp_pp;
  uint32 port_extender_map_lc = 0;
  uint32 flags, pp_rcy_port, tchannel;
  int core_rcy_port, egr_if, egr_if_i;
  soc_port_t rcy_port, local_dest_port, port_i;
  uint8 found;
  SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;
  soc_pbmp_t ports_bm;
  uint8 channels[SOC_DPP_MAX_NOF_CHANNELS] = {0};
  soc_port_if_t interface_type;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  *channel = 0;

  ARAD_PORT_PP_PORT_INFO_clear(&in_info);
  dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

  res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, out_pp_port, &local_dest_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  {
      res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core_id, out_pp_port, (uint32 *)&egr_if));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.get(unit, core_id, egr_if, &rcy_port);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
      if (rcy_port == ARAD_PORT_INVALID_RCY_PORT) { 
          res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_drv_allocate_rcy_port, (unit, core_id, &rcy_port));
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
          res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.set(unit, core_id, egr_if, rcy_port);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
          res = soc_port_sw_db_channel_get(unit, rcy_port, channel);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

          if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
               res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, rcy_port, reassembly_context);
               SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 37, exit);
          } else {

              
              res = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
              res = soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, reassembly_context, &found);
              SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
              if (!found) {
                  SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 37, exit);
              }
          }

          res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_reassembly_ctxt.set(unit, core_id, egr_if, *reassembly_context);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);

      } else { 
               
            
            if (SOC_DPP_CONFIG(unit)->arad->init.rcy_channelized_shared_context_enable) {
                res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_drv_allocate_rcy_port, (unit, core_id, &rcy_port));
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

                res = soc_port_sw_db_channel_get(unit, rcy_port, channel);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);
            } else {
                
                res = soc_port_sw_db_valid_ports_core_get(unit, core_id, 0, &ports_bm);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
                SOC_PBMP_ITER(ports_bm, port_i){
                    res = soc_port_sw_db_interface_type_get(unit, port_i, &interface_type);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);
                    if (interface_type == SOC_PORT_IF_RCY) {
                        res = soc_port_sw_db_channel_get(unit, port_i, &tchannel);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 56, exit);
                        channels[tchannel] = 1;
                    }
                }

                
                for (*channel = 0; *channel < SOC_DPP_MAX_NOF_CHANNELS; (*channel)++) {
                    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.get(unit, core_id, *channel, &egr_if_i);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 37, exit);

                    if ((egr_if_i == ARAD_PORT_INVALID_EGQ_INTERFACE) && (!channels[*channel])) {
                        break;
                    }
                }

                if (*channel >= SOC_DPP_MAX_NOF_CHANNELS) {
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 38, exit);
                }
            }

           
           res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_reassembly_ctxt.get(unit, core_id, egr_if, reassembly_context);
           SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit);
      }

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.set(unit, core_id, *channel, egr_if);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 39, exit);

      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 815, exit, soc_port_sw_db_local_to_pp_port_get(unit, rcy_port, &pp_rcy_port, &core_rcy_port));
      res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_mirrored_channel_and_context_map, (unit, core_id, pp_rcy_port, *reassembly_context, *channel));
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit); 

  }
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 815, exit, soc_port_sw_db_local_to_pp_port_get(unit, rcy_port, &pp_rcy_port, &core_rcy_port));

  

  
  res = arad_port_pp_port_get_unsafe(unit, core_id, out_pp_port, &out_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_port_header_type_get_unsafe(unit, core_id, out_pp_port, &header_type_incoming, &header_type_outgoing);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

  
  if (out_info.header_type_out == SOC_TMC_PORT_HEADER_TYPE_ETH && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_snooping_advanced", 0)) {
      out_info.header_type_out = SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW;
  }

  
  if ((header_type_outgoing == SOC_TMC_PORT_HEADER_TYPE_CPU) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_mirror_large_pkt", 0)) {
      header_type_outgoing = SOC_TMC_PORT_HEADER_TYPE_RAW;
      out_info.header_type_out = SOC_TMC_PORT_HEADER_TYPE_RAW;
  }

  
  in_info.header_type = out_info.header_type_out;
  res = arad_port_pp_port_set_unsafe(unit, core_id, pp_rcy_port,  &in_info, &success);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  if (success != SOC_SAND_SUCCESS) 
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_EG_TM_PROFILE_FULL_ERR, 65, exit);
  }
    
  new_header_type_incoming = header_type_outgoing;
  
  if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH || header_type_outgoing == ARAD_PORT_HEADER_TYPE_MPLS_RAW) && out_info.header_type_out == ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT)  
  {  
    
    res = arad_parser_nof_bytes_to_remove_set(
            unit,
            core_id,
            pp_rcy_port,
            SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_FRC_PPD
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);
  }

    
    port_extender_map_lc = dpp_pp->port_extender_map_lc_exists;
    
    if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH) && (port_extender_map_lc == 1)) {        
        res = soc_port_sw_db_flags_get(unit, local_dest_port, &flags);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 66, exit);
        
        
        if (SOC_PORT_IS_COE_PORT(flags)) {
            res = arad_parser_nof_bytes_to_remove_set(
                unit,
                core_id,
                pp_rcy_port,
                1);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 67, exit);
        }
    }
  
    if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH) && (port_extender_map_lc == 2)) {
        res = soc_port_sw_db_flags_get(unit, local_dest_port, &flags);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 68, exit);
        
        if (SOC_PORT_IS_COE_PORT(flags)) {
            ARAD_PORTS_SWAP_INFO ports_swap_info;
            
            arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);
            
            ports_swap_info.enable = TRUE;
                
            res = arad_ports_swap_set_unsafe(unit, rcy_port, &ports_swap_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
            
            
            res = arad_parser_nof_bytes_to_remove_set(unit, core_id, pp_rcy_port, 4);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 71, exit);
        }
    }

    if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH) 
        && (SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0))) {
        ARAD_PP_EPNI_PP_PCT_TBL_DATA pp_pct_tbl_data;
        res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, out_pp_port, &pp_pct_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 72, exit);
        
        if (pp_pct_tbl_data.prge_profile == ARAD_PRGE_PP_SELECT_CH_CONN_TO_FPGA) {
            res = arad_parser_nof_bytes_to_remove_set(unit, core_id, pp_rcy_port, 5);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 71, exit);
        }
    }

  res = arad_port_header_type_set_unsafe(unit, core_id, pp_rcy_port, ARAD_PORT_DIRECTION_INCOMING, new_header_type_incoming);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);


  
  {
      uint32
         data[1];
      soc_mem_t
         pinfo_flp_mem;
      int
         block;

      ARAD_CLEAR(data, uint32, 1);

      if (SOC_IS_JERICHO(unit)) {
          pinfo_flp_mem = IHP_PINFO_FLP_1m;
          block =  IHP_BLOCK(unit, core_id);
      } else {
          pinfo_flp_mem = IHB_PINFO_FLPm;
          block = MEM_BLOCK_ANY;
      }
      res = soc_mem_read(
         unit,
         pinfo_flp_mem,
         block,
         pp_rcy_port,
         data
         );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);


      soc_mem_field32_set(unit, pinfo_flp_mem, data, LEARN_ENABLEf, 0);

      res = soc_mem_write(
         unit,
         pinfo_flp_mem,
         block,
         pp_rcy_port,
         data
         );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in alloc_reassembly_context_and_recycle_channel_unsafe()", 0, 0);
}



STATIC 
uint32 
 get_rcy_context_map_base_address(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id,
     SOC_SAND_OUT uint32 *base_address)
{
    uint32 data[2], rcy_offset, reg32;
    soc_field_t field;
    uint32 base_address_lcl[1];

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        *base_address=0;
        SOCDNX_FUNC_RETURN;
    }
    
    if (SOC_IS_QAX(unit)) {
        field = INTERFACE_EGQ_PORTf;
    } else {
        field = (core_id == 0 ? INTERFACE_EGQ_0_PORTf : INTERFACE_EGQ_1_PORTf);
    }
    SOCDNX_IF_ERR_EXIT(READ_IRE_INTERFACE_PORT_CONFIGURATIONr_REG32(unit, &reg32));
    rcy_offset = soc_reg_field_get(unit, IRE_INTERFACE_PORT_CONFIGURATIONr, reg32, field);
    SOCDNX_IF_ERR_EXIT(READ_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, rcy_offset, data));
    *base_address_lcl=0;
    soc_mem_field_get(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, data, CTXT_MAP_BASE_ADDRESSf, base_address_lcl);
    *base_address = *base_address_lcl;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC 
soc_mem_t 
get_context_map_id(SOC_SAND_IN int unit)
{
    return SOC_IS_JERICHO(unit) ? IRE_CTXT_MAPm :IRE_RCY_CTXT_MAPm;
}


int 
 get_recycling_port(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id,
     SOC_SAND_IN uint32  pp_port, 
     SOC_SAND_IN uint32 channel, 
     SOC_SAND_OUT soc_port_t *logical_port)
{
    uint32 base_address=0,index,data,tm_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(get_rcy_context_map_base_address(unit, core_id, &base_address));
    index = base_address + channel;

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit,get_context_map_id(unit),MEM_BLOCK_ANY,index, &data));
    tm_port = soc_mem_field32_get(unit, get_context_map_id(unit), &data, PORT_TERMINATION_CONTEXTf);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core_id, tm_port, (soc_port_t *)logical_port));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 release_reassembly_context_and_mirror_channel_unsafe(int unit, int core, uint32 out_pp_port, uint32 channel)
{
  int i, res, egr_if, egr_if_i;
  uint32 nof_rcy_channels = 0;
  soc_port_t local_dest_port, rcy_port;
  uint32 reassembly_context;
  SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;

  uint32 port_extender_map_lc;
  ARAD_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;
  uint32 tm_rcy_port;
  int core_rcy_port;
  soc_dpp_config_pp_t *dpp_pp;
  uint32 flags;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

  res = soc_port_sw_db_pp_to_local_port_get(unit, core, out_pp_port, &local_dest_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, out_pp_port, (uint32 *)&egr_if));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.get(unit, core, egr_if, &rcy_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
  if (rcy_port == ARAD_PORT_INVALID_RCY_PORT) { 
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
  }

  
  for (i = 0; i < SOC_DPP_MAX_NOF_CHANNELS; i++) {
       res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.get(unit, core, i, &egr_if_i);
       SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);
       if ((egr_if_i != ARAD_PORT_INVALID_RCY_PORT) && (egr_if_i == egr_if)) {
           nof_rcy_channels++;
       }
  }

  
  res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_channels_to_egr_nif_mapping.set(unit, core, channel, ARAD_PORT_INVALID_EGQ_INTERFACE);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 101, exit);

  if (nof_rcy_channels == 1) { 

      
      if (SOC_IS_JERICHO(unit)) {
          res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_reassembly_ctxt.get(unit, core, egr_if, &reassembly_context);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

          
          res = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
          res = soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassembly_context,0);
          SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
      }

      
      res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.set(unit, core, egr_if, ARAD_PORT_INVALID_RCY_PORT);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
  }

  
  res = get_recycling_port(unit, core, out_pp_port, channel, &rcy_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);

  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_mirrored_channel_and_context_map, (unit, core, 
                                             ARAD_PORTS_IF_UNMAPPED_INDICATION, ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED, channel));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit); 

  
  if (nof_rcy_channels == 1 || SOC_DPP_CONFIG(unit)->arad->init.rcy_channelized_shared_context_enable) { 

        port_extender_map_lc = dpp_pp->port_extender_map_lc_exists;
        
        res = arad_port_header_type_get_unsafe(unit, core, out_pp_port, &header_type_incoming, &header_type_outgoing);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);
 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, soc_port_sw_db_local_to_tm_port_get(unit, rcy_port, &tm_rcy_port, &core_rcy_port));

        if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH) && ((port_extender_map_lc == 1) || (port_extender_map_lc == 2))) {
 
            res = soc_port_sw_db_flags_get(unit, local_dest_port, &flags);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);
 
            
            if (SOC_PORT_IS_COE_PORT(flags)) {
                ARAD_PORTS_SWAP_INFO ports_swap_info;
                arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);

                res = arad_parser_nof_bytes_to_remove_set(
                    unit,
                    core_rcy_port,
                    tm_rcy_port,
                    0);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 125, exit);

                if (port_extender_map_lc == 2) {
                    ports_swap_info.enable = FALSE;
                    res = arad_ports_swap_set_unsafe(unit, rcy_port, &ports_swap_info);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
                }
            }
        }

        if ((header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH) 
            && (SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0))) {
            ARAD_PP_EPNI_PP_PCT_TBL_DATA pp_pct_tbl_data;
            res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, out_pp_port, &pp_pct_tbl_data);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 135, exit);
            if (pp_pct_tbl_data.prge_profile == ARAD_PRGE_PP_SELECT_CH_CONN_TO_FPGA) {
              res = arad_parser_nof_bytes_to_remove_set(unit, core_rcy_port, tm_rcy_port, 0);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
            }
        }

      
      res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_free_tm_port_and_recycle_channel,(unit, rcy_port));
      if (res != SOC_E_NONE) {
          SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_NOT_TM_PORT_ERR, 10, exit); 
      }  
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in release_reassembly_context_and_mirror_channel_unsafe()", 0, 0);
}



  uint32 
    arad_port_direct_lb_key_get(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN int core_id,
      SOC_SAND_IN uint32  local_port,
      SOC_SAND_OUT uint32 *min_lb_key,
      SOC_SAND_OUT uint32 *max_lb_key
   )
{
  int 
      res;
  uint32 
      base_q_pair;
  ARAD_EGQ_PPCT_TBL_DATA
      egq_pp_ppct_data;
#ifdef BCM_88660_A0
  uint32 field_val, use_table_2=0; 
  ARAD_PER_PORT_LB_RANGE_TBL_DATA egq_per_port_lb_range_tbl_data;
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit,  core_id, local_port, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef BCM_88660_A0
    
  if(SOC_IS_ARADPLUS(unit)) {
    if(SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) {
            
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf, &field_val));
             
      if(field_val == 1) {
        use_table_2 = 1;
      }
    }
  } 

    if(use_table_2 == 1) {
    
      res = arad_egq_per_port_lb_range_tbl_get_unsafe(
                    unit,
                    base_q_pair,
                    &egq_per_port_lb_range_tbl_data
                  ); 
                  
       if (min_lb_key != NULL) {
         *min_lb_key = egq_per_port_lb_range_tbl_data.lb_key_min & (~0x80); 
       }

       if (max_lb_key != NULL) {
         *max_lb_key = egq_per_port_lb_range_tbl_data.lb_key_max & (~0x80); 
       }

  } else 
#endif 
  {  
    res = arad_egq_ppct_tbl_get_unsafe(
                unit,
                core_id,
                base_q_pair,
                &egq_pp_ppct_data
              );

     SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        
     if (min_lb_key != NULL) {
       *min_lb_key = egq_pp_ppct_data.lb_key_min;
     }

     if (max_lb_key != NULL) {
       *max_lb_key = egq_pp_ppct_data.lb_key_max;
     }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_get()", 0, 0);
  
}

uint32 
  arad_port_direct_lb_key_min_get_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* min_lb_key
   )
{
  int 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_direct_lb_key_get, (unit, core_id, local_port, min_lb_key, NULL)));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_min_get_unsafe()", 0, 0);

}

uint32 
  arad_port_direct_lb_key_max_get_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    uint32* max_lb_key
   )
{
  int 
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_direct_lb_key_get, (unit, core_id, local_port, NULL, max_lb_key)));
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_max_get_unsafe()", 0, 0);
}


uint32 arad_port_synchronize_lb_key_tables_at_egress_unsafe(
            SOC_SAND_IN int unit 
        )
{
    int             port_i;
    int             core = SOC_CORE_INVALID;
    uint32          max_lb_key = 0;
    uint32          min_lb_key = 0;
    uint32          is_valid = 0;
    uint32          pp_port;
    uint32          flags = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    if(unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("ERROR: invalid unit")));
    }
    for(port_i=0; port_i < ARAD_PORT_NDX_MAX(unit) ; ++port_i) 
    {
        if (!SOC_PORT_VALID(unit, port_i))
        {
            continue;
        }
     
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) 
        {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
        {
            continue;
        }
    
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &pp_port, &core));
     
        SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_direct_lb_key_get, (unit, core, pp_port, &min_lb_key, &max_lb_key)));
     
        SOCDNX_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_set,(unit, core, pp_port, min_lb_key, TRUE, max_lb_key, TRUE)));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
} 
     
uint32
  arad_port_switch_lb_key_tables_unsafe(
     SOC_SAND_IN int unit
    )
{
  uint32
    res,
    fld_val;
  uint64
    val64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_LBP_GENERAL_CONFIG_1r(unit, REG_PORT_ANY, &val64));   

  
  fld_val = soc_reg64_field32_get(unit, IHB_LBP_GENERAL_CONFIG_1r, val64, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf);
  fld_val = (fld_val == 0x0) ? 0x1 : 0x0; 
  soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_1r, &val64, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf, fld_val);  
 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_LBP_GENERAL_CONFIG_1r(unit, REG_PORT_ANY, val64)); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_switch_lb_key_tables_unsafe()", 0, 0);
}


uint32 
  arad_port_direct_lb_key_set_verify(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 lb_key
   )
{
  uint32
    max_lb_key;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    max_lb_key = ARAD_PORT_LAG_LB_RNG_MAX_VAL(unit); 

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit
  ); 

  SOC_SAND_ERR_IF_ABOVE_MAX(
   local_port , ARAD_PORT_NDX_MAX(unit) ,
   ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
  ); 

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lb_key, max_lb_key,
    ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
  );

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_min_set_verify()", 0, 0);
}

uint32 
  arad_port_direct_lb_key_get_verify(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32  local_port
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 20, exit
  ); 

  SOC_SAND_ERR_IF_ABOVE_MAX(
   local_port , ARAD_PORT_NDX_MAX(unit) ,
   ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
  ); 

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_direct_lb_key_min_get_unsafe()", 0, 0);
}



uint32 arad_ports_swap_set_verify(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_SET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(unit, ARAD_MAX_DEVICE_ID,
        ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_NOF_LOCAL_PORTS(unit) - 1, 
        ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_set_verify()",
        unit, port_ndx);
}

uint32 arad_ports_swap_set_unsafe(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info)
{
    soc_reg_above_64_val_t reg_data, field_val;
    uint32 enable_state;
    int res;
    uint32 pp_port;
    int core;
    soc_mem_t mem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_SET_UNSAFE);

    
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(field_val);

    
    if (SOC_IS_JERICHO(unit))  {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_local_to_pp_port_get(unit,port_ndx,&pp_port, &core));
        if (SOC_IS_QAX(unit)) {
            mem = IRE_TAG_SWAP_CFGm;
        } else {
            if (core == 0) {
                mem = IRE_TAG_SWAP_0_CFGm;
            } else {
                mem = IRE_TAG_SWAP_1_CFGm;
            }
        }
        
        
        enable_state = SOC_SAND_BOOL2NUM(ports_swap_info->enable);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, pp_port, reg_data));
        soc_mem_field_set(unit, mem, reg_data, TAG_SWAP_ENABLEf, &enable_state);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_mem_write(unit, mem, MEM_BLOCK_ANY, pp_port, reg_data));

    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IRE_TAG_SWAP_ENABLEr(unit, reg_data));
        ARAD_FLD_FROM_REG_ABOVE_64(IRE_TAG_SWAP_ENABLEr, TAG_SWAP_ENABLEf, field_val, reg_data, 20, exit);

        
        enable_state = SOC_SAND_BOOL2NUM(ports_swap_info->enable);
        res = soc_sand_bitstream_set_any_field(&enable_state, port_ndx, 1, field_val);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        
        ARAD_FLD_TO_REG_ABOVE_64(IRE_TAG_SWAP_ENABLEr, TAG_SWAP_ENABLEf, field_val, reg_data, 40, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IRE_TAG_SWAP_ENABLEr(unit, reg_data));

    } 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_swap_info_set_unsafe()", port_ndx, ports_swap_info->enable);
}

uint32 arad_ports_swap_get_verify(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_IN ARAD_PORTS_SWAP_INFO     *ports_swap_info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_GET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(unit, ARAD_MAX_DEVICE_ID,
        ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit);
  
    SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_NOF_LOCAL_PORTS(unit) - 1, 
        ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_get_verify()",
        unit, port_ndx);
}

uint32 arad_ports_swap_get_unsafe(
   SOC_SAND_IN int                   unit,
   SOC_SAND_IN ARAD_FAP_PORT_ID         port_ndx,
   SOC_SAND_OUT ARAD_PORTS_SWAP_INFO    *ports_swap_info)
{
    soc_reg_above_64_val_t reg_data, field_val;
    uint32 enable_state = 0;
    int res, core;
    uint32 pp_port;
    soc_mem_t mem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_GET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(ports_swap_info);
    arad_ARAD_PORTS_SWAP_INFO_clear(ports_swap_info);

    
    SOC_REG_ABOVE_64_CLEAR(reg_data);
    SOC_REG_ABOVE_64_CLEAR(field_val);

    if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_local_to_pp_port_get(unit,port_ndx,&pp_port, &core));

        if (core == 0) {
            mem = IRE_TAG_SWAP_0_CFGm;
        } else {
            mem = IRE_TAG_SWAP_1_CFGm;
        }

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_mem_read(unit, mem, MEM_BLOCK_ANY, pp_port, reg_data));
        enable_state = soc_mem_field32_get(unit, mem, reg_data, TAG_SWAP_ENABLEf);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    } else {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IRE_TAG_SWAP_ENABLEr(unit, reg_data));
        ARAD_FLD_FROM_REG_ABOVE_64(IRE_TAG_SWAP_ENABLEr, TAG_SWAP_ENABLEf, field_val, reg_data, 50, exit);

        
        res = soc_sand_bitstream_get_any_field(field_val, port_ndx, 1, &enable_state);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

    ports_swap_info->enable = SOC_SAND_NUM2BOOL(enable_state);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_swap_get_unsafe()", port_ndx, enable_state);
}

uint32 arad_swap_info_set_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SWAP_INFO_SET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(unit, ARAD_MAX_DEVICE_ID,
        ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(swap_info->offset, ARAD_PORTS_SWAP_MAX_OFFSET_VAL,
        ARAD_PORTS_SWAP_OFFSET_OUT_OF_RANGE_ERR, 20, exit);

#ifdef BCM_88660_A0
    ARAD_DEVICE_CHECK(unit, exit);

    if (SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_ERR_IF_OUT_OF_RANGE(swap_info->mode, 0, (ARAD_SWAP_MODES - 1),
            ARAD_PORTS_SWAP_OFFSET_OUT_OF_RANGE_ERR, 30, exit);
    }
#endif 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_swap_info_set_verify()",
        unit, swap_info->offset);
}

uint32 arad_swap_info_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SWAP_INFO_SET_UNSAFE);

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, TAG_SWAP_OFFSETf,  swap_info->offset));

    
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, TAG_SWAP_8_BYTES_ENABLEf,  swap_info->mode));
    }
#endif 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_swap_info_set_unsafe()", swap_info->offset, 0);
}

uint32 arad_swap_info_get_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SWAP_INFO_GET_VERIFY);

    SOC_SAND_ERR_IF_ABOVE_MAX(unit, ARAD_MAX_DEVICE_ID,
        ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_swap_info_get_verify()",
        unit, 0);
}

uint32 arad_swap_info_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_OUT ARAD_SWAP_INFO     *swap_info)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SWAP_INFO_GET_UNSAFE);
  
    
    SOC_SAND_CHECK_NULL_INPUT(swap_info);
    arad_ARAD_SWAP_INFO_clear(swap_info);

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, TAG_SWAP_OFFSETf, &swap_info->offset));

#ifdef BCM_88660_A0
    
    if (SOC_IS_ARADPLUS(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, TAG_SWAP_8_BYTES_ENABLEf, &swap_info->mode));
    }
#endif 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_swap_info_get_unsafe()", 0, 0);
}


uint32
  arad_ports_pon_tunnel_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit
  ); 
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_ndx, ARAD_PORTS_PON_PORT_NDX_MAX(unit), 
    ARAD_PORTS_PON_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
  );
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    tunnel, ARAD_PORTS_PON_TUNNEL_ID_MAX,
    ARAD_PORTS_PON_TUNNEL_ID_OUT_OF_RANGE_ERR, 30, exit
  );
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->pp_port, ARAD_PORTS_PON_IN_PP_NDX_MAX,
    ARAD_PORTS_PON_IN_PP_OUT_OF_RANGE_ERR, 40, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_set_verify()", port_ndx, tunnel);
}

uint32
  arad_ports_pon_tunnel_info_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  int    
    tbl_data_32b = 0,
    res;
  uint32 
    sys_port = 0,
    nof_channels = 0,
    offset = 0,
    local_port = 0,
    tbl_idx = 0,
    pon_tunnel_id_alloc_mode = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);  
  if (pon_tunnel_id_alloc_mode == 0) {
    tbl_idx = ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port_ndx, tunnel);
  } else {
    tbl_idx = ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port_ndx, tunnel);
  }

  
  res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, info->pp_port);

  if (SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable) {
      res = soc_port_sw_db_num_of_channels_get(unit, port_ndx, &nof_channels);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

      
      offset = (ARAD_PORTS_FEM_PP_PORT_PON_PTC_SIZE(unit)+(nof_channels-1));

      
      local_port = info->pp_port & ((1<<offset)-1);

      
      if (local_port != port_ndx) {
        sys_port = local_port;
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, sys_port);
      }
  }

  res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_pon_tunnel_info_set_unsafe()", port_ndx, tunnel);
}


uint32
  arad_ports_pon_tunnel_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit
  ); 
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_ndx, ARAD_PORTS_PON_PORT_NDX_MAX(unit), 
    ARAD_PORTS_PON_PORT_NDX_OUT_OF_RANGE_ERR, 20, exit
  );
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
    tunnel, ARAD_PORTS_PON_TUNNEL_ID_MAX,
    ARAD_PORTS_PON_TUNNEL_ID_OUT_OF_RANGE_ERR, 30, exit
  );

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_set_verify()", port_ndx, tunnel);
}


uint32
  arad_ports_pon_tunnel_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  int    
    tbl_data_32b = 0,
    pp_port,
    res;
  uint32 
    tbl_idx = 0,
    pon_tunnel_id_alloc_mode = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  arad_ARAD_PORTS_PON_TUNNEL_INFO_clear(info);

  pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);  
  if (pon_tunnel_id_alloc_mode == 0) {
    tbl_idx = ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port_ndx, tunnel);
  } else {
    tbl_idx = ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port_ndx, tunnel);
  }

  
  res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_data_32b);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  pp_port = soc_IHP_VIRTUAL_PORT_TABLEm_field32_get(unit, &tbl_data_32b, IN_PP_PORTf);
  info->pp_port = pp_port;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_pon_tunnel_info_set_unsafe()", port_ndx, tunnel);
}


STATIC uint32
soc_arad_ports_egq_enabler(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  int     core,
    SOC_SAND_IN  uint32  tm_port)
{
    int res;
    uint32 base_q_pair, memlock = 0;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA dsp_tbl_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
    memlock = 1;

    res = arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit,core,tm_port,&dsp_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_egr_dsp_pp_to_base_q_pair_get_unsafe(unit, core, tm_port, &base_q_pair);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    dsp_tbl_data.out_tm_port = base_q_pair;
    res = arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit,core,tm_port,&dsp_tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    memlock = 0;
    MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);   

exit:
    if(memlock) {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }
    SOC_SAND_EXIT_AND_SEND_ERROR("soc_arad_ports_egq_enabler()", tm_port, 0);
}


int
soc_arad_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable)
{
    uint32 tm_port, memlock, reg_val, field_val;
    int core;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA dsp_tbl_data;

    SOCDNX_INIT_FUNC_DEFS;
     
    memlock = 0;     
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    if (enable) {
        SOCDNX_IF_ERR_EXIT(soc_arad_ports_egq_enabler(unit, core, tm_port));

    } else { 

        
        

        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_INVALID_OTMr(unit, core, &reg_val));

        field_val = soc_reg_field_get(unit, EGQ_INVALID_OTMr, reg_val, INVALID_OTMf);         

        MEM_LOCK(unit, EGQ_DSP_PTR_MAPm);
        memlock = 1;

        SOCDNX_IF_ERR_EXIT(arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit,core,tm_port,&dsp_tbl_data));

        dsp_tbl_data.out_tm_port = field_val;

        SOCDNX_IF_ERR_EXIT(arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(unit,core,tm_port,&dsp_tbl_data));

        memlock = 0;
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }

exit:
    if(memlock) {
        MEM_UNLOCK(unit, EGQ_DSP_PTR_MAPm);
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_arad_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable)
{
    uint32 tm_port, reg_val, field_val;
    ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA dsp_tbl_data;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));  

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_INVALID_OTMr(unit, core, &reg_val));
    field_val = soc_reg_field_get(unit, EGQ_INVALID_OTMr, reg_val, INVALID_OTMf);

    SOCDNX_IF_ERR_EXIT(arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(unit,core,tm_port,&dsp_tbl_data));

    
    if (field_val == dsp_tbl_data.out_tm_port) {
        *enable = FALSE;
    } else {
        *enable = TRUE;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


#define ARAD_OTM_PORTS_NUM                (256)      

uint32
soc_arad_ports_stop_egq(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  soc_port_t     port,
    SOC_SAND_OUT uint32     enable)
{
    uint32 tm_port, base_q_pair, nof_queues, q_pair;
    uint32 reg_val, force_fc_mode = 0, if_spr_reg_val, if_spr_reg_val_valid = 0;
    int res, core, is_channelized;
    uint32 otm_rates[ARAD_EGR_NOF_Q_PAIRS] = {0};
    uint32 mem_off = 0;
    soc_reg_above_64_val_t data_above_64, ignore_fc;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1, exit, soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 2, exit, soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 4, exit, soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_queues));
    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOC_REG_ABOVE_64_CLEAR(ignore_fc);

    if(enable) 
    {
        
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_control_tx_nif_enable_set,(unit, port, enable));
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

    } else {

        
        if (is_channelized) 
        {
            res=arad_ofp_if_shaper_disable(unit,port,&if_spr_reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res , 90, enable_shapers);
            if_spr_reg_val_valid = 1;
        }
        res=arad_ofp_q_pair_shapers_enable(unit,port,0);
        SOC_SAND_CHECK_FUNC_RESULT(res , 60, exit);
        res=arad_ofp_tcg_shapers_enable(unit,port,0);
        SOC_SAND_CHECK_FUNC_RESULT(res , 70, enable_shapers);
        if (SOC_IS_JERICHO(unit)) {
            res=jericho_ofp_otm_shapers_enable(unit,port,0);
            SOC_SAND_CHECK_FUNC_RESULT(res , 71, enable_shapers);
        } else {
            res=arad_ofp_otm_shapers_disable(unit,port, ARAD_EGR_NOF_Q_PAIRS, otm_rates);
            SOC_SAND_CHECK_FUNC_RESULT(res , 80, enable_shapers);
        }

        
        res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_control_tx_nif_enable_set,(unit, port, enable));
        SOC_SAND_CHECK_FUNC_RESULT(res , 90, enable_shapers);

        ARAD_DEVICE_CHECK(unit, exit);
        
        if (SOC_IS_JERICHO(unit)) {
            res = READ_EGQ_FQP_CONFIGURATIONr(unit, core, &reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res , 91, enable_shapers);
            force_fc_mode = soc_reg_field_get(unit, EGQ_FQP_CONFIGURATIONr, reg_val, FORCE_FC_MODEf);
            soc_reg_field_set(unit, EGQ_FQP_CONFIGURATIONr, &reg_val, FORCE_FC_MODEf, 0);
            res  = WRITE_EGQ_FQP_CONFIGURATIONr(unit, core, reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res , 92, enable_shapers);

            res = READ_EGQ_FORCE_OR_IGNORE_FCr(unit, core, data_above_64);
            SOC_SAND_CHECK_FUNC_RESULT(res , 93, enable_shapers);
            res = READ_EGQ_FORCE_OR_IGNORE_FCr(unit, core, ignore_fc);
            SOC_SAND_CHECK_FUNC_RESULT(res , 94, enable_shapers);
            for (q_pair = base_q_pair; q_pair < (base_q_pair + nof_queues); q_pair++) {
                SHR_BITSET(data_above_64, q_pair);
            }
            res = WRITE_EGQ_FORCE_OR_IGNORE_FCr(unit, core, data_above_64);
            SOC_SAND_CHECK_FUNC_RESULT(res , 95, enable_shapers);
        }

        
        sal_usleep(1000);

        
        res = arad_mem_polling(
            unit,
            EGQ_BLOCK(unit, core),
            ARAD_TIMEOUT * 1000,
            ARAD_MIN_POLLS,
            EGQ_PDCMm,
            tm_port,
            ARAD_OTM_PORTS_NUM,
            PDCMf,
            0);
        if (SOC_SAND_FAILURE(res)) {
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Error: Port queues are not empty\n")));
        }
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, enable_shapers);

        mem_off = tm_port + ARAD_EGR_CGM_OTM_PORTS_NUM;
        res = arad_mem_polling(
            unit,
            EGQ_BLOCK(unit, core),
            ARAD_TIMEOUT * 1000,
            ARAD_MIN_POLLS,
            EGQ_PDCMm,
            mem_off,
            ARAD_OTM_PORTS_NUM,
            PDCMf,
            0);
        if (SOC_SAND_FAILURE(res)) {
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Error: Port queues are not empty\n")));
        }
        SOC_SAND_CHECK_FUNC_RESULT(res , 140, enable_shapers);

        
        if (SOC_IS_JERICHO(unit)) {

            soc_reg_field_set(unit, EGQ_FQP_CONFIGURATIONr, &reg_val, FORCE_FC_MODEf, force_fc_mode);
            res  = WRITE_EGQ_FQP_CONFIGURATIONr(unit, core, reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res , 141, enable_shapers);

            res = WRITE_EGQ_FORCE_OR_IGNORE_FCr(unit, core, ignore_fc);
            SOC_SAND_CHECK_FUNC_RESULT(res , 142, enable_shapers);
        }
        
enable_shapers:
       ARAD_DEVICE_CHECK(unit, exit);
       res = arad_ofp_q_pair_shapers_enable(unit, port, TRUE); 
       SOC_SAND_CHECK_FUNC_RESULT(res , 150, exit);
       res=arad_ofp_tcg_shapers_enable(unit,port,1);
       SOC_SAND_CHECK_FUNC_RESULT(res , 160, exit);
       if (SOC_IS_JERICHO(unit)) {
           res=jericho_ofp_otm_shapers_enable(unit,port,1);
           SOC_SAND_CHECK_FUNC_RESULT(res , 161, exit);
       } else {
           res=arad_ofp_otm_shapers_set(unit,port,ARAD_EGR_NOF_Q_PAIRS,otm_rates);
           SOC_SAND_CHECK_FUNC_RESULT(res , 170, exit);
       }
       if(is_channelized && if_spr_reg_val_valid)
       {
           res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_interface_shaper_set,(unit, core, tm_port, SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE,if_spr_reg_val));
           SOC_SAND_CHECK_FUNC_RESULT(res , 180, exit);
       }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("soc_arad_ports_stop_egq()", port, 0);
}


uint32
arad_port_encap_config_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN ARAD_L2_ENCAP_INFO       *info
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 10, exit);
  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_encap_config_verify()", 0, 0);
}

uint32
arad_port_encap_config_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_L2_ENCAP_INFO       *info
    )
{
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA tbl_data;
  ARAD_EGQ_PCT_TBL_DATA 
    pct_tbl_data;
  uint32
    res = SOC_SAND_OK,
    index,
    base_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
  res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,ARAD_PRGE_DATA_MEMORY_ENTRY_L2_REMOTE_CPU,&tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->vlan = tbl_data.prge_data_entry[0] & 0xffff;
  info->tpid = (tbl_data.prge_data_entry[0] & 0xffff0000) >> 16;
  for (index = 0; index < 6; ++index) {
      info->sa[5-index] = (tbl_data.prge_data_entry[1 + (index/4)] >> (index % 4)*8) & 0xff;
      info->da[5-index] = (tbl_data.prge_data_entry[2 + ((index + 2)/4)] >> ((index + 2) % 4) * 8) & 0xff;
  }

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, port_ndx, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

  res = arad_egq_pct_tbl_get_unsafe(unit, core_id, base_q_pair, &pct_tbl_data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  info->eth_type = pct_tbl_data.prog_editor_value;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_encap_config_get_unsafe()", port_ndx, 0);
}


uint32
arad_port_encap_config_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_L2_ENCAP_INFO       *info
    )
{
  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA tbl_data;
  ARAD_EGQ_PCT_TBL_DATA 
    pct_tbl_data;
  ARAD_PORT_PP_PORT_INFO pp_port_info;
  SOC_SAND_SUCCESS_FAILURE success;
  uint32
    res = SOC_SAND_OK;
  uint32
    index = port_ndx;
  uint32
    base_q_pair,
    nof_pairs,
    curr_q_pair;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
    
  res = arad_port_header_type_set_unsafe(unit,core_id,index,ARAD_PORT_DIRECTION_OUTGOING,ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_port_pp_port_get_unsafe(unit,core_id,index,&pp_port_info);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);

  pp_port_info.header_type_out = ARAD_PORT_HEADER_TYPE_L2_REMOTE_CPU;

  res = arad_port_pp_port_set_unsafe(unit,core_id,index,&pp_port_info,&success);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_pp_epni_prge_data_tbl_get_unsafe(unit,ARAD_PRGE_DATA_MEMORY_ENTRY_L2_REMOTE_CPU,&tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_REG_ABOVE_64_CLEAR(tbl_data.prge_data_entry);
  tbl_data.prge_data_entry[0] = info->vlan;
  tbl_data.prge_data_entry[0] |= (uint32)info->tpid << 16;
  for(index = 1; index <= 3; ++index) {
      tbl_data.prge_data_entry[index] = 0;
  }
  for (index = 0; index < 6; ++index) {
      tbl_data.prge_data_entry[1 + (index /4)] |= (uint32)info->sa[5-index] << ((index %4)*8);
      tbl_data.prge_data_entry[2 + ((index + 2) / 4)] |= (uint32)info->da[5-index] << (((index +2) % 4)*8);
  }

  res = arad_pp_epni_prge_data_tbl_set_unsafe(unit,ARAD_PRGE_DATA_MEMORY_ENTRY_L2_REMOTE_CPU,&tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit,  core_id, port_ndx, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_id, port_ndx, &nof_pairs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 37, exit);

  
  for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) 
  {
      res = arad_egq_pct_tbl_get_unsafe(unit, core_id, curr_q_pair,&pct_tbl_data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      pct_tbl_data.prog_editor_value = info->eth_type;

      res = arad_egq_pct_tbl_set_unsafe(unit, core_id, curr_q_pair, &pct_tbl_data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
  }

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_encap_config_set_unsafe()", port_ndx, 0);
}


STATIC uint32
  arad_ports_multicast_id_offset_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core_id,
    SOC_SAND_IN  uint32       port_ndx,
    SOC_SAND_IN  uint32       multicast_offset
    ) 
{
    uint32 res;
    uint32 data[2];
    uint32 tmp_multicast_offset;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    

    SOC_SAND_ERR_IF_ABOVE_MAX(
      unit, ARAD_MAX_DEVICE_ID,
      ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 5, exit
    );
    SOC_SAND_ERR_IF_ABOVE_MAX(
        port_ndx, ARAD_NOF_LOCAL_PORTS(unit), 
        ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit
    );
    SOC_SAND_ERR_IF_ABOVE_MAX(
        multicast_offset, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids-1, 
        ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 15, exit
    );

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core_id), port_ndx, data));

    tmp_multicast_offset = multicast_offset;
    soc_mem_field_set(
        unit, 
        IHB_PINFO_LBPm,
        data,
        MULTICAST_ID_BASEf, 
        &tmp_multicast_offset);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHB_PINFO_LBPm(unit, IHB_BLOCK(unit, core_id), port_ndx, data));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_multicast_id_offset_set()", port_ndx, 0);
}

int
  arad_port_rx_enable_get(
    int                         unit,
    soc_port_t                  port_ndx,
    int                       *enable
  )
{
    ARAD_IRE_NIF_CTXT_MAP_TBL_DATA
        nif_ctxt_table;
    uint32
        entry_offset=0,    
        res=0,
        ch_id=0,
        ctxt_map_base =0,
        if_id_internal=0,
        data=0,
        phy_port=0;
    soc_error_t
        rv = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_channel_get(unit, port_ndx, &ch_id);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_port_sw_db_first_phy_port_get(unit, port_ndx, &phy_port );
    SOCDNX_IF_ERR_EXIT(rv);
    if_id_internal= phy_port-1;
    SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));    
    ctxt_map_base = soc_IRE_NIF_PORT_MAPm_field32_get(unit, &data, CTXT_MAP_BASE_ADDRESSf);
        
    entry_offset = ctxt_map_base + ch_id;
    entry_offset = entry_offset % ARAD_PORT_NIF_CTXT_MAP_MAX;

    res = arad_ire_nif_ctxt_map_tbl_get_unsafe(
            unit,
            entry_offset,
            &(nif_ctxt_table)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);  


    if (nif_ctxt_table.reassembly_context == ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED)
    {
        *enable=FALSE;
    }
    else
    {
        *enable=TRUE;
    }

exit:
    SOCDNX_FUNC_RETURN;;

}

int
  arad_port_rx_enable_set(
    int                         unit,
    soc_port_t                  port_ndx,
    int                       enable
  )
{
    ARAD_IRE_NIF_CTXT_MAP_TBL_DATA
        nif_ctxt_table;
    uint32
        entry_offset=0,    
        reassembly_context=0,
        res=0,
        ch_id=0,
        ctxt_map_base =0,
        if_id_internal=0,
        data=0,
        phy_port=0;
    soc_error_t
        rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_port_sw_db_channel_get(unit, port_ndx, &ch_id);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_port_sw_db_first_phy_port_get(unit, port_ndx, &phy_port );
    SOCDNX_IF_ERR_EXIT(rv);
    if_id_internal= phy_port-1;
    SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_MAPm(unit, MEM_BLOCK_ANY, if_id_internal, &data));    
    ctxt_map_base = soc_IRE_NIF_PORT_MAPm_field32_get(unit, &data, CTXT_MAP_BASE_ADDRESSf);
        
    entry_offset = ctxt_map_base + ch_id;
    entry_offset = entry_offset % ARAD_PORT_NIF_CTXT_MAP_MAX;


    res = arad_ire_nif_ctxt_map_tbl_get_unsafe(
            unit,
            entry_offset,
            &(nif_ctxt_table)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);  
    
    if (enable)
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, port_ndx, &reassembly_context);
        SOCDNX_IF_ERR_EXIT(res);
    }
    else
    {
        reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED;
    }

    nif_ctxt_table.reassembly_context=reassembly_context;

    res = arad_ire_nif_ctxt_map_tbl_set_unsafe(
            unit,
            entry_offset,
            &(nif_ctxt_table)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;

}


uint32
  arad_ports_programs_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit
  ); 
  
  res = arad_fap_port_id_verify(unit, port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ptc_vt_profile, (ARAD_PORTS_NOF_VT_PROFILES-1),
    ARAD_PROFILE_NDX_OUT_OF_RANGE_ERR, 40, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ptc_tt_profile, (ARAD_PORTS_NOF_TT_PROFILES-1),
    ARAD_PROFILE_NDX_OUT_OF_RANGE_ERR, 50, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ptc_flp_profile, (ARAD_PORTS_NOF_FLP_PROFILES-1),
    ARAD_PROFILE_NDX_OUT_OF_RANGE_ERR, 60, exit
  );
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_set_verify()", port_ndx, 0);
}

uint32
  arad_ports_programs_info_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
    res,
    tbl_data_32b = 0,
    tbl_data_64b[3] = {0},
    entry_ndx,
    internal_offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  entry_ndx = port_ndx >> 3;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHP_VTT_PTC_CONFIGm(unit, MEM_BLOCK_ANY, entry_ndx, tbl_data_64b));    
  if (SOC_IS_JERICHO(unit)) {
    
    internal_offset = 5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(tbl_data_64b,internal_offset,&(info->ptc_vt_profile),0,2);

    
    internal_offset = 2+5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(tbl_data_64b,internal_offset,&(info->ptc_tt_profile),0,2);

    
    internal_offset = 4+5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(tbl_data_64b,internal_offset,&(info->en_trill_mc_in_two_path),0,1);
  } else {
    
    internal_offset = 8*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(tbl_data_64b,internal_offset,&(info->ptc_vt_profile),0,2);

    
    internal_offset = 4+8*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(tbl_data_64b,internal_offset,&(info->ptc_tt_profile),0,2);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_IHP_VTT_PTC_CONFIGm(unit, MEM_BLOCK_ANY, entry_ndx, tbl_data_64b));    

  
  entry_ndx = port_ndx >> 3;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHP_FLP_PTC_PROGRAM_SELECTm(unit, MEM_BLOCK_ANY, entry_ndx, &tbl_data_32b));    

  internal_offset = 2*(port_ndx & 0x7);
  SHR_BITCOPY_RANGE(&tbl_data_32b,internal_offset,&(info->ptc_flp_profile),0,2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_IHP_FLP_PTC_PROGRAM_SELECTm(unit, MEM_BLOCK_ANY, entry_ndx, &tbl_data_32b));    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_programs_info_set_unsafe()", port_ndx, 0);
}


uint32
  arad_ports_programs_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    unit, ARAD_MAX_DEVICE_ID,
    ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 10, exit
  ); 
  
  res = arad_fap_port_id_verify(unit, port_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_get_verify()", port_ndx, 0);
}


uint32
  arad_ports_programs_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_data_32b = 0,
    tbl_data_64b[3] = {0},
    entry_ndx,
    internal_offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  arad_ARAD_PORTS_PROGRAMS_INFO_clear(info);
  
  
  entry_ndx = port_ndx >> 3;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, READ_IHP_VTT_PTC_CONFIGm(unit, MEM_BLOCK_ANY, entry_ndx, tbl_data_64b));    

  if (SOC_IS_JERICHO(unit)) {
    
    internal_offset = 5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(&(info->ptc_vt_profile),0,tbl_data_64b,internal_offset,2);

    
    internal_offset = 2+5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(&(info->ptc_tt_profile),0,tbl_data_64b,internal_offset,2);

    
    internal_offset = 4+5*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(&(info->en_trill_mc_in_two_path),0,tbl_data_64b,internal_offset,1);
  } else {
    
    internal_offset = 8*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(&(info->ptc_vt_profile),0,tbl_data_64b,internal_offset,2);

    
    internal_offset = 4+8*(port_ndx & 0x7);
    SHR_BITCOPY_RANGE(&(info->ptc_tt_profile),0,tbl_data_64b,internal_offset,2);
  }
  
  entry_ndx = port_ndx >> 3;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHP_FLP_PTC_PROGRAM_SELECTm(unit, MEM_BLOCK_ANY, entry_ndx, &tbl_data_32b));    

  internal_offset = 2*(port_ndx & 0x7);
  SHR_BITCOPY_RANGE(&(info->ptc_flp_profile),0,&tbl_data_32b,internal_offset,2);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_ports_programs_info_get_unsafe()", port_ndx, 0);
}



uint32 arad_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
    )
{
#ifdef BCM_88660_A0
  uint32 nof_tiks, burstlog;
  uint32 fmac_index;
  int link;
  uint32 res = SOC_SAND_OK;
  uint32 freq_arad_hz;
  SOC_SAND_U64 sand_u64,
                 pps_sand_u64;
#endif 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  
  if (SOC_IS_ARAD_B1_AND_BELOW(unit))
  {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 1, exit);
  } else {
#ifdef BCM_88660_A0
      link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
      fmac_index = link / SOC_ARAD_NOF_LINKS_IN_MAC;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_TX_CELL_LIMITr,  fmac_index,  0, CELL_LIMIT_COUNTf, &burstlog));
      *burst = 1 << burstlog;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, FMAC_SPARE_REGISTER_2r,  fmac_index,  0, CELL_LIMIT_TIMER_CONFIGURATIONf, &nof_tiks));
          

      
      if (nof_tiks != 0)
      {
          freq_arad_hz = arad_chip_ticks_per_sec_get(unit);
          soc_sand_u64_clear(&sand_u64);
          soc_sand_u64_multiply_longs(*burst, freq_arad_hz, &sand_u64);
          soc_sand_u64_devide_u64_long(&sand_u64, nof_tiks, &pps_sand_u64);
          soc_sand_u64_to_long(&pps_sand_u64, pps); 
      } else {
          *pps = 0;
          *burst = 0;
      }
      
      if (*pps == 0 && *burst != 0)
      {
          *burst = 0;
      }
#endif 
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_rate_egress_pps_get()", port, 0);
}



uint32 
  arad_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
    )
{
#ifdef BCM_88660_A0
    uint32 freq_arad_hz;
    SOC_SAND_U64 sand_u64,
                 nof_tiks_sand_u64;
    uint32 nof_tiks;
    uint32 limit_max, max_length, max_length_log;
    uint32 fmac_index;
    int link;
    int is_overflowed;
    uint32 res = SOC_SAND_OK;
    uint32 burstlog, burst_temp;
    uint32 exact_burst, exact_burst_max, exact_burst_min;
    int disable = 0;
#endif 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  

  
  if (SOC_IS_ARAD_B1_AND_BELOW(unit))
  {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 1, exit);
  } else {
#ifdef BCM_88660_A0
      
       max_length_log = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_COUNTf);
       limit_max = 0;
       SHR_BITSET_RANGE(&limit_max, 0, max_length_log);

       
       burstlog = 0;
       burst_temp = burst;
       while (burst_temp > 1)
       {
           burst_temp /= 2;
           burstlog++;
       }

       
       exact_burst_min = burstlog == 0 ? 0 : 1 << burstlog;
       exact_burst_max = burstlog == 0 ? 0 : 1 << (burstlog + 1);
       if ((exact_burst_max - burst < burst - exact_burst_min) && 
           (burstlog < limit_max))
       {
           exact_burst = exact_burst_max;
           burstlog++;
       } else {
           exact_burst = exact_burst_min;
       }

      
      
       if (pps != 0)
       {
          freq_arad_hz = arad_chip_ticks_per_sec_get(unit);
          soc_sand_u64_clear(&sand_u64);
          soc_sand_u64_multiply_longs(exact_burst, freq_arad_hz, &sand_u64);
          soc_sand_u64_devide_u64_long(&sand_u64, pps, &nof_tiks_sand_u64);
          is_overflowed = soc_sand_u64_to_long(&nof_tiks_sand_u64, &nof_tiks);
          if (is_overflowed)
          {
              SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit);
          }
       } else if (burst != 0) {
           SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 10, exit);
       } else {
           nof_tiks = 0;
           disable = 1;
       }

      

 
       max_length_log = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_COUNTf);
       limit_max = 0;
       SHR_BITSET_RANGE(&limit_max, 0, max_length_log);
       SOC_SAND_ERR_IF_ABOVE_MAX(
           burstlog, limit_max,
           SOC_TMC_INPUT_OUT_OF_RANGE, 20, exit
        );

       
       max_length = soc_reg_field_length(unit, FMAC_SPARE_REGISTER_2r,  CELL_LIMIT_TIMER_CONFIGURATIONf);
       limit_max = 0;
       SHR_BITSET_RANGE(&limit_max, 0, max_length);
       SOC_SAND_ERR_IF_ABOVE_MAX(
           nof_tiks, limit_max,
           SOC_TMC_INPUT_OUT_OF_RANGE, 30, exit
        );

       if (nof_tiks == 0 && burst != 0)
       {
           
           SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 40, exit);
       }

       if (exact_burst < 3 && exact_burst != 0){
            
           SOC_SAND_SET_ERROR_CODE(SOC_TMC_INPUT_OUT_OF_RANGE, 50, exit);
       }

       
       link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
       fmac_index = link / SOC_ARAD_NOF_LINKS_IN_MAC;
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_TX_CELL_LIMITr,  fmac_index,  0, CELL_LIMIT_COUNTf,  burstlog));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_SPARE_REGISTER_2r,  fmac_index,  0, OVERRIDE_CELL_LIMIT_TIMER_CONFIGURATIONf,  0x1)); 
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_SPARE_REGISTER_2r,  fmac_index,  0, CELL_LIMIT_TIMER_CONFIGURATIONf,  nof_tiks));

       
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90 ,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FMAC_TX_CELL_LIMITr,  fmac_index,  0, CELL_LIMIT_PERIODf,  disable ? 0 : 0xf ));
#endif 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_port_rate_egress_pps_set()", port, 0);
}


uint32 
  arad_ports_application_mapping_info_set_unsafe (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info)
{
  uint32
    res,
    tbl_data_32b,
    entry_index;
  uint32
    tm_port, ptc_port = 0, pp_port;
  uint32
    base_q_pair,
    nof_pairs,
    curr_q_pair;
  ARAD_EGQ_PCT_TBL_DATA 
    pct_tbl_data;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA pp_pct_tbl_data;
  soc_port_t
    port_match_channel = 0;
  int core=0; 
  soc_pbmp_t 
    vendor_pp_port_bm,
    coe_port_bm;
  uint16 xgs_mac_ext_encode;
  uint32 value;
  uint32 tm_profile;
  uint32 flags;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 42, exit);
  res = soc_port_sw_db_local_to_pp_port_get(unit, port_ndx, &pp_port, &core);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);


  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER) 
  {    

    

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,  exit, soc_port_sw_db_port_ptc_get(unit, port_ndx, &port_match_channel));
    if(port_match_channel == SOC_MAX_NUM_PORTS)
    {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_SOC_ERR,20,exit);
    }
    ptc_port = port_match_channel;
    
    xgs_mac_ext_encode = arad_virtual_port_xgs_mac_ext_encode(unit, ptc_port,info->value.xgs_mac_ext.hg_port);
    res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, info->value.xgs_mac_ext.pp_port);

    res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), xgs_mac_ext_encode, &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    
    value = soc_property_port_get(unit, port_ndx, "xgs_mac_ext_tm_mode", 0);
    if (value == 1) {        
        res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, info->value.xgs_mac_ext.pp_port, 4);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);
    }

    
    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_port_sw_db_flags_get(unit, port_ndx, &flags);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit); 

    
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {

        res = arad_egq_pct_tbl_get_unsafe(
              unit,
              core,
              curr_q_pair,
              &pct_tbl_data
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80 + curr_q_pair, exit); 
        
        pct_tbl_data.prog_editor_value = ARAD_PORTS_XGS_MAC_EXT_PROG_EDITOR_VALUE(info->value.xgs_mac_ext.hg_modid,info->value.xgs_mac_ext.hg_port);
        
        if (value == 1) {
            pct_tbl_data.prog_editor_profile = 15;
        }

        res = arad_egq_pct_tbl_set_unsafe(
              unit,
              core,
              curr_q_pair,
              &pct_tbl_data
            );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90 + curr_q_pair, exit); 
    }
    
    if (flags & SOC_PORT_FLAGS_XGS_MAC_EXT) {
        res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port, &pp_pct_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 191, exit);
        pp_pct_tbl_data.prge_var = 1;        
        res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core, pp_port, &pp_pct_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 192, exit);
    }
  }  

  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER) {
    
    entry_index = info->value.pp_port_ext.index;
    
    SOC_PBMP_CLEAR(coe_port_bm);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_COE_PORT, &coe_port_bm));
    
    if (PBMP_MEMBER(coe_port_bm, port_ndx)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50,  exit, soc_port_sw_db_port_ptc_get(unit, port_ndx, &port_match_channel));
        if(port_match_channel == SOC_MAX_NUM_PORTS) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_SOC_ERR,20,exit);
        }
        ptc_port = port_match_channel;
        entry_index = ARAD_PORTS_VIRTUAL_PORT_COE_ENCODED(SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port, ptc_port, info->value.pp_port_ext.index);
    }

    res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), entry_index, &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
    
    
    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port == 1) {
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, pp_port);
    } else {
        soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, info->value.pp_port_ext.pp_port);
    }

    res = WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), entry_index, &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

    SOC_PBMP_CLEAR(vendor_pp_port_bm);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_VENDOR_PP_PORT, &vendor_pp_port_bm));

    if (SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()) {
        if (PBMP_MEMBER(vendor_pp_port_bm, port_ndx)) {
            ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
              vtt_in_pp_port_config_tbl_data;
            ARAD_PORTS_PROGRAMS_INFO programs_info;
            soc_port_if_t interface;
            int addtional_remove = 0;

            programs_info.ptc_vt_profile  = ARAD_PORTS_VT_PROFILE_CUSTOM_PP;
            programs_info.ptc_tt_profile  = ARAD_PORTS_TT_PROFILE_NONE;
            programs_info.ptc_flp_profile = ARAD_PORTS_FLP_PROFILE_NONE;
            programs_info.en_trill_mc_in_two_path = 0;
            res = arad_ports_programs_info_set_unsafe(
                    unit,
                    port_ndx,
                    &programs_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

            
            if ((info->value.pp_port_ext.index>>(ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_MSB+1)) < ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_UC_CTRL_TYPE) {
               addtional_remove = 1;
            } else if (((info->value.pp_port_ext.index>>ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_2_MSB) >= ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_3_MC_TYPE)
                 && ((((info->value.pp_port_ext.index>>(ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_MSB+1))&
                    ((0x1L<<ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SIZE)-1)) >= ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SUB_TYPE))) {
               if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "egress_l3_mc_recycle", 0)){
                   addtional_remove = 1;
               }
            }

            if (addtional_remove) {
                
                value = SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VENDOR_PP_2;
                
                value += info->value.pp_port_ext.index & ((0x1L<<ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_SIZE)-1);

                res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, info->value.pp_port_ext.pp_port, value);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
            }
            
            res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
              unit,
              core,
              info->value.pp_port_ext.pp_port,
              &vtt_in_pp_port_config_tbl_data
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
        
            
            value = (info->value.pp_port_ext.index>>ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_0_SIZE) & ((0x1L<<ARAD_PORTS_FEM_PP_PORT_VENDOR2_FIELD_1_SIZE)-1);
            vtt_in_pp_port_config_tbl_data.vlan_translation_profile = value;
        
            res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
              unit,
              core,
              info->value.pp_port_ext.pp_port,
              &vtt_in_pp_port_config_tbl_data
            );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

            
            res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &core);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);
            res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 161, exit);
            res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 162, exit);

            res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_CUSTOMER_CONN_TO_NP, &tm_profile);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 165, exit);

            for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {
                res = arad_egq_pct_tbl_get_unsafe(unit, core, curr_q_pair, &pct_tbl_data);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 170 + curr_q_pair, exit);
                
                pct_tbl_data.prog_editor_profile = tm_profile;

                res = arad_egq_pct_tbl_set_unsafe(unit, core, curr_q_pair, &pct_tbl_data);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180 + curr_q_pair, exit); 
            }

            res = soc_port_sw_db_interface_type_get(unit, port_ndx, &interface);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 185, exit);

            
            if (SOC_PORT_IF_RCY != interface ) {
                res = soc_port_sw_db_local_to_pp_port_get(unit, port_ndx, &pp_port, &core);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 190, exit);

                res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core, pp_port, &pp_pct_tbl_data);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 191, exit);
                	
                pp_pct_tbl_data.prge_profile = ARAD_PRGE_PP_SELECT_CUSTOMER_CONN_TO_NP;

                res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core, pp_port, &pp_pct_tbl_data);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 192, exit);
            }
        }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_set_unsafe()", port_ndx, 0);
}

uint32
  arad_ports_application_mapping_info_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info)
{
  uint8 
    is_xgs_mac_ext;
  uint32 
    res;
  soc_pbmp_t 
    vendor_pp_port_bm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_DEVICE_CHECK(unit, exit); 
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
      port_ndx, SOC_DPP_DEFS_GET(unit, nof_logical_ports), 
      ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit
  );

  
  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER) 
  {
    ARAD_PORT_IS_XGS_MAC_EXT_PORT(port_ndx,is_xgs_mac_ext);
  
    if (!is_xgs_mac_ext) 
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 30, exit);
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(
    info->value.xgs_mac_ext.hg_port, ARAD_PORTS_XGS_HG_PORT_ID_MAX,
      SOC_TMC_INPUT_OUT_OF_RANGE, 40, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
    info->value.xgs_mac_ext.hg_modid, ARAD_PORTS_XGS_HG_MODID_ID_MAX,
      SOC_TMC_INPUT_OUT_OF_RANGE, 50, exit);
    

    res = arad_fap_port_id_verify(unit, info->value.xgs_mac_ext.pp_port);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  
  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER) 
  {
    
    SOC_PBMP_CLEAR(vendor_pp_port_bm);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_VENDOR_PP_PORT, &vendor_pp_port_bm));
    if (PBMP_MEMBER(vendor_pp_port_bm, port_ndx)) {
      if (!SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()) {
        SOC_SAND_ERR_IF_ABOVE_MAX(
        ((info->value.pp_port_ext.index>>6)&0xFF), ARAD_PORTS_VENDOR_PP_SOURCE_BOARD_MAX,
          SOC_TMC_INPUT_OUT_OF_RANGE, 90, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_set_verify()", port_ndx, 0);
}

uint32 
  arad_ports_application_mapping_info_get_unsafe (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT ARAD_PORTS_APPLICATION_MAPPING_INFO *info)
{
  uint32
    res,
    tm_port,
    tbl_data_32b,
    entry_index;
  ARAD_PORTS_XGS_MAC_EXT_INFO
    mac_ext_info;
  int
    core;
  soc_pbmp_t 
    coe_port_bm;
  uint16 xgs_mac_ext_encode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER) 
  {
   
    
    res = soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    res = arad_ports_local_to_xgs_mac_extender_info_get(unit, tm_port, core, &mac_ext_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    
        
    xgs_mac_ext_encode = arad_virtual_port_xgs_mac_ext_encode(unit, mac_ext_info.ptc_port, mac_ext_info.hg_remote_port);
    
    res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, xgs_mac_ext_encode, &tbl_data_32b);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    info->value.xgs_mac_ext.pp_port = soc_IHP_VIRTUAL_PORT_TABLEm_field32_get(unit, &tbl_data_32b, IN_PP_PORTf);
    info->value.xgs_mac_ext.hg_modid = mac_ext_info.hg_remote_modid;
    info->value.xgs_mac_ext.hg_port  = mac_ext_info.hg_remote_port;
  }

  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_PP_PORT_EXTENDER) {
      entry_index = info->value.pp_port_ext.index;

      SOC_PBMP_CLEAR(coe_port_bm);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_COE_PORT, &coe_port_bm));

      if (PBMP_MEMBER(coe_port_bm, port_ndx)) {
          entry_index = ARAD_PORTS_VIRTUAL_PORT_COE_ENCODED(SOC_DPP_CONFIG(unit)->pp.custom_feature_pp_port, port_ndx, info->value.pp_port_ext.index);
      }

      res = READ_IHP_VIRTUAL_PORT_TABLEm(unit, MEM_BLOCK_ANY, entry_index, &tbl_data_32b);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      
      info->value.pp_port_ext.pp_port = soc_IHP_VIRTUAL_PORT_TABLEm_field32_get(unit, &tbl_data_32b, IN_PP_PORTf);
      info->value.pp_port_ext.sys_port = soc_IHP_VIRTUAL_PORT_TABLEm_field32_get(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_get_unsafe()", port_ndx, 0);
}

uint32
  arad_ports_application_mapping_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN ARAD_FAP_PORT_ID          port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    )
{
  uint8 
    is_xgs_mac_ext;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_DEVICE_CHECK(unit, exit); 
  
  
  SOC_SAND_ERR_IF_ABOVE_MAX(
      port_ndx, SOC_DPP_DEFS_GET(unit, nof_logical_ports), 
      ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 10, exit
  );


  
  if (info->type == ARAD_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER) 
  {
    ARAD_PORT_IS_XGS_MAC_EXT_PORT(port_ndx,is_xgs_mac_ext);
  
    if (!is_xgs_mac_ext) 
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_get_unsafe()", port_ndx, 0);
}

int
arad_ports_header_type_get(
    int                      unit,
    soc_port_t               port,
    uint32                   port_extender_enable,
    SOC_TMC_PORT_HEADER_TYPE *p_val_incoming, 
    SOC_TMC_PORT_HEADER_TYPE *p_val_outgoing, 
    uint32                   *is_hg_header
 )
{
    int rv = SOC_E_NONE;
    uint8 pp_enable;
    SOC_TMC_MGMT_TDM_MODE tdm_mode;
    soc_dpp_config_pp_t *dpp_pp;

    SOCDNX_INIT_FUNC_DEFS;

    if (NULL != is_hg_header) {
        (*is_hg_header) = 0;
    }

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_drv_prop_fap_device_mode_get, (unit, &pp_enable, &tdm_mode));
    SOCDNX_IF_ERR_EXIT(rv);
    
    if (pp_enable) {
        *p_val_incoming = ARAD_PORT_HEADER_TYPE_ETH;
        *p_val_outgoing = ARAD_PORT_HEADER_TYPE_ETH;
    } else {
        *p_val_incoming = ARAD_PORT_HEADER_TYPE_TM;
        *p_val_outgoing = ARAD_PORT_HEADER_TYPE_TM;
    }

    if (port_extender_enable){
        dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
        
        if (dpp_pp->pon_application_enable) {
            *p_val_incoming = ARAD_PORT_HEADER_TYPE_ETH;
            *p_val_outgoing = ARAD_PORT_HEADER_TYPE_ETH;
        }

        if (dpp_pp->xgs_port_exists) {
            if (NULL != is_hg_header) {
                if (soc_dpp_str_prop_parse_tm_port_header_type(unit, port, p_val_incoming, p_val_outgoing, is_hg_header)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_header_type error")));
                }
            }
            *p_val_incoming = ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT;
            *p_val_outgoing = ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT;
        }

        if (dpp_pp->udh_port_exists) {
            *p_val_incoming = ARAD_PORT_HEADER_TYPE_UDH_ETH;
            *p_val_outgoing = ARAD_PORT_HEADER_TYPE_UDH_ETH;
        }

        if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 1) {
            *p_val_incoming = ARAD_PORT_HEADER_TYPE_INJECTED_2_PP;
            *p_val_outgoing = ARAD_PORT_HEADER_TYPE_ETH;
        }

        if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
            *p_val_incoming = ARAD_PORT_HEADER_TYPE_ETH;
            *p_val_outgoing = ARAD_PORT_HEADER_TYPE_COE;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_header_info_update(
      int                   unit,
      soc_port_t            port,
      uint32                port_extender_enable
   )
{
    soc_port_if_t interface;
    uint32 first_header_size, pp_port, tm_port;
    ARAD_PORT_HEADER_TYPE header_type_in, header_type_out;
    int core;
    uint32 is_hg_header, is_sop_only;
    soc_dpp_config_pp_t *dpp_pp;
    soc_error_t rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));     

    if(SOC_PORT_IF_ERP != interface) {
        if (arad_ports_header_type_get(unit, port, port_extender_enable, &(header_type_in), &(header_type_out), &is_hg_header)) {    
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_header_type error")));       
        }
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_set(unit, port, header_type_in));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit, port, header_type_out));
        
        if(is_hg_header) { 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_hg_set(unit, port, is_hg_header));
        }

        is_sop_only = soc_property_port_get(unit, port, spn_PREAMBLE_SOP_ONLY, FALSE);
        if(IS_HG_PORT(unit,port)){
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                if(is_sop_only){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("can't set port %d to be hg and preamble sop only"), port));
                }
            }
#endif 
        }
        else{
#ifdef BCM_88660_A0
            if (SOC_IS_ARADPLUS(unit)) {
                if(is_sop_only){
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_set(unit, port, SOC_ENCAP_SOP_ONLY));
                }
            }
#endif 
        }
        
        if(SOC_PORT_IF_OLP != interface && SOC_PORT_IF_OAMP != interface) {
            
            if (header_type_in == ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT) {
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_XGS_MAC_EXT));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_XGS_MAC_EXT));
            }
        }

        if (port_extender_enable && dpp_pp->custom_feature_pp_port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_VENDOR_PP_PORT));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_VENDOR_PP_PORT));
        }

        if (port_extender_enable && dpp_pp->port_extender_map_lc_exists) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_COE_PORT));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_COE_PORT));
        }

        if (port_extender_enable && dpp_pp->pon_application_enable) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_PON_INTERFACE));
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_PON_INTERFACE));
        }


        first_header_size = soc_property_port_get(unit, port, spn_FIRST_HEADER_SIZE, 0);

        if (port_extender_enable) {
            if ((header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) || (header_type_in == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP)) 
            {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PTCH_2;
                
                if(SOC_IS_JERICHO(unit) && (SOC_PORT_IF_OAMP == interface) && dpp_pp->oam_statistics)
                    first_header_size += 2;
            }
            else if ((header_type_in == ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT)) {
               first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_FRC_PPD;
            }
            else if (dpp_pp->custom_feature_pp_port) {
                first_header_size += SOC_DPP_CONFIG_CUSTOM_PP_PORT_IS_VENDOR2()?SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VENDOR_PP_2:SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VENDOR_PP;
            } 
            else if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_COE;
            }
            else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination == 1) {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_VN_TAG;
            } 
            else if (SOC_DPP_CONFIG(unit)->extender.port_extender_init_status) {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_802_1_BR;
            }
            else if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_PON;
            }
    
             
            if (is_hg_header && (!((header_type_in == ARAD_PORT_HEADER_TYPE_XGS_HQoS) 
                                   || header_type_in == ARAD_PORT_HEADER_TYPE_XGS_DiffServ
                                   || header_type_in == ARAD_PORT_HEADER_TYPE_XGS_MAC_EXT))) 
            {
                first_header_size += SOC_DPP_ARAD_INJECTED_HEADER_SIZE_BYTES_HIGIG_FB;
            }
        }

        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_header_size.set(unit, port, first_header_size);
        SOCDNX_IF_ERR_EXIT(rv);
    } 

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_header_type_update(
      int                   unit,
      soc_port_t            port
   )
{
    soc_port_if_t interface;
    uint32 first_header_size, pp_port, tm_port, flags;
    ARAD_PORT_HEADER_TYPE header_type_in, header_type_out;
    int core;
    SOC_SAND_SUCCESS_FAILURE success;
    int is_tm_src_syst_port_ext_present;
    int is_stag_enabled, is_snoop_enabled, is_tm_ing_shaping_enabled;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

    if (interface != SOC_PORT_IF_ERP && !SOC_PORT_IS_ELK_INTERFACE(flags)) {
        ARAD_PORT_PP_PORT_INFO conf;
        

        
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_header_size.get(unit, port, &first_header_size);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port, &header_type_out);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_in.get(unit, port, &header_type_in);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        if (interface != SOC_PORT_IF_IPSEC) 
        {
            SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_set(unit, core, tm_port, first_header_size));
        }
        SOCDNX_IF_ERR_EXIT(handle_sand_result(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_set,(unit,core,tm_port,ARAD_PORT_DIRECTION_INCOMING,header_type_in))));
        SOCDNX_IF_ERR_EXIT(handle_sand_result(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_set,(unit,core,tm_port,ARAD_PORT_DIRECTION_OUTGOING,header_type_out))));

        if (!SOC_UNIT_VALID(unit))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
        }
        
        if (header_type_out == ARAD_PORT_HEADER_TYPE_TM && !SOC_PORT_IS_STAT_INTERFACE(flags)) {

            ARAD_PORTS_OTMH_EXTENSIONS_EN otmh_ext_en;
            int src_ext_en, dest_ext_en;

            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.get(unit, port, &otmh_ext_en.outlif_ext_en);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.get(unit, port, &src_ext_en);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.dst_ext_en.get(unit, port, &dest_ext_en);
            SOCDNX_IF_ERR_EXIT(rv);

            otmh_ext_en.src_ext_en = (src_ext_en ? 1 : 0);
            otmh_ext_en.dest_ext_en = (dest_ext_en ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(arad_ports_otmh_extension_set(unit,core,tm_port,&otmh_ext_en));
        }

        

        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.fc_type.get(unit, port, &conf.fc_type);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.pp_flags.get(unit, port, &conf.flags);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.mirror_profile.get(unit, port, &conf.mirror_profile);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_tm_src_syst_port_ext_present.get(unit, port, &is_tm_src_syst_port_ext_present);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_stag_enabled.get(unit, port, &is_stag_enabled);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_snoop_enabled.get(unit, port, &is_snoop_enabled);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.is_tm_ing_shaping_enabled.get(unit, port, &is_tm_ing_shaping_enabled);
        SOCDNX_IF_ERR_EXIT(rv);
        
        conf.is_tm_src_syst_port_ext_present = (is_tm_src_syst_port_ext_present ? 1 : 0);        
        conf.is_stag_enabled = (is_stag_enabled ? 1 : 0);
        conf.is_snoop_enabled = (is_snoop_enabled ? 1 : 0);
        conf.is_tm_ing_shaping_enabled = (is_tm_ing_shaping_enabled ? 1 : 0);
        conf.first_header_size = first_header_size;
        conf.header_type = header_type_in;
        conf.header_type_out = header_type_out;

        SOCDNX_IF_ERR_EXIT(arad_port_pp_port_set_unsafe(unit, core, pp_port, &conf,&success));

        if (!SOC_SAND_SUCCESS2BOOL(success)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_INIT_TM_PROFILE_OUT_OF_RSRC_ERR")));
        }

        

        SOCDNX_IF_ERR_EXIT(arad_port_to_pp_port_map_set_verify(unit, port, ARAD_PORT_DIRECTION_BOTH));
        SOCDNX_IF_ERR_EXIT(arad_port_to_pp_port_map_set_unsafe(unit, port, ARAD_PORT_DIRECTION_BOTH));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_reference_clock_set(
      int                   unit,
      soc_port_t            port
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if((ARAD_INIT_SERDES_REF_CLOCK_125 == SOC_DPP_CONFIG(unit)->arad->init.pll.nif_clk_freq)) {
        SOC_INFO(unit).port_refclk_int[port] = 125;
    } else {
        SOC_INFO(unit).port_refclk_int[port] = 156;
    }

    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_port_to_nif_id_get(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   int                   core,
      SOC_SAND_IN   uint32                tm_port,
      SOC_SAND_OUT  ARAD_INTERFACE_ID     *if_id
   )
{
    uint32          first_phy_port;
    soc_port_t      port;
    ARAD_NIF_TYPE   nif_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port,  &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port));

    SOCDNX_SAND_IF_ERR_EXIT(arad_port_to_nif_type(unit, tm_port, &nif_type));
    *if_id = arad_nif_intern2nif_id(unit, nif_type, first_phy_port-1);

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_ports_ptc_info_update(
      int                   unit,
      soc_port_t            port,
      uint32                port_extender_enable
   )
{
    uint32
      fld_val;
    uint32 pp_port;
    int core;
    uint32
      tbl_data_64b[2] = {0};
    soc_dpp_config_pp_t *dpp_pp;

    SOCDNX_INIT_FUNC_DEFS;

    dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    
    SOCDNX_IF_ERR_EXIT(READ_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b));

    
    fld_val = 0x1; 
    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUE_TO_USEf, fld_val);

    
    fld_val = ARAD_PORTS_FEM_PROFILE_DIRECT_EXTR;

    if (port_extender_enable) {
        
        if (dpp_pp->custom_feature_pp_port) {
            fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_VENDOR_PP;
        }

        
        if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
            fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_COE;
        }

        if (dpp_pp->pon_application_enable) {
            fld_val = ARAD_PORTS_FEM_PROFILE_PP_PORT_PROFILE_PON;
        }   

    }

    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, PROFILEf, fld_val);
    
    
    fld_val = pp_port;
    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, VALUEf, fld_val);
    
    
    fld_val = 0;

    if (port_extender_enable) {
        
        if (dpp_pp->custom_feature_pp_port) {
            fld_val = ARAD_PORTS_FEM_VENDOR_CUSTOM_PP_PORT_OFFSET;
        }

        
        if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
            fld_val = ARAD_PORTS_FEM_COE_PORT_OFFSET;
        }

        if (dpp_pp->pon_application_enable) {
            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0))) {
                fld_val = ARAD_PORTS_FEM_PON_PP_PORT_OFFSET;
            } else {
                fld_val = 0;
            }
        }        
    }
    soc_IHP_PTC_VIRTUAL_PORT_CONFIGm_field32_set(unit, tbl_data_64b, OFFSETf, fld_val);
    
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_PTC_VIRTUAL_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, tbl_data_64b));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_prog_editor_value_update(
      int                   unit,
      soc_port_t            port,
      uint32                prog_editor_value
   )
{
    uint32
        res;
    uint32
      base_q_pair,
      nof_pairs,
      curr_q_pair;
    ARAD_EGQ_PCT_TBL_DATA 
      pct_tbl_data;
    int
      core = 0;
    uint32
      tm_port = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_SAND_IF_ERR_RETURN(res); 
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
    SOCDNX_SAND_IF_ERR_RETURN(res); 

    
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {

        res = arad_egq_pct_tbl_get_unsafe(
              unit,
              core,
              curr_q_pair,
              &pct_tbl_data
            );
        SOCDNX_SAND_IF_ERR_RETURN(res); 

        pct_tbl_data.prog_editor_value = prog_editor_value; 

        res = arad_egq_pct_tbl_set_unsafe(
              unit,
              core,
              curr_q_pair,
              &pct_tbl_data
            );
        SOCDNX_SAND_IF_ERR_RETURN(res); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_prog_editor_value_read(
      int                   unit,
      soc_port_t            port,
      uint32                *prog_editor_value
   )
{
    uint32
        res;
    uint32
      base_q_pair,
      nof_pairs,
      curr_q_pair;
    ARAD_EGQ_PCT_TBL_DATA 
      pct_tbl_data;
    int
      core = 0;
    uint32
      tm_port = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_SAND_IF_ERR_RETURN(res); 
    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
    SOCDNX_SAND_IF_ERR_RETURN(res); 

    
    curr_q_pair = base_q_pair; 

        res = arad_egq_pct_tbl_get_unsafe(
              unit,
              core,
              curr_q_pair,
              &pct_tbl_data
            );
        SOCDNX_SAND_IF_ERR_RETURN(res); 

        *prog_editor_value = pct_tbl_data.prog_editor_value; 
exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_extender_mapping_enable_set(
      int                   unit,
      soc_port_t            port,
      int                   value
   )
{
    uint32
        res;
    ARAD_PORTS_SWAP_INFO ports_swap_info;
    uint32
        prog_editor_value;
    uint32 pp_port;
    int core;

    ARAD_PORT_HEADER_TYPE header_type_outgoing, header_type_incoming;
    uint32 base_q_pair;
    uint32 entry_context[3]; 
    uint32 channel;
    soc_port_t rcy_port;
    uint32 tm_rcy_port;
    int core_rcy_port;
    uint32 flags;


    SOCDNX_INIT_FUNC_DEFS;

    
    if (!((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists) && !(SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination == 1) && !(SOC_DPP_CONFIG(unit)->extender.port_extender_init_status)) {
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

    
    if (SOC_IS_JERICHO(unit) && ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2)) {
        if (!(value & 0x1) && (value & 0x2)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Don't set TC/DP indication while port isn't enabled")));
        }
    }

    SOCDNX_IF_ERR_EXIT(arad_ports_ptc_info_update(unit, port, value));
    SOCDNX_IF_ERR_EXIT(arad_ports_header_info_update(unit, port, value));
    SOCDNX_IF_ERR_EXIT(arad_ports_header_type_update(unit, port));

    arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);

    
    if (((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) || (SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination == 1) || (SOC_DPP_CONFIG(unit)->extender.port_extender_init_status)) {    
        ports_swap_info.enable = value ? TRUE:FALSE;
        
        
        res = arad_ports_swap_set_unsafe(unit,
                                         port,
                                         &ports_swap_info);
        SOCDNX_SAND_IF_ERR_RETURN(res);
    }

    if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists) {
        if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
            prog_editor_value = value;
        } else {
            prog_editor_value = value ? ARAD_EGR_PROG_EDITOR_PORT_EXTENDER_HEADER_SIZE : 0;
        }
        SOCDNX_IF_ERR_EXIT(arad_ports_prog_editor_value_update(unit, port, prog_editor_value));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core, pp_port, &base_q_pair));
    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core), base_q_pair, entry_context));

    SOCDNX_IF_ERR_EXIT(arad_port_header_type_get_unsafe(unit, core, pp_port, &header_type_incoming, &header_type_outgoing));

    channel = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf);
    if (soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) {
        SOCDNX_IF_ERR_EXIT(get_recycling_port(unit,core, pp_port, channel, &rcy_port));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, rcy_port, &tm_rcy_port, &core_rcy_port));
    }

    
    if (((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 1) && (header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH)) {
        if (soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) {            
            if ((value != 0) && (SOC_PORT_IS_COE_PORT(flags))) {
                SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_set(unit, core_rcy_port, tm_rcy_port, 1));
            } else if (value == 0) {
                SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_set(unit, core_rcy_port, tm_rcy_port, 0));
            }
        }
    }

    if (((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) && (header_type_outgoing == ARAD_PORT_HEADER_TYPE_ETH))  {
        if (soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) {
            ARAD_PORTS_SWAP_INFO ports_swap_info;            
            arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);

            if ((value != 0) && (SOC_PORT_IS_COE_PORT(flags))) {
                ports_swap_info.enable = TRUE;
                res = arad_ports_swap_set_unsafe(unit, rcy_port, &ports_swap_info);
                SOCDNX_SAND_IF_ERR_RETURN(res);               
                
                SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_set(unit, core_rcy_port, tm_rcy_port, 4));
            } else if (value == 0) {
                ports_swap_info.enable = FALSE;
                res = arad_ports_swap_set_unsafe(unit, rcy_port, &ports_swap_info);
                SOCDNX_SAND_IF_ERR_RETURN(res);
                SOCDNX_IF_ERR_EXIT(arad_parser_nof_bytes_to_remove_set(unit, core_rcy_port, tm_rcy_port, 0));
            }
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_ports_extender_mapping_enable_get(
      int                   unit,
      soc_port_t            port,
      int                   *value
   )
{
    uint32 
      res;
    uint32 
      tm_port;
    int 
      core;
    soc_pbmp_t 
      coe_port_bm;
    ARAD_PORTS_SWAP_INFO swap_info;

    SOCDNX_INIT_FUNC_DEFS;

    *value = 0;

    
    if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

        SOC_PBMP_CLEAR(coe_port_bm);
        res = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_COE_PORT, &coe_port_bm);
        SOCDNX_SAND_IF_ERR_RETURN(res);

        if (PBMP_MEMBER(coe_port_bm, port)) {
            *value = 1;
        }
        SOC_EXIT;
    } else if ((SOC_DPP_CONFIG(unit)->pp.custom_feature_vn_tag_port_termination == 1) || (SOC_DPP_CONFIG(unit)->extender.port_extender_init_status)) {
        
        res = arad_ports_swap_get_unsafe(unit, port, &swap_info);
        SOCDNX_SAND_IF_ERR_RETURN(res);
        *value = swap_info.enable;
        SOC_EXIT;
    } else {
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
  arad_pon_port_enable_set(
      int                   unit,
      soc_port_t            port,
      int                   value
   )
{
    uint32 flags;
    ARAD_PORTS_SWAP_INFO ports_swap_info;
    soc_port_if_t port_interface;
    uint32 nof_channels, is_hg, idx, channel;
    SOC_TMC_INTERFACE_ID nif_id;
    soc_pbmp_t phy_ports;
    ARAD_PORTS_PROGRAMS_INFO port_prog_info;
    uint32 pp_port, pp_port_tmp, tm_port;
    uint32 pon_app_status_is_change;
    soc_pbmp_t pon_port_bm; 
    
    int core;
    uint32 count;
    uint32 res;
    SOC_PPC_PORT_INFO port_info;
    SOC_SAND_PP_VLAN_ID vlan;
    uint32 pon_tunnel_id_alloc_mode, tunnel_id, tbl_data_32b;
    uint32 base_q_pair, nof_pairs, curr_q_pair;
    ARAD_EGQ_PCT_TBL_DATA pct_tbl_data;
    uint32 prge_prog_select;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

    if (port >= _BCM_PPD_NOF_PON_PHY_PORT(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Can't enable PON port on this port")));
    }

    if (!is_pon_dpp_support) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("PON DPP can't support, enable it first!")));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    if ((flags & SOC_PORT_FLAGS_PON_INTERFACE) && value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NONE, (_BSL_SOCDNX_MSG("PON port has been enabled")));
    }
    if (!(flags & SOC_PORT_FLAGS_PON_INTERFACE) && !value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_NONE, (_BSL_SOCDNX_MSG("PON port has been disabled")));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_arad_str_prop_parse_ucode_port(unit, port, &nif_id, &channel, &is_hg));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &port_interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_phy_ports_get(unit, port, &phy_ports));

    if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable == 0) && (value)) {
        pon_app_status_is_change = 1;
    } else if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable == 1) && (!value)) {
        SOC_PBMP_CLEAR(pon_port_bm);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));
        if (!SOC_PBMP_MEMBER(pon_port_bm, port)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("This is not a PON port, can't disable")));
        }
        
        SOC_PBMP_PORT_REMOVE(pon_port_bm, port);
        SOC_PBMP_COUNT(pon_port_bm, count);
        pon_app_status_is_change = (count) ? 0 : 1;
    } else {
        pon_app_status_is_change = 0;
    }


    if (value) {
        
        if (SOC_DPP_CONFIG(unit)->pp.evb_enable) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("EVB and PON do not co exist on same device")));
        }
        if (SOC_DPP_CONFIG(unit)->trill.mode) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("PON and Trill do not co exist on same device")));
        }

        
        if (pon_app_status_is_change) {
            SOC_DPP_CONFIG(unit)->pp.pon_application_enable = 1;            
        }
        PORT_SW_DB_PORT_ADD(pon, port);

        
        
        
        if (port_interface != SOC_PORT_IF_CPU) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &nof_channels));
            if (nof_channels > 1) {
              SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_enable = 1;
            }
            
            if (nof_channels > SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num) {
                SOC_DPP_CONFIG(unit)->pp.pon_port_channelization_num = nof_channels;
            }
        }

    } else {
        
        if (pon_app_status_is_change) {
            SOC_DPP_CONFIG(unit)->pp.pon_application_enable = 0;            
        }
        PORT_SW_DB_PORT_REMOVE(pon, port);
    }
    
    SOCDNX_IF_ERR_EXIT(arad_ports_ptc_info_update(unit, port, value));
    
    if (value) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_add(unit, port, SOC_PORT_FLAGS_PON_INTERFACE));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flag_remove(unit, port, SOC_PORT_FLAGS_PON_INTERFACE));
    }

    
    arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);
    ports_swap_info.enable = value ? TRUE : FALSE;
    res = arad_ports_swap_set_unsafe(unit, port, &ports_swap_info);
    SOCDNX_SAND_IF_ERR_RETURN(res);

    
    if (!_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
        for (idx = 0; idx < 16; idx++) {
            pp_port_tmp = ((idx << 3) | port);
            res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, pp_port_tmp, (value ? 4 : 0));
            SOCDNX_SAND_IF_ERR_RETURN(res);
        }
    } else {
        res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, pp_port, (value ? 4 : 0));
        SOCDNX_SAND_IF_ERR_RETURN(res);
    }

    
    pon_tunnel_id_alloc_mode = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "pon_tunnel_id_alloc_mode", 1);
    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit) == 0) {
        if (pon_tunnel_id_alloc_mode == 0) {
            for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_NOF_ENTRIES_MODE0(unit, port); tunnel_id++) {
                SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port,tunnel_id), &tbl_data_32b));
                soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, value? pp_port : 0);                
                soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, value? port : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED_MODE0(port,tunnel_id), &tbl_data_32b));
            }
        } else if (pon_tunnel_id_alloc_mode == 1) {
            
            for (tunnel_id = 0; tunnel_id < ARAD_PORTS_VIRTUAL_PORT_PON_MAX_TUNNEL_ID(unit); tunnel_id++) {
                SOCDNX_IF_ERR_EXIT(READ_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port, tunnel_id), &tbl_data_32b));
                soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, IN_PP_PORTf, value? pp_port : 0);
                
                soc_IHP_VIRTUAL_PORT_TABLEm_field32_set(unit, &tbl_data_32b, VIRTUAL_PORT_SRC_SYS_PORTf, value? port : 0);
                SOCDNX_IF_ERR_EXIT(WRITE_IHP_VIRTUAL_PORT_TABLEm(unit, IHP_BLOCK(unit, core), ARAD_PORTS_VIRTUAL_PORT_PON_ENCODED(unit, port, tunnel_id), &tbl_data_32b));
            }
        }
    }

    
    arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
    res = arad_ports_programs_info_get_unsafe(unit, port, &port_prog_info);
    SOCDNX_SAND_IF_ERR_RETURN(res);
    port_prog_info.ptc_flp_profile = value ? SOC_TMC_PORTS_FLP_PROFILE_PON : SOC_TMC_PORTS_FLP_PROFILE_NONE;
    if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable ||
        SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable ||
        SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        port_prog_info.ptc_tt_profile = value ? SOC_TMC_PORTS_TT_PROFILE_PON : SOC_TMC_PORTS_TT_PROFILE_NONE;
    }
    res = arad_ports_programs_info_set_unsafe(unit, port, &port_prog_info);
    SOCDNX_SAND_IF_ERR_RETURN(res);

    
    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_get(unit,core,pp_port,&port_info));   
    port_info.vlan_translation_profile = value ? SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT : 0;  
    port_info.flags = value ? SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL : 0;
    SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_port_info_set(unit,core,pp_port,&port_info));

    
    for (vlan = 0; vlan < 4096; vlan++) {
        SOCDNX_SAND_IF_ERR_RETURN(soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set(unit,core,pp_port,vlan,(value? TRUE:FALSE)));
    }
    
    
    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_ETH_WITH_LLID_TAG, &prge_prog_select));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs));
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {
        SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_get_unsafe(unit, core, curr_q_pair, &pct_tbl_data));
        pct_tbl_data.prog_editor_profile = value ? prge_prog_select : 0;
        SOCDNX_IF_ERR_EXIT(arad_egq_pct_tbl_set_unsafe(unit, core, curr_q_pair, &pct_tbl_data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
  arad_pon_port_enable_get(
      int                   unit,
      soc_port_t            port,
      int                   *value
   )
{
    uint32 flags;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

    

    *value = 0;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    if (flags & SOC_PORT_FLAGS_PON_INTERFACE) {
        *value = 1;
    }


exit:
    SOCDNX_FUNC_RETURN;
}



int arad_ports_pp_port_var_set(int unit, soc_port_t port, uint32 value)
{
    uint32 res;

    int core = 0;
    uint32 pp_port;
    uint32 entry[4];

    soc_mem_t
        packet_processing_port_mem = SOC_IS_QAX(unit) ? EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm : EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));

    res = soc_mem_read(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
    SOCDNX_SAND_IF_ERR_RETURN(res);
    soc_mem_field32_set(unit, packet_processing_port_mem, entry, PRGE_VARf, value);
    res = soc_mem_write(unit, packet_processing_port_mem, EPNI_BLOCK(unit, core), pp_port, entry);
    SOCDNX_SAND_IF_ERR_RETURN(res);

exit:
    SOCDNX_FUNC_RETURN; 

}

    
int
  arad_ports_tm_port_var_set(
     int                   unit,
     soc_port_t            port,
    int                   value
    ) 
{
    uint32
        prog_editor_value;
    SOCDNX_INIT_FUNC_DEFS;

    prog_editor_value = value;
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0)) {
        SOCDNX_IF_ERR_EXIT(arad_ports_pp_port_var_set(unit, port, prog_editor_value));
    } else {        
        SOCDNX_IF_ERR_EXIT(arad_ports_prog_editor_value_update(unit, port, prog_editor_value));
    }
exit:
    SOCDNX_FUNC_RETURN; 
}

int
  arad_ports_tm_port_var_get(
     int                   unit,
     soc_port_t            port,
     int                   *value
    ) 
{
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(arad_ports_prog_editor_value_read(unit, port, (uint32*)value));
exit:
    SOCDNX_FUNC_RETURN; 
}

int
arad_ports_mirrored_channel_and_context_map(int unit, int core, uint32 termination_context,
                                             uint32 reassembly_context, uint32 channel)
{
    ARAD_IRE_RCY_CTXT_MAP_TBL_DATA rcy_ctxt_map_data;
    int res;
    uint32 offset;

    SOCDNX_INIT_FUNC_DEFS;
 
    rcy_ctxt_map_data.port_termination_context = termination_context;
    rcy_ctxt_map_data.reassembly_context = reassembly_context;
    offset = channel;

    res = arad_ire_rcy_ctxt_map_tbl_set_unsafe(
            unit,
            offset,
            &(rcy_ctxt_map_data)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_ports_mirror_to_rcy_port_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  int                   core,
      SOC_SAND_IN  uint32                pp_port,
      SOC_SAND_OUT soc_port_t*          rcy_mirror_port
   )
{
    uint32      channel;
    uint32      base_queue_pair;
    uint32      is_mirrored = 0;
    uint32      entry_context[3]; 
    soc_port_t  forward_port;
    int         rv;

    SOCDNX_INIT_FUNC_DEFS;
        
    SOCDNX_IF_ERR_EXIT(
      soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &forward_port));

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(
      unit, forward_port, &base_queue_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(
      unit, EGQ_BLOCK(unit,core), base_queue_pair, entry_context));

    
    is_mirrored = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf);
    if (is_mirrored) {
      
      channel = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf);
      SOCDNX_IF_ERR_EXIT(get_recycling_port(unit, core, pp_port, channel, rcy_mirror_port));
    }
    else {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG(
        "Mirror not enabled for port %d"), forward_port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

#undef _ERR_MSG_MODULE_NAME
