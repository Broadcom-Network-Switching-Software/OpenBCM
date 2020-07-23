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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/JER/jer_ports.h>

#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

#include <soc/dpp/port_sw_db.h>
#include <shared/swstate/access/sw_state_access.h>






#define ARAD_PP_EG_MIRROR_VID_DFLT_NDX                          7 
#define ARAD_PP_EG_MIRROR_VID_NOT_IN_USE                        0 

#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_PROFILE_SHIFT       8
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS 3
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK     ((1 << ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS) - 1)
#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_SHIFT               (ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_PROFILE_SHIFT - ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS)


#define ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(port, vid) (((vid)<<ARAD_PP_EG_MIRROR_PROFILE_TABLE_VID_SHIFT) | ((port) >> ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_PER_ENTRY_BITS))
#define DPP_EG_MIRROR_PROFILE_TABLE_ENTRY_SHIFT(port, vid) ((SOC_DPP_IMP_DEFS_GET(unit,pp_eg_mirror_profile_table_bits_per_profile)) * ((port) & ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK))



#define DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH 4
#define DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START  (0)
#define JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH 2
#define JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START (DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_START + DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH)
#define JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH 2 
#define JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_START (JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_START + JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH) 
#define JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH 1
#define JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_START (JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_START + JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH)
#define JER_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_LENGTH 1
#define JER_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_START (JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_START + JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH)


















CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_mirror[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_MIRROR_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_MIRROR_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_EG_MIRROR_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_MIRROR_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_mirror[] =
{
  
  {
    ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR",
    "The parameter 'enable_mirror' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_TRAP_CODES.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR",
    "The parameter 'recycle_command' is out of range. \n\r "
    "The range is: 0 - DPP_MIRROR_ACTION_NDX_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },  
  



  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};




static soc_field_t arad_pp_eg_mirror_vlan_id_flds[ARAD_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES] =
  {MIRROR_VID_0f, MIRROR_VID_1f, MIRROR_VID_2f, MIRROR_VID_3f, MIRROR_VID_4f, MIRROR_VID_5f, MIRROR_VID_6f};








STATIC 
uint32 
 dpp_outbound_mirror_config_read(
     SOC_SAND_IN int unit,
     SOC_SAND_IN  int   core_id,
     SOC_SAND_IN SOC_PPC_PORT port,
     SOC_SAND_IN uint32 vid,
     SOC_SAND_OUT dpp_outbound_mirror_config_t *config
     )
{

    uint32 mirror_command_lcl = 0;
    uint32 entry_start,length,table_offset,entry[]={0,0,0};
 
    SOCDNX_INIT_FUNC_DEFS;
    
    table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(port, vid);
    SOCDNX_IF_ERR_EXIT(READ_EPNI_MIRROR_PROFILE_TABLEm(unit, EPNI_BLOCK(unit, core_id), table_offset, entry));

    length = SOC_DPP_IMP_DEFS_GET(unit,pp_eg_mirror_profile_table_bits_per_profile);
    entry_start =  length * (port & ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK);

    SHR_BITCOPY_RANGE(&mirror_command_lcl, 0, entry, entry_start, length);
    
    config->mirror_command = mirror_command_lcl & ((1 << DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH) - 1);

    if (SOC_IS_JERICHO(unit)) {
        mirror_command_lcl = mirror_command_lcl >> DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH;
        
        config->forward_strength = mirror_command_lcl & ((1 << JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH) - 1);
        mirror_command_lcl = mirror_command_lcl >> JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH;
        
        config->mirror_strength = mirror_command_lcl & ((1 << JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH) - 1);
        mirror_command_lcl = mirror_command_lcl >> JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH;
        
        config->forward_en = mirror_command_lcl & JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH;
        mirror_command_lcl = mirror_command_lcl >> JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH;
        
        config->mirror_en = mirror_command_lcl & JER_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_LENGTH;
    }

exit:
    SOCDNX_FUNC_RETURN;



}




STATIC 
uint32 
 dpp_outbound_mirror_config_write(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id,
     SOC_SAND_IN SOC_PPC_PORT port,
     SOC_SAND_IN uint32 vid,
     SOC_SAND_OUT uint32 index,
     SOC_SAND_IN uint8 read_first,
     SOC_SAND_IN dpp_outbound_mirror_config_t *config
     )
{

    uint32 entry_start, length, entry_temp = 0;
    uint32 entry[]={0,0,0}; 

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_EPNI_MIRROR_PROFILE_TABLEm(unit, EPNI_BLOCK(unit,core_id), index, entry));
    
    length = SOC_DPP_IMP_DEFS_GET(unit,pp_eg_mirror_profile_table_bits_per_profile);
    
    entry_start = length * (port & ARAD_PP_EG_MIRROR_PROFILE_TABLE_PROFILES_ENTRY_MASK);

    
    if (SOC_IS_JERICHO(unit)) {
        
        entry_temp = config->mirror_en & ((1 << JER_EG_MIRROR_PROFILE_TABLE_MIRROR_EN_LENGTH) - 1);
        entry_temp = entry_temp << JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH;
        
        entry_temp |= config->forward_en & ((1 << JER_EG_MIRROR_PROFILE_TABLE_FWD_EN_LENGTH) - 1);
        entry_temp = entry_temp << JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH;
        
        entry_temp |= config->mirror_strength & ((1 << JER_EG_MIRROR_PROFILE_TABLE_MIRROR_STRENGTH_LENGTH) - 1);
        entry_temp = entry_temp << JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH;
        
        entry_temp |= config->forward_strength & ((1 << JER_EG_MIRROR_PROFILE_TABLE_FWD_STRENGTH_LENGTH) - 1);
        entry_temp = entry_temp << DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH;
    }

    
    entry_temp |= config->mirror_command & ((1 << DPP_EG_MIRROR_PROFILE_TABLE_MIRROR_COMMAND_LENGTH_LENGTH) - 1);
    
    SHR_BITCOPY_RANGE(entry, entry_start, &entry_temp, 0, length);
    
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_MIRROR_PROFILE_TABLEm(unit,  EPNI_BLOCK(unit, core_id), index, entry));


exit:
    SOCDNX_FUNC_RETURN;


}

