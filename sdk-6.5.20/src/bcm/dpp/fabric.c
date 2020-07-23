/*

 * 

 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B COSQ
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <shared/gport.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/fabric.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/cosq.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/fabric.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_fabric.h> 
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/JER/jer_tdm.h>

/* Number of Ports for Enhance application */
#define DPP_FABRIC_ENHANCE_NOF_PORTS    (4)
#define DPP_FABRIC_ENHANCE_NOF_BE_PORTS (DPP_FABRIC_ENHANCE_NOF_PORTS-1)
#define SOC_SAND_TRUE  1
#define SOC_SAND_FALSE 0

#define DPP_TDM_DIRECT_ROUTING_DEFAULT_PROFILE (0)



STATIC int 
_bcm_petra_fabric_vsq_category_set(int unit,
                                   int core_id,
                                   bcm_fabric_control_t control_type,
                                   int queue_id_in,
                                   int *queue_id_out);
STATIC int 
_bcm_petra_fabric_vsq_category_get(int unit,
                                   int core_id,
                                   bcm_fabric_control_t control_type,
                                   int *queue_id);
STATIC int
_bcm_petra_fabric_queue_range_set(int unit,
                                  bcm_fabric_control_t control_type,
                                  int queue_id);
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_get(int unit,
                                               int core_id,
                                               int* mode);
STATIC int
_bcm_petra_fabric_multicast_queue_range_get(int unit,
                                            int core_id,
                                            bcm_fabric_control_t control_type,
                                            int* queue_id);
STATIC int
_bcm_petra_fabric_unicast_queue_range_get(int unit,
                                          int core_id,
                                          bcm_fabric_control_t control_type,
                                          int* queue_id);
STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_get(int unit,
                                                 int core_id,
                                                 bcm_fabric_control_t control_type,
                                                 int* queue_id);
STATIC int
_bcm_petra_fabric_egress_queue_range_set(int unit,
                                         int core_id,
                                          bcm_fabric_control_t type,
                                          int queue_id);
STATIC int
_bcm_petra_fabric_egress_queue_range_get(int unit,
                                         int core_id,
                                         bcm_fabric_control_t control_type,
                                         int* queue_id);
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_set(int unit,
                                               int mode);
STATIC int
_bcm_petra_fabric_credit_watchdog_range_set(int unit,
                      bcm_fabric_control_t control_type,
                      int queue_id,
                      int common_message_time);
STATIC int
_bcm_petra_fabric_credit_watchdog_range_get(int unit,
                                            bcm_fabric_control_t control_type,
                                            int *queue_id);
STATIC int
_bcm_petra_fabric_tdm_range_size_set(int unit,
                                     int minimum_size,
                                     int maximum_size); 

STATIC int
_bcm_petra_fabric_tdm_range_size_get(int unit,
                                     int *minimum_size,
                                     int *maximum_size);

STATIC int
_bcm_petra_fabric_tdm_from_action_to_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type);

STATIC int
_bcm_petra_fabric_tdm_to_action_from_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type);
int
bcm_petra_fabric_init(int unit)
{   
    bcm_error_t rc = BCM_E_NONE;
    uint8 enable_rpt_reachable = FALSE;
    int8 credit_watchdog_mode;
    int port;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_IS_FLAIR(unit)){
        /* If credit watchdog configuration is not set, set or restore it from hardware */
        BCMDNX_IF_ERR_EXIT(GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode));
        if (IS_CREDIT_WATCHDOG_UNINITIALIZED(unit, credit_watchdog_mode)) {
            SOC_TMC_ITM_CR_WD_INFO crwd_info;
            SOC_TMC_ITM_CR_WD_INFO_clear(&crwd_info);
            crwd_info.top_queue = crwd_info.bottom_queue = ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP;
            SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, BCM_CORE_ALL, &crwd_info, &crwd_info)));
        }
        if (!SOC_WARM_BOOT(unit)) {  /* if cold boot, initially turn on credit watchdog */
            BCMDNX_IF_ERR_EXIT(GET_CREDIT_WATCHDOG_MODE(unit, credit_watchdog_mode));
            BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, GET_CREDIT_WATCHDOG_MODE_BASE(credit_watchdog_mode)));
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        bcm_port_t link;
        int repeater_link_enable;

        /*links that connected to a repeater*/
        PBMP_SFI_ITER(unit, link) {
            repeater_link_enable = soc_property_port_get(unit, link, spn_REPEATER_LINK_ENABLE, 0);
            if (repeater_link_enable) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_link_control_set(unit, link, bcmFabricLinkRepeaterEnable, 1));
            }
        }
    }

    /* Initalize TDM fabric direct routing */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass && !SOC_WARM_BOOT(unit)) {
      SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing;
      int template_init_id;
      uint32 soc_sand_rc;
      SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing);

      /* By default enable all links */
      PBMP_SFI_ITER(unit, port){
          SHR_BITSET(direct_routing.link_bitmap.arr, port - FABRIC_LOGICAL_PORT_BASE(unit));
      }

      /* Set HW direct routing */
      template_init_id = DPP_TDM_DIRECT_ROUTING_DEFAULT_PROFILE;

      soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, template_init_id, &direct_routing, enable_rpt_reachable)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

      /* Init template management */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_init(unit,template_init_id,&direct_routing);
      BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rc);
    if(rc != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FABRIC,
                  (BSL_META_U(unit,
                              " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
    }
    BCMDNX_FUNC_RETURN;
}                                        
    
int
bcm_petra_fabric_port_get(int unit,
                          bcm_gport_t child_port,
                          uint32 flags,
                          bcm_gport_t *parent_port)
{
    int modid = 0, base_modid=0;
    bcm_port_t port = 0, tm_port;
    bcm_port_t interface = 0;
    int    core;
    int    rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(parent_port);

    rv = bcm_petra_stk_my_modid_get(unit, &base_modid);
    BCMDNX_IF_ERR_EXIT(rv);

    if (BCM_GPORT_IS_LOCAL(child_port)) {

        port = BCM_GPORT_LOCAL_GET(child_port);
        interface = port + BCM_DPP_PORT_INTERFACE_START;
        BCM_GPORT_LOCAL_SET(*parent_port, interface);

    } else if (BCM_GPORT_IS_MODPORT(child_port)) {

        modid = BCM_GPORT_MODPORT_MODID_GET(child_port);
        tm_port = BCM_GPORT_MODPORT_PORT_GET(child_port);

        if (SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, base_modid)){
            core = SOC_DPP_MODID_TO_CORE(unit, base_modid, modid);
            rv = soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, (soc_port_t *)&port);
            BCMDNX_IF_ERR_EXIT(rv);


            interface = port + BCM_DPP_PORT_INTERFACE_START;
            BCM_GPORT_LOCAL_SET(*parent_port, interface);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport type unsupported (0x%08x)"), child_port));
        }

    } else if ((SOC_PORT_VALID(unit, child_port)) && (IS_PORT(unit, child_port))) {

      interface = child_port + BCM_DPP_PORT_INTERFACE_START;
      BCM_GPORT_LOCAL_SET(*parent_port, interface);

    } else if (BCM_COSQ_GPORT_IS_E2E_PORT(child_port)) {

        port = BCM_COSQ_GPORT_E2E_PORT_GET(child_port);
        interface = port + BCM_DPP_PORT_INTERFACE_START;
        BCM_COSQ_GPORT_E2E_PORT_SET(*parent_port, interface);

    } else {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport type unsupported (0x%08x)"), child_port));
    }

    if (port >= BCM_DPP_PORT_INTERFACE_END) {
        *parent_port = -1;
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport interface parent currently unsupported interface_id(0x%08x)"), port));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_control_set(int unit,
                             bcm_fabric_control_t type,
                             int arg)
{
    bcm_error_t rc = BCM_E_NONE;    
    uint32 soc_sand_rv;   
    int minimum_size = 0;
    int maximum_size = 0;
    int queue_id; 
    ARAD_MGMT_OCB_MC_RANGE range;
    uint32 range_ndx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    switch (type) {
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricShaperQueueMin:            
        case bcmFabricShaperQueueMax:
        case bcmFabricMulticastQueueMin:
        case bcmFabricMulticastQueueMax:
            rc = _bcm_petra_fabric_queue_range_set(unit, type, arg);
            break;
        case bcmFabricTrafficManagementCosMode:
            break;

        case bcmFabricCellSizeFixed:
            /* Cell fixed size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            minimum_size = maximum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);
            
            break;
        case bcmFabricCellSizeMin:
            /* Cell size Minimum */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            minimum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);
            break;
        case bcmFabricCellSizeMax:
            /* Cell size Maximum */   
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            maximum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);         
            break;

        case bcmFabricCreditSize:
            /* one of the first configurations that should be done. Also set via SOC property */
            /* "credit_size" .                                                                */
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_set,(unit, arg)));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricCreditSizeRemoteDefault:   /* Default remote credit size */
            if (!(SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmFabricCreditSizeRemoteDefault is not supported on this device")));
            } else {
                BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_remote_set, (unit, arg)));
            }
            break;
        case bcmFabricVsqCategory:
            rc = bcm_petra_cosq_fmq_vsq_category_mode_set(unit, SOC_CORE_ALL, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            /* Set all VOQs to category 2 when mode is None. */
            if (arg == bcmFabricVsqCatagoryModeNone) {
                queue_id = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit);
                rc = _bcm_petra_fabric_vsq_category_set(unit, BCM_CORE_ALL, bcmFabricQueueMax, queue_id, &queue_id);
                BCMDNX_IF_ERR_EXIT(rc);
            }                       

            break;
        case bcmFabricMulticastSchedulerMode:
            rc = _bcm_petra_fabric_multicast_scheduler_mode_set(unit,arg);
            break;
        case bcmFabricIsolate:
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_set,(unit, arg ? FALSE : TRUE)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);  
            break;
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            /* Set queue range for egress queues */
            rc = _bcm_petra_fabric_egress_queue_range_set(unit, BCM_CORE_ALL, type, arg);
            break;
        case bcmFabricOcbMulticastId1Min:
        case bcmFabricOcbMulticastId1Max:
        case bcmFabricOcbMulticastId2Min:
        case bcmFabricOcbMulticastId2Max:
            if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId1Max)
            {
                range_ndx = 0;
            }
            else
            {
                range_ndx = 1;
            } 
            soc_sand_rv = arad_mgmt_ocb_mc_range_get(
                            unit,
                            range_ndx,
                            &range
                          );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId2Min)
            { 
                range.min = arg;
            }
            else
            {
                range.max = arg;
            }
            soc_sand_rv = arad_mgmt_ocb_mc_range_set(
                            unit,
                            range_ndx,
                            &range
                          );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;
        case bcmFabricWatchdogQueueMin:    /* Watchdog QID start index */
        case bcmFabricWatchdogQueueMax:    /* Watchdog QID end index */
        case bcmFabricWatchdogQueueEnable: /* Queue Watchdog Functionality Enable/Disable setting (True/False) */
            /* Set queue range and enable/disable for credit watchdog */
            rc = _bcm_petra_fabric_credit_watchdog_range_set(unit, type, arg, -1);
            break;

       case bcmFabricRCIControlSource:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_rci_enable_set,(unit, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

    case bcmFabricRCIIncrementValue:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_set, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciLeakyBucketEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_set, (unit, SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciBackoffEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_set, (unit, SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricMinimalLinksToDestination:
           if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
               if (!BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, arg)){
                   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("number of links %d is out of range"), arg));
               }
               rc = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_fabric_minimal_links_to_dest_set, (unit, BCM_MODID_ALL, arg));
           } else {
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmFabricMinimalLinksToDestination is not supported on this device")));
           }
           break;
        case bcmFabricControlCellsEnable:
              soc_sand_rv= MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_set, (unit,arg));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;
        case bcmFabricForceTdmBypassTrafficToFabric:
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set, (unit, arg)));
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}



