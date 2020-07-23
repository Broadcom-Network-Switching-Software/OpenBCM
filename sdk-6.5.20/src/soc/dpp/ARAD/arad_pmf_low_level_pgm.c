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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP



#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/mem.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_pmf_low_level_pgm.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
 
#include <soc/dpp/JER/JER_PP/jer_pp_lbp_init.h>





#define ARAD_PORTS_FC_TYPE_NONE_FLD_VAL   (0)
#define ARAD_PORTS_FC_TYPE_LL_FLD_VAL     (1)
#define ARAD_PORTS_FC_TYPE_CB2_FLD_VAL    (2)
#define ARAD_PORTS_FC_TYPE_CB8_FLD_VAL    (3)

#define ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_FC_TYPE_LSB         (1)
#define ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_FC_TYPE_MSB         (2)

#define ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_PP_PROCESSING   (0)
#define ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_ITMH            (9)
#define ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_TM_MC_FLOW      (8)


#define ARAD_PMF_PPH_TYPE_NO_PPH             (0)

#define ARAD_PMF_IHB_PROGRAM_KEY_GEN_VAR_LENGTH    (2)


#define ARAD_PMF_NOF_PGM_HEADER_PROFILE_NOT_PPS    (9)
#define ARAD_PMF_NOF_PGM_HEADER_PROFILE    (16)

#define ARAD_PMF_NOF_PGM_TM_HEADER_PROFILE (8)







#define ARAD_PMF_LOW_LEVEL_USER_HEADER_SIZE_ENCODE(user_header_size) (user_header_size / 8)



















uint32
  arad_pmf_low_level_pgm_header_profile_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PORTS_FC_TYPE                fc_type,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE              header_type,
    SOC_SAND_IN  uint8                             is_itmh_mc_flow,
    SOC_SAND_OUT uint32                            *header_profile,
    SOC_SAND_OUT uint32                            *internal_profile
  )
{
  uint32
      header_profile_build_ftmh,
      header_profile_learn_disable,
      header_profile_st_vsq_ptr_tc_mode,
      header_profile_build_pph,
      header_profile_always_add_pph_learn_ext,
      out_lif_in_user_header_2,
      mc_id_in_user_header_2,
      user_header_2_size,
      fld_val = 0;
  uint8
      is_itmh_mc_flow_lcl = FALSE,
      is_tdm = FALSE,
      is_arad_rev_a_in_system,
      is_pp_profile = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_SET);

  *header_profile = 0;
  *internal_profile = 0;

  switch (header_type)
  {
  case ARAD_PORT_HEADER_TYPE_NONE:
  case ARAD_PORT_HEADER_TYPE_RAW:
  case ARAD_PORT_HEADER_TYPE_MIRROR_RAW:
  case ARAD_PORT_HEADER_TYPE_TM:
  case ARAD_PORT_HEADER_TYPE_PROG: 
      is_itmh_mc_flow_lcl = is_itmh_mc_flow;
      is_pp_profile = FALSE;
      break;

  case ARAD_PORT_HEADER_TYPE_ETH:
  case ARAD_PORT_HEADER_TYPE_MPLS_RAW:
    is_pp_profile = TRUE;
    break;

  case ARAD_PORT_HEADER_TYPE_STACKING:
  case ARAD_PORT_HEADER_TYPE_TDM:
    is_tdm = TRUE;
    break;

  case ARAD_PORT_HEADER_TYPE_CPU:
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 15, exit);
  }

  if (is_tdm == TRUE)
  {
    
    header_profile_build_ftmh = FALSE;
    header_profile_learn_disable = FALSE;
    header_profile_st_vsq_ptr_tc_mode = 0;
    header_profile_build_pph = FALSE;
    header_profile_always_add_pph_learn_ext = FALSE;
  }
  else
  {
    header_profile_build_ftmh = TRUE;
    header_profile_learn_disable = FALSE;
    if (is_pp_profile == TRUE)
    {
      
      is_arad_rev_a_in_system = FALSE; 
      
      
      header_profile_always_add_pph_learn_ext = !is_arad_rev_a_in_system;
      header_profile_build_pph = TRUE;
    }
    else
    {
      header_profile_learn_disable = TRUE;
      header_profile_build_pph = FALSE;
      header_profile_always_add_pph_learn_ext = FALSE;
    }
  }


  
  switch (fc_type)
  {
  case ARAD_PORTS_FC_TYPE_NONE:
    fld_val = ARAD_PORTS_FC_TYPE_NONE_FLD_VAL;
  	break;
  case ARAD_PORTS_FC_TYPE_LL:
    fld_val = ARAD_PORTS_FC_TYPE_LL_FLD_VAL;
    break;
  case ARAD_PORTS_FC_TYPE_CB2:
    fld_val = ARAD_PORTS_FC_TYPE_CB2_FLD_VAL;
    break;
  case ARAD_PORTS_FC_TYPE_CB8:
    fld_val = ARAD_PORTS_FC_TYPE_CB8_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  header_profile_st_vsq_ptr_tc_mode = fld_val;

  user_header_2_size = 0;
  mc_id_in_user_header_2 = 0;
  out_lif_in_user_header_2 = 0;
  if (is_itmh_mc_flow_lcl) {
	  user_header_2_size = 0; 
	  mc_id_in_user_header_2 = TRUE;
	  out_lif_in_user_header_2 = TRUE;
  }

  
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, BUILD_FTMHf, header_profile_build_ftmh);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, NEVER_ADD_PPH_LEARN_EXTf, header_profile_learn_disable);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, ST_VSQ_PTR_TC_MODEf, header_profile_st_vsq_ptr_tc_mode);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, BUILD_PPHf, header_profile_build_pph);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, header_profile_always_add_pph_learn_ext);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, USER_HEADER_2_SIZEf, user_header_2_size);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, MC_ID_IN_USER_HEADER_2f, mc_id_in_user_header_2);
  soc_mem_field32_set(unit, IHB_HEADER_PROFILEm, header_profile, OUT_LIF_IN_USER_HEADER_2f, out_lif_in_user_header_2);

  
  *internal_profile = 0;
  if (is_pp_profile) {
	  *internal_profile = ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_PP_PROCESSING;
  }
  else if (is_tdm) {
	  *internal_profile = ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_PP_PROCESSING;
  }
  else if (is_itmh_mc_flow_lcl) {
	  *internal_profile = ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_TM_MC_FLOW;
  }
  else {
	  *internal_profile = ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_ITMH;
  }
  *internal_profile |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_FC_TYPE_MSB, ARAD_PMF_PGM_HEADER_PROFILE_ENCODED_FC_TYPE_LSB);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_pgm_header_profile_set()", 0, 0);
}

