/*! \file bcma_env_init.c
 *
 * CLI Environment
 *
 * Initialize/cleanup environment object.
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

int
bcma_env_var_list_init(env_var_list_t *vl, uint32_t hash_order)
{
    int htab_size, size, bucket;

    vl->hash_order = hash_order;

    htab_size = 1 << hash_order;

    size = htab_size * sizeof(env_var_t);

    vl->htab = sal_alloc(size, "bcmaEnvVarListInit");
    if (vl->htab == NULL) {
        return -1;
    }
    sal_memset(vl->htab, 0, size);

    /* Initialize variable list */
    for (bucket = 0; bucket < htab_size; bucket++) {
        env_var_t *htab = &vl->htab[bucket];

        htab->next = htab;
        htab->prev = htab;
    }

    return 0;
}

int
bcma_env_var_list_cleanup(env_var_list_t *vl)
{
    int bucket;
    env_var_t *ev;
    int htab_size = 1 << vl->hash_order;

    if (vl->htab == NULL) {
        return -1;
    }

    for (bucket = 0; bucket < htab_size; bucket++) {
        env_var_t *htab = &vl->htab[bucket];

        /* Empty hash bucket */
        for (ev = htab->next; ev != htab; ) {
            sal_free(ev->name);
            sal_free(ev->value);
            ev = ev->next;
            sal_free(ev->prev);
        }
    }
    sal_free(vl->htab);

    vl->htab = NULL;

    return 0;
}

int
bcma_env_init(bcma_env_handle_t eh, uint32_t hash_order)
{
    env_t *env = (env_t *)eh;

    if (env == NULL) {
        return -1;
    }

    sal_memset(env, 0, sizeof(*env));

    /* Initialize lock for list operations */
    env->lock = sal_spinlock_create("bcma_env_lock");
    if (env->lock == NULL) {
        return -1;
    }

    /* Initialize list of global variables */
    if (bcma_env_var_list_init(&env->var_global_list, hash_order) != 0) {
        return -1;
    }

    env->hash_order = hash_order;

    /* Initialize object signature for sanity checks */
    env->env_sig = ENV_SIGNATURE;

    return 0;
}

int
bcma_env_cleanup(bcma_env_handle_t eh)
{
    env_t *env = (env_t *)eh;

    /* Check for valid env structure */
    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    /* Pop local scopes (if any) */
    while (env->var_scope_list) {
        if (bcma_env_scope_pop(eh) < 0) {
            return -1;
        }
    }

    /* Delete list of global variables */
    bcma_env_var_list_cleanup(&env->var_global_list);

    /* Free lock */
    if (env->lock) {
        sal_spinlock_destroy(env->lock);
    }

    /* Mark object as uninitialized */
    env->env_sig = 0;

    return 0;
}
