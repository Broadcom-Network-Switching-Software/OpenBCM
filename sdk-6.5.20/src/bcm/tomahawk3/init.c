/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM Library Initialization
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
 *	PBMP = all switching Ethernet ports (non-fabric) and the CPU.
 *	UBMP = all switching Ethernet ports (non-fabric).
 *   No trunks configured
 *   No mirroring configured
 *   All L2 and L3 tables empty
 *   Ingress VLAN filtering disabled
 *   BPDU reception enabled
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/cmext.h>
#include <soc/counter.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/phyctrl.h>

#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
#include <bcm/ipfix.h>

#ifdef SW_AUTONEG_SUPPORT
#include <bcm_int/common/sw_an.h>
#endif 
 
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#ifdef BCM_RCPU_SUPPORT
#include <bcm_int/esw/rcpu.h>
#endif
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/range.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw/rate.h>
#ifdef BCM_INSTRUMENTATION_SUPPORT
#include <bcm_int/esw/instrumentation.h>
#endif /* BCM_INSTRUMENTATION_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/policer.h>
#endif
#ifdef BCM_TRX_SUPPORT
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TOMAHAWK3X_SUPPORT)
#include <bcm_int/tomahawk3x_dispatch.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/mcast.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/latency_monitor.h>
#endif

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif /* INCLUDE_MACSEC */

#ifdef INCLUDE_FCMAP
#include <bcm_int/common/fcmap_cmn.h>
#endif /* INCLUDE_FCMAP */

#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#include <bcm_int/esw/tomahawk.h>
#endif

#include <bcm_int/esw_dispatch.h>
#include <shared/shr_bprof.h>

#include <bcm_int/esw/xgs5.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/init/tomahawk3_ep_init.h>

#define BCM_CHECK_ERROR_RETURN(rv, dispname)                         \
    if ((rv < 0) && (rv != BCM_E_UNAVAIL)) {                         \
        LOG_WARN(BSL_LS_BCM_INIT,                                    \
                    (BSL_META_U(unit, "bcm_init failed in %s\n"),    \
                 shr_bprof_stats_name(dispname)));                   \
        return (rv);                                                 \
    }

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int soc_ctr_evict_start(int unit, uint32 flags, sal_usecs_t interval);
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_esw_tr3_port_lanes_init(int unit);
#endif


#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
extern int bcm_esw_time_capture_ts_init(int unit);
extern int bcm_esw_time_capture_ts_deinit(int unit);
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

/*
 * Function:
 *	_bcm_lock_init
 * Purpose:
 *	Allocate BCM_LOCK.
 */

STATIC int
_bcm_th3_lock_init(int unit)
{
    if (_bcm_lock[unit] == NULL) {
	_bcm_lock[unit] = sal_mutex_create("bcm_config_lock");
    }

    if (_bcm_lock[unit] == NULL) {
	return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_lock_deinit
 * Purpose:
 *	De-allocate BCM_LOCK.
 */

STATIC int
_bcm_th3_lock_deinit(int unit)
{
    if (_bcm_lock[unit] != NULL) {
        sal_mutex_destroy(_bcm_lock[unit]);
        _bcm_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}

#define _DEINIT_INFO_VERB(_mod) \
    LOG_VERBOSE(BSL_LS_BCM_COMMON, \
                (BSL_META_U(unit, \
                            "bcm_detach: Deinitializing %s...\n"),   \
                 _mod));

#define _DEINIT_CHECK_ERR(_rv, _mod) \
    if (_rv != BCM_E_NONE && _rv != BCM_E_UNAVAIL) { \
        LOG_WARN(BSL_LS_BCM_INIT, \
                 (BSL_META_U(unit, \
                             "Warning: Deinitializing %s returned %d\n"), \
                  _mod, _rv)); \
    }

/*
 * Function:
 *	   _bcm_th3_modules_deinit
 * Purpose:
 *     bcm_detach_late_txrx() will deinit all modules except TX and RX.
 *     A regular bcm_detach() must follow to detach the remaining modules.
 *	   De-initialize bcm modules
 * Parameters:
 *     unit - (IN) BCM device number.
 *     pktio_enabled - Indicating pktio is enabled,
 *                     Should clean up module correspondingly.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_modules_deinit(int unit, int pktio_enabled)
{
    int rv;    /* Operation return status. */

#ifdef INCLUDE_KNET
    uint8 deinit_knet = TRUE;
#endif

#ifdef INCLUDE_KNET
    if (SOC_WARM_BOOT(unit) && SOC_KNET_MODE(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        deinit_knet = FALSE;
    }
#endif


#ifdef INCLUDE_PKTIO
    if (pktio_enabled) {
        if (BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitAll) {
            _DEINIT_INFO_VERB("pktio");
            rv = bcm_tomahawk3x_pktio_cleanup(unit);
            _DEINIT_CHECK_ERR(rv, "pktio");
            _DEINIT_INFO_VERB("rx");
            rv = bcm_tomahawk3x_rx_deinit(unit);
            _DEINIT_CHECK_ERR(rv, "rx");
            BCM_UNLOCK(unit);
            _bcm_th3_lock_deinit(unit);
            return rv;
        }
    } else
#endif /* INCLUDE_PKTIO */
    {
        /* Only TX/RX deinit should be done here */
        if (BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitAll) {
            _DEINIT_INFO_VERB("rx");
            rv = bcm_esw_rx_deinit(unit);
            /* There is no tx deinit defined currently */
            BCM_UNLOCK(unit);
            _bcm_th3_lock_deinit(unit);
            return rv;
        }
    }
#ifdef INCLUDE_BFD
    if (soc_feature(unit, soc_feature_bfd)) {
        _DEINIT_INFO_VERB("bfd");
        rv =  bcm_esw_bfd_detach(unit);
        _DEINIT_CHECK_ERR(rv, "bfd");
    }
#endif
#ifdef INCLUDE_PTP
    if (soc_feature(unit, soc_feature_ptp)) {
        _DEINIT_INFO_VERB("ptp");
	    rv =  bcm_esw_ptp_detach(unit);
        _DEINIT_CHECK_ERR(rv, "ptp");
    }
#endif

#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset))
    {
        _DEINIT_INFO_VERB("failover");
        rv = bcm_esw_failover_cleanup(unit);
        _DEINIT_CHECK_ERR(rv, "failover");
    }
#endif

    if (soc_feature(unit, soc_feature_time_support))
    {
        _DEINIT_INFO_VERB("time");
        rv = bcm_esw_time_deinit(unit);
        _DEINIT_CHECK_ERR(rv, "time");
    }

    _DEINIT_INFO_VERB("udf");
    rv = bcm_esw_udf_detach(unit);
    _DEINIT_CHECK_ERR(rv, "udf");

#ifdef BCM_FIELD_SUPPORT
    _DEINIT_INFO_VERB("auth");
    rv = bcm_esw_auth_detach(unit);
    _DEINIT_CHECK_ERR(rv, "auth");

    _DEINIT_INFO_VERB("field");
    rv = bcm_esw_field_detach(unit);
    _DEINIT_CHECK_ERR(rv, "field");
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECKm)) {
            _DEINIT_INFO_VERB("range");
            rv = bcm_esw_range_detach(unit);
            _DEINIT_CHECK_ERR(rv, "range");
        }
    }
#endif
#endif /* BCM_FIELD_SUPPORT */

#ifdef INCLUDE_L3
    _DEINIT_INFO_VERB("proxy");
    rv = bcm_esw_proxy_cleanup(unit);
    _DEINIT_CHECK_ERR(rv, "proxy");

    _DEINIT_INFO_VERB("multicast");
    rv = bcm_esw_multicast_detach(unit);
    _DEINIT_CHECK_ERR(rv, "multicast");

    if (soc_feature(unit, soc_feature_port_extension)) {
        _DEINIT_INFO_VERB("extender");
        rv =  bcm_esw_extender_cleanup(unit);
        _DEINIT_CHECK_ERR(rv, "extender");
    }

#ifdef BCM_MPLS_SUPPORT
    _DEINIT_INFO_VERB("mpls");
    rv = bcm_esw_mpls_cleanup(unit);
    _DEINIT_CHECK_ERR(rv, "mpls");
#endif /* BCM_MPLS_SUPPORT */

    _DEINIT_INFO_VERB("ipmc");
    rv = bcm_esw_ipmc_detach(unit);
    _DEINIT_CHECK_ERR(rv, "ipmc");

    _DEINIT_INFO_VERB("l3");
    /*
     * COVERITY
     *
     * Coverity reports a call chain of depth greater than 20, but well
     * before the stack overflows, it calls bcm_esw_switch_control_get
     * with one control selection, then follows another control's path
     * to find more levels of stack.  This is spurious.
     */
    /* coverity[stack_use_overflow : FALSE] */
    rv = bcm_esw_l3_cleanup(unit);
    _DEINIT_CHECK_ERR(rv, "l3");
#endif /* INCLUDE_L3 */

#ifdef INCLUDE_PKTIO
    if (pktio_enabled) {
        if (!(BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitLateTxRx)) {
            _DEINIT_INFO_VERB("pktio");
            rv = bcm_tomahawk3x_pktio_cleanup(unit);
            _DEINIT_CHECK_ERR(rv, "pktio");
            _DEINIT_INFO_VERB("rx");
            rv = bcm_tomahawk3x_rx_deinit(unit);
            _DEINIT_CHECK_ERR(rv, "rx");
        }
    } else
#endif /* INCLUDE_PKTIO */
    {
        /* Delaying RX deinit so the CPU can stil receive packets
         * even when other modules are deinitialized. */
        if (!(BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitLateTxRx)) {
            _DEINIT_INFO_VERB("rx");
            rv = bcm_esw_rx_deinit(unit);
            _DEINIT_CHECK_ERR(rv, "rx");
        }

#if 0
        _DEINIT_INFO_VERB("tx");
        rv = bcm_esw_tx_deinit(unit);
        _DEINIT_CHECK_ERR(rv, "tx");
#endif 
    }
    _DEINIT_INFO_VERB("mirror");
    rv = bcm_esw_mirror_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "mirror");

