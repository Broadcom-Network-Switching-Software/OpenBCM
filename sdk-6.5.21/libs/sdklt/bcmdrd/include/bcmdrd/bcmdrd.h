/*! \file bcmdrd.h
 *
 * External BCMDRD Device API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_H
#define BCMDRD_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_hal.h>

/*!
 * \brief Device identification structure.
 *
 * This information defines a specific device variant, and the
 * information used to configure device drivers and features.
 */
typedef struct bcmdrd_dev_id_s {

    /*! Vendor ID (typically PCI vendor ID.) */
    uint16_t vendor_id;

    /*! Device ID (typically PCI device ID.) */
    uint16_t device_id;

    /*! Device revision (used to determine device features.) */
    uint16_t revision;

    /*! Additional identification (in case of ambiguous device IDs.) */
    uint16_t model;

} bcmdrd_dev_id_t;

/*!
 * \brief Check if a device type is supported.
 *
 * \param [in] id Device ID structure.
 *
 * \return true if supported, otherwise false.
 */
extern bool
bcmdrd_dev_supported(bcmdrd_dev_id_t *id);

/*!
 * \brief Destroy a device.
 *
 * Remove a device from the DRD and free all associated resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_INIT Device already destroyed (or never created).
 */
extern int
bcmdrd_dev_destroy(int unit);

/*!
 * \brief Create a device.
 *
 * Create a device in the DRD. This is a mandatory operation before
 * any other operation can be performed on the device.
 *
 * If a negative unit number is passed in, then the first available
 * unit number will be used. If no more unit numbers are available,
 * the function will return SHR_E_FULL.
 *
 * \param [in] unit unit number.
 * \param [in] id Device ID structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FULL No available unit numbers.
 * \retval SHR_E_EXIST Device with this unit number already exists.
 * \retval SHR_E_NOT_FOUND Device type not supported.
 */
extern int
bcmdrd_dev_create(int unit, bcmdrd_dev_id_t *id);

/*!
 * \brief Check if device exists.
 *
 * Check if a device has been created.
 *
 * \param [in] unit Unit number.
 *
 * \return true if device exists, otherwose false.
 */
extern bool
bcmdrd_dev_exists(int unit);

/*!
 * \brief Get device type.
 *
 * Get device type, which is an enumeration of all supported devices.
 *
 * \param [in] unit Unit number.
 *
 * \retval Device type.
 */
extern bcmdrd_dev_type_t
bcmdrd_dev_type(int unit);

/*!
 * \brief Get device type as a string.
 *
 * Get the device type as an ASCII string. If device does not exist,
 * an empty string ("") is returned.
 *
 * The device type string corresponds to the device type returned by
 * \ref bcmdrd_dev_type.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to base device name.
 */
extern const char *
bcmdrd_dev_type_str(int unit);

/*!
 * \brief Get device name.
 *
 * Get the official device (SKU) name. If device does not exist, an
 * empty string ("") is returned.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device name.
 */
extern const char *
bcmdrd_dev_name(int unit);

/*!
 * \brief Get device identification information.
 *
 * \param [in] unit Unit number.
 * \param [out] id Pointer to device identification structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device type not supported.
 */
extern int
bcmdrd_dev_id_get(int unit, bcmdrd_dev_id_t *id);

/*!
 * \brief Get ID structure for a given device name.
 *
 * Given a device name string, this API will look for a matching ID
 * structure in the list of supported devices.
 *
 * Mainly intended for testing and debugging.
 *
 * \param [in] dev_name Device name, e.g. "bcm56800_a0".
 * \param [out] id ID structure to be filled
 *
 * \retval SHR_E_NONE Match was found and ID structure was filled.
 * \retval SHR_E_NOT_FOUND No match was found.
 */
extern int
bcmdrd_dev_id_from_name(const char *dev_name, bcmdrd_dev_id_t *id);

/*!
 * \brief Check is the device is variant SKU.
 *
 * \param [in] unit Unit number.
 *
 * \retval true The unit is variant SKU.
 * \retval false The unit is base SKU.
 */
extern bool
bcmdrd_dev_is_variant_sku(int unit);

/*!
 * \brief Assign I/O resources to a device.
 *
 * I/O resources can be assigned either as one or more physical
 * addresses of memory-mappable register windows, or as a set of
 * system-provided register access functions.
 *
 * If physical I/O memory addresses are provided, then functions for
 * mapping and unmapping I/O registers must be provided as well.
 *
 * \param [in] unit Unit number.
 * \param [in] io I/O reousrce structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_io_init(int unit, bcmdrd_hal_io_t *io);

/*!
 * \brief Retrieve configured I/O resources for a device.
 *
 * Read back the I/O resource structure supplied via \ref
 * bcmdrd_dev_hal_io_init.
 *
 * This API is mainly for diagnostic purposes.
 *
 * \param [in] unit Unit number.
 * \param [out] io I/O reousrce structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_io_get(int unit, bcmdrd_hal_io_t *io);

/*!
 * \brief Assign DMA resources to a device.
 *
 * DMA resources are required for a number of device operations, and
 * to make this possible, the system must provide functions for
 * allocating and freeing DMA memory.
 *
 * \param [in] unit Unit number.
 * \param [in] dma DMA resource structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_dma_init(int unit, bcmdrd_hal_dma_t *dma);

/*!
 * \brief Assign interrupt control API to a device.
 *
 * Allow the SDK to connect an interrupt handler to a hardwrae
 * interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] intr Interrupt control API structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_NOT_FOUND Device does not exist.
 */
extern int
bcmdrd_dev_hal_intr_init(int unit, bcmdrd_hal_intr_t *intr);

#endif /* BCMDRD_H */