STATIC
int
arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(int unit, int core, uint32 pp_port, int enable)
{
    soc_port_t port;
    soc_port_if_t interface_type;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_pp_to_local_port_get(unit, core, pp_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    if (interface_type == SOC_PORT_IF_CAUI) {
        uint32 reg_val, fld_val[1], ifc;

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, pp_port, &ifc)));
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NIF_CANCEL_EN_REGISTER_1r(unit, &reg_val));

        *fld_val = soc_reg_field_get(unit, EGQ_NIF_CANCEL_EN_REGISTER_1r, reg_val, NIF_CANCEL_EN_1f);
        if (enable) {
            SHR_BITSET(fld_val, ifc);
        } else {
            SHR_BITCLR(fld_val, ifc);
        }
        soc_reg_field_set(unit, EGQ_NIF_CANCEL_EN_REGISTER_1r, &reg_val, NIF_CANCEL_EN_1f, *fld_val);

        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NIF_CANCEL_EN_REGISTER_1r(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
   
uint32
  arad_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  int i, res;
  uint32 value;
  soc_reg_above_64_val_t reg_above_64_val = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,
    WRITE_EPNI_MIRROR_VID_REGr(unit, REG_PORT_ANY, reg_above_64_val)); 

  if (!SOC_IS_JERICHO(unit)) {
      
      for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
          value = 0;
          soc_mem_field32_set(unit, EPNI_MIRROR_PROFILE_MAPm, &value, MIRROR_COMMANDf, i);

          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20 + i, exit, ARAD_REG_ACCESS_ERR, WRITE_EPNI_MIRROR_PROFILE_MAPm(unit, MEM_BLOCK_ALL, i, &value));
      }
  } else {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR, WRITE_IRR_MIRROR_SIZEr(unit, 0x3fffffff)); 
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 55, exit, ARAD_REG_ACCESS_ERR, WRITE_IDR_SNOOP_MIRROR_IS_OCB_ELIGIBLEr(unit, 0xffffffff));
  }

  
  for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
    value = 0; 
    soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, MIRROR_PROFILEf, i);
    if (i > 0) {       
      soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, FORWARD_STRENGTHf, 7); 
      
      soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, SNOOP_STRENGTHf,
                                soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "recycle_snoop_strength", 0));
      soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, CPU_TRAP_CODEf, SOC_PPC_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0);   
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100+i, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ALL, i, &value));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  SOC_PPC_PORT          pp_port,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID      vid,       
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config

  )
{
  uint32 res, q_pair;
  uint32 internal_vid_ndx; 
  uint32 entry[3]; 
  uint32 channel= 0; 
  uint32 base_q_pair = 0; 
  uint32 end_q_pair  = 0; 
  uint8 first_appear;
  int mirror_profile_table_offset, clean_stage = 0;
  uint32 reassembly_context;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core_id, pp_port, &end_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  res = arad_sw_db_multiset_add(unit, ARAD_SW_DB_CORE_ANY, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &first_appear, success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  if (*success != SOC_SAND_SUCCESS) { 
    goto exit;
  }
  clean_stage = 1;

  if (first_appear) { 
    soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
      READ_EPNI_MIRROR_VID_REGr(unit, SOC_CORE_ALL, reg_above_64_val)); 
    fld_value[0] = vid; 
    soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 40, exit, ARAD_REG_ACCESS_ERR,
      WRITE_EPNI_MIRROR_VID_REGr(unit, SOC_CORE_ALL, reg_above_64_val)); 
  }


  
  res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit,core_id, pp_port,  &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 45, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR, READ_EGQ_PCTm(unit, EGQ_BLOCK(unit ,core_id), base_q_pair, entry));

  if (!soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf)) { 
    res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core_id, pp_port, &channel, &reassembly_context); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit); 

    clean_stage |= 2;
    end_q_pair += base_q_pair;
    for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
      if (q_pair > base_q_pair) { 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 70, exit, ARAD_REG_ACCESS_ERR,
          READ_EGQ_PCTm(unit, EGQ_BLOCK(unit ,core_id), q_pair, entry));
      }
      soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_CHANNELf, channel);
      soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 1);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit ,core_id), q_pair, entry));
    }

    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
        res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
    }
  }

  
  mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(pp_port, internal_vid_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130, exit, ARAD_REG_ACCESS_ERR,
    READ_EPNI_MIRROR_PROFILE_TABLEm(unit, EPNI_BLOCK(unit ,core_id), mirror_profile_table_offset, entry));
 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                       140, 
                                       exit, 
                                       ARAD_REG_ACCESS_ERR, 
                                       dpp_outbound_mirror_config_write(unit, 
                                                                      core_id,
                                                                      pp_port, 
                                                                      internal_vid_ndx,
                                                                      mirror_profile_table_offset,
                                                                      TRUE, 
                                                                      config
                                                                        )
                                        );
  
  res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(unit, core_id, pp_port, internal_vid_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res,  150, exit);

