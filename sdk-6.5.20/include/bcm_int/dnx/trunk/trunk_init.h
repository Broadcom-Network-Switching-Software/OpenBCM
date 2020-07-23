/** \file bcm_int/dnx/trunk/trunk_init.h
 * 
 * Internal DNX TRUNK INIT/DEINIT APIs to be used in init 
 * sequence 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_TRUNK_TRUNKINIT_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_TRUNK_TRUNKINIT_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - initialize trunk module, to be called from init 
 *        sequence.
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_init(
    int unit);

/**
 * \brief - de-init trunk module, to be called from init 
 *        sequence.
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_deinit(
    int unit);

/*
 * } 
 */
#endif /* _BCMINT_DNX_TRUNK_TRUNKINIT_H_INCLUDED */
