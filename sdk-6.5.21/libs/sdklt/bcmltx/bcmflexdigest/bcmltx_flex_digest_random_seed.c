/*! \file bcmltx_flex_digest_random_seed.c
 *
 * Flex Digest Random Seed Transform Handler.
 *
 * The handler maps the logical table entry ID to the correct set of
 * hardware register.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcmflexdigest/bcmltx_flex_digest_random_seed.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_FLEXDIGEST

/*
 * \brief FLEX_DIGEST_NORM_PROFILE_SEED transform.
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
bcmltx_flex_digest_random_seed_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t field_count = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_flex_digest_random_seed_transform"
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
         *  transform table index. in data is 0 or 1 for table index
         */
        out->field[out->count]->idx  = 0;
        out->field[out->count]->id   = arg->rfield[i];
        out->field[out->count]->data = in->field[i]->data;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief FLEX_DIGEST_NORM_PROFILE_SEED reverse transform.
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
bcmltx_flex_digest_random_seed_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t field_count = 0;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_flex_digest_random_seed_rev_transform"
                            "\n")));

    field_count = in->count;

    if (field_count == 0) {
        SHR_FUNC_EXIT();
    }

    if (arg->rfield == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    for (i = 0; i < field_count; i++) {
        /*
         *  transform table index. in data is 0 or 1 for table index
         */
        out->field[out->count]->idx  = 0;
        out->field[out->count]->id   = arg->rfield[i];
        out->field[out->count]->data = in->field[i]->data;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}
