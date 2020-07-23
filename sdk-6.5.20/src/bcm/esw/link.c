/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Link Scan
 *
 * Linkscan should always run for all chips in a system. It manages
 * the current chip LINK state (EPC_LINK or equivalent), and programs
 * MACs to match auto-negotiated links.
 *
 * Linkscan is also responsible for determining the link up/down
 * status for a port. Since link down events signaled by most PHYs are
 * latched low and cleared on read, it is important that accesses to
 * to the PHY itself be managed carefully. Linkscan is intended to be
 * the only process that reads the actual PHY (using
 * _bcm_port_link_get). All other calls to retrieve link status
 * results in calls to _bcm_link_get which returns the linkscan view
 * of the link status. This ensures linkscan is the only process that
 * reads the actual PHYs.
 *
 * All modifications to the linkscan state are protected by LC_LOCK.
 *
 * Linkscan maintains the following port bitmaps
 *
 *     lc_pbm_link:
 *                 Current physical link up/down status. When a bit
 *                 in this mask is set, a link up or link down event
 *                 is recognized and signaled to any registered
 *                 handlers.
 *
 *     lc_pbm_link_change:
 *                 Mask of ports that need to recognize a link
 *                 down event. Ports are added to this mask by the
 *                 function bcm_link_change.
 *
 *     lc_pbm_override_ports:
 *                 Bit map of ports that are currently
 *                 being explicitly set to a value. These actual value
 *                 is determined by lb_pbm_override_link. Ports are
 *                 added and removed from this mode by the routine
 *                 _bcm_link_force.
 *
 *                 Ports that are forced to an UP state do NOT result
 *                 in a call to bcm_port_update. It is the
 *                 responsibility of the caller to configure the
 *                 correct MAC and PHY state.
 *
 *     lc_pbm_override_link:
 *                 Bit map indicating the link-up/link-down
 *                 status for those ports with override set.
 *
 *     lc_pbm_sgmii_autoneg_port:
 *                 Port that is configured in SGMII autoneg mode based
 *                 on spn_PHY_SGMII_AUTONEG. Maintaining this bitmap
 *                 avoids the overhead of the soc_property_port_get call.
 *
 *     lc_pbm_linkdown_tx:
 *                 Ports configured  with linkdown TX result in
 *                 leaving the MAC enabled when link is down. This allows
 *                 packets to egress without link being established.
 *
 * Calls to _bcm_link_get always returns the current status as
 * indicated by lc_pbm_link.
 *
 * For 10G+ ports, the an additional bitmap is used:
 *
 *     lc_pbm_remote_fault:
 *                 Bit map indicating the fault (local or remote) status of
 *                 a link-up port.
 *
 * To track valid fault status, the physical link must be up and
 * the port configured so that symbols reach the MAC.  Thus, physical
 * port configuration is tracked by the lc_pbm_link state.  However,
 * if the port is receiving a fault indication, we do not want
 * to allow packet traffic.  Thus, the logical port is not yet up.
 * The logical port up is what we want to return to _bcm_link_get,
 * so we fold these two states together.  Tracking the fault
 * state requires a bit more work in the _bcm_esw_linkscan_update_port
 * logic.
 *
 * If LAG failover capability is configured through the trunk module,
 * then three more bitmaps are used:
 *
 *     lc_pbm_failover:
 *                 Bit map of ports configured for LAG failover.
 *
 *     lc_pbm_failed:
 *                 Bit map of ports configured for LAG failover which
 *                 have experienced a link down or fault and
 *                 are now in BCM_PORT_LINK_STATUS_FAILED state.  These
 *                 ports require application intervention to leave
 *                 the FAILED state.
 *
 *     lc_pbm_failed_clear:
 *                 Bit map of ports which were previously in FAILED state
 *                 but have been cleared by application intervention and
 *                 are now in DOWN state (due to link down or fault).
 *                 A port remains in this set until the next link up
 *                 without fault, at which point the hardware
 *                 sequence to remove the failover state may complete.
 *
 * Accelerated linkscan:
 *
 * The accelerated linkscan mode allows the linkscan thread to
 * run at a faster interval of time where a set of ports (lc_pbm_accel)
 * are scanned in the faster rate.
 * A port is placed under the accelerated mode under the following conditions:
 *      - Linkscan mode is HW
 *      - Fast RX LOS is enabled.  This can be enabled with the
 *        SOC property 'fast_sw_rx_los_pbmp' or the port control
 *        bcmPortControlRxFastLOS.
 *      - State is down and hw link indicates an UP state

 * For the Accelerated linkscan mode, the following bitmaps are used:
 *     lc_pbm_fast:
 *                 Bit map of ports with Fast RX LOS property enabled.
 *                 When enabled, a port may be placed under the accelerated
 *                 linkscan mode during a link state transition from
 *                 down to up.  Note that the port linkscan mode
 *                 needs to be HW.
 *     lc_pbm_accel:
 *                 Bit map of ports currently under accelerated linkscan
 *                 mode.
 */

#include <shared/bsl.h>

#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/phyctrl.h>

#include <bcm/error.h>
#include <bcm/link.h>

/* #include <bcm_int/common/link.h> */
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/common/lock.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#include <soc/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPORT */

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#include <soc/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_PREEMPTION_SUPPORT)
#include <bcm_int/esw/preemption.h>
#endif /* BCM_PREEMPTION_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <bcm_int/esw/portctrl.h>

#if defined(INCLUDE_UC_ULINK)
#include <soc/uc_msg.h>
#endif
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
#include <soc/phy/phyctrl.h>
#endif

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_led.h>
#endif

typedef struct ls_handler_s {
    struct ls_handler_s         *lh_next;
    bcm_linkscan_handler_t      lh_f;
} ls_handler_t;

typedef struct ls_errstate_s {
    int          limit;    /* # errors to enter error state */
    int          delay;    /* Length of error state in seconds */
    int          count;    /* # of errors so far */
    int          wait;     /* Boolean, TRUE when in error state */
    sal_usecs_t  time;     /* Time error state was entered */
} ls_errstate_t;

typedef struct ls_cntl_s {
    char                lc_taskname[16];
    sal_mutex_t         lc_lock;        /* Synchronization */
    pbmp_t              lc_pbm_hw;      /* Hardware link scan ports */
    pbmp_t              lc_pbm_sw;      /* Software link scan ports */
    pbmp_t              lc_pbm_sgmii_autoneg_port; /* Ports with
                                                      SGMII autoneg */
    pbmp_t              lc_pbm_info_skip; /* Ports to skip info during
                                             linkdown */
    pbmp_t              lc_pbm_fast;    /* Ports eligible for accel mode */
    pbmp_t              lc_pbm_accel;   /* Active fast link scan ports */
    int                 lc_hw_change;   /* HW Link state has changed */
    int                 lc_warm_boot;   /* A Warm Boot occured */
    ls_handler_t        *lc_handler;    /* List of handlers */
    VOL int             lc_us;          /* Time between scans (us) */
    VOL sal_thread_t    lc_thread;      /* Link scan thread */
    sal_sem_t           lc_sema;        /* Link scan semaphore */
    ls_errstate_t       lc_error[SOC_MAX_NUM_PORTS];     /* Link error state */
    bcm_linkscan_port_handler_t lc_f[SOC_MAX_NUM_PORTS]; /* Handler for
                                                            link fn */
    int                 lc_accel_us;         /* Accel mode time scan interval */
    int                 lc_accel_poll_max;   /* Max polls for accel mode */
    int                 lc_accel_poll_count; /* Accel poll count */
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
    sal_time_t          lc_up_time[SOC_MAX_NUM_PORTS];   /* Link up time*/
#endif /* BCM_HURRICANE_SUPPORT || BCM_HAWKEYE_SUPORT || BCM_TRIDENT_SUPPORT */
#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
    uint32              lc_up_proc_state[SOC_MAX_NUM_PORTS]; /* Link up procedure
                                                                status */
#endif
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32          lc_ibppktsetlimit[SOC_MAX_NUM_PORTS];   /* Link up time */
    uint32          lc_ibpcelsetlimit[SOC_MAX_NUM_PORTS];   /* Link up time */
#endif /* BCM_HAWKEYE_SUPORT */
    pbmp_t              lc_pbm_eee_pending;    /* Ports waiting for EEE update */
} ls_cntl_t;

#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
#define LC_LINK_UP_STATE_NONE  (0)
#define LC_LINK_UP_STATE_START (1)
#define LC_LINK_UP_STATE_DONE  (2)
#endif

/*
 * Define:
 *     LC_LOCK/LC_UNLOCK
 * Purpose:
 *     Serialization Macros for access to lc_cntl structure.
 */

#define LC_LOCK(unit) \
        if(SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit) || \
           SOC_IS_TRIDENT(unit) || SOC_IS_TD2_TT2(unit) || \
           SOC_IS_KATANA2(unit) || SOC_IS_HURRICANEX(unit) || \
           SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) { \
            /* To avoid deadlock between port and linkscan mutexes */ \
            BCM_LOCK(unit); \
        } \
        sal_mutex_take(link_control[unit]->lc_lock, sal_mutex_FOREVER);

#define LC_UNLOCK(unit) \
        sal_mutex_give(link_control[unit]->lc_lock); \
        if(SOC_IS_TRIUMPH3(unit) || SOC_IS_SABER2(unit) || \
           SOC_IS_TRIDENT(unit) || SOC_IS_TD2_TT2(unit) || \
           SOC_IS_KATANA2(unit) || SOC_IS_HURRICANEX(unit) || \
           SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) { \
            BCM_UNLOCK(unit); \
        }

#define LC_CHECK_INIT(unit) \
        if (link_control[unit] == NULL) { \
            return(BCM_E_INIT); \
        }

/* Define this if you want to know the time take to prcess link up/down event.
 * This is *not* the time between actual HW link up/down and software link
 * up/down event.
 */
#define BCM_LINK_CHANGE_BENCHMARK

/* Return On error with link mutex unlocked */
#define _BCM_LINK_IF_ERROR_RETURN(op)  \
    do {                                        \
        int __rv__;                             \
        if (((__rv__ = (op)) < 0)) {            \
            LC_UNLOCK(unit);                    \
            return(__rv__);                     \
        }                                       \
    } while(0)


/*
 * Variable:
 *     link_control
 * Purpose:
 *     Hold per-unit global status for linkscan
 */

STATIC ls_cntl_t       *link_control[BCM_MAX_NUM_UNITS];

#if defined(BCM_HAWKEYE_SUPPORT)
#define PHY_AN_TX_PAUSE_CAP    (0x01)
#define PHY_AN_RX_PAUSE_CAP    (0x02)
#define HK_PAUSE_WAR_PHY_REG   (0x19)
#endif /* BCM_HAWKEYE_SUPPORT */

/*
 * Accelerated Linkscan Defaults
 * - Time interval during fast/accelerated mode.
 * - Max number of accelerated interval polls before clearing
 *   the accelerated bitmap.
 */
#define ACCELERATED_INTERVAL_USEC     100000 /* 100 msecs */
#define ACCELERATED_POLL_COUNT_MAX    10

static sal_usecs_t last_time_hw_link_up[BCM_MAX_NUM_UNITS];  /* Debug */
#ifdef BCM_TRIUMPH3_SUPPORT
extern int _bcm_esw_ibod_syn_port_down(int unit, int port);
#endif

#if defined(INCLUDE_UC_ULINK)
uint8 soc_cmic_uc_link_change_flag;
#endif

/*
 * Function:
 *      _bcm_esw_link_port_info_skip_set
 * Purpose:
 *      Adds/Removes given port to skip_info pbmp to indicate if
 *      skip port_info_get
 *      during link down event notification or not
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      gport - Port to set/remove
 *      skip - Identificator to set or remove
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_link_port_info_skip_set(int unit, bcm_port_t port, int skip)
{
    ls_cntl_t   *lc = link_control[unit];
    bcm_port_t  local_port;

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }
    if (!SOC_PORT_VALID(unit, local_port) || !IS_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (skip) {
        BCM_PBMP_PORT_ADD(lc->lc_pbm_info_skip, local_port);
    } else {
        BCM_PBMP_PORT_REMOVE(lc->lc_pbm_info_skip, local_port);
    }
    LC_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_esw_link_port_info_skip_get
 * Purpose:
 *      Get status of given port to indicate if to skip port_info_get
 *      during link down event notification or not
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      gport - Port to set/remove
 *      skip - [OUT] Identificator if set or not
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_link_port_info_skip_get(int unit, bcm_port_t port, int *skip)
{
    ls_cntl_t   *lc = link_control[unit];
    bcm_port_t  local_port;

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &local_port));
    } else {
        local_port = port;
    }
    if (!SOC_PORT_VALID(unit, local_port) || !IS_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }
    if (!skip) {
        return BCM_E_PARAM;
    }

    *skip = (BCM_PBMP_MEMBER(lc->lc_pbm_info_skip, local_port)) ? 1 : 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _lc_pbm_init
 * Purpose:
 *      Initialize the port bitmaps in the link_control structure
 * Parameters:
 *      unit - StrataSwitch unit #.
 */
static void
_lc_pbm_init(int unit)
{
    soc_persist_t *sop = SOC_PERSIST(unit);

    /*
     * Force initial scan by setting link change while pretending link
     * was initially up.
     */
    BCM_PBMP_ASSIGN(sop->lc_pbm_link, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        (void)_bcm_kt2_flexio_pbmp_update(unit, &sop->lc_pbm_link);
    }
#endif

       /* If we set sop->lc_pbm_link_change as PBMP_PORT_ALL(unit),
        * MAC of all ports will be reset, and traffic will be disrupted.
        */
       /* BCM_PBMP_ASSIGN(sop->lc_pbm_link_change, PBMP_PORT_ALL(unit)); */
    BCM_PBMP_CLEAR(sop->lc_pbm_override_ports);
    BCM_PBMP_CLEAR(sop->lc_pbm_override_link);
    BCM_PBMP_CLEAR(sop->lc_pbm_remote_fault);
    BCM_PBMP_CLEAR(sop->lc_pbm_failover);
    BCM_PBMP_CLEAR(sop->lc_pbm_failed);
    BCM_PBMP_CLEAR(sop->lc_pbm_failed_clear);
    BCM_PBMP_CLEAR(sop->lc_pbm_linkdown_tx);
    BCM_PBMP_CLEAR(sop->lc_pbm_roe_linkdown_tx);
    BCM_PBMP_CLEAR(sop->lc_pbm_roe_linkdown_rx);
    BCM_PBMP_CLEAR(sop->lc_pbm_fc);
    BCM_PBMP_CLEAR(sop->lc_pbm_eth_buffer_mode);

#ifdef BCM_RCPU_SUPPORT
    if (SOC_IS_RCPU_ONLY(unit) && SOC_PORT_VALID(unit, RCPU_PORT(unit))) {
        /* Do not update link status for rcpu port. */
        SOC_PBMP_PORT_ADD(sop->lc_pbm_override_ports, RCPU_PORT(unit));
        SOC_PBMP_PORT_ADD(sop->lc_pbm_override_link, RCPU_PORT(unit));
    }
#endif /* BCM_RCPU_SUPPORT */

}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_6

/*
 * Number of pbmp's to save:
 *      From ls_ctrl_t above - 3:
 *              lc_pbm_hw
 *              lc_pbm_sw
 *              lc_pbm_info_skip
 *
 *     From soc_persist_t - 8:
 *              link_fwd
 *              lc_pbm_override_ports
 *              lc_pbm_override_link
 *              lc_pbm_linkdown_tx
 *              lc_pbm_remote_fault
 *              lc_pbm_failover
 *              lc_pbm_failed
 *              lc_pbm_failed_clear
 *              lc_pbm_roe_linkdown_tx
 *              lc_pbm_roe_linkdown_rx
 *
 *     Constructed in sync - 1:
 *              disabled_pbmp   - bitmap of disabled PHYs (internal state)
 *
 * If any are added, that must be updated here.
 *
 * Note that lc_pbm_link and lc_pbm_link_change are not included, since
 * they should be derived from the current HW state, not that existing
 * at the save point.
 */

#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_0 12
#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_1 13
#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_2 14
#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_3 15
#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_4 16
#define BCM_LINK_PBMP_SCACHE_NUM_WB_1_5 18

#define BCM_LINK_PBMP_SCACHE_NUM        BCM_LINK_PBMP_SCACHE_NUM_WB_1_5


#define _LINK_PBMP_BYTE_MAX      (SOC_PBMP_WORD_MAX * sizeof(uint32))

#define _BCM_LINK_PBMP_SCACHE_COPY(_pbmp_) \
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) { \
        fldbuf[i] = SOC_PBMP_WORD_GET(_pbmp_, i); \
    } \
    sal_memcpy(link_scache_ptr, fldbuf, _LINK_PBMP_BYTE_MAX); \
    link_scache_ptr += _LINK_PBMP_BYTE_MAX;

#define _BCM_LINK_PBMP_SCACHE_RECOVER(_pbmp_) \
    sal_memset(fldbuf, 0, _LINK_PBMP_BYTE_MAX); \
    sal_memcpy(fldbuf, link_scache_ptr, _LINK_PBMP_BYTE_MAX); \
    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) { \
        SOC_PBMP_WORD_SET(_pbmp_, i, fldbuf[i]); \
    } \
    link_scache_ptr += _LINK_PBMP_BYTE_MAX;

/*
 * Function:
 *      _bcm_esw_link_sync
 * Purpose:
 *      Record Link module persisitent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *      Fields for previous version to be documented
 *  BCM_WB_VERSION_1_0
 *      ...
 *  BCM_WB_VERSION_1_1
 *      ...
 *  BCM_WB_VERSION_1_2
 *    lc_pbm_fast      Port bitmap for fast linkscan property (accel mode)
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_link_sync(int unit)
{
    int                 alloc_size;
    soc_scache_handle_t scache_handle;
    uint8               *link_scache, *link_scache_ptr;
    ls_cntl_t           *lc = link_control[unit];
    soc_persist_t       *sop = SOC_PERSIST(unit);
    uint32              fldbuf[SOC_PBMP_WORD_MAX];
    int                 i, enable;
    bcm_port_t          port;
    bcm_pbmp_t          pbmp, disabled_pbmp;
    int                 lc_us;

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_LINKSCAN, 0);

    alloc_size = BCM_LINK_PBMP_SCACHE_NUM * _LINK_PBMP_BYTE_MAX;

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
        /* For lc_ibppktsetlimit and lc_ibpcelsetlimit */
        alloc_size += (SOC_MAX_NUM_PORTS * 2 * sizeof(uint32));
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    /* For lc_us */
    alloc_size += sizeof(int);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size,
                                 &link_scache, BCM_WB_DEFAULT_VERSION, NULL));

    link_scache_ptr = link_scache;

    BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
    SOC_PBMP_CLEAR(disabled_pbmp);

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)){
        int oamp_enable;
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
        /* Skip OAMP port for saber2 device */
        if(oamp_enable) {
            SOC_PBMP_PORT_REMOVE(pbmp,
                    _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER);
        }
    }
#endif

    PBMP_ITER(pbmp, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_enable_get(unit, port, &enable));
        if (!enable) {
            SOC_PBMP_PORT_ADD(disabled_pbmp, port);
        }
    }

    /* Copy out the required persistent PBMP's */
    _BCM_LINK_PBMP_SCACHE_COPY(lc->lc_pbm_hw);
    _BCM_LINK_PBMP_SCACHE_COPY(lc->lc_pbm_sw);
    _BCM_LINK_PBMP_SCACHE_COPY(lc->lc_pbm_info_skip);

    _BCM_LINK_PBMP_SCACHE_COPY(sop->link_fwd);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_override_ports);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_override_link);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_linkdown_tx);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_remote_fault);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_failover);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_failed);
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_failed_clear);
    _BCM_LINK_PBMP_SCACHE_COPY(disabled_pbmp);

    /* Introduced fc bitmap sync/recovery with WB ver 1.1 */
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_fc);

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
        for (i = 0 ; i < SOC_MAX_NUM_PORTS ; i++) {
            sal_memcpy(link_scache_ptr,
                       &(lc->lc_ibppktsetlimit[i]), sizeof(uint32));
            link_scache_ptr += sizeof(uint32);
            sal_memcpy(link_scache_ptr,
                       &(lc->lc_ibpcelsetlimit[i]), sizeof(uint32));
            link_scache_ptr += sizeof(uint32);
        }
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    /* Introduced accel mode bitmap sync/recovery with WB ver 1.2 */
    _BCM_LINK_PBMP_SCACHE_COPY(lc->lc_pbm_fast);

    lc_us = lc->lc_us;
    sal_memcpy(link_scache_ptr, &(lc_us), sizeof(int));
    link_scache_ptr += sizeof(int);

    /* Introduced ethernet buffer mode port bitmap sync/recovery with WB ver 1.4 */
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_eth_buffer_mode);

    /* Introduced EEE update pending port bitmap sync/recovery with WB ver 1.5 */
    _BCM_LINK_PBMP_SCACHE_COPY(lc->lc_pbm_eee_pending);

    /* Introduced Roe tx WB ver 1.6 */
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_roe_linkdown_tx);
    /* Introduced Roe rx WB ver 1.6 */
    _BCM_LINK_PBMP_SCACHE_COPY(sop->lc_pbm_roe_linkdown_rx);

    return BCM_E_NONE;
}

STATIC int
_bcm_esw_link_reload(int unit)
{
    soc_port_t          port;
    int                 loopback;
    int                 alloc_size;
    int                 realloc_size = 0;
    uint16              recovered_ver;
    soc_scache_handle_t scache_handle;
    uint8               *link_scache, *link_scache_ptr;
    ls_cntl_t           *lc = link_control[unit];
    soc_persist_t       *sop = SOC_PERSIST(unit);
    uint32              fldbuf[SOC_PBMP_WORD_MAX];
    int                 i, rv, link=0;
    bcm_pbmp_t          pbmp_port, disabled_pbmp;
    int                 lc_us;

    /* Only toggle if we need to */
    SOC_PBMP_CLEAR(sop->lc_pbm_link_change);

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit, BCM_MODULE_LINKSCAN, 0);

    alloc_size = BCM_LINK_PBMP_SCACHE_NUM * _LINK_PBMP_BYTE_MAX;

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
        /* For lc_ibppktsetlimit and lc_ibpcelsetlimit */
        alloc_size += (SOC_MAX_NUM_PORTS * 2 * sizeof(uint32));
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    SOC_PBMP_ASSIGN(pbmp_port, PBMP_PORT_ALL(unit));
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)){
        int oamp_enable;
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
        /* Skip OAMP port for saber2 device */
        if(oamp_enable) {
            SOC_PBMP_PORT_REMOVE(pbmp_port,
                    _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER);
        }
    }
#endif

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size, &link_scache,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_SUCCESS(rv)) {
        link_scache_ptr = link_scache;

        /* Recover out the required persistent PBMP's */
        _BCM_LINK_PBMP_SCACHE_RECOVER(lc->lc_pbm_hw);
        _BCM_LINK_PBMP_SCACHE_RECOVER(lc->lc_pbm_sw);
        _BCM_LINK_PBMP_SCACHE_RECOVER(lc->lc_pbm_info_skip);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->link_fwd);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_override_ports);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_override_link);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_linkdown_tx);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_remote_fault);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_failover);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_failed);
        _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_failed_clear);
        _BCM_LINK_PBMP_SCACHE_RECOVER(disabled_pbmp);

         /* mask2 needs to be restored before using it in  */
        BCM_PBMP_ASSIGN(SOC_CONTROL(unit)->link_mask2, sop->link_fwd);

        /* Introduced fc bitmap sync/recovery with WB ver 1.1 */
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
            _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_fc);
        } else {
            /*
             * Allocate extra scache space to recover fc bitmap for
             * syncing this info to scache in subsequent warm reloads
             */
            realloc_size += ((BCM_LINK_PBMP_SCACHE_NUM_WB_1_1 -
                              BCM_LINK_PBMP_SCACHE_NUM_WB_1_0) *
                             _LINK_PBMP_BYTE_MAX);
        }
