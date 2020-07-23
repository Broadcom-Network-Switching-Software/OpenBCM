/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        switch.c
 * Purpose:     BCM definitions for bcm_switch_control
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_SWITCH

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/drv.h>

#include <soc/dcmn/dcmn_wb.h>

#include <bcm/switch.h>
#include <bcm/error.h>
#include <bcm/debug.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/extender.h>

#include <shared/swstate/sw_state.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_llp_cos.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/ARAD/arad_parser.h>
#ifdef BCM_88660
# include <soc/dpp/PPD/ppd_api_eg_qos.h>
# include <soc/dpp/PPD/ppd_api_slb.h>
#endif
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/TMC/tmc_api_stack.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <shared/shr_template.h>

#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_api_cnm.h>
#include <soc/dpp/ARAD/arad_api_cnt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/shared/llm_appl.h>
#include <soc/shared/llm_msg.h>
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_JERICHO_SUPPORT
#include <bcm_int/common/sat.h>
#include <soc/shared/sat.h>
#endif
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_mgmt.h>
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/
/***************************************************************/
/***************************************************************/


/* 
 * Local defines
 */
#define DPP_SWITCH_MSG(string)   "%s[%d]: " string, __FILE__, __LINE__

#define DPP_SWITCH_UNIT_VALID_CHECK \
    if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
       LOG_ERROR(BSL_LS_BCM_SWITCH, \
                 (BSL_META_U(unit, \
                             "unit %d is not valid\n"), unit)); \
        return BCM_E_UNIT; \
    }
#define DPP_SWITCH_UNIT_INIT_CHECK
#define DPP_SWITCH_LOCK_TAKE
#define DPP_SWITCH_LOCK_RELEASE
#define DPP_SWITCH_TRAP_TO_CPU  (1)
#define DPP_SWITCH_DROP         (2)
#define DPP_SWITCH_FLOOD        (3)
#define DPP_SWITCH_FWD          (4)
#define DPP_SWITCH_SNOOP_TO_CPU (5)
#define DPP_SWITCH_METER_TC_MAX_VAL (3)

#define DPP_FRWRD_IP_ALL_VRFS_ID SOC_PPC_FRWRD_IP_ALL_VRFS_ID

#define DPP_SWITCH_MAX_NUM_ADDITIONAL_TPIDS  4

#define SWITCH_ACCESS                           sw_state_access[unit].dpp.bcm._switch

int _bcm_dpp_switch_control_congestion_mode_set(int unit, int arg);
int _bcm_dpp_switch_control_congestion_mode_get(int unit, int *arg);
int _bcm_dpp_switch_control_congestion_enable_set(int unit, int arg);
int _bcm_dpp_switch_control_congestion_enable_get(int unit, int *arg);
int _bcm_dpp_switch_control_congestion_options_set(int unit, bcm_switch_control_t bcm_type, int arg);
int _bcm_dpp_switch_control_congestion_options_get(int unit, bcm_switch_control_t bcm_type, int *arg);

/* udp dest port type casting from bcm to arad */
int _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_parser(int unit, bcm_switch_control_t bcm_type, ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE *arg);
int _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_prge(int unit, bcm_switch_control_t bcm_type, ARAD_EGR_PROG_EDITOR_PROGRAMS *arg);

#ifdef CRASH_RECOVERY_SUPPORT
extern soc_dcmn_cr_t *dcmn_cr_info[BCM_MAX_NUM_UNITS];
#endif

