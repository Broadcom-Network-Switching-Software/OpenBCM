/** \file l3_ecmp_vip.c
 *
 * Handles the control over the ECMPs entries.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */

#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/l3/l3_ecmp_vip.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <bcm/types.h>
#include <bcm_int/dnx/stat/stat_pp.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define L3_ECMP_VIP_SUPPORTED_BCM_L3_FLAGS              (BCM_L3_WITH_ID | BCM_L3_REPLACE)
#define L3_ECMP_VIP_SUPPORTED_BCM_L3_ECMP_FLAGS         (0)

#define LEM_PCC_STATE_ENTRY_GROUPING                    (0)
#define LEM_PCC_STATE_MAC_STRENGTH                      (DYNAMIC_MACT_ENTRY_STRENGTH)

#define VIP_ID_FROM_ECMP_ITF(vip_id, ecmp_intf) \
    vip_id = (ecmp_intf & 0xfff);

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_fec_id_min_get(
    int unit,
    uint32 *fec_id_min)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MIN, INST_SINGLE, fec_id_min));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_fec_id_max_get(
    int unit,
    uint32 *fec_id_max)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MAX, INST_SINGLE, fec_id_max));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_vip_ecmp_table_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!sw_state_is_warm_boot(unit))
    {
#endif /* BCM_WARM_BOOT_SUPPORT */

        /*
         * Initialize the VIP ECMP FEC min and max in virtual registers for ucode processing 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MIN, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MAX, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Save the VIP ECMP FEC min and max in SW DB for quick access 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MIN, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MAX, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        max_vip_ecmp_table_size = dnx_data_l3.vip_ecmp.max_vip_ecmp_table_size_get(unit);;

        /*
         * Set the initial value of SLLB table size in the SW table 
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &max_vip_ecmp_table_size));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE,
                                     max_vip_ecmp_table_size);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the user input for the VIP ECPM create function.
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \param [in] intf_count - The number of ECMP members in the group. 
 *                          For VIP ECMP, it must match the size configure with bcmSwitchVIPEcmpTableSize.
 * \param [in] intf_array - Array of the ECMP members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_vip_create
 */
static shr_error_e
dnx_l3_egress_ecmp_vip_create_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{

    uint32 fec_range_min;
    uint32 fec_range_max;
    uint32 vip_id;
    uint8 is_allocated;
    uint32 table_size;
    uint32 entry_handle_id;
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ecmp NULL check is performed in the calling function 
     */
    SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "bcm_if_t");

    /*
     * This function should only be called when dynamic_mode is BCM_L3_ECMP_DYNAMIC_MODE_VIP 
     */
    if (ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_VIP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported dynamic mode.\n");
    }

    /*
     * Check supported flags 
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, ~L3_ECMP_VIP_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x) when dynamic_mode is VIP.",
                     ecmp->flags, L3_ECMP_VIP_SUPPORTED_BCM_L3_FLAGS);
    }
    if (_SHR_IS_FLAG_SET(ecmp->ecmp_group_flags, ~L3_ECMP_VIP_SUPPORTED_BCM_L3_ECMP_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x) when dynamic_mode is VIP.",
                     ecmp->ecmp_group_flags, L3_ECMP_VIP_SUPPORTED_BCM_L3_ECMP_FLAGS);
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_max_get(unit, &fec_range_max));

    /*
     * Check that the vip ecmp fec id range has been set
     */
    if (fec_range_min == 0 || (fec_range_max - fec_range_min + 1) != dnx_data_l3.vip_ecmp.nof_vip_ecmp_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The VIP ECMP FEC-ID range minimum must be set with bcmSwitchVIPL3EgressIdMin before calling bcm_l3_egress_ecmp_create(). bcmSwitchVIPL3EgressIdMin = %d ",
                     fec_range_min);

    }

    /*
     * If WITH_ID, check if FEC-Ptr is in valid range
     * If WITH_ID, check that the VIP FEC-Ptr has been allocated
     * Check the intf_count == SLLB table size
     * Verify hat all the members are allocated FEs
     */
    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        /*
         * Check that the provided intf is in range
         */
        if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < fec_range_min)
            || (BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) > fec_range_max))

        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The FEC Id specified WITH_ID (%d) is not in the VIP FEC range (%d to %d).",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), fec_range_min, fec_range_max);

        }

        vip_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) - fec_range_min;
        /*
         * If REPLACE, check that the intf has been allocated
         */
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.is_allocated(unit, vip_id, &is_allocated));
            if (!is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The FEC Id specified WITH_ID (%d) and REPLACE is not allocated.",
                             BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
            }
        }
    }
    else
    {
        /*
         * If not WITH_ID, check that the intf is 0
         */
        if (BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not provided, but user has specified an ECMP interface %d.\n",
                         BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
        }

        /*
         * If not WITH_ID, REPLACE should not be set
         */
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flag BCM_L3_WITH_ID must be used with BCM_L3_REPLACE.");
        }
    }

    /*
     * Check that the intf_count matches the switch configured SLLB table size
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));

    if (intf_count != table_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf_count must match ECMP_VIP_CONFIG_SW TABLE_SIZE %d.\n", table_size);
    }

    /*
     * Check that all members of intf_array are specified
     */
    for (ii = 0; ii < intf_count; ii++)
    {
        if (intf_array[ii] == BCM_GPORT_INVALID)
        {
            if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
            {
                continue;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "intf_array member BCM_GPORT_INVALID is only supported for BCM_L3_REPLACE.");
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_create(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_count,
    bcm_if_t * intf_array)
{
    uint32 entry_handle_id;
    uint32 fec_range_min;
    uint32 vip_ecmp_fec_id;
    uint32 alloc_flags;
    uint32 member_index;
    uint32 vip_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_create_verify(unit, ecmp, intf_count, intf_array));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));

    if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_WITH_ID))
    {
        vip_ecmp_fec_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
        vip_id = vip_ecmp_fec_id - fec_range_min;
        if (!_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            /*
             * Allocate vip_id 
             */
            alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
            SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.
                            vip_ecmp_res_manager.allocate_single(unit, alloc_flags, NULL, (int *) &vip_id));
        }
    }
    else
    {
        /*
         * Allocate a new VIP FEC-Id 
         */
        alloc_flags = 0;
        SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.
                        vip_ecmp_res_manager.allocate_single(unit, alloc_flags, NULL, (int *) &vip_id));
        vip_ecmp_fec_id = vip_id + fec_range_min;
        BCM_L3_ITF_SET(ecmp->ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
    }

    for (member_index = 0; member_index < intf_count; member_index++)
    {
        if ((_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE)) && (intf_array[member_index] == BCM_GPORT_INVALID))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_CONSISTENT_HASH, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_REFERENCE, member_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE,
                                     BCM_L3_ITF_VAL_GET(intf_array[member_index]));
        if (_SHR_IS_FLAG_SET(ecmp->flags, BCM_L3_REPLACE))
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
        }
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify the VIP ECMP group has been allocated. 
 * \param [in] unit - The unit number.
 * \param [in] ecmp - The ECMP group information.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * dnx_l3_egress_ecmp_vip_destroy
 *  * dnx_l3_egress_ecmp_vip_get
 */
