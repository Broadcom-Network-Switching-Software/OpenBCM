/*! \file algo_mdb_em.h $Id$ Contains all of the MDB Exact Match algo callbacks
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifndef DNX_ALGO_MDB_EM_H_INCLUDED
/*
 * {
 */
#define DNX_ALGO_MDB_EM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_mngr_callbacks.h>

#include <soc/dnx/dbal/dbal.h>
/*
 * DEFINES 
 * { 
 */

/*
 * }
 */

/**
 * FUNCTIONS
 * {
 */

/**
* \brief
*   Print an entry of the MDB EM format template manager.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] data -
*      Pointer to the entry key_size.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
void dnx_algo_mdb_em_format_template_manager_print_cb(
    int unit,
    const void *data);

/**
* \brief
*   Initialize the MDB EM format template manager for the specified DBAL physical table.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] dbal_physical_table_id -
*      The DBAL physical table.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \par INDIRECT OUTPUT:
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_mdb_em_format_template_manager_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id);

/*
 * }
 */

#endif /* DNX_ALGO_MDB_EM_H_INCLUDED */
