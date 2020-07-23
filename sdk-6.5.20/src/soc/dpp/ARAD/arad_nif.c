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
#include <soc/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/ARAD/arad_api_flow_control.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/ARAD/arad_nif_recover.h>
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/mem.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/phy/phyctrl.h>
#include <soc/phyreg.h>

#include <soc/dpp/ARAD/arad_defs.h>





#define ARAD_NIF_COUNTER_TYPE_MAX                                (ARAD_NIF_NOF_COUNTERS-1)

#define ARAD_TX_LANE_MAP_DEFAULT                0x0123
#define ARAD_RX_LANE_MAP_DEFAULT                0x3210

#define SOC_ARAD_NOF_LINKS_IN_MAC   SOC_DCMN_NOF_LINKS_IN_MAC

#define ARAD_NIF_LOAD_METHOD_INTERNAL   (1)
#define ARAD_NIF_LOAD_METHOD_EXTERNAL   (2)

extern unsigned char  tsce_ucode[];
extern unsigned short tsce_ucode_len;

extern int (*_phy_wcmod_firmware_set_helper[SOC_MAX_NUM_DEVICES]) (int unit , int port, uint8 *arr,int arr_len);


typedef struct counter_ranges_s{
    uint32 first_index;
    uint32 range_size;
    soc_mem_t mem;
} counter_ranges_t;


uint32
  arad_nif_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
    uint64 
        reg_val;
    uint32
        fld_val, 
        res;
    soc_reg_above_64_val_t
        reg_val_above_64,
        fld_val_above_64;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_REGS_INIT);
  SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
  SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
    READ_NBI_STATISTICSr(unit, &fld_val));

  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_ENf, 0x1);
  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_RESET_STARTf, 0x1);

  
  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_PACKET_COUNT_ENf, 0x1);
  
  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_UCAST_COUNT_ENABLEf, 0x0);
  
  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_USE_GLOBAL_INDIRECTf, 0x1);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
    WRITE_NBI_STATISTICSr(unit, fld_val));
  soc_reg_field_set(unit, NBI_STATISTICSr, &fld_val, STAT_RESET_STARTf, 0x0); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
    WRITE_NBI_STATISTICSr(unit, fld_val));

  
  SHR_BITSET_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAGf));
  soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,NIF_NO_FRAGf,fld_val_above_64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit,REG_PORT_ANY, reg_val_above_64));
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_NBI_TX_0_ILKN_CONTROLr(unit, &reg_val));
  soc_reg64_field32_set(unit, NBI_TX_0_ILKN_CONTROLr, &reg_val, TX_0_LAST_LANEf, 11);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 65, exit, WRITE_NBI_TX_0_ILKN_CONTROLr(unit, reg_val));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_NBI_TX_1_ILKN_CONTROLr(unit, &reg_val));
  soc_reg64_field32_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val, TX_1_LAST_LANEf, 11);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 75, exit, WRITE_NBI_TX_1_ILKN_CONTROLr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_regs_init()",0,0);
}

STATIC uint32
  arad_nif_chan_arb_enum_to_field_val(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 required_chan_arb_val,
    SOC_SAND_OUT uint32                 *chan_arb_field_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *chan_arb_field_val  = required_chan_arb_val;
    
  if (required_chan_arb_val > ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (required_chan_arb_val == ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {
    *chan_arb_field_val = 0;
  }
  else if (required_chan_arb_val == ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
  {
    *chan_arb_field_val = 11; 
  }
  else
  {
    *chan_arb_field_val = required_chan_arb_val+1;
  }  

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_chan_arb_enum_to_field_val()",required_chan_arb_val,0);
}

uint32
  arad_nif_chan_arb_field_val_to_enum(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 chan_arb_field_val,
    SOC_SAND_OUT uint32                 *required_chan_arb_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *required_chan_arb_val = chan_arb_field_val;

  if (chan_arb_field_val > 11)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (chan_arb_field_val == 0)
  {
    *required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN;
  }
  else if (chan_arb_field_val == 11)
  {
    *required_chan_arb_val = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB;
  }
  else
  {
    *required_chan_arb_val = chan_arb_field_val-1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_chan_arb_field_val_to_enum()",chan_arb_field_val,0);
}


uint32
  arad_nif_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    ifc,
    chan_arb_val,
    cpu_exist = 0,
    egr_interface_id,
    arbc,
    field_32[2];
  uint64
    data64,
    field64;
  soc_reg_above_64_val_t
    data,
    field;
  soc_port_t  first_phy_port;
  soc_pbmp_t      ports_map;
  soc_port_if_t interface_type;
  ARAD_NIF_TYPE nif_type;
  int if_id_internal;
  uint32 phy_port, base_q_pair;
  ARAD_INTERFACE_ID
    if_id = ARAD_IF_ID_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_INIT);

  
  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field);

  
  chan_arb_val = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB;
  for (ifc = 0; ifc < ARAD_EGQ_NOF_IFCS; ifc++)
  {   
    SHR_BITCOPY_RANGE(field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*ifc,&chan_arb_val,0,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
  }

  soc_reg_above_64_field_set(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data));

  
  egr_interface_id = ARAD_EGQ_IFC_OAMP;
  field_32[0] = field_32[1] = 0;
  for (arbc = 0; arbc < ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN; arbc++)
  {
      SHR_BITCOPY_RANGE(field_32,ARAD_OFP_RATES_IFC_NOF_BITS*arbc,&egr_interface_id,0,ARAD_OFP_RATES_IFC_NOF_BITS);
  }

  
  COMPILER_64_SET(field64,field_32[1],field_32[0]);
  COMPILER_64_ZERO(data64);
  soc_reg64_field_set(unit,EGQ_SHAPER_MAP_CH_ARB_TO_IFCr,&data64,SHAPER_MAP_CH_ARB_TO_IFCf,field64);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, WRITE_EGQ_SHAPER_MAP_CH_ARB_TO_IFCr(unit, data64));

  SOC_PBMP_COUNT(SOC_INFO(unit).cmic_bitmap, cpu_exist);
  if(cpu_exist > 0)
  {
    res = arad_nif_channelize_arbiter_set_unsafe(unit, SOC_INFO(unit).cmic_port, ARAD_IF_ID_CPU, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  } 
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_map));

    SOC_PBMP_ITER(ports_map, first_phy_port) {break;}

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, soc_port_sw_db_interface_type_get(unit, first_phy_port, &interface_type));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, soc_port_sw_db_first_phy_port_get(unit, first_phy_port, &phy_port));

    if_id_internal = phy_port-1;       
    arad_port_to_nif_type(unit, first_phy_port, &nif_type);
    if_id = arad_nif_intern2nif_id(unit, nif_type, if_id_internal);

    
    
    base_q_pair = 0;
    res = arad_nif_channelize_arbiter_set_unsafe(unit, first_phy_port, if_id, base_q_pair);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

  
  if (!SOC_IS_ARDON(unit)) {
    res = arad_nif_regs_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_init()",0,0);
}


uint32 arad_nif_synce_clk_sel_port_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port)
{
    uint32
        res = SOC_SAND_OK,
        is_valid;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_MAX(synce_cfg_num, ARAD_NIF_NOF_SYNCE_CLK_IDS-1, SOC_SAND_GEN_ERR, 10, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_verify()", synce_cfg_num, port);
}

uint32 arad_nif_synce_clk_cfg_num_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_ERR_IF_ABOVE_MAX(synce_cfg_num, ARAD_NIF_NOF_SYNCE_CLK_IDS-1, SOC_SAND_GEN_ERR, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_cfg_num_verify()", synce_cfg_num, 0);
}

uint32 arad_nif_synce_clk_div_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_IN  ARAD_NIF_SYNCE_CLK_DIV   clk_div)
{
    uint32
        res = SOC_SAND_OK,
        reg_val, fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    
    if (clk_div == ARAD_NIF_SYNCE_CLK_DIV_1) {
        fld_val = 0x1;
    } else if (clk_div == ARAD_NIF_SYNCE_CLK_DIV_2) {
        fld_val = 0x2;
    } else if (clk_div == ARAD_NIF_SYNCE_CLK_DIV_4) {
        fld_val = 0x3;
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit, ARAD_REG_ACCESS_ERR);
    }
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, fld_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_div_set()", synce_cfg_num, clk_div);
}

uint32 arad_nif_synce_clk_div_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   synce_cfg_num,
    SOC_SAND_OUT  ARAD_NIF_SYNCE_CLK_DIV*   clk_div)
{
    uint32
        res = SOC_SAND_OK,
        reg_val, fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));
    fld_val = soc_reg_field_get(unit, NBI_SYNC_ETH_CFGr, reg_val, SYNC_ETH_CLOCK_DIV_Nf);

    
    if (fld_val == 0x1) {
        *clk_div = ARAD_NIF_SYNCE_CLK_DIV_1;
    } else if (fld_val == 0x2) {
        *clk_div = ARAD_NIF_SYNCE_CLK_DIV_2;
    } else if (fld_val == 0x3) {
        *clk_div = ARAD_NIF_SYNCE_CLK_DIV_4;
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit, ARAD_REG_ACCESS_ERR);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_div_set()", synce_cfg_num, clk_div);
}

uint32 arad_nif_synce_clk_sel_port_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  soc_port_t                  port)
{
    uint32
        res = SOC_SAND_OK,
        reg_val,
        phy_port,
        num_lanes;
    soc_error_t
        rv;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    
    rv = soc_port_sw_db_first_phy_port_get(unit,  port, &phy_port);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 20, exit);
    }
    
    phy_port -= 0x1; 
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_SEL_Nf,  phy_port);

    
    rv = soc_port_sw_db_num_lanes_get(unit,  port, &num_lanes);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 40, exit);
    }
    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_NUM_OF_LANES_Nf, num_lanes);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_set_unsafe()", synce_cfg_num, port);
}

uint32 arad_nif_synce_clk_sel_port_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT soc_port_t                  *port)
{
    uint32
        res = SOC_SAND_OK,
        reg_val,
        phy_port = 0x0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    phy_port = soc_reg_field_get(unit, NBI_SYNC_ETH_CFGr, reg_val, SYNC_ETH_CLOCK_SEL_Nf) + 1;
    *port = SOC_INFO(unit).port_p2l_mapping[phy_port];

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_set_unsafe()", synce_cfg_num, phy_port);
}

uint32 arad_nif_synce_clk_squelch_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_IN  int                      enable)
{
    uint32
        res = SOC_SAND_OK,
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, enable ? 1 : 0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_set_unsafe()", synce_cfg_num, 0);
}

uint32 arad_nif_synce_clk_squelch_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      synce_cfg_num,
    SOC_SAND_OUT int                      *enable)
{
    uint32
    res = SOC_SAND_OK,
    reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    *enable = soc_reg_field_get(unit, NBI_SYNC_ETH_CFGr, reg_val, SYNC_ETH_SQUELCH_EN_Nf);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_clk_sel_port_set_unsafe()", synce_cfg_num, 0);
}


uint32 arad_nif_synce_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_SYNCE        *info)
{
    uint32
        res,
        i,
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    reg_val = 0x0;
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_OE_Nf, 0x0 );

    if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x0 );  
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x1 );
    } else if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x4 );  
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x5 );
    }  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, reg_val));

    
    for (i=0; i < ARAD_NIF_NOF_SYNCE_CLK_IDS; i++) {

        if (info->conf[i].enable == 0x1) {

            res = arad_nif_synce_clk_sel_port_set(unit, i, info->conf[i].port_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

            res = arad_nif_synce_clk_div_set(unit, i, info->conf[i].clk_divider);
            SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_NBI_SYNC_ETH_CFGr(unit, i, &reg_val));

            soc_reg_field_set(unit, NBI_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_SQUELCH_EN_Nf,  info->conf[i].squelch_enable);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_NBI_SYNC_ETH_CFGr(unit, i, reg_val));
        }
    }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_synce_init()", info->enable, info->mode);
}


