/*! \file bcmltm_ha_state_upgrade.c
 *
 * Logical Table Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>

#include <bcmcfg/comp/bcmcfg_ltm_resources.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_types.h>

#include <bcmltm/bcmltm_ha_internal.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_db_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_HA

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Provide a sort comparison for LT IDs within state structures.
 *
 * This function compares the LTIDs of two state structures and returns
 * a sort indicator.
 *
 *
 * \param [in] key1 First LT state structure pointer.
 * \param [in] key2 Second LT state structure pointer.
 *
 * \retval 0 if LTIDs are equal.
 *        -1 if LTID of key1 is less than LTID of key2.
 *         1 if LTID of key1 is greater than LTID of key2.
 */
static int
ltid_state_comp(const void *key1, const void *key2)
{
    bcmltm_lt_state_t *lt_state11 = (bcmltm_lt_state_t *)key1;
    bcmltm_lt_state_t *lt_state12 = (bcmltm_lt_state_t *)key2;

    if (lt_state11->ltid == lt_state12->ltid) {
        return 0;
    } else if (lt_state11->ltid < lt_state12->ltid) {
        return -1;
    } else {
        return 1;
    }
}

/*!
 * \brief Convert HA encoded pointer to physical memory pointer.
 *
 * This function converts the HA encoded (subblock, offset) format
 * of a LTM HA pointer into the physical memory required for further
 * processing.  If the HA pointer is invalid, it returns NULL.
 *
 * This function requires more information than the standard LTM
 * HA pointer conversion because no HA init has occurred.
 *
 * \param [in] block_type The HA block type expected in ha_ptr.
 * \param [in] block_ptr LTM HA block base pointer.
 * \param [in] ha_ptr LTM HA encoded pointer.
 *
 * \retval Physical memory pointer if HA pointer is valid.
 *         NULL if HA pointer is invalid.
 */
static void *
state_up_ha_ptr_to_mem_ptr(bcmltm_ha_block_type_t block_type,
                           bcmltm_ha_block_header_t *block_ptr,
                           bcmltm_ha_ptr_t ha_ptr)
{
    uint8_t *ptr_math;

    if (BCMLTM_HA_PTR_TO_BLOCK_TYPE(ha_ptr) != block_type) {
        /* Inconsistent block */
        return NULL;
    }

    ptr_math = (uint8_t *)block_ptr + BCMLTM_HA_PTR_OFFSET(ha_ptr);
    return ptr_math;
}

/*!
 * \brief Update LTM HA state data FIDs.
 *
 * This function updates the LTM HA state field IDs during ISSU.
 * Operations:
 *  - Follow LT state to LT state data.
 *  - Update the LTID in the state data.
 *
 * If the provided new LTID is invalid, the state data is marked
 * with that invalid LTID to indicate it should be eliminated during
 * the state data cleanup phase.
 *
 * \param [in] unit The unit associated with the structure instances.
 * \param [in] old_ltid Prior LTID for this table.
 * \param [in] new_ltid New LTID for this table.
 * \param [in] lt_state_data_hap HA pointer to the start of state data lsit.
 * \param [in] state_data_blk_hdr The live HA memory block where the state
 *                       data structures are contained.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_FAIL The LTID conversion failed.
 */
static int
bcmltm_state_data_update(int unit,
                         uint32_t old_ltid,
                         uint32_t new_ltid,
                         bcmltm_ha_ptr_t lt_state_data_hap,
                         bcmltm_ha_block_header_t *state_data_blk_hdr)
{
    bcmltm_lt_state_data_t *state_iter;
    bcmltm_ha_ptr_t state_iter_hap;

    state_iter_hap = lt_state_data_hap;

    while (state_iter_hap != BCMLTM_HA_PTR_INVALID) {
        state_iter =
            state_up_ha_ptr_to_mem_ptr(BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                                       state_data_blk_hdr,
                                       state_iter_hap);
        if (state_iter == NULL) {
            break;
        }
        if (state_iter->ltid != old_ltid) {
            return SHR_E_INTERNAL;
        }
        state_iter->ltid = new_ltid;
        state_iter_hap = state_iter->next_state_data_hap;
    }

    return SHR_E_NONE;
}

/*!
 *\brief Compact LTM HA state data block to eliminate abandoned LTs.
 *
 * This function consolidates the LTM HA state data.
 * Operations:
 *  - Examine each block of state data.
 *  - Determine if valid LTID recorded.
 *  - If compression required:
 *    - Move state data.
 *    - Find LT state.
 *    - Update LT state record if needed.
 *    - Continue for each block of that LT's state.
 *  - Skip invalid LTID state data.
 *
 * \param [in] state_ha_ptr The live HA memory where the state structures
 *                       start. This memory used to read the original
 *                       values and update them.
 * \param [in] state_blk_hdr The live HA memory block where the state
 *                       structures are contained.
 * \param [in] state_data_ptr The live HA memory where the state data
 *                       structures start. This memory used to read
 *                       the original values and update them.
 * \param [in] state_data_blk_hdr The live HA memory block where the state
 *                       data structures are contained.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_FAIL The LTID conversion failed.
 */
