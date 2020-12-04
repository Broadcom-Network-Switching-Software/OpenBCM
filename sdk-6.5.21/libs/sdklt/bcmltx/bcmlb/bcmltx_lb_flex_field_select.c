/*! \file bcmltx_lb_flex_field_select.c
 *
 * Load balance hashing Flex Field Select Transform Handler.
 * transform for FLEX_FIELD_SELECT which converts logical array
 * HASH0/1_BIN_FLEX_FIELD_SELECT[] to a physical ENABLE_FLEX_FIELD_3_A/B.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmlb/bcmltx_lb_flex_field_select.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_LB

/*
 * \brief LB_FLEX_FIELD_SELECT transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_flex_field_select_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t   i = 0;
    uint32_t field_count = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_lb_flex_field_select_transform"
                            "\n")));

    field_count = in->count;

    if (field_count == 0) {
        SHR_FUNC_EXIT();
    }

    if (arg->rfield == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Update with user value */
    out->count = 0;
    for (i = 0; i < field_count; i++) {
        /*
         * The input array will be the list of the LT fields, and we only
         * handle LB_HASH_BINS_ASSIGNMENTt_HASH0_BIN_FLEX_FIELD_SELECTf/
         * LB_HASH_BINS_ASSIGNMENTt_HASH1_BIN_FLEX_FIELD_SELECTf.
         */
        out->field[out->count]->idx  = 0;
        out->field[out->count]->id	 = arg->rfield[i];
        out->field[out->count]->data = in->field[i]->data;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_FLEX_FIELD_SELECT reverse transform.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_flex_field_select_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    size_t   i = 0;
    uint32_t field_count = 0;
    uint32_t field_id = 0;
    uint8_t  num_hw_fields = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_lb_flex_field_select_rev_transform"
                            "\n")));

    field_count = in->count;

    if (field_count == 0) {
        SHR_FUNC_EXIT();
    }

    if ((arg->rfield == NULL) || (arg->values == 0) ||
        (arg->value == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num_hw_fields = arg->value[0];
    if (num_hw_fields == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Update with H/W values */
    out->count = 0;
    for (i = 0; i < field_count; i++) {
        if (i >= num_hw_fields) {
            field_id = 1;
            out->field[out->count]->id = arg->rfield[field_id];
            out->field[out->count]->idx  = (i % num_hw_fields);
            out->field[out->count]->data = in->field[i]->data;
            out->count++;
        } else {
            field_id = 0;
            out->field[out->count]->id = arg->rfield[field_id];
            out->field[out->count]->idx  = i;
            out->field[out->count]->data = in->field[i]->data;
            out->count++;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
