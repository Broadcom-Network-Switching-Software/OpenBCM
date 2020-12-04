/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_PACKET_INCLUDED__

#define __SOC_TMC_API_PACKET_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>








#define SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE        (10 * 1024)












typedef enum
{
 
  SOC_TMC_PACKET_SEND_PATH_TYPE_INGRESS=0,
 
  SOC_TMC_PACKET_SEND_PATH_TYPE_EGRESS=1,

  SOC_TMC_PACKET_SEND_NOF_PATH_TYPES=2
}SOC_TMC_PACKET_SEND_PATH_TYPE;

typedef enum
{
 
  SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB=0,
 
  SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB=1,

  SOC_TMC_PACKET_SEND_NOF_RECV_MODES=2
}SOC_TMC_PKT_PACKET_RECV_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 data[SOC_TMC_PKT_MAX_CPU_PACKET_BYTE_SIZE];
  
  uint32 data_byte_size;

}SOC_TMC_PKT_PACKET_BUFFER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_TMC_PKT_PACKET_BUFFER packet;
 
  SOC_TMC_PACKET_SEND_PATH_TYPE path_type;

}SOC_TMC_PKT_TX_PACKET_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_TMC_PKT_PACKET_BUFFER packet;

}SOC_TMC_PKT_RX_PACKET_INFO;

typedef
  uint32
    (*SOC_TMC_PKT_PACKET_SEND_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_TMC_PKT_TX_PACKET_INFO     *tx_packet
   );

typedef
  uint32
    (*SOC_TMC_PKT_PACKET_RECV_TRANSFER_PTR)(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                     bytes_to_get,
      SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO     *rx_packet
    );

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PKT_PACKET_SEND_TRANSFER_PTR packet_send;

  
  SOC_TMC_PKT_PACKET_RECV_TRANSFER_PTR  packet_recv;
} SOC_TMC_PKT_PACKET_TRANSFER;








void
  SOC_TMC_PKT_PACKET_BUFFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_BUFFER *info
  );

void
  SOC_TMC_PKT_TX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_TX_PACKET_INFO *info
  );

void
  SOC_TMC_PKT_RX_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PKT_RX_PACKET_INFO *info
  );

void
  SOC_TMC_PKT_PACKET_TRANSFER_clear(
    SOC_SAND_OUT SOC_TMC_PKT_PACKET_TRANSFER *info
  );

#if SOC_TMC_DEBUG_IS_LVL1
const char*
  SOC_TMC_PACKET_SEND_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PACKET_SEND_PATH_TYPE enum_val
  );

const char*
  SOC_TMC_PKT_PACKET_RECV_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_PKT_PACKET_RECV_MODE enum_val
  );

void
  SOC_TMC_PKT_PACKET_BUFFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_BUFFER *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_TX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_TX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_RX_PACKET_INFO_print(
    SOC_SAND_IN SOC_TMC_PKT_RX_PACKET_INFO *info,
    SOC_SAND_IN SOC_TMC_PKT_PACKET_RECV_MODE recv_to_msb
  );

void
  SOC_TMC_PKT_PACKET_TRANSFER_print(
    SOC_SAND_IN SOC_TMC_PKT_PACKET_TRANSFER *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
