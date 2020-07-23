/*! \file cth_alpm_device.h
 *
 * CTH_ALPM device specific info
 *
 * This file contains device specific info which are internal to CTH_ALPM CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CTH_ALPM_DEVICE_H
#define CTH_ALPM_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmptm/bcmptm_cth_alpm_internal.h>


/*******************************************************************************
 * Defines
 */
#define CTH_ALPM_DEV(u, m)             (cth_alpm_device_info[u][m])
#define CTH_ALPM_DRV(u, m)             (cth_alpm_driver[u][m])
#define CTH_ALPM_DRV_EXEC(u, m, fn)    (CTH_ALPM_DRV(u, m)->fn)

/*******************************************************************************
 * Typedefs
 */



/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get CTH_ALPM device specfic info
 *
 * \param [in] u Device u.
 *
 * \return ALPM device specfic info ptr
 */
extern const cth_alpm_device_info_t *
bcmptm_cth_alpm_device_info_get(int u, int m);

/*!
 * \brief Validate ALPM control.
 *
 * \param [in] u Device u.
 * \param [in] stage Stage.
 * \param [in] control ALPM control info
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_cth_alpm_device_control_validate(int u, int m, uint8_t stage,
                                        bcmptm_cth_alpm_control_t *control);

/*!
 * \brief Enclode ALPM control memory/registers.
 *
 * \param [in] u Device u.
 * \param [in] control ALPM control info
 * \param [in|out] control_pt Control physical table info
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_cth_alpm_device_control_encode(int u, int m,
                                      bcmptm_cth_alpm_control_t *control,
                                      cth_alpm_control_pt_t *control_pt);

/*!
 * \brief Cleanup RM CTH_ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_cth_alpm_device_cleanup(int u, int m);


/*!
 * \brief Initialize RM CTH_ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_cth_alpm_device_init(int u, int m);


#endif /* CTH_ALPM_DEVICE_H */
