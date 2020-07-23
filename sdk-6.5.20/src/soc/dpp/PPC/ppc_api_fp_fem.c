/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>




#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_fp_fem.h>





















void
  SOC_PPC_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY *info
  )
{
  uint32
    action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_FEM_ENTRY));
  info->is_for_entry = 0;
  info->db_strength = 0;
  info->db_id = 0;
  info->entry_strength = 0;
  info->entry_id = 0;
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    info->action_type[action_ndx] = SOC_PPC_FP_ACTION_TYPE_INVALID;
    info->is_base_positive[action_ndx] = FALSE;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FP_FEM_CYCLE));
  info->is_cycle_fixed = 0;
  info->cycle_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FP_FEM_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_for_entry: %u\n\r"),info->is_for_entry));
  LOG_CLI((BSL_META_U(unit,
                      "db_strength: %u\n\r"),info->db_strength));
  LOG_CLI((BSL_META_U(unit,
                      "db_id: %u\n\r"),info->db_id));
  LOG_CLI((BSL_META_U(unit,
                      "entry_strength: %u\n\r"),info->entry_strength));
  LOG_CLI((BSL_META_U(unit,
                      "entry_id: %u\n\r"),info->entry_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FP_FEM_CYCLE_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_cycle_fixed: %u\n\r"),info->is_cycle_fixed));
  LOG_CLI((BSL_META_U(unit,
                      "cycle_id: %u\n\r"),info->cycle_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

