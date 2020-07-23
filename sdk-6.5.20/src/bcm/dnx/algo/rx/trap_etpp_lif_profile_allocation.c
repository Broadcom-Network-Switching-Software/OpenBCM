/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  trap_etpp_lif_profile_allocation.c
 *
 *  Created on: Jan 3, 2018
 *      Author: dp889757
 *
 *
 *      The file contains the customized allocation for ETPP LIF profile template manager.
 *
 *      If the data given is gport encoding a trap allocate one of two trap profiles
 *      If the data given is gport encoding a mirror command allocate the mirror profile
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/rx/trap_etpp_lif_profile_allocation.h>
#include <bcm/rx.h>
#include <bcm_int/dnx/rx/rx.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/algo/rx/algo_rx.h>

shr_error_e
dnx_algo_rx_trap_etpp_lif_profile_allocate(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference)
{
    int tmp_profile, rv;
    int tmp_nof_references = 0;
    uint8 allocation_needed;
    bcm_gport_t *action_gport_p = (bcm_gport_t *) profile_data;
    bcm_gport_t tmp_action_gport;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * 1. Check if the data already exists or if it needs to be allocated.
     */
    rv = multi_set_member_lookup(unit, module_id, multi_set_template, profile_data, profile);

    /*
     * If E_NOT_FOUND was returned, it means the data wasn't allocated yet and we need to allocate it.
     */
    if (rv == _SHR_E_NOT_FOUND)
    {
        *profile = DNX_RX_TRAP_LIF_PROFILE_DEFAULT;
        allocation_needed = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        allocation_needed = FALSE;
    }

    /*
     * 2. Get the profile to allocate based on gport type.
     */
    if ((allocation_needed == TRUE) && (*action_gport_p != BCM_GPORT_INVALID))
    {
        if (BCM_GPORT_IS_TRAP(*action_gport_p))
        {
            /** action is trap, allocate an avaiable trap profile */
            for (tmp_profile = DNX_RX_TRAP_LIF_PROFILE_STACK_TRAP_1;
                 tmp_profile <= DNX_RX_TRAP_LIF_PROFILE_STACK_TRAP_2; tmp_profile++)
            {
                SHR_IF_ERR_EXIT(multi_set_get_by_index
                                (unit, module_id, multi_set_template, tmp_profile,
                                 &tmp_nof_references, &tmp_action_gport));

                if (tmp_nof_references == 0)
                {
                    *profile = tmp_profile;
                    break;
                }
            }

            if (*profile == DNX_RX_TRAP_LIF_PROFILE_DEFAULT)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "No avaiable trap profile to allocate\n");
            }
        }
        else
        {
            /** action is mirror command, allocate mirror profile */
            tmp_profile = DNX_RX_TRAP_LIF_PROFILE_STACK_MIRROR;
            SHR_IF_ERR_EXIT(multi_set_get_by_index
                            (unit, module_id, multi_set_template, tmp_profile, &tmp_nof_references, &tmp_action_gport));

            if (tmp_nof_references != 0)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "No avaiable mirror profile to allocate\n");
            }

            *profile = tmp_profile;
        }
    }

    /*
     * 3. Use the allocated profile for the multi set template WITH_ID.
     */

    SHR_IF_ERR_EXIT(multi_set_member_add
                    (unit, module_id, multi_set_template, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, nof_references,
                     profile_data, NULL, profile, first_reference));

exit:
    SHR_FUNC_EXIT;
}
