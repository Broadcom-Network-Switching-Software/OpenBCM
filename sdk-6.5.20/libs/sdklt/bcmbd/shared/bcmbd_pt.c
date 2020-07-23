/*! \file bcmbd_pt.c
 *
 * Wrappers for physical table (PT) driver operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <bcmbd/bcmbd.h>

int
bcmbd_pt_read(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data, size_t wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_read == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_read(unit, sid, dyn_info, ovrr_info, data, wsize);
}

int
bcmbd_pt_write(int unit, bcmdrd_sid_t sid,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_write == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_write(unit, sid, dyn_info, ovrr_info, data);
}

int
bcmbd_pt_range_read(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    void *ovrr_info, size_t num_entries,
                    uint64_t dma_addr, uint32_t flags)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_range_op == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_range_op(unit, sid, dyn_info, ovrr_info,
                           num_entries, dma_addr,
                           flags |= BCMBD_ROF_READ);
}

int
bcmbd_pt_range_write(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info, size_t num_entries,
                     uint64_t dma_addr, uint32_t flags)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_range_op == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_range_op(unit, sid, dyn_info, ovrr_info,
                           num_entries, dma_addr,
                           flags &= ~BCMBD_ROF_READ);
}

int
bcmbd_pt_insert(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_insert == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_insert(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_delete(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_delete == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_delete(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_lookup(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_lookup == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_lookup(unit, sid, dyn_hash_info, ovrr_hash_info, data, wsize,
                         resp_info, resp_data);
}

int
bcmbd_pt_pop(int unit, bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
             uint32_t *data, size_t wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_pop == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_pop(unit, sid, dyn_info, ovrr_info, data, wsize);
}

int
bcmbd_pt_push(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_push == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_push(unit, sid, dyn_info, ovrr_info, data);
}

int
bcmbd_pt_addr_decode(int unit,
                     uint32_t adext, uint32_t addr, uint32_t drd_flags,
                     bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_addr_decode == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_addr_decode(unit, adext, addr, drd_flags, sid, dyn_info);
}

int
bcmbd_pt_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info, bcmbd_pt_cmd_t cmd,
                     uint32_t wsize, uint32_t *data,
                     uint32_t *hdr_wsize)
{
    bcmbd_drv_t *bd = bcmbd_drv_get(unit);

    if (bd == NULL) {
        return SHR_E_UNIT;
    }

    if (bd->pt_cmd_hdr_get == NULL) {
        return SHR_E_UNAVAIL;
    }

    return bd->pt_cmd_hdr_get(unit, sid, dyn_info, ovrr_info, cmd, wsize,
                              data, hdr_wsize);
}
