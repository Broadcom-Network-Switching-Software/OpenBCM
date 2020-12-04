/*! \file bcma_envcmd_show_var_list.c
 *
 * CLI Environment
 *
 * Display environment variables on console.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcma/env/bcma_env.h>

#include "envcmd_internal.h"

/*
 * Show a single variable.
 */
static int
show_var_entry(void *cookie, const char *name, const char *value, uint32_t *tag)
{
    COMPILER_REFERENCE(cookie);
    COMPILER_REFERENCE(tag);

    cli_out("%20s = %-20s\n", name, value);

    return 0;
}

void
bcma_envcmd_show_var_list(bcma_env_handle_t eh, const char *hdr, int scope)
{
    cli_out("\n%20s | %-20s\n"
            "---------------------+---------------------\n",
            hdr, "Value");

    bcma_env_var_traverse(eh, scope, show_var_entry, NULL);

    cli_out("-------------------------------------------\n");
}
