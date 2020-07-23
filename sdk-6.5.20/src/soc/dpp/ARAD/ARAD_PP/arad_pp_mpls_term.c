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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MPLS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/drv.h>





#define ARAD_PP_MPLS_TERM_RANGE_NDX_MAX                          (2)
#define SOC_PPC_MPLS_TERM_KEY_TYPE_MAX                           (SOC_PPC_NOF_MPLS_TERM_KEY_TYPES-1)
#define ARAD_PP_MPLS_TERM_RANGES_NOF_BITS                        (16*1024)
#define ARAD_PP_MPLS_TERM_MODEL_MAX                              (SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1)
#define ARAD_PP_MPLS_TERM_PROFILE_MAX                            (7)
#define ARAD_PP_MPLS_TERM_ACTION_PROFILE_MAX                     (15)

#define ARAD_PP_MPLS_RANGE_NDX_ELI                               (0)
#define ARAD_PP_MPLS_RANGE_NDX_GAL                               (2)

#define ARAD_PP_MPLS_RANGE_NDX_IPV4                               (0)
#define ARAD_PP_MPLS_RANGE_NDX_IPV6                               (1)













CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_mpls_term[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LKUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_MPLS_TERM_COS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MPLS_TERM_GET_ERRS_PTR),
  
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_IHP_MPLS_TUNNEL_TERMINATION_UPDATE_BASES),
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_mpls_term[] =
{
  
  {
    ARAD_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'range_ndx' is out of range. \n\r "
    "The range is: 0 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR",
    "The parameter 'is_terminated_label' is out of range. \n\r "
    "The range is: 0 - No max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MPLS_TERM_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'processing_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR",
    "The parameter 'model' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'snoop_action_strength' or. \n\r "
    "'frwrd_action_strength' is out of range. \n\r "
    "The range is: 0 - 0 set using soc_ppd_trap_mgmt apis\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR",
    "The parameter 'trap_code' for reserved labels\n\r "
    "BOS/non-BOS is out of range \n\r"
    "The range is: SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0(60) - SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3(63)",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR",
    " calling arad_pp_mpls_term_range_terminated_label_set/get \n\r"
    " The parameter 'label_ndx' doesn't match any range\n\r "
    "use arad_pp_mpls_term_label_range_get to see valid range",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR,
    "ARAD_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR",
    " calling arad_pp_mpls_term_label_range_set \n\r"
    " where next protocol is not MPLS label \n\r "
    " ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR,
    "ARAD_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR",
    " calling arad_pp_mpls_term_label_range_set \n\r"
    " where exceed nof entries in range\n\r "
    " total size of ranges is 16K",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR,
    "ARAD_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR",
    " calling arad_pp_mpls_term_label_range_set \n\r"
    " where last label is smaller than first label \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR,
    "ARAD_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR",
    " calling arad_pp_mpls_term_label_range_set \n\r"
    " while not keeping order, ranges in lower index \n\r "
    " has to include lower values\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'term_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'action_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MPLS_TERM_NOF_HEADERS_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_TERM_NOF_HEADERS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_headers' is out of range. \n\r "
    "The range is: 1 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
	ARAD_PP_MPLS_TERM_TTL_EXP_OUT_OF_RANGE_ERR,
    "ARAD_PP_MPLS_TERM_TTL_EXP_OUT_OF_RANGE_ERR",
    "The parameter 'ttl_exp_ndx' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





uint32
  arad_pp_mpls_term_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_MPLS_TERM_PROFILE_INFO
    term_profile_info;
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO
    mpls_label_range_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&term_profile_info);
  res = arad_pp_mpls_term_profile_info_set_unsafe(unit,0,&term_profile_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
  mpls_label_range_info.range.first_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
  mpls_label_range_info.range.last_label  = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
  res = arad_pp_mpls_term_label_range_set_unsafe(unit,ARAD_PP_MPLS_RANGE_NDX_GAL,&mpls_label_range_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (!(SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20 && SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23)) {
   if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    
    SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
    mpls_label_range_info.range.first_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
    mpls_label_range_info.range.last_label  = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
    res = arad_pp_mpls_term_label_range_set_unsafe(unit,ARAD_PP_MPLS_RANGE_NDX_ELI,&mpls_label_range_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
   }

  if (SOC_IS_JERICHO(unit)) {
    
    SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
    mpls_label_range_info.range.first_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP;
    mpls_label_range_info.range.last_label  = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP;
    res = arad_pp_mpls_term_label_range_set_unsafe(unit,ARAD_PP_MPLS_RANGE_NDX_IPV4,&mpls_label_range_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
    mpls_label_range_info.range.first_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP;
    mpls_label_range_info.range.last_label  = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP;
    res = arad_pp_mpls_term_label_range_set_unsafe(unit,ARAD_PP_MPLS_RANGE_NDX_IPV6,&mpls_label_range_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
   }
  }
  else {
	
	SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(&mpls_label_range_info);
	mpls_label_range_info.range.first_label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
	mpls_label_range_info.range.last_label  = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
	res = arad_pp_mpls_term_label_range_set_unsafe(unit,ARAD_PP_MPLS_RANGE_NDX_ELI,&mpls_label_range_info);
	SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_init_unsafe()", 0, 0);
}



uint32
  arad_pp_mpls_term_lkup_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LKUP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_lkup_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LKUP_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_LKUP_INFO, lkup_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_set_verify()", 0, 0);
}

uint32
  arad_pp_mpls_term_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LKUP_INFO_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_get_verify()", 0, 0);
}


uint32
  arad_pp_mpls_term_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    with_in_rif;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LKUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(lkup_info);

  res = arad_pp_isem_access_prog_sel_in_rif_key_get_unsafe(
         unit,
         &with_in_rif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  if (with_in_rif)
  {
    lkup_info->key_type = SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF;
  }
  else
  {
    lkup_info->key_type = SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_label_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO        *label_range_info
  )
{
  uint32
    old_low,
    temp;
  int32
    diff = 0,
    old_diff = 0 ;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(label_range_info);

  diff = (label_range_info->range.last_label - label_range_info->range.first_label);

  res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, range_ndx, &temp);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  old_diff = temp;
  res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, range_ndx, &old_low);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  old_diff -= old_low;
  diff -= old_diff;
  
  res = WRITE_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, range_ndx, label_range_info->range.first_label);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  res = WRITE_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, range_ndx, label_range_info->range.last_label);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_range_set_unsafe()", range_ndx, 0);
}