uint32
  arad_pmf_low_level_pgm_header_type_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           internal_profile_ndx,
    SOC_SAND_OUT ARAD_PORTS_FC_TYPE                  *fc_type,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE               *header_type
  )
{
  uint32
    res,
    header_profile_tbl_data,
    header_profile_build_ftmh,
    header_profile_st_vsq_ptr_tc_mode,
    header_profile_build_pph,
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_LOW_LEVEL_PGM_HEADER_TYPE_GET);

  
  res = soc_mem_read(
          unit,
          IHB_HEADER_PROFILEm,
          MEM_BLOCK_ANY,
          internal_profile_ndx,
          &header_profile_tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  header_profile_build_ftmh = soc_mem_field32_get(unit, IHB_HEADER_PROFILEm, &header_profile_tbl_data, BUILD_FTMHf);
  header_profile_st_vsq_ptr_tc_mode = soc_mem_field32_get(unit, IHB_HEADER_PROFILEm, &header_profile_tbl_data, ST_VSQ_PTR_TC_MODEf);
  header_profile_build_pph = soc_mem_field32_get(unit, IHB_HEADER_PROFILEm, &header_profile_tbl_data, BUILD_PPHf);

  if (header_profile_build_ftmh == FALSE)
  {
    *header_type = ARAD_PORT_HEADER_TYPE_TDM;
  }
  else if (header_profile_build_pph == TRUE)
  {
    *header_type = ARAD_PORT_HEADER_TYPE_ETH;
  }
  else 
  {
    *header_type = ARAD_PORT_HEADER_TYPE_TM;
  }

  
  switch (header_profile_st_vsq_ptr_tc_mode)
  {
  case ARAD_PORTS_FC_TYPE_NONE_FLD_VAL:
    fld_val = ARAD_PORTS_FC_TYPE_NONE;
  	break;
  case ARAD_PORTS_FC_TYPE_LL_FLD_VAL:
    fld_val = ARAD_PORTS_FC_TYPE_LL;
    break;
  case ARAD_PORTS_FC_TYPE_CB2_FLD_VAL:
    fld_val = ARAD_PORTS_FC_TYPE_CB2;
    break;
  case ARAD_PORTS_FC_TYPE_CB8_FLD_VAL:
    fld_val = ARAD_PORTS_FC_TYPE_CB8;
    break;
  default:
    LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "Unit %d Iternal profile index %d - Failed to get header type. Got invalid flow control type.\n\r"),
               unit, internal_profile_ndx));
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 30, exit);
  }

  *fc_type = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_pgm_header_type_get()", 0, 0);
}