static shr_error_e
dnx_l3_egress_ecmp_vip_group_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{

    uint32 fec_range_min;
    uint32 fec_range_max;
    uint32 vip_id;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ecmp, _SHR_E_PARAM, "bcm_l3_egress_ecmp_t");

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_max_get(unit, &fec_range_max));

    /*
     * Check that the provided intf is in range
     */
    if ((BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) < fec_range_min) || (BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) > fec_range_max))

    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FEC Id specified (%d) is not in the VIP FEC range (%d to %d).",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf), fec_range_min, fec_range_max);

    }

    vip_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf) - fec_range_min;
    SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.is_allocated(unit, vip_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The VIP FEC Id specified (%d) has not been created.",
                     BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_destroy(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp)
{
    uint32 vip_ecmp_fec_id;
    uint32 vip_id;
    uint32 member_index;
    uint32 fec_range_min;
    uint32 entry_handle_id;
    uint32 table_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_group_verify(unit, ecmp));

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    vip_ecmp_fec_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    vip_id = vip_ecmp_fec_id - fec_range_min;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));

    for (member_index = 0; member_index < table_size; member_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_CONSISTENT_HASH, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_REFERENCE, member_index);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

    SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.free_single(unit, vip_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_get(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    uint32 fec_range_min;
    uint32 vip_ecmp_fec_id;
    uint32 vip_id;
    uint32 entry_handle_id;
    uint32 table_size;
    uint32 kaps_result;
    uint32 index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_group_verify(unit, ecmp));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    vip_ecmp_fec_id = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
    vip_id = vip_ecmp_fec_id - fec_range_min;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));

    if (intf_size < table_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "intf_size (%d) must be equal to table size (%d).", intf_size, table_size);
    }

    for (index = 0; index < table_size; index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_CONSISTENT_HASH, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER_REFERENCE, index);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &kaps_result));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
        BCM_L3_ITF_SET(intf_array[index], BCM_L3_ITF_TYPE_FEC, kaps_result);
    }
    *intf_count = index;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_vip_traverse(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data)
{
    bcm_l3_egress_ecmp_t ecmp;
    int intf_count;
    bcm_if_t *intf_array = NULL;
    uint32 table_size;
    uint32 fec_range_min;
    uint32 fec_range_max;
    uint32 vip_ecmp_fec_id;
    uint32 entry_handle_id;
    uint8 is_allocated;
    uint32 vip_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_ALLOC(intf_array, (sizeof(bcm_if_t) * DNX_DATA_MAX_L3_VIP_ECMP_MAX_VIP_ECMP_TABLE_SIZE), "VIP ECMP intf_array",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_max_get(unit, &fec_range_max));

    for (vip_ecmp_fec_id = fec_range_min; vip_ecmp_fec_id <= fec_range_max; vip_ecmp_fec_id++)
    {
        vip_id = vip_ecmp_fec_id - fec_range_min;
        SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.is_allocated(unit, vip_id, &is_allocated));
        if (is_allocated)
        {
            bcm_l3_egress_ecmp_t_init(&ecmp);
            BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_ecmp_fec_id);
            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_get(unit, &ecmp, table_size, intf_array, &intf_count));
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_VIP;
            SHR_IF_ERR_EXIT((*trav_fn) (unit, &ecmp, intf_count, intf_array, user_data));
            SHR_NULL_CHECK(intf_array, _SHR_E_PARAM, "trav_fn must not free bcm_if_t *intf_array");
        }
    }