#ifdef INCLUDE_KNET
    if (deinit_knet) {
        _DEINIT_INFO_VERB("knet");
        rv = bcm_esw_knet_cleanup(unit);
        _DEINIT_CHECK_ERR(rv, "knet");
    }
#endif

#ifdef INCLUDE_KNETSYNC
    if (deinit_knet && SOC_IS_TOMAHAWK3(unit)) {
        _DEINIT_INFO_VERB("knetsync");
        rv = bcm_esw_knetsync_deinit(unit);
        _DEINIT_CHECK_ERR(rv, "knetsync");
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_latency_monitor)) {
        _DEINIT_INFO_VERB("latency_monitor");
        rv =  bcm_tomahawk3_latency_monitor_detach(unit);
        _DEINIT_CHECK_ERR(rv, "latency_monitor");
    }
#endif

    _DEINIT_INFO_VERB("stats");
    rv = _bcm_esw_stat_detach(unit);
    _DEINIT_CHECK_ERR(rv, "stats");

    _DEINIT_INFO_VERB("linkscan");
    rv = bcm_esw_linkscan_detach(unit);
    _DEINIT_CHECK_ERR(rv, "linkscan");

#ifdef SW_AUTONEG_SUPPORT
    if (soc_feature(unit, soc_feature_sw_autoneg)) {
        _DEINIT_INFO_VERB("SW_AN");
        rv = bcm_sw_an_module_deinit(unit);
        _DEINIT_CHECK_ERR(rv, "SW_AN");
    }
#endif    

    _DEINIT_INFO_VERB("mcast");
    rv = _bcm_esw_mcast_detach(unit);
    _DEINIT_CHECK_ERR(rv, "mcast");

    _DEINIT_INFO_VERB("cosq");
    rv = bcm_esw_cosq_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "cosq");

    _DEINIT_INFO_VERB("trunk");
    rv = bcm_esw_trunk_detach(unit);
    _DEINIT_CHECK_ERR(rv, "trunk");

    _DEINIT_INFO_VERB("vlan");
    rv = bcm_tomahawk3_vlan_detach(unit);
    _DEINIT_CHECK_ERR(rv, "vlan");

    _DEINIT_INFO_VERB("stg");
    rv = bcm_esw_stg_detach(unit);
    _DEINIT_CHECK_ERR(rv, "stg");

    _DEINIT_INFO_VERB("l2");
    rv = bcm_esw_l2_detach(unit);
    _DEINIT_CHECK_ERR(rv, "l2");

    _DEINIT_INFO_VERB("port");
    rv = _bcm_esw_port_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "port");

    _DEINIT_INFO_VERB("ipfix");
    rv = _bcm_esw_ipfix_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "ipfix");

    _DEINIT_INFO_VERB("mbcm");
    rv = mbcm_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "mbcm");

    _DEINIT_INFO_VERB("qos");
    rv = bcm_esw_qos_detach(unit);
    _DEINIT_CHECK_ERR(rv, "qos");

    _DEINIT_INFO_VERB("switch");
    rv = _bcm_esw_switch_detach(unit);
    _DEINIT_CHECK_ERR(rv, "switch");

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)){
        _DEINIT_INFO_VERB("tsn");
        rv = bcm_esw_tsn_detach(unit);
        _DEINIT_CHECK_ERR(rv, "tsn");
    }
#endif /* BCM_TSN_SUPPORT */

#ifdef BCM_TRX_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_switching) ||
        soc_feature(unit, soc_feature_int_common_init)) {
        _DEINIT_INFO_VERB("common");
        rv = _bcm_common_cleanup(unit);
        _DEINIT_CHECK_ERR(rv, "common");
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    /* Service meter */
    if (soc_feature(unit, soc_feature_global_meter)) {
        _DEINIT_INFO_VERB("gmeter");
        rv = _bcm_esw_global_meter_cleanup(unit);
        _DEINIT_CHECK_ERR(rv, "gmeter");
    }
#endif /*BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    _DEINIT_INFO_VERB("rcpu");
    rv = _bcm_esw_rcpu_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "rcpu");
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
    _DEINIT_INFO_VERB("latency");
    rv = bcm_esw_switch_latency_deinit(unit);
    _DEINIT_CHECK_ERR(rv, "latency");
#endif

    _DEINIT_INFO_VERB("flowtracker");
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        rv =  bcm_tomahawk3_cosq_burst_monitor_detach(unit);
    } else {
        rv =  bcm_tomahawk3_flowtracker_detach(unit);
    }
    _DEINIT_CHECK_ERR(rv, "flowtracker");

#ifdef INCLUDE_TELEMETRY
    if (soc_feature(unit, soc_feature_telemetry)) {
        _DEINIT_INFO_VERB("telemetry");
        rv = bcm_esw_telemetry_detach(unit);
        _DEINIT_CHECK_ERR(rv, "telemetry");
    }
#endif

#if defined (INCLUDE_INT)
    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        _DEINIT_INFO_VERB("int");
        rv = bcm_tomahawk3_int_detach(unit);
        _DEINIT_CHECK_ERR(rv, "int");
    }
#endif /* INCLUDE_INT */