#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
            for (i = 0 ; i < SOC_MAX_NUM_PORTS ; i++) {
                sal_memcpy(&(lc->lc_ibppktsetlimit[i]),
                           link_scache_ptr, sizeof(uint32));
                link_scache_ptr += sizeof(uint32);
                sal_memcpy(&(lc->lc_ibpcelsetlimit[i]),
                           link_scache_ptr, sizeof(uint32));
                link_scache_ptr += sizeof(uint32);
            }
        }
#endif /* BCM_HAWKEYE_SUPPORT */

        /* Introduced accel mode bitmap sync/recovery with WB ver 1.2 */
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            _BCM_LINK_PBMP_SCACHE_RECOVER(lc->lc_pbm_fast);
        } else {
            realloc_size += ((BCM_LINK_PBMP_SCACHE_NUM_WB_1_2 -
                              BCM_LINK_PBMP_SCACHE_NUM_WB_1_1) *
                             _LINK_PBMP_BYTE_MAX);
        }

        if (recovered_ver >= BCM_WB_VERSION_1_3) {
            sal_memcpy(&lc_us, link_scache_ptr, sizeof(int));
            link_scache_ptr += sizeof(int);
            lc->lc_us = lc_us;
        } else {
            realloc_size += sizeof(int);
        }

        /* Introduced eth buffer_mode bitmap sync/recovery with WB ver 1.4 */
        if (recovered_ver >= BCM_WB_VERSION_1_4) {
            _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_eth_buffer_mode);
        } else {
            realloc_size += ((BCM_LINK_PBMP_SCACHE_NUM_WB_1_3 -
                              BCM_LINK_PBMP_SCACHE_NUM_WB_1_2) *
                             _LINK_PBMP_BYTE_MAX);
        }


        /* Introduced EEE update pending port bitmap sync/recovery with WB ver 1.5 */
        if (recovered_ver >= BCM_WB_VERSION_1_5) {
            _BCM_LINK_PBMP_SCACHE_RECOVER(lc->lc_pbm_eee_pending);
        } else {
            realloc_size += ((BCM_LINK_PBMP_SCACHE_NUM_WB_1_4 -
                              BCM_LINK_PBMP_SCACHE_NUM_WB_1_3) *
                             _LINK_PBMP_BYTE_MAX);
        }
        if (recovered_ver >= BCM_WB_VERSION_1_6) {
            _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_roe_linkdown_tx);
            _BCM_LINK_PBMP_SCACHE_RECOVER(sop->lc_pbm_roe_linkdown_rx);
        } else {
            realloc_size += ((BCM_LINK_PBMP_SCACHE_NUM_WB_1_5 -
                              BCM_LINK_PBMP_SCACHE_NUM_WB_1_4) *
                             _LINK_PBMP_BYTE_MAX);
        }

        /*
         * Allocate extra scache space
         */
        if (realloc_size > 0) {
            SOC_IF_ERROR_RETURN
                (soc_scache_realloc(unit, scache_handle, realloc_size));
        }

        /* Recover port enable state */
        PBMP_ITER(pbmp_port, port) {
            if (SOC_PBMP_MEMBER(disabled_pbmp, port)) {
                /* MAC reg writes are suppressed, but this will set
                 * the SW PHY flag as disabled. */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_enable_set(unit, port, FALSE));
            }
        }

        /* Best guess, based on link foward state (subset of up ports) */
        SOC_PBMP_ASSIGN(sop->lc_pbm_link, sop->link_fwd);
        /* Also remote fault, since link must be up to detect it */
        SOC_PBMP_OR(sop->lc_pbm_link, sop->lc_pbm_remote_fault);
        /* Already in fault, toggle link status to see if it's cleared */
        SOC_PBMP_OR(sop->lc_pbm_link_change, sop->lc_pbm_remote_fault);
    } else if (BCM_E_NOT_FOUND == rv) {
        /* Can't recover forced ports without persistent storage,
         * but we can check for ports in loopback and put them as
         * forced with link up.
         */
        PBMP_ITER(pbmp_port, port) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_loopback_get(unit, port, &loopback));
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                        (BSL_META_U(unit,
                                    "Linkscan init loopback recovery, "
                                    "unit %d, port %d, lb=%d\n"),
                         unit, port, loopback));
            if (loopback != BCM_PORT_LOOPBACK_NONE) {
                SOC_PBMP_PORT_ADD(sop->lc_pbm_override_link, port);
                SOC_PBMP_PORT_ADD(sop->lc_pbm_override_ports, port);
            }
        }
        /*
         * Force initial scan by setting link change while pretending link
         * was initially up.  We'll pull out the expected link up ports
         * below.
         */
        SOC_PBMP_ASSIGN(sop->lc_pbm_link, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
            (void)_bcm_kt2_flexio_pbmp_update(unit, &sop->lc_pbm_link);
        }
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)){
        int oamp_enable;
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
        /* Skip OAMP port for saber2 device */
        if(oamp_enable) {
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link,
                    _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER);
        }
    }
#endif
    } else {
        return rv;
    }

    PBMP_ITER(pbmp_port, port) {
        if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port)) {
            continue;
        }

        /* Retrieve link status from CMIC_MIIM_LINK_STATUS register for
         * ports configured in Hardware Linkscan mode and from LATCH_DOWN
         * register in Software Linkscan mode. This approach ensures that
         * Link status is reliably retrieved as Latch down status register
         * is getting cleared when toggling ports in Hardware linkscan mode
         */
        /* link variable is filled up here, so it is valid */
        if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, port)) {
            /* coverity[uninit_use_in_call] : FALSE */
            /*
             * COVERITY
             *
             * The iterator above PBMP_ITER will protect the port variable
             * from overrun.
             */
            /* coverity[overrun-call] */
            BCM_IF_ERROR_RETURN
                   (_bcm_port_link_get(unit, port, 1, &link));
        } else if (SOC_PBMP_MEMBER(lc->lc_pbm_sw, port)) {
             /* coverity[uninit_use_in_call] : FALSE */
             /*
             * COVERITY
             * The iterator above PBMP_ITER will protect the port variable
             * from overrun.
             */
            /* coverity[overrun-call] */
            BCM_IF_ERROR_RETURN
                (_bcm_port_link_get(unit, port, 0, &link));
        }
        if ((!link && SOC_PBMP_MEMBER(sop->lc_pbm_link, port)) ||
            (link && !SOC_PBMP_MEMBER(sop->lc_pbm_link, port))) {
            /* Link status changed from expectation, process */
            SOC_PBMP_PORT_ADD(sop->lc_pbm_link_change, port);
        }
    }

    /* Enable loopback on disabled port */
    BCM_IF_ERROR_RETURN
            (_bcm_esw_link_force_for_disabled_loopback_port(unit));


#ifdef BCM_CMICX_SUPPORT
    /* Recovery LED software override feature. */
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_led_link_status_t led_link_sts;
        led_link_sts.link_sts = 1;
        BCM_PBMP_ASSIGN(pbmp_port, sop->lc_pbm_link);
        BCM_PBMP_OR(pbmp_port, sop->lc_pbm_override_link);

        PBMP_ITER(pbmp_port, port) {
            led_link_sts.port = port;
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                led_link_sts.port = SOC_INFO(unit).port_l2p_mapping[port];
            }
            soc_cmicx_led_link_status(unit, &led_link_sts);
        }
    }
#endif /* BCM_CMICX_SUPPORT */

    /* Note Warm Boot occurred for linkscan thread start */
    lc->lc_warm_boot = TRUE;

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_esw_linkscan_enable_port_get
 * Purpose:
 *      Determine whether or not linkscan is managing a given port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to check.
 * Returns:
 *      BCM_E_NONE - port being scanned
 *      BCM_E_DISABLED - port not being scanned
 */
int
bcm_esw_linkscan_enable_port_get(int unit, bcm_port_t port)
{
    soc_persist_t       *sop = SOC_PERSIST(unit);
    ls_cntl_t           *lc = link_control[unit];

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    if (lc == NULL || lc->lc_us == 0 ||
        (!BCM_PBMP_MEMBER(lc->lc_pbm_sw, port) &&
         !BCM_PBMP_MEMBER(lc->lc_pbm_hw, port) &&
         !BCM_PBMP_MEMBER(sop->lc_pbm_override_ports, port))) {
        return (BCM_E_DISABLED);
    }

    return (BCM_E_NONE);
}

#if defined(BCM_TRIDENT_SUPPORT)
/*
 * Function:
 *      _bcm_trident_linkscan_update_asf
 * Purpose:
 *      Update Alternate Store and Forward parameters for a port on Trident.
 * Parameters:
 *      unit - device
 *      port - port
 *      linkup - port link state (0=down, 1=up)
 *      speed - port speed
 *      duplex - port duplex (0=half, 1=full)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This routine is the same as _bcm_esw_linkscan_update_asf but Trident
 *      has a different mapping table for speeds.
 */
STATIC int
_bcm_trident_linkscan_update_asf(int unit, bcm_port_t port, int linkup,
                                 int speed, int duplex)
{
    int no_10_100, asf_speed;

    if (!linkup || IS_CPU_PORT(unit, port)) {
        speed = 0;
    }
    no_10_100 = soc_feature(unit, soc_feature_asf_no_10_100);
    switch (speed) {
    case 0:
        asf_speed = 0;
        break;
    case 10:
        if (no_10_100) {
            asf_speed = 0;
        } else {
            asf_speed = duplex ? 3 : 2;
        }
        break;
    case 100:
        if (no_10_100) {
            asf_speed = 0;
        } else {
            asf_speed = duplex ? 5 : 4;
        }
        break;
    case 1000:
        asf_speed = duplex ? 7 : 6;
        break;
    case 2500:
        asf_speed = duplex ? 9 : 8;
        break;
    case 10000:
        asf_speed = 12;
        break;
    case 12000:
        asf_speed = 13;
        break;
    case 13000:
        asf_speed = 14;
        break;
    case 15000:
        asf_speed = 15;
        break;
    case 16000:
        asf_speed = 16;
        break;
    case 20000:
        asf_speed = 18;
        break;
    case 24000:
        asf_speed = 20;
        break;
    case 30000:
        asf_speed = 24;
        break;
    case 40000:
        asf_speed = 28;
        break;
    default:
        asf_speed = 0;
        break;
    }

    return soc_reg_field32_modify(unit, ASF_PORT_SPEEDr, port,
                                  ASF_PORT_SPEEDf, asf_speed);
}
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * Function:
 *      _bcm_gh2_linkscan_update_asf
 * Purpose:
 *      Update Alternate Store and Forward destination ports.
 * Parameters:
 *      unit - device
 *      port - port
 *      linkup - port link state (0=down, 1=up)
 *      speed - port speed
 *      duplex - port duplex (0=half, 1=full)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_gh2_linkscan_update_asf(int unit, bcm_port_t port, int linkup,
                                 int speed, int duplex)
{
    int dst_port;
    int dst_port_speed;
    int dst_mmu_port = 0;
    bcm_pbmp_t pbmp;
    uint32 old_val0, old_val1, old_val2;
    uint32 new_val0, new_val1, new_val2;
    soc_info_t *si = &SOC_INFO(unit);
    int rv = BCM_E_NONE;

    if (IS_CPU_PORT(unit, port)) {
        /* Cut-through mode not supported on CPU port, skip it */
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        READ_ASF_PORT_SPEED_MATCH_BMP0r(unit, port, &old_val0));
    BCM_IF_ERROR_RETURN(
        READ_ASF_PORT_SPEED_MATCH_BMP1r(unit, port, &old_val1));
    BCM_IF_ERROR_RETURN(
        READ_ASF_PORT_SPEED_MATCH_BMP2r(unit, port, &old_val2));

    SOC_PBMP_ASSIGN(pbmp, PBMP_E_ALL(unit));
    new_val0 = 0;
    new_val1 = 0;
    new_val2 = 0;
    PBMP_ITER(pbmp, dst_port) {
        rv = bcm_esw_port_speed_get(unit, dst_port, &dst_port_speed);
        if (BCM_SUCCESS(rv)) {
            if (dst_port_speed <= speed) {
                dst_mmu_port = \
                    si->port_p2m_mapping[si->port_l2p_mapping[dst_port]];
                if (dst_mmu_port < 32) {
                    new_val0 |= 1 << (dst_mmu_port % 32);
                } else if ((dst_mmu_port >=32) && (dst_mmu_port < 64)) {
                    new_val1 |= 1 << (dst_mmu_port % 32);
                } else { /* dst_mmu_port >= 64 */
                    new_val2 |= 1 << (dst_mmu_port % 32);
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                        (BSL_META_U(unit,
                                    "ASF is not set on inactive "
                                    "destination port %d\n"),
                         port));
        }
    }
    if (old_val0 != new_val0) {
        BCM_IF_ERROR_RETURN(
            WRITE_ASF_PORT_SPEED_MATCH_BMP0r(unit, port, new_val0));
    }
    if (old_val1 != new_val1) {
        BCM_IF_ERROR_RETURN(
            WRITE_ASF_PORT_SPEED_MATCH_BMP1r(unit, port, new_val1));
    }
    if (old_val2 != new_val2) {
        BCM_IF_ERROR_RETURN(
            WRITE_ASF_PORT_SPEED_MATCH_BMP2r(unit, port, new_val2));
    }
    return BCM_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT */

/*
 * Function:
 *      _bcm_esw_linkscan_update_asf
 * Purpose:
 *      Update Alternate Store and Forward parameters for a port.
 * Parameters:
 *      unit - device
 *      port - port
 *      linkup - port link state (0=down, 1=up)
 *      speed - port speed
 *      duplex - port duplex (0=half, 1=full)
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_linkscan_update_asf(int unit, bcm_port_t port, int linkup,
                             int speed, int duplex)
{
    if (SOC_REG_IS_VALID(unit, ASFPORTSPEEDr)) {
        uint32 asf_reg, oasf_reg, asf_speed;
        int no_10_100;

        SOC_IF_ERROR_RETURN(READ_ASFPORTSPEEDr(unit, port, &asf_reg));
        oasf_reg = asf_reg;
        if (!linkup || IS_CPU_PORT(unit, port)) {
            speed = 0;
        }
        no_10_100 = soc_feature(unit, soc_feature_asf_no_10_100);
        switch (speed) {
        case 0:
            asf_speed = 0;
            break;
        case 10:
            if (no_10_100) {
                asf_speed = 0;
            } else {
                asf_speed = duplex ? 2 : 1;
            }
            break;
        case 100:
            if (no_10_100) {
                asf_speed = 0;
            } else {
                asf_speed = duplex ? 4 : 3;
            }
            break;
        case 1000:
            asf_speed = 5;
            break;
        case 10000:
            asf_speed = 6;
            break;
        case 12000:
            asf_speed = 7;
            break;
        default:
            asf_speed = 0;
            break;
        }
        soc_reg_field_set(unit, ASFPORTSPEEDr, &asf_reg,
                          PORTSPEEDf, asf_speed);
        if (asf_reg != oasf_reg) {
            SOC_IF_ERROR_RETURN(WRITE_ASFPORTSPEEDr(unit, port, asf_reg));
        }
        return BCM_E_NONE;
    }
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    /* Trident has a very different speed to ASF mapping */
    /* coverity[result_independent_of_operands] */
    if (SOC_REG_IS_VALID(unit, ASF_PORT_SPEEDr) && !SOC_IS_TD_TT(unit)) {
        uint32 asf_reg, oasf_reg, asf_speed;
        int no_10_100;

        SOC_IF_ERROR_RETURN(READ_ASF_PORT_SPEEDr(unit, port, &asf_reg));
        oasf_reg = asf_reg;
        if (!linkup || IS_CPU_PORT(unit, port)) {
            speed = 0;
        }
        no_10_100 = soc_feature(unit, soc_feature_asf_no_10_100);
        switch (speed) {
        case 0:
            asf_speed = 0;
            break;
        case 10:
            if (no_10_100) {
                asf_speed = 0;
            } else {
                asf_speed = duplex ? 3 : 2;
            }
            break;
        case 100:
            if (no_10_100) {
                asf_speed = 0;
            } else {
                asf_speed = duplex ? 5 : 4;
            }
            break;
        case 1000:
            asf_speed = duplex ? 7 : 6;
            break;
        case 2500:
            asf_speed = duplex ? 9 : 8;
            break;
        case 10000:
            asf_speed = 21;
            break;
        case 12000:
        case 13000:
            asf_speed = 25;
            break;
        case 16000:
            asf_speed = 27;
            break;
        case 20000:
        case 21000:
            asf_speed = 29;
            break;
        default:
            asf_speed = 0;
            break;
        }
        soc_reg_field_set(unit, ASF_PORT_SPEEDr, &asf_reg,
                          ASF_PORT_SPEEDf, asf_speed);
        if (asf_reg != oasf_reg) {
            SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_SPEEDr(unit, port, asf_reg));
        }
        return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    /* ASF_PORT_SPEED register should always be there for Trident */
    /* this check is to protect against some future variants to avoid crashes */
    /* coverity[result_independent_of_operands] */
    if (SOC_IS_TD_TT(unit) && SOC_REG_IS_VALID(unit, ASF_PORT_SPEEDr) ) {
        return _bcm_trident_linkscan_update_asf(unit, port, linkup,
                                                speed, duplex);
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        return _bcm_gh2_linkscan_update_asf(unit, port, linkup,
                                            speed, duplex);
    }
#endif
    return BCM_E_NONE;
}

#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
/*
 * Function:
 *      _bcm_esw_link_fault_get
 * Purpose:
 *      Check for remote and local fault on 10G+ link which is already up.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 *      fault - (IN/OUT) Fault status
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_link_fault_get(int unit, int port, int *fault)
{
    uint64 lss;
    int rmt_fault, lcl_fault, speed;
    soc_reg_t reg;
    soc_field_t rmt_fault_field, lcl_fault_field;
    soc_reg_t clr_reg;
    soc_persist_t *sop = SOC_PERSIST(unit);
    bcm_port_ability_t  port_ability;
    int ln_rmt_fault = 0;
    int ln_lcl_fault = 0;
    int en = 0;

    /* Don't check fault on disabled port. */
    BCM_IF_ERROR_RETURN(bcm_esw_port_enable_get(unit, port, &en));
    if (en == 0) {
        *fault = FALSE;
        return BCM_E_NONE;
    }

    if (SOC_PBMP_MEMBER(sop->lc_pbm_eth_buffer_mode, port)) {
        *fault = FALSE;
        if (!IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_phyctrl_linkfault_get(unit, port, fault));
        }
        if (*fault & BCM_PORT_FAULT_LOCAL) {
            ln_lcl_fault = TRUE;
        }
        if (*fault & BCM_PORT_FAULT_REMOTE) {
            ln_rmt_fault = TRUE;
        }
        if (ln_lcl_fault || ln_rmt_fault) {
            *fault = TRUE;
        }
    }

    if (SOC_PBMP_MEMBER(sop->lc_pbm_fc, port)) {
        if (!(SOC_PORT_USE_PORTCTRL(unit, port))) {
            SOC_IF_ERROR_RETURN(soc_phyctrl_linkfault_get(unit, port, fault));
                return BCM_E_NONE;
            }
        }

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_CE_PORT(unit, port) || IS_CDE_PORT(unit, port)) {

        sal_memset(&port_ability, 0, sizeof(bcm_port_ability_t));
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &speed));

        /* CD PORTS (PM8x50) do not support the below check */
        if (!IS_CD_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_ability_local_get(unit,
                port, &port_ability));

        /* check if the port has no fault status to retrieve */
        if (port_ability.speed_full_duplex & BCM_PORT_ABILITY_5000MB &&
                speed < 5000) {
            return BCM_E_NONE;
        } else if (speed < 5000) {
            return BCM_E_NONE;
        }
        }

        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
            BCM_IF_ERROR_NOT_UNAVAIL_RETURN
                (bcmi_esw_portctrl_link_fault_get(unit, port,
                                                  &lcl_fault, &rmt_fault));
            if (rmt_fault || lcl_fault) {
                *fault = TRUE;
            }
            bcmi_esw_portctrl_clear_rx_lss_status_set(unit, port, 1, 1);
            bcmi_esw_portctrl_clear_rx_lss_status_set(unit, port, 0, 0);
            return BCM_E_NONE;
        }

        clr_reg = INVALIDr;
        if (soc_feature(unit, soc_feature_clmac) &&
            (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port))) {
            /* Tomahawk Front panel port is using CL MAC - thus we should check CL port instead of speed here */
            reg = CLMAC_RX_LSS_STATUSr;
            rmt_fault_field = REMOTE_FAULT_STATUSf;
            lcl_fault_field = LOCAL_FAULT_STATUSf;
            clr_reg = CLMAC_CLEAR_RX_LSS_STATUSr;
        } else if (soc_feature(unit, soc_feature_cmac) &&
            IS_CL_PORT(unit, port) &&
            (IS_CE_PORT(unit, port) ||
             SOC_INFO(unit).port_speed_max[port] >= 100000)) {
            reg = CMAC_RX_LSS_STATUSr;
            rmt_fault_field = REMOTE_FAULT_STATUSf;
            lcl_fault_field = LOCAL_FAULT_STATUSf;
            clr_reg = CMAC_CLEAR_RX_LSS_STATUSr;
        } else if (soc_feature(unit, soc_feature_xmac)) {
            reg = XMAC_RX_LSS_STATUSr;
            rmt_fault_field = REMOTE_FAULT_STATUSf;
            lcl_fault_field = LOCAL_FAULT_STATUSf;
            clr_reg = XMAC_CLEAR_RX_LSS_STATUSr;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
        } else if (soc_feature(unit, soc_feature_xlmac)) {
            /* Tomahawk management port is taking XLMAC, so processed here */
            reg = XLMAC_RX_LSS_STATUSr;
            rmt_fault_field = REMOTE_FAULT_STATUSf;
            lcl_fault_field = LOCAL_FAULT_STATUSf;
            clr_reg = XLMAC_CLEAR_RX_LSS_STATUSr;
#endif
        } else {
            rmt_fault_field = REMOTEFAULTSTATf;
            lcl_fault_field = LOCALFAULTSTATf;
            reg = MAC_RXLSSSTATr;
        }

        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, port, 0, &lss));
        rmt_fault = soc_reg64_field32_get(unit, reg, lss, rmt_fault_field);
        lcl_fault = soc_reg64_field32_get(unit, reg, lss, lcl_fault_field);

        if (rmt_fault || lcl_fault) {
            *fault = TRUE;
        }

        if (clr_reg != INVALIDr) {
            COMPILER_64_ZERO(lss);
            soc_reg64_field32_set(unit, clr_reg, &lss,
                                  CLEAR_REMOTE_FAULT_STATUSf, 1);
            soc_reg64_field32_set(unit, clr_reg, &lss,
                                  CLEAR_LOCAL_FAULT_STATUSf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, clr_reg,
                             port, 0, lss));

            soc_reg64_field32_set(unit, clr_reg, &lss,
                                  CLEAR_REMOTE_FAULT_STATUSf, 0);
            soc_reg64_field32_set(unit, clr_reg, &lss,
                                  CLEAR_LOCAL_FAULT_STATUSf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, clr_reg, port, 0, lss));
        }
    }

    return BCM_E_NONE;
}
#endif /* HERC15, FIREBOLT */