int
_bcm_dpp_switch_init(int unit)
{
    uint8 is_allocated;
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
/*in order to recover the soc_control var in wb do this get*/
    if (SOC_WARM_BOOT(unit)) {
        rv = sw_state_access[unit].dpp.soc.config.autosync.get(unit, (uint32 *) &(SOC_CONTROL(unit)->autosync));
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

    if(!SOC_WARM_BOOT(unit)) {
        rv = SWITCH_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        if(!is_allocated) {
            rv = SWITCH_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_dpp_switch_eg_trap_enable_mask_get(int unit,
                                 SOC_PPC_TRAP_EG_TYPE trap_code_ndx,
                                 uint32 *trap_enable_mask,
                                 int *is_port_relevant)
{

    BCMDNX_INIT_FUNC_DEFS;
    *is_port_relevant = 1;

    switch (trap_code_ndx)
        {
            case SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP:
                *trap_enable_mask = SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP;
                break;
            case SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION:
                *trap_enable_mask = SOC_PPC_EG_FILTER_PORT_ENABLE_MTU;
                break;           
    default:
        *is_port_relevant = 0;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_eg_trap_code_enable_switch_control_port_set(int unit,
                                        SOC_PPC_TRAP_EG_TYPE trap_code_ndx,
                                        bcm_port_t port,                                   
                                        int enable)
{
    int                                  rv = BCM_E_NONE;
    int                                  is_port_relevant = 1;
    uint32                             trap_enable_mask=0;
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_EG_FILTER_PORT_INFO              port_info;
    uint32                              pp_port;
    int                                 core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _dpp_switch_eg_trap_enable_mask_get(unit, trap_code_ndx,
                                             &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));
    /* get before update*/
    soc_sand_rv = soc_ppd_eg_filter_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (enable) {
        /*enable*/
        port_info.filter_mask |= trap_enable_mask;
    } else {
        /*disable*/
        port_info.filter_mask &= ~trap_enable_mask;
    }

    /*update*/
    soc_sand_rv = soc_ppd_eg_filter_port_info_set(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_eg_trap_code_enable_switch_control_port_get(int unit,
                                  SOC_PPC_TRAP_EG_TYPE trap_code_ndx,
                                  bcm_port_t port,                                   
                                  int *is_enabled)
{
    int                                  rv = BCM_E_NONE;
    int                                  is_port_relevant = 1;
    uint32                             trap_enable_mask=0;
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_EG_FILTER_PORT_INFO              port_info;
    uint32                              pp_port;
    int                                 core;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    rv = _dpp_switch_eg_trap_enable_mask_get(unit, trap_code_ndx,
                                             &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_filter_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (port_info.filter_mask & trap_enable_mask) {
        *is_enabled = TRUE;
    } else {
        *is_enabled = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_dpp_eg_trap_code_switch_control_get(int unit,
                                 SOC_PPC_TRAP_EG_TYPE trap_code_ndx,
                                 int cpu_drop_flood, /* 1:cpu */
                                 int *is_enabled)
{
    int                                rv = BCM_E_NONE;
    int                                port_i, core;
    int                                is_port_relevant = 1;
    SOC_PPC_PORT                           soc_ppd_port_i;
    uint32                             soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_EG_FILTER_PORT_INFO            port_info;
    uint32                             trap_enable_mask = 0;
    uint32                             eg_action_profile;
    pbmp_t                             ports_map;


    BCMDNX_INIT_FUNC_DEFS;
    *is_enabled = FALSE;

    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_trap_to_eg_action_map_get(soc_sand_dev_id,
                               trap_code_ndx, &eg_action_profile);    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* check if destination or snoop is cpu */
    if (cpu_drop_flood == DPP_SWITCH_TRAP_TO_CPU) 
    {
        if (eg_action_profile == _DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX) 
        {          
            *is_enabled = TRUE;            
        }
        else
        {
            *is_enabled = FALSE;
        }            
    }


    rv = _dpp_switch_eg_trap_enable_mask_get(unit, trap_code_ndx,
                                     &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_port_relevant > 0)
    {
        /* check if all ports are enabled/disabled correctly */

    BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
    BCM_PBMP_ITER(ports_map, port_i) {          
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            soc_sand_rv = soc_ppd_eg_filter_port_info_get(soc_sand_dev_id, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (cpu_drop_flood == DPP_SWITCH_FWD) {
                if (port_info.filter_mask & trap_enable_mask) {
                    *is_enabled = FALSE;
                    BCM_EXIT;
                }
            } else {
                if (!(port_info.filter_mask & trap_enable_mask)) {
                    *is_enabled = FALSE;
                    BCM_EXIT;
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_eg_trap_code_switch_control_set(int unit,
                                  SOC_PPC_TRAP_CODE trap_code_ndx,
                                  int cpu_drop_flood, /* 1:cpu */
                                  int enable)
{
    int                               rv = BCM_E_NONE;  
    int                               port_i, core;
    int                               is_port_relevant = 1;
    SOC_PPC_PORT                          soc_ppd_port_i;
    uint32                            soc_sand_dev_id;
    uint32                            soc_sand_rv;
    SOC_PPC_EG_FILTER_PORT_INFO           port_info;
    uint32                            trap_enable_mask;
    uint32                            eg_action_profile;
    pbmp_t                            ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);   

    if (enable == FALSE)
    {
        eg_action_profile = SOC_PPC_TRAP_EG_NO_ACTION;      
    }
    else
    {       
        if (cpu_drop_flood == DPP_SWITCH_TRAP_TO_CPU) {
            eg_action_profile = _DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX;
        }
        else 
        {
           BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("If trap enabled, only trap to CPU is allowed")));
        }        
    }

    /* update*/
    soc_sand_rv = soc_ppd_trap_to_eg_action_map_set(soc_sand_dev_id,
                                                    trap_code_ndx, eg_action_profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = _dpp_switch_eg_trap_enable_mask_get(unit, trap_code_ndx,
                                &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_port_relevant > 0)
    {
        /* enable\disable for all ports */
        BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
        BCM_PBMP_ITER(ports_map, port_i) {            
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            /* get before update*/
            soc_sand_rv = soc_ppd_eg_filter_port_info_get(soc_sand_dev_id, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (!enable) {
                /*disable all*/
                port_info.filter_mask &= ~trap_enable_mask;
            } else {
                /*enable all*/
                port_info.filter_mask |= trap_enable_mask;
            }

            /* update */
            soc_sand_rv = soc_ppd_eg_filter_port_info_set(soc_sand_dev_id, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int
_dpp_switch_trap_enable_mask_get(int unit,
                                 SOC_PPC_TRAP_CODE trap_code_ndx,
                                 uint32 *trap_enable_mask,
                                 int *is_port_relevant)
{

    BCMDNX_INIT_FUNC_DEFS;
    *is_port_relevant = 1;

    switch (trap_code_ndx)
        {
            case SOC_PPC_TRAP_CODE_MY_ARP:
            case SOC_PPC_TRAP_CODE_ARP:
            case SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP:
                *trap_enable_mask = SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP;
                break;
            case SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
    case SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
    case SOC_PPC_TRAP_CODE_IGMP_UNDEFINED:
        *trap_enable_mask = SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP;
        break;
    case SOC_PPC_TRAP_CODE_DHCP_SERVER:
    case SOC_PPC_TRAP_CODE_DHCP_CLIENT:
    case SOC_PPC_TRAP_CODE_DHCPV6_SERVER:
    case SOC_PPC_TRAP_CODE_DHCPV6_CLIENT:
        *trap_enable_mask = SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP;
        break;
    case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED:
    case SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY:
    case SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG:
    case SOC_PPC_TRAP_CODE_ICMPV6_MLD_UNDEFINED:
        *trap_enable_mask = SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD;
        break;
    default:
        *is_port_relevant = 0;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _dpp_switch_my_arp_set(int unit, uint32 my_ip)
{  
  int                   i;
  uint32             soc_sand_dev_id;
  uint32              soc_sand_rv;
  SOC_PPC_LLP_TRAP_ARP_INFO arp_info;

  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rv = soc_ppd_llp_trap_arp_info_get(soc_sand_dev_id,&arp_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  for (i = 0; i < SOC_PPC_TRAP_NOF_MY_IPS; i++)
  {
    if (arp_info.my_ips[i] == 0 || arp_info.my_ips[i] == my_ip)
    {
      arp_info.my_ips[i] = my_ip;
      soc_sand_rv = soc_ppd_llp_trap_arp_info_set(soc_sand_dev_id,&arp_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      BCM_EXIT;      
    }    
  }

    /* if no room for another my_ip */
 BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("no room for another my_ip"))); 

exit:
  BCMDNX_FUNC_RETURN;
}

int _dpp_switch_my_arp_remove(int unit, uint32 my_ip)
{  
  int                   i;
  uint32             soc_sand_dev_id;
  uint32              soc_sand_rv;
  SOC_PPC_LLP_TRAP_ARP_INFO arp_info;

  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rv = soc_ppd_llp_trap_arp_info_get(soc_sand_dev_id,&arp_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  for (i = 0; i < SOC_PPC_TRAP_NOF_MY_IPS; i++)
  {
    if (arp_info.my_ips[i] == my_ip)
    {
      arp_info.my_ips[i] = 0;
      soc_sand_rv = soc_ppd_llp_trap_arp_info_set(soc_sand_dev_id,&arp_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      BCM_EXIT;      
    }    
  }

    /* if not found */
   BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("my_ip doesn\'t exist")));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * function for changing the trap information to default,
 * any init configuration that wants to be the default configuration
 * should be configured through here
 */
int
_dpp_switch_reset_trap_code(int unit,
                            SOC_PPC_TRAP_CODE trap_code_ndx,
                            int is_snoop)
{
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   profile_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO   snoop_info;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    if (is_snoop)
    {
      soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(soc_sand_dev_id, trap_code_ndx,
        &snoop_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } 
    else
    {
      soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id, trap_code_ndx, 
        &profile_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }    

    switch (trap_code_ndx)
    {
        default:
            if (is_snoop)
            {
              snoop_info.strength = 0;
              snoop_info.snoop_cmnd = 0;
            } 
            else
            {
              profile_info.strength = 0; /*default*/
              profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE;              
            }
            break;            
    }

    if (is_snoop)
    {
        soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id, trap_code_ndx,
                                                  &snoop_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } 
    else
    {
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, trap_code_ndx, 
                                                &profile_info,BCM_CORE_ALL);       
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_dpp_trap_code_switch_control_priority_set(int unit,
                                           SOC_PPC_TRAP_CODE trap_code_ndx,
                                           int priority /* 0-7 */)
{
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   profile_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* get before update*/
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id,
                                                      trap_code_ndx, &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* change the priority */
    profile_info.cos_info.tc = priority;

    if ((profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) &&
        (profile_info.dest_info.frwrd_dest.dest_id == ARAD_FRST_CPU_PORT_ID) &&
        (profile_info.cos_info.tc != 0))
    {
      /* if it is trapped to cpu - overwrite tc*/
      profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
    }
    else
    {
      /* if its not trapped to cpu - cancel the cpu related tc*/
      profile_info.bitmap_mask &= ~SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
    }

    /* update*/
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, trap_code_ndx, 
                                                      &profile_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_code_switch_control_priority_get(int unit,
                                           SOC_PPC_TRAP_CODE trap_code_ndx,
                                           int *priority /* 0-7 */)
{
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO   profile_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* get trap info */
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id,
                                                      trap_code_ndx, &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* return the priority (tc) */
    *priority = profile_info.cos_info.tc;

exit:
    BCMDNX_FUNC_RETURN;
}

/* enable = 1 allows transplant events on static entries. enable = 0 doesn't allow. */
int _dpp_static_transplant_switch_control_set(int unit, uint8 enable)
{
    int rv = BCM_E_NONE;  

    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_transplant_static_set, (unit, enable));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* enable = 1 allows transplant events on static entries. enable = 0 doesn't allow. */
int _dpp_static_transplant_switch_control_get(int unit, uint8 *is_enable)
{
    int rv = BCM_E_NONE;  

    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_transplant_static_get, (unit, is_enable));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_code_switch_control_set(int unit,
                                  SOC_PPC_TRAP_CODE trap_code_ndx,
                                  int cpu_drop_flood, /* 1:cpu 2:drop 3:flood 4:default 5:snoop to cpu */
                                  int enable)
{
    int                                 rv = BCM_E_NONE;  
    int                                 port_i, core;
    int                                 is_port_relevant = 1;
    SOC_PPC_PORT                            soc_ppd_port_i;

    uint32                              soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO              port_info;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO  profile_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO  snoop_info;
    uint32                              trap_enable_mask;
    pbmp_t                              ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    /* get before update*/
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,
                                              trap_code_ndx, &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(unit, trap_code_ndx,
                                              &snoop_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (enable == FALSE)
    {
      if (cpu_drop_flood == DPP_SWITCH_SNOOP_TO_CPU)
      {
          rv = _dpp_switch_reset_trap_code(unit,trap_code_ndx,1);
          BCMDNX_IF_ERR_EXIT(rv);
      } 
      else
      {
          rv = _dpp_switch_reset_trap_code(unit,trap_code_ndx,0);
          BCMDNX_IF_ERR_EXIT(rv);
      }            
    }
    else
    {
        /* cpu snoop treatment */
        if (cpu_drop_flood == DPP_SWITCH_SNOOP_TO_CPU) {
          snoop_info.snoop_cmnd = _BCM_DPP_SNOOP_CMD_TO_CPU;
          snoop_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_snoop_strength);
        }
        
        if (cpu_drop_flood == DPP_SWITCH_TRAP_TO_CPU) {
            /* change the destination info and the strength */
            profile_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            SOC_PPD_FRWRD_DECISION_LOCAL_CPU_SET(unit,
                                                 &(profile_info.dest_info.frwrd_dest),
                                                 soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        if (cpu_drop_flood == DPP_SWITCH_DROP) {
            /* change the destination info and the strength */
            profile_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
            SOC_PPD_FRWRD_DECISION_DROP_SET(unit,
                                            &(profile_info.dest_info.frwrd_dest),
                                            soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            profile_info.dest_info.frwrd_dest.dest_id = 0;
        }
        if (cpu_drop_flood == DPP_SWITCH_FLOOD) {
            /* change the destination info and the strength */
            profile_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;

            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(profile_info.dest_info.frwrd_dest), 0, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            profile_info.dest_info.add_vsi = TRUE;
        }
        if (cpu_drop_flood == DPP_SWITCH_FWD) {
            /* change the destination info and the strength */
            profile_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
            profile_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE;
        }

        if ((profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) &&
            (profile_info.dest_info.frwrd_dest.dest_id == ARAD_FRST_CPU_PORT_ID) &&
            (profile_info.cos_info.tc != 0))
        {
          /* if it is trapped to cpu - overwrite tc*/
          profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
        }
        else
        {
          /* if its not trapped to cpu - cancel the cpu related tc*/
          profile_info.bitmap_mask &= ~SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
        }

        /* update*/
        soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit, trap_code_ndx, 
                                                          &profile_info,BCM_CORE_ALL);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(unit, trap_code_ndx,
                                                          &snoop_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    rv = _dpp_switch_trap_enable_mask_get(unit, trap_code_ndx,
                                     &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_port_relevant > 0)
    {
        /* enable\disable for all ports */
        BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
        BCM_PBMP_ITER(ports_map, port_i) {           
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            /* get before update*/
            soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (!enable) {
                /*disable all*/
                port_info.trap_enable_mask &= ~trap_enable_mask;
            } else {
                /*enable all*/
                port_info.trap_enable_mask |= trap_enable_mask;
            }

            /* update */
            soc_sand_rv = soc_ppd_llp_trap_port_info_set(unit, core, soc_ppd_port_i, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_code_switch_control_get(int unit,
                                 SOC_PPC_TRAP_CODE trap_code_ndx,
                                 int cpu_drop_flood, /* 1:cpu 2:drop 3:flood 4:drop 5:snoop to cpu */
                                 int *is_enabled)
{
    int                                 rv = BCM_E_NONE;
    int                                 port_i, core;
    int                                 is_port_relevant = 1;
    SOC_PPC_PORT                            soc_ppd_port_i;
    uint32                              soc_sand_dev_id;
    uint32                              soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO              port_info;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO  profile_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO  snoop_info;
    uint32                              trap_enable_mask;
    pbmp_t                              ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    *is_enabled = TRUE;

    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(soc_sand_dev_id, trap_code_ndx,
                                                      &profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(soc_sand_dev_id, trap_code_ndx,
                                                      &snoop_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* check if destination or snoop is cpu */
    if (cpu_drop_flood == DPP_SWITCH_TRAP_TO_CPU || cpu_drop_flood == DPP_SWITCH_SNOOP_TO_CPU) {
        if (((profile_info.dest_info.frwrd_dest.dest_id !=
                 ARAD_FRST_CPU_PORT_ID) ||
             (profile_info.dest_info.frwrd_dest.type !=
             SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT) ||
             !(profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST))
             && cpu_drop_flood == DPP_SWITCH_TRAP_TO_CPU) {
            *is_enabled = FALSE;            
        }
        if ((snoop_info.snoop_cmnd != _BCM_DPP_SNOOP_CMD_TO_CPU) &&
              cpu_drop_flood == DPP_SWITCH_SNOOP_TO_CPU) 
        {          
            *is_enabled = FALSE;            
        }
        if (*is_enabled == FALSE)
        {
            BCM_EXIT;
        }        
    }
    /* check if destination is drop or uc flow 32767 (==drop)  */
    if (cpu_drop_flood == DPP_SWITCH_DROP) {
        if (((profile_info.dest_info.frwrd_dest.type != SOC_PPC_FRWRD_DECISION_TYPE_DROP) && 
             !((profile_info.dest_info.frwrd_dest.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW) &&
                (profile_info.dest_info.frwrd_dest.dest_id == 32767))) ||
          !(profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST)) 
        {
            *is_enabled = FALSE;
            BCM_EXIT;
        }
    }
    /* check if destination is flood (add_vsi is on and dest = mc id 0) */
    if (cpu_drop_flood == DPP_SWITCH_FLOOD) {
        if (profile_info.dest_info.frwrd_dest.type != SOC_PPC_FRWRD_DECISION_TYPE_MC ||
            profile_info.dest_info.frwrd_dest.dest_id != 0 ||
            profile_info.dest_info.add_vsi != TRUE ||
            !(profile_info.bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST)
            ) {
            *is_enabled = FALSE;
            BCM_EXIT;
        }
    }

    rv = _dpp_switch_trap_enable_mask_get(unit, trap_code_ndx,
                                     &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_port_relevant > 0)
    {
        /* check if all ports are enabled/disabled correctly */
        BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
        BCM_PBMP_ITER(ports_map, port_i) {       
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port_i, &core)));
            soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id,core,soc_ppd_port_i,&port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (cpu_drop_flood == DPP_SWITCH_FWD) {
                if (port_info.trap_enable_mask & trap_enable_mask) {
                    *is_enabled = FALSE;
                    BCM_EXIT;
                }
            } else {
                if (!(port_info.trap_enable_mask & trap_enable_mask)) {
                    *is_enabled = FALSE;
                    BCM_EXIT;
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_code_enable_switch_control_port_set(int unit,
                                    SOC_PPC_TRAP_CODE trap_code_ndx,
                                    bcm_port_t port,                                   
                                    int enable)
{
    int                                  rv = BCM_E_NONE;
    int                                  is_port_relevant = 1;
    uint32                             trap_enable_mask = 0;
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO               port_info;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _dpp_switch_trap_enable_mask_get(unit, trap_code_ndx,
                                          &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    /* get before update*/
    soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (enable) {
        /*enable*/
        port_info.trap_enable_mask |= trap_enable_mask;
    } else {
        /*disable*/
        port_info.trap_enable_mask &= ~trap_enable_mask;
    }

    /* update */
    soc_sand_rv = soc_ppd_llp_trap_port_info_set(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_code_enable_switch_control_port_get(int unit,
                                  SOC_PPC_TRAP_CODE trap_code_ndx,
                                  bcm_port_t port,                                   
                                  int *is_enabled)
{
    int                                  rv = BCM_E_NONE;
    int                                  is_port_relevant = 1;
    uint32                             trap_enable_mask=0;
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO               port_info;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    rv = _dpp_switch_trap_enable_mask_get(unit, trap_code_ndx,
                                          &trap_enable_mask, &is_port_relevant);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    /* get before update*/
    soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (port_info.trap_enable_mask & trap_enable_mask) {
        *is_enabled = TRUE;
    } else {
        *is_enabled = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_trap_to_cpu_if_dos_attack(int unit, bcm_switch_control_t bcm_type, int enable)
{
    int rv;
    int to_cpu;
    int is_member;
    int to_enable;


    BCMDNX_INIT_FUNC_DEFS;
    /* check if dos attack is trapped to cpu */
    rv = _bcm_sw_db_switch_dos_attack_info_get(unit, bcmSwitchDosAttackToCpu, &to_cpu);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check if the switch_control is a member in dos attack */
    rv = _bcm_sw_db_switch_dos_attack_info_get(unit, bcm_type, &is_member);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_member && to_cpu && enable)
    {
        /* should enable trap to cpu */
        to_enable = TRUE;
    }
    else if ((to_cpu  && !is_member && !enable ) ||
             (!to_cpu && is_member  && !enable ))
    {
        /* should disable trap to cpu */
        to_enable = FALSE;
    } 
    else
    {
        /*nothing to do*/
        BCM_EXIT;
    }

    switch (bcm_type)
    {
        case bcmSwitchDosAttackSipEqualDip:
            rv = _dpp_trap_code_switch_control_set(unit, 
                                            SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP, DPP_SWITCH_TRAP_TO_CPU, to_enable);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        case bcmSwitchDosAttackMinTcpHdrSize:          
        break;
    case bcmSwitchDosAttackV4FirstFrag:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackTcpFlags:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_SN_FLAGS_ZERO, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackL4Port:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackTcpFrag:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackIcmp:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackIcmpPktOversize:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackMACSAEqualMACDA:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_SA_EQUALS_DA, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackIcmpV6PingSize:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackIcmpFragments:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_FRAGMENTED, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackUdpPortsEqual:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_UDP_EQUAL_PORTS, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackTcpPortsEqual:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackSynFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_TCP_SYN_FIN, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackL3Header:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, to_enable);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDosAttackTcpOffset:
        break;
    case bcmSwitchDosAttackTcpFlagsSF:
        break;
    case bcmSwitchDosAttackTcpFlagsFUP:
        break;
    case bcmSwitchDosAttackTcpHdrPartial:
        break;
    case bcmSwitchDosAttackPingFlood:
        break;
    case bcmSwitchDosAttackTcpSmurf:
        break;
    case bcmSwitchDosAttackTcpXMas:
        break;
    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid switch control type")));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_dos_attack_set(int unit, bcm_switch_control_t bcm_type, int enable)
{
    int rv;
    int to_cpu;
    int bcm_type_indx;
    
    BCMDNX_INIT_FUNC_DEFS;
    /* change the state in sw_db */
    rv = _bcm_sw_db_switch_dos_attack_info_set(unit, bcm_type, enable);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* check if dos attack is trapped to cpu */
    rv = _bcm_sw_db_switch_dos_attack_info_get(unit, bcmSwitchDosAttackToCpu, &to_cpu);
    BCMDNX_IF_ERR_EXIT(rv);

    if (bcm_type == bcmSwitchDosAttackToCpu)
    {
        /* if to_cpu - should go over all enabled dos attack switch controls and change trap to cpu */
        for (bcm_type_indx=bcmSwitchDosAttackSipEqualDip; bcm_type_indx<=bcmSwitchDosAttackL3Header; bcm_type_indx++)
        {
            rv = _dpp_trap_to_cpu_if_dos_attack(unit, bcm_type_indx, enable);
            BCMDNX_IF_ERR_EXIT(rv);    
        }
        BCM_EXIT;
    }
    else
    {
        /* if regular dos attack switchcontrol - should change trap to cpu on that specific case if to_cpu is on */
        rv = _dpp_trap_to_cpu_if_dos_attack(unit, bcm_type, enable);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_dpp_dos_attack_get(int unit, bcm_switch_control_t bcm_type, int *enable)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_sw_db_switch_dos_attack_info_get(unit, bcm_type, enable);
    BCMDNX_IF_ERR_EXIT(rv); 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcmSwitchL2LearnMode */
int static _bcm_dpp_switch_L2LearnMode_set(int unit, int arg)
{
    /* showed be called once at init time */
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_OPER_MODE_INFO oper_mode_info;
    SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO distr_info;
    int learn_type;
    int rv = BCM_E_NONE;
    uint32 dma_supported;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    rv = arad_pp_frwrd_mact_is_dma_supported(unit, &dma_supported);
    BCM_SAND_IF_ERR_EXIT(rv);

    /* From Jericho BCM_L2_LEARN_CPU can be used only if the soc property learning_fifo_dma_buffer_size > 0 */
    if (!dma_supported && (arg & BCM_L2_LEARN_CPU) && SOC_IS_JERICHO(unit)) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("BCM_L2_LEARN_CPU can be used only if the soc property learning_fifo_dma_buffer_size > 0")));
    }

    /* Disabling the DMA (if needed) is done here. Enabling (if needed) is done in the end of the function */
    if (!(arg & BCM_L2_LEARN_CPU) || !SOC_MAC_LIMIT_PER_TUNNEL_DISABLE_GET(unit) || (arg & BCM_L2_LEARN_DISABLE))
    {
        if (SOC_IS_JERICHO(unit)) {
            if (dma_supported)
            { 
                /* Unset the FIFO DMA */
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_learning_dma_unset, (unit));
                BCMDNX_IF_ERR_EXIT(rv);

                /* Unset the DMA's distribution FIFO */
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_event_handle_info_set_dma, (unit, 0));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else
            {
                if (SOC_IS_QAX(unit)) {
                    BCMDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_INTERRUPT_MASK_REGISTER_TWOr, REG_PORT_ANY, 0,  MACT_EVENT_READY_MASKf, 0));
                } else {
                    BCMDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_INTERRUPT_MASK_REGISTER_TWOr, REG_PORT_ANY, 0,  MACT_MASTER_EVENT_READY_MASKf, 0));
                    BCMDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_INTERRUPT_MASK_REGISTER_TWOr, REG_PORT_ANY, 0,  MACT_SLAVE_EVENT_READY_MASKf, 0));
                }
            }
        }
        else
        {
            rv = _bcm_dpp_mact_interrupts_mask(unit, TRUE);
        }
      
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_rv = soc_ppd_frwrd_mact_oper_mode_info_get(soc_sand_dev_id, &oper_mode_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* check that required configuration for relevant learning is set*/
    /* learning distribution is not needed when learning mode is egress distributed */
    if (!(arg & BCM_L2_EGRESS_INDEPENDENT) && (dma_supported || !(arg & BCM_L2_LEARN_CPU)) && !(arg & BCM_L2_LEARN_DISABLE)) {
        soc_sand_rv = soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(soc_sand_dev_id,
                                                                          &distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (distr_info.header_type == SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Learnig message is not configure, need to call configure bcm_petra_l2_learn_msgs_config_set"))); /* need to configure the msgs distribution info*/
        }

        /* copy learning information */
        sal_memcpy(&(oper_mode_info.learn_msgs_info.info), &distr_info,
                   sizeof(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO));

        soc_sand_rv = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(soc_sand_dev_id,
                                                                           &distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* copy shadow information */
        sal_memcpy(&(oper_mode_info.shadow_msgs_info.info), &distr_info,
                   sizeof(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO));

    }

    learn_type = arg & (BCM_L2_INGRESS_CENT | BCM_L2_INGRESS_DIST | BCM_L2_EGRESS_DIST | BCM_L2_EGRESS_CENT | BCM_L2_EGRESS_INDEPENDENT);

    switch (learn_type) {
    case BCM_L2_INGRESS_CENT:
        oper_mode_info.learning_mode =
           SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED;
        break;
    case BCM_L2_INGRESS_DIST:
        oper_mode_info.learning_mode =
           SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED;
        break;
    case BCM_L2_EGRESS_DIST:
        oper_mode_info.learning_mode =
           SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED;
        break;
    case BCM_L2_EGRESS_CENT:
        oper_mode_info.learning_mode =
           SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED;
        break;
    case BCM_L2_EGRESS_INDEPENDENT:
        oper_mode_info.learning_mode =
           SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT;
        break;
        /* one of the above has to be side beside to LEARN_CPU or LEARN_DISABLE, as above setting affect the aging mode, event generation */
    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2 mode was not provided one of the above should be set beside to learn_disable or learn_cpu ")));
    }

    oper_mode_info.disable_learning = (arg & BCM_L2_LEARN_DISABLE)?1:0;

    /* init shadow mode to none, if not already initialized*/
    if(oper_mode_info.shadow_mode >= SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES)
    {
      oper_mode_info.shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE;
      oper_mode_info.shadow_msgs_info.type = 
          SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
    }
    /* event handler can be OLP or CPU */
	if (dma_supported) 
	{
	    /* In Jericho the OLP is always in the game */
		oper_mode_info.learn_msgs_info.type = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
	}
	else
	{
    	oper_mode_info.learn_msgs_info.type = 
    	(arg & BCM_L2_LEARN_CPU) ? SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS : SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
	}

    soc_sand_rv = soc_ppd_frwrd_mact_oper_mode_info_set(soc_sand_dev_id, &oper_mode_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
#ifdef BCM_CMICM_SUPPORT  
    /* Set the ARM CPU MACT rx mode. 
     * 0 -> DSP paring mode, 1 -> MACT event FIFO mode.
     */
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
        if (shr_llm_appl_is_init(unit)) {
            uint32 rx_mode;
            
            if (arg & BCM_L2_LEARN_CPU) {
                rx_mode = 1;
            } else {
                rx_mode = 0;       
            }    
            rv = shr_llm_msg_mac_rx_mode_set(unit, rx_mode);   
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
#endif

    /*
     * Because of ARM CPU also need read this event-fifo if MAC limiting per tunnel ID
     * is enabled. So don't disable ARAD_INT_IHP_MACTEVENTREADY event mask 
     * if MAC limiting per tunnel ID is enabled.
     */
    if ((arg & BCM_L2_LEARN_CPU) && !(arg & BCM_L2_LEARN_DISABLE) && SOC_MAC_LIMIT_PER_TUNNEL_DISABLE_GET(unit)) {
        if (SOC_IS_JERICHO(unit)) {

            if (dma_supported)
            {      
                /* rv = bcm_dpp_counter_reserve_dma_channel(unit, ARAD_LEARNING_DMA_CHANNEL_USED); */
                /* BCMDNX_IF_ERR_EXIT(rv); */
                /* Set the FIFO DMA. Jericho is handling the CPU callbacks using DMA. */
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_learning_dma_set, (unit));
                BCMDNX_IF_ERR_EXIT(rv);

                /* Set the DMA FIFO as the OLP's destination */
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_event_handle_info_set_dma, (unit, 1));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            rv = _bcm_dpp_mact_interrupts_mask(unit, FALSE);
        }
      BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}   

/* bcmSwitchL2LearnMode */
int static _bcm_dpp_switch_L2LearnMode_get(int unit, int *arg)
{
    /* showed be called once at init time */
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_OPER_MODE_INFO oper_mode_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_mact_oper_mode_info_get(soc_sand_dev_id, &oper_mode_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (oper_mode_info.learning_mode) {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
        *arg = BCM_L2_INGRESS_CENT;
        break;
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
        *arg = BCM_L2_INGRESS_DIST;
        break;
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
        *arg = BCM_L2_EGRESS_DIST;
        break;
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
        *arg = BCM_L2_EGRESS_CENT;
        break;
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
        *arg = BCM_L2_EGRESS_INDEPENDENT;
        break;
    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_mact_oper_mode_info_get retured invalid learning mode")));
    }

    if (oper_mode_info.disable_learning) {
        *arg |= BCM_L2_LEARN_DISABLE;
    }

    if (oper_mode_info.learn_msgs_info.type == SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS) {
        *arg |= BCM_L2_LEARN_CPU;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/* bcmSwitchTraverseMode */
int static _bcm_dpp_switch_traverse_mode_set(int unit, int arg)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO trvrs_mode_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    switch (arg) {
    case bcmSwitchTableUpdateNormal:
         trvrs_mode_info.state = SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_NORMAL;
    break;
    case bcmSwitchTableUpdateRuleAdd:
         trvrs_mode_info.state = SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE;
    break;
    case bcmSwitchTableUpdateRuleClear:
         trvrs_mode_info.state = SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RESET;
    break;
    case bcmSwitchTableUpdateRuleCommit:
         trvrs_mode_info.state = SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RUN;
    break;
    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("input mode not supported")));
    }
    soc_sand_rv = soc_ppd_frwrd_mact_traverse_mode_info_set(soc_sand_dev_id,&trvrs_mode_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}



/* bcmSwitchTraverseMode */
int static _bcm_dpp_switch_traverse_mode_get(int unit, int *arg)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO trvrs_mode_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_mact_traverse_mode_info_get(soc_sand_dev_id,&trvrs_mode_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* note set/get not symmetric, as only normal and ruleAdd are real states.
       other are just actions.
     */
    if (trvrs_mode_info.state == SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE) {
        *arg = bcmSwitchTableUpdateRuleAdd;
    }
    else {
        *arg = bcmSwitchTableUpdateNormal;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT

#define _BCM_SYNC_SUCCESS(unit) \
  (BCM_SUCCESS(rv) || (BCM_E_INIT == rv) || (BCM_E_UNAVAIL == rv))
  

/* 
 * this function behave differently in different scenarios:
 * 1. when called in manual sync it perfors a full sync
 * 2. when called automatically by the dispatcher at autosync mode,
 *    it only sync modules that don't have autosync implementation.
 *    in the future all modules should have built in autosync and won't
 *    dispatcher level _bcm_dpp_switch_control_sync call
 * 3. when called in a TEST_MODE compilation - for now act the same. 
 *    the different behaviour is in the calling to this function 
 */
 
   
STATIC int
_bcm_dpp_switch_control_sync(int unit, int arg)
{
    uint32 scache_handle;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* for now KBP/KAPS need to be always synced even in autosync mode */
    /* when KBP/KAPS will support autosync the sync should be pushed out*/
#if defined(INCLUDE_KBP)
    if (SOC_DPP_IS_ELK_ENABLE(unit)) {
        rv = arad_kbp_sync(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* defined(INCLUDE_KBP) */

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
    if (JER_KAPS_ENABLE(unit)) {
        rv = jer_kaps_sync(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/

    /* for now sat need to be always synced even in autosync mode */
    /* when sat will support autosync the sync should be pushed out*/
#if defined(BCM_JERICHO_SUPPORT)
        if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SAT_ENABLE, 0)) {
            rv = bcm_common_sat_wb_sync(unit, 0);
            BCMDNX_IF_ERR_EXIT(rv);

#if defined(BCM_SAT_SUPPORT)
            rv = soc_sat_wb_sync(unit, 0);
            BCMDNX_IF_ERR_EXIT(rv);
#endif
        }
#endif

    /****************/
    /*** autosync ***/
    /****************/
    if (SOC_AUTOSYNC_IS_ENABLE(unit)) {

        /* commit the specific modules that don't have autosync*/
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SAT, 0);
        rv = soc_scache_commit_handle(unit, scache_handle, 0x0);
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FIELD, 0);
        rv = soc_scache_commit_handle(unit, scache_handle, 0x0);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Mark scache as clean */
        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->scache_dirty = 0;
        SOC_CONTROL_UNLOCK(unit);

        /* if autosync is enabled we don't need to sync all state
           for now sync only the data that does not support autosync
           so EXIT here */
        BCM_EXIT;
    }

    /* wb engine sync is not needed if running in autosync mode */
    rv = soc_dpp_wb_engine_sync(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /*Take counter processor's cache lock. 
      mainly to not allow changing the state while main thread is syncing*/
    rv = bcm_dpp_counter_state_lock_take(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* now commit the scache to persistent storage (all scache buffers)*/
    rv = soc_scache_commit(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Mark scache as clean */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 0;
    SOC_CONTROL_UNLOCK(unit);

    BCMDNX_IF_ERR_EXIT(rv);

    /*Give counter processor's cache lock.*/
    rv = bcm_dpp_counter_state_lock_give(unit);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int
_bcm_dpp_trunk_bcm_hash_to_ppd_hash(int unit, int bcm_hash_config, 
                              uint8 *dpp_hash_func_id)
{
    int rv = BCM_E_NONE;
    int hash_function_found = FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    if (dpp_hash_func_id == NULL) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL pointer")));
    }

    hash_function_found = FALSE;
#ifdef BCM_88660_A0
    /* Supported polynoms after Arad plus */
    if (SOC_IS_ARADPLUS(unit)) {
        switch (bcm_hash_config) {
        case BCM_HASH_CONFIG_CRC16_0x1eb21:
        case BCM_HASH_CONFIG_CRC16_0x13965:
        case BCM_HASH_CONFIG_CRC16_0x1698D:
        case BCM_HASH_CONFIG_CRC16_0x1105D:
            hash_function_found = TRUE;
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                      (BSL_META_U(unit,
                                  "unit %d. Invalid hash_config for devices after BCM886660 (%d)\n"), unit, bcm_hash_config));
            rv = BCM_E_PARAM;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8003:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x8003;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8011:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x8011;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8423:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x8423;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8101:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x8101;
            break;
        case BCM_HASH_CONFIG_CRC16_0x84a1:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x84a1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x9019:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x9019;
            break;
        default:
            break;
        }
    }
#endif /* BCM_88660_A0 */
    if (!hash_function_found) {
        /* Hashing function not found in Arad+, look in Arad has functions */
        switch (bcm_hash_config) {
        case BCM_HASH_CONFIG_FP_DATA:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_KEY;
            break;
        case BCM_HASH_CONFIG_ROUND_ROBIN:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_ROUND_ROBIN;
            break;
        case BCM_HASH_CONFIG_CLOCK_BASED:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_2_CLOCK;
            break;
        case BCM_HASH_CONFIG_CRC16_0x10861:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x10861;
            break;
        case BCM_HASH_CONFIG_CRC16_0x10285:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x10285;
            break;
        case BCM_HASH_CONFIG_CRC16_0x101a1:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x101A1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x12499:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x12499;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1f801:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x1F801;
            break;
        case BCM_HASH_CONFIG_CRC16_0x172e1:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x172E1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1eb21:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x1EB21;
            break;
        case BCM_HASH_CONFIG_CRC16_0x13965:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x13965;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1698D:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x1698D;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1105D:
            *dpp_hash_func_id = SOC_PPC_LAG_LB_CRC_0x1105D;
            break;
        default:
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid hash_config (%d)\n"), unit, bcm_hash_config));
            rv = BCM_E_PARAM;
            break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_trunk_ppd_hash_to_bcm_hash(int unit, int dpp_hash_func_id, 
                              int *bcm_hash_config)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (bcm_hash_config == NULL) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL pointer")));
    }

    switch (dpp_hash_func_id) {
    case SOC_PPC_LAG_LB_KEY:
        *bcm_hash_config = BCM_HASH_CONFIG_FP_DATA;
        break;
    case SOC_PPC_LAG_LB_ROUND_ROBIN:
        *bcm_hash_config = BCM_HASH_CONFIG_ROUND_ROBIN;
        break;
    case SOC_PPC_LAG_LB_2_CLOCK:
        *bcm_hash_config = BCM_HASH_CONFIG_CLOCK_BASED;
        break;
    case SOC_PPC_LAG_LB_CRC_0x10861:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x10861;
        break;
    case SOC_PPC_LAG_LB_CRC_0x10285:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x10285;
        break;
    case SOC_PPC_LAG_LB_CRC_0x101A1:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x101a1;
        break;
    case SOC_PPC_LAG_LB_CRC_0x12499:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x12499;
        break;
    case SOC_PPC_LAG_LB_CRC_0x1F801:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1f801;
        break;
    case SOC_PPC_LAG_LB_CRC_0x172E1:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x172e1;
        break;
    case SOC_PPC_LAG_LB_CRC_0x1EB21:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1eb21;
        break;
    case SOC_PPC_LAG_LB_CRC_0x13965:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x13965;
        break;
    case SOC_PPC_LAG_LB_CRC_0x1698D:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1698D;
        break;
    case SOC_PPC_LAG_LB_CRC_0x1105D:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1105D;
        break;
    default:
#ifdef BCM_88660_A0
        /* Supported polynoms after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            switch (dpp_hash_func_id) {
            case SOC_PPC_LAG_LB_CRC_0x8003:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8003;
                break;
            case SOC_PPC_LAG_LB_CRC_0x8011:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8011;
                break;
            case SOC_PPC_LAG_LB_CRC_0x8423:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8423;
                break;
            case SOC_PPC_LAG_LB_CRC_0x8101:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8101;
                break;
            case SOC_PPC_LAG_LB_CRC_0x84a1:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x84a1;
                break;
            case SOC_PPC_LAG_LB_CRC_0x9019:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x9019;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_SWITCH,
                          (BSL_META_U(unit,
                                      "unit %d. Invalid hash_config (%d)\n"),
                           unit, dpp_hash_func_id));
                 rv = BCM_E_INTERNAL;
                break;
            }
        } else
#endif /* BCM_88660_A0 */
        {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid hash_config (%d)\n"),
                      unit, dpp_hash_func_id));
            rv = BCM_E_INTERNAL;
        }
        break;
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_global_set(int unit, bcm_switch_control_t bcm_type, 
                                      int arg)
{
    int                         rv = BCM_E_NONE, max_hash_offset;
    SOC_PPC_LAG_HASH_GLOBAL_INFO    info;
    uint32                    soc_sand_rv;
    uint32                   soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_global_info_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashConfig:
        rv = _bcm_dpp_trunk_bcm_hash_to_ppd_hash(unit, arg, &info.hash_func_id);
        break;
    case bcmSwitchTrunkHashOffset:
        max_hash_offset = 15;
        if ((arg < 0) || (arg > max_hash_offset)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HashOffset(%d). "
                                 "Valid values: 0-%d\n"), unit, arg, max_hash_offset));
        } else {
            info.key_shift = arg;
        }
        break;
    case bcmSwitchTrunkHashSeed:
        if (arg >> 16) {
           LOG_WARN(BSL_LS_BCM_SWITCH, (BSL_META_U(unit, "unit %d: Using only 16 LSBs for hash seed. \n"), unit));
        }
        info.seed = arg & 0xffff; /* take only the last byte */
        break;
    case bcmSwitchTrunkHashSrcPortEnable:
        if ((arg != 0) && (arg != 1)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid value (%d) specified "
                                 "Valid values 0 & 1 \n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.use_port_id = arg;
        }
        break;
    case bcmSwitchHashELISearch:
        if ((arg != 0) && (arg != 1)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid value (%d) specified for enable ELI search"
                                 "Valid values 0 & 1 \n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.eli_search  = arg;
        }
        break;
    default:
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_lag_hashing_global_info_set(soc_sand_dev_id, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "extra_hash_enable", 0)) {
            if ((bcm_type == bcmSwitchTrunkHashConfig) && (arg == BCM_HASH_CONFIG_FP_DATA)) {
                soc_sand_rv = arad_pp_flp_update_programs_pem_contex(unit);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_global_get(int unit, bcm_switch_control_t bcm_type, 
                                      int *arg)
{
    int                         rv = BCM_E_NONE, max_hash_offset;
    SOC_PPC_LAG_HASH_GLOBAL_INFO    info;
    uint32                    soc_sand_rv;
    uint32                   soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_LAG_HASH_GLOBAL_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_global_info_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashConfig:
        rv = _bcm_dpp_trunk_ppd_hash_to_bcm_hash(unit, info.hash_func_id, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTrunkHashOffset:
        max_hash_offset = 15;
        if (info.key_shift > max_hash_offset) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HashOffset(%d) "
                                 "configured. Valid values: 0-%d\n"), unit, info.key_shift, max_hash_offset));
            rv = BCM_E_INTERNAL;
        } else {
            *arg = info.key_shift;
        }
        break;
    case bcmSwitchTrunkHashSeed:
        *arg = info.seed;
        break;
    case bcmSwitchTrunkHashSrcPortEnable:
        *arg = info.use_port_id;
        break;
    case bcmSwitchHashELISearch:
        *arg = info.eli_search;
        break;
    default:
        rv = BCM_E_PARAM;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_port_set(int unit, bcm_port_t port, 
                                    bcm_switch_control_t bcm_type, int arg)
{
    int                     rv = BCM_E_NONE, core;
    SOC_PPC_LAG_HASH_PORT_INFO  info;
    uint32               soc_sand_dev_id;
    uint32                soc_sand_rv;
    bcm_port_t              local_port = 0;
    SOC_PPC_PORT                soc_ppd_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_LOCAL(port)) {
            local_port = BCM_GPORT_LOCAL_GET(port);
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Unsupported gport type"
                                 "specified. Only BCM_GPORT_LOCAL is supported"), unit));
            rv = BCM_E_PARAM;
        }
    } else {
        local_port = port;
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

    if (soc_ppd_port > SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)) {
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d: Invalid local port(%d) to soc_ppd_port(%d) ."
                             "Valid values: 0-%d\n"), unit, local_port, soc_ppd_port, SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)));
        rv = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_LAG_HASH_PORT_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashPktHeaderCount:
        if ((arg < 0) || (arg > 3)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderCount(%d) "
                                 "specified. Valid values: 0-3\n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.nof_headers = arg;
        }
        break;
    case bcmSwitchTrunkHashPktHeaderSelect:
        if (arg == BCM_HASH_HEADER_FORWARD) {
            info.first_header_to_parse = SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING;
        } else if (arg == BCM_HASH_HEADER_TERMINATED) {
            info.first_header_to_parse = SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED;
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderSelect(%d) "
                                 "specified. Valid values: BCM_HASH_HEADER_FORWARD & "
                                 "BCM_HASH_HEADER_TERMINATED\n"), unit, arg));
            rv = BCM_E_PARAM;
        }
        break;
    case bcmSwitchTrunkHashMPLSLabelBOS:
        switch (arg) {
        case BCM_HASH_MPLS_LABEL_NONE:
            info.start_from_bos = 0;
            break;
        case BCM_HASH_MPLS_LABEL_BOS_START:
            info.start_from_bos = 1;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                         (BSL_META_U(unit,
                                     "unit %d: Invalid value(%d) specified "
                                     "valid: BCM_HASH_MPLS_LABEL_BOS_START or BCM_HASH_MPLS_LABEL_NONE\n"), unit, arg));
            rv = BCM_E_PARAM;
            break;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_lag_hashing_port_info_set(soc_sand_dev_id, core, soc_ppd_port, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_port_get(int unit, bcm_port_t port, 
                                    bcm_switch_control_t bcm_type, int *arg)
{
    int                     rv = BCM_E_NONE, core;
    SOC_PPC_LAG_HASH_PORT_INFO  info;
    uint32               soc_sand_dev_id;
    uint32                soc_sand_rv;
    bcm_port_t              local_port = 0;
    SOC_PPC_PORT                soc_ppd_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_LOCAL(port)) {
            local_port = BCM_GPORT_LOCAL_GET(port);
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Unsupported gport type"
                                 "specified. Only BCM_GPORT_LOCAL is supported"), unit));
            rv = BCM_E_PARAM;
        }
    } else {
        local_port = port;
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

    if (soc_ppd_port > SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)) {
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d: Invalid local port(%d) to ppd port(%d) specified."
                             "Valid values: 0-%d\n"), unit, local_port, soc_ppd_port, SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)));
        rv = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_LAG_HASH_PORT_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashPktHeaderCount:
        if (info.nof_headers > 3) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderCount(%d) "
                                 "configured. \n"), unit, info.nof_headers));
            rv = BCM_E_PARAM;
        } else {
            *arg = info.nof_headers;
        }
        break;
    case bcmSwitchTrunkHashPktHeaderSelect:
        if (info.first_header_to_parse == SOC_PPC_LAG_HASH_FRST_HDR_FARWARDING) {
            *arg = BCM_HASH_HEADER_FORWARD;
        } else if (info.first_header_to_parse ==
                      SOC_PPC_LAG_HASH_FRST_HDR_LAST_TERMINATED) {
            *arg = BCM_HASH_HEADER_TERMINATED;
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderSelect(%d) "
                                 "configured. \n"), unit, info.first_header_to_parse));
            rv = BCM_E_PARAM;
        }
        break;
    case bcmSwitchTrunkHashMPLSLabelBOS:
        *arg = (info.start_from_bos == 1 ? BCM_HASH_MPLS_LABEL_BOS_START : BCM_HASH_MPLS_LABEL_NONE);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_mask_set(int unit, bcm_switch_control_t bcm_type, 
                                    int arg)
{
    int                 rv = BCM_E_NONE;
    SOC_PPC_HASH_MASK_INFO  info;
    uint32           soc_sand_dev_id;
    uint32            soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_HASH_MASK_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_mask_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashMPLSPWControlWord:
        if ((arg != 1) && (arg != 0)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid value(%d) specified "
                                 "valid: 0 or 1\n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.expect_cw = arg;
        }
        break;
    case bcmSwitchTrunkHashNormalize:
        if ((arg != 1) && (arg != 0)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid value(%d) specified "
                                 "valid: 0 or 1\n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.is_symmetric_key = arg;
        }
        break;
    case bcmSwitchHashFCOEField0:

        if (((arg & BCM_HASH_FCOE_FIELD_DST_ID_LO) && !(arg & BCM_HASH_FCOE_FIELD_DST_ID_HI)) ||
            ((arg & BCM_HASH_FCOE_FIELD_DST_ID_HI) && !(arg & BCM_HASH_FCOE_FIELD_DST_ID_LO)) ||
            ((arg & BCM_HASH_FCOE_FIELD_SRC_ID_LO) && !(arg & BCM_HASH_FCOE_FIELD_SRC_ID_HI)) ||
            ((arg & BCM_HASH_FCOE_FIELD_SRC_ID_HI) && !(arg & BCM_HASH_FCOE_FIELD_SRC_ID_LO))) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid value(0x%08x) specified "
                                 "setting FCoE hash value src/dst ONLY hi/low is not supported."
                                 "hi & low must be use\n"), unit, arg));
            rv = BCM_E_PARAM;
        }
        else {

            if(arg & BCM_HASH_FCOE_FIELD_ORIGINATOR_EXCHANGE_ID) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_ORG_EX_ID;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_ORG_EX_ID;
            }
            if(arg & BCM_HASH_FCOE_FIELD_RESPONDER_EXCHANGE_ID) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_RES_EX_ID;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_RES_EX_ID;
            }

            /* SOC level api does not support HI/LOW bit, so not distinguishing between them. */
            if(arg & BCM_HASH_FCOE_FIELD_DST_ID_LO ||
               arg & BCM_HASH_FCOE_FIELD_DST_ID_HI) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_DEST_ID;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_DEST_ID;
            }
            /* SOC level api does not support HI/LOW bit, so not distinguishing between them. */
            if(arg & BCM_HASH_FCOE_FIELD_SRC_ID_LO ||
               arg & BCM_HASH_FCOE_FIELD_SRC_ID_HI) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_SRC_ID;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_SRC_ID;
            }

            if(arg & BCM_HASH_FCOE_FIELD_VIRTUAL_FABRIC_ID) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_VFI;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_VFI;
            }
            
            if(arg & BCM_HASH_FCOE_FIELD_CNTAG_FLOW_ID) {
                info.mask &= ~SOC_PPC_HASH_MASKS_FC_SEQ_ID;
            }
            else {
                info.mask |= SOC_PPC_HASH_MASKS_FC_SEQ_ID;
            }
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_lag_hashing_mask_set(soc_sand_dev_id, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_trunk_hash_mask_get(int unit, bcm_switch_control_t bcm_type, 
                                    int *arg)
{
    int                 rv = BCM_E_NONE;
    SOC_PPC_HASH_MASK_INFO  info;
    uint32           soc_sand_dev_id;
    uint32            soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_HASH_MASK_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lag_hashing_mask_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchTrunkHashMPLSPWControlWord:
        if ((info.expect_cw != 1) && (info.expect_cw != 0)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid value(%d) "
                                 "configured\n"), unit, info.expect_cw));
            rv = BCM_E_PARAM;
        } else {
            *arg = info.expect_cw;
        }
        break;
    case bcmSwitchTrunkHashNormalize:
        if ((info.is_symmetric_key != 1) && (info.is_symmetric_key != 0)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid value(%d) "
                                 "configured\n"), unit, info.is_symmetric_key));
            rv = BCM_E_PARAM;
        } else {
            *arg = info.is_symmetric_key;
        }
        break;
    case bcmSwitchHashFCOEField0:
        *arg = 0;
        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_ORG_EX_ID)) {
            *arg |= BCM_HASH_FCOE_FIELD_RESPONDER_EXCHANGE_ID;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_RES_EX_ID)) {
            *arg |= BCM_HASH_FCOE_FIELD_ORIGINATOR_EXCHANGE_ID;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_DEST_ID)) {
            /* SOC level api does not support HI/LOW bit, so not adding both of them. */
            *arg |= BCM_HASH_FCOE_FIELD_DST_ID_LO;
            *arg |= BCM_HASH_FCOE_FIELD_DST_ID_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_SRC_ID)) {
            /* SOC level api does not support HI/LOW bit, so not adding both of them. */
            *arg |= BCM_HASH_FCOE_FIELD_SRC_ID_LO;
            *arg |= BCM_HASH_FCOE_FIELD_SRC_ID_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_SEQ_ID)) {
            *arg |= BCM_HASH_FCOE_FIELD_CNTAG_FLOW_ID;
        }

        if (!(info.mask & SOC_PPC_HASH_MASKS_FC_VFI)) {
            *arg |= BCM_HASH_FCOE_FIELD_VIRTUAL_FABRIC_ID;
        }

        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_lag_hash_mask_set(int unit, bcm_switch_control_t bcm_type, 
                                    int arg)
{
    int                 rv = BCM_E_NONE;
    SOC_PPC_HASH_MASK_INFO  info;
    uint32           soc_sand_dev_id;
    uint32            soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_HASH_MASK_INFO_clear(&info);
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_lag_hashing_mask_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* in BCM BCM_HASH_FIELD_* means use in hash, in PPD SOC_PPC_HASH_MASKS_* means do not use in hash */
    
    switch (bcm_type) {
    case bcmSwitchHashIP4Field0:
        if ((arg & BCM_HASH_FIELD_IP4DST_LO) && (arg & BCM_HASH_FIELD_IP4DST_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV4_DIP; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV4_DIP; 
        }
        if (((arg & BCM_HASH_FIELD_IP4DST_HI) && !(arg & BCM_HASH_FIELD_IP4DST_LO)) || 
            (!(arg & BCM_HASH_FIELD_IP4DST_HI) && (arg & BCM_HASH_FIELD_IP4DST_LO))) {
            rv = BCM_E_PARAM;
        }
        
        if ((arg & BCM_HASH_FIELD_IP4SRC_LO) && (arg & BCM_HASH_FIELD_IP4SRC_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV4_SIP; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV4_SIP; 
        }
        if (((arg & BCM_HASH_FIELD_IP4SRC_HI) && !(arg & BCM_HASH_FIELD_IP4SRC_LO)) || 
            (!(arg & BCM_HASH_FIELD_IP4SRC_HI) && (arg & BCM_HASH_FIELD_IP4SRC_LO))) {
            rv = BCM_E_PARAM;
        }
        
        if (arg & BCM_HASH_FIELD_PROTOCOL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV4_PROTOCOL; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV4_PROTOCOL; 
        }
        
        if (arg & BCM_HASH_FIELD_DSTL4) {
            info.mask &= ~SOC_PPC_HASH_MASKS_L4_DEST_PORT; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_L4_DEST_PORT; 
        }

        if (arg & BCM_HASH_FIELD_SRCL4) {
            info.mask &= ~SOC_PPC_HASH_MASKS_L4_SRC_PORT; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_L4_SRC_PORT; 
        }
        break;
    case bcmSwitchHashIP6Field0:
        if ((arg & BCM_HASH_FIELD_IP6DST_LO) && (arg & BCM_HASH_FIELD_IP6DST_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV6_DIP; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV6_DIP; 
        }
        if (((arg & BCM_HASH_FIELD_IP6DST_HI) && !(arg & BCM_HASH_FIELD_IP6DST_LO)) ||
            (!(arg & BCM_HASH_FIELD_IP6DST_HI) && (arg & BCM_HASH_FIELD_IP6DST_LO))) {
            rv = BCM_E_PARAM;
        }
        
        if ((arg & BCM_HASH_FIELD_IP6SRC_LO) && (arg & BCM_HASH_FIELD_IP6SRC_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV6_SIP; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV6_SIP; 
        }
        if (((arg & BCM_HASH_FIELD_IP6SRC_HI) && !(arg & BCM_HASH_FIELD_IP6SRC_LO)) ||
            (!(arg & BCM_HASH_FIELD_IP6SRC_HI) && (arg & BCM_HASH_FIELD_IP6SRC_LO))) {
            rv = BCM_E_PARAM;
        }
        
        if (arg & BCM_HASH_FIELD_NXT_HDR) {
            info.mask &= ~SOC_PPC_HASH_MASKS_IPV6_PROTOCOL; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_IPV6_PROTOCOL; 
        }
        
        if (arg & BCM_HASH_FIELD_DSTL4) {
            info.mask &= ~SOC_PPC_HASH_MASKS_L4_DEST_PORT; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_L4_DEST_PORT; 
        }

        if (arg & BCM_HASH_FIELD_SRCL4) {
            info.mask &= ~SOC_PPC_HASH_MASKS_L4_SRC_PORT; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_L4_SRC_PORT; 
        }
        break;
    case bcmSwitchHashL2Field0:
        if ((arg & BCM_HASH_FIELD_MACDA_LO) && (arg & BCM_HASH_FIELD_MACDA_MI) && (arg & BCM_HASH_FIELD_MACDA_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MAC_DA; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MAC_DA; 
        }
        if ((arg & (BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI)) && 
            !((arg & BCM_HASH_FIELD_MACDA_LO) && (arg & BCM_HASH_FIELD_MACDA_MI) && (arg & BCM_HASH_FIELD_MACDA_HI))) {
            rv = BCM_E_PARAM;
        }
        
        if ((arg & BCM_HASH_FIELD_MACSA_LO) && (arg & BCM_HASH_FIELD_MACSA_MI) && (arg & BCM_HASH_FIELD_MACSA_HI)) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MAC_SA; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MAC_SA; 
        }
        if ((arg & (BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI)) && 
            !((arg & BCM_HASH_FIELD_MACSA_LO) && (arg & BCM_HASH_FIELD_MACSA_MI) && (arg & BCM_HASH_FIELD_MACSA_HI))) {
            rv = BCM_E_PARAM;
        }
        
        if (arg & BCM_HASH_FIELD_VLAN) {
            info.mask &= ~SOC_PPC_HASH_MASKS_VSI; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_VSI; 
        }
        
        if (arg & BCM_HASH_FIELD_ETHER_TYPE) {
            info.mask &= ~SOC_PPC_HASH_MASKS_ETH_TYPE_CODE; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_ETH_TYPE_CODE; 
        }
        break;
    case bcmSwitchHashMPLSField0:
        if (arg & BCM_HASH_MPLS_FIELD_TOP_LABEL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MPLS_LABEL_1; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MPLS_LABEL_1; 
        }

        if (arg & BCM_HASH_MPLS_FIELD_2ND_LABEL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MPLS_LABEL_2; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MPLS_LABEL_2; 
        }

        if (arg & BCM_HASH_MPLS_FIELD_3RD_LABEL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MPLS_LABEL_3; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MPLS_LABEL_3; 
        }
        break;
    case bcmSwitchHashMPLSField1:
        if (arg & BCM_HASH_MPLS_FIELD_TOP_LABEL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MPLS_LABEL_4; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MPLS_LABEL_4; 
        }

        if (arg & BCM_HASH_MPLS_FIELD_2ND_LABEL) {
            info.mask &= ~SOC_PPC_HASH_MASKS_MPLS_LABEL_5; 
        }
        else {
            info.mask |= SOC_PPC_HASH_MASKS_MPLS_LABEL_5; 
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_lag_hashing_mask_set(soc_sand_dev_id, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_lag_hash_mask_get(int unit, bcm_switch_control_t bcm_type, 
                                    int *arg)
{
    int                 rv = BCM_E_NONE;
    SOC_PPC_HASH_MASK_INFO  info;
    uint32           soc_sand_dev_id;
    uint32            soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_HASH_MASK_INFO_clear(&info);
    *arg = 0;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_lag_hashing_mask_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchHashIP4Field0:
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV4_DIP)) {
            *arg |= BCM_HASH_FIELD_IP4DST_LO | BCM_HASH_FIELD_IP4DST_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV4_SIP)) {
            *arg |= BCM_HASH_FIELD_IP4SRC_LO | BCM_HASH_FIELD_IP4SRC_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV4_PROTOCOL)) {
            *arg |= BCM_HASH_FIELD_PROTOCOL;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_L4_DEST_PORT)) {
            *arg |= BCM_HASH_FIELD_DSTL4;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_L4_SRC_PORT)) {
            *arg |= BCM_HASH_FIELD_SRCL4;
        }
        break;
    case bcmSwitchHashIP6Field0:
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV6_DIP)) {
            *arg |= BCM_HASH_FIELD_IP6DST_LO | BCM_HASH_FIELD_IP6DST_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV6_SIP)) {
            *arg |= BCM_HASH_FIELD_IP6SRC_LO | BCM_HASH_FIELD_IP6SRC_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_IPV6_PROTOCOL)) {
            *arg |= BCM_HASH_FIELD_NXT_HDR;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_L4_DEST_PORT)) {
            *arg |= BCM_HASH_FIELD_DSTL4;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_L4_SRC_PORT)) {
            *arg |= BCM_HASH_FIELD_SRCL4;
        }
        break;
    case bcmSwitchHashL2Field0:
        if (!(info.mask & SOC_PPC_HASH_MASKS_MAC_DA)) {
            *arg |= BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_MAC_SA)) {
            *arg |= BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_VSI)) {
            *arg |= BCM_HASH_FIELD_VLAN;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_ETH_TYPE_CODE)) {
            *arg |= BCM_HASH_FIELD_ETHER_TYPE;
        }
        break;
    case bcmSwitchHashMPLSField0:
        if (!(info.mask & SOC_PPC_HASH_MASKS_MPLS_LABEL_1)) {
            *arg |= BCM_HASH_MPLS_FIELD_TOP_LABEL;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_MPLS_LABEL_2)) {
            *arg |= BCM_HASH_MPLS_FIELD_2ND_LABEL;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_MPLS_LABEL_3)) {
            *arg |= BCM_HASH_MPLS_FIELD_3RD_LABEL;
        }
        break;
    case bcmSwitchHashMPLSField1:
        if (!(info.mask & SOC_PPC_HASH_MASKS_MPLS_LABEL_4)) {
            *arg |= BCM_HASH_MPLS_FIELD_TOP_LABEL;
        }
        if (!(info.mask & SOC_PPC_HASH_MASKS_MPLS_LABEL_5)) {
            *arg |= BCM_HASH_MPLS_FIELD_2ND_LABEL;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ecmp_bcm_hash_to_ppd_hash(int unit, int bcm_hash_config, 
                              uint8 *dpp_hash_func_id)
{
    int rv = BCM_E_NONE;
    int hash_function_found;

    BCMDNX_INIT_FUNC_DEFS;

    if (dpp_hash_func_id == NULL) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL pointer")));
    }

    hash_function_found = FALSE;
#ifdef BCM_88660_A0
    /* Unsupported polynoms after Arad plus */
    if (SOC_IS_ARADPLUS(unit)) {
        switch (bcm_hash_config) {
        case BCM_HASH_CONFIG_CRC16_0x1eb21:
        case BCM_HASH_CONFIG_CRC16_0x13965:
        case BCM_HASH_CONFIG_CRC16_0x1698D:
        case BCM_HASH_CONFIG_CRC16_0x1105D:
            LOG_ERROR(BSL_LS_BCM_SWITCH,
                      (BSL_META_U(unit,
                                  "unit %d. Invalid hash_config for devices after BCM886660 (%d)\n"), unit, bcm_hash_config));
            rv = BCM_E_PARAM;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8003:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x8003;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8011:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x8011;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8423:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x8423;
            break;
        case BCM_HASH_CONFIG_CRC16_0x8101:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x8101;
            break;
        case BCM_HASH_CONFIG_CRC16_0x84a1:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x84a1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x9019:
            hash_function_found = TRUE;
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_0x9019;
            break;
        default:
            break;
        }
    }
#endif /* BCM_88660_A0 */
    if (!hash_function_found) {
       switch (bcm_hash_config) {
        case BCM_HASH_CONFIG_CRC16_0x10861:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_10861;
            break;
        case BCM_HASH_CONFIG_CRC16_0x10285:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_10285;
            break;
        case BCM_HASH_CONFIG_CRC16_0x101a1:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_101A1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x12499:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_12499;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1f801:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_1F801;
            break;
        case BCM_HASH_CONFIG_CRC16_0x172e1:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_172E1;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1eb21:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_1EB21;
            break;
        case BCM_HASH_CONFIG_CRC16_0x13965:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_13965;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1698D:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_1698D;
            break;
        case BCM_HASH_CONFIG_CRC16_0x1105D:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_CRC_1105D;
            break;
        case BCM_HASH_CONFIG_NONE:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_KEY;
            break;
        case BCM_HASH_CONFIG_ROUND_ROBIN:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_ROUND_ROBIN;
            break;
        case BCM_HASH_CONFIG_CLOCK_BASED:
            *dpp_hash_func_id = SOC_PPC_FEC_LB_2_CLOCK;
            break;
        default:
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid hash_config (%d)\n"), unit, bcm_hash_config));
            rv = BCM_E_PARAM;
            break;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ecmp_ppd_hash_to_bcm_hash(int unit, int dpp_hash_func_id, 
                              int *bcm_hash_config)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (bcm_hash_config == NULL) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL pointer")));
    }

    switch (dpp_hash_func_id) {
    case SOC_PPC_FEC_LB_CRC_10861:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x10861;
        break;
    case SOC_PPC_FEC_LB_CRC_10285:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x10285;
        break;
    case SOC_PPC_FEC_LB_CRC_101A1:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x101a1;
        break;
    case SOC_PPC_FEC_LB_CRC_12499:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x12499;
        break;
    case SOC_PPC_FEC_LB_CRC_1F801:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1f801;
        break;
    case SOC_PPC_FEC_LB_CRC_172E1:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x172e1;
        break;
    case SOC_PPC_FEC_LB_CRC_1EB21:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1eb21;
        break;
    case SOC_PPC_FEC_LB_CRC_13965:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x13965;
        break;
    case SOC_PPC_FEC_LB_CRC_1698D:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1698D;
        break;
    case SOC_PPC_FEC_LB_CRC_1105D:
        *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x1105D;
        break;
    case SOC_PPC_FEC_LB_KEY:
        *bcm_hash_config = BCM_HASH_CONFIG_NONE;
        break;
    case SOC_PPC_FEC_LB_ROUND_ROBIN:
        *bcm_hash_config = BCM_HASH_CONFIG_ROUND_ROBIN;
        break;
    case SOC_PPC_FEC_LB_2_CLOCK:
        *bcm_hash_config = BCM_HASH_CONFIG_CLOCK_BASED;
        break;
    default:
#ifdef BCM_88660_A0
        /* Supported polynoms after Arad plus */
        if (SOC_IS_ARADPLUS(unit)) {
            switch (dpp_hash_func_id) {
            case SOC_PPC_FEC_LB_CRC_0x8003:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8003;
                break;
            case SOC_PPC_FEC_LB_CRC_0x8011:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8011;
                break;
            case SOC_PPC_FEC_LB_CRC_0x8423:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8423;
                break;
            case SOC_PPC_FEC_LB_CRC_0x8101:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x8101;
                break;
            case SOC_PPC_FEC_LB_CRC_0x84a1:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x84a1;
                break;
            case SOC_PPC_FEC_LB_CRC_0x9019:
                *bcm_hash_config = BCM_HASH_CONFIG_CRC16_0x9019;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_SWITCH,
                          (BSL_META_U(unit,
                                      "unit %d. Invalid hash_config (%d)\n"),
                           unit, dpp_hash_func_id));
                 rv = BCM_E_INTERNAL;
                break;
            }
        } else
#endif /* BCM_88660_A0 */
        {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid hash_config\n"), unit));
            rv = BCM_E_PARAM;
        }
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_hash_global_set(int                  unit,
                                     bcm_switch_control_t bcm_type,
                                     int                  arg)
{
    int                                        rv = BCM_E_NONE;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO    info;
    uint32                                     soc_sand_rv;
    uint32                                     soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchECMPHashConfig:
        rv = _bcm_dpp_ecmp_bcm_hash_to_ppd_hash(unit, arg, &info.hash_func_id);
        break;
    case bcmSwitchECMPSecondHierHashConfig:
        rv = _bcm_dpp_ecmp_bcm_hash_to_ppd_hash(unit, arg, &info.hash_func_id_second_hier);
        break;
    case bcmSwitchECMPHashOffset:
    case bcmSwitchECMPSecondHierHashOffset:
        if ((arg < 0) || (arg > ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HashOffset(%d). "
                                 "Valid values: 0-%d\n"), unit, arg, ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX));
        } else if (bcm_type == bcmSwitchECMPHashOffset) {
            info.key_shift             = arg;
        } else {
            info.key_shift_second_hier = arg;
        }
        break;
    case bcmSwitchECMPHashSeed:
        if (arg > ARAD_PP_FRWRD_FEC_SEED_MAX) {
           LOG_WARN(BSL_LS_BCM_SWITCH,
                    (BSL_META_U(unit,
                                "unit %d: ignoring upper bytes for hash seed.\n"), unit));
        }
        info.seed = arg & ARAD_PP_FRWRD_FEC_SEED_MAX; /* take only the one/two bytes */
        break;
    case bcmSwitchECMPHashSrcPortEnable:
        if ((arg != 0) && (arg != 1)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d. Invalid value (%d) specified "
                                 "Valid values 0 & 1 \n"), unit, arg));
            rv = BCM_E_PARAM;
        } else {
            info.use_port_id = arg;
        }
        break;
    default:
        rv = BCM_E_PARAM;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_global_info_set(soc_sand_dev_id, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_hash_global_get(int                  unit,
                                     bcm_switch_control_t bcm_type,
                                     int                  *arg)
{
    int                                      rv = BCM_E_NONE;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO  info;
    uint32                                   soc_sand_rv;
    uint32                                   soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchECMPHashConfig:
        rv = _bcm_dpp_ecmp_ppd_hash_to_bcm_hash(unit, info.hash_func_id, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPSecondHierHashConfig:
        rv = _bcm_dpp_ecmp_ppd_hash_to_bcm_hash(unit, info.hash_func_id_second_hier, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPHashOffset:
    case bcmSwitchECMPSecondHierHashOffset:
        *arg = (bcm_type == bcmSwitchECMPHashOffset) ? info.key_shift : info.key_shift_second_hier;
        if ((*arg < 0) || (*arg > ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HashOffset(%d) "
                                 "configured. Valid values: 0-%d\n"), unit, *arg, ARAD_PP_FRWRD_FEC_HASH_FUNC_ID_MAX));
            rv = BCM_E_INTERNAL;
        }
        break;
    case bcmSwitchECMPHashSeed:
        *arg = info.seed;
        break;
    case bcmSwitchECMPHashSrcPortEnable:
        *arg = info.use_port_id;
        break;
    default:
        rv = BCM_E_PARAM;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_hash_port_set(int unit, bcm_port_t port, 
                                    bcm_switch_control_t bcm_type, int arg)
{
    int                                  rv = BCM_E_NONE, core;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO    info;
    uint32                            soc_sand_dev_id;
    uint32                             soc_sand_rv;
    bcm_port_t                           local_port = 0;
    SOC_PPC_PORT                             soc_ppd_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_LOCAL(port)) {
            local_port = BCM_GPORT_LOCAL_GET(port);
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Unsupported gport type"
                                 "specified. Only BCM_GPORT_LOCAL is supported"), unit));
            rv = BCM_E_PARAM;
        }
    } else {
        local_port = port;
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

    if (soc_ppd_port > SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)) {
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d: Invalid local port(%d/%d) specified."
                             "Valid values: 0-%d\n"), unit, local_port, soc_ppd_port, SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)));
        rv = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchECMPHashPktHeaderCount:
        if (  (arg < ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MIN) || (arg > ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MAX)) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderCount(%d) "
                                 "specified. Valid values: %d-%d\n"), unit, arg, ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MIN, ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MAX));
            rv = BCM_E_PARAM;
        } else {
            info.nof_headers = arg;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    if (BCM_SUCCESS(rv)) {
        soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_port_info_set(soc_sand_dev_id, core, soc_ppd_port, &info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_ecmp_hash_port_get(int unit, bcm_port_t port, 
                                    bcm_switch_control_t bcm_type, int *arg)
{
    int                                rv = BCM_E_NONE, core;
    SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO  info;
    uint32                          soc_sand_dev_id;
    uint32                           soc_sand_rv;
    bcm_port_t                         local_port = 0;
    SOC_PPC_PORT                           soc_ppd_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_LOCAL(port)) {
            local_port = BCM_GPORT_LOCAL_GET(port);
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Unsupported gport type"
                                 "specified. Only BCM_GPORT_LOCAL is supported"), unit));
            rv = BCM_E_PARAM;
        }
    } else {
        local_port = port;
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

    if (soc_ppd_port > SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)) {
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d: Invalid local port(%d) to ppd port (%d) specified."
                             "Valid values: 0-%d\n"), unit, local_port, soc_ppd_port, SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core)));
        rv = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(&info);
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(soc_sand_dev_id, core, soc_ppd_port, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch (bcm_type) {
    case bcmSwitchECMPHashPktHeaderCount:
        if (info.nof_headers > ARAD_PPC_FRWRD_FEC_NOF_HEADERS_MAX) {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit %d: Invalid HeaderCount(%d) "
                                 "configured. \n"), unit, info.nof_headers));
            rv = BCM_E_PARAM;
        } else {
            *arg = info.nof_headers;
        }
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_switch_control_color_select_port_set(int unit, bcm_port_t port, int arg)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_PPC_PORT soc_ppd_port;
    int port_i, core;
    SOC_PPC_LLP_COS_PORT_INFO port_info;
    uint8 use_dei;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_LLP_COS_PORT_INFO_clear(&port_info);

    /* Verify arg */
    if (arg != BCM_COLOR_PRIORITY && arg != BCM_COLOR_OUTER_CFI) {
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d. Invalid arg (%d)\n"),
                  unit, arg));
       BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    use_dei = (arg == BCM_COLOR_OUTER_CFI) ? TRUE : FALSE;

    /* Verify port */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        port_info.l2_info.use_dei = use_dei;

        soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_switch_control_color_select_port_get(int unit, bcm_port_t port, int *arg)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_PPC_PORT soc_ppd_port;
    int port_i, core;
    SOC_PPC_LLP_COS_PORT_INFO port_info;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_LLP_COS_PORT_INFO_clear(&port_info);

    BCMDNX_NULL_CHECK(arg);

    /* Verify port */
    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        /* Only one iteration is needed */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_llp_cos_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *arg = (port_info.l2_info.use_dei) ? BCM_COLOR_OUTER_CFI : BCM_COLOR_PRIORITY;

        soc_sand_rv = soc_ppd_llp_cos_port_info_set(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_trill_ethertype_set
 * Purpose:
 *      Configure trill ethertype.
 * Parameters:
 *      unit - (IN) Unit number.
 *      arg - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_dpp_trill_ethertype_set(int unit, int arg)
{
    uint32                              soc_sand_rv;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO     glbl_info;
    uint32                              soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    glbl_info.ethertype = arg;

    soc_sand_rv = soc_ppd_frwrd_trill_global_info_set(soc_sand_dev_id, &glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      _bcm_dpp_trill_ethertype_set
 * Purpose:
 *      Configure trill ethertype.
 * Parameters:
 *      unit - (IN) Unit number.
 *      arg - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_dpp_trill_ethertype_get(int unit, int *arg)
{
    uint32                              soc_sand_rv;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO       glbl_info;
    uint32                              soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *arg = glbl_info.ethertype;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_mim_bvid_range_set(int unit, int high, int low)
{
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    SOC_PPC_BMACT_PBB_TE_VID_RANGE bvid_ranges;

    BCMDNX_INIT_FUNC_DEFS;
    /* make sure Mac-in-MAc is initialized on the device */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    /* get prev B-VID ranges */
    soc_sand_rv = soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(soc_sand_dev_id, &bvid_ranges);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* update ranges */
    if (low != -1) {
        bvid_ranges.first_vid = (SOC_SAND_PP_VLAN_ID)low;
    }
    if (high != -1) {
        bvid_ranges.last_vid = (SOC_SAND_PP_VLAN_ID)high;
    }

    /* set new ranges */
    soc_sand_rv = soc_ppd_frwrd_bmact_pbb_te_bvid_range_set(soc_sand_dev_id, &bvid_ranges);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_mim_bvid_range_get(int unit, int *high, int *low)
{
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    SOC_PPC_BMACT_PBB_TE_VID_RANGE bvid_ranges;

    BCMDNX_INIT_FUNC_DEFS;
    /* make sure Mac-in-MAc is initialized on the device */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    /* get prev B-VID ranges */
    soc_sand_rv = soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(soc_sand_dev_id, &bvid_ranges);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (low != NULL) {
        *low = bvid_ranges.first_vid;
    }
    if (high != NULL) {
        *high = bvid_ranges.last_vid;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_routed_learning_set(int unit, int bcmAppFlags)
{
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    uint32 ppdAppFlags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    if (bcmAppFlags & BCM_SWITCH_CONTROL_L3_LEARN_IPV4_UC) {
        ppdAppFlags |= SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_UC;
    }
    if (bcmAppFlags & BCM_SWITCH_CONTROL_L3_LEARN_IPV4_MC) {
        ppdAppFlags |= SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_MC;
    }
    if (bcmAppFlags & BCM_SWITCH_CONTROL_L3_LEARN_IPV6_UC) {
        ppdAppFlags |= SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_UC;
    }
    if (bcmAppFlags & BCM_SWITCH_CONTROL_L3_LEARN_IPV6_MC) {
        ppdAppFlags |= SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_MC;
    }
    if (bcmAppFlags & BCM_SWITCH_CONTROL_L3_LEARN_MPLS) {
        ppdAppFlags |= SOC_PPC_FRWRD_MACT_L3_LEARN_MPLS;
    }

    soc_sand_rv = soc_ppd_frwrd_mact_routed_learning_set(soc_sand_dev_id, ppdAppFlags);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_routed_learning_get(int unit, int *bcmAppFlags)
{
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    uint32 ppdAppFlags;
    uint32 bcmAppTypes = 0;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_frwrd_mact_routed_learning_get(soc_sand_dev_id, &ppdAppFlags);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (ppdAppFlags & SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_UC) {
        bcmAppTypes |= BCM_SWITCH_CONTROL_L3_LEARN_IPV4_UC;
    }
    if (ppdAppFlags & SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_MC) {
        bcmAppTypes |= BCM_SWITCH_CONTROL_L3_LEARN_IPV4_MC;
    }
    if (ppdAppFlags & SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_UC) {
        bcmAppTypes |= BCM_SWITCH_CONTROL_L3_LEARN_IPV6_UC;
    }
    if (ppdAppFlags & SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_MC) {
        bcmAppTypes |= BCM_SWITCH_CONTROL_L3_LEARN_IPV6_MC;
    }
    if (ppdAppFlags & SOC_PPC_FRWRD_MACT_L3_LEARN_MPLS) {
        bcmAppTypes |= BCM_SWITCH_CONTROL_L3_LEARN_MPLS;
    }

    *bcmAppFlags = bcmAppTypes;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#if defined(BCM_RCPU_SUPPORT)

/*
 * Function:
 *      _bcm_arad_rcpu_switch_regall_get
 * Description:
 *      Helper function to return arg = 1 if register is set to all ones 
 *  and to zero otherwise.
 * Parameters:
 *      unit - Device unit number
 *  reg - The desired register to read
 *  arg - The value returnrd
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_arad_rcpu_switch_regall_idx_get(int unit, soc_reg_t reg, int idx, int *arg)
{
    uint32  raddr, rval;

    BCMDNX_INIT_FUNC_DEFS;

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, idx);
    BCMDNX_IF_ERR_EXIT(
        soc_pci_getreg(unit, raddr, &rval));
    
    *arg = (!rval) ? 0 : 1;

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_arad_rcpu_switch_regall_get(int unit, soc_reg_t reg, int *arg)
{
    return _bcm_arad_rcpu_switch_regall_idx_get(unit, reg, 0, arg);
}

/*
 * Function:
 *      _bcm_arad_rcpu_switch_vlan_tpid_sig_set
 * Description:
 *      Helper function to set VLAN, TPID ot Signature for RCPU  
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to modify
 *  arg - The value to set
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_arad_rcpu_switch_vlan_tpid_sig_set(int unit, 
                                   bcm_switch_control_t type, 
                                   int arg)
{
    soc_reg_t   reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32      value, raddr, rval;

    BCMDNX_INIT_FUNC_DEFS;

    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    switch (type) {
    case bcmSwitchRemoteCpuVlan:
        reg = CMIC_PKT_VLANr;
        field = VLAN_IDf;
        value = (arg & 0xffff);
        break;
    case bcmSwitchRemoteCpuTpid:
        reg = CMIC_PKT_VLANr;
        field = TPIDf;
        value = (arg & 0xffff);
        break;
    case bcmSwitchRemoteCpuSignature:
        reg = CMIC_PKT_ETHER_SIGr;
        field = SIGNATUREf;
        value = (arg & 0xffff);
        break;
    case bcmSwitchRemoteCpuEthertype:
        reg = CMIC_PKT_ETHER_SIGr;
        field = ETHERTYPEf;
        value = (arg & 0xffff);
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, raddr, &rval));
    soc_reg_field_set(unit, reg, &rval, field, value);

    BCMDNX_IF_ERR_EXIT(soc_pci_write(unit, raddr, rval));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_rcpu_switch_vlan_tpid_sig_get
 * Description:
 *      Helper function to get VLAN, TPID ot Signature for RCPU  
 * Parameters:
 *      unit - Device unit number
 *  type - The desired configuration parameter to get
 *  arg - The value returned
 * Returns:
 *      BCM_E_xxx
 */
STATIC int 
_bcm_arad_rcpu_switch_vlan_tpid_sig_get(int unit, 
                                   bcm_switch_control_t type, 
                                   int *arg)
{
    soc_reg_t   reg;
    soc_field_t field;
    uint32  raddr, rval;

    BCMDNX_INIT_FUNC_DEFS;

    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    switch (type) {
    case bcmSwitchRemoteCpuVlan:
        reg = CMIC_PKT_VLANr;
        field = VLAN_IDf;
        break;
    case bcmSwitchRemoteCpuTpid:
        reg = CMIC_PKT_VLANr;
        field = TPIDf;
        break;
    case bcmSwitchRemoteCpuSignature:
        reg = CMIC_PKT_ETHER_SIGr;
        field = SIGNATUREf;
        break;
    case bcmSwitchRemoteCpuEthertype:
        reg = CMIC_PKT_ETHER_SIGr;
        field = ETHERTYPEf;
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    raddr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, raddr, &rval));

    *arg = soc_reg_field_get(unit, reg, rval, field);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_rcpu_switch_mac_lo_set
 * Description:
 *      Set the low 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set the mac for
 *      mac_lo      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_arad_rcpu_switch_mac_lo_set(int unit,
                            bcm_switch_control_t type,
                            uint32 mac_lo)
{
    uint32      regval, fieldval;
    soc_reg_t   reg = INVALIDr ;
    soc_field_t f_lo = INVALIDf;

    BCMDNX_INIT_FUNC_DEFS;

    /* Given control type select register. */
    switch (type) {
    case bcmSwitchRemoteCpuLocalMacNonOui:
        reg = CMIC_PKT_LMAC0_LOr;
        f_lo = MAC0_LOf;
        break;
    case bcmSwitchRemoteCpuDestMacNonOui:
        reg = CMIC_PKT_RMACr;
        f_lo = MAC_LOf;
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                      &regval));

    fieldval = soc_reg_field_get(unit, reg, regval, f_lo);
    fieldval = (fieldval & 0xff000000) | (mac_lo & 0xffffff);
    soc_reg_field_set(unit, reg, &regval, f_lo, fieldval);

    BCMDNX_IF_ERR_EXIT(soc_pci_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                         regval));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_rcpu_switch_mac_hi_set
 * Description:
 *      Set the upper 24 bits of MAC address field for RCPU registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set upper MAC for
 *      mac_hi      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_arad_rcpu_switch_mac_hi_set(int unit,
                            bcm_switch_control_t type,
                            uint32 mac_hi)
{
    soc_reg_t   reg1, reg2;
    soc_field_t f_lo, f_hi;
    uint32      fieldval, regval1, regval2, raddr1, raddr2;

    BCMDNX_INIT_FUNC_DEFS;

    reg1 = reg2 = INVALIDr;

    /* Given control type select register. */
    switch (type) {
    case bcmSwitchRemoteCpuLocalMacOui:
        reg1 = CMIC_PKT_LMAC0_LOr;
        reg2 = CMIC_PKT_LMAC0_HIr;
        f_lo = MAC0_LOf;
        f_hi = MAC0_HIf;
        break;
    case bcmSwitchRemoteCpuDestMacOui:
        reg1 = CMIC_PKT_RMACr;
        reg2 = CMIC_PKT_RMAC_HIr;
        f_lo = MAC_LOf;
        f_hi = MAC_HIf;
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    regval1 = regval2 = 0;

    raddr1 = soc_reg_addr(unit, reg1, REG_PORT_ANY, 0);
    raddr2 = soc_reg_addr(unit, reg2, REG_PORT_ANY, 0);

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, raddr1, &regval1));

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, raddr2, &regval2));

    fieldval = soc_reg_field_get(unit, reg1, regval1, f_lo);
    fieldval = (fieldval & 0xffffff) | ((mac_hi & 0xff) << 24);
    soc_reg_field_set(unit, reg1, &regval1, f_lo, fieldval);
    fieldval = (mac_hi >> 8) & 0xffff;
    soc_reg_field_set(unit, reg2, &regval2, f_hi, fieldval);

    BCMDNX_IF_ERR_EXIT(soc_pci_write(unit, raddr1, regval1));
    BCMDNX_IF_ERR_EXIT(soc_pci_write(unit, raddr2, regval2));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_rcpu_switch_mac_lo_get
 * Description:
 *      Get the lower 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the lower MAC address
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_arad_rcpu_switch_mac_lo_get(int unit, 
                            bcm_switch_control_t type,
                            int *arg)
{
    soc_reg_t   reg = INVALIDr ;
    soc_field_t f_lo = INVALIDf; 
    uint32      regval, mac;

    BCMDNX_INIT_FUNC_DEFS;

    /* Given control type select register. */
    switch (type) {
    case bcmSwitchRemoteCpuLocalMacNonOui:
        reg = CMIC_PKT_LMAC0_LOr;
        f_lo = MAC0_LOf;
        break;
    case bcmSwitchRemoteCpuDestMacNonOui:
        reg = CMIC_PKT_RMACr;
        f_lo = MAC_LOf;
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                      &regval));
    mac = soc_reg_field_get(unit, reg, regval, f_lo);

    *arg = (mac << 8) >> 8;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_rcpu_switch_mac_hi_get
 * Description:
 *      Get the upper 24 bits of MAC address field for RCPU MAC registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the upper MAC address
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_arad_rcpu_switch_mac_hi_get(int unit, 
                            bcm_switch_control_t type,
                         int *arg)
{
    soc_reg_t   reg1, reg2;
    soc_field_t f_lo, f_hi;
    uint32      mac, regval1, regval2;

    BCMDNX_INIT_FUNC_DEFS;

    reg1 = reg2 = INVALIDr;


    /* Given control type select register. */
    switch (type) {
    case bcmSwitchRemoteCpuLocalMacOui:
        reg1 = CMIC_PKT_LMAC0_LOr;
        reg2 = CMIC_PKT_LMAC0_HIr;
        f_lo = MAC0_LOf;
        f_hi = MAC0_HIf;
        break;
    case bcmSwitchRemoteCpuDestMacOui:
        reg1 = CMIC_PKT_RMACr;
        reg2 = CMIC_PKT_RMAC_HIr;
        f_lo = MAC_LOf;
        f_hi = MAC_HIf;
        break;
    default:
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, soc_reg_addr(unit, reg1, REG_PORT_ANY, 0),
                      &regval1));
    mac = (soc_reg_field_get(unit, reg1, regval1, f_lo) >> 24);

    BCMDNX_IF_ERR_EXIT(
       soc_pci_getreg(unit, soc_reg_addr(unit, reg2, REG_PORT_ANY, 0),
                      &regval2));

    mac |= (soc_reg_field_get(unit, reg2, regval2, f_hi) << 8);

    *arg = (int)mac;

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* #if defined(BCM_RCPU_SUPPORT) */



uint32
  _arad_pp_frwrd_lem_traverse_check_flush_is_done(int unit,
											SOC_SAND_OUT uint32                               *nof_matched_entries,
											SOC_SAND_OUT uint32                               *waiting_time_in_ms,
											SOC_SAND_OUT int                                  *is_wait_success);
STATIC int
_bcm_dpp_switch_flp_not_found_trap_set(int unit, int trap_id)
{
    int rv = BCM_E_NONE;
    int hw_trap_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_rx_trap_sw_id_to_hw_id(unit, trap_id, &hw_trap_id);
    BCMDNX_IF_ERR_EXIT(rv);


    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_flp_not_found_trap_config_set, (unit, hw_trap_id));
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_switch_flp_not_found_trap_get(int unit, int *trap_id)
{
    int rv = BCM_E_NONE;
    int hw_trap_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_flp_not_found_trap_config_get, (unit, &hw_trap_id));
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_rx_trap_hw_id_to_sw_id(unit, hw_trap_id, trap_id);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_switch_control_get(int unit,
                            bcm_switch_control_t bcm_type,
                            int *arg)
{
    int         rv = BCM_E_UNAVAIL; /* initialization value should not be changed */
    uint32      soc_sand_dev_id;
    uint32      soc_sand_rv;
    uint32      soc_sand_arg;
    uint64      aux_arg_64;  
    uint8       is_enable = FALSE;
    int         tmp1,tmp2,tmp3,tmp4;
    SOC_PPC_LLP_TRAP_ARP_INFO arp_info;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32      flags;
#endif
    SOC_PPC_MYMAC_TRILL_INFO    trill_info;
    bcm_mac_t                   bcm_mac;
    int                         tmp = 0;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO trill_global_info;
    SOC_PPC_MTR_GROUP_INFO          mtr_group_info;
    SOC_PPC_EG_ENCAP_PWE_GLBL_INFO  pwe_encap_glbl;
    SOC_PPC_EG_ENCAP_GLBL_INFO      eg_encap_glbl;
    SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE aging_info;
    SOC_PPC_FRWRD_IPV4_GLBL_INFO    route_glbl_info;
    uint32  tmp32 = 0;
    int     unsupported_switch = FALSE;
#if defined(INCLUDE_KBP)
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO  control_info;
#endif /* defined(INCLUDE_KBP) */
    int     fabric_synce_enable;
#ifdef BCM_JERICHO_SUPPORT 
    uint32 reg_val;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(arg);

    soc_sand_dev_id = (unit);

    switch (bcm_type)
    {
    case bcmSwitchUnknownMcastToCpu:
        break;
    case bcmSwitchControlSync:
        break;
    case bcmSwitchCpuCopyDestination:
        break;
    case bcmSwitchCpuProtoBpduPriority:
        break;
    case bcmSwitchStgInvalidToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_STP_STATE_BLOCK, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_STP_STATE_LEARN, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchUnknownUcastToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0, DPP_SWITCH_TRAP_TO_CPU, &tmp1);
        rv = _dpp_trap_code_switch_control_get(unit, 
                                            SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1, DPP_SWITCH_TRAP_TO_CPU, &tmp2);
        rv = _dpp_trap_code_switch_control_get(unit, 
                                            SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2, DPP_SWITCH_TRAP_TO_CPU, &tmp3);
        rv = _dpp_trap_code_switch_control_get(unit, 
                                            SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3, DPP_SWITCH_TRAP_TO_CPU, &tmp4);
        if (tmp1==tmp2 && tmp1==tmp3 && tmp1==tmp4)
        {
          *arg = tmp1;
        }
        else
        {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not all DA-Not-found trap codes are in the same state: enabled or disabled")));
        }
        break;
    case bcmSwitchNonIpL3ErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                            SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchSourceRouteToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

/************************************************************************/
/*                              dos attack                              */
/************************************************************************/
    case bcmSwitchDosAttackSipEqualDip:
    case bcmSwitchDosAttackMinTcpHdrSize:
    case bcmSwitchDosAttackV4FirstFrag:
    case bcmSwitchDosAttackTcpFlags:
    case bcmSwitchDosAttackL4Port:
    case bcmSwitchDosAttackTcpFrag:
    case bcmSwitchDosAttackIcmp:
    case bcmSwitchDosAttackIcmpPktOversize:
    case bcmSwitchDosAttackMACSAEqualMACDA:
    case bcmSwitchDosAttackIcmpV6PingSize:
    case bcmSwitchDosAttackIcmpFragments:
    case bcmSwitchDosAttackTcpOffset:
    case bcmSwitchDosAttackUdpPortsEqual:
    case bcmSwitchDosAttackTcpPortsEqual:
    case bcmSwitchDosAttackTcpFlagsSF:
    case bcmSwitchDosAttackTcpFlagsFUP:
    case bcmSwitchDosAttackTcpHdrPartial:
    case bcmSwitchDosAttackPingFlood:
    case bcmSwitchDosAttackSynFlood:
    case bcmSwitchDosAttackTcpSmurf:
    case bcmSwitchDosAttackTcpXMas:
    case bcmSwitchDosAttackL3Header:
        rv = _dpp_dos_attack_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
    case bcmSwitchIgmpMldToCpu:
        rv = _bcm_dpp_switch_control_get(unit, bcmSwitchIgmpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_get(unit, bcmSwitchMldPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              igmp                                    */
/************************************************************************/
    case bcmSwitchIgmpPktDrop:
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_DROP,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_DROP,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_DROP,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        break;

    case bcmSwitchIgmpPktToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_SNOOP_TO_CPU,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_SNOOP_TO_CPU,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_SNOOP_TO_CPU,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        break;
    case bcmSwitchIgmpQueryToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpQueryDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpQueryFlood:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchIgmpReportLeaveToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpReportLeaveDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpReportLeaveFlood:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownFlood:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoIgmpPriority:
        rv = _dpp_trap_code_switch_control_priority_get(unit, 
                                            SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, &tmp1);
        rv = _dpp_trap_code_switch_control_priority_get(unit, 
                                            SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, &tmp2);
        rv = _dpp_trap_code_switch_control_priority_get(unit, 
                                            SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, &tmp3);
        if (tmp1==tmp2 && tmp1==tmp3)
        {
          *arg = tmp1;
        }
        else
        {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not all IGMP trap codes (membership-query, report-leave-message, undefined) are "
                                                    "in the same state: enabled or disabled")));
        }
        break;
/************************************************************************/
/*                              mld                                     */
/************************************************************************/
    case bcmSwitchMldPktToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_SNOOP_TO_CPU,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_SNOOP_TO_CPU,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        break;
    case bcmSwitchMldPktDrop:
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_DROP,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_DROP,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg==FALSE)
        {
          /* if one of the options isnt enabled return */
          break;
        }
        break;
    case bcmSwitchMldReportDoneToCpu  :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldReportDoneDrop   :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldReportDoneFlood  :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryToCpu       :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryDrop        :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryFlood       :
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              arp                                     */
/************************************************************************/
    case bcmSwitchArpRequestMyStationIPToCPU:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestMyStationIPDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestMyStationIPFwd:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_FWD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ARP, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyMyStationL2ToCPU:
        soc_sand_rv = soc_ppd_llp_trap_arp_info_get(soc_sand_dev_id, &arp_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        *arg = !arp_info.ignore_da;
        break;


/************************************************************************/
    case bcmSwitchArpDhcpToCpu:
        rv = _bcm_dpp_switch_control_get(unit, bcmSwitchDhcpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_get(unit, bcmSwitchArpReplyToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_get(unit, bcmSwitchArpRequestToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV6L3ErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3HeaderErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              DHCPv4                                  */
/************************************************************************/
    case bcmSwitchDhcpPktToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_DHCP_CLIENT, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDhcpPktDrop:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_DHCP_CLIENT, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoDhcpPriority:
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                                        SOC_PPC_TRAP_CODE_DHCP_CLIENT, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/

    case bcmSwitchCpuProtoIpOptionsPriority:
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                                        SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV4L3ErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == FALSE) {
            break;
        }
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoExceptionsPriority:
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                            SOC_PPC_TRAP_CODE_IPV4_TTL1, &tmp1);
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                            SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1, &tmp2);
        if (tmp1==tmp2)
        {
          *arg = tmp1;
        }
        else
        {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not all TTL trap codes are in the same state: enabled or disabled")));
        }
        break;
    case bcmSwitchL3UcastTtl1ToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                          SOC_PPC_TRAP_CODE_IPV4_TTL1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3UcTtlErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TTL0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;    
    case bcmSwitchCpuProtoArpPriority:
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                            SOC_PPC_TRAP_CODE_MY_ARP, &tmp1);
        rv = _dpp_trap_code_switch_control_priority_get(unit, 
                                            SOC_PPC_TRAP_CODE_ARP, &tmp2);
        if (tmp1==tmp2)
        {
          *arg = tmp1;
        }
        else
        {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not all ARP trap codes are in the same state: enabled or disabled")));
        }
        break;
    case bcmSwitchUnknownVlanToCpu:
        rv = _dpp_eg_trap_code_switch_control_get(unit,
                                        SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIcmpRedirectToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_REDIRECT, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuIcmpRedirectPrio:
        rv = _dpp_trap_code_switch_control_priority_get(unit,
                                                        SOC_PPC_TRAP_CODE_ICMP_REDIRECT, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTunnelErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV6L3DstMissToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV4L3DstMissToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMplsLabelMissToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMplsTtlErrToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_TTL0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_get(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_TTL1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3SrcUrpfErrToCpu:
    case bcmSwitchL3UrpfFailToCpu:
        rv = _dpp_trap_code_switch_control_get(unit,
                                        SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL, DPP_SWITCH_TRAP_TO_CPU, &tmp1);
        rv = _dpp_trap_code_switch_control_get(unit,
                                        SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL, DPP_SWITCH_TRAP_TO_CPU, &tmp2);
        if (tmp1 == tmp2)
        {
          *arg = tmp1;
        }
        else
        {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not all RPF trap codes are in the same state: enabled or disabled")));
        }
        break;
    case bcmSwitchSharedVlanMismatchToCpu:
        rv = _dpp_eg_trap_code_switch_control_get(unit,
                                        SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3MtuFailToCpu:
        rv = _dpp_eg_trap_code_switch_control_get(unit,
                                                  SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchL2GreProtocolType:         /* Set protocol-type for L2-GRE */
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* If unknown is mpls then return it instead of l2_gre_prtcl_type*/
        if (eg_encap_glbl.unkown_gre_prtcl_type == 0x8847) {
            *arg = eg_encap_glbl.unkown_gre_prtcl_type;
        } else {
            *arg = eg_encap_glbl.l2_gre_prtcl_type;
        }
        rv = BCM_E_NONE;
        break;

    case bcmSwitchVxlanUdpDestPortSet:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        *arg = eg_encap_glbl.vxlan_udp_dest_port; /* get vxlan udp port */
        rv = BCM_E_NONE;
        break;
    case bcmSwitchSetMplsEntropyLabelOffset:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        *arg = eg_encap_glbl.mpls_entry_label_msbs; /* get MPLS entropy 12 msbs */
        rv = BCM_E_NONE;
        break;
    case bcmSwitchMplsPWControlWord:
        soc_sand_rv = soc_ppd_eg_encap_pwe_glbl_info_get(soc_sand_dev_id, &pwe_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        *arg = pwe_encap_glbl.cw;
        rv = BCM_E_NONE;
        break;
    case bcmSwitchInvalidGreToCpu:
        break;
	case bcmSwitchL2AgeDelete:
		{ /* Abuse this flag to be used for non blocking flush. This flag is used for polling the flush status. (done/in progress)*/
            /* arg will return finished=1/0 indicationg flush machine is done/not done */
			uint32 nof_matched_entries;
			uint32 waiting_time_in_ms;
			if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "flush_non_blocking_when_all_actions_delete", 0)) {
				soc_sand_rv = _arad_pp_frwrd_lem_traverse_check_flush_is_done(unit, &nof_matched_entries, &waiting_time_in_ms, arg);
				BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
				rv = BCM_E_NONE;
			}
		}
		break; 
    case bcmSwitchL2HitClear:
        soc_sand_rv = soc_ppd_frwrd_mact_aging_events_handle_info_get(soc_sand_dev_id, &aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* get clear hit on get*/
        *arg  = aging_info.clear_hit_bit;
        rv = BCM_E_NONE;
        break;
    case bcmSwitchL2Cache:
        break;
    case bcmSwitchL2InvalidCtlToCpu:
        break;
    case bcmSwitchL2StaticMoveToCpu:
        if (SOC_IS_ARADPLUS(unit)) 
        {
            rv = _dpp_static_transplant_switch_control_get(unit, &is_enable);
            BCMDNX_IF_ERR_EXIT(rv);
        
            *arg = (uint32)is_enable;
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: bcmSwitchL2StaticMoveToCpu is not supported before Arad Plus.")));
        }

        break;
    case bcmSwitchL3UrpfMode:
#ifdef BCM_88660_A0
        /* For Arad+ the uRPF mode is per RIF instead of global. */
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When bcm886XX_l3_ingress_urpf_enable is set this switch control can not be used.")));
        }
#endif /* BCM_88660_A0 */
        rv = _bcm_sw_db_switch_urpf_mode_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3UrpfDefaultRoute:
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* get RPF default check*/
        *arg  = route_glbl_info.router_info.rpf_default_route;
        rv = BCM_E_NONE;
        break;
    case bcmSwitchUnknownIpmcAsMcast:
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;
            uint32 programs[] =
                {PROG_FLP_IPV4COMPMC_WITH_RPF, PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF, PROG_FLP_BIDIR,
                 PROG_FLP_IPV6MC, PROG_FLP_IPV6_MC_SSM};
            uint32 nof_programs = sizeof(programs) / sizeof(programs[0]);
            uint32 index = 0;
            uint32 program_hw_id = SOC_DPP_DEFS_GET(unit, nof_flp_programs);

           /* Iterate over all listed programs */
           for (index = 0; index < nof_programs; index++)
           {
                /* Get FLP program hardware id */
                soc_sand_rv = arad_pp_sw_db_flp_prog_app_to_index_get(unit, programs[index], &program_hw_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (program_hw_id >= SOC_DPP_DEFS_GET(unit, nof_flp_programs)){
                    continue;
                }

                /* Get one program configuration */
                soc_sand_rv = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, program_hw_id, &flp_process_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                *arg = flp_process_tbl.compatible_mc_bridge_fallback;
                break;
            }
        } else {
            soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* get clear hit on get*/
            *arg  = route_glbl_info.router_info.flood_unknown_mc;
        }
        rv = BCM_E_NONE;
        break;
    case bcmSwitchL3McastL2:
        if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.ipmc_l3mcastl2_mode == 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When ipmc_l3mcastl2_mode is set the bcmSwitchL3McastL2 can not be used.")));
        }
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* get mc lookup */
        *arg  = route_glbl_info.router_info.ipv4_mc_l2_lookup;
        rv = BCM_E_NONE;
        break;
    case bcmSwitchL2LearnMode:
        rv = _bcm_dpp_switch_L2LearnMode_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
#ifdef BCM_88660_A0

    case bcmSwitchMplsShortPipe:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("This feature is not supported")));
    case bcmSwitchMplsPipeTunnelLabelExpSet:
        if (SOC_IS_ARADPLUS(unit)) {
            uint8 is_exp_copy;

            soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(soc_sand_dev_id, &is_exp_copy);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = !is_exp_copy;
            rv = BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
      break;

    case bcmSwitchMplsDefaultTtlCopy:
    {
        uint8 ttl = 0;
        rv = arad_pp_eg_encap_mpls_default_ttl_get(unit, &ttl);
        BCMDNX_IF_ERR_EXIT(rv);
        *arg = (int) ttl;
        break;
    }

    case bcmSwitchMcastTrunkIngressCommit:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported")));
        break;

    case bcmSwitchMcastTrunkEgressCommit:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported")));
        break; 

#endif /* BCM_88660_A0 */
    case bcmSwitchTraverseMode:
        rv = _bcm_dpp_switch_traverse_mode_get(unit,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3EgressMode:
    case bcmSwitchL3IngressMode:
        *arg = 1;
        rv = BCM_E_NONE;
        break;
    case bcmSwitchL3HostCache:
        /* not supported due to coupling of l3 host and l2 mac databases */
        rv = BCM_E_UNAVAIL;
        break;
    case bcmSwitchL3HostCommit:
        rv = BCM_E_UNAVAIL;
        break;
    case bcmSwitchL3RoutedLearn:
        rv = _bcm_dpp_switch_control_routed_learning_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3RouteCache:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO_PLUS(unit)) {
            uint32 desc_num_max;
            uint32 mem_buff_size;
            uint32 timeout_usec;
            rv = jer_sbusdma_desc_status(unit, &desc_num_max, &mem_buff_size, &timeout_usec);
            *arg = (desc_num_max << 16) | mem_buff_size;
            BCM_SAND_IF_ERR_EXIT(rv);
        } else if (SOC_IS_JERICHO(unit)) {
            rv = BCM_E_UNAVAIL;
        } else
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/
        {
            soc_sand_rv = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(soc_sand_dev_id,
                                                                        DPP_FRWRD_IP_ALL_VRFS_ID, &soc_sand_arg);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = (soc_sand_arg == SOC_PPC_FRWRD_IP_CACHE_MODE_NONE) ? 0 : 1;
        }
        break;
    case bcmSwitchL3RouteCommit:
        break;
    case bcmSwitchFieldCache:
#if defined(INCLUDE_KBP)
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            /* get KBP caching mode */
            SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
            SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_KBP_CACHE;
            soc_sand_rv = soc_ppd_fp_control_get(soc_sand_dev_id,
                                                 SOC_CORE_INVALID,
                                                 &control_ndx,
                                                 &control_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = control_info.val[0];
            rv = BCM_E_NONE;
        } else
#endif /* defined(INCLUDE_KBP) */
        {
            rv = BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchFieldCommit:
        rv = BCM_E_UNAVAIL;
        break;
    case bcmSwitchL3SlowpathToCpu:
        break;
    case bcmSwitchMartianAddr:
        break;
    case bcmSwitchMartianAddrToCpu:
        break;
    case bcmSwitchMcastUnknownErrToCpu:
        break;
    case bcmSwitchMplsInvalidActionToCpu:
        break;
    case bcmSwitchReserveHighEncap:
        break;
    case bcmSwitchReserveHighL2Egress:
        break;
    case bcmSwitchReserveHighMimPort:
        break;
    case bcmSwitchReserveHighMplsPort:
        break;
    case bcmSwitchReserveHighVlanPort:
        break;
    case bcmSwitchReserveHighVpn:
        break;
    case bcmSwitchReserveLowEncap:
        break;
    case bcmSwitchReserveLowL2Egress:
        break;
    case bcmSwitchReserveLowMimPort:
        break;
    case bcmSwitchReserveLowMplsPort:
        break;
    case bcmSwitchReserveLowVlanPort:
        break;
    case bcmSwitchReserveLowVpn:
        break;
    case bcmSwitchSnapNonZeroOui:
        break;

    case bcmSwitchControlAutoSync:
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = sw_state_access[unit].dpp.soc.config.autosync.get(unit, (uint32 *) arg);
        *arg = (*arg && (!SOC_WARM_BOOT(unit)));
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchCrashRecoveryMode:
#ifdef CRASH_RECOVERY_SUPPORT
        rv = soc_dcmn_cr_journaling_mode_get(unit); 
#else
        rv = BCM_E_UNAVAIL;
#endif /* CRASH_RECOVERY_SUPPORT */
        break;

    /* CR Transaction Start get is not supported */
    case bcmSwitchCrTransactionStart:
        rv = BCM_E_UNAVAIL;
        break;

    /* CR commit get is not supported */
    case bcmSwitchCrCommit:
        rv = BCM_E_UNAVAIL;
        break;

    /* CR abort get is not supported */
    case bcmSwitchCrAbort:
        rv = BCM_E_UNAVAIL;
        break;

    case bcmSwitchCrLastTransactionStatus:
#ifdef CRASH_RECOVERY_SUPPORT
        *arg = (int) dcmn_cr_info[unit]->transaction_status;
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchCrCouldNotRecover:
#ifdef CRASH_RECOVERY_SUPPORT
        *arg = (int) dcmn_cr_info[unit]->not_recoverable_reason;
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchStableSelect:
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = soc_stable_get(unit, arg, &flags);
#endif /* BCM_WARM_BOOT_SUPPORT */
        break;
    case bcmSwitchStableSize:
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = soc_stable_size_get(unit, arg);
#endif /* BCM_WARM_BOOT_SUPPORT */
        break;
    case bcmSwitchStableUsed:
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = soc_stable_used_get(unit, arg);
#endif /* BCM_WARM_BOOT_SUPPORT */
        break;
    case bcmSwitchWarmBoot:
#ifdef BCM_WARM_BOOT_SUPPORT
        (*arg) = SOC_WARM_BOOT(unit);
        rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */
        break;
    case bcmSwitchSynchronousPortClockSource:
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 1 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_link_get, (unit, 1, arg));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_sel_port_get, (unit, 0, arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = BCM_E_NONE;
        }
        break;
    case bcmSwitchSynchronousPortClockSourceBkup:
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 0 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_link_get, (unit, 0, arg));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_sel_port_get, (unit, 1, arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = BCM_E_NONE;
        }
        break;
    case bcmSwitchSynchronousPortClockSourceDivCtrl:
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 1 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_divider_get, (unit, 1, arg));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_div_get, (unit, 0, (ARAD_NIF_SYNCE_CLK_DIV*)arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = BCM_E_NONE;
        }
        break;
    case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 0 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_divider_get, (unit, 0, arg));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_div_get, (unit, 1, (ARAD_NIF_SYNCE_CLK_DIV*)arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = BCM_E_NONE;
        }
        break;
    case bcmSwitchECMPSecondHierHashConfig:
    case bcmSwitchECMPSecondHierHashOffset:
        if(!SOC_IS_JERICHO_PLUS_A0(unit) && !SOC_IS_QUX(unit)) {
            unsupported_switch = 1;
            break;
        }
    case bcmSwitchECMPHashConfig:
    case bcmSwitchECMPHashOffset:
    case bcmSwitchECMPHashSeed:
    case bcmSwitchECMPHashSrcPortEnable:
        rv = _bcm_dpp_switch_ecmp_hash_global_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPResilientHashCombine:
        if (SOC_SUPPORTS_ECMP_HASH_COMBINE(unit)) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ecmp_hash_slb_combine_get,(unit, arg)));
            BCMDNX_IF_ERR_EXIT(rv);
        } else{
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchHashELISearch:
        if (SOC_DPP_CONFIG(unit)->pp.parser_mode == 0) {
            unsupported_switch = 1;
            break;
        }
    case bcmSwitchTrunkHashConfig:
    case bcmSwitchTrunkHashOffset:
    case bcmSwitchTrunkHashSeed:
    case bcmSwitchTrunkHashSrcPortEnable:

        rv = _bcm_dpp_switch_trunk_hash_global_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTrunkHashMPLSPWControlWord:
    case bcmSwitchTrunkHashNormalize:
        rv = _bcm_dpp_switch_trunk_hash_mask_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchUseGport:
        *arg = SOC_USE_GPORT(unit);
        rv = BCM_E_NONE;
        break;
    case bcmSwitchHashIP4Field0:
    case bcmSwitchHashIP6Field0:
    case bcmSwitchHashL2Field0:
    case bcmSwitchHashMPLSField0:
    case bcmSwitchHashMPLSField1:
        rv = _bcm_dpp_switch_ecmp_lag_hash_mask_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

/************************************************************************/
/*                              Mac-in-Mac                              */
/************************************************************************/
    case bcmSwitchMiMTeBvidLowerRange:
        rv = _bcm_dpp_switch_mim_bvid_range_get(unit, NULL, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMiMTeBvidUpperRange:
        rv = _bcm_dpp_switch_mim_bvid_range_get(unit, arg, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              Trill                                   */
/************************************************************************/
    case bcmSwitchTrillEthertype:
        rv = _bcm_dpp_trill_ethertype_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case  bcmSwitchTrillBroadcastDestMacOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));
        BCMDNX_IF_ERR_EXIT(rv);
        sal_memcpy(&tmp, bcm_mac, 3);
#ifdef LE_HOST
        tmp = _shr_swap32(tmp);
#endif /* LE_HOST */
        *arg = (tmp >> 8) & 0xFFFFFF;


        break;
    case  bcmSwitchTrillBroadcastDestMacNonOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));
        BCMDNX_IF_ERR_EXIT(rv);

        sal_memcpy(&tmp, &(bcm_mac[3]), 3);
