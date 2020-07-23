/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Management
 */

#include <sal/core/libc.h> 
#include <sal/appl/io.h> 
#include <shared/bsl.h>
#include <shared/bslnames.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bsltrace.h>
#include <appl/diag/bslfile.h>
#ifdef INCLUDE_TCL
#include <appl/diag/bslerror.h>
#endif /* INCLUDE_TCL */
#include <appl/diag/bslsink.h>
#include <appl/diag/bslmgmt.h>

/* Optional output hook for redirected log messages */
static bsl_out_hook_f redir_hook;

/*
 * Output hook for core BSL configuration
 */
STATIC int
bslmgmt_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    int rv = 0;
    int sink_rv;
    va_list args_copy;
    bslsink_sink_t *sink = bslsink_sink_find_by_id(0);

    if (meta->severity > bslenable_get(meta->layer, meta->source)) {
        return 0;
    }

    /* Check if this output type is being redirected */
    if (redir_hook && redir_hook(meta, format, args) >= 0) {
        return 0;
    }

    while (sink != NULL) {
        /* Avoid consuming same arg list twice. */
        va_copy(args_copy, args);
        sink_rv = bslsink_out(sink, meta, format, args_copy);
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
STATIC int
bslmgmt_check_hook(bsl_packed_meta_t meta_pack)
{
    int layer, source, severity;

    layer = BSL_LAYER_GET(meta_pack);
    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    return (severity <= bslenable_get(layer, source));
}

int
bslmgmt_cleanup(void)
{
    return bslsink_cleanup();
}

int
bslmgmt_init(void)
{
    bsl_config_t bsl_config;

    bslenable_reset_all();

    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = bslmgmt_out_hook;
    bsl_config.check_hook = bslmgmt_check_hook;
    bsl_init(&bsl_config);

    /* Initialize output hook */
    bslsink_init();

    /* Create console sink */
    bslcons_init();

#ifndef NO_FILEIO
    /* Create file sink */
    bslfile_init();
#endif

#ifdef BSL_TRACE_INCLUDE
    /* Create trace sink */
    bsltrace_init();
    /* Turn on debug so that trace sink can catch it */
    bslmgmt_set(bslLayerBcm, bslSourceTrace, bslSeverityDebug);
    bslmgmt_set(bslLayerSoc, bslSourceTrace, bslSeverityDebug);
#endif

#ifdef INCLUDE_TCL
    /* Create error sink */
    bslerror_init();
#endif /* INCLUDE_TCL */

    return 0;
}

int
bslmgmt_redir_hook_set(bsl_out_hook_f out_hook)
{
    redir_hook = out_hook;

    return 0;
}

