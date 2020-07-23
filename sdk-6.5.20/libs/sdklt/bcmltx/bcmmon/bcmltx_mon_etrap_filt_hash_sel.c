/*! \file bcmltx_mon_etrap_filt_hash_sel.c
 *
 * MON_ETRAP_CANDIDATE_FILTER Transform Handler
 *
 * This file contains field/index transform information
 * for MON_ETRAP_CANDIDATE_FILTER.
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbol_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltx/bcmmon/bcmltx_mon_etrap_filt_hash_sel.h>

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMLTX_MON

/*!
 * \brief ETRAP filter hash fields transform
 *
 * This function transforms the hash selection
 * array into individual HW fields.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_mon_etrap_filt_hash_sel_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    int         fld_count = 0;
    uint32_t    idx = 0;
    uint32_t    alloc_sz = 0;
    uint32_t    *data = NULL;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fld_count  = arg->value[0];

    alloc_sz = (sizeof(uint32_t) * fld_count);
    SHR_ALLOC(data, alloc_sz, "bcmltxMonEtrapFldList");
    sal_memset(data, 0, alloc_sz);

    for (idx = 0; idx < in->count; idx++) {
        data[idx] = in->field[idx]->data;
    }

    for (idx = 0; idx < out->count; idx++) {
        out->field[idx]->id = arg->rfield[idx];
        out->field[idx]->data = data[idx];
    }

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief ETRAP filter hsah fields rev transform
 *
 * This function transforms the HE fields into
 * ETRAP hash function selector array.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_mon_etrap_filt_hash_sel_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    int      fld_count = 0;
    uint32_t idx = 0;
    uint32_t alloc_sz = 0;
    uint32_t fid = 0;
    uint32_t *data = NULL;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fld_count  = arg->value[0];
    fid = arg->rfield[0];

    alloc_sz = (sizeof(uint32_t) * fld_count);
    SHR_ALLOC(data, alloc_sz, "bcmltxMonEtrapFldList");
    sal_memset(data, 0, alloc_sz);

    for (idx = 0; idx < in->count; idx++) {
        data[idx] = in->field[idx]->data;
    }

    for (idx = 0; idx < out->count; idx++) {
        out->field[idx]->id = fid;
        out->field[idx]->idx = idx;
        out->field[idx]->data = data[idx];
    }

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}