#ifdef LE_HOST
        tmp = _shr_swap32(tmp);
#endif /* LE_HOST */
        *arg = (tmp >> 8) & 0xFFFFFF;


        break;
    case  bcmSwitchTrillEndStationDestMacOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));
        BCMDNX_IF_ERR_EXIT(rv);
        sal_memcpy(&tmp, bcm_mac, 3);

        *arg = tmp >> 8;

        break;
    case  bcmSwitchTrillEndStationDestMacNonOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));
        BCMDNX_IF_ERR_EXIT(rv);
        sal_memcpy(&tmp, &(bcm_mac[3]), 3);
        *arg = tmp >> 8;

        break;
    case  bcmSwitchTrillHopCount:

        soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &trill_global_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *arg = trill_global_info.cfg_ttl;
        break;
    case bcmSwitchPolicerFairness:

        /*configuration shared between cores and groups*/
        soc_sand_rv = soc_ppd_mtr_meters_group_info_get(soc_sand_dev_id, 0/*core*/, 0/*group*/, &mtr_group_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *arg = mtr_group_info.rand_mode_enable;
        break;
/************************************************************************/
/*              Stacking                                                */
/************************************************************************/
    case bcmSwitchTMDomain:
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_tm_domain_get,(unit, &tmp32)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = BCM_E_NONE;
        *arg =  (int)tmp32;
        break;

/************************************************************************/
/*                Congestion                                            */
/************************************************************************/
    case bcmSwitchCongestionCnm:
        if (SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("CNM congestion isn't supported for this device")));
        }
        rv = _bcm_dpp_switch_control_congestion_enable_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
      break;

    case bcmSwitchCongestionMissingCntag:
    case bcmSwitchCongestionExcludeReplications:
    case bcmSwitchCongestionCntag:
    case bcmSwitchCongestionCntagEthertype:
        rv = _bcm_dpp_switch_control_congestion_options_get(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
      break;

/************************************************************************/
/*                RCPU                                                  */
/************************************************************************/
#if defined(BCM_RCPU_SUPPORT)
    case bcmSwitchRemoteCpuSchanEnable:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            uint32 rval;

            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                     ENABLE_SCHAN_REQUESTf);
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuMatchLocalMac:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            uint32 rval;

            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                     LMAC0_MATCHf);
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuCmicEnable:
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            uint32 rval;

            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                     ENABLE_CMIC_REQUESTf);
            return BCM_E_NONE;
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuMatchVlan:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            uint32 rval;

            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            *arg = soc_reg_field_get(unit, CMIC_PKT_CTRLr, rval,
                                     VLAN_MATCHf);
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuForceScheduling:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            int l_arg, h_arg;

            l_arg = h_arg = 0;
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_arad_rcpu_switch_regall_get(unit, CMIC_PKT_COS_0r, &l_arg));
                if (SOC_REG_IS_VALID(unit, CMIC_PKT_COS_1r)) {
                    BCM_IF_ERROR_RETURN(
                       _bcm_arad_rcpu_switch_regall_get(unit, CMIC_PKT_COS_1r, &h_arg));
                }
            } else
