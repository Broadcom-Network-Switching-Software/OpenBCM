/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_FABRIC_INCLUDED__

#define __ARAD_API_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_fabric.h>






#define  ARAD_FABRIC_NOF_COEXIST_DEVICES (SOC_TMC_FABRIC_NOF_COEXIST_DEVICES)


#define  ARAD_FABRIC_NOF_COEXIST_DEV_ENTRIES (SOC_TMC_FABRIC_NOF_COEXIST_DEV_ENTRIES)


#define  ARAD_FABRIC_NOF_FAP20_DEVICES (SOC_TMC_FABRIC_NOF_FAP20_DEVICES)


#define  ARAD_FABRIC_MESH_DEVICES_MAX (SOC_TMC_FABRIC_MESH_DEVICES_MAX)


#define ARAD_FABRIC_SHAPER_BYTES_MAX  (SOC_TMC_FABRIC_SHAPER_BYTES_MAX)


#define ARAD_FABRIC_SHAPER_CELLS_MAX  (SOC_TMC_FABRIC_SHAPER_CELLS_MAX)











#define ARAD_LINK_ERR_TYPE_CRC                            SOC_TMC_LINK_ERR_TYPE_CRC
#define ARAD_LINK_ERR_TYPE_SIZE                           SOC_TMC_LINK_ERR_TYPE_SIZE
#define ARAD_LINK_ERR_TYPE_MISALIGN                       SOC_TMC_LINK_ERR_TYPE_MISALIGN
#define ARAD_LINK_ERR_TYPE_CODE_GROUP                     SOC_TMC_LINK_ERR_TYPE_CODE_GROUP
typedef SOC_TMC_LINK_ERR_TYPE                                  ARAD_LINK_ERR_TYPE;

#define ARAD_LINK_INDICATE_TYPE_SIG_LOCK                  SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK
#define ARAD_LINK_INDICATE_TYPE_ACCEPT_CELL               SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL
#define ARAD_LINK_INDICATE_INTRNL_FIXED                   SOC_TMC_LINK_INDICATE_INTRNL_FIXED
typedef SOC_TMC_LINK_INDICATE_TYPE                             ARAD_LINK_INDICATE_TYPE;

#define ARAD_FABRIC_CONNECT_MODE_FE                       SOC_TMC_FABRIC_CONNECT_MODE_FE
#define ARAD_FABRIC_CONNECT_MODE_BACK2BACK                SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK
#define ARAD_FABRIC_CONNECT_MODE_MESH                     SOC_TMC_FABRIC_CONNECT_MODE_MESH
#define ARAD_FABRIC_CONNECT_MODE_MULT_STAGE_FE            SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE
#define ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP               SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP
#define ARAD_FABRIC_NOF_CONNECT_MODES                     SOC_TMC_FABRIC_NOF_CONNECT_MODES
typedef SOC_TMC_FABRIC_CONNECT_MODE                            ARAD_FABRIC_CONNECT_MODE;

#define ARAD_LINK_STATE_ON                                SOC_TMC_LINK_STATE_ON
#define ARAD_LINK_STATE_OFF                               SOC_TMC_LINK_STATE_OFF
#define ARAD_LINK_NOF_STATES                              SOC_TMC_LINK_NOF_STATES
typedef SOC_TMC_LINK_STATE                                     ARAD_LINK_STATE;

#define ARAD_FABRIC_LINE_CODING_8_10                      SOC_TMC_FABRIC_LINE_CODING_8_10
#define ARAD_FABRIC_LINE_CODING_8_9_FEC                   SOC_TMC_FABRIC_LINE_CODING_8_9_FEC
#define ARAD_FABRIC_NOF_LINE_CODINGS                      SOC_TMC_FABRIC_NOF_LINE_CODINGS
typedef SOC_TMC_FABRIC_LINE_CODING                             ARAD_FABRIC_LINE_CODING;

typedef SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO             ARAD_FABRIC_LINKS_CONNECT_MAP_STAT_INFO;
typedef SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR                 ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR;
typedef SOC_TMC_FABRIC_CELL_FORMAT                             ARAD_FABRIC_CELL_FORMAT;
typedef SOC_TMC_FABRIC_FC                                      ARAD_FABRIC_FC;
typedef SOC_TMC_FABRIC_COEXIST_INFO                            ARAD_FABRIC_COEXIST_INFO;
typedef SOC_TMC_FABRIC_LINKS_STATUS_SINGLE                     ARAD_FABRIC_LINKS_STATUS_SINGLE;
typedef SOC_TMC_FABRIC_LINKS_STATUS_ALL                        ARAD_FABRIC_LINKS_STATUS_ALL;
typedef SOC_TMC_LINK_STATE_INFO                                ARAD_LINK_STATE_INFO;

