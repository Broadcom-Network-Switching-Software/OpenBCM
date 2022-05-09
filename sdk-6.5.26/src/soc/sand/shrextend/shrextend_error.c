
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <soc/sand/shrextend/shrextend_error.h>

#include <bcm/error.h>

static BSLDNX_GET_PREFIX_RANGE_MIN_MAX get_prefix_range_min_max = (BSLDNX_GET_PREFIX_RANGE_MIN_MAX) 0;
static BSLDNX_SET_PREFIX_RANGE_MIN_MAX set_prefix_range_min_max = (BSLDNX_SET_PREFIX_RANGE_MIN_MAX) 0;

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
