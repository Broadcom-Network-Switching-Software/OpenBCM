/*! \file variant.h
 *
 * SOC driver
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_LTSW_VARIANT_H
#define _SOC_LTSW_VARIANT_H

#if defined(BCM_LTSW_SUPPORT)

/*!
 * \brief Show the supported variant drivers.
 *
 * \retval None.
 */
extern void
soc_ltsw_variant_drivers_show(void);

/*!
 * \brief Get the variant name.
 *
 * \param [in]  unit          Unit number.
 * \param [out]  name         Variant name.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
soc_ltsw_variant_name_get(int unit, const char **name);

#endif
#endif  /* _SOC_LTSW_VARIANT_H */