#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
STATIC int
_bcm_esw_link_lag_failover_loopback_get(int unit, int port, int* value)
{
    uint64 lag_fail_status;
    soc_reg_t lag_failover_status_reg;
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return bcmi_esw_portctrl_lag_failover_loopback_get(unit,
                                                           port,
                                                           value);
    }
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit, port)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (SOC_REG_IS_VALID(unit, LAG_FAILOVER_STATUSr)) {
        lag_failover_status_reg = LAG_FAILOVER_STATUSr;
    } else if (SOC_REG_IS_VALID(unit, GXPORT_LAG_FAILOVER_STATUSr)) {
        lag_failover_status_reg = GXPORT_LAG_FAILOVER_STATUSr;
    } else {
        if (SOC_IS_TOMAHAWKX(unit)) {
            lag_failover_status_reg = CLMAC_LAG_FAILOVER_STATUSr;
        } else {
            lag_failover_status_reg = XLMAC_LAG_FAILOVER_STATUSr;
        }
    }

    /* Has this port failed? */
    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, lag_failover_status_reg,
                     port, 0, &lag_fail_status));

    *value = soc_reg64_field32_get(unit, lag_failover_status_reg,
                                   lag_fail_status,
                                   LAG_FAILOVER_LOOPBACKf);
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_link_failover_link_up_with_portmod(int unit, int port) {
    int failed = TRUE;
    int retries = 5;
    int failover_loopback_usec = 5000;
    soc_timeout_t to;

    /* Unimac may take several tries before clearing the loopback state. */
    while (retries--) {
        BCM_IF_ERROR_RETURN(
            bcmi_esw_portctrl_lag_remove_failover_lpbk_set(unit, port, 0));
        BCM_IF_ERROR_RETURN(
            bcmi_esw_portctrl_lag_remove_failover_lpbk_set(unit, port, 1));

        soc_timeout_init(&to, failover_loopback_usec, 0);
        while (!soc_timeout_check(&to)) {
            int rv = BCM_E_NONE;
            rv = _bcm_esw_link_lag_failover_loopback_get(unit, port, &failed);
            if (BCM_SUCCESS(rv) && !failed) {
                break;
            }
        }
        if (!failed) {
            break;
        }
    }

    if (failed) {
        return BCM_E_TIMEOUT;
    }

    /* Disable LAG failover until port is returned to trunk */
    BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_lag_failover_disable(unit, port));
    BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_trunk_hwfailover_set(unit, port, 0));

    /* Flush any flow control status to guarantee normal traffic goes
     * to the port first. */
    if (SOC_IS_HELIX5(unit)) {
        uint64 zero64;
        COMPILER_64_ZERO(zero64);
        BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKP_64r(unit, port, zero64));
    }
    
    else if (!(SOC_IS_TOMAHAWK3(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit))) {
        BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));
    }

    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Unit %d: LAG Failed port %d status completed\n"),
                 unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_failover_link_up
 * Purpose:
 *      Perform the recovery sequence to restore a previously failed port
 *      when link up occurs.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_link_up(int unit, int port)
{
    int failed = TRUE, retries;
    uint64 lag_fail_config, lag_fail_status, rx_lss_ctrl;
    int failover_loopback_usec;
    soc_timeout_t to;
    soc_reg_t lag_failover_config_reg;
    soc_reg_t lag_failover_status_reg;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return _bcm_esw_link_failover_link_up_with_portmod(unit, port);
    }

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit, port)) {
        return BCM_E_UNAVAIL;
    }
#endif

    failover_loopback_usec = 5000;
    retries = 5;

    if (SOC_IS_SABER2(unit) && SOC_REG_PORT_VALID(unit,
         XLPORT_LAG_FAILOVER_CONFIGr, port)) {
        lag_failover_config_reg = XLPORT_LAG_FAILOVER_CONFIGr;
    } else if (SOC_REG_IS_VALID(unit, LAG_FAILOVER_CONFIGr)) {
        lag_failover_config_reg = LAG_FAILOVER_CONFIGr;
    } else if (SOC_REG_IS_VALID(unit, GXPORT_LAG_FAILOVER_CONFIGr)) {
        lag_failover_config_reg = GXPORT_LAG_FAILOVER_CONFIGr;
    } else {
        /* The LAG_FAILOVER_EN and REMOVE_FAILOVER_LPBK fields
         * moved to XLMAC_CTRL register in Trident2, CLMAC_CTRL register in TH
         */
        if (SOC_IS_TOMAHAWKX(unit)) {
            lag_failover_config_reg = CLMAC_CTRLr;
        } else {
            lag_failover_config_reg = XLMAC_CTRLr;
        }
    }

    if (SOC_REG_IS_VALID(unit, LAG_FAILOVER_STATUSr)) {
        lag_failover_status_reg = LAG_FAILOVER_STATUSr;
    } else if (SOC_REG_IS_VALID(unit, GXPORT_LAG_FAILOVER_STATUSr)) {
        lag_failover_status_reg = GXPORT_LAG_FAILOVER_STATUSr;
    } else {
        if (SOC_IS_TOMAHAWKX(unit)) {
            lag_failover_status_reg = CLMAC_LAG_FAILOVER_STATUSr;
        } else {
            lag_failover_status_reg = XLMAC_LAG_FAILOVER_STATUSr;
        }
    }

    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, lag_failover_config_reg, port, 0,
                     &lag_fail_config));

    /* Unimac may take several tries before clearing the loopback state. */
    while (retries--) {
        soc_reg64_field32_set(unit, lag_failover_config_reg,
                              &lag_fail_config, REMOVE_FAILOVER_LPBKf, 0);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, lag_failover_config_reg,
                         port, 0, lag_fail_config));
        soc_reg64_field32_set(unit, lag_failover_config_reg,
                              &lag_fail_config, REMOVE_FAILOVER_LPBKf, 1);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, lag_failover_config_reg,
                         port, 0, lag_fail_config));

        soc_timeout_init(&to, failover_loopback_usec, 0);
        while (!soc_timeout_check(&to)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_get(unit, lag_failover_status_reg,
                             port, 0, &lag_fail_status));
            failed =
                soc_reg64_field32_get(unit, lag_failover_status_reg,
                                      lag_fail_status, LAG_FAILOVER_LOOPBACKf);
            if (!failed) {
                break;
            }
        }
        if (!failed) {
            break;
        }
    }

    if (failed) {
        return BCM_E_TIMEOUT;
    }

    /* Disable LAG failover until port is returned to trunk */
    soc_reg64_field32_set(unit, lag_failover_config_reg,
                          &lag_fail_config, REMOVE_FAILOVER_LPBKf, 0);
    soc_reg64_field32_set(unit, lag_failover_config_reg,
                          &lag_fail_config, LAG_FAILOVER_ENf, 0);
    if (soc_reg_field_valid(unit, lag_failover_config_reg,
                            LINK_STATUS_SELECTf)) {
        soc_reg64_field32_set(unit, lag_failover_config_reg,
                              &lag_fail_config, LINK_STATUS_SELECTf, 0);
    }
    BCM_IF_ERROR_RETURN
        (soc_reg_set(unit, lag_failover_config_reg, port, 0, lag_fail_config));

    if (SOC_REG_IS_VALID(unit, XLMAC_RX_LSS_CTRLr)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, XLMAC_RX_LSS_CTRLr, port, 0, &rx_lss_ctrl));
        soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rx_lss_ctrl,
                              RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, XLMAC_RX_LSS_CTRLr, port, 0, rx_lss_ctrl));
    }

    /* Flush any flow control status to guarantee normal traffic goes
     * to the port first. */
    if (SOC_IS_HELIX5(unit)) {
        uint64 zero64;
        COMPILER_64_ZERO(zero64);
        BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKP_64r(unit, port, zero64));
    } else if (SOC_REG_IS_VALID(unit, XPORT_TO_MMU_BKPr)) {
        BCM_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));
    }

    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Unit %d: LAG Failed port %d status completed\n"),
                 unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_failover_link_down_force
 * Purpose:
 *      Force a link down event on a LAG failover enabled port when
 *      a fault is detected.  This will engage the HW failover
 *      mechanism.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_link_down_force(int unit, int port)
{
    int failed = FALSE;
    int failover_loopback_usec;
    soc_timeout_t to;
    int rv;

    PORT_LOCK(unit);
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        rv = bcmi_esw_portctrl_phy_enable_set(unit, port, FALSE);
    } else {
        rv = soc_phyctrl_enable_set(unit, port, FALSE);
    }
    PORT_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    failover_loopback_usec = 5000;
    soc_timeout_init(&to, failover_loopback_usec, 0);
    while (!soc_timeout_check(&to)) {
        rv = _bcm_esw_link_lag_failover_loopback_get(unit, port, &failed);
        if (BCM_SUCCESS(rv) && failed) {
            break;
        }
    }

    if (!failed) {
        return BCM_E_TIMEOUT;
    }

    /* Turn PHY back on now that we've entered failed state */
    PORT_LOCK(unit);
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        rv = bcmi_esw_portctrl_phy_enable_set(unit, port, TRUE);
    } else {
        rv = soc_phyctrl_enable_set(unit, port, TRUE);
    }
    PORT_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_failover_port_disable
 * Purpose:
 *      When a port enters failed state, disable the RX path and
 *      flush the TX FIFO.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_link_failover_port_disable(int unit, int port)
{
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        /* disable rx path of (unit, port) */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_mac_failover_notify(unit,port));

        if (!SOC_PORT_USE_PORTCTRL(unit, port)) {
            if (SOC_REG_IS_VALID(unit, MAC_TXCTRLr)) {
                /* Flush BigMAC TX FIFO */
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                            TXFIFO_RESETf, 1));
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                            TXFIFO_RESETf, 0));
            }
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
STATIC int
_hawkeye_new_link_check(int unit, int port)
{
    uint32 val;
    bcm_port_medium_t medium;
    bcm_port_ability_t local_advert;
    bcm_port_ability_t remote_advert;
    uint32 mac_data = 0, phy_data = 0;
    int speed = 0, duplex = 0, an = 0;
    int tx_pause = 0, rx_pause = 0;
    int rv;

    BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_get(unit, port, &an));
    BCM_IF_ERROR_RETURN(bcm_esw_port_medium_get(unit, port, &medium));
    if (medium == BCM_PORT_MEDIUM_COPPER){
        /* medium is copper */
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_phy_get(unit, port, 0,
                                  HK_PAUSE_WAR_PHY_REG, &phy_data));
        if (phy_data & PHY_AN_TX_PAUSE_CAP) {
            tx_pause = 1;
        } else {
            tx_pause = 0;
        }

        if (phy_data & PHY_AN_RX_PAUSE_CAP) {
            rx_pause = 1;
        } else {
            rx_pause = 0;
        }
    } else {
        /* medium is fiber */

        sal_memset(&local_advert, 0, sizeof(bcm_port_ability_t));
        rv = bcm_esw_port_ability_advert_get(unit, port, &local_advert);
        if (BCM_FAILURE(rv)) {
            LOG_WARN(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "p=%d soc_phyctrl_adv_local_get rv=%d\n"),
                      port, rv));
            return rv;
        }

        sal_memset(&remote_advert, 0, sizeof(bcm_port_ability_t));
        rv = bcm_esw_port_ability_remote_get(unit, port, &remote_advert);
        if (BCM_FAILURE(rv)) {
            LOG_WARN(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "p=%d soc_phyctrl_adv_remote_get rv=%d\n"),
                      port, rv));
            return rv;
        }

        /*
         * IEEE 802.3 Flow Control Resolution.
         * Please see $SDK/doc/pause-resolution.txt
         * for more information.
         */

        tx_pause =
            ((remote_advert.pause & SOC_PA_PAUSE_RX) &&
             (local_advert.pause & SOC_PA_PAUSE_RX)) ||
            ((remote_advert.pause & SOC_PA_PAUSE_RX) &&
             !(remote_advert.pause & SOC_PA_PAUSE_TX) &&
             (local_advert.pause & SOC_PA_PAUSE_TX));

        rx_pause =
            ((remote_advert.pause & SOC_PA_PAUSE_RX) &&
             (local_advert.pause & SOC_PA_PAUSE_RX)) ||
            ((local_advert.pause & SOC_PA_PAUSE_RX) &&
             (remote_advert.pause & SOC_PA_PAUSE_TX) &&
             !(local_advert.pause & SOC_PA_PAUSE_TX));
    }

    if(soc_feature(unit, soc_feature_hawkeye_a0_war)) {
        /* QSGMII Pause Work aroud for HAWKEYE A0*/
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_TX_PAUSE_CAPABILITYr(unit, &mac_data));

        if (tx_pause) {
            mac_data |= (1 << (port-1));
        } else {
            mac_data &= ~(1 << (port-1));
        }
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, mac_data));

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_RX_PAUSE_CAPABILITYr(unit, &mac_data));

        if (rx_pause) {
            mac_data |= (1 << (port-1));
        } else {
            mac_data &= ~(1 << (port-1));
        }
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, mac_data));

        PORT_LOCK(unit);
        /* Toggle ENA_EXT_CONFIG of command_config register */
        rv = READ_COMMAND_CONFIGr(unit, port, &mac_data);
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                              ENA_EXT_CONFIGf, 0);
            rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
        }
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                              ENA_EXT_CONFIGf, 1);
            rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
        }

        if (BCM_SUCCESS(rv)) {
            sal_udelay(10000);
        }
        if (!an) {
            if (BCM_SUCCESS(rv)) {
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                                  ENA_EXT_CONFIGf, 0);
                rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
            }
        }
        PORT_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* QSGMII Pause Work aroud for NON HAWKEYE A0*/
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_TX_PAUSE_CAPABILITYr(unit, &mac_data));

        if (tx_pause) {
            mac_data |= (1 << port);
        } else {
            mac_data &= ~(1 << port);
        }
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, mac_data));

        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_RX_PAUSE_CAPABILITYr(unit, &mac_data));

        if (rx_pause) {
            mac_data |= (1 << port);
        } else {
            mac_data &= ~(1 << port);
        }
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, mac_data));

        PORT_LOCK(unit);
        /* Toggle ENA_EXT_CONFIG of command_config register */
        rv = READ_COMMAND_CONFIGr(unit, port, &mac_data);
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                              ENA_EXT_CONFIGf, 0);
            rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
        }
        if (BCM_SUCCESS(rv)) {
            soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                              ENA_EXT_CONFIGf, 1);
            rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
        }
        if (BCM_SUCCESS(rv)) {
            sal_udelay(10000);
        }
        if (!an) {
            if (BCM_SUCCESS(rv)) {
                soc_reg_field_set(unit, COMMAND_CONFIGr, &mac_data,
                                  ENA_EXT_CONFIGf, 0);
                rv = WRITE_COMMAND_CONFIGr(unit, port, mac_data);
            }
        }
        PORT_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
        /*
         * In HK A0, the back-pressure is disabled
         *        when the bit HD_FC_ENA is cleared (binary 0).
         * In other revision of HK, the back-pressure is disabled
         *        when the bit HD_FC_ENA is set (binary 1).
         * To avoid confusion with HK A0,
         * S/W set the bit HD_FC_ENA to 1 in other revision of HK.
         *     P.S. Because the value of register IPG_HD_BKP_CNTL
         *          is set to default on link up, S/W needs to
         *          set the value again for every link-up.
         */
        SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &val));
        soc_reg_field_set(unit, IPG_HD_BKP_CNTLr,
                          &val, HD_FC_ENAf, 0x1);
        SOC_IF_ERROR_RETURN(WRITE_IPG_HD_BKP_CNTLr(unit, port, val));
    }
    /* Issue :
     *    1. Port link flapping during 10/100M half duplex flow control
     *    2. Fiber port will lose packet when use 156.2Mhz clock
     *    3. Packet loss on 2 HK systems cascade test
     * Solution :
     *    1. Set HD_FC_BKOFF_OK=1 for 10/100M half duplex
     *    2. Set HD_FC_BKOFF_OK=0(default value) for 1000M
     *    3. Set IPG_CONFIG_RX=0xA for 10/100M and
     *       IPG_CONFIG_RX=0x5(default value) for 1000M
     *
     *    P.S. Because the value of register IPG_HD_BKP_CNTL is set to
     *         default on link up, S/W needs to set the value
     *         again for every link-up.
     */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &speed));
    BCM_IF_ERROR_RETURN(bcm_esw_port_duplex_get(unit, port, &duplex));
    SOC_IF_ERROR_RETURN(READ_IPG_HD_BKP_CNTLr(unit, port, &val));
    if(speed < 1000) {
        soc_reg_field_set(unit, IPG_HD_BKP_CNTLr, &val,
                          IPG_CONFIG_RXf, 0xa);
        if(duplex == SOC_PORT_DUPLEX_HALF) {
            soc_reg_field_set(unit, IPG_HD_BKP_CNTLr, &val,
                              HD_FC_BKOFF_OKf, 0x1);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_IPG_HD_BKP_CNTLr(unit, port, val));

    if(soc_feature(unit, soc_feature_eee)) {
        uint32 eee_duration_timer_pulse;
        soc_timeout_t to_time;
        int need_uni_sw_reset = FALSE;
        int need_pau_eee_war = FALSE;
        int eee_en;
        ls_cntl_t *lc = link_control[unit];
        _bcm_port_info_t *port_info;

        /* Each GPORT uses the GMII_Tx_clk from its second port
         * to count Tx/Rx LPI Duration .
         * Hence SW needs to program EEE_DURATION_TIMER_PULSE based
         * on the speed of only GE9 of gport1 & GE17 of gport2.
         */
        SOC_IF_ERROR_RETURN
            (bcm_esw_port_control_get(unit, port,
                                      bcmPortControlEEEEnable, &eee_en));

        if (eee_en && ((port == 10) || (port == 18))) {
            if (speed == 10){
                eee_duration_timer_pulse = 0x18;
            } else if (speed == 100) {
                eee_duration_timer_pulse = 0xF9;
            } else {
                /* restore default value in chip */
                eee_duration_timer_pulse = 0x4E1;
            }

            if (soc_feature(unit, soc_feature_unified_port)) {
                SOC_IF_ERROR_RETURN
                    (READ_PORT_EEE_DURATION_TIMER_PULSEr(unit, port, &val));
                soc_reg_field_set(unit, PORT_EEE_DURATION_TIMER_PULSEr, &val,
                                  CNT_VALUEf, eee_duration_timer_pulse);
                SOC_IF_ERROR_RETURN
                    (WRITE_PORT_EEE_DURATION_TIMER_PULSEr(unit, port, val));
            } else {
                if (SOC_REG_IS_VALID(unit,GE0_EEE_CONFIGr)) {
                    SOC_IF_ERROR_RETURN
                        (READ_GE0_EEE_CONFIGr(unit, port, &val));
                    soc_reg_field_set
                        (unit, GE0_EEE_CONFIGr, &val,
                         EEE_DURATION_TIMER_PULSEf, eee_duration_timer_pulse);
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE0_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE1_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE2_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE3_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE4_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE5_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE6_EEE_CONFIGr(unit, port, val));
                    SOC_IF_ERROR_RETURN
                        (WRITE_GE7_EEE_CONFIGr(unit, port, val));
                }
                need_uni_sw_reset = TRUE;
            }
        }

        /* EEE-Pause WAR */
        /* step 1. Program IBPPKTSETLIMIT and IBPCELLSETLIMIT */
        /* step 2. Poll IBPBKPSTATUS register in MMU */
        /* step 3. COMMAND_CONFIG. SW_RESET */

        /* 1. Program IBPPKTSETLIMIT and IBPCELLSETLIMIT */
        if (!an) {
            /* coverity[check_return] */
            bcm_esw_port_pause_get(unit, port, &tx_pause, &rx_pause);
        }

        SOC_IF_ERROR_RETURN(READ_IBPPKTSETLIMITr(unit, port, &val));
        if((rx_pause && tx_pause) || (!eee_en)){
            if(val == 0x3FFF) {
                val = lc->lc_ibppktsetlimit[port-1];
                SOC_IF_ERROR_RETURN
                    (WRITE_IBPPKTSETLIMITr(unit, port, val));
                val = lc->lc_ibpcelsetlimit[port-1];
                SOC_IF_ERROR_RETURN
                    (WRITE_IBPCELLSETLIMITr(unit, port, val));

                need_pau_eee_war = TRUE;
            }
        } else {
            if (val != 0x3FFF) {
                /* save value for restore at PAUSE mode */
                lc->lc_ibppktsetlimit[port-1] = val;
                SOC_IF_ERROR_RETURN
                    (READ_IBPCELLSETLIMITr(unit, port, &val));
                lc->lc_ibpcelsetlimit[port-1] = val;

                val = 0x3FFF;
                SOC_IF_ERROR_RETURN
                    (WRITE_IBPPKTSETLIMITr(unit, port, val));
                SOC_IF_ERROR_RETURN
                    (WRITE_IBPCELLSETLIMITr(unit, port, val));

                need_pau_eee_war = TRUE;
            }
        }
        /* 2. Poll IBPBKPSTATUS register in MMU */
        if (need_pau_eee_war) {
            soc_timeout_init(&to_time, 100000, 0);
            for (;;) {
                SOC_IF_ERROR_RETURN(READ_IBPBKPSTATUSr(unit, &val));
                if(!(val & (0x1 << (port)))) {
                    break;
                }

                if (soc_timeout_check(&to_time)) {
                    break;
                }
            }
        }

        /* 3. COMMAND_CONFIG. SW_RESET */
        if ((need_pau_eee_war) || (need_uni_sw_reset)) {
            _bcm_port_info_access(unit, port, &port_info);
            PORT_LOCK(unit);
            rv = MAC_CONTROL_SET(port_info->p_mac,
                                 unit, port,
                                 SOC_MAC_CONTROL_SW_RESET, TRUE);
            if (BCM_SUCCESS(rv)) {
                sal_udelay(2);
                rv = MAC_CONTROL_SET(port_info->p_mac,
                                     unit, port,
                                     SOC_MAC_CONTROL_SW_RESET, FALSE);
            }
            PORT_UNLOCK(unit);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}
#endif /* #if defined(BCM_HAWKEYE_SUPPORT) */

/*
 * Function:
 *      _bcm_esw_linkscan_hw_link_port_get
 * Purpose:
 *      Get port link status from HW.
 *      Link status may be derived from the SW PHY driver as needed.
 *
 *      In addition, it checks for ports that need to be added to
 *      the accelerated port bitmap.  Conditions:
 *      - Linkscan mode is HW
 *      - Specified in the SOC property 'fast_sw_rx_los_pbmp' bitmap
 *      - State is DOWN and HW link indicates an UP state
 * Parameters:
 *      unit      - Device unit.
 *      port      - Port to update.
 *      cur_link  - Current port link status
 *      new_link  - Returns new port link status
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     Assumes LOCK.
 *     Assumes port is in HW linkscan mode.
 */
STATIC int
_bcm_esw_linkscan_hw_link_port_get(int unit, int port,
                                   int cur_link, int *new_link)
{
    ls_cntl_t *lc = link_control[unit];
    int sw_new_link = FALSE;

    if (BCM_PBMP_MEMBER(lc->lc_pbm_sgmii_autoneg_port, port)) {
        BCM_IF_ERROR_RETURN(_bcm_port_link_get(unit, port, 0, new_link));
    } else {
        /* Get HW link status */
        /* If port is in accelerated mode,
         * then there was an ealier HW link up event.
         * Skip the HW link status get and set the new_link to TRUE.
         */
        if (!BCM_PBMP_MEMBER(lc->lc_pbm_accel, port)) {
            BCM_IF_ERROR_RETURN(_bcm_port_link_get(unit, port, 1, new_link));
            /* Capture time from last HW event in a down to up transition */
            if ((cur_link == FALSE) && (*new_link == TRUE)) {
                last_time_hw_link_up[unit] = sal_time_usecs();
            }
       } else {
            *new_link = TRUE;
        }

        /* Check for down to up transition on ports with fast linkscan */
        if (BCM_PBMP_MEMBER(lc->lc_pbm_fast, port) && (cur_link == FALSE)) {
            /* Get SW link status from PHY */
            BCM_IF_ERROR_RETURN
                (_bcm_port_link_get(unit, port, 0, &sw_new_link));
            /*
             * Add port to accelerated bitmap if HW indicates a link up
             * but SW indicates link down
             */
            if (*new_link == TRUE) {
                if (sw_new_link == TRUE) {
                    /*
                     * Do not remove the port from the accelerated bitmap yet.
                     * Linkscan logic requires that the current and new link
                     * status are up before the fault status is read and
                     * cleared.
                     *
                     * Remove is done below after link is already up.
                     */
                    /* Not only get SW link status from PHY,  but also trigger PHY to UP state */
                    BCM_IF_ERROR_RETURN
                        (_bcm_port_link_get(unit, port, 0, &sw_new_link));
                } else {
                    /* Add port to accelerated bitmap */
                    if (!BCM_PBMP_MEMBER(lc->lc_pbm_accel, port)) {
                        BCM_PBMP_PORT_ADD(lc->lc_pbm_accel, port);
                        lc->lc_accel_poll_count = 0;
                        LOG_INFO(BSL_LS_BCM_LINK,
                                 (BSL_META_U(unit,
                                             "Port %s: add to accelerated mode\n"),
                                  SOC_PORT_NAME(unit, port)));
                    }
                }
            }
            *new_link = sw_new_link;
        }

        /* Check if port needs to be removed from the accelerated bitmap */
        if (BCM_PBMP_MEMBER(lc->lc_pbm_accel, port)) {
            if ((cur_link == TRUE) && (*new_link == TRUE)) {
                /*
                 * Everything is fine, proceed with link up.
                 * Remove port from accelerated bitmap.
                 */
                BCM_PBMP_PORT_REMOVE(lc->lc_pbm_accel, port);
                LOG_INFO(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                     "Port %s: remove from accelerated mode\n"),
                          SOC_PORT_NAME(unit, port)));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_linkscan_sw_link_port_get
 * Purpose:
 *      Get port link status from PHY.
 *
 *      In addition, it checks for ports that need to be added to
 *      the accelerated port bitmap.  Conditions:
 *      - Linkscan mode is SW
 *      - Specified in the SOC property 'fast_sw_rx_los_pbmp' bitmap
 *      - State is DOWN
 * Parameters:
 *      unit      - Device unit.
 *      port      - Port to update.
 *      cur_link  - Current port link status
 *      new_link  - Returns new port link status
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     Assumes LOCK.
 *     Assumes port is in SW linkscan mode.
 */
STATIC int
_bcm_esw_linkscan_sw_link_port_get(int unit, int port, int *new_link)
{
    ls_cntl_t *lc = link_control[unit];

    /* Get link status from PHY */
    BCM_IF_ERROR_RETURN
        (_bcm_port_link_get(unit, port, 0, new_link));
    /* Check current link status and update accelerate port bitmap */
    if (BCM_PBMP_MEMBER(lc->lc_pbm_fast, port)) {
        if (*new_link == FALSE) {
            /* Add port to accelerated bitmap */
            if (!BCM_PBMP_MEMBER(lc->lc_pbm_accel, port)) {
                BCM_PBMP_PORT_ADD(lc->lc_pbm_accel, port);
                lc->lc_accel_poll_count = 0;
                LOG_INFO(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                     "Port %s: add to accelerated mode\n"),
                          SOC_PORT_NAME(unit, port)));
            }
         } else {
            if (BCM_PBMP_MEMBER(lc->lc_pbm_accel, port)) {
                /*
                 * Everything is fine, proceed with link up.
                 * Remove port from accelerated bitmap.
                 */
                BCM_PBMP_PORT_REMOVE(lc->lc_pbm_accel, port);
                LOG_INFO(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                     "Port %s: remove from accelerated mode\n"),
                          SOC_PORT_NAME(unit, port)));
            }
        }
    }
    return BCM_E_NONE;
}

