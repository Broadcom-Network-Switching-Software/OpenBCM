/*! \file bcmltx_member_cnt_set.c
 *
 * Member Count Set Transform Handler.
 * transform for MEMBER_CNT which converts a logical min/max range
 * to a physical range.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltx/bcmmirror/bcmltx_member_cnt_set.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

/*
 * xform logical min/max range of 1 to 8 to a physical range of 0 to 7
 */
#define BCMLTX_MEMBER_CNT_SET_MIN (1)
#define BCMLTX_MEMBER_CNT_SET_MAX (8)

/* Member Count equals to IM/EM_MTP_INDEX.MEMBER_CNT + 1 */
int
bcmltx_member_cnt_set_rev_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    uint32_t member_cnt, cnt;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mem_cnt_set_rev_transform \n")));

    /* Sanity check inputs, outputs, parameters */
    if ((NULL == in) ||
        (NULL == out) ||
        (NULL == arg)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    member_cnt = 0;
    for (cnt = 0; cnt < in->count; cnt++) {
        if (in->field[cnt]->id == arg->field[0]) {
            /* Physical value range check */
            if (in->field[cnt]->data >= BCMLTX_MEMBER_CNT_SET_MAX) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Execute transform */
            member_cnt = in->field[cnt]->data + 1;

            out->field[0]->data = member_cnt;
            out->field[0]->id   = arg->rfield[0];
            out->field[0]->idx  = 0;
            out->count++;
            break;
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/* IM/EM_MTP_INDEX.MEMBER_CNT equals to Member Count - 1 */
int
bcmltx_member_cnt_set_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t member_cnt, cnt;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mem_cnt_set_transform \n")));

    /* Sanity check inputs, outputs, parameters */
    if ((NULL == in) ||
        (NULL == out) ||
        (NULL == arg)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;
    member_cnt = 0;

    for (cnt = 0; cnt < in->count; cnt++) {
        if (in->field[cnt]->id == MIRROR_ING_INSTANCEt_MEMBER_CNTf) {
            /* Logical value range check */
            if (!((in->field[cnt]->data >= BCMLTX_MEMBER_CNT_SET_MIN) &&
                  (in->field[cnt]->data <= BCMLTX_MEMBER_CNT_SET_MAX))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Execute transform */
            member_cnt = in->field[cnt]->data - 1;

            out->field[0]->data = member_cnt;
            out->field[0]->id   = arg->rfield[0];
            out->field[0]->idx  = 0;
            out->count++;
            break;
        }
    }
    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}
