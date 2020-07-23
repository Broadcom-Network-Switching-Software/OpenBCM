/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_HEADER_PARSING_UTILS_INCLUDED__

#define __SOC_TMC_HEADER_PARSING_UTILS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/TMC/tmc_api_ports.h>





#define SOC_TMC_HPU_FTMH_SIZE_UINT32S         2










typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base;
  
  uint32 extention_src_port;
}SOC_TMC_HPU_ITMH_HDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 packet_size;
  
  uint32 tr_cls;
  
  SOC_TMC_DEST_SYS_PORT_INFO src_sys_port;
  
  uint32 ofp;
  
  uint32 dp;
  
  uint32 signature;
  
  uint8 pp_header_present;
  
  uint8 out_mirror_dis;
  
  uint8 exclude_src;
  
  uint8 multicast;
}SOC_TMC_HPU_FTMH_BASE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 outlif;
}SOC_TMC_HPU_FTMH_EXT_OUTLIF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_HPU_FTMH_BASE base;
  
  SOC_TMC_HPU_FTMH_EXT_OUTLIF extension;
}SOC_TMC_HPU_FTMH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base[SOC_TMC_HPU_FTMH_SIZE_UINT32S];
  
  uint32 extension_outlif;
}SOC_TMC_HPU_FTMH_HDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 pp_header_present;
  
  uint32 signature;
  
  uint8 multicast;
  
  uint32 dp;
  
  uint32 packet_size;
  
  uint32 tr_cls;
  
  uint32 outlif;
}SOC_TMC_HPU_OTMH_BASE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_DEST_SYS_PORT_INFO src_sys_port;
}SOC_TMC_HPU_OTMH_EXT_SRC_PORT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_DEST_SYS_PORT_INFO dest_sys_port;
}SOC_TMC_HPU_OTMH_EXT_DEST_PORT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_HPU_FTMH_EXT_OUTLIF outlif;
  
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT src;
  
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT dest;
}SOC_TMC_HPU_OTMH_EXTENSIONS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_HPU_OTMH_BASE base;
  
  SOC_TMC_HPU_OTMH_EXTENSIONS extension;
}SOC_TMC_HPU_OTMH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base;
  
  uint32 extension_outlif;
  
  uint32 extension_src_port;
  
  uint32 extension_dest_port;
}SOC_TMC_HPU_OTMH_HDR;











void
  SOC_TMC_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_ITMH_HDR *info
  );

void
  SOC_TMC_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_BASE *info
  );

void
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_EXT_OUTLIF *info
  );

void
  SOC_TMC_HPU_FTMH_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH *info
  );

void
  SOC_TMC_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_FTMH_HDR *info
  );

void
  SOC_TMC_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_BASE *info
  );

void
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXT_SRC_PORT *info
  );

void
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXT_DEST_PORT *info
  );

void
  SOC_TMC_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_EXTENSIONS *info
  );

void
  SOC_TMC_HPU_OTMH_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH *info
  );

void
  SOC_TMC_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT SOC_TMC_HPU_OTMH_HDR *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_HPU_ITMH_HDR_print(
    SOC_SAND_IN SOC_TMC_HPU_ITMH_HDR *info
  );

void
  SOC_TMC_HPU_FTMH_BASE_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_BASE *info
  );

void
  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_EXT_OUTLIF *info
  );

void
  SOC_TMC_HPU_FTMH_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH *info
  );

void
  SOC_TMC_HPU_FTMH_HDR_print(
    SOC_SAND_IN SOC_TMC_HPU_FTMH_HDR *info
  );

void
  SOC_TMC_HPU_OTMH_BASE_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH_BASE *info
  );

void
  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH_EXT_SRC_PORT *info
  );

void
  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH_EXT_DEST_PORT *info
  );

void
  SOC_TMC_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH_EXTENSIONS *info
  );

void
  SOC_TMC_HPU_OTMH_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH *info
  );

void
  SOC_TMC_HPU_OTMH_HDR_print(
    SOC_SAND_IN SOC_TMC_HPU_OTMH_HDR *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
