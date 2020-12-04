/*! \file bcmcth_info_tbl_util.c
 *
 * BCMCTH information table utility API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_flex_qos_drv.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_info_table_entry.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_FLEX_QOS

/*!
 * \brief Free info table buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] in   Data pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to free required resource.
 */
static
int info_tbl_flds_free(int unit, bcmltd_fields_t *in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate info table buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] in   Data pointer.
 * \param [in] num_fields Number of fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resource.
 */
static int
info_tbl_flds_allocate(int unit, bcmltd_fields_t *in, size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    SHR_ALLOC(in->field, sizeof(bcmltd_field_t *) * num_fields, "bcmcthInfoTbl");

    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);

    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthInfoTbl");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    info_tbl_flds_free(unit, in);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Load data into info table at init time.
 *
 * \param [in] unit Unit number.
 * \param [in] data data buffer that holds information.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resource.
 */
int
info_tbl_init(int unit, bcmcth_info_table_t *data, bool warm)
{
    bcmltd_fields_t info_lt_flds = {0};
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fields = 0;
    size_t f_cnt;
    size_t ix, jx;
    size_t enum_cnt;
    size_t policy_entry_cnt;

    size_t profile_cnt;
    bcmlrd_sid_t table_id;
    bcmcth_info_table_enum_entry_t *entry;
    bcmcth_info_table_scalar_entry_t *scalar_entry;
    bcmcth_info_table_policy_entry_t *policy_entry;

    uint32_t enum_value;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_attributes_get(unit,
                                     (const char *)&data->table_name[0],
                                     &t_attrib));

    num_fields = t_attrib.number_of_fields;

    if (data == NULL) {
        return SHR_E_MEMORY;
    }

    profile_cnt = data->profile_cnt;
    enum_cnt = data->enum_cnt;

    /*
     * We need give each array entry an allocation
     * Policy field is enum_array with NPL determined usage. So total field needed
     * is TOTAL_FIELD - 1.
     */
    if (data->policy_info == NULL) {
        policy_entry_cnt = 0;
        SHR_IF_ERR_EXIT
            (info_tbl_flds_allocate(unit,
                                    &info_lt_flds,
                                    num_fields));
    } else {
        policy_entry_cnt = data->policy_info[0].policy_size.value;
        SHR_IF_ERR_EXIT
            (info_tbl_flds_allocate(unit,
                                    &info_lt_flds,
                                    (num_fields + data->max_policy_supported -1)));
    }

    /* Populate ingress pool LT. */
    for (ix = 0; ix < profile_cnt; ix++) {
        entry = data->enum_info + ix * enum_cnt;

        table_id = entry->table_id;

        for (f_cnt = 0; f_cnt < enum_cnt; f_cnt++) {
            /* Decode enum ascii value to number */
            SHR_IF_ERR_EXIT
                (bcmlrd_field_symbol_to_value(unit,
                                              entry->table_id,
                                              entry->field_id,
                                              entry->value,
                                              &enum_value));

            /* Load data to info struct */
            info_lt_flds.field[f_cnt]->id = entry->field_id;
            info_lt_flds.field[f_cnt]->data = enum_value;

            /* Advance pointer */
            entry++;
        }

        if (policy_entry_cnt != 0) {
            /* Policy array size field first */
            scalar_entry = &data->policy_info[ix].policy_size;
            info_lt_flds.field[f_cnt]->id = scalar_entry->field_id;
            info_lt_flds.field[f_cnt]->data = scalar_entry->value;

            f_cnt++;
            policy_entry = &data->policy_info[ix].policy_field;

            for (jx = 0; jx < data->max_policy_supported; jx++) {
                SHR_IF_ERR_EXIT
                    (bcmlrd_field_symbol_to_value(unit,
                                                  policy_entry->table_id,
                                                  policy_entry->field_id,
                                                  policy_entry->value[jx],
                                                  &enum_value));

                /* Load each data array as a regular field with idx  */
                info_lt_flds.field[f_cnt]->id = policy_entry->field_id;
                info_lt_flds.field[f_cnt]->data = enum_value;
                info_lt_flds.field[f_cnt]->idx = jx;
                f_cnt++;
            }
        }

        info_lt_flds.count = f_cnt;
        if (!warm) {
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit,
                                     table_id,
                                     &info_lt_flds));
        }
    }

exit:
    info_tbl_flds_free(unit, &info_lt_flds);

    SHR_FUNC_EXIT();
}

