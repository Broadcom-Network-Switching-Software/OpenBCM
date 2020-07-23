/*! \file bcmltx_port_policy_drop_on_pri.c
 *
 * PORT_POLICY.DROP_ON_PRI Transform Handler
 *
 * This file contains field transform information for PORT_POLICY.DROP_ON_PRI.
 *
 * Element 'N' of PORT_POLICY.DROP_ON_PRI array corresponds to the field
 * arg->rfield[0](forward transform), arg->field[N](reverse transform).
 * PORT_POLICY.DROP_ON_PRI index should be
 * between 0 and arg->rfields - 1 (forward transform)
 * between 0 and arg->fields - 1 (reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_policy_drop_on_pri.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

#define MAX_PRI_CNT 8

/*
 * \brief PORT_POLICY.DROP_ON_PRI array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_POLICY.DROP_ON_PRI field array.
 * \param [out] out             PORT_TAB.PROHIBITED_DOT1P fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_port_policy_drop_on_pri_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    uint32_t priority;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_policy_drop_on_pri_transform\n")));

    if (in->count <= 0) {
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = 0;
    for (i = 0; i < in->count; i++) {
        priority = in->field[i]->idx;
        out->field[out->count]->data |= in->field[i]->data << priority;
    }
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief PORT_POLICY.DROP_ON_PRI array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              PORT_TAB/PROHIBITED_DOT1P fields.
 * \param [out] out             PORT_POLICY.DROP_ON_PRI field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_port_policy_drop_on_pri_rev_transform(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg)
{
    size_t idx;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmltx_port_policy_drop_on_pri_rev_transform\n")));

    for (idx = out->count = 0; idx < MAX_PRI_CNT; idx++) {
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->idx  = idx;
        out->field[out->count]->data = (in->field[0]->data >> idx) & 1;
        out->count++;
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