#endif
            {
                return BCM_E_UNAVAIL;
            }
            *arg = l_arg | h_arg;

            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuLocalMacOui:
    case bcmSwitchRemoteCpuDestMacOui:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_mac_hi_get(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuLocalMacNonOui:
    case bcmSwitchRemoteCpuDestMacNonOui:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_mac_lo_get(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuEthertype:
    case bcmSwitchRemoteCpuVlan:
    case bcmSwitchRemoteCpuTpid:
    case bcmSwitchRemoteCpuSignature:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_vlan_tpid_sig_get(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;


#endif /* #if  defined(BCM_RCPU_SUPPORT) */

#ifdef BCM_88660_A0

    case bcmSwitchMeterAdjustInterframeGap:

        if (SOC_IS_ARADPLUS(unit)) {
            if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchMeterAdjustInterframeGap is Supported only in Arad+")));
            }else{
                uint8 is_ipg_enabled = 0;

    			BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_ipg_compensation_get,(unit,&is_ipg_enabled)));
                
                *arg  = (is_ipg_enabled ? SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE : SOC_PPC_MTR_IPG_COMPENSATION_DISABLED_SIZE);
                rv = BCM_E_NONE;
            }
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchColorL3L2Marking:
        if (SOC_IS_ARADPLUS(soc_sand_dev_id)) {
             SOC_PPC_EG_QOS_GLOBAL_INFO info;
             int col_idx;
             int flag_num = 4;
             uint32 possible_flags[] = { BCM_PRIO_GREEN, BCM_PRIO_YELLOW, BCM_PRIO_RED, BCM_PRIO_BLACK };
             uint32 bitmap = 0, flags = 0;

             SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

             /* First get the bitmap. */
             soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
             bitmap = info.resolved_dp_bitmap;

             /* We assume that if bit i is on, then possible_flags[i] should be included. */
             /* Set the proper flags according to the bitmap. */
             for (col_idx = 0; col_idx < flag_num; col_idx++) {
                 if (bitmap & (1 << col_idx)) {
                     flags |= possible_flags[col_idx];
                 }
             }

             *arg = flags;

             return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchResilientHashAgeTimer:
        if (SOC_IS_ARADPLUS(unit)) {
            if (soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, FALSE)) {
                uint32 soc_sand_rv;
                SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS aging;

                SOC_PPC_SLB_CLEAR(&aging, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS);
                
                rv = sw_state_access[unit].dpp.soc.arad.pp.slb_config.age_time_in_seconds.get(soc_sand_dev_id, &(aging.age_time_in_seconds));
                BCMDNX_IF_ERR_EXIT(rv);

                soc_sand_rv = soc_ppd_slb_get_global_cfg(soc_sand_dev_id, SOC_PPC_SLB_DOWNCAST(&aging, SOC_PPC_SLB_CONFIGURATION_ITEM));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                *arg = aging.age_time_in_seconds;

                return BCM_E_NONE;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Stateful load balancing is disabled on this device.\n")));
            }
        } else {
            unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchVpnDetachPorts:
        rv = sw_state_access[unit].dpp.bcm.vswitch.vpn_detach_ports.get(soc_sand_dev_id, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTrunkResilientHashConfig:
      if (SOC_IS_ARADPLUS(unit)) {
          SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION lag_hash_func;
		  SOC_PPC_SLB_CLEAR(&lag_hash_func, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION);

          soc_sand_rv = soc_ppd_slb_get_global_cfg(soc_sand_dev_id, SOC_PPC_SLB_DOWNCAST(&lag_hash_func, SOC_PPC_SLB_CONFIGURATION_ITEM));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          
          rv = _bcm_dpp_trunk_ppd_hash_to_bcm_hash(unit, lag_hash_func.hash_function, arg);
          BCMDNX_IF_ERR_EXIT(rv);
      } else {
        unsupported_switch = TRUE;
      }
      break;
    case bcmSwitchTrunkResilientHashSeed:
    case bcmSwitchECMPResilientHashSeed:
	case bcmSwitchTrunkResilientHashOffset:
	case bcmSwitchResilientHashCRCSeed:
      if (SOC_IS_ARADPLUS(unit)) {
        SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET lag_hash_offset;
        SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED lag_hash_seed;
        SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED ecmp_hash_seed;
		SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED crc_hash_seed;
        SOC_PPC_SLB_CONFIGURATION_ITEM *cfg_item = NULL;

        switch (bcm_type) {
        case bcmSwitchTrunkResilientHashSeed:
          SOC_PPC_SLB_CLEAR(&lag_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED);
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&lag_hash_seed;
          break;
        case bcmSwitchECMPResilientHashSeed:
          SOC_PPC_SLB_CLEAR(&ecmp_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED);
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_hash_seed;
          break;
        case bcmSwitchTrunkResilientHashOffset:
          SOC_PPC_SLB_CLEAR(&lag_hash_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET);
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&lag_hash_offset;
          break;
		case bcmSwitchResilientHashCRCSeed:
		  SOC_PPC_SLB_CLEAR(&crc_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED);
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_seed;
          break;

        default:
          BCMDNX_VERIFY(FALSE);
          break;
        }

        soc_sand_rv = soc_ppd_slb_get_global_cfg(soc_sand_dev_id, cfg_item);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        switch (bcm_type) {
        case bcmSwitchTrunkResilientHashSeed:
          *arg = lag_hash_seed.seed;
          break;
        case bcmSwitchECMPResilientHashSeed:
          *arg = ecmp_hash_seed.seed;
          break;
        case bcmSwitchTrunkResilientHashOffset:
          *arg = lag_hash_offset.offset;
          break;
		case bcmSwitchResilientHashCRCSeed:
		  *arg = crc_hash_seed.seed;
		  break;
        /* must default. Oveerwise - compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default:
          BCMDNX_VERIFY(FALSE);
          break;
        }

		return BCM_E_NONE;

      } else {
        unsupported_switch = TRUE;
      }
      break;

	case bcmSwitchECMPResilientHashKey0Shift:
	case bcmSwitchECMPResilientHashKey1Shift:
	case bcmSwitchResilientHashCRC0ByteMask:
	case bcmSwitchResilientHashCRC1ByteMask:
	case bcmSwitchResilientHashCRC2ByteMask:
	case bcmSwitchResilientHashCRC3ByteMask:
      if (SOC_IS_JERICHO(unit)) {
        SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET ecmp_key_offset;
		SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK crc_hash_mask;
        SOC_PPC_SLB_CONFIGURATION_ITEM *cfg_item = NULL;

        switch (bcm_type) {
		case bcmSwitchECMPResilientHashKey0Shift:
			SOC_PPC_SLB_CLEAR(&ecmp_key_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET);
			ecmp_key_offset.index = 0;
			cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_key_offset;
			break;
		case bcmSwitchECMPResilientHashKey1Shift:
            SOC_PPC_SLB_CLEAR(&ecmp_key_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET);
			ecmp_key_offset.index = 1;
			cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_key_offset;
            break;
		case bcmSwitchResilientHashCRC0ByteMask:
			SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
			crc_hash_mask.index = 0;
            cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
            break;
		case bcmSwitchResilientHashCRC1ByteMask:
			SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
			crc_hash_mask.index = 1;
            cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
            break;
		case bcmSwitchResilientHashCRC2ByteMask:
			SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
			crc_hash_mask.index = 2;
            cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
            break;
		case bcmSwitchResilientHashCRC3ByteMask:
            SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
			crc_hash_mask.index = 3;
            cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
            break;
        default:
            BCMDNX_VERIFY(FALSE);
            break;
        }

        soc_sand_rv = soc_ppd_slb_get_global_cfg(soc_sand_dev_id, cfg_item);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        switch (bcm_type) {
		case bcmSwitchECMPResilientHashKey0Shift:
	    case bcmSwitchECMPResilientHashKey1Shift:
          *arg = ecmp_key_offset.offset;
          break;
		case bcmSwitchResilientHashCRC0ByteMask:
		case bcmSwitchResilientHashCRC1ByteMask:
		case bcmSwitchResilientHashCRC2ByteMask:
        case bcmSwitchResilientHashCRC3ByteMask:
		  *arg = crc_hash_mask.mask;
		  break;
        default:
          BCMDNX_VERIFY(FALSE);
          break;
        }

		return BCM_E_NONE;

      } else {
        unsupported_switch = TRUE;
      }
      break;
#endif /* BCM_88660_A0 */

      case bcmSwitchL2LearnLimitToCpu:
        if (SOC_IS_ARADPLUS(unit)){
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get,(unit, &is_enable)));
            (*arg) = (int)(is_enable);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            unsupported_switch = TRUE;
        }
        break;

        case bcmSwitchFcoeNpvModeEnable:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_fcf_npv_switch_get,(unit, arg)));
            break;

/************************************************************************/
/*                              ecn                                   */
/************************************************************************/

    case bcmSwitchEcnDelayMeasurementThreshold:
        if (!soc_property_get(unit, spn_ECN_DM_ENABLE, 0)) 
               BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("ecn_dm_enable must be on for type bcmSwitchEcnDelayMeasurementThreshold.")));
        rv = BCM_E_NONE;
        soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_DELAY_BASED_ECN2, &aux_arg_64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        *arg = (int)COMPILER_64_LO(aux_arg_64);
        ARAD_PRGE_ECN_DM_PROGRAM_VAR_GET(*arg);
        break;

#ifdef BCM_JERICHO_SUPPORT 
    case bcmSwitchPortVlanTranslationAdvanced:
        /* such fields only in Jericho */
        if (SOC_IS_JERICHO(unit)){
            rv = soc_reg_above_64_field32_read(unit, IHP_VTT_GENERAL_CONFIGS_0r, SOC_CORE_ALL, 0, FORCE_VEC_FROM_LIFf, &reg_val);
            BCMDNX_IF_ERR_EXIT( rv) ;
            *arg = reg_val;
        } else {
            unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchGlobalTpidEnable:
        if (SOC_IS_JERICHO(unit))  {
            reg_val = 0;
            rv = soc_reg32_get(unit, IHP_IHP_ENABLERSr, REG_PORT_ANY,
                               0, &reg_val);
            BCMDNX_IF_ERR_EXIT(rv) ;
            *arg = soc_reg_field_get(unit, IHP_IHP_ENABLERSr,
                                     reg_val, ADDITIONAL_TPID_ENABLEf);
        } else {
            unsupported_switch = TRUE;
        }
        break;
#endif
    case bcmSwitchEtagEthertype:
        if (SOC_IS_ARADPLUS(unit)) {
            uint16 etag_tpid;
            /* Get global Etag Ethertype */
            _BCM_DPP_EXTENDER_INIT_CHECK(unit);

            /* Read the tag.*/
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_global_etag_ethertype_get, (unit, &etag_tpid));
            BCMDNX_IF_ERR_EXIT(rv);

            *arg = etag_tpid;
        } else {
            unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchClassL3L2Marking:
        if (SOC_IS_JERICHO_B0(soc_sand_dev_id) || SOC_IS_QMX_B0(soc_sand_dev_id)) {
            SOC_PPC_EG_QOS_GLOBAL_INFO info;

            SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

            /* First get the bitmap. */
            soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = info.resolved_in_lif_profile_bitmap;

            return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchL3L2MarkingMode:
        if (SOC_IS_JERICHO_B0(soc_sand_dev_id) || SOC_IS_QMX_B0(soc_sand_dev_id) || SOC_IS_QAX(soc_sand_dev_id)) {
            SOC_PPC_EG_QOS_GLOBAL_INFO info;

            SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

            /* First get the bitmap. */
            soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = info.dp_map_mode;

            return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchDefaultNativeOutVlanPort:
        /* get default native out AC gport. */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            _bcm_dpp_gport_hw_resources gport_hw_resources;
            uint32 local_outlif = 0; 
            bcm_gport_t gport = 0; 
            
            gport_hw_resources.local_out_lif = _BCM_GPORT_ENCAP_ID_LIF_INVALID; 

            /* get local outlif from HW */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_default_native_ac_outlif_get, (unit, &local_outlif)); 
            
            BCMDNX_IF_ERR_EXIT(rv);
            gport_hw_resources.local_out_lif = (int) local_outlif; 

            /* get native AC gport from local lif */
            rv = _bcm_dpp_gport_from_hw_resources(unit, 
                                                  &gport, 
                                                  _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, 
                                                  &gport_hw_resources); 
            BCMDNX_IF_ERR_EXIT(rv); 
            *arg = (int) gport; 
        } else {
            unsupported_switch = 1;
        }
        break; 
    case bcmSwitchDefaultEgressVlanEditClassId:
        /* get Vlan edit class id (aka vlan edit profile) from default out-AC register */

        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get,(unit, &soc_sand_arg));
        BCMDNX_IF_ERR_EXIT(rv); 

        *arg = (int) soc_sand_arg; 
        
        break; 
#ifdef BCM_QAX_SUPPORT
    case bcmSwitch3rdTagStampMode:
        if (SOC_IS_QAX(unit)) {
            reg_val = 0;
            rv = soc_reg32_get(unit, IHP_IHP_ENABLERSr, REG_PORT_ANY,
                               0, &reg_val);
            BCMDNX_IF_ERR_EXIT(rv) ;
            *arg = soc_reg_field_get(unit, IHP_IHP_ENABLERSr,
                                     reg_val, PRSR_ENCAPSULATION_OR_3RD_TAGf);
        } else {
            unsupported_switch = 1;
        }
        break;
#endif

    case bcmSwitchUdpTunnelIPv4DstPort:
    case bcmSwitchUdpTunnelIPv6DstPort:
    case bcmSwitchUdpTunnelMplsDstPort:
       if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable) {
          ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE arad_dst_port_type;
          rv =  _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_parser(unit, bcm_type, &arad_dst_port_type);
          soc_sand_rv = arad_parser_udp_tunnel_dst_port_get(unit, arad_dst_port_type, arg);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Udp tunnel not supported on this device")));
       }
       break;
    case bcmSwitchIngressRateLimitMpps:
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_rate_limit_mpps_get,(unit, arg));
        BCMDNX_IF_ERR_EXIT(rv);
        break;

#ifdef BCM_JERICHO_SUPPORT
    case bcmSwitchMirrorSnoopForwardOriginalWhenDropped:
        if (SOC_IS_QAX(unit) || !SOC_IS_JERICHO(unit)) { /* supported only for Jer/QMX/Jer+/QMX+ */
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmSwitchMirrorSnoopForwardOriginalWhenDropped supported only for BCM88{3,6}{7,8}x.")));
        }
        BCMDNX_IF_ERR_EXIT(jer_mgmt_mirror_snoop_forward_original_when_dropped_get(unit, &is_enable));
        *arg = is_enable;
        BCM_EXIT;
#endif /* BCM_JERICHO_SUPPORT */
    case bcmSwitchMeterMef10dot3Enable:
        if (SOC_IS_JERICHO(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_global_mef_10_3_get,(unit, arg));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchMeterMef10dot3Enable is Supported only in Jericho and above")));        
        }
        
        break;

    case bcmSwitchForwardLookupNotFoundTrap:
        rv = _bcm_dpp_switch_flp_not_found_trap_get(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchEtherTypeCustom1:
    case bcmSwitchEtherTypeCustom2:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (bcm_type == bcmSwitchEtherTypeCustom1) {
            *arg = eg_encap_glbl.custom1_ether_type;
        } else {
            *arg = eg_encap_glbl.custom2_ether_type;
        }
        rv = BCM_E_NONE;

        break;

    case bcmSwitchNoneEthernetPacketMeterPtr:
        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_none_ethernet_packet_ptr_get,(unit, arg));
        BCMDNX_IF_ERR_EXIT(rv);
        
        break;
    case bcmSwitchEtherDscpRemark:
        if (SOC_IS_JERICHO(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_qos_ether_dscp_remark_get,(unit, arg));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchEtherDscpRemark is Supported only in Jericho and above")));        
        }
        break;
    case bcmSwitchLlcHeaderControlMode:
        if (SOC_IS_JERICHO(unit)) {
            rv = soc_reg_above_64_field32_read(unit, IHP_IHP_ENABLERSr, REG_PORT_ANY, 0, LLC_CHECK_LSBf, (uint32 *)arg);
        } else {
            rv = soc_reg_above_64_field32_read(unit, IHP_IHP_ENABLERSr, REG_PORT_ANY, 0, FIELD_6_6f, (uint32 *)arg);
        }
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchPFCStopInterfaceBitmap:
        if (!SOC_IS_JERICHO(unit) || SOC_IS_FLAIR(unit)) { /* supported only for Jer/QMX/Jer+/QMX+/QAX/QUX */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmSwitchPFCStopInterfaceBitmap supported only for Jericho and above, besides FLAIR.")));
        }

        if(SOC_IS_QUX(unit)) {
            rv = soc_reg_above_64_field32_read(unit, NIF_FC_PFC_RX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Af, (uint32 *)arg);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = soc_reg_above_64_field32_read(unit, NBIH_FC_PFC_RX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Af, (uint32 *)arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        break;

    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control type")));
    }

    if (unsupported_switch) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control type")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int custom_change_tpid_over_prge_var(int unit, int arg)
{
    uint32                             soc_sand_rv;
    uint64 arg64;
    uint32 high;
    uint32 low;
    uint32 tpids[4];
    uint32 value_;
    int index;
    BCMDNX_INIT_FUNC_DEFS;

    value_ = arg & 0x0000ffff;
    index = (arg & 0x00030000) >> 16 ; 

    soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED, &arg64);
    COMPILER_64_TO_32_LO(low, arg64);
    COMPILER_64_TO_32_HI(high, arg64);

    tpids[0] = low & 0x0000ffff;
    tpids[1] = low & 0xffff0000;
    tpids[2] = high & 0x0000ffff;
    tpids[3] = high & 0xffff0000;

    switch (index) {
        case (0):
            tpids[0] = value_;
            break;
        case (1):
            tpids[1] = value_<<16;
            break;
        case (2):
            tpids[2] = value_;
            break; 
        case (3):
            tpids[3] = value_<<16;
            break; 
        default:
            break;
    }

    low = tpids[0] | tpids[1]; 
    high = tpids[2] | tpids[3];
    COMPILER_64_SET(arg64, high, low);

    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_SERVICE_MODE_PTAGGED, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_NOT_PTAGGED, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_PTAGGED_MPLS_RAW, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_SERVICE_MODE_PTAGGED_MPLS_RAW, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_USER_MODE_NOT_PTAGGED_MPLS_RAW, arg64);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int custom_change_rif_over_prge_var(int unit, int arg)
{
    uint32                             soc_sand_rv;
    uint64 prge_var;
    uint32 entry_high;
    uint32 entry_low;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_pp_prge_program_var_get(unit, ARAD_EGR_PROG_EDITOR_PROG_MIM_SPB_UC_ROUTE_TO_OVERLAY, &prge_var);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    entry_low  = COMPILER_64_LO(prge_var);
    entry_high = COMPILER_64_HI(prge_var);

   entry_high |= (arg << 16);

    COMPILER_64_SET(prge_var, entry_high, entry_low);

    soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_MIM_SPB_UC_ROUTE_TO_OVERLAY, prge_var);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
   BCMDNX_FUNC_RETURN;
}


int custom_prge_set_op_val_in_instruction(int unit, int instruction_num,int arg)
{
uint32 soc_sand_rv;
int prog_used;
int entry = -1;
int mem = -1;
ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA 
prge_instruction_tbl_data;
BCMDNX_INIT_FUNC_DEFS;

SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.egr_prog_editor.programs.prog_used.get(
unit, ARAD_EGR_PROG_EDITOR_PROG_TEST1, &prog_used));

if (prog_used != -1) {
    mem = EPNI_PRGE_INSTRUCTION_0m + instruction_num/2; /*instruction is 16th, so its in the 8th memory */
    entry = prog_used*2 +instruction_num%2; /* instruction is second, two instruction per program in each table */

    soc_sand_rv = arad_pp_epni_prge_instruction_tbl_get_unsafe(unit, mem, entry, &prge_instruction_tbl_data);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    prge_instruction_tbl_data.op_value = arg;

    soc_sand_rv = arad_pp_epni_prge_instruction_tbl_set_unsafe(unit, mem, entry, &prge_instruction_tbl_data);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
}else {
    soc_sand_rv = SOC_SAND_ERR;
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
} 
    exit:
    BCMDNX_FUNC_RETURN;
}

   
int
_bcm_dpp_switch_control_set(int unit, 
                            bcm_switch_control_t bcm_type,
                            int arg)
{
    int                         rv = BCM_E_NONE;
    uint32                      soc_sand_dev_id;
    uint32                      soc_sand_rv;
    SOC_SAND_SUCCESS_FAILURE    success;
    SOC_PPC_LLP_TRAP_ARP_INFO   arp_info;
    int                         unsupported_switch = FALSE;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int                     stable_select;
#endif
    SOC_PPC_MYMAC_TRILL_INFO    trill_info;
    bcm_mac_t                   bcm_mac;
    int                         tmp, l2_learn_mode, l3_routed_learn;
    SOC_PPC_FRWRD_TRILL_GLOBAL_INFO trill_global_info;
    SOC_PPC_EG_ENCAP_GLBL_INFO  eg_encap_glbl;
    SOC_PPC_EG_ENCAP_PWE_GLBL_INFO  pwe_encap_glbl;
    SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO pass_info;
    SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE aging_info;
    SOC_PPC_FRWRD_IPV4_GLBL_INFO route_glbl_info;
    SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO defrag_info;
    int     fabric_synce_enable;
#if defined(INCLUDE_KBP)
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
#endif /* defined(INCLUDE_KBP) */
#ifdef BCM_JERICHO_SUPPORT 
    uint32 reg_val;
#endif
	int core_id;
    uint64 arg64;
    uint32 outrif_range_lower_limit = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "outrif_range_lower_limit", 0x1000);

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    switch (bcm_type)
    {
    case bcmSwitchUnknownMcastToCpu:    
    case bcmSwitchWarmBoot:
    case bcmSwitchStableSelect:
    case bcmSwitchStableSize:
    case bcmSwitchStableUsed:
        /* all should be configured through config.bcm */
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported switch control")));
    case bcmSwitchControlSync:
#if defined(BCM_WARM_BOOT_SUPPORT)
        if (!SOC_WARM_BOOT(unit)) {
            rv = _bcm_dpp_switch_control_sync(unit, arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;
    case bcmSwitchControlAutoSync:
#if defined(BCM_WARM_BOOT_SUPPORT)
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_switch_control_get(unit,
                                                    bcmSwitchStableSelect, &stable_select));
        if (BCM_SWITCH_STABLE_NONE != stable_select) {
            SOC_CONTROL_LOCK(unit);
            arg = arg ? 1 : 0;

            /* perform a one time sync to make sure we start autosyncying when everything is in sync */
            /* important - need to call the sync before enabling the autosync flag */
            if (arg) {
                rv = _bcm_dpp_switch_control_sync(unit, arg);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* this flag is used internally and recovered in wb */
            rv = sw_state_access[unit].dpp.soc.config.autosync.set(unit, arg);

            /* this flag is a common soc_control flag that may be used by common modules */
            SOC_CONTROL(unit)->autosync = arg;
            SOC_CONTROL_UNLOCK(unit);

            /* now enable ELK software auto sync */
#if defined(INCLUDE_KBP)
            rv = arad_kbp_autosync_set(unit,0x1);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /* defined(INCLUDE_KBP) */
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
            rv = jer_kaps_autosync_set(unit,0x1);
            BCMDNX_IF_ERR_EXIT(rv);
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/

        } else {
            rv = BCM_E_NOT_FOUND;
        }
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchCrashRecoveryMode:
#ifdef CRASH_RECOVERY_SUPPORT
        if (arg == 0) {
            /* compensate the api counter as once we turned of journaling counter won't
               be decreased at the end of the API */
            BCMDNX_IF_ERR_EXIT(soc_dcmn_cr_api_counter_decrease(unit));
        }

        /* if we are currently logging then we dalay the change in journaling mode until after the next commit */
        if (!soc_dcmn_cr_utils_is_logging(unit)) {
            if(SOC_CR_ENABALED(unit)) {
                dcmn_cr_info[unit]->set_journaling_mode_after_commit = FALSE;
            }

            rv = soc_dcmn_cr_journaling_mode_set(unit, arg);

        }else{

            /* sets the flags that indicates to change the journaling mode after commit */
            dcmn_cr_info[unit]->set_journaling_mode_after_commit = TRUE;
            dcmn_cr_info[unit]->journaling_mode_after_commit = arg;
        }
#else
        rv = BCM_E_UNAVAIL;
#endif /* CRASH_RECOVERY_SUPPORT */
        break;

    case bcmSwitchCrTransactionStart:
#ifdef CRASH_RECOVERY_SUPPORT
        /* start a new transaction */
        rv = soc_dcmn_cr_transaction_start(unit);
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchCrCommit:
#ifdef CRASH_RECOVERY_SUPPORT
        /* end the current transaction */
        rv = soc_dcmn_cr_commit(unit);
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    case bcmSwitchCrAbort:
#ifdef CRASH_RECOVERY_SUPPORT
        /* abort the current transaction */
        rv = soc_dcmn_cr_abort(unit);
#else
        rv = BCM_E_UNAVAIL;
#endif
        break;

    /* Last transaction status setter function not available */
    case bcmSwitchCrLastTransactionStatus:
        rv = BCM_E_UNAVAIL;
        break;

    /* Could not recover setter function not available */
    case bcmSwitchCrCouldNotRecover:
        rv = BCM_E_UNAVAIL;
        break;

    case bcmSwitchCpuCopyDestination:
        break;
    case bcmSwitchCpuProtoBpduPriority:
        break;
    case bcmSwitchStgInvalidToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_STP_STATE_BLOCK, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_STP_STATE_LEARN, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchUnknownUcastToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoExceptionsPriority:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IPV4_TTL1, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3UcastTtl1ToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TTL1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3UcTtlErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TTL0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoArpPriority:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_ARP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_MY_ARP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoIgmpPriority:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchNonIpL3ErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchSourceRouteToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

/************************************************************************/
/*                              dos attack                              */
/************************************************************************/
    case bcmSwitchDosAttackSipEqualDip:
    case bcmSwitchDosAttackMinTcpHdrSize:
    case bcmSwitchDosAttackV4FirstFrag:
    case bcmSwitchDosAttackTcpFlags:
    case bcmSwitchDosAttackL4Port:
    case bcmSwitchDosAttackTcpFrag:
    case bcmSwitchDosAttackIcmp:
    case bcmSwitchDosAttackIcmpPktOversize:
    case bcmSwitchDosAttackMACSAEqualMACDA:
    case bcmSwitchDosAttackIcmpV6PingSize:
    case bcmSwitchDosAttackIcmpFragments:
    case bcmSwitchDosAttackTcpOffset:
    case bcmSwitchDosAttackUdpPortsEqual:
    case bcmSwitchDosAttackTcpPortsEqual:
    case bcmSwitchDosAttackTcpFlagsSF:
    case bcmSwitchDosAttackTcpFlagsFUP:
    case bcmSwitchDosAttackTcpHdrPartial:
    case bcmSwitchDosAttackPingFlood:
    case bcmSwitchDosAttackSynFlood:
    case bcmSwitchDosAttackTcpSmurf:
    case bcmSwitchDosAttackTcpXMas:
    case bcmSwitchDosAttackL3Header:
        rv = _dpp_dos_attack_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpMldToCpu:
        rv = _bcm_dpp_switch_control_set(unit, bcmSwitchIgmpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_set(unit, bcmSwitchMldPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              igmp                                    */
/************************************************************************/
    case bcmSwitchIgmpPktDrop:
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpPktToCpu:
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpQueryToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpQueryDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpQueryFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpReportLeaveToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpReportLeaveDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpReportLeaveFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              mld                                     */
/************************************************************************/
    case bcmSwitchMldPktToCpu:
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldPktDrop:
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldReportDoneToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldReportDoneDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldReportDoneFlood:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, DPP_SWITCH_FLOOD, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              DHCPv4                                  */
/************************************************************************/
    case bcmSwitchDhcpPktToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DHCP_CLIENT, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDhcpPktDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_DHCP_CLIENT, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoDhcpPriority:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_DHCP_CLIENT, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                              arp                                     */
/************************************************************************/
        /* trap ARP with IP = arg
         *   1. config MY-ARP trap to trap to CPU
         *   2. add IP to statoin IPs (2 IPs)
         *   Note: last config of trap holds
         */

    case bcmSwitchArpRequestMyStationIPToCPU:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_SNOOP_TO_CPU, 1);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_switch_my_arp_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
        /* Drop ARP with IP = arg
         *   1. config MY-ARP trap to drop
         *   2. add IP to statoin IPs (2 IPs)
         *   Note: last config of trap holds
         */
    case bcmSwitchArpRequestMyStationIPDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_DROP, 1);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_switch_my_arp_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
        /* Forward ARP with IP = arg
         *   (Don't touch MY-ARP config)
         *   1. remove IP to statoin IPs (2 IPs)
         */
    case bcmSwitchArpRequestMyStationIPFwd:
        rv = _dpp_switch_my_arp_remove(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ARP, DPP_SWITCH_SNOOP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestDrop:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MY_ARP, DPP_SWITCH_DROP, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyMyStationL2ToCPU:
        soc_sand_rv = soc_ppd_llp_trap_arp_info_get(soc_sand_dev_id, &arp_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        arp_info.ignore_da = !arg;
        soc_sand_rv = soc_ppd_llp_trap_arp_info_set(soc_sand_dev_id, &arp_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;

/************************************************************************/
    case bcmSwitchArpDhcpToCpu:
        rv = _bcm_dpp_switch_control_set(unit, bcmSwitchDhcpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_set(unit, bcmSwitchArpReplyToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_set(unit, bcmSwitchArpRequestToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuProtoIpOptionsPriority:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV6L3ErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV4L3ErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3HeaderErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchUnknownVlanToCpu:
        rv = _dpp_eg_trap_code_switch_control_set(unit,
                                                  SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIcmpRedirectToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_ICMP_REDIRECT, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchCpuIcmpRedirectPrio:
        rv = _dpp_trap_code_switch_control_priority_set(unit,
                                                        SOC_PPC_TRAP_CODE_ICMP_REDIRECT, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTunnelErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV6L3DstMissToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchV4L3DstMissToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMplsLabelMissToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMplsTtlErrToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_TTL0, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_MPLS_TTL1, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3SrcUrpfErrToCpu:
    case bcmSwitchL3UrpfFailToCpu:
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _dpp_trap_code_switch_control_set(unit,
                                               SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIp4Compress:
        SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(&defrag_info);
        soc_sand_rv = soc_ppd_frwrd_ipv4_mem_defrage(soc_sand_dev_id, arg, &defrag_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchSharedVlanMismatchToCpu:
        rv = _dpp_eg_trap_code_switch_control_set(unit,
                                                  SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3MtuFailToCpu:
        rv = _dpp_eg_trap_code_switch_control_set(unit,
                                                  SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION, DPP_SWITCH_TRAP_TO_CPU, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
        /* Gre Setting */
        /* Set protocol-type for L2-GRE */
    case bcmSwitchL2GreProtocolType:         /* Set protocol-type for L2-GRE */

        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* If mpls, then keep the type as default. */
        if (arg != 0x8847) {
            eg_encap_glbl.l2_gre_prtcl_type = arg;    
        }

        /* at egress, l2-GRE protocol set according to */
        eg_encap_glbl.unkown_gre_prtcl_type = arg;

        soc_sand_rv = soc_ppd_eg_encap_glbl_info_set(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchVxlanUdpDestPortSet:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        eg_encap_glbl.vxlan_udp_dest_port = arg; /* set vxlan udp port */
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_set(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmSwitchSetMplsEntropyLabelOffset:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        eg_encap_glbl.mpls_entry_label_msbs = arg; /* set MPLS entropy 12 msbs */
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_set(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmSwitchMplsPWControlWord:
        soc_sand_rv = soc_ppd_eg_encap_pwe_glbl_info_get(soc_sand_dev_id, &pwe_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        pwe_encap_glbl.cw = arg;
        soc_sand_rv = soc_ppd_eg_encap_pwe_glbl_info_set(soc_sand_dev_id, &pwe_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchL2AgeScan:
        SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO_clear(&pass_info);

        soc_sand_rv = soc_ppd_frwrd_mact_aging_one_pass_set(soc_sand_dev_id,&pass_info,&success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(success == SOC_SAND_FAILURE_OUT_OF_RESOURCES) {/* aging is active */
            rv = BCM_E_UNAVAIL;
        }
        else
        if(success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2) {/* prev run not finished */
            rv = BCM_E_BUSY;
        }
        break;
    case bcmSwitchL2HitClear:
        soc_sand_rv = soc_ppd_frwrd_mact_aging_events_handle_info_get(soc_sand_dev_id,&aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* update clear hit on get*/
        aging_info.clear_hit_bit = (arg != 0);

        soc_sand_rv = soc_ppd_frwrd_mact_aging_events_handle_info_set(soc_sand_dev_id, &aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmSwitchL2Cache:
        break;
    case bcmSwitchL2InvalidCtlToCpu:
        break;
    case bcmSwitchL2StaticMoveToCpu:
        if (SOC_IS_ARADPLUS(unit)) 
        {
            rv = _dpp_static_transplant_switch_control_set(unit, arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("Error: bcmSwitchL2StaticMoveToCpu is not supported before Arad Plus.")));        
        }
        
        break;
    case bcmSwitchL3UrpfMode:
#ifdef BCM_88660_A0
        /* For Arad+ the uRPF mode is per RIF instead of global. */
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When bcm886XX_l3_ingress_urpf_enable is set this switch control can not be used.")));
        }
#endif /* BCM_88660_A0 */        
        {
            int curr_urpf_mode = 0;
            rv = _bcm_sw_db_switch_urpf_mode_get(unit, &curr_urpf_mode);
            BCMDNX_IF_ERR_EXIT(rv);

            if (curr_urpf_mode != arg) {
                rv = _bcm_sw_db_switch_urpf_mode_set(unit, arg);
                BCMDNX_IF_ERR_EXIT(rv);

                if (SOC_IS_JERICHO(unit)){
                    rv = _bcm_ppd_rif_global_urpf_mode_traverse_set(unit, arg);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /*Update existing FECs and RIFs*/
                rv = _bcm_ppd_frwrd_fec_urpf_mode_traverse_set(unit, arg);
                BCMDNX_IF_ERR_EXIT(rv);

                
                if ((bcmL3IngressUrpfDisable == curr_urpf_mode && (bcmL3IngressUrpfLoose == arg || bcmL3IngressUrpfStrict == arg))
                    || ((bcmL3IngressUrpfLoose == curr_urpf_mode || bcmL3IngressUrpfStrict == curr_urpf_mode) && (bcmL3IngressUrpfDisable == arg))){
                    BCMDNX_IF_ERR_EXIT(rv);
                         LOG_WARN(BSL_LS_BCM_L3,
                         (BSL_META_U(unit,
                                     "Warning- Existing entries were not updated by this call.\n")));
                }

            }

        }
        break;

    case bcmSwitchL3UrpfDefaultRoute:
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* get RPF default check*/
        route_glbl_info.router_info.rpf_default_route = arg != 0;
        rv = BCM_E_NONE;
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_set(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchUnknownIpmcAsMcast:
        if (SOC_IS_JERICHO(unit)) {
            ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;
            uint32 programs[] =
                {PROG_FLP_IPV4COMPMC_WITH_RPF, PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF, PROG_FLP_BIDIR,
                PROG_FLP_IPV6MC, PROG_FLP_IPV6_MC_SSM};
            uint32 nof_programs = sizeof(programs) / sizeof(programs[0]);
            uint32 index = 0;
            uint32 program_hw_id = SOC_DPP_DEFS_GET(unit, nof_flp_programs);

           /* Iterate over all listed programs */
           for (index = 0; index < nof_programs; index++)
           {
                /* Get FLP program hardware id */
                soc_sand_rv = arad_pp_sw_db_flp_prog_app_to_index_get(unit, programs[index], &program_hw_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (program_hw_id >= SOC_DPP_DEFS_GET(unit, nof_flp_programs)){
                    continue;
                }

                soc_sand_rv = arad_pp_ihb_flp_process_tbl_get_unsafe(unit, program_hw_id, &flp_process_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                flp_process_tbl.compatible_mc_bridge_fallback = (arg != 0)? 1: 0;
                flp_process_tbl.procedure_ethernet_default = (arg != 0)? 3: 0;
                soc_sand_rv = arad_pp_ihb_flp_process_tbl_set_unsafe(unit, program_hw_id, &flp_process_tbl);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        } else {
            soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* get clear hit on get*/
            route_glbl_info.router_info.flood_unknown_mc = arg != 0;
            soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_set(soc_sand_dev_id, &route_glbl_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchL3McastL2:
        if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.ipmc_l3mcastl2_mode == 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When ipmc_l3mcastl2_mode is set the bcmSwitchL3McastL2 can not be used.")));
        }
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_get(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* get clear hit on get*/
        route_glbl_info.router_info.ipv4_mc_l2_lookup = arg;
        soc_sand_rv = soc_ppd_frwrd_ipv4_glbl_info_set(soc_sand_dev_id, &route_glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchL2LearnMode:
        /* Make sure that Egress independent is not configured for Jerico */
        if ((arg & BCM_L2_EGRESS_INDEPENDENT) &&
             SOC_IS_JERICHO(unit)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress independent is not supported in Jericho")));
        }

        rv = _bcm_dpp_switch_control_routed_learning_get(unit, &l3_routed_learn);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((l3_routed_learn != 0) && (arg & (BCM_L2_EGRESS_INDEPENDENT | BCM_L2_EGRESS_CENT | BCM_L2_EGRESS_DIST)))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When l3 routed learn mode is set, l2 learning mode can not be set to egress.")));
        }
        rv = _bcm_dpp_switch_L2LearnMode_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
        
#ifdef BCM_88660_A0
    case bcmSwitchMplsShortPipe:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("This feature is not supported")));
    case bcmSwitchMplsPipeTunnelLabelExpSet:
      if (SOC_IS_ARADPLUS(unit)) {
            /* Check that arg is 0 or 1 */
            if (arg < 0 || arg > 1) {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The only valid argument values for switch control"
                                                            " bcmSwitchMplsPipeTunnelLabelExpSet are 0 and 1")));
            }

          soc_sand_rv = soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(soc_sand_dev_id, !arg);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
      break;

    case bcmSwitchMplsDefaultTtlCopy:
        if ((arg < 0) || (arg > 255)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TTL value must be between 0 and 255")));
        }
        rv = arad_pp_eg_encap_mpls_default_ttl_set(unit, (uint8) arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchMcastTrunkIngressCommit:

        if (SOC_IS_ARADPLUS(unit)) {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit, mbcm_dpp_port_switch_lb_key_tables, ((unit))));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Supported only in Arad+")));
        }
        break;

    case bcmSwitchMcastTrunkEgressCommit:

        if (SOC_IS_ARADPLUS(unit)) {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit, mbcm_dpp_port_synchronize_lb_key_tables_at_egress, ((unit))));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Supported only in Arad+")));
        }
        break;

#endif /* BCM_88660_A0 */
    case bcmSwitchTraverseMode:
        rv = _bcm_dpp_switch_traverse_mode_set(unit,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3EgressMode:
    case bcmSwitchL3IngressMode:
        if(arg == 0) {
            rv = BCM_E_UNAVAIL;
        }
        else {
            rv = BCM_E_NONE;
        }
        break;
    case bcmSwitchL3HostCache:
        /* not supported due to coupling of l3 host and l2 mac databases */
        rv = BCM_E_UNAVAIL;
        break;
    case bcmSwitchL3HostCommit:
        rv = BCM_E_UNAVAIL;
        break;
    case bcmSwitchL3RoutedLearn:
        rv = _bcm_dpp_switch_L2LearnMode_get(unit, &l2_learn_mode);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((l2_learn_mode & (BCM_L2_INGRESS_CENT | BCM_L2_INGRESS_DIST)) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("When l2 learn mode is set to egress, l3 routed packets learning can not be set.")));
        }
        rv = _bcm_dpp_switch_control_routed_learning_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchL3RouteCache:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO_PLUS(unit)) {
            uint32 desc_num_max;
            uint32 mem_buff_size;
            uint32 timeout_usec;
            rv = jer_sbusdma_desc_status(unit, &desc_num_max, &mem_buff_size, &timeout_usec);
            BCM_SAND_IF_ERR_EXIT(rv);
            if (arg && desc_num_max == 0) {
                uint32 desc_num_max = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_num_max", 0) ?
                    soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_num_max", 0) :(arg >> 16) & 0xFFFF;
                uint32 mem_buff_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_mem_buff_size", 0) ?
                    soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_mem_buff_size", 0) : arg & 0xFFFF;
                uint32 timeout_usec = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_timeout_usec", 0) ?
                    soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "desc_timeout_usec", 0) : 10000;
                rv = jer_sbusdma_desc_init(unit, desc_num_max, mem_buff_size, timeout_usec);
            } else if (!arg && desc_num_max != 0) {
                rv = jer_sbusdma_desc_deinit(unit);
            } else {
                if (arg) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Descriptor DMA already initialized.")));
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Descriptor DMA already disabled.")));
                }
            }
            BCM_SAND_IF_ERR_EXIT(rv);
        } else if (SOC_IS_JERICHO(unit)) {
            rv = BCM_E_UNAVAIL;
        } else
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/
        {
            tmp = (arg == 0) ? SOC_PPC_FRWRD_IP_CACHE_MODE_NONE : SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM;
            soc_sand_rv = soc_ppd_frwrd_ip_routes_cache_mode_enable_set(soc_sand_dev_id,
                                                                        DPP_FRWRD_IP_ALL_VRFS_ID, arg);
        }
        break;
    case bcmSwitchL3RouteCommit:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO(unit)) {
            rv = jer_pp_xpt_wait_dma_done(unit);
            BCM_SAND_IF_ERR_EXIT(rv);
            rv = jer_sbusdma_desc_wait_done(unit);
            BCM_SAND_IF_ERR_EXIT(rv);
        } else
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP)*/
        {
            /* mapping document says to use soc_ppd_frwrd_ip_routes_commit */
            soc_sand_rv = soc_ppd_frwrd_ip_routes_cache_commit(soc_sand_dev_id,
                                                               SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM,
                                                               DPP_FRWRD_IP_ALL_VRFS_ID,
                                                               &success);
            SOC_SAND_IF_FAIL_RETURN(success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchFieldCache:
#if defined(INCLUDE_KBP)
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            /* set KBP caching mode */
            SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
            SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_KBP_CACHE;
            control_info.val[0] = arg ? 1 : 0;
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
		soc_sand_ll_timer_set("ARAD_KBP_TIMERS_SOC", ARAD_KBP_TIMERS_SOC);
#endif
            soc_sand_rv = soc_ppd_fp_control_set(soc_sand_dev_id,
                                                 SOC_CORE_INVALID,
                                                 &control_ndx,
                                                 &control_info,
                                                 &success);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
			soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_SOC);
#endif
            SOC_SAND_IF_FAIL_RETURN(success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else
#endif /* defined(INCLUDE_KBP) */
        {
            rv = BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchFieldCommit:
#if defined(INCLUDE_KBP)
        if (SOC_DPP_IS_ELK_ENABLE(unit)) {
            /* commit KBP cached configuration */
            SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
            SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_KBP_COMMIT;
            control_info.val[0] = 1;
			soc_sand_ll_timer_set("ARAD_KBP_TIMERS_SOC", ARAD_KBP_TIMERS_SOC);
            soc_sand_rv = soc_ppd_fp_control_set(soc_sand_dev_id,
                                                 SOC_CORE_INVALID,
                                                 &control_ndx,
                                                 &control_info,
                                                 &success);
			soc_sand_ll_timer_set("ARAD_KBP_TIMERS_SOC", ARAD_KBP_TIMERS_SOC);
            SOC_SAND_IF_FAIL_RETURN(success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else
#endif /* defined(INCLUDE_KBP) */
        {
            rv = BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchL3SlowpathToCpu:
        break;
    case bcmSwitchMartianAddr:
        break;
    case bcmSwitchMartianAddrToCpu:
        break;
    case bcmSwitchMcastUnknownErrToCpu:
        break;
    case bcmSwitchMplsInvalidActionToCpu:
        break;
    case bcmSwitchReserveHighEncap:
        break;
    case bcmSwitchReserveHighL2Egress:
        break;
    case bcmSwitchReserveHighMimPort:
        break;
    case bcmSwitchReserveHighMplsPort:
        break;
    case bcmSwitchReserveHighVlanPort:
        break;
    case bcmSwitchReserveHighVpn:
        break;
    case bcmSwitchReserveLowEncap:
        break;
    case bcmSwitchReserveLowL2Egress:
        break;
    case bcmSwitchReserveLowMimPort:
        break;
    case bcmSwitchReserveLowMplsPort:
        break;
    case bcmSwitchReserveLowVlanPort:
        break;
    case bcmSwitchReserveLowVpn:
        break;
    case bcmSwitchSnapNonZeroOui:
        break;
    case bcmSwitchSynchronousPortClockSource:
        if (arg >= SOC_INFO(unit).fabric_logical_port_base) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_link_set, (unit, 1, arg)));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_sel_port_set, (unit, 0, arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchSynchronousPortClockSourceBkup:
        if (arg >= SOC_INFO(unit).fabric_logical_port_base) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_link_set, (unit, 0, arg)));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_nif_synce_clk_sel_port_set,(unit, 1, arg)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchSynchronousPortClockSourceDivCtrl:
    	/* there is an overlap in synce divider options in fabric and nif. Thus check if fabric synce is enabled */
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 1 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
          BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_divider_set, (unit, 1, arg)));
        } else {
          soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_div_set, (unit, 0, arg)));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchSynchronousPortClockSourceBkupDivCtrl:
        SOCDNX_IF_ERR_EXIT(soc_dpp_fabric_sync_e_enable_get(unit, 0 ,&fabric_synce_enable));
        if (fabric_synce_enable) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_port_sync_e_divider_set, (unit, 0, arg)));
        } else {
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nif_synce_clk_div_set, (unit, 1, arg)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
    case bcmSwitchECMPSecondHierHashConfig:
    case bcmSwitchECMPSecondHierHashOffset:
        if(!SOC_IS_JERICHO_PLUS_A0(unit) && !SOC_IS_QUX(unit)) {
            unsupported_switch = 1;
            break;
        }
    case bcmSwitchECMPHashConfig:
    case bcmSwitchECMPHashOffset:
    case bcmSwitchECMPHashSeed:
    case bcmSwitchECMPHashSrcPortEnable:
        rv = _bcm_dpp_switch_ecmp_hash_global_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPResilientHashCombine:
        if (SOC_SUPPORTS_ECMP_HASH_COMBINE(unit)) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ecmp_hash_slb_combine_set,(unit, arg)));
            BCMDNX_IF_ERR_EXIT(rv);
        } else{
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchHashELISearch:
        if (SOC_DPP_CONFIG(unit)->pp.parser_mode == 0) {
            unsupported_switch = 1;
            break;
        }
    case bcmSwitchTrunkHashConfig:
    case bcmSwitchTrunkHashOffset:
    case bcmSwitchTrunkHashSeed:
    case bcmSwitchTrunkHashSrcPortEnable:
        rv = _bcm_dpp_switch_trunk_hash_global_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTrunkHashMPLSPWControlWord:
    case bcmSwitchTrunkHashNormalize:
    case bcmSwitchHashFCOEField0:
        rv = _bcm_dpp_switch_trunk_hash_mask_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchFcoeNpvModeEnable:
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_fcf_npv_switch_set,(unit, arg)));
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchHashIP4Field0:
    case bcmSwitchHashIP6Field0:
    case bcmSwitchHashL2Field0:
    case bcmSwitchHashMPLSField0:
    case bcmSwitchHashMPLSField1:
        rv = _bcm_dpp_switch_ecmp_lag_hash_mask_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchUseGport:
        if (arg) {
            SOC_USE_GPORT_SET(unit, TRUE);
            rv = BCM_E_NONE;
        } else {
           LOG_ERROR(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "unit:%d Disabling the use of Gports "
                                 "is not supported. \n"), unit));
            rv = BCM_E_PARAM;
        }
        break;
/************************************************************************/
/*                              trill                                     */
/************************************************************************/
    case  bcmSwitchTrillEthertype:
        rv = _bcm_dpp_trill_ethertype_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case  bcmSwitchTrillBroadcastDestMacOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));
        tmp = arg << 8;
#ifdef LE_HOST
        tmp = _shr_swap32(tmp);
#endif /* LE_HOST */
        sal_memcpy(bcm_mac, &tmp, 3);
        soc_sand_rv = _bcm_petra_mac_to_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));


        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case  bcmSwitchTrillBroadcastDestMacNonOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));
        tmp = arg << 8;
