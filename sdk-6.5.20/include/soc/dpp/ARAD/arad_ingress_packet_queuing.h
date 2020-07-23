/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_INGRESS_PACKET_QUEUING_INCLUDED__

#define __ARAD_INGRESS_PACKET_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>



#define ARAD_IPQ_INVALID_FLOW_QUARTET          ((SOC_DPP_DEFS_MAX(NOF_FLOWS_PER_PIPE) / 8 ) - 1)

#define ARAD_IPQ_STACK_LAG_ENTRY_PER_TMD_BIT_NUM        (SOC_IS_JERICHO(unit) ? 8 : 6)
#define ARAD_IPQ_STACK_FEC_ENTRY_PER_TMD_BIT_NUM        (4)

#define ARAD_IPQ_UC_FIFO_SNOOP_THRESHOLD(limit)    (((limit)*2)/10)
#define ARAD_IPQ_UC_FIFO_MIRROR_THRESHOLD(limit)    (((limit)*6)/10)


typedef uint32 ARAD_IPQ_TR_CLS_PROFILE;





















uint32
  arad_ipq_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_ingress_packet_queuing_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_ipq_explicit_mapping_mode_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );


uint32
  arad_ipq_explicit_mapping_mode_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );


uint32
  arad_ipq_explicit_mapping_mode_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );


uint32
  arad_ipq_base_q_is_valid_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *is_valid
  );


uint32
  arad_ipq_traffic_class_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  );


uint32
  arad_ipq_traffic_class_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  );


uint32
  arad_ipq_traffic_class_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT ARAD_IPQ_TR_CLS          *new_class
  );


uint32
  arad_ipq_destination_id_packets_base_queue_id_verify(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  int                 core,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_IN  uint8               valid,
   SOC_SAND_IN  uint8               sw_only,
   SOC_SAND_IN  uint32              base_queue
  );



uint32
  arad_ipq_traffic_class_multicast_priority_map_set_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_IN  uint8                  enable
  );

uint32
  arad_ipq_traffic_class_multicast_priority_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_IN  uint8                  enable
  );


uint32
  arad_ipq_traffic_class_multicast_priority_map_get_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_OUT uint8                  *enable
  );

uint32
  arad_ipq_traffic_class_multicast_priority_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_OUT uint8                  *enable
  );

uint32
  arad_ipq_destination_id_packets_base_queue_id_set_unsafe(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              dest_ndx,
     SOC_SAND_IN  uint8               valid,
     SOC_SAND_IN  uint8               sw_only,
     SOC_SAND_IN  uint32              base_queue
  );


uint32
  arad_ipq_destination_id_packets_base_queue_id_get_unsafe(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              dest_ndx,
     SOC_SAND_OUT uint8               *valid,
     SOC_SAND_OUT uint8               *sw_only,
     SOC_SAND_OUT uint32              *base_queue
  );

uint32
  arad_ipq_stack_lag_packets_base_queue_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_IN  uint32                 base_queue
  );

uint32
  arad_ipq_stack_lag_packets_base_queue_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_IN  uint32                 base_queue
  );

uint32
  arad_ipq_stack_lag_packets_base_queue_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_OUT uint32              *base_queue
  );

uint32
  arad_ipq_stack_fec_map_stack_lag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_IN  uint32                 stack_lag
  );

uint32
  arad_ipq_stack_fec_map_stack_lag_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_IN  uint32                 stack_lag
  );

uint32
  arad_ipq_stack_fec_map_stack_lag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_OUT uint32*                stack_lag
  );


uint32
  arad_ipq_queue_interdigitated_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  );


uint32
  arad_ipq_queue_interdigitated_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  );


uint32
  arad_ipq_queue_interdigitated_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  );


uint32
  arad_ipq_queue_to_flow_mapping_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              queue_quartet_ndx,
    SOC_SAND_IN  ARAD_IPQ_QUARTET_MAP_INFO *info
  );


uint32
  arad_ipq_queue_to_flow_mapping_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32              queue_quartet_ndx,
    SOC_SAND_IN  ARAD_IPQ_QUARTET_MAP_INFO *info
  );


uint32
  arad_ipq_queue_to_flow_mapping_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core, 
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT ARAD_IPQ_QUARTET_MAP_INFO *info
  );


uint32
  arad_ipq_queue_qrtt_unmap_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  );


uint32
  arad_ipq_quartet_reset_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  );


uint32
  arad_ipq_quartet_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  );

uint32
  arad_ipq_k_quartet_reset_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core,
    SOC_SAND_IN  uint32                  queue_k_quartet_ndx,
    SOC_SAND_IN  uint32                  region_size
  );

uint32
  arad_ipq_k_quartet_reset_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 queue_k_quartet_ndx,
    SOC_SAND_IN uint32 region_size
  ); 

uint32
  arad_ipq_attached_flow_port_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  int  core,
    SOC_SAND_IN  uint32  queue_ndx,
    SOC_SAND_OUT uint32  *flow_id,
    SOC_SAND_OUT uint32  *sys_port
  );

uint32
  arad_ipq_queue_id_verify(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    queue_id
  );

uint32
  arad_ipq_tc_profile_set_unsafe(
    SOC_SAND_IN	  int	  unit,
    SOC_SAND_IN   int     core,
    SOC_SAND_IN	  uint8	  is_flow_ndx,
    SOC_SAND_IN	  uint32	  dest_ndx,
    SOC_SAND_IN   uint32	  tc_profile
  );

uint32
  arad_ipq_tc_profile_get_unsafe(
   SOC_SAND_IN	  int	  unit,
   SOC_SAND_IN    int     core,
   SOC_SAND_IN	  uint8	  is_flow_ndx,
   SOC_SAND_IN	  uint32	  dest_ndx,
   SOC_SAND_OUT   uint32	  *tc_profile
 );

uint32
  arad_ipq_tc_profile_verify(
    SOC_SAND_IN	  int	  unit,
    SOC_SAND_IN	  uint8	  is_flow_ndx,
    SOC_SAND_IN	  uint32	  dest_ndx,
    SOC_SAND_IN   uint32	  tc_profile
  );

uint32
  arad_ipq_traffic_class_profile_map_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS_PROFILE  profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  uint8                is_multicast_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  );

uint32
  arad_ipq_traffic_class_profile_map_verify(
  SOC_SAND_IN  int                unit,
  SOC_SAND_IN  int                core_id,
  SOC_SAND_IN  ARAD_IPQ_TR_CLS_PROFILE  profile_ndx,
  SOC_SAND_IN  uint8                is_flow_profile,
  SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
  SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
 );

uint32
  arad_ipq_traffic_class_profile_map_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS_PROFILE  profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  uint8                is_multicast_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT ARAD_IPQ_TR_CLS          *new_class
  );

#if ARAD_DEBUG

uint32
arad_ips_non_empty_queues_info_get_unsafe(
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