uint32
  arad_pp_mpls_term_label_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO        *label_range_info
  )
{
  uint32
    rng_indx;
  uint32
    temp,
    temp2 = 0;
  uint32
    nof_bits = 0,
    total_bits = 0;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, ARAD_PP_MPLS_TERM_RANGE_NDX_MAX, ARAD_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_verify(unit, label_range_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 
  for (rng_indx = 0; rng_indx <= ARAD_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
    if (rng_indx == range_ndx)
    {
      nof_bits = label_range_info->range.last_label - label_range_info->range.first_label + 1;
    }
    else
    {
      res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, range_ndx, &temp);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      nof_bits = temp + 1;
      res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, range_ndx, &temp);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      nof_bits -= temp;
    }
    total_bits += nof_bits;
   

    res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, range_ndx, &temp);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, range_ndx, &temp);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

    
    if (rng_indx == range_ndx || temp == temp2 || label_range_info->range.first_label == label_range_info->range.last_label)
    {
      continue;
    }
    else if (rng_indx < range_ndx)
    {
      res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, range_ndx, &temp);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      if (label_range_info->range.first_label < temp)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR, 45, exit);
      }
    }
    else if (rng_indx > range_ndx)
    {
      res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, range_ndx, &temp);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      if (label_range_info->range.last_label > temp)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR, 55, exit);
      }
    }
  }

  if (total_bits > ARAD_PP_MPLS_TERM_RANGES_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_range_set_verify()", range_ndx, 0);
}

