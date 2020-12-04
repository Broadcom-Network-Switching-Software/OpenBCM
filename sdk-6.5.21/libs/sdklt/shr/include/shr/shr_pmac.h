/*! \file shr_pmac.h
 *
 * Shared address format for Port/MAC drivers.
 *
 * The address format is a uniform representation of a Port/MAC
 * register address. An access driver (typically a CMIC driver) will
 * convert the uniform representation into a suitable hardware
 * format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_PMAC_H
#define SHR_PMAC_H

/*!
 * \brief Port/MAC register types
 *
 * The types of Port/MAC register.
 */
typedef enum shr_pmac_reg_type_e {
    SHR_PMAC_REGTYPE_BLK = 0,   /*!< To access block register */
    SHR_PMAC_REGTYPE_PORT = 1,  /*!< To access port register */
    SHR_PMAC_REGTYPE_MEM = 2,   /*!< To access block memory */
    SHR_PMAC_REGTYPE_COUNT      /*!< Must be last */
} shr_pmac_reg_type_t;

/*!
 * \brief Macros for Port/MAC address format
 *
 * Portamc address is 32 bits and the layout is,
 * <b> acc_type (2b) | reg_type (2b) | reg_offset (28b) </b>
 *
 * \arg acc_type: Access Type
 * \arg reg_type: \ref shr_pmac_reg_type_e
 * \arg reg_offset: Register offset
 *
 * \{
 */
#define SHR_PMAC_ACC_TYPE_MASK   (0x3)          /*!< Access type mask */
#define SHR_PMAC_ACC_TYPE_OFFSET (30)           /*!< Access type offset */
#define SHR_PMAC_REG_TYPE_MASK   (0x3)          /*!< Register type mask */
#define SHR_PMAC_REG_TYPE_OFFSET (28)           /*!< Register type offset */
#define SHR_PMAC_REG_ADDR_MASK   (0xfffffff)    /*!< Register addr mask */
#define SHR_PMAC_REG_ADDR_OFFSET (0)            /*!< Register addr offset */
/*! \} */

/*!
 * \addtogroup PortMAC_Address_Manipulation
 *
 * \brief Macros for Port/MAC address mainpulations.
 *
 * Only decoding (GET) macros are required.
 *
 * \{
 */

/*! Get access type from the Port/MAC address */
#define SHR_PMAC_ACC_TYPE_GET(_addr) \
    ((_addr >> SHR_PMAC_ACC_TYPE_OFFSET) & SHR_PMAC_ACC_TYPE_MASK)

/*! Get register type from the Port/MAC address */
#define SHR_PMAC_REG_TYPE_GET(_addr) \
    ((_addr >> SHR_PMAC_REG_TYPE_OFFSET) & SHR_PMAC_REG_TYPE_MASK)

/*! Get register address from the Port/MAC address */
#define SHR_PMAC_REG_ADDR_GET(_addr) \
    ((_addr >> SHR_PMAC_REG_ADDR_OFFSET) & SHR_PMAC_REG_ADDR_MASK)

/*! \} */

#endif  /* SHR_PMAC_H */
