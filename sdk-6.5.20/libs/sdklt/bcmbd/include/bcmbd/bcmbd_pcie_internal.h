/*! \file bcmbd_pcie_internal.h
 *
 * BD PCIE internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PCIE_INTERNAL_H
#define BCMBD_PCIE_INTERNAL_H

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
typedef int (*bcmbd_pcie_core_reg_read_f)(int unit, uint32_t addr,
                                          uint32_t *data);

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
typedef int (*bcmbd_pcie_core_reg_write_f)(int unit, uint32_t addr,
                                           uint32_t data);

/*!
 * \brief PCIe firmware programming is done.
 *
 * \ref bcmbd_pcie_fw_prog_done.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
typedef int (*bcmbd_pcie_fw_prog_done_f)(int unit);

/*!
 * \brief PCIE driver object.
 */
typedef struct bcmbd_pcie_drv_s {

    /*! Read PCIe core/MAC register. */
    bcmbd_pcie_core_reg_read_f read;

    /*! Write PCIe core/MAC register. */
    bcmbd_pcie_core_reg_write_f write;

    /*! Firmware programming done. */
    bcmbd_pcie_fw_prog_done_f fw_prog_done;

} bcmbd_pcie_drv_t;

/*!
 * Initialize PCIE driver.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver for the given device.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_pcie_drv_init(int unit, bcmbd_pcie_drv_t *drv);

#endif /* BCMBD_PCIE_INTERNAL_H */
