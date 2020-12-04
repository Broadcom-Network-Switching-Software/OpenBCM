/*! \file bcma_cfgdb.c
 *
 * Configuration data manager.
 *
 * Initialize/cleanup/access cfgdb object.
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

static cfgdb_t *cfgdb = NULL;

cfgdb_t *
bcma_cfgdb_get(void)
{
    return cfgdb;
}

int
bcma_cfgdb_init(bcma_cfgdb_cfg_t *cfg)
{
    if (cfgdb != NULL) {
        bcma_cfgdb_cleanup();
    }

    cfgdb = sal_alloc(sizeof(*cfgdb), "bcmaCfgdbInit");
    if (cfgdb == NULL) {
        return -1;
    }

    sal_memset(cfgdb, 0, sizeof(*cfgdb));

    /* Create environment object for handling variables */
    cfgdb->env = bcma_env_create(10);
    if (cfgdb->env == NULL) {
        return -1;
    }

    if (cfg) {
        if (cfg->filename) {
            cfgdb->fname = sal_strdup(cfg->filename);
        }
        if (cfg->tmp_filename) {
            cfgdb->tmp_fname = sal_strdup(cfg->tmp_filename);
        }
    }

    /* Initialize object signature for sanity checks */
    cfgdb->cfgdb_sig = CFGDB_SIGNATURE;

    return 0;
}

int
bcma_cfgdb_cleanup(void)
{
    if (cfgdb == NULL) {
        return -1;
    }

    if (cfgdb->env) {
        bcma_env_destroy(cfgdb->env);
    }

    if (cfgdb->fname) {
        sal_free(cfgdb->fname);
    }

    if (cfgdb->tmp_fname) {
        sal_free(cfgdb->tmp_fname);
    }

    sal_free(cfgdb);

    cfgdb = NULL;

    return 0;
}

int
bcma_cfgdb_file_name_get(const char **filename, const char **tmp_filename)
{
    if (cfgdb == NULL || BAD_CFGDB(cfgdb)) {
        return -1;
    }

    if (filename) {
        *filename = cfgdb->fname;
    }

    if (tmp_filename) {
        *tmp_filename = cfgdb->tmp_fname;
    }

    return 0;
}
