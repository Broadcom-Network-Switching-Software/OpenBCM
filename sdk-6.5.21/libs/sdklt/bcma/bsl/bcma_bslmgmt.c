/*! \file bcma_bslmgmt.c
 *
 * Broadcom System Log Management
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl_ext.h>

#include <bcma/bsl/bcma_bslenable.h>
#include <bcma/bsl/bcma_bslcons.h>
#include <bcma/bsl/bcma_bslfile.h>
#include <bcma/bsl/bcma_bslsink.h>
#include <bcma/bsl/bcma_bslmgmt.h>

/* Optional output hook for redirected log messages */
static bsl_out_hook_f redir_hook;

/*
 * Output hook for core BSL configuration
 */
static int
bcma_bslmgmt_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    int rv = 0;
    int sink_rv;
    va_list args_copy;
    bcma_bslsink_sink_t *sink = bcma_bslsink_sink_find_by_id(0);

    if (meta->severity > bcma_bslenable_get(meta->layer, meta->source)) {
        return 0;
    }

    /* Check if this output type is being redirected */
    if (redir_hook && redir_hook(meta, format, args) >= 0) {
        return 0;
    }

    while (sink != NULL) {
        /* Avoid consuming same arg list twice. */
        va_copy(args_copy, args);
        sink_rv = bcma_bslsink_out(sink, meta, format, args_copy);
        va_end(args_copy);
        if (sink_rv > 0) {
            rv = sink_rv;
        }
        sink = sink->next;
    }
    return rv;
}

/*
 * Check hook for core BSL configuration
 */
static int
bcma_bslmgmt_check_hook(bsl_packed_meta_t meta_pack)
{
    int layer, source, severity;

    layer = BSL_LAYER_GET(meta_pack);
    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    return (severity <= (int)bcma_bslenable_get(layer, source));
}

int
bcma_bslmgmt_cleanup(void)
{
    return bcma_bslsink_cleanup();
}

int
bcma_bslmgmt_init(void)
{
    bsl_config_t bsl_config;

    bcma_bslenable_init();

    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = bcma_bslmgmt_out_hook;
    bsl_config.check_hook = bcma_bslmgmt_check_hook;
    bsl_init(&bsl_config);

    /* Initialize output hook */
    bcma_bslsink_init();

    /* Create console sink */
    bcma_bslcons_init();

    /* Create file sink */
    bcma_bslfile_init();

#ifdef BSL_TRACE_INCLUDE
    /* Create trace sink */
    bcma_bsltrace_init();
#endif

    return 0;
}

int
bcma_bslmgmt_redir_hook_set(bsl_out_hook_f out_hook)
{
    redir_hook = out_hook;

    return 0;
}