#ifdef LE_HOST
        tmp = _shr_swap32(tmp);
#endif /* LE_HOST */
        sal_memcpy(&(bcm_mac[3]), &tmp, 3);

        soc_sand_rv = _bcm_petra_mac_to_sand_mac(bcm_mac, &(trill_info.all_rbridges_mac));

        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case  bcmSwitchTrillEndStationDestMacOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));
        BCMDNX_IF_ERR_EXIT(rv);

        tmp = arg << 8;
        sal_memcpy(bcm_mac, &tmp, 3);
        soc_sand_rv = _bcm_petra_mac_to_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));

        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case  bcmSwitchTrillEndStationDestMacNonOui:
        soc_sand_rv = soc_ppd_mymac_trill_info_get(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));
        BCMDNX_IF_ERR_EXIT(rv);
        tmp = arg << 8;
        sal_memcpy(&(bcm_mac[3]), &tmp, 3);

        soc_sand_rv = _bcm_petra_mac_to_sand_mac(bcm_mac, &(trill_info.all_esadi_rbridges));


        soc_sand_rv = soc_ppd_mymac_trill_info_set(soc_sand_dev_id, &trill_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case  bcmSwitchTrillHopCount:

        soc_sand_rv = soc_ppd_frwrd_trill_global_info_get(soc_sand_dev_id, &trill_global_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        trill_global_info.cfg_ttl = arg;

        soc_sand_rv = soc_ppd_frwrd_trill_global_info_set(soc_sand_dev_id, &trill_global_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case  bcmSwitchPolicerFairness:
        {

			SOC_PPC_MTR_GROUP_INFO mtr_group_info;
            uint8 meter_group;

            /* update both core and meters groups */
            SOC_PPC_MTR_GROUP_INFO_clear(&mtr_group_info);
            mtr_group_info.rand_mode_enable = arg;

            _DPP_POLICER_METER_CORES_ITER(core_id){
                for (meter_group = 0; meter_group < 2; meter_group++) {
                    soc_sand_rv = soc_ppd_mtr_meters_group_info_set(soc_sand_dev_id, core_id, meter_group, &mtr_group_info); 
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
        }
        break;
/************************************************************************/
/*                              Mac-in-Mac                              */
/************************************************************************/
    case bcmSwitchMiMTeBvidLowerRange:
        rv = _bcm_dpp_switch_mim_bvid_range_set(unit, -1, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMiMTeBvidUpperRange:
        rv = _bcm_dpp_switch_mim_bvid_range_set(unit, arg, -1);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
/************************************************************************/
/*                                            Stacking                  */
/************************************************************************/
    case bcmSwitchTMDomain:
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_tm_domain_set,(unit, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;

/************************************************************************/
/*                Congestion                                            */
/************************************************************************/
    case bcmSwitchCongestionCnm:
      if (SOC_IS_ARAD_A0(unit) || SOC_IS_JERICHO(unit)) {
          BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("CNM congestion isn't supported for this device")));
      }
      else {
          rv = _bcm_dpp_switch_control_congestion_enable_set(unit, arg); 
          BCMDNX_IF_ERR_EXIT(rv);
      }
      break;

    case bcmSwitchCongestionMissingCntag:
    case bcmSwitchCongestionExcludeReplications:
    case bcmSwitchCongestionCntag:
    case bcmSwitchCongestionCntagEthertype:
        rv = _bcm_dpp_switch_control_congestion_options_set(unit, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
      break;

/************************************************************************/
/*                RCPU                                                  */
/************************************************************************/
#if defined(BCM_RCPU_SUPPORT)
    case bcmSwitchRemoteCpuSchanEnable:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            uint32 rval;
            rval = 0;
            if ((arg != 0x0) && (arg != 0x1)) {
                return BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval,
                              ENABLE_SCHAN_REQUESTf, arg);
            BCMDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_CTRLr(unit, rval));
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuMatchLocalMac:
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            uint32 rval;
            rval = 0;
            if ((arg != 0x0) && (arg != 0x1)) {
                return BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval,
                              LMAC0_MATCHf, arg);
            BCMDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_CTRLr(unit, rval));
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuCmicEnable:
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            uint32 rval;
            rval = 0;
            if ((arg != 0x0) && (arg != 0x1)) {
                return BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval,
                              ENABLE_CMIC_REQUESTf, arg);
            BCMDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_CTRLr(unit, rval));
            return BCM_E_NONE;
        } else
#endif
        {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuMatchVlan:
        if (soc_feature(unit, soc_feature_rcpu_1)) {

            uint32 rval;
            rval = 0;
            if ((arg != 0x0) && (arg != 0x1)) {
                return BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(READ_CMIC_PKT_CTRLr(unit, &rval));
            soc_reg_field_set(unit, CMIC_PKT_CTRLr, &rval, VLAN_MATCHf, arg);
            BCMDNX_IF_ERR_EXIT(WRITE_CMIC_PKT_CTRLr(unit, rval));
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuLocalMacOui:
    case bcmSwitchRemoteCpuDestMacOui:
        if ((arg < 0x0) || (arg > 0xffffff)) {
                return BCM_E_PARAM;
        }
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_mac_hi_set(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
    case bcmSwitchRemoteCpuLocalMacNonOui:
    case bcmSwitchRemoteCpuDestMacNonOui:
        if ((arg < 0x0) || (arg > 0xffffff)) {
                return BCM_E_PARAM;
        }
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_mac_lo_set(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;

    case bcmSwitchRemoteCpuEthertype:
    case bcmSwitchRemoteCpuVlan:
    case bcmSwitchRemoteCpuTpid:
    case bcmSwitchRemoteCpuSignature:
        if ((arg < 0x0) || (arg > 0xffff)) {
            return BCM_E_PARAM;
        }
        if (soc_feature(unit, soc_feature_rcpu_1)) {
            return _bcm_arad_rcpu_switch_vlan_tpid_sig_set(unit, bcm_type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
        break;
#endif /* #if defined(BCM_RCPU_SUPPORT) */


#ifdef BCM_88660_A0

    case bcmSwitchMeterAdjustInterframeGap:

        if (SOC_IS_ARADPLUS(unit)) {
            if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchMeterAdjustInterframeGap is Supported only in Arad+")));
            }else{
                char is_ipg_enabled = 0;

                switch (arg) {
                case SOC_PPC_MTR_IPG_COMPENSATION_DISABLED_SIZE:
                    is_ipg_enabled = 0;
                    break;
                case SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE:
                    is_ipg_enabled = 1;
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The only valid argument values for switch control"
                                                            " bcmSwitchMeterAdjustInterframeGap are 0 and 20")));
                    break;
                }

                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_ipg_compensation_set,(unit,is_ipg_enabled)));

                return BCM_E_NONE;
            }
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchColorL3L2Marking:
        if (SOC_IS_ARADPLUS(soc_sand_dev_id)) {
             SOC_PPC_EG_QOS_GLOBAL_INFO info;
             int col_idx;
             int flag_num = 4;
             uint32 possible_flags[] = { BCM_PRIO_GREEN, BCM_PRIO_YELLOW, BCM_PRIO_RED, BCM_PRIO_BLACK };
             uint32 bitmap = 0, flags = 0;

             SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

             flags = arg;

             /* We assume that if possible_flags[i] is included then bit i should be on. */

             /* Construct the bitmap. We assume that the idx of the flag is the bit to be set for it. */
             for (col_idx = 0; col_idx < flag_num; col_idx++) {
                 if (flags & possible_flags[col_idx]) {
                     bitmap |= (1 << col_idx);
                 }
             }

             /* Get the info and set the bitmap. */
             soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

             info.resolved_dp_bitmap = bitmap;

             soc_sand_rv = soc_ppd_eg_qos_global_info_set(soc_sand_dev_id, &info);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

             return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchResilientHashAgeTimer:
      if (SOC_IS_ARADPLUS(unit)) {
        if (soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, FALSE)) {
          uint32 soc_sand_rv;
          SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS aging;

          if (arg <= 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only positive values are allowed.\n")));
          }

          SOC_PPC_SLB_CLEAR(&aging, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS);
          
          aging.age_time_in_seconds = arg;

          soc_sand_rv = soc_ppd_slb_set_global_cfg(soc_sand_dev_id, SOC_PPC_SLB_DOWNCAST(&aging, SOC_PPC_SLB_CONFIGURATION_ITEM));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

          /* Set the time in SW DB. */
          rv = sw_state_access[unit].dpp.soc.arad.pp.slb_config.age_time_in_seconds.set(soc_sand_dev_id, arg);
          BCMDNX_IF_ERR_EXIT(soc_sand_rv);

          return BCM_E_NONE;
        } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Stateful load balancing is disabled on this device.\n")));
        }
      } else {
        unsupported_switch = TRUE;
      }
      break;
    case bcmSwitchVpnDetachPorts:
        rv = sw_state_access[unit].dpp.bcm.vswitch.vpn_detach_ports.set(soc_sand_dev_id,arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchTrunkResilientHashConfig:
      if (SOC_IS_ARADPLUS(unit)) {
          uint8 dpp_hash_idx;

          rv = _bcm_dpp_trunk_bcm_hash_to_ppd_hash(unit, arg, &dpp_hash_idx);
          BCMDNX_IF_ERR_EXIT(rv);

          /* Set the seed in HW. */
          {
            SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION slb_lag_func;

            SOC_PPC_SLB_CLEAR(&slb_lag_func,SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION);

            slb_lag_func.hash_function = dpp_hash_idx;

            soc_sand_rv = soc_ppd_slb_set_global_cfg(soc_sand_dev_id, SOC_PPC_SLB_DOWNCAST(&slb_lag_func, SOC_PPC_SLB_CONFIGURATION_ITEM));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
      } else {
        unsupported_switch = TRUE;
      }
      break;
    case bcmSwitchTrunkResilientHashSeed:
    case bcmSwitchECMPResilientHashSeed:
	case bcmSwitchTrunkResilientHashOffset:
	case bcmSwitchResilientHashCRCSeed:
      if (SOC_IS_ARADPLUS(unit)) {
        SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET lag_hash_offset;
        SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED lag_hash_seed;
        SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED ecmp_hash_seed;
		SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED crc_hash_seed;
        SOC_PPC_SLB_CONFIGURATION_ITEM *cfg_item = NULL;

        switch (bcm_type) {
        case bcmSwitchTrunkResilientHashSeed:
          SOC_PPC_SLB_CLEAR(&lag_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED);
          lag_hash_seed.seed = arg;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&lag_hash_seed;
          break;
        case bcmSwitchECMPResilientHashSeed:
          SOC_PPC_SLB_CLEAR(&ecmp_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED);
          ecmp_hash_seed.seed = arg;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_hash_seed;
          break;
        case bcmSwitchTrunkResilientHashOffset:
          SOC_PPC_SLB_CLEAR(&lag_hash_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET);
          lag_hash_offset.offset = arg;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&lag_hash_offset;
          break;
		case bcmSwitchResilientHashCRCSeed:
          SOC_PPC_SLB_CLEAR(&crc_hash_seed, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED);
          crc_hash_seed.seed = arg;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_seed;
          break;
        /* must default. Oveerwise - compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default:
          BCMDNX_VERIFY(FALSE);
          break;
        }

        soc_sand_rv = soc_ppd_slb_set_global_cfg(soc_sand_dev_id, cfg_item);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      } else {
        unsupported_switch = TRUE;
      }
      break;
	case bcmSwitchECMPResilientHashKey0Shift:
	case bcmSwitchECMPResilientHashKey1Shift:
	case bcmSwitchResilientHashCRC0ByteMask:
	case bcmSwitchResilientHashCRC1ByteMask:
	case bcmSwitchResilientHashCRC2ByteMask:
	case bcmSwitchResilientHashCRC3ByteMask:
	  if (SOC_IS_JERICHO(unit)) {
		SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET ecmp_key_offset;
		SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK crc_hash_mask;
        SOC_PPC_SLB_CONFIGURATION_ITEM *cfg_item = NULL;

        switch (bcm_type) {
		case bcmSwitchECMPResilientHashKey0Shift:
          SOC_PPC_SLB_CLEAR(&ecmp_key_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET);
          ecmp_key_offset.offset = arg;
		  ecmp_key_offset.index = 0;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_key_offset;
          break;
        case bcmSwitchECMPResilientHashKey1Shift:
          SOC_PPC_SLB_CLEAR(&ecmp_key_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET);
          ecmp_key_offset.offset = arg;
		  ecmp_key_offset.index = 1;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&ecmp_key_offset;
          break;
		case bcmSwitchResilientHashCRC0ByteMask:
          SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
          crc_hash_mask.mask = arg;
		  crc_hash_mask.index = 0;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
          break;
	    case bcmSwitchResilientHashCRC1ByteMask:
          SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
          crc_hash_mask.mask = arg;
		  crc_hash_mask.index = 1;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
          break;
	    case bcmSwitchResilientHashCRC2ByteMask:
          SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
          crc_hash_mask.mask = arg;
		  crc_hash_mask.index = 2;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
          break;
	    case bcmSwitchResilientHashCRC3ByteMask:
          SOC_PPC_SLB_CLEAR(&crc_hash_mask, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK);
          crc_hash_mask.mask = arg;
		  crc_hash_mask.index = 3;
          cfg_item = (SOC_PPC_SLB_CONFIGURATION_ITEM *)&crc_hash_mask;
          break;
        default:
          BCMDNX_VERIFY(FALSE);
          break;
        }

        soc_sand_rv = soc_ppd_slb_set_global_cfg(soc_sand_dev_id, cfg_item);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
	  } else {
         unsupported_switch = TRUE;
      }
      break;
#endif /* BCM_88660_A0 */
    case bcmSwitchCacheTableUpdateAll:
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cache_table_update_all, (unit)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;

    case bcmSwitchL2LearnLimitToCpu:

        if (SOC_IS_ARADPLUS(unit)){
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set,(unit, arg)));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            unsupported_switch = TRUE;
        }
        break;

/************************************************************************/
/*                              ecn                                   */
/************************************************************************/

    case bcmSwitchEcnDelayMeasurementThreshold:
        if (!soc_property_get(unit, spn_ECN_DM_ENABLE, 0))  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("ecn_dm_enable must be on for type bcmSwitchEcnDelayMeasurementThreshold.")));
        }

        ARAD_PRGE_ECN_DM_PROGRAM_VAR_SET(arg);

        {
            COMPILER_64_SET(arg64, 0, arg);
            soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_DELAY_BASED_ECN2, arg64);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;
#ifdef BCM_JERICHO_SUPPORT 
     /* Set port default mode to advanced or normal */
    case bcmSwitchPortVlanTranslationAdvanced:
       /* such fields only in Jericho */
       if (SOC_IS_JERICHO(unit))  {
           BCMDNX_IF_ERR_EXIT(READ_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY, &reg_val));
           soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_0r, &reg_val, FORCE_VEC_FROM_LIFf, arg?0x1:0x0);
           BCMDNX_IF_ERR_EXIT(WRITE_IHP_VTT_GENERAL_CONFIGS_0r(unit, REG_PORT_ANY,reg_val));
       }else{
           unsupported_switch = TRUE;
       }
       break;
    case bcmSwitchGlobalTpidEnable:
        if (SOC_IS_JERICHO(unit))  {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                IHP_IHP_ENABLERSr, REG_PORT_ANY, 0, ADDITIONAL_TPID_ENABLEf,
                (arg? TRUE : FALSE)));
        } else {
            unsupported_switch = TRUE;
        }
        break;
#endif
    case bcmSwitchEtagEthertype:
        if (SOC_IS_ARADPLUS(unit)) {
            /* Set global Etag Ethertype */
            _BCM_DPP_EXTENDER_INIT_CHECK(unit);

            /* Add a sem entry for non e tag filtering. */
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_extender_global_etag_ethertype_set, (unit, arg));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
           unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchReserved872:
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "prge_ptagged_load", 0)) {
            rv = custom_change_tpid_over_prge_var(unit,arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0) && SOC_IS_JERICHO(unit)) {
            rv = custom_change_rif_over_prge_var(unit,outrif_range_lower_limit + arg - 1);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchReserved958:
        if (SOC_DPP_CONFIG(unit)->pp.test1) {
            rv = custom_prge_set_op_val_in_instruction(unit,15/*instruction*/,arg);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            unsupported_switch = TRUE;
        }
        break;
    case bcmSwitchClassL3L2Marking:
        if (SOC_IS_JERICHO_B0(soc_sand_dev_id) || SOC_IS_QMX_B0(soc_sand_dev_id)) {
            SOC_PPC_EG_QOS_GLOBAL_INFO info;
            int col_idx;
            int flag_num = 4;
            uint32 bitmap = 0, flags = 0;

            SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

            flags = arg;

            /* Construct the bitmap. We assume that the idx of the flag is the bit to be set for it. */
            for (col_idx = 0; col_idx < flag_num; col_idx++) { 
             if (flags & 0x1) {
                 bitmap |= (1 << col_idx);
                 flags >>= 1;
             }
            }

            /* Get the info and set the bitmap. */
            soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            info.resolved_in_lif_profile_bitmap = bitmap;

            soc_sand_rv = soc_ppd_eg_qos_global_info_set(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchL3L2MarkingMode:
        if (SOC_IS_JERICHO_B0(soc_sand_dev_id) || SOC_IS_QMX_B0(soc_sand_dev_id) || SOC_IS_QAX(soc_sand_dev_id)) {
            SOC_PPC_EG_QOS_GLOBAL_INFO info;

            SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);

            /* Get the info and set the bitmap. */
            soc_sand_rv = soc_ppd_eg_qos_global_info_get(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);            

            info.dp_map_mode = ((arg == 0) ? 0 : 1);

            soc_sand_rv = soc_ppd_eg_qos_global_info_set(soc_sand_dev_id, &info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            return BCM_E_NONE;
        } else {
            unsupported_switch = 1;
        }
        break;
    case bcmSwitchDefaultNativeOutVlanPort:
        /* set default native out AC. */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            _bcm_dpp_gport_hw_resources gport_hw_resources; 


            /* get out AC from vlan port */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_hw_resources(unit, 
                                                              arg, 
                                                              _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                              &gport_hw_resources
                                                              ));

            /* set default native out AC*/
            if (gport_hw_resources.local_out_lif != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {

                /* check AC is native out AC */
                _bcm_lif_type_e lif_usage;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, gport_hw_resources.local_out_lif, NULL, &lif_usage));
                if (lif_usage != _bcmDppLifTypeNativeVlan) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("AC gport is not a native AC gport")));
                }

                /* set default native out AC */
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_default_native_ac_outlif_set, (unit, gport_hw_resources.local_out_lif)));
            }
            break;
        } else {
            unsupported_switch = 1;
        }

    case bcmSwitchDefaultEgressVlanEditClassId:
        /* set Vlan edit class id (aka vlan edit profile) for default out-AC register */


        /* check in AVT (vlan edit profile is managed by user only in AVT) */
        if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Set vlan edit class at for default out-AC is only available when VLAN translation is set to Advanced mode")));
      }

        /* check parameters */
        /* Validate the VLAN edit profile */
        BCM_DPP_VLAN_EDIT_PROFILE_VALID(unit, 
                                        arg /* vlan edit profile */,
                                        FALSE /* at ingress */);

        BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set,(unit, arg)));
        break;

#ifdef BCM_QAX_SUPPORT
    case bcmSwitch3rdTagStampMode:
        if (SOC_IS_JERICHO_PLUS(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                               IHP_IHP_ENABLERSr, REG_PORT_ANY, 0,
                               PRSR_ENCAPSULATION_OR_3RD_TAGf,
                               (arg? TRUE : FALSE)));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit,
                               IHP_VTT_GENERAL_CONFIGS_0r, REG_PORT_ANY, 0,
                               VTT_ENCAPSULATION_OR_3RD_TAGf,
                               (arg? TRUE : FALSE)));
        } else {
            unsupported_switch = 1;
        }
        break;
#endif


    case bcmSwitchUdpTunnelIPv4DstPort:
    case bcmSwitchUdpTunnelIPv6DstPort:
    case bcmSwitchUdpTunnelMplsDstPort:
       if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable) {
          ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE arad_dst_port_type;
          rv =  _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_parser(unit, bcm_type, &arad_dst_port_type);
          soc_sand_rv = arad_parser_udp_tunnel_dst_port_set(unit, arad_dst_port_type, arg);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          {
             ARAD_EGR_PROG_EDITOR_PROGRAMS prge_udp_dest_port_type;
             rv = _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_prge(unit, bcm_type, &prge_udp_dest_port_type);
             COMPILER_64_SET(arg64, 0, arg);
             soc_sand_rv = arad_pp_prge_program_var_set(unit, prge_udp_dest_port_type, arg64);
             BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
       } else {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Udp tunnel not supported on this device")));
       }
       break;

#ifdef BCM_JERICHO_SUPPORT
    case bcmSwitchMirrorSnoopForwardOriginalWhenDropped:
        if (SOC_IS_QAX(unit) || !SOC_IS_JERICHO(unit)) { /* supported only for Jer/QMX/Jer+/QMX+ */
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcmSwitchMirrorSnoopForwardOriginalWhenDropped supported only for BCM88{3,6}{7,8}x.")));
        } else {
            uint8 traffic_en = 0;
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_enable_traffic_get,(unit, &traffic_en)));
            if (traffic_en == TRUE &&
              /* Check the override flag for allowing modifications during traffic */
              soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "allow_modifications_during_traffic", 0) == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmSwitchMirrorSnoopForwardOriginalWhenDroppedis not allowed while traffic is enabled")));
            }
            BCM_DPP_UNIT_CHECK(unit);
        }
        BCMDNX_IF_ERR_EXIT(jer_mgmt_mirror_snoop_forward_original_when_dropped_set(unit, arg));
        break;
