/*! \file bcmltx_member_cnt_val.c
 *
 * Mirror Member Counter Validation function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmmirror/bcmltx_member_cnt_val_validate.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_member_cnt_val_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_field_val_arg_t *arg)
{
    uint32_t is_trunk = 0;
    uint32_t member_cnt = 1;
    size_t i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        switch (in->field[i]->id) {
            case MIRROR_ING_INSTANCEt_IS_TRUNKf:
                is_trunk = in->field[i]->data & 0xFFFFFFFF;
                break;
            case MIRROR_ING_INSTANCEt_MEMBER_CNTf:
                member_cnt = in->field[i]->data & 0xFFFFFFFF;
                break;
            default:
                break;
        }
    }

    /* member cnt can be only 1 at this moment with IS_TRUNK=0 */
    if ((is_trunk == 0) && (member_cnt != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

