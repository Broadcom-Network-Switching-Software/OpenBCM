/*! \file bcmltx_l3mc_rpa_array_to_bitmap_transform.c
 *
 * RPA Array Transform Handler
 *
 * This file contains field transform information for
 * RPA_ID_PROFILE field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_l3mc_rpa_array_to_bitmap.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3
#define L3_MC_BIT_GET(_val, _pos) ((_val >> _pos) & 1)

/*
 * \brief bcmltx_l3mc_rpa_array_to_bitmap_transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              RPA Array.
 * \param [out] out             RPA_ID_PROFILE fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_rpa_array_to_bitmap_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    uint32_t cnt, idx;
    uint64_t one = 1;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l3mc_rpa_array_to_bitmap_transform\n")));
    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    out->count = 0;
    /*
     * Program RPA[] to RPA_ID_PROFILE Bitmap
     */
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->field[out->count]->data = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        /* Transform the array into bitmap */
        if ((in->field[cnt]->id == arg->field[0]) &&
             in->field[cnt]->data == 1) {
            idx = in->field[cnt]->idx;
            out->field[out->count]->data |= (one << idx);
        }
    }
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmltx_l3mc_rpa_array_to_bitmap_rev_transform is reverse transform for
 * rpa array to bitmap transform function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              RRPA_ID_PROFILE fields.
 * \param [out] out             PA Array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3mc_rpa_array_to_bitmap_rev_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                              const bcmltd_transform_arg_t *arg)
{
    int idx;

    SHR_FUNC_ENTER(unit);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l3mc_rpa_array_to"
                            "_bitmap_rev_transform\n")));
    out->count = 0;
    for (idx = 0; idx < RPA_PROFILE_ID_LEN; idx++) {
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->idx  = idx;
        out->field[out->count]->data = L3_MC_BIT_GET(in->field[0]->data, idx);
        out->count ++;
    }
    SHR_FUNC_EXIT();
}
