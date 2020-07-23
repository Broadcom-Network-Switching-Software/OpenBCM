/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/





#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/TMC/tmc_api_header_parsing_utils.h>





























































void
  SOC_TMC_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_ITMH_HDR));
  info->base = 0;
  info->extention_src_port = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_FTMH_BASE));
  info->packet_size = 0;
  info->tr_cls = 0;
  SOC_TMC_DEST_SYS_PORT_INFO_clear(&(info->src_sys_port));
  info->ofp = 0;
  info->dp = 0;
  info->signature = 0;
  info->pp_header_present = 0;
  info->out_mirror_dis = 0;
  info->exclude_src = 0;
  info->multicast = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_FTMH_EXT_OUTLIF));
  info->enable = 0;
  info->outlif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_FTMH));
  SOC_TMC_HPU_FTMH_BASE_clear(&(info->base));
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(&(info->extension));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_HDR *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_FTMH_HDR));
  for (ind=0; ind<SOC_TMC_HPU_FTMH_SIZE_UINT32S; ++ind)
  {
    info->base[ind] = 0;
  }
  info->extension_outlif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH_BASE));
  info->pp_header_present = 0;
  info->signature = 0;
  info->multicast = 0;
  info->dp = 0;
  info->packet_size = 0;
  info->tr_cls = 0;
  info->outlif = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH_EXT_SRC_PORT));
  info->enable = 0;
  SOC_TMC_DEST_SYS_PORT_INFO_clear(&(info->src_sys_port));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH_EXT_DEST_PORT));
  info->enable = 0;
  SOC_TMC_DEST_SYS_PORT_INFO_clear(&(info->dest_sys_port));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH_EXTENSIONS));
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(&(info->outlif));
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_clear(&(info->src));
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_clear(&(info->dest));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH));
  SOC_TMC_HPU_OTMH_BASE_clear(&(info->base));
  SOC_TMC_HPU_OTMH_EXTENSIONS_clear(&(info->extension));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_HPU_OTMH_HDR));
  info->base = 0;
  info->extension_outlif = 0;
  info->extension_src_port = 0;
  info->extension_dest_port = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_HPU_ITMH_HDR_print(
    SOC_SAND_IN SOC_TMC_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base: %u\n\r"),info->base));
  LOG_CLI((BSL_META_U(unit,
                      "extention_src_port: %u\n\r"),info->extention_src_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_BASE_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "packet_size: %u\n\r"),info->packet_size));
  LOG_CLI((BSL_META_U(unit,
                      "tr_cls: %u\n\r"),info->tr_cls));
  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port:")));
  SOC_TMC_DEST_SYS_PORT_INFO_print(&(info->src_sys_port));
  LOG_CLI((BSL_META_U(unit,
                      "ofp: %u\n\r"),info->ofp));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "signature: %u\n\r"),info->signature));
  LOG_CLI((BSL_META_U(unit,
                      "pp_header_present: %u\n\r"),info->pp_header_present));
  LOG_CLI((BSL_META_U(unit,
                      "out_mirror_dis: %u\n\r"),info->out_mirror_dis));
  LOG_CLI((BSL_META_U(unit,
                      "exclude_src: %u\n\r"),info->exclude_src));
  LOG_CLI((BSL_META_U(unit,
                      "multicast: %u\n\r"),info->multicast));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "outlif: %u\n\r"),info->outlif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base:")));
  SOC_TMC_HPU_FTMH_BASE_print(&(info->base));
  LOG_CLI((BSL_META_U(unit,
                      "extension:")));
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(&(info->extension));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_FTMH_HDR_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_HDR *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_HPU_FTMH_SIZE_UINT32S; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "base[%u]: %u\n\r"),ind,info->base[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "extension_outlif: %u\n\r"),info->extension_outlif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_BASE_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "pp_header_present: %u\n\r"),info->pp_header_present));
  LOG_CLI((BSL_META_U(unit,
                      "signature: %u\n\r"),info->signature));
  LOG_CLI((BSL_META_U(unit,
                      "multicast: %u\n\r"),info->multicast));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "packet_size: %u\n\r"),info->packet_size));
  LOG_CLI((BSL_META_U(unit,
                      "tr_cls: %u\n\r"),info->tr_cls));
  LOG_CLI((BSL_META_U(unit,
                      "outlif: %u\n\r"),info->outlif));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port:")));
  SOC_TMC_DEST_SYS_PORT_INFO_print(&(info->src_sys_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "dest_sys_port:")));
  SOC_TMC_DEST_SYS_PORT_INFO_print(&(info->dest_sys_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "outlif:")));
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(&(info->outlif));
  LOG_CLI((BSL_META_U(unit,
                      "src:")));
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_print(&(info->src));
  LOG_CLI((BSL_META_U(unit,
                      "dest:")));
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_print(&(info->dest));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base:")));
  SOC_TMC_HPU_OTMH_BASE_print(&(info->base));
  LOG_CLI((BSL_META_U(unit,
                      "extension:")));
  SOC_TMC_HPU_OTMH_EXTENSIONS_print(&(info->extension));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_HPU_OTMH_HDR_print(
    SOC_SAND_IN  SOC_TMC_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "base: %u\n\r"),info->base));
  LOG_CLI((BSL_META_U(unit,
                      "extension_outlif: %u\n\r"),info->extension_outlif));
  LOG_CLI((BSL_META_U(unit,
                      "extension_src_port: %u\n\r"),info->extension_src_port));
  LOG_CLI((BSL_META_U(unit,
                      "extension_dest_port: %u\n\r"),info->extension_dest_port));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

