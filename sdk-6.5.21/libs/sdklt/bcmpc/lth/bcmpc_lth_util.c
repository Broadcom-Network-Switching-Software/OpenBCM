/*! \file bcmpc_lth_util.c
 *
 * BCMPC Logical Table Handler (LTH) Utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>

#include <bcmpc/bcmpc_lth_internal.h>

/*******************************************************************************
 * Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LTH


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Convert the union value bcmlt_field_data_t to uint64_t.
 *
 * \param [in] width Field width in bits.
 * \param [in] field_data Field map data which includes the max and min field
 *                        value in bcmlt_field_data_t.
 * \param [out] min Minimum field value in uint64_t.
 * \param [out] max Maximum field value in uint64_t.
 */
static void
lth_field_data_to_u64(uint32_t width, const bcmlrd_field_data_t *field_data,
                      uint64_t *min, uint64_t *max)
{
    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
    }
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_lth_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                    uint64_t *fval)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    *fval = 0;

    for (i = 0; i < in->count; i++) {
        if (fid == in->field[i]->id) {
            *fval = in->field[i]->data;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                uint64_t *min, uint64_t *max)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    lth_field_data_to_u64(field_md->width, field_data, min, max);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_field_validate(int unit, uint32_t sid, const bcmltd_field_t *field)
{
    uint32_t fid;
    uint64_t fval, min = 0, max = 0;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    SHR_IF_ERR_EXIT
        (bcmpc_lth_field_value_range_get(unit, sid, fid, &min, &max));

    if (fval < min || fval > max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lth_fields_validate(int unit, uint32_t sid, const bcmltd_fields_t *in)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        SHR_IF_ERR_EXIT
            (bcmpc_lth_field_validate(unit, sid, in->field[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

