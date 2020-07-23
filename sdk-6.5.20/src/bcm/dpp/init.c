/*

 * 

 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *   This module calls the initialization routine of each BCM module.
 *
 * Initial System Configuration
 *
 *   Each module should initialize itself without reference to other BCM
 *   library modules to avoid a chicken-and-the-egg problem.  To do
 *   this, each module should initialize its respective internal state
 *   and hardware tables to match the Initial System Configuration.  The
 *   Initial System Configuration is:
 *
 *   STG 1 containing VLAN 1
 *   STG 1 all ports in the DISABLED state
 *   VLAN 1 with
 *      PBMP = all switching Ethernet ports (non-fabric) and the CPU.
 *      UBMP = all switching Ethernet ports (non-fabric).
 *   No trunks configured
 *   No mirroring configured
 *   All L2 and L3 tables empty
 *   Ingress VLAN filtering disabled
 *   BPDU reception enabled
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/counter.h>
#include <soc/dcmn/dcmn_wb.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/JER/jer_init.h>

#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_egr_queuing.h>
#endif

#include <bcm_int/control.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/init.h>
#include <bcm/l2.h>
#include <bcm/link.h>
#include <bcm/rx.h>
#include <bcm/stat.h>
#include <bcm/stg.h>
#include <bcm/vlan.h>
#include <bcm/vswitch.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/mpls.h>
#include <bcm/fabric.h>
#include <bcm/policer.h>
#include <bcm/oam.h>
#include <bcm/field.h>
#include <bcm/mirror.h>
#include <bcm/mcast.h>
#include <bcm/qos.h>
#include <bcm/trunk.h>
#include <bcm/trill.h>
#include <bcm/mim.h>
#include <bcm/extender.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/family.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/extender.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/stat.h>
#include <bcm_int/dpp/link.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/dpp_eyescan.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/lb.h>

#include <soc/dpp/drv.h>

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif

#ifdef INCLUDE_FCMAP
#include <bcm_int/common/fcmap_cmn.h>
#endif


#include <soc/dpp/ARAD/arad_nif.h>

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
#include <shared/mem_measure_tool.h>
#endif

#ifdef BCM_PETRA_SUPPORT
extern int _bcm_dpp_time_capture_init(int unit);
#endif

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
#define BCM_INIT_MODULE(func,rv,str) do { \
    int rv_mem = memory_consumption_start_measurement(str); \
    BCMDNX_IF_ERR_EXIT(rv_mem); \
    rv = (func); \
    BCMDNX_IF_ERR_EXIT(rv); \
    rv_mem = memory_consumption_end_measurement(str); \
    BCMDNX_IF_ERR_EXIT(rv_mem); \
    memory_measurement_print(str); \
} while(0)
#else
#define BCM_INIT_MODULE(func,rv,str) do { \
    rv = (func); \
    BCMDNX_IF_ERR_EXIT(rv); \
} while(0)
#endif

int _bcm_dpp_init_finished_ok[BCM_MAX_NUM_UNITS] = { 0 };

STATIC int
_bcm_petra_lock_init(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (_bcm_lock[unit] == NULL) {
        _bcm_lock[unit] = sal_mutex_create("bcm_petra_config_lock");
    }

    if (_bcm_lock[unit] == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_lock_deinit(int unit)
{
    if (_bcm_lock[unit] != NULL) {
        BCM_UNLOCK(unit);
        sal_mutex_destroy(_bcm_lock[unit]);
        _bcm_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 *  assumes unit, rv and stage_internal_id are declared in local scope
 */
