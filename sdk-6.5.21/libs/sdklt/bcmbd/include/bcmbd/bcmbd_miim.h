/*! \file bcmbd_miim.h
 *
 * MIIM Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MIIM_H
#define BCMBD_MIIM_H

#include <bcmdrd/bcmdrd_types.h>

/*! Types of MIIM operations. */
typedef enum bcmbd_miim_opcode_e {
    BCMBD_MIIM_OPC_NONE = 0,
    BCMBD_MIIM_OPC_CL22_READ,
    BCMBD_MIIM_OPC_CL22_WRITE,
    BCMBD_MIIM_OPC_CL45_READ,
    BCMBD_MIIM_OPC_CL45_WRITE,
    BCMBD_MIIM_OPC_COUNT
} bcmbd_miim_opcode_t;

/*! Generic MIIM operation type. */
typedef struct bcmbd_miim_op_s {

    /*! Type of MIIM operation to perform. */
    bcmbd_miim_opcode_t opcode;

    /*! MIIM hardware channel (use -1 to auto-select). */
    int chan;

    /*! Set to true to select internal MIIM bus(es). */
    bool internal;

    /*! Bus number (unused if \c busmap is non-zero). */
    uint32_t busno;

    /*!
     * Bitmap of buses on which to perform the operation. Used for
     * broadcasting to multiple PHYs. Should only be used for write
     * operations. Overrides \c busno if non-zero.
     */
    uint32_t busmap;

    /*! PHY address on MIIM bus. */
    uint32_t phyad;

    /*! Clause 45 PHY device number. */
    uint32_t devad;

    /*! PHY register address. */
    uint32_t regad;

} bcmbd_miim_op_t;

/*!
 * The MIIM data rate is calculated as a fraction of a base frequency,
 * and this structure defines the dividend and divisor of this
 * fraction.
 */
typedef struct bcmbd_miim_rate_config_s {

    /*!
     * The MIIM data rate is calculated as a fraction of a base
     * frequency, and this field defines the dividend of this fraction.
     *
     * If this field is 0, then a default value of 1 will be used
     * instead.
     */
    uint32_t dividend;

    /*!
     * The MIIM data rate is calculated as a fraction of a base
     * frequency, and this field defines the divisor of this fraction.
     *
     * If this field is 0, then a default value of 1 will be used
     * instead.
     */
    uint32_t divisor;

} bcmbd_miim_rate_config_t;

/*! Set PHYAD field in PHY address word. */
#define BCMBD_MIIM_PHY_ID_SET(_a, _v)   F32_SET(_a, 0, 5, _v)
/*! Get PHYAD field in PHY address word. */
#define BCMBD_MIIM_PHY_ID_GET(_a)       F32_GET(_a, 0, 5)

/*! Set MIIM bus number field in PHY address word. */
#define BCMBD_MIIM_PHY_BUS_SET(_a, _v)  F32_SET(_a, 5, 4, _v)
/*! Get MIIM bus number field in PHY address word. */
#define BCMBD_MIIM_PHY_BUS_GET(_a)      F32_GET(_a, 5, 4)

/*! Set clause 45 access bit in PHY address word. */
#define BCMBD_MIIM_PHY_C45_SET(_a, _v)  F32_SET(_a, 14, 1, _v)
/*! Get clause 45 access bit in PHY address word. */
#define BCMBD_MIIM_PHY_C45_GET(_a)      F32_GET(_a, 14, 1)

/*! Set internal bus selction bit in PHY address word. */
#define BCMBD_MIIM_PHY_INT_SET(_a, _v)  F32_SET(_a, 15, 1, _v)
/*! Get internal bus selction bit in PHY address word. */
#define BCMBD_MIIM_PHY_INT_GET(_a)      F32_GET(_a, 15, 1)

