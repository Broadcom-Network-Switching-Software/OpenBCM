/*! \file bcmltx_ctr_eflex_quant_cfg_obj_set.c
 *
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * Transform handler file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmctreflex/bcmltx_ctr_eflex_quant_cfg_obj_set.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*
 * \brief
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * forward transform
 *
 * Transform direction is logical to physical.
 *
 * In the field array BANK_OBJECT, the array index is the index of PT field
 * BANK_x_OBJECT.
 * There are N output fields, where N is the x in BANK_x_OBJECT of
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.
 *
 * This transform simply copies the input fields from the field array to
 * the physical table field corresponding to the index.
 */
int
bcmltx_ctr_eflex_quant_cfg_obj_set_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    size_t i;

    /* The x in the PT fields BANK_x_OBJECT. */
    uint32_t pt_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (i = out->count = 0; i < in->count; i++) {
        pt_idx = in->field[i]->idx;
        if (pt_idx < arg->rfields) {
            out->field[out->count]->id = arg->rfield[pt_idx];
            /* Get physical object id. */
            out->field[out->count]->data = bcmcth_ctr_eflex_obj_id_get(
                                              unit,
                                              0,
                                              in->field[i]->data);
            out->count++;
        } else {
            rv = SHR_E_FAIL;
            break;
        }
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * reverse transform
 *
 * Transform direction is physical to logical.
 *
 * In the field array BANK_OBJECT, the array index is the index of PT field
 * BANK_x_OBJECT.
 * There are N output fields, where N is the x in BANK_x_OBJECT of
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.
 *
 * This transform simply copies the input fields from the field array
 * to the logical table field corresponding to the index.
 */
int
bcmltx_ctr_eflex_quant_cfg_obj_set_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    /* The index of LT BANK_OBJECT field array. */
    size_t lt_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    for (lt_idx = out->count = 0; lt_idx < in->count; lt_idx++) {
        if (lt_idx < arg->fields) {
            out->field[out->count]->id = arg->rfield[0];
            out->field[out->count]->idx = lt_idx;
            /* Get logical object id. */
            out->field[out->count]->data = bcmcth_ctr_eflex_logical_obj_id_get(
                                              unit,
                                              0,
                                              in->field[lt_idx]->data);
            out->count++;
        } else {
            rv = SHR_E_FAIL;
            break;
        }
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();

}