#define ARAD_FAR_NOF_DEVICE_TYPES                         SOC_TMC_FAR_NOF_DEVICE_TYPES

#define ARAD_SRD_POWER_STATE_UP                           SOC_TMC_SRD_POWER_STATE_UP      
#define ARAD_SRD_POWER_STATE_DOWN                         SOC_TMC_SRD_POWER_STATE_DOWN  
#define ARAD_SRD_POWER_STATE_UP_AND_RELOCK                SOC_TMC_SRD_POWER_STATE_UP_AND_RELOCK
#define ARAD_SRD_NOF_POWER_STATES                         SOC_TMC_SRD_NOF_POWER_STATES                           
typedef SOC_TMC_SRD_POWER_STATE                                ARAD_SRD_POWER_STATE;


#define ARAD_FABRIC_SHAPER_BYTES_MODE                     SOC_TMC_FABRIC_SHAPER_BYTES_MODE
#define ARAD_FABRIC_SHAPER_CELLS_MODE                     SOC_TMC_FABRIC_SHAPER_CELLS_MODE
#define ARAD_FABRIC_SHAPER_NOF_MODES                      SOC_TMC_FABRIC_SHAPER_NOF_MODES
typedef SOC_TMC_FABRIC_SHAPER_MODE                             ARAD_FABRIC_SHAPER_MODE;

typedef SOC_TMC_FABRIC_FC_SHAPER_MODE_INFO                     ARAD_FABRIC_FC_SHAPER_MODE_INFO;

typedef SOC_TMC_SHAPER_INFO                                    ARAD_SHAPER_INFO;

typedef SOC_TMC_FABRIC_FC_SHAPER                              ARAD_FABRIC_FC_SHAPER;

#define ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0                    SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0
#define ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1                    SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1
#define ARAD_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2                    SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH              SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH              SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH              SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH              SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL                       SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL                       SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL                       SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL
#define ARAD_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL                       SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL
#define ARAD_FABRIC_GCI_CONFIG_TYPE_NOF                             SOC_TMC_FABRIC_GCI_CONFIG_TYPE_NOF
typedef SOC_TMC_FABRIC_GCI_CONFIG_TYPE                              ARAD_FABRIC_GCI_CONFIG_TYPE;

#define ARAD_FABRIC_GCI_TYPE_LEAKY_BUCKET                           SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET
#define ARAD_FABRIC_GCI_TYPE_RANDOM_BACKOFF                         SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF
#define ARAD_FABRIC_GCI_TYPE_NOF                                    SOC_TMC_FABRIC_GCI_TYPE_NOF
typedef SOC_TMC_FABRIC_GCI_TYPE                                     ARAD_FABRIC_GCI_TYPE;

#define ARAD_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH                     SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH
#define ARAD_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE                 SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE
#define ARAD_FABRIC_RCI_CONFIG_TYPE_NOF                             SOC_TMC_FABRIC_RCI_CONFIG_TYPE_NOF
typedef SOC_TMC_FABRIC_RCI_CONFIG_TYPE                              ARAD_FABRIC_RCI_CONFIG_TYPE;












uint32
  arad_fabric_fc_enable_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  );


uint32
  arad_fabric_fc_enable_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_FABRIC_FC           *info_rx,
    SOC_SAND_OUT ARAD_FABRIC_FC           *info_tx
  );




uint32
  arad_fabric_fc_shaper_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  );




uint32
  arad_fabric_fc_shaper_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         link_ndx,
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER            *info
  );





uint32
  arad_fabric_stand_alone_fap_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  );

uint32
  arad_fabric_connect_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  );


uint32
  arad_fabric_connect_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_FABRIC_CONNECT_MODE *fabric_mode
  );

uint32
  arad_fabric_topology_status_connectivity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );



void
  arad_ARAD_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT ARAD_FABRIC_FC *info
  );

void
  arad_ARAD_FABRIC_FC_SHAPER_clear(
      SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER *shaper
  );

void
  arad_ARAD_FABRIC_FC_SHAPER_MODE_INFO_clear(
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode
  );

void
  arad_ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  );

void
  arad_ARAD_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT ARAD_FABRIC_CELL_FORMAT *info
  );

void
  arad_ARAD_LINK_STATE_INFO_clear(
    SOC_SAND_OUT ARAD_LINK_STATE_INFO *info
  );
const char*
  arad_ARAD_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN ARAD_FABRIC_CONNECT_MODE enum_val
  );





#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

