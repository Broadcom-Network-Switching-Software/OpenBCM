
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_FP_KEY_INCLUDED__

#define __ARAD_PP_FP_KEY_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_ce.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_tcam.h>





#define ARAD_PP_FP_KEY_CYCLE_EVEN                      0x0
#define ARAD_PP_FP_KEY_CYCLE_ODD                       0x1
#define ARAD_PP_FP_KEY_CYCLE_ANY                       0x2

#define ARAD_PP_FP_KEY_CE_SIZE_16                      0x1
#define ARAD_PP_FP_KEY_CE_SIZE_32                      0x2
#define ARAD_PP_FP_KEY_CE_SIZE_ANY                     0x3


#define ARAD_PP_FP_KEY_NOF_FREE_SEGMENTS_IN_BUFFER_MAX 4
#define ARAD_PP_FP_MAX_NOF_CONFS                       6

 
#define ARAD_PP_FP_KEY_CE_LOW                          1 
#define ARAD_PP_FP_KEY_CE_HIGH                         2 
#define ARAD_PP_FP_KEY_CE_PLACE_ANY_NOT_DOUBLE         3 
#define ARAD_PP_FP_KEY_CE_PLACE_ANY                    15 
#define ARAD_PP_FP_NOF_KEY_CE_PLACES                   4 


#define ARAD_PP_FP_KEY_CE_PLACE_HASH_KEY               0x70
#define ARAD_PP_FP_NOF_KEY_CE_PLACES_STAGE              ((ARAD_PMF_LOW_LEVEL_NOF_ZONES_PER_STAGE == 1 )? 2: 4)


#define ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY     0x2





#define ARAD_PP_FP_TCAM_ENTRY_SIZE   ARAD_TCAM_ENTRY_MAX_LEN
#define ARAD_PP_FP_TCAM_KEY_SIZE   160
#define ARAD_PP_FP_TCAM_HALF_KEY_SIZE   (ARAD_PP_FP_TCAM_KEY_SIZE/2)

#define ARAD_PP_FP_KEY_ALLOC_CE_WITH_ID         (0x1)
#define ARAD_PP_FP_KEY_ALLOC_CE_CHECK_ONLY      (ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY)
#define ARAD_PP_FP_KEY_ALLOC_CE_USE_RSRC_BMP    (0x4)
#define ARAD_PP_FP_KEY_ALLOC_CE_USE_QUAL_ORDER  (0x8)  
#define ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT    (0x10) 

#define ARAD_PP_FP_KEY_ALLOC_USE_CE_CONS        (0x20)
#define ARAD_PP_FP_KEY_ALLOC_CE_USE_HEADER_SELECTION (0x40) 
#define ARAD_PP_FP_KEY_ALLOC_CE_SINGLE_MAPPING (0x80) 
#define ARAD_PP_FP_KEY_ALLOC_USE_KEY_A         (0x100) 
#define ARAD_PP_FP_KEY_ALLOC_ELK_RANGE         (0x200) 


#define ARAD_PP_FLP_KEY_C_ZONE_SIZE_ARAD_PLUS_BITS  160
#define ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS    80
#define ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BYTES   (ARAD_PP_FLP_KEY_C_ZONE_SIZE_JERICHO_BITS/8)
#define ARAD_PP_FLP_KEY_C_NUM_OF_ZONES_JERICHO      2





#define ARAD_PP_FP_KEY_FIRST_SET_BIT(_bits, _first, _bit_count, _bits_size, _next, _result) \
    { \
        int _bit_index = 0; \
        int _cur_bit = _first; \
        if(_next) { \
            ++_bit_index; \
        } \
        _result = 0; \
        while (((_result) == 0) && (_bit_index < (_bit_count))) \
        { \
            _result |= SHR_BITGET(_bits, (_cur_bit + (_bit_index)) % _bits_size); \
            if(_result) { \
            break; \
            } \
            ++_bit_index; \
        } \
        _first = (_cur_bit + (_bit_index)) % _bits_size; \
    }





 

typedef uint32 ARAD_PP_KEY_CE_ID;

