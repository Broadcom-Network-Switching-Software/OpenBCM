
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_general.h>



























int
  arad_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
    uint32 res;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_ipq_explicit_mapping_mode_info_verify(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_ipq_explicit_mapping_mode_info_set_unsafe(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}



int
  arad_ipq_explicit_mapping_mode_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
    uint32  res;
    SOCDNX_INIT_FUNC_DEFS;

    res = arad_ipq_explicit_mapping_mode_info_get_unsafe(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_ipq_traffic_class_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TRAFFIC_CLASS_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ipq_traffic_class_map_verify(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_traffic_class_map_set_unsafe(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_traffic_class_map_set()",0,0);
}


uint32
  arad_ipq_traffic_class_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT ARAD_IPQ_TR_CLS          *new_class
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TRAFFIC_CLASS_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(new_class);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_traffic_class_map_get_unsafe(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_traffic_class_map_get()",0,0);
}


int
  arad_ipq_traffic_class_multicast_priority_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_IN  uint8                  enable
  )
{
  uint32 res;
  SOCDNX_INIT_FUNC_DEFS;

  res = arad_ipq_traffic_class_multicast_priority_map_set_verify(
      unit,
      traffic_class,
      enable
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_ipq_traffic_class_multicast_priority_map_set_unsafe(
     unit,
     traffic_class,
     enable
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

 
int
  arad_ipq_traffic_class_multicast_priority_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 traffic_class,
    SOC_SAND_OUT uint8                  *enable
  )
{
  uint32 res;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(enable);

  res = arad_ipq_traffic_class_multicast_priority_map_get_verify(
      unit,
      traffic_class,
      enable
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_ipq_traffic_class_multicast_priority_map_get_unsafe(
     unit,
     traffic_class,
     enable
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_ipq_destination_id_packets_base_queue_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              dest_ndx,
    SOC_SAND_IN  uint8               valid,
    SOC_SAND_IN  uint8               sw_only,
    SOC_SAND_IN  uint32              base_queue
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ipq_destination_id_packets_base_queue_id_verify(
        unit,
        core,
        dest_ndx,
        valid,
        sw_only,
        base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_destination_id_packets_base_queue_id_set_unsafe(
        unit,
        core,
        dest_ndx,
        valid,
        sw_only,
        base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_destination_id_packets_base_queue_id_set()",0,0);
}


uint32
  arad_ipq_destination_id_packets_base_queue_id_get(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  int                 core,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_OUT uint8               *valid,
   SOC_SAND_OUT uint8               *sw_only,
   SOC_SAND_OUT uint32              *base_queue
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(valid);
  SOC_SAND_CHECK_NULL_INPUT(base_queue);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_destination_id_packets_base_queue_id_get_unsafe(
       unit,
       core,
       dest_ndx,
       valid,
       sw_only,
       base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_destination_id_packets_base_queue_id_get()",0,0);
}


uint32
  arad_ipq_stack_lag_packets_base_queue_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  base_queue
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ipq_stack_lag_packets_base_queue_id_verify(unit, tmd, entry, base_queue);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_stack_lag_packets_base_queue_id_set_unsafe(unit, tmd, entry,base_queue);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_stack_lag_packets_base_queue_id_set()",0,0);
}


uint32
  arad_ipq_stack_lag_packets_base_queue_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tmd,
    SOC_SAND_IN  uint32                 entry,
    SOC_SAND_OUT uint32              *base_queue
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

 SOC_SAND_CHECK_NULL_INPUT(base_queue);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_stack_lag_packets_base_queue_id_get_unsafe(unit, tmd, entry, base_queue);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_stack_lag_packets_base_queue_id_get()",0,0);
}


uint32
  arad_ipq_stack_fec_map_stack_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_IN  uint32                  stack_lag
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ipq_stack_fec_map_stack_lag_verify(unit, tmd, entry, stack_lag);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_stack_fec_map_stack_lag_set_unsafe(unit, tmd, entry,stack_lag);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_stack_fec_map_stack_lag_set()",0,0);
}


uint32
  arad_ipq_stack_fec_map_stack_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  tmd,
    SOC_SAND_IN  uint32                  entry,
    SOC_SAND_OUT  uint32*                stack_lag
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(stack_lag);

  res = arad_ipq_stack_fec_map_stack_lag_verify(unit, tmd, entry, 0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_stack_fec_map_stack_lag_get_unsafe(unit, tmd, entry, stack_lag);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_stack_fec_map_stack_lag_get()",0,0);
}


uint32
  arad_ipq_queue_interdigitated_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_ipq_queue_interdigitated_mode_verify(
    unit,
    core,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_queue_interdigitated_mode_set_unsafe(
    unit,
    core,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_queue_interdigitated_mode_set()",0,0);
}


uint32
  arad_ipq_queue_interdigitated_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_interdigitated);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_queue_interdigitated_mode_get_unsafe(
    unit,
    core,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_queue_interdigitated_mode_get()",0,0);
}

uint32
  arad_ipq_queue_to_flow_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  ARAD_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_queue_to_flow_mapping_verify(
    unit,
    core,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_ipq_queue_to_flow_mapping_set_unsafe(
    unit,
    core,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_queue_to_flow_mapping_set()",0,0);
}


uint32
  arad_ipq_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT ARAD_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_queue_to_flow_mapping_get_unsafe(
    unit,
    core,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_queue_to_flow_mapping_get()",0,0);
}


uint32
  arad_ipq_queue_qrtt_unmap(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUEUE_QRTT_UNMAP);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_queue_qrtt_unmap_unsafe(
          unit,
          core,
          queue_quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_queue_qrtt_unmap()",queue_quartet_ndx,0);
}


uint32
  arad_ipq_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_QUARTET_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_quartet_reset_unsafe(
    unit,
    core,
    queue_quartet_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_quartet_reset()",0,0);
}

uint32
  arad_ipq_k_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_k_quartet_ndx,
    SOC_SAND_IN uint32                  region_size
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_K_QUARTET_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_k_quartet_reset_unsafe(
    unit,
    core,
    queue_k_quartet_ndx,
    region_size
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_k_quartet_reset()",0,0);
}

void
  arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_IPQ_BASEQ_MAP_INFO_clear(
    SOC_SAND_OUT ARAD_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_BASEQ_MAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT ARAD_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  arad_ipq_attached_flow_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32 queue_ndx,
    SOC_SAND_OUT uint32 *flow_id,
    SOC_SAND_OUT uint32 *sys_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_ATTACHED_FLOW_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(flow_id);
  SOC_SAND_CHECK_NULL_INPUT(sys_port);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_attached_flow_port_get_unsafe(
          unit,
          core,
          queue_ndx,
          flow_id,
          sys_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_attached_flow_port_get()",0,0);
}


#if ARAD_DEBUG_IS_LVL1


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
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ips_non_empty_queues_info_get_unsafe(
          unit,
          core,
          first_queue,
          max_array_size,
          queues,
          nof_queues_filled,
          next_queue,
          reached_end
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ips_non_empty_queues_print()",0,0);

}

#endif 




uint32
  arad_ipq_tc_profile_set(
    SOC_SAND_IN	  int	  unit,
    SOC_SAND_IN   int     core,
    SOC_SAND_IN	  uint8	  is_flow_ndx,
    SOC_SAND_IN	  uint32	  dest_ndx,
    SOC_SAND_IN   uint32	  tc_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TC_PROFILE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_tc_profile_verify(
            unit,
            is_flow_ndx,
            dest_ndx,
            tc_profile
          );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);
  
  res = arad_ipq_tc_profile_set_unsafe(
          unit,
          core,
          is_flow_ndx,
          dest_ndx,
          tc_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_tc_profile_set()",0,0);                                 
}


uint32
  arad_ipq_tc_profile_get(
    SOC_SAND_IN	  int         unit,
    SOC_SAND_IN   int         core,
    SOC_SAND_IN	  uint8       is_flow_ndx,
    SOC_SAND_IN	  uint32      dest_ndx,
    SOC_SAND_OUT  uint32      *tc_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TC_PROFILE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_tc_profile_verify(
          unit,
          is_flow_ndx,
          dest_ndx,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);

  res = arad_ipq_tc_profile_get_unsafe(
          unit,
          core,
          is_flow_ndx,
          dest_ndx,
          tc_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_tc_profile_get()",0,0);                                 
}


uint32
  arad_ipq_tc_profile_map_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS_PROFILE  profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          new_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_traffic_class_profile_map_verify(
          unit,
          core_id,
          profile_ndx,
          is_flow_profile,
          tr_cls_ndx,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

  res = arad_ipq_traffic_class_profile_map_set_unsafe(
          unit,
          core_id,
          profile_ndx,
          is_flow_profile,
          FALSE,
          tr_cls_ndx,
          new_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_traffic_class_profile_map_set()",0,0);                                 
}


uint32
  arad_ipq_tc_profile_map_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS_PROFILE  profile_ndx,
    SOC_SAND_IN  uint8                is_flow_profile,
    SOC_SAND_IN  ARAD_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT  ARAD_IPQ_TR_CLS         *new_class
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ipq_traffic_class_profile_map_verify(
          unit,
          core_id,
          profile_ndx,
          is_flow_profile,
          tr_cls_ndx,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

  res = arad_ipq_traffic_class_profile_map_get_unsafe(
          unit,
          core_id,
          profile_ndx,
          is_flow_profile,
          FALSE,
          tr_cls_ndx,
          new_class
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ipq_traffic_class_profile_map_set()",0,0);                                 
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

