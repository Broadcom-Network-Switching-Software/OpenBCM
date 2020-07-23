/*! \file bcmltx_lb_output_select_port_index.c
 *
 * Load balance hashing logical port id to hardware table index transform.
 *
 * This file contains field transform information for LB_HASH port based
 * output select logical port to hardware table index transform.
 *
 * HARDWARE_TABLE.__INDEX_= Logical Port ID + offset.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmlb/bcmltx_lb_output_select_port_index.h>
#include <bcmlrd/bcmlrd_types.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT

/*
 * \brief LB_OUTPUT_SELECT_PORT_INDEX transform.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Logical PORT_ID.
 *                              field array.
 * \param [out] out             Hardware_table.__INDEX.
 *                              fields.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_output_select_port_index_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t "
                            "bcmltx_lb_output_select_port_index_transform"
                            "\n")));
    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = 0;
    if (in->count != 1) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }

    if ((arg->values != 1) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Multiple LB_HASH tables share the same transform function with one input
     * parameter, so it is safe to do the addition work here.
     */
    out->field[out->count]->data = in->field[0]->data + arg->value[0];
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_OUTPUT_SELECT_PORT_INDEX reverse transform.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              Hardware_table.__INDEX.
 *                              fields.
 * \param [out] out             Logical PORT_ID.
 *                              field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_output_select_port_index_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t "
                            "bcmltx_lb_output_select_port_index_rev_transform"
                            "\n")));
    out->count = 0;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = 0;
    if (in->count != 1) {
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }

    if ((arg->values != 1) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t reverse transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Multiple LB_HASH tables share the same transform function with one input
     * parameter, so it is safe to do the addition work here.
     */
    out->field[out->count]->data = in->field[0]->data - arg->value[0];
    out->count++;

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}