uint32
  arad_nif_channelize_arbiter_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t         port_ndx,
    SOC_SAND_IN  ARAD_INTERFACE_ID  nif_ndx,
    SOC_SAND_IN  uint32          base_q_pair
  )
{
  uint32
    res = SOC_SAND_OK,
    prev_ifc,
    egr_interface_id,
    chan_arb_val = 0,
    tmp_arb,
    new_place,
    sch_offset,
    to_map_val,
    data_32,
    chan_arb_field_val = 0,
    required_chan_arb_val,
    rate,
    field_32[2] = {0},
    max_burst;
  soc_reg_above_64_val_t
    data,
    tbl_data,
    field;
  uint64
    data64,
    field64;
  uint8
    found,
    indx_in_use,
    is_channalized;
  SOC_SAND_OCC_BM_PTR
    chanif2chan_arb_occ;
  SOC_SAND_OCC_BM_PTR
    nonchanif2sch_offset_occ;
  soc_port_t
    local_port;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID
      first_free_chn_arb;
  uint32
      is_tdm_port;
  int core=0; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field);
  SOC_REG_ABOVE_64_CLEAR(tbl_data);

  COMPILER_64_ZERO(data64);
  COMPILER_64_ZERO(field64);

  
  is_tdm_port = IS_TDM_PORT(unit,port_ndx);
  ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chanif2chan_arb_occ.get(unit, &chanif2chan_arb_occ);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.nonchanif2sch_offset_occ.get(unit, &nonchanif2sch_offset_occ);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_port2egress_offset(unit, core, port_ndx, &egr_interface_id);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data));
  soc_reg_above_64_field_get(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
  SHR_BITCOPY_RANGE(&chan_arb_field_val,0,field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
  res = arad_nif_chan_arb_field_val_to_enum(unit,chan_arb_field_val,&chan_arb_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1200, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1005, exit, READ_EGQ_SHAPER_MAP_CH_ARB_TO_IFCr(unit, &data64));
  field64 = soc_reg64_field_get(unit,EGQ_SHAPER_MAP_CH_ARB_TO_IFCr,data64,SHAPER_MAP_CH_ARB_TO_IFCf);

  if (chan_arb_val != ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
  { 
    ARAD_DO_NOTHING_AND_EXIT;
  }

  
  switch (nif_ndx)
  {
  case ARAD_NIF_ID_ILKN_0:
    required_chan_arb_val = ((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (is_tdm_port)) ? 
                                    ARAD_OFP_RATES_EGQ_CHAN_ARB_02 : ARAD_OFP_RATES_EGQ_CHAN_ARB_00;
    break;
  case ARAD_NIF_ID_ILKN_1:
    required_chan_arb_val = ((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (is_tdm_port)) ?
                                    ARAD_OFP_RATES_EGQ_CHAN_ARB_03 : ARAD_OFP_RATES_EGQ_CHAN_ARB_01;
    break;
  case ARAD_IF_ID_CPU:
    required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_CPU;
    break;
  case ARAD_IF_ID_RCY:
    required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_RCY;
    break;
  case ARAD_IF_ID_ERP:
  case ARAD_IF_ID_OLP:
  case ARAD_IF_ID_OAMP:
    
    required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN;
    break;
  default:
    
    required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID;
    break;
  } 

  
  if (required_chan_arb_val == ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID)
  {
    
    res = arad_nif_is_channelized(unit, port_ndx, &is_channalized);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (!is_channalized) {
      required_chan_arb_val = ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN;
    }
  }

  
  if (required_chan_arb_val != ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {  
    
    if (required_chan_arb_val == ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID)
    {
      
      first_free_chn_arb = (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) ? ARAD_OFP_RATES_EGQ_CHAN_ARB_04 : ARAD_OFP_RATES_EGQ_CHAN_ARB_02;
      res = soc_sand_occ_bm_get_next_in_range(
            unit,
            chanif2chan_arb_occ,
            first_free_chn_arb,
            ARAD_OFP_RATES_EGQ_CHAN_ARB_07,
            TRUE,
            &required_chan_arb_val,
            &found
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (!found)
      {
        
        res = soc_sand_occ_bm_get_next(
          unit,
          chanif2chan_arb_occ,
          &required_chan_arb_val,
          &found
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }

      if (!found)
      {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 50, exit);
      }
    }
    
    res = soc_sand_occ_bm_is_occupied(
            unit,
            chanif2chan_arb_occ,
            required_chan_arb_val,
            &indx_in_use
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    if (indx_in_use)
    {
      
      res = soc_sand_occ_bm_alloc_next(
              unit,
              chanif2chan_arb_occ,
              &new_place,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      if (!found)
      {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 75, exit);
      }

      
      for (prev_ifc = 0; prev_ifc < ARAD_EGQ_NOF_IFCS; prev_ifc++)
      {
        res = soc_sand_bitstream_get_field(field,prev_ifc*ARAD_OFP_RATES_CHAN_ARB_NOF_BITS,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS,&tmp_arb);
        SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
        if (tmp_arb == required_chan_arb_val)
        {
          
          break;
        }
      }

      if (prev_ifc == ARAD_EGQ_NOF_IFCS)
      {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 85, exit);
      }

      
      res = soc_sand_occ_bm_occup_status_set(
            unit,
            chanif2chan_arb_occ,
            new_place,
            TRUE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
      
      SHR_BITCOPY_RANGE(field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*prev_ifc,&new_place,0,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);

      res = soc_sand_occ_bm_occup_status_set(
            unit,
            chanif2chan_arb_occ,
            required_chan_arb_val,
            FALSE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
    } 
    

    
    res = soc_sand_occ_bm_occup_status_set(
            unit,
            chanif2chan_arb_occ,
            required_chan_arb_val,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
  } 
  else 
  {
    
    
    res = soc_sand_occ_bm_alloc_next(
            unit,
            nonchanif2sch_offset_occ,
            &new_place,
            &found
          );    
    SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);

    if (!found) {
      first_free_chn_arb = (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) ? ARAD_OFP_RATES_EGQ_CHAN_ARB_04 : ARAD_OFP_RATES_EGQ_CHAN_ARB_02;
      
      res = soc_sand_occ_bm_get_next_in_range(
              unit,
              chanif2chan_arb_occ,
              first_free_chn_arb,
              ARAD_OFP_RATES_EGQ_CHAN_ARB_07,
              TRUE,
              &required_chan_arb_val,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
    }

    if (!found)
    {
      
      res = soc_sand_occ_bm_get_next(
        unit,
        chanif2chan_arb_occ,
        &required_chan_arb_val,
        &found
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
    }

    if (!found) 
    {
      
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 470, exit);   
    } 

    if (required_chan_arb_val != ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
    {
      
      res = soc_sand_occ_bm_occup_status_set(
            unit,
            chanif2chan_arb_occ,
            required_chan_arb_val,
            TRUE
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }    
  }
    
  
  
  chan_arb_field_val = 0;
  res = arad_nif_chan_arb_enum_to_field_val(unit, required_chan_arb_val, &chan_arb_field_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SHR_BITCOPY_RANGE(field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,&chan_arb_field_val,0,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
  
  soc_reg_above_64_field_set(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data));


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 111, exit, soc_port_sw_db_tm_to_local_port_get(unit, core, port_ndx, &local_port));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 112, exit, soc_port_sw_db_high_priority_cal_set(unit, local_port, required_chan_arb_val));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, soc_port_sw_db_low_priority_cal_set(unit, local_port, required_chan_arb_val));  

  
  if (required_chan_arb_val != ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {
    field_32[1] = COMPILER_64_HI(field64);
    field_32[0] = COMPILER_64_LO(field64);
    
    SHR_BITCOPY_RANGE(field_32,ARAD_OFP_RATES_IFC_NOF_BITS*required_chan_arb_val,&egr_interface_id,0,ARAD_OFP_RATES_IFC_NOF_BITS);
    
    COMPILER_64_SET(field64,field_32[1],field_32[0]);    
    soc_reg64_field_set(unit,EGQ_SHAPER_MAP_CH_ARB_TO_IFCr,&data64,SHAPER_MAP_CH_ARB_TO_IFCf,field64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, WRITE_EGQ_SHAPER_MAP_CH_ARB_TO_IFCr(unit, data64));

    
    rate = SOC_TMC_RATE_1K * SOC_TMC_IF_MAX_RATE_MBPS_ARAD(unit);

    max_burst = ARAD_OFP_RATES_CHNIF_BURST_LIMIT_MAX;

    res = arad_ofp_rates_egq_chnif_shaper_set_unsafe(
            unit,
            core,
            port_ndx,
            rate,
            max_burst
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  

  
  
  SOC_REG_ABOVE_64_CLEAR(data);  
  data_32 = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, READ_SCH_FC_MAP_FCMm(unit,MEM_BLOCK_ANY,egr_interface_id,&data_32));
  if (required_chan_arb_val != ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {
    
    sch_offset = required_chan_arb_val;
  }
  else
  {
    
    
    if (!found)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR, 20, exit)
    }
    sch_offset = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB - 1 + new_place;   

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,MEM_BLOCK_ANY,new_place,tbl_data));
    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_IDf,base_q_pair);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1160, exit, WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,MEM_BLOCK_ANY,new_place,tbl_data));    
  }
  soc_SCH_FC_MAP_FCMm_field32_set(unit,&data_32,FC_MAP_FCMf,sch_offset);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, WRITE_SCH_FC_MAP_FCMm(unit,MEM_BLOCK_ANY,egr_interface_id,&data_32));

  
  to_map_val = 1;
  data_32 = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1730,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DVS_CONFIG_0r, REG_PORT_ANY, 0, NIF_FORCE_PAUSEf, &data_32));
  SOC_SAND_SET_BIT(data_32, (!to_map_val), sch_offset);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1735,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_0r, REG_PORT_ANY, 0, NIF_FORCE_PAUSEf,  data_32));
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_channelize_arbiter_set_unsafe()",nif_ndx,0);
}



uint32
  arad_nif_channelize_arbiter_delete_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  soc_port_t      port_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    egr_interface_id,
    sch_offset,
    to_map_val,
    chan_arb_val = 0,
    chan_arb_field_val = 0,
    def_chan_arb_val = 0,
    new_place = 0,
    data_32; 
  soc_reg_above_64_val_t
    data,
    field;
  SOC_SAND_OCC_BM_PTR
    chanif2chan_arb_occ;
  SOC_SAND_OCC_BM_PTR
    nonchanif2sch_offset_occ;
  soc_port_t local_port;

  int core=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field);

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chanif2chan_arb_occ.get(unit, &chanif2chan_arb_occ);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.nonchanif2sch_offset_occ.get(unit, &nonchanif2sch_offset_occ);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_port2egress_offset(unit, core, port_ndx, &egr_interface_id);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data));
  soc_reg_above_64_field_get(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
  SHR_BITCOPY_RANGE(&chan_arb_field_val,0,field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
  res = arad_nif_chan_arb_field_val_to_enum(unit,chan_arb_field_val,&chan_arb_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1200, exit);

  if (chan_arb_val == ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB)
  { 
    ARAD_DO_NOTHING_AND_EXIT;
  }

  
  if (chan_arb_val != ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {
    res = soc_sand_occ_bm_occup_status_set(
           unit,
           chanif2chan_arb_occ,
           chan_arb_val,
           FALSE
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  
  def_chan_arb_val = ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB;
  chan_arb_field_val = 0;
  res = arad_nif_chan_arb_enum_to_field_val(unit, def_chan_arb_val, &chan_arb_field_val);
  SHR_BITCOPY_RANGE(field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,&chan_arb_field_val,0,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  soc_reg_above_64_field_set(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data));

  
  SOC_REG_ABOVE_64_CLEAR(data);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1120, exit, READ_SCH_FC_MAP_FCMm(unit,MEM_BLOCK_ANY,egr_interface_id,&data_32));
  sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data_32,FC_MAP_FCMf);

  
  if (chan_arb_val == ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  { 
    new_place = sch_offset - (ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB - 1);
    res = soc_sand_occ_bm_occup_status_set(
           unit,
           nonchanif2sch_offset_occ,
           new_place,
           FALSE
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    SOC_REG_ABOVE_64_CLEAR(data);

    
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,MEM_BLOCK_ANY,new_place,(data)));
    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,data,PORT_IDf,0);
    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,data,PORT_NIF_MAX_CR_RATEf,0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1160, exit, WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,MEM_BLOCK_ANY,new_place,(data)));    
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 111, exit, soc_port_sw_db_tm_to_local_port_get(unit, core, port_ndx, &local_port));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 112, exit, soc_port_sw_db_high_priority_cal_set(unit, local_port, INVALID_CALENDAR));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, soc_port_sw_db_low_priority_cal_set(unit, local_port, INVALID_CALENDAR));         
  }

    
  to_map_val = 0;
  data_32 = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1730,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_DVS_CONFIG_0r, REG_PORT_ANY, 0, NIF_FORCE_PAUSEf, &data_32));
  SOC_SAND_SET_BIT(data_32, (!to_map_val), sch_offset);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1735,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_DVS_CONFIG_0r, REG_PORT_ANY, 0, NIF_FORCE_PAUSEf,  data_32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_nif_channelize_arbiter_delete()",port_ndx,0);
}

uint32
  arad_nif_id_verify(
    SOC_SAND_IN ARAD_INTERFACE_ID nif_ndx
  )
{
  uint8
    is_valid_id;


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_NIF_ID_VERIFY);

  is_valid_id = ARAD_NIF_IS_VALID_ID(nif_ndx);
  if (!(is_valid_id))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_ID_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_id_verify()", nif_ndx, 0);
}


STATIC uint32
  arad_nif_statistic_read_select_port_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE      cntr,
    SOC_SAND_OUT uint32                     *nif_id_intern
  )
{
  soc_error_t rv;
  uint32      channel,
              first_phy_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  

  rv = soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port);
  if(SOC_FAILURE(rv)) {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 20, exit);
  }

  switch(cntr) {
    case ARAD_NIF_RX_ILKN_PER_CHANNEL:
    case ARAD_NIF_TX_ILKN_PER_CHANNEL:
      rv = soc_port_sw_db_channel_get(unit, port, &channel);
      if(SOC_FAILURE(rv)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 10, exit);
      }
      if (first_phy_port-1 == 0) {
        
        *nif_id_intern = channel / ARAD_NIF_NOF_STAT_BINS;
      } else {
        
        *nif_id_intern = 16 + channel / ARAD_NIF_NOF_STAT_BINS;
      }
      break;
    default:
      *nif_id_intern = first_phy_port-1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_statistic_read_select_port_get()", port, cntr);
}

