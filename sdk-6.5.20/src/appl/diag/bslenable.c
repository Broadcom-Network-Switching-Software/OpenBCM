/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Enables
 */

#include <shared/bslnames.h>
#include <appl/diag/bslenable.h>
#include <sal/core/thread.h>

/* Table for tracking which layers/sources/severities to enable */
static bsl_severity_t bslenable_severity[bslLayerCount][bslSourceCount];

/* Which sources are valid in which layers (fixed) */
BSL_SOURCES_VALID_DEF(bsl_source_t);
static bsl_source_t *sources_valid[] = BSL_SOURCES_VALID_INIT;

/* List of threads for which a specific debug level is enabled.*/
typedef struct {
    int   thread[MAX_BSLENABLE_THREAD_COUNT];
} bslenable_thread_t;

/* Table of tracking which layers/sources/thread-list to enable */
static bslenable_thread_t bslenable_thread[bslLayerCount][bslSourceCount];

void
bslenable_thread_set(bsl_layer_t layer, bsl_source_t source, int tidx,
                     int thread)
{
    bslenable_thread_t *thread_list;

    if (layer < 0 || layer >= bslLayerCount) {
        return;
    }
    if (tidx < 0 || tidx >= MAX_BSLENABLE_THREAD_COUNT) {
        return;
    }
    thread_list = &bslenable_thread[layer][source];

    thread_list->thread[tidx] = thread;
}

int
bslenable_thread_get(bsl_layer_t layer, bsl_source_t source, int tidx)
{
    bslenable_thread_t *thread_list;

    thread_list = &bslenable_thread[layer][source];
    if (layer < bslLayerCount && source < bslSourceCount &&
        tidx < MAX_BSLENABLE_THREAD_COUNT) {
        return thread_list->thread[tidx];
    }

    return 0;
}

void
bslenable_thread_reset(bsl_layer_t layer, bsl_source_t source, int tidx)
{
    bslenable_thread_set(layer, source, tidx, 0);
}

void
bslenable_thread_reset_all(void)
{
    int layer, source, tidx;

    for (layer = 0; layer < bslLayerCount; layer++) {
        for (source = 0; source < bslSourceCount; source++) {
            for (tidx = 0; tidx < MAX_BSLENABLE_THREAD_COUNT; tidx ++) {
                bslenable_thread_reset(layer, source, tidx);
            }
        }
    }
}

int
bslenable_source_valid(bsl_layer_t layer, bsl_source_t source)
{
    bsl_source_t *src;
    int idx;

    if (layer < 0 || layer >= bslLayerCount) {
        return 0;
    }
    if (source < 0 || source >= bslSourceCount) {
        return 0;
    }
    src = sources_valid[layer];
    for (idx = 0; src[idx] != bslSourceCount && idx < bslSourceCount; idx++) {
        if (source == src[idx]) {
            return 1;
        }
    }
    return 0;
}

void
bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity)
{
    if (layer < 0 || layer >= bslLayerCount) {
        return;
    }
    bslenable_severity[layer][source] = severity;
}

bsl_severity_t
bslenable_get(bsl_layer_t layer, bsl_source_t source)
{
    if (layer < bslLayerCount && source < bslSourceCount) {
        return bslenable_severity[layer][source];
    }
        
    return 0;
}

void
bslenable_reset(bsl_layer_t layer, bsl_source_t source)
{
    bsl_severity_t severity = bslSeverityWarn;

    if (source == bslSourceShell ||
        source == bslSourceEcho ||
        (layer == bslLayerTks && source == bslSourceCommon)) {
        severity = bslSeverityInfo;
    }
        
    bslenable_set(layer, source, severity);
}

void
bslenable_reset_all(void)
{
    int layer, source;

    for (layer = 0; layer < bslLayerCount; layer++) {
        for (source = 0; source < bslSourceCount; source++) {
            bslenable_reset(layer, source);
        }
    }
}

int
bslenable_init(void)
{
    bslenable_reset_all();
    bslenable_thread_reset_all();
    return 0;
}
