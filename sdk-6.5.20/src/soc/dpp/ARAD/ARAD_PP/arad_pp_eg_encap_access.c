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
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/utils.h>
#include <soc/sand/sand_mem.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/dpp_config_defs.h>






#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index)           (34 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_MSB(start_index)           (36 + start_index)
#define ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_NOF_BITS(start_index)      SOC_SAND_RNG_COUNT(ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_LSB(start_index), ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_MSB(start_index))






#define ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif) \
  ((outlif & 0x1) == 0x0)


#define ARAD_PP_EG_ENCAP_ACCESS_GET_OTHER_HALF_ENTRY_ID(outlif) \
  ((outlif) ^ 0x1)

#ifdef BCM_88660_A0

#define ARAD_PP_EG_ENCAP_ACCESS_IS_BIG_AC_LOCATION(outlif) \
  ((outlif & 0x1) == 0x0)
#endif

#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT 0x01
#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(half_entry_format)    \
      (half_entry_format == ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT)

#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_AC_FORMAT 0x00
#define ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(half_entry_format)    \
      (half_entry_format == ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_AC_FORMAT)



 
#define ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(_unit, _next_outlif)\
      ((_next_outlif) = (SOC_IS_JERICHO(_unit)) ? ((_next_outlif) << 1) : (_next_outlif))

#define ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(_unit, _next_outlif) \
      if (SOC_IS_JERICHO(_unit) && ((_next_outlif) & 1)) { SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_LSB_MUST_BE_0, 999, exit);}

#define ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(_unit, _next_outlif) \
      ((SOC_IS_JERICHO(_unit)) ? (_next_outlif >> 1) : (_next_outlif))








CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_eg_encap_access[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_GET_ERRS_PTR),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_eg_encap_access[] =
{
  
  

  {
    ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR,
    "ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR",
    "Entry type mismatch the requested entry hadle. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR,
    "ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR",
    "mpls1_command is out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};










STATIC uint32
  arad_pp_eg_encap_access_key_prefix_get_internal(    
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                             outlif,
                 uint32                             *data,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE  *key_prefix
    );

STATIC uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_in_buffer(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint32                                         outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT   *tbl_data,
    SOC_SAND_INOUT uint32                                       *raw_data
  );

STATIC uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_in_buffer(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint32                                         outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT    *tbl_data,
    SOC_SAND_INOUT uint32                                       *raw_data
  );


soc_error_t 
  arad_pp_eg_encap_access_create_none_entry_in_buffer(
     int     unit,
     uint32  table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE]
     )
{
    uint32
      res = SOC_SAND_OK;
    uint32 tmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_CLEAR(table_entry, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

    if (SOC_IS_JERICHO_PLUS(unit)) {
        soc_mem_field32_set(unit, EDB_AC_HALF_ENTRY_FORMATm, table_entry, AC_1_ACTION_DROPf, 0x1);
    } else if (SOC_IS_ARADPLUS(unit) && SOC_IS_JERICHO_AND_BELOW(unit))  {
        
        soc_mem_field32_set(unit, EDB_AC_HALF_ENTRY_FORMATm, table_entry, AC_1_ENTRY_FORMATf, 0x2);
    } else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        
        soc_mem_field32_set(unit, EPNI_AC_FORMATm, table_entry, AC_0_ACTION_DROPf, 1);
        soc_mem_field32_set(unit, EPNI_AC_FORMATm, table_entry, AC_1_ACTION_DROPf, 1);
        soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, table_entry, OUT_RIF_0_ACTION_DROPf, 1);
        soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, table_entry, OUT_RIF_1_ACTION_DROPf, 1);
    }

    
    tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE;
    res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
    arad_pp_eg_encap_access_clear_bank(int unit, int bank_id){
    uint32
      res = SOC_SAND_OK;
    uint32
      table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    
    res = arad_pp_eg_encap_access_create_none_entry_in_buffer(unit, table_entry);
    SOCDNX_IF_ERR_EXIT(res);

     
      
    res = sand_fill_partial_table_with_entry(unit, EDB_EEDB_BANKm, bank_id, bank_id, MEM_BLOCK_ANY, 0, (1 << SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs)) - 1, table_entry);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_eg_encap_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    table_entry[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE]; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  if (SOC_IS_JERICHO(unit)) {
      res = jer_pp_eg_encap_access_init_outrif_max(unit); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
  }

  if (SOC_IS_QAX(unit)) {
       
      res = soc_reg_field32_modify(unit, EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 999, exit);
  } 

  
  if (SOC_IS_JERICHO(unit) && SOC_IS_JERICHO_AND_BELOW(unit)) {
      uint32 reg_val;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 295, exit, READ_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, &reg_val));
      soc_reg_field_set(unit, EPNI_PP_CONFIGr, &reg_val, INVALID_OUTLIF_FIX_ENf, 1);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, WRITE_EPNI_PP_CONFIGr(unit, REG_PORT_ANY, reg_val));
  }

  
  res = arad_pp_eg_encap_access_create_none_entry_in_buffer(unit, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  res = arad_fill_table_with_entry(unit, EDB_EEDB_BANKm, MEM_BLOCK_ANY, &table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (SOC_IS_JERICHO(unit)) {
      res = arad_fill_table_with_entry(unit, EDB_EEDB_TOP_BANKm, MEM_BLOCK_ANY, &table_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_init_unsafe()", 0, 0);
}


uint32
  arad_pp_eg_encap_access_entry_init_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif
  )
{  
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE
      key_prefix, key_prefix2;
  uint8 
      is_location_at_start,
      full_reset_entry;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = arad_pp_eg_encap_access_key_prefix_get_internal(unit, outlif, data, &key_prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (key_prefix == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF 
      || key_prefix == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {

      
      res = arad_pp_eg_encap_access_key_prefix_get_internal(unit, ARAD_PP_EG_ENCAP_ACCESS_GET_OTHER_HALF_ENTRY_ID(outlif), data, &key_prefix2);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (key_prefix2 == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE) {
          
          full_reset_entry = TRUE;
      } else {
          

          
          if (key_prefix2 == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF) {
              
              ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  tbl_data;

              sal_memset(&tbl_data, 0, sizeof(tbl_data));

              res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_in_buffer(unit, outlif, &tbl_data, data);
              SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          } else if (key_prefix2 == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC) {
              
              ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  tbl_data;

              sal_memset(&tbl_data, 0, sizeof(tbl_data));

              res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_in_buffer(unit, outlif, &tbl_data, data);
              SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
          } else {
              
              SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 47, exit);
          }

          if (SOC_IS_JERICHO_AND_BELOW(unit)) {
              
              soc_mem_field32_set(unit, EDB_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf, 0);
          }

          
          if (SOC_IS_ARADPLUS(unit) && SOC_IS_JERICHO_AND_BELOW(unit))  {
              soc_mem_field32_set(unit, EDB_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_USE_AS_DATA_BITf : AC_1_ACTION_USE_AS_DATA_BITf, 0);
          } else if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
              soc_mem_field32_set(unit, EPNI_AC_FORMATm, data, (is_location_at_start) ? AC_0_ACTION_DROPf : AC_1_ACTION_DROPf, 1);
              soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_DROPf : OUT_RIF_1_ACTION_DROPf, 1);
          }

          
          full_reset_entry = FALSE;
      }
  } 
  else if (!is_location_at_start){ 
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 60, exit);
  } else {
      full_reset_entry = TRUE;
  }

  
  if (full_reset_entry) {
      res = arad_pp_eg_encap_access_create_none_entry_in_buffer(unit, data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_entry_init_unsafe()", outlif, 0);
}



uint32 
arad_pp_eg_encap_access_entry_location_info_get(int unit, int outlif,
                    soc_mem_t *mem, uint32 *bank_id, uint32 *internal_offset)
{
    uint32 entry_offset;
    uint8 is_top_bank_entry;
    SOCDNX_INIT_FUNC_DEFS;

    entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, outlif);

    is_top_bank_entry = entry_offset >= ARAD_PP_EG_ENCAP_ACCESS_NOF_ALIGNED_EEDB_ENTRIES(unit);

    if (is_top_bank_entry) {
        *bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_ID(unit, entry_offset);
        *internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_INTERNAL_OFFSET(unit, entry_offset);
        *mem = EDB_EEDB_TOP_BANKm;

        
        if (*bank_id >= SOC_DPP_DEFS_GET(unit, eg_encap_nof_top_banks)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Outlif out of range.")));
        }
    } else {
        *bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit, entry_offset);
        *internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset);
        *mem = EDB_EEDB_BANKm;

        
        if ((*bank_id >= ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(unit)) || 
            (*internal_offset >= ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_EEDB_BANK(unit))) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOC_MSG("Outlif out of range.")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
arad_pp_eg_encap_access_entry_write(int unit, int outlif, uint32 *data){
    int rv;
    uint32 bank_id, internal_offset;
    soc_mem_t eedb_mem;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_eg_encap_access_entry_location_info_get(unit, outlif, &eedb_mem, &bank_id, &internal_offset);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_mem_array_write(unit, eedb_mem, bank_id, MEM_BLOCK_ANY, internal_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
arad_pp_eg_encap_access_entry_read(int unit, int outlif, uint32 *data){
    int rv;
    uint32 bank_id, internal_offset;
    soc_mem_t eedb_mem;
    SOCDNX_INIT_FUNC_DEFS;

    rv = arad_pp_eg_encap_access_entry_location_info_get(unit, outlif, &eedb_mem, &bank_id, &internal_offset);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_mem_array_read(unit, eedb_mem, bank_id, MEM_BLOCK_ANY, internal_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32
arad_pp_eg_encap_access_key_prefix_get_internal(    
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                             outlif,
                 uint32                             *data,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE  *key_prefix
    )
{
    uint32
      res = SOC_SAND_OK;
    uint32
      is_location_at_start,
      access_prefix = 0,
      access_suffix = 0,
      access_prefix_2_ent_ac = 0,
      access_prefix_2_ent_rif = 0,
      mpls1_command = 0;
    soc_mem_t mem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

      
    res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &access_prefix);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS){
      *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS;
    }
    else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA){
        uint64 data_entry_data;
        uint32 data_lsbs;
        uint32 data_outlif_profile = 0;
        uint32 data_outlif_profile_val = 0;
        
        mem = EDB_DATA_FORMATm;

        if (SOC_IS_JERICHO(unit)) {
            data_outlif_profile = soc_mem_field32_get(unit, mem, data, DATA_ACTION_OUTLIF_PROFILEf);            
            res = arad_pp_occ_mgmt_app_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL, &data_outlif_profile, &data_outlif_profile_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            
        }
        
        soc_mem_field64_get(unit, mem, data, DATA_ACTION_DATAf, &data_entry_data);
        COMPILER_64_TO_32_LO(data_lsbs, data_entry_data);
        res = soc_sand_bitstream_get_any_field(&data_lsbs, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_LSB, ARAD_PP_EG_ENCAP_ACCESS_SUFFIX_NOF_BITS, &access_suffix);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE) {
            if (SOC_IS_JERICHO(unit) &&
                (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) && 
                (data_outlif_profile_val == 1) && 
                SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_enable) {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6;
            }
            else if ((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) &&
                      access_suffix == ARAD_PRGE_DATA_ENTRY_LSBS_IPV6_TUNNEL && SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_enable) {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA_IPV6;
            } else {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA;
            }
        } else {
            
            *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA;
        }
    } else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER) {
        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) || SOC_IS_JERICHO_PLUS(unit)) {
            *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER;
        }
        
        else if (SOC_IS_JERICHO_AND_BELOW(unit)) {
            uint32 roo_link_format_identifier;

            
            mem = EDB_LINK_LAYER_OR_ARP_NEW_FORMATm;
            roo_link_format_identifier = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_ACTION_IDENTIFIERf);

            
            if (roo_link_format_identifier) {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_ROO_LINK_LAYER;
            } else {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER;
            }
        }
        else {
            *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_LINK_LAYER;
        }

    } else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK) ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP) {
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_IP;
    }
    
    else if (access_prefix  ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE) {
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
    } else if ((access_prefix & ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_PREFIX_MASK) == ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_TRILL) {
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_TRILL;
    } else if (access_prefix  ==  ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER) {

        is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

        if (SOC_IS_JERICHO_PLUS(unit)) {
            
            *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC;
        } else if (SOC_IS_JERICHO_AND_BELOW(unit)) {
            
            access_prefix_2_ent_ac = soc_mem_field32_get(unit, EDB_AC_HALF_ENTRY_FORMATm, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf);
            access_prefix_2_ent_rif = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf);
            
            if (ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(access_prefix_2_ent_rif)){
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF;
            }
            else if (ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(access_prefix_2_ent_ac)){
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_AC;
            }
            else {
                *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
            }
        }
    }

    if(*key_prefix == ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS) {
        
        mpls1_command = soc_mem_field32_get(unit, EDB_MPLS_PUSH_FORMATm, data, MPLS_ACTION_MPLS_1_CMDf);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        if (mpls1_command==ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
            *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_POP;
        } 
        else if (mpls1_command==ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
            *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_SWAP;
        }
        else 
        {
            *key_prefix=ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_MPLS_TUNNEL;
        }
    }
    
    if (SOC_IS_ARAD_A0(unit)) {
        
        if ((outlif == 0) && ((*key_prefix) != ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_DATA)) {
            LOG_INFO(BSL_LS_SOC_EGRESS,
                     (BSL_META_U(unit,
                                 "Notice: OutLIF 0 is being set to non DATA entry, this might affect Bridge traffic \n ")));
        }    
    }
    
    
    if (outlif == 0) {
        *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_NONE;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_key_prefix_type_get_internal()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_key_prefix_type_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE *key_prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  
  
  if (outlif < ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, 0)) {
      *key_prefix = ARAD_PP_EG_ENCAP_EEDB_ACCESS_TYPE_OUT_RIF;
      ARAD_DO_NOTHING_AND_EXIT;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));
  
  res = arad_pp_eg_encap_access_key_prefix_get_internal(unit, outlif, data, key_prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_key_prefix_type_get_unsafe()", outlif, 0);
}

