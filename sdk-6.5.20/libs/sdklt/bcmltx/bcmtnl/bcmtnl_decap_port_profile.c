/*! \file bcmtnl_decap_port_profile.c
 *
 * TNL_DECAP_PORT_PROFILE Transform Handler
 *
 * This file contains field/index transform information
 * for TNL_DECAP_PORT_PROFILE.
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
#include <bcmltx/bcmtnl/bcmltx_tnl_decap_port_profile.h>

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMLTX_TNL

/*!
 * \brief TNL decap port transform
 *
 * This function transforms DECAP_PORT list
 * into portbitmap.
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
bcmltx_tnl_decap_port_profile_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    int         idx = 0, port_count = 0;
    int         fld_count = 0, out_idx = 0;
    uint32_t    port = 0;
    uint64_t    pbmp = 0;
    uint32_t    alloc_sz = 0;
    uint32_t    fld_sz = 0;
    uint64_t     *data = NULL;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    port_count = in->count;
    fld_count  = arg->value[0];
    fld_sz     = arg->value[1];

    alloc_sz = (sizeof(uint64_t) * fld_count);
    SHR_ALLOC(data, alloc_sz, "bcmltxTnlDecapFldList");
    sal_memset(data, 0, alloc_sz);

    for (idx = 0; idx < port_count; idx++) {
        if (in->field[idx]->data == 0) {
            continue;
        } else {
            port = idx;
        }
        pbmp = 1;
        pbmp = (pbmp << (port % fld_sz));
        out_idx = (port / fld_sz);
        data[out_idx] |= pbmp;
    }

    for (idx = 0; idx < fld_count; idx++) {
        out->field[idx]->id = arg->rfield[idx];
        out->field[idx]->data = data[idx];
    }
    out->count = fld_count;

exit:
    SHR_FREE(data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TNL decap port rev transform
 *
 * This function transforms DECAP port pbmp
 * into port list.
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
bcmltx_tnl_decap_port_profile_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    int      idx = 0, count = 0;
    bool     member = false;
    uint32_t fid = 0, out_idx = 0;
    uint32_t fld_sz = 0, arr_idx = 0;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);

    if ((arg->values == 0) || (arg->value == NULL) ||
        (arg->comp_data == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    count = in->count;
    fld_sz = arg->value[1];
    fid = arg->rfield[0];

    for (idx = 0; idx < count; idx++) {
        data = in->field[idx]->data;
        for (out_idx = 0; out_idx < fld_sz; out_idx++) {
            member = (((data >> out_idx) & 0x1) ? true : false);
            arr_idx = ((idx * fld_sz) + out_idx);
            out->field[arr_idx]->id = fid;
            out->field[arr_idx]->idx = arr_idx;
            out->field[arr_idx]->data = member;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