STATIC void
  arad_nif_counter2intern(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE  cntr,
    SOC_SAND_OUT uint32             *cntr_intern,
    SOC_SAND_OUT uint8            *is_nbi_not_mal
  )
{
  uint32
    cntr_val,
    channel;
  uint8
    is_nbi = TRUE;
  soc_error_t
    rv;  
  

  switch(cntr) {
  case ARAD_NIF_RX_OK_OCTETS:
    cntr_val = 0;
  break;
  case ARAD_NIF_RX_BCAST_PACKETS:
    cntr_val = 16;
  break;
  case ARAD_NIF_RX_MCAST_BURSTS:
    cntr_val = 17;
  break;
  case ARAD_NIF_RX_OK_PACKETS:
    cntr_val = 32;
  break;
  case ARAD_NIF_RX_ERR_PACKETS:
    cntr_val = 33;
  break;

  case ARAD_NIF_RX_LEN_BELOW_MIN:
    cntr_val = 48;
  break;
  case ARAD_NIF_RX_LEN_MIN_59:
    cntr_val = 49;
  break;
  case ARAD_NIF_RX_LEN_60:
    cntr_val = 50;
  break;
  case ARAD_NIF_RX_LEN_61_123:
    cntr_val = 51;
  break;
  case ARAD_NIF_RX_LEN_124_251:
    cntr_val = 52;
  break;
  case ARAD_NIF_RX_LEN_252_507:
    cntr_val = 53;
  break;
  case ARAD_NIF_RX_LEN_508_1019:
    cntr_val = 54;
  break;
  case ARAD_NIF_RX_LEN_1020_1514:
    cntr_val = 55;
  break;
  case ARAD_NIF_RX_LEN_1515_1518:
    cntr_val = 56;
  break;
  case ARAD_NIF_RX_LEN_1519_2043:
    cntr_val = 57;
  break;
  case ARAD_NIF_RX_LEN_2044_4091:
    cntr_val = 58;
  break;
  case ARAD_NIF_RX_LEN_4092_9212:
    cntr_val = 59;
  break;
  case ARAD_NIF_RX_LEN_9213CFG_MAX:
    cntr_val = 60;
  break;
  case ARAD_NIF_RX_LEN_ABOVE_MAX:
    cntr_val = 61;
  break;

  case ARAD_NIF_TX_LEN_BELOW_MIN:
    cntr_val = 112;
  break;
  case ARAD_NIF_TX_LEN_MIN_59:
    cntr_val = 113;
  break;
  case ARAD_NIF_TX_LEN_60:
    cntr_val = 114;
  break;
  case ARAD_NIF_TX_LEN_61_123:
    cntr_val = 115;
  break;
  case ARAD_NIF_TX_LEN_124_251:
    cntr_val = 116;
  break;
  case ARAD_NIF_TX_LEN_252_507:
    cntr_val = 117;
  break;
  case ARAD_NIF_TX_LEN_508_1019:
    cntr_val = 118;
  break;
  case ARAD_NIF_TX_LEN_1020_1514:
    cntr_val = 119;
  break;
  case ARAD_NIF_TX_LEN_1515_1518:
    cntr_val = 120;
  break;
  case ARAD_NIF_TX_LEN_1519_2043:
    cntr_val = 121;
  break;
  case ARAD_NIF_TX_LEN_2044_4091:
    cntr_val = 122;
  break;
  case ARAD_NIF_TX_LEN_4092_9212:
    cntr_val = 123;
  break;
  case ARAD_NIF_TX_LEN_9213CFG_MAX:
    cntr_val = 124;
  break;
  case ARAD_NIF_TX_OK_OCTETS:
    cntr_val = 64;
  break;
  case ARAD_NIF_TX_BCAST_PACKETS:
    cntr_val = 80;
  break;
  case ARAD_NIF_TX_MCAST_BURSTS:
    cntr_val = 81;
  break;
  case ARAD_NIF_TX_OK_PACKETS:
    cntr_val = 96;
  break;
  case ARAD_NIF_TX_ERR_PACKETS:
    cntr_val = 97;
  break;
  case ARAD_NIF_RX_ILKN_PER_CHANNEL:
    rv = soc_port_sw_db_channel_get(unit, port, &channel);
    if (SOC_FAILURE(rv)) {
      cntr_val = ARAD_NIF_INVALID_VAL_INTERN;
      is_nbi = FALSE; 
    } else {
      cntr_val = 48 + (channel % ARAD_NIF_NOF_STAT_BINS);
    }
    break;
  case ARAD_NIF_TX_ILKN_PER_CHANNEL:
    rv = soc_port_sw_db_channel_get(unit, port, &channel);
    if (SOC_FAILURE(rv)) {
      cntr_val = ARAD_NIF_INVALID_VAL_INTERN;
      is_nbi = FALSE; 
    } else {
      cntr_val = 112 + (channel % ARAD_NIF_NOF_STAT_BINS);
    }
    break;
  default:
    cntr_val = ARAD_NIF_INVALID_VAL_INTERN;
    is_nbi = FALSE;
  }
  
  *cntr_intern = cntr_val;
  *is_nbi_not_mal = is_nbi;
}



STATIC uint32
arad_nif_counter_id_to_table_offset(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 nif_internal_id,
    SOC_SAND_IN uint32  counter_id,
    SOC_SAND_OUT soc_mem_t *mem,
    SOC_SAND_OUT uint32 *entry_index
    )
{
    counter_ranges_t ranges[] = {
        {0 , 1, NBI_RLENG_MEMm},
        {16, 2, NBI_RTYPE_MEMm},
        {32, 2, NBI_RPKTS_MEMm},
        {48, 14, NBI_RBINS_MEMm},
        {64, 1, NBI_TLENG_MEMm},
        {80, 2, NBI_TTYPE_MEMm},
        {96, 2, NBI_TPKTS_MEMm},
        {112, 14, NBI_TBINS_MEMm}
    };
    int i;
    uint8 is_match_found = FALSE;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (counter_id == ARAD_NIF_INVALID_VAL_INTERN) {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 5, exit);
    }
    for ( i = 0 ; i < sizeof(ranges)/sizeof(ranges[0]); i++){
        if((counter_id >=  ranges[i].first_index) && (counter_id < ranges[i].first_index + ranges[i].range_size)){
            soc_mem_info_t       *mip = NULL;

            *mem = ranges[i].mem;
            mip = &SOC_MEM_INFO(unit, ranges[i].mem);
            *entry_index = counter_id - ranges[i].first_index;
            if(!(mip->flags & SOC_MEM_FLAG_IS_ARRAY)){
                *entry_index +=  nif_internal_id*ranges[i].range_size;
            }
            is_match_found = TRUE;
            break;
        }
    }
    if(!is_match_found){
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 10, exit);
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_counter_id_to_table_offset()", counter_id, 0);  
}



uint32
  arad_nif_counter_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE      counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT             *counter_val
  )
{
  uint32 res, cntr_id, nif_internal_id = 0;
  uint8 is_nbi_not_mal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_val);

  soc_sand_64cnt_clear(counter_val);
  

  res = arad_nif_statistic_read_select_port_get(
     unit,
     port,
     counter_type,
     &nif_internal_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  arad_nif_counter2intern(
    unit,
    port,
    counter_type,
    &cntr_id,
    &is_nbi_not_mal
  );


  if (is_nbi_not_mal)
  {
    uint32 entry_id = 0;
    soc_mem_t mem = INVALIDm;
    soc_mem_info_t *mip;
    uint64 fld_val64, counter_data;

    res = arad_nif_counter_id_to_table_offset(unit, nif_internal_id, cntr_id, &mem, &entry_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20 , exit);
    mip = &SOC_MEM_INFO(unit, mem);
    if(mip->flags & SOC_MEM_FLAG_IS_ARRAY){
      res = soc_mem_array_read(unit, mem, nif_internal_id, MEM_BLOCK_ALL, entry_id, &counter_data);
    }
    else{
      res = soc_mem_read(unit, mem, MEM_BLOCK_ALL, entry_id, &counter_data);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 30 , exit);
    soc_mem_field64_get(unit, mem , &counter_data, DATAf, &fld_val64);

    counter_val->u64.arr[0] = COMPILER_64_LO(fld_val64);
    counter_val->u64.arr[1] = COMPILER_64_HI(fld_val64);

  }
  else
  {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 312, exit);
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_counter_get_unsafe()", port, counter_type);
}


uint32
  arad_nif_counter_ndx_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  soc_port_t              port,
    SOC_SAND_IN  ARAD_NIF_COUNTER_TYPE   counter_type
  )
{
  uint32
    is_valid;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_COUNTER_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(counter_type, ARAD_NIF_COUNTER_TYPE_MAX, ARAD_NIF_COUNTER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  
  rv = soc_port_sw_db_is_valid_port_get(unit, port, &is_valid);
  if(SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_ID_ERR, 10, exit);
  }

  if(!is_valid) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_ID_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_counter_ndx_verify()", port, 0);
}


uint32
  arad_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  uint8                  non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT         counter_val[ARAD_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;
  int32
    counter_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_ALL_COUNTERS_GET_UNSAFE);

  for (counter_type = 0; counter_type < ARAD_NIF_NOF_COUNTERS; ++counter_type)
  {
    soc_sand_64cnt_clear(&(counter_val[counter_type]));
    res = arad_nif_counter_get_unsafe(
            unit,
            port,
            counter_type,
            &(counter_val[counter_type])
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_all_counters_get_unsafe()", port, 0);
}

uint32
  arad_nif_all_counters_ndx_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t         port
  )
{
  uint32
    is_valid;
  soc_error_t
    rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_ALL_COUNTERS_GET_VERIFY);

  rv = soc_port_sw_db_is_valid_port_get(unit, port, &is_valid);
  if(SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_ID_ERR, 10, exit);
  }

  if(!is_valid) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_INVALID_ID_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_all_counters_ndx_verify()", 0, 0);
}


uint32
  arad_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                 counters_val[ARAD_NIF_NOF_NIFS][ARAD_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_error_t
    rv;
  uint32
    cntr_id,
    is_master,
    nif_id,
    first_phy_port;
  soc_pbmp_t 
    port_bm;
  soc_port_t
    port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_ALL_NIFS_ALL_COUNTERS_GET_UNSAFE);

  for (nif_id = 0; nif_id < ARAD_NIF_NOF_NIFS; nif_id++)
  {
    for (cntr_id = 0; cntr_id < ARAD_NIF_NOF_COUNTERS; cntr_id++)
    {
      soc_sand_64cnt_clear(&(counters_val[nif_id][cntr_id]));
    }
  }

  rv = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm);
  if(SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 2, exit);
  }

  SOC_PBMP_ITER(port_bm, port) {
      rv = soc_port_sw_db_is_master_get(unit, port, &is_master);
      if(SOC_FAILURE(rv)) {
          SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 4, exit);
      }
      if(!is_master) {
          continue;
      }
      rv = soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port);
      if(SOC_FAILURE(rv)) {
          SOC_SAND_SET_ERROR_CODE(ARAD_NIF_CNT_RD_INVALID_ERR, 6, exit);
      }
      nif_id = first_phy_port-1;

      res = arad_nif_all_counters_get_unsafe(
              unit,
              port,
              non_data_also,
              counters_val[nif_id]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_all_nifs_all_counters_get_unsafe()", 0, 0);
}

uint32
  arad_nif_link_status_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_LINK_STATUS_GET_VERIFY);

  res = arad_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_link_status_ndx_verify()", 0, 0);
}


uint32
  arad_nif_is_channelized(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT uint8* is_channelized
  )
{
  uint32 
    nof_channels;
  soc_error_t
      rv;
  int
      is_local_port_channelized;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_IS_CHANNELIZED);

  *is_channelized = FALSE;

  rv = soc_port_sw_db_is_channelized_port_get(unit, port, &is_local_port_channelized);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);

  if(is_local_port_channelized) {
      *is_channelized = TRUE;
  } else {
      rv = soc_port_sw_db_num_of_channels_get(
              unit, 
              port, 
              &nof_channels);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);
      if(nof_channels>1) {
          *is_channelized = TRUE;
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_is_channelized()", 0, 0);
}




