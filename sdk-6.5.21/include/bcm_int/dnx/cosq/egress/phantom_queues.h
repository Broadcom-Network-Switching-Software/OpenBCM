/*
 * phantom_queues.h
 *
 *  Created on: Dec 16, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_PHANTOM_QUEUES_H_INCLUDED_
#define _DNX_PHANTOM_QUEUES_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * brief - callback function to print the template manager for phantom queues threshold profile
 *
 * \param [in] unit - Relevant unit.
 * \param [in] data - data to print. here it is the phantom queues threshold per profile
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
void dnx_cosq_ohantom_queues_profile_print_cb(
    int unit,
    const void *data);

#endif /* _DNX_PHANTOM_QUEUES_H_INCLUDED_ */
