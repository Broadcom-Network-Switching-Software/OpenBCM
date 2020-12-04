/*! \file bsl_stdout.h
 *
 * Broadcom System Log (BSL)
 *
 * Conveneince function for simple console output.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BSL_STDOUT_H
#define BSL_STDOUT_H

#include <bsl/bsl_ext.h>

static int bsl_log_level;

/* Simple output hook for core BSL configuration */
static inline int
bsl_out_hook(bsl_meta_t *meta, const char *format, va_list args)
{
    return vprintf(format, args);
}

/* Simple check hook for core BSL configuration */
static inline int
bsl_check_hook(bsl_packed_meta_t meta_pack)
{
    int source, severity;

    source = BSL_SOURCE_GET(meta_pack);
    severity = BSL_SEVERITY_GET(meta_pack);

    if (source == BSL_SRC_SHELL && severity <= BSL_SEV_INFO) {
        return 1;
    }
    if (severity <= bsl_log_level) {
        return 1;
    }
    return 0;
}

/*!
 * \brief Initialize BSL log to use stdout.
 *
 * This is a convenience function to quickly enable BSL log output
 * without setting up a framework for output sinks and enable
 * controls.
 *
 * If the \c log_level is <= 0, then the default level (BSL_SEV_WARN)
 * will be used.
 *
 * \param [in] log_level Desired log level.
 *
 * \return Always 0.
 */
static inline int
bsl_stdout_init(int log_level)
{
    bsl_config_t bsl_config;

    bsl_log_level = log_level;
    if (bsl_log_level <= 0) {
        /* Default log level */
        bsl_log_level = BSL_SEV_WARN;
    }

    /* Initialize system log output */
    bsl_config_t_init(&bsl_config);
    bsl_config.out_hook = bsl_out_hook;
    bsl_config.check_hook = bsl_check_hook;
    return bsl_init(&bsl_config);
}

#endif /* BSL_STDOUT_H */
