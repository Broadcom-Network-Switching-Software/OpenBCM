/*! \file bcmecmp_imm.h
 *
 * This file contains device ECMP imm callback functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_IMM_H
#define BCMECMP_IMM_H
/*!
 * ECMP LT IMM register function.
 */
extern int
bcmecmp_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP overlay LT IMM register function.
 */
extern int
bcmecmp_overlay_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP underlay LT IMM register function.
 */
extern int
bcmecmp_underlay_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP weighted LT IMM register function.
 */
extern int
bcmecmp_weighted_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP weighted underlay LT IMM register function.
 */
extern int
bcmecmp_weighted_underlay_group_imm_register(int unit, bcmlrd_sid_t sid);

/*!
 * ECMP weighted overlay LT IMM register function.
 */
extern int
bcmecmp_weighted_overlay_group_imm_register(int unit, bcmlrd_sid_t sid);

/*!
 * ECMP limit control LT IMM register function.
 */
extern int
bcmecmp_limit_imm_register(int unit);

/*!
 * \brief Pre-insert the ECMP LIMIT CONTROL LT with default values.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_MEMORY NULL memory.
 */
extern int
bcmecmp_limit_control_pre_insert(int unit);

/*!
 * ECMP level0 LT IMM register function.
 */
extern int
bcmecmp_lvl0_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP level1 LT IMM register function.
 */
extern int
bcmecmp_lvl1_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP level0 weighted LT IMM register function.
 */
extern int
bcmecmp_weighted_0_group_imm_register(int unit, bcmlrd_sid_t sid);
/*!
 * ECMP level1 weighted LT IMM register function.
 */
extern int
bcmecmp_weighted_1_group_imm_register(int unit, bcmlrd_sid_t sid);
#endif /* BCMECMP_IMM_H */