uint32
  arad_pp_mpls_term_label_range_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_ndx, ARAD_PP_MPLS_TERM_RANGE_NDX_MAX, ARAD_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_range_get_verify()", range_ndx, 0);
}


uint32
  arad_pp_mpls_term_label_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO        *label_range_info
  )
{
  SOCDNX_INIT_FUNC_DEFS;

                             
  SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (_BSL_SOCDNX_MSG("Error, arad_pp_mpls_term_label_range_get_unsafe is not used\n")));

exit:
    SOCDNX_FUNC_RETURN;  
}

STATIC
uint32
  arad_pp_mpls_term_range_label_to_bit_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT  uint32                                 *bit_index,
    SOC_SAND_OUT  uint8                                *found
  )
{
  uint32
    rng_low=0,
    rng_high=0,
    rng_base=0,
    rng_indx,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (rng_indx = 0; rng_indx <= ARAD_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
  {
	  res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, rng_indx, &rng_low);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
	  res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, rng_indx, &rng_high);
	  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
   
	  if (SOC_SAND_IS_VAL_IN_RANGE(label_ndx,rng_low,rng_high))
	  {
		  *found = TRUE;
		  break;
	  }
	  if (label_ndx < rng_low)
	  {
		  *found = FALSE;
		  break;
	  }
  }

  if (rng_indx > ARAD_PP_MPLS_TERM_RANGE_NDX_MAX)
  {
    *found = FALSE;
    goto exit;
  }

  if (*found)
  {
	res = READ_IHP_MPLS_LABEL_RANGE_BASE_LIFr(unit, REG_PORT_ANY, rng_indx, &rng_base);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    
    *bit_index = label_ndx - rng_base;
  }
  else
  {
    res = arad_pp_mpls_term_range_label_to_bit_index_get(
            unit,
            rng_low,
            bit_index,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_label_to_bit_index_get()", label_ndx, *bit_index);
}


uint32
  arad_pp_mpls_term_range_terminated_label_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_range_terminated_label_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_set_verify()", label_ndx, 0);
}

uint32
  arad_pp_mpls_term_range_terminated_label_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_get_verify()", label_ndx, 0);
}


uint32
  arad_pp_mpls_term_range_terminated_label_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_reserved_labels_global_info_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_reserved_labels_global_info_set_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY);

  res = SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify(unit, reserved_labels_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_set_verify()", 0, 0);
}

uint32
  arad_pp_mpls_term_reserved_labels_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_get_verify()", 0, 0);
}


uint32
  arad_pp_mpls_term_reserved_labels_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(reserved_labels_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_reserved_label_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO     *label_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_reserved_label_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_verify(unit, label_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_set_verify()", label_ndx, 0);
}

uint32
  arad_pp_mpls_term_reserved_label_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_get_verify()", label_ndx, 0);
}


