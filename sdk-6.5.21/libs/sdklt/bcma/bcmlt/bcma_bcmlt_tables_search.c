/*! \file bcma_bcmlt_tables_search.c
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
#include <sal/sal_mutex.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

static sal_mutex_t tsrch_mtx[BCMDRD_CONFIG_MAX_UNITS];

static bool initialized = false;

/*
 * This function checks if search name matches table name (case insensitive).
 * Return value is 1 if matched, otherwise 0.
 */
static int
table_match(const char *tname, const char *sname,
            bcma_bcmlt_match_mode_t match_mode)
{
    const char *str;

    if (tname == NULL || sname == NULL ||
        tname[0] == '\0' || sname[0] == '\0') {
        return 0;
    }

    if (match_mode == BCMA_BCMLT_MATCH_SUBSTR) {

        /* Match for all */
        if (sal_strcmp(sname, "*") == 0) {
            return 1;
        }

        switch (sname[0]) {
        case '^':
            /* Match string from start. */
            str = sname + 1;
            if (sal_strncasecmp(tname, str, sal_strlen(str)) == 0) {
                return 1;
            }
            break;
        case '*':
            str = sname + 1;
            /* Match string if ever occurred. */
            if (sal_strcasestr(tname, str) != NULL) {
                return 1;
            }
            break;
        case '@':
            str = sname + 1;
            /* Match string exactly */
            if (sal_strcasecmp(tname, str) == 0) {
                return 1;
            }
            break;
        default:
            /* Default match is a substring match */
            if (sal_strcasestr(tname, sname) != NULL) {
                return 1;
            }
            break;
        }
    } else {
        /* Exact match */
        if (sal_strcasecmp(tname, sname) == 0) {
            return 1;
        }
    }

    return 0;
}

int
bcma_bcmlt_tables_search(int unit, const char *search_name,
                         bcma_bcmlt_match_mode_t match_mode,
                         uint32_t search_scope,
                         bcma_bcmlt_table_info_f cb, void *cookie)
{
    int rv;
    char *table_name;
    bcmlt_field_def_t *fields_def = NULL;
    uint32_t chunk_size = 64, max_size = 256;
    uint32_t actual_num_of_fields = 0;
    int match = 0;
    uint32_t flags = 0;

    fields_def = sal_alloc(max_size * sizeof(bcmlt_field_def_t),
                           "bcmaLtDefFields");
    if (fields_def == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META_U(unit, "Fail to allocate default fields\n")));
        return 0;
    }

    if (search_scope & BCMA_BCMLT_SCOPE_F_LTBL) {
        flags |= BCMLT_TABLE_SEARCH_LT;
    }
    if (search_scope & BCMA_BCMLT_SCOPE_F_PTBL) {
        flags |= BCMLT_TABLE_SEARCH_PT;
    }
    if (flags == 0) {
        /* Search both logical and physical tables if no flags are specified. */
        flags = BCMLT_TABLE_SEARCH_LT | BCMLT_TABLE_SEARCH_PT;
    }

    if (tsrch_mtx[unit]) {
        sal_mutex_take(tsrch_mtx[unit], SAL_MUTEX_FOREVER);
    }

    rv = bcmlt_table_names_get_first(unit, flags, &table_name);
    while (SHR_SUCCESS(rv)) {
        if (table_match(table_name, search_name, match_mode)) {

            if (match > 0 && match_mode == BCMA_BCMLT_MATCH_EXACT) {
                /* Optimize for exact match */
                continue;
            }

            match++;

            /* Get default fields information of the matched table. */
            rv = bcmlt_table_field_defs_get(unit, table_name,
                                            max_size, fields_def,
                                            &actual_num_of_fields);
            if (SHR_FAILURE(rv)) {
                continue;
            }

            if (actual_num_of_fields > max_size) {
                bcmlt_field_def_t *nfd;
                size_t ns = max_size;

                do {
                    ns += chunk_size;
                } while(actual_num_of_fields > ns);
                nfd = sal_alloc(ns * sizeof(bcmlt_field_def_t),
                                "bcmaLtDefFields");
                if (nfd == NULL) {
                    LOG_ERROR(BSL_LS_APPL_BCMLT,
                              (BSL_META_U(unit,
                                          "Fail to allocate %d default fields"
                                          "information of table %s.\n"),
                               (int)actual_num_of_fields, table_name));
                    continue;
                }

                /* Update default fields pointer */
                sal_free(fields_def);
                fields_def = nfd;
                max_size = ns;


                /* Try again with the expended default fields space. */
                rv = bcmlt_table_field_defs_get(unit, table_name,
                                                max_size, fields_def,
                                                &actual_num_of_fields);
                if (SHR_FAILURE(rv)) {
                    continue;
                }
            }

            if (cb) {
                if (cb(cookie, table_name, fields_def,
                       actual_num_of_fields) < 0) {
                    /* Stop searching on error. */
                    break;
                }
            }
        }
        rv = bcmlt_table_names_get_next(unit, flags, &table_name);
    }

    if (tsrch_mtx[unit]) {
        sal_mutex_give(tsrch_mtx[unit]);
    }

    sal_free(fields_def);

    return match;
}

int
bcma_bcmlt_tables_search_init(void)
{
    int idx;

    if (initialized) {
        return 0;
    }
    initialized = true;

    for (idx = 0; idx < BCMDRD_CONFIG_MAX_UNITS; idx++) {
        tsrch_mtx[idx] = sal_mutex_create("bcmaBcmltTblsSearch");
        if (tsrch_mtx[idx] == NULL) {
            bcma_bcmlt_tables_search_cleanup();
            return -1;
        }
    }

    return 0;
}

int
bcma_bcmlt_tables_search_cleanup(void)
{
    int idx;

    if (!initialized) {
        return 0;
    }

    for (idx = 0; idx < BCMDRD_CONFIG_MAX_UNITS; idx++) {
        if (tsrch_mtx[idx]) {
            sal_mutex_destroy(tsrch_mtx[idx]);
        }
    }

    initialized = false;

    return 0;
}
