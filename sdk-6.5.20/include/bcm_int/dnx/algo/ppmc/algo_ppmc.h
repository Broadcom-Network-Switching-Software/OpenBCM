/**
 * \file algo_ppmc.h
 * Internal DNX PP Multicast Management APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_PPMC_H_INCLUDED

#define ALGO_PPMC_H_INCLUDED
/*
 * Includes
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.h>

/*
 * }
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/** Define the name of the PPMC resource */
#define DNX_ALGO_PPMC "DNX_ALGO_PPMC"

/** Verify if the mc_rep_id is in the valid range for MC replication indexes - between 0x300000 and 0x3BFFFF */
#define DNX_ALGO_PPMC_IS_MC_REPLICATION_ID(unit, mc_rep_id) ((mc_rep_id) >= dnx_data_ppmc.param.min_mc_replication_id_get(unit)\
        && (mc_rep_id) <= dnx_data_ppmc.param.max_mc_replication_id_get(unit))

/**
 * \brief
 *   Initialize the ppmc algorithms.
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * init_seq - ALGO
 */
shr_error_e dnx_algo_ppmc_init(
    int unit);

/**
 * \brief
 *  Allocate a PPMC key in the resource manager.
 * \param [in] unit - The unit number.
 * \param [in,out] mc_replication_id - The MC replication key of the entry
 * \param [in] flags - Control flags for the allocation:
 *      * DNX_FIELD_ACE_ENTRY_ADD_FLAG_WITH_ID - indicates that allocation will happen with a given ID in mc_replication_id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_field_ace_entry_add
 *  * bcm_dnx_multicast_encap_extension_create
 */
shr_error_e dnx_algo_ppmc_mc_replication_id_alloc(
    int unit,
    uint32 flags,
    int *mc_replication_id);

/**
 * \brief
 *  Free a PPMC key from the resource manager.
 * \param [in] unit - The unit number.
 * \param [in] mc_replication_id - The MC replication key of the entry
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_field_ace_entry_delete
 *  * bcm_dnx_multicast_encap_extension_destroy
 */
shr_error_e dnx_algo_ppmc_mc_replication_id_free(
    int unit,
    int mc_replication_id);

/**
 * \brief
 *  Verify whether a PPMC key exists in the resource manager.
 * \param [in] unit - The unit number.
 * \param [in] mc_replication_id - The MC replication key of the entry
 * \param [out] is_allocated - Indication whether the key is allocated or not.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_field_ace_entry_add_verify
 */
shr_error_e dnx_algo_ppmc_mc_replication_id_is_allocated(
    int unit,
    int mc_replication_id,
    uint8 *is_allocated);

/**
 * \brief
 *  Clear All PPMC keys from the resource manager.
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_field_ace_entry_add_verify
 */
shr_error_e dnx_algo_ppmc_clear_all(
    int unit);

#endif /* ALGO_PPMC_H_INCLUDED */
