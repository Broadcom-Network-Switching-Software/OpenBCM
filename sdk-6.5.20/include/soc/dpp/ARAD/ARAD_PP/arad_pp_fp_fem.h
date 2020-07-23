/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FP_FEM__INCLUDED__

#define __ARAD_PP_FP_FEM__INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/PPC/ppc_api_fp_fem.h>

#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>






#define ARAD_PP_FP_FEM_ALLOC_FES_TM                         (0x1)
#define ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY                 (0x2)
#define ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP      (0x4) 
#define ARAD_PP_FP_FEM_ALLOC_FES_FROM_KEY                   (0x8) 
#define ARAD_PP_FP_FEM_ALLOC_FES_KEY_IS_CONDITIONAL_VALID   (0x10) 
#define ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID               (0x20) 
#define ARAD_PP_FP_FEM_ALLOC_FES_POLARITY                   (0x40) 
#define ARAD_PP_FP_FEM_ALLOC_FES_COMBINED                   (0x80) 

#define ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_L             (0)     
#define ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H             (1)     

#define ARAD_PP_FP_FES_OPERATION_MOVE(_pgm, _from, _to) \
do { \
    sal_memcpy(&fes_info[(_to)],&fes_info[(_from)],sizeof(ARAD_PMF_FES)); \
    fes_info[(_from)].is_used = 0; \
    if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) { \
        res = arad_pmf_db_fes_move_unsafe( \
                unit, \
                (_pgm), \
                (_from), \
                (_to), \
                &fes_info[(_to)]); \
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); \
    } \
} while (0)

#define ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE   (ARAD_TCAM_ACTION_MAX_LEN + 1)


#define ARAD_PP_FP_KEY_LENGTH_TM_IN_BITS    (160)

#define ARAD_PP_FP_FEM_FES_RESOLUTION_16B                        (16)

#define ARAD_PP_FP_FEM_FES_ACTION_ZONE_SIZE                    (8)
#define ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB                        (8)
#define ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB                        (16)






#define ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width))
#define ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID(_lsb)  (((_lsb) < ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS)? 0: 1)
#define ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb)  ((_lsb) - (ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS * ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID(_lsb))) 

#define ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS (32)

#define ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT_USE_KAPS(_lsb) ( ((_lsb) < ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS)? \
      (_lsb) : (_lsb - ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS) )
      

#define ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_lsb)  ((ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb) < ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB)? \
                                                  0: (SOC_IS_JERICHO(unit)? ( (ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb) < ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB) ?  1 : 2 ) : 1)  )

#define ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID_USE_KAPS(_lsb) (((_lsb) < ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS)? 0: 1)

#define ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(_lsb)  ((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_lsb) == 0)? 0: \
                                                 ( SOC_IS_JERICHO(unit) ? ((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_lsb) == 1)?  ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB : ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB) : ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB )  )
#define ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT(_lsb) ((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_lsb) == 0)? \
    ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb): \
    (SOC_IS_JERICHO(unit) ? ((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_lsb) == 1)?  ( ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb) - ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB) :  ( ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb) - ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB) ) : ( ARAD_PP_FP_FEM_ACTION_LSB_TO_RELATIVE_LSB(_lsb) - ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB) ) )


#define ARAD_PP_FP_FEM_ACTION_LSB_CLOSEST_16B(_lsb)  (((_lsb) / ARAD_PP_FP_FEM_FES_RESOLUTION_16B) * ARAD_PP_FP_FEM_FES_RESOLUTION_16B)

#define ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(_lsb)  (((ARAD_PP_FP_FEM_ACTION_LSB_CLOSEST_16B(_lsb) % 80) == 64)? \
            (ARAD_PP_FP_FEM_ACTION_LSB_CLOSEST_16B(_lsb) - 16): ARAD_PP_FP_FEM_ACTION_LSB_CLOSEST_16B(_lsb))