typedef enum
{
  
  ARAD_PP_FP_KEY_LENGTH_GET = ARAD_PP_PROC_DESC_BASE_FP_KEY_FIRST,
  ARAD_PP_FP_KEY_LENGTH_GET_PRINT,
  ARAD_PP_FP_KEY_LENGTH_GET_UNSAFE,
  ARAD_PP_FP_KEY_LENGTH_GET_VERIFY,
  ARAD_PP_FP_KEY_SPACE_ALLOC,
  ARAD_PP_FP_KEY_SPACE_ALLOC_PRINT,
  ARAD_PP_FP_KEY_SPACE_ALLOC_UNSAFE,
  ARAD_PP_FP_KEY_SPACE_ALLOC_VERIFY,
  ARAD_PP_FP_KEY_SPACE_FREE,
  ARAD_PP_FP_KEY_SPACE_FREE_PRINT,
  ARAD_PP_FP_KEY_SPACE_FREE_UNSAFE,
  ARAD_PP_FP_KEY_SPACE_FREE_VERIFY,
  ARAD_PP_FP_KEY_DESC_GET,
  ARAD_PP_FP_KEY_DESC_GET_PRINT,
  ARAD_PP_FP_KEY_DESC_GET_UNSAFE,
  ARAD_PP_FP_KEY_DESC_GET_VERIFY,
  ARAD_PP_FP_KEY_GET_PROCS_PTR,
  ARAD_PP_FP_KEY_GET_ERRS_PTR,
  
   ARAD_PP_FP_KEY_HEADER_NDX_FIND,
   ARAD_PP_FP_NOF_VLAN_TAGS_GET,
   ARAD_PP_FP_KEY_TOTAL_SIZE_GET,
   ARAD_PP_FP_KEY_CE_INSTR_BOUNDS_GET,
   ARAD_PP_FP_KEY_CE_INSTR_INSTALL,
   ARAD_PP_FP_KEY_NEW_DB_POSSIBLE_CONFS_GET_UNSAFE,



  
  ARAD_PP_FP_KEY_PROCEDURE_DESC_LAST
} ARAD_PP_FP_KEY_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FP_KEY_PFG_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FP_KEY_FIRST,
  ARAD_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR,
  ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR,
  ARAD_PP_FP_KEY_BAD_PADDING_ERR,
  ARAD_PP_FP_KEY_COPY_OVERFLOW_ERR,
  ARAD_PP_FP_KEY_TOO_MANY_BITS_ERR,
  



  
  ARAD_PP_FP_KEY_ERR_LAST
} ARAD_PP_FP_KEY_ERR;

typedef enum
{
  
  ARAD_PP_FP_PMF_KEY_A = 0,
  
  ARAD_PP_FP_PMF_KEY_B = 1,
  
  ARAD__PP_FP_PMF_KEY_C = 2,
  
  ARAD__PP_FP_PMF_KEY_D = 3,
  
  ARAD__PP_NOF_FP_PMF_KEYS = 4
}ARAD_PP_FP_PMF_KEY;

typedef enum
{
  
  ARAD_PP_FP_KEY_CONS_TCAM_72B = 0,
  
  ARAD_PP_FP_KEY_CONS_TCAM_144B = 1,
  
  ARAD_PP_FP_KEY_CONS_TCAM_288B = 2,
  
  ARAD_PP_FP_KEY_CONS_DIR_TBL = 3,
  
  ARAD_PP_FP_KEY_CONS_FEM = 4,
  
  ARAD_PP_FP_KEY_CONS_TAG = 5,
  
  ARAD_PP_NOF_FP_KEY_CONSTRAINTS = 6
}ARAD_PP_FP_KEY_CONSTRAINT;


typedef struct
{
    uint32 cycle_cons;    
    uint32 place_cons;    
    uint32 size_cons;     
    uint32 group_cons;    
    uint32 qual_lsb;      
    uint32 lost_bits;     
    uint32 nof_bits;      
    uint32 msb_padding_bits; 
    uint32 is_second_key; 
    SOC_PPC_FP_QUAL_TYPE   qual_type; 
} ARAD_PP_FP_CE_CONSTRAINT;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 lsb;
  
  uint32 length;

} ARAD_PP_FP_KEY_FLD_DESC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_PP_FP_KEY_FLD_DESC fields[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

} ARAD_PP_FP_KEY_DESC;



typedef struct
{
  
  uint32 key;
  
  uint32 start_instr;
  
  uint32 end_instr;
  
  uint32 lsb;
  
  uint32 msb;
  
  uint8 const_length;

} ARAD_PP_FP_KEY_SEGMENT;

typedef enum
{
  ARAD_PP_FP_KEY_SRC_PKT_HDR,
  ARAD_PP_FP_KEY_SRC_IRPP
} ARAD_PP_FP_KEY_SRC;

typedef enum
{
  
  ARAD_PP_FP_KEY_ZONE_LSB_0 = 0, 

  
  ARAD_PP_FP_KEY_ZONE_MSB_0 = 1, 

  
  ARAD_PP_FP_KEY_ZONE_LSB_1 = 2, 

  
  ARAD_PP_FP_KEY_ZONE_MSB_1 = 3, 

  
  ARAD_PP_FP_KEY_NOF_ZONES  = 4

} ARAD_PP_FP_KEY_ZONE;

