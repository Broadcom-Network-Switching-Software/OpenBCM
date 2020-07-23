/*! \file bcma_testutil_phy.h
 *
 * PHY operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_PHY_H
#define BCMA_TESTUTIL_PHY_H

/*!
 * \brief Set PHY register.
 *
 * \param [in] unit Unit number.
 * \param [in] lport The logical port number.
 * \param [in] reg_name The register name.
 * \param [in] field_name The field name.
 * \param [in] val The field value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_phy_set(int unit, int lport, const char *reg_name,
                      const char *field_name, uint32_t val);


#endif /* BCMA_TESTUTIL_PHY_H */
