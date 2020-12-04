/*! \file bcmcth_trunk_vp_imm.h
 *
 * This file contains device TRUNK_VP imm callback functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_IMM_H
#define BCMCTH_TRUNK_VP_IMM_H

/*!
 * \brief TRUNK_VP IMM callback register
 *
 * regist TRUNK_VP LT callback in IMM module
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid  Logic table ID.
 *
 * \retval None.
 */
extern int
bcmcth_trunk_vp_overlay_group_imm_register(int unit, bcmlrd_sid_t sid);

/*!
 * \brief Allocate itbm group resources on warmboot
 *        initalization of TRUNK_VP subcomponent.
 *
 * \param [in] unit unit number.
 *
 * Allocate itbm group used by the TRUNK_VP component
 * calling the itbm resource manager.
 *
 * \retval SHR_E_NONE on success.
 *         !SHR_E_NONE on failure.
 */
extern int
bcmcth_trunk_vp_wb_itbm_group_alloc(int unit);
#endif /* BCMCTH_TRUNK_VP_IMM_H */
