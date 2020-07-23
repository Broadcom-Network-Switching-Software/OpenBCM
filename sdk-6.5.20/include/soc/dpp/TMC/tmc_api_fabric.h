/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_FABRIC_INCLUDED__

#define __SOC_TMC_API_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/dpp_config_defs.h>






#define  SOC_TMC_FABRIC_NOF_COEXIST_DEVICES 256


#define  SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES (SOC_TMC_FABRIC_NOF_COEXIST_DEVICES/2)


#define  SOC_TMC_FABRIC_NOF_FAP20_DEVICES 128


#define  SOC_TMC_FABRIC_MESH_DEVICES_MAX (8)


#define SOC_TMC_FABRIC_SHAPER_BYTES_MAX (65536)


#define SOC_TMC_FABRIC_SHAPER_CELLS_MAX (32768)











typedef enum
{
  
  SOC_TMC_LINK_ERR_TYPE_CRC=0x1,
  
  SOC_TMC_LINK_ERR_TYPE_SIZE=0x2,
  
  SOC_TMC_LINK_ERR_TYPE_MISALIGN=0x4,
  
  SOC_TMC_LINK_ERR_TYPE_CODE_GROUP=0x8,
  
  SOC_TMC_NOF_LINK_ERR_TYPES = 4
}SOC_TMC_LINK_ERR_TYPE;

typedef enum
{
  
  SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK=0x1,
  
  SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL=0x2,
  
  SOC_TMC_LINK_INDICATE_INTRNL_FIXED=0x4,
  
  SOC_TMC_NOF_LINK_INDICATE_TYPES = 3
}SOC_TMC_LINK_INDICATE_TYPE;

typedef enum
{
  
  SOC_TMC_FABRIC_CONNECT_MODE_FE=0,
  
  SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK=1,
  
  SOC_TMC_FABRIC_CONNECT_MODE_MESH=2,
  
  SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE=3,
  
  SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP=4,
  
  SOC_TMC_FABRIC_NOF_CONNECT_MODES=5

}SOC_TMC_FABRIC_CONNECT_MODE;

typedef enum
{
  
  SOC_TMC_LINK_STATE_ON=0,
  
  SOC_TMC_LINK_STATE_OFF=1,
  
  SOC_TMC_LINK_NOF_STATES=2
}SOC_TMC_LINK_STATE;


typedef enum 
{
   
  SOC_TMC_FABRIC_SHAPER_CELLS_MODE = 0,
   
  SOC_TMC_FABRIC_SHAPER_BYTES_MODE = 1,
  
  SOC_TMC_FABRIC_SHAPER_NOF_MODES = 2

}SOC_TMC_FABRIC_SHAPER_MODE;


typedef enum 
{

  
  SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH= 0,

  
  SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE = 1,

  
  SOC_TMC_FABRIC_RCI_CONFIG_TYPE_NOF = 2 

} SOC_TMC_FABRIC_RCI_CONFIG_TYPE;



typedef enum 
{
  
  SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET = 0,

  
  SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF= 1,

  
  SOC_TMC_FABRIC_GCI_TYPE_NOF = 3 

} SOC_TMC_FABRIC_GCI_TYPE;





typedef enum 
{
  
  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0 = 0,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1 = 1,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2 = 2,

  

  
  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH = 3,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH = 4,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH = 5,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH = 6,

  
  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL = 7,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL = 8,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL = 9,

  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL = 10,

  
  SOC_TMC_FABRIC_GCI_CONFIG_TYPE_NOF = 11 

} SOC_TMC_FABRIC_GCI_CONFIG_TYPE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
   
  uint32 cells;
   
  uint32 bytes;
}SOC_TMC_SHAPER_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SHAPER_INFO data_shaper;
  
  SOC_TMC_SHAPER_INFO fc_shaper;
}SOC_TMC_FABRIC_FC_SHAPER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
   
  SOC_TMC_FABRIC_SHAPER_MODE shaper_mode; 

}SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO;

