/*! \file bcma_cfgdb_var.c
 *
 * Configuration data manager for variables operation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cfgdb/bcma_cfgdb.h>

#include "cfgdb_internal.h"

/*
 * Cookie for bcma_cfgdb_var_traverse call-back function var_wildcard_delete.
 */
struct trvs_wc_del {

    /*! Wildcard string to be compared with variable name */
    const char *wildcard_str;

    /*! Number of variables deleted */
    int del_num;
};

/*
 * Call-back function of bcma_cfgdb_var_traverse to delete variables
 * if variable name matches wildcard string.
 */
static int
var_wildcard_delete(void *cookie, const char *name, const char *value,
                    uint32_t *tag)
{
    struct trvs_wc_del *wc_del = (struct trvs_wc_del *)cookie;
    const char *wc_str = wc_del->wildcard_str;

    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(tag);

    if (bcma_cfgdb_wildcard_match(name, wc_str)) {
        wc_del->del_num++;
        return BCMA_CFGDB_VAR_TRAVERSE_DELETE;
    }

    return 0;
}

int
bcma_cfgdb_wildcard_match(const char *str, const char *wc_str)
{
    int match = TRUE;
    const char *ptr = NULL;
    char s, w;

    while (1) {
        s = *str;
        w = *wc_str;

        if (s == '\0') {
            /*
             * When target string reaches end, match is true only if wildcard
             * string reaches end too or meets '*'.
             */
            if ( w == '\0') {
                break;
            } else if (w == '*') {
                wc_str++;
                continue;
            }
            match = FALSE;
            break;
        } else {
            if (s != w) {
                if (w == '*') {
                    /* If '*' is met, locate ptr to the position after '*'. */
                    ptr = ++wc_str;
                    continue;
                } else if (ptr) {
                    /*
                     * Wildcard string is always compared starting from
                     * character after '*' if character is not matching.
                     */
                    wc_str = ptr;
                    w = *wc_str;

                    if (w == '\0') {
                        break;
                    } else if (s == w) {
                        wc_str++;
                    }
                    str++;
                    continue;
                } else {
                    /*
                     * If characters don't match and wildcard '*' is not
                     * appearing yet, it should be no matching.
                     */
                    match = FALSE;
                    break;
                }
            }
        }

        /* Move to next if strings match for non-wildcard characters */
        str++;
        wc_str++;
    }

    return match;
}

int
bcma_cfgdb_var_tag_flags_clear(void *cookie, const char *name,
                               const char *value, uint32_t *tag)
{
    struct trvs_tag_flag_op *tfop = (struct trvs_tag_flag_op *)cookie;
    const char *target_name = tfop->name;
    int flags = tfop->flags;

    COMPILER_REFERENCE(value);

    /* Clear flags for all variables if target name is NULL. */
    if (target_name == NULL) {
        *tag &= ~(flags & BCMA_CFGDB_TAG_FLAG_MASK);
        return 0;
    }

    /* Stop traversing if the target variable flag has been cleared. */
    if (tfop->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(name, target_name) == 0) {
        *tag &= ~(flags & BCMA_CFGDB_TAG_FLAG_MASK);
        tfop->done = TRUE;
    }

    return 0;
}

int
bcma_cfgdb_var_tag_flags_set(void *cookie, const char *name, const char *value,
                             uint32_t *tag)
{
    struct trvs_tag_flag_op *tfop = (struct trvs_tag_flag_op *)cookie;
    const char *target_name = tfop->name;
    int flags = tfop->flags;

    COMPILER_REFERENCE(value);

    /* Set flags for all variables if target name is NULL. */
    if (target_name == NULL) {
        *tag |= (flags & BCMA_CFGDB_TAG_FLAG_MASK);
        return 0;
    }

    /* Stop traversing if the target variable flag has been set. */
    if (tfop->done) {
        return BCMA_CFGDB_VAR_TRAVERSE_STOP;
    }

    if (sal_strcmp(target_name, name) == 0) {
        *tag |= (flags & BCMA_CFGDB_TAG_FLAG_MASK);
        tfop->done = TRUE;
    }

    return 0;
}

int
bcma_cfgdb_var_set(const char *name, char *value)
{
    int rv = 0;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (value == NULL) {
        /* Delete variable if value is NULL. */
        rv = bcma_env_var_unset(cfgdb->env, name, FALSE, TRUE);

        /*
         * If no matching on delete variable, check if name contains
         * wildcard '*' and try deleting variables by wildcard comparison.
         */
        if (rv != 0) {
            if (sal_strchr(name, '*')) {
                struct trvs_wc_del wc_del;

                wc_del.wildcard_str = name;
                wc_del.del_num = 0;

                bcma_cfgdb_var_traverse(var_wildcard_delete, &wc_del);

                rv = wc_del.del_num ? 0 : -1;
            }
        }
    } else {
        /* Set variable is value is not NULL. */
        rv = bcma_env_var_set(cfgdb->env, name, value, FALSE);
    }

    return rv;
}

const char *
bcma_cfgdb_var_get(const char *name)
{
    const char * value;
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return NULL;
    }

    value = bcma_env_var_get(cfgdb->env, name);

    if (value != NULL) {
        struct trvs_tag_flag_op tfop;

        tfop.name = name;
        tfop.flags = BCMA_CFGDB_TAG_FLAG_HIT;
        tfop.done = FALSE;

        /* Set HIT flag to the variable. */
        bcma_cfgdb_var_traverse(bcma_cfgdb_var_tag_flags_set, &tfop);
    }

    return value;
}

int
bcma_cfgdb_var_traverse(int (*cb)(void *cookie, const char *name,
                                  const char *value, uint32_t *tag),
                        void *cookie)
{
    cfgdb_t *cfgdb = bcma_cfgdb_get();

    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    return bcma_env_var_traverse(cfgdb->env, 0, cb, cookie);
}
