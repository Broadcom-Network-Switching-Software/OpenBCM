/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM Link Scan Module
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
 * All modifications to the linkscan state are protected by LS_LOCK.
 *
 * Linkscan maintains the following port bitmaps
 *
 *     pbm_link:
 *                 Current physical link up/down status. When a bit
 *                 in this mask is set, a link up or link down event
 *                 is recognized and signaled to any registered
 *                 handlers.
 *
 *     pbm_link_change:
 *                 Mask of ports that need to recognize a link
 *                 down event. Ports are added to this mask by the
 *                 function bcm_link_change. 
 *
 *     pbm_override_ports:
 *                 Bitmap of ports that are currently
 *                 being explicitly set to a value. These actual value
 *                 is determined by pbm_override_link. Ports are
 *                 added and removed from this mode by the routine
 *                 _bcm_link_force. 
 *
 *                 Ports that are forced to an UP state do NOT result
 *                 in a call to bcm_port_update. It is the
 *                 responsibility of the caller to configure the
 *                 correct MAC and PHY state.
 *
 *     pbm_override_link:
 *                 Bitmap indicating the link-up/link-down
 *                 status for those ports with override set.
 *
 *     pbm_sgmii_autoneg:
 *                 Bitmap of the port that is configured in SGMII
 *                 autoneg mode based on spn_PHY_SGMII_AUTONEG.
 *                 Maintaining this bitmap avoids the overhead
 *                 of the soc_property_port_get call. 
 *
 *     pbm_remote_fault:
 *                 Bit map indicating the remote fault status of
 *                 a link-up port.
 *     pbm_local_fault:
 *                 Bit map indicating the local fault status of
 *                 a link-up port.
 *
 * Calls to _bcm_link_get always returns the current status as
 * indicated by pbm_link. 
 *
 * NOTE:
 * Original file is src/bcm/esw/link.c, version 1.49.
 * That file should eventually be removed, and XGS specific code
 * should be placed in corresponding routines to be attached to the
 * 'driver' in 'link_control' during module initialization
 * 'soc_linkctrl_init()'.
 */


#include <shared/bsl.h>

#include <sal/types.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/phyctrl.h>
#include <soc/linkctrl.h>
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_link.h>
#endif
#include <bcm/error.h>
#include <bcm/port.h>

#include <bcm/link.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/common/link.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/module.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_m0ssq.h>
#endif

#include <soc/dnxc/multithread_analyzer.h>

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_ha.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#endif
#endif

/*
 * Handler to registered callbacks for link changes
 */
typedef struct _ls_handler_cb_s {
    struct _ls_handler_cb_s  *next;
    bcm_linkscan_handler_t   cb_f;
} _ls_handler_cb_t;

/*
 * Linkscan error state
 */
typedef struct _ls_errstate_s {
    int          limit;    /* # errors to enter error state */
    int          delay;    /* Length of error state in seconds */
    int          count;    /* # of errors so far */
    int          wait;     /* Boolean, TRUE when in error state */
    sal_usecs_t  time;     /* Time error state was entered */
} _ls_errstate_t;

/*
 * Linkscan Module control structure
 */
typedef struct _ls_control_s {
    const _bcm_ls_driver_t  *driver;            /* Device specific routines */
    sal_mutex_t       lock;               /* Synchronization */
    char              taskname[16];       /* Linkscan thread name */
    VOL sal_thread_t  thread_id;          /* Linkscan thread id */
    VOL int           interval_us;        /* Time between scans (us) */
    sal_sem_t         sema;               /* Linkscan semaphore */
    pbmp_t            pbm_hw;             /* Hardware link scan ports */
    pbmp_t            pbm_sw;             /* Software link scan ports */
    pbmp_t            pbm_hw_upd;         /* Ports requiring HW link re-scan */
    pbmp_t            pbm_sgmii_autoneg;  /* Ports with SGMII autoneg */
    pbmp_t            pbm_link;           /* Ports currently up */
    pbmp_t            pbm_link_change;    /* Ports needed to recognize down */
    pbmp_t            pbm_override_ports; /* Force up/Down ports */
    pbmp_t            pbm_override_link;  /* Force up/Down status */
    pbmp_t            pbm_newly_enabled;  /* indicate linkscan is newly enabled for ports*/
    pbmp_t            pbm_remote_fault;   /* Ports receiving remote fault */
    int               link_fault_chk_en;  /* Enable Link remote fault checking in Linkscan thread */
    pbmp_t            pbm_local_fault;    /* Ports receiving local fault */
    int               link_local_fault_chk_en;  /* Enable Link local fault checking in Linkscan thread */
    int               hw_change;          /* HW Link state has changed */
    _ls_errstate_t    error[NUM_PORTS];   /* Link error state */
    _ls_handler_cb_t  *handler_cb;        /* Handler to list of callbacks */
    bcm_linkscan_port_handler_t
                      port_link_f[NUM_PORTS]; /* Port link fn */
} _ls_control_t;

_ls_control_t           *_linkscan_control[BCM_LOCAL_UNITS_MAX];

#define LS_CONTROL(unit)        (_linkscan_control[unit])

#define LS_CONTROL_DRV(unit)   (_linkscan_control[unit]->driver)

#define _LS_CALL(_ld, _lf, _la) \
    ((_ld) == 0 ? BCM_E_PARAM : \
     ((_ld)->_lf == 0 ? BCM_E_UNAVAIL : (_ld)->_lf _la))

#define LS_HW_INTERRUPT(_u, _b) \
    do { \
        if (LS_CONTROL_DRV(_u) && (LS_CONTROL_DRV(_u)->ld_hw_interrupt)) { \
            LS_CONTROL_DRV(_u)->ld_hw_interrupt((_u), (_b)); \
        } \
    } while (0)

#define LS_PORT_LINK_GET(_u, _p, _h, _l) \
    _LS_CALL(LS_CONTROL_DRV(_u), ld_port_link_get, ((_u), (_p), (_h), (_l)))

#define LS_INTERNAL_SELECT(_u, _p) \
    _LS_CALL(LS_CONTROL_DRV(_u), ld_internal_select, ((_u), (_p)))

#define LS_UPDATE_ASF(_u, _p, _l, _s, _d) \
    _LS_CALL(LS_CONTROL_DRV(_u), ld_update_asf, ((_u), (_p), (_l), (_s), (_d)))

#define LS_TRUNK_SW_FAILOVER_TRIGGER(_u, _a, _s) \
    _LS_CALL(LS_CONTROL_DRV(_u), ld_trunk_sw_failover_trigger, \
             ((_u), (_a), (_s)))


/*
 * Define:
 *     LS_LOCK/LS_UNLOCK
 * Purpose:
 *     Serialization Macros for access to _linkscan_control structure.
 */

#define LS_LOCK(unit) \
        sal_mutex_take(_linkscan_control[unit]->lock, sal_mutex_FOREVER)

#define LS_UNLOCK(unit) \
        sal_mutex_give(_linkscan_control[unit]->lock)

/*
 * General util macros
 */
#define UNIT_VALID_CHECK(unit) \
    if (((unit) < 0) || ((unit) >= BCM_LOCAL_UNITS_MAX)) { return BCM_E_UNIT; }

#define UNIT_INIT_CHECK(unit) \
    do { \
        UNIT_VALID_CHECK(unit); \
        if (_linkscan_control[unit] == NULL) { return BCM_E_INIT; } \
    } while (0)

#ifdef BCM_WARM_BOOT_SUPPORT
#define LINK_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define LINK_WB_CURRENT_VERSION            LINK_WB_VERSION_1_0
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_DNX_SUPPORT
/*
 * indicates that the API does not require error recovery support
 */
#define LINKSCAN_ERR_RECOVERY_NOT_NEEDED(_unit)\
if (SOC_IS_DNX(_unit))                         \
{                                              \
    DNX_ERR_RECOVERY_NOT_NEEDED(_unit);        \
}
#else /* BCM_DNX_SUPPORT */
#define LINKSCAN_ERR_RECOVERY_NOT_NEEDED(_unit)
#endif /* BCM_DNX_SUPPORT */


/* Local Function Declaration */
int bcm_common_linkscan_enable_set(int unit, int us);

/*
 * Function:
 *     _bcm_linkscan_pbm_init
 * Purpose:
 *     Initialize the port bitmaps in the link_control structure.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Notes:
 *     Assumes: valid unit number, lock is held.
 */
