/*! \file bcma_bslenable.c
 *
 * Broadcom System Log Enables
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl_names.h>

#include <bcma/bsl/bcma_bslenable.h>

/* Table for tracking which layers/sources/severities to enable */
static bsl_severity_t bcma_bslenable_severity[BSL_LAY_COUNT][BSL_SRC_COUNT];

/* Which sources are valid in which layers (fixed) */
BSL_SOURCES_VALID_DEF(bsl_source_t);
static bsl_source_t *sources_valid[] = BSL_SOURCES_VALID_INIT;

int
bcma_bslenable_source_valid(bsl_layer_t layer, bsl_source_t source)
{
    bsl_source_t *src;
    int idx;

    if (layer < 0 || layer >= BSL_LAY_COUNT) {
        return 0;
    }
    if (source < 0 || source >= BSL_SRC_COUNT) {
        return 0;
    }
    src = sources_valid[layer];
    for (idx = 0; src[idx] != BSL_SRC_COUNT && idx < BSL_SRC_COUNT; idx++) {
        if (source == src[idx]) {
            return 1;
        }
    }
    return 0;
}

void
bcma_bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity)
{
    if (layer < 0 || layer >= BSL_LAY_COUNT) {
        return;
    }
    bcma_bslenable_severity[layer][source] = severity;
}

bsl_severity_t
bcma_bslenable_get(bsl_layer_t layer, bsl_source_t source)
{
    if (layer < BSL_LAY_COUNT && source < BSL_SRC_COUNT) {
        return bcma_bslenable_severity[layer][source];
    }

    return 0;
}

void
bcma_bslenable_reset(bsl_layer_t layer, bsl_source_t source)
{
    bsl_severity_t severity = BSL_SEV_WARN;

    if (source == BSL_SRC_SHELL ||
        source == BSL_SRC_ECHO) {
        severity = BSL_SEV_INFO;
    }

    bcma_bslenable_set(layer, source, severity);
}

void
bcma_bslenable_reset_all(void)
{
    int layer, source;

    for (layer = 0; layer < BSL_LAY_COUNT; layer++) {
        for (source = 0; source < BSL_SRC_COUNT; source++) {
            bcma_bslenable_reset(layer, source);
        }
    }
}

int
bcma_bslenable_init(void)
{
    bcma_bslenable_reset_all();

    return 0;
}
