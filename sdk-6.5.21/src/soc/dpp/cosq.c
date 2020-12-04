/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/mbcm.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

int
_bcm_petra_cosq_resource_to_ingress_threshold(
    int unit,
    bcm_cosq_resource_t resource,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E *ingress_threshold);


soc_error_t
  soc_dpp_cosq_flow_and_up_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  int                             core,
    SOC_SAND_IN  uint8                              is_flow,
    SOC_SAND_IN  uint32                             dest_id, 
    SOC_SAND_IN  uint32                             reterive_status,
    SOC_SAND_INOUT  SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  )
{
  uint8    
    valid=0;
  uint32
    ret = SOC_E_NONE,
    flow_id =0,
    queue_quartet_ndx=0;
  uint32
    mapped_fap_id = 0,
    mapped_fap_port_id = 0;
  int 
      core_idx = core;
  uint8 
      is_fap_id_local,
      is_sw_only = FALSE;

  int i;
#if defined(BROADCOM_DEBUG)
  
#endif

  SOCDNX_INIT_FUNC_DEFS;  

  if (flow_and_up_info->credit_sources_nof == 0)
  {
      
      if (is_flow) {
         flow_id = flow_and_up_info->base_queue = dest_id;
      } else {
         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, dest_id, &mapped_fap_id, &mapped_fap_port_id));

         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_sys_phys_to_local_port_map_get.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }

         SOCDNX_IF_ERR_EXIT(soc_dpp_is_fap_id_local_and_get_core_id(unit, mapped_fap_id, &is_fap_id_local, &core_idx));

         if (!is_fap_id_local)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "Destination is on remote FAP. Cannot print flow and up.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }

         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_destination_id_packets_base_queue_id_get,(unit, core_idx, dest_id, &valid, &is_sw_only, &(flow_and_up_info->base_queue)));
         
         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_ipq_destination_id_packets_base_queue_id_get.\n\r")));
            SOCDNX_IF_ERR_EXIT(ret);
         }

         if (!valid)
         {
           SOCDNX_IF_ERR_EXIT(SOC_E_EXISTS);
         }

         queue_quartet_ndx = (flow_and_up_info->base_queue) / 4;

         ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_queue_to_flow_mapping_get,(unit, core, queue_quartet_ndx, &(flow_and_up_info->qrtt_map_info)));

         if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
         {        
            LOG_INFO(BSL_LS_SOC_COSQ,
                     (BSL_META_U(unit,
                                 "soc_petra_ipq_queue_to_flow_mapping_get.\n\r")));
            SOCDNX_SAND_IF_ERR_RETURN(ret);
         }
         flow_id = (flow_and_up_info->qrtt_map_info).flow_quartet_index * 4;
      }
  }

  if(SOC_IS_QAX(unit)){
    flow_id = BCM_COSQ_FLOW_ID_QAX_ADD_OFFSET(unit, flow_id);
  }
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_flow_and_up_info_get,(unit, core_idx, flow_id, reterive_status, flow_and_up_info));
  SOCDNX_SAND_IF_ERR_RETURN(ret);

  if(SOC_IS_QAX(unit)){
      int subflow_indx;
      for (subflow_indx = 0; subflow_indx < 2; subflow_indx++)
      {
          if(flow_and_up_info->sch_consumer.sub_flow[subflow_indx].is_valid){
              flow_and_up_info->sch_consumer.sub_flow[subflow_indx].id = BCM_COSQ_FLOW_ID_QAX_SUB_OFFSET(unit, flow_and_up_info->sch_consumer.sub_flow[subflow_indx].id);
          }
      }
      for (i = 0; i < flow_and_up_info->credit_sources_nof; i++)
      {
          for (subflow_indx = 0; subflow_indx < 2; subflow_indx++)
          {
              if(flow_and_up_info->sch_union_info[i].se_sch_info.sch_consumer.sub_flow[subflow_indx].is_valid){
                  flow_and_up_info->sch_union_info[i].se_sch_info.sch_consumer.sub_flow[subflow_indx].id  -= QAX_FLOW_ID_OFFSET(unit);
              }
          }
      }
      
  }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_hr2ps_info_get(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   int                                core,
    SOC_SAND_IN   uint32                             se_id,
    SOC_SAND_OUT  soc_hr2ps_info_t                   *hr2ps_info

  )
{

  uint32                          ret = SOC_E_NONE;
  uint32                          port_id = 0, tc = 0;

  SOC_TMC_SCH_PORT_INFO sch_port_info;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(hr2ps_info);

  SOC_TMC_SCH_PORT_INFO_clear(&sch_port_info);

  
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_se2port_tc_id, (unit, core, se_id, &port_id, &tc));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_se2port_tc_id\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  
  ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_aggregate_get, (unit, core, se_id, &(hr2ps_info->se_info), &(hr2ps_info->flow_info))); 
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_se_get_type_by_id\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_port_priority_rate_get,(unit, core, port_id , tc, &(hr2ps_info->kbits_sec_max)));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_ofp_rates_sch_port_priority_rate_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get,(unit,core, port_id , tc, &(hr2ps_info->max_burst)));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_ofp_rates_sch_port_priority_max_burst_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }

  
  ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_port_sched_get,(unit, core, port_id, &sch_port_info));
  if (SOC_SAND_FAILURE(ret)) {
    LOG_ERROR(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "error in mbcm_dpp_sch_port_sched_get\n")));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
  }
  hr2ps_info->mode = sch_port_info.tcg_ndx[tc];
  hr2ps_info->weight = 0;

  SOCDNX_SAND_IF_ERR_RETURN(ret);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_non_empty_queues_info_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                queue_to_read_from,
    SOC_SAND_IN  uint32                max_array_size,
    SOC_SAND_OUT soc_ips_queue_info_t* queues,
    SOC_SAND_OUT uint32*               nof_queues_filled,
    SOC_SAND_OUT uint32*               next_queue,
    SOC_SAND_OUT uint32*               reached_end
  )
{

    uint32 ret;
#if defined(BROADCOM_DEBUG)
  
#endif

  SOCDNX_INIT_FUNC_DEFS; 

  

    ret = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ips_non_empty_queues_info_get,(unit, core_id, queue_to_read_from,
                                                                            max_array_size,queues,nof_queues_filled,next_queue,reached_end));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO      *info    
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_queue_info_get,(unit, SOC_CORE_ALL, queue_ndx, info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_category_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT int                  *voq_category
  )
{
    uint32 ret;
    SOC_TMC_ITM_CATEGORY_RNGS info;
    int queue_category = 0;	
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_ITM_CATEGORY_RNGS_clear(&info);

    ret = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_get,(unit, BCM_CORE_ALL, &info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    if (queue_ndx <= info.vsq_ctgry0_end) {
        queue_category = 0;
    } else if (queue_ndx <= info.vsq_ctgry1_end) {
        queue_category = 1;
    } else if (queue_ndx <= info.vsq_ctgry2_end) {
        queue_category = 2;
    } else {
        queue_category = 3;
    }
    *voq_category = queue_category;

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO          *queue_map_info
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipq_queue_to_flow_mapping_get,
            (unit, SOC_CORE_ALL, SOC_TMC_IPQ_Q_TO_QRTT_ID(queue_ndx), queue_map_info));
	SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_ingress_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT  SOC_TMC_ITM_ADMIT_TSTS     *test_tmplt
  )
{	
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif
	
    SOCDNX_INIT_FUNC_DEFS;
	  
    ret = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_queue_test_tmplt_get,
				(unit, rt_cls_ndx, drop_precedence_ndx, test_tmplt));
    SOCDNX_SAND_IF_ERR_RETURN(ret);
	
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_cosq_vsq_index_global_to_group_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsq_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_GROUP      *soc_vsq_group_type,
    SOC_SAND_OUT uint32                        *vsq_index,
    SOC_SAND_OUT uint8                         *is_ocb_only
  )
{
    uint32 ret;
#if defined(BROADCOM_DEBUG)
    
#endif

    SOCDNX_INIT_FUNC_DEFS;

    ret = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit, mbcm_dpp_itm_vsq_index_global2group,(unit, vsq_id, soc_vsq_group_type, vsq_index, is_ocb_only)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

int 
soc_dpp_voq_max_size_drop(int unit, uint32 *is_max_size) 
{
    int ret;
    SOCDNX_INIT_FUNC_DEFS;

    ret = (MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_ingress_drop_status,(unit, is_max_size)));
    SOCDNX_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_dpp_is_fap_id_local_and_get_core_id(
    SOC_SAND_IN   int      unit,
    SOC_SAND_IN   uint32   fap_id,    
    SOC_SAND_OUT  uint8    *is_local, 
    SOC_SAND_OUT  int      *core_id   
)
{
    uint8 is_fap_id_local;
    uint32 local_base_fap_id;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(core_id);

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &local_base_fap_id))); 

    if (fap_id >= local_base_fap_id && fap_id < local_base_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        is_fap_id_local = TRUE;
        *core_id = fap_id - local_base_fap_id;
    } else {
        is_fap_id_local = FALSE;
        *core_id = SOC_CORE_ALL;
    }

    if (is_local != NULL) {
        *is_local = is_fap_id_local;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_fc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info 
  )
{
    uint32 ret;

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fc_status_info_get, (unit, fc_status_key, fc_status_info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}



soc_error_t 
dpp_gport_to_tm_dest_info( 
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  soc_gport_t       dest,      
    SOC_SAND_OUT SOC_TMC_DEST_INFO *dest_info 
)
{
    soc_port_t   port;
    uint32       modid, tm_port;
    soc_gport_t  gport;
    int          core, sysport_erp;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(dest_info);

    if (_SHR_GPORT_IS_SET(dest)) {
        gport = dest;
    } else if (SOC_PORT_VALID(unit, dest)) { 
        SOC_GPORT_LOCAL_SET(gport,dest);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("invalid port given as input"))); 
    }
    
    if (SOC_GPORT_IS_LOCAL(gport)) { 
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_get,(unit, &modid)));
        port = SOC_GPORT_LOCAL_GET(gport);
        if (port == SOC_DPP_PORT_INTERNAL_ERP(0) || port == SOC_DPP_PORT_INTERNAL_ERP(1)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.stack._sysport_erp.get(unit, SOC_DPP_CORE_TO_MODID(modid, core), &sysport_erp));
            dest_info->id = sysport_erp;
        } else { 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
            SOC_SAND_IF_ERR_EXIT(arad_modport_to_sys_phys_port_map_get(unit, SOC_DPP_CORE_TO_MODID(modid, core), tm_port, &dest_info->id));
        }
        dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
    } else if (SOC_GPORT_IS_MODPORT(gport)) { 
        SOC_SAND_IF_ERR_EXIT(arad_modport_to_sys_phys_port_map_get(unit, SOC_GPORT_MODPORT_MODID_GET(gport), SOC_GPORT_MODPORT_PORT_GET(gport), &dest_info->id));
        dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
    } else if (SOC_GPORT_IS_TRUNK(gport)) {  
        dest_info->type = SOC_TMC_DEST_TYPE_LAG;
        dest_info->id = SOC_GPORT_TRUNK_GET(gport);
    } else if (_SHR_GPORT_IS_SYSTEM_PORT(gport)) {  
        dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        dest_info->id = _SHR_GPORT_SYSTEM_PORT_ID_GET(gport);
    } else if (SOC_GPORT_IS_UCAST_QUEUE_GROUP(gport)) { 
        dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        dest_info->id = _SHR_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
    } else if (_SHR_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        dest_info->id = _SHR_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    } else if (_SHR_GPORT_IS_MCAST(gport)) {
        dest_info->type = SOC_TMC_DEST_TYPE_MULTICAST;
        dest_info->id = _SHR_MULTICAST_ID_GET(_SHR_GPORT_MCAST_GET(gport));
    } else if (SOC_GPORT_IS_BLACK_HOLE(gport)) {
        dest_info->type = SOC_TMC_DEST_TYPE_QUEUE;
        dest_info->id = 0; 
    } else if (SOC_GPORT_IS_LOCAL_CPU(gport)) { 
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_get,(unit, &modid)));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, CMIC_PORT(unit), &tm_port, &core));
        
        SOC_SAND_IF_ERR_EXIT(arad_local_to_sys_phys_port_map_get(unit, SOC_DPP_CORE_TO_MODID(modid, core), tm_port, &dest_info->id));
        dest_info->type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Gport given can not be parsed as TM information")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_lb_config_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_lb_info_key_t      *lb_key,
    SOC_SAND_OUT soc_lb_cfg_info_t      *lb_cfg_info 
  )
{
    uint32 ret;

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_lb_config_info_get, (unit, lb_key, lb_cfg_info)));
    SOCDNX_SAND_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_core_frequency_config_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int dflt_freq_khz,
    SOC_SAND_OUT uint32* freq_khz
  )
{
    int ret;

    SOCDNX_INIT_FUNC_DEFS;
  
    ret = soc_arad_core_frequency_config_get(unit, dflt_freq_khz, freq_khz);
    SOCDNX_IF_ERR_RETURN(ret);

    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_dpp_max_resource_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core, 
    SOC_SAND_IN  bcm_cosq_resource_t resource,
    SOC_SAND_OUT uint32* max_amount
  )
{
    int rv;
    soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;  
    SOC_TMC_ITM_GUARANTEED_RESOURCE guaranteed_resources; 
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_IF_ERR_RETURN(_bcm_petra_cosq_resource_to_ingress_threshold(unit, resource, &thresh_type));
    if (thresh_type == SOC_TMC_INGRESS_THRESHOLD_INVALID)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("invalid resource specified %d"), resource));
    }

    if (SOC_IS_QAX(unit)) {
        int scale;
        
        
        SOCDNX_IF_ERR_EXIT(soc_tmc_itm_voq_threshold_resolution_get(unit, thresh_type, &scale));

        rv = sw_state_access[unit].dpp.soc.qax.tm.guaranteed_info.get(unit, core, &guaranteed_resources);
        SOCDNX_IF_ERR_EXIT(rv);
        
        *max_amount = guaranteed_resources.guaranteed[thresh_type].total * scale;
        
    } else {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.get(unit, core, &guaranteed_q_resource);
        SOCDNX_IF_ERR_EXIT(rv);

        *max_amount = ((resource == bcmResourceOcbBuffers || SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams == 0) ? 
                       guaranteed_q_resource.ocb.total : guaranteed_q_resource.dram.total);
    }
        
exit:
    SOCDNX_FUNC_RETURN;
}

