/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_MPLS_TERM_INCLUDED__

#define __ARAD_PP_MPLS_TERM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_mpls_term.h>





#define ARAD_PP_MPLS_TERM_PROCESSING_TYPE_MAX              (SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES-1)

#define ARAD_PP_MPLS_TERM_NOF_HEADERS_MIN                  (1)
#define ARAD_PP_MPLS_TERM_NOF_HEADERS_MAX                  (3)
#define ARAD_PP_MPLS_TERM_TTL_EXP_MAX                      (1)










typedef enum
{
  
  SOC_PPC_MPLS_TERM_LKUP_INFO_SET = ARAD_PP_PROC_DESC_BASE_MPLS_TERM_FIRST,
  SOC_PPC_MPLS_TERM_LKUP_INFO_SET_PRINT,
  SOC_PPC_MPLS_TERM_LKUP_INFO_SET_UNSAFE,
  SOC_PPC_MPLS_TERM_LKUP_INFO_SET_VERIFY,
  SOC_PPC_MPLS_TERM_LKUP_INFO_GET,
  SOC_PPC_MPLS_TERM_LKUP_INFO_GET_PRINT,
  SOC_PPC_MPLS_TERM_LKUP_INFO_GET_VERIFY,
  SOC_PPC_MPLS_TERM_LKUP_INFO_GET_UNSAFE,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_SET,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_PRINT,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_UNSAFE,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_SET_VERIFY,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_GET,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_PRINT,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_VERIFY,
  SOC_PPC_MPLS_TERM_LABEL_RANGE_GET_UNSAFE,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_PRINT,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_PRINT,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY,
  ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_PRINT,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_PRINT,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY,
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_PRINT,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_PRINT,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY,
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE,
  ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK,
  ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_PRINT,
  ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE,
  ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY,
  SOC_PPC_MPLS_TERM_COS_INFO_SET,
  SOC_PPC_MPLS_TERM_COS_INFO_SET_PRINT,
  SOC_PPC_MPLS_TERM_COS_INFO_SET_UNSAFE,
  SOC_PPC_MPLS_TERM_COS_INFO_SET_VERIFY,
  SOC_PPC_MPLS_TERM_COS_INFO_GET,
  SOC_PPC_MPLS_TERM_COS_INFO_GET_PRINT,
  SOC_PPC_MPLS_TERM_COS_INFO_GET_VERIFY,
  SOC_PPC_MPLS_TERM_COS_INFO_GET_UNSAFE,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_PRINT,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_PRINT,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY,
  ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_SET,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_print,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_UNSAFE,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_SET_VERIFY,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_GET,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_GET_print,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_GET_UNSAFE,
  SOC_PPC_MPLS_TERM_PROFILE_INFO_GET_VERIFY,
  ARAD_PP_MPLS_TERM_ACTION_SET,
  ARAD_PP_MPLS_TERM_ACTION_SET_PRINT,
  ARAD_PP_MPLS_TERM_ACTION_SET_UNSAFE,
  ARAD_PP_MPLS_TERM_ACTION_SET_VERIFY,
  ARAD_PP_MPLS_TERM_ACTION_GET,
  ARAD_PP_MPLS_TERM_ACTION_GET_PRINT,
  ARAD_PP_MPLS_TERM_ACTION_GET_UNSAFE,
  ARAD_PP_MPLS_TERM_ACTION_GET_VERIFY,
  ARAD_PP_MPLS_TERM_GET_PROCS_PTR,
  ARAD_PP_MPLS_TERM_GET_ERRS_PTR,
  
   ARAD_PP_IHP_MPLS_TUNNEL_TERMINATION_UPDATE_BASES,



  
  ARAD_PP_MPLS_TERM_PROCEDURE_DESC_LAST
} ARAD_PP_MPLS_TERM_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_MPLS_TERM_FIRST,
  ARAD_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR,
  SOC_PPC_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR,
  
  ARAD_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR,
  ARAD_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR,
  ARAD_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR,
  ARAD_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR,
  ARAD_PP_MPLS_TERM_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_NOF_HEADERS_OUT_OF_RANGE_ERR,
  ARAD_PP_MPLS_TERM_TTL_EXP_OUT_OF_RANGE_ERR,

  
  ARAD_PP_MPLS_TERM_ERR_LAST
} ARAD_PP_MPLS_TERM_ERR;









