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

#include <soc/dpp/PPC/ppc_api_frwrd_ilm.h>





















void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO));
  info->mask_port = 0;
  info->mask_inrif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO));
  soc_sand_SAND_U32_RANGE_clear(&(info->labels_range));
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    info->exp_map_tbl[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_KEY));
  info->in_label = 0;
  info->mapped_exp = 0;
  info->in_local_port = 0;
  info->inrif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_ILM_GLBL_INFO));
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(&(info->key_info));
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(&(info->elsp_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mask_port: %u\n\r"),info->mask_port));
  LOG_CLI((BSL_META_U(unit,
                      "mask_inrif: %u\n\r"),info->mask_inrif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "labels_range: ")));
  soc_sand_SAND_U32_RANGE_print(&(info->labels_range));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "exp_map_tbl[%u]: %u\n\r"), ind,info->exp_map_tbl[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_label: %u\n\r"),info->in_label));
  LOG_CLI((BSL_META_U(unit,
                      "mapped_exp: %u\n\r"), info->mapped_exp));
  LOG_CLI((BSL_META_U(unit,
                      "in_local_port: %u\n\r"),info->in_local_port));
  LOG_CLI((BSL_META_U(unit,
                      "inrif: %u\n\r"),info->inrif));
  LOG_CLI((BSL_META_U(unit,
                      "VRF: %u\n\r"),info->vrf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "key_info:")));
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(&(info->key_info));
  LOG_CLI((BSL_META_U(unit,
                      "elsp_info:")));
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(&(info->elsp_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

