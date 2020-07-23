/** \file bcm_int/dnx/gtimer/gtimer.h
 * 
 * Internal DNX GTIMER APIs to be used from other modules
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_GTIMER_GTIMER_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_GTIMER_GTIMER_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/chip.h>
#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - configure and activate gtimer for a specified block
 * 
 * \param [in] unit - unit number
 * \param [in] period - number of clocks to run gtimer
 * \param [in] block_type - block type for which to activate gtimer
 * \param [in] instance - block instance for which to activate gtimer - value of -1 means all instances
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  Gtimer is a mechanism that allows certain counters in a block to sample for a defined period of clock.
 *      Thus allowing to calculate rate instead of flat counter value.
 * \see
 *   * None
 */
shr_error_e dnx_gtimer_set(
    int unit,
    uint32 period,
    soc_block_type_t block_type,
    int instance);

/**
 * \brief - get gtimer enabled status for a specified block
 * 
 * \param [in] unit - unit number
 * \param [in] block_type - block type for which to activate gtimer
 * \param [in] instance - block instance for which to activate gtimer - value of -1 means all instances
 * \param [out] is_enabled - return gtimer enabled status
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gtimer_get(
    int unit,
    soc_block_type_t block_type,
    int instance,
    int *is_enabled);

/**
 * \brief - wait for gtimer sampling period on given block to end
 *
 * \param [in] unit - unit number
 * \param [in] block_type - block type for which to wait
 * \param [in] instance - block instance for which to wait - value of -1 means all instances
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gtimer_wait(
    int unit,
    soc_block_type_t block_type,
    int instance);

/**
 * \brief - clear block from working in gtimer mode
 * 
 * \param [in] unit - unit number
 * \param [in] block_type - block type to clear
 * \param [in] instance - block instance to clear - value of -1 means all instances
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gtimer_clear(
    int unit,
    soc_block_type_t block_type,
    int instance);

/**
 * \brief - check if block type is supported by gtimer infrastructure.
 *
 * \param [in] unit - unit number
 * \param [in] block_type - block type to check if supported
 * \param [out] is_supported - indication if the block type is supported or not.
 *                             1 - block type is supported.
 *                             0 - block type is not supported.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gtimer_block_type_is_supported(
    int unit,
    soc_block_type_t block_type,
    int *is_supported);

/*
 * } 
 */
#endif /* _BCMINT_DNX_GTIMER_GTIMER_H_INCLUDED */
