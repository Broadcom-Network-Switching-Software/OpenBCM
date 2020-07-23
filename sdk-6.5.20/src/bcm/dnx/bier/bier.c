/** \file src/bcm/dnx/bier/bier.c
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

#include <bcm/multicast.h>
#include <bcm/types.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/bier/bier.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/multicast_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bier.h>

#include "bier_imp.h"
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

/*
 * See .h
 */
shr_error_e
dnx_multicast_bfr_id_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group)
{
    int set_size;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_MULTICAST_BIER_64_GROUP)
    {
        set_size = 64;
    }
    else if (flags & BCM_MULTICAST_BIER_128_GROUP)
    {
        set_size = 128;
    }
    /** BCM_MULTICAST_BIER_256_GROUP */
    else
    {
        set_size = 256;
    }

    /*
     * bfr-id group needs to be multiply of set size because MC-ID is the base for BFR set.
     */
    if (group % set_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "multicast group for bfr-id needs to be multiply of set size (%u) but its (%u)\n",
                     set_size, group);
    }

    /*
     * bfr-id group range check
     */
    if (group >= dnx_data_bier.params.nof_bfr_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "multicast group (%u) is not at the valid range (>= %u)\n", group,
                     dnx_data_bier.params.nof_bfr_entries_get(unit));
    }

    /*
     * verify that the group is created. Size equal to zero indicated that group is not created.
     */
    SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));
    if (set_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Multicast ID is already created\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for BCM-API: bcm_dnx_multicast_add/remove/set/get()
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group - multicast group id specifies the set
 *   \param [in] bfr_id - contains the BFR-ID inside the set
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_multicast_bfr_id_add_delete_get_verify(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id)
{
    int set_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * At egress only mode ingress replication can not be added
     */
    if (DNX_MULTICAST_IS_INGRESS(flags) && dnx_data_bier.params.egress_only_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress replication can not be added at egress only mode\n");
    }

    /*
     * verify that one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags used with this API
     */
    if (!DNX_MULTICAST_IS_INGRESS(flags) && !DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "one of BCM_MULTICAST_EGRESS_GROUP/BCM_MULTICAST_INGRESS_GROUP flags must be used with this API\n");
    }

    /*
     * BCM_MULTICAST_EGRESS_GROUP & BCM_MULTICAST_INGRESS_GROUP flags should not be used together
     */
    if (DNX_MULTICAST_IS_INGRESS(flags) && DNX_MULTICAST_IS_EGRESS(flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM_MULTICAST_EGRESS_GROUP & BCM_MULTICAST_INGRESS_GROUP flags should not be used together\n");
    }

    /*
     * verify that the group is created. Size equal to zero indicated that group is not created.
     * Verify that the BFR ID at group range
     */
    SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));
    if (set_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Multicast ID 0x%x is not created\n", group);
    }
    if (bfr_id >= set_size)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BFR ID (%u) is out of group BFR ID range 0:%u\n", bfr_id, set_size - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
