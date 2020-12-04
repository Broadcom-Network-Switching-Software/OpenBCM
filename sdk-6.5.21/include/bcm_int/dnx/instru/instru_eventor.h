/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains dnxInstrumentation module definitions internal to the BCM library.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef _INSTRU_EVENTOR_H_INCLUDED__
#define _INSTRU_EVENTOR_H_INCLUDED__

#include <bcm/types.h>

/**
 * \brief
 *  Eventor init function.
 *  Used to init the Eventor block at startup
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e dnx_instru_eventor_init(
    int unit);

#endif /* _INSTRU_EVENTOR_H_INCLUDED__ */