static int
bcmltm_state_data_compress(uint8_t *state_ha_ptr,
                           bcmltm_ha_block_header_t *state_blk_hdr,
                           uint8_t *state_data_ha_ptr,
                           bcmltm_ha_block_header_t *state_data_blk_hdr)
{
    bcmltm_lt_state_t *cur_lt_state, *base_lt_state, *max_lt_state;
    bcmltm_lt_state_data_t *cur_state_data;
    uint8_t *state_data_to, *state_data_from,
        *state_data_end, *state_data_base;
    uint32_t state_data_size;
    uint32_t last_ltid = BCMLTM_LTID_INVALID;
    bool state_lt_found;

    /* Set up bounds and pointer for LT state (fixed size structure) */
    base_lt_state = (bcmltm_lt_state_t *) state_ha_ptr;
    max_lt_state =
        (bcmltm_lt_state_t *) (base_lt_state +
                               state_blk_hdr->allocated_size);
    cur_lt_state = base_lt_state;

    /* Initialize LT state data pointers */
    state_data_base = (uint8_t *) state_data_blk_hdr;
    state_data_to = state_data_from = state_data_ha_ptr;
    state_data_end = state_data_base +
        state_data_blk_hdr->allocated_size;

    while ((state_data_from + sizeof(bcmltm_lt_state_data_t)) <=
           state_data_end) {
        cur_state_data = (bcmltm_lt_state_data_t *) state_data_from;
        state_data_size = sizeof(bcmltm_lt_state_data_t);
        state_data_size += cur_state_data->state_data_size;

        if ((state_data_from + state_data_size) > state_data_end) {
            /* Inconsistent previous HA records */
            return SHR_E_FAIL;
        }

        if (cur_state_data->ltid == BCMLTM_LTID_INVALID) {
            state_data_from += state_data_size;
            last_ltid = BCMLTM_LTID_INVALID;
            continue;
        }

        if (state_data_to != state_data_from) {
            /* Some LT has been skipped */
            if (cur_state_data->ltid != last_ltid) {
                /* Start of state data for LTID.  Update state hap. */
                last_ltid = cur_state_data->ltid;
                state_lt_found = FALSE;
                while ((cur_lt_state >= base_lt_state) &&
                       (cur_lt_state < max_lt_state)) {
                    /* LTIDs in state are sorted from previous work. */
                    if (cur_lt_state->ltid == cur_state_data->ltid) {
                        state_lt_found = TRUE;
                        break;
                    } else if (cur_lt_state->ltid < cur_state_data->ltid) {
                        cur_lt_state++;
                    } else {
                        cur_lt_state--;
                    }
                    /* LTIDs must match in the list from prior update. */
                }

                if (!state_lt_found) {
                    /* Inconsistent HA records */
                    return SHR_E_FAIL;
                }
                cur_lt_state->table_state_data_hap =
                    BCMLTM_HA_PTR(BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                                  state_data_to - state_data_base);
            }

            /* Move this state data block to eliminate gap */
            sal_memmove(state_data_to, state_data_from, state_data_size);

            /* Update state data block HAPs */
            cur_state_data = (bcmltm_lt_state_data_t *) state_data_to;
            cur_state_data->state_data_hap =
                    BCMLTM_HA_PTR(BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                                  (state_data_to - state_data_base) +
                                  sizeof(bcmltm_lt_state_data_t));

            if (!BCMLTM_HA_PTR_IS_INVALID(cur_state_data->next_state_data_hap)) {
                cur_state_data->next_state_data_hap =
                    BCMLTM_HA_PTR(BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                                  (state_data_to - state_data_base) +
                                  state_data_size);
            } /* Else this is the end of this LTs state data */
        }


        /* Advance pointers */
        state_data_to += state_data_size;
        state_data_from += state_data_size;
    }

    /* Update allocated size for present usage */
    state_data_blk_hdr->allocated_size = state_data_to - state_data_base;

    return SHR_E_NONE;
}

