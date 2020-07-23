/*! \file pciephy_acc.h
 *
 * PCIe PHY access utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PCIEPHY_PHY_ACC_H_
#define _PCIEPHY_PHY_ACC_H_

/*!
 * \brief Check if the access bus is specified properly.
 *
 * \param [in] pa_ PHY access object.
 *
 * \retval true The access bus is specified properly.
 * \retval false The access bus is not specified properly.
 */
#define SRDS_BUS_VALIDATE(pa_) (pa_ && pa_->bus && \
                                pa_->bus->read && pa_->bus->write)

/*!
 * \brief Read register via the given access object.
 *
 * \param [in] pa_ PHY access object.
 * \param [in] r_ PHY register address.
 * \param [out] v_ PHY register value.
 */
#define SRDS_BUS_READ(pa_,r_,v_) pa_->bus->read(pa_,r_,v_)

/*!
 * \brief Write register via the given access object.
 *
 * \param [in] pa_ PHY access object.
 * \param [in] r_ PHY register address.
 * \param [in] v_ PHY register value.
 */
#define SRDS_BUS_WRITE(pa_,r_,v_) pa_->bus->write(pa_,r_,v_)

#endif /* _PCIEPHY_PHY_ACC_H_ */
