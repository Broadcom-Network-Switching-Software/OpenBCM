/*! \file bcmcth_meter_fp_egr_imm.h
 *
 * This file contains EFP Meter Action Custom handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_EGR_IMM_H
#define BCMCTH_METER_FP_EGR_IMM_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * \brief Register EFP meter with IMM.
 *
 * Register with IMM to receive notifications for
 * EFP LT changes.
 *
 * \param [in] unit Unit number.
 */
extern int bcmcth_meter_fp_egr_imm_register(int unit);

/*!
 * \brief Cleanup EFP meter IMM DB.
 *
 * \param [in] unit Unit number.
 */
extern void bcmcth_meter_fp_egr_imm_db_cleanup(int unit);

/*!
 * \brief Get EFP meter field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_meter_fp_egr_template_fld_arr_hdl_get(int unit,
                                             shr_famm_hdl_t *hdl);

/*!
 * \brief Get EFP meter PDD field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_meter_fp_egr_pdd_fld_arr_hdl_get(int unit,
                                        shr_famm_hdl_t *hdl);

#endif /* BCMCTH_METER_FP_EGR_IMM_H */
