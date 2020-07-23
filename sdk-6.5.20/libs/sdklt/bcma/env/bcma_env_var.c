/*! \file bcma_env_var.c
 *
 * CLI Environment
 *
 * Manage environment variables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/env/bcma_env.h>

#include "env_internal.h"

/*
 * Return the index of the hash table for the specified variable.
 * It encodes the first to last character, as well as string length.
 */
static inline uint32_t
var_hash(const char *name, uint32_t hash_order)
{
    uint32_t hash, idx;

    /* No hashing */
    if (hash_order == 0) {
        return 0;
    }

    idx = 0;
    hash = 5381;

    while (name[idx] != 0) {
        hash = ((hash << 5) + hash) + name[idx];
        idx++;
    }
    hash = ((hash << 5) + hash) + idx;

    return (hash & ((1 << hash_order) - 1));
}

/*
 * Find variable in linked list.
 */
static env_var_t *
var_find_list(env_var_list_t *vl, const char *name)
{
    env_var_t *ev;
    uint32_t hash = var_hash(name, vl->hash_order);
    env_var_t *htab = &vl->htab[hash];

    for (ev = htab->next; ev != htab; ev = ev->next) {
        if (sal_strcmp(ev->name, name) == 0) {
            return ev;
        }
    }

    return NULL;
}

/*
 * Find local/global variable in current scope.
 */
static env_var_t *
var_find(env_t *env, const char *name, int local, int global)
{
    env_var_t *ev = NULL;

    if (local && env->var_scope_list) {
        ev = var_find_list(&env->var_scope_list->local, name);
    }
    if (global && ev == NULL) {
        ev = var_find_list(&env->var_global_list, name);
    }
    return ev;
}

/*
 * Remove variable from linked list and free associate memory.
 */
static void
var_delete(env_t *env, env_var_t *ev)
{
    ev->prev->next = ev->next;
    ev->next->prev = ev->prev;

    sal_free(ev->name);
    sal_free(ev->value);
    sal_free(ev);
}

/*
 * Allocate memory for and insert variable in linked list.
 */
static int
var_set_list(env_t *env, env_var_list_t *vl, const char *name, const char *val)
{
    env_var_t *ev;
    char *v_value = sal_strdup(val);

    if (v_value == NULL) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if ((ev = var_find_list(vl, name)) != NULL) {
        /* If already exists on list, replace value */
        sal_free(ev->value);
        ev->value = v_value;
        sal_spinlock_unlock(env->lock);
    } else {
        uint32_t hash;
        env_var_t *htab;

        sal_spinlock_unlock(env->lock);

        /* Otherwise, create a new entry and link it on the list */
        ev = (env_var_t *)sal_alloc(sizeof(*ev), "bcmaEnvDiagVar");
        if (ev == NULL) {
            sal_free(v_value);
            return -1;
        }
        sal_memset(ev, 0, sizeof(*ev));
        ev->name = sal_strdup(name);
        if (ev->name == NULL) {
            sal_free(ev);
            sal_free(v_value);
            return -1;
        }
        ev->value = v_value;

        hash = var_hash(name, vl->hash_order);
        htab = &vl->htab[hash];
        sal_spinlock_lock(env->lock);
        ev->prev  = htab->prev;
        ev->next  = htab;
        ev->prev->next = ev;
        ev->next->prev = ev;
        sal_spinlock_unlock(env->lock);
    }
    return 0;
}

int
bcma_env_var_set(bcma_env_handle_t eh, const char *name, const char *value,
                 int local)
{
    env_t *env = (env_t *)eh;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    if (local && env->var_scope_list) {
        return var_set_list(env, &env->var_scope_list->local, name, value);
    }
    return var_set_list(env, &env->var_global_list, name, value);
}

int
bcma_env_var_integer_set(bcma_env_handle_t eh, const char *name, int value,
                         int local)
{
    char v_str[12];

    sal_snprintf(v_str, sizeof(v_str), "%d", value);
    return bcma_env_var_set(eh, name, v_str, local);
}

int
bcma_env_var_hex_set(bcma_env_handle_t eh, const char *name, int value,
                     int local)
{
    char v_str[12];

    sal_snprintf(v_str, sizeof(v_str), "0x%04x", value);
    return bcma_env_var_set(eh, name, v_str, local);
}

int
bcma_env_var_unset(bcma_env_handle_t eh, const char *name,
                   int local, int global)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, local, global);
    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    var_delete(env, ev);

    sal_spinlock_unlock(env->lock);

    return 0;
}

const char *
bcma_env_var_get(bcma_env_handle_t eh, const char *name)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;
    const char * val = NULL;

    if (env == NULL || BAD_ENV(env)) {
        return NULL;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, TRUE, TRUE);
    if (ev) {
        val = ev->value;
    }

    sal_spinlock_unlock(env->lock);

    return val;
}

int
bcma_env_var_exists(bcma_env_handle_t eh, const char *name,
                    int local, int global)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return FALSE;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, local, global);

    sal_spinlock_unlock(env->lock);

    return (ev) ? TRUE : FALSE;
}

int
bcma_env_var_traverse(bcma_env_handle_t eh, int scope,
                      int (*cb)(void *cookie, const char *name,
                                const char *value, uint32_t *tag),
                      void *cookie)
{
    env_t *env = (env_t *)eh;
    env_var_list_t *vl = NULL;
    env_var_t *ev, *pev;
    env_var_scope_t *vs;
    int idx, htab_size, rv = 0;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if (scope == 0) {
        vl = &env->var_global_list;
    } else {
        /* Find matching local scope */
        for (vs = env->var_scope_list, idx = 1; vs; vs = vs->next, idx++) {
            if (scope == idx) {
                vl = &vs->local;
                break;
            }
        }
    }

    if (vl == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    htab_size = 1 << vl->hash_order;

    /* Loop through all variables in the selected scope */
    for (idx = 0; idx < htab_size; idx++) {
        env_var_t *htab = &vl->htab[idx];
        for (ev = htab->next; ev != htab; ev = ev->next) {
            rv = cb(cookie, ev->name, ev->value, &ev->tag);
            if (rv < 0 || rv == BCMA_ENV_VAR_TRAVERSE_STOP) {
                /* Stop traversing on error or stop control. */
                break;
            } else if (rv == BCMA_ENV_VAR_TRAVERSE_DELETE) {
                /* Delete variable */
                pev = ev->prev;
                var_delete(env, ev);
                ev = pev;
            }
        }
        if (rv < 0 || rv == BCMA_ENV_VAR_TRAVERSE_STOP) {
            break;
        }
    }

    sal_spinlock_unlock(env->lock);

    return rv;
}

int
bcma_env_var_tag_set(bcma_env_handle_t eh, const char *name, uint32_t tag,
                     int local)
{
    env_t *env = (env_t *)eh;
    env_var_list_t *vl;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    if (local && env->var_scope_list) {
        vl = &env->var_scope_list->local;
    } else {
        vl = &env->var_global_list;
    }

    ev = var_find_list(vl, name);

    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    ev->tag = tag;

    sal_spinlock_unlock(env->lock);

    return 0;
}

int
bcma_env_var_tag_get(bcma_env_handle_t eh, const char *name, uint32_t *tag)
{
    env_t *env = (env_t *)eh;
    env_var_t *ev;

    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    ev = var_find(env, name, TRUE, TRUE);

    if (ev == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }

    if (tag) {
        *tag = ev->tag;
    }

    sal_spinlock_unlock(env->lock);

    return 0;
}