#define INIT_CALL_CHK( m )                         \
  DCMN_RUNTIME_DEBUG_PRINT_LOC(unit, #m);          \
  LOG_INFO(BSL_LS_BCM_INIT,                        \
           (BSL_META_U(unit,                       \
                       "\t+ %d: %s\n"),unit ,#m)); \
  BCM_INIT_MODULE(bcm_petra_##m##_init(unit), rv, #m); \
  if (BCM_FAILURE(rv)) {                           \
      BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_init(%d): " #m " failed (%d -- %s)"), unit, rv, bcm_errmsg(rv))); \
  }

#ifdef BCM_PETRA_SUPPORT
int
bcm_petra_init_check(int unit)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = (_bcm_dpp_init_finished_ok[unit] == 1 ? BCM_E_NONE : BCM_E_UNIT);
    if (rc == BCM_E_UNIT) {
        /*bcm init not finished - return BCM_E_UNIT withot error printing*/
        BCM_ERR_EXIT_NO_MSG(rc);
    } else {
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_init_selective(int unit, uint32 flags)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_init_selective is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_init(int unit)
{
    int rv;
    int pp_enable=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    BCM_INIT_MODULE(_bcm_petra_lock_init(unit), rv, "petra_lock");

#ifdef INCLUDE_MACSEC
    BCM_IF_ERROR_RETURN(_bcm_common_macsec_init(unit));
#endif /* INCLUDE_MACSEC */

#ifdef INCLUDE_FCMAP
    BCM_IF_ERROR_RETURN(_bcm_common_fcmap_init(unit));
#endif /* INCLUDE_FCMAP */

    /* first thing we do is recover from scache if warm boot */
    
    BCM_INIT_MODULE(_bcm_sw_db_init(unit), rv, "bcm sw db init");

    /* Scache API completion for consistency state must be as early as possible */
    BCM_INIT_MODULE(_bcm_dpp_switch_init(unit), rv, "switch init");

    /*  The call sequence below replaces bcm_init_selective calls */
    bcm_chip_family_set(unit, BCM_FAMILY_DPP);

    pp_enable = SOC_DPP_PP_ENABLE(unit);

    DISPLAY_SW_STATE_MEM;
    DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to call %s\r\n",__func__,unit,"bcm_dpp_am_attach"));

    BCM_INIT_MODULE(bcm_dpp_am_attach(unit), rv, "am attach");

    if(!SOC_IS_FLAIR(unit)) { 
        DISPLAY_MEM;
        DISPLAY_SW_STATE_MEM;
        DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to call %s\r\n",__func__,unit,"bcm_dpp_counter_init"));
        BCM_INIT_MODULE(bcm_dpp_counter_init(unit), rv, "counters");
    }

    if (pp_enable) {
        DISPLAY_SW_STATE_MEM ;
        DISPLAY_MEM_PRINTF(("%s(): unit %d: Going to call %s\r\n",__func__,unit,"_bcm_dpp_gport_mgmt_init"));

        BCM_INIT_MODULE(_bcm_dpp_gport_mgmt_init(unit), rv, "mgmt init");
    }

    if ((!SOC_IS_FLAIR(unit)) && 
       ((soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0)== 0) ||
       ((soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) & 0x0002) == 0x0002))) {
        /* proceed multicase for non emulation chip or when emulation flag bit 2 is on */

        DISPLAY_SW_STATE_MEM;

        INIT_CALL_CHK(multicast);
    }

    if (pp_enable) {
        DISPLAY_SW_STATE_MEM;

        INIT_CALL_CHK(qos);
    }

    INIT_CALL_CHK(stk);

    if (pp_enable) {
        DISPLAY_MEM;
        DISPLAY_SW_STATE_MEM;

        INIT_CALL_CHK(l2);
    }


    INIT_CALL_CHK(port);
    
        
    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_IS_JERICHO(unit) && !SOC_IS_FLAIR(unit)) {
            /* synce depends on port_init and must be done after. we need to move it to soc with port_init */
            BCMDNX_IF_ERR_EXIT(jer_synce_init(unit));
        }
    }

#ifdef BCM_PETRA_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
      if (SOC_IS_JERICHO(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_dpp_time_capture_init(unit));
      }
    }
#endif

    if (pp_enable) {
        INIT_CALL_CHK(stg);
        INIT_CALL_CHK(mpls);

        if (SOC_DPP_CONFIG(unit)->trill.mode) {
            BCM_INIT_MODULE(bcm_petra_trill_init_data(unit), rv, "trill");
        }

        INIT_CALL_CHK(vswitch);
        INIT_CALL_CHK(vlan);
    }

    if(!SOC_IS_FLAIR(unit)) {
        INIT_CALL_CHK(cosq);
    }

    INIT_CALL_CHK(fabric);

    if(!SOC_IS_FLAIR(unit)) {
        INIT_CALL_CHK(linkscan);
    }

    if(!SAL_BOOT_PLISIM && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        BCM_INIT_MODULE(jer_mgmt_pvt_monitor_db_init(unit), rv, "PVT mon correction DB");
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_INIT_MODULE(jer_mgmt_pvt_monitor_correction_enable(unit, 2), rv, "PVT mon correction");
        BCMDNX_IF_ERR_EXIT(rv);
    }

#ifndef BCM_RX_DISABLE
    if(!SOC_IS_FLAIR(unit)) {
        INIT_CALL_CHK(rx);
    }
#else
    rv = _bcm_petra_rx_partial_init(unit);
    if (BCM_FAILURE(rv)) {
      BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("bcm_petra_init(%d):  _bcm_petra_rx_partial_init  failed (%d -- %s)"), unit, rv, bcm_errmsg(rv)));
    }