#define ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE                      (1)

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t mem = EDB_MPLS_SWAP_FORMATm;
  uint32 push_command_upper_limit_val = 0;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT, 1);

  push_command_upper_limit_val = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_JER_B0_UPPER_LIMIT_val : ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val;

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));
  
  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_MPLS_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  

  tbl_data->drop = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_OAM_LIF_SETf);

  tbl_data->mpls1_command = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_MPLS_1_CMDf);

  tbl_data->next_vsi_lsb = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_VSI_LSBf);

  tbl_data->next_outlif = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_OUTLIF_PROFILEf);
  }


  if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
      
    mem = EDB_MPLS_POP_FORMATm;
    tbl_data->tpid_profile = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_TPID_PROFILEf);

    tbl_data->model_is_pipe = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_MODEL_IS_PIPEf);

    tbl_data->has_cw = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_CWf);

    tbl_data->upper_layer_protocol = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_UPPER_LAYER_PROTOCOLf);


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
    
    mem = EDB_MPLS_SWAP_FORMATm;
    tbl_data->mpls1_label = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_MPLS_1_LABELf);    

    if (soc_mem_field_valid(unit, mem, MPLS_ACTION_SWAP_REMARK_PROFILEf)) {
        tbl_data->mpls_swap_remark_profile = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_SWAP_REMARK_PROFILEf);
    }else {
        uint32         mpls_cmd_data[ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE];

        res = soc_mem_read(
                                            unit,
                                            EPNI_MPLS_CMD_PROFILEm,
                                            MEM_BLOCK_ANY,
                                            9, 
                                            mpls_cmd_data
                                            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        tbl_data->mpls_swap_remark_profile = soc_mem_field32_get(unit, EPNI_MPLS_CMD_PROFILEm, mpls_cmd_data, REMARK_PROFILEf);
    }
  } else if (tbl_data->mpls1_command <= push_command_upper_limit_val) {
    
      
    mem = EDB_MPLS_PUSH_FORMATm;
    tbl_data->mpls1_label = soc_mem_field32_get(unit, mem, data, MPLS_1_LABELf);

    tbl_data->mpls2_command = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_MPLS_2_CMD_LSBf);

    tbl_data->mpls2_label = soc_mem_field32_get(unit, mem, data, MPLS_ACTION_MPLS_2_LABELf);
  }
  else {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR, 150, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_mpls_tunnel_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t mem = EDB_MPLS_POP_FORMATm;
  uint32 push_command_upper_limit_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
 
  push_command_upper_limit_val = SOC_IS_JERICHO_B0_AND_ABOVE(unit) ? ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_JER_B0_UPPER_LIMIT_val : ARAD_PP_EG_ENCAP_MPLS1_COMMAND_PUSH_UPPER_LIMIT_val;

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_MPLS;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  soc_mem_field32_set(unit, mem, data, MPLS_ACTION_DROPf, tbl_data->drop);


  soc_mem_field32_set(unit, mem, data, MPLS_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);


  soc_mem_field32_set(unit, mem, data, MPLS_ACTION_MPLS_1_CMDf, tbl_data->mpls1_command);  


  soc_mem_field32_set(unit, mem, data, MPLS_ACTION_VSI_LSBf, tbl_data->next_vsi_lsb);    

  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, mem, data, MPLS_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, mem, data, MPLS_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }


  if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_POP_val) {
    
    mem = EDB_MPLS_POP_FORMATm;
    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_TPID_PROFILEf, tbl_data->tpid_profile);       


    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_MODEL_IS_PIPEf, tbl_data->model_is_pipe);


    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_CWf, tbl_data->has_cw);  


    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_UPPER_LAYER_PROTOCOLf, tbl_data->upper_layer_protocol);    


  } else if (tbl_data->mpls1_command == ARAD_PP_EG_ENCAP_MPLS1_COMMAND_SWAP_val) {
    
    mem = EDB_MPLS_SWAP_FORMATm;
    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_MPLS_1_LABELf, tbl_data->mpls1_label);    

    if (soc_mem_field_valid(unit, mem, MPLS_ACTION_SWAP_REMARK_PROFILEf)) {
        soc_mem_field32_set(unit, mem, data, MPLS_ACTION_SWAP_REMARK_PROFILEf, tbl_data->mpls_swap_remark_profile);
    }else {
        uint32         mpls_cmd_data[ARAD_PP_EPNI_MPLS_CMD_TABLE_ENTRY_SIZE];

        res = soc_mem_read(
                                            unit,
                                            EPNI_MPLS_CMD_PROFILEm,
                                            MEM_BLOCK_ANY,
                                            9, 
                                            mpls_cmd_data
                                            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        soc_mem_field32_set(unit, EPNI_MPLS_CMD_PROFILEm, mpls_cmd_data, REMARK_PROFILEf, tbl_data->mpls_swap_remark_profile);
        res = soc_mem_write(
                                              unit,
                                              EPNI_MPLS_CMD_PROFILEm,
                                              MEM_BLOCK_ANY,
                                              9,
                                              mpls_cmd_data
        );

    }
  } else if (tbl_data->mpls1_command <= push_command_upper_limit_val) {
    
    
    mem = EDB_MPLS_PUSH_FORMATm;
    soc_mem_field32_set(unit, mem, data, MPLS_1_LABELf, tbl_data->mpls1_label);


    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_MPLS_2_CMD_LSBf, tbl_data->mpls2_command);


    soc_mem_field32_set(unit, mem, data, MPLS_ACTION_MPLS_2_LABELf, tbl_data->mpls2_label);

  } else {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_MPLS1_COMMAND_OUT_OF_RANGE_ERR, 150, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1100, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_mpls_tunnel_format_tbl_set_unsafe()", outlif, 0);
}




uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t mem = EDB_IPV4_TUNNEL_FORMATm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1200, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_IP_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  
  tbl_data->drop = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_DROPf);


  tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_OAM_LIF_SETf);


  tbl_data->next_vsi_lsb = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_VSI_LSBf);


  tbl_data->next_outlif = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);


  tbl_data->ipv4_dst = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_IPV4_DSTf);


  tbl_data->ipv4_src_index = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_IPV4_SRC_INDEXf);

  
  tbl_data->ipv4_tos_index = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_IPV4_TOS_INDEXf);

  
  tbl_data->ipv4_ttl_index = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_IPV4_TTL_INDEXf);

  
  tbl_data->encapsulation_mode = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_ENC_MODEf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, IP_TUNNEL_ACTION_OUTLIF_PROFILEf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_ip_tunnel_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t mem = EDB_IPV4_TUNNEL_FORMATm;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
 
  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_IP;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_VSI_LSBf, tbl_data->next_vsi_lsb);  
  
  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));    
  

  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_IPV4_DSTf, tbl_data->ipv4_dst);
  

  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_IPV4_SRC_INDEXf, tbl_data->ipv4_src_index);
  
  
  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_IPV4_TTL_INDEXf, tbl_data->ipv4_ttl_index);
  
  
  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_IPV4_TOS_INDEXf, tbl_data->ipv4_tos_index);
  
  
  soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_ENC_MODEf, tbl_data->encapsulation_mode);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, mem, data, IP_TUNNEL_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1300, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_ip_tunnel_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 mac64;
  soc_mem_t mem = EDB_LINK_LAYER_OR_ARP_FORMATm;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1400, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_LL_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  
  tbl_data->drop = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_OAM_LIF_SETf);
  
  tbl_data->remark_profile = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_REMARK_PROFILEf);
  
  soc_mem_field64_get(unit, mem, data, ARP_LL_ACTION_MACf, &mac64);
  
  COMPILER_64_TO_32_HI(tbl_data->dest_mac[1], mac64);
  COMPILER_64_TO_32_LO(tbl_data->dest_mac[0], mac64);
  
  tbl_data->vid_valid = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_VSI_VALIDf);
  
  tbl_data->vid = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_VSIf);
  
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_NEXT_OUT_LIF_VALIDf);
  
  tbl_data->next_outlif_lsb = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_NEXT_OUT_LIF_LSBf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_OUTLIF_PROFILEf);
  }
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      tbl_data->native_ll = soc_mem_field32_get(unit, mem, data, ARP_LL_ACTION_NATIVE_LLf);
  }  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_link_layer_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 mac64;
  soc_mem_t mem = EDB_LINK_LAYER_OR_ARP_FORMATm;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_LINK_LAYER_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_LINK_LAYER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_REMARK_PROFILEf, tbl_data->remark_profile);  
  
  COMPILER_64_SET(mac64, tbl_data->dest_mac[1], tbl_data->dest_mac[0]);

  soc_mem_field64_set(unit, mem, data, ARP_LL_ACTION_MACf, mac64);

  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_VSI_VALIDf, tbl_data->vid_valid);
  

  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_VSIf, tbl_data->vid);
  
  
  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_NEXT_OUT_LIF_VALIDf, tbl_data->next_outlif_valid);
  
  
  soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_NEXT_OUT_LIF_LSBf, tbl_data->next_outlif_lsb);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      soc_mem_field32_set(unit, mem, data, ARP_LL_ACTION_NATIVE_LLf, tbl_data->native_ll);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1920, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_link_layer_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint64 data64;
  soc_mem_t mem = EDB_DATA_FORMATm;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

      
      res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
      }

  
  tbl_data->drop = soc_mem_field32_get(unit, mem, data, DATA_ACTION_DROPf);       
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, DATA_ACTION_OAM_LIF_SETf);
  
  soc_mem_field64_get(unit, mem, data, DATA_ACTION_DATAf, &data64);
  
  COMPILER_64_TO_32_HI(tbl_data->data[1], data64);
  COMPILER_64_TO_32_LO(tbl_data->data[0], data64);
 
  if (SOC_IS_JERICHO_PLUS(unit)) {
      uint32 tmp = 0;
      tbl_data->rsv_bits = soc_mem_field32_get(unit, mem, data, DATA_ACTION_RESERVED_BITf);
      tmp = soc_mem_field32_get(unit, mem, data, DATA_ACTION_RESERVEDf);
      tbl_data->rsv_bits |= tmp<<1;
  }
 
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, mem, data, DATA_ACTION_NEXT_OUTLIF_VALIDf);
  
  
  tbl_data->next_outlif = soc_mem_field32_get(unit, mem, data, DATA_ACTION_NEXT_OUTLIFf);
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, DATA_ACTION_OUTLIF_PROFILEf);
  }

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_data_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_DATA_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_error_t
    rv = SOC_E_NONE;
  uint64 data64;
  soc_mem_t mem = EDB_DATA_FORMATm;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_DATA_FORMAT_TBL_SET_UNSAFE);

  

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_DATA;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  soc_mem_field32_set(unit, mem, data, DATA_ACTION_DROPf, tbl_data->drop);

  soc_mem_field32_set(unit, mem, data, DATA_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);

  COMPILER_64_SET(data64, tbl_data->data[1], tbl_data->data[0]);
  soc_mem_field64_set(unit, mem, data, DATA_ACTION_DATAf, data64);

  
  soc_mem_field32_set(unit, mem, data, DATA_ACTION_NEXT_OUTLIF_VALIDf, tbl_data->next_outlif_valid);
  
  ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);
  soc_mem_field32_set(unit, mem, data, DATA_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, mem, data, DATA_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }

  if (SOC_IS_JERICHO_PLUS(unit)) {
      soc_mem_field32_set(unit, mem, data, DATA_ACTION_RESERVED_BITf, tbl_data->rsv_bits & 1);
      soc_mem_field32_set(unit, mem, data, DATA_ACTION_RESERVEDf, (tbl_data->rsv_bits >> 1 & 3));
  }

  SOC_DPP_ALLOW_WARMBOOT_WRITE(arad_pp_eg_encap_access_entry_write(unit, outlif, data), rv);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 110, exit);