#endif /* BCM_JERICHO_SUPPORT */
 
  case bcmSwitchIngressRateLimitMpps:
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_rate_limit_mpps_set,(unit, arg));
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchLIUdpSrcPortSet:
        COMPILER_64_SET(arg64, 0, (arg & 0xFFFF));
        soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_LI, arg64);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_QAX(unit)) {
            COMPILER_64_SET(arg64, 0, (arg & 0xFFFF));
            soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_LI_IPV6, arg64);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        break;

    case bcmSwitchMeterMef10dot3Enable:
        if (SOC_IS_JERICHO(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_global_mef_10_3_set,(unit, &arg));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchMeterMef10dot3Enable is Supported only in Jericho and above")));        
        }        
        break;

    case bcmSwitchForwardLookupNotFoundTrap:
        rv = _bcm_dpp_switch_flp_not_found_trap_set(unit, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchEtherTypeCustom1:
    case bcmSwitchEtherTypeCustom2:
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_get(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (bcm_type == bcmSwitchEtherTypeCustom1) {
            eg_encap_glbl.custom1_ether_type = arg;
        } else {
            eg_encap_glbl.custom2_ether_type = arg;
        }
        soc_sand_rv = soc_ppd_eg_encap_glbl_info_set(soc_sand_dev_id, &eg_encap_glbl);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        break;
    case bcmSwitchNoneEthernetPacketMeterPtr:
        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_none_ethernet_packet_ptr_set,(unit, &arg));
        BCMDNX_IF_ERR_EXIT(rv);
        
        break;
    case bcmSwitchEtherDscpRemark:
        if ((arg != 0) && (arg != 1)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("arg for bcmSwitchEtherDscpRemark must be 0 or 1")));
        }
        if (SOC_IS_JERICHO(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_qos_ether_dscp_remark_set,(unit, arg));
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchEtherDscpRemark is Supported only in Jericho and above")));        
        }
        break;
    case bcmSwitchLlcHeaderControlMode:
        if ((arg != 0) && (arg != 1)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("arg for bcmSwitchEtherDscpRemark must be 0 or 1")));
        }
        if (SOC_IS_JERICHO(unit)) {
            rv = soc_reg_above_64_field32_modify(unit, IHP_IHP_ENABLERSr, SOC_CORE_ALL, 0, LLC_CHECK_LSBf, arg);
        } else {
            rv = soc_reg_above_64_field32_modify(unit, IHP_IHP_ENABLERSr, SOC_CORE_ALL, 0, FIELD_6_6f, arg);
        }
        BCMDNX_IF_ERR_EXIT(rv);
        break;

    case bcmSwitchPFCStopInterfaceBitmap:
        if (!SOC_IS_JERICHO(unit) || SOC_IS_FLAIR(unit)) { /* supported only for Jer/QMX/Jer+/QMX+/QAX/QUX */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmSwitchPFCStopInterfaceBitmapsupported only for Jericho and above, besides QUX and FLAIR.")));
        }
        if ((arg < 0) || (arg > 0xFF)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("arg for bcmSwitchPFCStopInterfaceBitmap must be in range [0, 0xFF]")));
        }
        if(SOC_IS_QUX(unit)) {
            rv = soc_reg_above_64_field32_modify(unit, NIF_FC_PFC_RX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Af, arg);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = soc_reg_above_64_field32_modify(unit, NIF_FC_PFC_RX_BITMAP_TYPE_Br, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Bf, arg);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = soc_reg_above_64_field32_modify(unit, NBIH_FC_PFC_RX_BITMAP_TYPE_Ar, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Af, arg);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = soc_reg_above_64_field32_modify(unit, NBIH_FC_PFC_RX_BITMAP_TYPE_Br, REG_PORT_ANY, 0, FC_LLFC_STOP_TX_FROM_PFC_RX_BITMAP_TYPE_Bf, arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        break;

    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control type")));
    }


    if (unsupported_switch) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control type")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_switch_control_port_header_type_map(int unit,
                                             int to_bcm,
                                             int *bcm_port_header_type, 
                                             SOC_TMC_PORT_HEADER_TYPE *soc_tmcport_header_type)
{
    int                                  rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (to_bcm) {

        switch (*soc_tmcport_header_type) {
        case SOC_TMC_PORT_HEADER_TYPE_NONE:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_TM;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_PROG:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_PROG;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_CPU;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_STACKING:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TDM:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_TDM;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TDM_RAW:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            *bcm_port_header_type = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
            break;
        default:
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm port header type, to_bcm.")));
        }

    } else {

        switch (*bcm_port_header_type) {
        case BCM_SWITCH_PORT_HEADER_TYPE_NONE:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_NONE;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_ETH:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_ETH;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_RAW:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_RAW;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TM:
        case BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP:
        case BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP:
        case BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_TM;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_PROG:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_PROG;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_CPU:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_CPU;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_STACKING:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_STACKING;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TDM:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_TDM;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_TDM_RAW;
            break;
        case BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW:
            *soc_tmcport_header_type = SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW;
            break;
        default:
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm port header type. from_bcm")));
        }

    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
     

int
_bcm_dpp_switch_control_port_get(int unit,
                                 bcm_port_t port,
                                 bcm_switch_control_t bcm_type,
                                 int *arg)
{
    int                                  rv = BCM_E_NONE;
    uint32                               soc_sand_rc;

    uint32 tm_port, pp_port;
    SOC_TMC_PORT_HEADER_TYPE soc_tmcport_header_type = SOC_TMC_PORT_NOF_HEADER_TYPES;
    SOC_TMC_PORT_PP_PORT_INFO pp_port_info;
    SOC_PPC_PORT_INFO  port_info;
    SOC_TMC_PORTS_SWAP_INFO port_swap_info;
    SOC_PPC_LLP_TRAP_PORT_INFO llp_trap_port_info;
    uint32    soc_sand_rv;
    int       port_i, core=SOC_CORE_INVALID, counter_adjust_type = 0;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    switch (bcm_type) {
    case bcmSwitchIgmpPktToCpu:

    case bcmSwitchIgmpPktDrop:
    case bcmSwitchIgmpQueryToCpu:
    case bcmSwitchIgmpQueryDrop:
    case bcmSwitchIgmpQueryFlood:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIcmpRedirectToCpu:
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core)));

            soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, pp_port, &llp_trap_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            *arg = (llp_trap_port_info.trap_enable_mask & SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT)?1:0;
            break;
        }


        break;
    case bcmSwitchIgmpReportLeaveToCpu:
    case bcmSwitchIgmpReportLeaveDrop:
    case bcmSwitchIgmpReportLeaveFlood:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownToCpu:
    case bcmSwitchIgmpUnknownDrop:
    case bcmSwitchIgmpUnknownFlood:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldPktToCpu:
    case bcmSwitchMldPktDrop:
    case bcmSwitchMldReportDoneToCpu:
    case bcmSwitchMldReportDoneDrop:
    case bcmSwitchMldReportDoneFlood:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryToCpu:
    case bcmSwitchMldQueryDrop:
    case bcmSwitchMldQueryFlood:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDhcpPktToCpu:
    case bcmSwitchDhcpPktDrop:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_DHCP_CLIENT, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyToCpu:
    case bcmSwitchArpReplyDrop:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestToCpu:
    case bcmSwitchArpRequestDrop:
        rv = _dpp_trap_code_enable_switch_control_port_get(unit, SOC_PPC_TRAP_CODE_ARP, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPHashPktHeaderCount:
        rv = _bcm_dpp_switch_ecmp_hash_port_get(unit, port, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    /* As the  BCMDNX_ERR_EXIT_MSG has a goto statement the break is redundant. */
    /* coverity[unterminated_case:FALSE] */
    case bcmSwitchTrunkHashMPLSLabelBOS:
        if (SOC_DPP_CONFIG(unit)->pp.parser_mode != 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Switch control bcmSwitchTrunkHashMPLSLabelBOS is supported only in with the parser_mode = 1 SOC property")));
        }
    case bcmSwitchTrunkHashPktHeaderCount:
    case bcmSwitchTrunkHashPktHeaderSelect:
        rv = _bcm_dpp_switch_trunk_hash_port_get(unit, port, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpMldToCpu:
        rv = _bcm_dpp_switch_control_port_get(unit, port, bcmSwitchIgmpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_get(unit, port, bcmSwitchMldPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpDhcpToCpu:
        rv = _bcm_dpp_switch_control_port_get(unit, port, bcmSwitchDhcpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_get(unit, port, bcmSwitchArpReplyToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_get(unit, port, bcmSwitchArpRequestToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchColorSelect:
        rv = _bcm_dpp_switch_control_color_select_port_get(unit, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass7Queue:
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_pfc_tc_map_get,(unit, bcm_type - bcmSwitchPFCClass0Queue, port, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break;
/************************************************************************/
/*                                            Header                                                 */
/************************************************************************/
    case bcmSwitchPortHeaderType:


        /* Get TM Header type */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_get,(unit, core, tm_port, &soc_tmcport_header_type, &soc_tmcport_header_type)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

        /* Convert bcm port header type to tmc port header type */
        rv = _bcm_dpp_switch_control_port_header_type_map(unit, 0x1 /* to_bcm */, arg, &soc_tmcport_header_type);
        BCMDNX_IF_ERR_EXIT(rv);
        if (*arg == BCM_SWITCH_PORT_HEADER_TYPE_ETH) {
            soc_sand_rc = soc_ppd_port_info_get(unit, core, (SOC_PPC_PORT)pp_port, &port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
            if (port_info.tunnel_termination_profile == SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC && soc_property_get(unit, spn_PORT_RAW_MPLS_ENABLE, 0)) {
                *arg = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
            }
        }
        if ((*arg == BCM_SWITCH_PORT_HEADER_TYPE_TM) && SOC_IS_JERICHO(unit)) {
            /*
             * Check OTMH extension
             */
            int src_ext_en;
            SOC_TMC_PORTS_FTMH_EXT_OUTLIF outlif_ext_en;
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.get(unit, port, &src_ext_en);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.get(unit, port, &outlif_ext_en);
            SOCDNX_IF_ERR_EXIT(rv);

            if (src_ext_en && outlif_ext_en) {
                *arg = BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP;
            } else if (src_ext_en && !outlif_ext_en) {
                *arg = BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP;
            } else if (!src_ext_en && outlif_ext_en) {
                *arg = BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP;
            }
        }
        break;

    case bcmSwitchEgressKeepSystemHeader:

        if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
            /* Convert local port to PP port */
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

            /* Get PP Header type */
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_get,(unit, core, (uint32)pp_port, &pp_port_info)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

            /* The calling to MBCM_DPP_DRIVER_CALL inits pp_port_info */ 
            /* coverity[uninit_use:FALSE] */
            if ((pp_port_info.header_type_out == SOC_TMC_PORT_HEADER_TYPE_ETH) || (pp_port_info.header_type_out == SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP) ||
                (pp_port_info.header_type_out == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP) || (pp_port_info.header_type_out == SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW) ) {
                *arg = 0; /* PP egress editing is enabled (remove sys-header) */
            }
            else { 
                *arg = 1; /* PP egress editing is disabled (keep sys-header) */
            }
        }
        else {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Switch control bcmSwitchEgressKeepSystemHeader is supported only in Arad B0 and above")));
        }
        break;
    
    case bcmSwitchMcastTrunkHashMin:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_min_get,(unit, core, tm_port, (uint32 *)arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break;
    case bcmSwitchMcastTrunkHashMax:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_direct_lb_key_max_get,(unit, core, tm_port, (uint32 *)arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

        break;
    case bcmSwitchMeterAdjust:
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_meter_hdr_compensation_get, (unit, core, pp_port, SOC_TMC_PORT_DIRECTION_INCOMING, SOC_TMC_CNT_ADJSUT_TYPE_GENERAL, arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        } else {
            BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_hdr_compensation_get,(unit, core, pp_port, arg)));
		}
		rv = BCM_E_NONE;
		break;
    case bcmSwitchCounterAdjust:
    case bcmSwitchCounterAdjustEgressTm:
    case bcmSwitchCounterAdjustEgressPp:
        counter_adjust_type = (bcm_type == bcmSwitchCounterAdjustEgressTm) ?
             SOC_TMC_CNT_ADJUST_TYPE_EGRESS_TM : ((bcm_type == bcmSwitchCounterAdjustEgressPp) ?
             SOC_TMC_CNT_ADJUST_TYPE_EGRESS_PP : SOC_TMC_CNT_ADJSUT_TYPE_GENERAL);
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_cnt_meter_hdr_compensation_get,(unit, core, pp_port, SOC_TMC_PORT_DIRECTION_OUTGOING, counter_adjust_type, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        rv = BCM_E_NONE;
        break;
    case bcmSwitchPrependTagEnable:
        SOC_TMC_PORTS_SWAP_INFO_clear(&port_swap_info);
        /* Get Prepend TAG Enable value */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_swap_get,(unit, port, &port_swap_info)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

        /* Set the retrieved port swap enable state */
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *arg = port_swap_info.enable;
        break;

        /* Added support 48/32bit timestamp for ports */
    case bcmSwitchTimesyncEgressTimestampingMode:
        if (soc_feature(unit, soc_feature_timesync_support) &&
            soc_feature(unit, soc_feature_timesync_timestampingmode)) {
            uint32 rval[1];
            rv = BCM_E_NONE;
            if (SOC_PORT_TYPE(unit, port) == SOC_BLK_XLP) {
                BCMDNX_IF_ERR_EXIT(READ_XLPORT_MODE_REGr(unit, port, rval));
                *arg = soc_reg_field_get(unit, XLPORT_MODE_REGr, *rval,
                                         EGR_1588_TIMESTAMPING_MODEf);
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_GPORT) {
                BCMDNX_IF_ERR_EXIT(READ_GPORT_MODE_REGr(unit, port, rval));
                *arg = soc_reg_field_get(unit, GPORT_MODE_REGr, *rval,
                                         EGR_1588_TIMESTAMPING_MODEf);
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_CLP) {
                BCMDNX_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, port, rval));
                *arg = soc_reg_field_get(unit, CLPORT_MODE_REGr, *rval,
                                         EGR_1588_TIMESTAMPING_MODEf);
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_MXQ) {
                uint32 first_phy, pm_index /* zero-based */;
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
                BCMDNX_IF_ERR_EXIT(READ_NIF_PM_TIMESTAMPING_MODEr(unit, rval));
                first_phy --; /* zero-based */
                pm_index = (first_phy - SOC_DPP_DEFS_GET(unit, pmx_base_lane)) / 4;
                *arg = SHR_BITGET(rval, pm_index) ? 1 : 0;
            } else {
                rv = BCM_E_UNAVAIL;
            }
        } else {
            rv = BCM_E_UNAVAIL;
        }
        break;


    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control port type")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}   

int
_bcm_dpp_switch_control_port_set(int unit,
                                 bcm_port_t port,
                                 bcm_switch_control_t bcm_type,
                                 int arg)
{
    bcm_error_t                          rv = BCM_E_NONE;
    uint32                               soc_sand_rc;

    uint32 pp_port, tm_port;
    SOC_TMC_PORT_PP_PORT_INFO pp_port_info;
    SOC_PPC_PORT_INFO  port_info;
    SOC_TMC_PORT_HEADER_TYPE soc_tmcport_header_type;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success = SOC_SAND_SUCCESS;
    SOC_TMC_PORTS_SWAP_INFO port_swap_info;
    SOC_PPC_LLP_TRAP_PORT_INFO llp_trap_port_info;
    uint32    soc_sand_rv;
    int port_i, core, is_last, is_tdm_queuing_on = 0, counter_adjust_type = 0;
    _bcm_dpp_gport_info_t gport_info;
    bcm_gport_t gport;
    SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO egr_discount_info;
    bcm_pbmp_t affected_ports;
    bcm_port_nif_prio_t priority;
    uint32 base_q_pair,nof_pairs,curr_q_pair;
    ARAD_EGQ_PCT_TBL_DATA    pct_tbl_data;
    BCMDNX_INIT_FUNC_DEFS;

    if (bcm_type == bcmSwitchReserved955 && ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "prge_ptagged_load", 0) ||
                                              soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,  "lsr_manipulation", 0) )  ) {
        soc_sand_rc = _bcm_petra_port_outlif_profile_set(unit, port, arg, SOC_OCC_MGMT_OUTLIF_APP_CUSTOM_P_TAGGED_TYPE);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        BCM_EXIT; 
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));


    switch (bcm_type) {
    case bcmSwitchIgmpPktDrop:
    case bcmSwitchIgmpQueryToCpu:
    case bcmSwitchIgmpQueryDrop:
    case bcmSwitchIgmpQueryFlood:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIcmpRedirectToCpu:
        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core)));

            soc_sand_rv = soc_ppd_llp_trap_port_info_get(unit, core, pp_port, &llp_trap_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (arg != 0) {
                /*enable filter*/
                llp_trap_port_info.trap_enable_mask |= SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT;
            } else {
                /*disable filter */
                llp_trap_port_info.trap_enable_mask &= ~SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT;
            }

            soc_sand_rv = soc_ppd_llp_trap_port_info_set(unit, core, pp_port, &llp_trap_port_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }


        break;
    case bcmSwitchIgmpReportLeaveToCpu:
    case bcmSwitchIgmpReportLeaveDrop:
    case bcmSwitchIgmpReportLeaveFlood:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpUnknownToCpu:
    case bcmSwitchIgmpUnknownDrop:
    case bcmSwitchIgmpUnknownFlood:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_IGMP_UNDEFINED, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldPktToCpu:
    case bcmSwitchMldPktDrop:
    case bcmSwitchMldReportDoneToCpu:
    case bcmSwitchMldReportDoneDrop:
    case bcmSwitchMldReportDoneFlood:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchMldQueryToCpu:
    case bcmSwitchMldQueryDrop:
    case bcmSwitchMldQueryFlood:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchDhcpPktToCpu:
    case bcmSwitchDhcpPktDrop:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_DHCP_CLIENT, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpReplyToCpu:
    case bcmSwitchArpReplyDrop:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpRequestToCpu:
    case bcmSwitchArpRequestDrop:
        rv = _dpp_trap_code_enable_switch_control_port_set(unit, SOC_PPC_TRAP_CODE_ARP, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchECMPHashPktHeaderCount:
        rv = _bcm_dpp_switch_ecmp_hash_port_set(unit, port, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    /* As the  BCMDNX_ERR_EXIT_MSG has a goto statement the break is redundant. */
    /* coverity[unterminated_case:FALSE] */
    case bcmSwitchTrunkHashMPLSLabelBOS:
        if (SOC_DPP_CONFIG(unit)->pp.parser_mode != 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Switch control bcmSwitchTrunkHashMPLSLabelBOS is supported only in with the parser_mode = 1 SOC property")));
        }
    case bcmSwitchTrunkHashPktHeaderCount:
    case bcmSwitchTrunkHashPktHeaderSelect:
        rv = _bcm_dpp_switch_trunk_hash_port_set(unit, port, bcm_type, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchIgmpMldToCpu:
        rv = _bcm_dpp_switch_control_port_set(unit, port, bcmSwitchIgmpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_set(unit, port, bcmSwitchMldPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchArpDhcpToCpu:
        rv = _bcm_dpp_switch_control_port_set(unit, port, bcmSwitchDhcpPktToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_set(unit, port, bcmSwitchArpReplyToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_switch_control_port_set(unit, port, bcmSwitchArpRequestToCpu, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchColorSelect:
        rv = _bcm_dpp_switch_control_color_select_port_set(unit, port, arg);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass7Queue:
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_pfc_tc_map_set,(unit, bcm_type - bcmSwitchPFCClass0Queue, port, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break;
/************************************************************************/
/*                                            Header                                                 */
/************************************************************************/
    case bcmSwitchPortHeaderType:
    case bcmSwitchEgressKeepSystemHeader:

        if ((bcm_type == bcmSwitchPortHeaderType) || (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            if (bcm_type == bcmSwitchPortHeaderType) {
                rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit,port,&soc_tmcport_header_type);
                BCMDNX_IF_ERR_EXIT(rv);
                if(soc_tmcport_header_type != SOC_TMC_PORT_HEADER_TYPE_NONE) {
                    /* unset egress compensation */
                    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
                    BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, 0));
                    /* release egress port cls */
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_egress_port_discount_cls_free(unit, core, tm_port, &is_last));
                }

                soc_sand_rc = soc_ppd_port_info_get(unit, core, (SOC_PPC_PORT)pp_port, &port_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                if (arg == BCM_SWITCH_PORT_HEADER_TYPE_ETH) {
                    if (port_info.tunnel_termination_profile == SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC && soc_property_get(unit, spn_PORT_RAW_MPLS_ENABLE, 0)) {
                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT;
                        port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                        soc_sand_rc = soc_ppd_port_info_set(unit, core, (SOC_PPC_PORT)pp_port, &port_info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                        /* for egress pwe ccm counting, need to use tm_port_val to indicate MPLS RAW port */
                        if(SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "eg_pwe_counting", 0)){
                            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
                            /* get the queues mapped to this port */
                            soc_sand_rc = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                            soc_sand_rc = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
                            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

                            /* go over all q-pairs */
                            for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {
                                soc_sand_rc = arad_egq_pct_tbl_get_unsafe(unit,core,curr_q_pair,&pct_tbl_data);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                                /*clear bit3*/
                                pct_tbl_data.prog_editor_value = (pct_tbl_data.prog_editor_value & 0xFFFFFFF7);
                                soc_sand_rc = arad_egq_pct_tbl_set_unsafe(unit,core,curr_q_pair,&pct_tbl_data);
                                BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                            }
                       }
                    }
                }else {
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW){
                        if (!soc_property_get(unit, spn_PORT_RAW_MPLS_ENABLE, 0)) 
                            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("port_raw_mpls_enable must be on for header type BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW.")));

                        port_info.tunnel_termination_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
                        port_info.flags = SOC_PPC_PORT_IHP_VTT_PP_PORT_CONFIG_TBL;
                        soc_sand_rc = soc_ppd_port_info_set(unit, core, (SOC_PPC_PORT)pp_port, &port_info);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                        /* for egress pwe ccm counting, need to use tm_port_val to indicate MPLS RAW port */
                        if(SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "eg_pwe_counting", 0)){
                           BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
                           /* get the queues mapped to this port */
                           soc_sand_rc = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                           BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                           soc_sand_rc = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
                           BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

                           /* go over all q-pairs */
                           for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {
                               soc_sand_rc = arad_egq_pct_tbl_get_unsafe(unit,core,curr_q_pair,&pct_tbl_data);
                               BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                               /*set bit3*/
                               pct_tbl_data.prog_editor_value = (pct_tbl_data.prog_editor_value |0x8);
                               soc_sand_rc = arad_egq_pct_tbl_set_unsafe(unit,core,curr_q_pair,&pct_tbl_data);
                               BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                           }
                       }
                    }
                }
                /* Convert bcm port header type to tmc port header type */
                rv = _bcm_dpp_switch_control_port_header_type_map(unit, 0x0 /* from_bcm */, &arg, &soc_tmcport_header_type);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Set TM Header type */
                soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_set,(unit, core, tm_port, SOC_TMC_PORT_DIRECTION_BOTH, soc_tmcport_header_type)));
                
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_out_set(unit,port,soc_tmcport_header_type));
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_hdr_type_in_set(unit,port,soc_tmcport_header_type));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

                /* discount cls init */
                SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(&egr_discount_info);
                egr_discount_info.mc_credit_discount = DPP_COSQ_EG_DISCOUNT_DEFAULT_VALUE;
                egr_discount_info.uc_credit_discount = DPP_COSQ_EG_DISCOUNT_DEFAULT_VALUE;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_egress_port_discount_cls_init(unit, core, tm_port, &egr_discount_info));

                BCM_GPORT_LOCAL_SET(gport, port);
                /* TDM port is set to high piority by default */
                BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port, (uint8*)&is_tdm_queuing_on));
                if (arg == BCM_SWITCH_PORT_HEADER_TYPE_TDM_RAW || arg == BCM_SWITCH_PORT_HEADER_TYPE_TDM || is_tdm_queuing_on) 
                {
                    rv = bcm_petra_cosq_gport_sched_set(unit, gport, 0, BCM_COSQ_SP0, 0);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if ((arg == BCM_SWITCH_PORT_HEADER_TYPE_TDM || is_tdm_queuing_on) && SOC_IS_JERICHO(unit))
                    {
                        priority.priority_level = 2 ;/* TDM priority */
                        BCMDNX_IF_ERR_EXIT( _bcm_petra_port_nif_priority_set( unit, gport, 0, 1, &priority, &affected_ports ));
                    }
                }
                else {

                    /* non TDM port is set to low priority by default */
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_ETH || arg == BCM_SWITCH_PORT_HEADER_TYPE_TM) 
                    {
                        rv = bcm_petra_cosq_gport_sched_set(unit, gport, 0, BCM_COSQ_SP1, 0);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
                if (SOC_IS_JERICHO(unit)) {
                    /*
                     * Read soc property for OTMH extension
                     */
                    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN otmh_ext_en;
                    soc_dpp_config_pp_t *dpp_pp;
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_TM) {
                        if (soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(unit, port, &otmh_ext_en)) {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("soc_dpp_str_prop_parse_tm_port_otmh_extensions_en error")));
                        }
                        
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.set(unit, port, otmh_ext_en.src_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.dst_ext_en.set(unit, port, otmh_ext_en.dest_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.set(unit, port, otmh_ext_en.outlif_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        if(otmh_ext_en.outlif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE) {
                          SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge = 1;
                        }
                        if (otmh_ext_en.outlif_ext_en != SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER) {
                            dpp_pp = &(SOC_DPP_CONFIG(unit))->pp;
                            dpp_pp->otmh_cud_ext_used = 1;
                        }                        
                    }
                    /*
                     * OTMH extension set by API header type
                     */
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP) {
                        otmh_ext_en.src_ext_en = 1;
                        otmh_ext_en.outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER;
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.set(unit, port, otmh_ext_en.src_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.set(unit, port, otmh_ext_en.outlif_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_TM_ENCAP) {
                        otmh_ext_en.src_ext_en = 0;
                        otmh_ext_en.outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS;
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.set(unit, port, otmh_ext_en.src_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.set(unit, port, otmh_ext_en.outlif_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);

                    }
                    if (arg == BCM_SWITCH_PORT_HEADER_TYPE_TM_SSP_ENCAP) {
                        otmh_ext_en.src_ext_en = 1;
                        otmh_ext_en.outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS;
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.src_ext_en.set(unit, port, otmh_ext_en.src_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.outlif_ext_en.set(unit, port, otmh_ext_en.outlif_ext_en);
                        SOCDNX_IF_ERR_EXIT(rv);
                    }

                    SOCDNX_IF_ERR_EXIT(arad_ports_header_type_update(unit, port));
                }
            }

            /* Set PP Header type */
            SOC_TMC_PORT_PP_PORT_INFO_clear(&pp_port_info);
            if (bcm_type == bcmSwitchPortHeaderType) {
                pp_port_info.header_type = pp_port_info.header_type_out = soc_tmcport_header_type;
            }
            else { /* bcmSwitchEgressKeepSystemHeader */
                soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_get,(unit, core, (uint32)pp_port, &pp_port_info)));
                BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

                if (arg) { 
                    /* disable PP egress editing - keep sys-headers */
                    pp_port_info.header_type_out = SOC_TMC_PORT_HEADER_TYPE_STACKING;
                }
                else { 
                    /* enable PP egress editing - remove sys-headers */
                    pp_port_info.header_type_out = SOC_TMC_PORT_HEADER_TYPE_ETH;
                }
            }
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_set,(unit, core, (uint32)pp_port, &pp_port_info, &soc_sand_success)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

            if (soc_sand_success != SOC_SAND_SUCCESS) {
               BCMDNX_ERR_EXIT_MSG(_SHR_E_FAIL, (_BSL_BCM_MSG("PP port Header type set Failed.")));
            }
        }
        else {
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Switch control bcmSwitchEgressKeepSystemHeader is supported only in Arad B0 and above")));
        }
        break;

    case bcmSwitchMcastTrunkHashMin:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_direct_lb_key_min_set, (unit, core, tm_port, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break;

    case bcmSwitchMcastTrunkHashMax:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_direct_lb_key_max_set,(unit, core, tm_port, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break;

    case bcmSwitchStackRouteHistoryBitmap1:
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_route_history_bitmap_set,(unit, core, tm_port, SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK1, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        rv = BCM_E_NONE;
        break;

    case bcmSwitchStackRouteHistoryBitmap2:
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_stacking_route_history_bitmap_set,(unit, core, tm_port, SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK2, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        rv = BCM_E_NONE;
        break;
    case bcmSwitchMeterAdjust:
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_meter_hdr_compensation_set, (unit, core, pp_port, SOC_TMC_PORT_DIRECTION_INCOMING, SOC_TMC_CNT_ADJSUT_TYPE_GENERAL, arg))); 
            BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        } else {
            BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mtr_policer_hdr_compensation_set,(unit, core, pp_port, arg)));
        }
        rv = BCM_E_NONE;
        break;
    case bcmSwitchCounterAdjust:
    case bcmSwitchCounterAdjustEgressTm:
    case bcmSwitchCounterAdjustEgressPp:
        counter_adjust_type = (bcm_type == bcmSwitchCounterAdjustEgressTm) ?
             SOC_TMC_CNT_ADJUST_TYPE_EGRESS_TM : ((bcm_type == bcmSwitchCounterAdjustEgressPp) ? 
             SOC_TMC_CNT_ADJUST_TYPE_EGRESS_PP : SOC_TMC_CNT_ADJSUT_TYPE_GENERAL);
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_cnt_meter_hdr_compensation_set,(unit, core, pp_port, SOC_TMC_PORT_DIRECTION_OUTGOING, counter_adjust_type, arg)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        rv = BCM_E_NONE;
        break;
    case bcmSwitchPrependTagEnable:
        /* Set the port swap enable state */
        port_swap_info.enable = arg;

        /* Call the Prepend TAG Enable function */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_swap_set,(unit, port, &port_swap_info)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
        break; 
        /* Added support 48/32bit timestamp for ports */
    case bcmSwitchTimesyncEgressTimestampingMode:
        if (soc_feature(unit, soc_feature_timesync_support) &&
            soc_feature(unit, soc_feature_timesync_timestampingmode)) {
            rv = BCM_E_NONE;
            if (SOC_PORT_TYPE(unit, port) == SOC_BLK_XLP) {
                BCMDNX_IF_ERR_EXIT(
                    soc_reg_field32_modify(unit, XLPORT_MODE_REGr, port,
                                           EGR_1588_TIMESTAMPING_MODEf,
                                           (arg == bcmTimesyncTimestampingMode48bit) ? 1: 0));
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_GPORT) {
                BCMDNX_IF_ERR_EXIT(
                    soc_reg_field32_modify(unit, GPORT_MODE_REGr, port,
                                           EGR_1588_TIMESTAMPING_MODEf,
                                           (arg == bcmTimesyncTimestampingMode48bit) ? 1: 0));
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_CLP) {
                BCMDNX_IF_ERR_EXIT(
                    soc_reg_field32_modify(unit, CLPORT_MODE_REGr, port,
                                           EGR_1588_TIMESTAMPING_MODEf,
                                           (arg == bcmTimesyncTimestampingMode48bit) ? 1: 0));
            } else if (SOC_PORT_TYPE(unit, port) == SOC_BLK_MXQ) {
                int pm_index /* zero-based */;
                uint32 reg_val[1], first_phy;
                BCMDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
                BCMDNX_IF_ERR_EXIT(READ_NIF_PM_TIMESTAMPING_MODEr(unit, reg_val));
                first_phy --; /* zero-based */
                pm_index = (first_phy - SOC_DPP_DEFS_GET(unit, pmx_base_lane)) / 4;
                SHR_BITWRITE(reg_val, pm_index, (arg == bcmTimesyncTimestampingMode48bit) ? 1: 0);
                BCMDNX_IF_ERR_EXIT(WRITE_NIF_PM_TIMESTAMPING_MODEr(unit, *reg_val));
            } else {
                rv = BCM_E_UNAVAIL;
            }

            if (rv == BCM_E_NONE) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_48_BITS_1588_TS_ENABLEr(unit, core,
                                   (arg == bcmTimesyncTimestampingMode48bit) ? 1: 0));
            }

        } else {
            rv = BCM_E_UNAVAIL;
        }
        break;

    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported bcm switch control port type")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}   

int
_bcm_dpp_switch_control_congestion_enable_set(int unit,
                                              int arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_INFO info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

    soc_sand_rc = arad_cnm_cp_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    info.is_cp_enabled = arg;

    soc_sand_rc = arad_cnm_cp_set(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_congestion_mode_set(int unit,
                                              int arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_INFO info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rc = arad_cnm_cp_get(soc_sand_dev_id, &info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

  switch(arg)
  {
  case 0:
    info.pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_DUNE_PP;
    break;
  case 1:
    info.pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_EXT_PP;
    break;
    case 2:
        info.pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_SAMPLING;
        break;
    case 3:
        info.pkt_gen_mode = SOC_TMC_CNM_GEN_MODE_HIGIG;
        break;
    default:
       LOG_ERROR(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "unit %d. Invalid CNM mode (%d), valid modes are 0-3.\n"), unit, arg));
        rv = BCM_E_PARAM;
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_rc = arad_cnm_cp_set(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_congestion_enable_get(int unit,
                                              int *arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_INFO info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

    soc_sand_rc = arad_cnm_cp_get(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    *arg = info.is_cp_enabled;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_congestion_mode_get(int unit,
                                            int *arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_INFO info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rc = arad_cnm_cp_get(soc_sand_dev_id, &info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

  switch(info.pkt_gen_mode)
  {
  case SOC_TMC_CNM_GEN_MODE_DUNE_PP:
    *arg = 0;
    break;
  case SOC_TMC_CNM_GEN_MODE_EXT_PP:
    *arg = 1;
    break;
    case SOC_TMC_CNM_GEN_MODE_SAMPLING:
        *arg = 2;
        break;
    case SOC_TMC_CNM_GEN_MODE_HIGIG:
        *arg = 3;
        break;
    default:
        *arg = -1;
    }

  BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_congestion_options_set(int unit, bcm_switch_control_t bcm_type, int arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_OPTIONS info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rc = arad_cnm_cp_options_get(soc_sand_dev_id, &info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

  switch(bcm_type)
  {
    case bcmSwitchCongestionMissingCntag:
      info.gen_if_no_cn_tag = arg;
      break;
    case bcmSwitchCongestionExcludeReplications:
      info.gen_if_replications = (arg ? 0 : 1);
      break;
    case bcmSwitchCongestionCntag:
        info.disable_cn_tag_if_exist_in_dram = arg;
        break;
    case bcmSwitchCongestionCntagEthertype:
        info.cn_tag_ethertype = arg;
        break;
    default:
        break;
    }

    soc_sand_rc = arad_cnm_cp_options_set(soc_sand_dev_id, &info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

  BCMDNX_IF_ERR_EXIT(rv);
exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_switch_control_congestion_options_get(int unit, bcm_switch_control_t bcm_type, int *arg)
{
  int rv = BCM_E_NONE;
  int soc_sand_dev_id, soc_sand_rc;
  ARAD_CNM_CP_OPTIONS info;
  
  BCMDNX_INIT_FUNC_DEFS;
  soc_sand_dev_id = (unit);

  soc_sand_rc = arad_cnm_cp_options_get(soc_sand_dev_id, &info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

  switch(bcm_type)
  {
    case bcmSwitchCongestionMissingCntag:
      *arg = info.gen_if_no_cn_tag;
      break;
    case bcmSwitchCongestionExcludeReplications:
      *arg = (info.gen_if_replications ? 0 : 1);
      break;
    case bcmSwitchCongestionCntag:
        *arg = info.disable_cn_tag_if_exist_in_dram;
        break;
    case bcmSwitchCongestionCntagEthertype:
        *arg = info.cn_tag_ethertype;
        break;
    default:
        break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/* 
 * Begin BCM Functions
 */
/*
 * Function:
 *      bcm_petra_switch_temperature_monitor_get
 * Purpose:
 *      Get temperature readings on all sensors on Petra.
 * Parameters:
 *      unit -
 *        (IN) Unit number.
 *      temperature_max -
 *        (IN) Maximal number of acceptable temperature sensors
 *      temperature_array -
 *        (OUT) Pointer to array to be loaded by this procedure. Each
 *        element contains current temperature and peak temperature.
 *      temperature_count -
 *        (OUT) Pointer to memory to be loaded by this procedure. This
 *        is the number of sensors (and, therefore, the number of valid
 *        elements on temperature_array).
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
   bcm_petra_switch_temperature_monitor_get(
                        int unit, 
                        int temperature_max, 
                        bcm_switch_temperature_monitor_t *temperature_array, 
                        int *temperature_count)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_INIT_CHECK;

    BCMDNX_NULL_CHECK(temperature_array);
    BCMDNX_NULL_CHECK(temperature_count);
    if (temperature_max <= 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_SOC_MSG("temperature_max expected to be bigger than 0.\n")));
    }
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_temp_pvt_get, (unit, temperature_max, temperature_array, temperature_count));
    BCMDNX_IF_ERR_EXIT(rv);

exit: 
   BCMDNX_FUNC_RETURN; 
}
/*
 * Function:
 *      bcm_petra_switch_control_get
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) <UNDEF>
 *      arg - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_switch_control_get(int unit, 
                             bcm_switch_control_t type, 
                             int *arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_get(unit, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    /* do NOT add _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE 
       (this function is called by dispatcher after every api call) !! */
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_control_set
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) <UNDEF>
 *      arg - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_switch_control_set(int unit, 
                             bcm_switch_control_t type, 
                             int arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_set(unit, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_control_port_get
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      type - (IN) <UNDEF>
 *      arg - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_switch_control_port_get(int unit, 
                                  bcm_port_t port, 
                                  bcm_switch_control_t type, 
                                  int *arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_port_get(unit, port, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_control_port_set
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) <UNDEF>
 *      type - (IN) <UNDEF>
 *      arg - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_switch_control_port_set(int unit, 
                                  bcm_port_t port, 
                                  bcm_switch_control_t type, 
                                  int arg)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_port_set(unit, port, type, arg);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_stable_register
 * Purpose:
 *      Register read/write functions for the application provided
 *      stable for Level 2 Warm Boot
 * Parameters:
 *      unit - (IN) Unit number.
 *      rf - (IN) <UNDEF>
 *      wf - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_switch_stable_register(int unit, 
                                 bcm_switch_read_func_t rf, 
                                 bcm_switch_write_func_t wf)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_switch_stable_register is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/* 
 * Switch Event Function
 */

/*
 * Function:
 *    bcm_petra_switch_event_register
 * Description:
 *    Registers a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *  cb          - The desired call back function to register for critical events.
 *  userdata    - Pointer to any user data to carry on.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      
 *    Several call back functions could be registered, they all will be called upon
 *    critical event. If registered callback is called it is adviced to log the 
 *  information and reset the chip. 
 *  Same call back function with different userdata is allowed to be registered. 
 */
int 
bcm_petra_switch_event_register(int unit, bcm_switch_event_cb_t cb, void *userdata)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_SWITCH_LOCK_TAKE; 
    rc = soc_event_register(unit, (soc_event_cb_t)cb, userdata);
     BCMDNX_IF_ERR_EXIT(rc);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *    bcm_petra_switch_event_unregister
 * Description:
 *    Unregisters a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *  cb          - The desired call back function to unregister for critical events.
 *  userdata    - Pointer to any user data associated with a call back function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      
 *  If userdata = NULL then all matched call back functions will be unregistered,
 */
int 
bcm_petra_switch_event_unregister(int unit, bcm_switch_event_cb_t cb, void *userdata)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_SWITCH_LOCK_TAKE;
    rc = soc_event_unregister(unit, (soc_event_cb_t)cb, userdata);
     BCMDNX_IF_ERR_EXIT(rc);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_event_control_set
 * Description:
 *    Set event control
 * Parameters:
 *    unit        - Device unit number
 *  type        - Event action.
 *  value       - Event value
 * Returns:
 *      BCM_E_xxx
 */
int bcm_petra_switch_event_control_set(
    int unit, 
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type, 
    uint32 value)
{
    int rc = BCM_E_NONE, nof_interrupts;
    soc_interrupt_db_t* interrupts;
    int inter = 0;
    soc_block_info_t *bi;
    int bi_index = 0, inf_index=type.index, port;
    int is_valid;
    soc_block_types_t block_types;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_SWITCH_LOCK_TAKE;

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("supports only interrupts event")));
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts) {
       BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device")));
    }

    /*verify interrupt id*/
    rc = soc_nof_interrupts(unit, &nof_interrupts);
    BCMDNX_IF_ERR_EXIT(rc);

    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL) {
        if (type.event_id >= nof_interrupts || type.event_id < 0) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
        }

        /*verify block instance*/
        if (!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg)) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_INTERNAL, (_BSL_SOC_MSG("Invalid interrupt register for the device")));
        }
        block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;
        if (!SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_CLP) && !SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_XLP)) {
            rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
            BCMDNX_IF_ERR_EXIT(rc);
            if (!is_valid) {
               BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
            }
        }

        inf_index = soc_interrupt_get_block_index_from_port(unit, type.event_id, type.index);
        if (inf_index < 0) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
        }

    }

    /*switch case for all*/
    switch (type.action) {

    case bcmSwitchEventMask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
#ifdef BCM_CMICM_SUPPORT
            if (value) {
                if (!soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
                    soc_cmicm_intr2_disable(unit, 0x1e);
                }
                soc_cmicm_intr3_disable(unit, 0xFFFFFFFF);
                soc_cmicm_intr4_disable(unit, 0xFFFFFFFF);
#ifdef BCM_JERICHO_SUPPORT 
                if ( SOC_IS_JERICHO(unit) ){
                    soc_cmicm_intr5_disable(unit, 0xFFFFFFFF);
                    soc_cmicm_intr6_disable(unit, 0xFFFFFFFF);
                }
#endif /* BCM_JERICHO_SUPPORT */
            } else {
                if (!soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
                    soc_cmicm_intr2_enable(unit, 0x1e);
                }
                soc_cmicm_intr3_enable(unit, 0xFFFFFFFF);
                soc_cmicm_intr4_enable(unit, 0xFFFFFFFF);
#ifdef BCM_JERICHO_SUPPORT 
                if ( SOC_IS_JERICHO(unit) ){
                    soc_cmicm_intr5_enable(unit, 0xFFFFFFFF);
                    soc_cmicm_intr6_enable(unit, 0xFFFFFFFF);
                }
#endif /* BCM_JERICHO_SUPPORT */
            }

#else
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Mask all supported only if BCM_CMICM_SUPPORT ")));
#endif  /*BCM_CMICM_SUPPORT*/
        } else {
            /* Set per interrupt */
            if (value) {
                rc = soc_interrupt_disable(unit, type.index, &(interrupts[type.event_id]));
            } else {
                rc = soc_interrupt_enable(unit, type.index, &(interrupts[type.event_id]));
            }
            BCMDNX_IF_ERR_EXIT(rc);
        }

        break;
    case bcmSwitchEventForce:
        /* Set/clear per interrupt */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {

                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                    if (!SOC_INFO(unit).block_valid[bi_index]) {
                        continue;
                    }

                    bi = &(SOC_BLOCK_INFO(unit, bi_index));

                    rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                    BCMDNX_IF_ERR_EXIT(rc);
                    if (is_valid) {

                        port = soc_interrupt_get_intr_port_from_index(unit, inter, bi->number);

                        rc = soc_interrupt_force(unit, port, &(interrupts[inter]), 1 - value);
                        BCMDNX_IF_ERR_EXIT(rc);
                    }
                }
            }
        } else {
            rc = soc_interrupt_force(unit, type.index, &(interrupts[type.event_id]), 1 - value);
            BCMDNX_IF_ERR_EXIT(rc);
        }

        break;
    case bcmSwitchEventClear:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            if (value) {
                rc = soc_interrupt_clear_all(unit);
                BCMDNX_IF_ERR_EXIT(rc);
            } else {
               BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid event clear parameter")));
            }
        } else {
            /* Set per interrupt */
            if (value) {
                if (NULL == interrupts[type.event_id].interrupt_clear) {
                    if (interrupts[type.event_id].vector_id == 0) {
                        /*BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id"))); */
                        LOG_WARN(BSL_LS_BCM_SWITCH,
                                 (BSL_META_U(unit,
                                             "Warning: Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id\n")));
                    } else {
                        LOG_WARN(BSL_LS_BCM_SWITCH,
                                 (BSL_META_U(unit,
                                             "Warning: call to interrupt clear for vector pointer, nothing done\n")));
                    }
                } else {
                    rc = interrupts[type.event_id].interrupt_clear(unit, type.index, type.event_id);
                    BCMDNX_IF_ERR_EXIT(rc);
                }

            } else {
               BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid event clear parameter")));
            }
        }
        break;
        case bcmSwitchEventRead:
            if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid read parameter (event_id)")));
            }
#ifdef BCM_CMICM_SUPPORT
            else {
                if (soc_property_get(unit, spn_L2_LEARN_LIMIT_MODE, 0) == mac_limit_per_tunnel_port) {
                    rc = shr_llm_mac_move_event_set(unit, type.event_id, value);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
#endif
            break;

    case bcmSwitchEventStormTimedCount:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {
                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                    if (!SOC_INFO(unit).block_valid[bi_index]) {
                        continue;
                    }

                    bi = &(SOC_BLOCK_INFO(unit, bi_index));
                    rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                    BCMDNX_IF_ERR_EXIT(rc);

                    if (is_valid) {
                        rc = soc_interrupt_storm_timed_count_set(unit, inter, value);
                        BCMDNX_IF_ERR_EXIT(rc);
                        (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                    }
                }
            }
        } else {
            /* Set per interrupt */
            rc = soc_interrupt_storm_timed_count_set(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
            (interrupts[type.event_id].storm_detection_occurrences)[inf_index] = 0x0;
        }
        break;

    case bcmSwitchEventStormTimedPeriod:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {
                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                    if (!SOC_INFO(unit).block_valid[bi_index]) {
                        continue;
                    }

                    bi = &(SOC_BLOCK_INFO(unit, bi_index));
                    rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                    BCMDNX_IF_ERR_EXIT(rc);

                    if (is_valid) {
                        rc = soc_interrupt_storm_timed_period_set(unit, inter, value);
                        BCMDNX_IF_ERR_EXIT(rc);

                        (interrupts[inter].storm_detection_start_time)[bi->number] = 0x0;
                    }
                }
            }
        } else {
            /* Set per interrupt */
            rc = soc_interrupt_storm_timed_period_set(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
            (interrupts[type.event_id].storm_detection_start_time)[inf_index] = 0;
        }
        break;

    case bcmSwitchEventStormNominal:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            SOC_SWITCH_EVENT_NOMINAL_STORM(unit) = value;
            /*Warm boot buffer updating*/
            SOCDNX_IF_ERR_EXIT(SWITCH_ACCESS.interrupts_event_storm_nominal.set(unit, value));
            for (inter = 0; inter < nof_interrupts; inter++) {
                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                    if (!SOC_INFO(unit).block_valid[bi_index]) {
                        continue;
                    }

                    bi = &(SOC_BLOCK_INFO(unit, bi_index));
                    rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                    BCMDNX_IF_ERR_EXIT(rc);

                    if (is_valid) {
                         (interrupts[inter].storm_nominal_count)[bi->number] = 0x0;
                         (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                    }
                }
            }

        } 
        else {
            /* Set per interrupt */
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid nominal storm detection parameter (event_id)")));
        }
        break;

    case bcmSwitchEventStat:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            if (value) {
                for (inter = 0; inter < nof_interrupts; inter++) {
                    for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                        if (!SOC_INFO(unit).block_valid[bi_index]) {
                            continue;
                        }

                        bi = &(SOC_BLOCK_INFO(unit, bi_index));
                        rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                        BCMDNX_IF_ERR_EXIT(rc);

                        if (is_valid) {
                            (interrupts[inter].statistics_count)[bi->number] = 0x0;
                        }
                    }
                }
            } else {
               BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid statistics parameter value")));
            }
        } else {
            (interrupts[type.event_id].statistics_count)[inf_index] = 0x0;
        }
        break;

    case bcmSwitchEventLog:

        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;
            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventCorrActOverride:
    /* Value - 0 : Only bcm callback will be called for this interrupt.
       Value - 1 : Only user callback will be called for this interrupt. At this mode BCM driver will only print the interrupt information for logging.
       Value - 2 : User call back will be called immediately after bcm callback. */

        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if(SOC_IS_JERICHO(unit)) {
                    if (value == 2) {
                        flags |= SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                    } else {
                        flags &= ~SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                    }
                }

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;

            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if(SOC_IS_JERICHO(unit)) {
                if (value == 2) {
                    flags |= SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                } else {
                    flags &= ~SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                }
            }

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventPriority:

        if (value > SOC_INTERRUPT_DB_FLAGS_PRIORITY_MAX_VAL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Priority value is out af range")));
        }

        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                SHR_BITCOPY_RANGE(&flags, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB, &value, 0, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN);

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);
            }
        } else {

            uint32 flags;

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            SHR_BITCOPY_RANGE(&flags, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB, &value, 0, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN);

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventUnmaskAndClearDisable:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;
            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    case bcmSwitchEventForceUnmask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;
            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    default:
       BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG_STR("Unsupported control")));
        break;
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_event_control_get
 * Description:
 *    Get event control
 * Parameters:
 *    unit        - Device unit number
 *  type        - Event action.
 *  value       - Event value
 * Returns:
 *      BCM_E_xxx
 */
