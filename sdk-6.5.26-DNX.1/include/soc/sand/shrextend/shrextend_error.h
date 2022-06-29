
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef SHREXTEND_ERROR_H_INCLUDED

#define SHREXTEND_ERROR_H_INCLUDED

#include <shared/error.h>
#include <shared/bslenum.h>

typedef int (
    *BSLDNX_GET_PREFIX_RANGE_MIN_MAX) (
    bsl_severity_t * min_severity,
    bsl_severity_t * max_severity);

typedef int (
    *BSLDNX_SET_PREFIX_RANGE_MIN_MAX) (
    bsl_severity_t min_severity,
    bsl_severity_t max_severity);

typedef bsl_severity_t(
    *BSLENABLE_GET) (
    bsl_layer_t layer,
    bsl_source_t source);

typedef void (
    *BSLENABLE_SET) (
    bsl_layer_t layer,
    bsl_source_t source,
    bsl_severity_t severity);

typedef _shr_error_t shr_error_e;

#define SHR_SUCCESS(_expr) ((_expr) >= 0)

#define SHR_FAILURE(_expr) ((_expr) < 0)

const char *shrextend_errmsg_get(
    int rv);

void set_proc_get_prefix_range_min_max(
    BSLDNX_GET_PREFIX_RANGE_MIN_MAX proc);

void set_proc_set_prefix_range_min_max(
    BSLDNX_SET_PREFIX_RANGE_MIN_MAX proc);

void get_proc_get_prefix_range_min_max(
    BSLDNX_GET_PREFIX_RANGE_MIN_MAX * proc);

void get_proc_set_prefix_range_min_max(
    BSLDNX_SET_PREFIX_RANGE_MIN_MAX * proc);

void set_proc_bslenable_get(
    BSLENABLE_GET proc);

void set_proc_bslenable_set(
    BSLENABLE_SET proc);

void get_proc_bslenable_get(
    BSLENABLE_GET * proc);

void get_proc_bslenable_set(
    BSLENABLE_SET * proc);

#endif