exit:
  SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rv);
  if(rv != SOC_E_NONE) {
    LOG_ERROR(BSL_LS_SOC_EGRESS,
              (BSL_META_U(unit,
                          " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_data_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t 
    mem;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1600, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_TRILL))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }


  if (SOC_IS_JERICHO(unit)) {
    mem = EDB_TRILL_FORMAT_FULL_ENTRYm;
    
    tbl_data->drop = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_DROPf);
    tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_OUTLIF_PROFILEf);
    tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_OAM_LIF_SETf);
    if( SOC_IS_JERICHO_A0(unit) || SOC_IS_JERICHO_B0(unit)) {
      tbl_data->remark_profile = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_RESERVED_BIITSf);
    }
    tbl_data->m = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_M_BITf);
    tbl_data->nick = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_NICK_BITSf);
    tbl_data->next_outlif_valid = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_NEXT_OUTLIF_VALID_BITf);
    tbl_data->next_outlif = soc_mem_field32_get(unit, mem, data, TRILL_ACTION_NEXT_OUTLIF_BITSf); 
  } else {
    mem = EPNI_TRILL_FORMATm;
    tbl_data->drop = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_DROPf);
    tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_OAM_LIF_SETf);
    tbl_data->my_nickname_index = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_MY_NICK_INDEXf);
    tbl_data->m = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_Mf);
    tbl_data->nick = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_NICKf);
    tbl_data->next_outlif_valid = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_NEXT_OUTLIF_VALIDf);
    tbl_data->next_outlif = soc_mem_field32_get(unit, mem, data, TRILL_0_ACTION_NEXT_OUTLIFf);
  }
  ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_HW_TO_SW(unit, tbl_data->next_outlif);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_trill_entry_format_tbl_get_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_TRILL_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  soc_mem_t 
    mem;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_TRILL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1700, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_TRILL;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (SOC_IS_JERICHO(unit)) {
    mem = EDB_TRILL_FORMAT_FULL_ENTRYm;
    soc_mem_field32_set(unit, mem, data, TRILL_ENTRY_FORMATf, TRUE);
    
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_DROPf, tbl_data->drop);
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
    if( SOC_IS_JERICHO_A0(unit) || SOC_IS_JERICHO_B0(unit)) {
      soc_mem_field32_set(unit, mem, data, TRILL_ACTION_RESERVED_BIITSf,tbl_data->remark_profile);
    }
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_M_BITf, tbl_data->m);
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_NICK_BITSf, tbl_data->nick);
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_NEXT_OUTLIF_VALID_BITf, tbl_data->next_outlif_valid);
    ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);    
    soc_mem_field32_set(unit, mem, data, TRILL_ACTION_NEXT_OUTLIF_BITSf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));
  } else {
    mem = EPNI_TRILL_FORMATm;
    soc_mem_field32_set(unit, mem, data, TRILL_0_ENTRY_FORMATf, TRUE);
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_DROPf, tbl_data->drop); 
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_MY_NICK_INDEXf, tbl_data->my_nickname_index);
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_Mf, tbl_data->m);
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_NICKf, tbl_data->nick);
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_NEXT_OUTLIF_VALIDf, tbl_data->next_outlif_valid);
    ARAD_PP_EG_ENCAP_ACCESS_VERIFY_NEXT_OUTLIF(unit, tbl_data->next_outlif);    
    soc_mem_field32_set(unit, mem, data, TRILL_0_ACTION_NEXT_OUTLIFf, ARAD_PP_EG_ENCAP_ACCESS_ADJUST_NEXT_OUTLIF_SW_TO_HW(unit, tbl_data->next_outlif));
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1710, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_trill_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT, 1);

  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1810, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }

  
  tmp = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf);
  

  
  if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_RIF(tmp))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 30, exit)
  }

  
  tbl_data->drop = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_DROPf : OUT_RIF_1_ACTION_DROPf);
  

  tbl_data->oam_lif_set = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OAM_LIF_SETf : OUT_RIF_1_ACTION_OAM_LIF_SETf);
  

  tbl_data->remark_profile = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_REMARK_PROFILEf : OUT_RIF_1_ACTION_REMARK_PROFILEf);
  
  
  tbl_data->next_vsi_lsb = (SOC_IS_JERICHO(unit)) ? outlif : 
        soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_VSI_LSBSf : OUT_RIF_1_ACTION_VSI_LSBSf);
  
  
  tbl_data->next_outlif_valid = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTR_VALIDf : OUT_RIF_1_ACTION_ARP_PTR_VALIDf);
  
  tbl_data->next_outlif = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTRf : OUT_RIF_1_ACTION_ARP_PTRf);
  
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, EDB_OUT_RIF_FORMATm, data, (is_location_at_start) ? OUT_RIF_0_ACTION_OUTLIF_PROFILEf : OUT_RIF_1_ACTION_OUTLIF_PROFILEf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe()", outlif, 0);
}


