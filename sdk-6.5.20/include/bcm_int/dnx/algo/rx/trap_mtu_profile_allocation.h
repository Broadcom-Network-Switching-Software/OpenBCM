/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * trap_mtu_profile_allocation.h
 *
 *  Created on: Jan 3, 2018
 *      Author: dp889757
 */

#ifndef INCLUDE_BCM_INT_DNX_ALGO_RX_TRAP_MTU_PROFILE_ALLOCATION_H_
#define INCLUDE_BCM_INT_DNX_ALGO_RX_TRAP_MTU_PROFILE_ALLOCATION_H_

/**
 * \brief
 *   Callback to allocate a free profile of an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] flags -
 *      SW_STATEALGO_TEMPLATE_ALLOCATE_* flags
 *   \param [in] nof_references -
 *      Number of references to be allocated.
 *  \param [in] profile_data -
 *      Pointer to memory holding template data to be saved to a profile.
 *  \param [in] extra_arguments -
 *      Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
 *  \param [in,out] profile -
 *      Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [in,out] first_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_rx_trap_mtu_profile_allocate(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

#endif /* INCLUDE_BCM_INT_DNX_ALGO_RX_TRAP_MTU_PROFILE_ALLOCATION_H_ */
