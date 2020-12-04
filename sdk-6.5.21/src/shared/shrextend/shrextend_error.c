/** \file shrextend_error.c
 *
 * Error message support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/shrextend/shrextend_error.h>
/*
 * Note:
 *   This file, bcm/error.h, is added here because JR2 uses the BCM API system
 *   of JR1 and the same error codes.
 */
#include <bcm/error.h>
/*
 * Pointers to procedures which are of the type of bsldnx_set_prefix_range_min_max()/
 * bsldnx_get_prefix_range_min_max().
 * These pointers are loaded at init and enable the DNX error-handling system to control
 * printing of prefix (file, line, function name, ...) on BSL log printout.
 * This is done using DIAG services. See bsldnx_mgmt_init().
 */
static BSLDNX_GET_PREFIX_RANGE_MIN_MAX get_prefix_range_min_max = (BSLDNX_GET_PREFIX_RANGE_MIN_MAX) 0;
static BSLDNX_SET_PREFIX_RANGE_MIN_MAX set_prefix_range_min_max = (BSLDNX_SET_PREFIX_RANGE_MIN_MAX) 0;
/*
 * Pointers to procedures which are of the type of bslenable_set()/bslenable_get().
 * These pointers are loaded at init and enable the DNX error-handling system to control
 * severity on BSL log printout.
 * This is done using DIAG services. See bsldnx_mgmt_init().
 */
static BSLENABLE_GET dnx_bslenable_get = (BSLENABLE_GET) 0;
static BSLENABLE_SET dnx_bslenable_set = (BSLENABLE_SET) 0;

void
set_proc_get_prefix_range_min_max(
    BSLDNX_GET_PREFIX_RANGE_MIN_MAX proc)
{
    get_prefix_range_min_max = proc;
    return;
}

void
set_proc_set_prefix_range_min_max(
    BSLDNX_SET_PREFIX_RANGE_MIN_MAX proc)
{
    set_prefix_range_min_max = proc;
    return;
}

void
get_proc_get_prefix_range_min_max(
    BSLDNX_GET_PREFIX_RANGE_MIN_MAX * proc)
{
    *proc = get_prefix_range_min_max;
    return;
}

void
get_proc_set_prefix_range_min_max(
    BSLDNX_SET_PREFIX_RANGE_MIN_MAX * proc)
{
    *proc = set_prefix_range_min_max;
    return;
}

void
set_proc_bslenable_get(
    BSLENABLE_GET proc)
{
    dnx_bslenable_get = proc;
    return;
}

void
set_proc_bslenable_set(
    BSLENABLE_SET proc)
{
    dnx_bslenable_set = proc;
    return;
}

void
get_proc_bslenable_get(
    BSLENABLE_GET * proc)
{
    *proc = dnx_bslenable_get;
    return;
}

void
get_proc_bslenable_set(
    BSLENABLE_SET * proc)
{
    *proc = dnx_bslenable_set;
    return;
}

const char *
shrextend_errmsg_get(
    shr_error_e rv)
{
    char *ret;

    ret = _SHR_ERRMSG(rv);
    return (ret);
}