#if defined (INCLUDE_SUM)
    if (soc_feature(unit, soc_feature_sum)) {
        _DEINIT_INFO_VERB("sum");
        rv = bcm_tomahawk3_sum_detach(unit);
        _DEINIT_CHECK_ERR(rv, "sum");
    }
#endif /* INCLUDE_SUM */

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_time_capture_ts_deinit(unit));
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

#ifdef INCLUDE_IFA
    if (soc_feature(unit, soc_feature_ifa)) {
        _DEINIT_INFO_VERB("ifa");
        rv = bcm_esw_ifa_detach(unit);
        _DEINIT_CHECK_ERR(rv, "ifa");
    }
#endif

    /* Don't treat UNAVAIL as an error. It's expected if
       soc feature or config property of the module is disabled.
       Retruning UNAVAIL as final result will cause the bcm_detach fail */
    if (rv == BCM_E_UNAVAIL) {
        rv = BCM_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "bcm_detach: All modules deinitialized.\n")));
    
    BCM_UNLOCK(unit);
      /* LOCK deinit should not be performed only during the final detach */
    if (!(BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitLateTxRx)) {
        _bcm_th3_lock_deinit(unit);
    }

    return rv;
}

#define _THREAD_STOP_CHECK_ERR(_rv, _mod) \
    if (_rv != BCM_E_NONE && _rv != BCM_E_UNAVAIL) { \
        LOG_WARN(BSL_LS_BCM_INIT, \
                 (BSL_META_U(unit, \
                             "Warning: Stopping %s thread returned %d\n"), \
                  _mod, _rv)); \
    }

/*
 * Function:
 *      _bcm_th3_threads_shutdown
 * Purpose:
 *      Terminate all the spawned threads for specific unit. 
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */
STATIC int
_bcm_th3_threads_shutdown(int unit)
{
    int rv;     /* Operation return status. */

    rv = _bcm_esw_port_mon_stop(unit);
    _THREAD_STOP_CHECK_ERR(rv, "portmon");
    rv = bcm_esw_linkscan_enable_set(unit, 0);
    _THREAD_STOP_CHECK_ERR(rv, "linkscan");

#ifdef SW_AUTONEG_SUPPORT
    if (soc_feature(unit, soc_feature_sw_autoneg))  {
        rv = bcm_sw_an_enable_set(unit, 0);
        _THREAD_STOP_CHECK_ERR(rv, "SW_AN");
    }    
#endif
    
#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_stop(unit);
    _THREAD_STOP_CHECK_ERR(rv, "l2x");

    /* Stop L2 learning and aging threads */
    rv = soc_th3_l2_learn_thread_stop(unit);
    _THREAD_STOP_CHECK_ERR(rv, "l2lrn");

    rv = soc_th3_l2_age_stop(unit);
    _THREAD_STOP_CHECK_ERR(rv, "l2age");
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    rv = soc_counter_detach(unit);
    _THREAD_STOP_CHECK_ERR(rv, "counter");

   rv = _bcm_esw_port_portmod_thread_stop(unit);
    _THREAD_STOP_CHECK_ERR(rv, "portmod");

    return BCM_E_NONE;
}

#ifdef INCLUDE_TCL
#define _BCM_MOD_INIT_CER(unit, rtn, dispname, init) {                      \
    if (init) {                                                             \
        sal_usecs_t stime, etime;                                           \
        int         rv;                                                     \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: (%s)\n"),                        \
                                shr_bprof_stats_name(dispname)));           \
        stime = sal_time_usecs();                                           \
        SHR_BPROF_STATS_TIME(dispname) {                                    \
            rv = rtn(unit);                                                 \
            BCM_CHECK_ERROR_RETURN(rv, dispname);                           \
        }                                                                   \
        etime = sal_time_usecs();                                           \
        if (SOC_WARM_BOOT(unit)) {                                          \
            LOG_CLI((BSL_META_U(unit,                                       \
                                "bcm_init: %8s   took %10d usec\n"),        \
                     shr_bprof_stats_name(dispname),                        \
                     SAL_USECS_SUB(etime, stime)));                         \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
        } else {                                                            \
            LOG_VERBOSE(BSL_LS_BCM_COMMON,                                  \
                        (BSL_META_U(unit,                                   \
                                    "bcm_init: %8s   took %10d usec\n"),    \
                         shr_bprof_stats_name(dispname),                    \
                         SAL_USECS_SUB(etime, stime)));                     \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
        }                                                                   \
    } else {                                                                \
        LOG_WARN(BSL_LS_BCM_INIT,                                           \
                 (BSL_META_U(unit,                                          \
                             "bcm_init: skipped %s init\n"),                \
                             shr_bprof_stats_name(dispname)));              \
    }                                                                       \
}
#else
#define _BCM_MOD_INIT_CER(unit, rtn, dispname, init) {                      \
    if (init) {                                                             \
        sal_usecs_t stime, etime;                                           \
        int         rv;                                                     \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: (%s)\n"),                        \
                                shr_bprof_stats_name(dispname)));           \
        stime = sal_time_usecs();                                           \
        SHR_BPROF_STATS_TIME(dispname) {                                    \
            rv = rtn(unit);                                                 \
            BCM_CHECK_ERROR_RETURN(rv, dispname);                           \
        }                                                                   \
        etime = sal_time_usecs();                                           \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: %8s   took %10d usec\n"),        \
                                shr_bprof_stats_name(dispname),             \
                     SAL_USECS_SUB(etime, stime)));                         \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
    } else {                                                                \
        LOG_WARN(BSL_LS_BCM_INIT,                                           \
                 (BSL_META_U(unit,                                          \
                             "bcm_init: skipped %s init\n"),                \
                             shr_bprof_stats_name(dispname)));              \
    }                                                                       \
}
#endif