uint32
  arad_pp_mpls_term_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_mpls_term_lkup_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

uint32
  arad_pp_mpls_term_lkup_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

uint32
  arad_pp_mpls_term_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mpls_term_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LKUP_INFO                 *lkup_info
  );


uint32
  arad_pp_mpls_term_label_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

uint32
  arad_pp_mpls_term_label_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

uint32
  arad_pp_mpls_term_label_range_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx
  );


uint32
  arad_pp_mpls_term_label_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );


uint32
  arad_pp_mpls_term_range_terminated_label_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  );

uint32
  arad_pp_mpls_term_range_terminated_label_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  );

uint32
  arad_pp_mpls_term_range_terminated_label_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  );


uint32
  arad_pp_mpls_term_range_terminated_label_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  );


uint32
  arad_pp_mpls_term_reserved_labels_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

uint32
  arad_pp_mpls_term_reserved_labels_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

uint32
  arad_pp_mpls_term_reserved_labels_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mpls_term_reserved_labels_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );


uint32
  arad_pp_mpls_term_reserved_label_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

uint32
  arad_pp_mpls_term_reserved_label_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

uint32
  arad_pp_mpls_term_reserved_label_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  );


uint32
  arad_pp_mpls_term_reserved_label_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );


uint32
  arad_pp_mpls_term_encountered_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  );

uint32
  arad_pp_mpls_term_encountered_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels
  );


uint32
  arad_pp_mpls_term_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO                  *term_cos_info
  );

uint32
  arad_pp_mpls_term_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO                  *term_cos_info
  );

uint32
  arad_pp_mpls_term_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mpls_term_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_COS_INFO                  *term_cos_info
  );


uint32
  arad_pp_mpls_term_label_to_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

uint32
  arad_pp_mpls_term_label_to_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

uint32
  arad_pp_mpls_term_label_to_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key
  );


uint32
  arad_pp_mpls_term_label_to_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PPC_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );


uint32
  arad_pp_mpls_term_profile_info_set_unsafe(
	SOC_SAND_IN  int                        unit,
	SOC_SAND_IN  uint32 								        term_profile_ndx, 
	SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO 	*term_profile_info
  );

uint32
  arad_pp_mpls_term_profile_info_set_verify(
	SOC_SAND_IN  int                        unit,
	SOC_SAND_IN  uint32 								        term_profile_ndx, 
	SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO 	*term_profile_info
  );

uint32
  arad_pp_mpls_term_profile_info_get_verify(
	SOC_SAND_IN  int                        unit,
	SOC_SAND_IN  uint32 								        term_profile_ndx
  );


uint32
  arad_pp_mpls_term_profile_info_get_unsafe(
	SOC_SAND_IN  int                        unit,
	SOC_SAND_IN  uint32 								        term_profile_ndx, 
	SOC_SAND_OUT SOC_PPC_MPLS_TERM_PROFILE_INFO 	*term_profile_info
  );


uint32
  arad_pp_mpls_term_action_set_unsafe(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  SOC_PPC_ACTION_PROFILE       					*action_profile
  );

uint32
  arad_pp_mpls_term_action_set_verify(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  SOC_PPC_ACTION_PROFILE      						*action_profile
  );

uint32
  arad_pp_mpls_term_action_get_verify(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx
  );


uint32
  arad_pp_mpls_term_action_get_unsafe(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_OUT SOC_PPC_ACTION_PROFILE      						*action_profile
  );


soc_error_t arad_pp_mpls_termination_spacial_labels_init(int unit);


soc_error_t arad_pp_mpls_termination_l4b_l5l_init(int unit);


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mpls_term_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mpls_term_get_errs_ptr(void);

uint32
  SOC_PPC_MPLS_TERM_LABEL_RANGE_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE *info
  );

uint32
  SOC_PPC_MPLS_TERM_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LKUP_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_INFO_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_verify(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_COS_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_COS_INFO *info
  );

uint32
  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_KEY *info
  );

uint32
  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_LABEL_COS_VAL *info
  );

uint32
  SOC_PPC_MPLS_TERM_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MPLS_TERM_PROFILE_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