STATIC void
_bcm_linkscan_pbm_init(int unit)
{
    _ls_control_t  *lc = LS_CONTROL(unit);
    /*
     * Force initial scan by setting link change while pretending link
     * was initially up.
     */
    BCM_PBMP_ASSIGN(lc->pbm_link, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(lc->pbm_link_change, PBMP_PORT_ALL(unit));

    BCM_PBMP_CLEAR(lc->pbm_override_ports);
    BCM_PBMP_CLEAR(lc->pbm_override_link);
    BCM_PBMP_CLEAR(lc->pbm_newly_enabled);
    BCM_PBMP_CLEAR(lc->pbm_remote_fault);
    BCM_PBMP_CLEAR(lc->pbm_local_fault);

    BCM_PBMP_ASSIGN(lc->pbm_hw_upd, PBMP_ALL(unit));
}

/*
 * Function:
 *      _bcm_link_fault_get
 * Purpose:
 *      Check for remote link which is already up.
 * Parameters:
 *      unit - unit.
 *      port - Port to process.
 *      remote_fault - (IN/OUT) Remote fault status.
 *      local_fault - (IN/OUT) Local fault status.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_link_fault_get(int unit, int port, int *remote_fault, int *local_fault)
{
    int rv, speed;
    uint32 flags;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        if (IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port)) {
            return BCM_E_UNAVAIL;
        }
    } else
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit)) {
        return BCM_E_UNAVAIL;
    } else
#endif
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_CE_PORT(unit, port) || IS_XL_PORT(unit, port) ||
        IS_CL_PORT(unit, port)) {
        rv = bcm_port_speed_get(unit, port, &speed);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        /* check if the port has no fault status to retrieve */
        if (speed < 5000) {
            return BCM_E_UNAVAIL;
        }
    } else {
        return BCM_E_UNAVAIL;
    }
    /* get the indications */
    rv = bcm_port_fault_get(unit, port, &flags);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (flags & BCM_PORT_FAULT_REMOTE) {
        *remote_fault = TRUE;
    }
    if (flags & BCM_PORT_FAULT_LOCAL) {
        *local_fault = TRUE;
    }
    /* clear the indications */
    rv = bcm_port_control_set(unit,
                              port,
                              bcmPortControlLinkFaultLocal, 0);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    rv = bcm_port_control_set(unit,
                              port,
                              bcmPortControlLinkFaultRemote, 0);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_link_fault_clear
 * Purpose:
 *     clear the fault indications
 * Parameters:
 *     unit          - Device number
 *     port          - Port to process.
 * Returns:
 *     SOC_E_NONE
 */