uint32
  arad_pp_mpls_term_reserved_label_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE);
  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_encountered_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  )
{
  uint32
    rng_low=0,
    rng_high,
    rng_base=0,
    rng_indx,
    bit_index,
    prev_bit,
    enc_lbls=0;
  ARAD_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA
    mpls_label_range_encountered_tbl;
  uint8
    found,
    encountered;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(next_label_id);



  res = arad_pp_mpls_term_range_label_to_bit_index_get(
          unit,
          first_label_ndx,
          &bit_index,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mpls_label_range_encountered_tbl.mpls_label_range_encountered = 0;
  if (!found)
  {
    SOC_SAND_TBL_ITER_SET_END(next_label_id);
    goto exit;
  }

  prev_bit = 0xFFFFFFFF;
  for (;bit_index < ARAD_PP_MPLS_TERM_RANGES_NOF_BITS; ++bit_index)
  {
    if (prev_bit / 16 != bit_index/16)
    {
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    prev_bit = bit_index;

    encountered = SOC_SAND_GET_BIT(mpls_label_range_encountered_tbl.mpls_label_range_encountered,bit_index%16);
    if (!encountered)
    {
      continue;
    }

    
    for (rng_indx = 0; rng_indx <= ARAD_PP_MPLS_TERM_RANGE_NDX_MAX; ++rng_indx)
    {
		 res = READ_IHP_MPLS_LABEL_RANGE_BASE_LIFr(unit, REG_PORT_ANY, rng_indx, &rng_base);
         SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
		 res = READ_IHP_MPLS_LABEL_RANGE_LOWr(unit, REG_PORT_ANY, rng_indx, &rng_low);
		 SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
		 res = READ_IHP_MPLS_LABEL_RANGE_HIGHr(unit, REG_PORT_ANY, rng_indx, &rng_high);
		 SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    
		 if (rng_low <= bit_index + rng_base && bit_index + rng_base <= rng_high)
		 {
			 break;
		 }
    }
    if (rng_indx > ARAD_PP_MPLS_TERM_RANGE_NDX_MAX)
    {
      continue;
    }
    encountered_labels[enc_lbls++] = rng_base + bit_index;
  }

  SOC_SAND_TBL_ITER_SET_END(next_label_id);
  
  *nof_encountered_labels = enc_lbls;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_encountered_entries_get_block_unsafe()", first_label_ndx, 0);
}

uint32
  arad_pp_mpls_term_encountered_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                *nof_encountered_labels
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(first_label_ndx, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_encountered_entries_get_block_verify()", first_label_ndx, 0);
}


uint32
  arad_pp_mpls_term_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO                *term_cos_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_COS_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO                *term_cos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_COS_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_COS_INFO, term_cos_info, 10, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_set_verify()", 0, 0);
}

uint32
  arad_pp_mpls_term_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_COS_INFO_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_get_verify()", 0, 0);
}


uint32
  arad_pp_mpls_term_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_COS_INFO                *term_cos_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_COS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  SOC_PPC_MPLS_TERM_COS_INFO_clear(term_cos_info);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_label_to_cos_info_set_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_label_to_cos_info_set_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_LABEL_COS_KEY, cos_key, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_LABEL_COS_VAL, cos_val, 20, exit);


  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_set_verify()", 0, 0);
}

uint32
  arad_pp_mpls_term_label_to_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY           *cos_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_LABEL_COS_KEY, cos_key, 10, exit);

  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_get_verify()", 0, 0);
}


uint32
  arad_pp_mpls_term_label_to_cos_info_get_unsafe(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(cos_val);


  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 105, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_profile_info_set_unsafe(
	SOC_SAND_IN  int                        		unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO 		*term_profile_info
  )
{
	uint32
	  res = SOC_SAND_OK;
	ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA
		term_profile_tbl_data;

	SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_UNSAFE);

	SOC_SAND_CHECK_NULL_INPUT(term_profile_info);

	term_profile_tbl_data.labels_to_terminate = SOC_IS_JERICHO(unit) ? term_profile_info->nof_headers : term_profile_info->nof_headers-1;
	term_profile_tbl_data.ttl_exp_label_index = term_profile_info->ttl_exp_index;
	if (term_profile_info->flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_0) 
	{
		term_profile_tbl_data.reject_ttl_0 = 1;
	}
	else
	{
		term_profile_tbl_data.reject_ttl_0 = 0;
	}
	if (term_profile_info->flags & SOC_PPC_MPLS_TERM_DISCARD_TTL_1) 
	{
		term_profile_tbl_data.reject_ttl_1 = 1;
	}
	else
	{
		term_profile_tbl_data.reject_ttl_1 = 0;
	}
	if (term_profile_info->flags & SOC_PPC_MPLS_TERM_HAS_CW) 
	{
		term_profile_tbl_data.has_cw = 1;
	}
	else
	{
		term_profile_tbl_data.has_cw = 0;
	}
	if (term_profile_info->flags & SOC_PPC_MPLS_TERM_SKIP_ETH) 
	{
		term_profile_tbl_data.skip_ethernet = 1;
	}
	else
	{
		term_profile_tbl_data.skip_ethernet = 0;
	}

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      term_profile_tbl_data.check_bos = term_profile_info->check_bos;
      if (SOC_IS_JERICHO(unit)) {
        term_profile_tbl_data.expect_bos = term_profile_info->expect_bos;
      }
    }
