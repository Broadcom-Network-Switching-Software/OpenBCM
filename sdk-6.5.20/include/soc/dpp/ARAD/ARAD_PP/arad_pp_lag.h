/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LAG_INCLUDED__

#define __ARAD_PP_LAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_lag.h>




#define ARAD_PP_FLEX_POST_HASH_KEYS_INPUT_SIZE                47
#define ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE        2
#define SOC_SUPPORTS_ECMP_HASH_COMBINE(_unit) (SOC_IS_JERICHO_B0(_unit) || SOC_IS_QMX_B0(_unit) || SOC_IS_QAX_B0(_unit) || SOC_IS_JERICHO_PLUS_ONLY(_unit) || SOC_IS_QUX_A0(_unit))








typedef enum
{
  
  ARAD_PP_LAG_SET = ARAD_PP_PROC_DESC_BASE_LAG_FIRST,
  ARAD_PP_LAG_SET_PRINT,
  ARAD_PP_LAG_SET_UNSAFE,
  ARAD_PP_LAG_SET_VERIFY,
  ARAD_PP_LAG_GET,
  ARAD_PP_LAG_GET_PRINT,
  ARAD_PP_LAG_GET_VERIFY,
  ARAD_PP_LAG_GET_UNSAFE,
  SOC_PPC_LAG_MEMBER_ADD,
  SOC_PPC_LAG_MEMBER_ADD_PRINT,
  SOC_PPC_LAG_MEMBER_ADD_UNSAFE,
  SOC_PPC_LAG_MEMBER_ADD_VERIFY,
  SOC_PPC_LAG_MEMBER_REMOVE,
  SOC_PPC_LAG_MEMBER_REMOVE_PRINT,
  SOC_PPC_LAG_MEMBER_REMOVE_UNSAFE,
  SOC_PPC_LAG_MEMBER_REMOVE_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_PRINT,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_PRINT,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_PRINT,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_VERIFY,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_PRINT,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_VERIFY,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE,
  ARAD_PP_LAG_HASHING_MASK_SET,
  ARAD_PP_LAG_HASHING_MASK_SET_PRINT,
  ARAD_PP_LAG_HASHING_MASK_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_MASK_SET_VERIFY,
  ARAD_PP_LAG_HASHING_MASK_GET,
  ARAD_PP_LAG_HASHING_MASK_GET_PRINT,
  ARAD_PP_LAG_HASHING_MASK_GET_VERIFY,
  ARAD_PP_LAG_HASHING_MASK_GET_UNSAFE,
  ARAD_PP_LAG_GET_PROCS_PTR,
  ARAD_PP_LAG_GET_ERRS_PTR,
  

  
  ARAD_PP_LAG_PROCEDURE_DESC_LAST
} ARAD_PP_LAG_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LAG_FIRST,
  ARAD_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_MASKS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_SEED_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_MASK_OUT_OF_RANGE_ERR,
  SOC_PPC_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PPC_LAG_LB_TYPE_OUT_OF_RANGE_ERR,
  
   ARAD_PP_LAG_ASYMETRIC_ERR,
   ARAD_PP_LAG_DOUPLICATE_MEMBER_ERR,

  
  ARAD_PP_LAG_ERR_LAST
} ARAD_PP_LAG_ERR;

typedef enum
{
  ARAD_PP_HIERARCHY_1 = 0,
  ARAD_PP_HIERARCHY_2,
  ARAD_PP_HIERARCHY_NOF
} ARAD_PP_HIERARCHY;

typedef enum
{
  ARAD_PP_LB_VECTOR_1 = 0,
  ARAD_PP_LB_VECTOR_2,
  ARAD_PP_LB_VECTOR_3,
  ARAD_PP_LB_VECTOR_4,
  ARAD_PP_LB_VECTOR_5,
  ARAD_PP_LB_VECTOR_NOF
} ARAD_PP_LB_VECTOR;

typedef enum
{
  ARAD_PP_DIAG_ECMP = 0,
  ARAD_PP_DIAG_LAG
} ARAD_PP_DIAG_TYPE;


typedef struct
{
  
  uint8 lb_key_is_symtrc;

  
  uint32 my_modid;

  SOC_PPC_HASH_MASKS masks;

} ARAD_PP_LAG;


typedef struct
{
    uint32 key_0_shift;
    uint32 key_1_shift;
    uint32 key_0_input[ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE];
    uint32 key_1_input[ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE];
    uint32 flow_label_dst[3];
}ARAD_PP_LB_PRINT_STATEFUL_SIGNALS;


