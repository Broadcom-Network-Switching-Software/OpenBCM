/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_DCMN_FABRIC_SOURCE_ROUTED_CELL_H
#define SOC_DCMN_FABRIC_SOURCE_ROUTED_CELL_H

#include <soc/dcmn/dcmn_defs.h>
#include <soc/error.h>
#include <soc/dcmn/fabric.h>

#define DCMN_CELL_NOF_LINKS_IN_PATH_LINKS            (4)

typedef struct dcmn_sr_cell_link_list_s{
  dcmn_fabric_device_type_t  src_entity_type;
  dcmn_fabric_device_type_t  dest_entity_type;
  
  soc_port_t                path_links[DCMN_CELL_NOF_LINKS_IN_PATH_LINKS];
  int                       pipe_id;
}dcmn_sr_cell_link_list_t;


soc_error_t
soc_dcmn_actual_entity_value(
                            int unit,
                            dcmn_fabric_device_type_t            device_entity,
                            soc_dcmn_device_type_actual_value_t* actual_entity
                            );

soc_error_t
soc_dcmn_device_entity_value(
                            int unit,
                            soc_dcmn_device_type_actual_value_t actual_entity,
                            dcmn_fabric_device_type_t*          device_entity
                            );

#endif 