STATIC uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_in_buffer(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint32                                         outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT   *tbl_data,
    SOC_SAND_INOUT uint32                                       *raw_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0;
  uint8
    is_location_at_start;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE);

  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

  
  tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER;
  res = soc_sand_bitstream_set_any_field(&tmp, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), raw_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_RIF_FORMAT;

  
  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ENTRY_FORMATf : OUT_RIF_1_ENTRY_FORMATf, tmp);
  

  
  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_DROPf : OUT_RIF_1_ACTION_DROPf, tbl_data->drop);       
  

  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_OAM_LIF_SETf : OUT_RIF_1_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
  

  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_REMARK_PROFILEf : OUT_RIF_1_ACTION_REMARK_PROFILEf, tbl_data->remark_profile);  
  
  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_VSI_LSBSf : OUT_RIF_1_ACTION_VSI_LSBSf, tbl_data->next_vsi_lsb);  
  }
  

  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTR_VALIDf : OUT_RIF_1_ACTION_ARP_PTR_VALIDf, tbl_data->next_outlif_valid);
  
  soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_ARP_PTRf : OUT_RIF_1_ACTION_ARP_PTRf, tbl_data->next_outlif);

  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EDB_OUT_RIF_FORMATm, raw_data, (is_location_at_start) ? OUT_RIF_0_ACTION_OUTLIF_PROFILEf : OUT_RIF_1_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_internal()", outlif, 0);
}


