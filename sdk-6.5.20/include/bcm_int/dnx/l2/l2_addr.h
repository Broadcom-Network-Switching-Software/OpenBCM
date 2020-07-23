/**
 * \file bcm_int/dnx/l2/l2_addr.h
 * Internal DNX L2 APIs 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef L2_ADDR_H_INCLUDED
/*
 * {
 */
#define L2_ADDR_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>

#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm/types.h>
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
  * Internal functions.
  * {
  */

/**
 * \brief - Get an handle to a FWD_MACT table. The handle should have one entry fetched in it. 
 * The function is retrieving the entry and parse it to l2addr struct.
 * 
 * \param [in] unit - unit id.
 * \param [in] entry_handle_id - handle to FWD_MACT with one entry fetched. 
 * \param [in] table_id - table ID associated with the entry_handle_id.
 * \param [in] l2addr - bcm_l2_addr_t struct that will be filled by the function.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_addr_from_dbal_to_struct(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    bcm_l2_addr_t * l2addr);

shr_error_e dnx_convert_l2_addr_to_dest(
    int unit,
    bcm_l2_addr_t * l2addr,
    uint32 *destination,
    uint32 *outlif,
    uint32 *result_type,
    uint32 *eei);
/*
 * }
 */
#endif /* L2_ADDR_H_INCLUDED */
