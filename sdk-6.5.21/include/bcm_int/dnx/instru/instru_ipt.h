/**
 * \file bcm_int/dnx/instru/instru_ipt.h
 * Internal DNX INSTRU APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INSTRU_IPT_H_INCLUDED
/*
 * {
 */
#define INSTRU_IPT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/**
 * \brief - Set Node ID for IPT.
 *
 * \param [in] unit - unit id.
 * \param [in] node_id - IPT node ID
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_instru_ipt_node_id_set(
    int unit,
    int node_id);

/**
 * \brief - Get Node ID for IPT.
 *
 * \param [in] unit - unit id.
 * \param [out] node_id - IPT node ID
 *
 * \return
 *   shr_error_e
 */
shr_error_e dnx_instru_ipt_node_id_get(
    int unit,
    int *node_id);

/**
 * \brief -
 * Init procedure for intru ipt module
 *
 * \param [in] unit -  Unit-ID
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ipt_init(
    int unit);

/**
 * \brief - Enable/disable ETPP trapping to recycle interface IPT packets which are destined to gport (egress port).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] gport -  destination port gport
 * \param [in] arg -  1 enable, 0 disable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ipt_trap_to_rcy_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int arg);

/**
 * \brief - Get whether ETPP trapping to recycle interface of IPT packets is Enabled/disable for gport (egress port).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] gport -  destination port gport
 * \param [in] arg -  1 enable, 0 disable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ipt_trap_to_rcy_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *arg);

/**
 * \brief - Enable/disable ETPP trapping to recycle interface with PTCH-1 of IFA packets which are destined to gport (egress port).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] gport -  destination port gport
 * \param [in] arg -  1 enable, 0 disable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ifa_initiator_rcy_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int arg);

/**
 * \brief - Get whether ETPP trapping to recycle interface with PTCH-1 of IFA packets is Enabled/disable for gport (egress port).
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags -  flags
 * \param [in] gport -  destination port gport
 * \param [in] arg -  1 enable, 0 disable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_ifa_initiator_rcy_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *arg);

/*
 * }
 */
#endif /* INSTRU_IPT_H_INCLUDED */
