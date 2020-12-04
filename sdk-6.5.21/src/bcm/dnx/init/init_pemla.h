/** \file init_pemla.h
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INIT_PEMLA_H_INCLUDED
/*
 * {
 */
#define INIT_PEMLA_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/init/init.h>

/**
 * \brief - wrapper for PEMLA drv initialization
 */
shr_error_e dnx_init_pemla_init(
    int unit);

/**
 * \brief - wrapper for PEMLA drv deinitialization
 */
shr_error_e dnx_init_pemla_deinit(
    int unit);

/*
 * \brief - PEM Init sequence
 */
shr_error_e dnx_init_pem_sequence_flow(
    int unit);
/**
 * /brief
 *  returns relevant tcam stage per app_db_id
 *
 * \param [in] unit         - Device ID
 * \param [in] app_db_id    - app_db_id
 * \param [out] tcam_stage    - tcam_stage
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pemla_app_db_id_to_stage_get(
    int unit,
    uint32 app_db_id,
    uint32 *tcam_stage);

/**
 * /brief
 *  returns relevant app_db_ids, which share the same context with given app_db_id
 *
 * \param [in] unit         - Device ID
 * \param [in] handler_id    - handler_id
 * \param [out] handler_ids_same_ctx    - array of handler_id sharing the same context
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_init_pemla_context_sharing_handler_ids_get_cb(
    int unit,
    uint32 handler_id,
    uint32 *handler_ids_same_ctx);
/**
 * /brief
 *  saves handler_id specific information
 *
 * \param [in] unit         - Device ID
 * \param [in] handler_id    - handler_id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_pemla_context_sharing_handler_ids_save(
    int unit,
    uint32 handler_id);
/*
 * }
 */
#endif /** INIT_PEMLA_H_INCLUDED */
