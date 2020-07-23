/** \file fabric_transmit.h
 *
 * Functions for handling Fabric transmit adapter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_TRANSMIT_INCLUDED__
/* { */
#define __DNX_FABRIC_TRANSMIT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

/*************
 * INCLUDES  *
 */
/* { */

#include <shared/shrextend/shrextend_error.h>

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

/**
 * \brief
 *   Initialize Fabric Transmit Adapter configurations,
 *   including DTQs, PDQs, etc.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_transmit_init(
    int unit);

/**
 * \brief
 *   Initialize DTQs contexts.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   This function is called as part of the init sequence.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_dtqs_contexts_init(
    int unit);

/**
 * \brief
 *   Configure PDQs to DQCQs FC thresholds using the 'select' mechanism.
 *   Each FC sets to one of predefined FC thresholds.
 * \param [in] unit -
 *   The unit number.
 * \param [in] min_low_dqcq_priority -
 *   Maximal low priority in DQCQ
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Can be used only when 8 priorities dqcq is supported.
 */
shr_error_e dnx_fabric_transmit_dqcq_priority_update(
    int unit,
    int min_low_dqcq_priority);

/* } */

/* } __DNX_FABRIC_TRANSMIT_INCLUDED__*/
#endif