int
bcm_petra_fabric_control_get(int unit,
                             bcm_fabric_control_t type,
                             int *arg)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint8 ret_bool = FALSE;
    int minimum_size = 0;
    int maximum_size = 0; 
    ARAD_MGMT_OCB_MC_RANGE range;
    uint32 range_ndx;

    BCMDNX_INIT_FUNC_DEFS;
    switch (type) {
        case bcmFabricQueueMin:                                    
        case bcmFabricQueueMax:
            rc = _bcm_petra_fabric_unicast_queue_range_get(unit, BCM_CORE_ALL, type,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricShaperQueueMin:                        
        case bcmFabricShaperQueueMax:
            rc = _bcm_petra_fabric_ingress_shaper_queue_range_get(unit, BCM_CORE_ALL, type, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMulticastQueueMin:
        case bcmFabricMulticastQueueMax:
            rc = _bcm_petra_fabric_multicast_queue_range_get(unit, BCM_CORE_ALL, type, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            /* Set queue range for egress queues */
            rc = _bcm_petra_fabric_egress_queue_range_get(unit, BCM_CORE_ALL, type, arg);        
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricTrafficManagementCosMode:
            break;
        case bcmFabricCellSizeFixed:
            /* Cell fixed size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);

            if (minimum_size != maximum_size) {
                LOG_ERROR(BSL_LS_BCM_FABRIC,
                          (BSL_META_U(unit,
                                      "unit %d, Asking for fixed size, while size is a range, %d - %d\n"), unit, minimum_size,maximum_size));
            }

            *arg = minimum_size;
            break;
        case bcmFabricCellSizeMin:
            /* Minimum cell size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);

            *arg = minimum_size;
            break;
        case bcmFabricCellSizeMax:
            /* Maximum cell size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            *arg = maximum_size;
            break;

        case bcmFabricCreditSize:
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, (uint32 *)arg)));
            BCM_SAND_IF_ERR_EXIT(rc);
            break;
        case bcmFabricCreditSizeRemoteDefault:   /* Default remote credit size */
            if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) {
                uint32 credit_size_remote = -1;                                  
                BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_credit_worth_remote_get, (unit, &credit_size_remote)));
                *arg = credit_size_remote;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmFabricCreditSizeRemoteDefault is not supported on this device")));
            }
            break;
        case bcmFabricVsqCategory:
            {
                bcm_fabric_vsq_category_mode_t vsq_category_mode;
                rc = bcm_petra_cosq_fmq_vsq_category_mode_get(unit, SOC_CORE_ALL, &vsq_category_mode);
                BCMDNX_IF_ERR_EXIT(rc);
                *arg = vsq_category_mode;
            }
            break;
        case bcmFabricMulticastSchedulerMode:
            rc = _bcm_petra_fabric_multicast_scheduler_mode_get(unit, BCM_CORE_ALL, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricIsolate:
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_get,(unit, &ret_bool)));
            BCM_SAND_IF_ERR_EXIT(rc);
            (*arg) = (ret_bool == FALSE ? 1 : 0);
             break;
        case bcmFabricOcbMulticastId1Min:
        case bcmFabricOcbMulticastId1Max:
        case bcmFabricOcbMulticastId2Min:
        case bcmFabricOcbMulticastId2Max:
            if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId1Max)
            {
                range_ndx = 0;
            }
            else
            {
                range_ndx = 1;
            } 
            soc_sand_rv = arad_mgmt_ocb_mc_range_get(
                        unit,
                        range_ndx,
                        &range
                      );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId2Min)
            { 
                *arg = range.min;
            }
            else
            {
                *arg = range.max;
            }
            break;
        case bcmFabricWatchdogQueueMin:    /* Watchdog QID start index */
        case bcmFabricWatchdogQueueMax:    /* Watchdog QID end index */
        case bcmFabricWatchdogQueueEnable: /* Queue Watchdog Functionality Enable/Disable setting (True/False) */
            /* Set queue range and enable/disable for credit watchdog */
            rc = _bcm_petra_fabric_credit_watchdog_range_get(unit, type, arg);
            break;

       case bcmFabricRCIControlSource:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_rci_enable_get,(unit, (soc_dcmn_fabric_control_source_t*) arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricRCIIncrementValue:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_get, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciLeakyBucketEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_get, (unit, SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciBackoffEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_get, (unit, SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricMinimalLinksToDestination:
          if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit))
           {
               soc_sand_rv= MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_minimal_links_to_dest_get, (unit, SOC_MODID_ALL, arg));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
               
           } else{
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmFabricMinimalLinksToDestination is not supported on this device")));
              
           }
           break;

        case bcmFabricForceTdmBypassTrafficToFabric:
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get, (unit, arg)));
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_core_profile_set(int unit, int core, uint32 flags,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
  uint32 soc_sand_rv;
  int ii;
  uint32 max_mbps;    /* maximum bandwidth (in Mega bps) */

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);

  BCMDNX_NULL_CHECK(profile_array);
  {
    /*
     * Validity checks
     * ---------------
     */
    if ((SOC_DPP_DEFS_GET(unit, nof_cores) < 2) && !SOC_IS_QAX(unit))
    {
      /*
       * This procedure is only meaningful for multi-core system.
       */
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("This procedure is only meaningful for multi-core system")));
    }

    if (profile_count > (SOC_DPP_DEFS_GET(unit, nof_sch_active_links) + 1) * SOC_DPP_DEFS_GET(unit, nof_rci_levels)  || profile_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
    }
    /*
     * First, verify all profiles are valid so as not to call 'mbcm_dpp_sch_device_rate_entry_core_set'
     * unless all input is OK.
     */
    for(ii = 0 ; ii < profile_count ; ii++)
    {
      if (profile_array[ii].rci > SOC_DPP_DEFS_GET(unit, nof_rci_levels) - 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RCI level on profile is out of range")));
      }
      if (profile_array[ii].num_links > SOC_DPP_DEFS_GET(unit, nof_sch_active_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of scheduler active links on profile is out of range")));
      }
    }
  }
  for (ii = 0 ; ii < profile_count ; ii++)
  {
    max_mbps = profile_array[ii].max_kbps / 1000 ;
    soc_sand_rv =
      (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_core_set,
        (unit, core, profile_array[ii].rci, profile_array[ii].num_links, max_mbps))) ;
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }

exit:
  BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_core_profile_get(int unit, int core, uint32 flags,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
  uint32 soc_sand_rv;
  int ii;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  
  BCMDNX_NULL_CHECK(profile_array);    
  {
    /*
     * Validity checks
     * ---------------
     */
    if ((SOC_DPP_DEFS_GET(unit, nof_cores) < 2) && !SOC_IS_QAX(unit))
    {
      /*
       * This procedure is only meaningful for multi-core system.
       */
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("This procedure is only meaningful for multi-core system")));
    }

    if (profile_count > (SOC_DPP_DEFS_GET(unit, nof_sch_active_links) + 1) * SOC_DPP_DEFS_GET(unit, nof_rci_levels)  || profile_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
    }
    /*
     * First, verify all profiles are valid so as not to call 'mbcm_dpp_sch_device_rate_entry_core_get'
     * unless all input is OK.
     */
    for(ii = 0 ; ii < profile_count ; ii++)
    {
      if (profile_array[ii].rci > SOC_DPP_DEFS_GET(unit, nof_rci_levels) - 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RCI level on profile is out of range")));
      }
      if (profile_array[ii].num_links > SOC_DPP_DEFS_GET(unit, nof_sch_active_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of scheduler active links on profile is out of range")));
      }
    }
  }
  for (ii = 0 ; ii < profile_count ; ii++)
  {  
    soc_sand_rv =
      (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_core_get,
        (unit, core, profile_array[ii].rci, profile_array[ii].num_links, (uint32*)&(profile_array[ii].max_kbps)))) ;
    /* Convert lower layers units (Mega bps) to units required by API (Kilo bps) */
    profile_array[ii].max_kbps *= 1000 ;
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }
exit:
  BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_profile_set(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
    uint32 soc_sand_rv;
    int ii;
    uint32 max_mbps;    /* maximum bandwidth (in Mega bps) */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(profile_array);
    {
      /*
       * Validity checks
       * ---------------
       */
      /*
       * For Jericho, verify fabric is set to 'shared' mode. Otherwise, device level
       * credit generator is not operational. This remark is also true when only one
       * core is operational.
       */
      if (SOC_IS_JERICHO(unit) && SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode !=
          SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No unit level credit generator when fabric is not 'shared'")));
        }
        if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores <= 1) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No unit level credit generator when not all two cores are active")));
        }
      }
      if (profile_count > (SOC_DPP_DEFS_GET(unit, nof_sch_active_links) + 1) * SOC_DPP_DEFS_GET(unit, nof_rci_levels)  || profile_count < 0) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
      }
      /*
       * First, verify all profiles are valid so as not to call 'mbcm_dpp_sch_device_rate_entry_set'
       * unless all input is OK.
       */
      for(ii = 0 ; ii < profile_count ; ii++)
      {
        if (profile_array[ii].rci > SOC_DPP_DEFS_GET(unit, nof_rci_levels) - 1) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RCI level on profile is out of range")));
        }
        if (profile_array[ii].num_links > SOC_DPP_DEFS_GET(unit, nof_sch_active_links)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of scheduler active links on profile is out of range")));
        }
      }
    }
    for (ii = 0 ; ii < profile_count ; ii++)
    {

      max_mbps = profile_array[ii].max_kbps / 1000 ;
      if (SOC_IS_QAX(unit)) {
          /*
           * For QAX/Kalia- the only relevant table that influences the rate of the device is the one PER CORE
           * but we approach it by the device-level API here, because it actually influences the whole device (QAX has only 1 core)
           */
          soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_core_set, (unit, 0, profile_array[ii].rci, profile_array[ii].num_links, max_mbps)));
      } else {
          soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_set,(unit, profile_array[ii].rci, profile_array[ii].num_links, max_mbps)));
      }
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
     
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_profile_get(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
    uint32 soc_sand_rv;
    int ii;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
    BCMDNX_NULL_CHECK(profile_array);    
    {
      /*
       * Validity checks
       * ---------------
       */
      /*
       * For Jericho, verify fabric is set to 'shared' mode. Otherwise, device level
       * credit generator is not operational. This remark is also true when only one
       * core is operational.
       */
      if (SOC_IS_JERICHO(unit) && SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_links_to_core_mapping_mode !=
          SOC_DPP_FABRIC_LINKS_TO_CORE_MAP_SHARED) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No unit level credit generator when fabric is not 'shared'")));
        }
        if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores <= 1) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No unit level credit generator when not all two cores are active")));
        }
      }
      if (profile_count > (SOC_DPP_DEFS_GET(unit, nof_sch_active_links) + 1) * SOC_DPP_DEFS_GET(unit, nof_rci_levels)  || profile_count < 0) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
      }
      /*
       * First, verify all profiles are valid so as not to call 'mbcm_dpp_sch_device_rate_entry_get'
       * unless all input is OK.
       */
      for(ii = 0 ; ii < profile_count ; ii++)
      {
        if (profile_array[ii].rci > SOC_DPP_DEFS_GET(unit, nof_rci_levels) - 1) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RCI level on profile is out of range")));
        }
        if (profile_array[ii].num_links > SOC_DPP_DEFS_GET(unit, nof_sch_active_links)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of scheduler active links on profile is out of range")));
        }
      }
    }
    for(ii = 0 ; ii < profile_count ; ii++)
    {  
        if (SOC_IS_QAX(unit)) {
            /*
             * For QAX/Kalia- the only relevant table that influences the rate of the device is the one PER CORE
             * but we approach it by the device-level API here, because it actually influences the whole device (QAX has only 1 core)
             */
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_core_get, (unit, 0, profile_array[ii].rci, profile_array[ii].num_links, (uint32*)&(profile_array[ii].max_kbps))));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_get,(unit, profile_array[ii].rci, profile_array[ii].num_links, (uint32*)&(profile_array[ii].max_kbps))));
        }
      /* Convert lower layers units (Mega bps) to units required by API (Kilo bps) */
      profile_array[ii].max_kbps *= 1000 ;
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * external functions
 */
