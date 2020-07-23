/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (BSL)
 *
 * BSL globals for EXTERNAL (application) usage.
 */

#include <shared/bsltypes.h>

static char *bsl_layer_names[bslLayerCount] = BSL_LAYER_NAMES_INIT;
static char *bsl_severity_names[bslSeverityCount] = BSL_SEVERITY_NAMES_INIT;
static char *bsl_source_names[bslSourceCount] = BSL_SOURCE_NAMES_INIT;

const char *
bsl_layer2str(bsl_layer_t layer)
{
    if (layer < bslLayerCount) {
        return bsl_layer_names[layer];
    }
    return "<none>";
}

const char *
bsl_source2str(bsl_source_t source)
{
    if (source < bslSourceCount) {
        return bsl_source_names[source];
    }
    return "<none>";
}

const char *
bsl_severity2str(bsl_severity_t severity)
{
    if (severity < bslSeverityCount) {
        return bsl_severity_names[severity];
    }
    return "<none>";
}
