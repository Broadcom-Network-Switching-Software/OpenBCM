/*! \file bcmltx_vlan_assignment_vlan_range_vlan_id_min.c
 *
 * VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN Transform Handler
 *
 * This file contains field transform information for
 * VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN.
 *
 * Element 'N' of the VLAN_ID_MIN array corresponds to the field
 * arg->rfield[N](forward transform), arg->field[N](reverse transform).
 * State index should be between 0 and arg->rfields - 1 (forward transform)
 * between 0 and arg->fields -1 (reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmvlan/bcmltx_vlan_assignment_vlan_range_vlan_id_min.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_VLAN

/*
 * \brief VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              VLAN_ID_MIN field array.
 * \param [out] out             ING_VLAN_RANGE/VLAN_MIN_x x=[0,7] fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * In the field array VLAN_ID_MIN, the array idx of the
 * field is the index of min vlan id ranges. There are N output fields,
 * where N is the x in VLAN_MIN_x of ING_VLAN_RANGE.
 * This transform simply copies the input
 * fields from the field array to the physical table field
 * corresponding to the index.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_assignment_vlan_range_vlan_id_min_transform(int unit,
                                                        const bcmltd_fields_t *in,
                                                        bcmltd_fields_t *out,
                                                        const bcmltd_transform_arg_t *arg)
{
    size_t i; /* The index of configuration data. */
    uint32_t  pt_idx; /* The x in the physical fields VLAN_MIN_x. */
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_vlan_assignment_vlan_range_vlan_id_min_transform\n")));

    for (i = out->count = 0; i < in->count; i++) {
        pt_idx = in->field[i]->idx;
        if (pt_idx < arg->rfields) {
            out->field[out->count]->id = arg->rfield[pt_idx];
            out->field[out->count]->data = in->field[i]->data;
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
 * \brief VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              ING_VLAN_RANGE.VLAN_MIN_x fields.
 * \param [out] out             VLAN_ASSINGMENT_VLAN_RANGE.VLAN_ID_MIN field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * In the field array VLAN_ID_MIN, the array idx of the
 * field is the index of min vlan id ranges. There are N output fields,
 * where N is the x in VLAN_MIN_x of ING_VLAN_RANGE.
 * This transform simply copies the input
 * fields from the field array to the logical table field
 * corresponding to the index.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_assignment_vlan_range_vlan_id_min_rev_transform(int unit,
                                                            const bcmltd_fields_t *in,
                                                            bcmltd_fields_t *out,
                                                            const bcmltd_transform_arg_t *arg)
{
    size_t lt_idx; /* The index of logical table field array. */
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_vlan_assignment_vlan_range_vlan_id_min_rev_transform\n")));

    for (lt_idx = out->count = 0; lt_idx < in->count; lt_idx++) {
        if (lt_idx < arg->fields) {
            out->field[out->count]->id = arg->rfield[0];
            out->field[out->count]->idx = lt_idx;
            out->field[out->count]->data = in->field[lt_idx]->data;
            out->count++;
        } else {
            /* port limit exceeded */
            rv = SHR_E_FAIL;
            break;
        }
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