#ifdef INCLUDE_TCL
#define _BCM_MOD_INIT_IER(unit, rtn, dispname, init) {                      \
    if (init) {                                                             \
        sal_usecs_t stime, etime;                                           \
        int         rv;                                                     \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: (%s)\n"),                        \
                                shr_bprof_stats_name(dispname)));           \
        stime = sal_time_usecs();                                           \
        SHR_BPROF_STATS_TIME(dispname) {                                    \
            rv = rtn(unit);                                                 \
            BCM_IF_ERROR_RETURN(rv);                                        \
        }                                                                   \
        etime = sal_time_usecs();                                           \
        if (SOC_WARM_BOOT(unit)) {                                          \
            LOG_CLI((BSL_META_U(unit,                                       \
                                "bcm_init: %8s   took %10d usec\n"),        \
                     shr_bprof_stats_name(dispname),                        \
                     SAL_USECS_SUB(etime, stime)));                         \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
        } else {                                                            \
            LOG_VERBOSE(BSL_LS_BCM_COMMON,                                  \
                        (BSL_META_U(unit,                                   \
                                    "bcm_init: %8s   took %10d usec\n"),    \
                         shr_bprof_stats_name(dispname),                    \
                         SAL_USECS_SUB(etime, stime)));                     \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
        }                                                                   \
    } else {                                                                \
        LOG_WARN(BSL_LS_BCM_INIT,                                           \
                 (BSL_META_U(unit,                                          \
                             "bcm_init: skipped %s init\n"),                \
                             shr_bprof_stats_name(dispname)));              \
    }                                                                       \
}
#else
#define _BCM_MOD_INIT_IER(unit, rtn, dispname, init) {                      \
    if (init) {                                                             \
        sal_usecs_t stime, etime;                                           \
        int         rv;                                                     \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: (%s)\n"),                        \
                                shr_bprof_stats_name(dispname)));           \
        stime = sal_time_usecs();                                           \
        SHR_BPROF_STATS_TIME(dispname) {                                    \
            rv = rtn(unit);                                                 \
            BCM_IF_ERROR_RETURN(rv);                                        \
        }                                                                   \
        etime = sal_time_usecs();                                           \
        LOG_VERBOSE(BSL_LS_BCM_COMMON,                                      \
                    (BSL_META_U(unit,                                       \
                                "bcm_init: %8s   took %10d usec\n"),        \
                                shr_bprof_stats_name(dispname),             \
                     SAL_USECS_SUB(etime, stime)));                         \
            _bcm_th3_modules_init_time += SAL_USECS_SUB(etime, stime);      \
    } else {                                                                \
        LOG_WARN(BSL_LS_BCM_INIT,                                           \
                 (BSL_META_U(unit,                                          \
                             "bcm_init: skipped %s init\n"),                \
                             shr_bprof_stats_name(dispname)));              \
    }                                                                       \
}
#endif

/*
 * Function:
 *	_bcm_modules_init
 * Purpose:
 * 	Initialize bcm modules
 * Parameters:
 *	unit - StrataSwitch unit #.
 *      flags - Combination of bit selectors (see init.h)
 * Returns:
 *	BCM_E_XXX
 */

STATIC int
_bcm_th3_modules_init(int unit)
{
    int init_cond, init_cond2; /* init condition */
    bcm_port_t port_iter;
    sal_usecs_t _bcm_th3_modules_init_time = 0;
    pbmp_t mask;
    bcm_port_rlm_config_t rlm;
    int enabled;
#ifdef INCLUDE_KNET
    uint8 init_knet = TRUE;
#endif

    SHR_BPROF_STATS_DECL;
    /*
     * Initialize each bcm module
     */

    if (!SOC_UNIT_VALID(unit)) {
	return BCM_E_UNIT;
    }

#ifdef INCLUDE_KNET
    if (SOC_WARM_BOOT(unit) && SOC_KNET_MODE(unit) &&
        soc_property_get(unit, spn_WARMBOOT_KNET_SHUTDOWN_MODE, 0)) {
        init_knet = FALSE;
    }
#endif

    /* Only Tx and Rx modules will be initialized if attaching early
     * during warmboot to minimize CPU TX/RX inactivity time.
     */
    if (BCM_CONTROL(unit)->attach_state == _bcmControlStateTxRxInit) {
#ifdef INCLUDE_PKTIO
        if (soc_feature(unit, soc_feature_sdklt_pktio)) {
            _BCM_MOD_INIT_CER(unit, bcm_tomahawk3x_pktio_init, SHR_BPROF_BCM_PKTIO, TRUE);
            _BCM_MOD_INIT_CER(unit, bcm_tomahawk3x_rx_init, SHR_BPROF_BCM_RX, TRUE);
        } else
#endif
        {
#ifdef INCLUDE_KNET
            if (init_knet) {
                _BCM_MOD_INIT_CER(unit, bcm_esw_knet_init, SHR_BPROF_BCM_KNET, TRUE);
#ifdef INCLUDE_KNETSYNC
                if (init_knet && SOC_IS_TOMAHAWK3(unit)) {
                    bcm_esw_knetsync_init(unit);
                }
#endif

            }
#endif
            _BCM_MOD_INIT_CER(unit, bcm_esw_tx_init, SHR_BPROF_BCM_TX, TRUE);
            _BCM_MOD_INIT_CER(unit, bcm_esw_rx_init, SHR_BPROF_BCM_RX, TRUE);
        }
        return BCM_E_NONE;
    }

    /* Must call mbcm init first to ensure driver properly installed */
    BCM_IF_ERROR_RETURN(mbcm_init(unit));

#if defined(BCM_WARM_BOOT_SUPPORT)    
    if (SOC_WARM_BOOT(unit) && !SOC_IS_XGS12_FABRIC(unit)) {
        /* Init local module id. */
        BCM_IF_ERROR_RETURN(bcm_esw_reload_stk_my_modid_get(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    if (SOC_FAST_REBOOT(unit)) {
        SOC_WARM_BOOT_DONE(unit);
    }

    /* When adding new modules, double check init condition 
     *  TRUE      - init the module always
     *  init_cond - Conditional init based on boot flags/soc properties
     */
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_feature(unit, soc_feature_virtual_switching) ||
        soc_feature(unit, soc_feature_gport_service_counters) ||
        soc_feature(unit, soc_feature_fast_init) ||
        soc_feature(unit, soc_feature_int_common_init)){
        /* Initialize the common data module here to avoid multiple
         * initializations in the required modules. */
        _BCM_MOD_INIT_CER(unit, _bcm_common_init, SHR_BPROF_BCM_COMMON, TRUE);

    }
#endif /* BCM_TRIUMPH_SUPPORT */

    /* Set Warm Boot Flag so we can fetch the BCM port function pointers */
    if (SOC_FAST_REBOOT(unit)) {
        SOC_WARM_BOOT_START(unit);
    }

    /* This call to bcm_esw_port_init is normal for CB and WB, but for
     * Fast Reboot, it returns after fetching port device function
     * pointers because Fast Reboot needs to disable and drain ports */
    _BCM_MOD_INIT_CER(unit, bcm_esw_port_init, SHR_BPROF_BCM_PORT, TRUE);

    /* Clear Warm Boot Flag */
    if (SOC_FAST_REBOOT(unit)) {
        SOC_WARM_BOOT_DONE(unit);
    }

    /* If we're in fast reboot, we warmbooted up until this point. Cold boot
     * as much as we can from this point on */
    if (SOC_FAST_REBOOT(unit)) {

        /* For resetting CPU credits */
        SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
            0, 1));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, NULL, 0, 0, 1));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, NULL, 0, 0, 1));

        /* Disable traffic and drain MMU */
        soc_link_mask2_get(unit, &mask);

        /* Save the mask so that it can be set again when the user calls the
         * API to re-enable traffic
         */
        BCM_IF_ERROR_RETURN(bcmi_th3_port_fast_reboot_epc_link_bmap_save(unit,
                                                                        mask));

        SOC_PBMP_CLEAR(mask);
        /* Clear EPC_LINK_BMAP and ING_DEST_PORT_ENABLE so that traffic
         * can no longer ingress */
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));

        PBMP_PORT_ITER(unit, port_iter) {

            BCM_IF_ERROR_RETURN(bcmi_xgs5_port_enable_set(unit, port_iter, 0));
            /* In TH3, MMU is drained when queuing is disabled. No need to do
             * bcmPortControlMmuDrain, BUT PROBABLY NEED TO IN OTHER DEVICES! */
        }


        /* init soc related functions */
        BCM_IF_ERROR_RETURN(soc_reset_fast_reboot(unit));

        /* init misc related functions */
        BCM_IF_ERROR_RETURN(soc_tomahawk3_misc_init_fast_reboot(unit));

        /* init mmu related functions */
        BCM_IF_ERROR_RETURN(soc_mmu_init(unit));
        /* this time go through port init cold booting EVERYTHING
         * except portmod/probe */

        _BCM_MOD_INIT_CER(unit, bcm_esw_port_init, SHR_BPROF_BCM_PORT, TRUE);
    }

    /* switch to default miim intr mode from polling mode after port init done */
    SOC_CONTROL(unit)->miimIntrEnb = soc_property_get(unit,
            spn_MIIM_INTR_ENABLE, 1);
    init_cond = (!(SAL_BOOT_SIMULATION &&
                   soc_property_get(unit, spn_SKIP_L2_VLAN_INIT, 0)));

    _BCM_MOD_INIT_CER(unit, bcm_esw_stg_init, SHR_BPROF_BCM_STG, TRUE);

    _BCM_MOD_INIT_CER(
        unit, bcm_tomahawk3_l2_init, SHR_BPROF_BCM_L2, init_cond);


    _BCM_MOD_INIT_CER(
        unit, bcm_tomahawk3_vlan_init, SHR_BPROF_BCM_VLAN, init_cond);

    init_cond = (!(SAL_BOOT_SIMULATION &&
                   soc_property_get(unit, "skip_trunk_init", 0)));
    _BCM_MOD_INIT_CER(unit, bcm_esw_trunk_init, SHR_BPROF_BCM_TRUNK, init_cond);

    init_cond  = soc_property_get(unit, "init_all_modules",
                                  (SAL_BOOT_BCMSIM) || (!SAL_BOOT_SIMULATION));
    init_cond2 = init_cond || (soc_property_get(unit, "skip_cosq_init",
                                                0) ? FALSE : TRUE);
    _BCM_MOD_INIT_CER(unit, bcm_esw_cosq_init, SHR_BPROF_BCM_COSQ, init_cond2);

    init_cond2 = init_cond || (!soc_property_get(unit, "skip_mcast_init", 0));
    _BCM_MOD_INIT_CER(unit, bcm_esw_mcast_init, SHR_BPROF_BCM_MCAST, init_cond2);

    _BCM_MOD_INIT_CER(unit, bcm_esw_linkscan_init, SHR_BPROF_BCM_LINKSCAN, TRUE);

