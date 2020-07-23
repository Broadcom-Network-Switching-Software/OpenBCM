/*! \file bcma_pcie.h
 *
 * BCMA PCIe functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_PCIECMD_H
#define BCMA_PCIECMD_H

#include <pciephy/pciephy.h>

/*!
 * \brief Get the PCIe PHY type.
 *
 * \param [in] unit Unit number.
 * \param [out] type PHY type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
typedef int
(*bcma_pcie_phy_type_get_f)(int unit, pciephy_driver_type_t *type);

/*!
 * \brief Get the PCIe loader trace buffer.
 *
 * \param [in] unit Unit number.
 * \param [out] idx Buffer index.
 * \param [out] val Buffer value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
typedef int
(*bcma_pcie_trace_get_f)(int unit, uint32_t idx, uint32_t *val);

/*!
 * \brief Get the PCIe firmware version.
 *
 * \param [in] pa PHY access object.
 * \param [out] ver Firmware version (0 means no firmware).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
typedef int
(*bcma_pcie_fw_ver_get_f)(pciephy_access_t *pa, uint16_t *ver);

/*!
 * \brief Chip-specific PCIe driver.
 */
typedef struct bcma_pcie_drv_s {
    /*! Function to get the PHY type. */
    bcma_pcie_phy_type_get_f phy_type_get;

    /*! Function to get the firmware version. */
    bcma_pcie_fw_ver_get_f fw_ver_get;

    /*! Function to read trace buffer. */
    bcma_pcie_trace_get_f trace_get;
} bcma_pcie_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcma_pcie_drv_t* _bd##_bcma_pcie_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Setup the PCIE driver.
 *
 * \param[in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
extern int
bcma_pcie_drv_init(int unit);

/*!
 * \brief Get the PCIe PHY type.
 *
 * \param [in] unit Unit number.
 * \param [out] type PHY type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
extern int
bcma_pcie_phy_type_get(int unit, pciephy_driver_type_t *type);

/*!
 * \brief Get the PCIe firmware version.
 *
 * \param [in] pa PHY access object.
 * \param [out] ver Firmware version (0 means no firmware).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
extern int
bcma_pcie_fw_ver_get(pciephy_access_t *pa, uint16_t *ver);

/*!
 * \brief Get the PCIe loader trace buffer.
 *
 * \param [in] unit Unit number.
 * \param [out] idx Buffer index.
 * \param [out] val Buffer value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Operation failure.
 */
int
bcma_pcie_trace_get(int unit, uint32_t idx, uint32_t *val);

#endif /* BCMA_PCIECMD_H */
