/*! \file bcma_bcmlt_entry.c
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

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/*
 * bcma_bcmlt_tables_search callback function.
 * Allocate entry information and set it to zero.
 * Allocate fields array with number of default fields and set it to zero.
 * Make a local copy of the default fields array information to entry.
 */
static int
entry_calloc(void *cookie, const char *name,
             bcmlt_field_def_t *table_fields, uint32_t num_table_fields)
{
    uint32_t idx;
    bcma_bcmlt_entry_info_t **entry_info = (bcma_bcmlt_entry_info_t **)cookie;
    bcma_bcmlt_entry_info_t *ei = NULL;

    *entry_info = NULL;

    ei = sal_alloc(sizeof(*ei), "bcmaLtEntryInfo");
    if (ei == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Fail to allocate entry for table %s\n"), name));
        return 0;
    }
    sal_memset(ei, 0, sizeof(*ei));
    ei->name = name;
    ei->num_def_fields = num_table_fields;

    /* Get number for key fields. */
    for (idx = 0; idx < num_table_fields; idx++) {
        if (table_fields[idx].key) {
            ei->num_key_fields++;
        }
    }

    if (num_table_fields > 0) {
        int arr_size = sizeof(bcmlt_field_def_t) * num_table_fields;

        ei->def_fields = sal_alloc(arr_size, "bcmaLtDefFields");
        if (ei->def_fields == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Fail to allocate %d default fields "
                                "for table %s\n"),
                       (int)num_table_fields, name));
            sal_free(ei);
            return 0;
        }
        sal_memcpy(ei->def_fields, table_fields, arr_size);

        if (bcma_bcmlt_entry_fields_create(ei, num_table_fields) < 0) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Fail to allocate %d fields "
                                "for table %s\n"),
                       (int)num_table_fields, name));
            sal_free(ei->def_fields);
            sal_free(ei);
            return 0;
        }
    }

    *entry_info = ei;

    return 0;
}

/*
 * Function to create entry handle and fill entry information
 */
static bcma_bcmlt_entry_info_t *
entry_info_create(int unit, bcma_bcmlt_entry_info_t *ei,
                  bool physical, uint32_t attr,
                  bcma_bcmlt_commit_params_t *params)
{
    int rv;
    bcma_bcmlt_commit_params_t *commit_params;

    if (!ei) {
        return NULL;
    }

    rv = bcmlt_entry_allocate(unit, ei->name, &ei->eh);
    if (SHR_FAILURE(rv)) {
        bcma_bcmlt_entry_info_destroy(ei);
        return NULL;
    }

    ei->unit = unit;
    ei->pt = physical;
    ei->attr = attr;
    ei->num_commit_fields = -1;
    commit_params = &ei->commit_params;
    if (params) {
        sal_memcpy(commit_params, params, sizeof(*commit_params));
    } else {
        /* Use default parameters */
        commit_params->async = false;
        commit_params->notify = BCMLT_NOTIF_OPTION_HW;
        commit_params->priority = BCMLT_PRIORITY_NORMAL;
    }

    return ei;
}

/*
 * Function to get default fields information for a specified table.
 * The returned default fields structure should be freed by the caller.
 */
static int
default_entry_fields_get(int unit, const char *table_name,
                         bcmlt_field_def_t **fields, uint32_t *num_fields)
{
    int rv;
    bcmlt_field_def_t *fields_def = NULL;
    uint32_t num_fields_def = 0;

    /* Get actual number of default fields. */
    rv = bcmlt_table_field_defs_get(unit, table_name, 0, NULL, &num_fields_def);
    if (SHR_FAILURE(rv) || num_fields_def == 0) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit,
                              "Failed to get actual number of default fields "
                              "for table %s.\n"), table_name));
        return -1;
    }

    fields_def = sal_alloc(num_fields_def * sizeof(bcmlt_field_def_t),
                           "bcmaLtDefFields");
    if (fields_def == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit,
                              "Failed to allocate %d default fields"
                              "information of table %s.\n"),
                   (int)num_fields_def, table_name));
        return -1;
    }

    rv = bcmlt_table_field_defs_get(unit, table_name, num_fields_def,
                                    fields_def, NULL);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit,
                              "Failed to get default fields for table %s.\n"),
                   table_name));
        sal_free(fields_def);
        return -1;
    }

    *fields = fields_def;
    *num_fields = num_fields_def;

    return 0;
}

bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create_exact_name(int unit, const char *table_name,
                                        bool physical, uint32_t attr,
                                        bcma_bcmlt_commit_params_t *params,
                                        bcmlt_field_def_t *fields,
                                        uint32_t num_fields)
{
    bcma_bcmlt_entry_info_t *ei = NULL;
    bcmlt_field_def_t *fields_def = NULL;
    uint32_t num_fields_def = 0;

    if (table_name == NULL) {
        return NULL;
    }

    if (fields == NULL || num_fields == 0) {
        if (default_entry_fields_get(unit, table_name,
                                     &fields_def, &num_fields_def) < 0) {
            return NULL;
        }
        fields = fields_def;
        num_fields = num_fields_def;
    }
    entry_calloc(&ei, table_name, fields, num_fields);

    if (fields_def) {
        sal_free(fields_def);
    }

    return entry_info_create(unit, ei, physical, attr, params);
}

bcma_bcmlt_entry_info_t *
bcma_bcmlt_entry_info_create(int unit, const char *table_name,
                             bool physical, uint32_t attr,
                             bcma_bcmlt_commit_params_t *params,
                             bcmlt_field_def_t *fields, uint32_t num_fields)
{
    int rv;
    bcma_bcmlt_entry_info_t *ei = NULL;
    uint32_t search_scope = physical ? BCMA_BCMLT_SCOPE_F_PTBL:
                                       BCMA_BCMLT_SCOPE_F_LTBL;

    if (table_name == NULL) {
        return NULL;
    }

    if (fields == NULL) {
        rv = bcma_bcmlt_tables_search(unit, table_name, BCMA_BCMLT_MATCH_EXACT,
                                      search_scope, entry_calloc, &ei);
        if (rv <= 0) {
            cli_out("%sUnsupported %s table: %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    physical ? "physical" : "logical", table_name);
            return NULL;
        }
    } else {
        entry_calloc(&ei, table_name, fields, num_fields);
    }

    return entry_info_create(unit, ei, physical, attr, params);
}

int
bcma_bcmlt_entry_info_destroy(bcma_bcmlt_entry_info_t *ei)
{
    if (!ei) {
        return -1;
    }

    if (ei->eh) {
        bcmlt_entry_free(ei->eh);
    }

    if (ei->fields) {
        bcma_bcmlt_entry_fields_destroy(ei);
    }

    if (ei->def_fields) {
        sal_free(ei->def_fields);
    }

    if (ei->qlfr_fields) {
        bcma_bcmlt_entry_qlfr_fields_destroy(ei);
    }

    if (ei->ev_ctoks) {
        sal_free(ei->ev_ctoks);
    }
    if (ei->ev_arrayinfo) {
        sal_free(ei->ev_arrayinfo);
    }

    sal_free(ei);

    return 0;
}
