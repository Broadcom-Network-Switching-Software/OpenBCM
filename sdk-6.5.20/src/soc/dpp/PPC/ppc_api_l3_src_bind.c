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

#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>





















void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_SRC_BIND_IPV4_ENTRY));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_SRC_BIND_PPPOE_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_PPPOE_ENTRY     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_SRC_BIND_PPPOE_ENTRY));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_SRC_BIND_IPV6_ENTRY));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  LOG_CLI((BSL_META_U(unit,
                      "lif_ndx:%u\n\r"), info->lif_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "sip:%u\n\r"), info->sip));
  if (info->smac_valid)
  {
    LOG_CLI((BSL_META_U(unit,
                        "smac:")));
    soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->smac));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "lif_ndx:%u\n\r"), info->lif_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "sip6:")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&(info->sip6));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  if (info->smac_valid)
  {
    LOG_CLI((BSL_META_U(unit,
                        "smac:")));
    soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->smac));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}




#include <soc/dpp/SAND/Utils/sand_footer.h>

