/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    fcmap_cmn.c
 * Purpose: FCMAP software module intergation support
 */

#ifdef INCLUDE_FCMAP

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/phy.h>
#include <soc/ll.h>
#include <soc/ptable.h>
#include <soc/tucana.h>
#include <soc/firebolt.h>
#include <soc/xaui.h>
#include <soc/phyctrl.h>
#include <soc/phyreg.h>
#include <soc/fcmapphy.h>
#include <bfcmap.h>
#include <sal/core/libc.h>
#include <bcm/debug.h>

#include <bcm_int/common/fcmap_cmn.h>

#include <bcm_int/control.h>
#include <bcm_int/esw_dispatch.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}

static bfcmap_error_t error_tbl[] = {
                                BCM_E_NONE,
                                BCM_E_INTERNAL,
                                BCM_E_MEMORY,
                                BCM_E_PARAM,
                                BCM_E_EMPTY,
                                BCM_E_FULL,
                                BCM_E_NOT_FOUND,
                                BCM_E_EXISTS,
                                BCM_E_TIMEOUT,
                                BCM_E_FAIL,
                                BCM_E_DISABLED,
                                BCM_E_BADID,
                                BCM_E_RESOURCE,
                                BCM_E_CONFIG,
                                BCM_E_UNAVAIL,
                                BCM_E_INIT,
                                BCM_E_PORT,
                            };
/*
 * Convert FCMAP return codes to BCM return code.
 */
#define BCM_ERROR(e)    error_tbl[-(e)]

typedef struct _bcm_fcmap_cb_args_s {
    void *user_arg;
    void *user_cb;
    int   unit;
    int   port;
} _bcm_fcmap_cb_args_t;

static int fcmap_cb_registered = 0;

/*
 * Function:
 *      bcm_common_fcmap_init
 * Purpose:
 *      Intiialize FCMAP for the specified unit.
 */
int _bcm_common_fcmap_init(int unit)
{
    static int _bfcmap_init_done = 0;

    /*
     * Do FCMAP init and MDIO registration for the FCMAP module.
     */
    if (!_bfcmap_init_done) {
        /* Initilaize the FCMAP module */
        bfcmap_init();
        /* register MDIO routines */
        bfcmap_dev_mmi_mdio_register(soc_fcmapphy_miim_read, 
                                      soc_fcmapphy_miim_write);
        /* Initialize the default print vectors. */
        
        bfcmap_config_print_set(BFCMAP_PRINT_PRINTF | BFCMAP_PRINT_DEBUG, bsl_printf);
        _bfcmap_init_done = 1;
    }
    return BCM_E_NONE;
}

/*
 * Local port traverse handler to filter out ports belonging to specified
 * unit.
 */
STATIC int 
_bcm_fcmap_port_traverse_cb(bfcmap_port_t p, 
                             bfcmap_core_t dev_core, 
                             bfcmap_dev_addr_t dev_addr, 
                             int dev_port, 
                             bfcmap_dev_io_f devio_f, 
                             void *user_data)
{
    int rv = BFCMAP_E_NONE ;
    _bcm_fcmap_cb_args_t *ua;
    bcm_fcmap_port_traverse_cb callback;

    ua = (_bcm_fcmap_cb_args_t*) user_data;

    if (SOC_FCMAP_PORTID2UNIT(p) == ua->unit) {
        callback = (bcm_fcmap_port_traverse_cb) ua->user_cb;
        rv = callback(SOC_FCMAP_PORTID2UNIT(p), 
                 SOC_FCMAP_PORTID2PORT(p), 
                 dev_core, dev_addr, 
                 dev_port, devio_f, ua->user_arg);
    }
    return rv;
}

/*
 * Function:
 *      bcm_fcmap_port_traverse
 * Purpose:
 *      Destroy a FCMAP Port
 */
int 
bcm_common_fcmap_port_traverse(int unit, 
                             bcm_fcmap_port_traverse_cb callback, 
                             void *user_data)
{
    int rv;
    _bcm_fcmap_cb_args_t ua;

    UNIT_VALID(unit);

    ua.user_arg = user_data;
    ua.user_cb = (void*) callback;
    ua.unit = unit;

    rv = bfcmap_port_traverse(_bcm_fcmap_port_traverse_cb, (void*) &ua);
#ifdef BCM_CB_ABORT_ON_ERR
    return BCM_ERROR(rv);
#else
    return rv;
#endif
}


/*
 * Function:
 *      bcm_fcmap_port_config_selective_set
 * Purpose:
 *      Set the FCMAP Port configuration.
 */