#if defined(INCLUDE_PHY_542XX)
/*
 * Function:
 *      _bcm_esw_linkscan_serdes_link_update
 * Purpose:
 *      Update the Force SerDes link status.
 *
 * Parameters:
 *      unit      - Device unit.
 *      port      - Port to update.
 *      link      - current port link status
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     Assumes LOCK.
 *     Assumes port is in SW linkscan mode.
 */
STATIC int
_bcm_esw_linkscan_serdes_link_update(int unit, int port, int link)
{
    int rv = BCM_E_NONE;

    if (SOC_PORT_USE_PORTCTRL(unit, port) && (!IS_ROE_PORT(unit, port))) {
        rv = bcmi_esw_portctrl_serdes_link_update(unit, port, link);
    }
    return rv;
}
#endif /* defined(INCLUDE_PHY_542XX) */

/*
 * The introduction of fault state complicates the transitions for
 * the linkscan update.  If the link is established, then its fault
 * status is valid.  If the link is down, then the fault status must be
 * false.  Since we track the "current" and "new" states of each, we
 * get this truth table:
 *
 *     Link   Fault
 *     C  N   C  N
 *     -  -   -  -
 * 1)  1  1   1  1   No change
 * 2)  1  1   1  0   Fault cleared, enable logical layer, notify link up
 * 3)  1  1   0  1   Fault detected, disable logical layer, notify link down
 * 4)  1  1   0  0   No change
 * 5)  1  0   1  0   Faulted link down, disable port, no notify
 * 6)  1  0   0  0   No fault link down, disable logical layer and port,
 *                       notify link down
 * 7)  0  1   0  1   Link up to fault state, enable port but not
 *                       logical layer, no notify
 * 8)  0  1   0  0   Link up, enable port and logical layer, notify link up
 * 9)  0  0   0  0   No change
 *
 * Now to the above we add LAG failover.  For LAG failover, when a
 * front-panel port on which trunk failover is configured goes down by
 * either of the definitions above (3 or 6), it is not actually disabled
 * but instead moved to the new FAILED state.  This state remains until
 * explicitly cleared by the application reporting that the FAILED state
 * has been properly handled (failed port removed from trunk groups), via
 * bcm_port_link_failed_clear.  When bcm_port_link_failed_clear is called,
 * link down processing completes (forced if necessary) and the link down
 * status is reported.  When the link goes back up, some final cleanup
 * of the port is performed.  After this point, the port may be put back
 * into any LAGs by the application.
 */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
extern void
_bcm_stk_modport_map_linkscan_handler(int unit, bcm_port_t port,
                                      bcm_port_info_t *info);
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */



/*
 * Function:
 *      _bcm_esw_linkscan_update_port
 * Purpose:
 *      Check for and process a link event on one port
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      flags - flags to indicate notification
 *      port  - Port to process.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_linkscan_update_port(int unit, uint32 flags, int port)
{
    soc_persist_t *sop = SOC_PERSIST(unit);
    ls_cntl_t *lc = link_control[unit];
    int cur_link, change, new_link = FALSE;
    bcm_port_info_t info;
    ls_handler_t  *lh, *lh_next = NULL;
    int rv = BCM_E_NONE;
    int cur_fault, new_fault = FALSE,
        unforced = FALSE, notify = FALSE,
        logical_link, asf_link, info_skip = 0;
    int cur_failed, new_failed = FALSE;
    soc_pbmp_t pbm_link_fwd;
#if defined(BCM_LINK_CHANGE_BENCHMARK)
    sal_usecs_t time_start = 0;
    sal_usecs_t time_end = 0;
#endif /* BCM_LINK_CHANGE_BENCHMARK */
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 mac_data = 0;
#endif /* BCM_HAWKEYE_SUPPORT */
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
    int eee_en = 0, mac_val;
    sal_time_t current_time = 0;
    _bcm_port_info_t *port_info;
    bcm_port_ability_t remote_advert;
#endif /* HURRICANE TRIDENT HAWKEYE HURRICANE2 */
#if defined(BCM_KATANA2_SUPPORT)
    soc_port_t  pp_port;
    int         bypass = 0;
    int         wc_10g_21g_sel = 0;
#endif /* BCM_KATANA2_SUPPORT */
    int hw_link_port = FALSE;
#ifdef BCM_KATANA2_SUPPORT
    uint32      rval = 0;
#endif
    char *ll_status[] = {"down", "up", "failed", "fault"};

    /* Here, the assert may lead to thread 'bcmLink' exit. */
    /* assert(SOC_PORT_VALID(unit, port)); */
    if (!SOC_PORT_VALID(unit, port)) {
       LOG_ERROR(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                             "unit=%d port=%d is invaild\n"),
                  unit, port));
       return BCM_E_PARAM;
    }

    /*
     * Current link status is calculated in the following order:
     *   1) If link status is in an override state, the override
     *      state is used.
     *   2) If link is required to recognize a link down event, the
     *      current scan will recognize the link down (if the link
     *      was previously down, this will result in no action
     *      being taken)
     *   3) Use real detected link status.
     *        a) If using Hardware link scanning, use captured H/W
     *           value since the H/W scan will clear the latched
     *           link down event.
     *        b) If using S/W link scanning, retrieve the current
     *           link status from the PHY.
     */

    cur_link = SOC_PBMP_MEMBER(sop->lc_pbm_link, port);
    cur_fault = SOC_PBMP_MEMBER(sop->lc_pbm_remote_fault, port);
    cur_failed = SOC_PBMP_MEMBER(sop->lc_pbm_failed, port);

    change = SOC_PBMP_MEMBER(sop->lc_pbm_link_change, port);
    SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link_change, port);

    if (change) {                                                   /* 2) */
        new_link = FALSE;
        rv = BCM_E_NONE;
    } else if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port)) { /* 1) */
        new_link = SOC_PBMP_MEMBER(sop->lc_pbm_override_link, port);
        rv = BCM_E_NONE;
    } else if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, port) ||
               SOC_PBMP_MEMBER(lc->lc_pbm_sw, port)) {       /* 3a) or 3b) */
        if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, port)) {                 /* 3a) */
            rv = _bcm_esw_linkscan_hw_link_port_get(unit, port,
                                                    cur_link, &new_link);
            unforced = TRUE;
            hw_link_port = TRUE;
            if (new_link) {
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                        (BSL_META_U(unit,
                                    "Unit %d: HW link p=%d up\n"),
                                    unit, port));
            } else {
                LOG_VERBOSE(BSL_LS_BCM_LINK,
                            (BSL_META_U(unit,
                                        "Unit %d: HW link p=%d down\n"),
                                        unit, port));
            }
        } else if (SOC_PBMP_MEMBER(lc->lc_pbm_sw, port)) {          /* 3b) */
            if (lc->lc_f[port]) {
                int state;

                rv = lc->lc_f[port](unit, port, &state);
                if (rv == BCM_E_NONE) {
                    new_link = state ? TRUE : FALSE;
                } else if (rv == BCM_E_UNAVAIL) {
                    rv = _bcm_esw_linkscan_sw_link_port_get(unit, port,
                                                            &new_link);
                }
            } else {
                rv = _bcm_esw_linkscan_sw_link_port_get(unit, port, &new_link);
#if defined(INCLUDE_PHY_542XX)
                if ((rv == BCM_E_NONE) && (cur_link != new_link)){
                    rv = _bcm_esw_linkscan_serdes_link_update(unit, port,
                                                              new_link);
                }
#endif /* defined(INCLUDE_PHY_542XX) */
            }
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                if (cur_link != new_link) {
                    if (!new_link) {
                        _bcm_esw_ibod_syn_port_down (unit, port);
                    }
                }
            }
#endif
            unforced = TRUE;
        }

        /* Update EEE in both 3a) and 3b) */
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if (SOC_IS_HURRICANE (unit) || SOC_IS_TD_TT (unit) ||
            SOC_IS_HAWKEYE (unit) || SOC_IS_KATANAX(unit) ||
            SOC_IS_TRIUMPH3(unit) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            /* EEE standard compliance Work Around:*/
            /* When link is up, delay EEE enable in MAC by atleast 1sec from
             * link up time if EEE is enabled in software database and disabled
             * in MAC */
            if (soc_feature(unit, soc_feature_eee)) {
                /* Get EEE status from software database */
                if (bcm_esw_port_control_get(unit, port,
                                             bcmPortControlEEEEnable,
                                             &eee_en) == BCM_E_NONE) {
                    /* Remove port from EEE pending if EEE is not enabled */
                    if (!eee_en) {
                        BCM_PBMP_PORT_REMOVE(lc->lc_pbm_eee_pending, port);
                    }
                    /* If EEE is enabled in software and port link is up */
                    if (eee_en && new_link) {
                        /* Get EEE status from hardware (MAC) */
                        current_time = sal_time();
                        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                            if (current_time > lc->lc_up_time[port-1]) {
                                /* Do not enable native EEE on the MAC if
                                 * autoEEE is enabled */
                                int r;
                                uint32 auto_eee = 1;
                                r = bcm_esw_port_phy_control_get(
                                        unit, port,
                                        BCM_PORT_PHY_CONTROL_EEE_AUTO,
                                        &auto_eee);
                                if (r == BCM_E_UNAVAIL || !auto_eee) {
                                    bcmi_esw_portctrl_eee_enable_set(unit,
                                                                     port,
                                                                     1);
                                    /* EEE has been enabled, remove the port
                                     * from pending */
                                    BCM_PBMP_PORT_REMOVE(lc->lc_pbm_eee_pending,
                                                         port);
                                }
                            }
                        } else {
                            _bcm_port_info_access(unit, port, &port_info);
                            if (MAC_CONTROL_GET(port_info->p_mac, unit, port,
                                                SOC_MAC_CONTROL_EEE_ENABLE,
                                                &eee_en) == BCM_E_NONE) {
                                /* If EEE is disabled in MAC and atleast 1
                                 * second elapsed then enable EEE in MAC */
                                if ((current_time > lc->lc_up_time[port-1]) &&
                                    !eee_en) {
                                    LOG_VERBOSE(BSL_LS_BCM_LINK,
                                                (BSL_META_U(unit,
                                                 "u=%d p=%d EEE enabled in MAC\n"),
                                                 unit, port));
                                    PORT_LOCK(unit);
                                    MAC_CONTROL_SET(port_info->p_mac,
                                                    unit,
                                                    port,
                                                    SOC_MAC_CONTROL_EEE_ENABLE,
                                                    1);
                                    PORT_UNLOCK(unit);
                                    /* EEE has been enabled, remove the port
                                     * from pending */
                                    BCM_PBMP_PORT_REMOVE(lc->lc_pbm_eee_pending,
                                                         port);
                                }
                            }
                        }
                    }
                }
            }
        }
#endif /* BCM_HURRICANE_SUPPORT || BCM_HAWKEYE_SUPORT || BCM_TRIDENT_SUPPORT */
    } else {
        return BCM_E_NONE;  /* Port not being scanned */
    }

    if (BCM_FAILURE(rv)) {

#ifdef BCM_RCPU_SUPPORT
        if (SOC_IS_RCPU_ONLY(unit) && (BCM_E_TIMEOUT != rv)) {
            /* Ignore errors other than timeout for rcpu devices. */
            return BCM_E_NONE;
        }
#endif /* BCM_RCPU_SUPPORT */

        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "Port %s: Failed to recover link status: %s\n"),
                   SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
        return rv;
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    /* Check for failed state before fault since it takes priority,
     * and because we may force the link down on clearing the failed state.
     */
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        if (cur_failed) {
            if (!SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
                new_failed = TRUE; /* Sticky until cleared */
            } else { /* Clearing failed status */
                new_failed = FALSE;
                new_link = FALSE; /* Force link down on clear */
                SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failed, port);
            }
        } else if (SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
            rv = _bcm_esw_link_lag_failover_loopback_get(
                             unit, port, &new_failed);
            if (BCM_SUCCESS(rv) && new_failed) {
                PORT_LOCK(unit);
                rv = _bcm_esw_link_failover_port_disable(unit, port);
                PORT_UNLOCK(unit);
                BCM_IF_ERROR_RETURN(rv);
                SOC_PBMP_PORT_ADD(sop->lc_pbm_failed, port);
            }
        } /* else, new_failed = FALSE from init above */
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if ((soc_feature(unit, soc_feature_bigmac_fault_stat) ||
        soc_feature(unit, soc_feature_xlmac)) &&
        unforced && (cur_link) && (new_link) && !new_failed) {
        /*
         * Check the fault status here only if:
         * 1) The chip supports it.
         * 2) The link state is coming from the PHY (unforced).
         * 3) The link was already up and is still.
         */

        /* Check 10G fault status for change */
        rv = _bcm_esw_link_fault_get(unit, port, &new_fault);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                       "Unit %d, Port %s: Failed to read fault status: %s\n"),
                       unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        if (new_fault && soc_feature(unit, soc_feature_port_lag_failover) &&
            SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
            /* Must force port into LAG failover mode by forcing
             * a PHY linkdown event. */
            rv = _bcm_esw_link_failover_link_down_force(unit, port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                  "Unit %d, Port %s: Failed to force link down "
                                  "for fault on LAG failover: %s\n"),
                           unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                return rv;
            } else {
                new_failed = TRUE;
                PORT_LOCK(unit);
                rv = _bcm_esw_link_failover_port_disable(unit, port);
                PORT_UNLOCK(unit);
                BCM_IF_ERROR_RETURN(rv);
                SOC_PBMP_PORT_ADD(sop->lc_pbm_failed, port);
            }
        }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    }
#endif /* HERC15, FIREBOLT */

    /*
    * Add/Remove linkdown_tx ports to forward pbmp in order to add/Remove ports
    * to/from EPC_LINK_BMAPm, when notified by bcm_esw_link_change() with
    * cur_link down, new_link down.
    */
    if (change && (!cur_link) && (!new_link)) {
        SOC_PBMP_ASSIGN(pbm_link_fwd, sop->lc_pbm_link);
        SOC_PBMP_REMOVE(pbm_link_fwd, sop->lc_pbm_remote_fault);
        SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_linkdown_tx);
        SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_roe_linkdown_tx);
        rv = soc_link_fwd_set(unit, pbm_link_fwd);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: soc_link_fwd_set failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    if (((cur_failed && new_failed)) ||
        ((cur_link) && (new_link) && (cur_fault == new_fault)) ||
        ((!cur_link) && (!new_link))) {
        /* No change */
        return BCM_E_NONE;
    }

#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
    /* Because of actual time of link partner port up transition,
     * it is possible we may receive delayed Remote Fault signal from link
     * partner. Assuming this situation, added internal status for each port
     * to ignore this delayed Remote Fault signal condition.
     *
     * When new_link is detected, set status LC_LINK_UP_STATE_START.
     *
     * In the next cycle,
     * if cur_link, new_link, cur_fault is true, but new_fault is false,
     * then move to LC_LINK_UP_STATE_DONE.
     * (Assuming this means link up is stable and didn't receive Remote Fault.)
     *
     * In the next cycle,
     * if cur_link, new_link is true, and state is LC_LINK_UP_STATE_DONE,
     * but detects new_fault, that is unpreferrable remote fault.
     * For all other cases, set state back to default LC_LINK_UP_STATE_NONE.
     */
    if ((cur_link) && (new_link) && (!new_fault) && (cur_fault) &&
        (lc->lc_up_proc_state[port] == LC_LINK_UP_STATE_START)) {
        lc->lc_up_proc_state[port] = LC_LINK_UP_STATE_DONE;
    } else if ((cur_link) && (new_link) && (new_fault) && (!cur_fault) &&
        (lc->lc_up_proc_state[port] == LC_LINK_UP_STATE_DONE)) {
        /* port link up is stable in the previous cycle, but detected new_fault.
           So ignore this remote fault */
        lc->lc_up_proc_state[port] = LC_LINK_UP_STATE_NONE;
        LOG_VERBOSE(BSL_LS_BCM_LINK, (BSL_META_U(unit,
                    "Unit %d, Port %s: ignored new fault\n"),
                    unit, SOC_PORT_NAME(unit, port)));
        return BCM_E_NONE;
    } else {
        lc->lc_up_proc_state[port] = LC_LINK_UP_STATE_NONE;
    }
#endif

#if defined(BCM_LINK_CHANGE_BENCHMARK)
    time_start = sal_time_usecs();
#endif /* BCM_LINK_CHANGE_BENCHMARK */

    /* In failed state, no link processing is performed until
       the state is cleared */
    if (!new_failed) {
        /*
         * If disabling, stop ingresses from sending any more traffic to
         * this port.
         */
        if (!new_link) {
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link, port);
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_remote_fault, port);
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_linkphy_coe) &&
                SOC_INFO(unit).linkphy_enabled) {
                if (IS_LP_PORT(unit, port)) {
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                            SOC_PBMP_PORT_REMOVE
                                (sop->lc_pbm_link, pp_port);
                        }
                    } else
#endif
                    {
                        for (pp_port = 0;
                                pp_port < SOC_INFO(unit).port_num_subport[port];
                                pp_port++) {
                            SOC_PBMP_PORT_REMOVE
                                (sop->lc_pbm_link,
                                 pp_port + SOC_INFO(unit).port_subport_base[port]);
                        }
                    }
                }
            }
            if (soc_feature(unit, soc_feature_subtag_coe) &&
                SOC_INFO(unit).subtag_enabled) {
                if (SOC_PBMP_MEMBER(SOC_INFO(unit).subtag_allowed_pbm, port)) {
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                            SOC_PBMP_PORT_REMOVE
                                (sop->lc_pbm_link, pp_port);
                        }
                    } else
#endif
                    {
                        for (pp_port = 0;
                                pp_port < SOC_INFO(unit).port_num_subport[port];
                                pp_port++) {
                            SOC_PBMP_PORT_REMOVE
                                (sop->lc_pbm_link,
                                 pp_port + SOC_INFO(unit).port_subport_base[port]);
                        }
                    }
                }
            }
#endif /* BCM_KATANA2_SUPPORT */
            SOC_PBMP_ASSIGN(pbm_link_fwd, sop->lc_pbm_link);
            SOC_PBMP_REMOVE(pbm_link_fwd, sop->lc_pbm_remote_fault);
            /*
            * If new_link down, Add linkdown_tx pbmp to forward pbmp
            * in order to add linkdown_tx pbmp to EPC_LINK_BMAPm
            */
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
           if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
               bcm_port_info_t port_info;
               bcm_port_info_t_init(&port_info);
               port_info.linkstatus = BCM_PORT_LINK_STATUS_DOWN;
               _bcm_stk_modport_map_linkscan_handler(unit, port, &port_info);
           }
#endif /* #(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */

            SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_linkdown_tx);
            SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_roe_linkdown_tx);
            rv = soc_link_fwd_set(unit, pbm_link_fwd);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "Port %s: soc_link_fwd_set failed: %s\n"),
                           SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                return rv;
            }