#ifdef SW_AUTONEG_SUPPORT
    if (soc_feature(unit, soc_feature_sw_autoneg)) {
        _BCM_MOD_INIT_CER(unit, bcm_sw_an_module_init, SHR_BPROF_BCM_SW_AN, TRUE);
    }
#endif

    _BCM_MOD_INIT_CER(unit, bcm_esw_stat_init, SHR_BPROF_BCM_STAT, TRUE);
#ifdef INCLUDE_KNET
    /* KNET module already initialized during early attach */
    if (BCM_CONTROL(unit)->attach_state != _bcmControlStateTxRxInited) {
        if (init_knet) {
            _BCM_MOD_INIT_CER(unit, bcm_esw_knet_init, SHR_BPROF_BCM_KNET, init_cond);
        }
#ifdef INCLUDE_KNETSYNC
        if (init_knet && SOC_IS_TOMAHAWK3(unit)) {
            bcm_esw_knetsync_init(unit);
        }
#endif
    }
#endif

    if (SOC_MEM_IS_VALID(unit, FP_UDF_TCAMm)) {
        _BCM_MOD_INIT_CER(unit, bcm_esw_udf_init, SHR_BPROF_BCM_UDF, TRUE);
    }
#ifdef BCM_FIELD_SUPPORT
    if (SOC_MEM_IS_VALID(unit, IFP_RANGE_CHECKm)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_range_init, SHR_BPROF_BCM_RANGE, TRUE);
    }

    init_cond2 = init_cond && (!soc_property_get(unit, "skip_field_init", 0));
    if (soc_feature(unit, soc_feature_field)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_field_init, SHR_BPROF_BCM_FIELD,
                          TRUE);
    }
#endif
    init_cond2 = init_cond || (soc_property_get(unit, "skip_mirror_init", 
                                                0) ? FALSE : TRUE);
    _BCM_MOD_INIT_CER(unit, bcm_esw_mirror_init, SHR_BPROF_BCM_MIRROR, init_cond2);

#ifdef INCLUDE_PKTIO
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        _BCM_MOD_INIT_CER(unit, bcm_tomahawk3x_pktio_init, SHR_BPROF_BCM_PKTIO, TRUE);
        _BCM_MOD_INIT_CER(unit, bcm_tomahawk3x_rx_init, SHR_BPROF_BCM_RX, TRUE);
    } else
#endif
    {
        /* TX module already initialized during early attach */
        if (BCM_CONTROL(unit)->attach_state != _bcmControlStateTxRxInited) {
            _BCM_MOD_INIT_CER(unit, bcm_esw_tx_init, SHR_BPROF_BCM_TX, TRUE);
        }

        /* RX init is done in phases during early attach */
        _BCM_MOD_INIT_CER(unit, bcm_esw_rx_init, SHR_BPROF_BCM_RX, TRUE);
    }
#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_failover_fixed_nh_offset)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_failover_init, SHR_BPROF_BCM_FAILOVER, TRUE);
    }

    if (soc_feature(unit, soc_feature_l3) && soc_property_get(unit, spn_L3_ENABLE, 1)) {
        init_cond2 = init_cond || (!soc_property_get(unit, "skip_l3_init", 0));
        _BCM_MOD_INIT_IER(unit, bcm_esw_l3_init, SHR_BPROF_BCM_L3, init_cond2);
    }
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        init_cond2 = init_cond || (!soc_property_get(unit, "skip_ipmc_init", 0));
        _BCM_MOD_INIT_IER(unit, bcm_esw_ipmc_init, SHR_BPROF_BCM_IPMC, init_cond2);
    }

#endif

#if defined(BCM_MPLS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_mpls)) {
        init_cond2 = init_cond || (!soc_property_get(unit, "skip_mpls_init", 0));
        _BCM_MOD_INIT_IER(unit, bcm_esw_mpls_init, SHR_BPROF_BCM_MPLS, init_cond2);
    }
#endif

    if (soc_feature(unit, soc_feature_qos_profile)) {
        init_cond2 = init_cond || (!soc_property_get(unit, "skip_qos_init", 0));
        _BCM_MOD_INIT_IER(unit, bcm_esw_qos_init, SHR_BPROF_BCM_QOS, init_cond2);
    }
#ifdef INCLUDE_L3
    if (soc_feature(unit, soc_feature_flex_flow)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_flow_init, SHR_BPROF_BCM_FLOW, init_cond);
    }
    if (soc_feature(unit, soc_feature_port_extension)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_extender_init, SHR_BPROF_BCM_EXTENDER, init_cond);
    }

    /* This must be after the modules upon which it is build, in order
     * for Warm Boot to operate correctly. */
    _BCM_MOD_INIT_IER(unit, bcm_esw_multicast_init, SHR_BPROF_BCM_MULTICAST, TRUE);

#endif

#ifdef BCM_FIELD_SUPPORT
    if (soc_feature(unit, soc_feature_field)) {
         init_cond2 = init_cond && (!soc_property_get(unit, "skip_auth_init", 0));
        _BCM_MOD_INIT_IER(unit, bcm_esw_auth_init, SHR_BPROF_BCM_AUTH, init_cond2);
    }
#endif
#ifdef INCLUDE_REGEX
    if (soc_feature(unit, soc_feature_regex)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_regex_init, SHR_BPROF_BCM_REGEX, init_cond);
    }
#endif
    if (soc_feature(unit, soc_feature_time_support)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_time_init, SHR_BPROF_BCM_TIME, TRUE);
    }

#ifdef BCM_INSTRUMENTATION_SUPPORT
    if (soc_feature(unit, soc_feature_visibility)) {
        BCM_IF_ERROR_RETURN(bcm_esw_pkt_trace_init(unit));
    }
#endif
#ifdef INCLUDE_PTP
    if (soc_feature(unit, soc_feature_ptp)) {
	_BCM_MOD_INIT_IER(unit, bcm_esw_ptp_init, SHR_BPROF_BCM_PTP, init_cond);
    }
#endif
#ifdef INCLUDE_BFD
    if (soc_feature(unit, soc_feature_bfd)) {
	_BCM_MOD_INIT_IER(unit, bcm_esw_bfd_init, SHR_BPROF_BCM_BFD, init_cond);
    }
