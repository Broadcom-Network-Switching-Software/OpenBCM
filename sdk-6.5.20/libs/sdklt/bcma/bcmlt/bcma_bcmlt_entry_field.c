/*! \file bcma_bcmlt_entry_field.c
 *
 * Functions based on bcmlt library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_pb_format.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/*
 * Return the matching qualifier enum struct in the user-specified string.
 * The qualifier is only supported in LT traverse operations.
 */
static const bcma_cli_parse_enum_t *
trvs_qualifier(bcma_bcmlt_entry_info_t *ei, char *str)
{
    const bcma_cli_parse_enum_t *e = bcma_bcmlt_field_qualifier_enum_get();

    /* Only check for LT traverse opcode */
    if (ei->pt || ei->opcode != BCMLT_OPCODE_TRAVERSE) {
        return NULL;
    }

    while (e->name != NULL) {
        if (sal_strstr(str, e->name)) {
            return e;
        }
        e++;
    }

    return NULL;
}

/*
 * Return the qualifier string pointer from the user-specified string.
 * The qualifier is only supported in LT traverse operations.
 */
static char *
trvs_qualifier_str(bcma_bcmlt_entry_info_t *ei, char *field_arg)
{
    char *ptr;
    const bcma_cli_parse_enum_t *e = bcma_bcmlt_field_qualifier_enum_get();

    /* Only check for LT traverse opcode */
    if (ei->pt || ei->opcode != BCMLT_OPCODE_TRAVERSE) {
        return NULL;
    }

    while (e->name != NULL) {
        if ((ptr = sal_strstr(field_arg, e->name)) != NULL) {
            return ptr;
        }
        e++;
    }

    return NULL;
}

int
bcma_bcmlt_entry_field_get(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field, bool ignore_not_found)
{
    int rv;

    if (!ei || !ei->eh || !field) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;
        uint32_t idx, sz = 0;

        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_array_symbol_get(ei->eh, field->name, 0,
                                                    arr->u.strs, arr->size,
                                                    &sz);
        } else {
            rv = bcmlt_entry_field_array_get(ei->eh, field->name, 0,
                                             arr->u.vals, arr->size, &sz);
        }
        /*
         * Mark all array elements as valid for scalar array and
         * symbol strings array.
         */
        if (field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            for (idx = 0; idx < sz; idx++) {
                arr->elements[idx] = true;
            }
        }
    } else {
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_symbol_get(ei->eh, field->name,
                                              &field->u.str);
        } else {
            rv = bcmlt_entry_field_get(ei->eh, field->name, &field->u.val);
        }
    }

    if (ignore_not_found && rv == SHR_E_NOT_FOUND) {
        /* Clean up the field */
        bcma_bcmlt_field_array_free(field);
        field->name = NULL;
        return 0;
    }
    if (SHR_FAILURE(rv)) {
        cli_out("%sFail to get field %s value: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, field->name, BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }
    return 0;
}

int
bcma_bcmlt_entry_field_add(bcma_bcmlt_entry_info_t *ei,
                           bcma_bcmlt_field_t *field)
{
    int rv = 0;

    if (!ei || !ei->eh || !field) {
        return -1;
    }

    if (BCMA_BCMLT_FIELD_IS_ARRAY(field)) {
        bcma_bcmlt_field_array_t *arr = field->u.array;

        /* Sanity check */
        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE &&
            field->flags & BCMA_BCMLT_FIELD_F_ARRAY) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Scalar array and wide field can not be "
                                "suppported in a single field.\n")));
            return -1;
        }

        if (field->flags & BCMA_BCMLT_FIELD_F_WIDE) {
            /* Wide field */
            rv = bcmlt_entry_field_array_add(ei->eh, field->name, 0,
                                             arr->u.vals, arr->size);
        } else {
            /* Scalar array or symbol strings array */
            int idx, sidx, cnt;

            /* Add all valid elements of array field to entry. */
            for (idx = 0; idx < arr->size; idx++) {
                if (!arr->elements[idx]) {
                    continue;
                }

                /*
                 * Get a continuous range set of valid elements
                 * in the array.
                 */
                sidx = idx;
                do {
                    idx++;
                } while (idx < arr->size && arr->elements[idx]);
                cnt = idx - sidx;

                /* Set the range of field array data to entry */
                if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
                    rv = bcmlt_entry_field_array_symbol_add(ei->eh,
                                                            field->name, sidx,
                                                            &arr->u.strs[sidx],
                                                            cnt);
                } else {
                    rv = bcmlt_entry_field_array_add(ei->eh,
                                                     field->name, sidx,
                                                     &arr->u.vals[sidx],
                                                     cnt);
                }

                if (SHR_FAILURE(rv)) {
                    cli_out("%sFail to add array field %s value: "
                            BCMA_BCMLT_ERRMSG_FMT"\n",
                            BCMA_CLI_CONFIG_ERROR_STR, field->name,
                            BCMA_BCMLT_ERRMSG(rv));
                    return -1;
                }
            }
        }
    } else {
        /* Non-array field */
        if (field->flags & BCMA_BCMLT_FIELD_F_SYMBOL) {
            rv = bcmlt_entry_field_symbol_add(ei->eh, field->name,
                                              field->u.str);
        } else {
            rv = bcmlt_entry_field_add(ei->eh, field->name, field->u.val);
        }
    }

    if (SHR_FAILURE(rv)) {
        cli_out("%sFail to add field %s value: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, field->name, BCMA_BCMLT_ERRMSG(rv));
        return -1;
    }

    return 0;

}

