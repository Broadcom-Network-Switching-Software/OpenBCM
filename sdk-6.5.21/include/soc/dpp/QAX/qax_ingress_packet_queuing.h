/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_mgmt.h
 */

#ifndef __QAX_INGRESS_PACKET_QUEUING_INCLUDED__

#define __QAX_INGRESS_PACKET_QUEUING_INCLUDED__

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_api_ingress_packet_queuing.h>

int
  qax_ipq_init(
    SOC_SAND_IN  int unit
  );

int
  qax_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );

int
qax_ipq_explicit_mapping_mode_info_get(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   );

int
qax_ipq_explicit_mapping_mode_info_set(
   SOC_SAND_IN  int                            unit,
   SOC_SAND_IN ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   );

int
qax_ipq_default_invalid_queue_set(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_IN  uint32         queue_id,
   SOC_SAND_IN  int            enable);

int
qax_ipq_default_invalid_queue_get(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_OUT uint32         *queue_id,
   SOC_SAND_OUT int            *enable);

int
  qax_ipq_traffic_class_multicast_priority_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              traffic_class,
    SOC_SAND_IN  uint8               enable
  );
  
int
  qax_ipq_traffic_class_multicast_priority_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              traffic_class,
    SOC_SAND_OUT uint8               *enable
  );

#endif 

