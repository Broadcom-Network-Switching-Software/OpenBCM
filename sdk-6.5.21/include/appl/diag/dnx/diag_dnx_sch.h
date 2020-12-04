/** \file diag_dnx_scheduler.h
 *
 * DNX TM scheduler diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_SCHEDULER_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_SCHEDULER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Return the actual credit rate of a given port HR
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] port_hr - Port HR for which we want to get the credit rate
 * \param [out] credit_rate_kbps - Actual credit rate of the current port HR
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int sh_dnx_scheduler_port_actual_credit_rate_get(
    int unit,
    int core,
    int port_hr,
    uint32 *credit_rate_kbps);

/**
 * \brief - Return the actual FC status of a given port HR
 *
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] port_hr - Port HR for which we want to get the FC status
 * \param [out] fc_count - Number of FC indications received by the port HR
 * \param [out] fc_percent - Percent of time that the HR is under FC
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int sh_dnx_scheduler_port_current_fc_info_get(
    int unit,
    int core,
    int port_hr,
    uint32 *fc_count,
    uint32 *fc_percent);

#endif /** DIAG_DNX_SCHEDULER_H_INCLUDED */