uint32
 arad_pmf_low_level_pgm_counter_set_unsafe(
   SOC_SAND_IN  int                      unit,
   SOC_SAND_IN  int                      command_id,
   SOC_SAND_IN  SOC_TMC_CNT_MODE_EG_TYPE eg_type,
   SOC_SAND_IN  int                      counter_base
   ) 
{
    uint32      
        field_value = 0x1;
    soc_reg_above_64_val_t 
        tbl_data_above_64;
    uint32 
        res = SOC_SAND_OK;
    int 
        pmf_pgm_ndx;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_TM) {
        field_value = 0x0;
    } else if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_VSI) {
        field_value = 0x1;
    } else if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
        field_value = 0x2;
    } else if (eg_type == SOC_TMC_CNT_MODE_EG_TYPE_PMF) {
        field_value = 0x3;
    }
    
    for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); pmf_pgm_ndx++) {
        SOC_REG_ABOVE_64_CLEAR(tbl_data_above_64);
        res = READ_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_ndx, tbl_data_above_64);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        if (command_id == 0) {
            soc_IHB_PMF_PROGRAM_COUNTERSm_field32_set(unit, tbl_data_above_64, COUNTER_MODE_Af, field_value);
            soc_IHB_PMF_PROGRAM_COUNTERSm_field32_set(unit, tbl_data_above_64, COUNTER_BASE_Af, counter_base);
        } else {
            soc_IHB_PMF_PROGRAM_COUNTERSm_field32_set(unit, tbl_data_above_64, COUNTER_MODE_Bf, field_value);
            soc_IHB_PMF_PROGRAM_COUNTERSm_field32_set(unit, tbl_data_above_64, COUNTER_BASE_Bf, counter_base);
        }
        res = WRITE_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, pmf_pgm_ndx, tbl_data_above_64);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_pgm_counter_set_unsafe()", 0, 0);
}
uint32
 arad_pmf_low_level_pgm_counter_get_unsafe(
   SOC_SAND_IN  int                         unit,
   SOC_SAND_IN  int                         command_id,
   SOC_SAND_OUT SOC_TMC_CNT_MODE_EG_TYPE    *eg_type,
   SOC_SAND_OUT int                         *counter_base) 
{
    uint32 
        res = SOC_SAND_OK;
    soc_reg_above_64_val_t 
        tbl_data_above_64;
    uint32      
        field_value = 0x1;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(tbl_data_above_64);
    
    res = READ_IHB_PMF_PROGRAM_COUNTERSm(unit, MEM_BLOCK_ANY, 0, tbl_data_above_64);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (command_id == 0) {
        field_value = soc_IHB_PMF_PROGRAM_COUNTERSm_field32_get(unit, tbl_data_above_64, COUNTER_MODE_Af);
        *counter_base = soc_IHB_PMF_PROGRAM_COUNTERSm_field32_get(unit, tbl_data_above_64, COUNTER_BASE_Af);
    } else if (command_id == 1){
        field_value = soc_IHB_PMF_PROGRAM_COUNTERSm_field32_get(unit, tbl_data_above_64, COUNTER_MODE_Bf);
        *counter_base = soc_IHB_PMF_PROGRAM_COUNTERSm_field32_get(unit, tbl_data_above_64, COUNTER_BASE_Bf);
    } else {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 20, exit);
    }
    if (field_value == 0x1) {
        *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
    } else if (field_value == 0x2) {
        *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
    } else if (field_value == 0x3) {
        *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_pgm_counter_get_unsafe()", 0, 0);

}

uint32
  arad_pmf_low_level_pgm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
      user_header_0_size,
      user_header_1_size,
      user_header_egress_pmf_offset_0,
      user_header_egress_pmf_offset_1,
      header_profile_build_ftmh,
      mc_id_in_user_header_2,
      outlif_in_user_header_2,
      user_header_size[2]={0, 0},
      internal_profile,
      header_profile,
      res = SOC_SAND_OK,
      i,
      val,
      fhei_fwd_size = 0,
      fhei_eei_size = 0,
      routed = 0,
      pph_vsi_source = 0xffff,
      pph_eep_vsi_source = 0xffff,
      eei_swap_command  = 0x9,
      system_header_mode;
  soc_reg_above_64_val_t 
      reg_above64_val,
      fhei_ive_size,
      fhei_fwd_size_above64;
  soc_reg_t
    reg;
  uint64
      eei_pop_command;
  uint8
      always_add_pph_learn_ext = 0,
      never_add_pph_learn_ext =0;
      
  int16
      nof_vlan_edit_command_ids;
  int
      nof_reserved_ive_commands;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_LOW_LEVEL_PGM_INIT_UNSAFE);

  
  res = soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr), REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf, &system_header_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

  
  res = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0,
            &user_header_egress_pmf_offset_1
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  user_header_size[0] = ARAD_PMF_LOW_LEVEL_USER_HEADER_SIZE_ENCODE(user_header_0_size); 
  user_header_size[1] = ARAD_PMF_LOW_LEVEL_USER_HEADER_SIZE_ENCODE(user_header_1_size); 