exit:

  if (ex != no_err) { 
    if (clean_stage & 2) { 
      for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
        if (READ_EGQ_PCTm(unit, EGQ_BLOCK(unit ,core_id), q_pair, entry) == SOC_E_NONE) {                                    \
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit ,core_id), q_pair, entry));
        }
      }
      release_reassembly_context_and_mirror_channel_unsafe(unit, core_id, pp_port, channel);

      
      if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
          res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 0);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
      }
    }
    if (clean_stage & 1) { 
		uint8 last_appear;
      arad_sw_db_multiset_remove_by_index(unit, ARAD_SW_DB_CORE_ANY, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, internal_vid_ndx, &last_appear);
      if (first_appear) { 
        soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
        if (READ_EPNI_MIRROR_VID_REGr(unit, core_id, reg_above_64_val) == SOC_E_NONE) { 
          fld_value[0] = ARAD_PP_EG_MIRROR_VID_NOT_IN_USE; 
          soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
          res = WRITE_EPNI_MIRROR_VID_REGr(unit, core_id, reg_above_64_val); 
        }
      }
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_add_unsafe()", pp_port, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_PORT         out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID  vid,
    SOC_SAND_IN  uint8                enable_mirror

  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(enable_mirror, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR, 30, exit);

  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_add_verify()", out_port_ndx, vid);
}


