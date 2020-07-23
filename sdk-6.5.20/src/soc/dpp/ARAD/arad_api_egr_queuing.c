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

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS


#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>


























uint32
  arad_egr_ofp_thresh_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID    tm_port,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_THRESH_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = arad_egr_ofp_thresh_type_verify(
    unit,
    tm_port,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_ofp_thresh_type_set_unsafe(
    unit,
	core_id,
    tm_port,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_thresh_type_set()",0,0);
}


uint32
  arad_egr_ofp_thresh_type_get(
    SOC_SAND_IN  int                 unit,
	SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_THRESH_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ofp_thresh_type);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_ofp_thresh_type_get_unsafe(
    unit,
	core_id,
    ofp_ndx,
    ofp_thresh_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_thresh_type_get()",0,0);
}


uint32
  arad_egr_sched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = arad_egr_sched_drop_verify(
    unit,
    prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_sched_drop_set_unsafe(
    unit,
    core,
    profile,
    prio_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_set()",0,0);
}


uint32
  arad_egr_sched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_SCHED_DROP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_sched_drop_get_unsafe(
    unit,
    prio_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_drop_get()",0,0);
}


uint32
  arad_egr_unsched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  res = arad_egr_unsched_drop_verify(
    unit,
    prio_ndx,
    dp_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_unsched_drop_set_unsafe(
    unit,
    core,
    profile,
    prio_ndx,
    dp_ndx,
    thresh,
    exact_thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_set()",0,0);
}

uint32
  arad_egr_sched_port_fc_thresh_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_SCHED_PORT_FC_THRESH_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_sched_port_fc_thresh_set_unsafe(
    unit,
    core,
    threshold_type,
    thresh
  );

  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_port_fc_thresh_set()",0,0);

}

uint32
  arad_egr_sched_q_fc_thresh_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  )
{
  uint32 res;
  ARAD_EGR_THRESH_INFO thresh_info;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_SCHED_Q_FC_THRESH_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  thresh_info.dbuff = thresh->data_buffers;
  thresh_info.packet_descriptors = thresh->packet_descriptors;

  res = arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
               unit,
               core,
               prio,
               threshold_type,
               &thresh_info);

  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_sched_q_fc_thresh_set()",0,0);
  
}


uint32
  arad_egr_unsched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_UNSCHED_DROP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_unsched_drop_get_unsafe(
    unit,
    prio_ndx,
    dp_ndx,
    thresh
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_unsched_drop_get()",0,0);
}


