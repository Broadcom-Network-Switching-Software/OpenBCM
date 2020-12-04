/*! \file bcma_env_scope.c
 *
 * CLI Environment
 *
 * Push/pop scope for local environment variables.
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
bcma_env_scope_push(bcma_env_handle_t eh)
{
    env_t *env = (env_t *)eh;
    env_var_scope_t *vs;

    /* Check for valid env structure */
    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    /* Allocate new scope */
    vs = (env_var_scope_t *)sal_alloc(sizeof(env_var_scope_t),
                                      "bcmaEnvDiagScope");
    if (vs == NULL) {
        return -1;
    }

    /* Initialize local variable list */
    if (bcma_env_var_list_init(&vs->local, env->hash_order) != 0) {
        sal_free(vs);
        return -1;
    }

    /* Add new scope to list */
    sal_spinlock_lock(env->lock);
    vs->next  = env->var_scope_list;
    env->var_scope_list = vs;
    sal_spinlock_unlock(env->lock);

    return 0;
}

int
bcma_env_scope_pop(bcma_env_handle_t eh)
{
    env_t *env = (env_t *)eh;
    env_var_scope_t *vs;

    /* Check for valid env structure */
    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    /* Remove scope from list */
    sal_spinlock_lock(env->lock);
    vs = env->var_scope_list;
    if (vs == NULL) {
        sal_spinlock_unlock(env->lock);
        return -1;
    }
    env->var_scope_list = vs->next;
    sal_spinlock_unlock(env->lock);

    /* Free contents of popped scope */
    bcma_env_var_list_cleanup(&vs->local);

    /* Finally free scope */
    sal_free(vs);

    return 0;
}

int
bcma_env_num_scopes_get(bcma_env_handle_t eh)
{
    env_t *env = (env_t *)eh;
    env_var_scope_t *vs;
    int scopes = 0;

    /* Check for valid env structure */
    if (env == NULL || BAD_ENV(env)) {
        return -1;
    }

    sal_spinlock_lock(env->lock);

    for (vs = env->var_scope_list; vs; vs = vs->next) {
        scopes++;
    }

    sal_spinlock_unlock(env->lock);

    return scopes;
}
