/*! \file bcmltx_vlan_stg.c
 *
 * STG Transform Handler
 *
 * This file contains field transform information for STG.
 *
 * Element 'N' of the state array corresponds to the field
 * arg->rfield[N](forward transform), arg->field[N](reverse transform).
 * State index should be between 0 and arg->rfields - 1 (forward transform)
 * between 0 and arg->fields -1 (reverse transform).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltx/bcmvlan/bcmltx_vlan_stg.h>
#include <bcmltx/bcmvlan/bcmltx_vlan_stg_a.h>
#include <bcmltx/bcmvlan/bcmltx_vlan_stg_b.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_VLAN

/*
 * \brief STG state array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              STATE field array.
 * \param [out] out             STG_VLAN/EGR_STG_VLAN fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * There is a single input STG state field array. The array idx of the
 * field is effectively a port number. There are N output fields,
 * where N is the number of ports, and each output field corresponds
 * to the port STG state. This transform simply copies the input
 * fields from the field array to the physical table field
 * corresponding to a port.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_stg_transform(int unit,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_transform_arg_t *arg)
{
    size_t i; /* logical in field index */
    size_t port; /* physical out field index, which is essentially port */
    int rv = SHR_E_NONE;
    bcmltd_sid_t sid;
    uint32_t idx_count;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_stg_transform\n")));

    sid = arg->comp_data->sid;
    idx_count = bcmlrd_field_idx_count_get(unit, sid, arg->field[0]);

    for (i = out->count = 0; i < in->count; i++) {
        port = in->field[i]->idx;
        /* The number of rfields is equal to the number of ports */
        if (port < arg->rfields) {
            out->field[out->count]->id = arg->rfield[port];
            out->field[out->count]->data = in->field[i]->data;
            out->count++;
        } else {
            /* Port limit exceeded */
            if (port >= idx_count) {
                rv = SHR_E_FAIL;
                break;
            }
        }
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief STG state array reverse transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              STG_VLAN/EGR_STG_VLAN fields.
 * \param [out] out             STATE field array.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is physical to logical.
 *
 * There are N input fields, where N is the number of ports, and each
 * input field corresponds to the STG state of a port. There is a
 * single output STG state field array. The array idx of the field is
 * effectively a port number.  This transform simply copies the input
 * physical table field corresponding to a port to the field array.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_stg_rev_transform(int unit,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_transform_arg_t *arg)
{
    size_t port; /* physical out field index */
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_rev_stg_transform\n")));

    for (port = out->count = 0; port < in->count; port++) {
        /* The number of fields is equal to the number of ports */
        if (port < arg->fields) {
            out->field[out->count]->id = arg->rfield[0];
            out->field[out->count]->idx = port;
            out->field[out->count]->data = in->field[port]->data;
            out->count++;
        } else {
            /* port limit exceeded */
            rv = SHR_E_FAIL;
            break;
        }
    }

    SHR_ERR_EXIT(rv);
 exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief STG state array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              STATE field array.
 * \param [out] out             STG_TAB_A/EGR_VLAN_STG_A fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * There is a single input STG state field array. The array idx of the
 * field is effectively a port number. There are N output fields, where
 * N is the number of the first part of ports, and each output field
 * corresponds to the port STG state. This transform simply copies the
 * input fields from the field array to the physical table field
 * corresponding to a port.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_stg_a_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    bcmltd_sid_t sid;
    uint32_t idx_count, i, ridx;
    uint32_t port, end_port;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_stg_a_transform\n")));

    sid = arg->comp_data->sid;
    idx_count = bcmlrd_field_idx_count_get(unit, sid, arg->field[0]);

    out->count = 0;
    end_port = arg->rfields;
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id != VLAN_STGt_STATEf) {
            continue;
        }

        port = in->field[i]->idx;

        if (port < end_port) {
            ridx = port;
            out->field[out->count]->id = arg->rfield[ridx];
            out->field[out->count]->data = in->field[i]->data;
            out->count++;
        } else {
            if (port >= idx_count) {
                rv = SHR_E_PARAM;
                break;
            }
        }
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief STG state array forward transform
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              STATE field array.
 * \param [out] out             STG_TAB_B/EGR_VLAN_STG_B fields.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * There is a single input STG state field array. The array idx of the
 * field is effectively a port number. There are N output fields, where
 * N is the number of the second part of ports, and each output field
 * corresponds to the port STG state. This transform simply copies the
 * input fields from the field array to the physical table field
 * corresponding to a port.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

int
bcmltx_vlan_stg_b_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    bcmltd_sid_t sid;
    uint32_t idx_count, i, ridx;
    uint32_t start_port, end_port, port;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t bcmvlan_stg_b_transform\n")));

    sid = arg->comp_data->sid;
    idx_count = bcmlrd_field_idx_count_get(unit, sid, arg->field[0]);

    out->count = 0;
    start_port = idx_count - arg->rfields;
    end_port = start_port + arg->rfields;
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id != VLAN_STGt_STATEf) {
            continue;
        }

        port = in->field[i]->idx;

        if (port >= start_port && port < end_port) {
            ridx = port - start_port;
            out->field[out->count]->id = arg->rfield[ridx];
            out->field[out->count]->data = in->field[i]->data;
            out->count++;
        } else {
            if (port >= idx_count) {
                rv = SHR_E_PARAM;
                break;
            }
       }
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}
