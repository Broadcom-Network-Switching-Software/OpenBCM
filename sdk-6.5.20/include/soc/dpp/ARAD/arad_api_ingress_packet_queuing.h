/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_INGRESS_PACKET_QUEUING_INCLUDED__

#define __ARAD_API_INGRESS_PACKET_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/cosq.h>




#define ARAD_IPQ_TR_CLS SOC_TMC_IPQ_TR_CLS

#define ARAD_IPQ_MAX_NOF_GLAG_PORTS 16





#define ARAD_IPQ_Q_TO_QRTT_ID(que_id)                 SOC_TMC_IPQ_Q_TO_QRTT_ID(que_id)
#define ARAD_IPQ_QRTT_TO_Q_ID(q_que_id)               SOC_TMC_IPQ_QRTT_TO_Q_ID(q_que_id)
#define ARAD_IPQ_Q_TO_1K_ID(que_id)                   SOC_TMC_IPQ_Q_TO_1K_ID(que_id)
#define ARAD_IPQ_1K_TO_Q_ID(k_que_id)                 SOC_TMC_IPQ_1K_TO_Q_ID(k_que_id)






typedef SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO                 ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO;
typedef SOC_TMC_IPQ_BASEQ_MAP_INFO                             ARAD_IPQ_BASEQ_MAP_INFO;
typedef SOC_TMC_IPQ_QUARTET_MAP_INFO                           ARAD_IPQ_QUARTET_MAP_INFO;

#define ARAD_IPQ_TR_CLS_MIN        SOC_TMC_IPQ_TR_CLS_MIN
#define ARAD_IPQ_TR_CLS_MAX        SOC_TMC_IPQ_TR_CLS_MAX
typedef SOC_TMC_IPQ_TR_CLS_RNG          ARAD_IPQ_TR_CLS_RNG;

#define ARAD_IPQ_TR_CLS_RNG_LAST   SOC_TMC_IPQ_TR_CLS_RNG_LAST


#define ARAD_NOF_INGRESS_UC_TC_MAPPING_PROFILES    SOC_TMC_NOF_INGRESS_UC_TC_MAPPING_PROFILES
#define ARAD_NOF_INGRESS_FLOW_TC_MAPPING_PROFILES  SOC_TMC_NOF_INGRESS_FLOW_TC_MAPPING_PROFILES


#define ARAD_IPQ_STACK_LAG_DOMAIN_MIN     SOC_TMC_IPQ_STACK_LAG_DOMAIN_MIN
#define ARAD_IPQ_STACK_LAG_DOMAIN_MAX     SOC_TMC_IPQ_STACK_LAG_DOMAIN_MAX

#define ARAD_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MIN     SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MIN
#define ARAD_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX     SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_MAX
#define ARAD_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL     SOC_TMC_IPQ_STACK_LAG_STACK_TRUNK_RESOLVE_ENTRY_ALL

#define ARAD_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MIN     SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MIN
#define ARAD_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MAX     SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MAX
#define ARAD_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_ALL     SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_ALL


#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MIN     0

#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MIN     0


#define ARAD_IPQ_DESTINATION_ID_STACKING_BASE_QUEUE_MIN           (SOC_IS_QUX(unit) ? ((0xff) << 6) :\
                                                                     (SOC_IS_QAX(unit) ? ((0x1ff) << 6) : ((0x7ff) << 6)))
#define ARAD_IPQ_DESTINATION_ID_STACKING_BASE_QUEUE_MAX(unit)     (SOC_IS_QUX(unit) ? (0x3fff) :\
                                                                     (SOC_IS_QAX(unit) ? (0x7fff) : (0x1ffff)))
#define ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit) ARAD_IPQ_DESTINATION_ID_STACKING_BASE_QUEUE_MAX(unit)














int
  arad_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );


int
  arad_ipq_explicit_mapping_mode_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );


uint32
  arad_ipq_traffic_class_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  );


uint32
  arad_ipq_traffic_class_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT ARAD_IPQ_TR_CLS          *new_class
  );

int
  arad_ipq_traffic_class_multicast_priority_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_IN  uint8                  enable
  );

int
  arad_ipq_traffic_class_multicast_priority_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_OUT uint8                  *enable
  );


uint32
  arad_ipq_destination_id_packets_base_queue_id_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              dest_ndx,
     SOC_SAND_IN  uint8               valid,
     SOC_SAND_IN  uint8               sw_only,
     SOC_SAND_IN  uint32              base_queue
  );


uint32
  arad_ipq_destination_id_packets_base_queue_id_get(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  int                 core,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_OUT uint8               *valid,
   SOC_SAND_OUT uint8               *sw_only,
   SOC_SAND_OUT uint32              *base_queue
  );


uint32
  arad_ipq_stack_lag_packets_base_queue_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  base_queue
  );

uint32
  arad_ipq_stack_lag_packets_base_queue_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_OUT uint32              *base_queue
  );

uint32
  arad_ipq_stack_fec_map_stack_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  stack_lag
  );

uint32
  arad_ipq_stack_fec_map_stack_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_OUT  uint32*                stack_lag
  );


uint32
  arad_ipq_queue_interdigitated_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  );


uint32
  arad_ipq_queue_interdigitated_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  );


uint32
  arad_ipq_queue_to_flow_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  ARAD_IPQ_QUARTET_MAP_INFO *info
  );


uint32
  arad_ipq_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT ARAD_IPQ_QUARTET_MAP_INFO *info
  );


uint32
  arad_ipq_queue_qrtt_unmap(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  );


uint32
  arad_ipq_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  );

uint32
  arad_ipq_k_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_k_quartet_ndx,
    SOC_SAND_IN uint32                  region_size
  );

void
  arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  arad_ARAD_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT ARAD_IPQ_QUARTET_MAP_INFO *info
  );

uint32
  arad_ipq_attached_flow_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32 queue_ndx,
    SOC_SAND_OUT uint32 *flow_id,
    SOC_SAND_OUT uint32 *sys_port
  );

uint32
  arad_ipq_tc_profile_set(
    SOC_SAND_IN	  int	  unit,
    SOC_SAND_IN   int     core,
    SOC_SAND_IN	  uint8	  is_flow_ndx,
    SOC_SAND_IN	  uint32	  dest_ndx,
    SOC_SAND_IN   uint32	  tc_profile
  );

uint32
  arad_ipq_tc_profile_get(
    SOC_SAND_IN	  int	  unit,
    SOC_SAND_IN   int     core,
    SOC_SAND_IN	  uint8	  is_flow_ndx,
    SOC_SAND_IN	  uint32	  dest_ndx,
    SOC_SAND_OUT  uint32	  *tc_profile
  );

uint32
  arad_ipq_tc_profile_map_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32                profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  );
uint32
  arad_ipq_tc_profile_map_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32                profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT  ARAD_IPQ_TR_CLS         *new_class
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN ARAD_IPQ_TR_CLS_RNG enum_val
  );


void
  arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );



void
  arad_ARAD_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN ARAD_IPQ_QUARTET_MAP_INFO *info
  );

uint32
  arad_ips_non_empty_queues_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                first_queue,
    SOC_SAND_IN  uint32                max_array_size,
    SOC_SAND_OUT soc_ips_queue_info_t* queues,
    SOC_SAND_OUT uint32*               nof_queues_filled,
    SOC_SAND_OUT uint32*               next_queue,
    SOC_SAND_OUT uint32*               reached_end
  );



#endif 





#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


