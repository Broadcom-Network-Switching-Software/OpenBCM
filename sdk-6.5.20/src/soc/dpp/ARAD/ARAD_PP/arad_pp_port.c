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



#include <shared/swstate/sw_state_defs.h>

#include <shared/swstate/access/sw_state_access.h>

#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_port.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/drv.h>





#define SOC_PPC_PORT_STP_STATE_MAX                     (SOC_PPC_NOF_PORT_STP_STATES-1)

#define SOC_PPC_PORT_PORT_PROFILE_MAX                  (7)
#define SOC_PPC_PORT_DA_NOT_FOUND_PROFILE_MAX          (1)
#define SOC_PPC_PORT_PORT_TYPE_MAX                     (SOC_SAND_PP_NOF_PORT_L2_TYPES-1)
#define SOC_PPC_PORT_ORIENTATION_MAX                   (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_MAX      (7)
#define SOC_PPC_PORT_MTU_MAX                           ((1 << soc_mem_field_length(unit, EGQ_PP_PPCTm, MTUf)) - 1)
#define SOC_PPC_PORT_MTU_ENCAP_MAX                     ((1 << soc_mem_field_length(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, MTUf)) - 1)
#define SOC_PPC_PORT_VLAN_TRANSLATION_MAX              (SOC_IS_JERICHO(unit) ? 31 : 7)
#define SOC_PPC_PORT_VSI_PROFILE_MAX                   (7)

#define ARAD_MAPPED_PP_PORT_CORE_BIT                   (8)










typedef enum
{
  SOC_PPC_PORT_STP_STATE_EG_FLD_VAL_BLOCK = 1,
  SOC_PPC_PORT_STP_STATE_EG_FLD_VAL_LEARN = 1,
  SOC_PPC_PORT_STP_STATE_EG_FLD_VAL_FORWARD = 0
} SOC_PPC_PORT_STP_STATE_EG_FLD_VAL;





CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_port[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_STP_STATE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORT_GET_ERRS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORTS_REGS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_PORTS_INIT),
  

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_port[] =
{
  
  {
    SOC_PPC_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'topology_id_ndx' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_STP_STATE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_STP_STATE_OUT_OF_RANGE_ERR",
    "The parameter 'stp_state' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_PORT_STP_STATES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR",
    "The parameter 'sys_phy_port_id' is out of range. \n\r "
    "The range is: 0 - 4095.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'direction_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_PORT_DIRECTIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'learn_dest_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_DEST_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_PORT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_PORT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'port_type' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_PORT_L2_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'ether_type_based_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_MTU_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_MTU_OUT_OF_RANGE_ERR",
    "The parameter 'mtu' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'da_not_found_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_VT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_VT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_translation_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PPC_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'ac_p2p_to_pbb_vsi_profile' or 'vlabel_pwe_p2p_vsi_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};






uint32
  arad_pp_port_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    epni_stp_data[8],
    ihp_stp_data[SOC_DPP_IMP_DEFS_MAX(IHP_STP_TABLE_NOF_LONGS)],
    pinfo_llr_tbl[3],
    ihb_flp_pinfo_tbl[1];
  int core = SOC_CORE_INVALID;
  uint32 fld_val,pp_port, mapped_pp_port;
  soc_reg_above_64_val_t 
		in_port_tbl_data;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    port_info_flp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  soc_sand_os_memset(epni_stp_data, 0x0, sizeof(epni_stp_data));
  res = arad_fill_table_with_entry(unit, EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm, MEM_BLOCK_ANY, &epni_stp_data); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_sand_os_memset(ihp_stp_data, 0x0, sizeof(ihp_stp_data));
  res = arad_fill_table_with_entry(unit, IHP_STP_TABLEm, MEM_BLOCK_ANY, &ihp_stp_data); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_sand_os_memset(pinfo_llr_tbl, 0x0, sizeof(pinfo_llr_tbl));
  res = arad_fill_table_with_entry(unit, IHP_PINFO_LLRm, MEM_BLOCK_ANY, &pinfo_llr_tbl); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_IS_JERICHO(unit)) {

	SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        soc_sand_os_memset(ihb_flp_pinfo_tbl, 0x0, sizeof(ihb_flp_pinfo_tbl));
        res = arad_fill_table_with_entry(unit, IHP_PINFO_FLP_0m, IHP_BLOCK(unit, core), &ihb_flp_pinfo_tbl); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        soc_sand_os_memset(ihb_flp_pinfo_tbl, 0x0, sizeof(ihb_flp_pinfo_tbl));
        res = arad_fill_table_with_entry(unit, IHP_PINFO_FLP_1m, IHP_BLOCK(unit, core), &ihb_flp_pinfo_tbl); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

        for (pp_port = 0; pp_port < MAX_PORTS_IN_CORE ; pp_port++) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_IHP_VTT_IN_PP_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, in_port_tbl_data));
            
            fld_val = (1 << SOC_PPC_MYMAC_NOF_MY_NICKNAMES) -1; 
            soc_IHP_VTT_IN_PP_PORT_CONFIGm_field_set(unit, in_port_tbl_data, ENABLE_MY_NICK_NAME_BITMAPf, &fld_val);
            
            mapped_pp_port = ( (core << ARAD_MAPPED_PP_PORT_CORE_BIT) & 0x100 ) | (pp_port & 0xff);
            soc_IHP_VTT_IN_PP_PORT_CONFIGm_field_set(unit, in_port_tbl_data, MAPPED_PP_PORTf, &mapped_pp_port);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, WRITE_IHP_VTT_IN_PP_PORT_CONFIGm(unit, IHP_BLOCK(unit, core), pp_port, in_port_tbl_data));


            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHP_PINFO_FLP_0m(unit, IHP_BLOCK(unit, core), pp_port, in_port_tbl_data));
			soc_IHP_PINFO_FLP_0m_field_set(unit, in_port_tbl_data, MAPPED_PP_PORTf, &mapped_pp_port);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 65, exit, WRITE_IHP_PINFO_FLP_0m(unit, IHP_BLOCK(unit, core), pp_port, in_port_tbl_data));

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_IHB_PINFO_PMFm(unit, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));
			soc_IHB_PINFO_PMFm_field_set(unit, in_port_tbl_data, MAPPED_PP_PORTf, &mapped_pp_port);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 75, exit, WRITE_IHB_PINFO_PMFm(unit, IHB_BLOCK(unit, core), pp_port, in_port_tbl_data));
            if ( soc_property_port_suffix_num_get(unit, pp_port, -1, spn_CUSTOM_FEATURE, "drop_unmatched_ces_packet", -1) == 1) {
                res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
                        unit,
                        core,
                        pp_port,
                        &port_info_flp_tbl_data
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
                port_info_flp_tbl_data.program_selection_profile |= ARAD_PP_FLP_PORT_PROFILE_CES;
                res = arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
                        unit,
                        core,
                        pp_port,
                        &port_info_flp_tbl_data
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
            }
		}
	}
  } else {
    soc_sand_os_memset(ihb_flp_pinfo_tbl, 0x0, sizeof(ihb_flp_pinfo_tbl));
    res = arad_fill_table_with_entry(unit, IHB_PINFO_FLPm, MEM_BLOCK_ANY, &ihb_flp_pinfo_tbl); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_init_unsafe()", 0, 0);
}



STATIC void
  arad_pp_port_orientation2intern(
    SOC_SAND_IN SOC_SAND_PP_HUB_SPOKE_ORIENTATION port_orientation,
    SOC_SAND_OUT uint32                     *egq_ppct_intern
  )
{
  uint32
    port_o_intern;
  
  switch(port_orientation) {
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:
    port_o_intern = 0x1;
    break;
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE:
    port_o_intern = 0x0;
    break;
  default:
    
    port_o_intern = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  }

  *egq_ppct_intern =  port_o_intern;
}


STATIC void
  arad_pp_port_intern2orientation(
    SOC_SAND_IN uint32                        egq_ppct_intern,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION  *port_orientation
  )
{
  SOC_SAND_PP_HUB_SPOKE_ORIENTATION
    port_o_intern;
  
  switch(egq_ppct_intern) {
  case 0x1:
    port_o_intern = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
    break;
  case 0x0:
    port_o_intern = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    break;
  default:
    
    port_o_intern = SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS;
  }

  *port_orientation =  port_o_intern;
}



