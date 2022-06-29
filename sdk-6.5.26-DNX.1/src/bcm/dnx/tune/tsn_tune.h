/** \file tsn_tune.h
 * 
 * 
 * Internal DNX tsn tune
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef _TSN_TUNE_H_INCLUDED_
/** { */
#define _TSN_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/types.h>

/**
 * \brief
 *      TSN default values configured in port add
 * \param [in] unit -unit id
 * \param [in] port -logical port
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_tsn_tune_port_add(
    int unit,
    bcm_port_t port);

#endif /*_TSN_TUNE_H_INCLUDED_*/
