/** \file src/bcm/dnx/multicast/multicast.c
 * 
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/bier/bier.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx_dispatch.h>

#include "multicast_imp.h"
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * \brief - verify that multicast group at the soc property range
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_mc_id_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_MULTICAST_IS_INGRESS_GROUP(flags) && (group > dnx_data_multicast.params.max_ing_mc_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress multicast ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.max_ing_mc_groups_get(unit));
    }
    if (DNX_MULTICAST_IS_EGRESS_GROUP(flags) && (group > dnx_data_multicast.params.max_egr_mc_groups_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress multicast ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.max_egr_mc_groups_get(unit));
    }
    if (DNX_MULTICAST_IS_EGRESS_BITMAP(flags) && (group >= dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress bitmap ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit));
    }
    if (DNX_MULTICAST_IS_INGRESS_BITMAP(flags) && (group >= dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress bitmap ID (%d) is not at the valid range (> %u)\n",
                     group, dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_create()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group_id - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *  this verifies that the correct flags are used and that group id in appropriate range and that the group is not open
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_create_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t * group_id)
{
    uint32 is_open;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_id, _SHR_E_PARAM, "group_id");

    
#if 0
    if (flags &
        ~(BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP |
          BCM_MULTICAST_EGRESS_TDM_GROUP | BCM_MULTICAST_TYPE_PORTS_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported bcm multicast create flag was specified : 0x%x", flags);
    }
#endif
    /*
     * Multicast without ID is not supported
     */
    if (!(flags & BCM_MULTICAST_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_MULTICAST_WITH_ID must be used\n");
    }

    /*
     * Multicast TDM verification
     */
    if (DNX_MULTICAST_IS_TDM(flags))
    {
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported for this device\n");
        }
        if (flags & ~(BCM_MULTICAST_EGRESS_TDM_GROUP | BCM_MULTICAST_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported flags (0x%x) was specified for tdm group", flags);
        }
    }

    /*
     * one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API
     */
    if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_EGRESS(flags) && !DNX_MULTICAST_IS_TDM(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * BIER verification
     */
    if (DNX_MULTICAST_IS_BIER(flags))
    {
        SHR_INVOKE_VERIFY_DNX(dnx_multicast_bfr_id_verify(unit, flags, *group_id));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_mc_id_verify(unit, flags, *group_id));

    /*
     * verify that the group is not open
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, *group_id, flags, &is_open));
    if (is_open)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Multicast ID is already created\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_destroy()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group_id - group/bitmap id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *  this verifies that the group id in appropriate range and that the group/bitmap is open
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_destroy_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group_id)
{
    uint32 group_found = FALSE, group_is_open;
    int set_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify valid flags
     */
    if (flags & ~(BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_PORTS_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported bcm multicast destroy flag was specified : 0x%x", flags);
    }
    /*
     * bitmap
     */
    if (DNX_MULTICAST_IS_BITMAP(flags))
    {
        if (DNX_MULTICAST_IS_INGRESS(flags) && group_id <= dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                            (unit, group_id, BCM_MULTICAST_TYPE_PORTS_GROUP | BCM_MULTICAST_INGRESS_GROUP,
                             &group_is_open));
            group_found |= group_is_open;
        }
        if (DNX_MULTICAST_IS_EGRESS(flags) && group_id <= dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                            (unit, group_id, BCM_MULTICAST_TYPE_PORTS_GROUP | BCM_MULTICAST_EGRESS_GROUP,
                             &group_is_open));
            group_found |= group_is_open;
        }
    }
    else
        /*
         * link list
         */
    {
        /*
         * ingress and egress multicast group are destroyed together cause flags doesn't have ing/egr indication
         */
        if (DNX_MULTICAST_IS_INGRESS(flags) && group_id <= dnx_data_multicast.params.max_ing_mc_groups_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, BCM_MULTICAST_INGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }
        if (DNX_MULTICAST_IS_EGRESS(flags) && group_id <= dnx_data_multicast.params.max_egr_mc_groups_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, BCM_MULTICAST_EGRESS_GROUP, &group_is_open));
            group_found |= group_is_open;
        }

        /*
         * BIER
         */
        if (group_id < dnx_data_bier.params.nof_bfr_entries_get(unit) * dnx_data_bier.params.bfr_entry_size_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group_id, &set_size));
            group_found |= set_size ? TRUE : FALSE;
        }
    }

    /*
     * verify that at least one of the groups are open
     */
    if (group_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "group ID (%u) is not created\n", group_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_add/delete/set/get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   \param [in] is_get - indicates if the verify function was called from the Get API
 *   \param [in] rep_count - replication count, relevant only for the case when verify is called from the Get API
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   this verifies that the the number of replications are bigger than zero and that
 *   the group id in appropriate range and that the group is open and that the flags are valid
 *   the bitmap/group creation is always with it so BCM_MULTICAST_WITH_ID always need to be used.
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_add_delete_set_get_verify(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array,
    uint8 is_get,
    int *rep_count)
{
    uint32 is_open;
    uint8 is_allocated = 0;
    bcm_multicast_t group_id;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_replications > 0)
    {
        SHR_NULL_CHECK(rep_array, _SHR_E_PARAM, "replication array");
    }

    if (is_get == TRUE)
    {
        SHR_NULL_CHECK(rep_count, _SHR_E_PARAM, "replication count");
    }

    
#if 0
    if (flags &
        ~(BCM_MULTICAST_REMOVE_ALL | BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP |
          BCM_MULTICAST_TYPE_PORTS_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags (0x%x)\n", flags);
    }
#endif
    /*
     * verify that number of replications are bigger than zero
     */
    if (nof_replications < 0 && !(flags & BCM_MULTICAST_REMOVE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number of replications (%d) must not be negative\n", nof_replications);
    }

    /*
     * adding bitmap flag for internal group verification
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        flags |= BCM_MULTICAST_TYPE_PORTS_GROUP;
    }

    group_id = _BCM_MULTICAST_ID_GET(group);
    SHR_INVOKE_VERIFY_DNX(dnx_multicast_mc_id_verify(unit, flags, group_id));

    /*
     * Multicast TDM verification
     */
    if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_BITMAP(flags))
    {
        SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, group_id, &is_allocated));
    }
    if (is_allocated)
    {
        if (flags & ~(BCM_MULTICAST_REMOVE_ALL | BCM_MULTICAST_EGRESS_GROUP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "An unsupported flags (0x%x) was specified for tdm group", flags);
        }
    }
    else
    {
        /*
         * verify that one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags used with this API
         */
        if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_EGRESS(flags))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
        }
    }

    /*
     * BCM_MULTICAST_EGRESS_GROUP & BCM_MULTICAST_INGRESS_GROUP flags should not be used together
     */
    if (DNX_MULTICAST_IS_INGRESS(flags) && DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * verify that the group is open
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, flags, &is_open));
    if (is_open == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Multicast ID (%d) is not created. Flags: 0x%X\n", group_id, flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - creates multicast empty (ingress/egress) group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group - group/bitmap id.
 *   \param [in] core_id - core_id
 *
 * \remark
 * allocate an mcdb entry and move the existing one if needed
 * \see
 *   * None
 */
static int
dnx_multicast_create_allocation(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 core_id)
{
    uint8 is_allocated = FALSE;
    uint32 entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if desired entry already allocated, relocate the existing entry otherwise allocate it
     */
    SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, DNX_MULTICAST_IS_BITMAP(flags),
                                              DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_id));
    SHR_IF_ERR_EXIT(multicast_db.mcdb.is_allocated(unit, entry_id, &is_allocated));
    if (is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_entry_relocate(unit, flags, core_id, entry_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, TRUE, &entry_id));
    }

    /*
     * clear entry
     */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_clear
                    (unit, entry_id, DNX_MULTICAST_IS_TDM(flags), DNX_MULTICAST_IS_BITMAP(flags)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - creates multicast empty (ingress/egress) group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id. see remarks
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   the bitmap/group creation is always with it so BCM_MULTICAST_WITH_ID always need to be used.
 *   for bitmap the group encapsulated with bitmap indication. this way other API's will recognize this group as bitmap
 * \see
 *   * None
 */
int
bcm_dnx_multicast_create(
    int unit,
    uint32 flags,
    bcm_multicast_t * group)
{
    uint32 core_id;
    uint32 bmp_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    *group = _BCM_MULTICAST_ID_GET(*group);
    SHR_INVOKE_VERIFY_DNX(dnx_multicast_create_verify(unit, flags, group));

    /*
     * create bier group
     */
    if (DNX_MULTICAST_IS_BIER(flags))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bier_create(unit, flags, *group));

        /*
         * mark group as open. HW for ingress & SW for egress
         */
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, *group, flags, TRUE));
        SHR_EXIT();
    }

    /*
     * allocation
     */
    if (DNX_MULTICAST_IS_INGRESS(flags))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_create_allocation(unit, flags & (~BCM_MULTICAST_EGRESS_GROUP), *group, 0));
    }
    if (DNX_MULTICAST_IS_EGRESS(flags))
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_create_allocation
                            (unit, flags & (~BCM_MULTICAST_INGRESS_GROUP), *group, core_id));
        }
    }

    /*
     * mark group as open. HW for ingress & SW for egress
     */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, *group, flags, TRUE));

    /*
     * mark group as TDM
     */
    if (DNX_MULTICAST_IS_TDM(flags))
    {
        SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, group));
    }

    /*
     * set the representation for bitmap
     */
    if (DNX_MULTICAST_IS_BITMAP(flags))
    {
        bmp_flags = 0;
        /** set the representation for bitmap */
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            bmp_flags |= _BCM_MULTICAST_TYPE_ING_BMP;
        }
        if (DNX_MULTICAST_IS_EGRESS(flags))
        {
            bmp_flags |= _BCM_MULTICAST_TYPE_EGR_BMP;
        }
        _BCM_MULTICAST_GROUP_SET(*group, bmp_flags, *group);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy the ingress and egress multicast for bitmap/groups id.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] group - group/bitmap id created at bcm_multicast_create API
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_group_destroy(
    int unit,
    uint32 flags,
    bcm_multicast_t group)
{
    uint32 entry_id;
    uint32 core_id = 0;
    uint32 tmp_flags;
    uint32 is_open = FALSE;
    uint8 is_tdm_allocated;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_destroy_verify(unit, flags, _BCM_MULTICAST_ID_GET(group)));

    /*
     * try to destroy bier group. if this is not an bier group continue to multicast
     */
    if (!DNX_MULTICAST_IS_BITMAP(flags))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bier_destroy(unit, group));
    }

    /*
     * free ingress link lists and close the group if open.
     */
    if (DNX_MULTICAST_IS_INGRESS(flags))
    {
        tmp_flags = flags & (~BCM_MULTICAST_EGRESS_GROUP);
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, _BCM_MULTICAST_ID_GET(group), tmp_flags, &is_open));
        if (is_open)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_multicast_delete(unit, group, tmp_flags | BCM_MULTICAST_REMOVE_ALL, 0, NULL));
            SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                            (unit, _BCM_MULTICAST_ID_GET(group), DNX_MULTICAST_IS_BITMAP(flags), TRUE, core_id,
                             &entry_id));
            SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, entry_id));
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, _BCM_MULTICAST_ID_GET(group), tmp_flags, FALSE));
        }
    }

    /*
     * free egress link list and close the group if open.
     */
    if (DNX_MULTICAST_IS_EGRESS(flags))
    {
        tmp_flags = flags & (~BCM_MULTICAST_INGRESS_GROUP);
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, _BCM_MULTICAST_ID_GET(group), tmp_flags, &is_open));
        if (is_open)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_multicast_delete(unit, group, tmp_flags | BCM_MULTICAST_REMOVE_ALL, 0, NULL));
            for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                                (unit, _BCM_MULTICAST_ID_GET(group), DNX_MULTICAST_IS_BITMAP(flags), FALSE, core_id,
                                 &entry_id));
                SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, entry_id));
            }
            SHR_IF_ERR_EXIT(dnx_multicast_group_open_set(unit, _BCM_MULTICAST_ID_GET(group), tmp_flags, FALSE));
        }
        SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, _BCM_MULTICAST_ID_GET(group), &is_tdm_allocated));
        if (is_tdm_allocated)
        {
            SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.free_single(unit, group));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy the ingress and egress multicast for bitmap/groups id.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id created at bcm_multicast_create API
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_destroy(
    int unit,
    bcm_multicast_t group)
{
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP)
    {
        flags = BCM_MULTICAST_TYPE_PORTS_GROUP | BCM_MULTICAST_INGRESS_GROUP;
    }
    else if (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP)
    {
        flags = BCM_MULTICAST_TYPE_PORTS_GROUP | BCM_MULTICAST_EGRESS_GROUP;
    }
    else
    {
        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_INGRESS_GROUP;
    }

    SHR_IF_ERR_EXIT(bcm_dnx_multicast_group_destroy(unit, flags, group));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the content (replications) of a multicast group/bitmap
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint8 is_tdm_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_delete_set_get_verify
                          (unit, group, flags, nof_replications, rep_array, FALSE, NULL));

    /*
     * handle bitmap case
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_create
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, TRUE, nof_replications, rep_array));
    }
    else
    {
        /*
         * handle TDM case
         */
        if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_BITMAP(flags))
        {
            SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, group, &is_tdm_allocated));
        }
        if (is_tdm_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_tdm_set(unit, group, flags, nof_replications, rep_array));
        }
        else
        {
            /*
             * sort egress replications by CUD in order to use one entry for 2 replications with same cud
             */

            /*
             * next link list with replications created and the old link list is removed
             */
            SHR_IF_ERR_EXIT(dnx_multicast_linklist_create(unit, group, flags, TRUE, nof_replications, rep_array));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the content (replications) of a multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] max_replications - specifies the rep_array size
 *   \param [in,out] out_rep_array - pointer to the the group/bitmap replications array
 *   \param [in,out] rep_count - pointer to number of replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   See out_rep_array above - pointer to the the group/bitmap replications array read from HW
 *   See rep_count - pointer to number of replications read from HW
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * out_rep_array,
    int *rep_count)
{
    uint8 is_tdm_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_delete_set_get_verify
                          (unit, group, flags, max_replications, out_rep_array, TRUE, rep_count));

    /*
     * handle bitmap case
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_get
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, max_replications, out_rep_array, rep_count));
    }
    else
    {
        /*
         * handle TDM case
         */
        if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_BITMAP(flags))
        {
            SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, group, &is_tdm_allocated));
        }
        if (is_tdm_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_tdm_get(unit, group, flags, max_replications, out_rep_array, rep_count));
        }
        else
        {
            /*
             * Go through the link list in order to get the replications
             */
            SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter(unit, group, flags, NULL, DNX_MULTICAST_ENTRY_ITER_GET,
                                                        max_replications, out_rep_array, rep_count));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Adds multicast destinations for multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_add(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint8 is_tdm_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_delete_set_get_verify
                          (unit, group, flags, nof_replications, rep_array, FALSE, NULL));

    /*
     * handle bitmap case
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_create
                        (unit, _BCM_MULTICAST_ID_GET(group), flags, FALSE, nof_replications, rep_array));
    }
    else
    {
        /*
         * handle TDM case
         */
        if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_BITMAP(flags))
        {
            SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, group, &is_tdm_allocated));
        }
        if (is_tdm_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_tdm_add(unit, group, flags, nof_replications, rep_array));
        }
        else
        {
            /*
             * sort egress replications by CUD in order to use one entry for 2 replications with same cud
             */

            /*
             * create link list with new replications
             */
            SHR_IF_ERR_EXIT(dnx_multicast_linklist_create(unit, group, flags, FALSE, nof_replications, rep_array));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Removes multicast destinations from multicast group/bitmap.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint8 is_tdm_allocated = FALSE;
    uint32 action, rep_exist;
    int rep_count_out;
    bcm_core_t core_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_add_delete_set_get_verify
                          (unit, group, flags, nof_replications, rep_array, FALSE, NULL));

    action = (flags & BCM_MULTICAST_REMOVE_ALL) ? DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL :
        DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST;

    /*
     * Go through the link list in order to find the replications and delete them
     */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_delete(unit, _BCM_MULTICAST_ID_GET(group), flags,
                                                    action, nof_replications, rep_array));
    }
    else
    {
        /*
         * handle TDM case
         */
        if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_BITMAP(flags))
        {
            SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, group, &is_tdm_allocated));
        }
        if (is_tdm_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_tdm_delete(unit, group, flags, nof_replications, rep_array));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter(unit, group, flags, NULL, action,
                                                        nof_replications, rep_array, &rep_count_out));
            /*
             * update replication per core for egress group
             */
            if (!DNX_MULTICAST_IS_INGRESS_GROUP(flags))
            {
                for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_exr_core_rep_exist(unit, group, core_id, &rep_exist));
                    SHR_IF_ERR_EXIT(dnx_multicast_egr_core_enable_set(unit, group, core_id, rep_exist));
                }
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_multicast_stat_control_set()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_stat_control_set_verify(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int arg)
{
    uint32 legal_flags;
    int nof_command;
    SHR_FUNC_INIT_VARS(unit);

    /** check for valid core - BCM_CORE_ALL also allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    legal_flags = BCM_MULICAST_STAT_EXTERNAL;

    if ((flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }
    if (type == bcmMulticastStatControlCountAllCopiesAsOne)
    {
        if (arg != FALSE && arg != TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal arg (=%d) value", arg);
        }

        nof_command = dnx_data_crps.src_interface.command_id_get(unit, bcmStatCounterInterfaceIngressReceivePp)->size;
        if (command_id < 0 || command_id > nof_command - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal command_id, got (=%d), for max value (=%d)", command_id,
                         nof_command - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_multicast_stat_count_copies_symmetric_verify(
    int unit,
    int flags,
    int command_id)
{
    int first_core_arg;
    int current_core_arg;
    int nof_cores;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    core = 0;
    SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_get(unit, core, flags, command_id, &first_core_arg));

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (core = 1; core < nof_cores; core++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_get(unit, core, flags, command_id, &current_core_arg));

        if (first_core_arg != current_core_arg)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "get count copies for asymetric configuration requier a specific core, got BCM_CORE_ALL");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_multicast_stat_control_get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_stat_control_get_verify(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int *arg)
{
    uint32 legal_flags;
    SHR_FUNC_INIT_VARS(unit);
  
    /** check for valid core */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    legal_flags = BCM_MULICAST_STAT_EXTERNAL;

    if ((flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /** BCM_CORE_ALL not allowed for symmetric configuration only API*/
    if (core_id == BCM_CORE_ALL)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_multicast_stat_count_copies_symmetric_verify(unit, flags, command_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - statistics control API, related to gather statistics on multicast packets in CRPS/STIF.
 *      currently refer to ingress multicast only.
 *      valid types:
 *      1. bcmMulticastStatControlCountAllCopiesAsOne - count all copies or just original. (for CRPS configured per command_id, for STIF single configuration)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id
 *   \param [in] type - type of control
 *   \param [in] arg - arg of control
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_stat_control_set(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_stat_control_set_verify(unit, core_id, flags, command_id, type, arg));

    switch (type)
    {
        case bcmMulticastStatControlCountAllCopiesAsOne:
            SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_set(unit, core_id, flags, command_id, arg));
            break;
        default:
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - statistics control API, related to gather statistics on multicast packets in CRPS/STIF.
 *      currently refer to ingress multicast only.
 *      valid types:
 *      1. bcmMulticastStatControlCountAllCopiesAsOne - count all copies or just original. (for CRPS configured per command_id, for STIF single configuration)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - ALL/0/1
 *   \param [in] flags - BCM_MULICAST_STAT_EXTERNAL for STIF counters, otherwise CRPS
 *   \param [in] command_id - counter processor command id
 *   \param [in] type - type of control
 *   \param [out] arg - arg of control
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_stat_control_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int command_id,
    bcm_multicast_stat_control_type_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_stat_control_get_verify(unit, core_id, flags, command_id, type, arg));

    switch (type)
    {
        case bcmMulticastStatControlCountAllCopiesAsOne:
            SHR_IF_ERR_EXIT(dnx_multicast_stat_count_copies_get(unit, core_id, flags, command_id, arg));
            break;
        default:
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}
/**
 * \brief - Verify function for BCM-API: bcm_multicast_group_get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap ID created at bcm_multicast_create API
 *   \param [in] flags - variable to hold the requested flags for the specified group
 *
 * \remark
 *  This verifies that the group id is in appropriate range and that the flags pointer is not NULL
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_group_get_verify(
    int unit,
    bcm_multicast_t group,
    uint32 *flags)
{
    bcm_multicast_t group_id;
    int type = 0;
    SHR_FUNC_INIT_VARS(unit);

    /**
      * Verify flags pointer
      */
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "Flags");

    /**
      * Get the Multicast goup ID and type
      */
    group_id = _BCM_MULTICAST_ID_GET(group);
    type = _SHR_MULTICAST_TYPE_GET(group);

    if ((type == _BCM_MULTICAST_TYPE_ING_BMP) || (type == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        /**
          * Verify bitmap group ID
          */
        if ((group_id >= dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit)) &&
            (group_id >= dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Bitmap group ID out of range (0x%x)\n", group_id);
        }
    }
    else if (type == 0)
    {
        /**
          * Verify link list and BIER group ID
          */
        if ((group_id > dnx_data_multicast.params.max_ing_mc_groups_get(unit)) &&
            (group_id > dnx_data_multicast.params.max_egr_mc_groups_get(unit)) &&
            (group_id > dnx_data_bier.params.nof_bfr_entries_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Linked list or BIER group ID out of range (0x%x)\n", group_id);
        }
    }
    else
    {
        /**
          * This is not supported type, nor type 0 (TM)
          */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported group type (%d)\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the flags of Ingress or Egress Multicast groups
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap ID created at bcm_multicast_create API
 *   \param [out] flags - variable to hold the requested flags for the specified group
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_multicast_group_get(
    int unit,
    bcm_multicast_t group,
    uint32 *flags)
{
    uint32 is_open_egr = FALSE;
    uint32 is_open_ing = FALSE;
    uint8 is_tdm_allocated = FALSE;
    int set_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_group_get_verify(unit, group, flags));

    /**
      * Init flags to 0
      */
    *flags = 0;

    /**
      * Check if group is of bitmap type.
      */
    if ((_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_ING_BMP) ||
        (_SHR_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_EGR_BMP))
    {
        /**
          * If group is of bitmap type, add relevant flag.
          */
        *flags |= BCM_MULTICAST_TYPE_PORTS_GROUP;
    }
    else
    {
        /**
          * If group is not of bitmap type, it might be BIER.
          * Check if BIER group is created with the given group ID.
          * Size equal to zero indicates that group is not created.
          */
        if (dnx_data_bier.params.nof_bfr_entries_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));
            if (set_size != 0)
            {
                /**
                  * If BIER group is created, add relevant flag according to size.
                  */
                if (set_size == 64)
                {
                    *flags |= BCM_MULTICAST_BIER_64_GROUP;
                }
                else if (set_size == 128)
                {
                    *flags |= BCM_MULTICAST_BIER_128_GROUP;
                }
                else
                {
                    *flags |= BCM_MULTICAST_BIER_256_GROUP;
                }
            }
        }
    }

    /**
      * Check if group is open as Egress group.
      */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                    (unit, _BCM_MULTICAST_ID_GET(group), *flags | BCM_MULTICAST_EGRESS_GROUP, &is_open_egr));

    /**
      * Check if group is open as Ingress group.
      */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                    (unit, _BCM_MULTICAST_ID_GET(group), *flags | BCM_MULTICAST_INGRESS_GROUP, &is_open_ing));

    /**
      * Add the relevant flags.
      */
    if (is_open_egr == TRUE)
    {
        *flags |= BCM_MULTICAST_EGRESS_GROUP;
    }
    if (is_open_ing == TRUE)
    {
        *flags |= BCM_MULTICAST_INGRESS_GROUP;
    }

    /**
      * Always return BCM_MULTICAST_WITH_ID flag.
      */
    *flags |= BCM_MULTICAST_WITH_ID;

    SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.is_allocated(unit, _BCM_MULTICAST_ID_GET(group), &is_tdm_allocated));
    if (is_tdm_allocated)
    {
        *flags |= BCM_MULTICAST_EGRESS_TDM_GROUP;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
