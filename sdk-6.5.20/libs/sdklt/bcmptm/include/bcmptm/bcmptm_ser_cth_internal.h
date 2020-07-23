/*! \file bcmptm_ser_cth_internal.h
 *
 * Interfaces to get/set data from/to SER IMM LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_CTH_INTERNAL_H
#define BCMPTM_SER_CTH_INTERNAL_H

/*!
 * \brief Initialize SER (allocate and initialize resources etc.)
 *
 * Called during System Manager INIT state for each unit
 *
 * \param [in] unit   Unit number
 * \param [in] warm   TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_init(int unit, bool warm);

/*!
 * \brief Configure SER LT Link to IMM.
 *
 * Called during System Manager component configure state for each unit
 *
 * \param [in] unit   Unit number
 * \param [in] warm   TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_lt_comp_config(int unit, bool warm);

/*!
 * \brief Cleanup SER (Free resources).
 *
 * API should be called during system shutdown phase.
 *
 * \param [in] unit Unit number
 * \param [in] graceful TRUE => graceful shutdown, FALSE => fast
 * shutdown
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_cleanup(int unit, bool graceful);

#endif /* BCMPTM_SER_CTH_INTERNAL_H */
