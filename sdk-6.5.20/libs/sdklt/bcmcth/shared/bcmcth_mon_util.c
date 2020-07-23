/*! \file bcmcth_mon_util.c
 *
 * bcmmon utility function
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
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_mon_sysm.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_pt.h>
/*******************************************************************************
 * Defines
 */
/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Generic hw mem table write for BCMMON.
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
bcmcth_mon_pt_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                    bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
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
 * \brief Generic hw mem table read for BCMMON.
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
bcmcth_mon_pt_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
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
 * \brief Generic hw reg write for BCMMON.
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
bcmcth_mon_pr_read(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
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
 * \brief Generic hw reg read for BCMMON.
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
bcmcth_mon_pr_write(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
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
/*!
 * \brief Generic interactive hw mem table read for a given pipe for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe  Pipe number.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_pt_iread_pipe(int unit, bcmltd_sid_t lt_id,
        bcmdrd_sid_t pt_id, int pipe, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t            rsp_entry_wsize;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    pt_info.tbl_inst = pipe;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_IF_ERR_EXIT(
            bcmptm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                rsp_entry_wsize, lt_id,
                entry_data, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic interactive hw mem table read for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_pt_iread(int unit, bcmltd_sid_t lt_id,
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
            bcmptm_ireq_read(unit, 0, pt_id, &pt_info, NULL,
                rsp_entry_wsize, lt_id,
                entry_data, &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Generic hw mem table write for BCMMON.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_pt_iwrite(int unit, bcmltd_sid_t lt_id,
        bcmdrd_sid_t pt_id, int index, void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = index;
    SHR_IF_ERR_EXIT(
            bcmptm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                entry_data, lt_id,
                &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generic hw mem table write for a given pipe for BCMMON
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe  Pipe number.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_pt_iwrite_pipe(int unit, bcmltd_sid_t lt_id,
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
            bcmptm_ireq_write(unit, 0, pt_id, &pt_info, NULL,
                entry_data, lt_id,
                &rsp_ltid, &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