#endif

    DISPLAY_MEM;
    DISPLAY_SW_STATE_MEM;

    INIT_CALL_CHK(stat);

    if (pp_enable) {
#if defined(INCLUDE_L3)
        if (SOC_DPP_CONFIG(unit)->l3.ip_enable) {
            INIT_CALL_CHK(l3);
        }
#endif
        /* Call IPMC only in case it is enabled */
        if (SOC_DPP_CONTROL(unit)->cfg->pp.ipmc_enable) {
            INIT_CALL_CHK(ipmc);
        }

        DISPLAY_MEM;

        INIT_CALL_CHK(field);

        DISPLAY_MEM;

        INIT_CALL_CHK(policer);

        if (_BCM_DPP_EXTENDER_IS_ENABLED(unit)) {
            INIT_CALL_CHK(extender);
        }

        INIT_CALL_CHK(failover);
    }
     else if (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "support_petra_itmh", 0)) {
        /*TM mode*/
        INIT_CALL_CHK(field);
    }

    if(!SOC_IS_FLAIR(unit)) {
        INIT_CALL_CHK(mirror);
    }


        if (soc_feature(unit, soc_feature_time_support)) {
            INIT_CALL_CHK(time);
        }
#ifdef INCLUDE_PTP
        if (soc_feature(unit, soc_feature_ptp)) {
            LOG_VERBOSE(BSL_LS_BCM_INIT,
                        (BSL_META_U(unit,
                                    "PTP feature being initialized\n")));
            INIT_CALL_CHK(ptp);
        }
#endif /* INCLUDE_PTP */


    if(!SOC_IS_FLAIR(unit)) {
        INIT_CALL_CHK(tx);

        INIT_CALL_CHK(trunk);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        soc_pbmp_t okay_nif_ports;
        uint32 counter_flags;
        int counter_interval;
        bcm_pbmp_t counter_pbmp;

        BCM_PBMP_CLEAR(okay_nif_ports);

        /*
         * stop port related threads
         */

        /*counter thread*/
        /*get counter thread configuration*/
        BCMDNX_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

        if(!SOC_IS_JERICHO(unit)){
            /*Recover port module according to port sw data base*/
            BCM_IF_ERROR_RETURN(soc_pm_port_recover(unit, &okay_nif_ports));
        }

        if (pp_enable) {
            INIT_CALL_CHK(mim);
            INIT_CALL_CHK(trill);
        }

        if (pp_enable) {
           if ( SOC_DPP_CONFIG(unit)->pp.oam_enable || SOC_DPP_CONFIG(unit)->pp.bfd_enable ) {
              BCM_INIT_MODULE(bcm_petra_oam_init(unit), rv, "oam");
              if (BCM_FAILURE(rv)) {
                 LOG_ERROR(BSL_LS_APPL_COMMON,
                           (BSL_META_U(unit,
                                       "bcm_oam_init failed. Error:%d (%s)\n"),
                            rv, bcm_errmsg(rv)));
                 return rv;
              }
           }
        }

    } else { /* !SOC_WARM_BOOT*/
        /*init ofp rates configuration*/
        if (!SOC_IS_FLAIR(unit)) {
            BCM_IF_ERROR_RETURN(soc_arad_default_ofp_rates_set(unit));
        }
    }
