
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
#include <soc/mcm/memregs.h> 
#include <soc/mem.h>
#include <soc/dpp/JER/jer_api_egr_queuing.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>



int
  jer_egr_queuing_sp_tc_drop_set(
        int    unit,
        int    core,
        int    tc,
        soc_dpp_cosq_threshold_type_t threshold_type,
        int    threshold_value,
        soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    int
      res;

    SOCDNX_INIT_FUNC_DEFS;

    res = soc_jer_egr_queuing_sp_tc_drop_set_unsafe(
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
  jer_egr_queuing_sp_tc_drop_get(
        int    unit,
        int    core,
        int    tc,
        soc_dpp_cosq_threshold_type_t threshold_type,
        int*   threshold_value,
        soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    int
      res;

    SOCDNX_INIT_FUNC_DEFS;

    res = soc_jer_egr_queuing_sp_tc_drop_get_unsafe(
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
  jer_egr_queuing_sch_unsch_drop_set(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_jer_egr_queuing_sch_unsch_drop_set_unsafe(
          unit,
          core,
          threshold_type,
          dev_thresh
        );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_egr_queuing_sch_unsch_drop_set()", 0, 0);
}

uint32
  jer_egr_unsched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_jer_egr_unsched_drop_set_unsafe(
       unit,
       core,
       profile,
       prio_ndx,
       dp_ndx,
       thresh,
       exact_thresh);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_egr_unsched_drop_set()", 0, 0);
}

uint32
  jer_egr_unsched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_jer_egr_unsched_drop_get_unsafe(
    unit,
    core,
    prio_ndx,
    dp_ndx,
    thresh);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_egr_unsched_drop_get()", 0, 0);
}



uint32
  jer_egr_sched_port_fc_thresh_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_jer_egr_sched_port_fc_thresh_set_unsafe(
       unit,
       core,
       threshold_type,
       thresh
       );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_egr_sched_port_fc_thresh_set()", 0, 0);
}

uint32
  jer_egr_sched_q_fc_thresh_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = soc_jer_egr_sched_q_fc_thresh_set_unsafe(
       unit,
       core,
       prio,
       threshold_type,
       thresh
       );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_egr_sched_q_fc_thresh_set()", 0, 0);
}

int
  jer_egr_queuing_if_fc_uc_set(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  )
{
    int
      res;

    SOCDNX_INIT_FUNC_DEFS;

    res = soc_jer_egr_queuing_if_fc_uc_set_unsafe(
       unit,
       core,
       uc_if_profile_ndx,
       info
       );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME
