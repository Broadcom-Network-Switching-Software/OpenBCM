/*! \file bcmcth_meter_fp_ing_imm.h
 *
 * This file contains IFP Meter Action Custom handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_ING_IMM_H
#define BCMCTH_METER_FP_ING_IMM_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * \brief Register IFP meter with IMM.
 *
 * Register with IMM to receive notifications for
 * IFP LT changes.
 *
 * \param [in] unit Unit number.
 */
extern int bcmcth_meter_fp_ing_imm_register(int unit);

/*!
 * \brief Cleanup IFP meter IMM DB.
 *
 * \param [in] unit Unit number.
 */
extern void bcmcth_meter_fp_ing_imm_db_cleanup(int unit);

/*!
 * \brief Get IFP meter field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_meter_fp_ing_template_fld_arr_hdl_get(int unit,
                                             shr_famm_hdl_t *hdl);

/*!
 * \brief Get IFP meter PDD field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_meter_fp_ing_pdd_fld_arr_hdl_get(int unit,
                                        shr_famm_hdl_t *hdl);

#endif /* BCMCTH_METER_FP_ING_IMM_H */
