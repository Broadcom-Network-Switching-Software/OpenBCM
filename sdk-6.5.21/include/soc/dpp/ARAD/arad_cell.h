/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_CELL_INCLUDED__

#define __ARAD_CELL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dcmn/vsc256_fabric_cell.h>
#include <soc/dcmn/dcmn_fabric_cell.h>
#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>

#include <soc/dpp/TMC/tmc_api_cell.h>




#define SOC_DPP_ARAD_NUM_OF_ROUTE_GROUPS    (32)
#define SOC_DPP_ARAD_NUM_OF_ENTRIES         (2048)

#define ARAD_DATA_CELL_BYTE_SIZE           (139)
#define ARAD_DATA_CELL_U32_SIZE            (35)

#define ARAD_DATA_CELL_RECEIVED_BYTE_SIZE                 (73)
#define ARAD_DATA_CELL_RECEIVED_UINT32_SIZE                 (19)
#define ARAD_RECEIVED_DATA_CELL_HEADER_START              (520)
#define ARAD_RECEIVED_DATA_CELL_HEADER_OFFSET_VSC256      (16)
#define ARAD_RECEIVED_DATA_CELL_TYPE_START                (583)
#define ARAD_RECEIVED_DATA_CELL_SIZE_IN_BITS              (512)
#define ARAD_RECUEVED_DATA_CELL_CELL_SIZE_LENGTH_IN_BITS  (8)

#define ARAD_RECEIVED_DATA_CELL_HEADER_SIZE_IN_BITS_VSC128 (56)

#define ARAD_DATA_CELL_NOT_INBAND_BYTE_SIZE     (64)
#define ARAD_MIN_PAYLOAD_BYTE_SIZE              (64)

#define ARAD_PARSING_CELL_TYPE_START            (512 + 63)
#define ARAD_PARSING_CELL_TYPE_LENGTH           (2)

#define ARAD_SR_DATA_CELL_PAYLOAD_START         (0)
#define ARAD_SR_DATA_CELL_PAYLOAD_LENGTH        (1024)
#define ARAD_SR_DATA_CELL_PAYLOAD_LENGTH_U32    (32)

#define ARAD_INBAND_PAYLOAD_CELL_OFFSET         (512)

#define ARAD_CELL_NOF_LINKS_IN_PATH_LINKS            (4)
#define ARAD_CELL_NOF_BYTES_IN_UINT32                  (4)
#define ARAD_CELL_NOF_DATA_WORDS_IN_INDIRECT_CELL    (3)

#define ARAD_IN_BAND_TIMEOUT_ATTEMPTS_CFG       (10)




#define ARAD_DO_NOTHING_AND_EXIT    goto exit






typedef struct
{
  
  uint16  current_cell_ident;
}  ARAD_CELL;











int soc_dpp_arad_fabric_inband_is_valid(int unit, int id, uint32 *is_valid);
int soc_dpp_arad_fabric_inband_route_group_set(int unit, int group_id, int flags, int route_count, int *route_ids);
int soc_dpp_arad_fabric_inband_route_set(int unit, int route_id, soc_fabric_inband_route_t *route);
int soc_dpp_arad_fabric_inband_route_group_get(int unit, int group_id, int flags, int route_count_max, int *route_count, int *route_ids);
int soc_dpp_arad_fabric_inband_route_get(int unit, int route_id, soc_fabric_inband_route_t *route);

soc_error_t
soc_arad_parse_cell(int unit, uint8 is_inband, uint32* buf, vsc256_sr_cell_t* cell);

uint32
  arad_sr_send_cell(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       is_fe1600,
    SOC_SAND_IN  uint32        route_group_id,
    SOC_SAND_IN  uint8       is_group,
    SOC_SAND_IN  SOC_SAND_DATA_CELL    *data_cell_sent_old_format,
    SOC_SAND_IN  vsc256_sr_cell_t  *data_cell_sent
  );

uint32
  arad_cell_ack_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      is_inband,
    SOC_SAND_OUT uint32                       *ack,
    SOC_SAND_OUT uint8                      *success
  );

uint32
  arad_transaction_with_fe1600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        route_group_id,
    SOC_SAND_IN  uint8                       is_group,
    SOC_SAND_IN  soc_reg_t                       *regs,
    SOC_SAND_IN  soc_mem_t                       *mems,
    SOC_SAND_IN  int32                        *port_or_copyno,
    SOC_SAND_IN  int32                        *index,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_IN  uint8                       is_memory,
    SOC_SAND_OUT soc_reg_above_64_val_t          *data_out,
    SOC_SAND_OUT uint32                        *data_out_size
  );

uint32
  arad_cpu2cpu_write_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        *data_in
  );

uint32
  arad_cpu2cpu_read_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint32                        *data_out
  );

uint32
  arad_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  );

uint32
  arad_indirect_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  );

uint32
  arad_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  );


uint32
  arad_sr_send_and_wait_ack(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  vsc256_sr_cell_t                 *data_cell_sent,
    SOC_SAND_IN  SOC_SAND_DATA_CELL                   *data_cell_sent_old,
    SOC_SAND_IN  uint32                         route_group_id,
    SOC_SAND_IN  uint8                        is_group,
    SOC_SAND_OUT vsc256_sr_cell_t                 *data_cell_rcv,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell_rcv_old
  );

uint32
  arad_fabric_cpu2cpu_write(    
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN dcmn_sr_cell_link_list_t                *sr_link_list,
    SOC_SAND_IN uint32                                  data_in_size,
    SOC_SAND_IN uint32                                  *data_in
   );

uint32
  arad_fabric_cell_cpu_data_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT uint32                  *cell_buffer
  );


#define SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT_NOF_LINES    (18)
#define SOC_ARAD_FABRIC_CELL_CAPTURED_SRC_FORMAT {\
  \
    {soc_dcmn_fabric_cell_dest_id_src_cell_type,        NULL,         0,                583,            2,      "CELL_TYPE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_device,       NULL,         0,                555,            11,     "SOURCE_DEVICE"},\
    {soc_dcmn_fabric_cell_dest_id_src_src_level,        NULL,         0,                552,            3,      "SOURCE_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_dest_level,       NULL,         0,                549,            3,      "DEST_LEVEL"},\
    {soc_dcmn_fabric_cell_dest_id_src_fip,              NULL,         0,                544,            5,      "FIP[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fip,              NULL,         5,                527,            1,      "FIP[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,              NULL,         0,                539,            5,      "FE1[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,              NULL,         5,                526,            1,      "FE1[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe1,              NULL,         6,                569,            2,      "FE1[6:7]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,              NULL,         0,                533,            6,      "FE2[0:5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,              NULL,         6,                525,            1,      "FE2[6]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe2,              NULL,         7,                568,            1,      "FE2[7]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,              NULL,         0,                528,            5,      "FE3[0:4]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,              NULL,         5,                524,            1,      "FE3[5]"},\
    {soc_dcmn_fabric_cell_dest_id_src_fe3,              NULL,         6,                566,            2,      "FE3[6:7]"},\
    {soc_dcmn_fabric_cell_dest_id_src_is_inband,        NULL,         0,                523,            1,      "IS_INBAND"},\
    {soc_dcmn_fabric_cell_dest_id_src_pipe_id,          NULL,         0,                571,            3,      "PIPE_ID"},\
    {soc_dcmn_fabric_cell_dest_id_src_payload,          NULL,         0,                0,              512,    "PAYLOAD"}\
    }


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

