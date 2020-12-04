/**
 * \file bcm_int/dnx/tdm/tdm.h
 *
 * Internal DNX TDM APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TDM_H_INCLUDED
/*
 * {
 */
#define TDM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/fabric.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
/*
 * }
 */
/*
 * DEFINES
 * {
 */
/*
 * }
 */
/*
 * MACROS
 * {
 */
#define DNX_TDM_BYPASS_MODE_VERIFY(unit)                                                                        \
    if(dnx_data_tdm.params.mode_get(unit) != TDM_MODE_OPTIMIZED)                                                \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress TDM Configuration is available only in TDM_OPTIMIZED mode\n");      \
    }

/*
 * }
 */

/*
 * Typedefs
 * {
 */

/**
 * \brief
 *  Enum for TDM modes
 */
typedef enum dnx_tdm_mode_e
{
    /*
     * TDM mode NONE - TDM disabled
     */
    TDM_MODE_NONE = 0,

    /*
     * TDM mode OPTIMIZED
     */
    TDM_MODE_OPTIMIZED = 1,

    /*
     * TDM mode STANDARD
     */
    TDM_MODE_STANDARD = 2,

    /*
     * TDM mode PACKET
     */
    TDM_MODE_PACKET = 3,

    /*
     * Number of TDM modes
     */
    TDM_MODES_COUNT = 4
} dnx_tdm_mode_t;

/*
 * }
 */
/*
  * Internal functions
  * {
  */

/**
 * \brief - Configure switch tdm out header type
 *
 * \param [in] unit - Relevant unit
 * \param [in] switch_tdm_header_type - Switch tdm header type
 *  It could be one of BCM_SWITCH_CONTROL_TDM_OUT_HEADER_XXX
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tdm_out_header_type_set(
    int unit,
    int switch_tdm_header_type);

/**
 * \brief - Get switch tdm out header type
 *
 * \param [in] unit - Relevant unit
 * \param [out] switch_tdm_header_type - Switch tdm header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tdm_out_header_type_get(
    int unit,
    int *switch_tdm_header_type);

/**
 * \brief - Configure TDM system FTMH mode
 *
 * \param [in] unit - Relevant unit
 * \param [in] use_optimized_ftmh - If TRUE TDM will use optimized FTMH, FALSE - STANDARD
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   * dnx_tdm_ftmh_type_get
 */
shr_error_e dnx_tdm_system_ftmh_type_set(
    int unit,
    uint32 use_optimized_ftmh);

/**
 * \brief - Get TDM system FTMH mode
 *
 * \param [in] unit - Relevant unit
 * \param [out] use_optimized_ftmh_p - pointer where current system setting of Optimized TDM FTMH is returned
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   * dnx_tdm_ftmh_type_set
 */
shr_error_e dnx_tdm_system_ftmh_type_get(
    int unit,
    uint32 *use_optimized_ftmh_p);

/**
 * \brief - Set MC Prefix to make MC ID from Stream ID
 *
 * \param [in] unit - Relevant unit
 * \param [in] stream_mc_prefix - multicast prefix
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   * dnx_tdm_ftmh_type_get
 */
shr_error_e dnx_tdm_stream_mc_prefix_set(
    int unit,
    int stream_mc_prefix);

/**
 * \brief - Get TDM system FTMH mode
 *
 * \param [in] unit - Relevant unit
 * \param [out] stream_mc_prefix_p - pointer where multicast prefix is returned
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   * dnx_tdm_ftmh_type_set
 */
shr_error_e dnx_tdm_stream_mc_prefix_get(
    int unit,
    int *stream_mc_prefix_p);

/**
 * \brief - Initialize all TDM related resources
 *
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_tdm_init(
    int unit);
/* } */
/* } */
#endif /** TDM_H_INCLUDED */
