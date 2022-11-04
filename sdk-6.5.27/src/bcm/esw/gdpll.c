/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:       gdpll.c
 *
 * Remarks:    Broadcom StrataSwitch Time GDPLL API
 *
 * Functions:
 *   Public Interface functions
 *      bcm_esw_gdpll_chan_create
 *      bcm_esw_gdpll_chan_destroy
 *      bcm_esw_gdpll_chan_enable
 *      bcm_esw_gdpll_chan_state_get
 *      bcm_esw_gdpll_chan_debug_enable
 *      bcm_esw_gdpll_debug_cb_register
 *      bcm_esw_gdpll_flush
 *      bcm_esw_gdpll_cb_register
 *      bcm_esw_gdpll_cb_unregister
 *      bcm_esw_gdpll_debug
 *      bcm_esw_gdpll_offset_get
 *      bcm_esw_gdpll_offset_set
 *      bcm_esw_gdpll_chan_get
 *      bcm_esw_gdpll_input_event_poll
 */

#if defined(INCLUDE_GDPLL)
#include <shared/util.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/iproc.h>

#include <bcm/time.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/gdpll.h>
#include <bcm_int/gdpll.h>
#include <bcm_int/esw/switch.h>

#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm/module.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION  BCM_WB_VERSION_1_0
#endif  /* BCM_WARM_BOOT_SUPPORT */

extern gdpll_context_t *pGdpllCtx[BCM_MAX_NUM_UNITS];

int bcm_esw_gdpll_init(int unit)
{
    return bcm_common_gdpll_init(unit);
}

int bcm_esw_gdpll_deinit(int unit)
{
    return bcm_common_gdpll_deinit(unit);
}

int bcm_esw_gdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan)
{
    return bcm_common_gdpll_chan_create(unit, flags, pGdpllChan, pChan);
}

int bcm_esw_gdpll_chan_destroy(int unit, int chan)
{
    return bcm_common_gdpll_chan_destroy(unit, chan);
}

int bcm_esw_gdpll_chan_enable(int unit, int chan, int enable)
{
    return bcm_common_gdpll_chan_enable(unit, chan, enable);
}

int bcm_esw_gdpll_chan_state_get(int unit, int chan, uint32 *dpllState)
{
    return bcm_common_gdpll_chan_state_get(unit, chan, dpllState);
}

int bcm_esw_gdpll_chan_debug_enable(int unit, int chan, bcm_gdpll_debug_mode_t debug_mode, int enable)
{
    return bcm_common_gdpll_chan_debug_enable(unit, chan, debug_mode, enable);
}

int bcm_esw_gdpll_debug_cb_register(int unit, bcm_gdpll_debug_cb cb, void *user_data)
{
    return bcm_common_gdpll_debug_cb_register(unit, cb, user_data);
}

int bcm_esw_gdpll_flush (int unit)
{
    return bcm_common_gdpll_flush(unit);
}

int bcm_esw_gdpll_cb_register(int unit, bcm_gdpll_cb_reg_t cb_type, bcm_gdpll_cb cb, void *user_data)
{
    return bcm_common_gdpll_cb_register(unit, cb_type, cb, user_data);
}

int bcm_esw_gdpll_cb_unregister(int unit, bcm_gdpll_cb_reg_t cb_type)
{
    return bcm_common_gdpll_cb_unregister(unit, cb_type);
}

int bcm_esw_gdpll_debug (int unit, int enable)
{
    return bcm_common_gdpll_debug(unit, enable);
}

int bcm_esw_gdpll_offset_get(int unit, int chan, uint64 *pOffset)
{
    return bcm_common_gdpll_offset_get(unit, chan, pOffset);
}

int bcm_esw_gdpll_offset_set(int unit, int chan, uint64 Offset)
{
    return bcm_common_gdpll_offset_set(unit, chan, Offset);
}

int bcm_esw_gdpll_chan_get(int unit, int chan, bcm_gdpll_chan_t *gdpll_chan)
{
    return bcm_common_gdpll_chan_get(unit, chan, gdpll_chan);
}

int bcm_esw_gdpll_input_event_poll(int unit, bcm_gdpll_input_event_t input_event, uint32 timeout)
{
    return bcm_common_gdpll_input_event_poll(unit, input_event, timeout);
}

/*
 * Function:
 *    _bcm_gdpll_scache_alloc
 * Purpose:
 *    Internal routine used to allocate scache memory for gdpll module
 */
int _bcm_gdpll_scache_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *gdpll_scache;
    int alloc_size = 0;

    alloc_size = sizeof(gdpll_context_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 1,
              alloc_size, &gdpll_scache, BCM_WB_DEFAULT_VERSION, NULL));
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_gdpll_scache_sync
 * Purpose:
 *    Routine to sync the gdpll module to scache
 */
int _bcm_gdpll_scache_sync(int unit)
{
    int  rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *gdpll_scache;
    int alloc_size = 0;

    alloc_size = sizeof(gdpll_context_t);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, alloc_size,
                         &gdpll_scache, BCM_WB_DEFAULT_VERSION,
                         NULL);
    if (!BCM_FAILURE(rv)) {
        sal_memcpy(gdpll_scache, pGdpllCtx[unit]/*(TIME_INTERFACE(unit, idx))*/,
        sizeof(gdpll_context_t));
    }

    return rv;
}

