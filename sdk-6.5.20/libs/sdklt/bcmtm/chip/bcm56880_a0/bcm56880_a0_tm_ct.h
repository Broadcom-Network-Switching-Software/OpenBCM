/*! \file bcm56880_a0_tm_ct.h
 *
 * File containing cut-through related defines and internal function for
 * bcm56880_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_TM_CT_H
#define BCM56880_A0_TM_CT_H

#include <bcmpc/bcmpc_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>

/*!
 * \brief  Device specific cut-through initialization.
 *
 * \param [in] unit         Unit number.
 *
 * \retval SHR_E_NONE       No error.
 * \retval SHR_E_NOT_FOUND  Port not found.
 * \retval SHR_E_PARAM      Cut-through speed class invalid.
 */
extern int
bcm56880_a0_tm_ct_init(int unit);


/*!
 * \brief  Initialize device type based function pointers for cut-through.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  ct_drv       Cut-through driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcm56880_a0_tm_ct_drv_get(int unit, void *ct_drv);

#endif /* BCM56880_A0_TM_CT_H */
