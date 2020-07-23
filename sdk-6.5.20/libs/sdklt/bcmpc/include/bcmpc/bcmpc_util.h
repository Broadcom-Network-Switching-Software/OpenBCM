/*! \file bcmpc_util.h
 *
 *  BCMPC utilites.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_UTIL_H
#define BCMPC_UTIL_H


#include <phymod/phymod.h>
#include <bcmpc/bcmpc_types_internal.h>


/*!
 * \brief BCMPC user access data for PHY access bus.
 *
 * This structure is used as \c user_acc of \c phymod_access_t.
 *
 * This structure stores the extra data according to the host CPU interface to
 * construct the hardware dependent address.
 */
typedef struct bcmpc_phy_access_data_s {

    /*! Unit number. */
    int unit;

    /*! Physical port number. */
    bcmpc_pport_t pport;

} bcmpc_phy_access_data_t;

/*!
 * Convenient macro for setting BCMPC user access data
 * \ref bcmpc_phy_access_data_t \c _phy_data.
 */
#define BCMPC_PHY_ACCESS_DATA_SET(_phy_data, _u, _p) \
    do { \
        (_phy_data)->unit = (_u); \
        (_phy_data)->pport = (_p); \
    } while (0)

/*!
 * \brief Initialize the major data of \c phymod_phy_access_t used in BCMPC.
 *
 * The function will use the lane mask of the specifed physical port
 * as the default lane mask in \c phymod_access_t \c access of \c pa.
 * The lane mask of \c phymod_access_t can be override by passing a
 * non-zero value of \c ovrr_lane_mask.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ovrr_lane_mask Lane mask override value if non-zero.
 * \param [in] phy_data User data for \c phymod_phy_access_t.
 * \param [out] pa Pointer to \c phymod_phy_access_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Argument \c phy_data or \c pa is NULL.
 * \retval SHR_E_PORT Invalid physical port number.
 */
int
bcmpc_phymod_phy_access_t_init(int unit, bcmpc_pport_t pport,
                               uint32_t ovrr_lane_mask,
                               bcmpc_phy_access_data_t *phy_data,
                               phymod_phy_access_t *pa);

/*!
 * \brief Initialize the major data of \c phymod_core_access_t used in BCMPC.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] phy_data User data for \c phymod_core_access_t.
 * \param [out] ca Pointer to \c phymod_core_access_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Argument \c phy_data or \c ca is NULL.
 * \retval SHR_E_PORT Invalid physical port number.
 */
int
bcmpc_phymod_core_access_t_init(int unit, bcmpc_pport_t pport,
                                bcmpc_phy_access_data_t *phy_data,
                                phymod_core_access_t *ca);

#endif /* BCMPC_UTIL_H */
