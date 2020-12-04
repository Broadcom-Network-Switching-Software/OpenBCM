/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PMF_LOW_LEVEL_DB_INCLUDED__

#define __ARAD_PMF_LOW_LEVEL_DB_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_api_framework.h>

#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>





#define ARAD_PMF_DIRECT_TBL_KEY_SRC_A_9_0                 SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_9_0
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_A_19_10               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_19_10
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_A_29_20               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_29_20
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_A_39_30               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_39_30
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_B_9_0                 SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_9_0
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_B_19_10               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_19_10
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_B_29_20               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_29_20
#define ARAD_PMF_DIRECT_TBL_KEY_SRC_B_39_30               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_39_30
#define ARAD_NOF_PMF_DIRECT_TBL_KEY_SRCS                  SOC_TMC_NOF_PMF_DIRECT_TBL_KEY_SRCS
typedef SOC_TMC_PMF_DIRECT_TBL_KEY_SRC                         ARAD_PMF_DIRECT_TBL_KEY_SRC;



typedef SOC_TMC_PMF_TCAM_ENTRY_ID                              ARAD_PMF_TCAM_ENTRY_ID;
typedef SOC_TMC_PMF_TCAM_DATA                                  ARAD_PMF_TCAM_DATA;
typedef SOC_TMC_PMF_LKP_PROFILE                                ARAD_PMF_LKP_PROFILE;
typedef SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO                    ARAD_PMF_TCAM_KEY_SELECTION_INFO;
typedef SOC_TMC_PMF_TCAM_BANK_SELECTION                        ARAD_PMF_TCAM_BANK_SELECTION;
typedef SOC_TMC_PMF_TCAM_RESULT_INFO                           ARAD_PMF_TCAM_RESULT_INFO;
typedef SOC_TMC_PMF_DIRECT_TBL_DATA                            ARAD_PMF_DIRECT_TBL_DATA;


















uint32
  arad_pmf_low_level_db_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pmf_low_level_tcam_bank_enable_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 cycle,
    SOC_SAND_IN uint8 enabled
  );


uint32
  arad_pmf_tcam_entry_add_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_IN  ARAD_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_IN  uint8                            allow_new_bank,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success
  );

uint32
  arad_pmf_tcam_entry_add_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_IN  ARAD_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_IN  uint8                            allow_new_bank
  );


uint32
  arad_pmf_tcam_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT ARAD_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_OUT uint8                            *is_found
  );

uint32
  arad_pmf_tcam_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx
  );


uint32
  arad_pmf_tcam_entry_remove_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT uint8                         *is_found
  );

uint32
  arad_pmf_tcam_entry_remove_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID              *entry_ndx
  );



uint32
  arad_pmf_db_direct_tbl_key_src_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_KEY_SRC         key_src
  );

uint32
  arad_pmf_db_direct_tbl_key_src_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_KEY_SRC         key_src
  );

uint32
  arad_pmf_db_direct_tbl_key_src_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE                *lkp_profile_ndx
  );


uint32
  arad_pmf_db_direct_tbl_key_src_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_OUT ARAD_PMF_DIRECT_TBL_KEY_SRC         *key_src
  );


uint32
  arad_pmf_db_direct_tbl_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_DATA            *data
  );

uint32
  arad_pmf_db_direct_tbl_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_DATA            *data
  );

uint32
  arad_pmf_db_direct_tbl_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx
  );


uint32
  arad_pmf_db_direct_tbl_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_OUT ARAD_PMF_DIRECT_TBL_DATA            *data
  );


uint32
  ARAD_PMF_TCAM_ENTRY_ID_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID *info
  );

uint32
  ARAD_PMF_LKP_PROFILE_verify(
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE *info
  );

uint32
  ARAD_PMF_TCAM_KEY_SELECTION_INFO_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_KEY_SELECTION_INFO *info
  );
uint32
  ARAD_PMF_TCAM_BANK_SELECTION_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_BANK_SELECTION *info
  );

uint32
  ARAD_PMF_TCAM_RESULT_INFO_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_RESULT_INFO *info
  );


uint32
  ARAD_PMF_TCAM_DATA_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_DATA *info
  );

uint32
  ARAD_PMF_DIRECT_TBL_DATA_verify(
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_DATA *info
  );

void
  ARAD_PMF_TCAM_ENTRY_ID_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_ENTRY_ID *info
  );

void
  ARAD_PMF_LKP_PROFILE_clear(
    SOC_SAND_OUT ARAD_PMF_LKP_PROFILE *info
  );

void
  ARAD_PMF_TCAM_KEY_SELECTION_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  ARAD_PMF_TCAM_BANK_SELECTION_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_BANK_SELECTION *info
  );

void
  ARAD_PMF_TCAM_RESULT_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_RESULT_INFO *info
  );

void
  ARAD_PMF_TCAM_DATA_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_DATA *info
  );

void
  ARAD_PMF_DIRECT_TBL_DATA_clear(
    SOC_SAND_OUT ARAD_PMF_DIRECT_TBL_DATA *info
  );

#if ARAD_DEBUG_IS_LVL1
const char*
  ARAD_PMF_DIRECT_TBL_KEY_SRC_to_string(
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_KEY_SRC enum_val
  );

void
  ARAD_PMF_TCAM_ENTRY_ID_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_ENTRY_ID *info
  );

void
  ARAD_PMF_LKP_PROFILE_print(
    SOC_SAND_IN  ARAD_PMF_LKP_PROFILE *info
  );

void
  ARAD_PMF_TCAM_KEY_SELECTION_INFO_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  ARAD_PMF_TCAM_BANK_SELECTION_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_BANK_SELECTION *info
  );

void
  ARAD_PMF_TCAM_RESULT_INFO_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_RESULT_INFO *info
  );

void
  ARAD_PMF_TCAM_DATA_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_DATA *info
  );

void
  ARAD_PMF_DIRECT_TBL_DATA_print(
    SOC_SAND_IN  ARAD_PMF_DIRECT_TBL_DATA *info
  );

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