int
  arad_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  )
{
  uint32
      erp_base_q_pair;
  ARAD_EGQ_PPCT_TBL_DATA
      egq_ppct_tbl_data;
  soc_port_if_t          
      interface_type;
  uint32 
      phy_port;
  soc_port_t
      local_port;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
  uint32
      is_tdm_port;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(egr_if);

  is_tdm_port = IS_TDM_PORT(unit,tm_port);
  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port));
  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, local_port, &interface_type));

  ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

  switch (interface_type)
  {
      case SOC_PORT_IF_XFI:
      case SOC_PORT_IF_RXAUI:
      case SOC_PORT_IF_DNX_XAUI:
      case SOC_PORT_IF_SGMII:
      case SOC_PORT_IF_XLAUI:
      case SOC_PORT_IF_CAUI:
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, local_port, &phy_port));
          (*egr_if) = phy_port-1;
          break;
      case SOC_PORT_IF_ILKN:
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, local_port, &phy_port));
          (*egr_if) = (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) ?  ((is_tdm_port) ? phy_port-1 : phy_port) : phy_port-1;
          break;
      case SOC_PORT_IF_CPU:
          (*egr_if) = ARAD_EGQ_IFC_CPU;
          break;
      case SOC_PORT_IF_RCY:
          (*egr_if) = ARAD_EGQ_IFC_RCY;
          break;
      case SOC_PORT_IF_OLP:
          (*egr_if) = ARAD_EGQ_IFC_OLP;
          break;
      case SOC_PORT_IF_OAMP:
          (*egr_if) = ARAD_EGQ_IFC_OAMP;
          break;
      case SOC_PORT_IF_ERP:
          erp_base_q_pair = ARAD_EGQ_ERP_BASE_Q_PAIR;
          SOCDNX_IF_ERR_EXIT(arad_egq_ppct_tbl_get_unsafe(unit,  0, erp_base_q_pair, &egq_ppct_tbl_data));
          (*egr_if) = egq_ppct_tbl_data.cgm_interface;
          break;
      default:
          (*egr_if) = ARAD_EGQ_IFC_DEF_VAL;
          break;
  }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  arad_nif_ipg_set_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN soc_port_t        port, 
    SOC_SAND_IN int               ipg
  )
{
    soc_port_if_t interface;
    uint64 val64;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_BLK_CLP == SOC_PORT_TYPE(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

        switch(interface) {
            case SOC_PORT_IF_XFI:
            case SOC_PORT_IF_SGMII:
            case SOC_PORT_IF_RXAUI:
            case SOC_PORT_IF_DNX_XAUI:
            case SOC_PORT_IF_XLAUI:
                COMPILER_64_SET(val64, 0, ipg);
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_modify(unit, CLP_XMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf,  val64));
                break;
            case SOC_PORT_IF_CAUI:
                COMPILER_64_SET(val64, 0, ipg);
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_modify(unit, CLP_CMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf,  val64));
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown NIF type")));
                break;
        }
    } else {
        COMPILER_64_SET(val64, 0, ipg);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_modify(unit, XLP_XMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf,  val64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_nif_ipg_get_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN soc_port_t        port, 
    SOC_SAND_OUT int              *ipg
  )
{
    uint32 fld_val = 0;
    soc_port_if_t interface;
    uint64 fld_val64;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_BLK_CLP == SOC_PORT_TYPE(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

        switch(interface) {
            case SOC_PORT_IF_XFI:
            case SOC_PORT_IF_SGMII:
            case SOC_PORT_IF_RXAUI:
            case SOC_PORT_IF_DNX_XAUI:
            case SOC_PORT_IF_XLAUI:
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_read(unit, CLP_XMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf, &fld_val64));
                fld_val = COMPILER_64_LO(fld_val64);
                break;
            case SOC_PORT_IF_CAUI:
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_read(unit, CLP_CMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf, &fld_val64));
                fld_val = COMPILER_64_LO(fld_val64);
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown NIF type")));
                break;
        }
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field64_read(unit, XLP_XMAC_TX_CTRLr,  port,  0, AVERAGE_IPGf, &fld_val64));
        fld_val = COMPILER_64_LO(fld_val64);
    }

    *ipg = fld_val;

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_port_cntmaxsize_set(
    int            unit,
    soc_port_t        port,
    int               val
  )
{
    uint32 reg_val32[1];
    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_SFI_PORT(unit, port) && !IS_IL_PORT(unit, port)
        && !SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        SOCDNX_IF_ERR_EXIT(READ_PORT_CNTMAXSIZEr(unit, port, reg_val32));
        soc_reg_field_set(unit, PORT_CNTMAXSIZEr, reg_val32, CNTMAXSIZEf, val);
        SOCDNX_IF_ERR_EXIT(WRITE_PORT_CNTMAXSIZEr(unit, port, *reg_val32));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Port isn't supported")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_port_cntmaxsize_get(
    int            unit,
    soc_port_t        port,
    int              *val
  )
{
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;

    if (!IS_SFI_PORT(unit, port) && !IS_IL_PORT(unit, port)
        && !SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        SOCDNX_IF_ERR_EXIT(READ_PORT_CNTMAXSIZEr(unit, port, &reg_val32));
        *val = soc_reg_field_get(unit, PORT_CNTMAXSIZEr, reg_val32, CNTMAXSIZEf);
    } else
{
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Port isn't supported")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_nif_type_verify(
    SOC_SAND_IN ARAD_NIF_TYPE nif_type
  )
{
  uint8
    is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_NIF_TYPE_VERIFY);

  switch(nif_type) {
  case ARAD_NIF_TYPE_XAUI:
  case ARAD_NIF_TYPE_RXAUI:
  case ARAD_NIF_TYPE_SGMII:
  case ARAD_NIF_TYPE_ILKN:
  case ARAD_NIF_TYPE_10GBASE_R:
  case ARAD_NIF_TYPE_100G_CGE:
  case ARAD_NIF_TYPE_40G_XLGE:
    is_valid = TRUE;
      break;
  default:
    is_valid = FALSE;
  }
  
  if (!(is_valid))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_type_verify()", nif_type, 0);
}

uint32
  arad_port_to_nif_type(
      SOC_SAND_IN int unit,
      SOC_SAND_IN soc_port_t port,
      SOC_SAND_OUT ARAD_NIF_TYPE* nif_type
  )
{
    uint32 is_valid;
    soc_error_t rv;
    soc_port_if_t interface;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_NIF_TYPE_VERIFY);

    rv = soc_port_sw_db_is_valid_port_get(unit, port, &is_valid);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 10, exit);
    }
    if(!is_valid) {
        *nif_type = ARAD_NIF_TYPE_NONE;
    } else {
        rv = soc_port_sw_db_interface_type_get(unit, port, &interface);
        if(SOC_FAILURE(unit)) {
            SOC_SAND_SET_ERROR_CODE(ARAD_UNKNOWN_NIF_TYPE_ERR, 12, exit);
        }
        switch(interface) {
            case SOC_PORT_IF_XFI:
                *nif_type = ARAD_NIF_TYPE_10GBASE_R;
                break;
            case SOC_PORT_IF_SGMII:
                *nif_type = ARAD_NIF_TYPE_SGMII;
                break;
            case SOC_PORT_IF_RXAUI:
                *nif_type = ARAD_NIF_TYPE_RXAUI;
                break;
            case SOC_PORT_IF_DNX_XAUI:
                *nif_type = ARAD_NIF_TYPE_XAUI;
                break;
            case SOC_PORT_IF_XLAUI: 
                *nif_type = ARAD_NIF_TYPE_40G_XLGE;
                break;
            case SOC_PORT_IF_CAUI:
                *nif_type = ARAD_NIF_TYPE_100G_CGE;
                break;
            case SOC_PORT_IF_ILKN:
                *nif_type = ARAD_NIF_TYPE_ILKN;
                break;
            case SOC_PORT_IF_TM_INTERNAL_PKT:
                *nif_type = ARAD_NIF_TYPE_TM_INTERNAL_PKT;
                break;
            case _SHR_PORT_IF_CPU:
                *nif_type = ARAD_NIF_TYPE_CPU;
                break;
            default:
                *nif_type = ARAD_NIF_ID_NONE;
                break;
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_nif_type_verify()", port, 0);
}



ARAD_INTERFACE_ID
  arad_nif_intern2nif_id(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         internal_id
  )
{
  ARAD_INTERFACE_ID
    nif_id,
    nif_offset;
  uint32 ilkn1_intern_id = 16;

  if (internal_id >= ARAD_NIF_NOF_NIFS && nif_type != ARAD_NIF_TYPE_CPU)
  {
    nif_id = ARAD_NIF_ID_NONE;
  }
  else
  {
    switch(nif_type) {
    case ARAD_NIF_TYPE_XAUI:
      nif_offset = internal_id / 4;
      nif_id = ARAD_NIF_ID(XAUI, nif_offset);
      break;
    case ARAD_NIF_TYPE_RXAUI:
      nif_offset = internal_id / 2;
      nif_id = ARAD_NIF_ID(RXAUI, nif_offset);
      break;
    case ARAD_NIF_TYPE_SGMII:
      nif_offset = internal_id / 1;
      nif_id = ARAD_NIF_ID(SGMII, nif_offset);
      break;
    case ARAD_NIF_TYPE_ILKN:

      if (SOC_IS_ARADPLUS(unit)) {
#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
          ilkn1_intern_id = (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)? 10 : 16;
#endif
      }
      else{
          ilkn1_intern_id = 16;
      }
      if(internal_id == 0){
          nif_offset = 0;
      }
      else if(internal_id == ilkn1_intern_id){
          nif_offset = 1;
      }
      else{
          nif_id = ARAD_NIF_ID_NONE;
          break;
      }
      nif_id = ARAD_NIF_ID(ILKN, nif_offset);
      break;
    case ARAD_NIF_TYPE_10GBASE_R:
      nif_offset = internal_id / 1;
      nif_id = ARAD_NIF_ID(10GBASE_R, nif_offset);
      break;
    case ARAD_NIF_TYPE_100G_CGE:
        nif_offset = internal_id / 16;
        nif_id = ARAD_NIF_ID(CGE, nif_offset);
        break;
    case ARAD_NIF_TYPE_40G_XLGE:
        nif_offset = internal_id / 4;
        nif_id = ARAD_NIF_ID(XLGE, nif_offset);
        break;
    case ARAD_NIF_TYPE_TM_INTERNAL_PKT:
       nif_offset = 0x0;
       nif_id = ARAD_NIF_ID(TM_INTERNAL_PKT, nif_offset);
       break;
    case ARAD_NIF_TYPE_CPU:
       nif_offset = 0x0;
       nif_id = ARAD_NIF_ID(CPU, nif_offset);
       break;
    default:
      nif_id = ARAD_NIF_ID_NONE;
    }
  }

  return nif_id;
}

ARAD_INTERFACE_ID
  arad_nif_ilkn_tdm_intern2nif_id(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  ARAD_NIF_TYPE  nif_type,
    SOC_SAND_IN  uint32         internal_id,
    SOC_SAND_IN  uint32         is_tdm
  )
{
  ARAD_INTERFACE_ID
    nif_id;

  nif_id = arad_nif_intern2nif_id(unit, nif_type, internal_id);

  if(nif_id == ARAD_NIF_ID_ILKN_0 && is_tdm){
    nif_id = ARAD_NIF_ID_ILKN_TDM_0;
  }
  else if(nif_id == ARAD_NIF_ID_ILKN_1 && is_tdm){
    nif_id = ARAD_NIF_ID_ILKN_TDM_1;
  }

  return nif_id;
}



uint32
  arad_nif2intern_id(
    SOC_SAND_IN  int  unit,
    ARAD_INTERFACE_ID  arad_nif_id
  )
{
  ARAD_INTERFACE_ID
    nif_intern_id;
  uint32 ilkn1_intern_id = 16;
  
  if (ARAD_NIF_IS_TYPE_ID(XAUI, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(XAUI, arad_nif_id) * 4;
  }
  else if (ARAD_NIF_IS_TYPE_ID(RXAUI, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(RXAUI, arad_nif_id) * 2;
  }
  else if (ARAD_NIF_IS_TYPE_ID(SGMII, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(SGMII, arad_nif_id) * 1;
  }
  else if (ARAD_NIF_IS_TYPE_ID(ILKN, arad_nif_id))
  {
      if (SOC_IS_ARADPLUS(unit)) {
#if defined(INCLUDE_KBP) && defined(BCM_88660_A0)
          ilkn1_intern_id = (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)? 10 : 16;
#endif
      }
      else{
          ilkn1_intern_id = 16;
      }
      nif_intern_id = ARAD_NIF_ID_OFFSET(ILKN, arad_nif_id) * ilkn1_intern_id;
  }
  else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(10GBASE_R, arad_nif_id) * 1;
  }
  else if (ARAD_NIF_IS_TYPE_ID(XLGE, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(XLGE, arad_nif_id) * 4;
  }
  else if (ARAD_NIF_IS_TYPE_ID(CGE, arad_nif_id))
  {
    nif_intern_id = ARAD_NIF_ID_OFFSET(CGE, arad_nif_id) * 16;
  }
  else if (ARAD_NIF_IS_TYPE_ID(TM_INTERNAL_PKT, arad_nif_id))
  {
    nif_intern_id = 0x0;
  }
  else
  {
    nif_intern_id = ARAD_NIF_ID_NONE;
  }
     
  return nif_intern_id;
}


STATIC int
arad_nif_wcmod_ucode_load(int unit, int port, uint8 *array,int datalen)
{
    int entry_bytes;
    int blk, core_idx;
    int count, extra_bytes, i, j, k;
    uint8 *array_ptr;
    uint32 *dma_buf_ptr;
    int spl_level;
    uint32 quad, phy_port, direct_port;
    uint32 shift, index;
    soc_pbmp_t quads_in_use;
    int dma_able;
    static int _arad_wc_ucode_alloc_size;
    static void *_arad_wc_ucode_dma_buf;

    SOCDNX_INIT_FUNC_DEFS;

    entry_bytes = soc_mem_entry_bytes(unit, PORT_WC_UCMEM_DATAm);

    spl_level = sal_splhi();

    if (_arad_wc_ucode_dma_buf == NULL) {
        count = datalen / entry_bytes;
        extra_bytes = datalen % entry_bytes;
        _arad_wc_ucode_alloc_size = datalen;
        if (extra_bytes != 0) {
            _arad_wc_ucode_alloc_size += entry_bytes - extra_bytes;
        }
        dma_able = soc_mem_slamable(unit, PORT_WC_UCMEM_DATAm, 0);
        if (dma_able)
        {
            _arad_wc_ucode_dma_buf = soc_cm_salloc(unit, _arad_wc_ucode_alloc_size, "WC ucode DMA buffer");
        } else {
            _arad_wc_ucode_dma_buf = sal_alloc(_arad_wc_ucode_alloc_size, "WC ucode buffer");
        }
        if (_arad_wc_ucode_dma_buf == NULL) {
            sal_spl(spl_level);
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate WC ucode DMA buffer"))); 
        }
        
        array_ptr = array;
        dma_buf_ptr = _arad_wc_ucode_dma_buf;
        for (i = 0; i < count; i++) {
            for (j = 0; j < entry_bytes; j += sizeof(uint32)) {
                index = (entry_bytes - sizeof(uint32) - j)/sizeof(uint32);
                dma_buf_ptr[index] = 0;
                for (k = 0; k < sizeof(uint32); k++) {
                    shift = 8*k;
                    dma_buf_ptr[index] |= (array_ptr[j + k] & 0xff) << shift;
                }
                dma_buf_ptr[index] = _shr_swap32(dma_buf_ptr[index]);
            }
            array_ptr += entry_bytes;
            dma_buf_ptr += entry_bytes/sizeof(uint32);
        }
        if (extra_bytes != 0) {
            sal_memset(dma_buf_ptr, 0, entry_bytes);
            for (j = 0; j < extra_bytes; j += sizeof(uint32)) {
                index = (entry_bytes - sizeof(uint32) - j)/sizeof(uint32);
                dma_buf_ptr[index] = 0;
                for (k = 0; k < sizeof(uint32) && j + k < extra_bytes; k++) {
                    shift = 8*k;
                    dma_buf_ptr[index] |= (array_ptr[j + k] & 0xff) << shift;
                }
                dma_buf_ptr[index] = _shr_swap32(dma_buf_ptr[index]);
            }
        }
    }

    sal_spl(spl_level);
    

    SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use));

    SOC_PBMP_ITER(quads_in_use, quad){

        phy_port = quad*LANES_IN_QUAD + 1;
        
        direct_port = phy_port + SOC_INFO(unit).physical_port_offset;
        core_idx = SOC_DRIVER(unit)->port_info[phy_port].bindex / 4;
                        

        
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, PORT_WC_UCMEM_CTRLr, direct_port, ACCESS_MODEf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, PORT_WC_UCMEM_CTRLr, direct_port, INST_SELECTf, core_idx));

        blk = SOC_PORT_BLOCK(unit, SOC_INFO(unit).port_l2p_mapping[direct_port]);

        
        SOCDNX_IF_ERR_RETURN(soc_mem_write_range(unit, PORT_WC_UCMEM_DATAm, blk, 0,
                                 _arad_wc_ucode_alloc_size / entry_bytes - 1,
                                 _arad_wc_ucode_dma_buf));


        
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, PORT_WC_UCMEM_CTRLr, direct_port, INST_SELECTf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, PORT_WC_UCMEM_CTRLr, direct_port, ACCESS_MODEf, 0));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
arad_port_wcmod_ucode_load(int unit ,int port, uint8 *array, int datalen)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(IS_SFI_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(arad_fabric_wcmod_ucode_load(unit, port, array, datalen));
    } else if (IS_ALL_PORT(unit,port)){
        SOCDNX_IF_ERR_EXIT(arad_nif_wcmod_ucode_load(unit, port, array, datalen));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("invalid port: %d"),port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_port_init(int unit)
{  
    int rv = SOC_E_NONE;
    soc_port_t port;
    pbmp_t     nif_ports;
    int port_local, clause = 22;
    uint16 phy_addr, data_mmd_select, data_lane_reset, data_lane_reset_new;
    uint16 data_lane_reset_cur;
    int min_port, max_port;
    int qrtt_active, i;
    SOCDNX_INIT_FUNC_DEFS;

    
    _phy_wcmod_firmware_set_helper[unit] = arad_port_wcmod_ucode_load;

         
    rv = soc_phy_common_init(unit);  
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOC_MSG("Failed phy common init: %s"),soc_errmsg(rv)));
    }

     
    MIIM_LOCK(unit);
    rv = soc_phyctrl_software_init(unit);
    MIIM_UNLOCK(unit);    
    if (SOC_FAILURE(rv)) {
        SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOC_MSG("Failed phyctrl software init: %s"), soc_errmsg(rv)));
    }
        
    
    PBMP_SFI_ITER(unit, port) {
       rv = arad_port_speed_max(unit, port , &(SOC_INFO(unit).port_speed_max[port])); 
       SOCDNX_IF_ERR_EXIT(rv);
    }
        
    
    SOC_PBMP_CLEAR(nif_ports);
    SOC_PBMP_ASSIGN(nif_ports, PBMP_PORT_ALL(unit));
    SOC_PBMP_REMOVE(nif_ports, PBMP_SFI_ALL(unit));

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ITER(nif_ports, port) {
           SOCDNX_IF_ERR_EXIT(soc_pm_mac_reset_set(unit, port, 1 ));
        }
    }

    sal_usleep(10);

    if(!SOC_WARM_BOOT(unit) && !SOC_IS_ARDON(unit) && !SAL_BOOT_PLISIM) {

        
        
        data_mmd_select = (0x0001 |
                           0x0002 |
                           0x0004 |
                           0x0008 |
                           0x4000);

        data_lane_reset = (0x8000 |
                           0x00f0 |
                           0x000f);

        min_port = FABRIC_LOGICAL_PORT_BASE(unit);

        max_port = min_port + SOC_DPP_DEFS_GET(unit, nof_fabric_links);

        for (port_local=min_port; port_local<max_port; port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {

            qrtt_active = 0;
            for (i=0; i<4; i++) {
                if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port_local + i)) {
                    qrtt_active = 1;
                    break;
                }
            }
            if (qrtt_active == 0) {
                 continue;
            }

            phy_addr = PORT_TO_PHY_ADDR_INT(unit, port_local);

            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phy_addr, 0x1f, 0xffd0));
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phy_addr, 0x1e, 0x0));
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause , phy_addr, 0x1f, 0x8000)); 
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phy_addr, 0x1d, data_mmd_select));
             
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause , phy_addr, 0x1f, 0x8100)); 
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, phy_addr, 0x1a, &data_lane_reset_cur));

            data_lane_reset_new = data_lane_reset | data_lane_reset_cur;

            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phy_addr, 0x1a, data_lane_reset_new));

        }

        sal_usleep(10);

        for (port_local=min_port; port_local<max_port; port_local+=SOC_ARAD_NOF_LINKS_IN_MAC) {

            qrtt_active = 0;
            for (i=0; i<4; i++) {
                if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port_local + i)) {
                    qrtt_active = 1;
                    break;
                }
            }
            if (qrtt_active == 0) {
                 continue;
            }

            phy_addr = PORT_TO_PHY_ADDR_INT(unit, port_local);

            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause , phy_addr, 0x1f, 0x8000)); 
            SOCDNX_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phy_addr, 0x1d, data_mmd_select)); 

        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

 
