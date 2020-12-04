/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_packet.h>


























void
  SOC_TMC_PKT_PACKET_BUFFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PKT_PACKET_BUFFER));
  for (ind = 0; ind < SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE; ++ind)
  {
    info->data[ind] = 0;
  }
  info->data_byte_size = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_TX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_TX_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PKT_TX_PACKET_INFO));

  SOC_TMC_PKT_PACKET_BUFFER_clear(&(info->packet));
  info->path_type = SOC_TMC_PACKET_SEND_NOF_PATH_TYPES;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_RX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PKT_RX_PACKET_INFO));

  SOC_TMC_PKT_PACKET_BUFFER_clear(&(info->packet));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_PACKET_TRANSFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_TRANSFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_PKT_PACKET_TRANSFER));
  info->packet_send = NULL;
  info->packet_recv = NULL;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PACKET_SEND_PATH_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_PACKET_SEND_PATH_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PACKET_SEND_PATH_TYPE_INGRESS:
    str = "ingress";
  break;
  case SOC_TMC_PACKET_SEND_PATH_TYPE_EGRESS:
    str = "egress";
  break;
  default:
    str = "Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PKT_PACKET_RECV_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_PKT_PACKET_RECV_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB:
    str = "msb_to_lsb";
  break;
  case SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB:
    str = "lsb_to_msb";
  break;
  default:
    str = " Unknown";
  }
  return str;
}
void
  SOC_TMC_PKT_PACKET_BUFFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_BUFFER *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  uint32 ind=0;
  char next_char;
  static int32 bytes_per_row = 10;


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->data_byte_size > SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE)
  {
    LOG_CLI((BSL_META_U(unit,
                        "Invalid data size (%u)\n\r"
                        "Maximal allowed is %u"),
             info->data_byte_size,
             SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE
             ));
    SOC_TMC_DO_NOTHING_AND_EXIT;
  }

  for (ind=0; ind < info->data_byte_size; ++ind)
  {
    if (recv_to_msb == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
    {
      next_char = info->data[SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE - ind - 1];
    }
    else
    {
      next_char = info->data[ind];
    }

    if ((ind % bytes_per_row) == 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          "Data[%04u-%04u]:   "), ind, SOC_SAND_MIN((ind+bytes_per_row-1), (info->data_byte_size-1))));
    }

    LOG_CLI((BSL_META_U(unit,
                        "%02x "), next_char));

    if ((((ind+1) % bytes_per_row) == 0) || ((ind+1) == info->data_byte_size))
    {
      LOG_CLI((BSL_META_U(unit,
                          "\n\r")));
    }
  }

  LOG_CLI((BSL_META_U(unit,
                      "Data_byte_size: %u\n\r"),info->data_byte_size));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_TX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_TX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_PACKET_BUFFER_print(&(info->packet), recv_to_msb);
  LOG_CLI((BSL_META_U(unit,
                      "Path type:     %s \n\r"),
           SOC_TMC_PACKET_SEND_PATH_TYPE_to_string(info->path_type)
           ));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_RX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_RX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PKT_PACKET_BUFFER_print(&(info->packet), recv_to_msb);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_PKT_PACKET_TRANSFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_TRANSFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