int bcm_petra_switch_event_control_get(
    int unit, 
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type, 
    uint32 *value)
{
    int rc = BCM_E_NONE, nof_interrupts;
    soc_interrupt_db_t* interrupts;
    int inter_get;
    int is_enable;
    soc_block_types_t block_types;
    int is_valid;
    int inf_index = type.index;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(value);
    DPP_SWITCH_LOCK_TAKE;

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("supports only interrupts event")));
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts) {
       BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device")));
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    BCMDNX_IF_ERR_EXIT(rc);



    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL) {
        if (type.event_id >= nof_interrupts || type.event_id < 0) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
        }

        /*verify block instance*/
        if (!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg)) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_INTERNAL, (_BSL_SOC_MSG("Invalid interrupt register for the device")));
        }

        block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;

        if (!SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_CLP) && !SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_XLP)) {
            rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
            BCMDNX_IF_ERR_EXIT(rc);
            if (!is_valid) {
               BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
            }
        }

        inf_index = soc_interrupt_get_block_index_from_port(unit, type.event_id, type.index);
        if (inf_index < 0) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
        }
    }
   LOG_VERBOSE(BSL_LS_BCM_SWITCH,
               (BSL_META_U(unit,
                           "%s(): interrupt id=%3d, name=%s\n"), FUNCTION_NAME(), type.event_id, interrupts[type.event_id].name));

    switch (type.action) {
    case bcmSwitchEventMask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            rc = soc_interrupt_is_all_mask(unit, (int *)value);
            BCMDNX_IF_ERR_EXIT(rc);
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_is_enabled(unit, type.index, &(interrupts[type.event_id]), &is_enable);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = (is_enable == 0);
        }
        break;

    case bcmSwitchEventForce:
        /* Set/clear per interrupt */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid <controll all> event parameter for force option")));
        } else {
            rc = soc_interrupt_force_get(unit, type.index, &(interrupts[type.event_id]), &is_enable);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = is_enable;
        }
        break;

    case bcmSwitchEventClear:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            rc = soc_interrupt_is_all_clear(unit, (int *)value);
            BCMDNX_IF_ERR_EXIT(rc);
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = (inter_get == 0);
        }
        break;

    case bcmSwitchEventRead:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid read parameter (event_id)")));
        } else {
#ifdef BCM_CMICM_SUPPORT
            if (soc_property_get(unit, spn_L2_LEARN_LIMIT_MODE, 0) == mac_limit_per_tunnel_port) {
                rc = shr_llm_mac_move_event_get(unit, type.event_id, value);
                BCMDNX_IF_ERR_EXIT(rc);
            } else
#endif
            {
                /* Get per interrupt */
                rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
                BCMDNX_IF_ERR_EXIT(rc);
                *value = (inter_get == 1);
            }
        }
        break;

    case bcmSwitchEventStormTimedPeriod:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Storm Timed Period parameter (event_id)")));
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_storm_timed_period_get(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventStormTimedCount:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Storm Timed Count parameter (event_id)")));
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_storm_timed_count_get(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);

        }
        break;

    case bcmSwitchEventStormNominal:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            *value = SOC_SWITCH_EVENT_NOMINAL_STORM(unit);
        } else {
            /* Get per interrupt */
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid nominal storm parameter (event_id)")));
        }
        break;

    case bcmSwitchEventStat:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Statistics Control parameter (event_id)")));
        } else {
            /* Get per interrupt */
            *value = (interrupts[type.event_id].statistics_count)[inf_index];
        }
        break;

    case bcmSwitchEventLog:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {

            uint32 flags;
            /* Get per interrupt */

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE) != 0) ? 0x1 : 0x0;
        }
        break;

    case bcmSwitchEventCorrActOverride:
    /* Value - 0 : Only bcm callback will be called for this interrupt.
       Value - 1 : Only user callback will be called for this interrupt. At this mode BCM driver will only print the interrupt information for logging.
       Value - 2 : User call back will be called immediately after bcm callback. */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {
            /* Get per interrupt */
            uint32 flags;

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if(SOC_IS_JERICHO(unit)) {
                if (flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) {
                    *value = 0x2;
                    break;
                } 
            }
            *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE) != 0) ? 0x1 : 0x0;
        }
        break;

    case bcmSwitchEventPriority:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {
            uint32 flags;

            /* Get per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value = ((flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB);
        }
        break;

    case bcmSwitchEventUnmaskAndClearDisable:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {

            uint32 flags;
            /* Get per interrupt */

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS) != 0) ? 0x1 : 0x0;
        }
        break;
    case bcmSwitchEventForceUnmask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {

            uint32 flags;
            /* Get per interrupt */

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS) != 0) ? 0x1 : 0x0;
        }
        break;

    default:
       BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported control")));
        break;
    }

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_user_buffer_write( 
    int unit, 
    uint32 flags, 
    bcm_switch_user_buffer_type_t buff_type, 
    uint8 *buf, 
    int offset, 
    int nbytes)
{
    uint32 
        soc_sand_rv,
        mbcm_flags = 0x0;

    BCMDNX_INIT_FUNC_DEFS;

    if (buf == NULL) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "%s: Unit %d, passed argument 'buf' is NULL \n"),FUNCTION_NAME(), unit));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    if (flags & BCM_SWITCH_USER_BUFFER_LOGICAL2PHY_TRANS) {
        mbcm_flags |= SOC_ARAD_DRAM_USER_BUFFER_FLAGS_LOGICAL2PHY_TRANS;
    }

    switch (buff_type) {
    case bcmSwitchUserBufferTypeDram:
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_user_buffer_dram_write, (unit, mbcm_flags, buf, offset, nbytes)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "%s: Unit %d, buffer type %d is not supported \n"), FUNCTION_NAME(), unit, buff_type));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_user_buffer_read( 
    int unit, 
    uint32 flags, 
    bcm_switch_user_buffer_type_t buff_type, 
    uint8 *buf, 
    int offset, 
    int nbytes)
{
    uint32 
        soc_sand_rv,
        mbcm_flags = 0x0;

    BCMDNX_INIT_FUNC_DEFS;

    if (buf == NULL) {
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "%s: Unit %d, passed argument 'buf' is NULL \n"),FUNCTION_NAME(), unit));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
 
    if (flags & BCM_SWITCH_USER_BUFFER_LOGICAL2PHY_TRANS) {
        mbcm_flags |= SOC_ARAD_DRAM_USER_BUFFER_FLAGS_LOGICAL2PHY_TRANS;
    }

    switch (buff_type) {
    case bcmSwitchUserBufferTypeDram:
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_user_buffer_dram_read, (unit, mbcm_flags, buf, offset, nbytes)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_SWITCH,
                  (BSL_META_U(unit,
                              "%s: Unit %d, buffer type %d is not supported \n"),FUNCTION_NAME(), unit, buff_type));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_switch_service_set(
    int unit, 
    bcm_switch_service_t service, 
    bcm_switch_service_config_t *config)
{
    int enable;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(config);
    
    switch(service){
    case bcmServiceCounterCollection:
        switch(config->enabled){
        case bcmServiceStateEnabled:
            enable = TRUE;
            break;
        case bcmServiceStateDisabled:
            enable = FALSE;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid config->enabled param %d"), config->enabled));
        }        
        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_background_collection_enable_set(unit, enable));
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is unavailable for service %d"), service));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_switch_service_get(
    int unit, 
    bcm_switch_service_t service, 
    bcm_switch_service_config_t *config)
{
    int enable;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(config);

    bcm_switch_service_config_t_init(config);
    switch(service){
    case bcmServiceCounterCollection:
        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_background_collection_enable_get(unit, &enable));
        config->enabled = (enable)? bcmServiceStateEnabled : bcmServiceStateDisabled;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The API is unavailable for service %d"), service));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_switch_l3_protocol_group_set(int unit, uint32 group_members, bcm_l3_protocol_group_id_t group_id){
    int rv, i;
    uint32 internal_flags;
    uint32 nof_members;
    uint32 old_group_members, curr_group_members;
    BCMDNX_INIT_FUNC_DEFS;

    /** 
     *  Input validation
 */
    DPP_SWITCH_L3_PROTOCOL_GROUP_VERIFY(unit);

    if (group_id >= SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given protocol group is too high.")));
    }

    if (!group_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't change group 0. Remove members from other groups to put them in group 0.")));
    }

    /* Translate flags to internal flags */
    DPP_SWITCH_L3_PROTOCOL_GROUP_TRANSLATE_API_FLAGS_TO_INTERNAL(group_members, internal_flags);

    /* Traverse all groups other than 0. If the given protocol group is in use, or one of the protocols is in a group that's in use,
       return error */
    for (i = DPP_SWITCH_L3_PROTOCOL_GROUP_LOWEST_VALID ; i < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) ; i++) {
        /* Read SW DB */
        rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.get(unit, i, &curr_group_members);
        BCMDNX_IF_ERR_EXIT(rv);
        if (i == group_id || (internal_flags & curr_group_members)) {
            /* verify that group is not in use. */
            rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_counts.get(unit, i, &nof_members);
            BCMDNX_IF_ERR_EXIT(rv);
            if (nof_members) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Protocol group is in use, or one of the protocols is in use. "
                                                          "Delete all multiple mymac termination or vrrp entries "
                                                          "associated with it and try again.")));
            }
        }
    }

    /** 
     * Write to HW 
 */ 

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mymac_protocol_group_set, (unit, internal_flags, group_id));
    BCMDNX_IF_ERR_EXIT(rv);

    /** 
     *  Update SW DB
 */ 

    /* Get old members of the group so they can be set to group 0. */
    rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.get(unit, group_id, &old_group_members);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Iterate over all the groups. When reaching group 0, update it to contain all the protocols that were removed from group_id.
       When reaching group_id, update it with the new members. From all other groups, remove the group members. */
    for (i = 0 ; i < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) ; i++) {
        rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.get(unit, i, &curr_group_members);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Remove the new group members from all groups. */
        curr_group_members &= ~internal_flags;

        if (i == 0) {
            /* Set all old members of the group that are not in the group anymore to group 0. */
            curr_group_members |= (old_group_members & ~internal_flags);
        }

        if (i == group_id) {
            /* Set the group with its new members. */
            curr_group_members = internal_flags;
        }

        /* Write to SW DB. */
        rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.set(unit, i, curr_group_members);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_switch_l3_protocol_group_get_members_by_group
 * Purpose:
 *      Given a group_id and group mask, returns the members of all groups that fit the id & mask.
 * Parameters: 
 *      unit        :   (IN) Device to be configured.
 *      group_id    :   (IN) Required group id.
 *      group_mask  :   (IN) Required group mask.
 *      group_members : (OUT)Members of all groups that fit group_id & mask, encoded in SOC_PPC_L3_VRRP_PROTOCOL_GROUP_* flags.
 *  
 * Returns:
 *      BCM_E_PARAM : If group_id or group mask are greater than SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)
 *      BCM_E_*     : If there was an error in the WB DB.
 *      BCM_E_NONE  : Otherwise.
 */
int
bcm_dpp_switch_l3_protocol_group_get_members_by_group(int unit, uint32 group_id, uint32 group_mask, uint32 *group_members){
    int rv, i;
    uint32 internal_flags;

    BCMDNX_INIT_FUNC_DEFS;

    /* Validate input. */
    if (group_id >= SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given protocol group is too high.")));
    }
    if (group_mask >= SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given protocol group mask is too high.")));
    }

    *group_members = 0;
    /* Read all groups from SW database. If the group matches the mask, add its members to the return value. */
    for (i = 0; i < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) ; i++) {
        if ((i & group_mask) == (group_id & group_mask)) {
            rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.get(unit, i, &internal_flags);
            BCMDNX_IF_ERR_EXIT(rv);
            *group_members |= internal_flags;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_switch_l3_protocol_group_get_group_by_members
 * Purpose:
 *      Given protocol flags, returns the minimal group_id - group_mask combination that contains all flags.
 * Parameters: 
 *      unit        :   (IN) Device on which the protocols are configured.
 *      group_members : (IN) Required protocols, encoded in SOC_PPC_L3_VRRP_PROTOCOL_GROUP_* flags.
 *      group_id    :   (OUT) Group_id containing the flags.
 *      group_mask  :   (OUT) Minimal mask required to contain the flags.
 *      exact_match :   (IN) If set, returns error in case the groups contain more than just the asked protocols.
 *  
 * Returns:
 *      BCM_E_*     : If there was an error in the WB DB.
 *      BCM_E_NONE  : Otherwise.
 */
int
bcm_dpp_switch_l3_protocol_group_get_group_by_members(int unit, uint32 protocol_flags, uint32 *group_id, uint32 *group_mask, uint8 exact_match){
    int i, rv;
    uint32 internal_flags, and_groups, nand_groups, existing_members = 0;
    BCMDNX_INIT_FUNC_DEFS;

    if (!protocol_flags) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't find empty protocol group.")));
    }

    *group_id = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID;
    *group_mask = DPP_SWITCH_L3_PROTOCOL_GROUP_MASK_EXACT;

    /* Traverse over the groups. If a match is found, keep it. If more than one match is found, calculate all the matches's mask.*/
    for (i = 0; i < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) ; i++) {
        rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.get(unit, i, &internal_flags);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* 
         * If there's a match between the required protocol and the group, accumulate it.
         *   We'll return error later if we accumulated too many groups.
         */
        if (internal_flags & protocol_flags) {
            if (*group_id == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
                /* When the first match if found, keep it. */
                *group_id = i;
            } else {
                /* Only keep in the mask the bits that are common to all the groups found. */
                and_groups = *group_id & i;
                nand_groups = ~(*group_id) & ~i;
                *group_mask &= (and_groups | nand_groups);
            }

            /* Accumulate all members of the group in case the user wants to be warned. */
            existing_members |= internal_flags;
        } 
    }

    if (exact_match && (existing_members != protocol_flags)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Required protocols' group(s) contains other members. Remove extra members and try again.")));
    }

    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_switch_l3_protocol_group_get(int unit, uint32 *group_members, bcm_l3_protocol_group_id_t *group_id){
    int rv;
    uint32 internal_flags, requested_flags, group_mask;
    BCMDNX_INIT_FUNC_DEFS;

    /* Input validation */
    DPP_SWITCH_L3_PROTOCOL_GROUP_VERIFY(unit); 

    if (*group_members == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID && *group_id == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Either group members or group_id must be set.")));
    } 

    if (*group_members != SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID && *group_id != SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group members and group_id cant be both set.")));
    }

    /* Get members by group */
    if (*group_members == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
        rv = bcm_dpp_switch_l3_protocol_group_get_members_by_group(unit, *group_id, DPP_SWITCH_L3_PROTOCOL_GROUP_MASK_EXACT, &internal_flags);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Translate to API. */
        DPP_SWITCH_L3_PROTOCOL_GROUP_TRANSLATE_INTERNAL_FLAGS_TO_API(internal_flags, *group_members);
        BCM_EXIT;
    }

    /* Get group by members */
    if (*group_id == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
        /* Translate the flags to match the internal flags */
        DPP_SWITCH_L3_PROTOCOL_GROUP_TRANSLATE_API_FLAGS_TO_INTERNAL(*group_members, requested_flags);

        rv = bcm_dpp_switch_l3_protocol_group_get_group_by_members(unit, requested_flags, group_id, &group_mask, FALSE);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If the mask is not exact match mask, then the protocols are in more than one group. Return error. */
        if (group_mask != DPP_SWITCH_L3_PROTOCOL_GROUP_MASK_EXACT || *group_id == SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID) {
            *group_id = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_INVALID;
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Given protocols are in more than one group.")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dpp_switch_l3_protocol_group_change_count
 * Purpose:
 *      Given protocol group and protocol mask, changes the count of the groups that fit them by the amount given in diff. 
 * Parameters: 
 *      unit        :   (IN) Device on which the protocols are configured.
 *      group_id    :   (IN) Required group_id.
 *      group_mask  :   (IN) Required group mask.
 *      diff        :   (IN) The amount by which the count will change (can be positive or negative).
 *  
 * Returns: 
 *      BCM_E_INTERNAL : If the count for one of the groups go below zero.  
 *      BCM_E_*     : If there was an error in the WB DB.
 *      BCM_E_NONE  : Otherwise.
 */
int
bcm_dpp_switch_l3_protocol_group_change_count(int unit, uint32 group_id, uint32 group_mask, int diff){
    int rv, i;
    uint32 count;
    BCMDNX_INIT_FUNC_DEFS;

    /* For each entry that fits the description, get the previous value, add the diff and set it back. */
    for (i = 0; i < SOC_PPC_L3_NOF_PROTOCOL_GROUPS(unit) ; i++) {
        if ((i & group_mask) == (group_id & group_mask)) {
            rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_counts.get(unit, i, &count);
            BCMDNX_IF_ERR_EXIT(rv);

            count += diff;

            if ((int32)count < 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("More entries were deleted than configured")));
            }

            rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_counts.set(unit, i, count);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_dpp_switch_l3_protocol_group_init(int unit){
    int rv;
    uint32 internal_flags;
    BCMDNX_INIT_FUNC_DEFS;

    /* Intisialise all valid protocols to be in group 0 in SW DB. HW is set by default. */
    internal_flags = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ALL_VALID;
    rv = sw_state_access[unit].dpp.bcm.l3.vrrp_protocol_set_types.set(unit, 0, internal_flags);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set IPV4 and IPV6 to their default groups. */
    rv = bcm_petra_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV4, DPP_SWITCH_L3_PROTOCOL_GROUP_IPV4_DEFAULT);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = bcm_petra_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_IPV6, DPP_SWITCH_L3_PROTOCOL_GROUP_IPV6_DEFAULT);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_tpid_add
 * Purpose:
 * Add a global TPID.
 *             BCM8865x and BCM8866x family of devices support 1 global
 * TPID, which is different from port TPIDs. The global TPID is
 * initialized to 0. When the global TPID is added via bcmTpidTypeOuter,
 * ingress parser uses it as an additional TPID to parse tag format of
 * packets.
 *             BCM8867x family of devices support 4 global TPIDs, which
 * are different from port TPIDs. Two global TPIDs are used or outer TPID
 * parsing. Two are used for inner TPID parsing. All 4 global TPIDs are
 * initialized to 0. Outer TPID can be configured same as inner TPID.
 * When a global outer TPID is added via bcmTpidTypeOuter, ingress parser
 * uses it as an additional outer TPID to parse tag format of
 * packets.When a global inner TPID is added via bcmTpidTypeInner,
 * ingress parser uses it as an additional inner TPID to parse tag format
 * of packets.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Future expansion i.e, REPLACE option.
 *      tpid_info - (IN) TPID info includes key: {tpid_type, tpid_value} 
 * and Associated value(s): {flags, color}.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_add(
    int unit, 
    uint32 options, 
    bcm_switch_tpid_info_t *tpid_info) {

    int rv = BCM_E_NONE;
    uint32 additional_tpid_enable;
    SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(tpid_info);

    SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);

    if (SOC_IS_JERICHO(unit)) {
        switch(tpid_info->tpid_type) {
        case bcmTpidTypeOuter:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            if ((tpid_info->tpid_value == additional_tpids.tpid_vals[0]) || (tpid_info->tpid_value == additional_tpids.tpid_vals[1])){
                BCM_EXIT;
            } else if (additional_tpids.tpid_vals[0] == 0) {
                additional_tpids.tpid_vals[0] = tpid_info->tpid_value;
            } else if (additional_tpids.tpid_vals[1] == 0) {
                additional_tpids.tpid_vals[1] = tpid_info->tpid_value;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("No more entries for additional outer TPIDs")));
            }

            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeInner:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            if ((tpid_info->tpid_value == additional_tpids.tpid_vals[2]) || (tpid_info->tpid_value == additional_tpids.tpid_vals[3])){
                BCM_EXIT;
            } else if (additional_tpids.tpid_vals[2] == 0) {
                additional_tpids.tpid_vals[2] = tpid_info->tpid_value;
            } else if (additional_tpids.tpid_vals[3] == 0) {
                additional_tpids.tpid_vals[3] = tpid_info->tpid_value;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("No more entries for additional inner TPIDs")));
            }

            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeItag:
            rv = dpp_mim_set_global_mim_tpid(unit, tpid_info->tpid_value);
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only bcmTpidTypeOuter and bcmTpidTypeInner are supported")));;
        }        
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        if (tpid_info->tpid_type != bcmTpidTypeInner) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only bcmTpidTypeInner is supported")));
        }

        additional_tpid_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0);
        if (!additional_tpid_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not enabled")));
        }
        
        additional_tpids.tpid_vals[0] = tpid_info->tpid_value;
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_tpid_delete
 * Purpose:
 * Delete a global TPID.
 *             BCM8865x and BCM8866x family of devices support deleting
 * the global TPID via bcmTpidTypeOuter.
 *             BCM8867x family of devices support deleting an outer
 * global TPID via bcmTpidTypeOuter or an inner global TPID via
 * bcmTpidTypeInner.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tpid_info - (IN) TPID info includes key: {tpid_type, tpid_value} 
 * and Associated value(s): {flags, color}.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_delete(
    int unit, 
    bcm_switch_tpid_info_t *tpid_info)
{
    int rv = BCM_E_NONE;
    uint32 additional_tpid_enable;
    uint16 tpid = 0;
    SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(tpid_info);

    if (SOC_IS_JERICHO(unit)) {
        switch(tpid_info->tpid_type) {
        case bcmTpidTypeOuter:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            if (additional_tpids.tpid_vals[0] == tpid_info->tpid_value) {
                additional_tpids.tpid_vals[0] = 0;
            } else if (additional_tpids.tpid_vals[1] == tpid_info->tpid_value) {
                additional_tpids.tpid_vals[1] = 0;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Addtional Outer TPID not found")));
            }

            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeInner:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            if (additional_tpids.tpid_vals[2] == tpid_info->tpid_value) {
                additional_tpids.tpid_vals[2] = 0;
            } else if (additional_tpids.tpid_vals[3] == tpid_info->tpid_value) {
                additional_tpids.tpid_vals[3] = 0;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Addtional Inner TPID not found")));
            }

            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeItag:
            rv = dpp_mim_get_global_mim_tpid(unit, &tpid);
            BCMDNX_IF_ERR_EXIT(rv);
            if (tpid == tpid_info->tpid_value) {
                rv = dpp_mim_set_global_mim_tpid(unit, 0);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ITAG TPID not found")));
            }
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only bcmTpidTypeOuter and bcmTpidTypeInner are supported")));;
        }        
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        if (tpid_info->tpid_type != bcmTpidTypeInner) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only bcmTpidTypeInner is supported")));
        }

        additional_tpid_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0);
        if (!additional_tpid_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not enabled")));
        }
        
        SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
        BCMDNX_IF_ERR_EXIT(rv);

        if (tpid_info->tpid_value != additional_tpids.tpid_vals[0]) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Addtional TPID not found")));
        }

        SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_tpid_delete_all
 * Purpose:
 * Delete all global TPIDs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_delete_all(
    int unit)
{
    int rv = BCM_E_NONE;
    uint32 additional_tpid_enable;
    SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        additional_tpid_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0);
        if (!additional_tpid_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not enabled")));
        }
    }
    
    SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);
    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_switch_tpid_get
 * Purpose:
 * Get a global TPID.
 *             BCM8865x and BCM8866x family of devices support getting
 * the global TPID via bcmTpidTypeOuter.
 *             BCM8867x family of devices support getting an outer global
 * TPIDs via bcmTpidTypeOuter and an inner global TPIDs via
 * bcmTpidTypeInner.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tpid_info - (IN/OUT) TPID info includes key: {tpid_type, tpid_value} 
 * and Associated value(s): {flags, color}.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_get(
    int unit, 
    bcm_switch_tpid_info_t *tpid_info)
{
    int rv = BCM_E_NONE;
    uint16 tpid = 0;
    uint32 additional_tpid_enable;
    SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(tpid_info);

    SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);

    if (SOC_IS_JERICHO(unit)) {
        switch(tpid_info->tpid_type) {
        case bcmTpidTypeOuter:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            tpid_info->tpid_value = additional_tpids.tpid_vals[0];
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeInner:
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            tpid_info->tpid_value = additional_tpids.tpid_vals[2];
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_set,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            break;

        case bcmTpidTypeItag:
            rv = dpp_mim_get_global_mim_tpid(unit, &tpid);
            BCMDNX_IF_ERR_EXIT(rv);
            tpid_info->tpid_value = tpid;
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only bcmTpidTypeOuter and bcmTpidTypeInner are supported")));;
        }        
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        additional_tpid_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0);
        if (!additional_tpid_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not enabled")));
        }
        
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
        BCMDNX_IF_ERR_EXIT(rv);

        if (additional_tpids.tpid_vals[0] != 0) {
            tpid_info->tpid_type = bcmTpidTypeInner;
            tpid_info->tpid_value = additional_tpids.tpid_vals[0];
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_tpid_get_all
 * Purpose:
 * Get global TPID count or TPIDs. if (size==0), get TPID count of all
 * global TPIDs, else, get global TPIDs based on size.
 *             BCM8865x and BCM8866x family of devices support getting
 * the global TPID.
 *             BCM8867x family of devices support getting all 4 global
 * TPIDs.
 * Parameters:
 *      unit - (IN) Unit number.
 *      size - (IN) Size of tpid_info_array to be got.
 *      tpid_info - (OUT) TPID info array.
 *      count - (IN) Item count of tpid_info_array.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_get_all(
    int unit, 
    int size, 
    bcm_switch_tpid_info_t *tpid_info_array, 
    int *count)
{
    int rv = BCM_E_NONE;
    uint32 additional_tpid_enable;
    SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids;
    uint32 indx;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(tpid_info_array);
    BCMDNX_NULL_CHECK(count);

    if (SOC_IS_JERICHO(unit)) {
        if (size == 0) {
            *count = 4;
            BCM_EXIT;
        } else if (size > 4) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("size is not correct")));
        } else {
            SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);
            
            *count = 0;
            for (indx = 0; indx < 4; indx++) {
                if (additional_tpids.tpid_vals[indx] != 0) {
                    tpid_info_array[*count].tpid_type = (indx<2) ? bcmTpidTypeOuter:bcmTpidTypeInner;
                    tpid_info_array[*count].tpid_value= additional_tpids.tpid_vals[indx];
                    (*count)++;
                }
            }
        }
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        additional_tpid_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0);
        if (!additional_tpid_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not enabled")));
        }
    
        if (size == 0) {
            *count = 1;
            BCM_EXIT;
        } else if (size > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("size is not correct")));
        } else {
            SOC_PPC_ADDITIONAL_TPID_VALUES_clear(&additional_tpids);
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_port_additional_tpids_get,(unit, &additional_tpids)));
            BCMDNX_IF_ERR_EXIT(rv);

            if (additional_tpids.tpid_vals[0] != 0) {
                tpid_info_array[0].tpid_type = bcmTpidTypeInner;
                tpid_info_array[0].tpid_value= additional_tpids.tpid_vals[0];
                *count = 1;
            }
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Addtional TPID not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_tpid_class_get
 * Purpose:
 * Getting the tpid profile from a given port or ingress vlan translation
 * action ID.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tpid_class - (IN/OUT) TPID class retrieval information.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_tpid_class_get(int unit,
                                    bcm_switch_tpid_class_t *tpid_class)
{
    int rv = BCM_E_NONE;
    int core;
    int soc_sand_rv = 0;
    _bcm_dpp_gport_info_t gport_info;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_PORT soc_ppd_port_i;
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO ing_command_info;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(tpid_class);

    /* get the tpid profile */
    if (tpid_class->match == bcmSwitchTpidClassMatchPort) {
        /* map gport to ppd-port */
        rv = _bcm_dpp_gport_to_phy_port(unit, tpid_class->port, 0, &gport_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get first port */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get,
                                            (unit, gport_info.local_port,
                                             &soc_ppd_port_i, &core)));

        /* Get tpid profile from port*/
        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port_i,
                                            &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        tpid_class->tpid_class_id = port_info.tpid_profile;

    } else if (tpid_class->match == bcmSwitchTpidClassMatchIngVlanActionId) {
        /* Verify that the entry is allocated */
        rv = bcm_dpp_am_vlan_edit_action_id_is_alloc(unit,
                                        BCM_VLAN_ACTION_SET_INGRESS,
                                        tpid_class->vlan_translation_action_id);
        if (rv == BCM_E_NOT_FOUND) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The supplied "
                                "VLAN translate action ID isn't allocated")));
        } else if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The supplied "
                                "VLAN translate action ID is invalid")));
        }

        SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(&ing_command_info);
        /* Get the command to from Ingress HW commands table */
        soc_sand_rv = soc_ppd_lif_ing_vlan_edit_command_info_get(unit,
                                        tpid_class->vlan_translation_action_id,
                                        &ing_command_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        tpid_class->tpid_class_id = ing_command_info.tpid_profile;

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                            (_BSL_BCM_MSG("Match type is Unsupported!")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_switch_network_group_config_set(
    int unit, 
    bcm_switch_network_group_t source_network_group_id, 
    bcm_switch_network_group_config_t *config)
{
    int rv = BCM_E_NONE;
    uint32 is_filter = config->config_flags & BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("bcm_switch_network_group_config_set is not supported for this device")));
    }

    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG(
           "Split horizon filter configuration is not supported when soc property split_horizon_forwarding_groups_mode is unset")));
    }

    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_network_group_config_set,(unit, source_network_group_id, config->dest_network_group_id, is_filter)));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_switch_network_group_config_get(
    int unit, 
    bcm_switch_network_group_t source_network_group_id, 
    bcm_switch_network_group_config_t *config)
{
    int rv = BCM_E_NONE;
    uint32 is_filter;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("bcm_switch_network_group_config_get is not supported for this device")));
    }

    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG(
           "Split horizon filter configuration is not supported when soc property split_horizon_forwarding_groups_mode is unset")));
    }

    rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_network_group_config_get,(unit, source_network_group_id, config->dest_network_group_id, &is_filter)));
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_filter) {
        config->config_flags |= BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    } else {
        config->config_flags &= ~BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_switch_lif_property_validate
 * Purpose:
 * 
 * Given the lif property and lif config, validates that the bank id legal, and returned the eedb bank index.
 * Parameters:
 *      unit - (IN) Unit number.
 *      lif_property - (IN) lif property property: bcmLifPropertyEEDBBankPhase | bcmLifPropertyOutGlobalLifRangeType | bcmLifPropertyEEDBBankExtension | bcmLifPropertyEEDBBankExtensionType
 *      lif_config   - (IN) lif configuration value : key and value.
 *      eedb_bank_id - (OUT) The bank's index in the eedb. 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_lif_property_validate(
    int unit, 
    bcm_switch_lif_property_t lif_property, 
    bcm_switch_lif_property_config_t *lif_config)
{
    int rif_max_id, nof_rif_half_banks;
    int eedb_bank_id = 0;
    uint8 bank_in_range = FALSE;
    char *prop_key;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    /*This API is supported only for Jericho*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_switch_lif_property_set is not supported in this device")));
    }

    /*Get the max RIF ID from SOC property*/
    prop_key = spn_RIF_ID_MAX;
    rif_max_id = soc_property_get(unit, prop_key, 0);
    nof_rif_half_banks = ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id);

    /*property key and validate bank */
    /* property key : 0-47  in case of bcmLifPropertyOutGlobalLifRangeType and
                      0-23 in case of bcmLifPropertyEEDBBankPhase and  
                      0-21 in case of bcmLifPropertyEEDBBankExtension */
    switch (lif_property) {
    case bcmLifPropertyOutGlobalLifRangeType:
        eedb_bank_id = lif_config->key - nof_rif_half_banks;
        bank_in_range = BCM_DPP_AM_LOCAL_OUT_LIF_HALF_BANK_IN_RANGE(unit, eedb_bank_id);
        break;
    case bcmLifPropertyEEDBBankPhase:
        eedb_bank_id = lif_config->key * 2 - nof_rif_half_banks;
        bank_in_range = BCM_DPP_AM_LOCAL_OUT_LIF_HALF_BANK_IN_RANGE(unit, eedb_bank_id);
        break;
    case bcmLifPropertyEEDBBankExtension:
    case bcmLifPropertyEEDBBankExtensionType:
        eedb_bank_id = lif_config->key * 2 - nof_rif_half_banks;
        bank_in_range = BCM_DPP_AM_LOCAL_OUT_LIF_REGULAR_HALF_BANK_IN_RANGE(unit, eedb_bank_id);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported switch lif property")));
    }

    if (SOC_IS_JERICHO_PLUS(unit) && eedb_bank_id < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("In JR+ and above the outrifs are not in the eedb. SOC rif_max_id should not set.")));
    }

    if(eedb_bank_id < 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("lif property can not be set/get for bank allocated for RIF")));
    }

    if (!bank_in_range)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Bank id is too high.")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_switch_lif_property_set
 * Purpose:
 * 
 * Set the LIF property along with the desired property and the
 * configuration values.
 * Parameters:
 *      unit - (IN) Unit number.
 *      lif_property - (IN) lif property property: bcmLifPropertyEEDBBankPhase | bcmLifPropertyOutGlobalLifRangeType | bcmLifPropertyEEDBBankExtension | bcmLifPropertyEEDBBankExtensionType
 *      lif_config   - (IN) lif configuration value : key and value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_lif_property_set(
    int unit, 
    bcm_switch_lif_property_t lif_property, 
    bcm_switch_lif_property_config_t *lif_config)
{
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    int eedb_bank_id = lif_config->key;
    uint8 ext_bank_found = FALSE;
    int bank_id_iter, start_bank_id, end_bank_id;
    int rif_max_id;
    int ext_bank_id;
    int tmp_ext_bank_id = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;
    uint8 ext_bank_is_set = FALSE;
    int rv = BCM_E_NONE;
    uint8 is_direct_bank;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    /* Validate input */
    rv = bcm_petra_switch_lif_property_validate(unit, lif_property, lif_config);
    BCMDNX_IF_ERR_EXIT(rv); 

    /* In case Phase or extension, both half banks need to be set (per bank property)
      bcmLifPropertyOutGlobalLifRangeType --- key is logical bank id (half bank)
      phase or extension --- key is physical bank id, but in software, should set both half bank*/

    /* property key : 0-47  in case of bcmLifPropertyOutGlobalLifRangeType and
                      0-23 in case of bcmLifPropertyEEDBBankPhase and  
                      0-21 in case of bcmLifPropertyEEDBBankExtension */
    start_bank_id = (lif_property == bcmLifPropertyOutGlobalLifRangeType) ? eedb_bank_id : (eedb_bank_id*2);
    end_bank_id = (lif_property == bcmLifPropertyOutGlobalLifRangeType) ? start_bank_id : start_bank_id + 1;

    /* 
     * In QAX, rifs are outside the eedb, so we don't care if a bank is in the rif range.
     *  It would just return an error in the verify function.
     */
    rif_max_id = (SOC_IS_JERICHO_PLUS(unit)) ? 0 : SOC_DPP_CONFIG(unit)->l3.nof_rifs;

    for (bank_id_iter = start_bank_id ; bank_id_iter <= end_bank_id ; bank_id_iter++ ) {

        /* Get the eedb bank info from the SW DB*/
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.get(unit, bank_id_iter, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 

        /*if eedb bank is not empty*/
        if (eedb_bank_info.valid) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG(" lif property can not be set for un empty bank")));
        }

        /*Configure the LIF property*/
        switch (lif_property) 
        {
        case bcmLifPropertyOutGlobalLifRangeType :
            is_direct_bank = (lif_config->value == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT);
            if (is_direct_bank) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG(" configuring direct range is currently unsupported")));
            }

            if (  bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id)  &&   lif_config->value !=  BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT  ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG(" lif property can not be set for bank allocated for RIF")));
            }

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id_iter,( lif_config->value == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ) ? TRUE : FALSE));
            BCMDNX_IF_ERR_EXIT(rv);
            
            /* Prevent global lif allocation in this range */
            rv = _bcm_dpp_am_global_lif_set_direct_bank(unit, bank_id_iter, is_direct_bank);
            BCMDNX_IF_ERR_EXIT(rv);

            eedb_bank_info.type = lif_config->value;
            eedb_bank_info.force_flags |=  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE; 
            break;
        case bcmLifPropertyEEDBBankPhase:
            eedb_bank_info.phase = lif_config->value;

            if (eedb_bank_info.phase == BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC) {
                eedb_bank_info.force_flags &=  ~BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE; 
            } else {
                eedb_bank_info.force_flags |=  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE; 
            }

            /* Set HW */
            rv= _bcm_dpp_am_local_outlif_bank_phase_set(unit,eedb_bank_id,eedb_bank_info.phase);
            BCMDNX_IF_ERR_EXIT(rv); 

            break;
        case bcmLifPropertyEEDBBankExtension:
            if (eedb_bank_info.is_extended == lif_config->value ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("EEDB bank extension already set to %d"), lif_config->value));
            }
            eedb_bank_info.is_extended = lif_config->value;
            eedb_bank_info.force_flags |=  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXTENSION ; 
  
            /* If bank is extended, allocate the extension bank only once*/
            if ( eedb_bank_info.is_extended ) {

                if (!ext_bank_is_set) 
                {    
                    rv = _bcm_dpp_am_local_outlif_extension_bank_alloc(unit,eedb_bank_id, &ext_bank_id, &ext_bank_found );
                    BCMDNX_IF_ERR_EXIT(rv); 

                    if (ext_bank_found) 
                    {
                        ext_bank_is_set = TRUE;
                        tmp_ext_bank_id = ext_bank_id;

                        /*set extension in HW*/
                        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,eedb_bank_id, TRUE ,(uint32)ext_bank_id ));
                        BCMDNX_IF_ERR_EXIT(rv); 
                    }
                }
                eedb_bank_info.extension_bank = tmp_ext_bank_id;
            } 
            else 
            {
                if (!ext_bank_is_set) 
                { 
                    rv = _bcm_dpp_am_local_outlif_extension_bank_dealloc(unit,eedb_bank_id );
                    BCMDNX_IF_ERR_EXIT(rv); 

                    /*set extension in HW*/
                    rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,eedb_bank_id, FALSE ,0));
                    BCMDNX_IF_ERR_EXIT(rv); 
                    ext_bank_is_set = TRUE;
                }
                eedb_bank_info.extension_bank = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;
            }

            break;
        case bcmLifPropertyEEDBBankExtensionType:
            if ( !eedb_bank_info.is_extended ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("EEDB bank extension type can only be set for extnded bank")));
            }
            if (eedb_bank_info.ext_type == lif_config->value ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("EEDB bank extension type already set to %d"), lif_config->value));
            }
            eedb_bank_info.ext_type = ( lif_config->value == BCM_SWITCH_LIF_PROPERTY_EEDB_BANK_EXT_TYPE_DATA ) ? TRUE : FALSE;
            eedb_bank_info.force_flags |=  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXT_TYPE ; 
  
            /* If bank is extended, set extension type only once*/
            if (!ext_bank_is_set) 
            {    
                ext_bank_is_set = TRUE;

                /*set extension TYPE in HW*/
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_type_set,(unit,eedb_bank_id, eedb_bank_info.ext_type));
                BCMDNX_IF_ERR_EXIT(rv); 
            }

            break;
        }




        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.set(unit, bank_id_iter, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv); 
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_switch_lif_property_get
 * Purpose:
 * 
 * Get the LIF property along with the desired property and the
 * configuration values.
 * Parameters:
 *      unit - (IN) Unit number.
 *      lif_property - (IN) lif property property: bcmLifPropertyEEDBBankPhase | bcmLifPropertyOutGlobalLifRangeType | bcmLifPropertyEEDBBankExtension.
 *      lif_config   - (In/OUT) lif configuration value. the kery in the structure is an input, the value is output.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_lif_property_get(
    int unit, 
    bcm_switch_lif_property_t lif_property, 
    bcm_switch_lif_property_config_t *lif_config)
{
    int bank_id;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    /* Validate input and get the eedb bank id. */
    rv = bcm_petra_switch_lif_property_validate(unit, lif_property, lif_config);
    BCMDNX_IF_ERR_EXIT(rv); 

    /* Get the eedb bank info from the SW DB*/
    /*bcmLifPropertyOutGlobalLifRangeType --- key is logical bank id (half bank)
      phase or extension --- key is physical bank id, but in software, should get from half bank*/

    /* property key : 0-47  in case of bcmLifPropertyOutGlobalLifRangeType and
                      0-23 in case of bcmLifPropertyEEDBBankPhase and  
                      0-21 in case of bcmLifPropertyEEDBBankExtension */
    bank_id = (lif_property == bcmLifPropertyOutGlobalLifRangeType) ? lif_config->key : (lif_config->key*2);
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
    BCMDNX_IF_ERR_EXIT(rv); 

    switch (lif_property) 
    {
    case bcmLifPropertyOutGlobalLifRangeType :
         lif_config->value =   eedb_bank_info.type ;
        break;
    case bcmLifPropertyEEDBBankPhase:
        lif_config->value =   eedb_bank_info.phase;
        break;
    case bcmLifPropertyEEDBBankExtension:
        lif_config->value =   eedb_bank_info.is_extended ;
        break;
    case bcmLifPropertyEEDBBankExtensionType:
        lif_config->value =   eedb_bank_info.ext_type ;
        break;
    /* No need. It's a deadcode
    default :
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported switch lif property")));*/
    }

exit:
    BCMDNX_FUNC_RETURN;

}


