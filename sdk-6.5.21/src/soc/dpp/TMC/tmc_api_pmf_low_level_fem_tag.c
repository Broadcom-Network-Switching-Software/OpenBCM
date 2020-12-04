/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_pmf_low_level_fem_tag.h>





















void
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_INPUT_SRC_ARAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_INPUT_SRC_ARAD));
  info->is_key_src = FALSE;
  info->key_tcam_id = 0;
  info->is_nop = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_INPUT_INFO));
  info->pgm_id = 0;
  info->src = SOC_TMC_NOF_PMF_FEM_INPUT_SRCS;
  info->db_id = SOC_TMC_PMF_LOW_LEVEL_NOF_DATABASES;
  info->is_16_lsb_overridden = FALSE;
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_clear(&(info->src_arad));
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_clear(&(info->lsb16_src));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FES_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FES_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FES_INPUT_INFO));
  info->shift = 0;
  info->action_type = BCM_FIELD_ENTRY_INVALID;
  info->is_action_always_valid = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TAG_SRC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TAG_SRC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TAG_SRC_INFO));
  info->val_src = SOC_TMC_NOF_PMF_TAG_VAL_SRCS;
  info->stat_tag_lsb_position = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_NDX_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_NDX));
  info->id = 0;
  info->cycle_ndx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_SELECTED_BITS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_SELECTED_BITS_INFO));
  info->sel_bit_msb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO));
  info->action_fomat_id = 0;
  info->map_data = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_BIT_LOC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_BIT_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_BIT_LOC));
  info->type = SOC_TMC_PMF_FEM_BIT_LOC_TYPE_CST; 
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO));
  info->type = SOC_TMC_NOF_PMF_FEM_ACTION_TYPES;
  info->size = 0;
  for (ind = 0; ind < SOC_TMC_PMF_FEM_MAX_OUTPUT_SIZE_IN_BITS; ++ind)
  {
    SOC_TMC_PMF_FEM_BIT_LOC_clear(&(info->bit_loc[ind]));
  }
  info->base_value = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_TMC_DEBUG_IS_LVL1


const char*
  SOC_TMC_PMF_FEM_INPUT_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_FEM_INPUT_SRC_A_31_0:
    str = "a_31_0";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_A_47_16:
    str = "a_47_16";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_A_63_32:
    str = "a_63_32";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_A_79_48:
    str = "a_79_48";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_A_95_64:
    str = "a_95_64";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_B_31_0:
    str = "b_31_0";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_B_47_16:
    str = "b_47_16";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_B_63_32:
    str = "b_63_32";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_B_79_48:
    str = "b_79_48";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_B_95_64:
    str = "b_95_64";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_0:
    str = "tcam_0";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_1:
    str = "tcam_1";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_2:
    str = "tcam_2";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_TCAM_3:
    str = "tcam_3";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_DIR_TBL:
    str = "dir_tbl";
  break;
  case SOC_TMC_PMF_FEM_INPUT_SRC_NOP:
    str = "nop";
  break;
  case SOC_TMC_PMF_FEM_INPUT_TCAM:
    str = "tcam_general";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TAG_TYPE_STAT_TAG:
    str = "stat_tag";
  break;
  case SOC_TMC_PMF_TAG_TYPE_LAG_LB_KEY:
    str = "lag_lb_key";
  break;
  case SOC_TMC_PMF_TAG_TYPE_ECMP_LB_KEY:
    str = "ecmp_lb_key";
  break;
  case SOC_TMC_PMF_TAG_TYPE_STACK_RT_HIST:
    str = "stack_rt_hist";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_TAG_VAL_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_VAL_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_TAG_VAL_SRC_A_29_0:
    str = "a_29_0";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_A_61_32:
    str = "a_61_32";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_B_29_0:
    str = "b_29_0";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_B_61_32:
    str = "b_61_32";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_TCAM_0:
    str = "tcam_0";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_TCAM_1:
    str = "tcam_1";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_TCAM_2:
    str = "tcam_2";
  break;
  case SOC_TMC_PMF_TAG_VAL_SRC_DIR_TBL:
    str = "dir_tbl";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_FEM_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_FEM_ACTION_TYPE_DEST:
    str = "dest";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_DP:
    str = "dp";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_TC:
    str = "tc";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_TRAP:
    str = "fwd";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_SNP:
    str = "snp";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_MIRROR:
    str = "mirror";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_MIR_DIS:
    str = "mir_dis";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_EXC_SRC:
    str = "exc_src";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_IS:
    str = "is";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_METER:
    str = "meter";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_COUNTER:
    str = "counter";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_STAT:
    str = "stat";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_OUTLIF:
    str = "outlif";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA:
    str = "type_2nd_pass_data";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM:
    str = "type_2nd_pass_pgm";
  break;
  case SOC_TMC_PMF_FEM_ACTION_TYPE_NOP:
    str = "nop";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PMF_FEM_BIT_LOC_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_BIT_LOC_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_FEM_BIT_LOC_TYPE_CST:
    str = "cst";
  break;
  case SOC_TMC_PMF_FEM_BIT_LOC_TYPE_KEY:
    str = "key";
  break;
  case SOC_TMC_PMF_FEM_BIT_LOC_TYPE_MAP_DATA:
    str = "map_data";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_PMF_FEM_INPUT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "FEM-Program: %u, "),info->pgm_id));
  LOG_CLI((BSL_META_U(unit,
                      "source: %s, "), SOC_TMC_PMF_FEM_INPUT_SRC_to_string(info->src)));
  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u\n\r"),info->db_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_INPUT_SRC_ARAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_nop: %u, "),info->is_nop));
  LOG_CLI((BSL_META_U(unit,
                      "is_key_src: %u, "),info->is_key_src));
  LOG_CLI((BSL_META_U(unit,
                      "key_tcam_id: %u, "),info->key_tcam_id));
  LOG_CLI((BSL_META_U(unit,
                      "key_lsb: %u, "),info->key_lsb));
  LOG_CLI((BSL_META_U(unit,
                      "lookup_cycle_id: %u, "),info->lookup_cycle_id));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TAG_SRC_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TAG_SRC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "val_src %s "), SOC_TMC_PMF_TAG_VAL_SRC_to_string(info->val_src)));
  LOG_CLI((BSL_META_U(unit,
                      "stat_tag_lsb_position: %u\n\r"),info->stat_tag_lsb_position));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_NDX_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "id: %u\n\r"),info->id));
  LOG_CLI((BSL_META_U(unit,
                      "cycle_ndx: %u\n\r"),info->cycle_ndx));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sel_bit_msb: %u\n\r"),info->sel_bit_msb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "action_fomat_id: %u\n\r"),info->action_fomat_id));
  LOG_CLI((BSL_META_U(unit,
                      "map_data: %u\n\r"),info->map_data));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_BIT_LOC_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_BIT_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_PMF_FEM_BIT_LOC_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_PMF_FEM_ACTION_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "size: %u\n\r"),info->size));
  for (ind = 0; ind < SOC_TMC_PMF_FEM_MAX_OUTPUT_SIZE_IN_BITS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "bit_loc[%u]:"),ind));
    SOC_TMC_PMF_FEM_BIT_LOC_print(&(info->bit_loc[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "base_value: %u\n\r"),info->base_value));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