#else /* !BCM_WARM_BOOT_SUPPORT */
        BCM_IF_ERROR_RETURN(soc_arad_default_ofp_rates_set(unit));
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_SAT_SUPPORT
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SAT_ENABLE, 0)) {
        BCM_IF_ERROR_RETURN(bcm_common_sat_init(unit));
    }
#endif /* BCM_SAT_SUPPORT */

#ifdef BCM_LB_SUPPORT
  if (SOC_IS_QAX(unit)) {
    if (SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
        INIT_CALL_CHK(lb);
    }
  }
#endif /* BCM_LB_SUPPORT */

#ifdef INCLUDE_KNET
    BCM_IF_ERROR_RETURN(bcm_petra_knet_init(unit));
#endif

/* init issu example */
#ifdef _SHR_SW_STATE_EXM
  if (!SOC_WARM_BOOT(unit)) {
      sw_state_access[unit].issu_example.alloc(unit);
      sw_state_access[unit].issu_example.myUint8.set(unit, 8);
      sw_state_access[unit].issu_example.myUint16.set(unit, 16);
#ifndef BCM_ISSU_TEST_AFTER_ISSU
      sw_state_access[unit].issu_example.myUint32.set(unit, 32);
      sw_state_access[unit].issu_example.myArray.set(unit, 0, 10);
      sw_state_access[unit].issu_example.myArray.set(unit, 0, 11);
      sw_state_access[unit].issu_example.myArray.set(unit, 0, 12);
      sw_state_access[unit].issu_example.myArray.set(unit, 0, 13);
      sw_state_access[unit].issu_example.myArray.set(unit, 0, 14);

      sw_state_access[unit].issu_example.myArray2D.set(unit, 0, 0, 1);
      sw_state_access[unit].issu_example.myArray2D.set(unit, 1, 0, 2);
      sw_state_access[unit].issu_example.myArray2D.set(unit, 1, 2, 21);
#endif
  }
#endif

    _bcm_dpp_init_finished_ok[unit] = 1;

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_PETRA_SUPPORT */