int
bcm_petra_fabric_enhance_ports_get(int unit, bcm_core_t core, int *nbr_ports, int *ports)
{
    bcm_error_t           rc = BCM_E_NONE;
    SOC_SAND_U32_RANGE    queue_range;
    SOC_TMC_MULT_FABRIC_INFO  fabric_info;
    int                   be_class;


    BCMDNX_INIT_FUNC_DEFS;

    (*nbr_ports) = 0;

    /* check if in enhanced mode */
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit, core, &queue_range)));

    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */
    if ( (queue_range.start == DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MIN) && (queue_range.end == DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MAX) ) {
        BCM_EXIT;
    }

    /* determine associated ports */
    SOC_TMC_MULT_FABRIC_INFO_clear(&fabric_info);

    rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit, core, &fabric_info)));
    BCMDNX_IF_ERR_EXIT(rc);

    /* consistency checks */
    if (fabric_info.credits_via_sch != SOC_SAND_TRUE) {
        BCM_EXIT;
    }

    if (fabric_info.guaranteed.gr_sch_port.multicast_class_valid) {
        ports[(*nbr_ports)++] = fabric_info.guaranteed.gr_sch_port.mcast_class_port_id;
    }
    for (be_class = 0; be_class < SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES; be_class++) {
        if (fabric_info.best_effort.be_sch_port[be_class].be_sch_port.multicast_class_valid) {
            ports[(*nbr_ports)++] = fabric_info.best_effort.be_sch_port[be_class].be_sch_port.mcast_class_port_id;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Set FMQ range
 */
STATIC int
_bcm_petra_fabric_queue_range_set(int unit,
                                  bcm_fabric_control_t control_type,
                                  int queue_id)
{
    bcm_error_t rc = BCM_E_NONE; 
    SOC_SAND_U32_RANGE queue_range;
    int queue_config_type;
    int *queue_region_config = NULL;
    int have_first = FALSE, have_last = FALSE;
    int region;
    int current_voq_1k_start = -1; /*Default values*/
    int current_voq_1k_end = -1; /*Default values*/
    int core = 0; /*We can only configure a core symmetrically with the API - so we get the relevant parameters from core 0.*/
    int is_min;
    bcm_cosq_control_range_type_t range_type;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Allocation manager configuration */
    /* Validate queue id */
    if (queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit) || queue_id < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id %d invalid"),queue_id));
    }    
    /*convert to range type*/
    switch (control_type) {
    case bcmFabricMulticastQueueMin:
    case bcmFabricMulticastQueueMax:
        queue_config_type = DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST;
        range_type = bcmCosqRangeMulticastQueue;
        break;
    case bcmFabricShaperQueueMin:            
    case bcmFabricShaperQueueMax:
        queue_config_type = DPP_DEVICE_COSQ_QUEUE_REGION_ISQ;
        range_type = bcmCosqRangeShaperQueue;
        break;
    case bcmFabricQueueMin:
    case bcmFabricQueueMax:
        queue_config_type = DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST;
        range_type = bcmCosqRangeFabricQueue;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("control type %d invalid"), control_type));
    }
    if (control_type == bcmFabricMulticastQueueMin || control_type == bcmFabricShaperQueueMin || control_type == bcmFabricQueueMin || control_type == bcmFabricRecycleQueueMin) {
        is_min = 1;
    } else {
        is_min = 0;
    }

    rc = _bcm_petra_cosq_queue_range_get(unit, BCM_CORE_ALL, range_type, &queue_range);
    BCMDNX_IF_ERR_EXIT(rc);
    /*validate input per range type*/
    
    if ((queue_id % 1024) && (control_type == bcmFabricMulticastQueueMin)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id min %d, must be a multiple of 1024"), queue_id));
    }
    if (((queue_id + 1) % 1024) && (queue_id != 3) && (control_type == bcmFabricMulticastQueueMax)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, FMQ queue_id max %d, must be 3 or a multiple of 1024 -1\n"), unit, queue_id));
    }

    if ((queue_id % 1024) && (queue_id != 4) && (control_type == bcmFabricShaperQueueMin || control_type == bcmFabricQueueMin)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VOQ/ISQ min %d, must be 4 or a multiple of 1024 "),queue_id));
    }    
    /* Special case: queue_id is 4, in that case we treat it as queue_id 0. */
    if (queue_id == 4 && (control_type == bcmFabricShaperQueueMin || control_type == bcmFabricQueueMin)) {
        queue_id = 0;
    }
    if (((queue_id + 1) % 1024) && (control_type == bcmFabricQueueMax || control_type == bcmFabricShaperQueueMax)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, VOQ/ISQ queue_id max %d, must be a multiple of 1024 -1\n"), unit, queue_id));
    }

    
    /* Allocation succeeded, Set enhance range queues */
    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */

    /* 
     *  Check if enhance is disable, if it is and queue range defined is the
     *  same as before, Skip allocation management. Since this is the default
     *  range of multicast fabric which assumed to be already allocated 0-3 or
     *  new range that the allocation management already allocated.
     */
    BCMDNX_ALLOC(queue_region_config, sizeof(int) * DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit), "queue_region");
    
    if (queue_region_config == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("resource memory allocation failur")));
    }
    rc = bcm_dpp_am_cosq_queue_region_config_get(unit, core, queue_region_config, queue_config_type);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Default values*/
    current_voq_1k_start = 0;
    current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
    for (region = 0; region < (DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)); region++) {
        if (queue_region_config[region] == queue_config_type && (have_first == FALSE)) {
            have_first = TRUE;
            current_voq_1k_start = region;
        } else if ((queue_region_config[region] != queue_config_type) && 
                   (have_first == TRUE) && 
                   (have_last == FALSE)){
            have_last = TRUE;
            current_voq_1k_end = region;
            /*We do not support non-continues ranges*/
            break;
        }
    }
    if (is_min) {
        queue_range.end = (current_voq_1k_end * 1024) - 1;            
        queue_range.start = queue_id;
    } else {
        queue_range.start = current_voq_1k_start * 1024;
        queue_range.end = queue_id;
    }
    rc = _bcm_petra_cosq_queue_range_set(
           unit, 
           BCM_CORE_ALL, 
           range_type, 
           (is_min ? BCM_PETRA_COSQ_QUEUE_RANGE_SET_MIN : BCM_PETRA_COSQ_QUEUE_RANGE_SET_MAX),
           FALSE,
           &queue_range);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if (queue_region_config != NULL) {
       BCM_FREE(queue_region_config);
    }

    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Get FMQ range
 */
STATIC int
_bcm_petra_fabric_multicast_queue_range_get(int unit,
                                            int core_id,
                                            bcm_fabric_control_t control_type,
                                            int* queue_id)
{
    SOC_SAND_U32_RANGE queue_range;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(queue_id);    

    /* Get queue id range */
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit, core_id, &queue_range)));

    if (control_type == bcmFabricMulticastQueueMin) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = queue_range.start;
    }

    if (control_type == bcmFabricMulticastQueueMax) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = queue_range.end;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Purpose: Set Enhance mode
 */
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_set(int unit,
                                               int mode)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 tm_ports[DPP_FABRIC_ENHANCE_NOF_PORTS];
    int index, set_required = 0, core;
    SOC_TMC_MULT_FABRIC_INFO fabric_info;    

    BCMDNX_INIT_FUNC_DEFS;
    if (mode < 0 || mode > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mode %d invalid"),mode));
    }

    /* MC Scheduler Mode should be configured in all active cores */
    BCM_DPP_CORES_ITER(_SHR_CORE_ALL, core) {

        rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit, core, &fabric_info)));
        BCMDNX_IF_ERR_EXIT(rc);

        if (mode == 0) {
            if (fabric_info.credits_via_sch == SOC_SAND_TRUE) {
                /* Apply changes, enhance was enabled */
                /* 1. Disable enhance */
                /* 2. Disalloc HRs */
                /* 3. Set ports configuration to be invalid */

                /* Disable enhance */
                fabric_info.credits_via_sch = SOC_SAND_FALSE;  
            
                tm_ports[0] = fabric_info.guaranteed.gr_sch_port.mcast_class_port_id;
                for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_BE_PORTS; index++) {
                    tm_ports[index+1] = fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id;            
                }
    
                /* Disalloc dummy HR "hard wire" ports */
                rc = bcm_petra_cosq_fmq_hr_deallocate(unit, core, DPP_FABRIC_ENHANCE_NOF_PORTS, tm_ports);
                BCMDNX_IF_ERR_EXIT(rc);
    
                /* Disable ports configuration */
                fabric_info.guaranteed.gr_sch_port.mcast_class_port_id = 0;
                fabric_info.guaranteed.gr_sch_port.multicast_class_valid = SOC_SAND_FALSE;
                for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                    fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id = 0;
                    fabric_info.best_effort.be_sch_port[index].be_sch_port.multicast_class_valid = SOC_SAND_FALSE;
                }     
                
                set_required = 1;   
            }          
        } else {
            if (fabric_info.credits_via_sch == SOC_SAND_FALSE) {
                /* Apply changes , enhance was disabled */
                /* 1. Enable enhance */
                /* 2. Alloc HRs */
                /* 3. Set ports configuration */

                /* Enable enhance */
                fabric_info.credits_via_sch = SOC_SAND_TRUE;
        
                /* Obtain Dummy HR "hard wire" Port for scheduelr properties */
                rc = bcm_petra_cosq_fmq_hr_allocate(unit, core, DPP_FABRIC_ENHANCE_NOF_PORTS, tm_ports);
                BCMDNX_IF_ERR_EXIT(rc);
        
                /* Set ports configruation */
                fabric_info.credits_via_sch = 1;
                fabric_info.guaranteed.gr_sch_port.mcast_class_port_id = tm_ports[0];
                fabric_info.guaranteed.gr_sch_port.multicast_class_valid = SOC_SAND_TRUE;
                for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                    fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id = tm_ports[index+1];
                    fabric_info.best_effort.be_sch_port[index].be_sch_port.multicast_class_valid = SOC_SAND_TRUE;
                }

                set_required = 1;
            }
        }
          
        /* Commit configuration */
        if(set_required) {
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_set,(unit, core, &fabric_info)));
            BCMDNX_IF_ERR_EXIT(rc);    
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* wrapper function for external call */
int
bcm_petra_fabric_multicast_scheduler_mode_get(int unit, int core_id, int *mode)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_petra_fabric_multicast_scheduler_mode_get(unit, core_id, mode);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Purpose: Get Enhance mode
 */
