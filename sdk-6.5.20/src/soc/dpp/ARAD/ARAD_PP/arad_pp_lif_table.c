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

#include <soc/mem.h>
#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_table.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>






#define ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_MAX                    (SOC_SAND_U32_MAX)













CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_lif_table[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_LIF_TABLE_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_lif_table[] =
{
  
  {
    ARAD_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_LIF_ENTRY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR,
    "ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR",
    "The parameter 'entries_type_bm' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR,
    "ARAD_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR",
    "when update entry using arad_pp_lif_table_entry_update_unsafe. \n\r "
    "information type has to be same as entry type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_lif_table_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_init_unsafe()", 0, 0);
}




uint32
  arad_pp_lif_table_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                      *block_range,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    entry_ndx,
    entries_to_scan,
    nof_valid_entries = 0;
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO
    accessed_info;
  uint8
    match=FALSE;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  
  if (block_range->iter > SOC_DPP_DEFS_GET(unit, nof_local_lifs) - 1)
  {
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  entries_to_scan = block_range->entries_to_scan;

  if (entries_to_scan == SOC_SAND_TBL_ITER_SCAN_ALL)
  {
    entries_to_scan = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
  }
  

  for (entry_ndx = block_range->iter; entry_ndx < block_range->iter + entries_to_scan; ++entry_ndx)
  {
    if (nof_valid_entries >= block_range->entries_to_act || entry_ndx > SOC_DPP_DEFS_GET(unit, nof_local_lifs) - 1)
    {
      
      break;
    }

    res = arad_pp_lif_table_entry_get_unsafe(
            unit,
            entry_ndx,
            &entries_array[nof_valid_entries]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (entries_array[nof_valid_entries].type & rule->entries_type_bm)
    {
      if (rule->accessed_only)
      {
        res = arad_pp_lif_table_entry_accessed_info_get_unsafe(unit,entry_ndx,FALSE,&accessed_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
        if (accessed_info.accessed)
        {
          match = TRUE;
        }
        else
        {
          match = FALSE;
        }
      }
      else
      {
        match = TRUE;
      }
    }
    else
    {
      match = FALSE;
    }
    if (match)
    {
      ++nof_valid_entries;
    }
  }
  *nof_entries = nof_valid_entries;
  block_range->iter = entry_ndx;
  if (block_range->iter > SOC_DPP_DEFS_GET(unit, nof_local_lifs) - 1)
  {
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_lif_table_get_block_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_GET_BLOCK_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE, rule, 10, exit);
  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_get_block_verify()", 0, 0);
}


uint32
  arad_pp_lif_table_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                  lif_ndx,
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  SOC_PPC_LIF_ENTRY_TYPE
    entry_type;
  uint8
    found;
  SOC_PPC_LIF_ID
    lif_ndex = lif_ndx;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_RIF_IP_TERM_KEY
    term_key;
  SOC_PPC_SYS_VSI_ID
      vsi_index;
  uint32
      sub_type;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO	pwe_additional_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_GET_UNSAFE);

  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  SOC_PPC_LIF_ENTRY_INFO_clear(lif_entry_info);
  SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);

  res = arad_pp_sw_db_lif_table_entry_use_get(
          unit,
          lif_ndx,
          &entry_type,
          &sub_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  lif_entry_info->type = entry_type;
  lif_entry_info->index = lif_ndx;
  switch (entry_type)
  {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
      res = arad_pp_l2_lif_ac_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &(lif_entry_info->value.ac),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
      res = arad_pp_l2_lif_pwe_get_internal_unsafe(
              unit,
              0,
              TRUE,
              &lif_ndex,
			  &pwe_additional_info,
              &(lif_entry_info->value.pwe),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
      res = arad_pp_rif_ip_tunnel_term_get_internal_unsafe(
              unit,
              &term_key,
              TRUE,
              &lif_ndex,
              &(lif_entry_info->value.ip_term_info),
              &(lif_entry_info->value.rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
      res = arad_pp_rif_mpls_label_map_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &(lif_entry_info->value.mpls_term_info),
              &(lif_entry_info->value.rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_ISID:
      res = arad_pp_l2_lif_isid_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &vsi_index,
              &(lif_entry_info->value.isid),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
      res = arad_pp_l2_lif_trill_uc_get_internal_unsafe(
               unit,
               0,
               TRUE,
               &lif_ndex,
               &(lif_entry_info->value.trill),
               &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
      res = arad_pp_l2_lif_extender_get(
               unit,
               0,
               &lif_ndex,
               &(lif_entry_info->value.extender),
               &found);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;
    default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_entry_get_unsafe()", lif_ndx, 0);
}

uint32
  arad_pp_lif_table_entry_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_ndx, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_entry_get_verify()", lif_ndx, 0);
}



uint32
  arad_pp_lif_table_entry_update_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                                  lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  SOC_PPC_LIF_ENTRY_TYPE
    entry_type;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PPC_RIF_IP_TERM_KEY
    term_key;
  SOC_PPC_MPLS_LABEL_RIF_KEY
    mpls_term_key;
  SOC_PPC_LIF_ID
    lif_indx=lif_ndx;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_L2_LIF_ISID_INFO
      tmp_isid;
  SOC_PPC_SYS_VSI_ID
      vsi_index;
  uint32
      sub_type;
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO        pwe_additional_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);
  SOC_PPC_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  res = arad_pp_sw_db_lif_table_entry_use_get(
          unit,
          lif_ndx,
          &entry_type,
          &sub_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 05, exit);

  
  if (entry_type != lif_entry_info->type && entry_type != SOC_PPC_LIF_ENTRY_TYPE_EMPTY)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR, 10, exit);
  }

  switch (lif_entry_info->type)
  {
    case SOC_PPC_LIF_ENTRY_TYPE_AC:
    case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
      res = arad_pp_l2_lif_ac_add_internal_unsafe(
              unit,
              NULL,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.ac),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_PWE:
      res = arad_pp_l2_lif_pwe_add_internal_unsafe(
              unit,
              0,
              TRUE,
              lif_ndx,
			  &pwe_additional_info,
              &(lif_entry_info->value.pwe),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:

      

      res = arad_pp_rif_ip_tunnel_term_add_internal_unsafe(
              unit,
              &term_key,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.ip_term_info),
              &(lif_entry_info->value.rif),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:

      res = arad_pp_rif_mpls_label_map_add_internal_unsafe(
              unit,
              &mpls_term_key,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.mpls_term_info),
              NULL,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;
    case SOC_PPC_LIF_ENTRY_TYPE_ISID:
      
      res = arad_pp_l2_lif_isid_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_indx,
              &vsi_index,
              &(tmp_isid),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      res = arad_pp_l2_lif_isid_add_internal_unsafe(
              unit,
              vsi_index,
              NULL,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.isid),
              TRUE,
              TRUE,
              0,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      break;
  case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
      res = arad_pp_l2_lif_trill_add_internal_unsafe(
          unit,
          0,
          TRUE,
          lif_ndx,
          &(lif_entry_info->value.trill),
          &success);
      SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
      break;
    case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
        res = arad_pp_l2_lif_extender_add_internal_unsafe(
                unit,
                0,
                TRUE,
                lif_ndx,
                &(lif_entry_info->value.extender),
                &success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        break;
    case SOC_PPC_LIF_ENTRY_TYPE_EMPTY:
      res = arad_pp_sw_db_lif_table_entry_use_set(
              unit,
              lif_ndx,
              SOC_PPC_LIF_ENTRY_TYPE_EMPTY,
              0
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    default:
      break;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_entry_update_unsafe()", lif_ndx, 0);
}

uint32
  arad_pp_lif_table_entry_update_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_ndx, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_LIF_ENTRY_INFO_verify(unit, lif_entry_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_entry_update_verify()", lif_ndx, 0);
}


uint32
  arad_pp_lif_table_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID                           lif_ndx,
    SOC_SAND_IN  uint8                              clear_access_stat,
    SOC_SAND_OUT SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO    *accessed_info
  )
{
    uint32 write_access_enable;
  uint32
    tmp,
    entry_indx,
    data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_ndx, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 5, exit);

  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(accessed_info);

  entry_indx = lif_ndx /32;

  res = READ_IHP_LIF_ACCESSEDm(unit,
                               entry_indx/SOC_DPP_IMP_DEFS_GET(unit, ihp_lif_accessed_nof_lines),
                               MEM_BLOCK_ANY,
                               entry_indx % SOC_DPP_IMP_DEFS_GET(unit, ihp_lif_accessed_nof_lines),
                               &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  

  tmp = SOC_SAND_GET_BIT(data,(lif_ndx) % 32);
  accessed_info->accessed = SOC_SAND_NUM2BOOL(tmp);

  
  if (clear_access_stat)
  {
    tmp = 0;
    res = soc_sand_set_field(
            &(data),
            lif_ndx % 32,
            lif_ndx % 32,
            tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (SOC_IS_JERICHO(unit)) {
        
        READ_IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, &write_access_enable);
        if (0 == write_access_enable) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1));
            write_access_enable = 1;
        }
    }

    res = WRITE_IHP_LIF_ACCESSEDm(unit,
                                  entry_indx/SOC_DPP_IMP_DEFS_GET(unit, ihp_lif_accessed_nof_lines),
                                  MEM_BLOCK_ANY,
                                  entry_indx % SOC_DPP_IMP_DEFS_GET(unit, ihp_lif_accessed_nof_lines),
                                  &data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);
    
    if (SOC_IS_JERICHO(unit) && write_access_enable) {
         SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 0));
    }

  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_lif_table_entry_accessed_info_get_unsafe()", lif_ndx, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lif_table_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_lif_table;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lif_table_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_lif_table;
}

uint32
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO    *info,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE             type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (type)
  {
  case SOC_PPC_LIF_ENTRY_TYPE_AC:
  case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
  case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
    res = SOC_PPC_L2_LIF_AC_INFO_verify(unit, &(info->ac));
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_PWE:
    res = SOC_PPC_L2_LIF_PWE_INFO_verify(unit, &(info->pwe));
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_ISID:
    res = SOC_PPC_L2_LIF_ISID_INFO_verify(unit, &(info->isid));
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
  case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
    res = SOC_PPC_RIF_INFO_verify(unit, &(info->rif));
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
    res = SOC_PPC_L2_LIF_TRILL_INFO_verify(unit, &(info->trill));
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
    res = SOC_PPC_L2_LIF_EXTENDER_INFO_verify(unit, &(info->extender));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  break;
  default:
    break;
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_ENTRY_INFO_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_verify(unit, &(info->value),info->type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  SOC_SAND_ERR_IF_ABOVE_NOF(info->index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_type_bm, ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_MAX, ARAD_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

