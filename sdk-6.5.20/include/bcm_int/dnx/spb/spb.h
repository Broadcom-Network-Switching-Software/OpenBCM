/** \file bcm_int/dnx/spb/spb.h
 *
 * sram packet buffer internal.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SPB_API_INCLUDED__
/*
 * { 
 */
#define _DNX_SPB_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/**
 * \brief - Call Back function for mem_init mechanism, will init the context MRU to simulation values
 * 
 * \param [in] unit - unit number
 * \param [in] array_index - irrelevant for this cb
 * \param [in] copyno - irrelevant for this cb
 * \param [in] index - irrelevant for this cb
 * \param [in] entry_data - returned value to set to memory
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_context_mru_default_get_f(
    int unit,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data);

/**
 * \brief - Initialize dnx spb module
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_init(
    int unit);

/**
 * \brief - De-initialize dnx spb module
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_spb_deinit(
    int unit);

#endif/*_DNX_SPB_API_INCLUDED__*/