#endif
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    /* Service meter */
    if (soc_feature(unit, soc_feature_global_meter)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_global_meter_init,
                          SHR_BPROF_BCM_GLB_METER, init_cond);
    }
#endif /* BCM_KATANA_SUPPORT  or BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)){
        _BCM_MOD_INIT_CER(unit, bcm_esw_tsn_init, SHR_BPROF_BCM_TSN, TRUE);
    }
#endif /* BCM_TSN_SUPPORT */

    BCM_IF_ERROR_RETURN(bcm_th_phy_lb_set(unit));

    if (soc_feature(unit, soc_feature_latency_monitor)) {
        _BCM_MOD_INIT_IER(unit, bcm_tomahawk3_latency_monitor_init,
                          SHR_BPROF_BCM_LATENCY_MONITOR, TRUE);
    }

#if defined(BCM_COLLECTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_collector)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_collector_init, SHR_BPROF_BCM_COLLECTOR,
                          TRUE);
    }
#endif

    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        _BCM_MOD_INIT_IER(unit, bcm_tomahawk3_cosq_burst_monitor_init,
                SHR_BPROF_BCM_FT, TRUE);
    } else {
        if (soc_feature(unit, soc_feature_hw_etrap)) {
            _BCM_MOD_INIT_IER(unit, bcm_tomahawk3_flowtracker_init,
                    SHR_BPROF_BCM_FT, TRUE);
        }
   }

#if defined(INCLUDE_TELEMETRY)
    if (soc_feature(unit, soc_feature_telemetry)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_telemetry_init, SHR_BPROF_BCM_TELEMETRY,
                          TRUE);
    }
#endif

#ifdef INCLUDE_INT
    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        _BCM_MOD_INIT_IER(unit, bcm_tomahawk3_int_init, SHR_BPROF_BCM_INT, TRUE);
    }
#endif

#ifdef INCLUDE_SUM
    if (soc_feature(unit, soc_feature_sum)) {
        _BCM_MOD_INIT_IER(unit, bcm_tomahawk3_sum_init, SHR_BPROF_BCM_SUM, TRUE);
    }
#endif

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_esw_time_capture_ts_init(unit));
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

#if defined(INCLUDE_IFA)
    if (soc_feature(unit, soc_feature_ifa)) {
        _BCM_MOD_INIT_IER(unit, bcm_esw_ifa_init, SHR_BPROF_BCM_IFA,
                          TRUE);
    }
#endif

    if (SOC_WARM_BOOT(unit) || SOC_FAST_REBOOT(unit)) {
        LOG_CLI((BSL_META_U(unit,
                 "bcm_init all modules took %10d usec\n"),
                 _bcm_th3_modules_init_time));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "bcm_init all modules took %10d usec\n"),
                    _bcm_th3_modules_init_time));
    }

    if (SOC_FAST_REBOOT(unit)) {
        sal_memset(&rlm, 0, sizeof(rlm));

        /* Re-enable traffic to the MMU after it has been reset */

        PBMP_PORT_ITER(unit, port_iter) {
            /* Toggle ports in the event they are already enabled */
            BCM_IF_ERROR_RETURN(bcmi_xgs5_port_enable_set(unit, port_iter, 0));
            BCM_IF_ERROR_RETURN(bcmi_xgs5_port_enable_set(unit, port_iter, 1));
            /* In TH3, MMU is drained when queuing is disabled. No need to do
             * bcmPortControlMmuDrain, BUT PROBABLY NEED TO IN OTHER DEVICES! */

            if (!(IS_CPU_PORT(unit, port_iter) || IS_LB_PORT(unit, port_iter) ||
                  IS_MANAGEMENT_PORT(unit, port_iter))) {
                /* Make sure that RLM is disabled because the thread was killed
                 * when the SDK exited cold boot */
                BCM_IF_ERROR_RETURN(bcm_esw_port_rlm_config_get(unit,
                                                        port_iter,
                                                        &rlm,
                                                        &enabled));
                if (enabled) {
                    BCM_IF_ERROR_RETURN(bcm_esw_port_rlm_config_set(unit,
                                                        port_iter,
                                                        &rlm,
                                                        0));
                }
            }
        }

        /* Restore credits to the CPU by toggling these */
        SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
            0, 1));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, NULL, 0, 0, 1));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, NULL, 0, 0, 1));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, NULL, 0, 0, 0));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, NULL, 0, 0, 0));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_idb_cpu_ca_reset_buffer(unit,
            0, 0));

        SOC_IF_ERROR_RETURN(
                soc_cmicx_dma_ctrl_release_cpu_credits(unit));
    }
    return BCM_E_NONE;
}

#if defined(BCM_XGS3_SWITCH_SUPPORT)
#ifdef BCM_WARM_BOOT_SUPPORT

STATIC int
_bcm_th3_mem_scache_init(int unit)
{
    SOC_IF_ERROR_RETURN(soc_mem_cache_scache_init(unit));
    return BCM_E_NONE;
}

STATIC int 
_bcm_th3_switch_control_scache_init(int unit)
{
    return soc_switch_control_scache_init(unit);
}

#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* defined(BCM_XGS3_SWITCH_SUPPORT) */

/*
 * Function:
 *	_bcm_th3_init
 * Purpose:
 * 	Initialize the BCM API layer only, without resetting the switch chip.
 * Parameters:
 *	unit - StrataSwitch unit #.
 * Returns:
 *	BCM_E_XXX
 */

