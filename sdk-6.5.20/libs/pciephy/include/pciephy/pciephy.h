/*! \file pciephy.h
 *
 * PCIe diagnostic APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PCIEPHY_H_
#define _PCIEPHY_H_

#include <pciephy/pciephy_config.h>

/*! Maximum number of cores. */
#define PCIEPHY_MAX_CORES 4

/*! Maximum number of lanes per core. */
#define PCIEPHY_MAX_LANES_PER_CORE 4

/*! Driver types. */
typedef enum pciephy_driver_type_s {
    PCIEPHY_DRIVER_TYPE_MERLIN16,
    PCIEPHY_DRIVER_TYPE_MERLIN7,
    PCIEPHY_DRIVER_TYPE_COUNT
} pciephy_driver_type_t;

/*!
 * \brief Function definition for register read operations.
 *
 * \param [inout] user_acc Optional application data.
 * \param [in] addr Register address to read.
 * \param [out] val Read value.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to read.
 */
typedef int (*pciephy_acc_bus_read_f)(void* user_acc, uint32_t addr,
                                     uint16_t* val);

/*!
 * \brief Function definition for register write operations.
 *
 * \param [inout] user_acc Optional application data.
 * \param [in] addr Register address to write.
 * \param [in] val Value to write.
 *
 * \retval 0 No errors.
 * \retval -1 Failed to write.
 */
typedef int (*pciephy_acc_bus_write_f)(void* user_acc, uint32_t addr,
                                      uint16_t val);

/*!
 * PHY register access bus.
 */
typedef struct pciephy_acc_bus_s {
    /*! Bus name. */
    char* name;

    /*! Read handler. */
    pciephy_acc_bus_read_f read;

    /*! Write handler. */
    pciephy_acc_bus_write_f write;
} pciephy_acc_bus_t;

/*!
 * PCIE access object.
 */
typedef struct pciephy_access_s {
    /*! Optional application data - not used by PHY driver. */
    void* user_acc;

    /*! PHY bus driver. */
    pciephy_acc_bus_t *bus;

    /*! Driver type. */
    pciephy_driver_type_t type;

    /*! Switch unit. */
    int unit;

    /*! Serdes core number, 0 - 3, each core has 4 lanes. */
    uint8_t core;

    /*! Specific lanes bitmap. */
    uint8_t lane_mask;

    /*! Device id, not used, should be 1 for this device. */
    uint8_t _devid;

    /*! iproc_version of the device. */
    uint8_t iproc_ver;
} pciephy_access_t;

extern int
pciephy_diag_reg_read(pciephy_access_t *sa, uint16_t address, uint16_t *pdata);

extern int
pciephy_diag_reg_write(pciephy_access_t *sa, uint16_t address, uint16_t data);

extern int
pciephy_diag_reg_mwr(pciephy_access_t *sa, uint16_t address, uint16_t mask,
                      uint8_t lsb, uint16_t val);

extern int
pciephy_diag_pram_read(pciephy_access_t *sa, uint32_t address, uint32_t size);

extern int
pciephy_diag_dsc(pciephy_access_t *sa, int array_size);

extern int
pciephy_diag_state(pciephy_access_t *sa, int array_size);

extern int
pciephy_diag_eyescan(pciephy_access_t *sa, int array_size);

#endif /* _PCIEPHY_H_ */
