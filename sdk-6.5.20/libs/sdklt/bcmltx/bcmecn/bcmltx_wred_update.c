/*! \file bcmltx_wred_update.c
 *
 * ECN_WRED_UPDATE Transform Handler
 *
 * This file contains field/index transform information for ECN_PROTOCOL
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmecn/bcmltx_wred_index.h>
#include <bcmltx/bcmecn/bcmltx_packet_cng_validate.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECN


/*!
 * \brief ecn index transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 *
 *
 * This transform is used to derive index to update internal
 * congestion mapping from packet color, traffic managers congestion
 * status and incoming internal congestion notification.
 */
int
bcmltx_wred_index_transform(int unit,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_transform_arg_t *arg)

{
    uint32_t tm_cng = 0, cng = 0;
    uint32_t index_shift = 0;
    uint32_t int_cn = 0;
    uint32_t idx = 0;
    uint32_t i = 0, field_count = 0;
    bcmltd_field_t **flist = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmecn_wred_index_transform\n")));

    field_count = in->count;
    flist = in->field;

    /* No input, no output */
    if (field_count == 0) {
        SHR_EXIT();
    }

    /* Input Data validation. */
    if (field_count != 3) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The argument count should be 3.\n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Retrieve input info from buffers */
    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  " The input buffers can not be NULL.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        switch (flist[i]->id) {
        case ECN_WRED_UPDATEt_INT_ECN_CNGf:
            int_cn = flist[i]->data;
            break;
        case ECN_WRED_UPDATEt_TM_CONGESTIONf:
            tm_cng = flist[i]->data;
            break;
        case ECN_WRED_UPDATEt_PACKET_CNGf:
            cng = flist[i]->data;
            break;
        default:
            break;
        }
    }

    /*
     * Set up index transform.
     * Below is the index key in hardware
     *  [INT_CN, CCBE__CE_GREEN, CCBE__CE_YELLOW, CCBE__CE_RED, MPB__CNG]
 */
    if (cng == BCM_XGS_COLOR_RED) {
        index_shift = 2;
    } else if (cng == BCM_XGS_COLOR_YELLOW ) {
        index_shift = 3;
    } else if (cng == BCM_XGS_COLOR_GREEN ) {
        index_shift = 4;
    }

    /* Now set up the index */
    idx = ((int_cn << 5) | (tm_cng << index_shift) | cng);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Index generated for hw table is %d.\n"), idx));


    /* Add information back to output buffers. */
    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = idx;
    out->count++;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ecn index reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 *
 * This reverse transform is used to convert the hardware index to internal
 * congestion mapping table back to packet color, traffic managers congestion
 * status and incoming internal congestion notification.
 */
int
bcmltx_wred_index_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t tm_cng = 0, cng = 0;
    uint32_t index_shift = 0;
    uint32_t int_cn = 0;
    uint32_t idx = 0;
    uint32_t i = 0;
    uint32_t fid = 0;
    uint32_t field_count = 0;
    bcmltd_field_t **flist = NULL;
    const bcmltd_field_desc_t *finfo = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    field_count = in->count;

    /* No input, no output */
    if (field_count == 0) {
        SHR_EXIT();
    }

    /* Input Data validation. */
    if (field_count != 1) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The argument count should be 1.\n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (in->field[0] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The input buffers can not be NULL.\n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((out == NULL) || (arg == NULL) || (arg->rfield_list == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, " The output buffers can not be NULL.\n")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Retrieve input info (index) from buffers */
    idx = in->field[0]->data;

    /* Convert index into the key fields. */
    cng = (idx & 0x3);
    int_cn = ((idx >> 5) & 0x3);

    if (cng == BCM_XGS_COLOR_RED) {
        index_shift = 2;
    } else if (cng == BCM_XGS_COLOR_YELLOW ) {
        index_shift = 3;
    } else if (cng == BCM_XGS_COLOR_GREEN ) {
        index_shift = 4;
    }
    tm_cng = ((idx >> index_shift) & 0x1);

    flist = out->field;
    finfo = arg->rfield_list->field_array;
    field_count = arg->rfield_list->field_num;

    out->count = 0;
    for (i = 0; i < field_count; i++) {
        if (flist[i] == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, " The output buffers can not be NULL.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fid = finfo[i].field_id;
        switch (fid) {
        case ECN_WRED_UPDATEt_INT_ECN_CNGf:
            flist[i]->id = ECN_WRED_UPDATEt_INT_ECN_CNGf;
            flist[i]->data = int_cn;
            out->count++;
            break;
        case ECN_WRED_UPDATEt_PACKET_CNGf:
            flist[i]->id = ECN_WRED_UPDATEt_PACKET_CNGf;
            flist[i]->data = cng;
            out->count++;
            break;
        case ECN_WRED_UPDATEt_TM_CONGESTIONf:
            flist[i]->id = ECN_WRED_UPDATEt_TM_CONGESTIONf;
            flist[i]->data = tm_cng;
            out->count++;
            break;
        default:
            break;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_packet_cng_validate(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg)
{
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id == ECN_WRED_UPDATEt_PACKET_CNGf) {
            /*
             * PACKET_CNG = 2 is an invalid input.
             * This value is not supported.
             */
            if (in->field[i]->data == 2) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
