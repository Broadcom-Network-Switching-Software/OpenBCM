/**
 * \file tsn.c
 * 
 *
 * TSN functionality for DNX.
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <bcm_int/dnx/tsn/tsn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_counter.h"

/*
 * }
 */

/*
 * Defines:
 * {
 */

 /*
  * }
  */

/*
 * See .h file
 */
shr_error_e
dnx_tsn_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize TSN counter */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 TAF (Time Aware Filtering)
 * ------------------------------------------------------------------------------------------
 */

/* Create/Destroy TAF gate */
int
bcm_dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Create/Destroy TAF gate */
int
bcm_dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Create the TAF profile */
int
bcm_dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Destroy the TAF profile */
int
bcm_dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Get/set TAF profile information */
int
bcm_dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Get/set TAF profile information */
int
bcm_dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Commit the TAF profile */
int
bcm_dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Get the TAF profile status */
int
bcm_dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