uint32
  arad_pp_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_INFO                           *port_info
  )
{ 
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA
    pp_port_tbl;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_conf_tbl;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    pinfo_flp_tbl;
  ARAD_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  SOC_PPC_TRAP_CODE_INTERNAL
    trap_code_internal;
  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA
    vsi_profiles_tbl;
  ARAD_SOC_REG_FIELD
    strength_fld_fwd,
    strength_fld_snp;
  uint32
    tmp;
  soc_reg_above_64_val_t
    reg_val; 
  uint8
    mac_in_mac_enabled=0;
  uint8
    vid_sa_base_enabled=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  
  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(unit, core_id, local_port_ndx, &pinfo_llr_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  
  pinfo_llr_tbl.default_action_profile_fwd = port_info->initial_action_profile.frwrd_action_strength;
  pinfo_llr_tbl.default_action_profile_snp = port_info->initial_action_profile.snoop_action_strength;
  
  res = arad_pp_trap_mgmt_trap_code_to_internal(
          unit,
          port_info->initial_action_profile.trap_code,
          &trap_code_internal,
          &strength_fld_fwd,
          &strength_fld_snp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  pinfo_llr_tbl.default_cpu_trap_code = trap_code_internal;

  if (!SOC_DPP_PP_ENABLE(unit) && (port_info->flags & SOC_PPC_PORT_IHP_PINFO_LLR_TBL)) {
      
      res = arad_pp_ihp_pinfo_llr_tbl_set_unsafe(unit, core_id, local_port_ndx, &pinfo_llr_tbl);
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  
  

  res = arad_pp_ihp_pp_port_info_tbl_get_unsafe(unit, core_id, local_port_ndx, &pp_port_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core_id, local_port_ndx, &vtt_in_pp_port_conf_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(unit, core_id, local_port_ndx, &pinfo_flp_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core_id, local_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);

  res = arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_get_unsafe(unit, core_id, local_port_ndx, &vsi_profiles_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  
  
  
  
  
  pinfo_llr_tbl.vlan_classification_profile = port_info->port_profile;
 
  
  res = arad_pp_is_mac_in_mac_enabled(unit, &mac_in_mac_enabled);
  if (res != SOC_SAND_OK)
    goto exit;
  if (mac_in_mac_enabled)
  {
    res = sw_state_access[unit].dpp.soc.arad.pp.llp_vid_assign.vid_sa_based_enable.bit_get(unit, local_port_ndx, &vid_sa_base_enabled);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    pinfo_llr_tbl.sa_lookup_enable = vid_sa_base_enabled || pinfo_llr_tbl.enable_sa_authentication;
    pinfo_flp_tbl.port_is_pbp = port_info->is_pbp;
  }
  
  
  vtt_in_pp_port_conf_tbl.vlan_translation_profile = port_info->vlan_translation_profile;
  epni_pp_pct_tbl.llvp_profile = port_info->port_profile_eg;

  
  vtt_in_pp_port_conf_tbl.termination_profile = port_info->tunnel_termination_profile;

  
  pp_port_tbl.tpid_profile = port_info->tpid_profile;

  
  pp_port_tbl.mpls_next_address_use_specualte = port_info->enable_mpls_speculative_parsing;
  
  vtt_in_pp_port_conf_tbl.vlan_domain = port_info->vlan_domain;
  epni_pp_pct_tbl.vlan_domain = port_info->vlan_domain;

  
  if (port_info->dflt_port_lif != 0) {
        vtt_in_pp_port_conf_tbl.default_port_lif = port_info->dflt_port_lif;
  }

  
  pinfo_flp_tbl.port_is_pbp = 
  epni_pp_pct_tbl.port_is_pbp =
  egq_pp_ppct_tbl.port_is_pbp =
      SOC_SAND_BOOL2NUM(port_info->is_pbp);
  if (port_info->is_pbp) {
      pinfo_flp_tbl.program_selection_profile |= ARAD_PP_FLP_PORT_PROFILE_PBP;       
  } else {
      pinfo_flp_tbl.program_selection_profile &= ~(ARAD_PP_FLP_PORT_PROFILE_PBP);
  }

  if (SOC_DPP_CONFIG(unit)->pp.mim_vsi_mode) {
     if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_two_ive_reserved", 0)) {
         epni_pp_pct_tbl.prge_var = epni_pp_pct_tbl.port_is_pbp * 0x7E; 
     }else{
         epni_pp_pct_tbl.prge_var = epni_pp_pct_tbl.port_is_pbp * 127;
     }
  }
  
  
  vtt_in_pp_port_conf_tbl.learn_lif = SOC_SAND_BOOL2NUM(port_info->is_learn_ac);

  

  
  pinfo_flp_tbl.learn_enable = SOC_SAND_BOOL2NUM(port_info->enable_learning);


  
  tmp = SOC_SAND_BOOL2NUM_INVERSE(port_info->enable_outgoing_learning);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_EGQ_OPP_LEARNING_DISABLEr(unit, REG_PORT_ANY, reg_val));
  soc_sand_bitstream_set_any_field(&tmp, local_port_ndx, 1, reg_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_EGQ_OPP_LEARNING_DISABLEr(unit, REG_PORT_ANY, reg_val));
  
  
  if (SOC_SAND_NUM2BOOL_INVERSE(port_info->same_interface_filter_skip)) 
  {
    
    pinfo_flp_tbl.enable_unicast_same_interface_filter =
    egq_pp_ppct_tbl.enable_unicast_same_interface_filter =
    egq_pp_ppct_tbl.enable_same_interface_filters_ports = 
        SOC_SAND_BOOL2NUM(port_info->enable_same_interfac_filter);
  }
  
  pinfo_llr_tbl.protocol_profile = port_info->ether_type_based_profile;

  pinfo_llr_tbl.non_authorized_mode_8021x = SOC_SAND_BOOL2NUM(port_info->not_authorized_802_1x);
  
  
  arad_pp_port_orientation2intern(port_info->orientation, &egq_pp_ppct_tbl.orientation);
  
  
  epni_pp_pct_tbl.cep_c_vlan_edit = (port_info->port_type == SOC_SAND_PP_PORT_L2_TYPE_CEP)?1:0;
     
  
  if(port_info->is_n_port){
      pinfo_flp_tbl.program_selection_profile |= ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT;
  }else{
      pinfo_flp_tbl.program_selection_profile &= ~(ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT);      
  }

  
  egq_pp_ppct_tbl.mtu = port_info->mtu;
  epni_pp_pct_tbl.mtu = port_info->mtu_encapsulated;

  
  pinfo_flp_tbl.action_profile_da_not_found_index = port_info->da_not_found_profile;
 
  
  epni_pp_pct_tbl.default_sem_result = port_info->dflt_egress_ac;

  
   vsi_profiles_tbl.ac_p2p_to_pbb_vsi_profile = port_info->ac_p2p_to_pbb_vsi_profile;
   vsi_profiles_tbl.label_pwe_p2p_vsi_profile = port_info->label_pwe_p2p_vsi_profile;
  
  
  
  
  if (port_info->flags & SOC_PPC_PORT_IHP_PP_PORT_INFO_TBL) {
    res = arad_pp_ihp_pp_port_info_tbl_set_unsafe(unit, core_id, local_port_ndx, &pp_port_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (port_info->flags & SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL) {
    res = arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(unit, core_id, local_port_ndx, &vtt_in_pp_port_conf_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
    
    res = sw_state_access[unit].dpp.soc.arad.pp.vt_profile_info.vlan_translation_profile.set(unit, vtt_in_pp_port_conf_tbl.vlan_domain, vtt_in_pp_port_conf_tbl.vlan_translation_profile);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
  }

  if (port_info->flags & SOC_PPC_PORT_IHP_PINFO_LLR_TBL) {
    res = arad_pp_ihp_pinfo_llr_tbl_set_unsafe(unit, core_id, local_port_ndx, &pinfo_llr_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
  }

  if (port_info->flags & SOC_PPC_PORT_IHB_PINFO_FLP_TBL) {
    res = arad_pp_ihb_pinfo_flp_tbl_set_unsafe(unit, core_id, local_port_ndx, &pinfo_flp_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
  }
  
  if (port_info->flags & SOC_PPC_PORT_EPNI_PP_PCT_TBL) {
    res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core_id, local_port_ndx, &epni_pp_pct_tbl);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (port_info->flags & SOC_PPC_PORT_EGQ_PP_PPCT_TBL) {
    res = arad_pp_egq_pp_ppct_tbl_set_unsafe(unit, core_id, local_port_ndx, &egq_pp_ppct_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

  if (port_info->flags & SOC_PPC_PORT_IHP_VTT_PP_PORT_VSI_PROFILE_TBL) {
    res = arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_set_unsafe(unit, core_id, local_port_ndx, &vsi_profiles_tbl);
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  }
 
  ARAD_PP_PON_DOUBLE_LOOKUP_VD_SET(port_info->vlan_domain, port_info->enable_pon_double_lookup);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_info_set_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_PORT_INFO_verify(unit, port_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_INFO_GET_VERIFY);
  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_info_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA
    pp_port_tbl;
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_in_pp_port_conf_tbl;
  ARAD_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    pinfo_flp_tbl;
  ARAD_PP_EGQ_PP_PPCT_TBL_DATA
    egq_pp_ppct_tbl;
  ARAD_PP_EPNI_PP_PCT_TBL_DATA
    epni_pp_pct_tbl;
  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA
    vsi_profiles_tbl;
  uint32
    tmp = 0;
  uint8
    pon_double_lookup_enabled=0;
  soc_reg_above_64_val_t
    reg_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_INFO_SET_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_REG_ABOVE_64_CLEAR(reg_val);

  SOC_PPC_PORT_INFO_clear(port_info);

  
  res = arad_pp_ihp_pinfo_llr_tbl_get_unsafe(unit, core_id, local_port_ndx, &pinfo_llr_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  
  port_info->initial_action_profile.frwrd_action_strength = pinfo_llr_tbl.default_action_profile_fwd;
  port_info->initial_action_profile.snoop_action_strength = pinfo_llr_tbl.default_action_profile_snp;
  res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(
          unit,
          pinfo_llr_tbl.default_cpu_trap_code,
          &(port_info->initial_action_profile.trap_code)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  if (!SOC_DPP_PP_ENABLE(unit)) {
      
       ARAD_DO_NOTHING_AND_EXIT;
  }

  
  
  
  res = arad_pp_ihp_pp_port_info_tbl_get_unsafe(unit, core_id, local_port_ndx, &pp_port_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(unit, core_id, local_port_ndx, &vtt_in_pp_port_conf_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(unit, core_id, local_port_ndx, &pinfo_flp_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = arad_pp_egq_pp_ppct_tbl_get_unsafe(unit, core_id, local_port_ndx, &egq_pp_ppct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core_id, local_port_ndx, &epni_pp_pct_tbl);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);

  res = arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_get_unsafe(unit, core_id, local_port_ndx, &vsi_profiles_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  
  
  
   
  
  port_info->port_profile = pinfo_llr_tbl.vlan_classification_profile;
  port_info->port_profile_eg  = epni_pp_pct_tbl.llvp_profile;

  
  port_info->tpid_profile = pp_port_tbl.tpid_profile;

  
  port_info->enable_mpls_speculative_parsing = pp_port_tbl.mpls_next_address_use_specualte;

  
  port_info->vlan_domain = vtt_in_pp_port_conf_tbl.vlan_domain;
  ARAD_PP_PON_DOUBLE_LOOKUP_VD_GET(port_info->vlan_domain, pon_double_lookup_enabled);
  
  
  port_info->is_pbp = SOC_SAND_NUM2BOOL(epni_pp_pct_tbl.port_is_pbp);

  
  port_info->is_n_port = ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT & pinfo_flp_tbl.program_selection_profile;
  
  
  port_info->is_learn_ac = SOC_SAND_NUM2BOOL(vtt_in_pp_port_conf_tbl.learn_lif);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EGQ_OPP_LEARNING_DISABLEr(unit, REG_PORT_ANY, reg_val));
  SHR_BITCOPY_RANGE(&tmp, 0 , reg_val, local_port_ndx, 1);
  
  port_info->enable_learning = pinfo_flp_tbl.learn_enable;
  port_info->enable_outgoing_learning = SOC_SAND_NUM2BOOL_INVERSE(tmp);
  
  
  
  port_info->enable_same_interfac_filter = SOC_SAND_NUM2BOOL(pinfo_flp_tbl.enable_unicast_same_interface_filter);
  if (pinfo_flp_tbl.enable_unicast_same_interface_filter != egq_pp_ppct_tbl.enable_unicast_same_interface_filter) 
  {
    
    port_info->same_interface_filter_skip = 1;
  }

  
  port_info->ether_type_based_profile = pinfo_llr_tbl.protocol_profile;

  port_info->not_authorized_802_1x = SOC_SAND_NUM2BOOL(pinfo_llr_tbl.non_authorized_mode_8021x);
  
  
  arad_pp_port_intern2orientation(egq_pp_ppct_tbl.orientation, &port_info->orientation);
  
  
  port_info->port_type = (epni_pp_pct_tbl.cep_c_vlan_edit == 1)?SOC_SAND_PP_PORT_L2_TYPE_CEP:SOC_SAND_PP_PORT_L2_TYPE_VBP;
     
  
  

  
  port_info->mtu = egq_pp_ppct_tbl.mtu; 
  port_info->mtu_encapsulated = epni_pp_pct_tbl.mtu;

  
  port_info->da_not_found_profile = pinfo_flp_tbl.action_profile_da_not_found_index;

  
  port_info->dflt_egress_ac = epni_pp_pct_tbl.default_sem_result;

  
  port_info->vlan_translation_profile = vtt_in_pp_port_conf_tbl.vlan_translation_profile;

  
  port_info->tunnel_termination_profile = vtt_in_pp_port_conf_tbl.termination_profile;

  
   port_info->ac_p2p_to_pbb_vsi_profile = vsi_profiles_tbl.ac_p2p_to_pbb_vsi_profile;
   port_info->label_pwe_p2p_vsi_profile = vsi_profiles_tbl.label_pwe_p2p_vsi_profile;

   
   port_info->enable_pon_double_lookup = SOC_SAND_NUM2BOOL(pon_double_lookup_enabled);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_info_get_unsafe()", local_port_ndx, 0);
}



uint32
  arad_pp_port_stp_state_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  SOC_PPC_PORT             local_port_ndx,
    SOC_SAND_IN  uint32                 topology_id_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE   stp_state
  )
{
  uint32
    res = SOC_SAND_OK,
    eg_stp_state_bit,
    fld_val = 0;
  ARAD_PP_IHP_STP_TABLE_TBL_DATA
    ing_stp_tbl_data;
  ARAD_PP_EPNI_STP_TBL_DATA
    eg_stp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_STP_STATE_SET_UNSAFE);

  res = arad_pp_ihp_stp_table_tbl_get_unsafe(unit, core_id, local_port_ndx, &ing_stp_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch (stp_state)
  {
  case SOC_PPC_PORT_STP_STATE_BLOCK:
    fld_val = SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK;
    break;
  case SOC_PPC_PORT_STP_STATE_LEARN:
    fld_val = SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN;
    break;
  case SOC_PPC_PORT_STP_STATE_FORWARD:
    fld_val = SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD;
    break;
  default:
    SOC_SAND_ERR_IF_ABOVE_MAX(stp_state, SOC_PPC_PORT_STP_STATE_MAX, SOC_PPC_PORT_STP_STATE_OUT_OF_RANGE_ERR, 15, exit);
  }

  SHR_BITCOPY_RANGE(ing_stp_tbl_data.per_port_stp_state, 2*topology_id_ndx, &fld_val, 0, 2); 

  res = arad_pp_ihp_stp_table_tbl_set_unsafe(unit, core_id, local_port_ndx, &ing_stp_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  eg_stp_state_bit = (stp_state == SOC_PPC_PORT_STP_STATE_FORWARD) ? SOC_PPC_PORT_STP_STATE_EG_FLD_VAL_FORWARD : SOC_PPC_PORT_STP_STATE_EG_FLD_VAL_BLOCK;

  res = arad_pp_epni_stp_tbl_get_unsafe(unit, core_id, topology_id_ndx, &eg_stp_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SHR_BITCOPY_RANGE(eg_stp_tbl_data.stp, local_port_ndx, &eg_stp_state_bit, 0, 1);

  res = arad_pp_epni_stp_tbl_set_unsafe(unit, core_id, topology_id_ndx, &eg_stp_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_stp_state_set_unsafe()", local_port_ndx, topology_id_ndx);
}

uint32
  arad_pp_port_stp_state_set_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT            local_port_ndx,
    SOC_SAND_IN  uint32                topology_id_ndx,
    SOC_SAND_IN  SOC_PPC_PORT_STP_STATE  stp_state
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_STP_STATE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(topology_id_ndx, SOC_DPP_DEFS_GET(unit, nof_topology_ids), SOC_PPC_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(stp_state, SOC_PPC_PORT_STP_STATE_MAX, SOC_PPC_PORT_STP_STATE_OUT_OF_RANGE_ERR, 30, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_stp_state_set_verify()", local_port_ndx, topology_id_ndx);
}

uint32
  arad_pp_port_stp_state_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT            local_port_ndx,
    SOC_SAND_IN  uint32                topology_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_STP_STATE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(topology_id_ndx, SOC_DPP_DEFS_GET(unit, nof_topology_ids), SOC_PPC_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_stp_state_get_verify()", local_port_ndx, topology_id_ndx);
}


uint32
  arad_pp_port_stp_state_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  SOC_PPC_PORT           local_port_ndx,
    SOC_SAND_IN  uint32               topology_id_ndx,
    SOC_SAND_OUT SOC_PPC_PORT_STP_STATE *stp_state
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val=0;
  ARAD_PP_IHP_STP_TABLE_TBL_DATA
    ing_stp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_PORT_STP_STATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(stp_state);

  res = arad_pp_ihp_stp_table_tbl_get_unsafe(unit, core_id, local_port_ndx, &ing_stp_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SHR_BITCOPY_RANGE(&fld_val, 0, ing_stp_tbl_data.per_port_stp_state, 2*topology_id_ndx, 2);

  switch (fld_val)
  {
  case SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK:
     *stp_state = SOC_PPC_PORT_STP_STATE_BLOCK;
    break;
  case SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN:
    *stp_state = SOC_PPC_PORT_STP_STATE_LEARN;
    break;
  case SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD:
    *stp_state = SOC_PPC_PORT_STP_STATE_FORWARD;
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_port_stp_state_get_unsafe()", local_port_ndx, topology_id_ndx);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_port_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_port;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_port_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_port;
}

uint32
  SOC_PPC_PORT_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, &(info->initial_action_profile), 14, exit);
  if (!SOC_DPP_PP_ENABLE(unit)) {
       ARAD_DO_NOTHING_AND_EXIT;
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_profile, SOC_PPC_PORT_PORT_PROFILE_MAX, SOC_PPC_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_TPID_PROFILE_MAX, SOC_PPC_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->vlan_domain, SOC_DPP_DEFS_GET(unit, nof_vlan_domains), SOC_PPC_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_type, SOC_PPC_PORT_PORT_TYPE_MAX, SOC_PPC_PORT_PORT_TYPE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PPC_PORT_ORIENTATION_MAX, SOC_PPC_PORT_ORIENTATION_OUT_OF_RANGE_ERR, 19, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ether_type_based_profile, SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_MAX, SOC_PPC_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR, 20, exit);
   
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mtu, SOC_PPC_PORT_MTU_MAX, SOC_PPC_PORT_MTU_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mtu_encapsulated, SOC_PPC_PORT_MTU_ENCAP_MAX, SOC_PPC_PORT_MTU_OUT_OF_RANGE_ERR, 24, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_not_found_profile, SOC_PPC_PORT_DA_NOT_FOUND_PROFILE_MAX, SOC_PPC_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR, 25, exit);
  ARAD_PP_EG_ENCAP_CHECK_OUTLIF_ID(unit, info->dflt_egress_ac, SOC_PPC_AC_ID_OUT_OF_RANGE_ERR, 26, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_translation_profile, SOC_PPC_PORT_VLAN_TRANSLATION_MAX, SOC_PPC_PORT_VT_PROFILE_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ac_p2p_to_pbb_vsi_profile, SOC_PPC_PORT_VSI_PROFILE_MAX, SOC_PPC_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->label_pwe_p2p_vsi_profile, SOC_PPC_PORT_VSI_PROFILE_MAX, SOC_PPC_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR, 60, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_PORT_INFO_verify()",0,0);
}



soc_error_t 
arad_pp_port_property_set(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value){
    uint32 data;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;    

    
    switch (port_property) {
    case soc_ppc_port_property_vlan_translation:
        
        rv = READ_IHP_VTT_PP_PORT_VT_KEY_VARm(unit, IHP_BLOCK(unit, core), port, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_SET(data, value);

        
        rv = WRITE_IHP_VTT_PP_PORT_VT_KEY_VARm(unit, IHP_BLOCK(unit, core), port, &data);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case soc_ppc_port_property_tunnel_termination:
        
        rv = READ_IHP_VTT_PP_PORT_TT_KEY_VARm(unit, IHP_BLOCK(unit, core), port, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_SET(data, value);

        rv = WRITE_IHP_VTT_PP_PORT_TT_KEY_VARm(unit, IHP_BLOCK(unit, core), port,&data);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown port property")));
    }        

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t arad_pp_port_property_get(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value){
    uint32 data;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;    

    
    switch (port_property) {
    case soc_ppc_port_property_vlan_translation:
      
        rv = READ_IHP_VTT_PP_PORT_VT_KEY_VARm(unit, IHP_BLOCK(unit, core), port, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        *value = SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_GET(data);
        break;

        case soc_ppc_port_property_tunnel_termination:
      
        rv = READ_IHP_VTT_PP_PORT_TT_KEY_VARm(unit, IHP_BLOCK(unit, core), port, &data);
        SOCDNX_IF_ERR_EXIT(rv);

        *value = SOC_PPC_PORT_PROPERTY_TUNNEL_TERMINATION_GET(data);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown port property")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t arad_pp_port_additional_tpids_set(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids)
{
    int rv;
    uint64 reg_64, field64;
    uint32 data32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
        COMPILER_64_ZERO(reg_64);
        COMPILER_64_SET(field64, 0, additional_tpids->tpid_vals[0]);
        
        soc_reg64_field_set(unit, IHP_TPID_ADDITIONAL_VALUESr, &reg_64, TPID_1_ADDITIONAL_VALUE_1f, field64);
        COMPILER_64_SET(field64, 0, additional_tpids->tpid_vals[1]);
        soc_reg64_field_set(unit, IHP_TPID_ADDITIONAL_VALUESr, &reg_64, TPID_1_ADDITIONAL_VALUE_2f, field64);
        COMPILER_64_SET(field64, 0, additional_tpids->tpid_vals[2]);
        soc_reg64_field_set(unit, IHP_TPID_ADDITIONAL_VALUESr, &reg_64, TPID_2_ADDITIONAL_VALUE_1f, field64);
        COMPILER_64_SET(field64, 0, additional_tpids->tpid_vals[3]);
        soc_reg64_field_set(unit, IHP_TPID_ADDITIONAL_VALUESr, &reg_64, TPID_2_ADDITIONAL_VALUE_2f, field64);
        
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_TPID_ADDITIONAL_VALUESr(unit, SOC_CORE_ALL, reg_64));
    } else {
        soc_mem_field32_set(unit, IHP_PARSER_ETH_PROTOCOLSm, &data32, ETH_TYPE_PROTOCOLf, additional_tpids->tpid_vals[0]);
        
        rv = WRITE_IHP_PARSER_ETH_PROTOCOLSm(unit, MEM_BLOCK_ANY, ARAD_PARSER_ETHER_PROTO_4_ADD_TPID, &data32);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}





soc_error_t arad_pp_port_additional_tpids_get(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids)
{
    int rv;
    uint64 reg_64;
    uint32 data32 = 0;
    int core_id = 0;

    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) {
        COMPILER_64_ZERO(reg_64);
        SOCDNX_IF_ERR_EXIT(READ_IHP_TPID_ADDITIONAL_VALUESr(unit, core_id, &reg_64));
        additional_tpids->tpid_vals[0] = (SOC_SAND_PP_TPID)soc_reg64_field32_get(unit, IHP_TPID_ADDITIONAL_VALUESr, reg_64, TPID_1_ADDITIONAL_VALUE_1f);
        additional_tpids->tpid_vals[1] = (SOC_SAND_PP_TPID)soc_reg64_field32_get(unit, IHP_TPID_ADDITIONAL_VALUESr, reg_64, TPID_1_ADDITIONAL_VALUE_2f);
        additional_tpids->tpid_vals[2] = (SOC_SAND_PP_TPID)soc_reg64_field32_get(unit, IHP_TPID_ADDITIONAL_VALUESr, reg_64, TPID_2_ADDITIONAL_VALUE_1f);
        additional_tpids->tpid_vals[3] = (SOC_SAND_PP_TPID)soc_reg64_field32_get(unit, IHP_TPID_ADDITIONAL_VALUESr, reg_64, TPID_2_ADDITIONAL_VALUE_2f);
    } else {
        rv = READ_IHP_PARSER_ETH_PROTOCOLSm(unit, MEM_BLOCK_ANY, ARAD_PARSER_ETHER_PROTO_4_ADD_TPID, &data32);
        SOCDNX_IF_ERR_EXIT(rv);
        
        additional_tpids->tpid_vals[0] = (SOC_SAND_PP_TPID)soc_mem_field32_get(unit, IHP_PARSER_ETH_PROTOCOLSm, &data32, ETH_TYPE_PROTOCOLf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