typedef struct
{
    uint32 polynomial;
    uint32 key_shift;
    uint32 seed;
    uint32 use_port_id;
    uint32 header_count;
    uint32 starting_header;
}ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION;

typedef struct
{
    uint32 plc;
    uint32 plc_profile;
    uint32 plc_bit;
    uint32 pfc_stage1;
    uint32 pfc_stage2;
    uint32 pfc_profile_index;
    uint32 port_profile;
    uint32 fwrd_index;
    uint32 bos_search;
    uint32 bos_include;
    uint32 eli_search;
    uint32 LB_vectors[ARAD_PP_LB_VECTOR_NOF];
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION lag;
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION ecmp;
    
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION ecmp_2nd;
}ARAD_PP_LB_PRINT_CONFIGURED_SIGNALS;


typedef struct
{
    int    ecmp_fec;
    uint8  ecmp_is_valid;
    uint32 offset;
    uint32 ecmp_fec_pointer;
    uint32 start_pointer;
    uint32 is_protected;
    uint32 group_size;
    
    uint32 ecmp_lb_key;
}ARAD_PP_LB_PRINT_HIERARCHY_ECMP_SIGNALS;


typedef struct
{
    uint32                                  in_port;
    uint32                                  is_stateful;
    uint32                                  lag_valid;
    uint32                                  is_specualte;
    ARAD_PP_LB_PRINT_CONFIGURED_SIGNALS     configured;
    ARAD_PP_LB_PRINT_STATEFUL_SIGNALS       stateful;
    ARAD_PP_LB_PRINT_HIERARCHY_ECMP_SIGNALS hier_sig[ARAD_PP_HIERARCHY_NOF];
}ARAD_PP_LB_PRINT_SIGNALS_PER_CORE;


typedef struct
{
    int                                     first_core;
    int                                     last_core;
    uint32                                  is_combine_flb_clb;
    ARAD_PP_DIAG_TYPE                       diag_type;
    ARAD_PP_LB_PRINT_SIGNALS_PER_CORE       core[SOC_MAX_NUM_IHB_BLKS];
}ARAD_PP_LB_PRINT_SIGNALS;








uint32
  arad_pp_lag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_lag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info
  );

uint32
  arad_pp_lag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info
  );

uint32
  arad_pp_lag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx
  );


uint32
  arad_pp_lag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO                            *lag_info
  );


uint32
  arad_pp_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_lag_member_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member
  );


uint32
  arad_pp_lag_member_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                     *member
  );

uint32
  arad_pp_lag_member_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                     *member
  );


uint32
  arad_pp_lag_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );

uint32
  arad_pp_lag_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );

uint32
  arad_pp_lag_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_lag_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );


uint32
  arad_pp_lag_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

uint32
  arad_pp_lag_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

uint32
  arad_pp_lag_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_ndx
  );



uint32
  arad_pp_lag_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );



int
  arad_pp_lag_hashing_port_lb_profile_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         pp_port,
    SOC_SAND_IN  uint32                         lb_profile
  );


uint32
  arad_pp_lag_hashing_mask_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_hashing_mask_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_hashing_mask_get_verify(
    SOC_SAND_IN  int                  unit
  );


uint32
  arad_pp_lag_hashing_mask_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_lb_key_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info);


uint32
  arad_pp_lag_hash_func_to_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8         hash_func_id,
    SOC_SAND_OUT  uint32        *hw_val
  );

uint32
  arad_pp_lag_hash_func_from_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        hw_val,
    SOC_SAND_OUT  uint8        *hash_func_id
  );


uint32
arad_pp_lag_hashing_ecmp_hash_slb_combine_set(
   int            unit,
   int            combine_slb
);

uint32
arad_pp_lag_hashing_ecmp_hash_slb_combine_get(
   int            unit,
   int            *combine_slb
);


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lag_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lag_get_errs_ptr(void);

uint32
  SOC_PPC_LAG_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  );

uint32
  SOC_PPC_LAG_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO *info
  );

uint32
  SOC_PPC_HASH_MASK_INFO_verify(
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO *info
  );

uint32
  SOC_PPC_LAG_MEMBER_verify(
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER *info
  );

uint32
  SOC_PPC_LAG_INFO_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO *info
  );

uint32
  arad_pp_lag_init_polynomial_for_tm_mode(
    SOC_SAND_IN int unit
  );

soc_error_t
  soc_jer_pp_lag_get_load_balancing_diagnostic_signal(
    SOC_SAND_IN    int  unit,
    SOC_SAND_INOUT ARAD_PP_LB_PRINT_SIGNALS* lb_signals
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


