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
 *      bcm_dnx_gdpll_chan_create
 *      bcm_dnx_gdpll_chan_destroy
 *      bcm_dnx_gdpll_chan_enable
 *      bcm_dnx_gdpll_chan_state_get
 *      bcm_dnx_gdpll_chan_debug_enable
 *      bcm_dnx_gdpll_debug_cb_register
 *      bcm_dnx_gdpll_flush
 *      bcm_dnx_gdpll_cb_register
 *      bcm_dnx_gdpll_cb_unregister
 *      bcm_dnx_gdpll_debug
 *      bcm_dnx_gdpll_offset_get
 *      bcm_dnx_gdpll_offset_set
 *      bcm_dnx_gdpll_chan_get
 *      bcm_dnx_gdpll_input_event_poll
 */

typedef int _bcm_gdpll_not_empty;       /* Make ISO compilers happy. */

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

int
bcm_dnx_gdpll_chan_create(
    int unit,
    uint32 flags,
    bcm_gdpll_chan_t * pGdpllChan,
    int *pChan)
{
    return bcm_common_gdpll_chan_create(unit, flags, pGdpllChan, pChan);
}

int
bcm_dnx_gdpll_chan_destroy(
    int unit,
    int chan)
{
    return bcm_common_gdpll_chan_destroy(unit, chan);
}

int
bcm_dnx_gdpll_chan_enable(
    int unit,
    int chan,
    int enable)
{
    return bcm_common_gdpll_chan_enable(unit, chan, enable);
}

int
bcm_dnx_gdpll_chan_state_get(
    int unit,
    int chan,
    uint32 *dpllState)
{
    return bcm_common_gdpll_chan_state_get(unit, chan, dpllState);
}

int
bcm_dnx_gdpll_chan_debug_enable(
    int unit,
    int chan,
    bcm_gdpll_debug_mode_t debug_mode,
    int enable)
{
    return bcm_common_gdpll_chan_debug_enable(unit, chan, debug_mode, enable);
}

int
bcm_dnx_gdpll_debug_cb_register(
    int unit,
    void *user_data,
    bcm_gdpll_debug_cb cb)
{
    return bcm_common_gdpll_debug_cb_register(unit, user_data, cb);
}

int
bcm_dnx_gdpll_flush(
    int unit)
{
    return bcm_common_gdpll_flush(unit);
}

int
bcm_dnx_gdpll_cb_register(
    int unit,
    bcm_gdpll_cb_reg_t cb_type,
    bcm_gdpll_cb cb,
    void *user_data)
{
    return bcm_common_gdpll_cb_register(unit, cb_type, cb, user_data);
}

int
bcm_dnx_gdpll_cb_unregister(
    int unit,
    bcm_gdpll_cb_reg_t cb_type)
{
    return bcm_common_gdpll_cb_unregister(unit, cb_type);
}

int
bcm_dnx_gdpll_debug(
    int unit,
    int enable)
{
    return bcm_common_gdpll_debug(unit, enable);
}

int
bcm_dnx_gdpll_offset_get(
    int unit,
    int chan,
    uint64 *pOffset)
{
    return bcm_common_gdpll_offset_get(unit, chan, pOffset);
}

int
bcm_dnx_gdpll_offset_set(
    int unit,
    int chan,
    uint64 Offset)
{
    return bcm_common_gdpll_offset_set(unit, chan, Offset);
}

int
bcm_dnx_gdpll_chan_get(
    int unit,
    int chan,
    bcm_gdpll_chan_t * gdpll_chan)
{
    return bcm_common_gdpll_chan_get(unit, chan, gdpll_chan);
}

int
bcm_dnx_gdpll_input_event_poll(
    int unit,
    bcm_gdpll_input_event_t input_event,
    uint32 timeout)
{
    return bcm_common_gdpll_input_event_poll(unit, input_event, timeout);
}

#endif