typedef enum
{
  
  SOC_TMC_FABRIC_LINE_CODING_8_10 = 0,
  
  SOC_TMC_FABRIC_LINE_CODING_8_9_FEC = 1,
  
  SOC_TMC_FABRIC_NOF_LINE_CODINGS = 2
}SOC_TMC_FABRIC_LINE_CODING;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 far_unit;
  
  SOC_TMC_FAR_DEVICE_TYPE far_dev_type;
  
  uint32 far_link_id;
  
  uint8 is_connected_expected;
  
  uint8 is_phy_connect;
  
  uint8 is_logically_connected;
}SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO link_info[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
}SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 variable_cell_size_enable;
  
  uint8 segmentation_enable;
}SOC_TMC_FABRIC_CELL_FORMAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
}SOC_TMC_FABRIC_FC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint8 coexist_dest_map[SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES];
  
  uint8 fap_id_odd_link_map[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
}SOC_TMC_FABRIC_COEXIST_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 valid;
  
  uint8 up_tx;
  
  uint8 up_rx;
  
  uint8 errors_bm;
  
  uint8 indications_bm;
  
  uint32 crc_err_counter;
  
  uint32 leaky_bucket_counter;
}SOC_TMC_FABRIC_LINKS_STATUS_SINGLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 error_in_some;
  
  SOC_TMC_FABRIC_LINKS_STATUS_SINGLE single_link_status[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
}SOC_TMC_FABRIC_LINKS_STATUS_ALL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_LINK_STATE on_off;
  
  uint8 serdes_also;
}SOC_TMC_LINK_STATE_INFO;











void
  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  );

void
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  );

void
  SOC_TMC_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_CELL_FORMAT *info
  );

void
  SOC_TMC_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_FC *info
  );

void
  SOC_TMC_FABRIC_COEXIST_INFO_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_COEXIST_INFO *info
  );

void
  SOC_TMC_FABRIC_LINKS_STATUS_SINGLE_clear(
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_STATUS_SINGLE *info
  );

void
  SOC_TMC_FABRIC_LINKS_STATUS_ALL_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_TMC_FABRIC_LINKS_STATUS_ALL *info
  );

void
  SOC_TMC_LINK_STATE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_LINK_STATE_INFO *info
  );

void 
  SOC_TMC_FABRIC_FC_SHAPER_clear(
      SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER *info
  );

void 
  SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO_clear(
      SOC_SAND_OUT SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1


uint32
  soc_tmcfabric_topology_status_connectivity_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

const char*
  SOC_TMC_LINK_ERR_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_LINK_ERR_TYPE enum_val
  );

const char*
  SOC_TMC_LINK_INDICATE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_LINK_INDICATE_TYPE enum_val
  );

const char*
  SOC_TMC_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN SOC_TMC_FABRIC_CONNECT_MODE enum_val
  );

const char*
  SOC_TMC_LINK_STATE_to_string(
    SOC_SAND_IN SOC_TMC_LINK_STATE enum_val
  );

const char*
  SOC_TMC_FABRIC_LINE_CODING_to_string(
    SOC_SAND_IN  SOC_TMC_FABRIC_LINE_CODING enum_val
  );

void
  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info,
    SOC_SAND_IN uint8                         table_format
  );

void
  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR *info,
    SOC_SAND_IN uint8                     table_format
  );

void
  SOC_TMC_FABRIC_CELL_FORMAT_print(
    SOC_SAND_IN SOC_TMC_FABRIC_CELL_FORMAT *info
  );

void
  SOC_TMC_FABRIC_FC_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_FC *info
  );

void
  SOC_TMC_FABRIC_COEXIST_INFO_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_COEXIST_INFO *info
  );


void
  SOC_TMC_FABRIC_LINKS_STATUS_ALL_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_FABRIC_LINKS_STATUS_ALL *info,
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS     print_flavour
  );

void
  SOC_TMC_LINK_STATE_INFO_print(
    SOC_SAND_IN SOC_TMC_LINK_STATE_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