STATIC int
_arad_port_settings_init(int unit, soc_port_t port)
{
    int val = 0;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    if (!IS_SFI_PORT(unit, port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &val));
    } else {
        val = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, 0);
    }
    
    if(-1 != val && 42000 != val) {
        if(val == 0) {
            val = SOC_INFO(unit).port_speed_max[port];
        }
        rv = arad_port_speed_set(unit, port, val); 
        SOCDNX_IF_ERR_EXIT(rv);
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_DUPLEX, -1);
    if (val != -1) {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_duplex_set(unit, port, val);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_AUTONEG, -1);
    if(val != -1) {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_auto_negotiate_set(unit, port, val);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }  

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_post_init(int unit, soc_pbmp_t* ports) 
{
    soc_pbmp_t     recover_pbmp;
    soc_dpp_config_arad_t *dpp_arad;
    soc_port_t port;
    soc_port_if_t port_intf;
    uint32 flags, is_master, recover_flags = 0;
    int speed;
    SOCDNX_INIT_FUNC_DEFS;

    dpp_arad = SOC_DPP_CONFIG(unit)->arad;

    if (!SOC_WARM_BOOT(unit) && !SAL_BOOT_PLISIM && dpp_arad->init.nif_recovery_enable) 
    {
        SOC_PBMP_CLEAR(recover_pbmp);

        SOC_PBMP_ITER(*ports, port) {
            if (!IS_SFI_PORT(unit, port)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags)); 
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit,  port, &port_intf));

                if ((SOC_BLK_XLP == SOC_PORT_TYPE(unit, port) || speed == 12500 || speed == 48000) && 
                    !SOC_PORT_IS_STAT_INTERFACE(flags)  &&  is_master  &&  
                    port_intf != _SHR_PORT_IF_RCY  &&  port_intf != _SHR_PORT_IF_ILKN) {

                    if ((speed != 12500) && (speed != 48000)) {
                        
                        recover_flags |= ARAD_NIF_RECOVER_F_DONT_RUN_RX_TRAFFIC;    
                        ARAD_NIF_RECOVER_FLAGS_XLP_SET(recover_flags);
                    }
                    SOC_PBMP_PORT_ADD(recover_pbmp, port);
                }
            }
        }   
            
        SOCDNX_IF_ERR_EXIT(arad_nif_recover_run_recovery_test(unit, &recover_pbmp, recover_flags, dpp_arad->init.nif_recovery_iter, 1 ));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_deinit(int unit)
{
    soc_pbmp_t ports_bm;
    uint32 is_master, port;
    int result = SOC_E_NONE; 
    SOCDNX_INIT_FUNC_DEFS;

    result = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm);
    if(SOC_FAILURE(result)) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Failed soc_port_sw_db_valid_ports_get %s"), soc_errmsg(result)));
    } else {
        SOC_PBMP_ITER(ports_bm, port) {
            result = soc_port_sw_db_is_master_get(unit, port, &is_master);
            if(SOC_FAILURE(result)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Failed soc_pm_is_master_get %s"), soc_errmsg(result)));
                INT_PHY_SW_STATE(unit, port) = NULL;
                EXT_PHY_SW_STATE(unit, port) = NULL;
            } else if(!is_master) {
                INT_PHY_SW_STATE(unit, port) = NULL;
                EXT_PHY_SW_STATE(unit, port) = NULL;
            }
        }
    }

    MIIM_LOCK(unit);
    result = soc_phyctrl_software_deinit(unit);
    if(SOC_FAILURE(result)) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,"Failed soc_phyctrl_software_deinit %s"), soc_errmsg(result)));
    }
    MIIM_UNLOCK(unit);

    SOCDNX_FUNC_RETURN;
}

int 
arad_port_enable_set(int unit, soc_port_t port, uint32 mac_only , int enable) 
{
    uint32 flags;
    SOC_TMC_LINK_STATE_INFO link_state; 
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        SOC_TMC_LINK_STATE_INFO_clear(&link_state);
        link_state.on_off = enable ? SOC_TMC_LINK_STATE_ON : SOC_TMC_LINK_STATE_OFF;
        link_state.serdes_also = TRUE;
        SOCDNX_IF_ERR_EXIT(arad_link_on_off_set(unit, port, &link_state));
    } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if(!SOC_PORT_IS_NETWORK_INTERFACE(flags)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("arad_port_enable_set is supported only for NIF ports")));
        }

         SOCDNX_IF_ERR_EXIT(soc_pm_enable_set(unit, port, enable));
    } else if(SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("%s: unit %d, Invalid port %d"), FUNCTION_NAME(), unit, port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_enable_get(int unit, soc_port_t port, uint32 mac_only , int* enable) 
{
    SOC_TMC_LINK_STATE_INFO link_state; 
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        SOCDNX_IF_ERR_EXIT(arad_link_on_off_get(unit, port, &link_state));
        *enable = (link_state.on_off == SOC_TMC_LINK_STATE_ON ? 1 : 0) && link_state.serdes_also;
    } else if (SOC_IS_ARDON(unit)) {
        
    } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port) || SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
        SOCDNX_IF_ERR_EXIT(soc_pm_enable_get(unit, port, (uint32*)enable));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("Invalid port %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_speed_set(int unit, soc_port_t port, int speed) 
{
    SOCDNX_INIT_FUNC_DEFS;

     if (IS_SFI_PORT(unit, port)) {
         SOCDNX_SAND_IF_ERR_EXIT(arad_fabric_port_speed_set(unit, port, speed));
     } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port) || SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
         SOCDNX_IF_ERR_EXIT(soc_pm_speed_set(unit, port, speed));
    } else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("Invalid port %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_speed_get(int unit, soc_port_t port, int* speed)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_SFI_PORT(unit, port)) {
         SOCDNX_IF_ERR_EXIT(arad_fabric_port_speed_get(unit, port, speed));
     } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port) || SOC_PBMP_MEMBER(PBMP_CMIC(unit), port)) {
         SOCDNX_IF_ERR_EXIT(soc_pm_speed_get(unit, port, speed));
    } else {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOC_MSG("Invalid port %d"), port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf) 
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_SFI_PORT(unit, port)) {
        return SOC_E_NONE;
    }
    MIIM_LOCK(unit);
    rv = soc_phyctrl_interface_set(unit, port, intf);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf) 
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_SFI_PORT(unit, port)) {
        *intf = _SHR_PORT_IF_NOCXN;
        return SOC_E_NONE;
    }

    MIIM_LOCK(unit);
    rv = soc_phyctrl_interface_get(unit, port, intf);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_PORT_IF_XGMII == *intf) {
        soc_port_if_t interface_type;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        *intf = interface_type;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_pm_link_status_get(unit, port, is_link_up, is_latch_down));
    if(clear_status) {
        SOCDNX_IF_ERR_EXIT(soc_pm_link_status_clear(unit, port));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_is_supported_encap_get(int unit, int mode, int* is_supported) 
{
    SOCDNX_INIT_FUNC_DEFS;

    (*is_supported) = (mode== SOC_ENCAP_HIGIG2) || (mode == SOC_ENCAP_IEEE);

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)){
        (*is_supported) = (*is_supported) || (mode == SOC_ENCAP_SOP_ONLY);
    }