int
_bcm_petra_fabric_multicast_scheduler_mode_get(int unit,
                                               int core_id,
                                               int* mode)
{
    int rc;
    SOC_TMC_MULT_FABRIC_INFO fabric_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(mode);    

    SOC_TMC_MULT_FABRIC_INFO_clear(&fabric_info);
    
    rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit, core_id, &fabric_info)));
    BCMDNX_IF_ERR_EXIT(rc);

    *mode = (fabric_info.credits_via_sch == SOC_SAND_TRUE) ? 1:0;

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_connectivity_status_get(
    int unit, 
    int link_partner_max, 
    bcm_fabric_link_connectivity_t *link_partner_array, 
    int *link_partner_count)
{
    int i, port_i;
    uint32 soc_sand_rv;
    int nof_links = 0;
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(link_partner_array);
    BCMDNX_NULL_CHECK(link_partner_count);
    
    BCM_DPP_UNIT_CHECK(unit);       
    
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_nof_links_get,(unit, &nof_links)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_topology_status_connectivity_get,(unit, SOC_DPP_DEFS_GET(unit, first_fabric_link_id), nof_links - 1 + SOC_DPP_DEFS_GET(unit, first_fabric_link_id), &connectivity_map)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    *link_partner_count = 0;
    

    PBMP_SFI_ITER(unit, port_i)
    {
        i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port_i);
        if(*link_partner_count >= link_partner_max)
        {
            *link_partner_count = 0;
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("link_partner_max %d is too small"),link_partner_max));
        }

        if(connectivity_map.link_info[i].far_unit != SOC_TMC_DEVICE_ID_IRRELEVANT
           && connectivity_map.link_info[i].is_logically_connected)
        {                      
            link_partner_array[*link_partner_count].module_id   = connectivity_map.link_info[i].far_unit;
            link_partner_array[*link_partner_count].link_id     = connectivity_map.link_info[i].far_link_id;
            switch(connectivity_map.link_info[i].far_dev_type)
            {
                  case SOC_TMC_FAR_DEVICE_TYPE_FE1:
                  case SOC_TMC_FAR_DEVICE_TYPE_FE3:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE13;
                  break;
                
                  case SOC_TMC_FAR_DEVICE_TYPE_FE2:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE2;
                  break;
                
                  case SOC_TMC_FAR_DEVICE_TYPE_FAP:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFAP;
                  break;
                
                  default:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeUnknown;
                    break;
            }                        
        }
        else
        {
            link_partner_array[*link_partner_count].link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
        }
        (*link_partner_count)++; 
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_connectivity_status_single_get(
    int unit, 
    bcm_port_t link_id, 
    bcm_fabric_link_connectivity_t *link_partner_info)
{
    uint32 soc_sand_rv;
    int link_index;
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(link_partner_info);
    BCM_DPP_UNIT_CHECK(unit);
    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("unit %d, Port %d is out-of-range"), unit, link_id));
    }       
    
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);

    link_index = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link_id);
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_topology_status_connectivity_get,(unit, link_index, link_index, &connectivity_map)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if(connectivity_map.link_info[link_index].far_unit != SOC_TMC_DEVICE_ID_IRRELEVANT
       && connectivity_map.link_info[link_index].is_logically_connected)
    {                      
    link_partner_info->module_id   = connectivity_map.link_info[link_index].far_unit;
    link_partner_info->link_id     = connectivity_map.link_info[link_index].far_link_id;
    switch(connectivity_map.link_info[link_index].far_dev_type)
    {
        case SOC_TMC_FAR_DEVICE_TYPE_FE1:
        case SOC_TMC_FAR_DEVICE_TYPE_FE3:
            link_partner_info->device_type = bcmFabricDeviceTypeFE13;
            break;
        
        case SOC_TMC_FAR_DEVICE_TYPE_FE2:
            link_partner_info->device_type = bcmFabricDeviceTypeFE2;
            break;
        
        case SOC_TMC_FAR_DEVICE_TYPE_FAP:
            link_partner_info->device_type = bcmFabricDeviceTypeFAP;
            break;
        
        default:
            link_partner_info->device_type = bcmFabricDeviceTypeUnknown;
            break;
    }                        
    }
    else
    {
        link_partner_info->link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
    }

    
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_arad_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    uint32 soc_rv = 0;
    uint32 link_status_tmp;
    bcm_port_t link_i;
    BCMDNX_INIT_FUNC_DEFS;

    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("unit %d, Port %d is out-of-range"), unit, link_id));
    }

    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link_id);
    
    BCMDNX_NULL_CHECK(link_status);
    BCMDNX_NULL_CHECK(errored_token_count);

    soc_rv = soc_dpp_fabric_link_status_get(unit, link_i, &link_status_tmp, errored_token_count);
    BCMDNX_IF_ERR_EXIT(soc_rv);

    
    *link_status = 0;
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_CRC_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_SIZE_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_SIZE_ERROR;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_MISALIGN) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_MISALIGN;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_CODE_GROUP_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;
    }
    if(*errored_token_count < 63) {
      (*link_status) |= BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS; 
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    bcm_error_t rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_petra_arad_fabric_link_status_get(unit, link_id, link_status, errored_token_count);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_editing_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_port_t local_port = 0;
    bcm_module_t modid = 0, my_mod;
    SOC_TMC_TDM_FTMH_INFO ftmh_info;
    uint32 soc_sand_rv;
    int is_ingress;
    int core;
    int dest_core;
    int index,nof_bytes;
    uint32 tmp, p_fap_port;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(editing);

    SOC_TMC_TDM_FTMH_INFO_clear(&ftmh_info);

    /* Check if TDM is supported */
    if (!SOC_IS_DPP_TDM_OPTIMIZE(unit) && !SOC_IS_DPP_TDM_STANDARD(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("TDM is not configured")));
    }
    
    /* Validate Gport, retreive port id */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rc);    

    /* Validate port is TDM port */
    if(!IS_TDM_PORT(unit, gport_info.local_port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
    }
    /* Check CRC is added*/
    if(!editing->add_packet_crc && !SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("crc must be added to a system with no petra")));
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, gport_info.local_port, &p_fap_port, &core)));

    /* Retrieve port TDM configuration */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_get,(unit, core, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (ftmh_info.action_ing == SOC_TMC_TDM_NOF_ING_ACTIONS) {
        ftmh_info.action_ing = SOC_TMC_TDM_ING_ACTION_NO_CHANGE;
    }

    if (ftmh_info.action_eg == SOC_TMC_TDM_NOF_EG_ACTIONS) {
        ftmh_info.action_eg = SOC_TMC_TDM_EG_ACTION_NO_CHANGE;
    }

    /* Set packet crc. (only if feature avaialable) */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass) { 
        uint8 add_crc = editing->add_packet_crc, configure_ingress = TRUE, configure_egress = TRUE;
        /* validate parameter */
        if (editing->add_packet_crc < 0 || editing->add_packet_crc > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid add_packet_crc parameter: %d"),editing->add_packet_crc));
        }
        if (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) {
            configure_ingress = (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) ? TRUE : FALSE;
            configure_egress  = (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS)  ? TRUE : FALSE;
        }

        /* Add packet CRC should be done when sending to ARAD or Soc_petra-B packet mode */
        /* Soc_petra-B ingress can not add CRC in case of Bypass mode */
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_port_packet_crc_set,(unit, gport_info.local_port, add_crc, configure_ingress, configure_egress)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
    }
    
    /* Set Ingress configuration */
    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        
        /* 
         * Ingress configuration includes: 
         * 1. Set Action ingress 
         * 2. Set destinatino if action ingress is append or custom external
         * 3. Set user defined if action ingress is append
         */

        /* 1. Set action ingress */
        is_ingress = 1;
        rc = _bcm_petra_fabric_tdm_to_action_from_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);

        /* 2. Set destination only if tdm type is append or custom external */
        rc = bcm_petra_stk_my_modid_get(unit, &my_mod);
        BCMDNX_IF_ERR_EXIT(rc);

        if (editing->type == bcmFabricTdmEditingAppend || editing->type == bcmFabricTdmEditingCustomExternal) {
        
            if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST && editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid falgs: 0x%x unicast and multicast flags cant be toghther"),editing->flags));
            }
            
            if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST) {
                ftmh_info.is_mc = SOC_SAND_TRUE;
                /* Multicast destination */
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
                    ftmh_info.ftmh.opt_mc.mc_id = _BCM_MULTICAST_ID_GET(editing->multicast_id);
                } else {
                    ftmh_info.ftmh.standard_mc.mc_id = _BCM_MULTICAST_ID_GET(editing->multicast_id);
                }            
            } else if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
                /* Unicast destination */                            
                uint32  sys_port = 0;
                bcm_error_t rv;
                uint32 tm_port = -1;
    
                ftmh_info.is_mc = SOC_SAND_FALSE;

                if (BCM_GPORT_IS_SYSTEM_PORT(editing->destination_port)) {
                    rc = _bcm_dpp_gport_to_phy_port(unit, editing->destination_port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                    BCMDNX_IF_ERR_EXIT(rc);              
                    
                    sys_port = gport_info.sys_port;  
                } else {                    
                    /* Retreive directly from MODPORT or LOCAL */
                    if (BCM_GPORT_IS_MODPORT(editing->destination_port)) {
                        modid = BCM_GPORT_MODPORT_MODID_GET(editing->destination_port);
                        tm_port = BCM_GPORT_MODPORT_PORT_GET(editing->destination_port);
                    } else if (BCM_GPORT_IS_LOCAL(editing->destination_port)){
                        modid = my_mod;
                        local_port = BCM_GPORT_LOCAL_GET(editing->destination_port);                          
                    } else {
                        modid = my_mod;
                        local_port = editing->destination_port;
                    }

                    if (tm_port == -1) {
                        rv = soc_port_sw_db_local_to_tm_port_get(unit, local_port, &tm_port, &dest_core);
                        if(BCM_FAILURE(rv)) {   
                            /* In case TM port is not configured assume 1:1 mapping */
                            tm_port = local_port;
                        }
                        modid = SOC_DPP_CORE_TO_MODID(my_mod, dest_core);
                    }
                }

                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {                                  
                    ftmh_info.ftmh.opt_uc.dest_fap_id = modid;
                    ftmh_info.ftmh.opt_uc.dest_if = tm_port;                
                } else {                
                    ftmh_info.ftmh.standard_uc.sys_phy_port = sys_port;

                    ftmh_info.ftmh.standard_uc.dest_fap_port = tm_port;
                    ftmh_info.ftmh.standard_uc.dest_fap_id = modid;
                }
            }                            
        }
        
        /* 3. User define settings */
        /* Adding user define only if editing type is add */
        if (editing->type == bcmFabricTdmEditingAppend && SOC_IS_DPP_TDM_STANDARD(unit)) {
            if (editing->user_field_count > SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid max user define bits %d. max allowed %d"),
                                               editing->user_field_count,SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits));
            }

            if (editing->user_field_count % 8 != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid user define bits %d. must be in a multipies of 8"), editing->user_field_count));
            }

            if (editing->user_field_count != 0) {
                if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {  
                    nof_bytes = editing->user_field_count >> 3;
                    
                    for (index = 0; index < nof_bytes; index++) {
                        tmp = 0;
                        tmp = editing->user_field[index];
                        if (index < 4) {
                          soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            index*8,
                            (index+1)*8-1,
                            &(ftmh_info.ftmh.standard_uc.user_def)
                            );            
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }                        
                        else {
                          /* User define for more than 32bits. */
                          soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            (index-4)*8,
                            ((index-4)+1)*8-1,
                            &(ftmh_info.ftmh.standard_uc.user_def_2)
                            );            
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }
                } else {
                    nof_bytes = editing->user_field_count >> 3;
                    for (index = 0; index <  nof_bytes; index++) {
                        tmp = 0;
                        tmp = editing->user_field[index];
                        
                        soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            index*8,
                            (index+1)*8-1,
                            &(ftmh_info.ftmh.standard_mc.user_def)
                            );            
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
        }                      
    } else if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        /* 
         * Egress configuration includes: 
         * 1. Set Action Egress         
         */
        is_ingress = 0;
        rc = _bcm_petra_fabric_tdm_to_action_from_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);         


    } else {
        /* No ask for either egress or ingress configuration */
        BCM_EXIT;
    }


    /* Set tdm ftmh settings */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_set,(unit, core, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_editing_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_module_t modid;
    SOC_TMC_TDM_FTMH_INFO ftmh_info;
    uint32 soc_sand_rv;
    int is_ingress;
    int core;
    int index, nof_bytes;
    uint32 tmp, p_fap_port;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(editing);

    SOC_TMC_TDM_FTMH_INFO_clear(&ftmh_info);

    /* Check TDM is supported */
    if (!SOC_IS_DPP_TDM_OPTIMIZE(unit) && !SOC_IS_DPP_TDM_STANDARD(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM")));
    }

    /* Validate Gport, retreive port id */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Validate port is TDM port */
    if(!IS_TDM_PORT(unit, gport_info.local_port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
    }
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, gport_info.local_port, &p_fap_port, &core)));

    /*Set default user defined values to be zero*/
    for (index = 0; index <  BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE; index++) {
        editing->user_field[index] = 0;
    }
    editing->user_field_count = 0;

    /* Retrieve port TDM configuration */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_get,(unit, core, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* Get packet crc. (only if feature avaialable) */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass) { 
      uint8 is_ingress_enabled = 0, is_egress_enabled = 0;

      /* validate parameter */
      soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_port_packet_crc_get,(unit, gport_info.local_port, &is_ingress_enabled, &is_egress_enabled)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      editing->add_packet_crc = (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) ?
          ((editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS ? is_ingress_enabled : FALSE) |
           (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS ? is_egress_enabled : FALSE)) :
        (is_ingress_enabled & is_egress_enabled);
    }

    /* Get ingress configuration */
    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        is_ingress = 1;
        rc = _bcm_petra_fabric_tdm_from_action_to_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);

        /* Get destination only if ftmh is appended */
        if (ftmh_info.action_ing != SOC_TMC_TDM_ING_ACTION_NO_CHANGE) {
            if (ftmh_info.is_mc) {
                editing->flags |= BCM_FABRIC_TDM_EDITING_MULTICAST;
                /* Multicast destination */
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
                    _BCM_MULTICAST_GROUP_SET(editing->multicast_id,_BCM_PETRA_MULTICAST_TM_TYPE,ftmh_info.ftmh.opt_mc.mc_id);
                } else {
                    _BCM_MULTICAST_GROUP_SET(editing->multicast_id,_BCM_PETRA_MULTICAST_TM_TYPE,ftmh_info.ftmh.standard_mc.mc_id);
                }            
            } else {
                /* Unicast destination */
                editing->flags |= BCM_FABRIC_TDM_EDITING_UNICAST;
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
                    modid = ftmh_info.ftmh.opt_uc.dest_fap_id;
                    BCM_GPORT_MODPORT_SET(editing->destination_port,modid,ftmh_info.ftmh.opt_uc.dest_if);
                } else {
                    modid = ftmh_info.ftmh.standard_uc.dest_fap_id;
                    BCM_GPORT_MODPORT_SET(editing->destination_port,modid,ftmh_info.ftmh.standard_uc.dest_fap_port);
                }
            }
        }

        /* Get user defined only if action is add */        
        if (ftmh_info.action_ing == SOC_TMC_TDM_ING_ACTION_ADD && SOC_IS_DPP_TDM_STANDARD(unit)) {
            /* 3. User define settings */            
            /* Soc_petra-B can handle only 32 bits */
            editing->user_field_count = SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits;
            
            if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {  
                nof_bytes = editing->user_field_count >> 3;
                for (index = 0; index <  nof_bytes; index++) {
                    tmp = 0;
                    if (index < 4) {
                        soc_sand_rv = soc_sand_bitstream_get_any_field(
                            &(ftmh_info.ftmh.standard_uc.user_def),
                            index*8,
                            (index+1)*8-1,
                            &tmp
                            );            
                    }
                    else{/* User define for more than 32bits. */
                        soc_sand_rv = soc_sand_bitstream_get_any_field(
                            &(ftmh_info.ftmh.standard_uc.user_def_2),
                            (index-4)*8,
                            ((index-4)+1)*8-1,
                            &tmp
                            );     
                    }
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    editing->user_field[index] = tmp;
                }
            } else {
                nof_bytes = editing->user_field_count >> 3;
                for (index = 0; index <  nof_bytes; index++) {
                    tmp = 0;
                    
                    soc_sand_rv = soc_sand_bitstream_get_any_field(
                        &(ftmh_info.ftmh.standard_mc.user_def),
                        index*8,
                        (index+1)*8-1,
                        &tmp
                        );            
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    editing->user_field[index] = tmp;
                }
            }
        }
    } else if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        is_ingress = 0;
        rc = _bcm_petra_fabric_tdm_from_action_to_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        /* No ask for Ingress or Egress flags */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid flags parameters 0x%x, should includ ingress or egress flag"),editing->flags));
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_direct_routing_set(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rc;
    bcm_port_t link_i;
    bcm_port_t port_i;
    int index, old_link_ptr, is_last, new_link_ptr, is_allocated;
    SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing_information, cleaned_direct_routing_information;
    _bcm_dpp_gport_info_t gport_info;
    uint8 enable_rpt_reachable = TRUE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* The API is useful only if direct routing feature is avaliable. i.e. working in bypass mode */
    if (!(SOC_DPP_CONFIG(unit)->tdm.is_bypass)) { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only in bypass mode is supported")));
    }

    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing_information);
    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&cleaned_direct_routing_information);

    /* Verify routing information */
    BCMDNX_NULL_CHECK(routing_info);

    if (routing_info->links_count < 0 || routing_info->links_count > SOC_DPP_DEFS_GET(unit, nof_fabric_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid links count, links_count %d"), routing_info->links_count));
    }

    /* Verify links and set accordingly link bitmap */
    for (index = 0; index < routing_info->links_count; index++)
    {
      link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, routing_info->links[index]);
    
      if(!SOC_PORT_VALID_RANGE(unit, link_i))
      {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link 0x%x is out-of-range"), unit, routing_info->links[index]));
      }
      
      /* Set link_i in bitmap */
      soc_sand_rc = soc_sand_bitstream_set_bit(&(direct_routing_information.link_bitmap.arr[0]),link_i);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
    }

    /* setting a link count of zero implies no direct routing - hence seeting all links to all-ones */
    if(routing_info->links_count == 0) {
        direct_routing_information.link_bitmap.arr[0] = 0xffffffff;
        direct_routing_information.link_bitmap.arr[1] = 0xf;
    }

    /* Retreive Port from gport */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      /* Validate port is TDM port */
      if(!IS_TDM_PORT(unit, port_i)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
      }

      /* Set all ports with the same routing information */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_exchange(unit,port_i,&direct_routing_information,&old_link_ptr, &is_last, &new_link_ptr,&is_allocated);
      BCMDNX_IF_ERR_EXIT(rc);

      /* Commit Changes in HW according to routing information */
      if (is_allocated) {
          soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, new_link_ptr, &direct_routing_information, enable_rpt_reachable)));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
      }

      /* Set new mapping */
      soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_profile_map_set,(unit, port_i, new_link_ptr)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

      /* last mapped, remove routing information */
      if (is_last && (new_link_ptr != old_link_ptr)) {
          soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, old_link_ptr, &cleaned_direct_routing_information, enable_rpt_reachable)));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
      }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_direct_routing_get(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_port_t link_i;
    bcm_port_t port_i;
    SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing_information;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    /* The API is useful only if direct routing feature is avaliable */
    if (!(SOC_DPP_CONFIG(unit)->tdm.is_bypass)) { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only bypass mode is supported")));
    }

    BCMDNX_NULL_CHECK(routing_info);

    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing_information);

    /* Retreive Port from gport */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      /* Validate port is TDM port */
      if(!IS_TDM_PORT(unit, port_i)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
      }
      /* Retreive routing information from port */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_data_get(unit, port_i ,&direct_routing_information);
      BCMDNX_IF_ERR_EXIT(rc);

      /* Fill routing information, all valid links */
      routing_info->links_count = 0;
      PBMP_SFI_ITER(unit, port_i) {
        link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port_i);
        if (soc_sand_bitstream_test_bit(direct_routing_information.link_bitmap.arr,link_i)) {
          routing_info->links[routing_info->links_count++] = port_i;
        }
      }
      
      /* Retreive only first port */
      break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_fabric_tdm_range_size_set(int unit,
                                     int minimum_size,
                                     int maximum_size)
{
    uint32 soc_sand_rv = 0;    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_IS_DPP_TDM_STANDARD(unit) || !SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) { /* Standard or not fixed Optimize size (not Petra-B in system) */       
        SOC_SAND_U32_RANGE size_range;

        /* TDM standard validate size range */
        
        if (minimum_size < SOC_DPP_CONFIG(unit)->tdm.min_cell_size || maximum_size > SOC_DPP_CONFIG(unit)->tdm.max_cell_size
            || minimum_size > maximum_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid cell size range: Minimum size: %d, Maximum size: %d"),minimum_size,maximum_size));
        }

        /* TDM standard set size range*/
        size_range.start = minimum_size;
        size_range.end = maximum_size;

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_stand_size_range_set,(unit, &size_range)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
        /* TDM optimize */

        /* In Soc_petra-B cell size is fixed */
        
        if (SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) {
            uint32 cell_size;

            cell_size = minimum_size;

            /* TDM optimize validate size range */
            
            if (cell_size < SOC_DPP_CONFIG(unit)->tdm.min_cell_size ||  maximum_size > SOC_DPP_CONFIG(unit)->tdm.max_cell_size
                || minimum_size != maximum_size) {
                LOG_ERROR(BSL_LS_BCM_FABRIC,
                          (BSL_META_U(unit,
                                      "unit %d, invalid cell size: Minimum size: %d, Maximum size: %d \n"), unit,minimum_size,maximum_size));
            }
    
            /* TDM optimize set size range*/              
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_opt_size_set,(unit, cell_size)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {       
            /* Not TDM Mode */
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
        }
    } else {
       /* Not TDM Mode */
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
    }
        
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_range_size_get(int unit,
                                     int *minimum_size,
                                     int *maximum_size)
{
    uint32 soc_sand_rv = 0;    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(minimum_size);
    BCMDNX_NULL_CHECK(maximum_size);

    if (SOC_IS_DPP_TDM_STANDARD(unit) || !SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) { /* Standard or Not fixed Optimize size (not Petra-B in system) */  
        SOC_SAND_U32_RANGE size_range;
        /* TDM standard size get */

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_stand_size_range_get,(unit, &size_range)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        

        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *minimum_size = size_range.start;
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *maximum_size = size_range.end;
    } else if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
        /* TDM optimize size get */
        /* In Soc_petra-B cell size is fixed */
        
        if (SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) {
            uint32 cell_size = 0;
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_opt_size_get,(unit, &cell_size)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            *minimum_size = *maximum_size = cell_size;
        } else {            
           /* Not TDM Mode */
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
        }
    } else {
       /* Not TDM Mode */
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_from_action_to_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type)
{

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(editing_type);
   BCMDNX_NULL_CHECK(ftmh_info);

   if (is_ingress) {
       /* get ingress action */
       switch(ftmh_info->action_ing) {
            case SOC_TMC_TDM_ING_ACTION_ADD:
                *editing_type = bcmFabricTdmEditingAppend;
                break;
            case SOC_TMC_TDM_ING_ACTION_NO_CHANGE:
                *editing_type = bcmFabricTdmEditingNoChange;
                break;
            case SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED:
                *editing_type = bcmFabricTdmEditingCustomExternal;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported ingress action type %d"), ftmh_info->action_ing));
        }
   } else {
       /* get egress action */
       switch(ftmh_info->action_eg) {
            case SOC_TMC_TDM_EG_ACTION_REMOVE:
                *editing_type = bcmFabricTdmEditingRemove;
                break;
            case SOC_TMC_TDM_EG_ACTION_NO_CHANGE:
                *editing_type = bcmFabricTdmEditingNoChange;
                break;
            case SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT:
                *editing_type = bcmFabricTdmEditingCustomExternal;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported egress action type %d"), ftmh_info->action_eg));
        }       
   }
   
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_to_action_from_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type)
{

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(editing_type);
   BCMDNX_NULL_CHECK(ftmh_info);

   if (is_ingress) {
       /* get ingress action */
       switch(*editing_type) {
            case bcmFabricTdmEditingAppend:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_ADD;
                break;
            case bcmFabricTdmEditingNoChange:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_NO_CHANGE;
                break;
           case bcmFabricTdmEditingCustomExternal:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported ingress editing type %d"), *editing_type));
        }
   } else {
       /* get egress action */
       switch(*editing_type) {
            case bcmFabricTdmEditingRemove:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_REMOVE;
                break;
            case bcmFabricTdmEditingNoChange:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_NO_CHANGE;
                break;
            case bcmFabricTdmEditingCustomExternal:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported egress editing type %d"),*editing_type));
        }       
   }
   
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}
STATIC int 
_bcm_petra_fabric_vsq_category_set(int unit,
                                   int core_id,
                                   bcm_fabric_control_t control_type,
                                   int queue_id_in,
                                   int *queue_id_out)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int vsq_category_id;
    SOC_TMC_ITM_CATEGORY_RNGS info;
    SOC_SAND_U32_RANGE queue_range;
    int is_min = 0;
    int *queue_id = queue_id_out;
    bcm_cosq_control_range_type_t range_type;
    BCMDNX_INIT_FUNC_DEFS;

    *queue_id = queue_id_in;

    /* Clear */
    SOC_TMC_ITM_CATEGORY_RNGS_clear(&info);

    /* validate parameters */        
    if (*queue_id < 0 || *queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id given %d is out of range"),*queue_id));
    }
    switch (control_type) {
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricMulticastQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricMulticastQueueMax:
        vsq_category_id = 0;
        range_type = bcmCosqRangeMulticastQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricShaperQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricShaperQueueMax:
        vsq_category_id = 1;
        range_type = bcmCosqRangeShaperQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricQueueMax:
        vsq_category_id = 2;
        range_type = bcmCosqRangeFabricQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricRecycleQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricRecycleQueueMax:
        vsq_category_id = 3;
        range_type = bcmCosqRecycleQueue;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for vsq categories"),control_type));
    }


    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_get,(unit, core_id, &info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_DPP_UNIT_CHECK(unit);
    switch (vsq_category_id) {
    case 0:
        queue_range.start = 0;
        queue_range.end = info.vsq_ctgry0_end;
        break;
    case 1:
        queue_range.start = SOC_SAND_MIN(info.vsq_ctgry1_end, info.vsq_ctgry0_end + 1);
        queue_range.end = info.vsq_ctgry1_end; 
        break;           
    case 2:
        queue_range.start = SOC_SAND_MIN(info.vsq_ctgry2_end, info.vsq_ctgry1_end + 1);
        queue_range.end = info.vsq_ctgry2_end; 
        break;
    case 3:
        queue_range.start = SOC_SAND_MIN(DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit), info.vsq_ctgry2_end + 1);
        queue_range.end = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit); 
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("fabric control mode %d invalid vsq category id %d "), control_type,vsq_category_id));
    }

    if (is_min) {
        queue_range.start = *queue_id;
    } else {
        queue_range.end = *queue_id;
    }
    if (queue_range.end < queue_range.start) {
        queue_range.end = *queue_id;
    }

    rc = _bcm_cosq_petra_vsq_category_set(unit, core_id, range_type, &queue_range);
    BCMDNX_IF_ERR_EXIT(rc); 
    *queue_id = is_min ? queue_range.start :queue_range.end;
