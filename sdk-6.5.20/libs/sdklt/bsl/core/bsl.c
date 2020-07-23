/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (BSL)
 *
 * BSL functions for INTERNAL (SDK) usage.
 */

#include <bsl/bsl.h>
#include <bsl/bsl_ext.h>

#define BSL_BUFFER_SIZE 4096

static bsl_config_t bsl_config;

static void *
bsl_memset(void *dest, int c, unsigned int cnt)
{
    unsigned char *d;

    d = dest;

    while (cnt) {
	*d++ = (unsigned char) c;
	cnt--;
    }

    return d;
}

static int
bsl_strncmp(const char *dest, const char *src, unsigned int cnt)
{
    int rv = 0;

    while (cnt) {
        if ((rv = *dest - *src++) != 0 || !*dest++) {
            break;
        }
        cnt--;
    }
    return rv;
}

int
bsl_init(bsl_config_t *config)
{
    bsl_config = *config;
    return 0;
}

void
bsl_config_t_init(bsl_config_t *config)
{
    bsl_memset(config, 0, sizeof(*config));
}

/* "Fast" checker function */
int
bsl_fast_check(unsigned int chk)
{
    if (bsl_config.check_hook != NULL) {
        return bsl_config.check_hook(chk);
    }
    return 0;
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
            if (bsl_strncmp(fmt, "u=%d", 4) == 0) {
                meta->unit = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "p=%d", 4) == 0) {
                meta->port = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "x=%d", 4) == 0) {
                meta->xtra = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "c=%u", 4) == 0) {
                unsigned int chk = va_arg(args, unsigned int);
                meta->layer = BSL_LAYER_GET(chk);
                meta->source = BSL_SOURCE_GET(chk);
                meta->severity = BSL_SEVERITY_GET(chk);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "f=%s", 4) == 0) {
                meta->file = va_arg(args, char *);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "l=%d", 4) == 0) {
                meta->line = va_arg(args, int);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "F=%s", 4) == 0) {
                meta->func = va_arg(args, char *);
                fmt += 4;
            } else if (bsl_strncmp(fmt, "o=%u", 4) == 0) {
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
    if (meta->layer == BSL_LAY_COUNT) {
        meta->layer = BSL_LAY_APPL;
        meta->source = BSL_SRC_SHELL;
        meta->severity = BSL_SEV_INFO;
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

void
bsl_meta_t_init(bsl_meta_t *meta)
{
    bsl_memset(meta, 0, sizeof(*meta));
    meta->options = BSL_META_OPT_START | BSL_META_OPT_END;
    meta->layer = BSL_LAY_COUNT;
    meta->source = BSL_SRC_COUNT;
    meta->severity = BSL_SEV_COUNT;
    meta->unit = -1;
    meta->port = -1;
    meta->xtra = -1;
}