STATIC int
_bcm_th3_init(int unit)
{

    int warmboot_post_config = FALSE;

    BCM_IF_ERROR_RETURN(_bcm_th3_lock_init(unit));

    /* Skip rest of the initialization if early attaching */
    if (BCM_CONTROL(unit)->attach_state == _bcmControlStateTxRxInit) {
        if(SOC_WARM_BOOT(unit)) {
            return (_bcm_th3_modules_init(unit));
        } else {
            /* Not supported if not warm booting */
            return BCM_E_CONFIG;
        }
    }

    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_ALL_MODULES_INITED;

    /* If linkscan is running, disable it. */
    bcm_esw_linkscan_enable_set(unit, 0);

#ifdef INCLUDE_MACSEC
    BCM_IF_ERROR_RETURN(_bcm_common_macsec_init(unit)); 
#endif /* INCLUDE_MACSEC */

#ifdef INCLUDE_FCMAP
    BCM_IF_ERROR_RETURN(_bcm_common_fcmap_init(unit)); 
#endif /* INCLUDE_FCMAP */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
#ifdef BCM_WARM_BOOT_SUPPORT
    /* switch global warmboot state init/reinit */
    BCM_IF_ERROR_RETURN(_bcm_th3_switch_control_scache_init(unit));

    /* mem cache warmboot state init/reinit */
    BCM_IF_ERROR_RETURN(_bcm_th3_mem_scache_init(unit));


#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* defined(BCM_XGS3_SWITCH_SUPPORT) */

    BCM_IF_ERROR_RETURN(_bcm_th3_modules_init(unit));

#ifdef BCM_XGS3_SWITCH_SUPPORT
    BCM_IF_ERROR_RETURN(_bcm_esw_switch_init(unit));
#endif

    if (SOC_WARM_BOOT(unit)) {
        warmboot_post_config = TRUE;
        SOC_WARM_BOOT_DONE(unit);
    }

    if (warmboot_post_config == TRUE) {
        /* To avoid acctidently reconfigure hardware, write to register is
           bypassed during the warm boot until SOC_WARM_BOOT_DONE */
        /* for the cases that we do need to reconfigure hardware, we do it here */
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) { 
            if (soc_property_get(unit, "ctr_evict_enable", 1) &&
                (SOC_CONTROL(unit)->counter_interval > 0)) {
                /* Clear all previously accumulated counter value in SW */
                soc_counter_ctr_reset_to_zero(unit);
                /* WB done. Set counter to Evition mode */
                SOC_IF_ERROR_RETURN
                    (soc_ctr_evict_start(unit, 0,
                                         SOC_CONTROL(unit)->counter_interval));
            }
        }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if ((SOC_IS_TOMAHAWK3(unit)) && 
            (soc_feature(unit, soc_feature_latency_monitor))) {
            SOC_IF_ERROR_RETURN(_bcm_th3_latency_monitor_wb_post_init(unit));
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

#if defined(BCM_RCPU_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcm_esw_rcpu_init(unit)); 
#endif /* BCM_RCPU_SUPPORT && BCM_XGS3_SWITCH_SUPPORT */

    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;
    if (SOC_HW_RESET(unit)) {
        SOC_HW_RESET_DONE(unit);
    }

    return BCM_E_NONE;
}    

/*
 * Function:
 *	bcm_tomahawk3_init
 * Purpose:
 * 	Initialize the BCM API layer only, without resetting the switch chip.
 * Parameters:
 *	unit - StrataSwitch unit #.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tomahawk3_init(int unit)
{
    if (0 == SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    return _bcm_th3_init(unit);
}    

/*      _bcm_th3_attach
 * Purpose:
 *      Attach and initialize bcm device
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_th3_attach(int unit, char *subtype)
{
    int  dunit;
    int  rv;

    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
    dunit = BCM_CONTROL(unit)->unit;

    /* Initialize soc layer */
    if ((NULL == SOC_CONTROL(dunit)) || 
        (0 == (SOC_CONTROL(dunit)->soc_flags & SOC_F_ATTACHED))) {
        return (BCM_E_INIT);
    }
    
    if (SAL_THREAD_ERROR == SOC_CONTROL(dunit)->counter_pid) {
        rv = soc_counter_attach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Initialize bcm layer */
    BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
    BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
    BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
    if (SOC_IS_XGS_SWITCH(dunit)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_SWITCH;
    }
    if (SOC_IS_XGS_FABRIC(dunit)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_FABRIC;
    }
    if (soc_feature(dunit, soc_feature_l3)) {
        BCM_CONTROL(unit)->capability |= BCM_CAPA_L3;
    }
    if (soc_feature(dunit, soc_feature_ip_mcast)) {
        BCM_CONTROL(unit)->capability |=
            BCM_CAPA_IPMC;
    }

    /* Initialize port mappings */
    _bcm_api_xlate_port_init(unit);

    rv = _bcm_th3_init(unit);
    return (rv);
}

int
_bcm_tomahawk3_attach(int unit, char *subtype)
{
    return _bcm_th3_attach(unit, subtype);
}

/*      _bcm_th3_match
 * Purpose:
 *      match BCM control subtype strings for ESW types 
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *    0 match
 *    !0 no match
 */
int
_bcm_th3_match(int unit, char *subtype_a, char *subtype_b)
{
    COMPILER_REFERENCE(unit);
    return sal_strcmp(subtype_a, subtype_b);
}


int
_bcm_tomahawk3_match(int unit, char *subtype_a, char *subtype_b)
{
    return _bcm_th3_match(unit, subtype_a, subtype_b);
}
/*
 * Function:
 *	bcm_th3_init_selective
 * Purpose:
 * 	Initialize specific bcm modules as desired.
 * Parameters:
 *	unit - StrataSwitch unit #.
 *    module_number - Indicate module number
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_th3_init_selective(int unit, uint32 module_number)
{
    switch (module_number) {
         case BCM_MODULE_PORT     :
                   BCM_IF_ERROR_RETURN(bcm_esw_port_init(unit));
                   break;
         case BCM_MODULE_L2       :
                   BCM_IF_ERROR_RETURN(bcm_esw_l2_init(unit));
                   break;
         case BCM_MODULE_VLAN     :   
                   BCM_IF_ERROR_RETURN(bcm_esw_vlan_init(unit));
                   break;
         case BCM_MODULE_TRUNK    :
                   BCM_IF_ERROR_RETURN(bcm_esw_trunk_init(unit));
                   break;
         case BCM_MODULE_COSQ     :
                   BCM_IF_ERROR_RETURN(bcm_esw_cosq_init(unit));
                   break;
         case BCM_MODULE_MCAST        :
                   BCM_IF_ERROR_RETURN(bcm_esw_mcast_init(unit));
                   break;
         case BCM_MODULE_LINKSCAN  :
                   BCM_IF_ERROR_RETURN(bcm_esw_linkscan_init(unit));
                   break;
         case BCM_MODULE_STAT     :
                   BCM_IF_ERROR_RETURN(bcm_esw_stat_init(unit));
                   break;
         case BCM_MODULE_MIRROR   :
                   BCM_IF_ERROR_RETURN(bcm_esw_mirror_init(unit));
                   break;
#ifdef INCLUDE_L3
         case BCM_MODULE_L3       :
                   BCM_IF_ERROR_RETURN(bcm_esw_l3_init(unit));
                   break;
         case BCM_MODULE_IPMC 	:
                   BCM_IF_ERROR_RETURN(bcm_esw_ipmc_init(unit));
                   break;
#ifdef BCM_MPLS_SUPPORT
         case BCM_MODULE_MPLS	 :
                   BCM_IF_ERROR_RETURN(bcm_esw_mpls_init(unit));
                   break;
#endif /* BCM_MPLS_SUPPORT */
         case BCM_MODULE_SUBPORT	:
                   BCM_IF_ERROR_RETURN(bcm_esw_subport_init(unit));
                   break;
#endif /* INCLUDE_L3 */
         case BCM_MODULE_QOS		:
                   BCM_IF_ERROR_RETURN(bcm_esw_qos_init(unit));
                   break;
         case BCM_MODULE_STG      :
                   BCM_IF_ERROR_RETURN(bcm_esw_stg_init(unit));
                   break;
         case BCM_MODULE_TX       :
             if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                 return BCM_E_UNAVAIL;
             } else {
                   BCM_IF_ERROR_RETURN(bcm_esw_tx_init(unit));
                   break;
             }
#ifdef BCM_FIELD_SUPPORT
         case BCM_MODULE_AUTH     :
                   BCM_IF_ERROR_RETURN(bcm_esw_auth_init(unit));
                   break;
#endif
         case BCM_MODULE_RX       :
#ifdef INCLUDE_PKTIO
             if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                 BCM_IF_ERROR_RETURN(bcm_tomahawk3x_rx_init(unit));
                 break;
             } else
#endif /* INCLUDE_PKTIO */
             {
                   BCM_IF_ERROR_RETURN(bcm_esw_rx_init(unit));
                   break;
             }
         case BCM_MODULE_UDF    :
                   BCM_IF_ERROR_RETURN(bcm_esw_udf_init(unit));
                   break;
#ifdef BCM_FIELD_SUPPORT
         case BCM_MODULE_FIELD    :
                   BCM_IF_ERROR_RETURN(bcm_esw_field_init(unit));
                   break;
#endif /* BCM_FIELD_SUPPORT */
         case BCM_MODULE_TIME     :
                   BCM_IF_ERROR_RETURN(bcm_esw_time_init(unit));
                   break;
         case BCM_MODULE_POLICER  :
                   break;
#ifdef  INCLUDE_L3
         case BCM_MODULE_FAILOVER :
                   BCM_IF_ERROR_RETURN(bcm_esw_failover_init(unit));
                   break;
#endif
         case BCM_MODULE_VSWITCH  :
                   break;
