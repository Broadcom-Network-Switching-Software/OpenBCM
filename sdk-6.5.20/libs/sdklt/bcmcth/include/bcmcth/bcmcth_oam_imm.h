/*! \file bcmcth_oam_imm.h
 *
 * BCMCTH OAM IMM functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_IMM_H
#define BCMCTH_OAM_IMM_H

#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Initialize IMM callbacks.
 *
 * The OAM custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_oam_imm_init(int unit, bool warm);

/*!
 * \brief Checks whether a LT is an IMM mapped or not.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID of a LT.
 *
 * \retval TRUE IMM mapped LT.
 * \retval FALSE Not an IMM mapped LT.
 */
bool
bcmcth_oam_imm_mapped(int unit, const bcmlrd_sid_t sid);

#endif /* BCMCTH_OAM_IMM_H */
