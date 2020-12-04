/*! \file uft_device.h
 *
 * UFT device specific info
 *
 * This file contains device specific info which are internal to UFT CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UFT_DEVICE_H
#define UFT_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_uft_internal.h>
#include "uft_internal.h"

/*******************************************************************************
 * Defines
 */
#define UFT_ALLOC(_ptr, _sz, _str) \
        SHR_ALLOC(_ptr, _sz, _str); \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY)

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Get UFT device specfic info
 *
 * \param [in] unit Device unit.
 *
 * \return ALPM device specfic info ptr
 */
extern uft_dev_info_t *
bcmptm_uft_device_info_get(int unit);

/*!
 * \brief Get UFT device specfic drivers
 *
 * \param [in] unit Device unit.
 *
 * \return ALPM device specfic driver ptr
 */
extern uft_driver_t *
bcmptm_uft_driver_get(int unit);

/*!
 * \brief Initialize uft related registers.
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_regs_init(int unit);

/*!
 * \brief Populate device info into sw data structure
 *
 * \param [in] unit Device unit.
 * \param [in] warm Warmboot flag.
 * \param [in] var_info Device variant info.
 * \param [out] dev Device info data structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_device_info_init(int unit,
                            bool warm,
                            const bcmptm_uft_var_drv_t *var_info,
                            uft_dev_info_t *dev);

/*!
 * \brief Validate for TABLE_EM_CONTROL LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] ctrl Uft_tbl_em_ctrl_t structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_tbl_em_ctrl_validate(int unit, uft_tbl_em_ctrl_t *ctrl);

/*!
 * \brief Validate for TABLE_EM_CONTROL LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] grp Uft_dev_em_grp_t structure
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_grp_validate(int unit, uft_dev_em_grp_t *grp);

/*!
 * \brief Update DEVICE_EM_GROUP LT.
 *
 * \param [in] unit Device unit.
 * \param [in] lt_id Logical table id.
 * \param [in] op_arg Operation arguments.
 * \param [in] grp Uft_dev_em_grp_t structure.
 * \param [out] pts_info New pt info of affected physical tables.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_grp_set(int unit, bcmltd_sid_t lt_id,
                          const bcmltd_op_arg_t *op_arg,
                          uft_dev_em_grp_t *grp);

/*!
 * \brief Validate for DEVICE_EM_BANK LT operation.
 *
 * \param [in] unit Device unit.
 * \param [in] bank Uft_dev_em_bank_t structure.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank);

/*!
 * \brief Update DEVICE_EM_BANK LT.
 *
 * \param [in] unit Device unit.
 * \param [in] lt_id Logical table id.
 * \param [in] op_arg Operation arguments.
 * \param [in] bank Uft_dev_em_bank_t structure.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_bank_set(int unit, bcmltd_sid_t lt_id,
                           const bcmltd_op_arg_t *op_arg,
                           uft_dev_em_bank_t *bank);

/*!
 * \brief Update DEVICE_EM_TILE LT.
 *
 * \param [in] unit Device unit.
 * \param [in] lt_id Logical table id.
 * \param [in] op_arg Operation arguments.
 * \param [in] tile Uft_dev_em_tile_t structure.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_dev_em_tile_set(int unit, bcmltd_sid_t lt_id,
                           const bcmltd_op_arg_t *op_arg,
                           uft_dev_em_tile_t *tile);


/*!
 * \brief Initialize RM UFT device module
 *
 * \param [in] unit Device unit.
 * \param [in] warm Warm boot flag.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_device_init(int unit, bool warm);

/*!
 * \brief Cleanup RM UFT device module
 *
 * \param [in] unit Device unit.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_device_cleanup(int unit);

#endif /* UFT_DEVICE_H */