#endif 

	res = arad_pp_ihp_termination_profile_table_tbl_set_unsafe(
		unit,
		term_profile_ndx,
		&term_profile_tbl_data
		);
	SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_profile_info_set_verify(
	SOC_SAND_IN  int                        		unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO 		*term_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(term_profile_ndx, ARAD_PP_MPLS_TERM_PROFILE_MAX, ARAD_PP_MPLS_TERM_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_PROFILE_INFO, term_profile_info, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_set_verify()", term_profile_ndx, 0);
}

uint32
  arad_pp_mpls_term_profile_info_get_verify(
	SOC_SAND_IN  int                      				unit,
	SOC_SAND_IN  uint32 								        term_profile_ndx
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_PROFILE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(term_profile_ndx, ARAD_PP_MPLS_TERM_PROFILE_MAX, ARAD_PP_MPLS_TERM_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_get_verify()", term_profile_ndx, 0);
}


uint32
  arad_pp_mpls_term_profile_info_get_unsafe(
	SOC_SAND_IN  int                              unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_OUT SOC_PPC_MPLS_TERM_PROFILE_INFO 		*term_profile_info
  )
{
	uint32
	  res = SOC_SAND_OK;
	ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA
		term_profile_tbl_data;

	SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_UNSAFE);

	SOC_SAND_CHECK_NULL_INPUT(term_profile_info);

	SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(term_profile_info);

	res = arad_pp_ihp_termination_profile_table_tbl_get_unsafe(
		unit,
		term_profile_ndx,
		&term_profile_tbl_data
		);
	SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	term_profile_info->nof_headers = SOC_IS_JERICHO(unit) ? term_profile_tbl_data.labels_to_terminate : term_profile_tbl_data.labels_to_terminate+1;
	term_profile_info->ttl_exp_index = term_profile_tbl_data.ttl_exp_label_index;
	term_profile_info->flags |= SOC_PPC_MPLS_TERM_DISCARD_TTL_0 & term_profile_tbl_data.reject_ttl_0;
	term_profile_info->flags |= SOC_PPC_MPLS_TERM_DISCARD_TTL_1 & term_profile_tbl_data.reject_ttl_1;
	term_profile_info->flags |=  (term_profile_tbl_data.has_cw) ? SOC_PPC_MPLS_TERM_HAS_CW : 0;
	term_profile_info->flags |= SOC_PPC_MPLS_TERM_HAS_CW & term_profile_tbl_data.skip_ethernet;
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    term_profile_info->check_bos = term_profile_tbl_data.check_bos;
    if (SOC_IS_JERICHO(unit)) {
      term_profile_info->expect_bos = term_profile_tbl_data.expect_bos;
    }
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mpls_term_action_set_unsafe(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  SOC_PPC_ACTION_PROFILE   				*action_profile
  )
{
	uint32
	  res = SOC_SAND_OK;
	ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA
		action_mpls_val_tbl_data;

	SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_SET_UNSAFE);

	SOC_SAND_CHECK_NULL_INPUT(action_profile);

	action_mpls_val_tbl_data.cpu_trap_code = action_profile->trap_code;
	action_mpls_val_tbl_data.cpu_trap_strength = action_profile->frwrd_action_strength;
	action_mpls_val_tbl_data.snoop_strength = action_profile->snoop_action_strength;

	res = arad_pp_ihp_action_profile_mpls_value_tbl_set_unsafe(
		unit,
		action_profile_ndx,
		&action_mpls_val_tbl_data
		);
	SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mpls_term_action_set_verify(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  SOC_PPC_ACTION_PROFILE  					*action_profile
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(action_profile_ndx, ARAD_PP_MPLS_TERM_ACTION_PROFILE_MAX, ARAD_PP_MPLS_TERM_ACTION_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, action_profile, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_set_verify()", action_profile_ndx, 0);
}

uint32
  arad_pp_mpls_term_action_get_verify(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx
  )
{
 uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(action_profile_ndx, ARAD_PP_MPLS_TERM_ACTION_PROFILE_MAX, ARAD_PP_MPLS_TERM_ACTION_PROFILE_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_get_verify()", action_profile_ndx, 0);
}


uint32
  arad_pp_mpls_term_action_get_unsafe(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_OUT SOC_PPC_ACTION_PROFILE      				*action_profile
  )
{
	uint32
	  res = SOC_SAND_OK;
	ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA
		action_mpls_val_tbl_data;

	SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_GET_UNSAFE);

	SOC_SAND_CHECK_NULL_INPUT(action_profile);

	SOC_PPC_ACTION_PROFILE_clear(action_profile);

	res = arad_pp_ihp_action_profile_mpls_value_tbl_get_unsafe(
		unit,
		action_profile_ndx,
		&action_mpls_val_tbl_data
		);
	SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

	action_profile->trap_code = action_mpls_val_tbl_data.cpu_trap_code;
	action_profile->frwrd_action_strength = action_mpls_val_tbl_data.cpu_trap_strength;
	action_profile->snoop_action_strength = action_mpls_val_tbl_data.snoop_strength;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_get_unsafe()", 0, 0);
}


soc_error_t arad_pp_mpls_termination_spacial_labels_init(int unit) {
    ARAD_PP_ISEM_ACCESS_KEY                  isem_key;
    ARAD_PP_ISEM_ACCESS_ENTRY                isem_entry;
    SOC_SAND_SUCCESS_FAILURE                 success;
    uint32                                   res = SOC_SAND_OK;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_FUNC_RETURN;
    }

    if (SOC_DPP_CONFIG(unit)->pp.explicit_null_support) {
                        
        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        SOCDNX_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);      
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_EXPLICIT_NULL;
        isem_key.key_info.mpls.label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP;
        isem_entry.sem_result_ndx = SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id;
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(success != SOC_SAND_SUCCESS) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, 
                             (_BSL_SOCDNX_MSG("Error, TCAM additions fails Explicit V4 on init, unexpected behavior.\n")));
        }

        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS_EXPLICIT_NULL;
        isem_key.key_info.mpls.label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP;
        isem_entry.sem_result_ndx = SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id;
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(success != SOC_SAND_SUCCESS) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, 
                             (_BSL_SOCDNX_MSG("Error, TCAM additions fails Explicit V6 on init, unexpected behavior.\n")));
        }


    }    

    if (SOC_DPP_CONFIG(unit)->pp.mldp_support) {
            
            SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
            SOCDNX_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
            isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
            isem_key.key_info.mpls.flags = SOC_PPC_MPLS_TERM_FLAG_DUMMY_LABEL;
            isem_key.key_info.mpls.is_bos = 0;
            isem_entry.sem_result_ndx = SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[1];
            res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
            SOCDNX_SAND_IF_ERR_EXIT(res);

            if(success != SOC_SAND_SUCCESS)
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif1 on init, unexpected behavior.\n")));
            }

            SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
            isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
            isem_key.key_info.mpls.flags = SOC_PPC_MPLS_TERM_FLAG_DUMMY_LABEL;
            isem_key.key_info.mpls.is_bos = 1;
            isem_entry.sem_result_ndx = SOC_DPP_CONFIG(unit)->pp.global_mldp_dummy_lif_id[0];
            res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
            SOCDNX_SAND_IF_ERR_EXIT(res);

            if(success != SOC_SAND_SUCCESS)
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                                 (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif2 on init, unexpected behavior.\n")));
            }

        }

