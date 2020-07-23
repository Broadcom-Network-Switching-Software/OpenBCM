/*! \file bcmcth_lb_hash_info.c
 *
 * Load balance hash information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmcth/bcmcth_lb_drv.h>
#include <bcmcth/bcmcth_lb_hash_info.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LB

/******************************************************************************
 * Local definitions
 */

/* BCMCTH load balance hash information */
static bcmcth_lb_hash_info_t bcmcth_lb_hash_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmcth_lb_hash_info_init(int unit)
{
    /* Get device-specific data */
    return bcmcth_lb_hash_info_get(unit, &bcmcth_lb_hash_info[unit]);
}

int
bcmcth_lb_hash_info_validate(int unit,
                             bcmlt_opcode_t opcode,
                             const bcmltd_fields_t *in,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if (opcode != BCMLT_OPCODE_LOOKUP) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    /* Read only table insertion is not allowed */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_lookup(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    size_t count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Number of sub-field for non-concatenate mode. */
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NUM_NONCONCAT_SUB_FIELDf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].num_nonconcat_sub_field;
    out->field[count]->idx = 0;
    count++;

    /* Array of sub-field widths (bits) for non-concatenate mode. */
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[0];
    out->field[count]->idx = 0;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[1];
    out->field[count]->idx = 1;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[2];
    out->field[count]->idx = 2;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[3];
    out->field[count]->idx = 3;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[4];
    out->field[count]->idx = 4;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[5];
    out->field[count]->idx = 5;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[6];
    out->field[count]->idx = 6;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NONCONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].nonconcat_sub_field_width[7];
    out->field[count]->idx = 7;
    count++;

    /* Number of sub-field for concatenate mode. */
    out->field[count]->id = LB_HASH_DEVICE_INFOt_NUM_CONCAT_SUB_FIELDf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].num_concat_sub_field;
    out->field[count]->idx = 0;
    count++;

    /* Array of sub-field widths (bits) for concatenate mode. */
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[0];
    out->field[count]->idx = 0;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[1];
    out->field[count]->idx = 1;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[2];
    out->field[count]->idx = 2;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[3];
    out->field[count]->idx = 3;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[4];
    out->field[count]->idx = 4;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[5];
    out->field[count]->idx = 5;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[6];
    out->field[count]->idx = 6;
    count++;
    out->field[count]->id = LB_HASH_DEVICE_INFOt_CONCAT_SUB_FIELD_WIDTHf;
    out->field[count]->data =
        bcmcth_lb_hash_info[unit].concat_sub_field_width[7];
    out->field[count]->idx = 7;
    count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    /* Read only table delete operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    /* Read only table update operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_first(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_lb_hash_info_lookup(unit, op_arg, out, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_lb_hash_info_next(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