/*! Set multicast write bitmap in PHY address word. */
#define BCMBD_MIIM_PHY_MC_SET(_a, _v)   F32_SET(_a, 16, 8, _v)
/*! Get multicast write bitmap in PHY address word. */
#define BCMBD_MIIM_PHY_MC_GET(_a)       F32_GET(_a, 16, 8)

/*! Set MIIM channel number field in PHY address word. */
#define BCMBD_MIIM_PHY_CH_SET(_a, _v)   F32_SET(_a, 28, 4, _v)
/*! Get MIIM channel number field in PHY address word. */
#define BCMBD_MIIM_PHY_CH_GET(_a)       F32_GET(_a, 28, 4)

/*! Set register offset field in PHY register word. */
#define BCMBD_MIIM_REGAD_SET(_r, _v)    F32_SET(_r, 0, 16, _v)
/*! Get register offset field in PHY register word. */
#define BCMBD_MIIM_REGAD_GET(_r)        F32_GET(_r, 0, 16)

/*! Set PHY device number field in PHY register word. */
#define BCMBD_MIIM_DEVAD_SET(_r, _v)    F32_SET(_r, 16, 5, _v)
/*! Get PHY device number field in PHY register word. */
#define BCMBD_MIIM_DEVAD_GET(_r)        F32_GET(_r, 16, 5)

/*!
 * \brief Initialize MIIM driver.
 *
 * Allocates locks and other resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient resources.
 */
extern int
bcmbd_miim_init(int unit);

/*!
 * \brief Clean up MIIM driver.
 *
 * Free resources allocated by \ref bcmbd_miim_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_miim_cleanup(int unit);

/*!
 * \brief Perform MIIM operation.
 *
 * This function will perform a read or write access on the specified
 * MIIM bus using either clause 22 or clause 45 access.
 *
 * \param [in] unit Unit number.
 * \param [in] op Structure with MIIM operation parameters.
 * \param [in,out] data Data to write or data read.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation did not complete within normal time.
 */
extern int
bcmbd_miim_op(int unit, bcmbd_miim_op_t *op, uint32_t *data);

/*!
 * \brief Configure MIIM data rate parameters.
 *
 * If \c busno is -1, then the configuration will be applied to all
 * MIIM buses. Some device types only support this option as all buses
 * share the same hardware configuration.
 *
 * Note that the data rate calculation is device-specific based on
 * available clock sources and dividers.
 *
 * \param [in] unit Unit number.
 * \param [in] internal Apply configuration to internal MIIM bus.
 * \param [in] busno MIIM bus number (use -1 for all).
 * \param [in] ratecfg MIIM data rate configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Unsupported MIIM bus parameter.
 */
extern int
bcmbd_miim_rate_config_set(int unit, bool internal, int busno,
                           bcmbd_miim_rate_config_t *ratecfg);

/*!
 * \brief Read a single MIIM register.
 *
 * This is a convenience function which will eventually call \ref
 * bcmbd_miim_op.
 *
 * \param [in] unit Unit number.
 * \param [in] phy PHY address (see BCMBD_MIIM_PHY_xxx macros).
 * \param [in] reg Register address (see BCMBD_MIIM_REGAD/DEVAD_xxx).
 * \param [out] val Register value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation did not complete within normal time.
 */
extern int
bcmbd_miim_read(int unit, uint32_t phy, uint32_t reg, uint32_t *val);

/*!
 * \brief Write a single MIIM register.
 *
 * This is a convenience function which will eventually call \ref
 * bcmbd_miim_op.
 *
 * \param [in] unit Unit number.
 * \param [in] phy PHY address (see BCMBD_MIIM_PHY_xxx macros).
 * \param [in] reg Register address (see BCMBD_MIIM_REGAD/DEVAD_xxx).
 * \param [in] val Register value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation did not complete within normal time.
 */
extern int
bcmbd_miim_write(int unit, uint32_t phy, uint32_t reg, uint32_t val);

#endif /* BCMBD_MIIM_H */