uint32
  arad_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id,
    SOC_SAND_IN  SOC_PPC_PORT         out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID  vid
  )
{
  uint32 res, internal_vid_ndx, ref_count;
  uint32 entry[3]; 
  uint32 q_pair, nof_priorities, end_q_pair; 
  uint8 last_appear, recycle_needed;
  int mirror_profile_table_offset;
  soc_port_t local_port;
  uint32 channel;
  dpp_outbound_mirror_config_t        config;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE);

  
  res = arad_sw_db_multiset_lookup(unit, ARAD_SW_DB_CORE_ANY, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count) { 
    
    res = arad_sw_db_multiset_remove_by_index(unit, ARAD_SW_DB_CORE_ANY, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, internal_vid_ndx, &last_appear);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (last_appear) { 
      soc_reg_above_64_val_t fld_value = {0}, reg_above_64_val;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_VID_REGr(unit, SOC_CORE_ALL, reg_above_64_val)); 
      fld_value[0] = ARAD_PP_EG_MIRROR_VID_NOT_IN_USE; 
      soc_reg_above_64_field_set(unit, EPNI_MIRROR_VID_REGr, reg_above_64_val, arad_pp_eg_mirror_vlan_id_flds[internal_vid_ndx], fld_value);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 40, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EPNI_MIRROR_VID_REGr(unit, SOC_CORE_ALL, reg_above_64_val)); 
    }

    
    res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_set(unit, core_id, out_port_ndx, internal_vid_ndx, FALSE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                         60, 
                                         exit, 
                                         ARAD_REG_ACCESS_ERR, 
                                         dpp_outbound_mirror_config_read(unit, 
                                                                        core_id,
                                                                        out_port_ndx, 
                                                                        ARAD_PP_EG_MIRROR_VID_DFLT_NDX, 
                                                                        &config)
                                         );

    mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(out_port_ndx, internal_vid_ndx);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR,
                                         dpp_outbound_mirror_config_write(unit, core_id, out_port_ndx, internal_vid_ndx, mirror_profile_table_offset,TRUE,&config ));

    
    res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, out_port_ndx, &local_port);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 85, exit);
    SOC_SAND_IF_ERR_EXIT(arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port, &recycle_needed));
    
    for (internal_vid_ndx = 0; !recycle_needed && internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
      res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, core_id, out_port_ndx, internal_vid_ndx, &recycle_needed);
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
    
    if (!recycle_needed && !(config.mirror_command)) {
      int enabled = 1;
      

      res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit,core_id, out_port_ndx,&q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 195, exit);
      res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core_id, out_port_ndx, &nof_priorities);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 197, exit);

      end_q_pair = q_pair + nof_priorities;

      for (; q_pair < end_q_pair; ++q_pair) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
          READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core_id), q_pair, entry));
        enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf);
        soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
          WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core_id), q_pair, entry));
      }
      
      channel = soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_CHANNELf);
      

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
        enabled ? SOC_E_NONE : SOC_E_INTERNAL); 

      res = release_reassembly_context_and_mirror_channel_unsafe(unit, core_id, out_port_ndx,  channel);
      SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); 

      
      if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
          res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, out_port_ndx, 0);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
      }
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_remove_unsafe()", out_port_ndx, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PPC_PORT        out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID vid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_remove_verify()", out_port_ndx, vid);
}


uint32
  arad_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  SOC_PPC_PORT        out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID vid,
    SOC_SAND_OUT dpp_outbound_mirror_config_t        *config

  )
{
  uint32 res, internal_vid_ndx, ref_count;
  uint8  port_vlan_exists;





  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(config);


  config->mirror_command = 0;

  
  res = arad_sw_db_multiset_lookup(unit, ARAD_SW_DB_CORE_ANY, ARAD_PP_SW_DB_MULTI_SET_EG_MIRROR_PROFILE, &vid, &internal_vid_ndx, &ref_count);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ref_count) { 
    res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, core_id, out_port_ndx, internal_vid_ndx, &port_vlan_exists);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (port_vlan_exists) { 
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                           30, 
                                           exit, 
                                           ARAD_REG_ACCESS_ERR, 
                                           dpp_outbound_mirror_config_read(unit, core_id, out_port_ndx, internal_vid_ndx, config));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_get_unsafe()", out_port_ndx, vid);
}

uint32
  arad_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(out_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_vlan_get_verify()", out_port_ndx, vid);
}