uint32
  arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1910, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  res = arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_in_buffer(unit, outlif, tbl_data, data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1920, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

  
  if ((SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "roo_remark_profile", 0)) {
    int 
      core_id = 0;
    uint32
      outrif_offset,
      outrif_idx,
      bits_val;
    ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA
      tx_tag_tbl_data;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
        outrif_offset = outlif >> 4; 
        outrif_idx = outlif & 0xf; 
        res = arad_pp_epni_tx_tag_table_tbl_get_unsafe(
              unit,
              core_id,
              outrif_offset,
              &tx_tag_tbl_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        bits_val = tbl_data->remark_profile;
        res = soc_sand_bitstream_set_any_field(&bits_val, outrif_idx * 4, 4, tx_tag_tbl_data.entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        res = arad_pp_epni_tx_tag_table_tbl_set_unsafe(
                unit,
                core_id,
                outrif_offset,
                &tx_tag_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_OUT ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
    res = SOC_SAND_OK;
  uint32
    tmp = 0, ent_access_prefix = 0,
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];
  uint8
    is_location_at_start;
  uint64 data64;
  soc_mem_t mem = EDB_AC_HALF_ENTRY_FORMATm;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT, 1);

  is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2020, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  
  res = soc_sand_bitstream_get_any_field(data, ARAD_PP_EG_ENCAP_ACCESS_PREFIX_LSB(unit), ARAD_PP_EG_ENCAP_ACCESS_PREFIX_NOF_BITS(unit), &tmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  if ((tmp & ARAD_PP_EG_ENCAP_ACCESS_OTHER_ENTRY_PREFIX_MASK) != (ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 10, exit);
  }


  
  if (SOC_IS_JERICHO_AND_BELOW(unit)) {
      
      ent_access_prefix = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf);

      
      if (!ARAD_PP_EG_ENCAP_ACCESS_EEDB_OTHER_FORMAT_HALF_ENTRY_IS_AC(ent_access_prefix))
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_EG_ENCAP_ACCESS_ENTRY_TYPE_MISMATCH_ERR, 30, exit);
      }
  }

  
  tbl_data->drop = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_DROPf : AC_1_ACTION_DROPf);
  
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_OAM_LIF_SETf : AC_1_ACTION_OAM_LIF_SETf);
  
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_VLAN_EDIT_PROFILEf : AC_1_ACTION_VLAN_EDIT_PROFILEf);
  
  tbl_data->vid[0] = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_VID_1f : AC_1_ACTION_VID_1f);
    
  tbl_data->vid[1] = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_VID_2f : AC_1_ACTION_VID_2f);
  
  tbl_data->pcp_dei_profile = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_PCP_DEI_PROFILEf : AC_1_ACTION_PCP_DEI_PROFILEf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->outlif_profile = soc_mem_field32_get(unit, mem, data, (is_location_at_start) ? AC_0_ACTION_OUTLIF_PROFILEf : AC_1_ACTION_OUTLIF_PROFILEf);
  }

      

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit) && is_location_at_start) {
          soc_field_t use_as_data_field = SOC_IS_JERICHO_PLUS(unit) ? AC_PLUS_DATA_ENTRYf : AC_ACTION_USE_AS_DATA_BITf;
          
          mem = EDB_AC_ENTRY_WITH_DATA_FORMATm;
          tbl_data->use_as_data_entry = soc_mem_field32_get(unit, mem, data, use_as_data_field);

          if (tbl_data->use_as_data_entry) {
              soc_mem_field64_get(unit, mem, data, AC_ACTION_DATAf, &data64);
              COMPILER_64_TO_32_HI(tbl_data->data[1], data64);
              COMPILER_64_TO_32_LO(tbl_data->data[0], data64);
          }
      }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_get_unsafe()", outlif, 0);
}



STATIC uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_in_buffer(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint32                                         outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT    *tbl_data,
    SOC_SAND_INOUT uint32                                       *raw_data
  )
{
    uint32
      tmp = 0,
      previous_prefix = 0;
    uint8
      is_location_at_start;
#ifdef BCM_88660_A0
    uint32
         is_big_ac_entry=0;
    uint64 data64;
    soc_mem_t data_entry_mem = EDB_AC_ENTRY_WITH_DATA_FORMATm;
    soc_field_t use_as_data_field = SOC_IS_JERICHO_PLUS(unit) ? AC_PLUS_DATA_ENTRYf : AC_ACTION_USE_AS_DATA_BITf;
#endif
    soc_mem_t mem = EDB_AC_HALF_ENTRY_FORMATm;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE);

    is_location_at_start = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_IS_LOCATION_START(outlif);

    if ((SOC_IS_ARADPLUS(unit)) && (tbl_data->use_as_data_entry) && (is_location_at_start == 0)) {
         SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 12, exit);
    }

    if (SOC_IS_JERICHO_PLUS(unit)) {
        previous_prefix = soc_mem_field32_get(unit, mem, raw_data, AC_ENTRY_FORMATf);
        if (previous_prefix == ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE) {
            soc_mem_field32_set(unit, mem, raw_data, AC_1_ACTION_DROPf, 0);
        }
    }
  
#ifdef BCM_88660_A0
    
    if (SOC_IS_ARADPLUS(unit)) {

        
        if (tbl_data->use_as_data_entry)
        {
            is_big_ac_entry = 1;
        } else if (ARAD_PP_EG_ENCAP_ACCESS_IS_BIG_AC_LOCATION(outlif)) {
            previous_prefix = soc_mem_field32_get(unit, mem, raw_data, AC_ENTRY_FORMATf);
            if (previous_prefix == ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER) {
                is_big_ac_entry = soc_mem_field32_get(unit, data_entry_mem, raw_data, use_as_data_field);
            }
        } else { 
            soc_mem_field32_set(unit, data_entry_mem, raw_data, use_as_data_field, is_big_ac_entry);
        }
    }
#endif

    
    tmp = ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER;
    soc_mem_field32_set(unit, mem, raw_data, AC_ENTRY_FORMATf, tmp);
    
   
    
    if (SOC_IS_JERICHO_AND_BELOW(unit)) {
        soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ENTRY_FORMATf : AC_1_ENTRY_FORMATf, ARAD_PP_EG_ENCAP_ACCESS_2_ENT_PREFIX_TYPE_OUT_AC);
    }
   
    
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_DROPf : AC_1_ACTION_DROPf, tbl_data->drop);
    
   
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_OAM_LIF_SETf : AC_1_ACTION_OAM_LIF_SETf, tbl_data->oam_lif_set);
    
   
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_VLAN_EDIT_PROFILEf : AC_1_ACTION_VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
    
    
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_VID_1f : AC_1_ACTION_VID_1f, (tbl_data->vid[0]));
    
   
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_VID_2f : AC_1_ACTION_VID_2f, (tbl_data->vid[1]));
    
    
    soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_PCP_DEI_PROFILEf : AC_1_ACTION_PCP_DEI_PROFILEf, tbl_data->pcp_dei_profile);
   
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && SOC_IS_JERICHO_AND_BELOW(unit)) {
        soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_USE_AS_DATA_BITf : AC_1_ACTION_USE_AS_DATA_BITf, tbl_data->use_as_data_entry);
    }
