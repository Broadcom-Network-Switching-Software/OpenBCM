/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        debug.c
 * Purpose:	BCM API Debug declarations
 * Requires:    
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/cm.h>
#include <bcm/error.h>

#if defined(BROADCOM_DEBUG)

/*
 * API Debug Message Helper.
 * Called from the BCM_API macro in <bcm/debug.h>
 * The BCM_API macro is called from the bcm api dispatcher.
 */
void
_bcm_debug_api(int log_src, char *api,
	       int nargs, int ninargs,
	       int arg1, int arg2, int arg3, int rv)
{
    char	*rvstr;

    if (rv < 0) {
	rvstr = bcm_errmsg(rv);
    } else {
	rvstr = bcm_errmsg(BCM_E_NONE);
    }

    switch (ninargs) {
    case 0:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%s) -> %d %s\n"),
                     api,
                     nargs > 0 ? "..." : "",
                     rv, rvstr));
	break;
    case 1:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d%s) -> %d %s\n"),
                     api, arg1,
                     nargs > 1 ? ",..." : "",
                     rv, rvstr));
	break;
    case 2:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d,%d%s) -> %d %s\n"),
                     api, arg1, arg2,
                     nargs > 2 ? ",..." : "",
                     rv, rvstr));
	break;
    default:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d,%d,%d%s) -> %d %s\n"),
                     api, arg1, arg2, arg3,
                     nargs > 3 ? ",..." : "",
                     rv, rvstr));
	break;
    }
}

#define _BSL_LOG_NOCHECK(_meta,...)   \
    do {                              \
        _meta.file = BSL_FILE;        \
        _meta.line = BSL_LINE;        \
        _meta.func = BSL_FUNC;        \
        bsl_log(&_meta, __VA_ARGS__); \
  } while (0)

/*
 * API Debug Message Helper.
 * Called from the BCM_API macro in <bcm/debug.h>
 * The BCM_API macro is called from the bcm api dispatcher.
 * Note: This function sends BCM API debug message to bsl_log() with
 *       meta->xtra = BSL_LS_BCM_API | BSL_DEBUG and WITHOUT subjecting to
 *       SDK's logging level check.
 */
void
_bcm_debug_api_nocheck(char *api, int nargs, int ninargs,
                       int arg1, int arg2, int arg3, int rv)
{
    char *rvstr;
    bsl_meta_t meta;

    if (rv < 0) {
        rvstr = bcm_errmsg(rv);
    } else {
        rvstr = bcm_errmsg(BCM_E_NONE);
    }

    bsl_meta_t_init(&meta);
    meta.xtra = BSL_LS_BCMAPI_CUSTOM | BSL_DEBUG;

    switch (ninargs) {
    case 0:
        _BSL_LOG_NOCHECK(meta, "API: %s(%s) -> %d %s\n",
                         api,
                         nargs > 0 ? "..." : "",
                         rv, rvstr);
        break;
    case 1:
        _BSL_LOG_NOCHECK(meta, "API: %s(%d%s) -> %d %s\n",
                         api, arg1,
                         nargs > 1 ? ",..." : "",
                         rv, rvstr);
        break;
    case 2:
        _BSL_LOG_NOCHECK(meta, "API: %s(%d,%d%s) -> %d %s\n",
                         api, arg1, arg2,
                         nargs > 2 ? ",..." : "",
                         rv, rvstr);
        break;
    default:
        _BSL_LOG_NOCHECK(meta, "API: %s(%d,%d,%d%s) -> %d %s\n",
                         api, arg1, arg2, arg3,
                         nargs > 3 ? ",..." : "",
                         rv, rvstr);
        break;
    }
}

#undef _BSL_LOG_NOCHECK

#endif /* BROADCOM_DEBUG */
