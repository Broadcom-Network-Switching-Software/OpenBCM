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

#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>






















void
  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_ENTRY_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_ENTRY_ID));
  info->db_id = 0;
  info->entry_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_PMF_LKP_PROFILE_clear(
    SOC_SAND_OUT SOC_TMC_PMF_LKP_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_LKP_PROFILE));
  info->id = 0;
  info->cycle_ndx = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_BANK_SELECTION_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_BANK_SELECTION *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_BANK_SELECTION));
  for (ind = 0; ind < SOC_TMC_PMF_TCAM_NOF_BANKS; ++ind)
  {
    info->bank_id_enabled[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_RESULT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_RESULT_INFO));
  for (ind = 0; ind < SOC_TMC_PMF_TCAM_NOF_LKP_RESULTS; ++ind)
  {
    SOC_TMC_PMF_TCAM_BANK_SELECTION_clear(&(info->bank_sel[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO));
  info->is_valid = 0;
  info->key_src = SOC_TMC_NOF_PMF_TCAM_KEY_SRCS;
  SOC_TMC_PMF_TCAM_RESULT_INFO_clear(&(info->bank_prio));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_DATA_clear(
    SOC_SAND_OUT SOC_TMC_PMF_TCAM_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_TCAM_DATA));
  SOC_TMC_TCAM_KEY_clear(&(info->key));
  info->priority = 0;
  SOC_TMC_TCAM_OUTPUT_clear(&(info->output));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_PMF_DIRECT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_TMC_PMF_DIRECT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PMF_DIRECT_TBL_DATA));
  info->val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_TMC_DEBUG_IS_LVL1


const char*
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_9_0:
    str = "a_9_0";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_19_10:
    str = "a_19_10";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_29_20:
    str = "a_29_20";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_39_30:
    str = "a_39_30";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_9_0:
    str = "b_9_0";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_19_10:
    str = "b_19_10";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_29_20:
    str = "b_29_20";
  break;
  case SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_39_30:
    str = "b_39_30";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_TMC_PMF_TCAM_ENTRY_ID_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_ENTRY_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u\n\r"),info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "entry_id: %u\n\r"),info->entry_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_LKP_PROFILE_print(
    SOC_SAND_IN  SOC_TMC_PMF_LKP_PROFILE *info
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
  SOC_TMC_PMF_TCAM_BANK_SELECTION_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_BANK_SELECTION *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_PMF_TCAM_NOF_BANKS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "bank_id_enabled[%u]: %u\n\r"),ind,info->bank_id_enabled[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_RESULT_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_RESULT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_PMF_TCAM_NOF_LKP_RESULTS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "bank_sel[%u]:"),ind));
    SOC_TMC_PMF_TCAM_BANK_SELECTION_print(&(info->bank_sel[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_valid: %u\n\r"),info->is_valid));
  LOG_CLI((BSL_META_U(unit,
                      "key_src %s "), SOC_TMC_PMF_TCAM_KEY_SRC_to_string(info->key_src)));
  LOG_CLI((BSL_META_U(unit,
                      "bank_prio:")));
  SOC_TMC_PMF_TCAM_RESULT_INFO_print(&(info->bank_prio));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PMF_TCAM_DATA_print(
    SOC_SAND_IN  SOC_TMC_PMF_TCAM_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key:")));
  SOC_TMC_TCAM_KEY_print(&(info->key));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
  LOG_CLI((BSL_META_U(unit,
                      "output:")));
  SOC_TMC_TCAM_OUTPUT_print(&(info->output));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_TMC_PMF_DIRECT_TBL_DATA_print(
    SOC_SAND_IN  SOC_TMC_PMF_DIRECT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

