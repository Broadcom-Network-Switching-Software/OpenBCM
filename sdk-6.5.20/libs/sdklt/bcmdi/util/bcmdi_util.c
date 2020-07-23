/*! \file bcmdi_util.c
 *
 * BCMDI Utilities
 *
 * This module contains the implementation of BCMDI utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm.h>
#include <bcmdi/bcmdi_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMDI_UTIL

int
bcmdi_entry_load_from_host(int unit, uint8_t *dst, const uint8_t *src, int size)
{
    int i;
    uint32_t *wdst = (uint32_t *)dst;
    uint32_t *wsrc = (uint32_t *)src;
    int wsize = (size + DI_BYTES_PER_WORD - 1) / DI_BYTES_PER_WORD;

    shr_util_host_to_le32(wdst, wsrc, wsize);

    for (i = size; i < wsize * DI_BYTES_PER_WORD; i++) {
        dst[i] = 0x0;
    }

    return SHR_E_NONE;
}

int
bcmdi_entry_load_to_host(int unit, uint8_t *dst, const uint8_t *src, int size)
{
    int wsize = size / DI_BYTES_PER_WORD;
    int wmod = size % DI_BYTES_PER_WORD;
    uint32_t *wdst = (uint32_t *)dst;
    uint32_t *wsrc = (uint32_t *)src;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(src, SHR_E_INTERNAL);
    SHR_NULL_CHECK(dst, SHR_E_INTERNAL);

    shr_util_host_to_le32(wdst, wsrc, wsize);
    if (wmod) {
        uint32_t wdata = 0;
        sal_memcpy(&wdata, &wsrc[wsize], wmod);
        shr_util_host_to_le32(&wdst[wsize], &wdata, 1);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmdi_pt_read(int unit, bcmdrd_sid_t sid, int index,
                       int inst, uint32_t *entry_bytes)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, sid);
    pt_info.index = index;
    pt_info.tbl_inst = inst;

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          0,
                          sid,
                          &pt_info,
                          NULL,
                          rsp_entry_wsize,
                          0,
                          entry_bytes,
                          &rsp_ltid,
                          &rsp_flags));
exit:
    SHR_FUNC_EXIT();

}


int
bcmdi_pt_write(int unit, bcmdrd_sid_t sid, int index,
                        int inst, uint32_t *entry_bytes)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = inst;

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           BCMPTM_REQ_FLAGS_NO_FLAGS,
                           sid,
                           &pt_info,
                           NULL,
                           entry_bytes,
                           0,
                           &rsp_ltid,
                           &rsp_flags));
exit:
    SHR_FUNC_EXIT();

}
