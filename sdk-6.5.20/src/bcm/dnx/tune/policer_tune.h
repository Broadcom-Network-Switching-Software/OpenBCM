/** \file policer_tune.h
 * 
 * 
 * Internal DNX Port APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _POLICER_TUNE_H_INCLUDED_
/** { */
#define _POLICER_TUNE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief
 *      policer configurations in tune stage
 * \param [in] unit -unit id
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_policer_tune_init(
    int unit);

#endif /*_POLICER_TUNE_H_INCLUDED_*/