exit:
    BCMDNX_FUNC_RETURN;
}                                                                    
 
/* Get vsq categories */
STATIC int 
_bcm_petra_fabric_vsq_category_get(int unit,
                                   int core_id,
                                   bcm_fabric_control_t control_type,
                                   int *queue_id)
{
    int is_min = 0;
    bcm_error_t rc = BCM_E_NONE;
    SOC_SAND_U32_RANGE queue_range;
    bcm_cosq_control_range_type_t range_type;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    /* Clear */
    BCMDNX_NULL_CHECK(queue_id);

    switch (control_type) {
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricMulticastQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricMulticastQueueMax:
        range_type = bcmCosqRangeMulticastQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricShaperQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricShaperQueueMax:
        range_type = bcmCosqRangeShaperQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricQueueMax:
        range_type = bcmCosqRangeFabricQueue;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricRecycleQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricRecycleQueueMax:
        range_type = bcmCosqRecycleQueue;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for vsq categories"),control_type));
    }
    rc = _bcm_cosq_petra_vsq_category_get(unit, core_id, range_type, &queue_range);
    BCMDNX_IF_ERR_EXIT(rc);
    *queue_id = (int)(is_min ? queue_range.start : queue_range.end);
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_unicast_queue_range_get(int unit,
                                          int core_id,
                                          bcm_fabric_control_t control_type,
                                          int* queue_id)
{
    bcm_error_t rc;
    BCMDNX_INIT_FUNC_DEFS;
    /* Currently get from VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_get(unit, core_id, control_type, queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_get(int unit,
                                                 int core_id,
                                                 bcm_fabric_control_t control_type,
                                                 int* queue_id)
{
    SOC_TMC_ITM_INGRESS_SHAPE_INFO isp_info;
    uint32 soc_sand_rc;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(queue_id);    

    if (SOC_IS_QAX(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("control type %d unsupported on this device"), control_type));
    }

    /* Get ISQ range in hardware */
    SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(&isp_info);
    
    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_ingress_shape_get,(unit, core_id, &isp_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    if (control_type == bcmFabricShaperQueueMin) {
        *queue_id = isp_info.q_range.q_num_low;
    }

    if (control_type == bcmFabricShaperQueueMax) {
        *queue_id = isp_info.q_range.q_num_high;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  Purpose: set Egress queue ranges 
 *  Egress TM queues are defined Queues for the process of enable the user to
 *  have some sort of scheduling for egress TM queues.
 *  This is done by:
 *  1. Define egress queue ranges (see 4.).
 *  2. Define MC egress group for certain ports.
 *  3. Set recycle process for specific ports (in the MC egress group).  
 *  4. Set ingress queues to receive the packets that coming from recycle, so it enables for the user to decide on scheduling for these egress queues.
 */ 
STATIC int
_bcm_petra_fabric_egress_queue_range_set(int unit,
                                         int core_id,
                                          bcm_fabric_control_t type,
                                          int queue_id)
{
    bcm_error_t rc;

    BCMDNX_INIT_FUNC_DEFS;
    

    /* Currently set only VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_set(unit, core_id, type, queue_id, &queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_petra_fabric_egress_queue_range_get(int unit,
                                         int core_id,
                                         bcm_fabric_control_t control_type,
                                         int* queue_id)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    /* Currently get from VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_get(unit, core_id, control_type, queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* System red */
/* 
 * set discard configuration. Configuration to enable, disable aging and
 * its settings
 */
int bcm_petra_fabric_config_discard_set(
    int unit, 
    bcm_fabric_config_discard_t *discard)
{

    SOC_TMC_ITM_SYS_RED_EG_INFO info, exact_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_TMC_ITM_SYS_RED_EG_INFO_clear(&info);
    SOC_TMC_ITM_SYS_RED_EG_INFO_clear(&exact_info);

    info.enable = discard->enable;
    info.aging_timer = discard->age;
    if (discard->age_mode == bcmFabricAgeModeReset) {
        info.reset_expired_q_size = 0x1;
    } 
    if (discard->age_mode == bcmFabricAgeModeDecrement) {
        info.aging_only_dec_q_size = 0x1;
    }

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_sys_red_eg_set,(unit, &info, &exact_info)));

exit:
  BCMDNX_FUNC_RETURN;
}

/* get discard configuration. */
int bcm_petra_fabric_config_discard_get(
    int unit, 
    bcm_fabric_config_discard_t *discard)
{
    SOC_TMC_ITM_SYS_RED_EG_INFO info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&info, 0x0, sizeof(info));

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_sys_red_eg_get,(unit, &info)));

    discard->enable = info.enable;
    discard->age = info.aging_timer;
    if (info.reset_expired_q_size == 0x1) {
        discard->age_mode = bcmFabricAgeModeReset;
    } 
    if (info.aging_only_dec_q_size == 0x1) {
        discard->age_mode = bcmFabricAgeModeDecrement;
    }

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_link_control_set_llfc_control_source(int unit, bcm_port_t link, int arg)
{
    uint32 soc_sand_rv, mask[1];
    bcm_port_t link_i;
    int nof_pipes;
    BCMDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DPP_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);
    
    BCM_DPP_UNIT_CHECK(unit);
    if(!SOC_PORT_VALID_RANGE(unit, link_i))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
    }

    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(mask, nof_pipes);

    /*validation*/
    if ( (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_CLEAR(arg))) &&  (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_SET(arg, *mask))) ) /* if enable, then all bits must be on */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("LLFC must be for all pipes")));
    }

    /*set registers*/
    soc_sand_rv = arad_fabric_links_llf_control_source_set(unit, link_i, arg);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int

