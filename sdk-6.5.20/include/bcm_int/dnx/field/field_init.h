/**
 *  \file bcm_int/dnx/field/field_init.h
 * 
 *  Field Processor initialization function for DNX.
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_FIELD_FIELD_INIT_H_INCLUDED
/* { */
#define DNX_FIELD_FIELD_INIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * { Include files
 */

#include <bcm/types.h>

/*
 * } Include files
 */
/*
 * Defines
 * {
 */
/** Tells whether the initial keys in iPMF1 should be set to be copied to iPMF2 in init.*/
#define DNX_FIELD_INIT_COPY_INITIAL_KEYS_TO_IPMF2 0
/*
 * }
 */
/**
 * MACROS
 * {
 */
/**
 * This macro is used in 'dnx_field_init_is_L4_profile_table'.
 * In the registers: IPPC_PMF_PRTOCOL_IS_L_4 and ERRP_PMF_PRTOCOL_IS_L_4.
 * We put 1 to TCP and UDP protocols because they are L4.
 */
#define DNX_FIELD_INIT_L4_OPS_INIT_VAL(unit)    \
    (1 << (dnx_data_field.L4_Ops.udp_position_get(unit))) | ( 1 << (dnx_data_field.L4_Ops.tcp_position_get(unit)))
/**
 * }
 */
/**
 * \brief
 *  Initiate the parametrs of SW state
 * \param [in] unit  - Device id
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_init_sw_state(
    int unit);
/**
 * \brief
 *  De-Initiate the parametrs of SW state
 * \param [in] unit  - Device id
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_deinit_sw_state(
    int unit);
/**
 * \brief
 *  Init all relevant information for Field module
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure is not used any more (replaced by automatic \n
 *     'init' invocations. See dnx_init_deinit_field_seq[].
 *   * It is left here in case it is required for diagnostics.
 * \see
 *   * None
 */
shr_error_e dnx_field_init(
    int unit);
/**
 * \brief
 *  Init all relevant information for Field related tables
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_init_tables(
    int unit);

/**
 * \brief
 *  Inits the system headers in the device, the system headers are pre-defined inside "field_init.c" file.
 *
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_system_headers_init(
    int unit);

/**
 * \brief
 *  Inits UDH types length inside the device, the type lengths of the UDH are pre-defined in "DNX_DATA" module.
 *
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_udh_init(
    int unit);

/**
 * \brief
 *  run over all DBAL tables that the access type is TCAM, create resources in the TCAM manager and update the tcam
 *  handler to the app_db_id
 *
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_init_tcam_fwd_tables(
    int unit);

shr_error_e dnx_field_init_tcam_handlers_save(
    int unit);

/**
 * \brief
 *  De-Init all relevant information for Field module
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure is not used any more (replaced by automatic \n
 *     'init' invocations. See dnx_init_deinit_field_seq[].
 *   * It is left here in case it is required for diagnostics.
 * \see
 *   * None
 */
shr_error_e dnx_field_init_deinit(
    int unit);

#if DBX_FIELD_BRUTE_FORCE_DEINIT_INIT
/* { */
/**
 * \brief
 *  This procedure takes two steps:                                   \n
 *    De-Init all relevant information for Field module (releases all \n
 *    resources but does not touch HW)             \n
 *    Init all relevant SW and HW for Field module \n
 *  See 'remarks' below.
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure is required for diagnostics only.
 * \see
 *   * dnx_field_init_deinit
 *   * dnx_field_init
 */
shr_error_e dnx_field_init_reinit(
    int unit);

/* } */
#endif /* DBX_FIELD_BRUTE_FORCE_DEINIT_INIT */

/* } */
#endif /* BCM_DNX_SUPPORT */
