/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__

#define __ARAD_PMF_LOW_LEVEL_FEM_TAG_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_fem_tag.h>


#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>




#define ARAD_PMF_FEM_KEY_LSB_MAX             (128)
#define ARAD_PMF_FEM_KEY_LSB_MULTIPLE         (16)
#define ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB     (80)
#define ARAD_PMF_FEM_KEY_LSB_FORBIDDEN         (64)

#define ARAD_PMF_FEM_BIT_LOC_TYPE_CST                     SOC_TMC_PMF_FEM_BIT_LOC_TYPE_CST
#define ARAD_PMF_FEM_BIT_LOC_TYPE_KEY                     SOC_TMC_PMF_FEM_BIT_LOC_TYPE_KEY
#define ARAD_PMF_FEM_BIT_LOC_TYPE_MAP_DATA                SOC_TMC_PMF_FEM_BIT_LOC_TYPE_MAP_DATA
#define ARAD_NOF_PMF_FEM_BIT_LOC_TYPES                    SOC_TMC_NOF_PMF_FEM_BIT_LOC_TYPES
typedef SOC_TMC_PMF_FEM_BIT_LOC_TYPE                           ARAD_PMF_FEM_BIT_LOC_TYPE;

typedef SOC_TMC_PMF_FEM_INPUT_SRC_ARAD                         ARAD_PMF_FEM_INPUT_SRC_ARAD;
typedef SOC_TMC_PMF_FES_INPUT_INFO                             ARAD_PMF_FES_INPUT_INFO;
typedef SOC_TMC_PMF_FEM_INPUT_INFO                             ARAD_PMF_FEM_INPUT_INFO;
typedef SOC_TMC_PMF_FEM_NDX                                    ARAD_PMF_FEM_NDX;
typedef SOC_TMC_PMF_FEM_SELECTED_BITS_INFO                     ARAD_PMF_FEM_SELECTED_BITS_INFO;
typedef SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO                 ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO;
typedef SOC_TMC_PMF_FEM_BIT_LOC                                ARAD_PMF_FEM_BIT_LOC;
typedef SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO                     ARAD_PMF_FEM_ACTION_FORMAT_INFO;

#define ARAD_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT (1)
#define ARAD_PMF_FEM_ACTION_DEFAULT_DEST_1        (1)
#define ARAD_PMF_FEM_FTMH_ACTION_FORMAT_NDX       (1)
#define ARAD_PMF_FEM_ETH_ACTION_FORMAT_NDX        (2)
#define ARAD_PMF_FEM_ACTION_DEFAULT_NOP_3         (3)










typedef struct
{
    
    SOC_PPC_FP_ACTION_TYPE    action_type;

    uint32 msb;
    uint32 lsb;
    uint32 base0;
    uint32 base1;

    
    uint32 lsb_hw;
    uint32 size;
} ARAD_PMF_FEM_ACTION_EGRESS_SIGNAL;

typedef struct
{
    
    SOC_PPC_FP_ACTION_TYPE    action_type;

    
    uint32 lsb_hw;
    uint32 size;
    int valid; 
} ARAD_PMF_FEM_ACTION_EGRESS_INFO;




extern CONST soc_mem_t Arad_pmf_fem_map_tbl[];
extern CONST soc_field_t Arad_pmf_fem_map_field_select_field[];








uint32 
     arad_pmf_fem_map_tbl_reference(
        SOC_SAND_IN  int                         unit,
        SOC_SAND_IN  uint32                      fem_type,
        SOC_SAND_OUT soc_mem_t                   *mem
     );


uint32
  arad_pmf_fem_action_type_array_size_get_unsafe(
      SOC_SAND_IN  int                            unit
  ) ;


uint32
  arad_pmf_fem_action_type_array_size_default_get_unsafe(
      SOC_SAND_IN  int                            unit
  ) ;


uint32
  arad_pmf_fem_action_type_array_element_default_get_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                table_line,
      SOC_SAND_IN  uint32                sub_index
  ) ;


uint32
  arad_pmf_fem_action_type_array_element_get_unsafe(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  uint32                            table_line,
      SOC_SAND_IN  uint32                            sub_index
  );

