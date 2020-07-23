/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        gdpll.h
 * Purpose:     Internal GDPLL header
 */

#ifndef __BCM_INT_GDPLL_H__
#define __BCM_INT_GDPLL_H__

#if defined(INCLUDE_GDPLL)

#include <sal/core/sync.h>

#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll_shared.h>

/* Number of GDPLL IA and OA locations */
#define BCM_GDPLL_NUM_IA_ENTRIES    256
#define BCM_GDPLL_NUM_OA_ENTRIES    128

#define BCM_GDPLL_IA_START          (0x03271000)
#define BCM_GDPLL_OA_START          (0x03272000)

/* TBD - As per section 6.6, port events are from 0 to 191 and misc events
 * start from 192
 */
#define BCM_GDPLL_NUM_INPUT_EVENTS       256
#define BCM_GDPLL_IA_START_TXPI_TXSOF    128
#define BCM_GDPLL_IA_START_MISC          192
#define BCM_GDPLL_IA_START_R5            214

#define GDPLL_DEBUGBUFF_LOCAL_COPY
#ifdef GDPLL_DEBUGBUFF_LOCAL_COPY
#define GDPLL_NUM_DEBUG_BUFFERS     2
#endif

#define SWAP32_ENDIAN(x) ( \
    (((x) << 24) & 0xff000000) | \
    (((x) <<  8) & 0x00ff0000) | \
    (((x) >>  8) & 0x0000ff00) | \
    (((x) >> 24) & 0x000000ff))
#define SWAP16_ENDIAN(x) ( \
    (((x) <<  8) & 0xff00) | \
    (((x) >>  8) & 0x00ff))

#ifdef LE_HOST
#define swap32(_l)      (_l)
#define swap16(_s)      (_s)
#define swap64_h(x)     (u64_H(x))
#define swap64_l(x)     (u64_L(x))
#else
#define swap32(_l)      _shr_swap32(_l)
#define swap16(_s)      _shr_swap16(_s)
#define swap64_h(x)     (swap32(u64_L(x)))
#define swap64_l(x)     (swap32(u64_H(x)))
#endif

/* M7 TCM region references */
#define BCM_M7_ITCM_BASE                (0x3260000)
#define BCM_M7_DTCM_BASE                (0x3264000)

#define READ_NS_REGr(unit, reg, idx, rvp) \
    soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rvp);
#define WRITE_NS_REGr(unit, reg, idx, rv) \
    soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rv)

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_gdpll_scache_sync(int unit);
#endif

/* Internal datastructure for event destination config */
typedef struct gdpll_event_dest_cfg_s {
    bcm_gdpll_event_dest_t event_dest;
    int                 ts_counter;       /* 0: TS0     1:TS1*/
    int                 ppm_check_enable; /* PPM check enable */
} gdpll_event_dest_cfg_t;

/* Internal datastructure for gdpll channel */
typedef struct dpll_chan_s {
    uint32 flag;        /* Channel flags */
    uint32 eventId_ref; /* Channel reference event */
    uint32 eventId_fb;  /* Channel feedback event */
    bcm_port_t lport_ref;
    bcm_port_t lport_fb;
    int     gdpllport_ref;
    int     gdpllport_fb;
    int    phaseConterRef;
    uint8  enable;
    bcm_gdpll_chan_state_t prev_state;
    bcm_gdpll_chan_state_t curr_state;
    bcm_gdpll_chan_t gdpll_chan_config;
}dpll_chan_t;

/* Internal datastructure for gdpll context */
typedef struct gdpll_context_s {
    /* Per dpll channel parameters */
    dpll_chan_t dpll_chan[BCM_GDPLL_NUM_CHANNELS];

    /* event_id array holds the Input array location for that event */
    int          eventId[BCM_GDPLL_NUM_INPUT_EVENTS];
    sal_mutex_t  mutex;

    /* User callback for debug registry */
    bcm_gdpll_debug_cb debug_cb;
    void         *pUserData;

    /* User callback for various events */
    bcm_gdpll_cb gdpll_cb[bcmGdpllCbMax];
    void         *pGdpll_cb_data[bcmGdpllCbMax];

    /* Debug buffer sync constructs */
    sal_sem_t    debug_sem;
    uint32       debugRdIdx;

    /* CB constructs */
    sal_sem_t    cb_sem;
}gdpll_context_t;

/* Internal datastructure for dpll config */
typedef struct dpll_param_s {
    int debugMode;
    bcm_gdpll_chan_dpll_config_t    dpll_config;
    bcm_gdpll_chan_dpll_state_t     dpll_state;
} dpll_param_t;

extern uint32 _bcm_esw_gdpll_event_fb_divisor_get(int unit, int chan);
#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_H__ */
