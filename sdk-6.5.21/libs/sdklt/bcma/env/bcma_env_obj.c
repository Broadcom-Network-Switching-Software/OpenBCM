/*! \file bcma_env_obj.c
 *
 * CLI Environment
 *
 * Create/destroy environment object.
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

bcma_env_handle_t
bcma_env_create(uint32_t hash_order)
{
    bcma_env_handle_t eh;
    env_t *env = sal_alloc(sizeof(*env), "bcmaEnvObj");

    if (env == NULL) {
        return NULL;
    }

    /* Set opaque handle */
    eh = (bcma_env_handle_t )env;

    /* Initialize environment */
    if (bcma_env_init(eh, hash_order) < 0) {
        sal_free(env);
        return NULL;
    }

    /* Mark object as dynamically allocated */
    env->dyn_sig = ENV_SIGNATURE;

    return eh;
}

int
bcma_env_destroy(bcma_env_handle_t eh)
{
    env_t *env = (env_t *)eh;

    if (env == NULL || BAD_OBJ(env)) {
        return -1;
    }

    /* Free environment resources */
    bcma_env_cleanup(eh);

    /* Mark object as invalid */
    env->dyn_sig = 0;

    sal_free(env);

    return 0;
}
