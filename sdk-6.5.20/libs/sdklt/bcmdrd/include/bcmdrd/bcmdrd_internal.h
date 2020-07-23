/*! \file bcmdrd_internal.h
 *
 * Internal DRD APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_INTERNAL_H
#define BCMDRD_INTERNAL_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_port.h>

#include <shr/shr_cht.h>

/*!
 * \brief BCMDRD Device structure.
 */
typedef struct bcmdrd_dev_s {

    /*! Device identification (typically PCI vendor/device ID). */
    bcmdrd_dev_id_t id;

    /*! Global chip flags. */
    uint32_t flags;

    /*! Device name (e.g. "BCM56801"). */
    const char *name;

    /*! Device type string (e.g. "bcm56800_a0"). */
    const char *type_str;

    /*! Device type (enumeration of supported devices). */
    bcmdrd_dev_type_t type;

    /*! I/O access functions and info */
    bcmdrd_hal_io_t io;

    /*! DMA access functions */
    bcmdrd_hal_dma_t dma;

    /*! Interrupt control API */
    bcmdrd_hal_intr_t intr;

    /*! Chip information structure. */
    const bcmdrd_chip_info_t *chip_info;

    /*! Port information array. */
    const bcmdrd_port_info_t *port_info;

    /*! Bit map of valid physical ports in the device. */
    bcmdrd_pbmp_t valid_ports;

    /*! Bit map of valid pipes in the device. */
    uint32_t valid_pipes;

    /*! Pipeline bypass mode (device-specific). */
    uint32_t bypass_mode;

    /*! Hash-based lookup for chip symbol modifiers. */
    shr_cht_t *chip_mod_cht;

} bcmdrd_dev_t;

/*!
 * \brief Get DRD device handle.
 *
 * \param [in] unit Unit number.
 *
 * \return DRD device handle or NULL if not found.
 */
extern bcmdrd_dev_t *
bcmdrd_dev_get(int unit);

#endif /* BCMDRD_INTERNAL_H */
