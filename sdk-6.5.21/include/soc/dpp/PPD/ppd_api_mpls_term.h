/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_MPLS_TERM_INCLUDED__

#define __SOC_PPD_API_MPLS_TERM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_mpls_term.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_MPLS_TERM_LKUP_INFO_SET = SOC_PPD_PROC_DESC_BASE_MPLS_TERM_FIRST,
  SOC_PPD_MPLS_TERM_LKUP_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_LKUP_INFO_GET,
  SOC_PPD_MPLS_TERM_LKUP_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_LABEL_RANGE_SET,
  SOC_PPD_MPLS_TERM_LABEL_RANGE_SET_PRINT,
  SOC_PPD_MPLS_TERM_LABEL_RANGE_GET,
  SOC_PPD_MPLS_TERM_LABEL_RANGE_GET_PRINT,
  SOC_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET,
  SOC_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_PRINT,
  SOC_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_GET,
  SOC_PPD_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_PRINT,
  SOC_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET,
  SOC_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET,
  SOC_PPD_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET,
  SOC_PPD_MPLS_TERM_RESERVED_LABEL_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_RESERVED_LABEL_INFO_GET,
  SOC_PPD_MPLS_TERM_RESERVED_LABEL_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK,
  SOC_PPD_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_PRINT,
  SOC_PPD_MPLS_TERM_COS_INFO_SET,
  SOC_PPD_MPLS_TERM_COS_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_COS_INFO_GET,
  SOC_PPD_MPLS_TERM_COS_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET,
  SOC_PPD_MPLS_TERM_LABEL_TO_COS_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET,
  SOC_PPD_MPLS_TERM_LABEL_TO_COS_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_PROFILE_INFO_GET,
  SOC_PPD_MPLS_TERM_PROFILE_INFO_GET_PRINT,
  SOC_PPD_MPLS_TERM_PROFILE_INFO_SET,
  SOC_PPD_MPLS_TERM_PROFILE_INFO_SET_PRINT,
  SOC_PPD_MPLS_TERM_ACTION_GET,
  SOC_PPD_MPLS_TERM_ACTION_GET_PRINT,
  SOC_PPD_MPLS_TERM_ACTION_SET,
  SOC_PPD_MPLS_TERM_ACTION_SET_PRINT,
  SOC_PPD_MPLS_TERM_GET_PROCS_PTR,
  



  
  SOC_PPD_MPLS_TERM_PROCEDURE_DESC_LAST
} SOC_PPD_MPLS_TERM_PROCEDURE_DESC;










uint32
  soc_ppd_mpls_term_lkup_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  );


uint32
  soc_ppd_mpls_term_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  );


uint32
  soc_ppd_mpls_term_label_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               range_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );


uint32
  soc_ppd_mpls_term_label_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               range_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );


uint32
  soc_ppd_mpls_term_range_terminated_label_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      label_ndx,
    SOC_SAND_IN  uint8                               is_terminated_label
  );


uint32
  soc_ppd_mpls_term_range_terminated_label_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      label_ndx,
    SOC_SAND_OUT uint8                               *is_terminated_label
  );


uint32
  soc_ppd_mpls_term_reserved_labels_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );


uint32
  soc_ppd_mpls_term_reserved_labels_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );


uint32
  soc_ppd_mpls_term_reserved_label_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      label_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );


uint32
  soc_ppd_mpls_term_reserved_label_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      label_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );


uint32
  soc_ppd_mpls_term_encountered_entries_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                      first_label_ndx,
    SOC_SAND_INOUT uint32                                *nof_encountered_labels,
    SOC_SAND_OUT uint32                                *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                      *next_label_id
  );


uint32
  soc_ppd_mpls_term_cos_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO                  *term_cos_info
  );


uint32
  soc_ppd_mpls_term_cos_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_COS_INFO                  *term_cos_info
  );


uint32
  soc_ppd_mpls_term_label_to_cos_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );


uint32
  soc_ppd_mpls_term_label_to_cos_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );


uint32
  soc_ppd_mpls_term_profile_info_set(
	SOC_SAND_IN  int                             	unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO 			*term_profile_info
  );


uint32
  soc_ppd_mpls_term_profile_info_get(
	SOC_SAND_IN  int                             	unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_OUT SOC_PPC_MPLS_TERM_PROFILE_INFO 			*term_profile_info
  );


uint32
  soc_ppd_mpls_term_action_set(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  SOC_PPC_ACTION_PROFILE 						*action_profile
  );


uint32
  soc_ppd_mpls_term_action_get(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_OUT SOC_PPC_ACTION_PROFILE 						*action_profile
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

