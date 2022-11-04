/*
 * $Id: gdpll_interpolator.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */


#include <shared/util.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#include <shared/pack.h>
#include <bcm/error.h>
#include <bcm/gdpll.h>
#include <bcm_int/gdpll.h>

#include <bcm_int/gdpll_interpolator.h>

#if EDKHLIB

typedef void* handle_t;

extern int edk_rpc_call_sync(handle_t rproc, char *fx_name, void *inp, int insize,
                      void *outp, int outsize, int32 *retval, int timeoutms);

extern handle_t edk_bs_handle;
#endif

#if defined(INCLUDE_GDPLL)

_bcm_gdpll_interpolator_ctxt_t interpolator_ctxt[BCM_GDPLL_MAX_NUM_INTERPOLATOR_EVENTID];
int interpolator_iA_addr_array[BCM_GDPLL_MAX_NUM_INTERPOLATORS] = {BCM_GDPLL_NUM_IA_ENTRIES};

int g_chanIdToInterpolatorIdMap[BCM_GDPLL_NUM_CHANNELS] = {BCM_GDPLL_MAX_NUM_INTERPOLATORS};

extern gdpll_context_t *pGdpllCtx[BCM_MAX_NUM_UNITS];

void init_interpolator( )
{
    int i=0;
    for (i=0; i< BCM_GDPLL_MAX_NUM_INTERPOLATORS; i++) {
        interpolator_iA_addr_array[i] = BCM_GDPLL_NUM_IA_ENTRIES;
    }
    for (i=0; i< BCM_GDPLL_NUM_CHANNELS; i++) {
        g_chanIdToInterpolatorIdMap[i] = BCM_GDPLL_MAX_NUM_INTERPOLATORS;
    }
}

int gdpll_interpolator_getTableIdx(bcm_gdpll_port_type_t fb_port_type, bcm_gdpll_port_type_t ref_port_type)
{
    fb_port_type--; ref_port_type--;
    return (fb_port_type*(bcmGdpllportTypeLast-1) + ref_port_type);
}

int _bcm_gdpll_interpolator_iaAddr_alloc(void)
{
    int i = 0;
    for (i = 0; i < BCM_GDPLL_MAX_NUM_INTERPOLATORS; i++)
    {
        if (interpolator_iA_addr_array[i] == BCM_GDPLL_NUM_IA_ENTRIES)
        {
            /*Found unused IA address. return it*/
            interpolator_iA_addr_array[i] = BCM_GDPLL_NUM_IA_ENTRIES - (i+1);
            return interpolator_iA_addr_array[i];
        }
    }
    return BCM_GDPLL_NUM_IA_ENTRIES;
}