#ifdef BCM_88660
  
  if (SOC_IS_ARADPLUS((unit))) {
      if (soc_property_get((unit), spn_SCHEDULER_COMPENSATION_ENABLE, 0)) {
          if (user_header_size[0] == 0) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d User-Header size not valid for scheduler compensation.\n\r"), unit));
              SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR, 11, exit);
          }
          else {
              if (SOC_IS_QAX(unit)) {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, UDH_MODEf,  1));
              } else {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, SCHEDULER_PACKET_SIZE_COMPENSATIONf,  1));
              }

              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_CFG_SCHEDULER_PKT_SZ_COMP_ENABLEr, SOC_CORE_ALL, 0, CFG_SCHEDULER_PKT_SZ_COMP_ENABLEf,  1));
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_CFG_SCHEDULER_PKT_SZ_COMP_ENABLEr, SOC_CORE_ALL, 0, CFG_SCHEDULER_PKT_SZ_COMP_ENABLEf,  1));
          }
      }
  }

   
  if (SOC_IS_ARADPLUS((unit))) {
      if (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH) {
          if (user_header_size[1] == 0) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d Null User-Header-2 size not valid for 16b full-hash LB-Key in FTMH.\n\r"), unit));
              SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR, 12, exit);
          }

          
          if (SOC_DPP_CONFIG(unit)->arad->init.fabric.trunk_hash_format == ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d: not valid to pass 16 bits and duplicate one byte of LB-Key in FTMH.\n\r"), unit));
              SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR, 13, exit);
          }
      }
  }
#endif 

#ifdef BCM_88650_B0
  
  if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_PP_CONFIGr, SOC_CORE_ALL, 0, NETWORK_OFFSETf,  ((user_header_0_size + user_header_1_size) / 8) ));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_PP_CONFIGr, SOC_CORE_ALL, 0, NETWORK_OFFSETf,  ((user_header_0_size + user_header_1_size) / 8) ));
  } else
