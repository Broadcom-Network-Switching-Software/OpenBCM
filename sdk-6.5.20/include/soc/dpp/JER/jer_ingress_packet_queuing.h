#include <soc/mcm/memregs.h>
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_INGRESS_SCHEDULER_QUEUEING_H__
#define __JER_INGRESS_SCHEDULER_QUEUEING_H__

#include <soc/error.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>



uint32
jer_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
    );

uint32
jer_ipq_explicit_mapping_mode_info_get(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
   );


int
jer_ipq_init(
   SOC_SAND_IN  int                 unit
   );


int
jer_ipq_default_invalid_queue_set(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_IN  uint32         queue_id,
   SOC_SAND_IN  int            enable);


int
jer_ipq_default_invalid_queue_get(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            core,
   SOC_SAND_OUT uint32         *queue_id,
   SOC_SAND_OUT int            *enable);

int
jer_ipq_sysport_ingress_queue_map_enable_set(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            sysport,
   SOC_SAND_IN  int            enable);

int
jer_ipq_sysport_ingress_queue_map_enable_get(
   SOC_SAND_IN  int            unit,
   SOC_SAND_IN  int            sysport,
   SOC_SAND_OUT int            *enable);

#endif 

