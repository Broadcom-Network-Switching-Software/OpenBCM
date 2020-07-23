/*! \file bcmltx_port_ing_block.c
 *
 * PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK Transform Handler
 *
 * This file contains field transform information for PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK.
 *
 * Element 'N' of SKIP_ING_PORT_BLOCK array corresponds to the field
 * arg->field[N](forward transform), arg->rfield[N](reverse transform).
 * SKIP_ING_PORT_BLOCK array index should be
 * between 0 and arg->fields - 1 (forward transform)
 * between 0 and arg->rfields - 1 (reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_ing_block.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

#define MAX_PORT_ING_BLOCK_NUM 4

/*
 * \brief PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK field array.
 * \param [out] out             PORT_TAB/OUTER_TPID_ENABLE fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_ing_block_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    uint32_t blk_idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_ing_block_transform\n")));

    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = 0;
    for (i = 0; i < in->count; i++) {
        blk_idx = in->field[i]->idx;
        out->field[out->count]->data |= in->field[i]->data << blk_idx;
    }
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB/OUTER_TPID_ENABLE fields.
 * \param [out] out             PORT_ING_BLOCK_CONTROL.SKIP_ING_PORT_BLOCK field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_ing_block_rev_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg)
{
    size_t idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_ing_block_rev_transform\n")));

    for (idx = out->count = 0; idx < MAX_PORT_ING_BLOCK_NUM; idx++) {
        out->field[out->count]->id = arg->rfield[0];
        out->field[out->count]->idx = idx;
        out->field[out->count]->data = (in->field[0]->data >> idx) & 1;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
