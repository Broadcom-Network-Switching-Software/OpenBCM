/*! \file bcmbd_hotswap.h
 *
 * BD PCIE hotswap API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_HOTSWAP_H
#define BCMBD_HOTSWAP_H

/*!
 * \brief Enable/disable PCIe hot swap.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable/disable mode.
 *
 * \retval SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_hotswap_enable_set(int unit, bool enable);

/*!
 * \brief Get PCIe hot swap enabled status.
 *
 * \param [in] unit Unit number.
 * \param [out] enable Eanble/disable status.
 *
 * \retval SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_hotswap_enable_get(int unit, bool *enable);

#endif /* BCMBD_HOTSWAP_H */