STATIC int
_bcm_link_fault_clear(int unit, int port)
{
    int rv = BCM_E_NONE;
    int speed;

    if(IS_QSGMII_PORT(unit, port) || IS_SFI_PORT(unit, port)
        || IS_IL_PORT(unit, port)) {
        return BCM_E_UNAVAIL;
    }

    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_CE_PORT(unit, port) || IS_XL_PORT(unit, port) ||
        IS_CL_PORT(unit, port)) {
        rv = bcm_port_speed_get(unit, port, &speed);
        if (BCM_FAILURE(rv) || speed < 5000 ) {
            return BCM_E_UNAVAIL;
        } else {
            /* clear the indications */
            rv = bcm_port_control_set(unit,
                                      port,
                                      bcmPortControlLinkFaultLocal, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            rv = bcm_port_control_set(unit,
                                      port,
                                      bcmPortControlLinkFaultRemote, 0);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Function:    
 *     _bcm_linkscan_update_port
 * Purpose:     
 *     Check for and process a link event on one port.
 * Parameters:  
 *     unit - Device unit number
 *     port - Device port to process
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Assumes: valid unit number, lock is held.
 */
STATIC int
_bcm_linkscan_update_port(int unit, int port)
{
    _ls_control_t       *lc = LS_CONTROL(unit);
    int                 cur_link, change, new_link = FALSE;
    bcm_port_info_t     info;
    _ls_handler_cb_t    *lh, *lh_next = NULL;
    int                 rv;
    int                 cur_remote_fault = FALSE, new_remote_fault = FALSE;
    int                 cur_local_fault = FALSE, new_local_fault = FALSE;
    soc_pbmp_t          pbm_link_fwd;
    int                 fault_chk = 0;

    assert(SOC_PORT_VALID(unit, port)); 
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
     *           link status from either:
     *           - registered port link routine (registered
     *             with 'bcm_linkscan_port_register'),
     *           - or the PHY.
     */

    cur_link = BCM_PBMP_MEMBER(lc->pbm_link, port);
    cur_remote_fault = SOC_PBMP_MEMBER(lc->pbm_remote_fault, port);
    cur_local_fault = SOC_PBMP_MEMBER(lc->pbm_local_fault, port);
    change = BCM_PBMP_MEMBER(lc->pbm_link_change, port);
    BCM_PBMP_PORT_REMOVE(lc->pbm_link_change, port);

    if (change) {                                                  /* 2) */
        new_link = FALSE;
        rv = BCM_E_NONE;

    } else if (BCM_PBMP_MEMBER(lc->pbm_override_ports, port)) {    /* 1) */
        new_link = BCM_PBMP_MEMBER(lc->pbm_override_link, port);
        rv = BCM_E_NONE;

    } else if (BCM_PBMP_MEMBER(lc->pbm_hw, port)) {                /* 3a) */
        new_link = BCM_PBMP_MEMBER(lc->pbm_hw_upd, port);
        /*
         * If link up, we should read the link status from the PHY because
         * the PHY may link up but autneg may not have completed.
         */
        rv = BCM_E_NONE;

#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
        } else
#endif /* BCM_CMICX_SUPPORT */
        {
            if (new_link ||
                BCM_PBMP_MEMBER(lc->pbm_sgmii_autoneg, port)) {
                /* Fall back on SW linkscan to detect possible medium change */
                rv = LS_PORT_LINK_GET(unit, port, 1, &new_link);
                /* Fault check enable on HW linkscan, we should clear the fault indication first*/
                if (soc_feature(unit, soc_feature_hw_linkscan_fault_support) &&
                    (lc->link_fault_chk_en == 1 || lc->link_local_fault_chk_en == 1)) {
                    /*We must clear fault when state1(up, fault) was changed to state2(up, no_fault)*/
                    if (new_link && (cur_remote_fault || cur_local_fault)) {
                        rv = _bcm_link_fault_clear(unit, port);
                        if (rv == BCM_E_UNAVAIL) {
                            rv = BCM_E_NONE;
                        }
                    }
                }
            }
        }
    } else if (BCM_PBMP_MEMBER(lc->pbm_sw, port)) {                /* 3b) */
        if (lc->port_link_f[port]) {    /* Use registered port link function */
            int state;

            rv = lc->port_link_f[port](unit, port, &state);
            if (rv == BCM_E_NONE) {
                new_link = state ? TRUE : FALSE;
            } else if (rv == BCM_E_UNAVAIL) {  /* Retrieve PHY link */
                rv = LS_PORT_LINK_GET(unit, port, 0, &new_link);
            }
        } else {
            rv = LS_PORT_LINK_GET(unit, port, 0, &new_link);
        }
        LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "SW link p=%d %s\n"),
                     port, new_link ? "up" : "down"));
    } else {
        return BCM_E_NONE;    /* Port not being scanned */
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "Port %s: Failed to recover link status: %s\n"), 
                   SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
        return rv;
    }

    if (new_link && (lc->link_fault_chk_en == 1 || lc->link_local_fault_chk_en == 1)) {
        if (soc_feature(unit, soc_feature_hw_linkscan_fault_support)) {
            fault_chk = (BCM_PBMP_MEMBER(lc->pbm_sw, port) | BCM_PBMP_MEMBER(lc->pbm_hw, port));
        } else {
            fault_chk = BCM_PBMP_MEMBER(lc->pbm_sw, port);
        }
        if (fault_chk) {
            rv = _bcm_link_fault_get(unit, port, &new_remote_fault, &new_local_fault);
            if (rv == BCM_E_UNAVAIL) {
                rv = BCM_E_NONE;
            } else if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                           "Unit %d, Port %s: Failed to read fault status: %s\n"),
                           unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                return rv;
            }
            /* Set remote fault to be false if not enabling remote fault checking */
            if (!lc->link_fault_chk_en) {
                new_remote_fault = FALSE;
            }
            /* Set local fault to be false if not enabling local fault checking */
            if (!lc->link_local_fault_chk_en) {
                new_local_fault = FALSE;
            }
        }
    }

    if ((((cur_link) && (new_link) && (cur_remote_fault == new_remote_fault) &&
        (cur_local_fault == new_local_fault)) ||
        ((!cur_link) && (!new_link))) && !(BCM_PBMP_MEMBER(lc->pbm_newly_enabled, port))) {
        /* No change and is not newly enabled*/
        return BCM_E_NONE;
    }

    /*
     * If disabling, stop ingresses from sending any more traffic to
     * this port.
     */

    if (!new_link) {
        BCM_PBMP_PORT_REMOVE(lc->pbm_link, port);
        SOC_PBMP_ASSIGN(pbm_link_fwd, lc->pbm_link);
        SOC_PBMP_PORT_REMOVE(lc->pbm_remote_fault, port);
        SOC_PBMP_REMOVE(pbm_link_fwd, lc->pbm_remote_fault);
        SOC_PBMP_PORT_REMOVE(lc->pbm_local_fault, port);
        SOC_PBMP_REMOVE(pbm_link_fwd, lc->pbm_local_fault);
        rv = soc_linkctrl_link_fwd_set(unit, pbm_link_fwd);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: soc_linkctrl_link_fwd_set failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    /* Program MACs (only if port is not forced) */

    /*
     * For AradPlus devices, as per SDK-51076 when link is up,
     * USE_EXTERNAL_FAULT_FOR_TX should be cleared, otherwise
     * remote fault will be sent from AradPlus. When link is
     * down, that field should be set.
     */
    if ((!BCM_PBMP_MEMBER(lc->pbm_override_ports, port) ||
        SOC_IS_ARADPLUS_A0(unit)) &&
        (cur_link != new_link)) {
        rv = bcm_port_update(unit, port, new_link);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: bcm_port_update failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    /*
     * If enabling, allow traffic to go to this port.
     */

    if (new_link) {
        BCM_PBMP_PORT_ADD(lc->pbm_link, port);
        SOC_PBMP_ASSIGN(pbm_link_fwd, lc->pbm_link);

        if (!new_remote_fault) {
            SOC_PBMP_PORT_REMOVE(lc->pbm_remote_fault, port);
        } else {
            SOC_PBMP_PORT_ADD(lc->pbm_remote_fault, port);
        }
        if (!new_local_fault) {
            SOC_PBMP_PORT_REMOVE(lc->pbm_local_fault, port);
        } else {
            SOC_PBMP_PORT_ADD(lc->pbm_local_fault, port);
        }
        SOC_PBMP_REMOVE(pbm_link_fwd, lc->pbm_remote_fault);
        SOC_PBMP_REMOVE(pbm_link_fwd, lc->pbm_local_fault);

        rv = soc_linkctrl_link_fwd_set(unit, pbm_link_fwd);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: soc_linkctrl_link_fwd_set failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    /*
     * Call registered handlers with complete link info.
     * Display link status message, if requested.
     *
     * In case link status changed again for bcm_port_info_get,
     * overwrite the linkstatus field with new_link.  This ensures
     * the handler is presented with a consistent alternating
     * sequence of link up/down.
     */

    if(soc_feature(unit, soc_feature_linkscan_remove_port_info)){
        bcm_port_info_t_init(&info);
        info.action_mask = BCM_PORT_ATTR_LINKSTAT_MASK;
    } else {
        rv = bcm_port_info_get(unit, port, &info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: bcm_port_info_get failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    if (soc_feature(unit, soc_feature_asf)) {
        rv = LS_UPDATE_ASF(unit, port, new_link, info.speed, info.duplex);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Port %s: linkscan ASF update failed: %s\n"),
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            return rv;
        }
    }

    if (new_link) {
        if(soc_feature(unit, soc_feature_linkscan_remove_port_info)){
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                        (BSL_META_U(unit,
                                    "Port %s: link up\n"),
                         SOC_PORT_NAME(unit, port)));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "Port %s: link up (%dMb %s)\n"),
                     SOC_PORT_NAME(unit, port),
                     info.speed,
                     info.duplex ? "Full Duplex" : "Half Duplex"));
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "Port %s: link down\n"),
                     SOC_PORT_NAME(unit, port)));
    }

    /* coverity[dead_error_line] */
    info.linkstatus = new_remote_fault ? (new_local_fault ? BCM_PORT_LINK_STATUS_LOCAL_AND_REMOTE_FAULT :
                      BCM_PORT_LINK_STATUS_REMOTE_FAULT ) : (new_local_fault ?
                      BCM_PORT_LINK_STATUS_LOCAL_FAULT : new_link);

    for (lh = lc->handler_cb; lh; lh = lh_next) {
        /*
         * save the next linkscan handler first, in case current handler
         * unregister itself inside the handler function
         */
        lh_next = lh->next;        
        lh->cb_f(unit, port, &info);
    }
    if (BCM_PBMP_MEMBER(lc->pbm_newly_enabled, port)) {
        BCM_PBMP_PORT_REMOVE(lc->pbm_newly_enabled, port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_linkscan_update
 * Purpose:     
 *     Check for a change in link status on each link.  If a change
 *     is detected, call bcm_port_update to program the MACs for that
 *     link, and call the list of registered handlers.
 * Parameters:  
 *     unit - Device unit number
 *     pbm  - Bitmap of ports to scan
 *     force_link_get - hw link get
 * Returns:
 *     Nothing.
 * Notes:
 *     Assumes: valid unit number.
 */
STATIC void
_bcm_linkscan_update(int unit, pbmp_t pbm, int force_link_get)
{
    _ls_control_t  *lc = LS_CONTROL(unit);
    pbmp_t         save_link_change;
    int            rv;
    bcm_port_t     port;

    LS_LOCK(unit);

    /*
     * Suspend hardware link scan here to avoid overhead of pause/resume
     * on MDIO accesses. Ideally this would not be required if the source
     * of the interrupt is the signal from the internal Serdes but we still
     * need to do this due to work-arounds in phy drivers where we need to
     * rely on SW linkscan to detect link up.
     */
    soc_linkctrl_linkscan_pause(unit);           /* Suspend linkscan */

    PBMP_ITER(pbm, port) {

        _ls_errstate_t *err = &lc->error[port];

        /* This check is done in case of a scenario of removing a port(dynamic port provisioning)
           right after the initiliaztion of the valid ports bitmap, causing for _bcm_linkscan_update
           call with an invalid port */
        if (!SOC_PORT_VALID(unit, port)) {
            continue;
        }

        if (err->wait) {    /* Port waiting in error state */
            if (SAL_USECS_SUB(sal_time_usecs(), err->time) >= err->delay) {
                err->wait = 0;    /* Exit error state */
                err->count = 0;

                LOG_WARN(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                     "Port %s: restored\n"),
                          SOC_PORT_NAME(unit, port)));
            } else {
                continue;
            }
        }

        save_link_change = lc->pbm_link_change;

        rv = _bcm_linkscan_update_port(unit, port);

        if (BCM_FAILURE(rv)) {

            lc->pbm_link_change = save_link_change;

            if (++err->count >= err->limit && err->limit > 0) {
                /* Enter error state */
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "Port %s: temporarily removed from linkscan\n"),
                           SOC_PORT_NAME(unit, port)));

                err->time = sal_time_usecs();
                err->wait = 1;
            }
        } else if (err->count > 0) {
            err->count--;    /* Reprieve */
        }
    }
    soc_linkctrl_linkscan_continue(unit);        /* Restart H/W link scan */

#ifdef BCM_CMICX_SUPPORT
    if (force_link_get && soc_feature(unit, soc_feature_cmicx)) {
        int nocare_link = 0;

        PBMP_ITER(pbm, port) {
            /* Call below API in order to trigger 400G an restart.
             * It is no harm to other speed.
             * After 400G AN completed,if we first disable port,then enable port,
             * we will find the port can't be up again if not calling below API.
             * The reason is trigger an restart is in pm8x50_port_link_get(),not
             * in port disable/enable process. Hence,once the port's link change,
             * we must trigger an restart again.
             */
            if (!IS_SFI_PORT(unit, port)) {
                rv = LS_PORT_LINK_GET(unit, port, 1, &nocare_link);
            }
        }
    }
#endif /* BCM_CMICX_SUPPORT */

    LS_UNLOCK(unit);
}

/*
 * Function:    
 *     _bcm_linkscan_hw_interrupt
 * Purpose:     
 *     Link scan interrupt handler.
 * Parameters:  
 *     unit - Device unit number
 * Returns:     
 *     Nothing
 * Notes:
 *     Assumes: valid unit number.
 */
STATIC void
_bcm_linkscan_hw_interrupt(int unit)
{
    _ls_control_t  *lc = LS_CONTROL(unit);

    if ((NULL != lc) && (NULL != lc->sema)) {
        lc->hw_change = 1;
        sal_sem_give(lc->sema);
    }
    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Linkscan interrupt unit %d\n"), unit));
}

