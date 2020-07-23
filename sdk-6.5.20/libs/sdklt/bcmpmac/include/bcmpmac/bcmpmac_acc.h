/*! \file bcmpmac_acc.h
 *
 * BCM PortMAC Access Driver.
 *
 * Declaration of the structures, enumerations, and functions to permit
 * management of a PortMAC on the server side of the SDK.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_ACC_H
#define BCMPMAC_ACC_H

#include <bcmpmac/bcmpmac_types.h>

/*!
 * \brief Function definition for register read operation.
 *
 * A function of this type is used to perform the actual hardware
 * access from a PMAC driver. The actual function is provided by the
 * application or some other higher software layer.
 *
 * \param [in,out] user_acc Optional application data.
 * \param [in] pm_port The port index in the block.
 * \param [in] addr Register address (see shr_pmac.h).
 * \param [in] idx Entry index.
 * \param [in] size Data size in bytes.
 * \param [out] data Data buffer.
 */
typedef int (*bcmpmac_reg_read_f)(void *user_acc, bcmpmac_pport_t pm_pport,
                                  uint32_t addr, uint32_t idx, size_t size,
                                  uint32_t *data);

/*!
 * \brief Function definition for register write operation
 *
 * A function of this type is used to perform the actual hardware
 * access from a PMAC driver. The actual function is provided by the
 * application or some other higher software layer.
 *
 * \param [in,out] user_acc Optional application data.
 * \param [in] pm_port The port index in the block.
 * \param [in] addr Register address (see shr_pmac.h).
 * \param [in] idx Entry index.
 * \param [in] size Data size in bytes.
 * \param [in] data Data buffer.
 */
typedef int (*bcmpmac_reg_write_f)(void *user_acc, bcmpmac_pport_t pm_pport,
                                   uint32_t addr, uint32_t idx, size_t size,
                                   uint32_t *data);

/*!
 * \brief The PortMAC register access method
 *
 * This data structure defines the register/memory access method.
 */
typedef struct bcmpmac_reg_access_s {
    char *name;                 /*!< the name of reg access driver*/
    bcmpmac_reg_read_f read;    /*!< read funcion */
    bcmpmac_reg_write_f write;  /*!< write funcion */
} bcmpmac_reg_access_t;

/*!
 * \brief The PortMAC access struct
 *
 * This data structure defines the information for PortMAC access
 */
typedef struct bcmpmac_access_s {
    void *user_acc;                     /*!< Optional application data */
    bcmpmac_reg_access_t *reg_acc;      /*!< PortMAC register access method */
} bcmpmac_access_t;

/*! Access field \c user_acc of \ref bcmpmac_access_t. */
#define PMAC_ACC_USER_ACC(_access) ((_access)->user_acc)

/*! Access field \c reg_acc of \ref bcmpmac_access_t. */
#define PMAC_ACC_BUS(_access) ((_access)->reg_acc)

#endif /* BCMPMAC_ACC_H */
