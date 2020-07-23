/*! \file drv.h
 *
 * SOC driver
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_LTSW_DRV_H
#define _SOC_LTSW_DRV_H

#if defined(BCM_LTSW_SUPPORT)

/*!
 * \brief Detach a SOC device and deallocate all resources allocated.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
soc_ltsw_detach(int unit);

/*!
 * \brief Initialize the soc_control structure for a device
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Out of memory
 * \retval SHR_E_INTERNAL Failed to attach the unit
 */
extern int
soc_ltsw_attach(int unit);

/*!
 * \brief Get the base driver name for a device
 *
 * \param [in] unit Unit number.
 *
 * \retval SOC_E_NONE No errors.
 * \retval SOC_E_UNIT Invalid unit number.
 * \retval SOC_E_INTERNAL Failed to get the name.
 */
extern int
soc_ltsw_base_drv_name_get(int unit, const char ** name);

#endif
#endif  /* _SOC_LTSW_DRV_H */
