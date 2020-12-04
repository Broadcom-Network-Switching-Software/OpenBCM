/*! \file bcma_bslcons.c
 *
 * Broadcom system log console sink.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcma/io/bcma_io_term.h>

#include <bcma/bsl/bcma_bslsink.h>
#include <bcma/bsl/bcma_bslcons.h>

/* Console sink object */
static bcma_bslsink_sink_t console_sink;

/* Variable to turn off the console output */
static int console_enabled = 1;

/* Variable to allow only CLI messages through */
static int cli_only = 0;

static int
bcma_bslcons_vfprintf(void *file, const char *format, va_list args)
{
    return bcma_io_term_vprintf(format, args);
}

static int
bcma_bslcons_check(bsl_meta_t *meta)
{
    if (cli_only) {
        return (console_enabled && meta->source == BSL_SRC_SHELL);
    }
    return console_enabled;
}

int
bcma_bslcons_init(void)
{
    bcma_bslsink_sink_t *sink;

    /* Create console sink */
    sink = &console_sink;
    bcma_bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "console", sizeof(sink->name));
    sink->vfprintf = bcma_bslcons_vfprintf;
    sink->check = bcma_bslcons_check;
    sink->enable_range.min = BSL_SEV_OFF + 1;
    sink->enable_range.max = BSL_SEV_COUNT - 1;
    sink->options = BCMA_BSLSINK_OPT_NO_ECHO;
    bcma_bslsink_sink_add(sink);

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "[%u] %f:%l %F(): %s: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = BSL_SEV_OFF + 1;
    sink->prefix_range.max = BSL_SEV_COUNT - 1;

    return 0;
}

int
bcma_bslcons_is_enabled(void)
{
    return console_enabled;
}

int
bcma_bslcons_enable(int enable)
{
    int cur_enable = bcma_bslcons_is_enabled();

    console_enabled = enable;

    return cur_enable;
}

int
bcma_bslcons_cli_only_get(void)
{
    return cli_only;
}

int
bcma_bslcons_cli_only_set(int new_val)
{
    int cur_val = bcma_bslcons_cli_only_get();

    cli_only = new_val;

    return cur_val;
}
