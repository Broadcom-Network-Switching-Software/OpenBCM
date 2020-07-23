/*! \file bcmbd_pcie.h
 *
 * BD PCIE API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PCIE_H
#define BCMBD_PCIE_H

/*!
 * \brief Read PCIe core/MAC register.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register address.
 * \param [out] data Read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_pcie_core_reg_read(int unit, uint32_t addr, uint32_t *data);

/*!
 * \brief Write PCIe core/MAC register.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register address.
 * \param [in] data Data to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_pcie_core_reg_write(int unit, uint32_t addr, uint32_t data);

/*!
 * \brief PCIe firmware programming is done.
 *
 * This function is called after a new firmware image is upgraded to flash.
 * It will make the new firmware can take effect upon device reset.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_pcie_fw_prog_done(int unit);

#endif /* BCMBD_PCIE_H */
