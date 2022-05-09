/*
 * $Id: arrakis_stats.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * Description: arrakis_stats.h and arrakis_stats.c implements API for storing data in an SQL DB to be later used in Arrakis Web Application Stats module.
 * Purpose: This API is a wrapper for dnxc_regression_utils_print(). It relieves the user from having to
 *            format the samples string. Also providing validation to check that user input conforms with the Stat Family description in stats_syntax.xml.
 */

#ifndef ARRAKIS_STATS_H_INCLUDED
#define ARRAKIS_STATS_H_INCLUDED

#include <sal/types.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/sand/shrextend/shrextend_error.h>

#define ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN             200
#define ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN                20
#define ARRAKIS_STATS_SAMPLES_STR_MAX_LEN                             ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN - ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN
#define ARRAKIS_STATS_FAMILY_NAME_MAX_LEN                             100
#define ARRAKIS_STATS_DIMENSIONS_MAX_NOF                            10
#define ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN                        50

typedef struct
{

    char name[ARRAKIS_STATS_FAMILY_NAME_MAX_LEN];

    char dimensions[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN];

    char dimensions_parsing_name[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN];

    int dimensions_nof;

    char samples[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_SAMPLES_STR_MAX_LEN];

    int samples_nof[ARRAKIS_STATS_DIMENSIONS_MAX_NOF];
} arrakis_stat_family_t;

shr_error_e arrakis_stat_family_init(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *stat_family_name);

shr_error_e arrakis_stat_family_float_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    float sample);

shr_error_e arrakis_stat_family_int_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    int sample);

shr_error_e arrakis_stat_family_uint32_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    uint32 sample);

shr_error_e arrakis_stat_family_float_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    float *samples);

shr_error_e arrakis_stat_family_int_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    int *samples);

shr_error_e arrakis_stat_family_uint32_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    uint32 *samples);

shr_error_e arrakis_stat_family_commit(
    int unit,
    arrakis_stat_family_t * stat_family);

#endif
