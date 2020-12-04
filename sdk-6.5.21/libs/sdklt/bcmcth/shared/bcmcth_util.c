/*! \file bcmcth_util.c
 *
 * CTH component data management.
 *
 * This file implements the CTH component data management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <sal/sal_alloc.h>

#include <bcmcth/bcmcth_util.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Public functions
 */

int
bcmcth_pt_write(int unit,
                bcmcth_pt_op_info_t *op_info,
                uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    op_info->req_flags,
                                    op_info->drd_sid,
                                    (void *) &(op_info->dyn_info),
                                    ptr_pt_ovr_info,
                                    NULL,
                                    op_info->op,
                                    buf,
                                    op_info->wsize,
                                    op_info->req_lt_sid,
                                    op_info->trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ent_buf,
                                    &(op_info->rsp_lt_sid),
                                    &(op_info->rsp_flags)
            ));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "pt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
               op_info->drd_sid,
               op_info->dyn_info.index,
               op_info->trans_id,
               op_info->req_lt_sid,
               op_info->rsp_lt_sid));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_pt_read(int unit,
               bcmcth_pt_op_info_t *op_info,
               uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "pt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%" PRIx64
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
               op_info->drd_sid,
               op_info->dyn_info.index,
               op_info->trans_id,
               op_info->req_flags,
               op_info->req_lt_sid,
               op_info->rsp_lt_sid));

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    op_info->req_flags,
                                    op_info->drd_sid,
                                    (void *) &(op_info->dyn_info),
                                    ptr_pt_ovr_info,
                                    NULL,
                                    op_info->op,
                                    ent_buf,
                                    op_info->wsize,
                                    op_info->req_lt_sid,
                                    op_info->trans_id,
                                    NULL,
                                    NULL,
                                    buf,
                                    &(op_info->rsp_lt_sid),
                                    &(op_info->rsp_flags)));

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "pt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%" PRIx64
                          " req_ltid=0x%x rsp_ltid=0x%x\n"),
               op_info->drd_sid,
               op_info->dyn_info.index,
               op_info->trans_id,
               op_info->req_flags,
               op_info->req_lt_sid,
               op_info->rsp_lt_sid));

    /*
     * NO SUPPORT FOR RSP_LTID IN PTM WRITE/READ APIs FOR INDEXED TABLES.
     *    SHR_IF_ERR_EXIT
     *        ((BCMPTM_LTID_RSP_NOT_SUPPORTED != op_info->rsp_lt_sid)
     *        ? SHR_E_INTERNAL : SHR_E_NONE);
     */
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_irq_pt_write(int unit,
                    bcmcth_pt_op_info_t *op_info,
                    uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    op_info->req_flags,
                                    op_info->drd_sid,
                                    (void *) &(op_info->dyn_info),
                                    ptr_pt_ovr_info,
                                    NULL,
                                    op_info->op,
                                    buf,
                                    op_info->wsize,
                                    op_info->req_lt_sid,
                                    op_info->trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ent_buf,
                                    &(op_info->rsp_lt_sid),
                                    &(op_info->rsp_flags)
            ));
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "pt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
               op_info->drd_sid,
               op_info->dyn_info.index,
               op_info->trans_id,
               op_info->req_lt_sid,
               op_info->rsp_lt_sid));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] pt_id Physical table ID.
 * \param [in] index Index to write to.
 * \param [in] entry_data Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
int
bcmcth_ireq_write(int unit, bcmltd_sid_t lt_id,
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

/*!
 * \brief Reads the field value from field list given
 *        field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] in Field list.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
int
bcmcth_imm_field_get(int unit,
                     const bcmltd_field_t *in,
                     uint32_t fid,
                     uint64_t *fval)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_INTERNAL);

    *fval = 0;
    do {
        if (fid == in->id) {
            *fval = in->data;
            SHR_EXIT();
        }
        in = in->next;
    } while (in != NULL);
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_uint64_to_uint32_array(uint64_t field_val64, uint32_t *field_val)
{
    if (field_val == NULL) {
        return SHR_E_PARAM;
    }
    field_val[0] = field_val64 & 0xffffffff;
    field_val[1] = (field_val64 >> 32) & 0xffffffff;
    return SHR_E_NONE;
}

int
bcmcth_uint32_array_to_uint64(uint32_t *field_val, uint64_t *field_val64)
{
    if (field_val == NULL || field_val64 == NULL) {
        return SHR_E_PARAM;
    }
    *field_val64 = (((uint64_t)field_val[1]) << 32) | field_val[0];
    return SHR_E_NONE;
}

