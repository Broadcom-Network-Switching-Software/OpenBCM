
/*! \file bcmfp_cth_common.c
 *
 * APIs that are common to all different FP LTs.
 *
 * This file contains functions to get values
 * of scalar, array, symbol type LT fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_util_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_fid_value_get(int unit,
                    uint32_t fid,
                    void *fid_values,
                    uint64_t *fid_value)
{
    bcmltd_field_t *buff = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid_values, SHR_E_PARAM);
    SHR_NULL_CHECK(fid_value, SHR_E_PARAM);

    buff = (bcmltd_field_t *)fid_values;
    do {
        if (buff->id == fid) {
            *fid_value = buff->data;
            break;
        }
        buff = buff->next;
    } while (buff != NULL);

    if (buff == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
  SHR_FUNC_EXIT();
}

int
bcmfp_fid_array_value_get(int unit,
                          uint32_t fid,
                          uint8_t index,
                          void *fid_values,
                          uint64_t *fid_value)
{
    bcmltd_field_t *buff = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid_values, SHR_E_PARAM);
    SHR_NULL_CHECK(fid_value, SHR_E_PARAM);

    buff = (bcmltd_field_t *)fid_values;
    do {
        if (buff->id == fid &&
            buff->idx == index) {
            *fid_value = buff->data;
            break;
        }
        buff = buff->next;
    } while (buff != NULL);

    if (buff == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
  SHR_FUNC_EXIT();
}

int
bcmfp_tbl_field_value_get(int unit,
                          uint64_t field_data,
                          uint32_t field_width,
                          uint32_t field_idx,
                          uint32_t *field_value)
{
    uint16_t remaining_bits = 0;
    uint8_t width = 0;
    uint8_t idx = 0;
    uint32_t *u32_value = NULL;
    uint32_t u32_lower = 0;
    uint32_t u32_upper = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field_value, SHR_E_PARAM);

    if (field_width <= 64) {
        field_idx = 0;
    }

    remaining_bits =
        (field_width - (field_idx * 64));
    if (remaining_bits < 64) {
        width = (remaining_bits % 64);
    } else {
        width = 64;
    }

    idx = (field_idx * 2);
    u32_value = &(field_value[idx]);

    u32_lower = field_data & 0xFFFFFFFF;
    u32_upper = ((field_data >> 32) & 0xFFFFFFFF);
    if (width <= 32) {
        u32_value[0] = u32_lower;
    } else if (width <= 64) {
        u32_value[0] = u32_lower;
        u32_value[1] = u32_upper;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_value_is_partial(int unit,
                           uint32_t width,
                           uint64_t u64_value,
                           bool *is_partial)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(is_partial, SHR_E_PARAM);

    if (width == 1) {
       *is_partial = FALSE;
    } else {
       *is_partial = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_imm_entry_lookup(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       uint16_t num_fid,
                       bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       bcmfp_ltd_buffers_t *buffers)
{
    size_t size = 0;
    uint16_t idx = 0;
    bcmltd_field_t *non_default_data = NULL;
    bcmltd_field_t *temp = NULL;
    uint16_t num_non_default_data = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(key_value, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    size = sizeof(bcmltd_field_t);
    in->field = buffers->key_fields;
    in->count = num_keys;
    for (idx = 0; idx < num_keys; idx++) {
         sal_memset(in->field[idx], 0, size);
         in->field[idx]->id = key_fid[idx];
         in->field[idx]->data = key_value[idx];
    }

    out->count = num_fid;
    out->field = buffers->data_fields;
    size = (num_fid * sizeof(bcmltd_field_t));
    sal_memset(out->field[0], 0, size);

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, tbl_id, in, out));
    /*
     * out->count will be updated by bcmimm_entry_lookup
     * to number of output fields found in IMM.
     */
    if (out->count == 0) {
        SHR_EXIT();
    }

    /* Keep all the output fields in linkedlist. */
    if (out->count == 1) {
        out->field[0]->next = NULL;
    } else {
        for (idx = 0; idx < (out->count - 1); idx++) {
             out->field[idx]->next = out->field[idx + 1];
        }
    }

    /* Unlink default fields. */
    SHR_IF_ERR_EXIT
        (bcmfp_ltd_non_default_data_get(unit,
                                        tbl_id,
                                        out->field[0],
                                        &non_default_data,
                                        buffers));
    temp = non_default_data;
    num_non_default_data = 0;
    idx = 0;
    size = sizeof(bcmltd_field_t);
    while (temp != NULL) {
        sal_memcpy(out->field[idx], temp, size);
        num_non_default_data++;
        idx++;
        temp = temp->next;
    }

    out->count = num_non_default_data;
    if (num_non_default_data == 0) {
        SHR_EXIT();
    }

    if (num_non_default_data == 1) {
        out->field[0]->next = NULL;
    } else {
        for (idx = 0; idx < (num_non_default_data - 1); idx++) {
             out->field[idx]->next = out->field[idx + 1];
        }
        out->field[idx]->next = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_imm_entry_insert(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys)
{
    uint8_t idx = 0;
    size_t size = 0;
    bcmltd_fields_t *insert = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(key_value, SHR_E_PARAM);

    SHR_ALLOC(insert, sizeof(bcmltd_fields_t),
              "bcmfpImmInsertField");
    sal_memset(insert, 0, sizeof(bcmltd_fields_t));

    size = num_keys * sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(insert->field, size,
                "bcmfpImmUpdateFieldArray");

    for (idx = 0; idx < num_keys; idx++) {

        /* Fill the KEY field info */
        size = sizeof(bcmltd_field_t);
        insert->field[idx] = shr_fmm_alloc();
        if (insert->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert->count += 1;
            sal_memset(insert->field[idx], 0, size);
            insert->field[idx]->id = key_fid[idx];
            insert->field[idx]->data = key_value[idx];
        }
    }

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, tbl_id, insert));
exit:
    if (insert != NULL) {
        if (insert->field != NULL) {
            for (idx = 0; idx < insert->count; idx++) {
                shr_fmm_free(insert->field[idx]);
            }
            SHR_FREE(insert->field);
        }
        SHR_FREE(insert);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_imm_entry_delete(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys)
{
    uint8_t idx = 0;
    size_t size = 0;
    bcmltd_fields_t *delete = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(key_value, SHR_E_PARAM);

    SHR_ALLOC(delete, sizeof(bcmltd_fields_t),
              "bcmfpImmDeleteField");
    sal_memset(delete, 0, sizeof(bcmltd_fields_t));

    size = num_keys * sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(delete->field, size,
                "bcmfpImmUpdateFieldArray");

    for (idx = 0; idx < num_keys; idx++) {
        /* Fill the KEY field info */
        size = sizeof(bcmltd_field_t);
        delete->field[idx] = shr_fmm_alloc();
        if (delete->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            delete->count += 1;
            sal_memset(delete->field[idx], 0, size);
            delete->field[idx]->id = key_fid[idx];
            delete->field[idx]->data = key_value[idx];
        }
    }

    SHR_IF_ERR_EXIT
        (bcmimm_entry_delete(unit, tbl_id, delete));
exit:
    if (delete != NULL) {
        if (delete->field != NULL) {
            for (idx = 0; idx < delete->count; idx++) {
                shr_fmm_free(delete->field[idx]);
            }
            SHR_FREE(delete->field);
        }
        SHR_FREE(delete);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_imm_entry_update(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       bcmltd_fid_t *data_fid,
                       uint32_t *data_value,
                       uint8_t num_data)
{
    uint8_t idx = 0;
    size_t size = 0;
    bcmltd_fields_t *update = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(key_value, SHR_E_PARAM);
    SHR_NULL_CHECK(data_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(data_value, SHR_E_PARAM);

    SHR_ALLOC(update, sizeof(bcmltd_fields_t),
              "bcmfpImmUpdateField");
    sal_memset(update, 0, sizeof(bcmltd_fields_t));

    size = (num_keys + num_data) * sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(update->field, size,
                "bcmfpImmUpdateFieldArray");

    /* Fill the KEY field info */
    for (idx = 0; idx < num_keys; idx++) {
        size = sizeof(bcmltd_field_t);
        update->field[idx] = shr_fmm_alloc();
        if (update->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            update->count += 1;
            sal_memset(update->field[idx], 0, size);
            update->field[idx]->id = key_fid[idx];
            update->field[idx]->data = key_value[idx];
        }
    }

    /* Fill the DATA field info */
    for (idx = num_keys; idx < (num_data + num_keys); idx++) {
        update->field[idx] = shr_fmm_alloc();
        if (update->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            update->count += 1;
            sal_memset(update->field[idx], 0, size);
            update->field[idx]->id = data_fid[idx - num_keys];
            update->field[idx]->data = data_value[idx - num_keys];
        }
    }

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, TRUE, tbl_id, update));
exit:
    if (update != NULL) {
        if (update->field != NULL) {
            for (idx = 0; idx < update->count; idx++) {
                shr_fmm_free(update->field[idx]);
            }
            SHR_FREE(update->field);
        }
        SHR_FREE(update);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_imm_entry_update_array(int unit,
                       bcmltd_sid_t tbl_id,
                       bcmltd_fid_t *key_fid,
                       uint32_t *key_value,
                       uint8_t num_keys,
                       bcmltd_fid_t data_fid,
                       uint32_t *data_value,
                       uint8_t num_data)
{
    uint8_t idx = 0;
    size_t size = 0;
    bcmltd_fields_t *update = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_fid, SHR_E_PARAM);
    SHR_NULL_CHECK(key_value, SHR_E_PARAM);
    SHR_NULL_CHECK(data_value, SHR_E_PARAM);

    SHR_ALLOC(update, sizeof(bcmltd_fields_t),
              "bcmfpImmUpdateField");
    sal_memset(update, 0, sizeof(bcmltd_fields_t));

    size = (num_keys + num_data) * sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(update->field, size,
                "bcmfpImmUpdateFieldArray");

    /* Fill the KEY field info */
    for (idx = 0; idx < num_keys; idx++) {
        size = sizeof(bcmltd_field_t);
        update->field[idx] = shr_fmm_alloc();
        if (update->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            update->count += 1;
            sal_memset(update->field[idx], 0, size);
            update->field[idx]->id = key_fid[idx];
            update->field[idx]->data = key_value[idx];
        }
    }

    /* Fill the DATA field info */
    for (idx = num_keys; idx < (num_data + num_keys); idx++) {
        update->field[idx] = shr_fmm_alloc();
        if (update->field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            update->count += 1;
            sal_memset(update->field[idx], 0, size);
            update->field[idx]->id = data_fid;
            update->field[idx]->data = data_value[idx - num_keys];
            update->field[idx]->idx = (idx - num_keys);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, TRUE, tbl_id, update));
exit:
    if (update != NULL) {
        if (update->field != NULL) {
            for (idx = 0; idx < update->count; idx++) {
                shr_fmm_free(update->field[idx]);
            }
            SHR_FREE(update->field);
        }
        SHR_FREE(update);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_buffers_free(int unit,
                       bcmltd_fields_t *fields,
                       uint16_t num_fid)
{
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    if (fields == NULL) {
        SHR_EXIT();
    }
    if (fields->field == NULL) {
        SHR_EXIT();
    }

    for (idx = 0; idx < num_fid; idx++) {
        shr_fmm_free(fields->field[idx]);
    }

    SHR_FREE(fields->field);
    fields->field = NULL;
    fields->count = 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_fields_array_free(int unit,
                            bcmltd_field_t **fields,
                            uint16_t num_fid)
{
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    if (fields == NULL) {
        SHR_EXIT();
    }

    for (idx = 0; idx < num_fid; idx++) {
        shr_fmm_free(fields[idx]);
    }

    SHR_FREE(fields);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_fields_list_free(int unit,
                           bcmltd_field_t *fields)
{
    bcmltd_field_t *temp = NULL;

    SHR_FUNC_ENTER(unit);

    if (fields == NULL) {
        SHR_EXIT();
    }

    while (fields != NULL) {
        temp = fields;
        fields = fields->next;
        shr_fmm_free(temp);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_field_add(int unit,
                    bcmltd_fid_t fid,
                    uint32_t data,
                    bcmltd_fields_t *fields)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fields, SHR_E_PARAM);
    SHR_NULL_CHECK(fields->field, SHR_E_PARAM);
    SHR_NULL_CHECK(fields->field[fields->count], SHR_E_PARAM);

    fields->field[fields->count]->id = fid;
    fields->field[fields->count]->data = data;
    fields->field[fields->count]->idx = 0;
    fields->field[fields->count]->flags = 0;
    fields->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_updated_data_get(int unit,
                           bcmltd_sid_t tbl_id,
                           uint16_t num_fid,
                           bcmltd_field_t *key,
                           bcmltd_field_t *data,
                           bcmltd_fields_t *updated_data)
{
    bcmltd_fields_t in;
    bcmltd_fields_t imm;
    bcmltd_fields_t mix;
    bcmltd_field_t *temp = NULL;
    size_t size = 0;
    uint16_t idx = 0;
    uint64_t min = 0;
    uint64_t max = 0;
    uint64_t def = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&imm, 0, sizeof(bcmltd_fields_t));
    sal_memset(&mix, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(updated_data, SHR_E_PARAM);

    in.count = 0;
    in.field = NULL;
    size = sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(in.field, size, "bcmfpImmInFieldArrayUpdated");
    in.field[0] = key;
    in.count = 1;

    imm.count = 0;
    imm.field = NULL;
    size = (num_fid * sizeof(bcmltd_field_t *));
    BCMFP_ALLOC(imm.field, size, "bcmfpImmOutFieldArrayUpdated");
    size = sizeof(bcmltd_field_t);
    for (idx = 0; idx < num_fid; idx++) {
        imm.field[idx] = shr_fmm_alloc();
        if (imm.field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                 "out fields in BCMFP IMM lookup\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            sal_memset(imm.field[idx], 0, size);
        }
    }
    imm.count = num_fid;

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, tbl_id, &in, &imm));

    mix.count = 0;
    mix.field = NULL;
    size = (num_fid * sizeof(bcmltd_field_t *));
    BCMFP_ALLOC(mix.field, size, "bcmfpImmOutFieldArrayUpGet");
    size = sizeof(bcmltd_field_t);
    for (idx = 0; idx < num_fid; idx++) {
        mix.field[idx] = shr_fmm_alloc();
        if (mix.field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                 "updated fields in BCMFP IMM lookup\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            sal_memset(mix.field[idx], 0, size);
        }
    }

    /* Copy fields not present in data but in imm to updated. */
    for (idx = 0; idx < imm.count; idx++) {
        temp = data;
        while (temp != NULL) {
            if (imm.field[idx]->id == temp->id &&
                imm.field[idx]->idx == temp->idx) {
                break;
            }
            temp = temp->next;
        }
        if (temp != NULL) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmfp_fid_min_max_default_get(unit,
                                           tbl_id,
                                           imm.field[idx]->id,
                                           &min,
                                           &max,
                                           &def));
        /* If incoming LT field data matched with
         * default values of the LT field, ignore it.
         */
        if (imm.field[idx]->data == def) {
            continue;
        }
        sal_memcpy(mix.field[mix.count],
                   imm.field[idx],
                   sizeof(bcmltd_field_t));
        mix.field[mix.count]->next = NULL;
        mix.count++;
    }

    /* Copy non default fields present in data to updated. */
    temp = data;
    while (temp != NULL) {
        SHR_IF_ERR_EXIT
            (bcmfp_fid_min_max_default_get(unit,
                                           tbl_id,
                                           temp->id,
                                           &min,
                                           &max,
                                           &def));
        /* If incoming LT field data matched with
         * default values of the LT field, ignore it.
         */
        if (temp->data == def) {
            temp = temp->next;
            continue;
        }
        sal_memcpy(mix.field[mix.count],
                   temp,
                   sizeof(bcmltd_field_t));
        mix.field[mix.count]->next = NULL;
        mix.count++;
        temp = temp->next;
    }

    /* Keep all the updated fields in linkedlist. */
    if (mix.count != 0) {
       if (mix.count == 1) {
           mix.field[0]->next = NULL;
       } else {
           for (idx = 0; idx < (mix.count - 1); idx++) {
                mix.field[idx]->next = mix.field[idx + 1];
           }
       }
    }
    updated_data->field = mix.field;
    updated_data->count = mix.count;
exit:
    bcmfp_ltd_buffers_free(unit, &imm, num_fid);
    if (SHR_FUNC_ERR()) {
        bcmfp_ltd_buffers_free(unit, &mix, num_fid);
    }
    SHR_FREE(in.field);
    SHR_FUNC_EXIT();
}

int
bcmfp_ltd_non_default_data_get(int unit,
                               bcmltd_sid_t tbl_id,
                               bcmltd_field_t *data,
                               bcmltd_field_t **non_default_data,
                               bcmfp_ltd_buffers_t *buffers)
{
    size_t size = 0;
    bcmltd_field_t *temp = NULL;
    uint64_t min = 0;
    uint64_t max = 0;
    uint64_t def = 0;
    uint16_t idx = 0;
    bcmltd_field_t **non_def_data_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(non_default_data, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    *non_default_data = NULL;
    if (data == NULL) {
        SHR_EXIT();
    }

    if (buffers->non_def_data_fields_1_in_use) {
        non_def_data_ptr = buffers->non_def_data_fields_2;
    } else {
        non_def_data_ptr = buffers->non_def_data_fields_1;
        buffers->non_def_data_fields_1_in_use = true;
    }

    temp = data;
    size = sizeof(bcmltd_field_t);
    while (temp != NULL) {
        /* Get the LT field LRD attributes. */
        SHR_IF_ERR_EXIT
            (bcmfp_fid_min_max_default_get(unit,
                                           tbl_id,
                                           temp->id,
                                           &min,
                                           &max,
                                           &def));
        /* If incoming LT field data matched with
         * default values of the LT field, ignore it.
         */
        if (temp->data == def) {
            temp = temp->next;
            continue;
        }

        sal_memcpy(non_def_data_ptr[idx], temp, size);
        if (idx > 0) {
           non_def_data_ptr[idx-1]->next = non_def_data_ptr[idx];
        }
        temp = temp->next;
        idx++;
    }
    /*
     * There are no data fields with non default values
     * assigned to it.
     */
    if (idx == 0) {
        SHR_EXIT();
    }

    /* Set the last non default fields next to NULL */
    non_def_data_ptr[idx-1]->next = NULL;

    /* Assign head of the linked list to non_default_data */
    *non_default_data = non_def_data_ptr[0];
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_fid_value_u32_get(int unit,
             bcmltd_field_t *buffer,
             uint32_t id_fid,
             uint32_t *id)
{
    uint64_t id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             id_fid,
                             (void *)buffer,
                             &id_u64),
         SHR_E_NOT_FOUND);

    *id = id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}