int _bcm_dpp_switch_control_indexed_set(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t info)
{

    int soc_sand_rv;
    uint32 rv;
    
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    switch(key.type) 
    {
    case bcmSwitchForwardingLayerMtuFilter:
        soc_sand_rv = arad_pp_vsi_egress_mtu_check_enable_set(unit, TRUE /*Forwarding Layer*/, key.index, info.value);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchLinkLayerMtuFilter:
        soc_sand_rv = arad_pp_vsi_egress_mtu_check_enable_set(unit, FALSE /*Link Layer*/, key.index, info.value);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchForwardingLayerMtuSize:
        soc_sand_rv = soc_ppd_vsi_egress_mtu_set(unit, TRUE /*Forwarding Layer*/, key.index, info.value);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchLinkLayerMtuSize:
        soc_sand_rv = soc_ppd_vsi_egress_mtu_set(unit, FALSE /*Link Layer*/, key.index, info.value);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchHeaderCompensationPerPacket:

        /* Input validation */
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported on this device.")));
        }
        if (key.index > BCM_SWITCH_HDR_COMP_PER_PACKET_MAX_INDEX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Index is too high.")));
        }
        if (info.value > BCM_SWITCH_HDR_COMP_PER_PACKET_MAX_ABS_VALUE
            || info.value < -BCM_SWITCH_HDR_COMP_PER_PACKET_MAX_ABS_VALUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Value is too high or too low.")));
        }
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_header_compensation_per_cud_set, (unit, key.index, info.value));
        BCMDNX_IF_ERR_EXIT(rv);
        break;  
    case bcmSwitchMplsSpeculativeNibbleMap:
        rv = BCM_E_NONE;
        if (!SOC_IS_JERICHO(unit) || (SOC_DPP_CONFIG(unit)->pp.parser_mode == 0))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature is supported for Jericho and above devices and with soc property spn_PARSER_MODE disabled.")));
        }
        /* Check for validity of input*/
        if (key.index > 15 || key.index < 0)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Nibble value must be between 0 and 15")));
        }
        rv = soc_ppd_mpls_speculative_map_set(unit, key.index, (bcm_switch_mpls_next_protocol_t)info.value);
        if (rv != BCM_E_NONE)
        {
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        break;
    case bcmSwitchL3LpmHitbitEnable:
        if (SOC_IS_JERICHO_PLUS(unit)) {
            uint8 enable = 0;
            /* Support activating hitbits in up to two tables with a single API call */
            SOC_DPP_DBAL_SW_TABLE_IDS table_id_1 = SOC_DPP_DBAL_SW_NOF_TABLES,
                                      table_id_2 = SOC_DPP_DBAL_SW_NOF_TABLES;

            if (info.value == 0) {
                enable = 0;
            } else if (info.value == 1) {
                enable = 1;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The info value of bcmSwitchL3LpmHitbitEnable must be 1(enable)/0(disable).")));
            }

            switch (key.index) {
                case bcmL3LpmHitbitTableIPv4UC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS; break;
                case bcmL3LpmHitbitTableIPv6UC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE; break;
                case bcmL3LpmHitbitTableIPv4MC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS; break;
                case bcmL3LpmHitbitTableIPv6MC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC; break;
                case bcmL3LpmHitbitTableIPv4UCScale: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS;
                                                     table_id_2 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS; break;
                case bcmL3LpmHitbitTableIPv6UCScale: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS;
                                                     table_id_2 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS; break;
                default: BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported key index for bcmSwitchL3LpmHitbitEnable/bcmSwitchL3LpmHitbitDisable")));
            }

            soc_sand_rv = arad_pp_dbal_table_kaps_hitbit_set(unit, table_id_1, enable);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (table_id_2 != SOC_DPP_DBAL_SW_NOF_TABLES) {
                soc_sand_rv = arad_pp_dbal_table_kaps_hitbit_set(unit, table_id_2, enable);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            rv = BCM_E_NONE;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchL3LpmHitbitEnable/Disable is Supported only in Jericho+ and above")));
        }

        break;
    case bcmSwitchMeterTrafficClass:
        rv = BCM_E_NONE;
        if ( key.index > ARAD_PP_TC_MAX_VAL || key.index < 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid TC, valid range 0-7")));
        }
        if(info.value > DPP_SWITCH_METER_TC_MAX_VAL || info.value < 0) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Meter TC, valid range 0-3")));
        }

        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_meter_tc_map_set, (unit, key.index , info.value));
        BCMDNX_IF_ERR_EXIT(rv); 
        break;
    case bcmSwitchConcatAppHeader:
        if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

            uint32 prog_id = 0;
            uint8 flp_app_type = 0;
            uint32 concat_header_select = 0;
            ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;

            if (!ARAD_KBP_IS_OP_OR_OP2) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is only supported with KBP OP.")));
            }
            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported only with soc property custom_feature_ext_flexible_mode.")));
            }

            if ((key.index >= bcmFieldAppTypeCount) || (key.index < bcmFieldAppTypeAny)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid application type, valid range bcmFieldAppTypeAny to (bcmFieldAppTypeCount-1)")));
            }
            if((info.value < bcmSwitchConcatHeaderNone)
                || (info.value >= bcmSwitchConcatHeader__Count)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid header, valid value bcmSwitchConcatHeaderNone/bcmSwitchConcatHeaderForwarding/bcmSwitchConcatHeaderNextForwarding")));
            }

            /* Translate the Field application type to the FLP program */
            SOCDNX_SAND_IF_ERR_EXIT(_bcm_dpp_field_app_type_bcm_to_ppd(unit, key.index, &prog_id));
            /* Translate the FLP program to the FLP application type */
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_prog_index_to_flp_app_get(unit, prog_id, &flp_app_type));

            if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_da_sa_sharing_enable", 0))
                && ARAD_KBP_PROG_IS_L2_SHARING(flp_app_type)) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is not supported with soc property custom_feature_kbp_da_sa_sharing_enable and related programs")));
            }
            if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header", 0))
                && ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(flp_app_type)) {
               BCMDNX_ERR_EXIT_MSG
                   (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is not supported with soc property custom_feature_kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header and related programs.")));
            }
            /* The info.value is independent in each case branch. */
            /* coverity[mixed_enums:FALSE] */
            switch(info.value) 
            {
            case bcmSwitchConcatHeaderNone:
                concat_header_select = 0;
            break;
            case bcmSwitchConcatHeaderForwarding:
                concat_header_select = SOC_PPC_FP_BASE_HEADER_TYPE_FWD;
            break;
            case bcmSwitchConcatHeaderNextForwarding:
                concat_header_select = SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST;
            break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid header, valid value bcmSwitchConcatHeaderNone/bcmSwitchConcatHeaderForwarding/bcmSwitchConcatHeaderNextForwarding")));
            }

            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));
            flp_lookups_tbl.elk_packet_data_select = concat_header_select; /* select forwarding header */
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, prog_id, &flp_lookups_tbl));
#endif
        } else {
            BCMDNX_ERR_EXIT_MSG
                (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported for Jericho and above devices.")));
        }
        break;
    default:
        break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_switch_control_indexed_get(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t *info)
{

    int soc_sand_rv, rv;
    uint8 enable;
    
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    BCMDNX_NULL_CHECK(info);

    switch(key.type) 
    {
    case bcmSwitchForwardingLayerMtuFilter:
        soc_sand_rv = arad_pp_vsi_egress_mtu_check_enable_get(unit, TRUE /*Forwarding Layer*/, key.index, &enable);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        info->value = enable;
        break;
    case bcmSwitchLinkLayerMtuFilter:
        soc_sand_rv = arad_pp_vsi_egress_mtu_check_enable_get(unit, FALSE /*Link Layer*/, key.index, &enable);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        info->value = enable;
        break;
    case bcmSwitchForwardingLayerMtuSize:
        soc_sand_rv = soc_ppd_vsi_egress_mtu_get(unit, TRUE /*Forwarding Layer*/, key.index, (uint32*)&(info->value));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchLinkLayerMtuSize:
        soc_sand_rv = soc_ppd_vsi_egress_mtu_get(unit, FALSE /*Link Layer*/, key.index, (uint32*)&(info->value));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
    case bcmSwitchHeaderCompensationPerPacket:
        /* Input validation */
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported on this device.")));
        }
        if (key.index > BCM_SWITCH_HDR_COMP_PER_PACKET_MAX_INDEX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Index is too high.")));
        }
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_header_compensation_per_cud_get, (unit, key.index, &info->value));
        BCMDNX_IF_ERR_EXIT(rv);
        break;  
    case bcmSwitchMplsSpeculativeNibbleMap:
        rv = BCM_E_NONE;
        if (!SOC_IS_JERICHO(unit) || (SOC_DPP_CONFIG(unit)->pp.parser_mode == 0))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature is supported for Jericho and above devices and with soc property spn_PARSER_MODE disabled.")));
        }
        /* Check for validity of input*/
        if (key.index > 15 || key.index < 0)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Nibble value must be between 0 and 15")));
        }
        rv = soc_ppd_mpls_speculative_map_get(unit, key.index, (bcm_switch_mpls_next_protocol_t*)&info->value);
        if (rv != BCM_E_NONE)
        {
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        break;
    case bcmSwitchL3LpmHitbitEnable:
        if (SOC_IS_JERICHO_PLUS(unit)) {
            /* In case of two relevant KAPS tables, simply return the first */
            SOC_DPP_DBAL_SW_TABLE_IDS table_id_1 = SOC_DPP_DBAL_SW_NOF_TABLES;
            uint8 enable;

            switch (key.index) {
                case bcmL3LpmHitbitTableIPv4UC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS; break;
                case bcmL3LpmHitbitTableIPv6UC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE; break;
                case bcmL3LpmHitbitTableIPv4MC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS; break;
                case bcmL3LpmHitbitTableIPv6MC: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC; break;
                case bcmL3LpmHitbitTableIPv4UCScale: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS; break;
                case bcmL3LpmHitbitTableIPv6UCScale: table_id_1 = SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS; break;
                default: BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported key index for bcmSwitchL3LpmHitbitEnable/bcmSwitchL3LpmHitbitDisable")));
            }

            soc_sand_rv = arad_pp_dbal_table_kaps_hitbit_get(unit, table_id_1, &enable);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            info->value = enable;

            rv = BCM_E_NONE;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("The switch bcmSwitchL3LpmHitbitEnable/Disable is Supported only in Jericho+ and above")));
        }

        break;
    case bcmSwitchMeterTrafficClass:
        rv = BCM_E_NONE;
        if ( key.index > ARAD_PP_TC_MAX_VAL || key.index < 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid TC, valid range 0-7\n")));
        }
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_meter_tc_map_get, (unit, key.index , (uint32*)&(info->value)));
        BCMDNX_IF_ERR_EXIT(rv); 
        break;
    case bcmSwitchConcatAppHeader:
        if (SOC_IS_JERICHO(unit)) {
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)

            uint32 prog_id = 0;
            uint8 flp_app_type = 0;
            ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;

            if (!ARAD_KBP_IS_OP_OR_OP2) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is only supported with KBP OP.")));
            }
            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported only with soc property custom_feature_ext_flexible_mode.")));
            }
            if ((key.index >= bcmFieldAppTypeCount) || (key.index < bcmFieldAppTypeAny)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid application type, valid range bcmFieldAppTypeAny to (bcmFieldAppTypeCount-1)")));
            }

            /* Translate the Field application type to the FLP program */
            SOCDNX_SAND_IF_ERR_EXIT(_bcm_dpp_field_app_type_bcm_to_ppd(unit, key.index, &prog_id));
            /* Translate the FLP program to the FLP application type */
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_prog_index_to_flp_app_get(unit, prog_id, &flp_app_type));

            if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_da_sa_sharing_enable", 0))
                && ARAD_KBP_PROG_IS_L2_SHARING(flp_app_type)) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is not supported with soc property custom_feature_kbp_da_sa_sharing_enable and related programs")));
            }
            if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header", 0))
                && ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(flp_app_type)) {
                BCMDNX_ERR_EXIT_MSG
                    (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is not supported with soc property custom_feature_kbp_ipv6_uc_no_rpf_dip_sip_sharing_from_fwd_header and related programs.")));
            }

            /* Translate the Application id to the FLP program */
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));
            /* return forwarding header */
            switch(flp_lookups_tbl.elk_packet_data_select)
            {
            case 0:
                info->value = bcmSwitchConcatHeaderNone;
            break;
            case SOC_PPC_FP_BASE_HEADER_TYPE_FWD:
                info->value = bcmSwitchConcatHeaderForwarding;
            break;
            case SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST:
                info->value = bcmSwitchConcatHeaderNextForwarding;
            break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid header, valid value bcmSwitchConcatHeaderNone/bcmSwitchConcatHeaderForwarding/bcmSwitchConcatHeaderNextForwarding")));
            }
#endif
        } else {
            BCMDNX_ERR_EXIT_MSG
                (BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported for Jericho and above devices.")));
        }
        break;
    default:
        break;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_switch_control_indexed_set(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t info)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_indexed_set(unit, key, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_control_indexed_get(
    int unit, 
    bcm_switch_control_key_t key, 
    bcm_switch_control_info_t *info)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;
    DPP_SWITCH_UNIT_INIT_CHECK;

    DPP_SWITCH_LOCK_TAKE;

    rv = _bcm_dpp_switch_control_indexed_get(unit, key, info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    DPP_SWITCH_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_dram_vendor_info_get(
    int unit,
    bcm_switch_dram_vendor_info_t *info)
{
    int rv = BCM_E_NONE;
    int drc_ndx;
    combo28_vendor_info_t soc_info;
    BCMDNX_INIT_FUNC_DEFS;
       
    
    if(SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_num > 0){     
        if (SOC_IS_JERICHO(unit)){
            drc_ndx = 0;
        } else if(SOC_IS_ARDON(unit)){
            drc_ndx = 3;
        } else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("ERROR not suported for ARAD")));
        }
            rv = soc_dpp_drc_combo28_shmoo_vendor_info_get(unit, drc_ndx, &soc_info);
            info->dram_density = soc_info.dram_density;
            info->fifo_depth = soc_info.fifo_depth;
            info->revision_id = soc_info.revision_id;
            info->manufacture_id = soc_info.manufacture_id;
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("No DRAM")));
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_thermo_sensor_read(
      int unit,
      bcm_switch_thermo_sensor_type_t sensor_type,
      int interface_id,
      bcm_switch_thermo_sensor_t* sensor_data)
{
    soc_dpp_drc_gddr5_temp_t gddr5_temp;

    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(sensor_data);

    switch (sensor_type) {
    case bcmSwitchThermoSensorDram:
        if (SOC_IS_DPP_DRC_COMBO28(unit)) {
            SOCDNX_IF_ERR_EXIT( soc_dpp_drc_combo28_gddr5_temp_get (unit, interface_id, &gddr5_temp));
            sensor_data->thermo_sensor_min = gddr5_temp.min_temp;
            sensor_data->thermo_sensor_max = gddr5_temp.max_temp;
            sensor_data->thermo_sensor_current = gddr5_temp.current_temp;
            break;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("sensor type %d is supported only in combo28 drams."), sensor_type));
        }
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("sensor type %d not supported"), sensor_type));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_parser(int unit, bcm_switch_control_t bcm_type, ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE *arg)
{
   int rv = BCM_E_NONE;
   BCMDNX_INIT_FUNC_DEFS;

   switch (bcm_type) {
   case bcmSwitchUdpTunnelIPv4DstPort:
      *arg = ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV4;
      break;
   case bcmSwitchUdpTunnelIPv6DstPort:
      *arg = ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_IPV6;
      break;
   case bcmSwitchUdpTunnelMplsDstPort:
      *arg = ARAD_PARSER_UDP_TUNNEL_NEXT_PRTCL_TYPE_MPLS;
      break;
   default:
      rv = BCM_E_PARAM;
   }

   BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_switch_control_udp_tunnel_dest_port_type_bcm_to_soc_prge(int unit, bcm_switch_control_t bcm_type, ARAD_EGR_PROG_EDITOR_PROGRAMS *arg)
{
   int rv = BCM_E_NONE;
   BCMDNX_INIT_FUNC_DEFS;

   switch (bcm_type) {
   case bcmSwitchUdpTunnelIPv4DstPort:
      *arg = ARAD_EGR_PROG_EDITOR_PROG_IPV4_OVER_UDP;
      break;
   case bcmSwitchUdpTunnelIPv6DstPort:
      *arg = ARAD_EGR_PROG_EDITOR_PROG_IPV6_OVER_UDP;
      break;
   case bcmSwitchUdpTunnelMplsDstPort:
      *arg = ARAD_EGR_PROG_EDITOR_PROG_MPLS_OVER_UDP;
      break;
   default:
      rv = BCM_E_PARAM;
   }
   BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_switch_rov_get(
    int unit,
    uint32 flags,
    uint32 *rov){

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(rov);

    BCMDNX_IF_ERR_EXIT(soc_dpp_avs_value_get(unit, rov));

exit:
    BCMDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP)
extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];

/* get kbp pointer from SDK */
int bcm_petra_switch_kbp_info_get(int unit, bcm_core_t core, bcm_switch_kbp_info_t *kbp_info)
{
    int res = 0;

    /* check custom_feature_ext_flexible_mode soc is enabled */
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
    {
         printf("Error, custom_feature_ext_flexible_mode must be set\n");
         res = 1; /* indicate error */
         return res;
    }

    /* get kbp device pointer */
    kbp_info->device_p = (int*)(AradAppData[unit]->device_p[core]);

    return res;
}

/* get apptype info per apptype */
int bcm_petra_switch_kbp_apptype_info_get(int unit, bcm_core_t core, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t *apptype_info)
{
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    ARAD_PMF_CE pgm_ce;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;
    ARAD_PMF_DB_INFO pmf_db_info;
    bcm_switch_kbp_master_key_segment_info_t acl_segments[BCM_MAX_NOF_SEGMENTS];
    uint32 ltr_index = 0;
    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);
    uint32 pgm_idx;
    uint32 cycle_idx;
    uint32 lsb_msb;
    uint32 ce_id;
    uint32 ce_idx;
    bcm_field_AppType_t apptype_check = 0;
    bcm_field_AppType_t curr_apptype= 0;
    int apptype_validitiy_error_check = 0;
    int srch_ndx = 0;
    uint8_t acl_segments_arr [BCM_MAX_NOF_SEGMENTS][BCM_SEGMENT_NAME_SIZE];
    uint32 segment_index;

    BCMDNX_INIT_FUNC_DEFS;

    apptype_info->is_valid = 0;
    apptype_info->fwd_db = NULL;
    apptype_info->dummy_db = NULL;
    apptype_info->rpf_db = NULL;
    apptype_info->master_key_nof_segments = 0;
    apptype_info->total_segments_size = 0;

    /* run over all KBP ltrs */
    for(ltr_index = 0; ltr_index < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; ltr_index++)
    {
        /* get ltr config info */
        BCMDNX_IF_ERR_EXIT(KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_index, &ltr_config_info));

        /* if ltr is valid, update kbp_info structure */
        if (ltr_config_info.valid)
        {
            uint32 prog_id;
            /*
             * PROG_FLP_IPV6UC_RPF is a dynamic opcode, therefore the opcode and context are not equal,
             * so we need to use the context and not the opcode id
             */
            if (ltr_config_info.opcode == PROG_FLP_IPV6UC_RPF)
            {
                BCMDNX_IF_ERR_EXIT(arad_pp_sw_db_flp_prog_app_to_index_get(unit, PROG_FLP_IPV6UC_RPF, &prog_id));
            }
            else
            {
                prog_id = ltr_config_info.opcode;
            }

            /* get apptype */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_app_type_ppd_to_bcm(unit, prog_id, &curr_apptype));

            /* curr apptype not equal to the apptype requested, look for the next one */
            if (apptype != curr_apptype)
            {
                continue;
            }

            /* the correct apptype is found, break */
            break;
        }
    }

    /* No valid LTR related to the apptype was found */
    if (ltr_index >= ARAD_KBP_MAX_NUM_OF_FRWRD_DBS)
    {
        BCM_EXIT;
    }

    /* apptype found, update ltr general info */
    apptype_info->is_valid = ltr_config_info.valid;
    apptype_info->ltr_index = ltr_config_info.ltr_id;
    apptype_info->opcode = ltr_config_info.opcode;
    apptype_info->inst_p = (int*)ltr_config_info.inst_p;

    /* update master key segments attributes */
    segment_index = 0;
    for(srch_ndx = 0; srch_ndx < ltr_config_info.master_key_fields.nof_key_segments; srch_ndx++)
    {
        int seg_size;
        /*
         * All segments with prefix ACL where created as a result of creating an ACL.
         * They  are not sent to the customer level as they are internal.
         * The customer should add them explicitly outside this function.
         */
        if (sal_strstr(((char*)ltr_config_info.master_key_fields.key_segment[srch_ndx].name), "ACL") != NULL)
        {
            continue;
        }

        seg_size = sizeof(ltr_config_info.master_key_fields.key_segment[srch_ndx].name);
        if(seg_size > BCM_SEGMENT_NAME_SIZE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("segment %s size %d too big, cannot be copied "),
                                             ltr_config_info.master_key_fields.key_segment[srch_ndx].name, seg_size));
        }

        /* update apptype_info struct with the forwarding segments */

        /* update apptype_info struct with the segment name */
        sal_strncpy((char*)apptype_info->master_key_segment[segment_index].segment_name, ltr_config_info.master_key_fields.key_segment[srch_ndx].name, seg_size);

        /* update segment size in bytes */
        apptype_info->master_key_segment[segment_index].segment_size_bits = (ltr_config_info.master_key_fields.key_segment[srch_ndx].nof_bytes) * 8;
        apptype_info->total_segments_size += apptype_info->master_key_segment[segment_index].segment_size_bits;

        /* update segment type */
        apptype_info->master_key_segment[segment_index].segment_type = ltr_config_info.master_key_fields.key_segment[srch_ndx].type;

        segment_index++;
    }

    /* update master key number of segments */
    apptype_info->master_key_nof_segments = segment_index;

    /* update fwd db, rpf db and dummy db handles for the apptype */
    for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++)
    {
        if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx))
        {
            /* fw db */
            if (srch_ndx == 0)
            {
                /* update fwd db */
                apptype_info->fwd_db = (int*)AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p;

                /* update ad fwd db */
                apptype_info->fwd_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
            }
            /* dummy db */
            else if (srch_ndx == 1)
            {
                /* update dummy db */
                apptype_info->dummy_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p);

                /* update ad dummy db */
                apptype_info->dummy_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
            }
            /* rpf db */
            else if (srch_ndx == 2)
            {
                /* update rpf db */
                apptype_info->rpf_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p);

                /* update ad rpf db */
                apptype_info->rpf_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
            }
        }
    }

    /* get acl qualifiers info - qualifier type, qualifier size and qualifier name */
    /* run over all flp programs */
    for(pgm_idx = 0; pgm_idx < ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; pgm_idx++)
    {
        /* initialize srch_ndx */
        srch_ndx = -1;

        /* run over number of cycles */
        for(cycle_idx = 0; cycle_idx < ARAD_PMF_LOW_LEVEL_NOF_CYCLES; cycle_idx++)
        {
            ce_id = 0;

            /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
            if (apptype_validitiy_error_check)
            {
                break;
            }

            for(lsb_msb = 0; lsb_msb < ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB; lsb_msb++)
            {
                /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
                if (apptype_validitiy_error_check)
                {
                    break;
                }

                for(ce_idx = 0; ce_idx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB; ce_idx++)
                {
                    ce_id = (SOC_PPC_FP_KEY_BIT_TYPE_LSB == lsb_msb) ?
                    (ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB - ce_idx - 1) :
                    (ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB * 2 - ce_idx - 1);

                    sal_memset(&pgm_ce, 0x0, sizeof(ARAD_PMF_CE));

                    /* get acl key qualifiers info */
                    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(unit, stage, pgm_idx, cycle_idx, ce_id, &pgm_ce));

                    if (pgm_ce.is_used)
                    {
                        sal_memset(&pmf_db_info, 0x0, sizeof(ARAD_PMF_DB_INFO));
                        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, stage, pgm_ce.db_id, &pmf_db_info));

                        if(SHR_BITGET(pmf_db_info.progs, pgm_idx) > 0)
                        {
                            /* update srch_ndx */
                            ++srch_ndx;

                            /* get apptype */
                            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_app_type_ppd_to_bcm(unit, pgm_idx, &apptype_check));

                            /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
                            if (apptype != apptype_check)
                            {
                                apptype_validitiy_error_check = 1;
                                break;
                            }

                            /* get qualifier name */
                            sal_strncpy(((char*)acl_segments_arr[srch_ndx]), SOC_PPC_FP_QUAL_TYPE_to_string(pgm_ce.qual_type), BCM_SEGMENT_NAME_SIZE);

                            /* get qualifier number of bytes */
                            acl_segments[srch_ndx].segment_size_bits = pgm_ce.msb + pgm_ce.msb_padding + 1;

                            /* set qualifier type */
                            acl_segments[srch_ndx].segment_type = KBP_KEY_FIELD_TERNARY;

                            /* update number of segments */
                            ++(apptype_info->master_key_nof_segments);
                        }
                    }
                }
            }
        }

        /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
        if (apptype_validitiy_error_check)
        {
            /* reset apptype_validitiy_error_check for next pgm_idx */
            apptype_validitiy_error_check = 0;
            continue;
        }

        /* update kbp info with acl segments */
        for(; srch_ndx >= 0; --srch_ndx)
        {
            /* update apptype_info struct with the segment name */
            sal_strncpy((char*)apptype_info->master_key_segment[apptype_info->master_key_nof_segments-srch_ndx-1].segment_name,
                        (char*)acl_segments_arr[srch_ndx], BCM_SEGMENT_NAME_SIZE);

            /* get qualifier number of bytes */
            apptype_info->master_key_segment[apptype_info->master_key_nof_segments-srch_ndx-1].segment_size_bits = acl_segments[srch_ndx].segment_size_bits;
            apptype_info->total_segments_size += apptype_info->master_key_segment[apptype_info->master_key_nof_segments-srch_ndx-1].segment_size_bits;

            /* set qualifier type */
            apptype_info->master_key_segment[apptype_info->master_key_nof_segments-srch_ndx-1].segment_type = acl_segments[srch_ndx].segment_type;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}
#else

/* get apptype info per apptype - added for non-kbp compilation case */
int bcm_petra_switch_kbp_apptype_info_get(int unit, bcm_core_t core, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t *apptype_info)
{
    return 0;
}

/* get kbp pointer from SDK - added for non-kbp compilation case */
int bcm_petra_switch_kbp_info_get(int unit, bcm_core_t core, bcm_switch_kbp_info_t *kbp_info)
{
    return 0;
}
#endif /*defined(INCLUDE_KBP)*/

int
bcm_petra_switch_encap_dest_map_key_verify(
    int unit,
    bcm_switch_encap_dest_t *encap_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    if (!(SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "two_pass_mc_dest_flow", 0))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported only on Jericho and above with custom_feature_two_pass_mc_dest_flow ")));
    }

    BCMDNX_NULL_CHECK(encap_info);

    if(encap_info->encap_id >= SOC_DPP_DEFS_GET(unit, nof_local_lifs))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap id should lower than Max %d."), encap_info->encap_id, SOC_DPP_DEFS_GET(unit, nof_local_lifs)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_switch_encap_dest_map_add_verify(
    int unit,
    bcm_switch_encap_dest_t *encap_info)
{
    uint32                              supported_flags = 0;
    uint32                              flow_id = 0;
    bcm_switch_encap_dest_t             encap_info_old;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    if (!(SOC_IS_JERICHO(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "two_pass_mc_dest_flow", 0))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This feature is supported only on Jericho and above with custom_feature_two_pass_mc_dest_flow ")));
    }

    BCMDNX_NULL_CHECK(encap_info);

    BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_key_verify(unit, encap_info));

    supported_flags = BCM_SWITCH_ENCAP_DEST_REPLACE;
    if(_SHR_IS_FLAG_SET(encap_info->flags, ~supported_flags))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags support BCM_SWITCH_ENCAP_DEST_REPLACE only.")));
    }

    if (encap_info->flags & BCM_SWITCH_ENCAP_DEST_REPLACE)
    {
        /* Copy provided structure to local because it will be modified. */
        sal_memcpy(&encap_info_old, encap_info, sizeof(bcm_switch_encap_dest_t));
        /* Retrun error if not found */
        BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_get(unit, &encap_info_old));
    }

    if(!BCM_GPORT_IS_UCAST_QUEUE_GROUP(encap_info->gport))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Only apply to UNICAST_QUEUE GPORT type in this API."), unit));
    }

    flow_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(encap_info->gport);
    if(flow_id >= SOC_DPP_DEFS_GET(unit, nof_flows))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flow id should lower than max %d."), flow_id, SOC_DPP_DEFS_GET(unit, nof_flows)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_encap_dest_map_add
 * Description:
 *    Add one encap id to destination mapping entry.
 * Parameters:
 *  unit        - Device unit number
 *  encap_info->flags    - BCM_SWITCH_ENCAP_DEST_xxx flags.
 *  encap_info->encap_id - Encap-ID.
 *  encap_info->gport - Gport for Destination.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_encap_dest_map_add(
    int unit,
    bcm_switch_encap_dest_t *encap_info) 
{
    SOC_SAND_SUCCESS_FAILURE           success = SOC_SAND_SUCCESS;
    ARAD_PP_LEM_ACCESS_PAYLOAD         payload;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL                qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_PPC_FRWRD_DECISION_INFO        fwd_decision;
    uint32                             flow_id = 0;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_add_verify(unit, encap_info));

    table_id = SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    qual_vals[1].type= SOC_PPC_FP_QUAL_HDR_FTMH; /* MCID_OR_LIF [16: 18] in [20:22] FTMH */
    qual_vals[1].val.arr[0]= ((encap_info->encap_id >> 16) << 20);
    qual_vals[1].is_valid.arr[0]= 0x700000;
    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_FTMH;
    qual_vals[0].val.arr[0]= (encap_info->encap_id << 4);
    qual_vals[0].is_valid.arr[0]= 0xFFFF0; /* MCID_OR_LIF [15: 0] in [4:19] FTMH */

    /* convert payload, get the encoded destination */
    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);
    flow_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(encap_info->gport);
    ARAD_PP_FRWRD_DECISION_EXPL_FLOW_SET(unit, &fwd_decision, flow_id);

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    BCM_SAND_IF_ERR_EXIT(arad_pp_fwd_decision_in_buffer_build(
                         unit,
                         ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
                         &fwd_decision,
                         &(payload.dest),
                         &(payload.asd)));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals, 0, &payload, &success));
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_encap_dest_map_delete
 * Description:
 *    Delete one encap id to destination mapping entry.
 * Parameters:
 *  unit        - Device unit number
 *  encap_info->encap_id - Encap-ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_encap_dest_map_delete(
    int unit,
    bcm_switch_encap_dest_t *encap_info) 
{

    SOC_SAND_SUCCESS_FAILURE           success = SOC_SAND_SUCCESS;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL                qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    bcm_switch_encap_dest_t            encap_info_old;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_key_verify(unit, encap_info));

    /* Copy provided structure to local because it will be modified. */
    sal_memcpy(&encap_info_old, encap_info, sizeof(bcm_switch_encap_dest_t));
    /* Retrun error if not found */
    BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_get(unit, &encap_info_old));

    table_id = SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    qual_vals[1].type= SOC_PPC_FP_QUAL_HDR_FTMH; /* MCID_OR_LIF [16: 18] in [20:22] FTMH */
    qual_vals[1].val.arr[0]= ((encap_info->encap_id >> 16) << 20);
    qual_vals[1].is_valid.arr[0]= 0x700000;
    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_FTMH;
    qual_vals[0].val.arr[0]= (encap_info->encap_id << 4);
    qual_vals[0].is_valid.arr[0]= 0xFFFF0; /* MCID_OR_LIF [15: 0] in [4:19] FTMH */

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_encap_dest_map_get
 * Description:
 *    Get the destination with the given encap_id.
 * Parameters:
 *  unit        - Device unit number
 *  encap_info->encap_id - Encap-ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_encap_dest_map_get(
    int unit,
    bcm_switch_encap_dest_t *encap_info) 
{

    ARAD_PP_LEM_ACCESS_PAYLOAD         payload;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FP_QUAL_VAL                qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_PPC_FRWRD_DECISION_INFO        fwd_decision;
    uint8 found = 0;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK;

    BCMDNX_IF_ERR_EXIT(bcm_petra_switch_encap_dest_map_key_verify(unit, encap_info));

    table_id = SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    qual_vals[1].type= SOC_PPC_FP_QUAL_HDR_FTMH; /* MCID_OR_LIF [16: 18] in [20:22] FTMH */
    qual_vals[1].val.arr[0]= ((encap_info->encap_id >> 16) << 20);
    qual_vals[1].is_valid.arr[0]= 0x700000;
    qual_vals[0].type= SOC_PPC_FP_QUAL_HDR_FTMH;
    qual_vals[0].val.arr[0]= (encap_info->encap_id << 4);
    qual_vals[0].is_valid.arr[0]= 0xFFFF0; /* MCID_OR_LIF [15: 0] in [4:19] FTMH */

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload, 0/*priority*/, NULL/*hit_bit*/, &found));
    if(!found) {
        return BCM_E_NOT_FOUND;
    }

    /* convert payload to bcm */
    BCM_SAND_IF_ERR_EXIT(arad_pp_fwd_decision_in_buffer_parse(
                         unit,
                         payload.dest,
                         0,
                         ARAD_PP_FWD_DECISION_PARSE_DEST,
                         &fwd_decision));
   BCM_GPORT_UNICAST_QUEUE_GROUP_SET(encap_info->gport, fwd_decision.dest_id);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *    bcm_petra_switch_encap_dest_map_traverse
 * Description:
 *    Traverse the all added encap id to destination map entries.
 * Parameters:
 *  unit        - Device unit number
 *  cb_fn          - Callback function.
 *  userdata    - Pointer to any user data associated with a call back function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_petra_switch_encap_dest_map_traverse(
    int unit, 
    bcm_switch_encap_dest_map_traverse_cb cb_fn, 
    void *user_data)
{

    bcm_switch_encap_dest_t            encap_info;
    SOC_PPC_FP_QUAL_VAL                qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_PP_LEM_ACCESS_PAYLOAD         payload;
    uint8                              hit_bit = 0;
    uint8                              found = 0;
    SOC_DPP_DBAL_SW_TABLE_IDS          table_id;
    SOC_PPC_FRWRD_DECISION_INFO        fwd_decision;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_SWITCH_UNIT_VALID_CHECK

    BCMDNX_NULL_CHECK(cb_fn);

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    table_id = SOC_DPP_DBAL_SW_TABLE_ID_ENCAP_TO_DEST_LEM;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_init(unit, table_id, NULL));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next(unit, table_id, 0, qual_vals, &payload, NULL, &hit_bit, &found));

    while (found) {
        sal_memset(&encap_info, 0x0, sizeof(bcm_switch_encap_dest_t));
        /* get info from existing entry */
        encap_info.encap_id = (((qual_vals[1].val.arr[0] >> 20) & 0x7) << 16)
                              + ((qual_vals[0].val.arr[0] >> 4) & 0xFFFF);

        BCM_SAND_IF_ERR_EXIT(arad_pp_fwd_decision_in_buffer_parse(
                             unit,
                             payload.dest,
                             0,
                             ARAD_PP_FWD_DECISION_PARSE_DEST,
                             &fwd_decision));
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(encap_info.gport, fwd_decision.dest_id);

        /* call for call-back */
        BCMDNX_IF_ERR_EXIT(cb_fn(unit, &encap_info, user_data));

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_get_next(unit, table_id, 0, qual_vals, &payload, NULL, &hit_bit, &found));
    }

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_iterator_deinit(unit, table_id));

exit:
    BCMDNX_FUNC_RETURN;
}