int
bcm_petra_info_get(int unit, bcm_info_t *info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(info);

    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = SOC_PCI_DEVICE(unit);
    info->revision = SOC_PCI_REVISION(unit);
    info->capability = 0;
    if (soc_feature(unit, soc_feature_l3)) {
        info->capability |= BCM_INFO_L3;
    }
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        info->capability |= BCM_INFO_IPMC;
    }
    BCM_EXIT;
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Call a function, ignore BCM_E_INIT but otherwise abort on errors */
#define SOC_PETRA_DETACH(_unit, _detach_f, _rv)                         \
  LOG_DEBUG(BSL_LS_BCM_INIT,                                            \
            (BSL_META_U(_unit,                                          \
                        "detach %s(%d)...\n"), #_detach_f, _unit));     \
  _rv = _detach_f(_unit);                                               \
  if (BCM_E_INIT == _rv) { _rv = BCM_E_NONE; }                          \
  if (BCM_FAILURE(_rv)) {                                               \
       LOG_ERROR(BSL_LS_BCM_INIT,                                       \
                 (BSL_META_U(_unit,                                     \
                             "detach unit %d, %s: %d (%s)\n"),          \
                             _unit, #_detach_f, _rv, bcm_errmsg(_rv))); \
  }                                                                     \
  BCMDNX_IF_ERR_CONT(_rv);

/*
 * Function:
 *         _bcm_petra_modules_deinit
 * Purpose:
 *         De-initialize bcm modules
 * Parameters:
 *     unit - (IN) BCM device number.
 *     silent - (IN) silent mode.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_petra_modules_deinit(int unit, int silent)
{
    int rv = BCM_E_NONE;
    int pp_enable=0;

    BCMDNX_INIT_FUNC_DEFS;

    pp_enable = SOC_DPP_PP_ENABLE(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
        /* SOC_PETRA_DETACH(unit, _bcm_petra_recovery_deinit, rv);*/
#endif  /* BCM_WARM_BOOT_SUPPORT */

    /* counter module should be always detached first */
    SOC_PETRA_DETACH(unit, bcm_dpp_counter_detach, rv);
    rv = soc_counter_stop(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /* raise a flag that indicate "detaching" */
    SOC_DETACH(unit,1);

    SOC_PETRA_DETACH(unit, bcm_petra_trunk_detach, rv);
    SOC_PETRA_DETACH(unit, bcm_common_tx_deinit, rv);
    SOC_PETRA_DETACH(unit, bcm_petra_mirror_detach, rv);

    {
#ifdef INCLUDE_PTP
        if (soc_feature(unit, soc_feature_ptp)) {
            SOC_PETRA_DETACH(unit, bcm_petra_ptp_detach, rv);
        }
#endif /* INCLUDE_PTP */
        if (soc_feature(unit, soc_feature_time_support)) {
            SOC_PETRA_DETACH(unit, bcm_petra_time_deinit, rv);
        }
    }

    if (pp_enable) {
        SOC_PETRA_DETACH(unit, bcm_petra_mim_detach, rv);
        SOC_PETRA_DETACH(unit, bcm_petra_policer_detach, rv);
        SOC_PETRA_DETACH(unit, bcm_petra_field_detach, rv);

        if (SOC_DPP_CONTROL(unit)->cfg->pp.ipmc_enable) {
            
            if (!(SOC_IS_ARAD(unit))) {
                SOC_PETRA_DETACH(unit, bcm_petra_ipmc_detach, rv);
                
            }
        }

        #ifdef INCLUDE_L3
            if (SOC_DPP_CONFIG(unit)->l3.ip_enable){
                SOC_PETRA_DETACH(unit, bcm_petra_mpls_cleanup, rv);
                SOC_PETRA_DETACH(unit, bcm_petra_vswitch_detach, rv);
                SOC_PETRA_DETACH(unit, bcm_petra_l3_cleanup, rv);
            }
        #endif /* def INCLUDE_L3 */
    }

    SOC_PETRA_DETACH(unit, bcm_petra_stat_stop, rv);
    SOC_PETRA_DETACH(unit, bcm_common_linkscan_detach, rv);
    /* SOC_PETRA_DETACH(unit, bcm_petra_fabric_detach, rv); no dynamic allocs */
    SOC_PETRA_DETACH(unit, bcm_petra_cosq_detach, rv);

    if (pp_enable) {
        SOC_PETRA_DETACH(unit, bcm_petra_vlan_detach, rv);
        SOC_PETRA_DETACH(unit, bcm_petra_stg_detach, rv);
        /* SOC_PETRA_DETACH(unit, bcm_petra_mim_deinit, rv); no dynamic alloc, no init is done by default */
        /* SOC_PETRA_DETACH(unit, bcm_petra_trill_detach, rv); no init is done by default */
#ifdef BCM_WARM_BOOT_SUPPORT
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                rv = bcm_petra_trill_deinit_data(unit);
                BCMDNX_IF_ERR_CONT(rv);
            }
#endif
        /* SOC_PETRA_DETACH(unit, bcm_petra_switch_detach, rv); no dynamic allocs */
    }

    SOC_PETRA_DETACH(unit, bcm_petra_port_deinit, rv);

    if (pp_enable) {
        SOC_PETRA_DETACH(unit, bcm_petra_l2_detach, rv);
    }

    SOC_PETRA_DETACH(unit, _bcm_petra_stk_detach, rv); /* no dynamic allocs in Soc_petra-B */
    if (pp_enable){ /* does not run for Arad for run Time */
        SOC_PETRA_DETACH(unit, bcm_petra_qos_detach, rv);
    }

    SOC_PETRA_DETACH(unit, bcm_petra_multicast_detach, rv);

    /* occupation management deinit */
    if (pp_enable) {
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_occ_mgmt_deinit,(unit)));
        BCMDNX_IF_ERR_CONT(rv);
    }

    if (pp_enable) {
        SOC_PETRA_DETACH(unit, _bcm_dpp_gport_mgmt_sw_state_cleanup, rv);

        rv = _bcm_sw_db_deinit(unit);
        BCMDNX_IF_ERR_CONT(rv);
    }

    rv = _bcm_dpp_switch_detach(unit);
    BCMDNX_IF_ERR_CONT(rv);
    SOC_PETRA_DETACH(unit, bcm_dpp_am_detach, rv); /* alloc_mngr */

    BCMDNX_IF_ERR_CONT(rv);
#ifdef BCM_SAT_SUPPORT
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SAT_ENABLE, 0)) {
        rv = bcm_common_sat_detach(unit);
        BCMDNX_IF_ERR_CONT(rv);
    }
