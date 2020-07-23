/*! \file bcmpc_phy.h
 *
 * BCMPC PHY APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PHY_H
#define BCMPC_PHY_H

#include <bcmpc/bcmpc_types_internal.h>

/*! Maximum PHY name size. */
#define BCMPC_PHY_NAME_MAX      30

/*! Value of ln index when users do not specified lane index on command line */
#define NO_LN_IDX_SPECIFIED 0xffffffff

/*!
 * \brief PHY information queried from \ref bcmpc_phy_info_get.
 */
typedef struct bcmpc_phy_info_s {

    /*! Phy address. */
    int addr;

    /*! Phy name. */
    char name[BCMPC_PHY_NAME_MAX];

    /*! Number of lane that phy supports. */
    int num_lanes;

    /*! PM id which the phy belongs. */
    int pm_id;

    /*! Bitmap of the phy. */
    uint32_t pbmp;

} bcmpc_phy_info_t;

/*!
 * \brief Override information used in PHY register read/write functions.
 */
typedef struct bcmpc_phy_acc_ovrr_s {

    /*! Override Lane index if non-zero. */
    uint32_t lane_index;

    /*! Override pll index if non-zero. */
    int pll_index;

} bcmpc_phy_acc_ovrr_t;

/*!
 * \brief Get the PHY information from the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] phy_info PHY information of the specified physical port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL Fail to get PHY information.
 */
extern int
bcmpc_phy_info_get(int unit, bcmpc_pport_t pport, bcmpc_phy_info_t *phy_info);

/*!
 * \brief Initialize the \ref bcmpc_phy_acc_ovrr_t structure.
 *
 * The \ref bcmpc_phy_acc_ovrr_t structure is used in \ref bcmpc_phy_reg_read
 * and \ref bcmpc_phy_reg_write functions for override options. The caller can
 * use this function to initialize the override structures.
 *
 * \param [in] ovrr Override structure to be initialized.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input \c ovrr is NULL.
 */
extern int
bcmpc_phy_acc_ovrr_t_init(bcmpc_phy_acc_ovrr_t *ovrr);

/*!
 * \brief Read PHY register data from a given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] reg_addr The PHY address to be read from.
 * \param [out] val The data returned from PHY register read.
 * \param [in] ovrr PHY access override data if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_reg_read(int unit, bcmpc_pport_t pport,
                   uint32_t reg_addr, uint32_t *val,
                   bcmpc_phy_acc_ovrr_t *ovrr);

/*!
 * \brief Write PHY register data to a given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] reg_addr The PHY address to be read from.
 * \param [in] val The data to be written to PHY register.
 * \param [in] ovrr PHY access override data if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_reg_write(int unit, bcmpc_pport_t pport,
                    uint32_t reg_addr, uint32_t val,
                    bcmpc_phy_acc_ovrr_t *ovrr);

/*!
 * \brief Read PHY register data from a given PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM id number.
 * \param [in] reg_addr The PHY address to be read from.
 * \param [out] val The data returned from PHY register read.
 * \param [in] ovrr PHY access override data if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_raw_reg_read(int unit, int pm_id,
                   uint32_t reg_addr, uint32_t *val,
                   bcmpc_phy_acc_ovrr_t *ovrr);

/*!
 * \brief Write PHY register data to a given PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM id number.
 * \param [in] reg_addr The PHY address to be read from.
 * \param [in] val The data to be written to PHY register.
 * \param [in] ovrr PHY access override data if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_raw_reg_write(int unit, int pm_id,
                       uint32_t reg_addr, uint32_t val,
                       bcmpc_phy_acc_ovrr_t *ovrr);

#endif /* BCMPC_PHY_H */