_bcm_petra_fabric_link_repeater_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv;
    uint32 burst, pps;
    int speed, empty_cell_size;
    uint64 empty_cell_size64, pps64;
    BCMDNX_INIT_FUNC_DEFS;

    if (enable)
    {
        /* 
         * Repeater link enable -
         *  if cell shaper is enabled set empty cell size accordingly.                      -
         *  Else set to repeater link default cell size.                                                                                -
         */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_rate_egress_pps_get,(unit, port, &pps, &burst)));

        rv = bcm_petra_port_speed_get(unit, port, &speed);
        BCMDNX_IF_ERR_EXIT(rv);

        if (pps == 0)
        {
            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 1, SOC_DPP_DEFS_GET(unit, repeater_default_empty_cell_size)));
            BCMDNX_IF_ERR_EXIT(rv); 
        } else {
            /*empty_cell_size [bytes/cell] = speed [bytes/sec] / pps [cells/sec]*/
            COMPILER_64_SET(empty_cell_size64, 0, speed);
            COMPILER_64_UMUL_32(empty_cell_size64, (1000000 / 8)); /*Mega-bits to bytes*/

            COMPILER_64_SET(pps64, 0, pps);
            COMPILER_64_UDIV_64(empty_cell_size64, pps64); 

            empty_cell_size = COMPILER_64_LO(empty_cell_size64);

            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 1, empty_cell_size));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    } else {
        /*Disable repeater link and reset empty cell size*/
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 0, SOC_DPP_DEFS_GET(unit, repeater_none_empty_cell_size)));
        BCMDNX_IF_ERR_EXIT(rv);           
    }
   
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_link_repeater_enable_get(int unit, bcm_port_t port, int *enable)
{
    int rv;
    int empty_cell_size;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_get,(unit, port, enable, &empty_cell_size));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:   
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_fabric_link_control_set
 * Purpose:
 *      Set fabric-link attribute (fifo ype) per link. Each link might
 *      have two fifo types; one per RX and one per TX
 * Parameters:
 *      unit - (IN) Unit number.
 *      link - (IN) Link Number 
 *      type - (IN) Control type
 *      arg  - (IN) Value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_fabric_link_control_set(
                                int unit, 
                                bcm_port_t link, 
                                bcm_fabric_link_control_t type, 
                                int arg)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_rv;
    bcm_port_t link_i;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /*General validation*/
    /*DPP_LINK_INPUT_CHECK(unit, link);*/

    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);

    BCM_DPP_UNIT_CHECK(unit);
    if(!SOC_PORT_VALID_RANGE(unit, link_i))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
    }
    if(arg!=0 && arg!=1 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported arg %d"), arg));
    }
    switch(type)
    {
        case bcmFabricLinkCellInterleavingEnable:
            if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported function in Jericho")));
            } else {
                soc_rv = arad_fabric_links_cell_interleaving_set(unit, link_i, arg ? 1 : 0); 
                BCMDNX_IF_ERR_EXIT(soc_rv);
            }
            break;
        case bcmFabricLLFControlSource:
            rc = _bcm_petra_fabric_link_control_set_llfc_control_source(unit, link, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;       
        case bcmFabricLinkIsolate:
            soc_rv =  arad_fabric_links_isolate_set(unit, link_i, arg ? soc_dcmn_isolation_status_isolated : soc_dcmn_isolation_status_active);
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        case bcmFabricLinkTxTrafficDisable:
        {
            int link_traffic_disable = arg ? 1 : 0;
            int is_port_enabled = 0;

            /* Update sw_state */
            if (link_traffic_disable) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.bit_clear(unit, link_i));
            } else {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.bit_set(unit, link_i));
            }

            /* 
             * Update HW only if port is enabled. If port is currently
             * disabled, HW will be updated according to sw_state when
             * port will be enabled again.
             */
            SOCDNX_IF_ERR_EXIT(bcm_petra_port_enable_get(unit, link, &is_port_enabled));
            if (is_port_enabled) {
                soc_rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_link_tx_traffic_disable_set, (unit, link_i, link_traffic_disable));
                BCMDNX_IF_ERR_EXIT(soc_rv);
            }
            break;
        }
        case bcmFabricLinkRepeaterEnable:
            soc_rv = _bcm_petra_fabric_link_repeater_enable_set(unit, link, arg);
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break; 
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
    }  
exit: 
    BCMDNX_FUNC_RETURN; 
}
/*
* Function:
*      bcm_petra_fabric_link_control_get
* Purpose:
*       Get fabric-link attribute (fifo type) per link and direction
* Parameters:
*      unit - (IN)  Unit number.
*      link - (IN)  Link Number 
*      type - (IN)  Control type 
*      arg  - (OUT) Value
* Returns:
*      BCM_E_xxx
* Notes:
*/
int 
bcm_petra_fabric_link_control_get(
                                int unit, 
                                bcm_port_t link, 
                                bcm_fabric_link_control_t type, 
                                int *arg)
{
    uint32 soc_rv;
    bcm_port_t link_i;
    BCMDNX_INIT_FUNC_DEFS;

    /*General validation*/
    BCMDNX_NULL_CHECK(arg);
    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);

    BCM_DPP_UNIT_CHECK(unit);
    if(!SOC_PORT_VALID_RANGE(unit, link_i))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
    }

    switch(type)
    {
        case bcmFabricLinkCellInterleavingEnable:
            soc_rv = arad_fabric_links_cell_interleaving_get(unit, link_i, arg); 
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        case bcmFabricLLFControlSource:
            soc_rv = arad_fabric_links_llf_control_source_get(unit, link_i, (soc_dcmn_fabric_control_source_t*)arg);
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;   
        case bcmFabricLinkIsolate:
            soc_rv = arad_fabric_links_isolate_get(unit, link_i, (soc_dcmn_isolation_status_t*)arg); 
            BCMDNX_IF_ERR_EXIT(soc_rv);
            break;
        case bcmFabricLinkTxTrafficDisable:
        {
            int is_fabric_link_allowed = 0;

            /*
             * The status is taken from sw_state and not from HW since
             * traffic can be conceptualy enabled even though the port
             * is currently disabled.
             */
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_fabric_link_allowed.bit_get(unit, link_i, (uint8*)&is_fabric_link_allowed));
            *arg = !is_fabric_link_allowed;
            break;
        }
        case bcmFabricLinkRepeaterEnable:
           soc_rv = _bcm_petra_fabric_link_repeater_enable_get(unit, link, arg);
           BCMDNX_IF_ERR_EXIT(soc_rv);
           break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
    }  
