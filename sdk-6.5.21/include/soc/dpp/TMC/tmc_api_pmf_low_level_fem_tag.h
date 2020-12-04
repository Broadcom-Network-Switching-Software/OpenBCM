/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__

#define __SOC_TMC_API_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_ports.h>

#include <soc/dpp/PPC/ppc_api_fp.h>






#define  SOC_TMC_PMF_FEM_NOF_SELECTED_BITS (4)


#define  SOC_TMC_PMF_FEM_MAX_OUTPUT_SIZE_IN_BITS    (24)

#define SOC_TMC_PMF_LOW_LEVEL_NOF_DATABASES                          (128)









typedef enum
{
  
  SOC_TMC_PMF_FEM_INPUT_SRC_A_31_0 = 0,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_A_47_16 = 1,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_A_63_32 = 2,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_A_79_48 = 3,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_A_95_64 = 4,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_B_31_0 = 5,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_B_47_16 = 6,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_B_63_32 = 7,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_B_79_48 = 8,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_B_95_64 = 9,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_0 = 10,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_1 = 11,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_2 = 12,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_3 = 13,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_DIR_TBL = 14,
  
  SOC_TMC_PMF_FEM_INPUT_SRC_NOP = 15,
  
   SOC_TMC_PMF_FEM_INPUT_TCAM = 16,
  
  SOC_TMC_NOF_PMF_FEM_INPUT_SRCS = 17
}SOC_TMC_PMF_FEM_INPUT_SRC;

typedef enum
{
  
  SOC_TMC_PMF_TAG_TYPE_STAT_TAG = 0,
  
  SOC_TMC_PMF_TAG_TYPE_LAG_LB_KEY = 1,
  
  SOC_TMC_PMF_TAG_TYPE_ECMP_LB_KEY = 2,
  
  SOC_TMC_PMF_TAG_TYPE_STACK_RT_HIST = 3,
  
  SOC_TMC_NOF_PMF_TAG_TYPES = 4
}SOC_TMC_PMF_TAG_TYPE;

typedef enum
{
  
  SOC_TMC_PMF_TAG_VAL_SRC_A_29_0 = 0,
  
  SOC_TMC_PMF_TAG_VAL_SRC_A_61_32 = 1,
  
  SOC_TMC_PMF_TAG_VAL_SRC_B_29_0 = 2,
  
  SOC_TMC_PMF_TAG_VAL_SRC_B_61_32 = 3,
  
  SOC_TMC_PMF_TAG_VAL_SRC_TCAM_0 = 4,
  
  SOC_TMC_PMF_TAG_VAL_SRC_TCAM_1 = 5,
  
  SOC_TMC_PMF_TAG_VAL_SRC_TCAM_2 = 6,
  
  SOC_TMC_PMF_TAG_VAL_SRC_DIR_TBL = 7,
  
  SOC_TMC_NOF_PMF_TAG_VAL_SRCS = 8
}SOC_TMC_PMF_TAG_VAL_SRC;

typedef enum
{
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_DEST = 0,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_DP = 1,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_TC = 2,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_TRAP = 3,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_SNP = 4,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_MIRROR = 5,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_MIR_DIS = 6,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_EXC_SRC = 7,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_IS = 8,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_METER = 9,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_COUNTER = 10,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_STAT = 11,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA = 12,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM = 13,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_OUTLIF = 14,
  
  SOC_TMC_PMF_FEM_ACTION_TYPE_NOP = 15,
  
  SOC_TMC_NOF_PMF_FEM_ACTION_TYPES = 16
}SOC_TMC_PMF_FEM_ACTION_TYPE;

typedef enum
{
  
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE_CST = 0,
  
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE_KEY = 1,
  
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE_MAP_DATA = 2,
  
  SOC_TMC_NOF_PMF_FEM_BIT_LOC_TYPES = 3
}SOC_TMC_PMF_FEM_BIT_LOC_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint8 is_nop;

  
  uint8 is_key_src;
  
  uint32 key_tcam_id;
  
  uint32 key_lsb;
  
  uint32 lookup_cycle_id;

  
  uint8 use_kaps;

  
  uint8 is_compare;


} SOC_TMC_PMF_FEM_INPUT_SRC_ARAD;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 pgm_id;
  
  SOC_TMC_PMF_FEM_INPUT_SRC src;
  
  uint32 db_id;

  
  
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD src_arad;
  
  uint8 is_16_lsb_overridden;
  
  
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD lsb16_src;

} SOC_TMC_PMF_FEM_INPUT_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 shift;
  
  SOC_PPC_FP_ACTION_TYPE action_type;
  
  uint8 is_action_always_valid;

  
  uint32 valid_bits;

  
  uint32 polarity;

} SOC_TMC_PMF_FES_INPUT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PMF_TAG_VAL_SRC val_src;
  
  uint32 stat_tag_lsb_position;

} SOC_TMC_PMF_TAG_SRC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 id;
  
  uint32 cycle_ndx;

} SOC_TMC_PMF_FEM_NDX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 sel_bit_msb;

} SOC_TMC_PMF_FEM_SELECTED_BITS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 action_fomat_id;
  
  uint32 map_data;
  
  uint8 is_action_valid;

} SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE type;
  
  uint32 val;

} SOC_TMC_PMF_FEM_BIT_LOC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 type; 
  
  uint32 size;
  
  SOC_TMC_PMF_FEM_BIT_LOC bit_loc[SOC_TMC_PMF_FEM_MAX_OUTPUT_SIZE_IN_BITS];
  
  uint32 base_value;

} SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO;








void
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_INPUT_SRC_ARAD *info
  );

void
  SOC_TMC_PMF_FEM_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_INPUT_INFO *info
  );

void
  SOC_TMC_PMF_FES_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FES_INPUT_INFO *info
  );


void
  SOC_TMC_PMF_TAG_SRC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TAG_SRC_INFO *info
  );

void
  SOC_TMC_PMF_FEM_NDX_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_NDX *info
  );

void
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  SOC_TMC_PMF_FEM_BIT_LOC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_BIT_LOC *info
  );

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PMF_FEM_INPUT_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_SRC enum_val
  );

const char*
  SOC_TMC_PMF_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_TYPE enum_val
  );

const char*
  SOC_TMC_PMF_TAG_VAL_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_VAL_SRC enum_val
  );

const char*
  SOC_TMC_PMF_FEM_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_TYPE enum_val
  );

const char*
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_BIT_LOC_TYPE enum_val
  );

void
  SOC_TMC_PMF_FEM_INPUT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_INFO *info
  );

void
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD *info
  );

void
  SOC_TMC_PMF_TAG_SRC_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_SRC_INFO *info
  );

void
  SOC_TMC_PMF_FEM_NDX_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_NDX *info
  );

void
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  SOC_TMC_PMF_FEM_BIT_LOC_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_BIT_LOC *info
  );

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

