/*! \file bcmcfg_read_util.c
 *
 * Utility function for bcmcfg reader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcfg/bcmcfg_read_util.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*! Bcmcfg reader skip mode. */
static bool read_skip = false;
/*! Bcmcfg reader context level of skip mode. */
static int read_skip_level = 0;

/*******************************************************************************
 * Public functions
 */
bool
bcmcfg_is_read_skip(bcmcfg_read_context_t *context)
{
    bool result = false;

    if (read_skip &&
        read_skip_level < context->level) {
        result = true;
    }

    return result;
}

void
bcmcfg_read_skip_set(bcmcfg_read_context_t *context)
{
    read_skip = true;
    read_skip_level = context->level;

    return;
}

void
bcmcfg_read_skip_clear(bcmcfg_read_context_t *context)
{
    if (read_skip_level >= context->level) {
        read_skip = false;
        read_skip_level = 0;
    }

    return;
}

void
bcmcfg_read_skip_document_set(void)
{
    read_skip = true;
    read_skip_level = 0;

    return;
}

void
bcmcfg_read_skip_document_clear(void)
{
    read_skip = false;
    read_skip_level = 0;

    return;
}