uint32
  arad_pmf_fem_action_width_default_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE  in_action_type,
      SOC_SAND_OUT uint32                  *out_action_width
  ) ;

uint32
  arad_pmf_is_field_initialized(
      SOC_SAND_IN  int   unit,
      SOC_SAND_OUT uint8 *field_is_initialized_p
  ) ;

uint32
  arad_pmf_fem_action_width_set_unsafe(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE  in_action_type,
      SOC_SAND_IN  uint32                  in_action_width,
      SOC_SAND_OUT uint32                  *db_identifier_p
  ) ;

uint32
  arad_pmf_fem_action_width_get_unsafe(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE  in_action_type,
      SOC_SAND_OUT uint32                  *out_action_width,
      SOC_SAND_OUT uint32                  *out_hw_id
  ) ;


uint32
  arad_pmf_fem_action_egress_info_get(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE               action_type,
      SOC_SAND_OUT uint8                             *is_found,
      SOC_SAND_OUT ARAD_PMF_FEM_ACTION_EGRESS_SIGNAL *action_egress_info
  );

uint32
  arad_pmf_low_level_fem_tag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
    arad_pmf_fem_action_elk_result_size_update(int unit, int res_sizes[ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS]);

uint32
  arad_egress_pmf_db_action_get_unsafe(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE  action_type,
      SOC_SAND_OUT uint32                 *action_lsb,
      SOC_SAND_OUT uint32                 *action_size,
      SOC_SAND_OUT int                    *action_valid
  );


uint32
  arad_pmf_db_fes_action_table_line_get_unsafe(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE     action_type,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_OUT uint32       *table_line,
      SOC_SAND_OUT int          *found
  );


uint32
  arad_pmf_db_fes_action_size_get_unsafe(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE     action_type,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_OUT uint32                 *action_size,
      SOC_SAND_OUT uint32                 *action_lsb_egress
  );


uint32
  arad_pmf_db_action_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 action_type_hw,
    SOC_SAND_OUT  uint8                    *is_found,
    SOC_SAND_OUT SOC_PPC_FP_ACTION_TYPE *action_type_sw
  );

uint32
  arad_pmf_fem_output_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX   *fem_ndx,
    SOC_SAND_OUT uint32            *output_size_in_bits
  );

uint32
    arad_pmf_fem_pgm_duplicate(
          SOC_SAND_IN  int                   unit,
          SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
          SOC_SAND_IN  uint32                   pmf_pgm_from,
          SOC_SAND_IN  uint32                   pmf_pgm_to,
          SOC_SAND_IN  uint32                   mem_offset
      );



uint32
  arad_pmf_db_fem_input_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint8			        is_fes,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_INPUT_INFO   *info
  );

uint32
  arad_pmf_db_fem_input_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint8			        is_fes,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_INPUT_INFO             *info
  );

uint32
  arad_pmf_db_fem_input_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint8			        is_fes,
    SOC_SAND_IN  uint32                  fem_fes_ndx
  );


uint32
  arad_pmf_db_fem_input_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint8			        is_fes,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_OUT ARAD_PMF_FEM_INPUT_INFO             *info
  );


uint32
  arad_pmf_db_fes_user_header_sizes_get(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_OUT uint32                 *user_header_0_size,
      SOC_SAND_OUT uint32                 *user_header_1_size,
      SOC_SAND_OUT uint32                 *user_header_egress_pmf_offset_0,
      SOC_SAND_OUT uint32                 *user_header_egress_pmf_offset_1
  );


uint32
  arad_pmf_db_fes_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_IN  ARAD_PMF_FES_INPUT_INFO   *info
  );

uint32
  arad_pmf_db_fes_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_IN  ARAD_PMF_FES_INPUT_INFO             *info
  );

uint32
  arad_pmf_db_fes_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint32                  fem_fes_ndx
  );

uint32
  arad_pmf_db_fes_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32			        pmf_pgm_ndx,
    SOC_SAND_IN  uint32                  fem_fes_ndx,
    SOC_SAND_OUT ARAD_PMF_FES_INPUT_INFO             *info
  );