#endif

    SOCDNX_FUNC_RETURN;
}





int
soc_ardon_fabric_broadcast_firmware_loader(int unit,  uint32 length, const uint8* data)
{
#ifdef BCM_88202
    soc_reg_above_64_val_t wr_data;
    int j, n, blk;
    int max_ndx = 32*1024; 
    int port_local;

    SOCDNX_INIT_FUNC_DEFS;

    for (port_local = 0; port_local < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); port_local++) {
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, port_local, 0xf));
    } 
        
    for (j=0; j<max_ndx; j+=16) {
         
        SOC_REG_ABOVE_64_CLEAR(wr_data);
         for (n=0; n<16; n++) {
             if (j+n<length) {
                 wr_data[(15- n)/4] |= (data[j+n] & 0xff) <<  ((15-n) % 4)*8;
             }
        }

        SOC_BLOCK_ITER(unit, blk, SOC_BLK_FSRD) {
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, FSRD_FSRD_WL_EXT_MEMm, blk, j/16, wr_data));

        }
    }
    
    for (port_local = 0; port_local < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd); port_local++) {
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_WC_UC_MEM_MASK_BITMAPr(unit, port_local, 0x0));
    }

    return SOC_E_NONE;

exit:
    SOCDNX_FUNC_RETURN;
#else
    return SOC_E_UNAVAIL;
#endif
}




int
arad_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence) 
{
    int rc = SOC_E_NONE;
    soc_port_t port_i; 
    soc_pbmp_t nif_ports, fabric_ports, ok_nif, ok_fabric;
    int locked;
    int counter_interval;
    uint32 counter_flags;
    pbmp_t counter_pbmp, chan_ports;
    soc_port_t phy_port, cport;
    soc_pbmp_t quads_in_use, quads_oor;
    uint32 quad;
    uint32 first_phy_port;
    uint32 reg_val;
    int rv, blk_ins; 
    int port;
    int quad_index, quad_disabled[SOC_ARAD_NOF_QUADS_IN_FSRD*SOC_DPP_DEFS_MAX(NOF_INSTANCES_FSRD)];
    int fw_ld_method;

    SOCDNX_INIT_FUNC_DEFS;
    
    locked = 0;
    SOCDNX_NULL_CHECK(okay_pbmp);

    SOC_PBMP_CLEAR(ok_nif);
    SOC_PBMP_CLEAR(ok_fabric);

    SOC_PBMP_ASSIGN(fabric_ports, pbmp);
    SOC_PBMP_AND(fabric_ports, PBMP_SFI_ALL(unit));
    SOC_PBMP_ASSIGN(nif_ports, pbmp);
    SOC_PBMP_REMOVE(nif_ports, fabric_ports);


    MIIM_LOCK(unit);
    locked = 1;

    
    if (!SOC_IS_ARDON(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_phyctrl_pbm_probe_init(unit, fabric_ports, &ok_fabric)); 
    } else {
        if (!SOC_WARM_BOOT(unit)) {
            
            for (quad=0; quad <(SOC_ARAD_NOF_QUADS_IN_FSRD*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)); quad++) {
                
                quad_disabled[quad] = TRUE;
                
                for (quad_index = 0; quad_index < 4; quad_index++) {
                    
                    if (!(SOC_PBMP_MEMBER(SOC_CONTROL(unit)->info.sfi.disabled_bitmap, 
                                          (FABRIC_LOGICAL_PORT_BASE(unit)+quad*4)+quad_index))) {
                        quad_disabled[quad] = FALSE;
                        break;
                    }
                }
            }
                
            
            for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
                for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
                    if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                        continue;
                    }

                    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, &reg_val));

                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_RX_H_PWRDNf, 0);
                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_TX_H_PWRDNf, 0);
                    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));

                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_IDDQf, 0);
                    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
                    sal_usleep(10);

                    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_POR_H_RSTBf, 1);
                    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));

                }
            }
        }

            SOCDNX_IF_ERR_EXIT(soc_phyctrl_pbm_probe_init(unit, fabric_ports, &ok_fabric));

            
            

            fw_ld_method = ARAD_NIF_LOAD_METHOD_INTERNAL;;
            fw_ld_method  = soc_property_get(unit, spn_LOAD_FIRMWARE, fw_ld_method);

            if(fw_ld_method == ARAD_NIF_LOAD_METHOD_EXTERNAL)
            {
                if (!SOC_WARM_BOOT(unit)) {
                    rc = soc_ardon_fabric_broadcast_firmware_loader(unit, tsce_ucode_len, tsce_ucode); 
                }

                SOCDNX_IF_ERR_EXIT(rc);
                PBMP_ITER(ok_fabric, port) {
                    SOCDNX_IF_ERR_EXIT(soc_phyctrl_init(unit, port));
                }
            } 
            if (!SOC_WARM_BOOT(unit)) {
                
                for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
                    for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
                        if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                            continue;
                        }
                        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, &reg_val));
                        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_CORE_DP_H_RSTBf, 1);
                        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
                        sal_usleep(1);

                        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_H_RSTBf, 0xf);
                        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
                        sal_usleep(1);

                        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_DP_H_RSTBf, 0xf);
                        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
                        sal_usleep(1);

                    }
                }
            }

            PBMP_ITER(ok_fabric, port) {
                SOCDNX_IF_ERR_EXIT(soc_phyctrl_init(unit, port));
            }

    }


    if ( !(SOC_IS_ARDON(unit) && SOC_WARM_BOOT(unit)) ) {
            
        PBMP_ITER(ok_fabric, port_i) {
            if ((rc = soc_phyctrl_enable_set(unit, port_i, 0)) < 0) {
                SOCDNX_IF_ERR_EXIT(rc);
            }   
        }
    }
    
    
    locked = 0;
    MIIM_UNLOCK(unit);

    if (!SOC_WARM_BOOT(unit)) {
        soc_dcmn_port_pcs_t pcs;
        uint32 cl72;
        uint32 rx_polarity, tx_polarity;
        uint32 rxlane_map, txlane_map, lane_map;
        soc_pbmp_t phy_ports;
        uint32 is_master;
        soc_port_if_t interface_type;

        
        SOCDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
        soc_counter_stop(unit);
    
        
        SOC_PBMP_CLEAR(quads_oor);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_serdes_quads_out_of_reset_get(unit, &quads_oor));

        if (!SOC_IS_ARDON(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_pm_ports_init(unit, nif_ports, &ok_nif)); 
        } else {
            SOC_PBMP_CLEAR(ok_nif);
        }

        
        PBMP_ITER(ok_fabric, port_i) {

            pcs = soc_property_port_get(unit, port_i, spn_BACKPLANE_SERDES_ENCODING, soc_dcmn_port_pcs_8_9_legacy_fec);
            rv = arad_port_control_pcs_set(unit, port_i, pcs);
            SOCDNX_IF_ERR_EXIT(rv);

            cl72 = soc_property_port_get(unit, port_i, spn_PORT_INIT_CL72, 0) ? 1 : 0;
            rx_polarity = soc_property_port_get(unit, port_i, spn_PB_SERDES_LANE_SWAP_POLARITY_RX, FALSE);
            tx_polarity = soc_property_port_get(unit, port_i, spn_PB_SERDES_LANE_SWAP_POLARITY_TX, FALSE);
            rxlane_map = soc_property_port_get(unit, port_i, spn_XGXS_RX_LANE_MAP, ARAD_RX_LANE_MAP_DEFAULT);
            txlane_map = soc_property_port_get(unit, port_i, spn_XGXS_TX_LANE_MAP, ARAD_TX_LANE_MAP_DEFAULT);
            lane_map = (txlane_map << 16) + rxlane_map;

            MIIM_LOCK(unit);
            locked = 1;

            SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port_i, SOC_PHY_CONTROL_CL72, cl72));
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port_i, SOC_PHY_CONTROL_RX_POLARITY, rx_polarity));
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port_i, SOC_PHY_CONTROL_TX_POLARITY, tx_polarity));
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port_i, SOC_PHY_CONTROL_LANE_SWAP , lane_map)); 

            locked = 0;
            MIIM_UNLOCK(unit);

        }

        
        if (!SOC_IS_ARDON(unit)) {
            PBMP_ITER(nif_ports, port_i) {

                
                SOCDNX_IF_ERR_RETURN(soc_port_sw_db_is_master_get(unit, port_i, &is_master));
                if(!is_master) {
                    continue;
                }
             
            SOCDNX_IF_ERR_EXIT(_arad_port_settings_init(unit, port_i));  
 
                rv = soc_port_sw_db_phy_ports_get(unit, port_i, &phy_ports);
                SOCDNX_IF_ERR_EXIT(rv);
                rv = soc_port_sw_db_interface_type_get(unit, port_i, &interface_type);
                SOCDNX_IF_ERR_EXIT(rv);

                
                if (interface_type == SOC_PORT_IF_CAUI) {
                    rv = soc_port_sw_db_first_phy_port_get(unit, port_i, &first_phy_port );
                    SOCDNX_IF_ERR_EXIT(rv);
                    SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 11 );
                    SOC_PBMP_PORT_ADD(phy_ports, first_phy_port - 1 + 12 );
                }

                PBMP_ITER(phy_ports, phy_port) {
                    uint32 tx_polarity, rx_polarity;

                    
                    tx_polarity = soc_property_suffix_num_get(unit, phy_port, spn_PB_SERDES_LANE_SWAP_POLARITY_TX, "phy", SOC_ARAD_SERDES_POLARITY_NO_CHANGE);
                    rx_polarity = soc_property_suffix_num_get(unit, phy_port, spn_PB_SERDES_LANE_SWAP_POLARITY_RX, "phy", SOC_ARAD_SERDES_POLARITY_NO_CHANGE);

                    rv = soc_arad_serdes_polarity_set(unit, phy_port, tx_polarity, rx_polarity);
                    SOCDNX_IF_ERR_EXIT(rv);

                }

                
                
                SOC_PBMP_CLEAR(quads_in_use);
                SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit, port_i, &quads_in_use));
                SOC_PBMP_ITER(quads_in_use, quad){
                    if(!SOC_PBMP_MEMBER(quads_oor, quad)) {
                        txlane_map = soc_property_suffix_num_get(unit, quad, spn_XGXS_TX_LANE_MAP, "quad", SOC_ARAD_SERDES_LANE_SWAP_NO_CHANGE);
                        rxlane_map = soc_property_suffix_num_get(unit, quad, spn_XGXS_RX_LANE_MAP, "quad", SOC_ARAD_SERDES_LANE_SWAP_NO_CHANGE);

                        rv = soc_arad_serdes_lane_map_set(unit, quad, txlane_map, rxlane_map);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }
                }

                
                if (is_init_sequence) {
                    cl72 = soc_property_port_get(unit, port_i, spn_PORT_INIT_CL72, 0) ? 1 : 0;
                    if (cl72) {
                        SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit, port_i, SOC_PHY_CONTROL_CL72, cl72));
                    }
                }
            }
        }
   
        
        SOC_PBMP_OR(counter_pbmp, ok_nif);
        SOC_PBMP_OR(counter_pbmp, ok_fabric);

        
        SOC_PBMP_ITER(ok_nif, port_i) {

            rv =  soc_port_sw_db_ports_to_same_interface_get(unit, port_i, &chan_ports);
            SOCDNX_IF_ERR_EXIT(rv);

            SOC_PBMP_ITER(chan_ports, cport) {
                if(SOC_PBMP_MEMBER(pbmp, cport)) {
                    SOC_PBMP_PORT_ADD(ok_nif, cport);
                } else {
                    SOC_PBMP_PORT_REMOVE(ok_nif, cport);
                }
            }

            
            if(counter_interval > 0) {
                if(SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
                    rv = soc_port_sw_db_interface_type_get(unit, port_i, &interface_type);
                    SOCDNX_IF_ERR_EXIT(rv);

                    if(interface_type == SOC_PORT_IF_ILKN) {
                        SOC_PBMP_ITER(chan_ports, cport) {
                            if(SOC_PBMP_MEMBER(pbmp, cport)) {
                                SOC_PBMP_PORT_ADD(counter_pbmp, cport);
                            }
                        }
                    }
                }
            }
        }
      
        
        
        SOCDNX_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    }

    
    SOC_PBMP_ASSIGN(*okay_pbmp, ok_nif);
    SOC_PBMP_OR(*okay_pbmp, ok_fabric);
    
exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN; 
}

int
arad_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback) 
{
    int rv;
    soc_phy_control_t control;
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    if(IS_SFI_PORT(unit, port)) { 

        switch(loopback) { 
            case soc_dcmn_loopback_mode_mac_pcs:
            case soc_dcmn_loopback_mode_mac_async_fifo:
            case soc_dcmn_loopback_mode_mac_outer:
            case soc_dcmn_loopback_mode_phy_gloop:
            case soc_dcmn_loopback_mode_phy_rloop:
            case soc_dcmn_loopback_mode_none:
                break;

            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported loopback %d"), loopback)); 
        }

        rv = arad_fabric_loopback_set(unit, port, loopback);
        SOCDNX_IF_ERR_EXIT(rv); 
    } else { 

        switch(loopback) { 
            case soc_dcmn_loopback_mode_mac_outer:
            case soc_dcmn_loopback_mode_phy_gloop:
            case soc_dcmn_loopback_mode_phy_rloop:
            case soc_dcmn_loopback_mode_none:
                break;

            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported loopback %d"), loopback)); 
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

        
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_set(unit, port, (loopback == soc_dcmn_loopback_mode_mac_outer)));

        
        MIIM_LOCK(unit);
        rv = soc_phyctrl_loopback_set(unit, port, (loopback == soc_dcmn_loopback_mode_phy_gloop), TRUE);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        
        if((EXT_PHY_SW_STATE(unit, port) == NULL) && (SOC_PORT_IF_ILKN == interface || SOC_PORT_IF_CAUI == interface)) {
            control = SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS;
        } else {
            control = SOC_PHY_CONTROL_LOOPBACK_REMOTE;
        }
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_set(unit, port, control, (loopback == soc_dcmn_loopback_mode_phy_rloop));
        MIIM_UNLOCK(unit);
        if((rv != SOC_E_UNAVAIL) || (loopback == soc_dcmn_loopback_mode_phy_rloop)){
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
arad_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback) 
{
    int rv, lb;
    soc_phy_control_t control;
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    if(IS_SFI_PORT(unit, port)) { 
        rv = arad_fabric_loopback_get(unit, port, loopback);
        SOCDNX_IF_ERR_EXIT(rv); 
    } else { 

        
        SOCDNX_IF_ERR_EXIT(soc_pm_mac_loopback_get(unit, port, (uint32*)&lb));
        if(lb) {
            *loopback = soc_dcmn_loopback_mode_mac_outer;
            SOC_EXIT;
        }

        
        MIIM_LOCK(unit);
        rv = soc_phyctrl_loopback_get(unit, port, &lb);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
        if(lb) {
            *loopback = soc_dcmn_loopback_mode_phy_gloop;
            SOC_EXIT;
        }

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
        if(SOC_PORT_IF_ILKN == interface || SOC_PORT_IF_CAUI == interface) {
            control = SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS;
        } else {
            control = SOC_PHY_CONTROL_LOOPBACK_REMOTE;
        }
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, control, (uint32*)&lb);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    
        if(lb) {
            *loopback = soc_dcmn_loopback_mode_phy_rloop;
            SOC_EXIT;
        }

        
        *loopback = soc_dcmn_loopback_mode_none;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int
arad_port_phy_reset(int unit, soc_port_t port) 
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_reset(unit, port, NULL);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}

int 
arad_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(lane != -1 || phyn != -1 || is_sys_side){
        MIIM_LOCK(unit);
        rv = soc_phyctrl_redirect_control_set(unit, port, phyn, lane, is_sys_side, type, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);     
    }else {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_set(unit, port, type, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
arad_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32* value)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if(lane != -1 || phyn != -1 || is_sys_side){
        MIIM_LOCK(unit);
        rv = soc_phyctrl_redirect_control_get(unit, port, phyn, lane, is_sys_side, type, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);     
    }else {
        MIIM_LOCK(unit);
        rv = soc_phyctrl_control_get(unit, port, type, value);
        MIIM_UNLOCK(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_port_phy_reg_get(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    uint32  phy_id;
    uint32 phy_reg;
    uint16 phy_rd_data;
    uint32 reg_flag;
    int    rv;
    int clause;
    SOCDNX_INIT_FUNC_DEFS;


    MIIM_LOCK(unit);

    clause = (flags & SOC_PHY_CLAUSE45 ? 45 : 22);

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        rv = soc_phyctrl_reg_read(unit, port, flags, phy_reg_addr, phy_data);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
        if (reg_flag & SOC_PHY_REG_INDIRECT) {
            if (flags & SOC_PHY_NOMAP) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("indirect register access is not supported if SOC_PHY_NOMAP flag is set")));
            }
            phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
            rv = soc_phyctrl_reg_read(unit, port, flags, phy_reg_addr, phy_data);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            if (flags & SOC_PHY_NOMAP) {
                phy_id = port;
            } else if (flags & SOC_PHY_INTERNAL) {
                phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
            } else {
                phy_id = PORT_TO_PHY_ADDR(unit, port);
            }


            phy_reg = phy_reg_addr;
            rv = soc_dcmn_miim_read(unit, clause, phy_id, phy_reg, &phy_rd_data);
            SOCDNX_IF_ERR_EXIT(rv);

            *phy_data = phy_rd_data;

        }
    }
    
exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

int
arad_port_phy_reg_set(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{
    uint32  phy_id;
    uint32 phy_reg;
    uint16 phy_wr_data;
    uint32 reg_flag;
    int    rv;
    int clause;
    SOCDNX_INIT_FUNC_DEFS;

    clause = (flags & SOC_PHY_CLAUSE45 ? 45 : 22);

    MIIM_LOCK(unit);

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        rv = soc_phyctrl_reg_write(unit, port, flags, phy_reg_addr, phy_data);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
        if (reg_flag & SOC_PHY_REG_INDIRECT) {
            if (flags & SOC_PHY_NOMAP) {
                
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("indirect register access is not supported if SOC_PHY_NOMAP flag is set")));
            }
            phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
            rv = soc_phyctrl_reg_write(unit, port, flags, phy_reg_addr, phy_data);
            SOCDNX_IF_ERR_EXIT(rv);
        } else {
            if (flags & SOC_PHY_NOMAP) {
                phy_id = port;
            } else if (flags & SOC_PHY_INTERNAL) {
                phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
            } else {
                phy_id = PORT_TO_PHY_ADDR(unit, port);
            }

            phy_wr_data = (uint16) (phy_data & 0xffff);
            phy_reg = phy_reg_addr;
            rv = soc_dcmn_miim_write(unit, clause, phy_id, phy_reg, phy_wr_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}

int
arad_port_phy_reg_modify(int unit, soc_port_t port, uint32 flags,
                        uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    uint32  phy_id;
    uint32 phy_reg;
    uint16 phy_rd_data;
    uint16 phy_wr_data;
    uint32 reg_flag;
    int    rv;
    int clause;
    SOCDNX_INIT_FUNC_DEFS;
    
    MIIM_LOCK(unit);

    clause = (flags & SOC_PHY_CLAUSE45 ? 45 : 22);

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT) {
        if (flags & SOC_PHY_NOMAP) {
            
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("indirect register access is not supported if SOC_PHY_NOMAP flag is set")));
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = soc_phyctrl_reg_modify(unit, port, flags, phy_reg_addr, phy_data, phy_mask);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        if (flags & SOC_PHY_NOMAP) {
            phy_id = port;
        } else if (flags & SOC_PHY_INTERNAL) {
            phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
        } else {
            phy_id = PORT_TO_PHY_ADDR(unit, port);
        }

        phy_wr_data = (uint16) (phy_data & phy_mask & 0xffff);
        phy_reg = phy_reg_addr;
        rv = soc_dcmn_miim_read(unit, clause, phy_id, phy_reg, &phy_rd_data);
        SOCDNX_IF_ERR_EXIT(rv);
        phy_wr_data |= (phy_rd_data & ~phy_mask);
        rv = soc_dcmn_miim_write(unit, clause, phy_id, phy_reg, phy_wr_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    MIIM_UNLOCK(unit);
    SOCDNX_FUNC_RETURN;
}


int
arad_port_protocol_offset_verify(
   SOC_SAND_IN   int                   unit,
   SOC_SAND_IN   soc_port_t            port,
   SOC_SAND_IN   uint32                protocol_offset
   )


{
    uint32 phy_port;
    uint32 lane_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));

    if (SOC_IS_JERICHO(unit)) { 
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &lane_id)));
    } else {
        lane_id = phy_port;
    }

    
    if (protocol_offset/ARAD_NIF_NUM_OF_OFFSETS_IN_PROTOCOL_GROUP != (lane_id-1)/ARAD_NIF_NUM_OF_LANES_IN_PROTOCOL_GROUP) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Protocol offset is out of range for the given interface"))); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
