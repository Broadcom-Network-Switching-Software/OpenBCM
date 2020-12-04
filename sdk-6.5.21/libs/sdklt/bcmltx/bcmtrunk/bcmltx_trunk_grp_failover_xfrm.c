/*! \file bcmltx_trunk_grp_failover_xfrm.c
 *
 * This file contains transform functions for
 * TRUNK_FAILOVER LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtrunk/bcmltx_trunk_grp_failover_add.h>

/* BSL Module TBD */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief TRUNK_FAILOVER entry forward transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Forward transform to map the TRUNK_FAILOVER.FAILOVER_PORT_SYSTEM
 * array to physical table field list
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_grp_failover_add_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg)
{
    uint8_t failover_cnt = 0;
    uint8_t array_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /*Get the failover count */
    if (in->field[0]->id != TRUNK_FAILOVERt_FAILOVER_CNTf) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }

    failover_cnt = in->field[0]->data;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->data = (failover_cnt - 1);
    out->field[out->count]->idx = 0;
    out->count++;

    for (array_idx = 1; array_idx < in->count; array_idx++) {
        out->field[out->count]->id = arg->rfield[out->count];
        out->field[out->count]->idx = 0;
        out->field[out->count]->data = in->field[array_idx]->data;
        out->count++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_FAILOVER entry reverse transform.
 *
 * \param [in]  unit   Unit number.
 * \param [in]  in     input field list.
 * \param [out] out    output field list.
 * \param [in]  arg    handler arguments.
 *
 * Reverse transform to map the physical table field list to
 * TRUNK_FAILOVER.FAILOVER_PORT_SYSTEM array.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_trunk_grp_failover_add_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg)
{
    uint8_t failover_cnt = 0;
    uint8_t array_idx = 0;
    uint8_t idx = 0;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 0;

    /* Get the failover count */
    failover_cnt = in->field[0]->data;
    out->field[out->count]->id  = arg->rfield[0];
    out->field[out->count]->data = (failover_cnt + 1);
    out->field[out->count]->idx  = 0;
    out->count++;

    for (idx = 1, array_idx = 0; idx < in->count; idx++, array_idx++) {
        out->field[out->count]->id = arg->rfield[1];
        out->field[out->count]->idx = array_idx;
        out->field[out->count]->data = in->field[out->count]->data;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}
