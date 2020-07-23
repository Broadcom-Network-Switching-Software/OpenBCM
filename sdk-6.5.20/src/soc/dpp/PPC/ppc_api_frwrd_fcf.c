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

#include <soc/dpp/PPC/ppc_api_frwrd_fcf.h>






















void
  SOC_PPC_FRWRD_FCF_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FCF_GLBL_INFO));

  info->enable = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FCF_ROUTE_KEY));
  info->flags = 0;
  info->vfi = 0;
  info->d_id = 0;
  info->prefix_len = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

 
void
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FCF_ROUTE_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&info->frwrd_decision);

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FCF_ZONING_KEY));
  info->vfi = 0;
  info->d_id = 0;
  info->s_id = 0;


  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_FRWRD_FCF_ZONING_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&info->frwrd_decision);
  info->flags = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#if SOC_PPC_DEBUG_IS_LVL1


const char*
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_STATUS enum_val
  )
{
  const char* str = " Unknown";
  if(enum_val & SOC_PPC_FRWRD_FCF_ROUTE_STATUS_COMMITED)
    str = "commited";
  if(enum_val & SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_ADD)
    str = "pend_add";
  if(enum_val & SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_REMOVE)
    str = "pend_remove";
  if(enum_val & SOC_PPC_FRWRD_FCF_ROUTE_STATUS_ACCESSED)
    str = "accessed";
  return str;
}

void
  SOC_PPC_FRWRD_FCF_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "info->enable %u\n\r"),info->enable));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  LOG_CLI((BSL_META_U(unit,
                      "flags: %u\n\r"),info->flags));
  LOG_CLI((BSL_META_U(unit,
                      "vfi: %u\n\r"),info->vfi));
  LOG_CLI((BSL_META_U(unit,
                      "d_id: %u\n\r"),info->d_id));
  LOG_CLI((BSL_META_U(unit,
                      "prefix_len: %u\n\r"),info->prefix_len));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "frwrd_decision: \n\r")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&info->frwrd_decision);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_FRWRD_FCF_ZONING_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vfi: %u\n\r"),info->vfi));
  LOG_CLI((BSL_META_U(unit,
                      "d_id: %u\n\r"),info->d_id));
  LOG_CLI((BSL_META_U(unit,
                      "s_id: %u\n\r"),info->s_id));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_FRWRD_FCF_ZONING_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "frwrd_decision:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->frwrd_decision));
  
  LOG_CLI((BSL_META_U(unit,
                      "flags: %u\n\r"),info->flags));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

