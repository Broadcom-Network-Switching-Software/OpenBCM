/*! \file bcmcth_l3_drv.h
 *
 * BCMCTH L3 ingress map Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L3_DRV_H
#define BCMCTH_L3_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Register l3 imm call back function.
 *
 * \param [in]  unit         Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l3_imm_register(int unit);

#endif /* BCMCTH_L3_DRV_H */
