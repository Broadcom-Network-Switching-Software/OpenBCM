/** \file ptp_tune.h
 * 
 * 
 * Internal DNX ptp tune 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PTP_TUNE_H_INCLUDED_
/** { */
#define _PTP_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief
 *      ptp configurations in tune stage
 * \param [in] unit -unit id
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_ptp_tune_init(
    int unit);

#endif /*_PTP_TUNE_H_INCLUDED_*/