int
bcm_common_fcmap_port_config_selective_set(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_config_selective_set(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_config_selective_get
 * Purpose:
 *      Get the current FCMAP Port configuration.
 */
int
bcm_common_fcmap_port_config_selective_get(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_config_selective_get(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_config_set
 * Purpose:
 *      Set the FCMAP Port configuration.
 */
int
bcm_common_fcmap_port_config_set(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_config_set(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_config_get
 * Purpose:
 *      Get the current FCMAP Port configuration.
 */
int
bcm_common_fcmap_port_config_get(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_config_get(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_speed_set
 * Purpose:
 *      Set speed attribute of the FC port.
 */
int
bcm_common_fcmap_port_speed_set(int unit, bcm_port_t port, 
                               bcm_fcmap_port_speed_t speed)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_speed_set(p, speed);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_enable
 * Purpose:
 *      Enable the FC port
 */
int
bcm_common_fcmap_port_enable(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_link_enable(p);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_shutdown
 * Purpose:
 *      Disable FC port
 */
int
bcm_common_fcmap_port_shutdown(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_shutdown(p);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_port_link_reset
 * Purpose:
 *      Issue a link reset on the FC port.
 */
int
bcm_common_fcmap_port_link_reset(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_reset(p);
    return BCM_ERROR(rv);
}
/*
 * Function:
 *      bcm_fcmap_vlan_map_add
 * Purpose:
 *      Add entry to VLAN - VSAN MAP for FCMAP port
 */
int
bcm_common_fcmap_vlan_map_add(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_vlan_map_add(p, vlan);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_vlan_map_get
 * Purpose:
 *      Get entry from VLAN - VSAN MAP for FCMAP port
 */
int
bcm_common_fcmap_vlan_map_get(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_vlan_map_get(p, vlan);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_fcmap_vlan_map_delete
 * Purpose:
 *      Delete entry from VLAN - VSAN MAP for FCMAP port
 */
int
bcm_common_fcmap_vlan_map_delete(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_vlan_map_delete(p, vlan);
    return BCM_ERROR(rv);
}

#if 0
/*
 * Function:
 *      bcm_fcmap_port_capability_get
 * Purpose:
 *      Get the FCMAP Port capability
 */
int
bcm_common_fcmap_port_capability_get(int unit, bcm_port_t port, 
                               bcm_fcmap_port_capability_t *cap)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_capability_get(p, cap);
    return BCM_ERROR(rv);
}
#endif


/*
 * Function:
 *      bcm_fcmap_stat_clear
 * Purpose:
 *      Clear stats for the specified port.
 */
int 
bcm_common_fcmap_stat_clear(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_stat_clear(p);
    return BCM_ERROR(rv);
}

int 
bcm_common_fcmap_stat_get(int unit, bcm_port_t port, bcm_fcmap_stat_t stat, 
                    uint64 *val)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_stat_get(p, stat, (buint64_t *)val);
    return BCM_ERROR(rv);
}

int 
bcm_common_fcmap_stat_get32(int unit, bcm_port_t port, 
                          bcm_fcmap_stat_t stat, uint32 *val)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_stat_get32(p, stat, (buint32_t*)val);
    return BCM_ERROR(rv);
}


typedef struct _bcm_fcmap_event_data_s {
    bcm_fcmap_event_cb callback;
    uint32              event_map;
} _bcm_fcmap_event_data_t;

STATIC _bcm_fcmap_event_data_t _fcmap_event_data[BCM_UNITS_MAX];

STATIC int
_bcm_fcmap_event_cb(bfcmap_port_t p,      /* Port ID          */
                     bfcmap_event_t event, /* Event            */
                     void *user_data)
{
    int     unit;
    bcm_fcmap_event_cb callback;
    _bcm_fcmap_event_data_t *event_data;

    unit = SOC_FCMAP_PORTID2UNIT(p);

    event_data = &_fcmap_event_data[unit];
    callback = (bcm_fcmap_event_cb) event_data->callback;

    if (callback && (event_data->event_map & (1 << (uint32) event))) {
        callback(SOC_FCMAP_PORTID2UNIT(p), 
                 SOC_FCMAP_PORTID2PORT(p), 
                 event, user_data);
    }
    return BFCMAP_E_NONE;
}

int 
bcm_common_fcmap_event_register(int unit, bcm_fcmap_event_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    _bcm_fcmap_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_fcmap_event_data[unit];
    event_data->callback = cb;
    event_data->event_map = 0;
    if (fcmap_cb_registered == 0) {
        rv = bfcmap_event_register(_bcm_fcmap_event_cb, user_data);
        if (rv == BFCMAP_E_NONE) {
            fcmap_cb_registered = 1;
        }
    }
    return BCM_ERROR(rv);
}

int 
bcm_common_fcmap_event_unregister(int unit, bcm_fcmap_event_cb cb)
{
    int rv = BCM_E_NONE, unregister = 1, ii;
    _bcm_fcmap_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_fcmap_event_data[unit];
    if (event_data->callback) {
        event_data->callback = NULL;
    }

    for (ii = 0; ii < BCM_UNITS_MAX; ii++) {
        event_data = &_fcmap_event_data[ii];
        if (event_data->callback) {
            unregister = 0;
            break;
        }
    }
    if (unregister) {
        fcmap_cb_registered = 0;
        rv = bfcmap_event_unregister(_bcm_fcmap_event_cb);
    }
    return BCM_ERROR(rv);
}

int
bcm_common_fcmap_event_enable_set(int unit,
                                bcm_fcmap_event_t event, int enable)
{
    int rv = BCM_E_NONE, disable, ii;
    _bcm_fcmap_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_fcmap_event_data[unit];
    if (enable) {
        event_data->event_map |= (1 << (uint32) event);
        rv = bfcmap_event_enable_set(event, enable);
    } else {
        event_data->event_map &= ~(1 << (uint32) event);
        disable = 1;
        for (ii = 0; ii < BCM_UNITS_MAX; ii++) {
            event_data = &_fcmap_event_data[ii];
            if (event_data->event_map & (1 << (uint32) event)) {
                disable = 0;
                break;
            }
        }
        if (disable) {
            rv = bfcmap_event_enable_set(event, enable);
        }
    }
    return BCM_ERROR(rv);
}

int
bcm_common_fcmap_event_enable_get(int unit,
                                bcm_fcmap_event_t event, int *enable)
{
    int rv = BCM_E_NONE;
    _bcm_fcmap_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_fcmap_event_data[unit];
    *enable = (event_data->event_map & (1 << (uint32) event)) ? 1 : 0;
    return BCM_ERROR(rv);
}


/*
 * Configure FCMAP print function for debug.
 */
int bcm_common_fcmap_config_print(uint32 level)
{
    int rv = BCM_E_NONE;
    uint32  flag = BFCMAP_PRINT_PRINTF;

#if defined(BROADCOM_DEBUG)
    if (level & BSL_S_FCOE) {
        flag |= BFCMAP_PRINT_DEBUG;
    }
#endif /* BROADCOM_DEBUG */

    rv = bfcmap_config_print_set(flag, bsl_printf);
    return BCM_ERROR(rv);
}


/*
 * Function:
 *      bcm_common_fcmap_linkfault_trigger_rc_get
 * Purpose:
 *      Get the current FCMAP Port linkfault trigger and reason code.
 */
int
bcm_common_fcmap_linkfault_trigger_rc_get(int unit, bcm_port_t port, 
                               bcm_fcmap_lf_tr_t *lf_trigger, bcm_fcmap_lf_rc_t *lf_rc)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_linkfault_trigger_rc_get(p, (bfcmap_lf_tr_t *)lf_trigger, (bfcmap_lf_rc_t *)lf_rc);
    return BCM_ERROR(rv);
}



/*
 * Function:
 *      bcm_common_fcmap_diag_get
 * Purpose:
 *      Get the current FCMAP Port diagnostic code.
 */
int
bcm_common_fcmap_diag_get(int unit, bcm_port_t port, 
                               bcm_fcmap_diag_code_t *diag)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_diag_get(p, (bfcmap_diag_code_t *)diag);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_common_fcmap_port_ability_advert_set
 * Purpose:
 *      Sets discrete port speeds for AN or single forced port speed
 */
int
bcm_common_fcmap_port_ability_advert_set(int unit, bcm_port_t port, 
                                         bcm_fcmap_port_ability_t *ability_mask)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_ability_advert_set(p, (bfcmap_port_ability_t *)ability_mask);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_common_fcmap_port_ability_advert_get
 * Purpose:
 *      Retrieves supported port speeds
 */
int
bcm_common_fcmap_port_ability_advert_get(int unit, bcm_port_t port, 
                                         bcm_fcmap_port_ability_t *ability_mask)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_ability_advert_get(p, (bfcmap_port_ability_t *)ability_mask);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_common_fcmap_private_data_set
 * Purpose:
 *      Pass private data to driver
 */
int
bcm_common_fcmap_private_data_set(int unit, bcm_port_t port, uint8 *data, int len)
{
    int rv = BCM_E_UNAVAIL;
    bfcmap_port_t p;

    UNIT_VALID(unit);

    p = SOC_FCMAP_PORTID(unit, port);
    rv = bfcmap_port_private_data_set(p, data, len);
    return BCM_ERROR(rv);
}

#else
typedef int _fcmap_cmn_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_FCMAP */