int
  arad_egr_dev_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *exact_thresh
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(thresh);
  SOCDNX_NULL_CHECK(exact_thresh);

  res = arad_egr_dev_fc_verify(
    unit,
    thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_egr_dev_fc_set_unsafe(
    unit,
    core,
    thresh,
    exact_thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_dev_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *thresh
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(thresh);

  res = arad_egr_dev_fc_get_unsafe(
    unit,
    core,
    thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_ofp_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *exact_thresh
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(thresh);
  SOCDNX_NULL_CHECK(exact_thresh);

  res = arad_egr_ofp_fc_verify(
    unit,
    prio_ndx,
    ofp_type_ndx,
    thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_egr_ofp_fc_set_unsafe(
    unit,
    core,
    prio_ndx,
    ofp_type_ndx,
    thresh,
    exact_thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;


  SOCDNX_NULL_CHECK(thresh);

  res = arad_egr_ofp_fc_get_unsafe(
    unit,
    core,
    prio_ndx,
    ofp_type_ndx,
    thresh
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_egr_ofp_scheduling_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_egr_ofp_scheduling_verify(
    unit,
    tm_port,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_ofp_scheduling_set_unsafe(
    unit,
    core,
    tm_port,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_scheduling_set()",0,0);
}


uint32
  arad_egr_ofp_scheduling_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_OFP_SCHEDULING_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_ofp_scheduling_get_unsafe(
    unit,
    core,
    tm_port,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_scheduling_get()",0,0);
}


uint32
  arad_egr_dsp_pp_to_base_q_pair_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT uint32                   *base_q_pair
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET);
  SOC_SAND_CHECK_NULL_INPUT(base_q_pair);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_egr_dsp_pp_to_base_q_pair_get_verify(
          unit,
          core,
          ofp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_dsp_pp_to_base_q_pair_get_unsafe(
          unit, 
          core,
          ofp_ndx,
          base_q_pair
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_to_base_q_pair_get()", ofp_ndx, 0);
}


uint32
  arad_egr_dsp_pp_priorities_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               ofp_ndx,
    SOC_SAND_OUT ARAD_EGR_PORT_PRIORITY_MODE    *priority_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET);
  SOC_SAND_CHECK_NULL_INPUT(priority_mode);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_egr_dsp_pp_priorities_mode_get_verify(
          unit,
          ofp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_dsp_pp_priorities_mode_get_unsafe(
          unit,
          ofp_ndx,
          priority_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_dsp_pp_priorities_mode_get()", ofp_ndx, 0);
}


int
  arad_egr_queuing_dev_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core,
      SOC_SAND_IN  ARAD_EGR_QUEUING_DEV_TH  *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_dev_set_unsafe(
          unit,
          core,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_queuing_dev_get(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_OUT ARAD_EGR_QUEUING_DEV_TH    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_dev_get_unsafe(
          unit,
          core,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



int
  arad_egr_queuing_global_drop_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_global_drop_set_unsafe(
          unit,
          core,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_queuing_global_drop_get(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_global_drop_get_unsafe(
          unit,
          core,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_queuing_global_fc_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_IN  int    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_global_fc_set_unsafe(
          unit,
          core,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_queuing_global_fc_get(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  int   core, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_type_t threshold_type,
      SOC_SAND_OUT int*    threshold_value, 
      SOC_SAND_IN  soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_global_fc_get_unsafe(
          unit,
          core,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



int
  arad_egr_queuing_mc_tc_fc_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_mc_tc_fc_set_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_queuing_mc_tc_fc_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_mc_tc_fc_get_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}




int
  arad_egr_queuing_sp_tc_drop_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

    res = arad_egr_queuing_sp_tc_drop_set_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_queuing_sp_tc_drop_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

    res = arad_egr_queuing_sp_tc_drop_get_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_queuing_sp_reserved_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

    res = arad_egr_queuing_sp_reserved_set_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  arad_egr_queuing_sp_reserved_get(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*    threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

    res = arad_egr_queuing_sp_reserved_get_unsafe(
          unit,
          core,
          tc,
          threshold_type,
          threshold_value,
          drop_type
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



uint32 
  arad_egr_queuing_sch_unsch_drop_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_SCH_UNSCH_DROP_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_egr_queuing_sch_unsch_drop_set_unsafe(
          unit,
          core,
          threshold_type,
          dev_thresh
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_sch_unsch_drop_set()", 0, 0);
}



int
  arad_egr_queuing_mc_cos_map_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    tc_ndx,
    SOC_SAND_IN    uint32    dp_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_MC_COS_MAP    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_mc_cos_map_set_unsafe(
          unit,
          core,
          tc_ndx,
          dp_ndx,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32    
  arad_egr_queuing_mc_cos_map_get(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_IN     uint32    tc_ndx,
    SOC_SAND_IN     uint32    dp_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_MC_COS_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_mc_cos_map_get_unsafe(
          unit,
          core,
          tc_ndx,
          dp_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_arad_egr_queuing_mc_cos_map_get()", 0, 0);
}



int
  arad_egr_queuing_if_fc_set(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_IN  ARAD_EGR_QUEUING_IF_FC    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_set_unsafe(
          unit,
          if_ndx,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_egr_queuing_if_fc_get(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     ARAD_INTERFACE_ID    if_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_FC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_if_fc_get_unsafe(
          unit,
          if_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_arad_egr_queuing_if_fc_get()", 0, 0);
}



int
  arad_egr_queuing_if_fc_uc_max_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_uc_set_unsafe(
          unit,
          core,
          uc_if_profile_ndx,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



int
  arad_egr_queuing_if_fc_uc_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    uc_if_profile_ndx,
    SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_uc_set_unsafe(
          unit,
          core,
          uc_if_profile_ndx,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_queuing_if_fc_uc_get(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     int    core,
    SOC_SAND_IN     uint32    uc_if_profile_ndx,
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC    *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_uc_get_unsafe(
          unit,
          core,
          uc_if_profile_ndx,
          info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



int
  arad_egr_queuing_if_fc_mc_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    uint32    mc_if_profile_ndx,
    SOC_SAND_IN uint32    pd_th
  )
{

  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_mc_set_unsafe(
          unit,
          core,
          mc_if_profile_ndx,
          pd_th
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  arad_egr_queuing_if_fc_mc_get(
    SOC_SAND_IN     int    unit,
    SOC_SAND_IN     uint32    mc_if_profile_ndx,
    SOC_SAND_OUT uint32   *pd_th
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_egr_queuing_if_fc_mc_get_unsafe(
          unit,
          mc_if_profile_ndx,
          pd_th
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 
  arad_egr_queuing_if_uc_map_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_UC_MAP_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    res = arad_egr_queuing_if_uc_map_set_unsafe(
          unit,
          core,
          interface_type,
          internal_if_id,
          profile
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_uc_map_set()", 0, 0);

}

uint32 
  arad_egr_queuing_if_mc_map_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  soc_port_if_t interface_type,
    SOC_SAND_IN  uint32     internal_if_id,
    SOC_SAND_IN  int        profile
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_IF_MC_MAP_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    res = arad_egr_queuing_if_mc_map_set_unsafe(
          unit,
          core,
          interface_type,
          internal_if_id,
          profile
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_queuing_if_mc_map_set()", 0, 0);

}


uint32
  arad_egr_queuing_ofp_tcg_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         tm_port,    
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tcg_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_ofp_tcg_set_verify(
    unit,
    core,
    tm_port,    
    tcg_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 

  res = arad_egr_queuing_ofp_tcg_set_unsafe(
    unit,
    core,
    tm_port,    
    tcg_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_queuing_ofp_tcg_set()",tm_port,0);
}


uint32
  arad_egr_queuing_ofp_tcg_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID               tm_port,    
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO      *tcg_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_OFP_TCG_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tcg_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_ofp_tcg_get_unsafe(
    unit,
    core,
    tm_port,    
    tcg_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_queuing_ofp_tcg_get()",tm_port,0);
}


uint32
  arad_egr_queuing_tcg_weight_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID          tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tcg_weight);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_tcg_weight_set_verify_unsafe(
    unit,
    core,
    tm_port,
    tcg_ndx,
    tcg_weight
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = arad_egr_queuing_tcg_weight_set_unsafe(
    unit,
    core,
    tm_port,
    tcg_ndx,
    tcg_weight
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_queuing_tcg_weight_set()",tm_port,tcg_ndx);
}


uint32
  arad_egr_queuing_tcg_weight_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ      *tcg_weight
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGR_QUEUING_TCG_WEIGHT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tcg_weight);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_egr_queuing_tcg_weight_get_verify_unsafe(
    unit,
    core,
    tm_port,
    tcg_ndx,
    tcg_weight
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 

  res = arad_egr_queuing_tcg_weight_get_unsafe(
    unit,
    core,
    tm_port,
    tcg_ndx,
    tcg_weight
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_queuing_tcg_weight_get()",tm_port,tcg_ndx);
}

void
  arad_ARAD_EGR_Q_PRIORITY_clear(
    SOC_SAND_OUT ARAD_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_EGR_Q_PRIORITY));
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_DROP_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_DROP_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_THRESH_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_THRESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_THRESH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_TH_DB_GLOBAL_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_GLOBAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_TH_DB_POOL_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_POOL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_TH_DB_POOL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_TH_DB_PORT_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TH_DB_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_TH_DB_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_DEV_TH_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_DEV_TH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_DEV_TH_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_MC_COS_MAP_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_MC_COS_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_MC_COS_MAP_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_IF_FC_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_IF_FC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_QUEUING_IF_UC_FC_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_IF_UC_FC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_DEV_THRESH_INNER_clear(
    SOC_SAND_OUT ARAD_EGR_FC_DEV_THRESH_INNER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEV_THRESH_INNER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_DEVICE_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_DEVICE_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEVICE_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_CHNIF_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_CHNIF_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_CHNIF_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_OFP_THRESH_clear(
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_OFP_THRESH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_OFP_SCH_WFQ_clear(
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_WFQ_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_OFP_SCH_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_OFP_SCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_EGR_QUEUING_TCG_INFO_clear(
    SOC_SAND_OUT ARAD_EGR_QUEUING_TCG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_TCG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_EGR_TCG_SCH_WFQ_clear(
    SOC_SAND_OUT ARAD_EGR_TCG_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_TCG_SCH_WFQ_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

void
  ARAD_EGR_Q_PRIORITY_print(
    SOC_SAND_IN  ARAD_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  arad_ARAD_EGR_Q_PRIO_to_string(
    SOC_SAND_IN  ARAD_EGR_Q_PRIO enum_val
  )
{
  return SOC_TMC_EGR_Q_PRIO_to_string(enum_val);
}

const char*
  arad_ARAD_EGR_PORT_THRESH_TYPE_to_string(
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE enum_val
  )
{
  return SOC_TMC_EGR_PORT_THRESH_TYPE_to_string(enum_val);
}


const char*
  arad_ARAD_EGR_OFP_INTERFACE_PRIO_to_string(
    SOC_SAND_IN  ARAD_EGR_OFP_INTERFACE_PRIO enum_val
  )
{
  return SOC_TMC_EGR_OFP_INTERFACE_PRIO_to_string(enum_val);
}

const char*
  arad_ARAD_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    SOC_SAND_IN  ARAD_EGR_Q_PRIO_MAPPING_TYPE enum_val
  )
{
  return SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE_to_string(enum_val);
}

void
  arad_ARAD_EGR_DROP_THRESH_print(
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_DROP_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_THRESH_INFO_print(
    SOC_SAND_IN  ARAD_EGR_THRESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_THRESH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_DEV_THRESH_INNER_print(
    SOC_SAND_IN  ARAD_EGR_FC_DEV_THRESH_INNER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEV_THRESH_INNER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_DEVICE_THRESH_print(
    SOC_SAND_IN  ARAD_EGR_FC_DEVICE_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_DEVICE_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_CHNIF_THRESH_print(
    SOC_SAND_IN  ARAD_EGR_FC_CHNIF_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_CHNIF_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_FC_OFP_THRESH_print(
    SOC_SAND_IN  ARAD_EGR_FC_OFP_THRESH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_FC_OFP_THRESH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_OFP_SCH_WFQ_print(
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_WFQ_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_EGR_OFP_SCH_INFO_print(
    SOC_SAND_IN  ARAD_EGR_OFP_SCH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_OFP_SCH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  ARAD_EGR_QUEUING_TCG_INFO_print(
    SOC_SAND_IN  ARAD_EGR_QUEUING_TCG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_QUEUING_TCG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_EGR_TCG_SCH_WFQ_print(
    SOC_SAND_IN  ARAD_EGR_TCG_SCH_WFQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_EGR_TCG_SCH_WFQ_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

