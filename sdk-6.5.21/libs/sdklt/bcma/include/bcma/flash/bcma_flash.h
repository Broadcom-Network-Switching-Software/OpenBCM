/*! \file bcma_flash.h
 *
 * QSPI flash API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_FLASH_H
#define BCMA_FLASH_H

/*!
 * \brief Flash device structure.
 */
typedef struct bcma_flash_dev_s {

    /*! Vender ID. */
    uint8_t id;

    /*! Page size. */
    uint32_t page_size;

    /*! Sector size. */
    uint32_t sector_size;

    /*! Number of sectors. */
    uint32_t num_sectors;

    /*! Number of sectors. */
    uint32_t sector_type;

    /*! Flash size. */
    uint32_t size;

} bcma_flash_dev_t;


/*!
 * Set the flash device profile.
 *
 * Set the flash properities for the use of all flash operations.
 *
 * \param [in] unit Unit number.
 * \param [in] fdev Flash device profile.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcma_flash_dev_set(int unit, bcma_flash_dev_t *fdev);

/*!
 * Get the probed flash device information.
 *
 * \param [in] unit Unit number.
 * \param [out] fdev Flash device profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Flash is not found or not supported.
 */
extern int
bcma_flash_dev_probe(int unit, bcma_flash_dev_t *fdev);

/*!
 * Read QSPI flash.
 *
 * \param [in] unit Unit number.
 * \param [in] offset Flash memory offset.
 * \param [in] len Length in bytes.
 * \param [out] data Data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
extern int
bcma_flash_read(int unit, uint32_t offset, size_t len, uint8_t *data);

/*!
 * Write QSPI flash.
 *
 * \param [in] unit Unit number.
 * \param [in] offset Flash memory offset.
 * \param [in] len Length in bytes.
 * \param [in] data Data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
extern int
bcma_flash_write(int unit, uint32_t offset, size_t len, uint8_t *data);

/*!
 * Erase QSPI flash.
 *
 * \param [in] unit Unit number.
 * \param [in] offset Flash memory offset.
 * \param [in] len Length in bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
extern int
bcma_flash_erase(int unit, uint32_t offset, size_t len);

#endif /* BCMA_FLASH_H */