#endif 
  {
      
      if (user_header_0_size + user_header_1_size) {
          SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR, 240, exit);
      }
  }

  if (SOC_IS_JERICHO(unit)) {
      if (soc_property_get((unit), spn_FIELD_EGRESS_ENABLE_IP_ACL_ON_BRIDGE, 0)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  17,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_ENABLE_IP_ACL_ON_BRIDGEr, SOC_CORE_ALL, 0, ENABLE_IP_ACL_ON_BRIDGEf,  1));
      }
  }

  
  for (internal_profile = 0; internal_profile <= ARAD_PMF_NOF_PGM_HEADER_PROFILE_NOT_PPS; ++internal_profile)
  {

      
      header_profile = 0;
      header_profile_build_ftmh = (internal_profile == 8)? 0x0:0x1; 
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_FTMHf, header_profile_build_ftmh);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, NEVER_ADD_PPH_LEARN_EXTf, 0x0);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ST_VSQ_PTR_TC_MODEf, 0x0); 
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_PPHf, 0x0);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, 0x0);      
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_1_SIZEf, header_profile_build_ftmh? user_header_size[0]: 0x0 );
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_2_SIZEf, header_profile_build_ftmh? user_header_size[1]: 0x0 );
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_MCf, 0x0);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_UCf, 0x0); 
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, FTMH_RESERVED_VALUEf, 0x0); 
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, OUT_LIF_IN_USER_HEADER_2f, 0x0);
      mc_id_in_user_header_2 = ((internal_profile <= 4) || (internal_profile == 6))? 0x1:0x0;
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, MC_ID_IN_USER_HEADER_2f, mc_id_in_user_header_2);
      outlif_in_user_header_2 = (internal_profile == 5)? 0x1:0x0;
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, OUT_LIF_IN_USER_HEADER_2f, outlif_in_user_header_2);
      if (internal_profile == ARAD_PMF_HEADER_PROFILE_OAM_TS_ITMH_USER_HEADER_WA) {
          
          soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_1_SIZEf, 0);
          soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_2_SIZEf, 0);
          soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, MC_ID_IN_USER_HEADER_2f, 0);
      }


      
      res = soc_mem_write(
          unit,
          IHB_HEADER_PROFILEm,
          MEM_BLOCK_ANY,
          internal_profile,
          &header_profile
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  
  }

  

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, IGNORE_MEM_ERRORf,  0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, DROP_IF_DEST_NOT_VALIDf,  1));
 
  if(SOC_IS_JERICHO(unit)) {
    reg = IHB_LBP_GENERAL_CONFIG_1r;
  } else {
    reg = IHB_LBP_GENERAL_CONFIG_2r;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, CREATE_FHEI_INSTEAD_OF_EEIf,  0x0300)); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, MPLS_POP_COMMANDf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, MPLS_SWAP_COMMANDf,  2));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, ORIENTATION_FROM_USER_HEADERf,  0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  82,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_FHEI_MPLS_COMMANDr, SOC_CORE_ALL, 0, FHEI_MPLS_COMMAND_POPf,  1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  87,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_FHEI_MPLS_COMMANDr, SOC_CORE_ALL, 0, FHEI_MPLS_COMMAND_SWAPf,  2));
 
  
  SOC_REG_ABOVE_64_CLEAR(fhei_ive_size);

  
  nof_vlan_edit_command_ids = SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit);
  if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
      
      val = 1;
      nof_reserved_ive_commands = SOC_DPP_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit);
      for (i = 0; i < nof_reserved_ive_commands; i++) {
          SHR_BITCOPY_RANGE(fhei_ive_size, i * 2, &val, 0, 2);
      }

      
      val = 2;
      for (i = nof_reserved_ive_commands; i < nof_vlan_edit_command_ids; i++) {
          SHR_BITCOPY_RANGE(fhei_ive_size, i * 2, &val, 0, 2);
      }
  }
  
  else {
      
      
      
      
      val = 1;
      if (SOC_DPP_CONFIG(unit)->pp.mim_vsi_mode) {
    	  val = 2;
      }
      for(i=0; i<=13; i++) { 
        SHR_BITCOPY_RANGE(fhei_ive_size, i*2, &val, 0, 2);
      }
      val = 2;
      for(i=14; i<64; i++) { 
          SHR_BITCOPY_RANGE(fhei_ive_size, i*2, &val, 0, 2);
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_IHB_PPH_FHEI_IVE_SIZEr(unit, REG_PORT_ANY, fhei_ive_size));

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      val = ARAD_IHB_PPH_FHEI_FWD_SIZE_3B; 
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_ETHERNET*2, &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_IPV4_UC*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_IPV4_MC*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_IPV6_UC*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_IPV6_MC*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_TRILL*2,    &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_CPU_TRAP*2, &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION*2, &val, 0, 2);
      val = ARAD_IHB_PPH_FHEI_FWD_SIZE_0B;
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_MPLS*2,     &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_CUSTOM1*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_CUSTOM2*2,  &val, 0, 2);
      SHR_BITCOPY_RANGE(&fhei_fwd_size, ARAD_PP_FWD_CODE_TM*2,       &val, 0, 2);

      SOC_REG_ABOVE_64_CLEAR(fhei_fwd_size_above64);
      SOC_REG_ABOVE_64_WORD_SET(fhei_fwd_size_above64, fhei_fwd_size, 0);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_IHB_PPH_FHEI_FWD_SIZEr(unit, REG_PORT_ANY, fhei_fwd_size_above64));
  } else if (SOC_IS_JERICHO(unit)) {
      res = soc_jer_php_fhei_fwd_size_init(unit); 

  }

  
  val = 1;
  SHR_BITCOPY_RANGE(&fhei_eei_size, 8*2, &val, 0, 2);
  val = 2;
  SHR_BITCOPY_RANGE(&fhei_eei_size, 9*2, &val, 0, 2);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_IHB_PPH_FHEI_EEI_SIZEr(unit, REG_PORT_ANY, fhei_eei_size));

  

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)
      || (SOC_IS_JERICHO(unit) && (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_ARAD))) {
      val = 0;
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_ETHERNET, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_TRILL, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_CPU_TRAP, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_CUSTOM1, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_CUSTOM2, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_TM, &val, 0, 1);
      val = 1;
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV4_UC, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV4_MC, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV6_UC, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV6_MC, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_MPLS, &val, 0, 1);
  } 
  
  else if (SOC_IS_JERICHO(unit) && (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO)) {
      val = 1;
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV4_UC, &val, 0, 1);
      SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV6_UC, &val, 0, 1);
      if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
          SHR_BITCOPY_RANGE(&routed, ARAD_PP_FWD_CODE_IPV6_MC, &val, 0, 1);
      }
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit) ) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, ROUTEDf,  routed));
  } else if (SOC_IS_JERICHO(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  125,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_1r, SOC_CORE_ALL, 0, ROUTED_IN_RIFf,  routed));
  }

  
  header_profile = 0;
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ST_VSQ_PTR_TC_MODEf, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_FTMHf, 1);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_PPHf, 1);
  
  if ((!SOC_IS_ARADPLUS(unit)) 
      || (SOC_IS_ARADPLUS(unit) && (SOC_IS_NEXT_HOP_MAC_EXT_ARAD_COMPATIBLE(unit))))
  {
    
    always_add_pph_learn_ext = (SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable);  
  }
  else
  {
    always_add_pph_learn_ext = 0;
  }

  
  always_add_pph_learn_ext |= ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cascaded_eg_learning",0) ) ? 0x1 : 0x0;

  
  never_add_pph_learn_ext = ( SOC_DPP_CONFIG(unit)->arad->init.ports.pph_learn_extension_disable) ? 0x1 : 0x0;

  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, always_add_pph_learn_ext);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, NEVER_ADD_PPH_LEARN_EXTf, never_add_pph_learn_ext);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_UCf, 0); 
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_MCf, 0);
  
    
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_1_SIZEf, user_header_size[0]);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_2_SIZEf, user_header_size[1]);
  
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, OUT_LIF_IN_USER_HEADER_2f, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, MC_ID_IN_USER_HEADER_2f, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, FTMH_RESERVED_VALUEf, 0); 

  res = soc_mem_write(
      unit, 
      IHB_HEADER_PROFILEm,
      MEM_BLOCK_ANY,
      ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET,
      &header_profile
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

  
  header_profile = 0;
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ST_VSQ_PTR_TC_MODEf, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_FTMHf, 1);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_PPHf, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, NEVER_ADD_PPH_LEARN_EXTf, 1);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_UCf, 0); 
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_MCf, 0);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_1_SIZEf,
          soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0)? 0 : user_header_size[0]);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_2_SIZEf,
          soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0)? 0 : user_header_size[1]);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, OUT_LIF_IN_USER_HEADER_2f, 0);
  if(SOC_IS_JERICHO_PLUS(unit))
  {
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, MC_ID_IN_USER_HEADER_2f, 0);
  } else {
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, MC_ID_IN_USER_HEADER_2f, 1);
  }
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, FTMH_RESERVED_VALUEf, 0); 

  res = soc_mem_write(
      unit, 
      IHB_HEADER_PROFILEm,
      MEM_BLOCK_ANY,
      11,
      &header_profile
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);

   
  header_profile = 0;

  res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET, &header_profile);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 145, exit);

  
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_UCf, 0x1); 
          
  
  res = soc_mem_write(
      unit, 
      IHB_HEADER_PROFILEm,
      MEM_BLOCK_ANY,
      ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET_DSP,
      &header_profile
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

     
  header_profile = 0;

  res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET, &header_profile);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 145, exit);

  
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, 0x1); 
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, NEVER_ADD_PPH_LEARN_EXTf, 0x0);
          
  
  res = soc_mem_write(
      unit, 
      IHB_HEADER_PROFILEm,
      MEM_BLOCK_ANY,
      ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN,
      &header_profile
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 156, exit);

   
  if(SOC_IS_JERICHO(unit))
  {
      header_profile = 0;
      
      res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET, &header_profile);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 147, exit);
      
      
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, BUILD_OAM_TS_HEADERf, 0x1); 
            
      
      res = soc_mem_write(
        unit, 
        IHB_HEADER_PROFILEm,
        MEM_BLOCK_ANY,
        ARAD_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LATENCY,
        &header_profile
        );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 148, exit);

      
      header_profile = 0;

      res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET, &header_profile);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 149, exit);

      
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ALWAYS_ADD_PPH_LEARN_EXTf, 0x0);
      soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, NEVER_ADD_PPH_LEARN_EXTf, 0x1);

      
      res = soc_mem_write(
          unit,
          IHB_HEADER_PROFILEm,
          MEM_BLOCK_ANY,
          ARAD_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN_DISABLE,
          &header_profile
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);
  }