typedef enum
{
  
  ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER = 0,
  
  ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY = 1,
  
  ARAD_PP_NOF_FP_KEY_BUFFER_DIRECTIONS = 2
}ARAD_PP_FP_KEY_BUFFER_DIRECTION;


typedef struct
{
  SOC_PPC_FP_QUAL_TYPE   qual_type;
  ARAD_PP_FP_KEY_SRC     data_src;
  uint32            msb;
  uint32            lsb;
  ARAD_PMF_CE_SUB_HEADER header_ndx_0; 
  ARAD_PMF_CE_SUB_HEADER header_ndx_1;
} ARAD_PP_FP_KEY_QUAL_INFO;

typedef struct
{
  uint8                  is_320b;
  uint32                 alloc_place;
  uint8                  key_id[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX]; 
  uint8                  used;
  uint8                  cycle;
  uint8                  ces[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
  uint8                  nof_ces;
} ARAD_PP_FP_KEY_DP_PROG_INFO;

typedef struct
{
    uint8                               cycle;
    uint8                               key_id[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX]; 
    uint32                              ce[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint32                              ce_key[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    uint8                               nof_ce; 
    ARAD_PP_FP_CE_CONSTRAINT            act_ce_const[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];
    SOC_PPC_FP_QUAL_TYPE                qual_type[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS]; 
    ARAD_TCAM_BANK_ENTRY_SIZE           key_size; 
    uint32                              alloc_place; 
    uint8                               use_kaps;
} ARAD_PP_FP_KEY_ALLOC_INFO;










uint32
  arad_pp_fp_key_alloc_constrain_calc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       cycle_cons, 
    SOC_SAND_IN  uint32                       place_cons,
    SOC_SAND_IN  uint8                        use_kaps,
    SOC_SAND_IN  uint8                        is_for_direct_extraction,
    SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT     ce_const[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS],
    SOC_SAND_OUT  uint32                      *nof_consts,
    SOC_SAND_INOUT  uint32                      *place_start,
    SOC_SAND_OUT  uint32                      selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_OUT  uint32                      total_bits_in_zone[ARAD_PP_FP_KEY_NOF_ZONES],
    SOC_SAND_OUT  uint8                       *found
  );

uint32
arad_pp_fp_key_alloc_specific_ce_constrain_calc(
        SOC_SAND_IN       int                         unit,
        SOC_SAND_IN       SOC_PPC_FP_DATABASE_STAGE   stage,
        SOC_SAND_IN       uint32                      flags,
        SOC_SAND_IN       SOC_PPC_FP_QUAL_TYPE        qual_types[ARAD_PP_QUAL_CHAIN_MAX], 
        SOC_SAND_IN       uint32                      max_nof_ppc_qual,
        SOC_SAND_IN       int                         ce_id[BCM_FIELD_NOF_CE_ID_PER_QUAL],
        SOC_SAND_OUT      ARAD_PP_FP_CE_CONSTRAINT    ce_const[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
        SOC_SAND_OUT      uint32                      *nof_consts
      );

uint32
  arad_pp_fp_key_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                       prog_ndx,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
    SOC_SAND_IN  uint32                       cycle_cons, 
    SOC_SAND_IN  uint32                       place_cons,
    SOC_SAND_IN  uint32                       start_place,
    SOC_SAND_IN ARAD_PP_FP_CE_CONSTRAINT      ce_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
    SOC_SAND_IN  uint32                       nof_ce_const,
   
    SOC_SAND_OUT  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info,
    SOC_SAND_OUT  uint8                       *key_alloced
 );

#if defined(INCLUDE_KBP)
uint32
  arad_pp_fp_elk_key_alloc_in_prog(
     SOC_SAND_IN  int                       unit,
     SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
     SOC_SAND_IN  uint32                       prog_ndx,
     SOC_SAND_IN  uint32                       db_id,
     SOC_SAND_IN  uint32                       flags,
     SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], 
     SOC_SAND_IN ARAD_PP_FP_CE_CONSTRAINT      ce_cons[ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS], 
     SOC_SAND_IN  uint32                       nof_ce_const,
    
     SOC_SAND_OUT  ARAD_PP_FP_KEY_ALLOC_INFO   *alloc_info,
     SOC_SAND_OUT  uint8                       *key_alloced
  );
#endif


uint32
  arad_pp_fp_key_nof_free_instr_get(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE       stage,
      SOC_SAND_IN  uint32                       ce_rsrc_bmp_glbl,
      SOC_SAND_IN  uint8                        is_32b,
      SOC_SAND_IN  uint8                        is_msb,
      SOC_SAND_IN  uint8                        is_high_group
    );

uint32
  arad_pp_fp_key_dealloc(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     db_id,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE       qual_types[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] 
  );


uint32
  arad_pp_fp_key_value_to_buffer(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL      qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_IN  uint32                   db_id,
      SOC_SAND_OUT  uint32                  value[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_OUT  uint32                  mask[ARAD_PP_FP_TCAM_ENTRY_SIZE]
  );


uint32
  arad_pp_fp_key_buffer_to_value(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
      SOC_SAND_IN  uint32                   db_id,
      SOC_SAND_IN  uint8                    nof_valid_bytes,
      SOC_SAND_IN  uint32                   value_in[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_IN  uint32                   mask_in[ARAD_PP_FP_TCAM_ENTRY_SIZE],
      SOC_SAND_OUT  SOC_PPC_FP_QUAL_VAL     qual_vals_out[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]
  );

uint32
  arad_pp_fp_key_value_buffer_mapping(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
      SOC_SAND_IN  uint32                           db_id,
      SOC_SAND_IN  ARAD_PP_FP_KEY_BUFFER_DIRECTION  direction, 
      SOC_SAND_IN  uint8                            is_for_kbp,
      SOC_SAND_IN  uint8                            is_from_dbal,
      SOC_SAND_IN  uint8                            nof_valid_bytes,
      SOC_SAND_INOUT  SOC_PPC_FP_QUAL_VAL      qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_INOUT  uint32                   value_out[ARAD_PP_FP_TCAM_ENTRY_SIZE], 
      SOC_SAND_INOUT  uint32                   mask_out[ARAD_PP_FP_TCAM_ENTRY_SIZE] 
  );

uint32
  arad_pp_fp_qual_length_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE       qual_type,
    SOC_SAND_OUT uint32                     *found,
    SOC_SAND_OUT uint32                     *length_padded_best_case,
    SOC_SAND_OUT uint32                     *length_padded_worst_case,
    SOC_SAND_OUT uint32                     *length_logical
  );

uint32
    arad_pp_fp_key_alloc_key_cascaded_key_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  db_id_ndx,
      SOC_SAND_OUT uint8  *is_key_fixed,
      SOC_SAND_OUT uint8  *key_id, 
      SOC_SAND_OUT uint32  *key_bitmap_constraint 
    );

uint32
    arad_pp_fp_key_alloc_key_is_equal_get(
      SOC_SAND_IN int    unit,
      SOC_SAND_IN uint32    db_id_ndx,
      SOC_SAND_OUT uint8    *is_key_fixed,
      SOC_SAND_OUT uint8    *key_id,
      SOC_SAND_OUT uint32   *key_place,
      SOC_SAND_OUT uint32   *key_bitmap_constraint 
    );

uint32
  arad_pp_fp_db_prog_info_get(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32                           db_ndx,
      SOC_SAND_IN  uint32                           prog_ndx,
      SOC_SAND_OUT  ARAD_PP_FP_KEY_DP_PROG_INFO     *db_prog_info
  );
 

soc_error_t
  arad_pp_fp_is_qual_in_qual_arr(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  *qual_types,
      SOC_SAND_IN  uint32                nof_qual_types,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  search_qual,
      SOC_SAND_OUT uint8                 *found
  );


soc_error_t
  arad_pp_fp_is_qual_identical(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  qual_type,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE  search_qual,
      SOC_SAND_OUT uint8                 *found
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_key_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_key_get_errs_ptr(void);

void
  ARAD_PP_FP_KEY_FLD_DESC_clear(
    SOC_SAND_OUT ARAD_PP_FP_KEY_FLD_DESC *info
  );

void
  ARAD_PP_FP_KEY_DESC_clear(
    SOC_SAND_OUT ARAD_PP_FP_KEY_DESC *info
  );


void
  ARAD_PP_FP_CE_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FP_CE_CONSTRAINT *info
  );

void
  ARAD_PP_FP_KEY_ALLOC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_KEY_ALLOC_INFO *info
  );


#if SOC_PPC_DEBUG_IS_LVL1

const char*
  ARAD_PP_FP_PMF_KEY_to_string(
    SOC_SAND_IN  ARAD_PP_FP_PMF_KEY enum_val
  );

const char*
  ARAD_PP_FP_KEY_CONSTRAINT_to_string(
    SOC_SAND_IN  ARAD_PP_FP_KEY_CONSTRAINT enum_val
  );

void
  ARAD_PP_FP_KEY_FLD_DESC_print(
    SOC_SAND_IN  ARAD_PP_FP_KEY_FLD_DESC *info
  );

void
  ARAD_PP_FP_KEY_DESC_print(
    SOC_SAND_IN  ARAD_PP_FP_KEY_DESC *info
  );

uint32
  arad_pp_fp_key_alloc_test1(
      SOC_SAND_IN  int                           unit
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