/*
 * Function:
 *    _gdpll_reinit
 * Purpose:
 *    Internal routine used to reinitialize gdpll module based on HW settings
 *    during Warm boot
 */
int _gdpll_reinit(int unit, gdpll_context_t *pGdpllContext)
{
    soc_scache_handle_t scache_handle;
    uint16      recovered_ver = 0;
    uint8       *gdpll_scache;
    int         rv = BCM_E_NONE;

    if(SOC_WARM_BOOT(unit)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &gdpll_scache, BCM_WB_DEFAULT_VERSION,
                &recovered_ver);
        if (!BCM_FAILURE(rv)) {
            sal_memcpy(pGdpllContext, gdpll_scache,
                        sizeof(gdpll_context_t));
        }
    }

    return (BCM_E_NONE);
}

int _bcm_common_synce_phy_lane_get_port(int unit,
                       int lanePort,
                       int isRx,
                       uint32 *lport)
{
    int first_phy_port, first_log_port, lane_indx = 0;
    int adjusted_lane = 0;
    int port = lanePort;
    int tmp_phy_port = port;
    int num_lanes = 4;
    uint32 flags = 0;

    first_phy_port = (((port - 1)/num_lanes) * num_lanes) + 1;
    adjusted_lane = port - first_phy_port;

    port = first_phy_port;
    while (port < (first_phy_port+num_lanes)) {
        first_log_port = SOC_INFO(unit).port_p2l_mapping[port++];
        if (first_log_port!=-1) {
            break;
        }
    }

#ifdef PORTMOD_SUPPORT
    if (soc_feature(unit, soc_feature_portmod)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port)) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        for (lane_indx = 0; lane_indx < num_lanes; lane_indx++) {
            if (isRx) {
                if (lane_map.lane_map_rx[lane_indx] == adjusted_lane) {
                    break;
                }
            } else {
                if (lane_map.lane_map_tx[lane_indx] == adjusted_lane) {
                    break;
                }
            }
        }
    }
#endif
    tmp_phy_port = first_phy_port + lane_indx;
    bsl_printf(" ### _bcm_synce_phy_lane_get_port: first_phy_port:%d lane_indx:%d\n", first_phy_port, lane_indx);
    if (lport) {
        *lport = SOC_INFO(unit).port_p2l_mapping[tmp_phy_port];;
    }

    return BCM_E_NONE;
}

STATIC int _bcm_common_port_get(int unit, int gdpllPort, bcm_port_t *pPort, int isRx)
{
    int rv  = BCM_E_NONE;

    /* Following is the mapping in Monterey */
    if ((gdpllPort >= 0) && (gdpllPort <= (BCM_GDPLL_NUM_PORTS-1))) {
        _bcm_common_synce_phy_lane_get_port(unit, gdpllPort+1, isRx, (uint32 *)pPort);
        bsl_printf("### _bcm_common_port_get: gdpllPort:%d lPort:%d isRx:%d\n", gdpllPort, *pPort, isRx);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_common_port_get: Error !! Invalid gdpll port\n")));
        rv = BCM_E_PARAM;
    }

    return rv;
}

int
_bcm_common_synce_phy_port_get_lane(int unit,
    int port,
    int rx_lane,
    int *adjusted_lane,
    int *adjusted_phy_port)
{
    /* Currently supported only for Monterey;
    * In order to extend it to various other devices and portmacros
    * take into account num_lanes portmod_support */

    int first_phy_port, first_log_port, lane = 0;
    uint32 flags = 0;
    int num_lanes = 4;

    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    first_phy_port = (SOC_INFO(unit).port_serdes[port] * num_lanes) + 1;
    first_log_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
#ifdef PORTMOD_SUPPORT
    if (soc_feature(unit, soc_feature_portmod)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port)) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        if (rx_lane) {
            lane = lane_map.lane_map_rx[(phy_port - 1) % num_lanes];
        } else {
            lane = lane_map.lane_map_tx[(phy_port - 1) % num_lanes];
        }
    }
#endif

    phy_port = first_phy_port + lane;

    if (adjusted_lane) {
        *adjusted_lane = lane;
    }
    if (adjusted_phy_port) {
        *adjusted_phy_port = phy_port;
    }

    return BCM_E_NONE;
}

int _gdpll_portmap_get(int unit, bcm_port_t port, int *pGdpllPort, int isRx)
{
    int rv  = BCM_E_NONE;

    /* Check if the port is valid */
    if (port <= 0) {
        return BCM_E_PORT;
    }

    if (NULL == pGdpllPort) {
        return BCM_E_PARAM;
    }

    /* Following is the mapping in Monterey */
    if ((port >= 1) && (port <= BCM_GDPLL_NUM_PORTS)) {
        rv = _bcm_common_synce_phy_port_get_lane(unit, (int)port, isRx, NULL, pGdpllPort);
        *pGdpllPort -= 1;
        bsl_printf("### _gdpll_portmap_get: lPort:%d gdpllPort:%d isRx:%d\n", port, *pGdpllPort, isRx);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_gdpll_portmap_get: Error !! Invalid port\n")));
        *pGdpllPort = BCM_GDPLL_NUM_PORTS;
    }

    return rv;
}
#else /* INCLUDE_GDPLL */
typedef int bcm_esw_gdpll_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_GDPLL */