exit: 
    BCMDNX_FUNC_RETURN;    
}

/*
 * Function:
 *      bcm_petra_fabric_reachability_status_get
 * Purpose:
 *      Retrieves the links through which a remote module ID is
 *      reachable
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      moduleid    - (IN)  Module Id 
 *      links_max   - (IN)  Max size of links_array. If the size doesn't correlate to the device's number of links, 
                            the function returns with the error BCM_E_FULL
 *      links_array - (OUT) bitmap of the links from which 'moduleid' is reachable 
 *      links_count - (OUT) Size of links_array 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      No support for Soc_petra-B.  
 */
int 
bcm_petra_fabric_reachability_status_get(
    int unit, 
    int moduleid, 
    int links_max, 
    uint32 *links_array, 
    int *links_count)
{    
    uint32 soc_rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);
    BCMDNX_NULL_CHECK(links_count);

    soc_rv = soc_dpp_fabric_reachability_status_get(unit, moduleid, links_max, links_array, links_count);  
    BCMDNX_IF_ERR_EXIT(soc_rv);
exit:     
    BCMDNX_FUNC_RETURN; 
}

 /*
  * Credit watchdog:
  *  1. retransmit flow control messages for active queues that haven't get Credits from
  *     schedule after some time.
  *  2. free "stuck" queues after time threshold to keep DRAM resources.
  */

/*
 * Set queue range or enable/disable for credit watchdog
 * Set VOQ range
 */
STATIC int
_bcm_petra_fabric_credit_watchdog_range_set(int unit,
                      bcm_fabric_control_t control_type,
                      int queue_id,
                      int common_message_time) /* if >= 0, this is the new common message time, for the common message time mode */
{
    uint32 flags = 0;
    bcm_cosq_range_t range;
    BCMDNX_INIT_FUNC_DEFS;
    sal_memset(&range, 0x0, sizeof(bcm_cosq_range_t));

    BCM_DPP_UNIT_CHECK(unit);
    flags |= (control_type == bcmFabricWatchdogQueueEnable) ? BCM_PETRA_COSQ_CREDIT_WATHDOG_USE_ENABLE: 0;
    flags |= (control_type == bcmFabricWatchdogQueueMin) ? BCM_PETRA_COSQ_CREDIT_WATHDOG_USE_MIN: 0;
    flags |= (control_type == bcmFabricWatchdogQueueMax) ? BCM_PETRA_COSQ_CREDIT_WATHDOG_USE_MAX: 0;

    if (control_type == bcmFabricWatchdogQueueEnable) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        range.is_enabled = queue_id;
    } else if (control_type == bcmFabricWatchdogQueueMin) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        range.range_start = queue_id;
    } else if (control_type == bcmFabricWatchdogQueueMax) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        range.range_end = queue_id;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong control_type argument")));
    }
    BCM_SAND_IF_ERR_EXIT(_bcm_petra_cosq_credit_watchdog_range_set(unit, SOC_CORE_ALL, flags, &range, common_message_time));

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Get queue range or enable/disable for credit watchdog
 * Set VOQ range
 */
STATIC int
_bcm_petra_fabric_credit_watchdog_range_get(int unit,
                      bcm_fabric_control_t control_type,
                      int* queue_id)
{
    bcm_cosq_range_t range;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(queue_id);       

    sal_memset(&range, 0x0, sizeof(bcm_cosq_range_t));
    BCM_SAND_IF_ERR_EXIT(_bcm_petra_cosq_credit_watchdog_range_get(unit, BCM_CORE_ALL, &range));
    if (control_type == bcmFabricWatchdogQueueEnable) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = range.is_enabled;
    } else if (control_type == bcmFabricWatchdogQueueMin) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = range.range_start;
    } else if (control_type == bcmFabricWatchdogQueueMax) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = range.range_end;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong control_type argument")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_priority_set(
    int unit,
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color, 
    int fabric_priority)
{
    BCMDNX_INIT_FUNC_DEFS;

    /*arguments check*/
    BCM_DPP_UNIT_CHECK(unit);

    if (ingress_pri < 0 || ingress_pri >= DPP_DEVICE_COSQ_ING_NOF_TC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_pri parameter %d"), ingress_pri));
    }
    if (color < 0 || color >= DPP_DEVICE_COSQ_ING_NOF_DP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_dp parameter %d"), color));
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags includes both  BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY")));
    }

    unit = (unit);
    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0) {
        /*set both hp and lp*/
        uint32 flags_high, flags_low;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags_high, fabric_priority)));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags_low, fabric_priority)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags, fabric_priority)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_priority_get(
    int unit,
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color,  
    int *fabric_priority)
{
    BCMDNX_INIT_FUNC_DEFS;

    /*arguments check*/
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(fabric_priority);

    if (ingress_pri < 0 || ingress_pri >= DPP_DEVICE_COSQ_ING_NOF_TC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_pri parameter %d"), ingress_pri));
    }
    if (color < 0 || color >= DPP_DEVICE_COSQ_ING_NOF_DP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_dp parameter %d"), color));
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags includes both  BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY")));
    }

    unit = (unit);
    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0) {
        /*set both hp and lp*/
        uint32 flags_high, flags_low;
        int fabric_priority_low = 0, fabric_priority_high = 0;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags_high, &fabric_priority_low)));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags_low, &fabric_priority_high)));

        if (fabric_priority_low != fabric_priority_high) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("fabric priority is different for QUEUE_PRIORITY_LOW and QUEUE_PRIORITY_HIGH. use flags BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY or BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY.")));
        }
        *fabric_priority = fabric_priority_high;
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags, fabric_priority)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_thresholds_set(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    uint32 soc_sand_rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (fifo_type == -1)
    {
        /*special functionality - no fifo_type required*/

        for(i = 0 ; i<count; i++)
        {
            switch(type[i])
            {
                /*
                 * GCI Backoff related thresholds configuraion
                 */
                case bcmFabricLinkTxGciLvl1FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl2FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl3FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /*
                 * GCI Leaky bucket thresholds
                 */
               case bcmFabricLinkGciLeakyBucket1Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

               case bcmFabricLinkGciLeakyBucket1Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;


                /*
                 * Link level thresholds configuration
                 */
                case bcmFabricLinkRxFifoLLFC:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_llfc_threshold_set,(unit, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /*
                 * Local RCI thresholds
                 */

               case bcmFabricLinkRciFC:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_set, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH,value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;


               default:
                    BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_link_thresholds_pipe_set(unit,fifo_type,socDppFabricPipeAll,0,1,(&type[i]),(&value[i])));
                    break;
                
            }
         }
        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Fifo type not supported: %d"), fifo_type));
    }



exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_thresholds_get(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    uint32 soc_sand_rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (fifo_type == -1)
    {
        /*special functionality - no fifo_type required*/


        for(i = 0 ; i<count; i++)
        {
            switch(type[i])
            {
                /*
                 * GCI Backoff related thresholds configuraion
                 */
                case bcmFabricLinkTxGciLvl1FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl2FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl3FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
               /*
                * GCI Leaky bucket thresholds
                */
               case bcmFabricLinkGciLeakyBucket1Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

               case bcmFabricLinkGciLeakyBucket1Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

                /*
                 * Link level thresholds configuration
                 */
                case bcmFabricLinkRxFifoLLFC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_llfc_threshold_get,(unit, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /* 
                 *Local RCI thresholds
                 */
                case bcmFabricLinkRciFC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_get, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

            default:
                    BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_link_thresholds_pipe_get(unit,fifo_type,socDppFabricPipeAll,0,1,(&type[i]),(&value[i])));
                    break;
            }
         }
        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Fifo type not supported: %d"), fifo_type));
    }



exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_fabric_link_thresholds_pipe_set(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    soc_dpp_fabric_pipe_t soc_pipe;
    int i;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    if (pipe == bcmFabricPipe0) {
        soc_pipe = socDppFabricPipe0;
    } else if (pipe == bcmFabricPipe1) {
        soc_pipe = socDppFabricPipe1;
    } else if (pipe == bcmFabricPipe2) {
        soc_pipe = socDppFabricPipe2;
    } else {
        soc_pipe = socDppFabricPipeAll;
    }

    for (i = 0; i < count; ++i) {
        switch (type[i]) {
        case bcmFabricLinkRxFifoLLFC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxFifoLLFC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl1FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl1FC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl2FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl2FC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl3FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl3FC, value[i])));
                break;
        default:
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("threshold type is not supported")));
                break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_fabric_link_thresholds_pipe_get(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    soc_dpp_fabric_pipe_t soc_pipe;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    if (pipe == bcmFabricPipe0) {
        soc_pipe = socDppFabricPipe0;
    } else if (pipe == bcmFabricPipe1) {
        soc_pipe = socDppFabricPipe1;
    } else if (pipe == bcmFabricPipe2) {
        soc_pipe = socDppFabricPipe2;
    } else {
        soc_pipe = socDppFabricPipeAll;
    }

    for (i = 0; i < count; ++i) {
        switch (type[i]) {
        case bcmFabricLinkRxFifoLLFC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxFifoLLFC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl1FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl1FC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl2FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl2FC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl3FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl3FC, value+i)));
                break;
        default:
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("threshold type is not supported")));
                break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * 
 * Function:
 *      bcm_fabric_route_rx
 * Purpose:
 *      Receive Source routed data cells
 * Parameters:
 *      unit                (IN) Unit number.
 *      flags               (IN) flags
 *      data_out_max_size   (IN) max "data_out" size
 *      data_out            (OUT) the received data.
 *      data_out_size       (OUT) "data_out" actual size.
 */
int 
bcm_petra_fabric_route_rx(
    int unit, 
    uint32 flags, 
    uint32 data_out_max_size, 
    uint32 *data_out, 
    uint32 *data_out_size)
{
    uint32 data_out_size_in_bytes;
    BCMDNX_INIT_FUNC_DEFS;

    /*Verify*/
    BCMDNX_NULL_CHECK(data_out);
    BCMDNX_NULL_CHECK(data_out_size);

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    if (data_out_max_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("buffer is too small")));
    }

    
    BCMDNX_IF_ERR_EXIT(soc_dpp_fabric_sr_cell_receive(unit, flags, (data_out_max_size * 4), &data_out_size_in_bytes, data_out));
    if(data_out_size_in_bytes % 4)
    {
      /* There is another data in no more then 3 bytes, which don't fits whole 32-bits unit */
      *data_out_size = (data_out_size_in_bytes / 4) + 1;
    }
    else
    {
      *data_out_size = data_out_size_in_bytes / 4;
    }

exit: 
    BCMDNX_FUNC_RETURN;
}
/*
 * 
 * Function:
 *      bcm_fabric_route_tx
 * Purpose:
 *      Send Source routed data cells
 * Parameters:
 *      unit         (IN) Unit number.
 *      flags        (IN) flags
 *      route        (IN) Specify the path for the generated cells.
 *      data_in_size (IN) "data_in" size (uint32 units).
 *      data_in      (IN) The data to send.
 */
int 
bcm_petra_fabric_route_tx(
    int unit, 
    uint32 flags, 
    bcm_fabric_route_t *route, 
    uint32 data_in_size, 
    uint32 *data_in)
{
    soc_dcmn_fabric_route_t soc_route;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    BCMDNX_NULL_CHECK(route);
    BCMDNX_NULL_CHECK(route->hop_ids);
    BCMDNX_NULL_CHECK(data_in);

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    if (data_in_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("no data to send")));
    }

    soc_route.number_of_hops = route->number_of_hops;
    if (soc_route.number_of_hops > FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("number_of_hops is too long\n")));
    }
    soc_route.hop_ids = route->hop_ids;
    
    BCMDNX_IF_ERR_EXIT(soc_dpp_fabric_sr_cell_send(unit, flags, &soc_route, sizeof(uint32) * data_in_size, data_in));

