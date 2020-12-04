/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_TDM_INCLUDED__

#define __ARAD_API_TDM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_tdm.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/dpp/ARAD/arad_api_general.h>






#define ARAD_TDM_PUSH_QUEUE_TYPE                            (SOC_TMC_ITM_QT_NDX_15)









#define ARAD_TDM_ING_ACTION_ADD                              SOC_TMC_TDM_ING_ACTION_ADD
#define ARAD_TDM_ING_ACTION_NO_CHANGE                        SOC_TMC_TDM_ING_ACTION_NO_CHANGE
#define ARAD_TDM_ING_ACTION_CUSTOMER_EMBED                   SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED
#define ARAD_TDM_NOF_ING_ACTIONS                             SOC_TMC_TDM_NOF_ING_ACTIONS
typedef SOC_TMC_TDM_ING_ACTION                                 ARAD_TDM_ING_ACTION;

#define ARAD_TDM_EG_ACTION_REMOVE                            SOC_TMC_TDM_EG_ACTION_REMOVE
#define ARAD_TDM_EG_ACTION_NO_CHANGE                         SOC_TMC_TDM_EG_ACTION_NO_CHANGE
#define ARAD_TDM_EG_ACTION_CUSTOMER_EXTRACT                  SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT
#define ARAD_TDM_NOF_EG_ACTIONS                              SOC_TMC_TDM_NOF_EG_ACTIONS
typedef SOC_TMC_TDM_EG_ACTION                                  ARAD_TDM_EG_ACTION;

typedef SOC_TMC_TDM_FTMH_OPT_UC                                ARAD_TDM_FTMH_OPT_UC;
typedef SOC_TMC_TDM_FTMH_OPT_MC                                ARAD_TDM_FTMH_OPT_MC;
typedef SOC_TMC_TDM_FTMH_STANDARD_UC                           ARAD_TDM_FTMH_STANDARD_UC;
typedef SOC_TMC_TDM_FTMH_STANDARD_MC                           ARAD_TDM_FTMH_STANDARD_MC;
typedef SOC_TMC_TDM_FTMH                                       ARAD_TDM_FTMH;
typedef SOC_TMC_TDM_FTMH_INFO                                  ARAD_TDM_FTMH_INFO;


typedef SOC_TMC_TDM_DIRECT_ROUTING_INFO                        ARAD_TDM_DIRECT_ROUTING_INFO;

typedef SOC_TMC_TDM_MC_STATIC_ROUTE_INFO                       ARAD_TDM_MC_STATIC_ROUTE_INFO;










uint32
  arad_tdm_ftmh_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  );


uint32
  arad_tdm_ftmh_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO            *info
  );


uint32
  arad_tdm_opt_size_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );


uint32
  arad_tdm_opt_size_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  );


uint32
  arad_tdm_stand_size_range_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );


uint32
  arad_tdm_stand_size_range_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  );


uint32
  arad_tdm_ofp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );


uint32
  arad_tdm_ofp_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  );


uint32
  arad_tdm_direct_routing_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN  uint8 enable_rpt_reachable
  );


uint32
  arad_tdm_direct_routing_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT uint8 *enable_rpt_reachable
  );


uint32
  arad_tdm_direct_routing_profile_map_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint32                   direct_routing_profile
  );


uint32
  arad_tdm_direct_routing_profile_map_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint32                   *direct_routing_profile
  );


uint32
  arad_tdm_port_packet_crc_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t               port_ndx,
    SOC_SAND_IN  uint8                    is_enable,         
    SOC_SAND_IN  uint8                    configure_ingress, 
    SOC_SAND_IN  uint8                    configure_egress   
  );


uint32
  arad_tdm_port_packet_crc_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t                   port_ndx,
    SOC_SAND_OUT uint8                    *is_ingress_enabled,
    SOC_SAND_OUT uint8                    *is_egress_enabled
  );

void
  ARAD_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_OPT_UC *info
  );

void
  ARAD_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_OPT_MC *info
  );

void
  ARAD_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_STANDARD_UC *info
  );

void
  ARAD_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_STANDARD_MC *info
  );

void
  ARAD_TDM_FTMH_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH *info
  );

void
  ARAD_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO *info
  );
void
  ARAD_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  );
void
  ARAD_TDM_DIRECT_ROUTING_INFO_clear(
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  ARAD_TDM_ING_ACTION enum_val
  );

const char*
  ARAD_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  ARAD_TDM_EG_ACTION enum_val
  );

void
  ARAD_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_UC *info
  );

void
  ARAD_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_MC *info
  );

void
  ARAD_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_UC *info
  );

void
  ARAD_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_MC *info
  );

void
  ARAD_TDM_FTMH_print(
    SOC_SAND_IN  ARAD_TDM_FTMH *info
  );

void
  ARAD_TDM_FTMH_INFO_print(
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO *info
  );
void
  ARAD_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  );

void
  ARAD_TDM_DIRECT_ROUTING_INFO_print(
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

