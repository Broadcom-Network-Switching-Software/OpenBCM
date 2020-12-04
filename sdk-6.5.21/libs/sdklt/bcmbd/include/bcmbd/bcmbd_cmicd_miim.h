/*! \file bcmbd_cmicd_miim.h
 *
 * Default CMICd MIIM driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_MIIM_H
#define BCMBD_CMICD_MIIM_H

#include <bcmbd/bcmbd_miim_internal.h>

/*!
 * \addtogroup CMICD_PHY_ADDR_FORMAT
 *
 * \brief CMICD PHY Address Format.
 *
 * The PHY address used in software is the same as it is in the CMIC MDIO
 * register.
 * <b> internal_bus (1b) | bus_number (3b) | clause_45 (1b) | phy_id (5b) </b>
 *
 * \{
 */

/*! PHY address in MII bus. */
#define BCMBD_CMICD_MIIM_PHY_ID        0x0000001f

/*! Select clause 45 access method. */
#define BCMBD_CMICD_MIIM_CLAUSE45      0x00000020

/*! Set internal bus number. */
#define BCMBD_CMICD_MIIM_IBUS(_b)      (((_b) << 6) | 0x200)

/*! Set external bus number. */
#define BCMBD_CMICD_MIIM_EBUS(_b)      ((_b) << 6)

/*! Get internal bus number. */
#define BCMBD_CMICD_MIIM_IBUS_NUM(_a)  ((_a & ~0x200) >> 6)

/*! \} */

/*!
 * \brief Initialize MIIM driver with the default CMICd MIIM driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicd_miim_drv_init(int unit);

/*!
 * \brief Set maximum number of MIIM buses.
 *
 * The MIIM_PARAM register layout differs depending of the number of
 * MIIM buses supported, so in order to encode the INTERNAL_SEL and
 * BUS_ID fields correctly, we need to know the maximum number of
 * buses supported for a given device.
 *
 * If \c max_bus is 8 or less, then we assume that the BUS_ID field is
 * 3 bits wide, otherwise we assume that it is 4 bits wide.
 *
 * \param [in] unit Unit number.
 * \param [in] max_bus Number of MIIM buses supported.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_miim_max_bus_set(int unit, int max_bus);

#endif /* BCMBD_CMICD_MIIM_H */
