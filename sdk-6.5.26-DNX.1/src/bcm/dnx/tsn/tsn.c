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
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_counter.h"
#include "tsn_taf.h"
#include "tsn_tas.h"
#include "tsn_cqf.h"
#include "tsn_thread.h"

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

    /** Initialize swstate db */
    SHR_IF_ERR_EXIT(dnx_tsn_db.init(unit));

    /** Initialize TSN counter */
    SHR_IF_ERR_EXIT(dnx_tsn_counter_init(unit));

    /** Initialize TAF */
    SHR_IF_ERR_EXIT(dnx_tsn_taf_init(unit));

    /** Initialize TAS */
    SHR_IF_ERR_EXIT(dnx_tsn_tas_init(unit));

    /** Initialize CQF */
    SHR_IF_ERR_EXIT(dnx_tsn_cqf_init(unit));

    /** Initialize TSN thread database */
    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_db_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 TAF (Time Aware Filtering)
 * ------------------------------------------------------------------------------------------
 */

/**
 * \brief -
 *  Allocate TAF Gate
 *
 * \param [in] unit - the relevant unit
 * \param [in] flags - use BCM_TSN_TAF_WITH_ID to allocate gate with ID
 * \param [out] taf_gate_id - allocated gate ID
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_create(unit, flags, taf_gate_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Free TAF Gate
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate_id - gate ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_destroy(unit, taf_gate_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Allocate TAF profile
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] profile - profile info
 * \param [out] pid - allocated profile ID
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_create(unit, taf_gate, profile, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Free TAF profile
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_destroy(unit, taf_gate, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set TAF profile info
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [in] profile - profile info
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_set(unit, taf_gate, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get TAF profile info
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [out] profile - profile info
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_get(unit, taf_gate, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Commit TAF profile to be written into HW
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_commit(unit, taf_gate, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get TAF profile status
 *
 * \param [in] unit - the relevant unit
 * \param [in] taf_gate - gate ID
 * \param [in] pid - profile ID to free
 * \param [out] status - profile status
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
int
bcm_dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_status_get(unit, taf_gate, pid, status));

exit:
    SHR_FUNC_EXIT;
}

/* Set/get TAF control configurations. */
int
bcm_dnx_tsn_taf_control_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_control_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/* Set/get TAF control configurations. */
int
bcm_dnx_tsn_taf_control_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_control_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 CQF (Cyclic Queuing and Forwarding)
 * ------------------------------------------------------------------------------------------
 */
/* Set cqf config */
int
bcm_dnx_tsn_cqf_config_set(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_set(unit, flags, port, config));

exit:
    SHR_FUNC_EXIT;
}

/* Get cqf config */
int
bcm_dnx_tsn_cqf_config_get(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_get(unit, flags, port, config));

exit:
    SHR_FUNC_EXIT;
}
