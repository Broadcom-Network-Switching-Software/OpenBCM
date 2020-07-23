/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_MULTICAST_FABRIC_INCLUDED__

#define __ARAD_API_MULTICAST_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>




#define ARAD_MULT_FABRIC_NOF_BE_CLASSES      3

#define ARAD_MULT_FABRIC_FLOW_CONTROL_DONT_MAP SOC_TMC_MULT_FABRIC_FLOW_CONTROL_DONT_MAP











#define ARAD_MULT_FABRIC_CLS_MIN                          SOC_TMC_MULT_FABRIC_CLS_MIN
#define ARAD_MULT_FABRIC_CLS_MAX                          SOC_TMC_MULT_FABRIC_CLS_MAX
typedef SOC_TMC_MULT_FABRIC_CLS_RNG                            ARAD_MULT_FABRIC_CLS_RNG;


typedef SOC_TMC_MULT_FABRIC_CLS ARAD_MULT_FABRIC_CLS;

typedef SOC_TMC_MULT_FABRIC_PORT_INFO                          ARAD_MULT_FABRIC_PORT_INFO;
typedef SOC_TMC_MULT_FABRIC_SHAPER_INFO                        ARAD_MULT_FABRIC_SHAPER_INFO;
typedef SOC_TMC_MULT_FABRIC_BE_CLASS_INFO                      ARAD_MULT_FABRIC_BE_CLASS_INFO;
typedef SOC_TMC_MULT_FABRIC_BE_INFO                            ARAD_MULT_FABRIC_BE_INFO;
typedef SOC_TMC_MULT_FABRIC_GR_INFO                            ARAD_MULT_FABRIC_GR_INFO;
typedef SOC_TMC_MULT_FABRIC_INFO                               ARAD_MULT_FABRIC_INFO;
typedef SOC_TMC_MULT_FABRIC_ACTIVE_LINKS                       ARAD_MULT_FABRIC_ACTIVE_LINKS;
typedef SOC_TMC_MULT_FABRIC_FLOW_CONTROL_MAP                   ARAD_MULT_FABRIC_FLOW_CONTROL_MAP;













uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  );


uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS         tr_cls_ndx,
    SOC_SAND_OUT ARAD_MULT_FABRIC_CLS     *new_mult_cls
  );


uint32
  arad_mult_fabric_base_queue_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );


uint32
  arad_mult_fabric_base_queue_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  );


uint32
  arad_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );


uint32
  arad_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  );


uint32
  arad_mult_fabric_active_links_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );


uint32
  arad_mult_fabric_active_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  );


uint32
  arad_mult_fabric_flow_control_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  );

uint32
  arad_mult_fabric_flow_control_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP     *fc_map
  );

void
  arad_ARAD_MULT_FABRIC_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_PORT_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_SHAPER_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_SHAPER_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_BE_CLASS_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_BE_CLASS_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_BE_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_BE_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_GR_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_GR_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_INFO_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_INFO *info
  );

void
  arad_ARAD_MULT_FABRIC_ACTIVE_LINKS_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_ACTIVE_LINKS *info
  );

void
  arad_ARAD_MULT_FABRIC_FLOW_CONTROL_MAP_clear(
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP *info
  );

#if ARAD_DEBUG_IS_LVL1


const char*
  arad_ARAD_MULT_FABRIC_CLS_RNG_to_string(
    SOC_SAND_IN ARAD_MULT_FABRIC_CLS_RNG enum_val
  );



void
  arad_ARAD_MULT_FABRIC_PORT_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_PORT_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_SHAPER_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_SHAPER_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_BE_CLASS_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_BE_CLASS_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_BE_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_BE_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_GR_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_GR_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_INFO_print(
    SOC_SAND_IN ARAD_MULT_FABRIC_INFO *info
  );



void
  arad_ARAD_MULT_FABRIC_ACTIVE_LINKS_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN ARAD_MULT_FABRIC_ACTIVE_LINKS *info
  );


#endif 





#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
