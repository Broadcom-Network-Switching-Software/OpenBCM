/*! \file bcmcth_phb_egr_config.h
 *
 * BCMCTH PHB egress map top-level APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PHB_EGR_CONFIG_H
#define BCMCTH_PHB_EGR_CONFIG_H

/*!
 * \brief Initialize device-specific data.
 *
 * \param [in] unit Unit nunmber.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_egr_botp_init(int unit);

#endif /* BCMCTH_PHB_EGR_CONFIG_H */
