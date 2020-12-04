/*! \file bcma_bcmbd_qspi.h
 *
 * BCMBD QSPI CLI shared functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBD_QSPI_H
#define BCMA_BCMBD_QSPI_H

/*!
 * Initialize QSPI.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to initialize QSPI.
 */
extern int
bcma_bcmbd_qspi_init(int unit);

/*!
 * Read the flash data from flash.
 *
 * \param [in] unit Unit number.
 * \param [in] offset Flash memory offset.
 * \param [in] len Length in bytes.
 * \param [out] data Data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to read flash.
 */
extern int
bcma_bcmbd_qspi_flash_read(int unit, uint32_t offset, uint32_t len,
                           uint8_t *data);

/*!
 * Load file content to flash device.
 *
 * \param [in] unit Unit number.
 * \param [in] offset Flash memory offset.
 * \param [in] fname File to get the data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to load data into flash.
 */
extern int
bcma_bcmbd_qspi_flash_load(int unit, uint32_t offset, const char *fname);

#endif /* BCMA_BCMBD_QSPI_H */
