/*! \file bcmltx_ctr_eflex_quant_cfg_obj_map.h
 *
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * Transform handler header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CTR_EFLEX_QUANT_CFG_OBJ_MAP_H
#define BCMLTX_CTR_EFLEX_QUANT_CFG_OBJ_MAP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * forward transform
 *
 * \param [in]  unit            Unit number
 * \param [in]  in              BANK_OBJ field arrays.
 * \param [out] out             BANK_x_OBJECT x=[0,3] fields.
 * \param [in]  arg             Transform arguments
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_ctr_eflex_quant_cfg_obj_map_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

/*!
 * \brief
 * FLEX_CTR_ING/EGR_OBJECT_QUANTIZATION_CTRL.BANK_x_OBJECT
 * reverse transform
 *
 * \param [in]  unit            Unit number
 * \param [in]  in              BANK_x_OBJECT x=[0,3] fields.
 * \param [out] out             BANK_OBJ field arrays.
 * \param [in]  arg             Transform arguments
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_ctr_eflex_quant_cfg_obj_map_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CTR_EFLEX_QUANT_CFG_OBJ_MAP_H */
