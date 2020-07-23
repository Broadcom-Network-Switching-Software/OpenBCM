/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (BSL)
 *
 * BSL functions for INTERNAL (SDK) usage.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>

#define BSL_BUFFER_SIZE 4096

/* 
  * convenience functions/macros to allow compiling out severity levels;
  * preferred to directly calling sal_log.  See sal_log for parameter
  * descriptions 
  */
static bsl_config_t bsl_config;

int
bsl_init(bsl_config_t *config)
{
    bsl_config = *config;
    return 0;
}

void
bsl_config_t_init(bsl_config_t *config)
{
    sal_memset(config, 0, sizeof(*config));
}

/* "Fast" checker function for a single 'chk' */
int
bsl_fast_check(uint32 chk)
{
    if (bsl_config.check_hook != NULL) {
        return bsl_config.check_hook(chk);
    }
    return 0;
}

/*
 * "Fast" checker function for a two 'chk's
 * Return '0' if both 'chk's indicate 'disabled'
 *   or if no hook was assigned.
 * Otherwise (one of the 'chk's indicates 'enabled'), return '1'
 */
int
bsl_fast_check_two(uint32 chk1,uint32 chk2)
{
    if (bsl_config.check_hook != NULL)
    {
        int rv1, rv2;
        rv1 = bsl_config.check_hook(chk1);
        rv2 = bsl_config.check_hook(chk2);
        if ((!rv1) && (!rv2))
        {
            return (0);
        }
        else
        {
            return (1);
        }
    }
    return 0;
}


int
bsl_check(bsl_layer_t layer, int source, bsl_severity_t severity, int unit) 
{
    uint32 chk;

    chk = BSL_LAYER_ENC(layer);
    chk |= BSL_SOURCE_ENC(source);
    chk |= BSL_SEVERITY_ENC(severity);

    return bsl_fast_check(chk);
}

/* Output function */
int
bsl_vlog(bsl_meta_t *meta, const char *format, va_list args)
{
    if (bsl_config.out_hook != NULL) {
        return bsl_config.out_hook(meta, format, args);
    }
    return 0;
}

/* BSL vprintf which can extract meta data */
int
bsl_log(bsl_meta_t *meta, const char *format, ...)
{
    int rc; 
    va_list vargs;

    va_start(vargs, format);
    rc = bsl_vlog(meta, format, vargs);
    va_end(vargs);

    return rc; 
}

/* BSL vprintf which can extract meta data */
int
bsl_vprintf(const char *format, va_list args)
{
    const char *fmt = format;
    bsl_meta_t meta_data, *meta = &meta_data;

    bsl_meta_t_init(meta);

    if (*fmt == '<') {
        fmt++;
        while (1) {
            if (sal_strncmp(fmt, "u=%d", 4) == 0) {
                meta->unit = va_arg(args, int);
                fmt += 4;
            } else if (sal_strncmp(fmt, "p=%d", 4) == 0) {
                meta->port = va_arg(args, int);
                fmt += 4;
            } else if (sal_strncmp(fmt, "x=%d", 4) == 0) {
                meta->xtra = va_arg(args, int);
                fmt += 4;
            } else if (sal_strncmp(fmt, "c=%u", 4) == 0) {
                unsigned int chk = va_arg(args, unsigned int);
                meta->layer = BSL_LAYER_GET(chk);
                meta->source = BSL_SOURCE_GET(chk);
                meta->severity = BSL_SEVERITY_GET(chk);
                fmt += 4;
            } else if (sal_strncmp(fmt, "f=%s", 4) == 0) {
                meta->file = va_arg(args, char *);
                fmt += 4;
            } else if (sal_strncmp(fmt, "l=%d", 4) == 0) {
                meta->line = va_arg(args, int);
                fmt += 4;
            } else if (sal_strncmp(fmt, "F=%s", 4) == 0) {
                meta->func = va_arg(args, char *);
                fmt += 4;
            } else if (sal_strncmp(fmt, "o=%u", 4) == 0) {
                meta->options = va_arg(args, unsigned int);
                fmt += 4;
            } else {
                break;
            }
        }
        if (*fmt == '>') {
            fmt++;
        }
    }

    /* Default to shell output */
    if (meta->layer == bslLayerCount) {
        meta->layer = bslLayerAppl;
        meta->source = bslSourceShell;
        meta->severity = bslSeverityNormal;
    }

    /* Call output hook with whatever is left over */
    return bsl_vlog(meta, fmt, args);
}

/* BSL printf which maps ... to vargs */
int
bsl_printf(const char *format, ...)
{
    int rc; 
    va_list vargs;

    va_start(vargs, format);
    rc = bsl_vprintf(format, vargs);
    va_end(vargs);

    return rc; 
}

int 
_bsl_error(char *file, unsigned int line, const char *func,
          bsl_layer_t layer, int source,
          int unit, const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, bslSeverityError, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = bslSeverityError;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}

int 
_bsl_warn(char *file, unsigned int line, const char *func,
          bsl_layer_t layer, int source,
          int unit, const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, bslSeverityWarn, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = bslSeverityWarn;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}

int 
_bsl_verbose(char *file, unsigned int line, const char *func,
          bsl_layer_t layer, int source,
          int unit, const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, bslSeverityVerbose, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = bslSeverityVerbose;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}

int 
_bsl_vverbose(char *file, unsigned int line, const char *func,
          bsl_layer_t layer, int source,
          int unit, const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, bslSeverityDebug, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = bslSeverityDebug;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}

void
bsl_meta_t_init(bsl_meta_t *meta)
{
    sal_memset(meta, 0, sizeof(*meta));
    meta->options = BSL_META_OPT_START | BSL_META_OPT_END;
    meta->layer = bslLayerCount;
    meta->source = bslSourceCount;
    meta->severity = bslSeverityCount;
    meta->unit = -1;
    meta->port = -1;
    meta->xtra = -1;
}

/* 
 * bsl_start
 * Parameters: as per bsl_vlog
 */
int
bsl_log_start(char *file, unsigned int line, const char *func, 
              bsl_layer_t layer, int source, bsl_severity_t severity,
              int unit, const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, severity, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->options = BSL_META_OPT_START;
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = severity;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}

/* 
 * bsl_end
 * Incrementally adds additional output to a message previously begun with bsl_start
 * and finishes that message
 * Parameters: as per bsl_vlog 
 */
int
bsl_log_end(char *file, unsigned int line, const char *func, 
            bsl_layer_t layer, int source, bsl_severity_t severity,
            int sinks, int unit, 
            const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, severity, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->options = BSL_META_OPT_END;
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = severity;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}
/* 
 * bsl_add
 * Incrementally adds additional output to a message previously begun with bsl_start 
 * Parameters: as per bsl_vlog 
 */
int
bsl_log_add(char *file, unsigned int line, const char *func, 
            bsl_layer_t layer, int source, bsl_severity_t severity,
            int sinks, int unit, 
            const char *msg, ...)
{
    int rc;
    va_list vargs;
    bsl_meta_t meta_data, *meta ;

    if (!bsl_check(layer, source, severity, -1)) {
        return 0;
    }

    meta = &meta_data;
    bsl_meta_t_init(meta);
    meta->options = 0;
    meta->func = func;
    meta->file = file;
    meta->line = line;
    meta->layer = layer;
    meta->source = source;
    meta->severity = severity;
    meta->unit = unit;

    va_start(vargs, msg);
    rc = bsl_vlog(meta, msg, vargs);
    va_end(vargs);

    return rc;
}