/*
 * Function:    
 *     _bcm_linkscan_thread
 * Purpose:     
 *     Scan the ports on the specified unit for link status
 *     changes and process them as they occur.
 * Parameters:  
 *     unit - Device unit number
 * Returns:     
 *     Nothing
 * Notes:
 *     Assumes: valid unit number.
 */
STATIC void
_bcm_linkscan_thread(int unit)
{
    _ls_control_t  *lc = LS_CONTROL(unit);
    sal_usecs_t    interval;
    int            rv;
    soc_port_t     port;
    char           thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t   thread;

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_LINKSCAN);
    }
#endif

    LOG_INFO(BSL_LS_BCM_LINK,
             (BSL_META_U(unit,
                         "Linkscan starting on unit %d\n"),
              unit));
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    /* Do not clear the pbm_override_ports and pbm_override_link.
     * If a port in Loopback mode forces link up before enabling linkscan,
     * the port status should be still forced to up after enabling linkscan.
     */

    BCM_PBMP_ASSIGN(lc->pbm_link, PBMP_ALL(unit));

    sal_memset(lc->error, 0, sizeof (lc->error));

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        lc->error[port].limit = soc_property_port_get(unit, port,
                                         spn_BCM_LINKSCAN_MAXERR, 5);
        lc->error[port].delay = soc_property_port_get(unit, port,
                                         spn_BCM_LINKSCAN_ERRDELAY, 10000000);
    }

    /* Clear initial value of forwarding ports. */
    rv = soc_linkctrl_link_fwd_set(unit, lc->pbm_link);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s, Failed to clear forwarding ports: %s\n"), 
                   thread_name, bcm_errmsg(rv)));
        DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
        sal_thread_exit(0);
    }

    /* Register for hardware linkscan interrupt. */
    rv = soc_linkctrl_linkscan_register(unit, _bcm_linkscan_hw_interrupt);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s, Failed to register handler: %s\n"),
                   thread_name, bcm_errmsg(rv)));
        DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
        sal_thread_exit(0);
    }

    lc->thread_id = sal_thread_self();

    while ((interval = lc->interval_us) != 0) {
        pbmp_t  change;
        pbmp_t  hw_link, hw_update;
        pbmp_t  all_update;

        if (BCM_PBMP_IS_NULL(lc->pbm_sw)) {
            interval = sal_sem_FOREVER;
        }
        BCM_PBMP_CLEAR(hw_link);
        /* all port changed */
        BCM_PBMP_ASSIGN(all_update, PBMP_PORT_ALL(unit));
        /* sample changed */
        BCM_PBMP_ASSIGN(change, lc->pbm_link_change);

        if (lc->hw_change) {
            soc_linkctrl_linkscan_pause(unit);
            lc->hw_change = 0;

            (void)soc_linkctrl_hw_link_get(unit, &hw_link);
            BCM_PBMP_AND(hw_link, lc->pbm_hw);
#ifdef BCM_TRUNK_FAILOVER_SUPPORT
            LS_TRUNK_SW_FAILOVER_TRIGGER(unit, lc->pbm_hw, lc->pbm_hw_upd);
#endif /* BCM_TRUNK_FAILOVER_SUPPORT */

            /* Make sure that only valid ports are scaned */
            BCM_PBMP_ASSIGN(hw_update, hw_link);

            /* In _bcm_linkscan_update_port, new_link is retrieved from pbm_hw_upd */
            BCM_PBMP_ASSIGN(lc->pbm_hw_upd, hw_link);

#ifdef BCM_CMICX_SUPPORT
            /* For CMICX, hw_update is really link-up pbmp from FW, 
             * don't have to process linkscan update twice.
             * For CMICm, the hw_update is all_port pbmp, and have to process  
             * linkscan update twice for all ports in case of missing of interrupt.
             */
            if (soc_feature(unit, soc_feature_cmicx)) {
                BCM_PBMP_XOR(hw_update, lc->pbm_link);
                BCM_PBMP_REMOVE(all_update, lc->pbm_hw);
            }
#endif /* BCM_CMICX_SUPPORT */

            BCM_PBMP_AND(hw_update, lc->pbm_hw);
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                pbmp_t  hw_change_down, hw_change_up;

                BCM_PBMP_ASSIGN(hw_change_down, hw_update);
                BCM_PBMP_AND(hw_change_down, lc->pbm_link);
                /* link down event ports scaned first */
                _bcm_linkscan_update(unit, hw_change_down, 1);

                /* link up event ports scaned first */
                BCM_PBMP_NEGATE(hw_change_up, hw_change_down);
                BCM_PBMP_AND(hw_change_up, hw_update);
                _bcm_linkscan_update(unit, hw_change_up, 0);
            } else
#endif
            {
                /* Make sure that only valid ports are scaned */
                _bcm_linkscan_update(unit, hw_update, 0);
            }
            /* Make sure that only valid ports are scaned */
            BCM_PBMP_AND(change, PBMP_PORT_ALL(unit));
            _bcm_linkscan_update(unit, change, 0);
            soc_linkctrl_linkscan_continue(unit);
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

        _bcm_linkscan_update(unit, all_update, 0);


        if (!BCM_PBMP_IS_NULL(change)) {
            /* Re-scan due to hardware force */
            continue;
        }

        (void)sal_sem_take(lc->sema, interval);
    }

    (void)soc_linkctrl_linkscan_register(unit, NULL);

    /*
     * Before exiting, re-enable all ports that were being scanned.
     *
     * For administrative reloads, application can enter reload
     * mode to avoid this disturbing of ports.
     */

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        (void)dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN);
    }
#endif
    BCM_PBMP_ITER(lc->pbm_sw, port) {
        int enable;

        if (BCM_SUCCESS(bcm_port_enable_get(unit, port, &enable))) {
            (void)bcm_port_update(unit, port, enable);
        }
    }

    BCM_PBMP_ITER(lc->pbm_hw, port) {
        int enable;

        if (BCM_SUCCESS(bcm_port_enable_get(unit, port, &enable))) {
            (void)bcm_port_update(unit, port, enable);
        }
    }
#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        (void)dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN);
    }
#endif
    LOG_INFO(BSL_LS_BCM_LINK,
             (BSL_META_U(unit,
                         "Linkscan exiting\n")));

    lc->thread_id = NULL;

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_LINKSCAN);
    }
#endif

    sal_thread_exit(0);
}


/*
 * Internal BCM link routines
 */

/*
 * Function:    
 *     _bcm_link_get
 * Purpose:
 *     Return linkscan's current link status for the given port.
 * Parameters:  
 *     unit - Device unit number
 *     port - Device port number
 *     link - (OUT) Current link status
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_DISABLED - Port not being scanned
 * Note:
 *     This routine does not acquire the LS_LOCK, as it only reads a 
 *     snapshot of the link bitmaps.  It also cannot hold the LS_LOCK 
 *     since it is called indirectly from the linkscan thread 
 *     when requesting port info.
 */
int
_bcm_link_get(int unit, bcm_port_t port, int *link)
{
    int            rv = BCM_E_NONE;
    _ls_control_t  *lc;
    UNIT_VALID_CHECK(unit);

    if ((lc = LS_CONTROL(unit)) == NULL) {
        return BCM_E_DISABLED;
    }

    if (BCM_PBMP_MEMBER(lc->pbm_override_ports, port)) {
        *link = BCM_PBMP_MEMBER(lc->pbm_override_link, port);
        return BCM_E_NONE;
    }

    rv = bcm_linkscan_enable_port_get(unit, port);
    if (BCM_SUCCESS(rv)) {
        *link = (SOC_PBMP_MEMBER(lc->pbm_link, port) &&
                 !SOC_PBMP_MEMBER(lc->pbm_remote_fault, port) &&
                 !SOC_PBMP_MEMBER(lc->pbm_local_fault, port)) ?
                 BCM_PORT_LINK_STATUS_UP : BCM_PORT_LINK_STATUS_DOWN;


    }

    return rv;
}

/*
 * Function:    
 *     _bcm_link_force
 * Purpose:
 *     Set linkscan's current link status for a port.
 * Parameters:  
 *     unit  - Device unit number
 *     port  - Device port number
 *     force - If TRUE, link status is forced to new link status;
 *             if FALSE, link status is no longer forced
 *     link  - New link status
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_INIT - Not initialized.
 * Notes:
 *     When a link is forced up or down, linkscan will stop scanning
 *     that port and _bcm_link_get will always return the forced status.
 *     This is used for purposes such as when a link is placed in MAC
 *     loopback.  If software forces a link up, it is responsible for
 *     configuring that port.
 */