#endif 

    if (SOC_IS_JERICHO(unit)) {
        soc_mem_field32_set(unit, mem, raw_data, (is_location_at_start) ? AC_0_ACTION_OUTLIF_PROFILEf : AC_1_ACTION_OUTLIF_PROFILEf, tbl_data->outlif_profile);
    }
 
#ifdef BCM_88660_A0
    
    if (is_big_ac_entry == 1)
    {
        COMPILER_64_SET(data64, tbl_data->data[1], tbl_data->data[0]);

        
        soc_mem_field32_set(unit, data_entry_mem, raw_data, use_as_data_field, tbl_data->use_as_data_entry);

        soc_mem_field64_set(unit, data_entry_mem, raw_data, AC_ACTION_DATAf, data64);  
    }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe()", outlif, 0);
}



soc_error_t 
  arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set(
     int     unit, 
     uint32  vlan_edit_profile
     ) {
    uint64 reg_val;
    uint32 data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    SOCDNX_IF_ERR_EXIT(READ_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, &reg_val)); 

    
    COMPILER_64_TO_32_LO(data[0], reg_val); 
    COMPILER_64_TO_32_HI(data[1], reg_val); 

    
    soc_mem_field32_set(unit, 
                        EDB_AC_HALF_ENTRY_FORMATm, 
                        data, 
                        AC_0_ACTION_VLAN_EDIT_PROFILEf, 
                        vlan_edit_profile);

    
    COMPILER_64_SET(reg_val, data[1], data[0]); 

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, reg_val)); 

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
  arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get(
     int      unit, 
     uint32*  vlan_edit_profile
     ) {
    uint64 reg_val;
    uint32 data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    SOCDNX_IF_ERR_EXIT(READ_EPNI_DEFAULT_AC_ENTRYr(unit, REG_PORT_ANY, &reg_val)); 

    
    COMPILER_64_TO_32_LO(data[0], reg_val); 
    COMPILER_64_TO_32_HI(data[1], reg_val); 

    
    *vlan_edit_profile = soc_mem_field32_get(unit, 
                            EDB_AC_HALF_ENTRY_FORMATm, 
                            data, 
                            AC_0_ACTION_VLAN_EDIT_PROFILEf);
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              outlif,
    SOC_SAND_IN  ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_OUT_AC_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2120, exit, arad_pp_eg_encap_access_entry_read(unit, outlif, data));

  res = arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_in_buffer(unit, outlif, tbl_data, data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 2130, exit, arad_pp_eg_encap_access_entry_write(unit, outlif, data));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
  arad_pp_eg_encap_access_local_outlif_additional_data_location_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int outlif, 
    SOC_SAND_OUT int *bank_id, 
    SOC_SAND_OUT int *entry_id 
  )
{
    int res, eedb_bank_id, entry_offset; 
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENCAP_ACCESS_LOCAL_OUTLIF_ADDITIONAL_DATA_LOCATION_GET);

    entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, outlif);

    eedb_bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_BANK_ID(unit,entry_offset);

    res = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.get(unit, eedb_bank_id*2, &eedb_bank_info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

    *bank_id =  (ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS(unit) + eedb_bank_info.extension_bank / ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK );
   
    *entry_id = (ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_INTERNAL_OFFSET(unit, entry_offset) / ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK)
            + ( ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_EEDB_BANK(unit) /ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK 
                * ( eedb_bank_info.extension_bank % ARAD_PP_EG_ENCAP_NOF_TOP_BANKS_PER_EEDB_BANK )  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_encap_access_out_ac_entry_format_tbl_set_unsafe()", outlif, 0);
}

uint32
arad_pp_eg_enacp_access_additional_data_entry_get(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  )
{

   uint32   
    res = SOC_SAND_OK,
    entry_offset,
    internal_offset,
    bank_id;

   SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_GET);

   entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, outlif);

   bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_ID(unit, entry_offset);
   internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_INTERNAL_OFFSET(unit, entry_offset);

   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, soc_mem_array_read(unit, EDB_EEDB_TOP_BANKm, bank_id, MEM_BLOCK_ANY, internal_offset, data));
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_enacp_access_additional_data_entry_get()", outlif, 0);
}

uint32
arad_pp_eg_enacp_access_additional_data_entry_set(    
    SOC_SAND_IN int unit, 
    SOC_SAND_IN  uint32 outlif,
    SOC_SAND_OUT uint32 *data
  )
{
   uint32
    res = SOC_SAND_OK,
    entry_offset,
    internal_offset,
    bank_id;

   SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EG_ENACP_ACCESS_ADDITIONAL_DATA_ENTRY_SET);

   entry_offset = ARAD_PP_EG_ENCAP_ACCESS_OUT_LIF_TO_OFFSET(unit, outlif);

   bank_id = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_ID(unit, entry_offset);
   internal_offset = ARAD_PP_EG_ENCAP_ACCESS_OFFSET_TO_TOP_BANK_INTERNAL_OFFSET(unit, entry_offset);

   SOC_SAND_SOC_IF_ERROR_RETURN(res, 1500, exit, soc_mem_array_write(unit, EDB_EEDB_TOP_BANKm, bank_id, MEM_BLOCK_ANY, internal_offset, data));
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_eg_enacp_access_additonal_data_entry_set()", outlif, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_eg_encap_access;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_eg_encap_access_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_eg_encap_access;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