exit:
    SOCDNX_FUNC_RETURN;    
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mpls_term_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_mpls_term;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mpls_term_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_mpls_term;
}

uint32
  SOC_PPC_MPLS_TERM_LABEL_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);
  if (info->last_label < info->first_label)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR, 60, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_LABEL_RANGE_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_type, SOC_PPC_MPLS_TERM_KEY_TYPE_MAX, SOC_PPC_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_LKUP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_INFO_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->processing_type, ARAD_PP_MPLS_TERM_PROCESSING_TYPE_MAX, ARAD_RIF_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX_AND_NOT_NULL(info->rif, SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1, SOC_PPC_RIF_NULL, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, ARAD_PP_RIF_COS_PROFILE_MAX, ARAD_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->next_prtcl, SOC_PPC_NOF_L3_NEXT_PRTCL_TYPES, ARAD_RIF_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR, 15, exit); 
  SOC_SAND_ERR_IF_ABOVE_MAX(info->forwarding_code, (SOC_TMC_NOF_PKT_FRWRD_TYPES-1), ARAD_RIF_FORWARADING_CODE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->lif_profile, ARAD_PP_LIF_PROFILE_MAX, ARAD_PP_RIF_LIF_PROFILE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_TPID_PROFILE_MAX, ARAD_PP_RIF_TPID_PROFILE_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_forward_profile, ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_MAX, ARAD_PP_RIF_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_assignment_mode, ARAD_RIF_VSI_ASSIGNMENT_MODE_MAX, ARAD_RIF_VSI_ASSIGNMENT_MODE_OUT_OF_RANGE_ERR, 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->protection_pointer, ARAD_RIF_PROTECTION_POINTER_MAX, ARAD_RIF_PROTECTION_POINTER_OUT_OF_RANGE_ERR, 22, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->trap_code_index, ARAD_RIF_TRAP_CODE_INDEX_MAX, ARAD_RIF_TRAP_CODE_INDEX_OUT_OF_RANGE_ERR, 22, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MPLS_TERM_LABEL_RANGE, &(info->range), 10, exit);
  res = SOC_PPC_MPLS_TERM_INFO_verify(unit, &(info->term_info));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, &(info->bos_action), 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, &(info->non_bos_action), 11, exit);
  res = SOC_PPC_MPLS_TERM_INFO_verify(unit, &(info->term_info));
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  if (info->bos_action.frwrd_action_strength != 0 || info->non_bos_action.frwrd_action_strength != 0)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR, 15, exit);
  }

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->bos_action.trap_code,
    SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0,
    SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3,
    ARAD_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,30,exit)


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->processing_type, ARAD_PP_MPLS_TERM_PROCESSING_TYPE_MAX, ARAD_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->default_rif, SOC_DPP_CONFIG(unit)->l3.nof_rifs, SOC_PPC_RIF_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_COS_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_COS_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->model, ARAD_PP_MPLS_TERM_MODEL_MAX, ARAD_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_LABEL_COS_KEY_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_SAND_PP_TC_MAX, SOC_SAND_PP_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dp, SOC_SAND_PP_DP_MAX, SOC_SAND_PP_DP_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_LABEL_COS_VAL_verify()",0,0);
}

uint32
  SOC_PPC_MPLS_TERM_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_headers, ARAD_PP_MPLS_TERM_NOF_HEADERS_MIN, ARAD_PP_MPLS_TERM_NOF_HEADERS_MAX, ARAD_PP_MPLS_TERM_NOF_HEADERS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttl_exp_index, ARAD_PP_MPLS_TERM_TTL_EXP_MAX, ARAD_PP_MPLS_TERM_TTL_EXP_OUT_OF_RANGE_ERR, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MPLS_TERM_PROFILE_INFO_verify()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