_arad_port_eee_mac_nof_ports_enabled(int unit, soc_port_t port, int *nof_ports_enabled)
{
    soc_port_t port_idx;
    uint32 value, enabled = 0;
    uint32 first_phy_port, mac_first_port, mac_last_port, phy_port;
    soc_pbmp_t ports_bm;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy_port ));
    --first_phy_port; 

    if (first_phy_port < ARAD_NIF_XLP0_PORT_MIN) {
        mac_first_port = ARAD_NIF_CLP0_PORT_MIN;
        mac_last_port = mac_first_port + ARAD_NIF_CLP_NOF_PORTS - 1;
    } else if (first_phy_port < ARAD_NIF_CLP1_PORT_MIN) {
        mac_first_port = ARAD_NIF_XLP0_PORT_MIN;
        mac_last_port = mac_first_port + ARAD_NIF_XLP0_NOF_PORTS - 1;
    } else { 
        mac_first_port = ARAD_NIF_CLP1_PORT_MIN;
        mac_last_port = mac_first_port + ARAD_NIF_CLP_NOF_PORTS - 1;
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &ports_bm));
    SOC_PBMP_ITER(ports_bm, port_idx) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port_idx, &phy_port ));
        --phy_port; 
        if (phy_port >= mac_first_port && phy_port <= mac_last_port) {
            SOCDNX_IF_ERR_EXIT(soc_pm_eee_enable_get(unit, port_idx, &value));
            if (value) {
                ++enabled;
            }
        }
    }

    *nof_ports_enabled = enabled;

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_enable_set(int unit, soc_port_t port, uint32 value)
{
    int lp_index;
    uint32 rval;
    soc_field_t neee_pd_en;
    uint32 is_enabled;
    int ports_enabled=0;
    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_BLK_CLP == SOC_PORT_TYPE(unit,port)) {
        neee_pd_en = CLP_NEEE_PD_ENf;
        (port < ARAD_NIF_XLP0_PORT_MIN) ? (lp_index = 0) : (lp_index = 1);
    } else {
        neee_pd_en = XLP_NEEE_PD_ENf;
        (port < ARAD_NIF_CLP1_PORT_MIN) ? (lp_index = 0) : (lp_index = 1);
    }

    
    SOCDNX_IF_ERR_EXIT(_arad_port_eee_mac_nof_ports_enabled(unit, port, &ports_enabled));
    SOCDNX_IF_ERR_EXIT(soc_pm_eee_enable_get(unit, port, &is_enabled));

    
    if (value && (0 == ports_enabled)) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_NIF_PORTS_CFGr(unit, lp_index, &rval));
        soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &rval, neee_pd_en, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_NIF_PORTS_CFGr(unit, lp_index, rval));
    } else if ((0 == value) && (1 == ports_enabled) && is_enabled) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_NIF_PORTS_CFGr(unit, lp_index, &rval));
        soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &rval, neee_pd_en, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_NIF_PORTS_CFGr(unit, lp_index, rval));
    } else {
        
    }

    
    SOCDNX_IF_ERR_EXIT(soc_pm_eee_enable_set(unit, port, value));

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_statistics_clear(int unit, soc_port_t port)
{
    uint32 val = 0;
    uint64 val64;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(val64);

    SOCDNX_IF_ERR_EXIT(READ_PORT_XGXS_COUNTER_MODEr(unit, port, &val));
    soc_reg_field_set(unit, PORT_XGXS_COUNTER_MODEr, &val, CNT_MODEf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_XGXS_COUNTER_MODEr(unit, port, val));

    
    SOCDNX_IF_ERR_EXIT(READ_RX_EEE_LPI_DURATION_COUNTERr(unit, port, &val64));
    SOCDNX_IF_ERR_EXIT(READ_RX_EEE_LPI_EVENT_COUNTERr(unit, port, &val64));
    SOCDNX_IF_ERR_EXIT(READ_TX_EEE_LPI_DURATION_COUNTERr(unit, port, &val64));
    SOCDNX_IF_ERR_EXIT(READ_TX_EEE_LPI_EVENT_COUNTERr(unit, port, &val64));

    
    SOCDNX_IF_ERR_EXIT(READ_PORT_XGXS_COUNTER_MODEr(unit, port, &val));
    soc_reg_field_set(unit, PORT_XGXS_COUNTER_MODEr, &val, CNT_MODEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_XGXS_COUNTER_MODEr(unit, port, val));

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_event_count_symmetric_set(int unit, soc_port_t port, uint32 value)
{
    uint32 val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_PORT_EEE_COUNTER_MODEr(unit, port, &val));
    soc_reg_field_set(unit, PORT_EEE_COUNTER_MODEr, &val, MODE_BITf, (value) ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_EEE_COUNTER_MODEr(unit, port, val));

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_event_count_symmetric_get(int unit, soc_port_t port, uint32 *value)
{
    uint32 val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_PORT_EEE_COUNTER_MODEr(unit, port, &val));
    *value = soc_reg_field_get(unit, PORT_EEE_COUNTER_MODEr, val, MODE_BITf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_tx_event_count_get(int unit, soc_port_t port, uint32 *value)
{
    uint64 reg64, field64;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64);
    COMPILER_64_ZERO(field64);

    SOCDNX_IF_ERR_EXIT(READ_TX_EEE_LPI_EVENT_COUNTERr(unit, port, &reg64));
    field64 = soc_reg64_field_get(unit, TX_EEE_LPI_EVENT_COUNTERr, reg64, COUNTf);
    *value = COMPILER_64_LO(field64);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_tx_duration_get(int unit, soc_port_t port, uint32 *value)
{
    uint64 reg64, field64;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64);
    COMPILER_64_ZERO(field64);

    SOCDNX_IF_ERR_EXIT(READ_TX_EEE_LPI_DURATION_COUNTERr(unit, port, &reg64));
    field64 = soc_reg64_field_get(unit, TX_EEE_LPI_DURATION_COUNTERr, reg64, COUNTf);
    *value = COMPILER_64_LO(field64);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_rx_event_count_get(int unit, soc_port_t port, uint32 *value)
{
    uint64 reg64, field64;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64);
    COMPILER_64_ZERO(field64);

    SOCDNX_IF_ERR_EXIT(READ_RX_EEE_LPI_EVENT_COUNTERr(unit, port, &reg64));
    field64 = soc_reg64_field_get(unit, RX_EEE_LPI_EVENT_COUNTERr, reg64, COUNTf);
    *value = COMPILER_64_LO(field64);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_eee_rx_duration_get(int unit, soc_port_t port, uint32 *value)
{
    uint64 reg64, field64;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64);
    COMPILER_64_ZERO(field64);

    SOCDNX_IF_ERR_EXIT(READ_RX_EEE_LPI_DURATION_COUNTERr(unit, port, &reg64));
    field64 = soc_reg64_field_get(unit, RX_EEE_LPI_DURATION_COUNTERr, reg64, COUNTf);
    *value = COMPILER_64_LO(field64);

exit:
    SOCDNX_FUNC_RETURN;
}


int
arad_port_cable_diag(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN soc_port_t              port,
    SOC_SAND_OUT soc_port_cable_diag_t  *status
  )
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_cable_diag(unit, port, status);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_link_get(int unit, int port, int* link)
{
    int rv;
    int is_latch_down;
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_SFI_PORT(unit, port)) {
        rv = arad_fabric_link_up_get(unit, port, link);
        SOCDNX_IF_ERR_EXIT(rv);
    } else { 
        rv = soc_pm_link_status_get(unit, port, link, &is_latch_down);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_autoneg_set(int unit, soc_port_t port, int autoneg)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_auto_negotiate_set(unit, port, autoneg);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_autoneg_get(int unit, soc_port_t port, int *autoneg)
{
    int rv, done;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_auto_negotiate_get(unit, port, autoneg, &done);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int rv;
    int pause_tx, pause_rx, loopback;
    soc_port_if_t interface, intf;
    uint32 is_valid;  
    
    soc_port_ability_t phy_ability, mac_ability;
    soc_dcmn_loopback_mode_t dcmn_loopback;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    rv = soc_phyctrl_ability_local_get(unit, port, &phy_ability);
    SOCDNX_IF_ERR_EXIT(rv);
    ability->speed_full_duplex = phy_ability.speed_full_duplex;
    ability->speed_half_duplex = phy_ability.speed_half_duplex;
    if (!IS_SFI_PORT(unit,port))
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if(!is_valid) {
            SOCDNX_EXIT_WITH_ERR(BCM_E_PORT,(_BSL_SOC_MSG("Invalid local port %d"), port));
        }
        if(interface != BCM_PORT_IF_ILKN){
            SOCDNX_IF_ERR_EXIT(soc_pm_mac_ability_get(unit, port, &mac_ability));
            ability->speed_full_duplex &= mac_ability.speed_full_duplex;
            ability->speed_half_duplex &= mac_ability.speed_half_duplex;
        }
    }
    
    if (interface == SOC_PORT_IF_ILKN) {
        pause_rx = pause_tx = 0;
    } 
    else {
        pause_tx = SOC_PA_PAUSE_TX;
        pause_rx = SOC_PA_PAUSE_RX;
    }
        

    
    SOCDNX_IF_ERR_EXIT(arad_port_loopback_get(unit, port, &dcmn_loopback));
    switch(dcmn_loopback) {
        case soc_dcmn_loopback_mode_none:
            loopback = BCM_PORT_LOOPBACK_NONE;
            break;
        case soc_dcmn_loopback_mode_mac_outer:
        case soc_dcmn_loopback_mode_mac_pcs:
        case soc_dcmn_loopback_mode_mac_async_fifo:
            loopback = BCM_PORT_LOOPBACK_MAC;
            break;
        case soc_dcmn_loopback_mode_phy_gloop:
            loopback = BCM_PORT_LOOPBACK_PHY;
            break;
        case soc_dcmn_loopback_mode_phy_rloop:
            loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(BCM_E_INTERNAL, (_BSL_SOC_MSG("unrecognized loopback type %d"), dcmn_loopback)); 
            break;
    }
    
    SOCDNX_IF_ERR_EXIT(arad_port_interface_get(unit, port, &intf));    
    
    ability->pause = (pause_tx | pause_rx);
    ability->interface = intf;   
    
    ability->eee = SOC_PA_ABILITY_ALL;    
    ability->loopback = (loopback | BCM_PORT_ABILITY_LB_PHY | BCM_PORT_ABILITY_LB_NONE);
    ability->flags = 0;
    ability->encap = SOC_PA_ABILITY_ALL;

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_ability_remote_get(unit, port, ability_mask);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability_mask)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_ability_advert_set(unit, port, ability_mask);;
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_ability_advert_get(unit, port, ability_mask);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
arad_port_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_mdix_set(unit, port, mode);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_mdix_get(unit, port, mode);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_mdix_status_get(int unit, soc_port_t port, soc_port_mdix_status_t *status)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_mdix_status_get(unit, port, status);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_duplex_set(int unit, soc_port_t port, int duplex)
{
    int locked = 0;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    locked = 1;
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_auto_negotiate_set(unit, port, FALSE));
    SOCDNX_IF_ERR_EXIT(soc_phyctrl_duplex_set(unit, port, duplex));
    locked = 0;
    MIIM_UNLOCK(unit);
    

exit:
    if (locked) {
        MIIM_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN;
}

int
arad_port_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    MIIM_LOCK(unit);
    rv = soc_phyctrl_duplex_get(unit, port, duplex);
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_fault_get(int unit, soc_port_t port, uint32 *flags)
{
    int local_fault, remote_fault;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_pm_local_fault_get(unit, port, &local_fault));
    SOCDNX_IF_ERR_EXIT(soc_pm_remote_fault_get(unit, port, &remote_fault));

    *flags = 0;
    if (remote_fault) {
        *flags |= BCM_PORT_FAULT_REMOTE;
    }
    if (local_fault) {
        *flags |= BCM_PORT_FAULT_LOCAL;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_port_fabric_clk_freq_init(int unit, soc_pbmp_t pbmp)
{
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ITER(pbmp, port)
    {
        if((ARAD_INIT_SERDES_REF_CLOCK_125 == SOC_DPP_CONFIG(unit)->arad->init.pll.fabric_clk_freq)) {
           SOC_INFO(unit).port_refclk_int[port] = 125;
        } else {
           SOC_INFO(unit).port_refclk_int[port] = 156;
        }
    }   

    SOCDNX_FUNC_RETURN;
}

int
arad_port_nif_nof_lanes_get(int unit, soc_port_if_t interface, uint32 first_phy_port, uint32 nof_lanes_to_set, uint32 *nof_lanes)
{
    soc_port_t port_i;

    SOCDNX_INIT_FUNC_DEFS;

    *nof_lanes = 0;

    switch(interface) {
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_QSGMII:
            *nof_lanes = 1;
            break;

        case SOC_PORT_IF_CPU:
            *nof_lanes = 1;
            if (first_phy_port != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
            break;

        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_XLAUI2:
            *nof_lanes = 2;
            if ((first_phy_port-1)%(*nof_lanes) != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
            break;

        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_XLAUI:
            *nof_lanes = 4;

            if ((first_phy_port-1)%(*nof_lanes) != 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
            break;

        case SOC_PORT_IF_ILKN:
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_from_interface_type_get(unit, interface, first_phy_port, &port_i));
            if (port_i != SOC_MAX_NUM_PORTS) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, nof_lanes));                   
            } else {
                *nof_lanes = nof_lanes_to_set;
            }
            if ((first_phy_port != 1) && (first_phy_port != 17) && (!((SOC_IS_ARADPLUS(unit)) && (first_phy_port == 11)))) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
 
            break;

        case SOC_PORT_IF_CAUI:
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_port_from_interface_type_get(unit, interface, first_phy_port, &port_i));
            if (port_i != SOC_MAX_NUM_PORTS) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port_i, nof_lanes));                   
            } else {
                *nof_lanes = nof_lanes_to_set;
            }

            if ((first_phy_port != 1) && (first_phy_port != 17)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("interface not supported on port")));
            }
            break;

        case SOC_PORT_IF_RCY:
        case SOC_PORT_IF_ERP:
        case SOC_PORT_IF_OLP:
        case SOC_PORT_IF_OAMP:
        case SOC_PORT_IF_IPSEC:
        case SOC_PORT_IF_SAT:
            *nof_lanes = 0;
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Interface %d isn't supported"), interface));
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_port_link_up_mac_update(int unit, soc_port_t port, int link)
{
    int speed, rv, an = 0, an_done = 0;
    int duplex, tx_pause, rx_pause;
    bcm_port_ability_t remote_advert, local_advert;
        
        
    SOCDNX_INIT_FUNC_DEFS;
    
    MIIM_LOCK(unit);
    rv = soc_phyctrl_speed_get(unit, port, &speed);
    if (rv == SOC_E_NONE){
        rv = soc_phyctrl_auto_negotiate_get(unit, port, &an, &an_done);
    }
    MIIM_UNLOCK(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    if(!an || !an_done){
        SOC_EXIT;
    }

         
    sal_memset(&local_advert,  0, sizeof(bcm_port_ability_t));
    sal_memset(&remote_advert, 0, sizeof(bcm_port_ability_t));
    rv = soc_phyctrl_duplex_get(unit, port, &duplex);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_phyctrl_ability_advert_get(unit, port, &local_advert);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_phyctrl_ability_remote_get(unit, port, &remote_advert);
    SOCDNX_IF_ERR_EXIT(rv);
    
    if (duplex){
        tx_pause = ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
            (local_advert.pause & SOC_PA_PAUSE_RX)) || \
            ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
            !(remote_advert.pause & SOC_PA_PAUSE_TX) && \
            (local_advert.pause & SOC_PA_PAUSE_TX));
         rx_pause = ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
            (local_advert.pause & SOC_PA_PAUSE_RX)) || \
            ((local_advert.pause & SOC_PA_PAUSE_RX) && \
            (remote_advert.pause & SOC_PA_PAUSE_TX) && \
            !(local_advert.pause & SOC_PA_PAUSE_TX));
    }
    else {
        rx_pause = tx_pause = 0;
    }
    rv = soc_pm_pause_set(unit, port, tx_pause, rx_pause);
    SOCDNX_IF_ERR_EXIT(rv);
    rv = soc_pm_mac_speed_set(unit, port, speed);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
arad_port_close_path(int unit, soc_port_t port) {

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_FUNC_RETURN;
}


int
soc_arad_nif_sku_restrictions(int unit, soc_pbmp_t phy_pbmp, soc_port_if_t interface, uint32 protocol_offset, uint32 is_kbp){

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_FUNC_RETURN;
}


int
soc_arad_port_speed_sku_restrictions(int unit, soc_port_t port, int speed){

    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