#endif/* BCM_SAT_SUPPORT */
    SOC_PETRA_DETACH(unit, bcm_petra_rx_detach, rv);
#ifdef INCLUDE_KNET
    SOC_PETRA_DETACH(unit, bcm_petra_knet_cleanup, rv);
#endif
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_attach(int unit, char *subtype)
{
    int dunit, rv;

    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
    dunit = BCM_CONTROL(unit)->unit;
    if (SOC_UNIT_VALID(dunit)) {
        BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
        BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
        BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
        BCM_CONTROL(unit)->capability |= BCM_CAPA_SWITCH;
    }

    rv = bcm_dpp_eyescan_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_threads_shutdown
 * Purpose:
 *      Terminate all the spawned threads for specific unit.
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_threads_shutdown(int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* Operation return status. */

    if(!SAL_BOOT_PLISIM && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        BCM_INIT_MODULE(jer_mgmt_pvt_monitor_correction_disable(unit), rv, "PVT mon correction");
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = bcm_common_linkscan_enable_set(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_detach(int unit)
{
    int rv;
    int silent = 0 ;
    soc_dpp_control_t *dpp_cntr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    dpp_cntr = SOC_DPP_CONTROL(unit);
    silent = dpp_cntr->is_silent_init;

    /* Shut down all the spawned threads. */
    rv = _bcm_petra_threads_shutdown(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /*
     *  Don't move up, holding lock or disabling hw operations
     *  might prevent theads clean exit.
     */

    if (_bcm_lock[unit]) {
        LOG_VERBOSE(BSL_LS_BCM_INIT,
                    (BSL_META_U(unit,
                                "unit %d lock %p\n"), unit, (void*)(_bcm_lock[unit])));
        BCM_LOCK(unit);
    }

    rv = _bcm_petra_modules_deinit(unit,silent);
    BCMDNX_IF_ERR_CONT(rv);

    rv = _bcm_petra_lock_deinit(unit);
    BCMDNX_IF_ERR_CONT(rv);

    rv = bcm_dpp_eyescan_deinit(unit);
    BCMDNX_IF_ERR_CONT(rv);

    _bcm_dpp_init_finished_ok[unit] = 0;

    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_match(int unit, char *subtype_a, char *subtype_b)
{
    BCMDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(unit);
    BCMDNX_IF_ERR_EXIT(sal_strcmp(subtype_a, subtype_b));
exit:
                                 BCMDNX_FUNC_RETURN;
}

/**
 * \brief - Check if device is member in a group 'member_type'.
 *          Allows to write generic c-int and applications.
 *
 * \param [in] unit - unit #
 * \param [in] flags - set to 0
 * \param [in] member_type - group
 * \param [in] is_member - 1 if device is a member in the group (otherwise 0)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_petra_device_member_get(
    int unit,
    uint32 flags,
    bcm_device_member_t member_type,
    int *is_member)
{
    BCMDNX_INIT_FUNC_DEFS;

    /** verify */
    BCMDNX_NULL_CHECK(is_member);
    if (flags != 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags must be 0\n")));
    }


    /** get is_member */
    switch (member_type)
    {
        case bcmDeviceMemberDPP:
            *is_member = 1;
            break;

        default:
            *is_member = 0;
            break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}