int
_bcm_link_force(int unit, bcm_port_t port, int force, int link)
{
    _ls_control_t  *lc;
    pbmp_t         pbm;

    UNIT_INIT_CHECK(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LS_LOCK(unit);
    lc = LS_CONTROL(unit);

    if (force) {
        BCM_PBMP_PORT_REMOVE(lc->pbm_override_link, port);
        if (link) {
            BCM_PBMP_PORT_ADD(lc->pbm_override_link, port);
        }
        BCM_PBMP_PORT_ADD(lc->pbm_override_ports, port);
    } else {
        BCM_PBMP_PORT_REMOVE(lc->pbm_override_ports, port);
        BCM_PBMP_PORT_REMOVE(lc->pbm_override_link, port);
        BCM_PBMP_PORT_ADD(lc->pbm_link_change, port);
    }

    /*
     * Force immediate update to just this port - this allows loopback 
     * forces to take effect immediately.
     */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, port);
    _bcm_linkscan_update(unit, pbm, 0);

    LS_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->sema != NULL) {
        sal_sem_give(lc->sema);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_linkscan_init
 * Purpose:
 *     Initialize the linkscan software module.
 * Parameters:
 *     unit   - Device unit number
 *     driver - Device specific link routines
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Device specific bcm_linkscan_init() must call this routine
 *     in order to initialize BCM linkscan common module.
 */
int
_bcm_linkscan_init(int unit, const _bcm_ls_driver_t *driver)
{
    _ls_control_t   *lc;
    bcm_port_if_t   intf;
    int             rv = BCM_E_NONE;
    uint32          size;
    soc_port_t      port;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint32          scache_handle;
    uint8           *scache_ptr = NULL;
    uint16          default_ver = LINK_WB_CURRENT_VERSION;
    uint16          recovered_ver = LINK_WB_CURRENT_VERSION;
    _ls_control_t   *free_lc = NULL;
#endif

    UNIT_VALID_CHECK(unit);

    if (_linkscan_control[unit] != NULL) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_detach(unit));
    }

    size = sizeof(_ls_control_t);
    if ((lc = sal_alloc(size, "link_control")) == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(lc, 0, size);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LINKSCAN, 0);

    /* on cold boot, setup scache */
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      (SOC_WARM_BOOT(unit) ? FALSE : TRUE),
                                      &size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, size));
        /* If warmboot initialization fails, skip using warmboot for linkscan */
        rv = SOC_E_NOT_FOUND;
        goto skip_wb;
    }

    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "LINKSCAN: allocating 0x%x (%d) bytes of scache:"),
                 size, size));

    rv = soc_scache_handle_used_set(unit, scache_handle, size);
    
    /* Get the pointer for the Level 2 cache */
    free_lc = lc;
    
    if (scache_ptr) {
        /* if supporting warmboot, use scache */
        sal_free(lc);
        free_lc = NULL;
        lc = (_ls_control_t*) scache_ptr;

        if (SOC_WARM_BOOT(unit)) {
            /* clear out non-recoverable resources */
            lc->thread_id = NULL;
            lc->sema = NULL;
            lc->lock = NULL;
            lc->driver = NULL;
            lc->handler_cb = NULL;
            sal_memset(lc->port_link_f, 0, sizeof(lc->port_link_f));
        } else {
            sal_memset(lc, 0, size);
        }
    }

