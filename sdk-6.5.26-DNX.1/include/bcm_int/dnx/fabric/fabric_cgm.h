
/** \file fabric_cgm.h
 * 
 * 
 * Fabric CGM APIs
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __FABRIC_CGM_H_INCLUDED__
#define __FABRIC_CGM_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Index types of fabric cgm DBAL tables.
 */
typedef enum
{
    /** Access to dbal table doesn't requires index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE = 0,
    /** Access to dbal table with 'leaky bucket' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET,
    /** Access to dbal table with 'pipe' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE,
    /** Access to dbal table with 'level' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL,
    /** Access to dbal table with 'level separate' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL_SEPARATE,
    /** Access to dbal table with 'FMC shaper' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SHAPER,
    /** Access to dbal table with 'priority' index */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PRIORITY,
    /** Access to dbal table with 'level' and 'pipe' indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL,
    /** Access to dbal table with 'level' and 'pipe' indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LINK,
    /** Access to dbal table with 'fmc_shaper' and 'slow_start_phase' indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SLOW_START_PHASE,
    /** Access to dbal table with 'cast' and 'priority' indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_PRIORITY,
    /** Access to dbal table with 'cast' and 'level' indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_LEVEL,
    /** Access to dbal table with zone indexes */
    DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_ZONE
} dnx_fabric_cgm_control_index_type_e;

/**
 * \brief
 *   Initialize Fabric CGM module.
 * 
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_cgm_init(
    int unit);

/**
 * \brief
 *   Enable/Disable link's LLFC.
 * 
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [in] type -
 *   0 - disable LLFC.
 *   1 - normal LLFC.
 *   2 - credit based LLFC.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_cgm_llfc_type_enabled_hw_set(
    int unit,
    bcm_port_t link,
    int type);

/**
 * \brief
 *   Get whether link's LLFC is enabled.
 * 
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [in] enable -
 *   1 - enable LLFC.
 *   0 - disable LLFC.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_cgm_llfc_enable_hw_get(
    int unit,
    bcm_port_t link,
    int *enable);

/**
 * \brief
 *   Get LLFC type which is enabled.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [out] type
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_cgm_llfc_type_enabled_hw_get(
    int unit,
    bcm_port_t link,
    int *type);

/**
 * \brief
 *   Get adaptive shaper watermarks.
 *   common to both mnol and tfl mechanisms
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] dbal_table -
 *   DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_CONTROL
 *   DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL
 * \param [out] is_enabled -
 *   output value
 * \param [out] min_watermark -
 *   output value
 * \param [out] max_watermark -
 *   output value
 *
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_cgm_control_adaptive_watermark_factor_get(
    int unit,
    dbal_tables_e dbal_table,
    uint32 *is_enabled,
    uint32 *min_watermark,
    uint32 *max_watermark);

#endif /** __FABRIC_CGM_H_INCLUDED__ */