#if defined(BCM_HAWKEYE_SUPPORT)
            if (SOC_IS_HAWKEYE(unit)) {
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (READ_CMIC_TX_PAUSE_CAPABILITYr(unit, &mac_data));
                if (soc_feature(unit, soc_feature_hawkeye_a0_war)) {
                    if (((port - 1) >= 32) || (port < 1)) {
                        return BCM_E_INTERNAL;
                    }
                    mac_data &= ~(1 << (port-1));
                } else {
                    if (port >= 32) {
                        return BCM_E_INTERNAL;
                    }
                    mac_data &= ~(1 << port);
                }
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, mac_data));

                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (READ_CMIC_RX_PAUSE_CAPABILITYr(unit, &mac_data));
                if (soc_feature(unit, soc_feature_hawkeye_a0_war)) {
                    if (((port - 1) >= 32) || (port < 1)) {
                        return BCM_E_INTERNAL;
                    }
                    mac_data &= ~(1 << (port-1));
                } else {
                    if (port >= 32) {
                        return BCM_E_INTERNAL;
                    }
                    mac_data &= ~(1 << port);
                }

                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN
                    (WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, mac_data));
            }
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
            if (SOC_IS_HURRICANE (unit) || SOC_IS_TD_TT (unit) ||
                SOC_IS_HAWKEYE (unit) || SOC_IS_KATANAX(unit) ||
                SOC_IS_TRIUMPH3(unit) || SOC_IS_HURRICANE2(unit) ||
                SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND2(unit)) {
                /* EEE standard compliance Work Around:*/
                if(soc_feature(unit, soc_feature_eee)) {
                    /* Check if Native EEE is supported in MAC */
                    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                        bcmi_esw_portctrl_eee_enable_set(unit, port, 0);
                    }
                    else {
                        _bcm_port_info_access(unit, port, &port_info);
                        if (MAC_CONTROL_GET(port_info->p_mac, unit, port,
                                        SOC_MAC_CONTROL_EEE_ENABLE,
                                        &mac_val) == BCM_E_NONE) {
                            /* When port link goes down, disable EEE in MAC.  */
                            PORT_LOCK(unit);
                            MAC_CONTROL_SET(port_info->p_mac,
                                        unit, port,
                                        SOC_MAC_CONTROL_EEE_ENABLE, 0);
                            PORT_UNLOCK(unit);
                        }
                    }
                }
            }
#endif /* BCM_HURRICANE_SUPPORT || BCM_HAWKEYE_SUPORT || BCM_TRIDENT_SUPPORT */
        }

        /* Program MACs
         * (only if port is not forced, and link state changed, and
         *  not marked as linkdown tx when link going down)
         */

        if (!SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port) &&
            (cur_link != new_link) &&
            (!(SOC_PBMP_MEMBER(sop->lc_pbm_linkdown_tx, port) &&
               !new_link))) {
            if (!SOC_PBMP_MEMBER(sop->lc_pbm_fc, port)) {
                rv = bcm_esw_port_update(unit, port, new_link);
#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
                if (new_link) {
                    lc->lc_up_proc_state[port] = LC_LINK_UP_STATE_START;
                }
#endif
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                              (BSL_META_U(unit,
                                    "Port %s: bcm_port_update failed: %s\n"),
                                    SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                    return rv;
                }
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
                /* Enable clock recovery */
                if (soc_feature(unit, soc_feature_gmii_clkout) && new_link &&
                    !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANEX(unit) &&
                    !SOC_IS_TRIUMPH3(unit) && !SOC_IS_GREYHOUND(unit) &&
                    !SOC_IS_KATANA2(unit) && !SOC_IS_GREYHOUND2(unit)) {
                    uint32 val = 0, pvalr = 0, pval = 0;

                    SOC_IF_ERROR_RETURN
                        (READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &val));
                    pvalr = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                             val, PRI_PORT_SELf);
                    pval = SOC_INFO(unit).port_l2p_mapping[port];
                    if (pvalr && (pvalr == pval) &&
                        SOC_REG_IS_VALID(unit, CMIC_MISC_CONTROLr) &&
                        soc_reg_field_valid(unit, CMIC_MISC_CONTROLr, CLK_RECOVERY_PRI_RESETf)) {
                        /* Toggle primary reset */
                        SOC_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, CMIC_MISC_CONTROLr,
                                                    REG_PORT_ANY,
                                                    CLK_RECOVERY_PRI_RESETf,
                                                    0));
                        SOC_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, CMIC_MISC_CONTROLr,
                                                    REG_PORT_ANY,
                                                    CLK_RECOVERY_PRI_RESETf,
                                                    1));
                    }
                    pvalr = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                             val, BKUP_PORT_SELf);
                    if (pvalr && (pvalr == pval) &&
                        SOC_REG_IS_VALID(unit, CMIC_MISC_CONTROLr) &&
                        soc_reg_field_valid(unit, CMIC_MISC_CONTROLr, CLK_RECOVERY_BKUP_RESETf)) {
                        /* Toggle backup reset */
                        SOC_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, CMIC_MISC_CONTROLr,
                                                    REG_PORT_ANY,
                                                    CLK_RECOVERY_BKUP_RESETf,
                                                    0));
                        SOC_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, CMIC_MISC_CONTROLr,
                                                    REG_PORT_ANY,
                                                    CLK_RECOVERY_BKUP_RESETf,
                                                    1));
                    }
                }
#endif /* TRIUMPH2, APOLLO, VALKYRIE2 */
            }

#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
            /* Physical link completed, now we can check fault */
            if ((soc_feature(unit, soc_feature_bigmac_fault_stat) ||
                soc_feature(unit, soc_feature_xlmac)) &&
                new_link) {
                /*
                 * Now our rules are:
                 * 1) The chip supports it:  feature check
                 * 2) The link state is coming from the PHY: not override.
                 * 3) The physical link is now up, so can check fault.
                 */
#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
                new_fault = FALSE;
#endif
                rv = _bcm_esw_link_fault_get(unit, port, &new_fault);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                              (BSL_META_U(unit,
                                 "Port %s: Failed to read fault status: %s\n"),
                                 SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                    return rv;
                }
            }
#endif /* HERC15, FIREBOLT */
        }

        /*
         * If enabling, allow traffic to go to this port.
         */
        if (new_link) {

            SOC_PBMP_PORT_ADD(sop->lc_pbm_link, port);
#if defined(BCM_KATANA2_SUPPORT)
            if (soc_feature(unit, soc_feature_linkphy_coe) &&
                SOC_INFO(unit).linkphy_enabled) {
                if (IS_LP_PORT(unit, port)) {
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                            SOC_PBMP_PORT_ADD
                                (sop->lc_pbm_link, pp_port);
                        }
                    } else
#endif
                    {
                        for (pp_port = 0;
                                pp_port < SOC_INFO(unit).port_num_subport[port];
                                pp_port++) {
                            SOC_PBMP_PORT_ADD
                                (sop->lc_pbm_link,
                                 pp_port + SOC_INFO(unit).port_subport_base[port]);
                        }
                    }
                }
            }
            if (soc_feature(unit, soc_feature_subtag_coe) &&
                SOC_INFO(unit).subtag_enabled) {
                if (IS_SUBTAG_PORT(unit, port)) {
#if defined BCM_METROLITE_SUPPORT
                    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                            SOC_PBMP_PORT_ADD
                                (sop->lc_pbm_link, pp_port);
                        }
                    } else
#endif
                    {
                        for (pp_port = 0;
                                pp_port < SOC_INFO(unit).port_num_subport[port];
                                pp_port++) {
                            SOC_PBMP_PORT_ADD
                                (sop->lc_pbm_link,
                                 pp_port + SOC_INFO(unit).port_subport_base[port]);
                        }
                    }
                }
            }
#endif /* BCM_KATANA2_SUPPORT */
            SOC_PBMP_ASSIGN(pbm_link_fwd, sop->lc_pbm_link);

            if (!new_fault) {
                SOC_PBMP_PORT_REMOVE(sop->lc_pbm_remote_fault, port);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                if (SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
                    /* Previously failed port completing recovery */
                    PORT_LOCK(unit);
                    rv = _bcm_esw_link_failover_link_up(unit, port);
                    PORT_UNLOCK(unit);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_LINK,
                                  (BSL_META_U(unit,
                                              "Port %s: failed link recovery "
                                              "unsuccessful: %s\n"),
                                   SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                        return rv;
                    }
                    SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failed_clear, port);
                }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
            } else {
                SOC_PBMP_PORT_ADD(sop->lc_pbm_remote_fault, port);
            }

            SOC_PBMP_REMOVE(pbm_link_fwd, sop->lc_pbm_remote_fault);
            /*
            * If new_link up, Add linkdown_tx pbmp to forward pbmp
            * in order to add linkdown_tx pbmp to EPC_LINK_BMAPm
            */
            SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_linkdown_tx);
            SOC_PBMP_OR(pbm_link_fwd, sop->lc_pbm_roe_linkdown_tx);
            rv = soc_link_fwd_set(unit, pbm_link_fwd);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "Port %s: soc_link_fwd_set failed: %s\n"),
                           SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                return rv;
            }
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
            /* Timing and Execution sequence sometimes might need to enforce
             * a port update even though only a fault state has changed.
             * This can happen in the case where a Disable followed by an enable
             * event is missed by Linkscan and where new and current links are up.
             * This will cause the MAC and PHY to be out of sync unless an update
             * is performed
             */
            if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))
                || SOC_IS_TRIDENT(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                if ((cur_link == new_link) && (cur_fault != new_fault)
                    && (!new_fault)) {
#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
                    if (lc->lc_up_proc_state[port] == LC_LINK_UP_STATE_NONE) {
#endif
                    rv = bcm_esw_port_update(unit, port, new_link);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_LINK,
                                  (BSL_META_U(unit,
                                        "Port %s: bcm_port_update failed: %s\n"),
                                        SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                        return rv;
                    }
#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
                    }
#endif
                }
            }
#endif

#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
            if (new_fault) {
                if (soc_feature(unit, soc_feature_xmac)) {
                    _bcm_port_info_access(unit, port, &port_info);
                    PORT_LOCK(unit);
                    MAC_ENABLE_SET(port_info->p_mac, unit, port, FALSE);
                    MAC_ENABLE_SET(port_info->p_mac, unit, port, TRUE);
                    PORT_UNLOCK(unit);
                }
            }
#endif /* BCM_HURRICANE_SUPPORT || BCM_TRIDENT_SUPPORT || BCM_HAWKEYE_SUPPORT */
        }

#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit) && new_link) {
            BCM_IF_ERROR_RETURN(_hawkeye_new_link_check(unit,port));
        }
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_TRIDENT_SUPPORT) || \
    defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if (SOC_IS_HURRICANE (unit) || SOC_IS_TD_TT (unit) ||
            SOC_IS_HAWKEYE (unit) || SOC_IS_KATANAX(unit) ||
            SOC_IS_TRIUMPH3(unit) || SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            /* EEE standard compliance Work Around:*/
            if(soc_feature(unit, soc_feature_eee)) {
                phy_ctrl_t   *ext_pc;
                uint32 phy_val;

#ifdef PORTMOD_SUPPORT
                /* Call Port Control module */
                if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                    rv = bcmi_esw_portctrl_eee_enable_get(unit, port, &mac_val);

                    if (BCM_SUCCESS(rv)) {
                        phymod_phy_access_t phy_access[1+MAX_PHYN];
                        int nof_phys = 0;
                        portmod_access_get_params_t params;
                        int is_most_ext;

                        BCM_IF_ERROR_RETURN
                            (portmod_access_get_params_t_init(unit, &params));

                        params.phyn = 0; /* Only find serdes */
                        BCM_IF_ERROR_RETURN
                            (portmod_port_phy_lane_access_get(
                                 unit, port, &params, (1+MAX_PHYN), phy_access,
                                 &nof_phys,&is_most_ext));

                        /* If serdes is the outmost phy, then there is no ext phy */
                        if (is_most_ext &&
                            bcmi_esw_portctrl_phy_control_get(
                                unit, port,
                                BCM_PORT_PHY_CONTROL_EEE,
                                &phy_val) != SOC_E_UNAVAIL) {
                            lc->lc_up_time[port-1] = sal_time();
                            /* Record the port for EEE update pending */
                            BCM_PBMP_PORT_ADD(lc->lc_pbm_eee_pending, port);
                        } else if (new_link &&
                                   (bcm_esw_port_ability_remote_get
                                   (unit, port, &remote_advert) == SOC_E_NONE)) {
                            /* Maintain link up timer.
                             * EEE WA : delay EEE enable in MAC by 1sec once
                             * the link is up
                             */
                            LOG_VERBOSE(BSL_LS_BCM_LINK,
                                        (BSL_META_U(unit,
                                         "u=%d p=%d remote_advert.eee = %d\n"),
                                         unit, port, remote_advert.eee ? 1 : 0));
                            if (remote_advert.eee) {
                                lc->lc_up_time[port-1] = sal_time();
                                /* Record the port for EEE update pending */
                                BCM_PBMP_PORT_ADD(lc->lc_pbm_eee_pending, port);
                            } else {
                                lc->lc_up_time[port-1] = -1;
                            }
                        } else {
                            lc->lc_up_time[port-1] = -1;
                        }
                    } else {
                        lc->lc_up_time[port-1] = -1;
                    }
                } else
#endif
                {
                    ext_pc = EXT_PHY_SW_STATE(unit, port);
                /* Check if Native EEE is supported in MAC */
                _bcm_port_info_access(unit, port, &port_info);
                if (MAC_CONTROL_GET(port_info->p_mac, unit, port,
                                    SOC_MAC_CONTROL_EEE_ENABLE,
                                    &mac_val) == BCM_E_NONE) {
                        PORT_LOCK(unit);
                        if ((NULL == ext_pc) &&
                            (soc_phyctrl_control_get(unit, port,
                                                     BCM_PORT_PHY_CONTROL_EEE,
                                                     &phy_val) != SOC_E_UNAVAIL)) {
                            /*
                            * If devices which have serDes supporting Native EEE but without external
                            * PHY attached to the port, we enable Native EEE on mac.
                            */
                            lc->lc_up_time[port-1] = sal_time();
                            /* Record the port for EEE update pending */
                            BCM_PBMP_PORT_ADD(lc->lc_pbm_eee_pending, port);
                        } else if (new_link &&
                        (bcm_esw_port_ability_remote_get
                         (unit, port, &remote_advert) == SOC_E_NONE)) {
                        /* Maintain link up timer.
                         * EEE WA : delay EEE enable in MAC by 1sec once
                         * the link is up
                         */
                        LOG_VERBOSE(BSL_LS_BCM_LINK,
                                    (BSL_META_U(unit,
                                     "u=%d p=%d remote_advert.eee = %d\n"),
                                     unit, port, remote_advert.eee ? 1 : 0));
                            if (remote_advert.eee) {
                                lc->lc_up_time[port-1] = sal_time();
                                /* Record the port for EEE update pending */
                                BCM_PBMP_PORT_ADD(lc->lc_pbm_eee_pending, port);
                            } else {
                                lc->lc_up_time[port-1] = -1;
                            }
                    } else {
                        lc->lc_up_time[port-1] = -1;
                    }
                        PORT_UNLOCK(unit);
                }
            }
        }
        }
#endif /* BCM_HURRICANE_SUPPORT || BCM_HAWKEYE_SUPORT || BCM_TRIDENT_SUPPORT */
    }

    /*
     * Call registered handlers with complete link info.
     * Display link status message, if requested.
     *
     * In case link status changed again for bcm_port_info_get,
     * overwrite the linkstatus field with logical_link.  This ensures
     * the handler is presented with a consistent alternating
     * sequence of link up/down.
     */

    if ((flags & _BCM_LINK_STATUS_NO_CALLBACK) == 0) {
        notify = (cur_failed != new_failed) ||  /* Change of failed state */
                 (cur_link && new_link)     ||  /* => cur_fault != new_fault */
                 (!new_link && !cur_fault)  ||  /* Port down w/o prior fault */
                 (new_link && !new_fault);      /* Port up w/o fault */
    }

    logical_link = new_failed ? BCM_PORT_LINK_STATUS_FAILED :
                   (new_fault ? BCM_PORT_LINK_STATUS_REMOTE_FAULT :
                   (new_link ? BCM_PORT_LINK_STATUS_UP :
                    BCM_PORT_LINK_STATUS_DOWN));

    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Unit %d, Port %s: Link: Current %s, New %s\n"
                            "\tFault: Current %s, New %s\n"
                            "\tFailed: Current %s, New %s\n"
                            "\tNotify %s, Logical link %s\n"),
                 unit,
                 SOC_PORT_NAME(unit, port),
                 cur_link ? "up" : "down",
                 new_link ? "up" : "down",
                 cur_fault ? "yes" : "no",
                 new_fault ? "yes" : "no",
                 cur_failed ? "yes" : "no",
                 new_failed ? "yes" : "no",
                 notify ? "yes" : "no",
                 ll_status[logical_link]));

    /* bcm_port_info_t info initialized */
    bcm_port_info_t_init(&info);

    if (notify) {
        if (!new_link) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_link_port_info_skip_get(unit, port, &info_skip));
        }
        if (!info_skip) {
            rv = bcm_esw_port_info_get(unit, port, &info);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                    "Port %s: bcm_port_info_get failed: %s\n"),
                                    SOC_PORT_NAME(unit, port),
                                    bcm_errmsg(rv)));
                return rv;
            }

            if ((info.speed == 0) && (new_link == TRUE)) {
                LOG_WARN(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                     "Port %s: Link Up with Speed 0M!\n"),
                          SOC_PORT_NAME(unit, port)));
            }

            if (soc_feature(unit, soc_feature_asf)) {
                asf_link =
                    ((logical_link != BCM_PORT_LINK_STATUS_DOWN) &&
                     (logical_link != BCM_PORT_LINK_STATUS_REMOTE_FAULT));
                rv = _bcm_esw_linkscan_update_asf(unit, port, asf_link,
                                                  info.speed, info.duplex);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                              (BSL_META_U(unit,
                                   "Port %s: linkscan ASF update failed: %s\n"),
                                   SOC_PORT_NAME(unit, port),
                                   bcm_errmsg(rv)));
                    return rv;
                }
            }
        } else {
            info.linkstatus = BCM_PORT_LINK_STATUS_DOWN;
        }
    }

#if defined(BCM_LINK_CHANGE_BENCHMARK)
    time_end = sal_time_usecs();
#endif /* BCM_LINK_CHANGE_BENCHMARK */

    if (cur_failed != new_failed) {
        if (new_failed) {
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Port %s: failed\n"),
                      SOC_PORT_NAME(unit, port)));
        } else {
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Port %s: failed state cleared\n"),
                      SOC_PORT_NAME(unit, port)));
        }
    }

    if (cur_link != new_link) {
        if (new_link) {
            if (hw_link_port) {
                LOG_INFO(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                    "Port %s: link up took %d usecs since last "
                                    "HW interrupt\n"),
                          SOC_PORT_NAME(unit, port),
                          SAL_USECS_SUB(sal_time_usecs(),
                          last_time_hw_link_up[unit])));
            }
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Port %s: link %s (%dMb %s %s)\n"),
                      SOC_PORT_NAME(unit, port),
                      ll_status[logical_link],
                      info.speed,
                      info.duplex ? "Full Duplex" : "Half Duplex",
                      PHY_FIBER_MODE(unit, port) ?
                      "Fiber" : "Copper"));
#if defined(BCM_LINK_CHANGE_BENCHMARK)
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Link %s processing took %d usecs\n"),
                                 ll_status[logical_link],
                                 SAL_USECS_SUB(time_end, time_start)));
#endif /* BCM_LINK_CHANGE_BENCHMARK */
#ifdef BCM_KATANA2_SUPPORT
   if (SOC_IS_KATANA2(unit) && (!SOC_IS_SABER2(unit))) {
       LOG_INFO(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "p=%d Applied DOWNSIZER \n"), port));
       /* To recover from clock glitch(not always), Serdes init sequence
          requires write to reset frequency downsizer logic in MXQPORT. */
       SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
       bypass = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, BYPASS_DOWNSIZERf);
       wc_10g_21g_sel = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, WC_10G_21G_SELf);
       if ((bypass == 0) && (wc_10g_21g_sel == 0)) {
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, SW_RESET_DOWNSIZERf , 1);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, SW_RESET_DOWNSIZERf , 0);
            SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
       }
       soc_katana2_mmu_port_enable_set(unit, port, 1);
   }
#endif
        } else {
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Port %s: link down\n"),
                      SOC_PORT_NAME(unit, port)));


#if defined(BCM_LINK_CHANGE_BENCHMARK)
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Link down processing took %d usecs\n"),
                      SAL_USECS_SUB(time_end, time_start)));
#endif /* BCM_LINK_CHANGE_BENCHMARK */
        }
    }

    if (new_fault) {
        LOG_INFO(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                             "Port %s: fault detected\n"),
                             SOC_PORT_NAME(unit, port)));
    } else if (cur_fault) {
        LOG_INFO(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                             "Port %s: fault cleared\n"),
                             SOC_PORT_NAME(unit, port)));
    }

    if (notify) {
        LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                          "Unit %d, Port %s: logical link notification - %s\n"),
                          unit, SOC_PORT_NAME(unit, port),
                          ll_status[logical_link]));
        info.linkstatus = logical_link;
#if defined(INCLUDE_UC_ULINK)
        soc_cmic_uc_link_change_flag = 1;
#endif
        BCM_API_XLATE_PORT_P2A(unit, &port);

        for (lh = lc->lc_handler; lh; lh = lh_next) {
            /*
             * save the next linkscan handler first, in case current handler
             * unregister itself inside the handler function
             */
            lh_next = lh->lh_next;
            /*
             * The called function must not invoke any API that may result in
             * a deadlock.
             */
            lh->lh_f(unit, port, &info);
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_esw_linkscan_update
 * Purpose:
 *      Check for a change in link status on each link.  If a change
 *      is detected, call bcm_port_update to program the MACs for that
 *      link, and call the list of registered handlers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbm - bit map of ports to scan.
 * Returns:
 *      Nothing.
 */

STATIC void
_bcm_esw_linkscan_update(int unit, uint32 flags, pbmp_t pbm)
{
    soc_persist_t *sop = SOC_PERSIST(unit);
    ls_cntl_t *lc = link_control[unit];
    pbmp_t save_link_change;
    int rv;
    bcm_port_t port;

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)){
        int oamp_enable;
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
        /* Skip OAMP port for saber2 device */
        if(oamp_enable) {
            SOC_PBMP_PORT_REMOVE(pbm,
                    _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER);
        }
    }
#endif

    /*
     * Suspend hardware link scan here to avoid overhead of pause/resume
     * on MDIO accesses. Ideally this would not be required if the source
     * of the interrupt is the signal from the internal Serdes but
     * we still need to do this due to work-arounds in phy drivers
     * where we need to rely on SW linkscan to detect link up.
     */
    soc_linkscan_pause(unit);           /* Suspend linkscan */

    PBMP_ITER(pbm, port) {
        ls_errstate_t *err;

        LC_LOCK(unit);
        err = &lc->lc_error[port];

        /* Port could be removed by flexport operation, so check if port
         * still active again before updating link */
        if (!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit),port)) {
            err->wait = 0;
            err->count = 0;
            LC_UNLOCK(unit);
            continue;
        }

        if (err->wait) {  /* Port waiting in error state */
            if (SAL_USECS_SUB(sal_time_usecs(), err->time) >= err->delay) {
                err->wait = 0;  /* Exit error state */
                err->count = 0;

                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "Port %s: restored\n"),
                           SOC_PORT_NAME(unit, port)));
            } else {
                LC_UNLOCK(unit);
                continue;
            }
        }

        save_link_change = sop->lc_pbm_link_change;

        rv = _bcm_esw_linkscan_update_port(unit, flags, port);

        if (BCM_FAILURE(rv)) {
            sop->lc_pbm_link_change = save_link_change;

            if (++err->count >= err->limit && err->limit > 0) {
                /* Enter error state */

                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "Port %s: temporarily removed from linkscan\n"),
                           SOC_PORT_NAME(unit, port)));

                err->time = sal_time_usecs();
                err->wait = 1;
            }

#ifdef BCM_RCPU_SUPPORT
            if (SOC_IS_RCPU_ONLY(unit) && (0 == lc->lc_us)) {
                /*
                 * Check if it's been stopped since it may take long time
                 * for rcpu to scan all ports.
                 */
                    LC_UNLOCK(unit);
                return;
            }