int _bcm_gdpll_interpolator_iaAddr_unalloc(int iaAddr)
{
    int i = 0;
    for (i = 0; i < BCM_GDPLL_MAX_NUM_INTERPOLATORS; i++)
    {
        if (interpolator_iA_addr_array[i] == iaAddr)
        {
            /*Found the array idx for the IA address. Unallocate it*/
            interpolator_iA_addr_array[i] = BCM_GDPLL_NUM_IA_ENTRIES;
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

int _bcm_uc_interpolator_ctxt_cfg(int ref_idx, int event_id_ref, int count)
{
    int rv = BCM_E_NONE;
#ifdef EDKHLIB
    uint8 msg[50];
    int32 reply_len;
    uint8 *buffer_ptr = NULL;
    uint16 buffer_len = 0;
    _bcm_gdpll_interpolator_t new_ctxt = interpolator_ctxt[ref_idx].interpolator[count];

    buffer_ptr = &msg[0];

    _SHR_PACK_U16(buffer_ptr, event_id_ref);
    _SHR_PACK_U8(buffer_ptr, count);
    _SHR_PACK_U8(buffer_ptr, interpolator_ctxt[ref_idx].num_interpolators);
    _SHR_PACK_U8(buffer_ptr, new_ctxt.enable);
    _SHR_PACK_U64(buffer_ptr, new_ctxt.M);
    _SHR_PACK_U64(buffer_ptr, new_ctxt.N);
    _SHR_PACK_U32(buffer_ptr, new_ctxt.InvM);
    _SHR_PACK_U16(buffer_ptr, new_ctxt.InvMShift);
    _SHR_PACK_U64(buffer_ptr, new_ctxt.prevTs);
    _SHR_PACK_U64(buffer_ptr, new_ctxt.Accum);
    _SHR_PACK_U32(buffer_ptr, new_ctxt.iaAddr);
    buffer_len = (buffer_ptr - msg);

    if (edk_bs_handle != NULL) {
        rv = edk_rpc_call_sync(edk_bs_handle, "uc_bs_interpolator_cfg", (void *)msg,
                buffer_len, NULL, 0, &reply_len, 1000);
    } else {
        bsl_printf(" proc mgr open not done or failed. retry time init\n");
        rv = BCM_E_INIT;
    }

#endif
    return rv;
}

int _bcm_gdpll_interpolator_setup(int unit, bcm_gdpll_chan_t *pGdpllChan, int *pChan, uint32 *piaAddr_updated)
{
    int rv = BCM_E_NONE;
    int count = 0, idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    bcm_gdpll_chan_t *pCurrentGdpllChan = pGdpllChan;
    _bcm_gdpll_interpolator_params_t currInterpolatorParams;
    uint32 event_id_ref = pGdpllContext->dpll_chan[*pChan].eventId_ref;
    bcm_gdpll_port_type_t current_chan_ref_port_type,current_chan_fb_port_type;
    _bcm_gdpll_interpolator_params_t loopIterInterpolatorParams;

    /* Assuming interpolator ctxt is required only for Rx ports. */
    uint32 ref_idx = ((event_id_ref-35)/2);

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

    if ((current_chan_fb_port_type == bcmGdpllPortTypeNone) ||
            (current_chan_ref_port_type == bcmGdpllPortTypeNone)) {
        bsl_printf("GDPLL INTERPOLATOR is not needed for this channel\n");
        goto exit;
    }

    if (ref_idx >= BCM_GDPLL_MAX_NUM_INTERPOLATOR_EVENTID) {
        bsl_printf("GDPLL INTERPOLATOR: Index:%d beyond permitted range for ref_evt:%d \n", ref_idx, event_id_ref);
        rv = BCM_E_PARAM;
        goto exit;
    }
    bsl_printf("GDPLL INTERPOLATOR: idx:%d ref_evt:%d \n", ref_idx, event_id_ref);

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

    pCurrentGdpllChan->event_ref.event_dest = bcmGdpllEventDestBitmap;
    pCurrentGdpllChan->event_ref.event_dest_bitmap |= BCM_GDPLL_EVENT_DEST_FIFO2;

    /* If we reach this stage, it means we need an interpolator.
     * Check if an existing one can be reused.
     */

    for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++) {
        dpll_chan_t *pLoopIterGdpllChan = &(pGdpllContext->dpll_chan[count]);
        bcm_gdpll_port_type_t loopIter_chan_ref_port_type, loopIter_chan_fb_port_type;

        loopIter_chan_ref_port_type = pLoopIterGdpllChan->gdpll_chan_config.event_ref.port_type;
        loopIter_chan_fb_port_type = pLoopIterGdpllChan->gdpll_chan_config.event_fb.port_type;

        if ((pLoopIterGdpllChan->flag) & (BCM_GDPLL_CHAN_ALLOC|BCM_GDPLL_EVENT_CONFIG_REF|BCM_GDPLL_EVENT_CONFIG_FB)){
            /* Valid channel is found. Check interpolator*/
            if (!sal_memcmp((void *)&(pLoopIterGdpllChan->gdpll_chan_config.event_ref), (void *)&(pCurrentGdpllChan->event_ref), sizeof(bcm_gdpll_chan_input_t))){
                bsl_printf("Channel[%d] with same ref found ... \n", count);
                /* Found a channel with same ref.
                 * Need to check if an interpolator with matching fb
                 * rate already exists that can be reused*/
                idx = gdpll_interpolator_getTableIdx(loopIter_chan_fb_port_type, loopIter_chan_ref_port_type);
                if ((idx < 0) || idx > ((bcmGdpllportTypeLast-1)*(bcmGdpllportTypeLast-1))){
                    bsl_printf(" GDPLL INTERPOLATOR: retrieved tbl idx: %d is invalid.\n", idx);
                    rv = BCM_E_PARAM;
                    goto exit;
                }
                loopIterInterpolatorParams = g_gdpll_interpolator_params[idx];
                if (!sal_memcmp((void *)&(loopIterInterpolatorParams), (void *)&(currInterpolatorParams), sizeof(_bcm_gdpll_interpolator_params_t))) {
                    if (g_chanIdToInterpolatorIdMap[count] >= BCM_GDPLL_MAX_NUM_INTERPOLATORS) {
                        bsl_printf("GDPLL INTERPOLATOR: Something went wrong. There should have been a valid interpolator for reuse!!!");
                        rv = BCM_E_PARAM;
                        goto exit;
                    }
                    /* Found existing interpolator that can be reused */
                    pGdpllContext->dpll_chan[*pChan].iA_ref = interpolator_ctxt[ref_idx].interpolator[g_chanIdToInterpolatorIdMap[count]].iaAddr;
                    *piaAddr_updated = pGdpllContext->dpll_chan[*pChan].iA_ref;
                    bsl_printf("GDPLL INTERPOLATOR: Reusing existing instance. ref_evt:%d IA_ref: %d\n", pLoopIterGdpllChan->eventId_ref, *piaAddr_updated);
                    goto exit;
                }
            }
        }
    }
    if (count == BCM_GDPLL_NUM_CHANNELS) {
        bsl_printf(" No matching instances found. Setup a new one. \n" );
    }

    /* If we reached this stage, it means we need an interpolator
     * but there isn't an existing one we can reuse. So we need to setup a new one.*/

    for(count = 0; count < BCM_GDPLL_MAX_NUM_INTERPOLATORS; count++) {
        if (!(interpolator_ctxt[ref_idx].interpolator[count].enable)) {
            /*Found an unused interpolator index. We can use this.*/
            interpolator_ctxt[ref_idx].interpolator[count].InvM = currInterpolatorParams.InvM;
            interpolator_ctxt[ref_idx].interpolator[count].Accum = currInterpolatorParams.N;
            interpolator_ctxt[ref_idx].interpolator[count].InvMShift = currInterpolatorParams.InvMShift;
            interpolator_ctxt[ref_idx].interpolator[count].M = currInterpolatorParams.M;
            interpolator_ctxt[ref_idx].interpolator[count].N = currInterpolatorParams.N;
            interpolator_ctxt[ref_idx].interpolator[count].iaAddr = _bcm_gdpll_interpolator_iaAddr_alloc();
            if (BCM_GDPLL_NUM_IA_ENTRIES == interpolator_ctxt[ref_idx].interpolator[count].iaAddr){
                /*All IA addressess are used up.*/
                rv = BCM_E_FULL;
                bsl_printf("GDPLL INTERPOLATOR: ref:%d count:%d Max inst. allocated. \n", ref_idx, count);
                goto exit;
            }
            *piaAddr_updated = interpolator_ctxt[ref_idx].interpolator[count].iaAddr;
            bsl_printf("GDPLL INTERPOLATOR: assigned iaAddr: %d ref:%d inst:%d\n", (int)interpolator_ctxt[ref_idx].interpolator[count].iaAddr, ref_idx, count);
            interpolator_ctxt[ref_idx].interpolator[count].prevTs = INTERPOLATOR_INVALID_TS;
            interpolator_ctxt[ref_idx].interpolator[count].enable = 1;
            g_chanIdToInterpolatorIdMap[*pChan] = count;
            if (count < interpolator_ctxt[ref_idx].num_interpolators)
            {
                /* Reusing a previously used interpolator context. Nothing to do here */
                bsl_printf("GDPLL INTERPOLATOR: Reusing interpolator[%d] \n", count);
            } else if((count) == interpolator_ctxt[ref_idx].num_interpolators) {
                /* Adding a new interpolator at the tail*/
                bsl_printf("GDPLL INTERPOLATOR: Adding interpolator[%d] \n", count);
                interpolator_ctxt[ref_idx].num_interpolators++;
            } else {
                /*Something went wrong!*/
                bsl_printf("GDPLL INTERPOLATOR: Something is wrong ... exiting\n");
                rv = BCM_E_PARAM;
                goto exit;
            }

            rv = _bcm_uc_interpolator_ctxt_cfg(ref_idx, event_id_ref, count);
            bsl_printf("GDPLL INTERPOLATOR: interpolator context config rv:%d \n", rv);

            goto exit;
        }
    }

exit:
    //bsl_printf("GDPLL INTERPOLATOR: return with rv: %d \n", rv);
    return rv;
}
#endif /* INCLUDE_GDPLL */