skip_wb:
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (BCM_SUCCESS(rv) || (rv == SOC_E_NOT_FOUND)) {
        lc->lock = sal_mutex_create("bcm_link_LOCK");
        if (lc->lock == NULL) {
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv) || (rv == SOC_E_NOT_FOUND)) {
        lc->sema = sal_sem_create("bcm_link_SLEEP", 
                                  sal_sem_BINARY, 0);
        if (lc->sema == NULL) {
            sal_mutex_destroy(lc->lock);
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
#ifdef BCM_WARM_BOOT_SUPPORT
        if (free_lc) {
            sal_free(free_lc);
        } else
#endif
        {
            sal_free(lc);
        }

        return rv;
    }

    _linkscan_control[unit] = lc;
    _linkscan_control[unit]->driver = driver;

    /*
     * Initialize link_control port bitmaps so bcm_port_update works
     * reasonably even if the linkscan thread is never started.
     */
    if (!SOC_WARM_BOOT(unit)) {
        _bcm_linkscan_pbm_init(unit);
    }

    /* 1. Select between C45 and C22 for HW linkscan
     * 2. Select appropriate MDIO Bus
     * 3. Select between MDIO scan vs. link status from internal PHY
     * 4. Initialize HW linkscan PHY address map.
     */
    if (!(soc_feature(unit, soc_feature_cmicx) && SAL_BOOT_PLISIM)) {
        BCM_IF_ERROR_RETURN(soc_linkctrl_linkscan_hw_init(unit));
    }
    /*
     * Select the source of the CMIC link status interrupt
     * to be the Internal Serdes on SGMII ports
     */ 
    BCM_PBMP_CLEAR(lc->pbm_sgmii_autoneg);
    if (soc_feature(unit, soc_feature_sgmii_autoneg)) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (soc_property_port_get(unit, port,
                                      spn_PHY_SGMII_AUTONEG, FALSE)) {
                rv = bcm_port_interface_get(unit, port, &intf);
                if ( BCM_SUCCESS(rv) && (intf == BCM_PORT_IF_SGMII)){
                    BCM_PBMP_PORT_ADD(lc->pbm_sgmii_autoneg, port);
                    LS_INTERNAL_SELECT(unit, port);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/* This function locks the LS thread, it's used outside the scope of the module in order to protect dynamic port
   provioning from removing a port while the thread is active, which causes a failure in port validity check */
int
_bcm_linkscan_pause(int unit)
{
    UNIT_VALID_CHECK(unit);

    /*  check if Linkscan is active */
    if (LS_CONTROL(unit)) {
        /* Lock the mutex */
        LS_LOCK(unit);
    }

    return BCM_E_NONE;
}


/* This function unlocks the LS thread, it's used outside the scope of the module in order to protect dynamic port
   provioning from removing a port while the thread is active, which causes a failure in port validity check */
int
_bcm_linkscan_continue(int unit)
{
    UNIT_VALID_CHECK(unit);

    /*  check if Linkscan is active */
    if (LS_CONTROL(unit)) {
        /* Lock the mutex */
        LS_UNLOCK(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_linkscan_available
 * Purpose:
 *     To check if linkscan module is initialized.
 * Parameters:
 *     unit   - Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_linkscan_available(int unit)
{
    _ls_control_t  *lc;
    UNIT_VALID_CHECK(unit);

    if ((lc = LS_CONTROL(unit)) == NULL) {
        return BCM_E_DISABLED;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_linkscan_port_init
 * Purpose:
 *     Add the port to bitmaps in the link_control structure.
 * Parameters:
 *     unit - Device unit number
 *     port - Logical port ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Used for dynamic port provisioning.
 */
int
_bcm_linkscan_port_init(int unit, int port)
{
    _ls_control_t  *lc = LS_CONTROL(unit);

    UNIT_VALID_CHECK(unit);

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    /*
     * Force initial scan by setting link change while pretending link
     * was initially up.
     */
    LS_LOCK(unit);
    BCM_PBMP_PORT_ADD(lc->pbm_link, port);
    BCM_PBMP_PORT_ADD(lc->pbm_link_change, port);
    BCM_PBMP_PORT_ADD(lc->pbm_hw_upd, port);

    /* Remove force configure while port initial */
    BCM_PBMP_PORT_REMOVE(lc->pbm_override_ports, port);
    BCM_PBMP_PORT_REMOVE(lc->pbm_override_link, port);
    LS_UNLOCK(unit);

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* This function compresses the info in _ls_control_t and stores it
 * to stable memory.
 * Input param: sync --> indicates whether to sync scache to Persistent memory
 */
int
bcm_linkscan_sync(int unit, int sync)
{
    int                     rv = BCM_E_NONE;
    uint8                   *scache_ptr = NULL;
    uint32                  scache_len = 0;
    soc_scache_handle_t     scache_handle;
    uint16                  default_ver = LINK_WB_CURRENT_VERSION;
    uint16                  recovered_ver = LINK_WB_CURRENT_VERSION;

    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Cannot write to SCACHE during WarmBoot\n")));
        return SOC_E_INTERNAL;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LINKSCAN, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &scache_len, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_LINK,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, scache_len));
        return rv;
    }

    rv = soc_scache_handle_used_set(unit, scache_handle, sizeof(_ls_control_t));

    if (sync) {
        rv = soc_scache_commit(unit);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_LINK,
                      (BSL_META_U(unit,
                                  "Error(%s) sync'ing scache to Persistent memory. \n"),
                       soc_errmsg(rv)));
            return rv;
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/************************************************************************
 *********                                                      *********
 *********         Start of BCM API Exported Routines           *********
 */

/*
 * Function:
 *     bcm_linkscan_detach
 * Purpose:
 *     Prepare linkscan module to detach specified unit.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     BCM_E_NONE - Detach successful
 *     BCM_E_XXX  - Detach failed
 * Notes:
 *     This is safe to call at any time, but linkscan should only be
 *     initialized or detached from the main application thread.
 */
int
bcm_common_linkscan_detach(int unit)
{
    _ls_control_t     *lc;
    _ls_handler_cb_t  *lh;
    pbmp_t            empty_pbm;
#ifdef BCM_WARM_BOOT_SUPPORT
    int               size, rv;
    uint32            scache_handle;
    uint8            *ptr;
    soc_wb_cache_t   *wb_cache = NULL;
#endif

    UNIT_VALID_CHECK(unit);

    lc = LS_CONTROL(unit);
    if (lc == NULL) {
        return BCM_E_NONE;
    }

    BCM_PBMP_CLEAR(empty_pbm);

    SOC_IF_ERROR_RETURN(soc_linkctrl_linkscan_config(unit,
                                                     empty_pbm, empty_pbm));

    BCM_IF_ERROR_RETURN(bcm_common_linkscan_enable_set(unit, 0));

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) && !SAL_BOOT_PLISIM)
    {
        /* Iproc M0 exit */
        soc_iproc_m0_exit(unit);
    }
#endif /* BCM_CMICX_SUPPORT */

    /* Clean up list of handlers */

    while (lc->handler_cb != NULL) {
        lh = lc->handler_cb;
        lc->handler_cb = lh->next;
        sal_free(lh);
    }

    /* Mark and not initialized and free mutex */

    if (lc->sema != NULL) {
        sal_sem_destroy(lc->sema);
        lc->sema = NULL;
    }

    if (lc->lock != NULL) {
        sal_mutex_destroy(lc->lock);
        lc->lock = NULL;
    }

    LS_CONTROL(unit) = NULL;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_LINKSCAN, 0);
    rv = soc_stable_size_get(unit, &size);
    if (BCM_SUCCESS(rv) && size) {
        rv = soc_scache_ptr_get(unit, scache_handle, &ptr, (uint32*)&size);
        wb_cache = (soc_wb_cache_t*)ptr;
    }

    /* lc was allocated when Warm Boot is compiled and 
     * the scache is not configured */
    if (BCM_SUCCESS(rv) && (wb_cache == NULL)) {
        sal_free(lc);
    }
    
#else
    sal_free(lc);
#endif

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_linkscan_enable_set
 * Purpose:
 *     Enable or disable the link scan feature.
 * Parameters:
 *     unit - Device unit number
 *     us   - Specifies the software polling interval in micro-seconds;
 *            the value 0 disables linkscan
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_enable_set(int unit, int us)
{
    _ls_control_t   *lc;
    int             rv = BCM_E_NONE;
    soc_timeout_t   to;
    pbmp_t          empty_pbm;
    sal_thread_t    tid;

    UNIT_VALID_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    lc = LS_CONTROL(unit);
    if (!us && lc == NULL) {    /* No error to disable if not initialized */
        return BCM_E_NONE;
    }

    UNIT_INIT_CHECK(unit);

    sal_snprintf(lc->taskname,
                 sizeof (lc->taskname),
                 "bcmLINK.%d",
                 unit);

    BCM_PBMP_CLEAR(empty_pbm);

    if (us) {
        if (us < BCM_LINKSCAN_INTERVAL_MIN) {
            us = BCM_LINKSCAN_INTERVAL_MIN;
        }

        lc->interval_us = us;

        if (lc->thread_id != NULL) {
            /* Linkscan is already running; just update the period */
            sal_sem_give(lc->sema);
            return BCM_E_NONE;
        }

        tid = sal_thread_create(lc->taskname,
                              SAL_THREAD_STKSZ,
                              soc_property_get(unit,
                                               spn_LINKSCAN_THREAD_PRI,
                                               50),
                              (void (*)(void*))_bcm_linkscan_thread,
                              INT_TO_PTR(unit));

        if (tid == SAL_THREAD_ERROR) {
            lc->interval_us = 0;
            rv = BCM_E_MEMORY;
        } else {
            soc_timeout_init(&to, 3000000, 0);

            while (lc->thread_id == NULL) {
                if (soc_timeout_check(&to)) {
                    if (lc->thread_id != NULL) {
                        break;
                    }
                    LOG_ERROR(BSL_LS_BCM_LINK,
                              (BSL_META_U(unit,
                                          "%s: Thread did not start after polling for %d microsec.\n"),
                              lc->taskname,soc_timeout_elapsed(&to)));
                    lc->interval_us = 0;
                    rv = BCM_E_INTERNAL;
                    break;
                }
            }
            if (BCM_SUCCESS(rv)) {
                /* Make sure HW linkscanning is enabled on HW linkscan ports */
                rv = soc_linkctrl_linkscan_config(unit,
                                                  lc->pbm_hw, empty_pbm);
            }

            sal_sem_give(lc->sema);
        }
    } else if (lc->thread_id != NULL) {
        lc->interval_us = 0;

        /* To prevent getting HW linkscan interrupt after linkscan is disabled,
         * HW linkscanning must be disabled. */ 
        rv = soc_linkctrl_linkscan_config(unit, empty_pbm, empty_pbm);

        sal_sem_give(lc->sema);

        soc_timeout_init(&to, 10000000, 0);   /* Enough time for Quickturn */

        while (lc->thread_id != NULL) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_BCM_LINK,
                          (BSL_META_U(unit,
                                      "%s: Thread did not exit\n"),
                           lc->taskname));
                rv = BCM_E_INTERNAL;
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:    
 *     bcm_linkscan_enable_get
 * Purpose:
 *     Retrieve the current linkscan mode.
 * Parameters:
 *     unit - Device unit number
 *     us   - (OUT) Pointer to microsecond scan time for software scanning, 
 *             0 if not enabled.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_enable_get(int unit, int *us)
{
    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    *us = LS_CONTROL(unit)->interval_us;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_linkscan_enable_port_get
 * Purpose:
 *     Determine whether or not linkscan is managing a given port.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port to check
 * Returns:
 *     BCM_E_NONE     - Port being scanned
 *     BCM_E_DISABLED - Port not being scanned
 */
int
bcm_common_linkscan_enable_port_get(int unit, bcm_port_t port)
{
    int            rv = BCM_E_NONE;
    _ls_control_t  *lc;

    UNIT_VALID_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if ((lc = LS_CONTROL(unit)) == NULL) {
        return BCM_E_DISABLED;
    }

    if (lc->interval_us == 0 ||
        (!BCM_PBMP_MEMBER(lc->pbm_sw, port) &&
         !BCM_PBMP_MEMBER(lc->pbm_hw, port) &&
         !BCM_PBMP_MEMBER(lc->pbm_override_ports, port))) {
        rv = BCM_E_DISABLED;
    }
    return rv;
}

/*
 * Function:    
 *     bcm_linkscan_mode_set
 * Purpose:
 *     Set the current scanning mode for the specified port.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     mode - New scan mode for specified port
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_mode_set(int unit, bcm_port_t port, int mode)
{
    _ls_control_t   *lc;
    int             rv = BCM_E_NONE;
    pbmp_t          empty_pbm;
    int             added = 0, sw_member = 0, hw_member = 0;

    UNIT_INIT_CHECK(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    BCM_PBMP_CLEAR(empty_pbm);

    LS_LOCK(unit);

    lc = LS_CONTROL(unit);

    /* First, remove from current configuration */
    if (BCM_PBMP_MEMBER(lc->pbm_sw, port)) {
        sw_member = 1;
        BCM_PBMP_PORT_REMOVE(lc->pbm_sw, port);
    } else if (BCM_PBMP_MEMBER(lc->pbm_hw, port)) {
        hw_member = 1;
        BCM_PBMP_PORT_REMOVE(lc->pbm_hw, port);
    }
    /* Now add back to proper map */
    switch (mode) {
    case BCM_LINKSCAN_MODE_NONE:
        SOC_PBMP_PORT_REMOVE(lc->pbm_remote_fault, port);
        SOC_PBMP_PORT_REMOVE(lc->pbm_local_fault, port);
        break;
    case BCM_LINKSCAN_MODE_SW:
        BCM_PBMP_PORT_ADD(lc->pbm_sw, port);
        BCM_PBMP_PORT_ADD(lc->pbm_newly_enabled, port);
        added = 1;
        break;
    case BCM_LINKSCAN_MODE_HW:
        if (lc->link_fault_chk_en == 1 ||
            lc->link_local_fault_chk_en == 1) {
#ifdef BCM_JERICHO_SUPPORT
            if (soc_feature(unit, soc_feature_hw_linkscan_fault_support)) {
                pbmp_t hw_pbm;
                BCM_PBMP_CLEAR(hw_pbm);
                BCM_PBMP_PORT_ADD(hw_pbm, port);
                rv = soc_jer_linkcan_fault_to_cmiclink_enable_set(unit, &hw_pbm,
                                                      lc->link_fault_chk_en, lc->link_local_fault_chk_en);
                if(BCM_FAILURE(rv)) {
                    rv = BCM_E_PARAM;
                    goto exit;
                }
            } else
#endif
            {
                LOG_ERROR(BSL_LS_BCM_LINK,
                         (BSL_META_U(unit,
                                  "HW link scan and link fault are not supported\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
        }

        BCM_PBMP_PORT_ADD(lc->pbm_hw, port);

        if (BCM_PBMP_MEMBER(lc->pbm_sgmii_autoneg, port)) {
            /* Need to run SW link scan as well on ports where the source 
             * of the link status is the internal Serdes - only SGMII ports */
            BCM_PBMP_PORT_ADD(lc->pbm_sw, port);
        }
        BCM_PBMP_PORT_ADD(lc->pbm_newly_enabled, port);
        added = 1;
        lc->hw_change = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                          "Unsupported linkscan mode(%d)\n"), mode));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Reconfigure HW linkscan in case changed */
    rv = soc_linkctrl_linkscan_config(unit, lc->pbm_hw, empty_pbm);

    /* Prime the HW linkscan pump */
    if (SOC_PBMP_NOT_NULL(lc->pbm_hw)) {
        if ((!SOC_IS_DNXF(unit)) && (!SOC_IS_DNX(unit)))
        {
            lc->hw_change = 1;
            _bcm_linkscan_hw_interrupt(unit);
        }
    }

    if (rv == BCM_E_UNAVAIL && added && sw_member) {
        BCM_PBMP_PORT_ADD(lc->pbm_sw, port);
        BCM_PBMP_PORT_REMOVE(lc->pbm_hw, port); 
    }

exit:
    /*Restore the configuration in case E_PARAM*/
    if (rv == BCM_E_PARAM) {
        if(sw_member) {
            BCM_PBMP_PORT_ADD(lc->pbm_sw, port);
        }
        if(hw_member) {
            BCM_PBMP_PORT_ADD(lc->pbm_hw, port);
        }
    }

    LS_UNLOCK(unit);

    if (lc->sema != NULL) {
        sal_sem_give(lc->sema);    /* register change now */
    }

    /* When no longer scanning a port, return it to the enabled state. */
    if (BCM_SUCCESS(rv) && !added) {
        int enable;

        rv = bcm_port_enable_get(unit, port, &enable);
        if (BCM_SUCCESS(rv)) {
            rv = bcm_port_update(unit, port, enable);
        }
    }

    return rv;
}

/*
 * Function:    
 *     bcm_linkscan_mode_set_pbm
 * Purpose:
 *     Set the current scanning mode for the specified ports.
 * Parameters:
 *     unit - Device unit number
 *     pbm  - Port bitmap indicating port to set
 *     mode - New scan mode for specified ports
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_mode_set_pbm(int unit, pbmp_t pbm, int mode)
{
    bcm_port_t  port;

    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_linkscan_mode_set(unit, port, mode));
    }

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_linkscan_mode_get
 * Purpose:
 *     Recover the current scanning mode for the specified port.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     mode - (OUT) current scan mode for specified port
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_mode_get(int unit, bcm_port_t port, int *mode)
{
    _ls_control_t  *lc;

    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    lc = LS_CONTROL(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if (PBMP_MEMBER(lc->pbm_hw, port)) {
        *mode = BCM_LINKSCAN_MODE_HW;
    } else if (PBMP_MEMBER(lc->pbm_sw, port)) {
        *mode = BCM_LINKSCAN_MODE_SW;
    } else {
        *mode = BCM_LINKSCAN_MODE_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_linkscan_update
 * Purpose:     
 *     Check for a change in link status on each link.  If a change
 *     is detected, call bcm_port_update to program the MACs for that
 *     link, and call the list of registered handlers.
 * Parameters:  
 *     unit - Device unit number
 *     pbm  - Bitmap of ports to scan
 * Returns:
 *     BCM_E_XXX
 */
int 
bcm_common_linkscan_update(int unit, bcm_pbmp_t pbm)
{                       
    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_PBMP_IS_NULL(pbm)) {
        return BCM_E_NONE;
    }

    _bcm_linkscan_update(unit, pbm, 0);

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_linkscan_register
 * Purpose:
 *     Register a handler to be called when a link status change is noticed.
 * Parameters:
 *     unit - Device unit number
 *     f    - Pointer to function to call when link status change is seen
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_register(int unit, bcm_linkscan_handler_t f)
{
    _ls_control_t     *lc;
    _ls_handler_cb_t  *lh;
    int               found = FALSE;

    UNIT_INIT_CHECK(unit);
    
    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (f == NULL){
        return BCM_E_PARAM;
    }
    
    LS_LOCK(unit);

    lc = LS_CONTROL(unit);

    /* First, see if this handler already registered */
    for (lh = lc->handler_cb; lh != NULL; lh = lh->next) {
        if (lh->cb_f == f) {
            found = TRUE;
            break;
        }
    }

    if (found) {
        LS_UNLOCK(unit);
        return BCM_E_NONE;
    }

    if ((lh = sal_alloc(sizeof(*lh), "bcm_linkscan_register")) == NULL) {
        LS_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    lh->next = lc->handler_cb;
    lh->cb_f = f;
    lc->handler_cb = lh;

    LS_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_linkscan_unregister
 * Purpose:
 *     Remove a previously registered handler from the callout list.
 * Parameters:
 *     unit - Device unit number
 *     f    - Pointer to function registered in call to 
 *            bcm_linkscan_register
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - Could not find matching handler
 */
int
bcm_common_linkscan_unregister(int unit, bcm_linkscan_handler_t f)
{
    _ls_control_t     *lc;
    _ls_handler_cb_t  *lh, *p;

    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    LS_LOCK(unit);

    lc = LS_CONTROL(unit);

    for (p = NULL, lh = lc->handler_cb; lh; p = lh, lh = lh->next) { 
        if (lh->cb_f == f) {
            if (p == NULL) {
                lc->handler_cb = lh->next;
            } else {
                p->next = lh->next;
            }
            break;
        }
    }

    LS_UNLOCK(unit);

    if (lh == NULL) {
        return BCM_E_NOT_FOUND;
    }        
    
    sal_free(lh);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_linkscan_port_register
 * Purpose:
 *     Register a handler to be called when a link status
 *     is to be determined by a caller provided function.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     f    - Pointer to function to call for true link status
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     This function works with software linkscan only.  
 */
int
bcm_common_linkscan_port_register(int unit, bcm_port_t port,
                                  bcm_linkscan_port_handler_t f)
{
    _ls_control_t  *lc;
  
    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LS_LOCK(unit);
    lc = LS_CONTROL(unit);
    lc->port_link_f[port] = f;
    LS_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_linkscan_port_unregister
 * Purpose:
 *     Remove a previously registered handler that is used
 *     for setting link status. 
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     f    - Pointer to function registered in call to 
 *            bcm_linkscan_port_register
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_linkscan_port_unregister(int unit, bcm_port_t port,
                                    bcm_linkscan_port_handler_t f)
{
    _ls_control_t  *lc;

    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    LS_LOCK(unit);
    lc = LS_CONTROL(unit);
    lc->port_link_f[port] = NULL;
    LS_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_link_wait
 * Purpose:
 *     Wait for all links in the mask to be "link up".
 * Parameters:  
 *     unit - Device unit number
 *     pbm  - (IN/OUT) Port bitmap to wait for, mask of those link up on 
 *            return
 *     us   - Number of microseconds to wait
 * Returns:
 *     BCM_E_NONE     - all links are ready.
 *     BCM_E_TIMEOUT  - not all links ready in specified time.
 *     BCM_E_DISABLED - linkscan not running on one or more of the ports.
 */
int
bcm_common_link_wait(int unit, pbmp_t *pbm, sal_usecs_t us)
{
    _ls_control_t   *lc;
    soc_timeout_t   to;
    pbmp_t          sofar_pbm;
    soc_port_t      port;

    UNIT_VALID_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    BCM_PBMP_ITER(*pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_linkscan_enable_port_get(unit, port));
    }

    /*
     * If a port was just configured, it may have gone down but
     * pbm_link may not reflect that until the next time linkscan
     * runs.  This is avoided by forcing an update of pbm_link.
     */
    _bcm_linkscan_update(unit, *pbm, 0);

    lc = LS_CONTROL(unit);

    soc_timeout_init(&to, us, 0);

    for (;;) {
        BCM_PBMP_ASSIGN(sofar_pbm, lc->pbm_link);
        BCM_PBMP_REMOVE(sofar_pbm, lc->pbm_remote_fault);
        BCM_PBMP_REMOVE(sofar_pbm, lc->pbm_local_fault);
        BCM_PBMP_AND(sofar_pbm, *pbm);
        if (BCM_PBMP_EQ(sofar_pbm, *pbm)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            BCM_PBMP_AND(*pbm, lc->pbm_link);
            SOC_PBMP_REMOVE(*pbm, lc->pbm_remote_fault);
            SOC_PBMP_REMOVE(*pbm, lc->pbm_local_fault);
            return BCM_E_TIMEOUT;
        }

        sal_usleep(lc->interval_us / 4);
    }

    return BCM_E_NONE;
}

/*
 * Function:    
 *     bcm_link_change
 * Purpose:
 *     Force a transient link down event to be recognized,
 *     regardless of the current physical up/down state of the
 *     port.  This does not affect the physical link status. 
 * Parameters:  
 *     unit - Device unit number
 *     pbm  - Bitmap of ports to operate on
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_common_link_change(int unit, pbmp_t pbm)
{
    _ls_control_t  *lc;

    UNIT_INIT_CHECK(unit);

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    LS_LOCK(unit);

    lc = LS_CONTROL(unit);
    BCM_PBMP_AND(pbm, PBMP_PORT_ALL(unit));
    BCM_PBMP_OR(lc->pbm_link_change, pbm);

    LS_UNLOCK(unit);

    /*
     * Wake up master thread to notice changes - required if using hardware
     * link scanning.
     */
    if (lc->sema != NULL) {
        sal_sem_give(lc->sema);
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_common_linkscan_trigger_event_set
 * Purpose:
 *     Set Link event to linkscan thread.
 *
 * Parameters:
 *     unit - Device unit number
 *     port - Port number, only -1 is acceptable.
 *     flags - flags
 *     trigger_event - Event type (BCM_LINKSCAN_TRIGGER_EVENT_XXX)
 *     enable - enable the event.
 * Returns:
 *     BCM_E_XXX
 */

int bcm_common_linkscan_trigger_event_set(
      int unit,
      bcm_port_t port,
      uint32 flags,
      bcm_linkscan_trigger_event_t trigger_event,
      int enable)
{
    int rv = BCM_E_NONE;
    _ls_control_t  *lc;

    UNIT_INIT_CHECK(unit);

    LS_LOCK(unit);

    lc = LS_CONTROL(unit);
    switch (trigger_event) {
        /* If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case
         * In the enum definition,
         * BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT
         */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
#ifdef BCM_JERICHO_SUPPORT
            if (soc_feature(unit, soc_feature_hw_linkscan_fault_support)) {
                rv = soc_jer_linkcan_fault_to_cmiclink_enable_set(unit, &lc->pbm_hw,
                                                                  1, lc->link_local_fault_chk_en);
                if (BCM_SUCCESS(rv)) {
                    lc->link_fault_chk_en = 1;
                }
            } else
#endif
            {
                if(BCM_PBMP_IS_NULL(lc->pbm_hw)) {
                    lc->link_fault_chk_en = 1;
                } else {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                             (BSL_META_U(unit,
                                      "HW link scan and link fault are not supported\n")));
                    rv = BCM_E_PARAM;
                }
            }
            break;
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
#ifdef BCM_JERICHO_SUPPORT
            if (soc_feature(unit, soc_feature_hw_linkscan_fault_support)) {
                rv = soc_jer_linkcan_fault_to_cmiclink_enable_set(unit, &lc->pbm_hw,
                                                                  lc->link_fault_chk_en, 1);
                if (BCM_SUCCESS(rv)) {
                    lc->link_local_fault_chk_en = 1;
                }
            } else
#endif
            {
                if(BCM_PBMP_IS_NULL(lc->pbm_hw)) {
                    lc->link_local_fault_chk_en = 1;
                } else {
                    LOG_ERROR(BSL_LS_BCM_LINK,
                             (BSL_META_U(unit,
                                      "HW link scan and link fault are not supported\n")));
                    rv = BCM_E_PARAM;
                }
            }
            break;
        default:
            rv = BCM_E_PARAM;
            break;
    }

    if (BCM_SUCCESS(rv) &&
        soc_feature(unit, soc_feature_hw_linkscan_fault_support) &&
        (lc->link_fault_chk_en == 1 || lc->link_local_fault_chk_en == 1)) {
        /* Prime the HW linkscan pump */
        if (SOC_PBMP_NOT_NULL(lc->pbm_hw)) {
            lc->hw_change = 1;
            _bcm_linkscan_hw_interrupt(unit);
        }
    }

    LS_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *     bcm_common_linkscan_trigger_event_get
 * Purpose:
 *     Get Link event status in linkscan thread.
 *
 * Parameters:
 *     unit - Device unit number
 *     port - Port number, only -1 is acceptable.
 *     flags - flags
 *     trigger_event - Event type (BCM_LINKSCAN_TRIGGER_EVENT_XXX)
 *     enable - event status.
 * Returns:
 *     BCM_E_XXX
 */

int bcm_common_linkscan_trigger_event_get(
      int unit,
      bcm_port_t port,
      uint32 flags,
      bcm_linkscan_trigger_event_t trigger_event,
      int *enable)
{
    _ls_control_t  *lc;

    UNIT_INIT_CHECK(unit);

    lc = LS_CONTROL(unit);
    switch (trigger_event) {
        /* If trigger_event is BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT, also reach this case
         * In the enum definition,
         * BCM_LINKSCAN_TRIGGER_EVENT_REMOTE_FAULT = BCM_LINKSCAN_TRIGGER_EVENT_FAULT
         */
        case BCM_LINKSCAN_TRIGGER_EVENT_FAULT:
            *enable = lc->link_fault_chk_en;
            break;
        case BCM_LINKSCAN_TRIGGER_EVENT_LOCAL_FAULT:
            *enable = lc->link_local_fault_chk_en;
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#if defined(BROADCOM_DEBUG)
int
bcm_common_linkscan_dump(int unit)
{
    _ls_handler_cb_t  *ent;

    /* indicates that the API does not require error recovery support */
    LINKSCAN_ERR_RECOVERY_NOT_NEEDED(unit);

    if (LS_CONTROL(unit) == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "BCM linkscan not initialized for unit %d\n"), unit));
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_LINK,
             (BSL_META_U(unit,
                         "BCM linkscan callbacks for unit %d\n"),
              unit));
    for (ent = LS_CONTROL(unit)->handler_cb; ent != NULL;
         ent = ent->next) {
#if !defined(__PEDANTIC__)
        LOG_INFO(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                             "    Fn %p\n"),
                  (void *)ent->cb_f));
#else /* !defined(__PEDANTIC__) */
        LOG_INFO(BSL_LS_BCM_LINK,
                 (BSL_META_U(unit,
                             "    Function pointer unprintable\n")));
#endif /* !defined(__PEDANTIC__) */
    }

    return BCM_E_NONE;
}
#endif  /* BROADCOM_DEBUG */

#if defined(BCM_CMICX_SUPPORT) || defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT)
/*
 * Function:
 *     cmicx_common_linkscan_hw_interrupt
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
cmicx_common_linkscan_hw_interrupt(int unit)
{
    _ls_control_t  *lc = LS_CONTROL(unit);

    if ((NULL != lc) && (NULL != lc->sema)) {
        lc->hw_change = 1;
        sal_sem_give(lc->sema);
    }
    LOG_VERBOSE(BSL_LS_BCM_LINK,
                (BSL_META_U(unit,
                            "Linkscan interrupt unit %d\n"), unit));
}
#endif /* BCM_CMICX_SUPPORT || BCM_DNXF_SUPPORT */