uint32
  arad_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                        pp_port,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config
  )
{
  uint32 res, prev_profile[1];
  uint32 entry[3]; 
  uint32 entry_context[3]; 
  uint32 q_pair, nof_pairs, end_q_pair; 
  unsigned internal_vid_ndx;
  int is_disable, has_mirrored_vlans = 0;
  int mirror_profile_table_offset;
  uint8 port_vlan_exist;
  uint32 channel;
  dpp_outbound_mirror_config_t        local_config;
  uint32 reassembly_context;
  soc_port_t local_port;
  uint8 is_recycled_by_apl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(config);


  res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core_id, pp_port, &end_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(pp_port, ARAD_PP_EG_MIRROR_VID_DFLT_NDX);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                       20, 
                                       exit, 
                                       ARAD_REG_ACCESS_ERR,
                                       dpp_outbound_mirror_config_read(unit, 
                                                                      core_id,
                                                                      pp_port, 
                                                                      ARAD_PP_EG_MIRROR_VID_DFLT_NDX , 
                                                                      &local_config));
  *prev_profile =  local_config.mirror_command;
  sal_memcpy(&local_config,config,sizeof(dpp_outbound_mirror_config_t));

  is_disable = !config->mirror_command && *prev_profile;

  if (config->mirror_command && !(*prev_profile)) { 
    
    uint32 base_q_pair;
    res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core_id, pp_port,  &base_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
      READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), base_q_pair, entry_context));

    if (!soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) { 
      res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core_id, pp_port, &channel, &reassembly_context); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); 

      end_q_pair += base_q_pair;
      for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
        if (q_pair > base_q_pair) { 
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR,
            READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry_context));
        }
        soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf, channel);
        soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
          WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry_context));
      }
    }

    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
        res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
    }
  }

  
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 100, exit, ARAD_REG_ACCESS_ERR,
    dpp_outbound_mirror_config_write(unit, 
                                   core_id,
                                   pp_port, 
                                   ARAD_PP_EG_MIRROR_VID_DFLT_NDX,
                                   mirror_profile_table_offset,
                                   TRUE, 
                                   &local_config
                                     )
                                       );

  
  for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
    if (!SOC_DPP_PP_ENABLE(unit)) {
      port_vlan_exist = FALSE;
    } else {
      res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, core_id, pp_port, internal_vid_ndx, &port_vlan_exist);
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    }
    if (port_vlan_exist) {
      has_mirrored_vlans = 1;
    } else {
      mirror_profile_table_offset = ARAD_PP_EG_MIRROR_PROFILE_TABLE_CALC_ENTRY_OFFSET(pp_port, internal_vid_ndx);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 120, exit, ARAD_REG_ACCESS_ERR,
        READ_EPNI_MIRROR_PROFILE_TABLEm(unit, EPNI_BLOCK(unit,core_id), mirror_profile_table_offset, entry));
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 130, exit, ARAD_REG_ACCESS_ERR,
                                           dpp_outbound_mirror_config_write(unit,
                                                                          core_id,
                                                                          pp_port,
                                                                          internal_vid_ndx,
                                                                          mirror_profile_table_offset, 
                                                                          TRUE,
                                                                          &local_config
                                                                            )        
                                            );
    }
  }

  

  res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, pp_port, &local_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
  SOC_SAND_IF_ERR_EXIT(arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port, &is_recycled_by_apl));

  if (is_disable && !has_mirrored_vlans && !is_recycled_by_apl) {
    
    int enabled = 1;

    res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core_id, pp_port, &q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

    res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core_id, pp_port, &nof_pairs);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 180, exit);

    end_q_pair = q_pair + nof_pairs;

    for (; q_pair < end_q_pair; ++q_pair) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
        READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry_context));
      enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf);
      soc_mem_field32_set(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf, 0);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
        WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry_context));
    }
    
    channel = soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_CHANNELf);
    

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
      enabled ? SOC_E_NONE : SOC_E_INTERNAL); 

     res = release_reassembly_context_and_mirror_channel_unsafe(unit, core_id, pp_port, channel);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); 

    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
        res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 0);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_set_unsafe()", pp_port, 0);
}

