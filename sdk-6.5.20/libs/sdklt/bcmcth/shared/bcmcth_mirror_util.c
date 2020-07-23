/*! \file bcmcth_mirror_util.c
 *
 * bcmmirror utility function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_mirror_sysm.h>
#include <bcmcth/bcmcth_mirror_util.h>
#include <bcmdrd/bcmdrd_pt.h>

/*******************************************************************************
 * Defines
 */
/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

#define IS_PIPE_ENABLE(map, p) ((map >> p) & 0x1)
/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Validate the pipe of the memory.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Memory table identifier.
 * \param [in] pipe Pipe of memory entry to check.
 *
 * \retval TRUE valid pipe.
 * \retval FALSE in-valid pipe.
 */
static bool
is_pipe_enable(int unit, bcmdrd_sid_t sid, int pipe)
{
    int blktype;
    uint32_t pipe_map;
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);

    return IS_PIPE_ENABLE(pipe_map, pipe);
}

/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Generic hw mem table write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pt_pipe_write(
    int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id, int pipe, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = pipe;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic hw mem table write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pt_write(
    int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    int pipe, pipe_num;

    SHR_FUNC_ENTER(unit);

    pipe_num = bcmdrd_pt_num_tbl_inst(unit, pt_id);
    /* pipe */
    for (pipe = 0; pipe < pipe_num; pipe++) {
        if (is_pipe_enable(unit, pt_id, pipe)) {
            SHR_IF_ERR_EXIT
                (bcmcth_mirror_pt_pipe_write(unit, trans_id, lt_id, pt_id,
                                             pipe, index, entry_data));
        }
    }
 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic hw mem table read for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pt_read(
    int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index   = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic hw reg write for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pr_read(
    int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_READ,
                                   NULL, rsp_entry_wsize, lt_id,
                                   trans_id, NULL, NULL,
                                   entry_data, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic hw reg read for BCMMIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mirror_pr_write(
    int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.tbl_inst = index;
    SHR_IF_ERR_EXIT(
        bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info, NULL, NULL,
                                   BCMPTM_OP_WRITE,
                                   entry_data, 0, lt_id,
                                   trans_id, NULL, NULL,
                                   NULL, &rsp_ltid, &rsp_flags));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_ireq_read(int unit, bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint64_t            req_flags;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index    = index;
    rsp_entry_wsize  = bcmdrd_pt_entry_wsize(unit, pt_id);
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_ireq_read(unit, req_flags,
                              pt_id, &pt_info, NULL, rsp_entry_wsize, 0,
                              entry_data, &rsp_ltid, &rsp_flags));
 exit:
    SHR_FUNC_EXIT();
}