#ifdef BCM_88650_B0
  
  if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  156,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, ARAD_FTMH_EXCLUDE_SRCf,  0x1));
  }
#endif 

  val = 0;
  SHR_BITCOPY_RANGE(&pph_vsi_source, ARAD_PP_FWD_CODE_IPV4_UC, &val, 0, 1);
  SHR_BITCOPY_RANGE(&pph_vsi_source, ARAD_PP_FWD_CODE_IPV6_UC, &val, 0, 1);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      
      if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  157,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_4r, REG_PORT_ANY, 0, PETRA_PPH_VSI_SOURCEf,  (pph_eep_vsi_source << 15) | pph_vsi_source));
      } else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_4r, REG_PORT_ANY, 0, PETRAB_PPH_VSI_SOURCEf,  pph_vsi_source));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_4r, REG_PORT_ANY, 0, PETRAB_PPH_EEP_VSI_SOURCEf,  pph_eep_vsi_source));
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  175,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_2r, REG_PORT_ANY, 0, PETRAB_EEI_SWAP_COMMANDf,  eei_swap_command));

      COMPILER_64_SET(eei_pop_command, 0x88888888, 0x88888888);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_PETRAB_EEI_POP_COMMANDr(unit,  eei_pop_command));
  }


  
 if ( (SOC_DPP_CONFIG(unit)->arad->init.ports.is_stacking_system == 0x1) || (SOC_DPP_CONFIG(unit)->arad->init.ports.add_dsp_extension_enable) ) {
      for (internal_profile = 0; internal_profile < ARAD_PMF_NOF_PGM_HEADER_PROFILE; ++internal_profile) {
          res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, internal_profile, &header_profile);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 190, exit);

          
          if (SOC_IS_JERICHO(unit) 
              && ((SOC_DPP_CONFIG(unit)->pp.routing_preserve_dscp)
                  || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "build_dsp_ext_for_mc", 0))) {
              soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_MCf, 0x1);
          }
          soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, ENABLE_STACKING_UCf, 0x1); 
          
          
          res = soc_mem_write(unit, IHB_HEADER_PROFILEm, MEM_BLOCK_ANY, internal_profile, &header_profile);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 200, exit);
      }
  }

  


    
	if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0 ) == 1)
	{
		for ( internal_profile = 0; internal_profile < ARAD_PMF_NOF_PGM_TM_HEADER_PROFILE; internal_profile++) {
        res = soc_mem_read(unit, IHB_HEADER_PROFILEm,  MEM_BLOCK_ANY, internal_profile, &header_profile);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 190, exit);
         
        soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_1_SIZEf, 0);
        soc_IHB_HEADER_PROFILEm_field32_set(unit, &header_profile, USER_HEADER_2_SIZEf, 0);

         
        res = soc_mem_write(unit, IHB_HEADER_PROFILEm, MEM_BLOCK_ANY, internal_profile, &header_profile);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 200, exit);
		}
	}

    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  496,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_PMF_GENERALr, SOC_CORE_ALL, 0, DISABLE_2ND_PASS_KEY_UPDATEf, 0));
        SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg_above_64_set(unit, IHB_PASS_2_INITIAL_KEYr, SOC_CORE_ALL, 0, reg_above64_val));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_pgm_init_unsafe()", 0, 0);
}


