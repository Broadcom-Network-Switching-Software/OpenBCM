/** \file mirror_profile_alloc.c
 *
 * Mirror profile allocation algorithm functions for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR
/**
* INCLUDE FILES:
* {
*/

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include "mirror_profile.h"
#include "mirror_profile_alloc.h"

/*
 * }
 */

/*
 * Other include files. 
 * { 
 */
#include "mirror_profile_alloc.h"

/*
 * }
 */

/**
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_allocate(
    int unit,
    bcm_mirror_destination_t * mirror_dest,
    int *action_profile_id)
{
    int alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = 0;

    /*
     * Get given profile ID 
     */
    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID)
    {
        *action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * allocate snif profile according to application (snoop, mirror, statistical sampling)  
     */
    if (mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_snoop.allocate_single(unit, alloc_flags, NULL,
                                                                                  action_profile_id));
    }
    else if (mirror_dest->flags & BCM_MIRROR_DEST_IS_STAT_SAMPLE)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_stat_sampling.allocate_single(unit, alloc_flags,
                                                                                          NULL, action_profile_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_mirror.allocate_single(unit,
                                                                                   alloc_flags, NULL,
                                                                                   action_profile_id));
    }

    /*
     * allocate SNIF profile
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_deallocate(
    int unit,
    bcm_gport_t mirror_dest_id)
{
    int action_profile_id;

    SHR_FUNC_INIT_VARS(unit);

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    /*
     * deallocate snif profile according to application (snoop, mirror, statistical sampling)  
     */
    if (BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id))
    {
        /** snoop profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_snoop.free_single(unit, action_profile_id));
    }
    else if (_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id))
    {
        /** mirror profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_stat_sampling.free_single(unit, action_profile_id));
    }
    else if (BCM_GPORT_IS_MIRROR_MIRROR(mirror_dest_id))
    {
        /** mirror profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_mirror.free_single(unit, action_profile_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported gport type: 0x%x", mirror_dest_id);
    }

    /** free snif profile */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_is_allocated(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint8 *is_allocated)
{
    int action_profile_id;

    SHR_FUNC_INIT_VARS(unit);

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    /*
     * deallocate snif profile according to application (snoop, mirror, statistical sampling)  
     */
    if (BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id))
    {
        /** snoop profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_snoop.is_allocated(unit, action_profile_id, is_allocated));
    }
    else if (BCM_GPORT_IS_MIRROR_MIRROR(mirror_dest_id))
    {
        /** mirror profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_mirror.is_allocated(unit, action_profile_id, is_allocated));
    }
    else if (_SHR_GPORT_IS_MIRROR_STAT_SAMPLE(mirror_dest_id))
    {
        /** stat sample profile */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_stat_sampling.is_allocated(unit,
                                                                                       action_profile_id,
                                                                                       is_allocated));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported gport type: 0x%x", mirror_dest_id);
    }

    /** Check if given profile is already allocated */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_profile_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    dnx_algo_template_create_data_t temp_data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Ingress Mirror resource manager  
     */
    {
        /*
         * First element should be 1 instead of 0 since profile 0 is special, profile 0 is reserved as default in HW 
         * and means don't mirror 
         */
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        /** First profile id in mirror res mngr */
        data.first_element = DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Number of mirror profiles is nof_profiles-1 since the first profile is reserved */
        data.nof_elements = dnx_data_snif.ingress.nof_profiles_get(unit) - DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_MIRROR, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "SNIF - SNIF profiles allocated IDs";
         */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_mirror.create(unit, &data, NULL));
    }

    /*
     * Ingress Snoop resource manager
     */
    {
        /** First profile id in snoop res mngr */
        data.first_element = DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Number of Snoop profiles is nof_profiles-1 since the first profile is reserved */
        data.nof_elements = dnx_data_snif.ingress.nof_profiles_get(unit) - DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_SNOOP, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "SNIF - SNIF profiles allocated IDs";
         */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_snoop.create(unit, &data, NULL));
    }

    /*
     * Ingress Statistical sampling resource manager
     */
    {
        /** First profile id in statistical sampling res mngr */
        data.first_element = DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Number of Statistical sampling profiles is nof_profiles-1 since the first profile is reserved */
        data.nof_elements = dnx_data_snif.ingress.nof_profiles_get(unit) - DNX_MIRROR_PROFILE_INGRESS_PROFILE_MIN;
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_SNIF_RES_MNGR_INGRESS_PROFILES_STAT_SAMPLING,
                    DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "SNIF - SNIF profiles allocated IDs";
         */
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_stat_sampling.create(unit, &data, NULL));
    }
    /*
     * Ingress VLAN mirror template manager
     */
    {
        temp_data.flags = 0;
        temp_data.first_profile = 0;
        temp_data.nof_profiles = dnx_data_snif.ingress.mirror_nof_vlan_profiles_get(unit);
        temp_data.max_references = dnx_data_port.general.nof_pp_ports_get(unit);
        temp_data.data_size = sizeof(bcm_vlan_t);
        temp_data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC;
        sal_strncpy(temp_data.name, DNX_ALGO_SNIF_TEMP_MNGR_INGRESS_VLAN_PROFILES_MIRROR,
                    DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.alloc(unit));
        SHR_IF_ERR_EXIT(dnx_mirror_db.mirror_on_drop_profile.alloc(unit));

        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.create(unit, core_id, &temp_data, NULL));
        }
    }

    /*
     * Egress VLAN mirror template manager
     */
    {
        temp_data.flags = 0;
        temp_data.first_profile = 0;
        temp_data.nof_profiles = dnx_data_snif.egress.mirror_nof_vlan_profiles_get(unit);
        temp_data.max_references = dnx_data_port.general.nof_pp_ports_get(unit);
        temp_data.data_size = sizeof(bcm_vlan_t);
        temp_data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC;
        sal_strncpy(temp_data.name, DNX_ALGO_SNIF_TEMP_MNGR_EGRESS_VLAN_PROFILES_MIRROR,
                    DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_egress_vlan_profiles_mirror.alloc(unit));

        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_mirror_db.snif_egress_vlan_profiles_mirror.create(unit, core_id, &temp_data, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_vlan_profile_allocate(
    int unit,
    int core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    int *action_profile_id,
    uint8 *first_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * allocate vlan profile according to direction
     */
    if (flags & BCM_MIRROR_PORT_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.allocate_single
                        (unit, core_id, flags, &vlan_id, NULL, action_profile_id, first_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_egress_vlan_profiles_mirror.allocate_single
                        (unit, core_id, flags, &vlan_id, NULL, action_profile_id, first_reference));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_vlan_profile_deallocate(
    int unit,
    uint32 core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    uint8 *last_reference)
{
    int action_profile_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * allocate vlan profile according to direction
     */
    if (flags & BCM_MIRROR_PORT_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.profile_get
                        (unit, core_id, &vlan_id, &action_profile_id));
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.free_single
                        (unit, core_id, action_profile_id, last_reference));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_egress_vlan_profiles_mirror.profile_get
                        (unit, core_id, &vlan_id, &action_profile_id));
        SHR_IF_ERR_EXIT(dnx_mirror_db.snif_egress_vlan_profiles_mirror.free_single
                        (unit, core_id, action_profile_id, last_reference));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_vlan_profile_get(
    int unit,
    int core_id,
    uint32 flags,
    bcm_vlan_t vlan_id,
    int *action_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get the vlan profile id
     * This call supposed to fail in a case which Vlan profile never added.
     * This will happen on add vlan rule, while checking that no previous rule is configured.
     */
    if (flags & BCM_MIRROR_PORT_INGRESS)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_mirror_db.snif_ingress_vlan_profiles_mirror.profile_get
                               (unit, core_id, &vlan_id, action_profile_id));
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_mirror_db.snif_egress_vlan_profiles_mirror.profile_get
                               (unit, core_id, &vlan_id, action_profile_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
void
dnx_mirror_vlan_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_vlan_t *vlan_id = (bcm_vlan_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "VLAN ID", *vlan_id, NULL, "%u");

    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}
