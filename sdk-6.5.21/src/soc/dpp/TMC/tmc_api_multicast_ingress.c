/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_multicast_ingress.h>
#include <soc/dpp/TMC/tmc_api_general.h>


























void
  SOC_TMC_MULT_ING_TR_CLS_MAP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_ING_TR_CLS_MAP *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_ING_TR_CLS_MAP));
  for (ind=0; ind<SOC_TMC_NOF_TRAFFIC_CLASSES; ++ind)
  {
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_ING_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_MULT_ING_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_ING_ENTRY));
  SOC_TMC_DEST_INFO_clear(&(info->destination));
  info->cud = 1;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_MULT_ING_TR_CLS_MAP_print(
    SOC_SAND_IN SOC_TMC_MULT_ING_TR_CLS_MAP *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_TMC_NOF_TRAFFIC_CLASSES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Map[%u]: %d\n\r"),ind,info->map[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_ING_ENTRY_print(
    SOC_SAND_IN SOC_TMC_MULT_ING_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Destination: ")));
  SOC_TMC_DEST_INFO_print(&(info->destination));
  LOG_CLI((BSL_META_U(unit,
                      "Copy-unique-data: %u\n\r"),info->cud));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

