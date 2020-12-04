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

#include <soc/dpp/PPC/ppc_api_frwrd_trill.h>





















void
  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRILL_MC_MASKED_FIELDS));
  info->mask_ing_nickname = 0;
  info->mask_adjacent_nickname = 0;
  info->mask_fid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRILL_MC_ROUTE_KEY));
  info->tree_nick = 0;
  info->fid = 0;
  info->ing_nick = 0;
  info->adjacent_eep = 0;
  info->esadi = 0;
  info->tts = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRILL_ADJ_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRILL_ADJ_INFO));
  info->expect_adjacent_eep = 0;
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&(info->expect_system_port));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO));
  info->cfg_ttl = 0;
  info->ethertype = DEFAULT_TRILL_ETHER_TYPE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_TRILL_MC_MASKED_FIELDS_print(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mask_ing_nickname: %u\n\r"),info->mask_ing_nickname));
  LOG_CLI((BSL_META_U(unit,
                      "mask_adjacent_nickname: %u\n\r"),info->mask_adjacent_nickname));
  LOG_CLI((BSL_META_U(unit,
                      "mask_fid: %u\n\r"),info->mask_fid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRILL_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tree_nick: %u\n\r"),info->tree_nick));
  LOG_CLI((BSL_META_U(unit,
                      "fid: %u\n\r"),info->fid));
  LOG_CLI((BSL_META_U(unit,
                      "ing_nick: %u\n\r"),info->ing_nick));
  LOG_CLI((BSL_META_U(unit,
                      "adjacent_eep: %u\n\r"),info->adjacent_eep));
  LOG_CLI((BSL_META_U(unit,
                      "esadi: %u\n\r"),info->esadi));
  LOG_CLI((BSL_META_U(unit,
                      "tts: %u\n\r"),info->tts));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRILL_ADJ_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "expect_adjacent_eep: %u\n\r"),info->expect_adjacent_eep));
  LOG_CLI((BSL_META_U(unit,
                      "expect_system_port:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print(&(info->expect_system_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cfg_ttl: %u\n\r"),info->cfg_ttl));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

