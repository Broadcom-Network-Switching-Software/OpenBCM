/*! \file bcmtm_pt.c
 *
 * TM moduler interaction with physical table functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmdrd/bcmdrd_pt.h>


#define BSL_LOG_MODULE  BSL_LS_BCMTM_PT

int
bcmtm_pt_indexed_write(int unit,
                       bcmdrd_sid_t sid,
                       bcmltd_sid_t lt_id,
                       bcmtm_pt_info_t *pt_info,
                       uint32_t *ltmbuf)
{
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    pt_dyn_info.index = pt_info->index;
    pt_dyn_info.tbl_inst = pt_info->tbl_inst;
    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_write(unit, pt_info->flags,
                               sid, (void *)&pt_dyn_info,
                               NULL, ltmbuf, lt_id, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}


int
bcmtm_pt_indexed_read(int unit,
                      bcmdrd_sid_t sid,
                      bcmltd_sid_t lt_id,
                      bcmtm_pt_info_t *pt_info,
                      uint32_t *ltmbuf)
{
    bcmdrd_sym_info_t sinfo;
    bcmltd_sid_t rsp_ltid;
    uint32_t rsp_flags;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);
    pt_dyn_info.index = pt_info->index;
    pt_dyn_info.tbl_inst = pt_info->tbl_inst;

    if (SHR_FAILURE(bcmdrd_pt_info_get(unit, sid, &sinfo))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmptm_ltm_ireq_read(unit, pt_info->flags,
                                         sid, (void *)&pt_dyn_info,
                                         NULL, sinfo.entry_wsize,
                                         lt_id, ltmbuf,
                                         &rsp_ltid, &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

uint32_t
bcmtm_pt_fid_size_get(int unit,
                      bcmdrd_sid_t sid,
                      bcmdrd_fid_t fid,
                      uint32_t *size)
{
    bcmdrd_sym_field_info_t finfo;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    *size = (finfo.maxbit - finfo.minbit + 1);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_set(int unit,
                bcmdrd_sid_t sid,
                bcmdrd_fid_t fid,
                uint32_t *entbuf,
                uint32_t *fbuf)
{
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_set(entbuf, finfo.minbit, finfo.maxbit, fbuf);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_get(int unit,
                bcmdrd_sid_t sid,
                bcmdrd_fid_t fid,
                uint32_t *entbuf,
                uint32_t *fbuf)
{
    bcmdrd_sym_field_info_t finfo;

    SHR_FUNC_ENTER(unit);
    *fbuf = 0;
    SHR_IF_ERR_EXIT(bcmdrd_pt_field_info_get(unit, sid, fid, &finfo));
    bcmdrd_field_get(entbuf, finfo.minbit, finfo.maxbit, fbuf);
exit:
    SHR_FUNC_EXIT();
}


