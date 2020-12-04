/*! \file bcmltx_l3uc_ecmp_or_nhop_transform.c
 *
 * NHOP_ID/ECMP_ID Transform Handler
 *
 * This file contains field transform information for
 * ECMP_NHOP field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_l3uc_ecmp_or_nhop.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

/*
 * \brief bcmltx_l3uc_ecmp_or_nhop_transform is forward transform for ECMP or NHOP ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              ECMP_NHOP/NHOP_ID/ECMP_ID
 * \param [out] out             L3_ENTRY_IPV4_UNICAST.ECMP/NEXT_HOP_INDEX fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_ecmp_or_nhop_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t ECMP_NHOP_field_val = 0;
    uint64_t NHOP_ID_field_val = 0;
    uint64_t ECMP_ID_field_val = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_l3uc_ecmp_or_nhop_transform\n")));

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ECMP_NHOP_field_val = in->field[0]->data;
    NHOP_ID_field_val   = in->field[1]->data;
    ECMP_ID_field_val   = in->field[2]->data;

    /* First  reverse argument[0] is always ECMP flag.
     * Second reverse argument[1] is NEXT_HOP_INDEX.
     */
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = ECMP_NHOP_field_val;
    out->field[1]->id   = arg->rfield[1];
    if (ECMP_NHOP_field_val) {
        if (NHOP_ID_field_val != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        out->field[1]->data = ECMP_ID_field_val;
    } else {
        if (ECMP_ID_field_val != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        out->field[1]->data = NHOP_ID_field_val;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief bcmltx_l3uc_ecmp_or_nhop_rev_transform is reverse transform for ECMP or NHOP ID
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              L3_ENTRY_IPV4_UNICAST/ECMP/NEXT_HOP_INDEX
 * \param [out] out             ECMP_NHOP/NHOP_ID/ECMP_ID fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_l3uc_ecmp_or_nhop_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint64_t ECMP_NHOP_field_val = 0;
    uint64_t ECMP_NHOP_ID_field_val = 0;

    SHR_FUNC_ENTER(unit);

    if (in->count != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* 1 indicates ECMP index, 0 indicates NEXT_HOP index. */
    ECMP_NHOP_field_val = in->field[0]->data;
    ECMP_NHOP_ID_field_val = in->field[1]->data;

    /* First  reverse argument[0] is always ECMP_NHOP.
     * Second reverse argument[1] is alwasy NHOP_ID.
     * Third  reverse argument[2] is alwasy ECMP_ID.
     */
    out->field[0]->id = arg->rfield[0];
    out->field[0]->data = ECMP_NHOP_field_val;
    if (!ECMP_NHOP_field_val) {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = ECMP_NHOP_ID_field_val;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = 0;
    } else {
        out->field[1]->id = arg->rfield[1];
        out->field[1]->data = 0;
        out->field[2]->id = arg->rfield[2];
        out->field[2]->data = ECMP_NHOP_ID_field_val;
    }

exit:
    SHR_FUNC_EXIT();
}

