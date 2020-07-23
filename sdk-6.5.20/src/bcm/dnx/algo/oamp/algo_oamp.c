/** \file algo_oamp.c
 * 
 *
 * Resource and templates needed for the OAMP OAM and BFD features.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/
#include <bcm/types.h>
#include <bcm/oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/** Number of entries in relevant hardware tables */
#define NOF_ITMH_PRIORITY_PROFILES     8
#define NOF_MPLS_PWE_TTL_EXP_PROFILES 16
#define NOF_PUNT_PROFILES             16
#define NOF_MEP_PROFILES              128

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
 * see documentation in the header file
 */
void
dnx_oam_mep_db_highest_index_get(
    int unit,
    uint32 *highest_index)
{

    uint32 clocks_in_3ms;
    uint32 mep_db_clocks;
    uint32 nof_entries_tot;
    uint32 nof_cycles;
    uint32 data_max;

    /** Number of clocks in 3ms(minimal interval for short entries) */
    clocks_in_3ms = 3 * dnx_data_device.general.core_clock_khz_get(unit);

    /** Number of clocks that take to support one mep index(entry) */
    mep_db_clocks = dnx_data_oam.oamp.mep_sys_clocks_get(unit);

    /** Number of mep indexes(entries),that could be supported in 3ms */
    nof_entries_tot = clocks_in_3ms / mep_db_clocks;

    /**
     *  Number of cycles that take to scan machine to return to next entry in same bank
     *  in worst case(should transmit each second cycle 48maid entry, as it can be splitted by phases)
     *  Number of banks that could have endpoint and have of them could be 48maid(extra data) that take additional cycle
     */
    nof_cycles = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit) +
        (dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit) + 1) / 2;

    *highest_index = (((nof_entries_tot / nof_cycles) >> 2) << 2);

    /** Actual number of short entries that fit in a bank */
    data_max = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    if (*highest_index > data_max)
    {
        *highest_index = data_max;
    }
}

/*
 * This function verify that we doesn't try to allocate entry out of scan area of tx scan machine.
 * Relevant for short entries bank.
 */
