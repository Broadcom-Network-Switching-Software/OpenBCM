/*
 * $Id: gdpll_interpolator.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */


#include <shared/util.h>
#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/gdpll.h>
#include <bcm_int/gdpll.h>

#include <bcm_int/gdpll_interpolator.h>

#if defined(INCLUDE_GDPLL)

_bcm_gdpll_interpolator_ctxt_t interpolator_ctxt[BCM_GDPLL_MAX_NUM_INTERPOLATOR_EVENTID];

int g_chanIdToInterpolatorIdMap[BCM_GDPLL_NUM_CHANNELS] = {BCM_GDPLL_MAX_NUM_INTERPOLATORS};

extern gdpll_context_t *pGdpllCtx[BCM_MAX_NUM_UNITS];

int gdpll_interpolator_getTableIdx(bcm_gdpll_port_type_t fb_port_type, bcm_gdpll_port_type_t ref_port_type)
{
    fb_port_type--; ref_port_type--;
    return (fb_port_type*(bcmGdpllportTypeLast-1) + ref_port_type);
}

int _bcm_gdpll_interpolator_setup(int unit, bcm_gdpll_chan_t *pGdpllChan, int *pChan) {

    int rv = BCM_E_NONE;
    int count = 0, idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    bcm_gdpll_chan_t *pCurrentGdpllChan = pGdpllChan;
    _bcm_gdpll_interpolator_params_t currInterpolatorParams;
    uint32 event_id_ref = pGdpllContext->dpll_chan[*pChan].eventId_ref;
    bcm_gdpll_port_type_t current_chan_ref_port_type,current_chan_fb_port_type;
    _bcm_gdpll_interpolator_params_t loopIterInterpolatorParams;

    current_chan_ref_port_type = pCurrentGdpllChan->event_ref.port_type;
    current_chan_fb_port_type = pCurrentGdpllChan->event_fb.port_type;

    if ((current_chan_fb_port_type < bcmGdpllPortTypeNone) ||
            (current_chan_fb_port_type >= bcmGdpllportTypeLast) ||
            (current_chan_ref_port_type < bcmGdpllPortTypeNone) ||
            (current_chan_ref_port_type >= bcmGdpllportTypeLast)) {
        bsl_printf("GDPLL INTERPOLATOR: Error! Invalid port type configured\n");
        rv = BCM_E_PARAM;
        goto exit;
    }

    if ((current_chan_fb_port_type = bcmGdpllPortTypeNone) ||
            (current_chan_ref_port_type = bcmGdpllPortTypeNone)) {
        bsl_printf("GDPLL INTERPOLATOR is not needed for this channel\n");
        goto exit;
    }

    idx = gdpll_interpolator_getTableIdx(current_chan_fb_port_type, current_chan_ref_port_type);
    if ((idx < 0) || idx > ((bcmGdpllportTypeLast-1)*(bcmGdpllportTypeLast-1))){
        rv = BCM_E_PARAM;
        goto exit;
    }

    currInterpolatorParams = g_gdpll_interpolator_params[idx];
    if ((currInterpolatorParams.M == 1) && (currInterpolatorParams.N == 1)) {
        bsl_printf("GDPLL INTERPOLATOR is not needed for this channel. Same rates.\n");
        goto exit;
    }

    /* If we reach this stage, it means we need an interpolator.
     * Check if an existing one can be reused.
     */

    for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++){
        dpll_chan_t *pLoopIterGdpllChan = &(pGdpllContext->dpll_chan[count]);
        bcm_gdpll_port_type_t loopIter_chan_ref_port_type, loopIter_chan_fb_port_type;

        loopIter_chan_ref_port_type = pLoopIterGdpllChan->gdpll_chan_config.event_ref.port_type;
        loopIter_chan_fb_port_type = pLoopIterGdpllChan->gdpll_chan_config.event_fb.port_type;

        if ((pLoopIterGdpllChan->flag) & (BCM_GDPLL_CHAN_ALLOC|BCM_GDPLL_EVENT_CONFIG_REF|BCM_GDPLL_EVENT_CONFIG_FB)){
            /* Valid channel is found. Check interpolator*/
            if (!sal_memcmp((void *)&(pLoopIterGdpllChan->gdpll_chan_config.event_ref), (void *)&(pCurrentGdpllChan->event_ref), sizeof(bcm_gdpll_chan_input_t))){
                /* Found a channel with same ref.
                 * Need to check if an interpolator with matching fb
                 * rate already exists that can be reused*/
                idx = gdpll_interpolator_getTableIdx(loopIter_chan_fb_port_type, loopIter_chan_ref_port_type);
                if ((idx < 0) || idx > ((bcmGdpllportTypeLast-1)*(bcmGdpllportTypeLast-1))){
                    rv = BCM_E_PARAM;
                    goto exit;
                }
                loopIterInterpolatorParams = g_gdpll_interpolator_params[idx];
                if (!sal_memcmp((void *)&(loopIterInterpolatorParams), (void *)&(currInterpolatorParams), sizeof(_bcm_gdpll_interpolator_params_t))) {
                    if (g_chanIdToInterpolatorIdMap[count] >= BCM_GDPLL_MAX_NUM_INTERPOLATORS) {
                        bsl_printf("GDPLL INTERPOLATOR: Something went wrong. There should have been a valid interpolator for reuse!!!");
                        goto exit;
                    }
                    /* Found existing interpolator that can be reused */
                    pGdpllContext->dpll_chan[*pChan].iA_ref = interpolator_ctxt[pLoopIterGdpllChan->eventId_ref].interpolator[g_chanIdToInterpolatorIdMap[count]].iaAddr;
                    goto exit;
                }
            }
        }
    }

    /* If we reached this stage, it means we need an interpolator
     * but there isn't an existing one we can reuse. So we need to setup a new one.*/

    /* First step is to redirect the timestamps to R5 */
    pCurrentGdpllChan->event_ref.event_dest = bcmGdpllEventDestALL;

    for(count = 0; count < BCM_GDPLL_MAX_NUM_INTERPOLATORS; count++){
        if (!(interpolator_ctxt[event_id_ref].interpolator[count].enable)) {

        }
    }






exit:
    return rv;
}
#endif /* INCLUDE_GDPLL */
