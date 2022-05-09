/** \file twamp.c
 * 
 *
 * TWAMP procedures for DNX2.
 * TWAMP functionality is unavailable for Jericho2
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TWAMP
#include <soc/sand/shrextend/shrextend_debug.h>

 /*
  * Include files.
  * {
  */

#include <bcm/types.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/twamp.h>

 /*
  * Global and Static
  */

/* Create or replace a TWAMP sender object. */
shr_error_e
bcm_dnx_twamp_sender_create(
    int unit,
    bcm_twamp_sender_info_t * twamp_sender_info)
{
    return _SHR_E_UNAVAIL;
}

/* Get an TWAMP sender object. */
shr_error_e
bcm_dnx_twamp_sender_get(
    int unit,
    bcm_twamp_sender_t twamp_id,
    bcm_twamp_sender_info_t * twamp_sender_info)
{
    return _SHR_E_UNAVAIL;
}

/* Get delay statistics of a TWAMP sender object. */
shr_error_e
bcm_dnx_twamp_sender_delay_get(
    int unit,
    bcm_twamp_sender_t twamp_id,
    bcm_twamp_sender_dm_statistics_t * twamp_sender_dm_statistics_info)
{
    return _SHR_E_UNAVAIL;
}

/* Get loss statistics of a TWAMP sender object. */
shr_error_e
bcm_dnx_twamp_sender_loss_get(
    int unit,
    bcm_twamp_sender_t twamp_id,
    bcm_twamp_sender_lm_statistics_t * twamp_sender_lm_statistics_info)
{
    return _SHR_E_UNAVAIL;
}

/* Destroy an TWAMP sender object. */
shr_error_e
bcm_dnx_twamp_sender_destroy(
    int unit,
    bcm_twamp_sender_t twamp_id)
{
    return _SHR_E_UNAVAIL;
}

/* Traverse the TWAMP sender nodes. */
shr_error_e
bcm_dnx_twamp_sender_traverse(
    int unit,
    uint32 flags,
    bcm_twamp_sender_traverse_cb cb,
    void *user_data)
{
    return _SHR_E_UNAVAIL;
}