/*!
 *\brief Update LTM HA state records.
 *
 * This function updates the LTM HA state during ISSU.
 * Operations:
 *  - Detect absent LTID and delete associate state data.
 *  - Convert Index LT key bounds field IDs.
 *  - Convert LTIDs.
 *  - Delete absent LTID state entries.
 *  - Add new LTIDs, mark for metadata to create new state data.
 *
 * \param [in] unit The unit associated with the structure instances.
 * \param [in] new_ltid_count New number of LTIDs.
 * \param [in] old_ltid_count Prior number of LTIDs.
 * \param [in] state_ptr The live HA memory where the state structures
 *                       start. This memory used to read the original
 *                       values and update them.
 * \param [in] state_blk_hdr The live HA memory block where the state
 *                       structures are contained.
 * \param [in] state_data_ptr The live HA memory where the state data
 *                       structures start. This memory used to read
 *                       the original values and update them.
 * \param [in] state_data_blk_hdr The live HA memory block where the state
 *                       data structures are contained.
 *
 * \return SHR_E_NONE Success
 * \return SHR_E_FAIL The LTID conversion failed.
 */
static int
bcmltm_update_state(int unit,
                    uint32_t sid_max_count,
                    uint32_t old_ltid_count,
                    uint8_t *state_ha_ptr,
                    bcmltm_ha_block_header_t *state_blk_hdr,
                    uint8_t *state_data_ha_ptr,
                    bcmltm_ha_block_header_t *state_data_blk_hdr)
{
    bcmltm_lt_state_t *lt_state;
    uint32_t inc_steps = BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_t));
    uint8_t *it = state_ha_ptr;
    uint32_t num_of_ent, recovered_ltids, abandoned_ltids;
    bcmlrd_sid_t *sid_list = NULL;
    uint32_t sid, old_ltid, new_ltid;
    size_t num_sid;
    size_t idx;
    int rv;
    size_t size;


    

    /*
     * Needs three passes:
     * 1. Correct all the LTIDs
     * 2. Sort all the entries based on their LTID
     * 3. Insert all the LTIDs that were not exist before
     */
    num_of_ent = recovered_ltids = abandoned_ltids = 0;
    while (((it - state_ha_ptr) < state_blk_hdr->block_size) &&
           (num_of_ent < old_ltid_count)) {
        lt_state = (bcmltm_lt_state_t *)it;
        if (!lt_state->initialized) {
            break;
        }
        it += inc_steps;
        num_of_ent++;  /* Count of all prior entries analyzed */
        old_ltid = lt_state->ltid;
         /* Max value for sorting discarded LT state to end */
        new_ltid = BCMLTM_LTID_INVALID;
        rv = bcmissu_ltid_to_current(unit, old_ltid, &new_ltid);
        if ((rv == SHR_E_UNAVAIL) ||
            (new_ltid == BCMLTM_LTID_INVALID)) {
            /* Will mark all of this LT's state data for deletion */
            abandoned_ltids++;
        } else if (SHR_FAILURE(rv)) {
            return SHR_E_BADID;
        } else {
            recovered_ltids++; /* Count of prior entries still valid */
        }
        lt_state->ltid = new_ltid;
        /*
         * If Index LT, update the key field IDs.
         * Update linked state data with new ltid.
         * If new LTID is invalid, state data is removed during defrag.
         */
        rv = bcmltm_state_data_update(unit, old_ltid, new_ltid,
                                      lt_state->table_state_data_hap,
                                      state_data_blk_hdr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    /* Step 2. */
    sal_qsort(state_ha_ptr, num_of_ent, inc_steps, ltid_state_comp);

    /*
     * After this sort, all of the prior obsolete entries should be at
     * the end of the list (since the LTID was set to uint32_t max).
     * During the resizing below, they may be ignored, so we use
     * recovered_ltids as the number of entries to work with.
     *
     * Math note-
     *
     * The calling function resizes this state HA block to have
     * enough space for all of the valid LTIDs in the new SDKLT version,
     * if that number is greater than the prior valid LTID count.
     * So the HA block size is max(prior LTID count, new LTID count).
     *
     * By ignoring now invalid LTIDs, there is room to add
     * new LTIDs without overflowing the HA block size.
     */
    num_of_ent = recovered_ltids;

    /* Step 3. */
    size = sizeof(*sid_list) * sid_max_count;
    sid_list = sal_alloc(size, "bcmltmSidArr");
    if (!sid_list) {
        return SHR_E_MEMORY;
    }
    sal_memset(sid_list, 0, size);

    /* Get SID list */
    rv = bcmlrd_table_list_get(unit, sid_max_count, sid_list, &num_sid);
    if (SHR_FAILURE(rv)) {
        SHR_FREE(sid_list);
        return rv;
    }
    for (idx = 0, it = state_ha_ptr; idx < num_sid; idx++) {
        sid = sid_list[idx];
        lt_state = (bcmltm_lt_state_t *)it;
        if (sid != lt_state->ltid) {
            if (sid < lt_state->ltid) {    /* New LT */
                sal_memmove(it+inc_steps, it, (num_of_ent-idx) * inc_steps);
                num_of_ent++;
                sal_memset(lt_state, 0, inc_steps);
                /* Mark this LT for state construction. */
                lt_state->initialized = FALSE;
                lt_state->ltid = sid;
                /* Other elements should be filled out during metadata init. */
            } else {
                
                sal_memmove(it, it + inc_steps,
                            (num_of_ent - idx - 1) * inc_steps);
                num_of_ent--;
                continue;
            }
        }
        it += inc_steps;
    }
    SHR_FREE(sid_list);

    if (num_of_ent != num_sid) {
        return SHR_E_INTERNAL;
    }

    /* Update allocated size to match now used portion of state block. */
    state_blk_hdr->allocated_size =
        sizeof(bcmltm_ha_block_header_t) + (num_of_ent * inc_steps);

    if (abandoned_ltids != 0) {
        /*
         * Some former LTs are no longer in use.
         * Remove the state data for these LTs from the HA record.
         */
        rv = bcmltm_state_data_compress(state_ha_ptr,
                                        state_blk_hdr,
                                        state_data_ha_ptr,
                                        state_data_blk_hdr);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_unit_state_upgrade(int unit, bcmltm_ha_state_sizes_t *ha_state_sizes)
{
    uint8_t *state_ha_ptr, *state_data_ha_ptr;
    uint32_t block_size;
    uint32_t new_ltid_count;
    bcmltm_ha_block_header_t *state_blk_hdr, *state_data_blk_hdr;
    size_t total_state_size = ha_state_sizes->total_state_size;
    size_t total_state_data_size = ha_state_sizes->total_state_data_size;
    uint32_t inc_steps = BCMLTM_WORDSIZEBYTES(sizeof(bcmltm_lt_state_t));
    uint32_t old_ltid_count = 0;

    SHR_FUNC_ENTER(unit);

    new_ltid_count = bcmltm_db_table_count_get();

    total_state_size += sizeof(bcmltm_ha_block_header_t);
    block_size = total_state_size;

    /* Recover and resize state structures block */
    state_ha_ptr = shr_ha_mem_alloc(unit,
                                    BCMMGMT_LTM_COMP_ID,
                                    BCMLTM_HA_BLOCK_TYPE_STATE,
                                    "ltmState",
                                    &block_size);
    SHR_NULL_CHECK(state_ha_ptr, SHR_E_MEMORY);
    if (block_size < total_state_size) {
        old_ltid_count =
             (block_size - sizeof(bcmltm_ha_block_header_t)) / inc_steps;
        block_size = total_state_size;
        state_ha_ptr = shr_ha_mem_realloc(unit,
                                          state_ha_ptr,
                                          block_size);
        SHR_NULL_CHECK(state_ha_ptr, SHR_E_MEMORY);
        state_blk_hdr = (bcmltm_ha_block_header_t *)state_ha_ptr;
        state_blk_hdr->block_size = block_size;
    } else {
        old_ltid_count =
             (block_size - sizeof(bcmltm_ha_block_header_t)) / inc_steps;
    }
    state_blk_hdr = (bcmltm_ha_block_header_t *)state_ha_ptr;
    state_ha_ptr += sizeof(bcmltm_ha_block_header_t);

    /* Next, update the size of the state data block */
    total_state_data_size += sizeof(bcmltm_ha_block_header_t);
    block_size = total_state_data_size;
    state_data_ha_ptr = shr_ha_mem_alloc(unit,
                                         BCMMGMT_LTM_COMP_ID,
                                         BCMLTM_HA_BLOCK_TYPE_STATE_DATA,
                                         "ltmStateData",
                                         &block_size);
    SHR_NULL_CHECK(state_data_ha_ptr, SHR_E_MEMORY);
    if (block_size < total_state_data_size) {
        block_size = total_state_data_size;
        state_data_ha_ptr = shr_ha_mem_realloc(unit,
                                    state_data_ha_ptr,
                                    block_size);
        SHR_NULL_CHECK(state_data_ha_ptr, SHR_E_MEMORY);
        state_data_blk_hdr = (bcmltm_ha_block_header_t *)state_data_ha_ptr;
        state_data_blk_hdr->block_size = block_size;
    } else {
        state_data_blk_hdr = (bcmltm_ha_block_header_t *)state_data_ha_ptr;
    }
    state_data_ha_ptr += sizeof(bcmltm_ha_block_header_t);

    /* Now update the state and state data themselves as needed */
    SHR_IF_ERR_EXIT
        (bcmltm_update_state(unit, new_ltid_count, old_ltid_count,
                             state_ha_ptr, state_blk_hdr,
                             state_data_ha_ptr, state_data_blk_hdr));

exit:
    SHR_FUNC_EXIT();
}
