/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    macsec_cmn.c
 * Purpose: MACSEC software module intergation support
 */

#ifdef INCLUDE_MACSEC

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
#include <soc/macsecphy.h>
#include <sal/core/libc.h>
#include <bcm/debug.h>

#include <bcm_int/common/macsec_cmn.h>

#include <bcm_int/control.h>
#include <bcm_int/esw_dispatch.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}

static bmacsec_error_t error_tbl[] = {
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
 * Convert MACSEC return codes to BCM return code.
 */
#define BCM_ERROR(e)    error_tbl[-(e)]

typedef struct _bcm_macsec_cb_args_s {
    void *user_arg;
    bcm_macsec_port_traverse_cb user_cb_port;
    bcm_macsec_chan_traverse_cb user_cb_chan;
    bcm_macsec_secure_assoc_traverse_cb user_cb_assoc;
    bcm_macsec_flow_traverse_cb     user_cb_flow;
    int   unit;
    int   port;
} _bcm_macsec_cb_args_t;

static int macsec_cb_registered = 0;

/*
 * Function:
 *      bcm_common_macsec_init
 * Purpose:
 *      Intiialize MACSEC for the specified unit.
 */
int _bcm_common_macsec_init(int unit)
{
    static int _bmacsec_init_done = 0;

    /*
     * Do MACSEC init and MDIO registration for the MACSEC module.
     */
    if (!_bmacsec_init_done) {
        /* Initilaize the MACSEC module */
        bmacsec_init();
        /* register MDIO routines */
        bmacsec_dev_mmi_mdio_register(soc_macsecphy_miim_read, 
                                      soc_macsecphy_miim_write);
        /* Initialize the default print vectors. */
        bmacsec_config_print_set(BMACSEC_PRINT_PRINTF, bsl_printf);
        _bmacsec_init_done = 1;
    }
    return BCM_E_NONE;
}

/*
 * Local port traverse handler to filter out ports belonging to specified
 * unit.
 */
STATIC int 
_bcm_macsec_port_traverse_cb(bmacsec_port_t p, 
                             bmacsec_core_t dev_core, 
                             bmacsec_dev_addr_t dev_addr, 
                             int dev_port, 
                             bmacsec_dev_io_f devio_f, 
                             void *user_data)
{
    int rv = BMACSEC_E_NONE ;
    _bcm_macsec_cb_args_t *ua;
    bcm_macsec_port_traverse_cb callback;

    ua = (_bcm_macsec_cb_args_t*) user_data;

    if (SOC_MACSEC_PORTID2UNIT(p) == ua->unit) {
        callback = ua->user_cb_port;
        rv = callback(SOC_MACSEC_PORTID2UNIT(p), 
                 SOC_MACSEC_PORTID2PORT(p), 
                 dev_core, dev_addr, 
                 dev_port, devio_f, ua->user_arg);
    }
    return rv;
}

/*
 * Function:
 *      bcm_macsec_port_traverse
 * Purpose:
 *      Destroy a MACSEC Port
 */
int 
bcm_common_macsec_port_traverse(int unit, 
                             bcm_macsec_port_traverse_cb callback, 
                             void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_macsec_cb_args_t ua;

    UNIT_VALID(unit);

    ua.user_arg = user_data;
    ua.user_cb_port = callback;
    ua.unit = unit;

    rv = bmacsec_port_traverse(_bcm_macsec_port_traverse_cb, (void*) &ua);

    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_macsec_port_config_set
 * Purpose:
 *      Set the MACSEC Port configuration.
 */
int
bcm_common_macsec_port_config_set(int unit, bcm_port_t port, 
                               bcm_macsec_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_port_config_set(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_macsec_port_config_get
 * Purpose:
 *      Get the current MACSEC Port configuration.
 */
int
bcm_common_macsec_port_config_get(int unit, bcm_port_t port, 
                               bcm_macsec_port_config_t *cfg)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_port_config_get(p, cfg);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_macsec_port_capability_get
 * Purpose:
 *      Get the MACSEC Port capability
 */
int
bcm_common_macsec_port_capability_get(int unit, bcm_port_t port, 
                               bcm_macsec_port_capability_t *cap)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_port_capability_get(p, cap);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_secure_chan_create
 * Purpose:
 *      Create MACSEC secure channel
 */
int 
bcm_common_macsec_secure_chan_create(int unit, bcm_port_t port, 
                                  uint32 flags,
                                  bcm_macsec_secure_chan_t *chan, 
                                  int *chanId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_chan_create(p, flags, chan, chanId);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_secure_chan_get
 * Purpose:
 *      Return current MACSEC secure channel corresponding to channel-id
 */
int 
bcm_common_macsec_secure_chan_get(int unit, bcm_port_t port, int chanId,
                               bcm_macsec_secure_chan_t *chan)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_chan_get(p, chanId, chan);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_macsec_secure_chan_destroy
 * Purpose:
 *      Destroy a secure channel. This function will free any resources
 *      associated with the secure channel, including any secure assocs.
 */
int 
bcm_common_macsec_secure_chan_destroy(int unit, bcm_port_t port, int chanId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_chan_destroy(p, chanId);
    return BCM_ERROR(rv);
}


/*
 * Local secure channel traver handler to call user provided callback
 * function for only the secure channels belonging to specified port.
 */
STATIC int _bcm_macsec_secure_chan_cb(bmacsec_port_t p,
                                      bmacsec_secure_chan_t *chan, 
                                      int chanId, 
                                      void *user_data)
{
    int rv = BMACSEC_E_NONE ;
    _bcm_macsec_cb_args_t *ua;
    bcm_macsec_chan_traverse_cb callback;

    ua = (_bcm_macsec_cb_args_t*) user_data;

    if ((SOC_MACSEC_PORTID2UNIT(p) == ua->unit) && 
        (SOC_MACSEC_PORTID2PORT(p) == ua->port)) {
        callback = ua->user_cb_chan;
        rv = callback(SOC_MACSEC_PORTID2UNIT(p), 
                 SOC_MACSEC_PORTID2PORT(p), 
                 chan, chanId, ua->user_arg);
    }
    return rv;
}

/*
 * Function:
 *      macsec_secure_chan_traverse
 * Purpose:
 *      This function will traverse over all the secure channels that
 *      are associated with the specified macsec port and call the
 *      user provided callback function for each of the secure channel.
 */
int 
bcm_common_macsec_secure_chan_traverse(int unit, bcm_port_t port, 
                                    bcm_macsec_chan_traverse_cb callback,
                                    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;
    _bcm_macsec_cb_args_t ua;

    UNIT_VALID(unit);

    ua.user_arg = user_data;
    ua.user_cb_chan = callback;
    ua.unit = unit;
    ua.port = port;

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_chan_traverse(p, _bcm_macsec_secure_chan_cb,
                                                            (void*) &ua);
    return BCM_ERROR(rv);
}


/*
 * Function:
 *      macsec_secure_assoc_create
 * Purpose:
 *      This function will create an secure association bound to a
 *      secure channel. 
 */
int
bcm_common_macsec_secure_assoc_create(int unit, bcm_port_t port, 
                                   uint32 flags, int chanId, 
                                   bcm_macsec_secure_assoc_t *assoc, 
                                   int *assocId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_assoc_create(p, flags, chanId, assoc, assocId);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_secure_assoc_get
 * Purpose:
 *      This function returns the secure assoc for the specified
 *      assoc-id.
 */
int
bcm_common_macsec_secure_assoc_get(int unit, bcm_port_t port, int assocId, 
                        bcm_macsec_secure_assoc_t *assoc, int *chanId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_assoc_get(p, assocId, assoc, chanId);
    return BCM_ERROR(rv);

}

/*
 * Function:
 *      macsec_secure_assoc_destroy
 * Purpose:
 *      This function destroys the secure assoc correspondig to
 *      assoc-id.
 */
int 
bcm_common_macsec_secure_assoc_destroy(int unit, bcm_port_t port, int assocId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_assoc_destroy(p, assocId);
    return BCM_ERROR(rv);
}

/*
 * Local secure assoc traver handler to filter assocs and call the user provided
 * callback function for only the assocs belonging to spcified port.
 */
STATIC int _bcm_macsec_secure_assoc_cb(bmacsec_port_t p,
                                       bcm_macsec_secure_assoc_t *chan, 
                                       int chanId, 
                                       int assocId,
                                       void *user_data)
{
    int rv = BMACSEC_E_NONE ;
    _bcm_macsec_cb_args_t *ua;
    bcm_macsec_secure_assoc_traverse_cb callback;

    ua = (_bcm_macsec_cb_args_t*) user_data;

    if ((SOC_MACSEC_PORTID2UNIT(p) == ua->unit) && 
        (SOC_MACSEC_PORTID2PORT(p) == ua->port)) {
        callback = ua->user_cb_assoc;
        rv = callback(SOC_MACSEC_PORTID2UNIT(p), 
                 SOC_MACSEC_PORTID2PORT(p), 
                 chan, chanId, assocId, (void*) ua->user_arg);
    }
    return rv;
}

/*
 * Function:
 *      macsec_secure_assoc_traverse
 * Purpose:
 *      Traverse all the secure assoc for the specified secure channel
 *      and calls user provided callabck function with information 
 *      about the assoc in macsec_secure_assoc_t structure and its
 *      corresponding assoc-id.
 */
int 
bcm_common_macsec_secure_assoc_traverse(int unit, bcm_port_t port, 
                                int chanId, 
                                bcm_macsec_secure_assoc_traverse_cb callback,
                                void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;
    _bcm_macsec_cb_args_t ua;

    UNIT_VALID(unit);

    ua.user_arg = user_data;
    ua.user_cb_assoc = callback;
    ua.unit = unit;
    ua.port = port;

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_secure_assoc_traverse(p, chanId,
                                _bcm_macsec_secure_assoc_cb, (void*) &ua);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_flow_create
 * Purpose:
 *      This fucntion creates/updates a flow and assigns the 
 *      actions for the flow.
 */
int
bcm_common_macsec_flow_create(int unit, bcm_port_t port, 
                           uint32 flags, bcm_macsec_flow_match_t *flow, 
                           bcm_macsec_flow_action_t *action, int *flowId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_flow_create(p, flags, flow, action, flowId);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_flow_get
 * Purpose:
 *      Return flow and corresponding action for the specified flowId
 */
int
bcm_common_macsec_flow_get(int unit, bcm_port_t port, int flowId, 
                bcm_macsec_flow_match_t *flow, bcm_macsec_flow_action_t *action)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_flow_get(p, flowId, flow, action);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      macsec_flow_destroy
 * Purpose:
 *      Destroy the flow corresponding to flowId.
 */
int 
bcm_common_macsec_flow_destroy(int unit, bcm_port_t port, int flowId)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_flow_destroy(p, flowId);
    return BCM_ERROR(rv);
}

/*
 * Local flow traver handler to filter the flows belonging to specified port.
 */
STATIC int _bcm_macsec_flow_cb(bmacsec_port_t p,
                               bcm_macsec_flow_match_t *flow, 
                               bcm_macsec_flow_action_t *action, 
                               int flowId, 
                               void *user_data)
{
    int rv = BMACSEC_E_NONE ;
    _bcm_macsec_cb_args_t *ua;
    bcm_macsec_flow_traverse_cb callback;

    ua = (_bcm_macsec_cb_args_t*) user_data;

    if ((SOC_MACSEC_PORTID2UNIT(p) == ua->unit) && 
        (SOC_MACSEC_PORTID2PORT(p) == ua->port)) {
        callback = ua->user_cb_flow;
        rv = callback(SOC_MACSEC_PORTID2UNIT(p), 
                 SOC_MACSEC_PORTID2PORT(p), 
                 flow, action, flowId, ua->user_arg);
    }
    return rv;
}

/*
 * Function:
 *      macsec_flow_traverse
 * Purpose:
 *      Traverse all the flows and call user provided callback
 *      for each flow.
 */
int 
bcm_common_macsec_flow_traverse(int unit, bcm_port_t port, 
                             bcm_macsec_flow_traverse_cb callbk,
                             void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;
    _bcm_macsec_cb_args_t ua;

    UNIT_VALID(unit);

    ua.user_arg = user_data;
    ua.user_cb_flow = callbk;
    ua.unit = unit;
    ua.port = port;

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_flow_traverse(p, _bcm_macsec_flow_cb, (void*) &ua);
    return BCM_ERROR(rv);
}

/*
 * Function:
 *      bcm_macsec_stat_clear
 * Purpose:
 *      Clear stats for the specified port.
 */
int 
bcm_common_macsec_stat_clear(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_stat_clear(p);
    return BCM_ERROR(rv);
}

int 
bcm_common_macsec_stat_get(int unit, bcm_port_t port, bcm_macsec_stat_t stat, 
                    int chanId, int assocId, uint64 *val)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_stat_get(p, stat, chanId, assocId, (buint64_t *)val);
    return BCM_ERROR(rv);
}

int 
bcm_common_macsec_stat_get32(int unit, bcm_port_t port, 
                          bcm_macsec_stat_t stat, int chanId, 
                          int assocId, uint32 *val)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_stat_get32(p, stat, chanId, assocId, (buint32_t*)val);
    return BCM_ERROR(rv);
}

int 
bcm_common_macsec_stat_set(int unit, bcm_port_t port, bcm_macsec_stat_t stat, 
                    int chanId, int assocId, uint64 val)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;
    buint64_t bval;
    BMACSEC_COMPILER_64_SET(bval, COMPILER_64_HI(val), COMPILER_64_LO(val));

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_stat_set(p, stat, chanId, assocId, bval);
    return BCM_ERROR(rv);
}

int 
bcm_common_macsec_stat_set32(int unit, bcm_port_t port, bcm_macsec_stat_t stat, 
                        int chanId, int assocId, uint32 val)
{
    int rv = BCM_E_UNAVAIL;
    bmacsec_port_t p;

    UNIT_VALID(unit);

    p = SOC_MACSEC_PORTID(unit, port);
    rv = bmacsec_stat_set32(p, stat, chanId, assocId, val);
    return BCM_ERROR(rv);
}

typedef struct _bcm_macsec_event_data_s {
    bcm_macsec_event_cb callback;
    uint32              event_map;
} _bcm_macsec_event_data_t;

STATIC _bcm_macsec_event_data_t _macsec_event_data[BCM_UNITS_MAX];

STATIC int
_bcm_macsec_event_cb(bmacsec_port_t p,      /* Port ID          */
                     bmacsec_event_t event, /* Event            */
                     int chanId,           /* secure channel ID*/
                     int assocId,
                     void *user_data)
{
    int     unit;
    bcm_macsec_event_cb callback;
    _bcm_macsec_event_data_t *event_data;

    unit = SOC_MACSEC_PORTID2UNIT(p);

    event_data = &_macsec_event_data[unit];
    callback = (bcm_macsec_event_cb) event_data->callback;

    if (callback && (event_data->event_map & (1 << (uint32) event))) {
        callback(SOC_MACSEC_PORTID2UNIT(p), 
                 SOC_MACSEC_PORTID2PORT(p), 
                 event, chanId, assocId, user_data);
    }
    return BMACSEC_E_NONE;
}

int 
bcm_common_macsec_event_register(int unit, bcm_macsec_event_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    _bcm_macsec_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_macsec_event_data[unit];
    event_data->callback = cb;
    event_data->event_map = 0;
    if (macsec_cb_registered == 0) {
        rv = bmacsec_event_register(_bcm_macsec_event_cb, user_data);
        if (rv == BMACSEC_E_NONE) {
            macsec_cb_registered = 1;
        }
    }
    return BCM_ERROR(rv);
}

int 
bcm_common_macsec_event_unregister(int unit, bcm_macsec_event_cb cb)
{
    int rv = BCM_E_NONE, unregister = 1, ii;
    _bcm_macsec_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_macsec_event_data[unit];
    if (event_data->callback) {
        event_data->callback = NULL;
    }

    for (ii = 0; ii < BCM_UNITS_MAX; ii++) {
        event_data = &_macsec_event_data[ii];
        if (event_data->callback) {
            unregister = 0;
            break;
        }
    }
    if (unregister) {
        macsec_cb_registered = 0;
        rv = bmacsec_event_unregister(_bcm_macsec_event_cb);
    }
    return BCM_ERROR(rv);
}

int
bcm_common_macsec_event_enable_set(int unit,
                                bcm_macsec_event_t event, int enable)
{
    int rv = BCM_E_NONE, disable, ii;
    _bcm_macsec_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_macsec_event_data[unit];
    if (enable) {
        event_data->event_map |= (1 << (uint32) event);
        rv = bmacsec_event_enable_set(event, enable);
    } else {
        event_data->event_map &= ~(1 << (uint32) event);
        disable = 1;
        for (ii = 0; ii < BCM_UNITS_MAX; ii++) {
            event_data = &_macsec_event_data[ii];
            if (event_data->event_map & (1 << (uint32) event)) {
                disable = 0;
                break;
            }
        }
        if (disable) {
            rv = bmacsec_event_enable_set(event, enable);
        }
    }
    return BCM_ERROR(rv);
}

int
bcm_common_macsec_event_enable_get(int unit,
                                bcm_macsec_event_t event, int *enable)
{
    int rv = BCM_E_NONE;
    _bcm_macsec_event_data_t *event_data;

    UNIT_VALID(unit);

    event_data = &_macsec_event_data[unit];
    *enable = (event_data->event_map & (1 << (uint32) event)) ? 1 : 0;
    return BCM_ERROR(rv);
}


/*
 * Configure MACSEC print function for debug.
 */
int bcm_common_macsec_config_print(uint32 level)
{
    int rv = BCM_E_NONE;
    uint32  flag = BMACSEC_PRINT_PRINTF;

#if defined(BROADCOM_DEBUG)
    if (LOG_CHECK(BSL_LS_BCM_MACSEC | BSL_INFO)) {
        flag |= BMACSEC_PRINT_DEBUG;
    }
#endif /* BROADCOM_DEBUG */

    rv = bmacsec_config_print_set(flag, bsl_printf);
    return BCM_ERROR(rv);
}


#else
typedef int _macsec_cmn_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_MACSEC */