static shr_error_e
dnx_oam_index_mep_db_verify(
    int unit,
    uint32 mep_db_index)
{
    uint32 mep_db_threshold;
    uint32 max_num_of_entries_in_bank;
    uint32 mep_db_index_rel;
    uint32 highest_mep_db_index;
    SHR_FUNC_INIT_VARS(unit);

    /** Get mep_db_threshold */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);

    /** Max number of entries per bank*/
    max_num_of_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    /** Relative MEP DB index, relative to start of bank*/
    mep_db_index_rel = mep_db_index % max_num_of_entries_in_bank;

    dnx_oam_mep_db_highest_index_get(unit, &highest_mep_db_index);

    if ((mep_db_threshold > mep_db_index) && (mep_db_index_rel > highest_mep_db_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal MEP ID index. MEP ID %u can't be allocated.\n\t"
                     " Short entry should be between 0 and %u index inside short bank(8K entries in short bank).\n\t",
                     mep_db_index, highest_mep_db_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see documentation in the header file
 */
void
dnx_mep_id_to_pool_type_and_index(
    int unit,
    uint32 mep_id,
    dnx_oamp_mep_db_memory_type_t * memory_type,
    int *index)
{
    int mep_db_treshold;
    int nof_umc_entries;
    int nof_mep_db_entries_per_bank;
    int nof_mep_db_short_entries;
    int nof_umc_banks;
    int nof_max_endpoints;
    uint32 highest_index_in_short_pool;
    int bank;
    int index_in_bank;

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_umc_entries = dnx_data_oam.oamp.nof_umc_get(unit);
    nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    nof_umc_banks = nof_umc_entries / nof_mep_db_entries_per_bank;
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    dnx_oam_mep_db_highest_index_get(unit, &highest_index_in_short_pool);

    if (mep_id >= nof_max_endpoints)
    {
        memory_type->is_extra_pool = 1;
        *index = (mep_id - nof_max_endpoints) / nof_mep_db_short_entries;
    }
    else
    {
        /** Do not assume this field is 0! */
        memory_type->is_extra_pool = 0;

        /** Quarter Pools area */
        if (mep_id < mep_db_treshold)
        {
            memory_type->is_full = 0;
            /*
             * Calculating the bank where enty resides
             */
            bank = mep_id / (nof_mep_db_entries_per_bank * nof_mep_db_short_entries);
            /*
             * Calculating the index inside the bank where entry resides
             */
            index_in_bank = mep_id % (nof_mep_db_entries_per_bank * nof_mep_db_short_entries);

            /** Quarter Long */
            if ((mep_id % nof_mep_db_short_entries == 0) && (mep_id / nof_mep_db_short_entries < nof_umc_entries))
            {
                memory_type->is_2byte_maid = 0;
                *index = bank * (highest_index_in_short_pool / nof_mep_db_short_entries) +
                    index_in_bank / nof_mep_db_short_entries;
            }
            /** Quarter Short */
            else
            {
                memory_type->is_2byte_maid = 1;
                /** We are located in area where one ID relate to ICC MAID pool and 3 ID's to Short pool */
                if (mep_id / nof_mep_db_short_entries < nof_umc_entries)
                {
                    *index = (bank * highest_index_in_short_pool -
                              bank * (highest_index_in_short_pool / nof_mep_db_short_entries)) +
                        (index_in_bank - index_in_bank / nof_mep_db_short_entries - 1);
                }
                /** We are located in area where all four ID's are from 2 Byte MAID Pool(it could be only on Option 2) */
                else
                {
                    *index = (nof_umc_banks * highest_index_in_short_pool -
                              nof_umc_banks * (highest_index_in_short_pool / nof_mep_db_short_entries)) +
                        (bank - nof_umc_banks) * highest_index_in_short_pool + index_in_bank;
                }
            }
        }
        /** Full Pools area */
        else
        {
            memory_type->is_full = 1;
            if (mep_id >= nof_umc_entries * nof_mep_db_short_entries)
            {
                memory_type->is_2byte_maid = 1;
                /**Full 2 Byte MAID area  Option 2 */
                if (mep_db_treshold / nof_mep_db_short_entries > nof_umc_entries)
                {
                    *index = (mep_id - mep_db_treshold) / nof_mep_db_short_entries;
                }
                /**Full 2 Byte MAID area  Option 1 */
                else
                {
                    *index = mep_id / nof_mep_db_short_entries - nof_umc_entries;
                }
            }
            /** Option 2 (above Threshold, below UMC size) */
            else
            {
                memory_type->is_2byte_maid = 0;
                *index = (mep_id - mep_db_treshold) / nof_mep_db_short_entries;
            }
        }
    }
}

/**
 * \brief - Mapping pool type and index inside the pool to mep id.
 *          Used for endpoint id allocation.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] memory_type - type of mep_id memory
 * \param [in] index - mep_id index inside the pool.
 * \param [out] mep_id   - mep id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_mep_id_alloc
 *   dnx_oamp_mep_db_memory_type_t
 */
static void
dnx_pool_type_and_index_to_mep_id(
    int unit,
    dnx_oamp_mep_db_memory_type_t * memory_type,
    int index,
    uint32 *mep_id)
{
    int nof_mep_db_short_entries_in_line;
    int nof_mep_db_entries_per_bank;
    int nof_umc_entries;
    int nof_umc_banks;
    int mep_db_treshold;
    int nof_max_endpoints;
    uint32 highest_index_in_short_pool;
    int nof_short_2byte_entries_in_bank;
    int bank;
    int index_in_bank;

    nof_mep_db_short_entries_in_line = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    nof_umc_entries = dnx_data_oam.oamp.nof_umc_get(unit);
    nof_umc_banks = nof_umc_entries / nof_mep_db_entries_per_bank;
    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    dnx_oam_mep_db_highest_index_get(unit, &highest_index_in_short_pool);


    if (memory_type->is_extra_pool)
    {
        *mep_id = nof_max_endpoints + index * nof_mep_db_short_entries_in_line;
    }
    else
    {
        /** F_ICC area */
        if (memory_type->is_2byte_maid && memory_type->is_full)
        {
            /** Option 1 */
            if (mep_db_treshold / nof_mep_db_short_entries_in_line < nof_umc_entries)
            {
                *mep_id = (nof_umc_entries + index) * nof_mep_db_short_entries_in_line;
            }
            /** Option 2 */
            else
            {
                *mep_id = mep_db_treshold + index * nof_mep_db_short_entries_in_line;
            }
        }
        /** S_2B area */
        if (memory_type->is_2byte_maid && !memory_type->is_full)
        {
            /** Number of 2byte maid entries in short line in UMC area */
            nof_short_2byte_entries_in_bank = (highest_index_in_short_pool / nof_mep_db_short_entries_in_line) *
                DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE;

            /** We are located in area where one ID relate to ICC MAID pool and 3 ID's to 2 Byte MAID pool */
            if (index < (nof_short_2byte_entries_in_bank * nof_umc_banks))
            {
                bank = index / nof_short_2byte_entries_in_bank;
                index_in_bank = index % nof_short_2byte_entries_in_bank;

                *mep_id = (bank * nof_mep_db_entries_per_bank * nof_mep_db_short_entries_in_line) +
                    ((index_in_bank / DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) * nof_mep_db_short_entries_in_line) +
                    ((index_in_bank % DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) + 1);
            }
            /** We are located in area where all 4 ID's related to 2 Byte MAID pool */
            else
            {
                bank = (index - nof_short_2byte_entries_in_bank * nof_umc_banks) / highest_index_in_short_pool;
                index_in_bank = (index - nof_short_2byte_entries_in_bank * nof_umc_banks) % highest_index_in_short_pool;

                *mep_id =
                    ((bank + nof_umc_banks) * nof_mep_db_entries_per_bank * nof_mep_db_short_entries_in_line) +
                    index_in_bank;
            }
        }
        /** F_2B area */
        if (!memory_type->is_2byte_maid && memory_type->is_full)
        {
            *mep_id = mep_db_treshold + index * nof_mep_db_short_entries_in_line;
        }
        /** S_ICC area */
        if (!memory_type->is_2byte_maid && !memory_type->is_full)
        {
            /**Bank where EP resides*/
            bank = index / (highest_index_in_short_pool / nof_mep_db_short_entries_in_line);
            /**Index inside bank where EP resides*/
            index_in_bank = index % (highest_index_in_short_pool / nof_mep_db_short_entries_in_line);
            *mep_id =
                (nof_mep_db_entries_per_bank * bank * nof_mep_db_short_entries_in_line) +
                (index_in_bank * nof_mep_db_short_entries_in_line);
        }
    }
}

/*
 * For the BFD over IPv6 update implementation we use resource manager
 * to manage all the tuples of size of nof_bfd_ipv6_extra_data_entries
 * with distance equals to the number of entries per bank between the entries
 * We need to allocate such tuple as temporary memory to use for the update operation.
 * For the update operation to be atomic, we copy the extra data to the temporary memory, then update
 * the extra data index, update the data in the original extra data and restore the extra
 * data pointer to it's original value
 */

static shr_error_e
dnx_algo_oamp_full_tuples_mep_id_alloc(
    int unit,
    uint32 mep_id)
{
    int ii;
    int nof_bfd_ipv6_extra_data_entries = DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES;
    int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    int nof_oamp_full_tuples_mep_id_banks;
    int bank_index, sub_index;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    algo_oamp_db.nof_oamp_full_tuples_mep_id_banks.get(unit, &nof_oamp_full_tuples_mep_id_banks);
    bank_index =
        OAM_ID_TO_MEP_DB_ENTRY(mep_id -
                               dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) / nof_mep_db_entries_per_bank;
    sub_index =
        OAM_ID_TO_MEP_DB_ENTRY(mep_id -
                               dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) % nof_mep_db_entries_per_bank;

    /*
     * when using specific mep_id we have to mark (as allocated) all the tuples which this mep_id is part of for
     * example, if allocate the entry in index 16k and the size of each bank in 2k and the size of tuple is 2, then, we
     * mark the pairs (16k, 18k) and (14k, 16k) as allocated if the tuple's size is 3, then we mark the triples (16k,
     * 18k, 20k), (14k, 16k, 18k) and (12k, 14k, 16k) as allocated
     */
    for (ii = 0; ii < nof_bfd_ipv6_extra_data_entries; ++ii)
    {
        if (bank_index >= 0 && bank_index < nof_oamp_full_tuples_mep_id_banks)
        {
            SHR_IF_ERR_EXIT(SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID.is_allocated
                            (unit, bank_index, sub_index, &is_allocated));
            if (!is_allocated)
            {
                SHR_IF_ERR_EXIT(SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID.allocate_single
                                (unit, bank_index, alloc_flags, NULL, &sub_index));
            }
        }
        bank_index -= 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/* look at the explanation above the function dnx_algo_oamp_full_tuples_mep_id_alloc */

static shr_error_e
dnx_algo_oamp_full_tuples_mep_id_free(
    int unit,
    uint32 mep_id)
{
    int ii;
    int nof_bfd_ipv6_extra_data_entries = DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES;
    int nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    int nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    uint8 is_allocated_bitmap = 0;
    uint8 is_allocated_bitmap_mask = 0;
    int bank_index =
        OAM_ID_TO_MEP_DB_ENTRY(mep_id -
                               dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) / nof_mep_db_entries_per_bank;
    int sub_index =
        OAM_ID_TO_MEP_DB_ENTRY(mep_id -
                               dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) % nof_mep_db_entries_per_bank;

    int nof_short_entries_banks =
        OAM_ID_TO_MEP_DB_ENTRY(dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit)) / nof_mep_db_entries_per_bank;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < nof_bfd_ipv6_extra_data_entries; ii++)
    {
        is_allocated_bitmap_mask = (is_allocated_bitmap_mask << 1) + 1;
    }

    /*
     * when freeing specific mep_id we have to check all the tuples which this mep_id is part of and if this free
     * caused the tuple to be empty, then we have to mark this tuple as free for example, if the freed entry is in
     * index 16k and the size of each bank in 2k and the size of tuple is 2, then, we have to check the pairs (16k,
     * 18k) and (14k, 16k) as mark them as free if both the mep_ids are free if the tuple's size is 3, we have to check
     * the triples (16k, 18k, 20k), (14k, 16k, 18k) and (12k, 14k, 16k)
     */

    bank_index = bank_index - (nof_bfd_ipv6_extra_data_entries - 1);
    /*
     * we have to check the (2 * nof_bfd_ipv6_extra_data_entries - 1) entries (nof_bfd_ipv6_extra_data_entries tuples
     * which have the potential to be marked as free)
     */
    for (ii = -nof_bfd_ipv6_extra_data_entries + 1; ii < nof_bfd_ipv6_extra_data_entries; ++ii)
    {
        if (bank_index >= 0 && (bank_index + nof_short_entries_banks) < nof_mep_db_entry_banks)
        {
            uint8 is_alloc = 0;
            uint32 mep_id_member =
                MEP_DB_ENTRY_TO_OAM_ID(OAM_ID_TO_MEP_DB_ENTRY(dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit))
                                       + bank_index * nof_mep_db_entries_per_bank + sub_index);
            int bank_index_to_free = bank_index - (nof_bfd_ipv6_extra_data_entries - 1);
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, mep_id_member, &is_alloc));
            /*
             * we decide whether to free the tuple or not by the last nof_bfd_ipv6_extra_data_entries entries
             */
            is_allocated_bitmap = (is_allocated_bitmap << 1) + is_alloc;
            if (ii >= 0 && (is_allocated_bitmap & is_allocated_bitmap_mask) == 0 && bank_index_to_free >= 0)
            {
                uint8 tuple_is_allocated = 0;
                SHR_IF_ERR_EXIT(SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID.is_allocated(unit,
                                                                                   bank_index_to_free, sub_index,
                                                                                   &tuple_is_allocated));
                if (tuple_is_allocated)
                {
                    SHR_IF_ERR_EXIT(SW_STATE_ALGO_OAMP_FULL_TUPLES_MEP_ID.free_single
                                    (unit, bank_index_to_free, sub_index));
                }
            }
        }
        bank_index += 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_alloc(
    int unit,
    uint32 flags,
    dnx_oamp_mep_db_memory_type_t * mep_id_memory_type,
    uint32 *mep_id)
{
    int rv;
    int alloc_flags = 0;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case BCM_OAM_PROFILE_WITH_ID flag is specified, allocate profile with profile_id
     */
    if (flags & DNX_ALGO_RES_ALLOCATE_WITH_ID)
    {
        SHR_IF_ERR_EXIT(dnx_oam_index_mep_db_verify(unit, *mep_id));
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        dnx_mep_id_to_pool_type_and_index(unit, *mep_id, mep_id_memory_type, &index);

        SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type->is_2byte_maid,
                                                 mep_id_memory_type->is_full, mep_id_memory_type->is_extra_pool,
                                                 allocate_single(unit, alloc_flags, NULL, &index)));
    }
    else
    {
        if (mep_id_memory_type->is_2byte_maid)
        {
            /** Check if the Short pool is full. */
            alloc_flags = DNX_ALGO_RES_ALLOCATE_SIMULATION;
            rv = DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                                          allocate_single(unit, alloc_flags, NULL, &index));
            if (rv == _SHR_E_NONE)
            {
                alloc_flags = 0;
                /** The Short pool is not full, so get Short pool free index*/
                SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                                (mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                                 allocate_single(unit, alloc_flags, NULL, &index)));
            }
            else if (rv == _SHR_E_RESOURCE)
            {
                /** Get Long pool, as Short pool doesn't have free index */
                mep_id_memory_type->is_2byte_maid = 0;
                alloc_flags = 0;
                SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                                (0, mep_id_memory_type->is_full, 0, allocate_single(unit, alloc_flags, NULL, &index)));
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE
                            (mep_id_memory_type->is_2byte_maid, mep_id_memory_type->is_full, 0,
                             allocate_single(unit, alloc_flags, NULL, &index)));
        }
        /** Get mep_id according to index */
        dnx_pool_type_and_index_to_mep_id(unit, mep_id_memory_type, index, mep_id);
    }

    /*
     * see documentation above dnx_algo_oamp_full_tuples_mep_id_alloc
     */
    if (*mep_id >= dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_full_tuples_mep_id_alloc(unit, *mep_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_is_allocated(
    int unit,
    uint32 mep_id,
    uint8 *is_alloc)
{
    int index;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_index_mep_db_verify(unit, mep_id));

    dnx_mep_id_to_pool_type_and_index(unit, mep_id, &mep_id_memory_type, &index);

    SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type.is_2byte_maid,
                                             mep_id_memory_type.is_full, mep_id_memory_type.is_extra_pool,
                                             is_allocated(unit, index, is_alloc)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_mep_id_dealloc(
    int unit,
    uint32 mep_id)
{
    int index;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_index_mep_db_verify(unit, mep_id));

    /** Get pool type and MEP ID index inside pool from MEP ID ID */
    dnx_mep_id_to_pool_type_and_index(unit, mep_id, &mep_id_memory_type, &index);

    SHR_IF_ERR_EXIT(DNX_OAMP_MEP_ID_RESOURCE(mep_id_memory_type.is_2byte_maid,
                                             mep_id_memory_type.is_full, mep_id_memory_type.is_extra_pool,
                                             free_single(unit, index)));

    /*
     * see documentation above dnx_algo_oamp_full_tuples_mep_id_free
     */
    if (mep_id >= dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_full_tuples_mep_id_free(unit, mep_id));
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_algo_oamp_mep_id_init(
    int unit)
{

    dnx_algo_res_create_data_t data;
    int mep_db_treshold;
    int nof_umc;
    int oamp_max_nof_mep_id;
    int oamp_max_nof_ep_id;
    int nof_mep_db_entries_per_bank;
    int nof_mep_db_entry_banks;
    int nof_bfd_ipv6_extra_data_entries;
    int nof_mep_db_short_entries;
    int nof_short_lines;
    int nof_oamp_full_tuples_mep_id_banks;
    uint32 highest_short_mep_db_index;
    int nof_short_banks;
    int index;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    mep_db_treshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (mep_db_treshold % 4 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Full entry threshold need to be multiples of 4\r\n");
    }
    nof_umc = dnx_data_oam.oamp.nof_umc_get(unit);
    oamp_max_nof_mep_id = dnx_data_oam.oamp.max_nof_mep_id_get(unit);
    oamp_max_nof_ep_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    nof_mep_db_entries_per_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit);
    nof_mep_db_entry_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    nof_bfd_ipv6_extra_data_entries = DNX_BFD_IPV6_NOF_EXTRA_DATA_ENTRIES;
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_treshold / nof_mep_db_short_entries;
    nof_short_banks = mep_db_treshold / (nof_mep_db_entries_per_bank * nof_mep_db_short_entries);
    dnx_oam_mep_db_highest_index_get(unit, &highest_short_mep_db_index);

    data.flags = 0;
    /*
     * First element id
     */
    data.first_element = 0;

    /*
     * init the algo_oapm sw state module.
     */
    SHR_IF_ERR_EXIT(algo_oamp_db.init(unit));
    /*
     * Configure the size of all EP pools.
     */
    if (nof_short_lines < nof_umc)
    {
        data.nof_elements = (highest_short_mep_db_index / nof_mep_db_short_entries) * nof_short_banks;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = data.nof_elements * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = nof_umc - nof_short_lines;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_full_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = oamp_max_nof_ep_id / nof_mep_db_short_entries - nof_umc;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_full_mep_id.create(unit, &data, NULL));
        }
    }
    else
    {
        data.nof_elements =
            (highest_short_mep_db_index / nof_mep_db_short_entries) * (nof_umc / nof_mep_db_entries_per_bank);
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = (data.nof_elements * DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE) +
            (highest_short_mep_db_index * (nof_short_banks - (nof_umc / nof_mep_db_entries_per_bank)));
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_short_mep_id.create(unit, &data, NULL));
        }

        data.nof_elements = (oamp_max_nof_ep_id / nof_mep_db_short_entries) - nof_short_lines;
        if (data.nof_elements)
        {
            sal_strncpy(data.name, DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_2b_maid_full_mep_id.create(unit, &data, NULL));
        }

    }

    data.nof_elements = (oamp_max_nof_mep_id - oamp_max_nof_ep_id) / nof_mep_db_short_entries;
    sal_strncpy(data.name, DNX_ALGO_OAMP_EXTRA_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_extra_mep_id.create(unit, &data, NULL));

    /*
     * the tuples which start in the last ((nof_bfd_ipv6_extra_data_entries - 1) * nof_mep_db_entries_per_bank) entries
     * overflow the size of the mep_db
     */

    nof_oamp_full_tuples_mep_id_banks =
        nof_mep_db_entry_banks - nof_bfd_ipv6_extra_data_entries + 1 - (nof_short_lines / nof_mep_db_entries_per_bank);

    data.nof_elements = nof_mep_db_entries_per_bank;
    sal_strncpy(data.name, DNX_ALGO_OAMP_FULL_TUPLES_MEP_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_full_tuples_mep_id.alloc(unit, nof_oamp_full_tuples_mep_id_banks));

    for (index = 0; index < nof_oamp_full_tuples_mep_id_banks; ++index)
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_full_tuples_mep_id.create(unit, index, &data, NULL));
    }

    algo_oamp_db.nof_oamp_full_tuples_mep_id_banks.set(unit, nof_oamp_full_tuples_mep_id_banks);
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oamp_pp_port_profile_print_cb(
    int unit,
    const void *data)
{
    uint16 system_port = *(uint16 *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "System port", system_port, NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
 * \brief - This function creates the template needed for mapping local port to system port
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_pp_port_2_sys_port_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oamp_port_map_profile_data;
    uint32 oamp_max_nof_endpoint_id;
    uint32 oamp_port_map_table_size;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    oamp_max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    oamp_port_map_table_size = dnx_data_oam.oamp.local_port_2_system_port_size_get(unit);

    /** Create template for OAMP ICC map table */
    sal_memset(&oamp_port_map_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oamp_port_map_profile_data.flags = 0;
    oamp_port_map_profile_data.first_profile = 0;
    oamp_port_map_profile_data.nof_profiles = oamp_port_map_table_size;
    oamp_port_map_profile_data.max_references = oamp_max_nof_endpoint_id;
    oamp_port_map_profile_data.default_profile = 0;
    oamp_port_map_profile_data.data_size = sizeof(uint16);
    oamp_port_map_profile_data.default_data = NULL;
    sal_strncpy(oamp_port_map_profile_data.name, DNX_ALGO_OAMP_PP_PORT_2_SYS_PORT_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.create(unit, &oamp_port_map_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_punt_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_oamp_punt_event_profile_t *punt_profile = (dnx_oam_oamp_punt_event_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "punt_enable", punt_profile->punt_enable,
                                   "Indicates whether punt is enabled", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "punt_rate", punt_profile->punt_rate,
                                   "Rate of sampling packets (with events) to CPU", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "rx_state_update_en",
                                   punt_profile->rx_state_update_en,
                                   "Refer dbal_enum_value_field_profile_rx_state_update_en_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "profile_scan_state_update_en",
                                   punt_profile->profile_scan_state_update_en,
                                   "Refer dbal_enum_value_field_profile_scan_state_update_en_e", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_loc_en",
                                   punt_profile->mep_rdi_update_loc_en,
                                   "Enable/disable MEP DB RDI indication in case LOC detected", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_loc_clear_en",
                                   punt_profile->mep_rdi_update_loc_clear_en,
                                   "Enable/disable MEP DB RDI indication in case LOC Clear detected", NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_rdi_update_rx_en",
                                   punt_profile->mep_rdi_update_rx_en,
                                   "Enable/disable MEP DB RDI indication by copying RDI from rx pkt", NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief - This function creates the template needed for the
 *        Punt profile management
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *      Punt profiles: mirror the entries in
 *      the OAMP_PUNT_EVENT_HENDLING available for Punt information\n
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_punt_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oam_punt_handling_profile_data;
    uint32 nof_references = 0;
    uint32 nof_scan_rmep_entries = 0;
    uint32 rmep_db_treshold = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        nof_references = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        nof_references = nof_scan_rmep_entries;
    }

    /** Create template for BFD OAMA Profiles */
    sal_memset(&oam_punt_handling_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oam_punt_handling_profile_data.flags = 0;
    oam_punt_handling_profile_data.first_profile = DNX_ALGO_PUNT_TABLE_START_ID;
    oam_punt_handling_profile_data.nof_profiles = DNX_ALGO_PUNT_TABLE_COUNT;
    oam_punt_handling_profile_data.max_references = nof_references;
    oam_punt_handling_profile_data.default_profile = 0;
    oam_punt_handling_profile_data.data_size = sizeof(dnx_oam_oamp_punt_event_profile_t);
    oam_punt_handling_profile_data.default_data = NULL;
    sal_strncpy(oam_punt_handling_profile_data.name, DNX_ALGO_OAMP_PUNT_EVENT_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.create(unit, &oam_punt_handling_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oamp_punt_good_profile_print_cb(
    int unit,
    const void *data)
{
    uint16 punt_good_packet_rate = *(uint16 *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "Punt good packet rate", punt_good_packet_rate,
                                   NULL, "0x%08X");
    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/**
 * \brief - This function creates the template needed for
            punt good profile management
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_punt_good_profile_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oamp_punt_good_profile_data;
    uint32 nof_references = 0;
    uint32 nof_scan_rmep_entries = 0;
    uint32 rmep_db_treshold = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data */
    nof_scan_rmep_entries = dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit) + 1;
    rmep_db_treshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_db_treshold < nof_scan_rmep_entries)
    {
        nof_references = rmep_db_treshold + (nof_scan_rmep_entries - rmep_db_treshold) / 2;
    }
    else
    {
        nof_references = nof_scan_rmep_entries;
    }

    /** Create template for OAMP punt good profile table */
    sal_memset(&oamp_punt_good_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oamp_punt_good_profile_data.flags = 0;
    oamp_punt_good_profile_data.first_profile = 1;
    oamp_punt_good_profile_data.nof_profiles = 7;
    oamp_punt_good_profile_data.max_references = nof_references;
    oamp_punt_good_profile_data.default_profile = 0;
    oamp_punt_good_profile_data.data_size = sizeof(uint16);
    oamp_punt_good_profile_data.default_data = NULL;
    sal_strncpy(oamp_punt_good_profile_data.name, DNX_ALGO_OAMP_PUNT_GOOD_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_punt_good_profile.create(unit, &oamp_punt_good_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_itmh_priority_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_itmh_priority_t *itmh_priority = (dnx_oam_itmh_priority_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "tc", itmh_priority->tc, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dp", itmh_priority->dp, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - This function creates the template for the ITMH
 *        TC/DP profiles.  This resource is used by OAM and BFD
 *        accelerated endpoints in the ITMH header of
 *        transmitted packets.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_itmh_priority_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t itmh_priority_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&itmh_priority_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    itmh_priority_profile_data.flags = 0;
    itmh_priority_profile_data.first_profile = 0;
    itmh_priority_profile_data.nof_profiles = NOF_ITMH_PRIORITY_PROFILES;
    itmh_priority_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    itmh_priority_profile_data.default_profile = 0;
    itmh_priority_profile_data.data_size = sizeof(dnx_oam_itmh_priority_t);
    itmh_priority_profile_data.default_data = NULL;
    sal_strncpy(itmh_priority_profile_data.name, DNX_OAM_TEMPLATE_ITMH_PRIORITY,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.create(unit, &itmh_priority_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_ttl_exp_profile_t *profile = (dnx_oam_ttl_exp_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "ttl", profile->ttl, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "exp", profile->exp, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - This function creates the template for the OAM/BFD
 *        MPLS/PWE TTL/EXP profiles.  This resource is used by OAM
 *        and BFD MPLS/PWE accelerated endpoints to construct
 *        transmitted packets.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_mpls_pwe_ttl_exp_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t mpls_pwe_exp_ttl_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mpls_pwe_exp_ttl_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    mpls_pwe_exp_ttl_profile_data.flags = 0;
    mpls_pwe_exp_ttl_profile_data.first_profile = 0;
    mpls_pwe_exp_ttl_profile_data.nof_profiles = NOF_MPLS_PWE_TTL_EXP_PROFILES;
    mpls_pwe_exp_ttl_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mpls_pwe_exp_ttl_profile_data.default_profile = 0;
    mpls_pwe_exp_ttl_profile_data.data_size = sizeof(dnx_oam_ttl_exp_profile_t);
    mpls_pwe_exp_ttl_profile_data.default_data = NULL;
    sal_strncpy(mpls_pwe_exp_ttl_profile_data.name, DNX_OAM_TEMPLATE_MPLS_PWE_EXP_TTL_PROFILE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.create(unit, &mpls_pwe_exp_ttl_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_algo_oam_oamp_mep_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_mep_profile_t *profile = (dnx_oam_mep_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "piggy_back_lm", profile->piggy_back_lm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "slm_lm", profile->slm_lm, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "slm_measurement_period",
                                   profile->slm_measurement_period_index, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "maid_check_dis", profile->maid_check_dis, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "report_mode_lm", profile->report_mode_lm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "report_mode_dm", profile->report_mode_dm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "rdi_gen_method", profile->rdi_gen_method, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "dmm_rate", profile->dmm_rate, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "lmm_rate", profile->lmm_rate, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_0_rate", profile->opcode_0_rate, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_1_rate", profile->opcode_1_rate, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dmm_offset", profile->dmm_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dmr_offset", profile->dmr_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "lmm_offset", profile->lmm_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "lmr_offset", profile->lmr_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ccm_count", profile->ccm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "dmm_count", profile->dmm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "lmm_count", profile->lmm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_0_count", profile->opcode_0_count, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_1_count", profile->opcode_1_count, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dm_measurement_type",
                                   profile->dm_measurement_type, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_id_shift", profile->mep_id_shift, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_mask", profile->opcode_mask, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_tx_statistics_enable",
                                   profile->opcode_tx_statistics_enable, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_rx_statistics_enable",
                                   profile->opcode_rx_statistics_enable, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * \brief - This function creates the resource needed to allocate
 *          entries in the OAMP_MEP_ID table
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *

 * \see
 *   * None
 */
static shr_error_e
dnx_algo_acc_oam_res_init(
    int unit)
{
    dnx_algo_res_create_data_t acc_mep_res_data;
    SHR_FUNC_INIT_VARS(unit);

    acc_mep_res_data.first_element = 0;
    acc_mep_res_data.flags = 0;
    acc_mep_res_data.nof_elements = dnx_data_oam.oamp.max_nof_mep_id_get(unit);
    acc_mep_res_data.advanced_algorithm = 0;
    sal_strncpy(acc_mep_res_data.name, DNX_ALGO_OAMP_MEP_DB_ENTRY, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.create(unit, &acc_mep_res_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for the
 *          OAMP MEP profile management
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oam_oamp_mep_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t mep_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mep_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    mep_profile_data.flags = 0;
    mep_profile_data.first_profile = 0;
    mep_profile_data.nof_profiles = NOF_MEP_PROFILES;
    mep_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mep_profile_data.default_profile = 0;
    mep_profile_data.data_size = sizeof(dnx_oam_mep_profile_t);
    mep_profile_data.default_data = NULL;
    sal_strncpy(mep_profile_data.name, DNX_OAM_TEMPLATE_MEP_PROFILE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.create(unit, &mep_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize templates for all oamp profile types.
 *
 * \param [in] unit - Number of hardware unit used.
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
dnx_algo_oamp_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_pp_port_2_sys_port_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_punt_profile_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_punt_good_profile_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_itmh_priority_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_mpls_pwe_ttl_exp_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_acc_oam_res_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oam_oamp_mep_profile_id_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP ID alloc/free
 *          This function helps calculate the allocated/freed id
 *          to be used in the alloc mngr for above threshold ids.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - RMEP id
 * \param [out] alloc_id - alloc_mngr id
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(
    int unit,
    int rmep_idx,
    int *alloc_id)
{
    uint32 self_contained_threshold;
    uint32 oamp_rmep_scale_limitataion = 0;
    uint32 nof_entries_per_rmep = 0;
    int temp_alloc_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);

    temp_alloc_id = rmep_idx - self_contained_threshold;

    /** Above the threshold, RMEP ID must be an even number in case oamp_rmep_scale_limitataion is 0 */
    nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 4 : 2;
    if (temp_alloc_id % nof_entries_per_rmep != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ids above self contained threshold need to be multiples of %d\r\n",
                     nof_entries_per_rmep);
    }
    temp_alloc_id = temp_alloc_id / nof_entries_per_rmep;
    *alloc_id = temp_alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP ID alloc/free
 *          This function helps calculate the allocated/freed id
 *          to be used in the alloc mngr for below threshold ids.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - RMEP id
 * \param [out] alloc_id - alloc_mngr id
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_id_below_threshold_to_alloc_mngr_id(
    int unit,
    int rmep_idx,
    int *alloc_id)
{
    uint32 oamp_rmep_scale_limitataion = 0;
    uint32 nof_entries_per_rmep = 0;
    int temp_alloc_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);

    temp_alloc_id = rmep_idx;

    /** Below the threshold, RMEP ID must be an even number in case oamp_rmep_scale_limitataion is 0 */
    nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
    if (temp_alloc_id % nof_entries_per_rmep != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ids below self contained threshold need to be multiples of %d\r\n",
                     nof_entries_per_rmep);
    }
    temp_alloc_id = temp_alloc_id / nof_entries_per_rmep;
    *alloc_id = temp_alloc_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function deletes the new RMEP ID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_id - ID of the remote endpoint.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_free(
    int unit,
    bcm_oam_endpoint_t rmep_id)
{
    uint32 self_contained_threshold;
    int alloc_id;

    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_id < self_contained_threshold)
    {
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_below_threshold_to_alloc_mngr_id(unit, rmep_id, &alloc_id));
        /*
         * Remove from below threshold pool
         */

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.free_single(unit, alloc_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(unit, rmep_id, &alloc_id));
        /*
         * Remove from above threshold pool
         */

        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_above_threshold.free_single(unit, alloc_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function allocates the new RMEP ID
 *          and handles WITH_ID case as well.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] alloc_flags - flags variable to be used
 *             for resource allocation
 * \param [in,out] rmep_id - Resulting ID.  If ID is
 *        specified, this is an output argument.
 *        Otherwise, it's an input argument.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_id_alloc(
    int unit,
    int alloc_flags,
    bcm_oam_endpoint_t * rmep_id)
{
    uint32 self_contained_threshold;
    int alloc_id;
    uint32 oamp_rmep_scale_limitataion = 0;
    uint32 nof_entries_per_rmep = 0;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);

    /** allocate the remote endpoint ID */
    if (_SHR_IS_FLAG_SET(alloc_flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        /*
         * in case BCM_OAM_ENDPOINT_WITH_ID flag exists, allocate RMEP id, according to endpoint_info->id
         */
        if (*rmep_id < self_contained_threshold)
        {
            SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_below_threshold_to_alloc_mngr_id(unit, *rmep_id, &alloc_id));
            /*
             * Allocate from below threshold pool
             */
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_above_threshold_to_alloc_mngr_id(unit, *rmep_id, &alloc_id));
            /*
             * Allocate from above threshold pool
             */
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
        }
    }
    else
    {
        /*
         * First try to allocate from above threshold. If resource is not there, then allocate
         * from below threshold.
         */
        rv = algo_oam_db.oam_rmep_id_above_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id);
        if (rv == _SHR_E_RESOURCE)
        {
            SHR_IF_ERR_EXIT(algo_oam_db.
                            oam_rmep_id_below_threshold.allocate_single(unit, alloc_flags, NULL, &alloc_id));
            nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
            *rmep_id = alloc_id * nof_entries_per_rmep;
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocate above threshold failed with %d\r\n", rv);
        }
        else
        {
            /*
             * Allocated from above threshold. 
             *   if oamp_rmep_scale_limitataion is 0, the endpoint id is 2 times the allocated number + threshold
             *   if oamp_rmep_scale_limitataion is 1, the endpoint id is 4 times the allocated number + threshold
             */
            nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 4 : 2;
            *rmep_id = (alloc_id * nof_entries_per_rmep) + self_contained_threshold;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_entry_alloc(
    int unit,
    int *oamp_entry_index)
{
    int full_entry_threshold, short_entries_in_full, short_index, temp_index;
    SHR_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (*oamp_entry_index >= full_entry_threshold)
    {
        /** Allocate full entry; reject index that isn't aligned */
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((*oamp_entry_index % short_entries_in_full) == 0)
        {
            for (short_index = 0; short_index < short_entries_in_full; short_index++)
            {
                        /** Allocate enough space for a full entry */
                temp_index = *oamp_entry_index + short_index;
                SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.allocate_single
                                (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &temp_index));
            }
        }
        else
        {
                /** Index is not aligned */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Index 0x%08X is above the threshold 0x%08X but not aligned.  Must be a multiple of %d\n",
                         *oamp_entry_index, full_entry_threshold, short_entries_in_full);
        }
    }
    else
    {
        /** Allocate 1 (short) entry */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.allocate_single
                        (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, oamp_entry_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_entry_dealloc(
    int unit,
    int oamp_entry_index)
{
    int full_entry_threshold, short_entries_in_full, short_index;
    SHR_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (oamp_entry_index >= full_entry_threshold)
    {
        /** Free full entry; reject index that isn't aligned */
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        if ((oamp_entry_index % short_entries_in_full) == 0)
        {
            for (short_index = 0; short_index < short_entries_in_full; short_index++)
            {
                        /** Free the space occupied by a full entry */
                SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.free_single(unit, oamp_entry_index + short_index));
            }
        }
        else
        {
                /** Index is not aligned */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error: Index 0x%08X is above the threshold 0x%08X but not aligned.  Must be a multiple of %d\n",
                         oamp_entry_index, full_entry_threshold, short_entries_in_full);
        }
    }
    else
    {
        /** Free 1 (short) entry */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.free_single(unit, oamp_entry_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_oam_oamp_get_next_icc_entry(
    int unit,
    uint32 *icc_id_in_out,
    uint8 *entry_found)
{
    int mep_db_threshold, nof_mep_db_short_entries, index;
    SHR_FUNC_INIT_VARS(unit);
    *entry_found = FALSE;

    /**
     * Search is possible only if threshold is non-zero.
     * If threshold is 0, short mep banks are not created
     * and the get_next function will cause the system to
     * crash.
     */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (mep_db_threshold > 0)
    {
        /** Find this entry index in the pool */
        nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
        index = *icc_id_in_out / nof_mep_db_short_entries;

        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.get_next(unit, &index));
        if (index != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            *entry_found = TRUE;
            *icc_id_in_out = nof_mep_db_short_entries * index;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in algo_oamp.h
 */
shr_error_e
dnx_algo_oamp_mep_id_get_free_icc_id(
    int unit,
    uint32 *icc_id_out,
    uint8 *entry_not_found)
{
    int index, mep_db_threshold, nof_mep_db_short_entries, nof_short_lines;
    uint8 is_alloc;
    SHR_FUNC_INIT_VARS(unit);
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);
    nof_short_lines = mep_db_threshold / nof_mep_db_short_entries;

    *entry_not_found = TRUE;
    for (index = 0; index < nof_short_lines; index++)
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_icc_maid_short_mep_id.is_allocated(unit, index, &is_alloc));
        if (is_alloc == FALSE)
        {
            *entry_not_found = FALSE;
            *icc_id_out = nof_mep_db_short_entries * index;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