#endif /* BCM_RCPU_SUPPORT */
        } else if (err->count > 0) {
            err->count--;  /* Reprieve */
        }
        LC_UNLOCK(unit);
    }
#if defined(INCLUDE_UC_ULINK)
    /* link change */
    if(soc_feature(unit, soc_feature_uc_ulink) &&
        (soc_cmic_uc_link_change_flag != 0)) {

        rv = soc_cmic_uc_pkd_link_notify(unit);
        if (rv == SOC_E_NONE) {
            soc_cmic_uc_link_change_flag = 0;
        }
    }
#endif
    soc_linkscan_continue(unit);        /* Restart H/W link scan */
}


/*
 * Function:
 *      bcm_esw_linkscan_update
 * Purpose:
 *      Check for a change in link status on each link.  If a change
 *      is detected, call bcm_port_update to program the MACs for that
 *      link, and call the list of registered handlers.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbm - bit map of ports to scan.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_esw_linkscan_update(int unit, bcm_pbmp_t pbm)
{
    LC_CHECK_INIT(unit);

    if (BCM_PBMP_IS_NULL(pbm)) {
        return BCM_E_NONE;
    }

    _bcm_esw_linkscan_update(unit, 0 /*flags*/, pbm);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_linkscan_hw_interrupt
 * Purpose:
 *      Link scan interrupt handler if using CMIC_LINK_SCAN.
 * Parameters:
 *      StrataSwitch unit #
 * Returns:
 *      Nothing
 */

STATIC void
_bcm_esw_linkscan_hw_interrupt(int unit)
{
    ls_cntl_t           *lc = link_control[unit];

    if ((NULL != lc) && (NULL != lc->lc_sema)) {
        lc->lc_hw_change = 1;
        sal_sem_give(lc->lc_sema);
    }
    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Linkscan interrupt unit %d\n"), unit));
}

/*
 * Function:
 *         bcm_link_change
 * Purpose:
 *         Force a transient link down event to be recognized,
 *         regardless of the current physical up/down state of the
 *         port.  This does not affect the physical link status.
 * Parameters:
 *         unit - StrataSwitch Unit #.
 *         pbm - Bitmap of ports to operate on.
 * Returns:
 *         BCM_E_XXX
 */

int
bcm_esw_link_change(int unit, pbmp_t pbm)
{
    soc_persist_t *sop = SOC_PERSIST(unit);
    ls_cntl_t *lc = link_control[unit];

    LC_CHECK_INIT(unit);

    if (lc->lc_warm_boot) {
        /* Do not change link status when recovering from Warm Boot. */
        return BCM_E_NONE;
    }

    SOC_PBMP_AND(pbm, PBMP_PORT_ALL(unit));

#ifdef BCM_RCPU_SUPPORT
    if (SOC_IS_RCPU_ONLY(unit) && SOC_PORT_VALID(unit, RCPU_PORT(unit))) {
        /* Do not update link status for rcpu port. */
        SOC_PBMP_PORT_REMOVE(pbm, RCPU_PORT(unit));
    }
#endif /* BCM_RCPU_SUPPORT */

    LC_LOCK(unit);
    SOC_PBMP_OR(sop->lc_pbm_link_change, pbm);
    SOC_PBMP_ASSIGN(pbm, sop->lc_pbm_link_change);
    SOC_PBMP_AND(pbm, lc->lc_pbm_hw);

    /* If there's any hardware linkscan port in lc_pbm_link_change,
     * then mark it as hardware link change event.
     */
    if (SOC_PBMP_NOT_NULL(pbm)) {
        lc->lc_hw_change = 1;
    }
    LC_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->lc_sema != NULL) {
        sal_sem_give(lc->lc_sema);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_link_get
 * Purpose:
 *      Return linkscan's current link status for the given port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 *      link - (OUT) Current link status.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_DISABLED - Port not being scanned.
 * Note:
 *      This routine does not acquire the LC_LOCK, as it only reads a
 *      snapshot of the link bitmaps.  It also cannot hold the LC_LOCK
 *      since it is called indirectly from the linkscan thread
 *      when requesting port info.
 */

int
_bcm_esw_link_get(int unit, bcm_port_t port, int *link)
{
    soc_persist_t *sop = SOC_PERSIST(unit);

    if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port)) {
        *link =  SOC_PBMP_MEMBER(sop->lc_pbm_override_link, port);
        return (SOC_E_NONE);
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_linkscan_enable_port_get(unit, port));

    if (SOC_PBMP_MEMBER(sop->lc_pbm_failed, port)) {
        *link = BCM_PORT_LINK_STATUS_FAILED;
    } else {
        *link = (SOC_PBMP_MEMBER(sop->lc_pbm_link, port) &&
                 !SOC_PBMP_MEMBER(sop->lc_pbm_remote_fault, port)) ?
            BCM_PORT_LINK_STATUS_UP : BCM_PORT_LINK_STATUS_DOWN;
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_link_force
 * Purpose:
 *      Set linkscan's current link status for a port.
 * Parameters:
 *      unit  - StrataSwitch Unit #.
 *      flags - flags
 *      port  - StrataSwitch physical port number.
 *      force - If TRUE, link status is forced to new link status.
 *              If FALSE, link status is no longer forced.
 *      link  - New link status.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 * Notes:
 *      When a link is forced up or down, linkscan will stop scanning
 *      that port and _bcm_link_get will always return the forced status.
 *      This is used for purposes such as when a link is placed in MAC
 *      loopback.  If software forces a link up, it is responsible for
 *      configuring that port.
 */

int
_bcm_esw_link_force(int unit, uint32 flags, bcm_port_t port,
                    int force, int link)
{
    soc_persist_t *sop = SOC_PERSIST(unit);
    ls_cntl_t *lc = link_control[unit];
    pbmp_t pbm;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (force) {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_link, port);
        if (link) {
            if (lc->lc_warm_boot) {
                /* Don't update ports when recovering from Warm Boot. */
                SOC_PBMP_PORT_ADD(sop->lc_pbm_link, port);
                SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link_change, port);
            }
            SOC_PBMP_PORT_ADD(sop->lc_pbm_override_link, port);
        }
        SOC_PBMP_PORT_ADD(sop->lc_pbm_override_ports, port);
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_ports, port);
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_override_link, port);

        /* Don't set link change (link down) if port is only on
           hardware linkscan. This is because a hardware interrupt
           may never come for a port that is currently up. */
        if (!(SOC_PBMP_MEMBER(lc->lc_pbm_hw, port) &&
              !SOC_PBMP_MEMBER(lc->lc_pbm_sw, port))) {
            SOC_PBMP_PORT_ADD(sop->lc_pbm_link_change, port);
        }
    }

    /*
     * Force immediate update to just this port - this allows loopback
     * forces to take effect immediately.
     */
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    _bcm_esw_linkscan_update(unit, flags, pbm);

    LC_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->lc_sema != NULL) {
        sal_sem_give(lc->lc_sema);
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_link_down_tx_set
 * Purpose:
 *      Mark port as linkdown TX ensuring MAC will transmit while
 *      link is down
 * Parameters:
 *      unit   - StrataSwitch Unit #.
 *      port   - StrataSwitch physical port number.
 *      enable - If TRUE, and link is down, enable MAC
 *               If FALSE, and link is down, disable MAC
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 * Notes:
 */

int
_bcm_esw_link_down_tx_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);
    bcm_pbmp_t pbm;
    int port_enable;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_port_enable_get(unit, port, &port_enable));

    LC_LOCK(unit);

    if (enable) {
        SOC_PBMP_PORT_ADD(sop->lc_pbm_linkdown_tx, port);
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_linkdown_tx, port);
    }
    /* If port is enable and link is down, must enable MAC */
    if (port_enable && !SOC_PBMP_MEMBER(sop->lc_pbm_link, port)) {
        rv = bcm_esw_port_update(unit, port, enable);
    }

    LC_UNLOCK(unit);

    if (BCM_SUCCESS(rv) && !SAL_BOOT_SIMULATION) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        (void)bcm_esw_link_change(unit, pbm);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_roe_link_down_tx_set
 * Purpose:
 *      Mark port as linkdown TX ensuring MAC will transmit while
 *      link is down
 * Parameters:
 *      unit   - StrataSwitch Unit #.
 *      port   - StrataSwitch physical port number.
 *      enable - If TRUE, and link is down, enable MAC
 *               If FALSE, and link is down, disable MAC
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 * Notes:
 */

int
_bcm_esw_roe_link_down_tx_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);
    pbmp_t mask;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port) ||
        !IS_ROE_PORT(unit,port) ) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    if (enable) {
        SOC_PBMP_PORT_ADD(sop->lc_pbm_roe_linkdown_tx, port);
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_roe_linkdown_tx, port);
    }

    rv = bcm_esw_roe_tx_port_update(unit, port, enable);

    soc_link_fwd_get(unit, &mask);
    if (enable) {
        BCM_PBMP_PORT_ADD(mask,port);
    } else {
        BCM_PBMP_PORT_REMOVE(mask,port);
    }
    rv = soc_link_fwd_set(unit, mask);

    LC_UNLOCK(unit);

    return rv;
}
int
_bcm_esw_roe_link_down_rx_set(int unit, bcm_port_t port, int enable)
{

    int rv = BCM_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port) ||
        !IS_ROE_PORT(unit,port) ) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    if (enable) {
        SOC_PBMP_PORT_ADD(sop->lc_pbm_roe_linkdown_rx, port);
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_roe_linkdown_rx, port);
    }


    rv = bcm_esw_roe_rx_port_update(unit, port, enable);
    LC_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      _bcm_esw_link_down_tx_get
 * Purpose:
 *      Get the link down transmit function status
 * Parameters:
 *      unit   - StrataSwitch Unit #.
 *      port   - StrataSwitch physical port number.
 *      *enable - return link down transmit status
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_PORT - Invalid port.
 * Notes:
 */

int
_bcm_esw_link_down_tx_get(int unit, bcm_port_t port, int *enable)
{
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (SOC_PBMP_MEMBER(SOC_PERSIST(unit)->lc_pbm_linkdown_tx, port)) {
        *enable = 1;
    } else {
        *enable = 0;
    }

    return BCM_E_NONE;
}

int
_bcm_esw_roe_link_down_tx_get(int unit, bcm_port_t port, int *enable)
{
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port) ||
        !IS_ROE_PORT(unit,port) ) {
        return BCM_E_PORT;
    }

    if (SOC_PBMP_MEMBER(SOC_PERSIST(unit)->lc_pbm_roe_linkdown_tx, port)) {
        *enable = 1;
    } else {
        *enable = 0;
    }

    return BCM_E_NONE;
}

int
_bcm_esw_roe_link_down_rx_get(int unit, bcm_port_t port, int *enable)
{
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port) ||
        !IS_ROE_PORT(unit,port) ) {
        return BCM_E_PORT;
    }

    if (SOC_PBMP_MEMBER(SOC_PERSIST(unit)->lc_pbm_roe_linkdown_rx, port)) {
        *enable = 1;
    } else {
        *enable = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_failover_set
 * Purpose:
 *      Set whether the given port is configured for LAG failover.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 *      enable - LAG failover mode.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 *      BCM_E_DISABLED - Port not being scanned.
 * Notes:
 *      Called by trunk failover configuration.
 */

int
_bcm_esw_link_failover_set(int unit, bcm_port_t port, int enable)
{
    soc_persist_t *sop = SOC_PERSIST(unit);
    uint32 val;

    LC_CHECK_INIT(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (enable) {
        if (SOC_PBMP_MEMBER(sop->lc_pbm_failed, port) ||
            SOC_PBMP_MEMBER(sop->lc_pbm_failed_clear, port)) {
            /* Not yet recovered from previous failover.
             * Must wait until resolved. */
            LC_UNLOCK(unit);
            return BCM_E_PORT;
        } else {
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                bcmi_esw_portctrl_lag_failover_status_toggle(unit, port);
            } else {
                soc_reg_t lag_failover_config_reg;

                if (SOC_IS_SABER2(unit) && SOC_REG_PORT_VALID(unit,
                      LAG_FAILOVER_CONFIGr, port)) {
                    lag_failover_config_reg = LAG_FAILOVER_CONFIGr;
                } else if ((IS_CL_PORT(unit, port)) && (SOC_REG_IS_VALID(unit, CLPORT_LAG_FAILOVER_CONFIGr))) {
                    lag_failover_config_reg = CLPORT_LAG_FAILOVER_CONFIGr;
                } else if (SOC_REG_IS_VALID(unit, XLPORT_LAG_FAILOVER_CONFIGr)) {
                    lag_failover_config_reg = XLPORT_LAG_FAILOVER_CONFIGr;
                } else if (SOC_REG_IS_VALID(unit, LAG_FAILOVER_CONFIGr)) {
                    lag_failover_config_reg = LAG_FAILOVER_CONFIGr;
                } else {
                    lag_failover_config_reg = GXPORT_LAG_FAILOVER_CONFIGr;
                }

                /* Toggle link bit to notify IPIPE on link up */
                _BCM_LINK_IF_ERROR_RETURN
                    (soc_reg32_get(unit, lag_failover_config_reg, port, 0, &val));
                soc_reg_field_set(unit, lag_failover_config_reg, &val,
                                  LINK_STATUS_UPf, 1);
                _BCM_LINK_IF_ERROR_RETURN
                    (soc_reg32_set(unit, lag_failover_config_reg, port, 0, val));
                soc_reg_field_set(unit, lag_failover_config_reg, &val,
                                  LINK_STATUS_UPf, 0);
                _BCM_LINK_IF_ERROR_RETURN
                    (soc_reg32_set(unit, lag_failover_config_reg, port, 0, val));
            }
            SOC_PBMP_PORT_ADD(sop->lc_pbm_failover, port);
        }
    } else {
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_failover, port);
    }
    LC_UNLOCK(unit);

    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Unit %d: LAG failover: Port %d - %s\n"),
                 unit, port, enable ? "enabled" : "disabled"));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_failed_clear
 * Purpose:
 *      Allow a LAG failover port in failed state to complete link down
 *      processing.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch physical port number.
 * Returns:
 *      BCM_E_NONE - Success
 *      BCM_E_INIT - Not initialized.
 *      BCM_E_PORT - Not a valid port or port not in failed state.
 *      BCM_E_DISABLED - Port not being scanned.
 */

int
_bcm_esw_link_failed_clear(int unit, bcm_port_t port)
{
    soc_persist_t *sop = SOC_PERSIST(unit);

    LC_CHECK_INIT(unit);

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (!soc_feature(unit, soc_feature_port_lag_failover)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);

    if (!SOC_PBMP_MEMBER(sop->lc_pbm_failed, port)) {
        /* Not a failed port */
        LC_UNLOCK(unit);
        return BCM_E_PORT;
    } else if (SOC_PBMP_MEMBER(sop->lc_pbm_failover, port)) {
        /* Port still in trunk, can't clear yet */
        LC_UNLOCK(unit);
        return BCM_E_PORT;
    }

    /* Signal to finish link down processing in update routine */
    SOC_PBMP_PORT_ADD(sop->lc_pbm_failed_clear, port);
    LC_UNLOCK(unit);
    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Unit %d: LAG failover failed state clear set: Port %d\n"),
                 unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_fast_set
 * Purpose:
 *      Enables or disables fast linkscan for given port.
 * Parameters:
 *      unit   - StrataSwitch Unit #.
 *      port   - Port number.
 *      enable - Indicates whether to enable or disable fast linkscan on port.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_link_fast_set(int unit, bcm_port_t port, int enable)
{
    ls_cntl_t   *lc = link_control[unit];

    LC_CHECK_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    LC_LOCK(unit);

    if (enable) {
        BCM_PBMP_PORT_ADD(lc->lc_pbm_fast, port);
    } else {
        BCM_PBMP_PORT_REMOVE(lc->lc_pbm_fast, port);
    }

    LC_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_link_fast_get
 * Purpose:
 *      Gets fast linkscan property for given port.
 * Parameters:
 *      unit   - StrataSwitch Unit #.
 *      port   - Port number.
 *      enable - (OUT) Returns fast linkscan property for port.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_link_fast_get(int unit, bcm_port_t port, int *enable)
{
    ls_cntl_t   *lc = link_control[unit];

    LC_CHECK_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));

    if (BCM_PBMP_MEMBER(lc->lc_pbm_fast, port)) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }

    return BCM_E_NONE;
}

/* TRUE: need to call bcm_esw_port_update
*   FALSE: need not to call bcm_esw_port_update
*/
STATIC int
_bcm_esw_port_soft_reset_is_set(int unit, soc_port_t port)
{
    int rv = BCM_E_NONE;
    int soft_reset_val = 1;

#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        rv = portmod_port_mac_reset_get(unit, port, &soft_reset_val);
    } else
#endif
    {
        _bcm_port_info_t *port_info;
        _bcm_port_info_access(unit, port, &port_info);
        rv = MAC_CONTROL_GET(port_info->p_mac, unit, port,
                             SOC_MAC_CONTROL_SW_RESET, &soft_reset_val);
    }

    if (BCM_E_NONE != rv) {
        return TRUE;
    } else {
        return ((soft_reset_val) ? TRUE : FALSE);
    }
}

STATIC void
_bcm_esw_port_update_before_linkscan_exit(int unit, pbmp_t port_pbmp)
{
    int enable = 0, soft_reset = 0;
    soc_port_t        port;
    int rv = 0, hw_linkscan_mode = 0, link = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);

    BCM_PBMP_ITER(port_pbmp, port) {
#ifdef BCM_RCPU_SUPPORT
        if (SOC_IS_RCPU_ONLY(unit) && IS_RCPU_PORT(unit, port)) {
            continue;
        }
#endif /* BCM_RCPU_SUPPORT */
        if (BCM_SUCCESS(bcm_esw_port_enable_get(unit, port, &enable))) {
            if (enable)  {
                soft_reset = _bcm_esw_port_soft_reset_is_set(unit, port);

                if (soft_reset == 0) {
                    /* when soft_reset == 0, if sdk calls  bcm_esw_port_update,
                    * mac can't work normally */
                    continue;
                }

                if (SOC_PBMP_MEMBER(sop->lc_pbm_override_ports, port)) {
                    link =  SOC_PBMP_MEMBER(sop->lc_pbm_override_link, port);
                } else {
                    rv = _bcm_port_link_get(unit, port, hw_linkscan_mode, &link);
                    if (soc_feature(unit,
                        soc_feature_link_status_get_need_phy_read_twice)) {
                        if ((BCM_SUCCESS(rv)) && !link) {
                            rv = _bcm_port_link_get(unit, port,
                                                    hw_linkscan_mode, &link);
                        }
                    }
                }

                if (BCM_SUCCESS(rv) && link) {
                    (void)bcm_esw_port_update(unit, port, TRUE);
                } else {
                    (void)bcm_esw_port_update(unit, port, FALSE);
                }
            } else {
                (void)bcm_esw_port_update(unit, port, FALSE);
            }
        }
    }
}

STATIC int
_bcm_esw_port_update_before_linkscan_start(int unit, soc_port_t port,
                                                      int hw_linkscan_mode,
                                                      int *link)
{
    int  rv = 0, rx_en = 0, soft_reset = 0;

    /*
     * COVERITY
     * The iterator above PBMP_ITER will protect the port variable
     * from overrun.
     */
    /* coverity[overrun-call] */
    rv = _bcm_port_link_get(unit, port, hw_linkscan_mode, link);
    if (soc_feature(unit,
                    soc_feature_link_status_get_need_phy_read_twice)) {
        if ((BCM_SUCCESS(rv)) && !(*link)) {
            /* In legacy devices, PHY status register may show wrong
             * status on first read and we need to confirm by reading
             * second time if PHY status is shown as down.
             */
            /*
             * COVERITY
             * The iterator above PBMP_ITER will protect the port
             * variable from overrun.
             */
            /* coverity[overrun-call] */
            rv = _bcm_port_link_get(unit, port, hw_linkscan_mode,
                                    link);
        }
    }
    if ((BCM_SUCCESS(rv)) && *link) {
        rv = bcm_esw_port_control_get(unit, port,
                                      bcmPortControlRxEnable, &rx_en);
        soft_reset = _bcm_esw_port_soft_reset_is_set(unit, port);
        /* when soft_reset == 0, if sdk calls  bcm_esw_port_update, mac can't work normally */
        if ((BCM_SUCCESS(rv)) && (!rx_en) && soft_reset) {
#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(PORTMOD_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                int autoneg;
                int enabled, locked;

                BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_get(unit, port, &autoneg));
                if (autoneg == 1) {
                    BCM_IF_ERROR_RETURN(
                            bcmi_esw_portctrl_autoneg_status_get(unit, port, &enabled, &locked));
                    /* Do not allow port update if AN hasn't finished negotiating */
                    if (!(enabled && locked)) {
                        return rv;
                    }
                }
            }
#endif
            (void)bcm_esw_port_update(unit, port, *link);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_linkscan_thread
 * Purpose:
 *      Scan the ports on the specified unit for link status
 *      changes and process them as they occur.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 */

STATIC void
_bcm_esw_linkscan_thread(int unit)
{
    soc_persist_t    *sop = SOC_PERSIST(unit);
    ls_cntl_t        *lc = link_control[unit];
    sal_usecs_t       interval;
    int               rv = BCM_E_NONE;
    int               loopback;
    soc_port_t        port;
    int               link = 0, hw_linkscan_mode = 0;
#if defined(BCM_KATANA2_SUPPORT)
    soc_port_t        pp_port;
#endif
    int               normal_scheduled;
    int               hw_force_scan;
    int               hw_link_interrupt;
    sal_usecs_t       elapsed_last_scheduled; /* Elapsed time since last
                                                 normal linkscan run */
    char              thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t      thread;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    pbmp_t            pre_pbmp;
#endif
#if defined (BCM_KATANA2_SUPPORT)
    pbmp_t mask;
#endif
    LOG_INFO(BSL_LS_BCM_LINK,
             (BSL_META_U(unit,
                         "Linkscan starting on unit %d\n"), unit));

    /* Do not clear the lc_pbm_override_ports and lc_pbm_override_link.
     * If a port in Loopback mode forces link up before enabling linkscan,
     * the port status should be still forced to up after enabling linkscan.
     */

    sal_memset(lc->lc_error, 0, sizeof (lc->lc_error));

#ifdef BCM_LINK_SUPPRESS_REMOTE_FAULT
    sal_memset(lc->lc_up_proc_state, 0, sizeof(lc->lc_up_proc_state));
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_KATANA2(unit)) {
        SOC_PBMP_CLEAR(pre_pbmp);
    }
#endif

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        lc->lc_error[port].limit =
            soc_property_port_get(unit, port,
                                  spn_BCM_LINKSCAN_MAXERR, 5);
        lc->lc_error[port].delay =
            soc_property_port_get(unit, port,
                                  spn_BCM_LINKSCAN_ERRDELAY, 10000000);
    }

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    /*
     * Assume ports are up before running linkscan.
     * This should avoid links to have traffic hits at warmboot.
     */
    BCM_PBMP_ASSIGN(sop->lc_pbm_link, PBMP_ALL(unit));

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) &&
            SOC_INFO(unit).linkphy_enabled) {
        SOC_PBMP_ITER(SOC_INFO(unit).linkphy_pbm, port) {
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                    SOC_PBMP_PORT_ADD
                        (sop->lc_pbm_link, pp_port);
                }
            } else
