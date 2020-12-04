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
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_ingress_scheduler.h>

#include <soc/dpp/ARAD/arad_api_ingress_scheduler.h>

#include <soc/mcm/memregs.h>
#include <soc/dpp/drv.h>




























uint32
  arad_ingress_scheduler_mesh_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  res = arad_ingress_scheduler_mesh_verify(
    unit,
    mesh_info,
    exact_mesh_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ingress_scheduler_mesh_set_unsafe(
    unit,
    mesh_info,
    exact_mesh_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_set()",0,0);
}


soc_error_t
  arad_ingress_scheduler_mesh_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  uint32              rate
    )
{
    uint32 soc_sand_rc;
    SOC_TMC_ING_SCH_MESH_INFO mesh_info, exact_mesh_info;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);

    arad_ARAD_ING_SCH_MESH_INFO_clear(&exact_mesh_info);

    
    arad_ARAD_ING_SCH_MESH_INFO_SHAPERS_dont_touch(&mesh_info);

    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].shaper.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].shaper.max_rate = rate;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }
    
    soc_sand_rc = arad_ingress_scheduler_mesh_set(unit, &mesh_info, &exact_mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);

exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_mesh_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 weight
    )
{
    uint32 soc_sand_rc;
    SOC_TMC_ING_SCH_MESH_INFO mesh_info, exact_mesh_info;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);

    arad_ARAD_ING_SCH_MESH_INFO_clear(&exact_mesh_info);

    
    arad_ARAD_ING_SCH_MESH_INFO_SHAPERS_dont_touch(&mesh_info);

    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].weight = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].weight = weight;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }

    soc_sand_rc = arad_ingress_scheduler_mesh_set(unit, &mesh_info, &exact_mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);
      
exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_mesh_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    )
{
    uint32 soc_sand_rc;
    SOC_TMC_ING_SCH_MESH_INFO mesh_info, exact_mesh_info;

    SOCDNX_INIT_FUNC_DEFS; 
    
    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);

    arad_ARAD_ING_SCH_MESH_INFO_clear(&exact_mesh_info);

    
    arad_ARAD_ING_SCH_MESH_INFO_SHAPERS_dont_touch(&mesh_info);

    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].shaper.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].shaper.max_burst = burst;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }
    
    soc_sand_rc = arad_ingress_scheduler_mesh_set(unit, &mesh_info, &exact_mesh_info);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);    
      
exit:    
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_ingress_scheduler_mesh_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO   *mesh_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INGRESS_SCHEDULER_MESH_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ingress_scheduler_mesh_get_unsafe(
    unit,
    mesh_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ingress_scheduler_mesh_get()",0,0);
}

soc_error_t
  arad_ingress_scheduler_mesh_bandwidth_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
    )
{
    ARAD_ING_SCH_MESH_INFO mesh_info;
    uint32 soc_sand_rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);

    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].shaper.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        *rate = mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].shaper.max_rate;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_mesh_sched_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *weight
    )
{
    ARAD_ING_SCH_MESH_INFO mesh_info;
    uint32 soc_sand_rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);  
    
    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        *weight = mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].weight;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_mesh_burst_get(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
    )
{
    ARAD_ING_SCH_MESH_INFO mesh_info;
    uint32 soc_sand_rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rc = arad_ingress_scheduler_mesh_get(unit, &mesh_info);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rc);    

    if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_LOCAL].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON1].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON2].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON3].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV4(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON4].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV5(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON5].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV6(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON6].shaper.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV7(gport)) {
        *burst = mesh_info.contexts[ARAD_ING_SCH_MESH_CON7].shaper.max_burst;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric mesh type\n"), gport));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_ingress_scheduler_clos_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO   *clos_info   
  )
{

  uint32  res;
  SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   exact_clos_info;

SOCDNX_INIT_FUNC_DEFS;

  arad_ARAD_ING_SCH_CLOS_INFO_clear(&exact_clos_info);
  res = arad_ingress_scheduler_clos_verify(
    unit,
    clos_info,
    &exact_clos_info
  );
 SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_ingress_scheduler_clos_set_unsafe(
    unit,
    clos_info,
    &exact_clos_info
  );

  SOCDNX_SAND_IF_ERR_EXIT(res);
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_ingress_scheduler_clos_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  uint32              rate
    )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));

    
    arad_ARAD_ING_SCH_CLOS_INFO_SHAPER_dont_touch(&clos_info);

    if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport)) {
        clos_info.shapers.local.max_rate = rate;
    }
    else if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        clos_info.shapers.hp.local.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)){
        clos_info.shapers.fabric.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        clos_info.shapers.hp.fabric_unicast.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        clos_info.shapers.hp.fabric_multicast.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        clos_info.shapers.lp.fabric_multicast.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        clos_info.shapers.lp.fabric_unicast.max_rate = rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport) || 
             _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("cannot set bandwidth, gport does not have shaper\n")));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("gport type is not matched to fabric clos type\n")));
    }
    
    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_set(unit, 0, &clos_info));

      
exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_clos_sched_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 weight
    )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));

    
    arad_ARAD_ING_SCH_CLOS_INFO_SHAPER_dont_touch(&clos_info);

    if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        clos_info.weights.fabric_hp.weight1 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        clos_info.weights.fabric_hp.weight2 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        clos_info.weights.fabric_lp.weight1 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        clos_info.weights.fabric_lp.weight2 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        clos_info.weights.global_hp.weight1 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport)) {
        clos_info.weights.global_hp.weight2 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        clos_info.weights.global_lp.weight1 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport)) {
        clos_info.weights.global_lp.weight2 = weight;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("cannot get scheduling policy, gport %d does not have weights\n"), gport));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric clos type\n"), gport));
    } 
    
    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_set(unit, 0, &clos_info));

exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_clos_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));

    
    arad_ARAD_ING_SCH_CLOS_INFO_SHAPER_dont_touch(&clos_info);

    
    if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport)) {
        clos_info.shapers.local.max_burst = burst;
    }
    else if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        clos_info.shapers.hp.local.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)){
        clos_info.shapers.fabric.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        clos_info.shapers.hp.fabric_unicast.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        clos_info.shapers.hp.fabric_multicast.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        clos_info.shapers.lp.fabric_multicast.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        clos_info.shapers.lp.fabric_unicast.max_burst = burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport) || 
             _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("cannot set max burst, gport %d does not have a shaper\n"), gport));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric clos type\n"), gport));
    }
    
    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_set(unit, 0, &clos_info));

exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_ingress_scheduler_clos_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO   *clos_info
  )
{

  uint32  res;

SOCDNX_INIT_FUNC_DEFS;

  res = arad_ingress_scheduler_clos_get_unsafe(
    unit,
    clos_info
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_clos_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
  )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;
        
    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));
 
    if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport)) {
        *rate = clos_info.shapers.local.max_rate;
    }
    else if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        *rate = clos_info.shapers.hp.local.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)){
        *rate = clos_info.shapers.fabric.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        *rate = clos_info.shapers.hp.fabric_unicast.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        *rate = clos_info.shapers.hp.fabric_multicast.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        *rate = clos_info.shapers.lp.fabric_multicast.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        *rate = clos_info.shapers.lp.fabric_unicast.max_rate;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport) || 
             _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL,(_BSL_SOCDNX_MSG("cannot get bandwidth, specified gport does not have a shaper\n")));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("specified gport type is not matched to fabric clos type\n")));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_clos_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *weight
  )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));    

    if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        *weight = clos_info.weights.fabric_hp.weight1;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        *weight = clos_info.weights.fabric_hp.weight2;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        *weight = clos_info.weights.fabric_lp.weight1;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        *weight = clos_info.weights.fabric_lp.weight2;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        *weight = clos_info.weights.global_hp.weight1;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport)) {
        *weight = clos_info.weights.global_hp.weight2;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        *weight = clos_info.weights.global_lp.weight1;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport)) {
        *weight = clos_info.weights.global_lp.weight2;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("cannot get scheduling policy, gport %d does not have weights\n"), gport));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric clos type\n"), gport));
    }


exit:    
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_ingress_scheduler_clos_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
  )
{
    ARAD_ING_SCH_CLOS_INFO clos_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_ingress_scheduler_clos_get(unit, 0, &clos_info));
        
    if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL(gport)) {
        *burst = clos_info.shapers.local.max_burst;
    }
    else if(_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_HIGH(gport)) {
        *burst = clos_info.shapers.hp.local.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)){
        *burst = clos_info.shapers.fabric.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)) {
        *burst = clos_info.shapers.hp.fabric_unicast.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)) {
        *burst = clos_info.shapers.hp.fabric_multicast.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)) {
        *burst = clos_info.shapers.lp.fabric_multicast.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)) {
        *burst = clos_info.shapers.lp.fabric_unicast.max_burst;
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC_HIGH(gport) || _SHR_GPORT_IS_FABRIC_CLOS_FABRIC_LOW(gport) || 
             _SHR_GPORT_IS_FABRIC_CLOS_UNICAST_LOCAL_LOW(gport)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("cannot get max burst, gport %d does not have a shaper\n"), gport));
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("gport %d type is not matched to fabric clos type\n"), gport));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}

void
  arad_ARAD_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT ARAD_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_SHAPER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_MESH_INFO_SHAPERS_dont_touch(
    SOC_SAND_OUT ARAD_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_ING_SCH_MESH_INFO_SHAPERS_dont_touch(info);
  
  exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_SHAPERS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_INFO_SHAPER_dont_touch(
    SOC_SAND_OUT ARAD_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_ING_SCH_CLOS_INFO_SHAPERS_dont_touch(info);
  
  exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN  ARAD_ING_SCH_MESH_CONTEXTS enum_val
  )
{
  return SOC_TMC_ING_SCH_MESH_CONTEXTS_to_string(enum_val);
}

void
  arad_ARAD_ING_SCH_SHAPER_print(
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_SHAPER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN  ARAD_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN  ARAD_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_SHAPERS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN  ARAD_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 

uint8
  arad_ingress_scheduler_conversion_test_api(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  )
{
  uint8 pass;

  pass = arad_ingress_scheduler_conversion_test(
           is_regression,
           silent
         );

  if (!pass)
  {
    LOG_CLI((BSL_META("The arad_ingress_scheduler_conversion_test has FAILED!"
                      "\n\r"
                 )));
    goto exit;
  }
  else
  {
    LOG_CLI((BSL_META("The arad_ingress_scheduler_conversion_test has passed successfully!"
                      "\n\r"
                 )));
  }

exit:
  return pass;

}

#include <soc/dpp/SAND/Utils/sand_footer.h> 

