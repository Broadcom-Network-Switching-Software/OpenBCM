/**
 * \file algo_esem_mnmgt.h Internal DNX L3 Managment APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_ESEM_MNMT_H_INCLUDED
/*
 * {
 */
#define ALGO_ESEM_MNMT_H_INCLUDED

#include <bcm_int/dnx/l3/l3_ecmp.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif
/**
 * Defines
 * {
 */

/**
 * }
 */

/**
 * emums
 * {
 */

/**
 * }
 */

/**
 * Structures
 * {
 */

/** List of sources for each entry */
typedef enum
{
    /** source of the entry is vxlan_port_xxx   */
    DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_VPN,

    /** source of the entry is vxlan_network_domain_config_xxx   */
    DNX_ALGO_ESEM_ENTRY_SOURCE_VXLAN_NWK_DOMAIN,

    /** source of the entry is AC */
    DNX_ALGO_ESEM_ENTRY_SOURCE_AC,

    ALGO_ESEM_ENTRY_NOF_SOURCES
} algo_esem_entry_source_e;

typedef enum
{
    /** The esem entry is not exists in the DB */
    ALGO_ESEM_ENTRY_STATUS_NOT_EXISTS,

    /** The esem entry was created by the input source */
    ALGO_ESEM_ENTRY_STATUS_CREATED_BY_SOURCE,

    /** The esem entry was added by a different source   */
    ALGO_ESEM_ENTRY_STATUS_ADDED_BY_SOURCE,

    /** The esem entry was created but the source is not related to this entry  */
    ALGO_ESEM_ENTRY_STATUS_EXISTS_SOURCE_NOT_MATCH,

    ALGO_ESEM_ENTRY_NOF_STATUS
} algo_esem_entry_status_e;
/**
 * }
 */

/*
 * FUNCTIONS
 * {
 */

shr_error_e dnx_algo_esem_mnmgt_init(
    int unit);

/**
* \brief
*   returns the entry status.
*
*  \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*    \param [in] entry_handle_id - the handle ID with the entry key set on the handle.
*    \param [in] source - the relevant source intreseted.
*    \param [out] entry_status - the entry status
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_esem_entry_sw_source_status_get(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source,
    algo_esem_entry_status_e * entry_status);
/**
* \brief
*   create an entry, must be first occurrence of the key, in case entry exists error returns (silenced).
*
*  \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*    \param [in] entry_handle_id - the handle ID with the entry key set on the handle.
*    \param [in] source - the relevant source intreseted.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_esem_entry_sw_source_create(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source);

/**
* \brief
*   add more souce to an existing entry.
*
*  \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*    \param [in] entry_handle_id - the handle ID with the entry key set on the handle.
*    \param [in] source - the relevant source intreseted.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_esem_entry_sw_source_add(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source);

/**
* \brief
*   delete an entry, according to the source status, the entry will be removed from DB or just source will be removed.
*
*  \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*    \param [in] entry_handle_id - the handle ID with the entry key set on the handle.
*    \param [in] source - the relevant source intreseted.
*  \par DIRECT OUTPUT:
*    shr_error_e -
*      Error return value
*  \par INDIRECT OUTPUT:
*      None
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_esem_entry_sw_source_delete(
    int unit,
    uint32 entry_handle_id,
    algo_esem_entry_source_e source);

/*
 * }
 */

 /*
  * }
  */
#endif /* ALGO_ESEM_MNMT_H_INCLUDED */
