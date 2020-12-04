/*! \file bcmltx_port_type_to_ctrl.c
 *
 * This file contains field transform information for Port.
 *
 * Transform handler for port type to port control
 * conversion.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_type_to_ctrl.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT
#define MIN_PORT_TYPE 0
#define MAX_PORT_TYPE 2

/*
 * \brief Forward transform for port type to
 *        port control conversion.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              in fields list array.
 * \param [out] out             out fields list array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_type_to_ctrl_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint8_t  port_ctrl[1] = {0};
    int      port_type = -1;

    SHR_FUNC_ENTER(unit);

    port_type = in->field[0]->data;

    if ((port_type < MIN_PORT_TYPE) ||
        (port_type > MAX_PORT_TYPE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMLTD_BIT_SET(port_ctrl, port_type);

    out->count = 0;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->field[out->count++]->data = port_ctrl[0];

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Reverse transform for port type to
 *        port control conversion.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              in fields list array.
 * \param [out] out             out fields list array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmltx_port_type_to_ctrl_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    uint8_t  port_ctrl[1] = {0};
    int      port_type = 0;
    uint8_t  array_idx = 0;

    SHR_FUNC_ENTER(unit);

    port_ctrl[0] = in->field[0]->data;

    for (array_idx = MIN_PORT_TYPE;
         array_idx < MAX_PORT_TYPE;
         array_idx++) {
        if ((bool)BCMLTD_BIT_GET(port_ctrl, array_idx)) {
           port_type = array_idx;
           break;
        }
    }

    out->count = 0;
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->idx = 0;
    out->field[out->count++]->data = port_type;

    SHR_FUNC_EXIT();
}

