/** \file appl_ref_sniff_ports.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_SNIFF_PORTS_H_INCLUDED
/*
 * {
 */
#define APPL_REF_SNIFF_PORTS_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * Typedefs
 * {
 */


/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/**
 * \brief - Create mirror recycle port for each ethernet port:
 * 1. Create mirror recycle port
 * 2. Map ethernet port to the created mirror recycle port
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_sniff_ports_init(
    int unit);

/**
 * \brief - Search for a free recycle port and map it to the forward port
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical forward port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_sniff_ports_recycle_port_map(
    int unit,
    bcm_port_t port);

/**
 * \brief - Unmap forward port from recycle mirror port.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical forward port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_sniff_ports_recycle_port_unmap(
    int unit,
    bcm_port_t port);


/*
 * }
 */
#endif /* APPL_REF_SNIFF_PORTS_H_INCLUDED */