uint32
  arad_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

   
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, SOC_DPP_DEFS_GET(unit, nof_logical_ports), SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

   
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  SOC_PPC_PORT                      local_port_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t        *config

  )
{
  int res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(config);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                       10, 
                                       exit, 
                                       ARAD_REG_ACCESS_ERR,
                                       dpp_outbound_mirror_config_read(unit, 
                                                                      core_id,
                                                                      local_port_ndx, 
                                                                      ARAD_PP_EG_MIRROR_VID_DFLT_NDX, 
                                                                      config)
                                       );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_dflt_get_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_eg_mirror_port_appl_set_unsafe(
    SOC_SAND_IN  int            unit,      
    SOC_SAND_IN  SOC_PPC_PORT   local_port, 
    SOC_SAND_IN  uint8          enable          
  )
{
  uint32 res;
  uint32 entry[3]; 
  uint32 q_pair, end_q_pair; 
  uint8 is_reserved;
  uint32 channel;
  dpp_outbound_mirror_config_t        config;
  uint32 reassembly_context;
  int  core_id;
  uint32  pp_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_APPL_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 5, exit, soc_port_sw_db_local_to_pp_port_get(unit,local_port, &pp_port, &core_id));
  
  res = arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port, &is_reserved);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
  if (is_reserved != (enable ? 1 : 0)) { 

    res = soc_port_sw_db_pp_port_to_out_port_priority_get(unit, core_id, pp_port, &end_q_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);

    if (!end_q_pair) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR, 20, exit);
    }

    if (enable) { 

      
      uint32 base_q_pair;
      res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core_id, pp_port, &base_q_pair);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
        READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), base_q_pair, entry));

      if (!soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf)) { 
        res = alloc_reassembly_context_and_recycle_channel_unsafe(unit, core_id, pp_port, &channel, &reassembly_context); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); 

        end_q_pair += base_q_pair;
        for (q_pair = base_q_pair; q_pair < end_q_pair; ++q_pair) {
          if (q_pair > base_q_pair) { 
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR,
              READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry));
          }
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_CHANNELf, channel);
          soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 1);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
            WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry));
        }

        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
            res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 1);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
        }
      }

    } else { 

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 
                                           100, 
                                           exit, 
                                           ARAD_REG_ACCESS_ERR,
                                           dpp_outbound_mirror_config_read(unit, 
                                                                          core_id,
                                                                          pp_port, 
                                                                          ARAD_PP_EG_MIRROR_VID_DFLT_NDX, 
                                                                          &config));

      
      if (!(config.mirror_command)) {

        
        unsigned internal_vid_ndx;
        uint8 port_vlan_exist;
        for (internal_vid_ndx = 0; internal_vid_ndx < ARAD_PP_EG_MIRROR_VID_DFLT_NDX; ++internal_vid_ndx) {
          if (!SOC_DPP_PP_ENABLE(unit)) {
              port_vlan_exist = FALSE;
          } else {
              res = arad_pp_sw_db_eg_mirror_port_vlan_is_exist_get(unit, core_id, pp_port, internal_vid_ndx, &port_vlan_exist);
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          }
          if (port_vlan_exist) {
            break;
          }
        }
        if (internal_vid_ndx >=ARAD_PP_EG_MIRROR_VID_DFLT_NDX) { 
          
          int was_recycle_enabled = 1;

          res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core_id, pp_port, &q_pair);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 150, exit);

          end_q_pair += q_pair;
          for (; q_pair < end_q_pair; ++q_pair) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 200, exit, ARAD_REG_ACCESS_ERR,
              READ_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry));
            was_recycle_enabled &=  soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_ENABLEf);
            soc_mem_field32_set(unit, EGQ_PCTm, entry, MIRROR_ENABLEf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
              WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core_id), q_pair, entry));
          }
          
          channel = soc_mem_field32_get(unit, EGQ_PCTm, entry, MIRROR_CHANNELf);

          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 250, exit, ARAD_REG_ACCESS_ERR,
            was_recycle_enabled ? SOC_E_NONE : SOC_E_INTERNAL); 

          res = release_reassembly_context_and_mirror_channel_unsafe(unit, core_id, pp_port, channel);
          SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit); 

          
          if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_CONFIG(unit)->arad->caui_fast_recycle) {
              res = arad_pp_eg_mirror_mirror_port_nif_cancel_rate_set(unit, core_id, pp_port, 0);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 65, exit);  
          }
        }
      }
    } 

    res = arad_sw_db_set_port_reserved_for_reassembly_context(unit, local_port, enable);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit); 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_appl_set_unsafe()", pp_port, enable);
}


