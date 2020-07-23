/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_MULTICAST_FABRIC_INCLUDED__

#define __ARAD_MULTICAST_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/ARAD/arad_api_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>


























uint32
  arad_multicast_fabric_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  );


uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS         tr_cls_ndx,
    SOC_SAND_IN  ARAD_MULT_FABRIC_CLS     new_mult_cls
  );


uint32
  arad_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_TR_CLS         tr_cls_ndx,
    SOC_SAND_OUT ARAD_MULT_FABRIC_CLS     *new_mult_cls
  );


uint32
  arad_mult_fabric_base_queue_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );


uint32
  arad_mult_fabric_base_queue_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  );


uint32
  arad_mult_fabric_base_queue_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  );


int
  arad_mult_fabric_credit_source_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core,
    SOC_SAND_IN  ARAD_MULT_FABRIC_INFO      *info
  );


int
  arad_mult_fabric_credit_source_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_OUT ARAD_MULT_FABRIC_INFO    *info
  );


uint32
  arad_mult_fabric_enhanced_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );


uint32
  arad_mult_fabric_enhanced_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  );


uint32
  arad_mult_fabric_enhanced_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  );


uint32
  arad_mult_fabric_active_links_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );


uint32
  arad_mult_fabric_active_links_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  );


uint32
  arad_mult_fabric_active_links_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  );



uint32
  arad_mult_fabric_flow_control_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  );

uint32
  arad_mult_fabric_flow_control_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  );

uint32
  arad_mult_fabric_flow_control_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP      *fc_map
  );

uint32
  arad_mult_fabric_flow_control_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_MULT_FABRIC_FLOW_CONTROL_MAP     *fc_map
  );



soc_error_t arad_multicast_table_size_get(int unit, uint32* mc_table_size);

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
