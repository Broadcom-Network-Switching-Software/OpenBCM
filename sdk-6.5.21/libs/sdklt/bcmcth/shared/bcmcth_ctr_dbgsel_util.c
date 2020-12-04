/*! \file bcmcth_ctr_dbgsel_util.c
 *
 * bcmcth ctr_dbg_sel utilizations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_pt.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_alloc.h>
#include <bcmptm/bcmptm.h>
#include <bcmbd/bcmbd.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmcth/bcmcth_ctr_dbgsel_util.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTRDBGSEL

int
bcmcth_ctr_dbgsel_ireq_read(int unit, bcmltd_sid_t lt_id,
                            bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_read(unit, 0, pt_id, &pt_info,
                             NULL, rsp_entry_wsize, lt_id,
                             entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_dbgsel_ireq_write(int unit, bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                              entry_data, lt_id, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_ctr_dbgsel_bitmap_convert(
    const int *enum2bit,
    const ctr_dbgsel_trigger_t *trigger_array,
    uint64_t *value)
{
    int i;
    int bid;
    bool enable;

    value[0] = value[1] = 0;

    /* Chip specific bit field. */
    for (i = 0; i < CTR_DBG_SEL_NUM; i++ ) {
        if (trigger_array[i].name == CTR_DBG_SEL_NUM) {
            /* No more triggers. */
            break;
        }

        enable = trigger_array[i].enable;
        bid = enum2bit[trigger_array[i].name];

        if (enable) { /* Set the trigger bit. */
            if (bid >= 64) {
                value[1] |= 1ull << (bid - 64);
            } else {
                value[0] |= 1ull << bid;
            }
        } else { /* Clear the trigger bit. */
            if (bid >= 64) {
                value[1] &= ~(1ull << (bid - 64));
            } else {
                value[0] &= ~(1ull << bid);
            }
        }
    }
}

void
bcmcth_ctr_dbgsel_bitmap_update(
    int nshift32,
    const int *enum2bit,
    const ctr_dbgsel_trigger_t *trigger_array,
    uint32_t *value)
{
    int i;
    int bid;
    bool enable;

    /* Chip specific bit field. */
    for (i = 0; i < CTR_DBG_SEL_NUM; i++ ) {
        if (trigger_array[i].name == CTR_DBG_SEL_NUM) {
            /* No more triggers. */
            break;
        }

        enable = trigger_array[i].enable;
        bid = enum2bit[trigger_array[i].name];

        /* Update value in expected range. */
        if ((bid < 32 * nshift32) || (bid >= 32 * (nshift32 + 1))){
            continue;
        }

        if (enable) { /* Set the trigger bit. */
            (*value) |= 1ull << (bid - 32 * nshift32);
        } else { /* Clear the trigger bit. */
            (*value) &= ~(1ull << (bid - 32 * nshift32));
        }
    }
}

