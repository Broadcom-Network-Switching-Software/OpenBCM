/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (BSL)
 *
 * BSL globals for EXTERNAL (application) usage.
 */

#include <bsl/bsl_names.h>

static char *bsl_layer_names[BSL_LAY_COUNT] = BSL_LAYER_NAMES_INIT;
static char *bsl_severity_names[BSL_SEV_COUNT] = BSL_SEVERITY_NAMES_INIT;
static char *bsl_source_names[BSL_SRC_COUNT] = BSL_SOURCE_NAMES_INIT;

const char *
bsl_layer2str(bsl_layer_t layer)
{
    if (layer < BSL_LAY_COUNT) {
        return bsl_layer_names[layer];
    }
    return "<none>";
}

const char *
bsl_source2str(bsl_source_t source)
{
    if (source < BSL_SRC_COUNT) {
        return bsl_source_names[source];
    }
    return "<none>";
}

const char *
bsl_severity2str(bsl_severity_t severity)
{
    if (severity < BSL_SEV_COUNT) {
        return bsl_severity_names[severity];
    }
    return "<none>";
}