uint32
  arad_pmf_db_fes_move_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32			         pmf_pgm_ndx,
    SOC_SAND_IN  uint32                  from_fem_fes_ndx,
    SOC_SAND_IN  uint32                  to_fem_fes_ndx,
    SOC_SAND_IN  ARAD_PMF_FES            *fes_info
  );

uint32
  arad_pmf_db_fes_chunk_move_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  uint32                 flags,
    SOC_SAND_IN  uint32                 start_fes_ndx,
    SOC_SAND_IN  uint32                 end_fes_ndx,
    SOC_SAND_IN  int                    move_step,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS]
  );


uint32
  arad_pmf_fem_select_bits_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_SELECTED_BITS_INFO     *info
  );

uint32
  arad_pmf_fem_select_bits_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_SELECTED_BITS_INFO     *info
  );

uint32
  arad_pmf_fem_select_bits_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx
  );


uint32
  arad_pmf_fem_select_bits_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_OUT ARAD_PMF_FEM_SELECTED_BITS_INFO     *info
  );


uint32
  arad_pmf_fem_action_format_map_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  arad_pmf_fem_action_format_map_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  arad_pmf_fem_action_format_map_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx
  );


uint32
  arad_pmf_fem_action_format_map_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_OUT ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );


uint32
  arad_pmf_fem_action_format_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint8                            fem_pgm_id,
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_INFO     *info
  );

uint32
  arad_pmf_fem_action_format_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint8                            fem_pgm_id,
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_INFO     *info
  );

uint32
  arad_pmf_fem_action_format_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint8                            fem_pgm_id
  );


uint32
  arad_pmf_fem_action_format_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint8                            fem_pgm_id,
    SOC_SAND_OUT ARAD_PMF_FEM_ACTION_FORMAT_INFO     *info
  );


uint32
  ARAD_PMF_FEM_INPUT_INFO_verify(
     SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FEM_INPUT_INFO *info
  );

uint32
  ARAD_PMF_FES_INPUT_INFO_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_FES_INPUT_INFO *info
  );

uint32
  ARAD_PMF_FEM_NDX_verify(
    SOC_SAND_IN  ARAD_PMF_FEM_NDX *info
  );

uint32
  ARAD_PMF_FEM_SELECTED_BITS_INFO_verify(
    SOC_SAND_IN  ARAD_PMF_FEM_SELECTED_BITS_INFO *info
  );

uint32
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_verify(
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

uint32
  ARAD_PMF_FEM_BIT_LOC_verify(
    SOC_SAND_IN  ARAD_PMF_FEM_BIT_LOC *info
  );

uint32
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_INFO *info
  );

void
  ARAD_PMF_FEM_INPUT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_INPUT_INFO *info
  );

void
  ARAD_PMF_FES_INPUT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_FES_INPUT_INFO *info
  );

void
  ARAD_PMF_FEM_NDX_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_NDX *info
  );

void
  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  ARAD_PMF_FEM_BIT_LOC_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_BIT_LOC *info
  );

void
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1
const char*
  ARAD_PMF_FEM_BIT_LOC_TYPE_to_string(
    SOC_SAND_IN  ARAD_PMF_FEM_BIT_LOC_TYPE enum_val
  );

void
  ARAD_PMF_FEM_INPUT_INFO_print(
    SOC_SAND_IN  ARAD_PMF_FEM_INPUT_INFO *info
  );

void
  ARAD_PMF_FEM_NDX_print(
    SOC_SAND_IN  ARAD_PMF_FEM_NDX *info
  );

void
  ARAD_PMF_FEM_SELECTED_BITS_INFO_print(
    SOC_SAND_IN  ARAD_PMF_FEM_SELECTED_BITS_INFO *info
  );

void
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  );

void
  ARAD_PMF_FEM_BIT_LOC_print(
    SOC_SAND_IN  ARAD_PMF_FEM_BIT_LOC *info
  );

void
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_print(
    SOC_SAND_IN  ARAD_PMF_FEM_ACTION_FORMAT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