int
dnx_multicast_bier_destroy(
    int unit,
    bcm_multicast_t group)
{
    int set_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify that the group is created. Size equal to zero indicated that group is not created.
     */
    if (group < dnx_data_bier.params.nof_bfr_entries_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));
    }
    if (set_size)
    {
        /*
         * size set back to zero.
         */
        SHR_IF_ERR_EXIT(dnx_bier_db.set_size.set(unit, group, 0));
        /*
         * set group entries to invalid.
         */
        SHR_IF_ERR_EXIT(dnx_bier_group_invalid_set(unit, group, set_size));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
int
dnx_multicast_bier_create(
    int unit,
    uint32 flags,
    bcm_multicast_t group)
{
    int set_size;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_MULTICAST_BIER_64_GROUP)
    {
        set_size = 64;
    }
    else if (flags & BCM_MULTICAST_BIER_128_GROUP)
    {
        set_size = 128;
    }
    /** BCM_MULTICAST_BIER_256_GROUP */
    else
    {
        set_size = 256;
    }

    SHR_IF_ERR_EXIT(dnx_bier_db.set_size.set(unit, group, set_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implementation for bcm_multicast_bfr_id_add API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group - multicast group id specifies the set
 *   \param [in] bfr_id - contains the BFR-ID inside the set
 *   \param [in] replication - contains the group replication
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   This API updates the set BFT for the requested BFR-ID
 *   For the BFR-ID entry the replication and the bitstring including BFR-ID neighbors are updated
 *   All neighbors bitstring entries are updated with the BFR-ID bit representation as well
 *
 * \see
 *   * None
 */
int
dnx_multicast_bfr_id_add(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication)
{
    /** holds the other set entries data */
    dnx_bier_mcdb_dbal_field_t set_entry_val;
    /** holds the bfr entry data */
    dnx_bier_mcdb_dbal_field_t entry_val;
    uint32 set_bfr_id;
    int set_size;
    int core_id, core_id_idx;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (DNX_MULTICAST_IS_INGRESS(flags) && dnx_data_bier.params.ingress_bfr_pos_msb_start_get(unit))
    {
        bfr_id = dnx_data_bier.params.bitstring_size_get(unit) - bfr_id - 1;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_bfr_id_add_delete_get_verify(unit, flags, group, bfr_id));
    sal_memset(&entry_val, 0, sizeof(entry_val));

    /*
     * convert gport and CUD to HW values and clear bfr bitstring
     */
    SHR_IF_ERR_EXIT(dnx_bier_convert_gport_to_hw(unit, DNX_MULTICAST_IS_INGRESS(flags),
                                                 replication.port, replication.encap1, &core_id, &entry_val, TRUE));
    SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));

    /*
     * iterate over both cores at egress in order to configure both cores
     */
    DNXCMN_CORES_ITER(unit, core_id, core_id_idx)
    {
        /*
         * iterate over all the entries in the set and update them with the new member
         */
        for (set_bfr_id = 0; set_bfr_id < set_size; set_bfr_id++)
        {
            sal_memset(&set_entry_val, 0, sizeof(set_entry_val));
            SHR_IF_ERR_EXIT(dnx_bier_entry_get
                            (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id, &set_entry_val));

            /*
             * bfr need to be removed from invalid entries
             */
            if (set_entry_val.valid == FALSE)
            {
                SHR_BITCLR(set_entry_val.bmp, bfr_id);
                SHR_IF_ERR_EXIT(dnx_bier_entry_set
                                (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id,
                                 &set_entry_val));
            }

            /*
             * bfr added to be bit string of the entries with the same destination
             */
            else if (set_entry_val.dest == entry_val.dest && set_entry_val.cud == entry_val.cud)
            {
                SHR_BITSET(set_entry_val.bmp, bfr_id);
                SHR_IF_ERR_EXIT(dnx_bier_entry_set
                                (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id,
                                 &set_entry_val));
                SHR_BITSET(entry_val.bmp, set_bfr_id);
            }
        }

        /*
         * update the bfr entry for the desired bfr.
         */
        entry_val.valid = TRUE;
        SHR_BITSET(entry_val.bmp, bfr_id);
        SHR_IF_ERR_EXIT(dnx_bier_entry_set(unit, DNX_MULTICAST_IS_INGRESS(flags), core_id, group + bfr_id, &entry_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implementation for bcm_multicast_bfr_id_delete API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group - multicast group id specifies the set
 *   \param [in] bfr_id - contains the BFR-ID inside the set
 *   \param [in] replication - contains the group replication
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   This API updates the set BFT for the requested BFR-ID
 *   For the BFR-ID entry the replication and the bitstring including BFR-ID neigbors are updated
 *   All neigbors bitstring entries are updated with the BFR-ID bit representation as well
 * \see
 *   * None
 */
int
dnx_multicast_bfr_id_delete(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication)
{
    /** holds the other set entries data */
    dnx_bier_mcdb_dbal_field_t set_entry_val;
    /** holds the bfr entry data */
    dnx_bier_mcdb_dbal_field_t entry_val;
    uint32 set_bfr_id;
    int set_size;
    int core_id;
    int core_id_idx;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (DNX_MULTICAST_IS_INGRESS(flags) && dnx_data_bier.params.ingress_bfr_pos_msb_start_get(unit))
    {
        bfr_id = dnx_data_bier.params.bitstring_size_get(unit) - bfr_id - 1;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_bfr_id_add_delete_get_verify(unit, flags, group, bfr_id));
    sal_memset(&entry_val, 0, sizeof(entry_val));

    /*
     * convert gport and CUD to HW values and clear bfr bitstring
     */
    SHR_IF_ERR_EXIT(dnx_bier_convert_gport_to_hw(unit, DNX_MULTICAST_IS_INGRESS(flags),
                                                 replication.port, replication.encap1, &core_id, &entry_val, FALSE));
    SHR_IF_ERR_EXIT(dnx_bier_db.set_size.get(unit, group, &set_size));

    /*
     * iterate over cores at egress in case the replication belongs to both cores.
     */
    DNXCMN_CORES_ITER(unit, core_id, core_id_idx)
    {
        /*
         * update the bfr bitstrings at the same set which having the same destination
         */
        for (set_bfr_id = 0; set_bfr_id < set_size; set_bfr_id++)
        {
            sal_memset(&set_entry_val, 0, sizeof(set_entry_val));
            SHR_IF_ERR_EXIT(dnx_bier_entry_get
                            (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id, &set_entry_val));

            /*
             * add bfr to invalid entries
             */
            if (set_entry_val.valid == FALSE)
            {
                SHR_BITSET(set_entry_val.bmp, bfr_id);
                SHR_IF_ERR_EXIT(dnx_bier_entry_set
                                (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id,
                                 &set_entry_val));
                SHR_BITSET(entry_val.bmp, set_bfr_id);
            }
            /*
             * remove bfr from bfr entries with the same destination
             */
            else if (set_entry_val.dest == entry_val.dest && set_entry_val.cud == entry_val.cud)
            {
                SHR_BITCLR(set_entry_val.bmp, bfr_id);
                SHR_IF_ERR_EXIT(dnx_bier_entry_set
                                (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + set_bfr_id,
                                 &set_entry_val));
            }
        }

        /*
         * update the bfr entry for the desired bfr with invalid and bitstring of all invalid entries
         */
        entry_val.valid = FALSE;
        SHR_BITSET(entry_val.bmp, bfr_id);
        SHR_IF_ERR_EXIT(dnx_bier_entry_set
                        (unit, DNX_MULTICAST_IS_INGRESS(flags), core_id_idx, group + bfr_id, &entry_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - implementation for bcm_multicast_bfr_id_get API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress
 *   \param [in] group - multicast group id specifies the set
 *   \param [in] bfr_id - contains the BFR-ID inside the set
 *   \param [in] replication - contains the group replication
 *   
 * \par INDIRECT INPUT:
 *    * None
 * \par DIRECT OUTPUT:
 *   int 
 * \par INDIRECT OUTPUT
 *    * None
 * \remark
 *   This API get's the BFR-ID replication by accessing the MCDB
 * \see
 *    * None
 */
int
dnx_multicast_bfr_id_get(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t * replication)
{
    dnx_bier_mcdb_dbal_field_t entry_val;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_bfr_id_add_delete_get_verify(unit, flags, group, bfr_id));

    /*
     * loop over the cores to get the valid replications
     */
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_bier_entry_get(unit, DNX_MULTICAST_IS_INGRESS(flags), core_id, group + bfr_id, &entry_val));
        SHR_IF_ERR_EXIT(dnx_bier_convert_hw_to_gport(unit, DNX_MULTICAST_IS_INGRESS(flags),
                                                     core_id, entry_val, &replication->port, &replication->encap1));
        if (DNX_MULTICAST_IS_INGRESS(flags) || entry_val.valid != FALSE)
        {
            break;
        }
    }

    if (entry_val.valid == FALSE)
    {
        replication->port = BCM_GPORT_INVALID;
        replication->encap1 = BCM_IF_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
int
dnx_bier_mcdb_size_get(
    int unit)
{
    uint32 copies_per_entry;
    uint32 bundle_bfr_entries;

    /*
     * if there is no ingress replication in the system optimization can be made to save only 2 copies per BFT
     */
    if (dnx_data_bier.params.egress_only_get(unit))
    {
        copies_per_entry = 2;
    }
    else
    {
        copies_per_entry = 3;
    }

    /*
     * the number of mcdb entries needed is calculated according to the number of bfr_entries requested multiplied by the number of copies.
     */
    bundle_bfr_entries = dnx_data_bier.params.bundle_nof_banks_net_get(unit) *
        dnx_data_bier.params.bank_entries_get(unit);
    return copies_per_entry * dnx_data_bier.params.bfr_entry_size_get(unit) *
        dnx_data_bier.params.bundle_nof_banks_get(unit) *
        UTILEX_ALIGN_UP(dnx_data_bier.params.nof_bfr_entries_get(unit), bundle_bfr_entries) /
        dnx_data_bier.params.bundle_nof_banks_net_get(unit);
}

/*
 * See .h
 */
int
dnx_bier_init(
    int unit)
{
    int bier_start_offset = 0;
    uint32 mcdb_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * the number of mcdb entries needed is calculated according to the number of bfr_entries requested multiplied by the number of copies.
     */
    mcdb_entries = dnx_bier_mcdb_size_get(unit);

    /*
     * check whether there are enough MCDB entries to support requested BFR-id entries.
     */
    if (mcdb_entries > dnx_data_multicast.params.nof_mcdb_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "there are not enough resources. available (%u), needed (%u)",
                     dnx_data_multicast.params.nof_mcdb_entries_get(unit), mcdb_entries);
    }

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * init bier data base
         */
        SHR_IF_ERR_EXIT(dnx_bier_db.init(unit));

        /*
         * reserve the number of entries which needed from multicast group pool
         * if the space needed by bier is not available error will be returned.
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.allocate_several(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, mcdb_entries, NULL,
                                                           &bier_start_offset));

    }
    /*
     * set bier offsets to HW.
     */
    SHR_IF_ERR_EXIT(dnx_bier_dbal_init(unit, mcdb_entries));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
int
dnx_bier_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