exit:
    SHR_FREE(intf_array);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_add(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * entry)
{
    uint32 fec_range_min;
    uint32 vip_id;
    uint32 signature;
    bcm_gport_t intf_gport;
    uint32 destination;
    uint32 is_default = 0;
    uint32 kbp_zeros[2] = { 0, 0 };
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "bcm_l3_egress_ecmp_resilient_entry_t");

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL) && !(dnx_kbp_device_enabled(unit) && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb)))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "KBP_PCC_STATE_TABLE is not enabled.\n");
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));

    vip_id = BCM_L3_ITF_VAL_GET(entry->ecmp.ecmp_intf) - fec_range_min;
    signature = (uint32) (entry->hash_key & UTILEX_U32_MAX);

    BCM_GPORT_FORWARD_PORT_SET(intf_gport, entry->intf);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, intf_gport, &destination));

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PCC_STATE, &entry_handle_id));
        /*
         * key 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, signature);
        /*
         * result 
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_PCC_STATE_FWD_MACT_RESULT_NO_OUTLIF);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE,
                                     LEM_PCC_STATE_ENTRY_GROUPING);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE,
                                     LEM_PCC_STATE_MAC_STRENGTH);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }
    else        /* if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)) */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_PCC_STATE_TABLE, &entry_handle_id));
        /*
         * key 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, signature);
        /*
         * result 
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_KBP_PCC_STATE_TABLE_KBP_DEST_W_DEFAULT);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, is_default);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA, INST_SINGLE,
                                         kbp_zeros);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_delete(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * entry)
{
    uint32 fec_range_min;
    uint32 vip_id;
    uint32 signature;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry, _SHR_E_PARAM, "bcm_l3_egress_ecmp_resilient_entry_t");

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL) && !(dnx_kbp_device_enabled(unit) && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb)))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "KBP_PCC_STATE_TABLE is not enabled.\n");
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    vip_id = BCM_L3_ITF_VAL_GET(entry->ecmp.ecmp_intf) - fec_range_min;
    signature = (uint32) (entry->hash_key & UTILEX_U32_MAX);

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PCC_STATE, &entry_handle_id));
        /*
         * key 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, signature);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else        /* if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL)) */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_PCC_STATE_TABLE, &entry_handle_id));
        /*
         * key 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_VIP_ID, vip_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, signature);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


/**
 * \brief
 *  Compares match criteria against the PCC resilient table entry. 
 * \param [in] unit -
 *   The unit number.
 * \param [in] flags -
 *   supported flags:
 *   - BCM_L3_ECMP_RESILIENT_MATCH_ECMP: Compare input match_vip_id to entry vip_id
 *   - BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY: Compare input signature to entry signature
 *   - BCM_L3_ECMP_RESILIENT_MATCH_INTF: Compare input intf to entry intf
 * \param [in] match_vip_id - vip_id for comparison to the entry vip_id
 * \param [in] match_signature - signature for comparison to the entry signature
 * \param [in] match_destination - intf for comparison to the entry intf
 * \param [in] entry_handle_id - dbal handle to entry
 * \param [in] dbal_field_vip_id - dbal field enum type for VIP_ID in LEM or KBP.
 * \return
 *   \retval _SHR_E_NOT_FOUND if no match
 *   \retval _SHR_E_EXISTS if entry matches criteria
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_match_check(
        int unit,
        uint32 flags,
        uint32 match_vip_id,
        uint32 match_signature,
        uint32 match_destination,
        uint32 entry_handle_id,
        dbal_fields_e dbal_field_vip_id)
{
    uint32 entry_vip_id=0;
    uint32 entry_signature=0;
    uint32 entry_destination=0;
    uint32 match_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, dbal_field_vip_id, &entry_vip_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, &entry_signature));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &entry_destination));

    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_MATCH_ECMP)) && (entry_vip_id == match_vip_id))
    {
        match_flags |= BCM_L3_ECMP_RESILIENT_MATCH_ECMP;
    }
    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY)) && (entry_signature == match_signature))
    {
        match_flags |= BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY;
    }
    if ((_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_MATCH_INTF)) && (entry_destination == match_destination))
    {
        match_flags |= BCM_L3_ECMP_RESILIENT_MATCH_INTF;
    }
    if (match_flags == (flags & (BCM_L3_ECMP_RESILIENT_MATCH_ECMP|BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY|BCM_L3_ECMP_RESILIENT_MATCH_INTF)))
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EXISTS);
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_replace(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    int *num_entries,
    bcm_l3_egress_ecmp_resilient_entry_t * replace_entry)
{
    shr_error_e rv;
    uint32 fec_range_min;
    uint32 match_vip_id;
    uint32 match_signature;
    uint32 match_destination;
    uint32 replace_destination;
    bcm_gport_t intf_gport;
    uint32 entry_handle_id;
    int is_end;
    dbal_fields_e dbal_field_vip_id;
    int match_count = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(match_entry, _SHR_E_PARAM, "bcm_l3_egress_ecmp_resilient_entry_t");
    SHR_NULL_CHECK(replace_entry, _SHR_E_PARAM, "bcm_l3_egress_ecmp_resilient_entry_t");

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL) && !(dnx_kbp_device_enabled(unit) && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb)))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "KBP_PCC_STATE_TABLE is not enabled.\n");
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    match_vip_id = BCM_L3_ITF_VAL_GET(match_entry->ecmp.ecmp_intf) - fec_range_min;
    match_signature = (uint32) (match_entry->hash_key & UTILEX_U32_MAX);
    BCM_GPORT_FORWARD_PORT_SET(intf_gport, match_entry->intf);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, intf_gport, &match_destination));

    BCM_GPORT_FORWARD_PORT_SET(intf_gport, replace_entry->intf);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, intf_gport, &replace_destination));

     
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PCC_STATE, &entry_handle_id));
        dbal_field_vip_id = DBAL_FIELD_VIP_ID;
    }
    else {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_PCC_STATE_TABLE, &entry_handle_id));
        dbal_field_vip_id = DBAL_FIELD_KBP_VIP_ID;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /*
     * is_end variable will be set when the last entry is traversed and iterator_get_next is called again.
     */
    while (!is_end)
    {
        rv = dnx_l3_egress_ecmp_pcc_resilient_match_check(unit, flags, match_vip_id, match_signature, match_destination, entry_handle_id, dbal_field_vip_id);
        if (rv == _SHR_E_EXISTS)
        {
            match_count++;
            /* This entry matches all match criteria */
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_REPLACE))
            {
                /* result */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, replace_destination);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_DELETE))
            {
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            }
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    if (match_count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No matching entry found.\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* 
 * For more details see the .h file
 */
shr_error_e
dnx_l3_egress_ecmp_pcc_resilient_traverse(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn,
    void *user_data)
{
    shr_error_e rv;
    uint32 fec_range_min;
    uint32 match_vip_id;
    uint32 match_signature;
    uint32 match_destination;
    bcm_gport_t intf_gport; 
    uint32 vip_id;
    uint32 signature;
    uint32 destination;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_vip_id;
    bcm_l3_egress_ecmp_resilient_entry_t entry;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    SHR_NULL_CHECK(match_entry, _SHR_E_PARAM, "bcm_l3_egress_ecmp_resilient_entry_t");

    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC_EXTERNAL) && !(dnx_kbp_device_enabled(unit) && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_sllb)))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "KBP_PCC_STATE_TABLE is not enabled.\n");
    }

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &fec_range_min));
    match_vip_id = BCM_L3_ITF_VAL_GET(match_entry->ecmp.ecmp_intf) - fec_range_min;
    match_signature = (uint32) (match_entry->hash_key & UTILEX_U32_MAX);
    BCM_GPORT_FORWARD_PORT_SET(intf_gport, match_entry->intf);
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport(unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, intf_gport, &match_destination));

     
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_ECMP_RESILIENT_PCC))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PCC_STATE, &entry_handle_id));
        dbal_field_vip_id = DBAL_FIELD_VIP_ID;
    }
    else {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_PCC_STATE_TABLE, &entry_handle_id));
        dbal_field_vip_id = DBAL_FIELD_KBP_VIP_ID;
    }

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /*
     * is_end variable will be set when the last entry is traversed and iterator_get_next is called again.
     */

    while (!is_end)
    {
        rv = dnx_l3_egress_ecmp_pcc_resilient_match_check(unit, flags, match_vip_id, match_signature, match_destination, entry_handle_id, dbal_field_vip_id);
        if (rv == _SHR_E_EXISTS)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, dbal_field_vip_id, &vip_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SIGNATURE, &signature));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));

            BCM_L3_ITF_SET(entry.ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, vip_id);
            BCM_L3_ITF_SET(entry.intf, BCM_L3_ITF_TYPE_FEC, destination);
            COMPILER_64_ZERO(entry.hash_key);
            COMPILER_64_SET(entry.hash_key, 0x0, signature);
            SHR_IF_ERR_EXIT((*trav_fn) (unit, &entry, user_data));
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_switch_control_sllb_default_destination_set(
    int unit,
    int arg)
{
    uint32 entry_handle_id;
    uint32 default_destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, arg, &default_destination));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Set the value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_DESTINATION, INST_SINGLE,
                                 default_destination);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_switch_control_sllb_default_destination_get(
    int unit,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 default_destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DEFAULT_ROUTE_DESTINATION, INST_SINGLE, &default_destination));

    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, default_destination, arg));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the requested SLLB table size is supported
 * \param [in] unit -
 *   The unit number.
 * \param [in] table_size -
 *   Size of the VIP ECMP group
 * \return
 *   \retval _SHR_E_NONE if no error
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_switch_control_sllb_table_size_verify(int unit, int table_size)
{
    int nof_vip_ecmp;
    int nof_free_vip_ecmp;
    int max_vip_ecmp_table_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    max_vip_ecmp_table_size = dnx_data_l3.vip_ecmp.max_vip_ecmp_table_size_get(unit);
    if (table_size < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SLLB Table Size must be > 0.\n");
    }
    else if (table_size > max_vip_ecmp_table_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%d exceeds the maximum supported SLLB VIP ECMP table size (%d).\n", table_size,
                     max_vip_ecmp_table_size);
    }
    /*
     * Allow setting table size only if no VIP ECMP group have been allocated.
     */
    nof_vip_ecmp = dnx_data_l3.vip_ecmp.nof_vip_ecmp_get(unit);
    SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.nof_free_elements_get(unit, &nof_free_vip_ecmp));
    if (nof_free_vip_ecmp < nof_vip_ecmp)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "The SLLB Table Size cannot be modified when any VIP ECMP FECs are configured\n");
    }


exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * For more details see the .h file
 */
shr_error_e
dnx_switch_control_sllb_table_size_set(
    int unit,
    int arg)
{
    uint32 entry_handle_id;
    uint32 table_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_size = (uint32)arg;
    SHR_IF_ERR_EXIT(dnx_switch_control_sllb_table_size_verify(unit, table_size));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Set the value
     */
    table_size = (uint32) arg;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, table_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, table_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_switch_control_sllb_table_size_get(
    int unit,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 table_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));

    *arg = (int) table_size;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the requested minimum VIP FEC ID can be supported
 * \param [in] unit -
 *   The unit number.
 * \param [in] nof_vip_ecmp -
 *   The number of VIP ECMP groups supported
 * \param [in] vip_ecmp_fec_id_min -
 *   The minimum VIP ECMP FEC ID in the range
 * \param [in] vip_ecmp_fec_id_max -
 *   The maximum  FEC ID in the range
 * \return
 *   \retval _SHR_E_NONE if no error
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_switch_control_vip_l3_egress_id_min_verify(int unit, int nof_vip_ecmp, int vip_ecmp_fec_id_min, int vip_ecmp_fec_id_max)
{
    int nof_free_vip_ecmp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allow setting vip ecmp fec Id  min only if no VIP ECMP group have been allocated.
     */

    SHR_IF_ERR_EXIT(algo_l3_db.vip_ecmp_alloc.vip_ecmp_res_manager.nof_free_elements_get(unit, &nof_free_vip_ecmp));

    if (nof_free_vip_ecmp < nof_vip_ecmp)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "The VIP ECMP range (min/max) cannot be modified if any VIP ECMP FECs are configured\n");
    }

    /*
     * Check that the range falls within the valid FEC range
     */
    if (vip_ecmp_fec_id_max >= dnx_data_l3.fec.nof_fecs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The VIP ECMP range (min/max) exceeds the valid FEC range, 0 to %d.\n", dnx_data_l3.fec.nof_fecs_get(unit));
    }

    /*
     * Check that the range does not overlap the ECMP FEC range
     */
    if (vip_ecmp_fec_id_min < dnx_data_l3.ecmp.nof_ecmp_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The VIP ECMP range (min/max) cannot overlap the ECMP FEC range, 0 to %d.\n", dnx_data_l3.ecmp.nof_ecmp_get(unit));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For more details see the .h file
 */
shr_error_e
dnx_switch_control_vip_l3_egress_id_min_set(
    int unit,
    int arg)
{
    uint32 entry_handle_id;
    int nof_vip_ecmp;
    uint32 vip_ecmp_fec_id_min;
    uint32 vip_ecmp_fec_id_max;
    uint32 old_min, old_max;
    uint32 range_start;
    uint32 range_size;
    uint32 fec_id;
    int hierarchy_iter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_vip_ecmp = dnx_data_l3.vip_ecmp.nof_vip_ecmp_get(unit);
    vip_ecmp_fec_id_min = (uint32) arg;
    vip_ecmp_fec_id_max = vip_ecmp_fec_id_min + nof_vip_ecmp - 1;
    SHR_IF_ERR_EXIT(dnx_switch_control_vip_l3_egress_id_min_verify(unit, nof_vip_ecmp, vip_ecmp_fec_id_min, vip_ecmp_fec_id_max));

    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_min_get(unit, &old_min));
    SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_vip_fec_id_max_get(unit, &old_max));

    /*
     * If no change, just return 
     */
    if (old_min == vip_ecmp_fec_id_min && old_max == vip_ecmp_fec_id_max)
    {
        SHR_EXIT();
    }

    /*
     * Block updating the range once set 
     */
    if (old_min != 0 && old_max != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "The VIP ECMP range cannot be changed once set.\n");
    }

    /*
     * If any of the ids in the range correspond to fecs in the mdb profile, reserve them for vip fecs 
     */
    for (hierarchy_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
         hierarchy_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); hierarchy_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, hierarchy_iter, &range_start, &range_size));
        for (fec_id = vip_ecmp_fec_id_min; fec_id <= vip_ecmp_fec_id_max; fec_id++)
        {
            if ((fec_id >= range_start) && (fec_id < (range_start + range_size)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocate
                                (unit, (int *) &fec_id, DNX_ALGO_L3_FEC_WITH_ID, hierarchy_iter,
                                 DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT));
            }
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_SLLB, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MIN, INST_SINGLE, vip_ecmp_fec_id_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MAX, INST_SINGLE, vip_ecmp_fec_id_max);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_VIP_CONFIG_SW, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MIN, INST_SINGLE, vip_ecmp_fec_id_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC_RANGE_MAX, INST_SINGLE, vip_ecmp_fec_id_max);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
