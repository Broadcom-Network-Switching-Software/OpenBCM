/*
 * \file bcm_int/dnx/cosq/ingress/fmq.h
 * 
 *
 *  Created on: Nov 8, 2018
 *      Author: si888124
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_FMQ_INCLUDED__
#define _DNX_FMQ_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - Set FMQ scheduler mode (Enhanced/ Basic)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] mode - FMQ scheduler mode
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_scheduler_mode_set(
    int unit,
    int mode);

/**
 * \brief - Get FMQ scheduler mode (Enhanced/ Basic)
 *
 * \param [in] unit -  Unit-ID
 * \param [out] mode - FMQ scheduler mode
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_scheduler_mode_get(
    int unit,
    int *mode);

#endif /* _DNX_FMQ_INCLUDED__ */