uint32
  arad_pmf_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  ARAD_PMF_PGM_INFO          *info
  )
{
  uint32
    internal_profile,
    fld_val,
    res = SOC_SAND_OK;
  uint32
    tbl_data,
    pgm_key_gen_var[ARAD_PMF_IHB_PROGRAM_KEY_GEN_VAR_LENGTH];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_PGM_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(pgm_key_gen_var, 0x0, ARAD_PMF_IHB_PROGRAM_KEY_GEN_VAR_LENGTH * sizeof(uint32));

  res = arad_pmf_pgm_set_verify(
          unit,
          pmf_pgm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = soc_mem_read(
          unit,
          IHB_PMF_PROGRAM_GENERALm,
          MEM_BLOCK_ANY,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  switch(info->bytes_to_rmv.header_type)
  {
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_START:
    fld_val = ARAD_PMF_PGM_BYTES_TO_RMV_HDR_START_FLD_VAL;
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_1ST:
    fld_val = ARAD_PMF_PGM_BYTES_TO_RMV_HDR_1ST_FLD_VAL;
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_FWDING:
    fld_val = ARAD_PMF_PGM_BYTES_TO_RMV_HDR_FWDING_FLD_VAL;
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING:
    fld_val = ARAD_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING_FLD_VAL;
    break;

  default:
    LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "Unit %d Program %d Header type %d - Failed to set program properties. Header type not found.\n\r"),
               unit, pmf_pgm_ndx, info->bytes_to_rmv.header_type));
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 15, exit);
  }
  
  soc_IHB_PMF_PROGRAM_GENERALm_field32_set(unit, &tbl_data, BYTES_TO_REMOVE_HEADERf, fld_val);

  
  
  
  soc_IHB_PMF_PROGRAM_GENERALm_field32_set(unit, &tbl_data, BYTES_TO_REMOVE_FIXf, info->bytes_to_rmv.nof_bytes);

  
  
  fld_val = ARAD_PMF_PPH_TYPE_NO_PPH;
  soc_IHB_PMF_PROGRAM_GENERALm_field32_set(unit, &tbl_data, PPH_TYPEf, fld_val);

  
  internal_profile = info->header_profile;
   
  soc_IHB_PMF_PROGRAM_GENERALm_field32_set(unit, &tbl_data, SYSTEM_HEADER_PROFILEf, internal_profile);

  
  res = soc_mem_write(
          unit,
          IHB_PMF_PROGRAM_GENERALm,
          MEM_BLOCK_ANY,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  
  soc_IHB_PROGRAM_KEY_GEN_VARm_field32_set(unit, pgm_key_gen_var, PROGRAM_KEY_GEN_VARf, info->copy_pgm_var);
  res = soc_mem_write(
          unit,
          IHB_PROGRAM_KEY_GEN_VARm,
          MEM_BLOCK_ANY,
          pmf_pgm_ndx,
          pgm_key_gen_var
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_pgm_set_unsafe()", pmf_pgm_ndx, 0);
}

uint32
  arad_pmf_pgm_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  ARAD_PMF_PGM_INFO                   *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_STAGE            
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_PGM_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_PMF_PGM_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_pgm_set_verify()", pmf_pgm_ndx, 0);
}

