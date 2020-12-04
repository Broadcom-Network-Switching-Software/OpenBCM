/*! \file bcmltx_l2_opaque_tag_id.c
 *
 * L2_OPAQUE_TAG Logic table fields Transform Handlers.
 * This file contains field transform information for
 * opaque tag id in L2_OPAQUE_TAG Logic table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml2/bcmltx_l2_opaque_tag_id.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_L2

/******************************************************************************
* Private functions
 */

/******************************************************************************
* Public functions
 */
/*!
 * \brief opaque tag id transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            L2_OPAQUE_TAG_ID field.
 * \param [out] out           __TABLE_SEL fields.
 * \param [in]  arg           Transform arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_l2_opaque_tag_id_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcml2_opaque_tag_id_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->field[count]->data = in->field[0]->data;
    out->field[count]->id = arg->rfield[count];

    out->count = ++count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief opaque tag id reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            __INDEX/__TABLE_SEL fields.
 * \param [out] out           L2_OPAQUE_TAG_ID field.
 * \param [in]  arg           Transform arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_l2_opaque_tag_id_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcml2_opaque_tag_id_rev_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->field[count]->data = in->field[count]->data;
    out->field[count]->id = arg->rfield[count];

    out->count = ++count;

exit:
    SHR_FUNC_EXIT();
}