#ifdef  INCLUDE_L3
         case BCM_MODULE_MULTICAST:
                   BCM_IF_ERROR_RETURN(bcm_esw_multicast_init(unit));
                   break;
         case BCM_MODULE_EXTENDER :
                   BCM_IF_ERROR_RETURN(bcm_esw_extender_init(unit));
                   break;
         case BCM_MODULE_FLOW :
                   BCM_IF_ERROR_RETURN(bcm_esw_flow_init(unit));
                   break;
#endif
#ifdef INCLUDE_PTP
         case BCM_MODULE_PTP:
                   BCM_IF_ERROR_RETURN(bcm_esw_ptp_init(unit));
                   break;
#endif 
#ifdef INCLUDE_BFD
         case BCM_MODULE_BFD:
                   BCM_IF_ERROR_RETURN(bcm_esw_bfd_init(unit));
                   break;
#endif 
#ifdef INCLUDE_REGEX
         case BCM_MODULE_REGEX :
                   BCM_IF_ERROR_RETURN(bcm_esw_regex_init(unit));
                   break;
#endif /* BCM_MPLS_SUPPORT */
#ifdef BCM_TSN_SUPPORT
         case BCM_MODULE_TSN:
                   BCM_IF_ERROR_RETURN(bcm_esw_tsn_init(unit));
                   break;
#endif /* BCM_TSN_SUPPORT */
         case BCM_MODULE_FLOWTRACKER:
                   if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
                       BCM_IF_ERROR_RETURN(bcm_tomahawk3_cosq_burst_monitor_init(unit));
                   } else {
                       BCM_IF_ERROR_RETURN(bcm_tomahawk3_flowtracker_init(unit));
                   }
                   break;
#ifdef INCLUDE_TELEMETRY
         case BCM_MODULE_TELEMETRY:
                   if (soc_feature(unit, soc_feature_telemetry)) {
                       BCM_IF_ERROR_RETURN(bcm_esw_telemetry_init(unit));
                   } else {
                       return BCM_E_UNAVAIL;
                   }
                   break;
#endif /* INCLUDE_TELEMETRY */
#ifdef INCLUDE_INT
         case BCM_MODULE_INT:
                   if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
                       BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_init(unit));
                   } else {
                       return BCM_E_UNAVAIL;
                   }
                   break;
#endif /* BCM_INT_SUPPORT */
#ifdef INCLUDE_SUM
         case BCM_MODULE_SUM:
                   if (soc_feature(unit, soc_feature_sum)) {
                       BCM_IF_ERROR_RETURN(bcm_tomahawk3_sum_init(unit));
                   } else {
                       return BCM_E_UNAVAIL;
                   }
                   break;
#endif /* INCLUDE_SUM */
         case BCM_MODULE_LATENCY_MONITOR:
                   BCM_IF_ERROR_RETURN(bcm_tomahawk3_latency_monitor_init(unit));
                   break;
#ifdef INCLUDE_PKTIO
         case BCM_MODULE_PKTIO:
             if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                 BCM_IF_ERROR_RETURN(bcm_tomahawk3x_pktio_init(unit));
                 break;
             } else {
                 return BCM_E_UNAVAIL;
             }
#endif /* INCLUDE_PKTIO */
#ifdef INCLUDE_IFA
         case BCM_MODULE_IFA:
                   if (soc_feature(unit, soc_feature_ifa)) {
                       BCM_IF_ERROR_RETURN(bcm_esw_ifa_init(unit));
                   } else {
                       return BCM_E_UNAVAIL;
                   }
                   break;
#endif /* INCLUDE_IFA */
         default:
                   return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * ************* Deprecated API ************
 * Function:
 *	bcm_esw_init_check
 * Purpose:
 *	Return TRUE if bcm_esw_init_bcm has already been called and succeeded
 * Parameters:
 *	unit- StrataSwitch unit #.
 * Returns:
 *	TRUE or FALSE
 */
int
bcm_th3_init_check(int unit)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_esw_detach
 * Purpose:
 *      Clean up bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 *      pktio_enabled - Indicating pktio is enabled,
 *                      Should clean up correspondingly.
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_th3_detach(int unit, int pktio_enabled)
{
    int rv;                    /* Operation return status. */

    /* Shut down all the spawned threads. */
    /* Threads should have been killed during the initial detach */
    if (!(BCM_CONTROL(unit)->attach_state == _bcmControlStateDeinitAll)) {
        rv = _bcm_th3_threads_shutdown(unit);
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);
    }

    /* 
     *  Don't move up, holding lock or disabling hw operations 
     *  might prevent theads clean exit.
     */
    BCM_LOCK(unit);

    rv = _bcm_th3_modules_deinit(unit, pktio_enabled);

    BCM_IF_ERROR_NOT_UNAVAIL_RETURN(rv);

    return rv;
}

int
_bcm_tomahawk3_detach(int unit)
{
    return _bcm_th3_detach(unit, 0);
}
/*
 * Function:
 *	bcm_esw_info_get
 * Purpose:
 *	Provide unit information to caller
 * Parameters:
 *	unit	- switch device
 *	info	- (OUT) bcm unit info structure
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_th3_info_get(int unit, bcm_info_t *info)
{
    uint16 dev_id = 0;
    uint8 rev_id = 0;

    if (!SOC_UNIT_VALID(unit)) {
	return BCM_E_UNIT;
    }
    if (info == NULL) {
	return BCM_E_PARAM;
    }
    soc_cm_get_id(unit, &dev_id, &rev_id);
    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = dev_id;
    info->revision = rev_id;
    info->capability = 0;
    if (SOC_IS_XGS_FABRIC(unit) || SOC_IS_XGS_FABRIC_TITAN(unit)) {
	info->capability |= BCM_INFO_FABRIC;
    } else {
	info->capability |= BCM_INFO_SWITCH;
    }
    if (soc_feature(unit, soc_feature_l3)) {
	info->capability |= BCM_INFO_L3;
    }
    if (soc_feature(unit, soc_feature_ip_mcast)) {
	info->capability |= BCM_INFO_IPMC;
    }
    info->num_pipes = NUM_PIPE(unit);
    return BCM_E_NONE;
}


/* ASSUMES unit PARAMETER which is not in macro's list. */
#define CLEAR_CALL(_rtn, _name) {                                       \
        int rv;                                                         \
        rv = (_rtn)(unit);                                              \
        if (rv < 0 && rv != BCM_E_UNAVAIL) {                            \
            LOG_ERROR(BSL_LS_BCM_INIT, \
                      (BSL_META_U(unit, \
                                  "bcm_clear %d: %s failed %d. %s\n"),    \
                                  unit, _name, rv, bcm_errmsg(rv)));              \
            return rv;                                                  \
        }                                                               \
}

/*
 * Function:
 *      bcm_tomahawk3_clear
 * Purpose:
 *      Initialize a device without a full reset
 * Parameters:
 *      unit        - The unit number of the device to clear
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      For each module, call the underlying init/clear operation
*/
int
bcm_tomahawk3_clear(int unit)
{
    return _bcm_th3_init(unit);
}

int
bcm_tomahawk3x_init(int unit)
{
    if (0 == SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    return _bcm_th3_init(unit);
}

int
_bcm_tomahawk3x_attach(int unit, char *subtype)
{
    return _bcm_th3_attach(unit, subtype);
}

int
_bcm_tomahawk3x_match(int unit, char *subtype_a, char *subtype_b)
{
    return _bcm_th3_match(unit, subtype_a, subtype_b);
}

int
_bcm_tomahawk3x_detach(int unit)
{
    return _bcm_th3_detach(unit, 1);
}