exit: 
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_pcp_dest_mode_config_set(
    int 				unit, 
    uint32  			flags, 
    bcm_module_t 			modid, 
    bcm_fabric_pcp_mode_config_t  	*pcp_config)
{
    ARAD_INIT_FABRIC *fabric = NULL;
    BCMDNX_INIT_FUNC_DEFS;
    
    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric);   
    if (fabric->fabric_pcp_enable) {
        if (pcp_config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_config argument is invalid")));
        }
        if (!(BCM_FABRIC_MODID_IS_VALID(unit, modid))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("modid argument is invalid")));
        }
        if (!(BCM_FABRIC_PCP_MODE_IS_VALID(pcp_config->pcp_mode))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_mode is invalid")));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_pcp_dest_mode_config_set, 
                                                 (unit, flags, modid, pcp_config->pcp_mode)));

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,(_BSL_BCM_MSG("fabric_pcp is disabled")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_pcp_dest_mode_config_get(
    int 				unit, 
    uint32  			flags, 
    bcm_module_t 			modid, 
    bcm_fabric_pcp_mode_config_t  	*pcp_config)
{
    ARAD_INIT_FABRIC *fabric = NULL;
    uint32 pcp_mode = 0;
    BCMDNX_INIT_FUNC_DEFS;
    
    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric); 
    if (fabric->fabric_pcp_enable) {
        if (pcp_config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_config argument is invalid")));
        }
        if (!(BCM_FABRIC_MODID_IS_VALID(unit, modid))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("modid argument is invalid")));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_pcp_dest_mode_config_get, 
                                                 (unit, flags, modid, &pcp_mode)));
        pcp_config->pcp_mode = pcp_mode;
         
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("fabric_pcp is disabled")));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_destination_link_min_set(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int num_of_links)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("number of links is invalid")));
    }
    if (SOC_DPP_IS_MESH(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("minimum number of links configuration not supported in mesh")));
    }
    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_all_reachable_set, 
                                                 (unit, num_of_links)));
    } else {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("module id is invalid")));
        }
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_to_dest_set, 
                                                     (unit, module_id, num_of_links)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_destination_link_min_get(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int* num_of_links)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(num_of_links);

    if (SOC_DPP_IS_MESH(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("minimum number of links configuration not supported in mesh")));
    }

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_all_reachable_get, 
                                                 (unit, num_of_links)));
    } else {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("module id is invalid")));
        }
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_to_dest_get, 
                                                 (unit, module_id, num_of_links)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_topology_set(
    int unit, 
    bcm_module_t destination, 
    int links_count, 
    bcm_port_t *links_array)
{
    int nof_fabric_links, is_mesh, i;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate links_count */
    if(links_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("links_count should be 0 and up"))); 
    }
    nof_fabric_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
    if(links_count > nof_fabric_links) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("links_count value too big"))); 
    }
    /* validate links in links_array are valid */
    for (i = 0; i < links_count; ++i) {
        if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), links_array[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("link %d is invalid"), links_array[i])); 
        }
    }
    /* validate destination */
    if (!BCM_FABRIC_MODID_IS_GROUP(destination)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));
    if (links_count == 0) { 
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_unset, 
                                                     (unit, local_dest_id)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_set, 
                                                     (unit, local_dest_id, links_count, links_array)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_topology_get(
    int unit, 
    bcm_module_t destination, 
    int max_links_count, 
    int *links_count, 
    bcm_port_t *links_array)
{
    int is_mesh;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);
    BCMDNX_NULL_CHECK(links_count);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate max_links_count */
    if(max_links_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal max_links_count"))); 
    }
    /*validate destination */
    if (!BCM_FABRIC_MODID_IS_GROUP(destination)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_get, 
                                                 (unit, local_dest_id, max_links_count, links_count, links_array)));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_multicast_set(
    int unit, 
    bcm_multicast_t mc_id, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array)
{
    int i;
    int is_mesh_mc;
    short max_mesh_replication;
    uint32 my_core0_fap_id, my_core1_fap_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(destid_array);

    /* validate mc_id (0 to fabric_mesh_mc_id_max_valid)*/
    if (!BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal mc_id"))); 
    }
    /* validate destid count */
    max_mesh_replication = ((SOC_DPP_DEFS_GET(unit, fabric_max_dest_mesh_mc_mode)) + (SOC_DPP_DEFS_GET(unit, nof_cores))); /*ejacobi - changing the behavior for Petra, ARAD and Ardon.*/

    if(destid_count > max_mesh_replication) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("too many destinations"))); 
    }
    /* validate destinatins in dest_array */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &my_core0_fap_id)));
    my_core1_fap_id = my_core0_fap_id +
                      ((SOC_DPP_DEFS_GET(unit, nof_cores) == 2) ? 1 : 0); /*1 for 2 cores and 0 for single core*/
    for (i = 0; i < destid_count; ++i) {
        if ((!SOC_DPP_FABRIC_MODID_IS_GROUP(destid_array[i]))
            && (destid_array[i] != my_core0_fap_id)
            && (destid_array[i] != my_core1_fap_id)) /*If nof_cores is 1 this condition is the same as the one for core0*/
        {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("invalid dest_id %d"), destid_array[i]));
        }
    }

    /* vaidate MESH_MC mode */
    is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);

    if (is_mesh_mc) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_multicast_set,
                                                 (unit, mc_id, destid_count, destid_array)));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_multicast_set is supported in MESH_MC mode only")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_multicast_get(
    int unit, 
    bcm_multicast_t mc_id, 
    uint32 flags, 
    int destid_count_max, 
    int *destid_count, 
    bcm_module_t *destid_array)
{
    int is_mesh_mc;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(destid_array);
    BCMDNX_NULL_CHECK(destid_count);

    /* validate mc_id (0 to fabric_mesh_mc_id_max_valid)*/
    if (!BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal mc_id"))); 
    }
    /* validate destid_count_max */
    if(destid_count_max <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal destid_count_max"))); 
    }
    /* validate MESH_MC mode */
    is_mesh_mc = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 1);

    if (is_mesh_mc) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_multicast_get, 
                                                 (unit, mc_id, destid_count_max, destid_count, destid_array)));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_multicast_get is supported in MESH_MC mode only")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_modid_group_set(
    int unit, 
    bcm_module_t group, 
    int modid_count, 
    bcm_module_t *modid_array)
{
    int is_mesh, i;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(modid_array);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate modid_count */
    if(modid_count <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("modid_count should be greater than 0"))); 
    }
    if (modid_count > SOC_DPP_DEFS_GET(unit, max_nof_fap_id_mapped_to_dest_limit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("only %d fAPs can be mapped to a single destination"), SOC_DPP_DEFS_GET(unit, max_nof_fap_id_mapped_to_dest_limit)));
    }
    /* validate faps in array are valid */
    for (i = 0; i < modid_count; ++i) {
        if (!BCM_FABRIC_MODID_IS_VALID(unit, modid_array[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fap id %d invalid"), modid_array[i])); 
        }
    }
    /* validate group */
    if (!BCM_FABRIC_MODID_IS_GROUP(group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);

    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_set, 
                                                 (unit, local_dest_id, modid_count, modid_array)));
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_modid_group_get(
    int unit, 
    bcm_module_t group, 
    int modid_max_count, 
    bcm_module_t *modid_array, 
    int *modid_count)
{
    int is_mesh;
    soc_module_t local_dest;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(modid_array);
    BCMDNX_NULL_CHECK(modid_count);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate modid_max_count */
    if(modid_max_count <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal modid_max_count"))); 
    }
    
    /* validate group */
    if (!BCM_FABRIC_MODID_IS_GROUP(group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest = BCM_FABRIC_GROUP_MODID_UNSET(group);

    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_get, 
                                                 (unit, local_dest, modid_max_count, modid_array, modid_count)));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_rci_config_set(
      int unit,
      bcm_fabric_rci_config_t rci_config)
{
    int i;
    soc_dpp_fabric_rci_config_t soc_rci_config;
    BCMDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    /*validate struct fields are valid and fill soc-layer struct*/
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
        if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
            if (!BCM_FABRIC_RCI_THRESHOLD_IS_VALID(rci_config.rci_core_level_thresholds[i])) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("threshold %d is invalid"), rci_config.rci_core_level_thresholds[i]));
            }
        } else { /*Single core*/
            if (rci_config.rci_core_level_thresholds[i] != 0) {
                LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit,"Threshold %d is invalid: For single core device, rci_core_level_thresholds has no meaning, therefore, the convention is to use value of 0\n"), rci_config.rci_core_level_thresholds[i]));
            }
        }
        if (!BCM_FABRIC_RCI_THRESHOLD_IS_VALID(rci_config.rci_device_level_thresholds[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("threshold %d is invalid"), rci_config.rci_device_level_thresholds[i]));
        }
        soc_rci_config.soc_dpp_fabric_rci_core_level_thresholds[i] = rci_config.rci_core_level_thresholds[i];
        soc_rci_config.soc_dpp_fabric_rci_device_level_thresholds[i] = rci_config.rci_device_level_thresholds[i];
    }
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_SEVERITIES; ++i) {
        if (!BCM_FABRIC_RCI_SEVERITY_FACTOR_IS_VALID(rci_config.rci_severity_factors[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("severity factor %d is invalid"), rci_config.rci_severity_factors[i]));
        }
        soc_rci_config.soc_dpp_fabric_rci_severity_factors[i] = rci_config.rci_severity_factors[i];
    }

    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_fabric_rx_queue)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_fabric_rx_queue is invalid")));
    }
    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_fabric_rx_local_queue)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_fabric_rx_local_queue is invalid")));
    }
    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_congested_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_congested_links is invalid")));
    }
    if (!BCM_FABRIC_RCI_CONGESTED_LINKS_THRESHOLD_IS_VALID(rci_config.rci_threshold_num_of_congested_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_threshold_num_of_congested_links is invalid")));
    }
    
    soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue = rci_config.rci_high_score_fabric_rx_queue;
    soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue = rci_config.rci_high_score_fabric_rx_local_queue;
    soc_rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links = rci_config.rci_threshold_num_of_congested_links;
    soc_rci_config.soc_dpp_fabric_rci_high_score_congested_links = rci_config.rci_high_score_congested_links;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_thresholds_config_set, (unit, soc_rci_config)));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_rci_config_get(
      int unit,
      bcm_fabric_rci_config_t* rci_config)
{
    int i;
    soc_dpp_fabric_rci_config_t soc_rci_config;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(rci_config);

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    /*initialize struct to invalid values*/
    bcm_fabric_rci_config_t_init(rci_config);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_thresholds_config_get, (unit, &soc_rci_config)));

    /*fill rci_config struct with fields*/
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
        rci_config->rci_core_level_thresholds[i] = soc_rci_config.soc_dpp_fabric_rci_core_level_thresholds[i];
        rci_config->rci_device_level_thresholds[i]= soc_rci_config.soc_dpp_fabric_rci_device_level_thresholds[i];
    }
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_SEVERITIES; ++i) {
        rci_config->rci_severity_factors[i] = soc_rci_config.soc_dpp_fabric_rci_severity_factors[i];
    }
    rci_config->rci_high_score_fabric_rx_queue = soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue;
    rci_config->rci_high_score_fabric_rx_local_queue = soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue;
    rci_config->rci_threshold_num_of_congested_links = soc_rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links;
    rci_config->rci_high_score_congested_links = soc_rci_config.soc_dpp_fabric_rci_high_score_congested_links;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_module_control_set(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int value)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    switch (control) {
        /* turn on/off NFMS feature per-device  */
        case bcmFabricModuleControlFSMSeqNumberEnable:
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_efms_enable_set, (unit, (uint32)modid, value)));
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for fabric module control"),control));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_module_control_get(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int *value)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (soc_feature(unit, soc_feature_no_fabric)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    switch (control) {
        /* turn on/off NFMS feature per-device  */
        case bcmFabricModuleControlFSMSeqNumberEnable:
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_efms_enable_get, (unit, (uint32)modid, value)));
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for fabric module control"),control));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_static_replication_set(
    int unit, 
    bcm_port_t port, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array)
{
    int is_mesh_mc_using_ire;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(destid_array);

    if (!IS_TDM_PORT(unit, port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port=%d not a TDM port "), port));
    }
    
    is_mesh_mc_using_ire = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable == 2);

    if (is_mesh_mc_using_ire) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_static_replication_set, (unit, port, flags, destid_count, destid_array)));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_static_replication_set is supported in TDM Mesh MC mode only")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