#endif
            {
                for (pp_port = 0;
                        pp_port < SOC_INFO(unit).port_num_subport[port];
                        pp_port++) {
                    SOC_PBMP_PORT_ADD
                        (sop->lc_pbm_link,
                         pp_port + SOC_INFO(unit).port_subport_base[port]);
                }
            }
        }
        SOC_PBMP_CLEAR(mask);
        soc_link_mask2_get(unit, &mask);
        rv = soc_link_mask2_set(unit, mask);
        if (BCM_FAILURE(rv)) {
            /* Just log error */
            LOG_ERROR(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "%s: Failed to set link mask2: %s\n"),
                     thread_name, bcm_errmsg(rv)));
        }
    }
    if (soc_feature(unit, soc_feature_subtag_coe) &&
            SOC_INFO(unit).subtag_enabled) {
        SOC_PBMP_ITER(SOC_INFO(unit).subtag_pbm, port) {
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), pp_port) {
                    SOC_PBMP_PORT_ADD
                        (sop->lc_pbm_link, pp_port);
                }
            } else
#endif
            {
                for (pp_port = 0;
                        pp_port < SOC_INFO(unit).port_num_subport[port];
                        pp_port++) {
                    SOC_PBMP_PORT_ADD
                        (sop->lc_pbm_link,
                         pp_port + SOC_INFO(unit).port_subport_base[port]);
                }
            }
        }
        SOC_PBMP_CLEAR(mask);
        soc_link_mask2_get(unit, &mask);
        rv = soc_link_mask2_set(unit, mask);
        if (BCM_FAILURE(rv)) {
            /* Just log error */
            LOG_ERROR(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "%s: Failed to set link mask2: %s\n"),
                     thread_name, bcm_errmsg(rv)));
        }
    }
#endif

    if (!lc->lc_warm_boot) {
        /*
         * Force initial scan by setting link change while pretending link
         * was initially up.
         */
        /* If we set sop->lc_pbm_link_change as PBMP_PORT_ALL(unit),
         * MAC of all ports will be reset, and traffic will be disrupted.
         */
        /* BCM_PBMP_ASSIGN(sop->lc_pbm_link_change, PBMP_PORT_ALL(unit)); */

#ifdef BCM_RCPU_SUPPORT
        if (SOC_IS_RCPU_ONLY(unit) && SOC_PORT_VALID(unit, RCPU_PORT(unit))) {
            /* Do not update link status for rcpu port. */
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link_change, RCPU_PORT(unit));
        }
#endif /* BCM_RCPU_SUPPORT */


        /* Clear initial value of forwarding ports. */
        rv = soc_link_fwd_set(unit, sop->lc_pbm_link);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "AbnormalThreadExit:%s, "
                                  "Failed to clear forwarding ports: %s\n"),
                       thread_name, bcm_errmsg(rv)));
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                               SOC_SWITCH_EVENT_THREAD_LINKSCAN, __LINE__, rv);
            sal_thread_exit(0);
        }
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (!SOC_PBMP_MEMBER(lc->lc_pbm_sw, port) &&
                !SOC_PBMP_MEMBER(lc->lc_pbm_hw, port)) {
                continue;
            }

            /* If port is not in Loopback link status is retrieved from
             * PHY link status. If port is in Loopback mode the port status
             * should be still forced to up after enabling linkscan.
             */
            rv = bcm_esw_port_loopback_get(unit, port, &loopback);
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "Linkscan init loopback recovery, "
                                "unit %d, port %d, lb=%d\n"),
                     unit, port, loopback));
            if (!BCM_FAILURE(rv) && (loopback != BCM_PORT_LOOPBACK_NONE)) {
                SOC_PBMP_PORT_ADD(sop->lc_pbm_override_link, port);
                SOC_PBMP_PORT_ADD(sop->lc_pbm_override_ports, port);
                continue;
            }

            rv = _bcm_esw_port_update_before_linkscan_start(unit, port,
                                                            hw_linkscan_mode,
                                                            &link);
            if ((!BCM_FAILURE(rv)) && !link) {
                LC_LOCK(unit);
                BCM_PBMP_PORT_ADD(sop->lc_pbm_link_change, port);
                LC_UNLOCK(unit);
            }
        }
    } else {
        /* Clear Warm Boot record */
        lc->lc_warm_boot = FALSE;
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            /* In Warm boot, if the link scan mode is HW, then link state
             * has to be retrieved from CMIC link status instead of
             * retrieving from LINK_LATCHDOWNr.
             */
            if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, port)) {
                hw_linkscan_mode = 1;
            } else {
                hw_linkscan_mode = 0;
            }
            (void)_bcm_esw_port_update_before_linkscan_start(unit, port,
                                                            hw_linkscan_mode,
                                                            &link);
        }
    }

    /* Register for hardware linkscan interrupt. */

    rv = soc_linkscan_register(unit, _bcm_esw_linkscan_hw_interrupt);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s, Failed to register handler: %s\n"),
                   thread_name, bcm_errmsg(rv)));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_LINKSCAN, __LINE__, rv);
        sal_thread_exit(0);
    }

    elapsed_last_scheduled = 0;
    hw_force_scan = 0;

    lc->lc_thread = sal_thread_self();

    while ((interval = lc->lc_us) != 0) {
        pbmp_t            change;
        pbmp_t            hw_link;
        pbmp_t            hw_update;
        pbmp_t            fast_sw_los_down;
        pbmp_t            pbmp_all;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        soc_port_t        p;
        pbmp_t            cur_pbmp, dif_pbmp;
        int               enable;
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_KATANA2(unit)) {
            LC_LOCK(unit);
            SOC_PBMP_ASSIGN(cur_pbmp, PBMP_ALL(unit));
            /* Updating the flex pbmp for TD2. */
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TRIDENT2(unit)) {
                PBMP_ITER(cur_pbmp, p) {
                    rv = bcm_esw_port_subsidiary_ports_get(unit, p, &dif_pbmp);
                    if (rv != BCM_E_NONE) {
                        SOC_PBMP_PORT_REMOVE(cur_pbmp, p);
                    } else {
                        p += 3;
                    }
                }
            }
#endif
            /*
             * Updating the flexio pbmp for KT2 since lanes
             * splitting is done for few cases and it should
             * also be updated to get the correct current pbmp.
             */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
                (void)_bcm_kt2_flexio_pbmp_update(unit, &cur_pbmp);
            }
#endif

#if defined(BCM_SABER2_SUPPORT)
                if(SOC_IS_SABER2(unit)){
                    (void)bcm_sb2_sat_oamp_enable_get(unit, &enable);
                    /* Skip OAMP port for saber2 device */
                    if(enable) {
                        SOC_PBMP_PORT_REMOVE(cur_pbmp,
                                _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER);
                    }
                }
#endif
            if (SOC_PBMP_NOT_NULL(cur_pbmp)) {
                PBMP_ITER(cur_pbmp, p) {
                    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), p) ||
                        IS_LB_PORT(unit, p) || IS_CPU_PORT(unit, p)) {
                        SOC_PBMP_PORT_REMOVE(cur_pbmp, p);
                        continue;
                    }
                    if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), p)) {
                        (void)bcm_esw_port_enable_get(unit, p, &enable);
                        if (!enable) {
                            SOC_PBMP_PORT_REMOVE(cur_pbmp, p);
                        }
                    }
                }
            }
            if (SOC_PBMP_NEQ(cur_pbmp, pre_pbmp)) {
#ifdef BCM_TRIDENT2_SUPPORT
                if (SOC_IS_TRIDENT2(unit)) {
                    SOC_PBMP_ASSIGN(dif_pbmp, pre_pbmp);
                }
#endif
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    SOC_PBMP_ASSIGN(dif_pbmp, lc->lc_pbm_sw);
                    SOC_PBMP_OR(dif_pbmp, lc->lc_pbm_hw);
                }
#endif
                SOC_PBMP_ASSIGN(pre_pbmp, cur_pbmp);
                SOC_PBMP_XOR(dif_pbmp, cur_pbmp);
                if (SOC_PBMP_NOT_NULL(dif_pbmp)) {
                    PBMP_ITER(dif_pbmp, p) {
                        if (SOC_PBMP_MEMBER(cur_pbmp, p)) {
                            if (SOC_PBMP_MEMBER(lc->lc_pbm_hw, p)) {
                                (void)bcm_esw_linkscan_mode_set(unit, p,
                                                                BCM_LINKSCAN_MODE_HW);
                            } else if(SOC_PBMP_MEMBER(lc->lc_pbm_sw, p)) {
                                (void)bcm_esw_linkscan_mode_set(unit, p,
                                                                BCM_LINKSCAN_MODE_SW);
                            } else {
                                (void)bcm_esw_linkscan_mode_set(unit, p,
                                                                BCM_LINKSCAN_MODE_NONE);
                            }
                        }
                    }
                }
            }
            LC_UNLOCK(unit);
        }
#endif

        PORT_LOCK(unit);
        /* Flexport operation may lead to change in all bitmap */
        BCM_PBMP_ASSIGN(pbmp_all, PBMP_PORT_ALL(unit));
        PORT_UNLOCK(unit);

        /* Sample changed for software linkscan. */
        BCM_PBMP_ASSIGN(change, sop->lc_pbm_link_change);

        /* If the interrupt indicates that link status changes,
         * process the ports indicated by HW linkscan first.
         */
        if (lc->lc_hw_change) {
            /* Pause linkscan to make sure that the HW link state does
             * change while updating software state.
             */
            soc_linkscan_pause(unit);
            lc->lc_hw_change = 0;

#if defined (BCM_CMICX_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicx)) {
                /* Get CMICx HW link status from cached copy */
                (void)soc_linkscan_hw_link_cache_get(unit, &hw_link);
            } else
#endif /* BCM_CMICX_SUPPORT */
            {
                (void)soc_linkscan_hw_link_get(unit, &hw_link);
            }
            BCM_PBMP_AND(hw_link, lc->lc_pbm_hw);

#ifdef BCM_TRUNK_FAILOVER_SUPPORT
            /*
             * If enabled on XGS3 devices, do an early callback to the
             * trunk code that will immediately remove link down ports
             * from trunks.  This only works with hardware linkscanned
             * ports.
             */
#ifdef BCM_TRIDENT_SUPPORT
            if (soc_feature(unit, soc_feature_shared_trunk_member_table)) {
                extern int _bcm_trident_trunk_swfailover_trigger(int unit,
                                                bcm_pbmp_t ports_active,
                                                bcm_pbmp_t ports_status);
                (void)_bcm_trident_trunk_swfailover_trigger(unit,
                                                            lc->lc_pbm_hw,
                                                            hw_link);
            } else
#endif /* BCM_TRIDENT_SUPPORT */
            if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) {
                extern int _bcm_xgs3_trunk_swfailover_trigger(int unit,
                        bcm_pbmp_t ports_active,
                        bcm_pbmp_t ports_status);
                (void)_bcm_xgs3_trunk_swfailover_trigger(unit,
                                                         lc->lc_pbm_hw,
                                                         hw_link);
            }
#endif /* BCM_TRUNK_FAILOVER_SUPPORT */

            /* Get real link change bits. */
            BCM_PBMP_ASSIGN(hw_update, hw_link);
            BCM_PBMP_XOR(hw_update, sop->lc_pbm_link);
            BCM_PBMP_AND(hw_update, lc->lc_pbm_hw);
            _bcm_esw_linkscan_update(unit, 0 /*flags*/, hw_update);

            /* Get software link change bits caused by bcm_link_change API. */
            BCM_PBMP_ASSIGN(change, sop->lc_pbm_link_change);
            BCM_PBMP_AND(change, pbmp_all);
            _bcm_esw_linkscan_update(unit, 0 /*flags*/, change);
            soc_linkscan_continue(unit);

            /* Set flag to trigger scan */
            hw_force_scan = 1;
        }

        /* After processing the link status changes of the ports
         * indicated by interrupt handler (mainly to trigger swfailover),
         * scan all the ports again to make sure that the link status
         * is stable.
         * For some PHYs such as 5228, hardware linkscan may say the
         * link is up while the PHY is actually not quite done
         * autonegotiating. Rescanning make sure that the PHY link is
         * in sync with switch HW link state.
         */

        /*
         * There is a new accelerated mode in the linkscan thread
         * that provides for a smaller linkscan interval time on
         * those ports that need a faster polling interval.
         *
         * The linkscan thread will run at a faster rate (faster than
         * the one provided by the customer), if the accelerated
         * port bitmap is not empty.
         *
         * The scheduling logic will still allow the normal linkscan
         * SW port bitmap to proceed as scheduled.
         */
        if (elapsed_last_scheduled >= lc->lc_us) {
            normal_scheduled = 1;   /* Time for normal schedule linkscan */
        } else {
            normal_scheduled = 0;
        }

        if (normal_scheduled || hw_force_scan) {
            _bcm_esw_linkscan_update(unit, 0 /*flags*/, pbmp_all);
            elapsed_last_scheduled = 0;  /* Reset */
        } else if (BCM_PBMP_NOT_NULL(lc->lc_pbm_accel)) {

            /* Scan accelerated bitmap */
            _bcm_esw_linkscan_update(unit, 0 /*flags*/, lc->lc_pbm_accel);
            lc->lc_accel_poll_count++;
        }

        if (BCM_PBMP_NOT_NULL(change)) {
            /* Re-scan due to hardware force */
            BCM_PBMP_CLEAR(change);
            hw_force_scan = 1;
            continue;
        }

        hw_force_scan = 0;

        /* Stop accelerated mode if max accelerated poll count is reached */
        if (lc->lc_accel_poll_count >= lc->lc_accel_poll_max) {
            BCM_PBMP_CLEAR(lc->lc_pbm_accel);
            LOG_INFO(BSL_LS_BCM_LINK,
                     (BSL_META_U(unit,
                                 "Stop accelerated mode\n")));
        }

        /* If all links are HW scanned, we can usually await the
         * next HW interrupt.
         * Exceptions:
         *    - Any port is experiencing remote fault
         *    - SW RX LOS processing is operating on a down link port
         *    - Any port is waiting for EEE one-second delay enable setting
         */
        if (BCM_PBMP_IS_NULL(lc->lc_pbm_sw) &&
            BCM_PBMP_IS_NULL(sop->lc_pbm_remote_fault)) {
            hw_link_interrupt = TRUE;

            if (BCM_PBMP_NOT_NULL(lc->lc_pbm_fast)) {
                /* Are any fast ports in link down?
                 * If so, we must continue to scan them so the PHY driver
                 * processing continues.
                 */
                BCM_PBMP_ASSIGN(fast_sw_los_down, lc->lc_pbm_fast);
                BCM_PBMP_XOR(fast_sw_los_down, sop->lc_pbm_link);
                BCM_PBMP_AND(fast_sw_los_down, lc->lc_pbm_fast);

                if (BCM_PBMP_NOT_NULL(fast_sw_los_down)) {
                    hw_link_interrupt = FALSE;
                }
            }

            if (BCM_PBMP_NOT_NULL(lc->lc_pbm_eee_pending)) {
                hw_link_interrupt = FALSE;
            }

        } else {
            hw_link_interrupt = FALSE;
        }

        /* Set linkscan interval */
        if (BCM_PBMP_NOT_NULL(lc->lc_pbm_accel)) {
            if (lc->lc_us < lc->lc_accel_us) {
                interval = lc->lc_us;
            } else {
                interval = lc->lc_accel_us;
            }
        } else if (hw_link_interrupt) {
            interval = sal_sem_FOREVER;
        } else {
            interval = lc->lc_us;
        }

        if (interval != sal_sem_FOREVER) {
            elapsed_last_scheduled += interval;
        }

        (void)sal_sem_take(lc->lc_sema, interval);
    }

    (void)soc_linkscan_register(unit, NULL);

    /*
     * Before exiting, re-enable all ports that were being scanned.
     *
     * For administrative reload preparation, such as _bcm_shutdown,
     * the reload mode is used to avoid this disturbing of ports.
     */

    if (!SOC_WARM_BOOT(unit)) {

        _bcm_esw_port_update_before_linkscan_exit(unit, lc->lc_pbm_sw);

        _bcm_esw_port_update_before_linkscan_exit(unit, lc->lc_pbm_hw);

    } /* !SOC_WARM_BOOT(unit) */

    LOG_INFO(BSL_LS_BCM_LINK,
             (BSL_META_U(unit,
                         "Linkscan exiting\n")));

    lc->lc_thread = NULL;
    sal_thread_exit(0);
}

/************************************************************************
 *********                                                      *********
 *********         Start of BCM API Exported Routines           *********
 */

/*
 * Function:
 *      bcm_linkscan_init
 * Purpose:
 *      Initialize the linkscan software module.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_init(int unit)
{
    ls_cntl_t *lc;
    bcm_port_if_t intf;
    uint32 regval;
    soc_port_t port, phy_port;
    soc_persist_t *sop = SOC_PERSIST(unit);
    int val = -1;

    if (link_control[unit] != NULL) {
        BCM_IF_ERROR_RETURN(bcm_esw_linkscan_detach(unit));
    }

    if ((lc = sal_alloc(sizeof (ls_cntl_t), "link_control")) == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(lc, 0, sizeof (ls_cntl_t));


    lc->lc_lock = sal_mutex_create("bcm_link_LOCK");
    if (lc->lc_lock == NULL) {
        sal_free(lc);
        return BCM_E_MEMORY;
    }

    lc->lc_sema = sal_sem_create("bcm_link_SLEEP",
                                 sal_sem_BINARY, 0);
    if (lc->lc_sema == NULL) {
        sal_mutex_destroy(lc->lc_lock);
        sal_free(lc);
        return BCM_E_MEMORY;
    }

    link_control[unit] = lc;

    /*
     * Initialize link_control port bitmaps so bcm_port_update works
     * reasonably even if the linkscan thread is never started.
     */
    _lc_pbm_init(unit);

    if (!SOC_IS_RCPU_ONLY(unit)) {

        /* 1. Select between C45 and C22 for HW linkscan
         * 2. Select appropriate MDIO Bus
         * 3. Select between MDIO scan vs. link status from internal PHY
         * 4. Initialize HW linkscan PHY address map.
         */
        BCM_IF_ERROR_RETURN
            (soc_linkscan_hw_init(unit));

        /* For XGS 3 devices, select the source of the CMIC link status
         * interrupt to be the Internal Serdes - only for SGMII ports
         */
        BCM_PBMP_CLEAR(lc->lc_pbm_sgmii_autoneg_port);
        if (soc_feature(unit, soc_feature_sgmii_autoneg)) {
            PBMP_ITER(PBMP_PORT_ALL(unit), port) {
                if (soc_property_port_get(unit, port,
                                          spn_PHY_SGMII_AUTONEG, FALSE)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_interface_get(unit, port, &intf));
                    if (intf == BCM_PORT_IF_SGMII) {
                        BCM_PBMP_PORT_ADD(lc->lc_pbm_sgmii_autoneg_port, port);
                        if (soc_feature(unit, soc_feature_logical_port_num)) {
                            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                        } else {
                            phy_port = port;
                        }
#ifdef BCM_CMICM_SUPPORT
                        if (soc_feature(unit, soc_feature_cmicm)) {
                            if (phy_port < 32) {
                                READ_CMIC_MIIM_INT_SEL_MAP_0r(unit, &regval);
                                regval |= (1 << phy_port);
                                WRITE_CMIC_MIIM_INT_SEL_MAP_0r(unit, regval);
                            } else if (phy_port < 64) {
                                READ_CMIC_MIIM_INT_SEL_MAP_1r(unit, &regval);
                                regval |= (1 << (phy_port-32));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_1r(unit, regval);
                            } else if (phy_port < 96) {
                                READ_CMIC_MIIM_INT_SEL_MAP_2r(unit, &regval);
                                regval |= (1 << (phy_port-64));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_2r(unit, regval);
                            } else if (phy_port < 128) {
                                READ_CMIC_MIIM_INT_SEL_MAP_3r(unit, &regval);
                                regval |= (1 << (phy_port-96));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_3r(unit, regval);
                            } else
#ifdef BCM_CMICDV2_SUPPORT
                        if (soc_feature(unit, soc_feature_cmicd_v2)) {
                            if (phy_port < 160) {
                                READ_CMIC_MIIM_INT_SEL_MAP_4r(unit, &regval);
                                regval |= (1 << (phy_port-128));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_4r(unit, regval);
                            } else if (phy_port < 192) {
                                READ_CMIC_MIIM_INT_SEL_MAP_5r(unit, &regval);
                                regval |= (1 << (phy_port-160));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_5r(unit, regval);
                            } else
#ifdef BCM_CMICDV3_SUPPORT
                        if (soc_feature(unit, soc_feature_cmicd_v3)) {
                            if (phy_port < 224) {
                                READ_CMIC_MIIM_INT_SEL_MAP_6r(unit, &regval);
                                regval |= (1 << (phy_port-192));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_6r(unit, regval);
                            } else if (phy_port < 256) {
                                READ_CMIC_MIIM_INT_SEL_MAP_7r(unit, &regval);
                                regval |= (1 << (phy_port-224));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_7r(unit, regval);
                            } else if (phy_port < 288) {
                                READ_CMIC_MIIM_INT_SEL_MAP_8r(unit, &regval);
                                regval |= (1 << (phy_port-256));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_8r(unit, regval);
                            } else if (phy_port < 320) {
                                READ_CMIC_MIIM_INT_SEL_MAP_9r(unit, &regval);
                                regval |= (1 << (phy_port-288));
                                WRITE_CMIC_MIIM_INT_SEL_MAP_9r(unit, regval);
                            } else {
                                return BCM_E_PORT;
                            }
                        } else
#endif /* CMICd V3 Support */
                            {
                                return BCM_E_PORT;
                            }
                        } else
#endif /* CMICd V2 Support */
                            {
                                return BCM_E_PORT;
                            }
                        } else
#endif /* CMICm Support */
#ifdef BCM_CMICX_SUPPORT
                        if (soc_feature(unit, soc_feature_cmicx)) {
                            /* TBA: Add cmicx specific code */
                        } else
#endif
                            {
                                if (phy_port < 32) {
                                    READ_CMIC_MIIM_INT_SEL_MAPr(unit, &regval);
                                    regval |= (1 << phy_port);
                                    WRITE_CMIC_MIIM_INT_SEL_MAPr(unit, regval);
                                }
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
                                else if ((SOC_IS_TR_VL(unit) &&
                                          !SOC_IS_ENDURO(unit) &&
                                          !SOC_IS_HURRICANEX(unit) &&
                                          !SOC_IS_GREYHOUND(unit) &&
                                          !SOC_IS_GREYHOUND2(unit)) ||
                                         soc_feature
                                         (unit, soc_feature_register_hi)) {
                                    if ((phy_port - 32) <= 31) {
                                        READ_CMIC_MIIM_INT_SEL_MAP_HIr
                                            (unit, &regval);
                                        regval |= (1 << (phy_port - 32));
                                        WRITE_CMIC_MIIM_INT_SEL_MAP_HIr
                                            (unit, regval);
                                    } else
#if defined(BCM_TRIDENT_SUPPORT)
                                    if ((phy_port - 64) <= 31) {
                                        READ_CMIC_MIIM_INT_SEL_MAP_HI_2r
                                            (unit, &regval);
                                        regval |= (1 << (phy_port - 64));
                                        WRITE_CMIC_MIIM_INT_SEL_MAP_HI_2r
                                            (unit, regval);
                                    } else
#endif
                                    {
                                        return BCM_E_PORT;
                                    }
                                }
#endif
                            } /* Not CMICm */
                    }
                }
            }
        }
    } /* !SOC_IS_RCPU_ONLY */

    /*
     * Accelerated mode
     * Property "fast_sw_rx_los_pbmp" is old (maintained for compatibility
     * purpose), "rx_fast_los_link" should be used instead.
     * If both properties are used for the same port, the approved
     * "rx_fast_los_link" takes precedence.
     */
    lc->lc_pbm_fast = soc_property_get_pbmp(unit, "fast_sw_rx_los_pbmp", 0);
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        val = soc_property_port_get(unit, port, spn_RX_FAST_LOS_LINK, -1);
        if (val == 0) {
            BCM_PBMP_PORT_REMOVE(lc->lc_pbm_fast, port);
        } else if (val == 1) {
            BCM_PBMP_PORT_ADD(lc->lc_pbm_fast, port);
        }
    }

    lc->lc_accel_us = soc_property_get(unit, spn_RX_FAST_LOS_USEC,
                                       ACCELERATED_INTERVAL_USEC);
    lc->lc_accel_poll_max = soc_property_get(unit,
                                             spn_RX_FAST_LOS_POLL_COUNT_MAX,
                                             ACCELERATED_POLL_COUNT_MAX);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_FAST_REBOOT(unit)) {
        SOC_WARM_BOOT_START(unit);
    }
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_link_reload(unit));
        if (SOC_FAST_REBOOT(unit)) {
            SOC_WARM_BOOT_DONE(unit);
        }
    } else {
        int                 alloc_size, rv;
        soc_scache_handle_t scache_handle;
        uint8               *link_scache;

        /* Alloc size calculation */
        alloc_size = BCM_LINK_PBMP_SCACHE_NUM * _LINK_PBMP_BYTE_MAX;

#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
            /* For lc_ibppktsetlimit and lc_ibpcelsetlimit */
            alloc_size += (SOC_MAX_NUM_PORTS * 2 * sizeof(uint32));
        }