uint32
  arad_pmf_pgm_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx
  )
{
    SOC_PPC_FP_DATABASE_STAGE            
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_PGM_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_pgm_get_verify()", pmf_pgm_ndx, 0);
}


uint32
  arad_pmf_pgm_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_OUT ARAD_PMF_PGM_INFO                   *info
  )
{
  uint32
    header_profile_internal,
    res = SOC_SAND_OK;
  uint32
    tbl_data,
    pgm_key_gen_var[ARAD_PMF_IHB_PROGRAM_KEY_GEN_VAR_LENGTH];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_PGM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(pgm_key_gen_var, 0x0, ARAD_PMF_IHB_PROGRAM_KEY_GEN_VAR_LENGTH * sizeof(uint32));

  ARAD_PMF_PGM_INFO_clear(info);

  res = arad_pmf_pgm_get_verify(
          unit,
          pmf_pgm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = soc_mem_read(
          unit,
          IHB_PMF_PROGRAM_GENERALm,
          MEM_BLOCK_ANY,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  switch(soc_IHB_PMF_PROGRAM_GENERALm_field32_get(unit, &tbl_data, BYTES_TO_REMOVE_HEADERf))
  {
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_START_FLD_VAL:
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_1ST_FLD_VAL:
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_FWDING_FLD_VAL:
    break;
  case ARAD_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING_FLD_VAL:
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  
  
  
  info->bytes_to_rmv.nof_bytes = soc_IHB_PMF_PROGRAM_GENERALm_field32_get(unit, &tbl_data, BYTES_TO_REMOVE_FIXf);


  
  header_profile_internal = soc_IHB_PMF_PROGRAM_GENERALm_field32_get(unit, &tbl_data, SYSTEM_HEADER_PROFILEf);
  res = arad_pmf_low_level_pgm_header_type_get(
          unit,
          header_profile_internal,
          &(info->fc_type),
          &(info->header_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  res = soc_mem_read(
          unit,
          IHB_PROGRAM_KEY_GEN_VARm,
          MEM_BLOCK_ANY,
          pmf_pgm_ndx,
          pgm_key_gen_var
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  info->copy_pgm_var = soc_IHB_PROGRAM_KEY_GEN_VARm_field32_get(unit, pgm_key_gen_var, PROGRAM_KEY_GEN_VARf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_pgm_get_unsafe()", pmf_pgm_ndx, 0);
}



uint32
  ARAD_PMF_PGM_BYTES_TO_RMV_verify(
    SOC_SAND_IN  ARAD_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, ARAD_PMF_LOW_LEVEL_HEADER_TYPE_MAX, ARAD_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bytes, ARAD_PMF_LOW_LEVEL_NOF_BYTES_MAX, ARAD_PMF_LOW_LEVEL_NOF_BYTES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_PGM_BYTES_TO_RMV_verify()",0,0);
}

uint32
  ARAD_PMF_PGM_INFO_verify(
    SOC_SAND_IN  ARAD_PMF_PGM_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  
  
  for (ind = 0; ind < ARAD_PMF_NOF_CYCLES; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->lkp_profile_id[ind], 0, ARAD_PMF_LOW_LEVEL_LKP_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_profile_id, 0, ARAD_PMF_LOW_LEVEL_TAG_PROFILE_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_profile, 16-1, ARAD_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 12, exit); 
  ARAD_STRUCT_VERIFY(ARAD_PMF_PGM_BYTES_TO_RMV, &(info->bytes_to_rmv), 13, exit);

  
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fc_type, ARAD_PMF_LOW_LEVEL_FC_TYPE_MAX, ARAD_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 15, exit);

  if (info->header_type == ARAD_PORT_HEADER_TYPE_CPU)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_PGM_INFO_verify()",0,0);
}


void
  ARAD_PMF_PGM_BYTES_TO_RMV_clear(
    SOC_SAND_OUT ARAD_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_BYTES_TO_RMV_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PMF_PGM_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_PGM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME 

#endif 