int
bcma_bcmlt_entry_field_parse_qualifier(bcma_bcmlt_entry_info_t *ei,
                                       char *field_arg)
{
    char *ptr;
    const bcma_cli_parse_enum_t *e = bcma_bcmlt_field_qualifier_enum_get();

    /* Only check for LT traverse opcode */
    if (ei->pt || ei->opcode != BCMLT_OPCODE_TRAVERSE) {
        return 0;
    }

    while (e->name != NULL) {
        if ((ptr = sal_strstr(field_arg, e->name)) != NULL) {
            if (ptr == field_arg) {
                return -1;
            }
            ptr += sal_strlen(e->name);
            if (*ptr == '\0') {
                return -1;
            }
            break;
        }
        e++;
    }

    return e->val;
}

int
bcma_bcmlt_entry_field_parse(bcma_bcmlt_entry_info_t *ei,
                             bcma_bcmlt_field_t *field, char *field_arg)
{
    int idx;
    const bcma_cli_parse_enum_t *qualifier;
    char *ptr;

    if (!ei || !field || !field_arg) {
        return -1;
    }

    /* Validate field name */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcmlt_field_def_t *df = &ei->def_fields[idx];
        int len = sal_strlen(df->name);

        if (sal_strncasecmp(field_arg, df->name, len) == 0) {
            /*
             * Might be one of the valid field assignment:
             * field=val
             * field[r]=v,...
             * Or might be qualifiers for LT traverse (==, !=, >=, <=, >, <)
             */
            qualifier = trvs_qualifier(ei, &field_arg[len]);
            if (qualifier != NULL || field_arg[len] == '=' ||
                (df->depth > 0 && field_arg[len] == '[')) {
                if (bcma_bcmlt_field_init(field, df) != 0) {
                    return -1;
                }
                field->qlfr = qualifier;
                break;
            }
        }
    }

    /* Return error if can not find a matching default field name */
    if (idx == ei->num_def_fields) {
        ptr = trvs_qualifier_str(ei, field_arg);
        if (ptr == NULL) {
            ptr = sal_strchr(field_arg, '=');
        }

        if (ptr) {
            *ptr = '\0';
        }
        cli_out("%sField %s is not valid for table %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, field_arg, ei->name);
        return -1;
    }

    /* Parse and validate field data */
    if (bcma_bcmlt_field_parse(field_arg, field) < 0) {
        cli_out("%sFail to parse field %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, field_arg);
        return -1;
    }

    return 0;
}

int
bcma_bcmlt_entry_qlfr_fields_create(bcma_bcmlt_entry_info_t *ei,
                                         uint32_t num_fields)
{
    bcma_bcmlt_field_t *fields = NULL;
    int szfields = num_fields;

    if (ei == NULL) {
        return -1;
    }

    fields = sal_alloc(sizeof(bcma_bcmlt_field_t) * szfields, "bcmaLtQFields");
    if (fields == NULL) {
        return -1;
    }
    sal_memset(fields, 0, sizeof(bcma_bcmlt_field_t) * szfields);

    if (ei->qlfr_fields) {
        bcma_bcmlt_entry_qlfr_fields_destroy(ei);
    }
    ei->qlfr_fields = fields;
    ei->szqlfr_fields = szfields;
    /* Clear the in-use fields count. */
    ei->num_qlfr_fields = 0;

    return 0;
}

int
bcma_bcmlt_entry_qlfr_fields_destroy(bcma_bcmlt_entry_info_t *ei)
{
    int idx;

    if (ei == NULL || ei->qlfr_fields == NULL) {
        return -1;
    }

    for (idx = 0; idx < ei->szqlfr_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->qlfr_fields[idx];
        bcma_bcmlt_field_array_free(field);
    }
    sal_free(ei->qlfr_fields);
    ei->qlfr_fields = NULL;
    ei->szqlfr_fields = 0;
    ei->num_qlfr_fields = 0;

    return 0;
}

int
bcma_bcmlt_entry_fields_create(bcma_bcmlt_entry_info_t *ei,
                               uint32_t num_fields)
{
    bcma_bcmlt_field_t *fields = NULL;
    int szfields = num_fields;

    if (ei == NULL) {
        return -1;
    }
    /* Always create the array size at least the default fields count. */
    if (szfields < ei->num_def_fields) {
        return -1;
    }

    /*
     * Key fields might be added internally for auto-fill key fields option.
     * Enlarge the array size with key fields number for this feature.
     */
    szfields += ei->num_key_fields;

    fields = sal_alloc(sizeof(bcma_bcmlt_field_t) * szfields, "bcmaLtFields");
    if (fields == NULL) {
        return -1;
    }
    sal_memset(fields, 0, sizeof(bcma_bcmlt_field_t) * szfields);

    if (ei->fields) {
        bcma_bcmlt_entry_fields_destroy(ei);
    }
    ei->fields = fields;
    ei->szfields = szfields;
    /* Clear the in-use fields count. */
    ei->num_fields = 0;

    return 0;
}

int
bcma_bcmlt_entry_fields_destroy(bcma_bcmlt_entry_info_t *ei)
{
    int idx;

    if (ei == NULL || ei->fields == NULL) {
        return -1;
    }

    for (idx = 0; idx < ei->szfields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];
        bcma_bcmlt_field_array_free(field);
    }
    sal_free(ei->fields);
    ei->fields = NULL;
    ei->szfields = 0;
    ei->num_fields = 0;

    return 0;
}