#endif /* BCM_HAWKEYE_SUPPORT */

        /* For lc_us */
        alloc_size += sizeof(int);

        SOC_SCACHE_HANDLE_SET(scache_handle,
                              unit, BCM_MODULE_LINKSCAN, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     alloc_size,
                                     &link_scache,
                                     BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        /* This is needed for init time initialization */
        if (soc_property_port_get(unit, port, spn_FCMAP_ENABLE, 0)) {
            SOC_PBMP_PORT_ADD(sop->lc_pbm_fc, port);
       }

       /* Check of the external PHy is set in EBE/ETHERRAM buffer mode */
       if (soc_property_port_get(unit, port, spn_PHY_OPERATIONAL_MODE, 0) == 3) {
            /* the External PHY is in ETHER RAM or buffered mode */
            SOC_PBMP_PORT_ADD(sop->lc_pbm_eth_buffer_mode, port);
       }
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    /* Linkscan module starts after port module. Hence register here */
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        bcm_esw_linkscan_register(unit, _bcm_port_force_forward_linkscan_handler);
    }
#endif /*defined(BCM_TRIUMPH3_SUPPORT)*/

#ifdef BCM_PREEMPTION_SUPPORT
    if (soc_feature(unit, soc_feature_preemption)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_linkscan_register(
                unit, bcmi_esw_preemption_linkscan_update));
    }
#endif /* BCM_PREEMPTION_SUPPORT */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_mode_set
 * Purpose:
 *      Set the current scanning mode for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port #.
 *      mode - New scan mode for specified port.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_set(int unit, bcm_port_t port, int mode)
{
    ls_cntl_t     *lc = link_control[unit];
    int           rv = BCM_E_NONE;
    pbmp_t        empty_pbm;
    int           added = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);
    pbmp_t tmp_pbm;
    int link_up = 0;
    LC_CHECK_INIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &port));
    }
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), port)
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
        && (!(SOC_IS_TRIDENT2(unit) || SOC_IS_KATANA2(unit)) ||
            mode != BCM_LINKSCAN_MODE_NONE)
#endif
       ) {
        return BCM_E_NONE;
    }
    if(!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit),port)) {
         return BCM_E_PORT;
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (mode < 0 || mode >= BCM_LINKSCAN_MODE_COUNT) {
        return BCM_E_PARAM;
    }

    SOC_PBMP_CLEAR(empty_pbm);

    LC_LOCK(unit);

    /* First, remove from current configuration */
    SOC_PBMP_PORT_REMOVE(lc->lc_pbm_sw, port);
    SOC_PBMP_PORT_REMOVE(lc->lc_pbm_hw, port);

    /* Now add back to proper map */
    switch (mode) {
    case BCM_LINKSCAN_MODE_NONE:
        if (SOC_PBMP_MEMBER(sop->lc_pbm_remote_fault, port)) {
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_remote_fault, port);
            SOC_PBMP_PORT_REMOVE(sop->lc_pbm_link, port);
        }
        break;
    case BCM_LINKSCAN_MODE_SW:
        SOC_PBMP_PORT_ADD(lc->lc_pbm_sw, port);
        added = 1;
        break;
    case BCM_LINKSCAN_MODE_HW:
        _BCM_LINK_IF_ERROR_RETURN(_bcm_port_link_get(unit, port, 0, &link_up));

        /* syncup pbm with cmic_miim_link_status registesrs,
        to avoid misleading up/down notification */
        _BCM_LINK_IF_ERROR_RETURN(soc_linkscan_hw_link_get(unit, &tmp_pbm));
        if(SOC_PBMP_MEMBER(tmp_pbm, port) && !link_up &&
		!BCM_PBMP_MEMBER(lc->lc_pbm_fast, port)) {
        /* if CMIC says up but phy says down, mark it up, and
           expect an interrupt later to trig all necessary procedure */
            SOC_PBMP_PORT_ADD(sop->lc_pbm_link, port);
        }

        SOC_PBMP_PORT_ADD(lc->lc_pbm_hw, port);

        /* Need to run SW link scan as well on ports where the source
         * of the link status is the internal Serdes - only SGMII ports
         */
        if (BCM_PBMP_MEMBER(lc->lc_pbm_sgmii_autoneg_port, port)) {
            SOC_PBMP_PORT_ADD(lc->lc_pbm_sw, port);
        }
        added = 1;
        lc->lc_hw_change = 1;
        break;
    /* coverity[dead_error_begin:FALSE] */
    default:
        LC_UNLOCK(unit);
        return BCM_E_PARAM;
    }


    /* Reconfigure HW linkscan in case changed */
    rv = soc_linkscan_config(unit, lc->lc_pbm_hw, empty_pbm);

    /* Prime the HW linkscan pump */
    if (SOC_PBMP_NOT_NULL(lc->lc_pbm_hw)) {
        lc->lc_hw_change = 1;
        _bcm_esw_linkscan_hw_interrupt(unit);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    LC_UNLOCK(unit);

    if (lc->lc_sema != NULL) {
        sal_sem_give(lc->lc_sema);  /* register change now */
    }

    /* When no longer scanning a port, return it to the enabled state. */
    if (BCM_SUCCESS(rv) && !added && !(lc->lc_warm_boot)) {
        int enable;

#ifdef BCM_RCPU_SUPPORT
        if (SOC_IS_RCPU_ONLY(unit) && (port == RCPU_PORT(unit))) {
            return BCM_E_NONE;
        }
#endif /* BCM_RCPU_SUPPORT */
        if (BCM_SUCCESS(bcm_esw_port_enable_get(unit, port, &enable))) {
            (void)bcm_esw_port_update(unit, port, enable);
        }
    }

    return(rv);
}

/*
 * Function:
 *      bcm_esw_linkscan_mode_set_pbm
 * Purpose:
 *      Set the current scanning mode for the specified ports.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbm  - Port bit map indicating port to set.
 *      mode - New scan mode for specified ports.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_set_pbm(int unit, pbmp_t pbm, int mode)
{
    bcm_port_t  port;

    LC_CHECK_INIT(unit);

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_linkscan_mode_set(unit, port, mode));
    }

    return (BCM_E_NONE);
}

int
_bcm_esw_lc_lock(int unit)
{
    LC_LOCK(unit);
    return BCM_E_NONE;
}

int
_bcm_esw_lc_unlock(int unit)
{
    LC_UNLOCK(unit);
    return BCM_E_NONE;
}

int _bcm_esw_lc_check_init(int unit)
{
    LC_CHECK_INIT(unit);
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_esw_linkscan_mode_get
 * Purpose:
 *      Recover the current scanning mode for the specified port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - StrataSwitch Port #.
 *      mode - (OUT) current scan mode for specified port.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_mode_get(int unit, bcm_port_t port, int *mode)
{
    ls_cntl_t   *lc = link_control[unit];

    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    if(!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit),port)) {
       return BCM_E_PORT;
    }
    if (NULL == mode) {
        return BCM_E_PARAM;
    }

    if (PBMP_MEMBER(lc->lc_pbm_hw, port)) {
        *mode = BCM_LINKSCAN_MODE_HW;
    } else if (PBMP_MEMBER(lc->lc_pbm_sw, port)) {
        *mode = BCM_LINKSCAN_MODE_SW;
    } else {
        *mode = BCM_LINKSCAN_MODE_NONE;
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_enable_get
 * Purpose:
 *      Retrieve the current linkscan mode.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      us - (OUT) Pointer to microsecond scan time for software scanning,
 *              0 if not enabled.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_enable_get(int unit, int *us)
{
    LC_CHECK_INIT(unit);

    *us = link_control[unit]->lc_us;

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_enable_set
 * Purpose:
 *      Enable or disable the link scan feature.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      us   - Specifies the software polling interval in micro-seconds;
 *             the value 0 disables linkscan.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_enable_set(int unit, int us)
{
    ls_cntl_t           *lc = link_control[unit];
    int                 rv = BCM_E_NONE;
    soc_timeout_t       to;
    pbmp_t              empty_pbm;
    sal_usecs_t         wait_usec;

    /* Time to wait for thread to start/end (longer for BCMSIM) */
    wait_usec = (SAL_BOOT_BCMSIM || SAL_BOOT_QUICKTURN) ? 30000000 : 10000000;
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit) && SAL_BOOT_QUICKTURN) {
        wait_usec = 70000000;
    }
#endif

    if (!us && lc == NULL) { /* No error to disable if not inited */
        return(BCM_E_NONE);
    }

    LC_CHECK_INIT(unit);

    sal_snprintf(lc->lc_taskname,
                 sizeof (lc->lc_taskname),
                 "bcmLINK.%d",
                 unit);

    SOC_PBMP_CLEAR(empty_pbm);

    if (us) {
        if (us < BCM_LINKSCAN_INTERVAL_MIN) {
            us = BCM_LINKSCAN_INTERVAL_MIN;
        }

        lc->lc_us = us;

        if (lc->lc_thread != NULL) {
            /* Linkscan is already running; just update the period */
            sal_sem_give(lc->lc_sema);
            return SOC_E_NONE;
        }


        if (sal_thread_create(lc->lc_taskname,
                              SAL_THREAD_STKSZ,
                              soc_property_get(unit,
                                               spn_LINKSCAN_THREAD_PRI,
                                               50),
                              (void (*)(void*))_bcm_esw_linkscan_thread,
                              INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
            lc->lc_us = 0;
            rv = BCM_E_MEMORY;
        } else {
            soc_timeout_init(&to, wait_usec, 0);

            while (lc->lc_thread == NULL) {
                if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                              (BSL_META_U(unit,
                                          "%s: Thread did not start\n"),
                               lc->lc_taskname));
                    lc->lc_us = 0;
                    rv = BCM_E_INTERNAL;
                    break;
                }
            }
            if (BCM_SUCCESS(rv)) {
                /* Make sure HW linkscanning is enabled on HW linkscan ports */
                rv = soc_linkscan_config(unit, lc->lc_pbm_hw, empty_pbm);
            }
        }
    } else if (lc->lc_thread != NULL) {
        lc->lc_us = 0;

        /* To prevent getting HW linkscan interrupt after linkscan is disabled,
         * HW linkscanning must be disabled. */
        rv = soc_linkscan_config(unit, empty_pbm, empty_pbm);

        sal_sem_give(lc->lc_sema);

        soc_timeout_init(&to, wait_usec, 0);

        while (lc->lc_thread != NULL) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "%s: Thread did not exit\n"),
                           lc->lc_taskname));
                rv = BCM_E_INTERNAL;
                break;
            }
        }
    }

    return(rv);
}

/*
 * Function:
 *      bcm_esw_linkscan_register
 * Purpose:
 *      Register a handler to be called when a link status change is noticed.
 *      The handler function must not suspend indefinitely or invoke any API
 *      that may result in a deadlock.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      f - pointer to function to call when link status change is seen
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_register(int unit, bcm_linkscan_handler_t f)
{
    ls_cntl_t       *lc = link_control[unit];
    ls_handler_t    *lh;
    int             found = FALSE;

    LC_CHECK_INIT(unit);

    /* First, see if this handler already registered */
    LC_LOCK(unit);

    for (lh = lc->lc_handler; lh != NULL; lh = lh->lh_next) {
        if (lh->lh_f == f) {
            found = TRUE;
            break;
        }
    }

    if (found) {
        LC_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if ((lh = sal_alloc(sizeof(*lh), "bcm_linkscan_register")) == NULL) {
        LC_UNLOCK(unit);
        return(BCM_E_MEMORY);
    }

    lh->lh_next = lc->lc_handler;
    lh->lh_f = f;
    lc->lc_handler = lh;

    LC_UNLOCK(unit);

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_unregister
 * Purpose:
 *      Remove a previously registered handler from the callout list.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      f    - Pointer to function registered in call to
 *             bcm_linkscan_register.
 * Returns:
 *      BCM_E_NOT_FOUND  Could not find matching handler
 *      BCM_E_NONE       Success
 */

int
bcm_esw_linkscan_unregister(int unit, bcm_linkscan_handler_t f)
{
    ls_cntl_t     *lc = link_control[unit];
    ls_handler_t  *lh, *p;

    LC_CHECK_INIT(unit);

    LC_LOCK(unit);

    for (p = NULL, lh = lc->lc_handler; lh; p = lh, lh = lh->lh_next) {
        if (lh->lh_f == f) {
            if (p == NULL) {
                lc->lc_handler = lh->lh_next;
            } else {
                p->lh_next = lh->lh_next;
            }
            break;
        }
    }

    LC_UNLOCK(unit);

    if (lh != NULL) {
        sal_free(lh);
        return BCM_E_NONE;
    } else {
        return BCM_E_NOT_FOUND;
    }
}

/*
 * Function:
 *      bcm_esw_linkscan_port_register
 * Purpose:
 *      Register a handler to be called when a link status
 *      is to be determined by a caller provided function.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      f - pointer to function to call for true link status
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function works with software linkscan only.
 */

int
bcm_esw_linkscan_port_register(int unit, bcm_port_t port,
                               bcm_linkscan_port_handler_t f)
{
    ls_cntl_t   *lc = link_control[unit];

    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit),port) ||
        !SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    lc->lc_f[port] = f;
    LC_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_port_unregister
 * Purpose:
 *      Remove a previously registered handler that is used
 *      for setting link status.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - StrataSwitch port number.
 *      f    - Pointer to function registered in call to
 *             bcm_linkscan_port_register.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_linkscan_port_unregister(int unit, bcm_port_t port,
                                 bcm_linkscan_port_handler_t f)
{
    ls_cntl_t   *lc = link_control[unit];
    int         rv;

    LC_CHECK_INIT(unit);
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }
    if (!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit),port) ||
        !SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LC_LOCK(unit);
    if (f == lc->lc_f[port]) {
        lc->lc_f[port] = NULL;
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    LC_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_link_wait
 * Purpose:
 *      Wait for all links in the mask to be "link up".
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbm - (IN/OUT) Port bit map to wait for, mask of those link up on
 *              return.
 *      us - number of microseconds to wait.
 * Returns:
 *      BCM_E_NONE - all links are ready.
 *      BCM_E_TIMEOUT - not all links ready in specified time.
 *      BCM_E_DISABLED - linkscan not running on one or more of the ports.
 */

int
bcm_esw_link_wait(int unit, pbmp_t *pbm, int us)
{
    soc_persist_t  *sop = SOC_PERSIST(unit);
    ls_cntl_t      *lc = link_control[unit];
    soc_timeout_t  to;
    pbmp_t         sofar_pbm;
    soc_port_t     port;

    /* Input parameters check. */
    if ((NULL == pbm) || (us < 0)) {
        return (BCM_E_PARAM);
    }

    BCM_PBMP_ITER(*pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_linkscan_enable_port_get(unit, port));
    }

    /*
     * If a port was just configured, it may have gone down but
     * lc_pbm_link may not reflect that until the next time linkscan
     * runs.  This is avoided by forcing an update of lc_pbm_link.
     */

    _bcm_esw_linkscan_update(unit, 0 /*flags*/, *pbm);

    soc_timeout_init(&to, (sal_usecs_t)us, 0);

    for (;;) {
        sal_usleep(lc->lc_us / 4);

        SOC_PBMP_ASSIGN(sofar_pbm, sop->lc_pbm_link);
        SOC_PBMP_REMOVE(sofar_pbm, sop->lc_pbm_remote_fault);
        SOC_PBMP_AND(sofar_pbm, *pbm);
        if (SOC_PBMP_EQ(sofar_pbm, *pbm)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            SOC_PBMP_AND(*pbm, sop->lc_pbm_link);
            SOC_PBMP_REMOVE(*pbm, sop->lc_pbm_remote_fault);
            return (BCM_E_TIMEOUT);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_linkscan_detach
 * Purpose:
 *      Prepare linkscan module to detach specified unit.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE - detach successful.
 *      BCM_E_XXX - detach failed.
 * Notes:
 *      This is safe to call at any time, but linkscan should only be
 *      initialized or detached from the main application thread.
 */

int
bcm_esw_linkscan_detach(int unit)
{
    ls_cntl_t           *lc = link_control[unit];
    ls_handler_t        *lh;
    pbmp_t              empty_pbm;

    if (lc == NULL) {
        return BCM_E_NONE;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
    if (soc_feature(unit, soc_feature_epc_linkflap_recover)) {
        bcm_esw_linkscan_unregister(unit, _bcm_port_force_forward_linkscan_handler);
    }
#endif /*defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)*/

#ifdef BCM_PREEMPTION_SUPPORT
    if (soc_feature(unit, soc_feature_preemption)) {
        bcm_esw_linkscan_unregister(unit, bcmi_esw_preemption_linkscan_update);
    }
#endif /* BCM_PREEMPTION_SUPPORT */

    SOC_PBMP_CLEAR(empty_pbm);

    SOC_IF_ERROR_RETURN(soc_linkscan_config(unit, empty_pbm, empty_pbm));

    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_enable_set(unit, 0));

    /* Clean up list of handlers */

    while (lc->lc_handler != NULL) {
        lh = lc->lc_handler;
        lc->lc_handler = lh->lh_next;
        sal_free(lh);
    }

    /* Mark and not initialized and free mutex */

    if (lc->lc_sema != NULL) {
        sal_sem_destroy(lc->lc_sema);
        lc->lc_sema = NULL;
    }

    if (lc->lc_lock != NULL) {
        sal_mutex_destroy(lc->lc_lock);
        lc->lc_lock = NULL;
    }

    link_control[unit] = NULL;

    sal_free(lc);

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_linkscan_port_attach
 * Purpose:
 *      Initialize linkscan info for newly added port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      This function only be called by flexport operation.
 */
int
bcm_esw_linkscan_port_attach(int unit, bcm_port_t port)
{
    ls_cntl_t           *lc = link_control[unit];
    int                 limit;      /* # errors to enter error state */
    int                 delay;      /* Length of error state in seconds */

    if (lc == NULL || lc->lc_thread == NULL) {
        return BCM_E_NONE;
    }

    limit = soc_property_port_get(unit, port, spn_BCM_LINKSCAN_MAXERR, 5);
    delay = soc_property_port_get(unit, port,
                                  spn_BCM_LINKSCAN_ERRDELAY, 10000000);

    LC_LOCK(unit);
    sal_memset(&lc->lc_error[port], 0, sizeof(ls_errstate_t));
    lc->lc_error[port].limit = limit;
    lc->lc_error[port].delay = delay;
    LC_UNLOCK(unit);

    return BCM_E_NONE;
}

#if defined(BROADCOM_DEBUG)
int
bcm_esw_linkscan_dump(int unit)
{
    ls_handler_t *ent;

    for (unit = 0; unit < BCM_MAX_NUM_UNITS; unit++) {
        if (link_control[unit] == NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "BCM linkscan not initialized for unit %d\n"), unit));
            continue;
        }

        LOG_CLI((BSL_META_U(unit,
                            "BCM linkscan callbacks for unit %d\n"), unit));
        for (ent = link_control[unit]->lc_handler; ent != NULL;
             ent = ent->lh_next) {
#if !defined(__PEDANTIC__)
            LOG_CLI((BSL_META_U(unit,
                                "    Fn %p\n"), (void *)ent->lh_f));
#else /* !defined(__PEDANTIC__) */
            LOG_CLI((BSL_META_U(unit,
                                "    Function pointer unprintable\n")));
#endif /* !defined(__PEDANTIC__) */
        }
    }

    return BCM_E_NONE;
}
#endif  /* BROADCOM_DEBUG */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_link_sw_dump
 * Purpose:
 *     Displays Linkscan information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 */
void
_bcm_link_sw_dump(int unit)
{
    ls_cntl_t           *lc = link_control[unit];
    soc_persist_t       *sop = SOC_PERSIST(unit);
    char                pfmt[SOC_PBMP_FMT_LEN];
    int                 rv, enable;
    bcm_port_t          port;
    bcm_pbmp_t          disabled_pbmp;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information Linkscan - Unit %d\n"), unit));

    LOG_CLI((BSL_META_U(unit,
                        "HW linkscan ports:              %s\n"),
             SOC_PBMP_FMT(lc->lc_pbm_hw, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "SW linkscan ports:              %s\n"),
             SOC_PBMP_FMT(lc->lc_pbm_sw, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "SGMII autoneg ports:            %s\n"),
             SOC_PBMP_FMT(lc->lc_pbm_sgmii_autoneg_port, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Info skip ports:                %s\n"),
             SOC_PBMP_FMT(lc->lc_pbm_info_skip, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Fast linkscan ports:            %s\n"),
             SOC_PBMP_FMT(lc->lc_pbm_fast, pfmt)));

    LOG_CLI((BSL_META_U(unit,
                        "Forwarding ports:               %s\n"),
             SOC_PBMP_FMT(sop->link_fwd, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "MAC enabled ports:              %s\n"),
             SOC_PBMP_FMT(SOC_CONTROL(unit)->link_mask2, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Link up ports:                  %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_link, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Impending down ports:           %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_link_change, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Forced ports:                   %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_override_ports, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Link up forced link ports:      %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_override_link, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "TX enabled without link ports:  %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_linkdown_tx, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Remote fault received ports:    %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_remote_fault, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Enabled for LAG failover ports: %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_failover, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Failed LAG failover ports:      %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_failed, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "Recovering LAG failover ports:  %s\n"),
             SOC_PBMP_FMT(sop->lc_pbm_failed_clear, pfmt)));

    SOC_PBMP_CLEAR(disabled_pbmp);
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        rv = bcm_esw_port_enable_get(unit, port, &enable);
        if (BCM_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "Error checking port %d enable state:  %s\n"),
                     port, bcm_errmsg(rv)));
            return;
        }
        if (!enable) {
            SOC_PBMP_PORT_ADD(disabled_pbmp, port);
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "Disabled ports:                 %s\n"),
             SOC_PBMP_FMT(disabled_pbmp, pfmt)));
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#if defined(BCM_CMICX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/*
 * Function:
 *     cmicx_esw_linkscan_hw_interrupt
 * Purpose:
 *     Link scan interrupt handler.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     Nothing
 * Notes:
 *     Assumes: valid unit number.
 */
void
cmicx_esw_linkscan_hw_interrupt(int unit)
{
    ls_cntl_t           *lc = link_control[unit];

    if ((NULL != lc) && (NULL != lc->lc_sema)) {
        lc->lc_hw_change = 1;
        sal_sem_give(lc->lc_sema);
    }
    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Linkscan interrupt unit %d\n"), unit));
}
#endif