#define ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_ARAD(_first_lsb, _action_length)  \
            (((_first_lsb + _action_length) <= (ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(_first_lsb) + 32))? _first_lsb : \
            (((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_first_lsb) == 0) && (ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB + _action_length <= 40))? ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB: 40)) 
            

#define ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_JERICHO_USE_KAPS(_first_lsb, _action_length) \
            ((_first_lsb + _action_length) <= 32 ? _first_lsb : 32)
            
#define ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_JERICHO(use_kaps,_first_lsb, _action_length)  \
            ((use_kaps) ? ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_JERICHO_USE_KAPS(_first_lsb, _action_length):\
            (((_first_lsb + _action_length) <= (ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(_first_lsb) + 32))? _first_lsb : \
            (((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_first_lsb) == 0) && (ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB + _action_length <= 40))? ARAD_PP_FP_FEM_FES_2ND_ZONE_LSB: \
            (((ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(_first_lsb) == 1) && (ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB + _action_length <= 48))? ARAD_PP_FP_FEM_FES_3RD_ZONE_LSB:48)))) 
            
  
#define ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(use_kaps,_first_lsb, _action_length)  \
            (SOC_IS_JERICHO(unit)? ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_JERICHO(use_kaps,_first_lsb, _action_length) : ARAD_PP_FP_FEM_ACTION_LOCAL_LSB_ARAD(_first_lsb, _action_length) )

#define ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_type)    ( (action_type == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY) || \
                                                                   (action_type == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_0 ) || \
                                                                   (action_type == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_1 ) || \
                                                                   (action_type == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_2 ) || \
                                                                   (action_type == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_3 ) || \
                                                                   (action_type == SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_KAPS ) )

#define ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(action_type) ((action_type == SOC_PPC_FP_ACTION_TYPE_EEI) \
                                                                    && SOC_IS_QUX(unit) \
                                                                    && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "large_dir_without_valid", 0))







typedef struct {

    
    uint32 cycle;

    
    uint32 tcam_res_id[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX];

    
    ARAD_TCAM_ACTION_SIZE action_size;

    

}ARAD_PP_FEM_ACTIONS_CONSTRAINT;


typedef enum
{
  
  ARAD_PP_FP_FEM_INSERT = ARAD_PP_PROC_DESC_BASE_FP_FEM_FIRST,
  ARAD_PP_FP_FEM_INSERT_PRINT,
  ARAD_PP_FP_FEM_INSERT_UNSAFE,
  ARAD_PP_FP_FEM_INSERT_VERIFY,
  ARAD_PP_FP_FEM_IS_PLACE_GET,
  ARAD_PP_FP_FEM_IS_PLACE_GET_PRINT,
  ARAD_PP_FP_FEM_IS_PLACE_GET_UNSAFE,
  ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY,
  ARAD_PP_FP_FEM_TAG_SET,
  ARAD_PP_FP_FEM_TAG_SET_PRINT,
  ARAD_PP_FP_FEM_TAG_SET_UNSAFE,
  ARAD_PP_FP_FEM_TAG_SET_VERIFY,
  ARAD_PP_FP_FEM_TAG_GET,
  ARAD_PP_FP_FEM_TAG_GET_PRINT,
  ARAD_PP_FP_FEM_TAG_GET_VERIFY,
  ARAD_PP_FP_FEM_TAG_GET_UNSAFE,
  ARAD_PP_FP_FEM_GET_PROCS_PTR,
  ARAD_PP_FP_FEM_GET_ERRS_PTR,
  
   ARAD_PP_FP_TAG_ACTION_TYPE_CONVERT,
   ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE,
   ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET,
   ARAD_PP_FP_FEM_DUPLICATE,
   ARAD_PP_FP_FEM_CONFIGURE,
   ARAD_PP_FP_FEM_CONFIGURATION_GET,
   ARAD_PP_FP_FEM_REMOVE,
   ARAD_PP_FP_FEM_REORGANIZE,
   ARAD_PP_FP_FEM_COMBINE_PRE_PROCESS,
   ARAD_PP_FP_FEM_COMBINE_PROCESS,
   ARAD_PP_FP_FEM_COMBINE_INVALID_GET,
   



  
  ARAD_PP_FP_FEM_PROCEDURE_DESC_LAST
} ARAD_PP_FP_FEM_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FP_FEM_FIRST,
  ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
  



  
  ARAD_PP_FP_FEM_ERR_LAST
} ARAD_PP_FP_FEM_ERR;









uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    );

uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_IN  uint8                          fem_pgm_id
    );

uint32
    arad_pp_fp_fem_pgm_id_remove(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id
    );

uint32
    arad_pp_fp_fem_pgm_id_alloc(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    );

uint32
  arad_pp_fp_fem_pgm_id_bmp_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_OUT uint8                     *fem_pgm_id
    );


uint32
  arad_pp_fp_action_alloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type, 
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  uint32                       my_priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_IN  uint32                       action_lsb,
    SOC_SAND_IN  uint32                       action_len,
    SOC_SAND_IN  int32                        required_nof_feses,
    SOC_SAND_INOUT ARAD_PMF_FES               fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                        *found
  );

uint32
  arad_pp_fp_action_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  );

uint32
  arad_pp_fp_action_alloc_in_prog_with_entry(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  );

uint32
  arad_pp_fp_action_value_to_buffer(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  uint32                   *buffer_size
  );


uint32
  arad_pp_fp_action_buffer_to_value(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]
  );

uint32
  arad_pp_fp_tag_action_type_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE         action_type,
    SOC_SAND_OUT uint8                    *is_tag_action,
    SOC_SAND_OUT uint32                    *action_ndx
  );

uint32
  arad_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE  pmf_fem_action_type,
    SOC_SAND_OUT SOC_PPC_FP_ACTION_TYPE    *fp_action_type
  );

uint32
  arad_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  );


uint32
  arad_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_IN  uint8                    used_kaps,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  );


uint32
  arad_pp_fp_qual_type_and_local_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                   bit_ndx,
    SOC_SAND_IN  uint32                   db_id_ndx,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE         *qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb
  );


uint32
  arad_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  );

uint32
  arad_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx
  );

uint32
  arad_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_IN  uint8                     fem_pgm_id,
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL            *qual_info
  );



uint32
  arad_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO              *fem_info,
    SOC_SAND_INOUT ARAD_PMF_FES                 fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_IN uint8                           fem_pgm_id,
    SOC_SAND_IN uint32                          fem_bypass_pgm_bmp,
    SOC_SAND_OUT uint32                         *fes_fem_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  arad_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *fem_info
  );


uint32
  arad_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY   *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE   *fem_info,
    SOC_SAND_IN  uint8                  is_for_tm,
    SOC_SAND_IN  uint8                  fem_pgm_id,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                  *place_found,
    SOC_SAND_OUT char                   *reason_for_fail,
    SOC_SAND_IN int32                   reason_for_fail_len
  );

uint32
  arad_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE            *fem_info
  );


uint32
  arad_pp_fp_fem_tag_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE           action_type
  );

uint32
  arad_pp_fp_fem_tag_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE           action_type
  );

uint32
  arad_pp_fp_action_alloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  uint32                       selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_INOUT  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                          *action_alloced
  );

uint32
  arad_pp_fp_action_dealloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] 
  );

uint32
  arad_pp_fp_action_to_lsbs(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO  *info,
      SOC_SAND_OUT uint32                    action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT uint32                    action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE     *action_size,
      SOC_SAND_OUT uint32                    *nof_actions,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_fem_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_fem_get_errs_ptr(void);

uint32
  SOC_PPC_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  );

uint32
  SOC_PPC_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  );




void
  ARAD_PP_FEM_ACTIONS_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FEM_ACTIONS_CONSTRAINT *info
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