uint32
  arad_pp_eg_mirror_port_appl_get_unsafe(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  SOC_PPC_PORT  local_port_ndx, 
    SOC_SAND_OUT uint8         *is_enabled     
  )
{
  int res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_PORT_APPL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_enabled);

  res = arad_sw_db_is_port_reserved_for_reassembly_context(unit, local_port_ndx, is_enabled);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_appl_get_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_eg_mirror_recycle_command_trap_set_unsafe(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_IN  uint32        trap_code, 
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  )
{
  int res;
  uint32 entry, reg;
  uint32 internal_trap_code; 
  ARAD_SOC_REG_FIELD strength_fld_fwd, strength_fld_snp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_UNSAFE);

  res = arad_pp_trap_mgmt_trap_code_to_internal(unit, trap_code, &internal_trap_code, &strength_fld_fwd, &strength_fld_snp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, CPU_TRAP_CODEf, &internal_trap_code);
  reg = forward_strengh;
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, FORWARD_STRENGTHf, &reg);
  reg = snoop_strength;
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, SNOOP_STRENGTHf, &reg);
  
  reg=0;
  soc_IHP_RECYCLE_COMMANDm_field_set(unit, &entry, MIRROR_PROFILEf, &reg);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_RECYCLE_COMMANDm(unit,  MEM_BLOCK_ANY, recycle_command, &entry));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_set_unsafe()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_set_verify(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_IN  uint32        trap_code, 
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(recycle_command, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trap_code, SOC_PPC_NOF_TRAP_CODES, ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_set_verify()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_verify(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command  
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(recycle_command, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_get_verify()", recycle_command, 0);
}

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_unsafe(
    SOC_SAND_IN  int        unit,      
    SOC_SAND_IN  uint32        recycle_command, 
    SOC_SAND_OUT  uint32       *trap_code, 
    SOC_SAND_OUT  uint32       *snoop_strength
  )
{
  int res;
  uint32 entry;
  uint32 internal_trap_code;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(trap_code);
  SOC_SAND_CHECK_NULL_INPUT(snoop_strength);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  soc_IHP_RECYCLE_COMMANDm_field_get(unit, &entry, CPU_TRAP_CODEf, &internal_trap_code);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ANY, recycle_command, &entry));
  *snoop_strength = soc_IHP_RECYCLE_COMMANDm_field32_get(unit, &entry, SNOOP_STRENGTHf);

  res = arad_pp_trap_cpu_trap_code_from_internal_unsafe(unit, internal_trap_code, trap_code);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_recycle_command_trap_get_unsafe()", recycle_command, 0);
}


uint32
  arad_pp_eg_mirror_port_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   pp_port,
    SOC_SAND_OUT SOC_PPC_EG_MIRROR_PORT_INFO       *info
  )
{
  uint32 base_q_pair;
  uint32 res;
  uint32 entry_context[3]; 
  soc_port_t rcy_port, local_dest_port;
  uint32 rcy_tm_port;
  int rcy_core, egr_if;
  soc_port_if_t interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_MIRROR_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_pp_port_to_base_q_pair_get(unit, core_id,  pp_port,  &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR, 
      READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core_id), base_q_pair, entry_context));

  if (!soc_mem_field32_get(unit, EGQ_PCTm, entry_context, MIRROR_ENABLEf)) 
  {
    info->outbound_mirror_enable = 0; 
  }
  else
  {
    info->outbound_mirror_enable = 1;

    res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, pp_port, &local_dest_port);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_port_sw_db_interface_type_get(unit, local_dest_port, &interface_type);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 15, exit);


    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core_id, pp_port, (uint32 *)&egr_if));
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.rcy_single_context_port.get(unit, core_id, egr_if, &rcy_port);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
    if (rcy_port == ARAD_PORT_INVALID_RCY_PORT) { 
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }

    res = soc_port_sw_db_local_to_tm_port_get(unit, rcy_port, &rcy_tm_port, &rcy_core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 99, exit); 
    info->outbound_port_ndx = rcy_port; 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_info_get_unsafe()", pp_port, 0);
}

uint32
  arad_pp_eg_mirror_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_EG_MIRROR_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

   
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_mirror_port_info_get_verify()", local_port_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_mirror_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_mirror;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_mirror_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_mirror;
}

uint32
  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_DFLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dflt_profile, DPP_MIRROR_ACTION_NDX_MAX, ARAD_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_EG_MIRROR_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_EG_MIRROR_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->outbound_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

