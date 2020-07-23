/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log (bSL)
 *
 * BSL data types and macros
 */

#ifndef _SHR_BSLTYPES_H_
#define _SHR_BSLTYPES_H_

#include <stdarg.h>
#include <sal/types.h>
#include <shared/bslenum.h>

typedef uint32 bsl_packed_meta_t;

#define BSL_META_OPT_START      0x1
#define BSL_META_OPT_END        0x2
#define BSL_META_OPT_PFX_NL     0x4

/* Meta data to accompany each log message */
typedef struct bsl_meta_s {
    uint32 options;
    const char *func;
    const char *file;
    int line;
    bsl_layer_t layer;
    bsl_source_t source;
    bsl_severity_t severity;
    int unit;
    int port;
    int xtra; /* Additonal data such as COS queue */
} bsl_meta_t;

#define BSL_UNIT_UNKNOWN    (-1)

typedef int (*bsl_out_hook_f)(bsl_meta_t *, const char *fmt, va_list);
typedef int (*bsl_check_hook_f)(bsl_packed_meta_t);

typedef struct bsl_config_s {
    bsl_out_hook_f out_hook;
    bsl_check_hook_f check_hook;
} bsl_config_t;

#endif /* _SHR_BSLTYPES_H_ */

