/*
 * 
 * fmq_dbal.h
 *
 *  Created on: Nov 1, 2018
 *      Author: si888124
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.*
 */

#ifndef FMQ_DBAL_H_INCLUDED
#define FMQ_DBAL_H_INCLUDED

/*
 * INCLUDE FILES:
 * {
 */
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/**
 * \brief - Set credit distribution mode between FMQ class. (WFQ / SP)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] is_wfq - is mode WFQ (otherwise its SP)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_credit_weight_mode_set(
    int unit,
    uint32 is_wfq);

/**
 * \brief - Get credit distribution mode between FMQ class. (WFQ / SP)
 *
 * \param [in] unit -  Unit-ID
 * \param [out] is_wfq - is mode WFQ (otherwise its SP)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_credit_weight_mode_get(
    int unit,
    uint32 *is_wfq);

/**
 * \brief - Set credit distribution weight between FMQ class. (only in WFQ mode)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] fmq_class_field_id - FMQ class DBAL field
 * \param [in] weight - weight to set to the FMQ class
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_credit_weight_set(
    int unit,
    dbal_fields_e fmq_class_field_id,
    uint32 weight);

/**
 * \brief - Get credit distribution weight between FMQ class. (only in WFQ mode)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] fmq_class_field_id - FMQ class DBAL field
 * \param [out] weight - weight for the FMQ class
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_credit_weight_get(
    int unit,
    dbal_fields_e fmq_class_field_id,
    uint32 *weight);

/**
 * \brief - Set FMQ Shaper credit rate
 *
 * \param [in] unit -  Unit-ID
 * \param [in] shaper_type - FMQ shaper type (global/BE/guaranteed)
 * \param [in] credit_rate - credit rate for the shaper
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_shaper_credit_rate_set(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 credit_rate);

/**
 * \brief - Get FMQ Shaper credit rate
 *
 * \param [in] unit -  Unit-ID
 * \param [in] shaper_type - FMQ shaper type (global/BE/guaranteed)
 * \param [out] credit_rate - credit rate for the shaper
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_shaper_credit_rate_get(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 *credit_rate);

/**
 * \brief - Set FMQ Shaper max burst
 *
 * \param [in] unit -  Unit-ID
 * \param [in] shaper_type - FMQ shaper type (global/BE/guaranteed)
 * \param [in] max_burst - max burst for the shaper
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_shaper_max_burst_set(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 max_burst);

/**
 * \brief - Get FMQ Shaper max burst
 *
 * \param [in] unit -  Unit-ID
 * \param [in] shaper_type - FMQ shaper type (global/BE/guaranteed)
 * \param [out] max_burst - max burst for the shaper
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_shaper_max_burst_get(
    int unit,
    dbal_enum_value_field_fmq_shaper_type_e shaper_type,
    uint32 *max_burst);

/**
 * \brief - Set FMQ enhanced mode
 *
 * \param [in] unit -  Unit-ID
 * \param [in] is_enhanced_mode - FMQ scheduler mode - enhanced / basic
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_enhanced_mode_set(
    int unit,
    uint32 is_enhanced_mode);

/**
 * \brief - Get FMQ enhanced mode
 *
 * \param [in] unit -  Unit-ID
 * \param [out] is_enhanced_mode - FMQ scheduler mode - enhanced / basic
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_enhanced_mode_get(
    int unit,
    uint32 *is_enhanced_mode);

/**
 * \brief - Set HR ID for FMQ enhanced scheduling scheme
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] mc_port_id - FMQ MC port - Guaranteed/BE0/1/2
 * \param [in] hr_id - multicast id
 * \param [in] is_hr_valid - is hr_id valid (should it get credits from IPS)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_enhanced_mode_hr_id_set(
    int unit,
    int core,
    uint32 mc_port_id,
    uint32 hr_id,
    uint32 is_hr_valid);

/**
 * \brief - Get HR ID for FMQ enhanced scheduling scheme
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] mc_port_id - FMQ MC port - Guaranteed/BE0/1/2
 * \param [out] hr_id - multicast id
 * \param [out] is_hr_valid - is hr_id valid (should it get credits from IPS)
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_cosq_fmq_dbal_enhanced_mode_hr_id_get(
    int unit,
    int core,
    uint32 mc_port_id,
    uint32 *hr_id,
    uint32 *is_hr_valid);

#endif /* FMQ_DBAL_H_INCLUDED */
