/*! \file issu_ctrl.c
 *
 * HA ISSU control module manages the components data structures for the
 * purpose of ISSU.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_debug.h>
#include <bcmdrd_config.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmissu/issu_internal.h>
#include <bcmissu/generated/bcmissu_libsum.h>

#define BSL_LOG_MODULE BSL_LS_BCMISSU_CTRL

/* The number of reported structures to save per ISSU HA block */
#define STRUCTS_PER_BLK  100

/* Every ISSU reported block carries this signature */
#define ISSU_BLK_SIGNATURE  0xD672BAEE

/* Link list of all ISSU HA blocks */
typedef struct ctrl_blk_chain_s {
    bcmissu_comp_s_ctrl_blk_t *blk;
    struct ctrl_blk_chain_s *next;
} ctrl_blk_chain_t;

/* Control structure per unit that is used to manage the ISSU HA blocks */
typedef struct {
    shr_ha_sub_id sub_comp_id; /* The next sub component to use for HA alloc */
    /*
     * The component ID to use for for next HA alloc. Note that the ISSU also
     * supports other external units which will have different component IDs.
     */
    shr_ha_mod_id comp_id;
    /*
     * Indicate if this unit was initialized cold or warm.
     */
    bool warm;
    /* Protect multiple threads reporting this structure */
    sal_mutex_t ctrl_sync_obj;
    ctrl_blk_chain_t *blk_chain; /* The HA blocks link list */
} issu_ctrl_unit_t;

/*
 * A linked list of control blocks. Typically this will be used when gathering
 * all the control blocks that are pointing to structures within a particular
 * HA block.
 */
typedef struct blk_s_info_s {
    bcmissu_comp_s_info_t *s_info;
    struct blk_s_info_s *next;
} blk_s_info_t;

static int issu_max_units; /* The current supported units */
static issu_ctrl_unit_t *issu_ctrl_unit;

static ctrl_blk_chain_t *issu_gen_comp_s_ctrl;
static shr_ha_sub_id issu_gen_sub_comp_ids;
static sal_mutex_t issu_gen_ctrl_sync_obj;

static bcmissu_general_ctrl_t *issu_general_ctrl;

/*******************************************************************************
 * Local functions
 */
/*!
 * \brief Warm boot initialization.
 *
 * This function allocates all the previously allocated HA blocks that were
 * used in previous run by the ISSU. The HA block allocation continues as long
 * as the newly allocated block has the ISSU block signature and as long as the
 * previous block next pointer is not NULL.
 * It also renew the 'next' link between the HA
 * blocks since the value of the pointer doesn't sustain warm boot.
 * By doing so it restores the linked list of ISSU control blocks.
 *
 * \param [in] unit The unit where the HA blocks were allocated.
 * \param [in] blk_size Is the size of each HA memory block.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failure to allocate HA block.
 * \return SHR_E_INTERNAL The last block 'next' pointer was not NULL.
 */
static int issu_warm_init(int unit, uint32_t blk_size)
{
    ctrl_blk_chain_t *blk, *prev_blk = NULL;
    uint32_t desired_size = blk_size;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    if (unit == BCMISSU_NO_UNIT) {
        blk = issu_gen_comp_s_ctrl;
    } else {
        blk = issu_ctrl_unit[unit].blk_chain;
    }

    /* Refresh the real pointer of the control blocks. */
    while (blk && blk->blk && !blk->blk->last &&
           blk->blk->signature == ISSU_BLK_SIGNATURE) {
        blk_size = desired_size;
        SHR_ALLOC(blk->next, sizeof(ctrl_blk_chain_t), "bcmissuUnitBlks");
        SHR_NULL_CHECK(blk->next, SHR_E_MEMORY);
        prev_blk = blk;
        blk = blk->next;
        blk->next = NULL;
        if (unit == BCMISSU_NO_UNIT) {
            blk->blk = shr_ha_gen_mem_alloc(BCMMGMT_ISSU_COMP_ID,
                                            issu_gen_sub_comp_ids++,
                                            NULL,
                                            &blk_size);
        } else {
            blk->blk = shr_ha_mem_alloc(unit,
                                        issu_ctrl_unit[unit].comp_id,
                                        issu_ctrl_unit[unit].sub_comp_id++,
                                        NULL,
                                        &blk_size);
        }
        if (!blk->blk || (blk_size < desired_size)) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    if (!blk->blk->last) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Mark the previous block as the last block */
        if (prev_blk) {
            prev_blk->blk->last = true;
            prev_blk->next = NULL;
        }
        /* Free the last 'bad' block */
        if (blk->blk) {
            if (unit == BCMISSU_NO_UNIT) {
                shr_ha_gen_mem_free(blk->blk);
                issu_gen_sub_comp_ids--;
            } else {
                shr_ha_mem_free(unit, blk->blk);
                issu_ctrl_unit[unit].sub_comp_id--;
            }
        }
        SHR_FREE(blk);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find the structure control entry for a particular structure ID.
 *
 * This function searches the structure control DB for the entry that records
 * the location of a structure within a particular HA block (given by the
 * component and sub IDs).
 *
 * \param [in] unit The unit to search the structure control entry.
 * \param [in] comp_id The component ID owns the HA block.
 * \param [in] sub_id The component sub ID identify the HA block.
 * \param [in] id The structure ID to search for.
 * \param [out] blk When not NULL this function also provides the control
 * block itself. This used when the caller wants to edit the control block,
 * typically after an entry was deleted, in order to prevent holes in the
 * records.
 *
 * \return The control record of the desired structure.
 */
static bcmissu_comp_s_info_t *bcmissu_find_struct_entry(
                                         int unit,
                                         shr_ha_mod_id comp_id,
                                         shr_ha_sub_id sub_id,
                                         bcmissu_struct_id_t id,
                                         ctrl_blk_chain_t **blk)
{
    ctrl_blk_chain_t *block;
    bcmissu_comp_s_info_t *s_info;
    uint32_t j;

    if (unit == BCMISSU_NO_UNIT) {
        block = issu_gen_comp_s_ctrl;
    } else {
        block = issu_ctrl_unit[unit].blk_chain;
    }
    /* Go through all the current struct info HA blocks */
    while (block && block->blk->signature == ISSU_BLK_SIGNATURE) {
        s_info = (bcmissu_comp_s_info_t *)(block->blk + 1);

        for (j = 0; j < block->blk->num_of_elements; j++, s_info++) {
            if (s_info->struct_id == id && s_info->comp_id == comp_id &&
                s_info->sub_id == sub_id) {
                if (blk) {
                    *blk = block;
                }
                return s_info;
            }
        }
        block = block->next;
    }
    return NULL;
}

/*!
 * \brief Clear specific structure record
 *
 * This function deletes previously reported structure entry made by
 * \ref bcmissu_struct_info_report or \ref bcmissu_struct_info_gen_report.
 *
 * \param [in] unit The unit to search the structure control entry.
 * \param [in] id The structure ID to clear.
 * \param [in] comp_id The component ID owns the HA block.
 * \param [in] sub_id The component sub ID identify the HA block.
 * \param [in] mtx Restricts the access to the control list to single thread at
 * a time.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND The report was not found.
 */
static int struct_info_clear(int unit,
                             bcmissu_struct_id_t id,
                             shr_ha_mod_id comp_id,
                             shr_ha_sub_id sub_id,
                             sal_mutex_t mtx)
{
    bcmissu_comp_s_info_t *s_info;
    bcmissu_comp_s_info_t *last_s_info;
    bcmissu_comp_s_ctrl_blk_t *blk;
    ctrl_blk_chain_t *block;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    s_info = bcmissu_find_struct_entry(unit, comp_id, sub_id, id, &block);
    if (!s_info) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (unit == BCMISSU_NO_UNIT) {
        block = issu_gen_comp_s_ctrl;
    } else {
        block = issu_ctrl_unit[unit].blk_chain;
    }
    SHR_NULL_CHECK(block, SHR_E_NOT_FOUND);

    /*
     * If the block is not full
     */
    if (block->blk->num_of_elements > 0 &&
        block->blk->num_of_elements < block->blk->block_size) {
        blk = block->blk;
        /* Find the last valid entry of the block */
        last_s_info = (bcmissu_comp_s_info_t *)(blk + 1);
        last_s_info += blk->num_of_elements - 1;
        if (blk->num_of_elements > 1 && last_s_info != s_info) {
            /* Copy the data from the kast element to the element to clear */
            sal_memcpy(s_info, last_s_info, sizeof(*s_info));
        }
        blk->num_of_elements--;
    } else {
        /*
         * To prevent holes we move the last entry of the last block to this
         * one
         */
        /* Find the last non-empty block */
        while (block->next && block->next->blk->num_of_elements > 0) {
            block = block->next;
        }
        blk = block->blk;
        /* Find the last valid entry of the block */
        last_s_info = (bcmissu_comp_s_info_t *)(blk + 1);
        last_s_info += blk->num_of_elements - 1;

        /* Copy the data from the kast element to the element to clear */
        sal_memcpy(s_info, last_s_info, sizeof(*s_info));

        /* Delete the last entry */
        blk->num_of_elements--;
    }
exit:
    sal_mutex_give(mtx);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Increase ISSU control unit array size for new unit.
 *
 * The ISSU is not aware of the max unit number. It therefore adjusts its
 * control unit structure when a unit number that is larger than the maximal
 * unit so far is being initialized.
 *
 * \param [in] unit The unit that is currently the max unit.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
static int issu_inc_array_sizes(int unit)
{
    issu_ctrl_unit_t *ctrl_unit_tmp;
    sal_mutex_t mtx = bcmissu_unit_sync_obj_get();

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mtx, SHR_E_INTERNAL);
    sal_mutex_take(mtx, SAL_MUTEX_FOREVER);
    ctrl_unit_tmp = issu_ctrl_unit;
    issu_ctrl_unit = NULL;

    SHR_ALLOC(issu_ctrl_unit,
              sizeof(issu_ctrl_unit_t) * (unit + 2),
              "bcmissuCtrlUnit");
    SHR_NULL_CHECK(issu_ctrl_unit, SHR_E_MEMORY);
    sal_memset(issu_ctrl_unit,
               0,
               sizeof(issu_ctrl_unit_t) * (unit + 2));

    /* Copy the old information if exist */
    if (ctrl_unit_tmp) {
        sal_memcpy(issu_ctrl_unit,
                   ctrl_unit_tmp,
                   sizeof(issu_ctrl_unit_t) * issu_max_units);
    }
    issu_max_units = unit + 2;

exit:
    if (mtx) {
        sal_mutex_give(mtx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create sorted linked list of control blocks.
 *
 * This function searches for all the control blocks that are pointing into
 * a particular HA block. The function put all these control blocks in a linked
 * list that is sorted by the structure offset within the HA block.
 *
 * \param [in] block The control block database.
 * \param [in] ha_blk_info Information about the HA block.
 *
 * \return Pointer to linked list on success.
 * \return NULL Failure.
 */
static blk_s_info_t *find_s_info_of_blk(ctrl_blk_chain_t *block,
                                        bcmissu_ha_blk_inc_t *ha_blk_info)
{
    bcmissu_comp_s_ctrl_blk_t *blk;
    bcmissu_comp_s_info_t *s_info;
    blk_s_info_t *blk_s_info_chain, *blk_s_info_it, *new_blk_s_info;
    uint32_t k;

    blk_s_info_chain = NULL;
    while (block && block->blk && block->blk->signature == ISSU_BLK_SIGNATURE) {
        blk = block->blk;
        s_info = (bcmissu_comp_s_info_t *)(blk + 1);
        for (k = 0; k < blk->num_of_elements; k++, s_info++) {
            /* Verify that the structure belong to the block */
            if (s_info->comp_id != ha_blk_info->comp_id ||
                s_info->sub_id != ha_blk_info->sub_id) {
                continue;
            }
            /* Add the s_info into the sorted linked list */
            new_blk_s_info = sal_alloc(sizeof(blk_s_info_t), "bcmissuCtrlBlk");
            if (!new_blk_s_info) {
                if (blk_s_info_chain) {
                    sal_free(blk_s_info_chain);
                }
                return NULL;
            }
            new_blk_s_info->s_info = s_info;
            /* Check if insert as the first element of the list */
            if (!blk_s_info_chain ||
                blk_s_info_chain->s_info->offset > s_info->offset) {
                new_blk_s_info->next = blk_s_info_chain;
                blk_s_info_chain = new_blk_s_info;
            } else {
                blk_s_info_it = blk_s_info_chain;
                while (blk_s_info_it->next &&
                       blk_s_info_it->next->s_info->offset < s_info->offset) {
                    blk_s_info_it = blk_s_info_it->next;
                }
                new_blk_s_info->next = blk_s_info_it->next;
                blk_s_info_it->next = new_blk_s_info;
            }
        }
        block = block->next;
    }
    return blk_s_info_chain;
}

/*!
 * \brief Process general purpose checks.
 *
 * This function is intended to process general purpose checks related to power
 * up. Initially it checks for s/w checksum mismatch during warm boot but it
 * can be expended later to verify other attributes.
 * The function allocates dedicated HA block (from the generic block pool)
 * to maintain the software signature. During warm boot this signature is
 * compared to the actual signature. The function fails if there is a
 * mismatch.
 *
 * \param [in] comp_id The component ID is used to allocate the HA block.
 * \param [in] warm Indicates if it is cold or warm boot.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate HA memory.
 * \return SHR_E_INTERNAL The s/w signature string length is too long.
 * \return SHR_E_FAIL An attempt to perform warm boot with mismatch s/w
 * signature.
 */
static int handle_general_checks(shr_ha_mod_id comp_id, bool warm)
{
    uint32_t blk_size;
    int len = sal_strlen(BCMISSU_LIBSUM_STR);
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!warm) {
        shr_ha_str_to_id("issu_general_blk",
                         SHR_HA_BLK_USAGE_OTHER,
                         MAX_BLK_ID_STR_LEN,
                         ha_id);
    }
    blk_size = sizeof(*issu_general_ctrl);
    issu_general_ctrl = shr_ha_gen_mem_alloc
            (comp_id, SHR_HA_SUB_IDS-1, (warm ? NULL : ha_id), &blk_size);
    SHR_NULL_CHECK(issu_general_ctrl,  SHR_E_MEMORY);
    if (blk_size < sizeof(*issu_general_ctrl)) {
        issu_general_ctrl = shr_ha_gen_mem_realloc(issu_general_ctrl,
                                                   sizeof(*issu_general_ctrl));
        SHR_NULL_CHECK(issu_general_ctrl,  SHR_E_MEMORY);
    }
    /* During cold boot or ISSU we need to update the s/w checksum */
    if (!warm || bcmissu_is_active()) {
        if (len >= BCMISSU_SW_SIGNATURE_MAX_LEN) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_strncpy(issu_general_ctrl->software_signature,
                    BCMISSU_LIBSUM_STR,
                    len);
        issu_general_ctrl->software_signature[len] = '\0';
    } else if (sal_strncmp(BCMISSU_LIBSUM_STR,
                           issu_general_ctrl->software_signature,
                           len)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Software signature mismatch\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * External functions local to this module
 */
bcmissu_comp_s_info_t* bcmissu_find_struct_using_offset(int unit,
                                                        shr_ha_mod_id comp_id,
                                                        shr_ha_sub_id sub_id,
                                                        uint32_t offset)
{
    bcmissu_comp_s_ctrl_blk_t *blk;
    ctrl_blk_chain_t *block;
    bcmissu_comp_s_info_t *s_info;
    uint32_t j;

    if (unit == BCMISSU_NO_UNIT) {
        block = issu_gen_comp_s_ctrl;
    } else {
        block = issu_ctrl_unit[unit].blk_chain;
    }
    /* Go through all the current struct info HA blocks */
    while (block && block->blk && block->blk->signature == ISSU_BLK_SIGNATURE) {
        blk = block->blk;
        s_info = (bcmissu_comp_s_info_t *)(blk + 1);

        for (j = 0; j < blk->num_of_elements; j++, s_info++) {
            if (s_info->comp_id == comp_id && s_info->sub_id == sub_id &&
                offset >= s_info->offset &&
                offset < s_info->offset + s_info->data_size) {
                return s_info;
            }
        }
        block = block->next;
    }
    return NULL;
}

/* Find the first instance of a struct info associated with the structure ID */
bcmissu_comp_s_info_t *bcmissu_find_first_s_info(
                                     int unit,
                                     bcmissu_struct_id_t id,
                                     void **hint_struct)
{
    bcmissu_comp_s_ctrl_blk_t *blk;
    ctrl_blk_chain_t *block;
    uint32_t j;
    bcmissu_comp_s_info_t *s_info = NULL;
    int rv = SHR_E_NOT_FOUND;

    if (unit == BCMISSU_NO_UNIT) {
        block = issu_gen_comp_s_ctrl;
    } else {
        block = issu_ctrl_unit[unit].blk_chain;
    }
    /* Go through all the current struct info blocks */
    while (rv == SHR_E_NOT_FOUND && block && block->blk &&
           block->blk->signature == ISSU_BLK_SIGNATURE) {
        blk = block->blk;
        s_info = (bcmissu_comp_s_info_t *)(blk + 1);
        for (j = 0; j < blk->num_of_elements; j++, s_info++) {
            if (s_info->struct_id == id) {
                if (hint_struct) {
                    *hint_struct = block;
                }
                rv = SHR_E_NONE;
                break;
            }
        }
        block = block->next;
    }

    if (rv == SHR_E_NONE) {
        return s_info;
    } else {
        return NULL;
    }
}

/* Find the next instance of a struct info associated with the structure ID */
bcmissu_comp_s_info_t *bcmissu_find_next_s_info(
                                    bcmissu_struct_id_t id,
                                    bcmissu_comp_s_info_t *hint_info,
                                    void **hint_struct)
{
    ctrl_blk_chain_t *block = (ctrl_blk_chain_t *)*hint_struct;
    bcmissu_comp_s_ctrl_blk_t *blk;
    uint32_t j;
    bcmissu_comp_s_info_t *s_info;
    int rv = SHR_E_NOT_FOUND;

    if (!block || !block->blk) {
        return NULL;
    }
    blk = block->blk;
    s_info = (bcmissu_comp_s_info_t *)(blk + 1);
    j = (hint_info - s_info) + 1;
    s_info = hint_info + 1;
    while (rv == SHR_E_NOT_FOUND && block &&
           block->blk && block->blk->signature == ISSU_BLK_SIGNATURE) {
        blk = block->blk;
        for (; j < blk->num_of_elements; j++, s_info++) {
            if (s_info->struct_id == id) {
                rv = SHR_E_NONE;
                *hint_struct = block;
                break;
            }
        }

        block = block->next;
        if (block && block->blk && rv != SHR_E_NONE) {
            s_info = (bcmissu_comp_s_info_t *)(block->blk + 1);
        }
        j = 0;
    }

    if (rv == SHR_E_NONE) {
        return s_info;
    } else {
        return NULL;
    }
}

int bcmissu_struct_init(int unit, shr_ha_mod_id comp_id, bool warm)
{
    uint32_t blk_size = sizeof(bcmissu_comp_s_ctrl_blk_t);
    ctrl_blk_chain_t *block = NULL;
    uint32_t desired_size;
    bcmissu_comp_s_ctrl_blk_t *blk = NULL;
    char ha_id[MAX_BLK_ID_STR_LEN];

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    if (unit == BCMISSU_NO_UNIT) {
        if (issu_gen_comp_s_ctrl) {
            SHR_EXIT();
        }
    } else if (unit >= issu_max_units) {
        SHR_IF_ERR_EXIT(issu_inc_array_sizes(unit));
    } else if (issu_ctrl_unit[unit].blk_chain) {
        SHR_EXIT();
    }

    blk_size += sizeof(bcmissu_comp_s_info_t) * STRUCTS_PER_BLK;
    desired_size = blk_size;
    SHR_ALLOC(block, sizeof(ctrl_blk_chain_t), "bcmissuUnitBlks");
    SHR_NULL_CHECK(block, SHR_E_MEMORY);
    if (!warm) {
        shr_ha_str_to_id("issu_track_first",
                         SHR_HA_BLK_USAGE_OTHER,
                         MAX_BLK_ID_STR_LEN,
                         ha_id);
    }
    if (unit == BCMISSU_NO_UNIT) {
        issu_gen_ctrl_sync_obj = sal_mutex_create("issuUnit");
        SHR_NULL_CHECK(issu_gen_ctrl_sync_obj, SHR_E_MEMORY);
        blk = shr_ha_gen_mem_alloc(comp_id,
                                   issu_gen_sub_comp_ids++,
                                   (warm ? NULL : ha_id),
                                   &blk_size);
        issu_gen_comp_s_ctrl = block;

        /* Also allocate the general block */
        SHR_IF_ERR_EXIT(handle_general_checks(comp_id, warm));
    } else {
        issu_ctrl_unit[unit].ctrl_sync_obj = sal_mutex_create("issuUnit");
        SHR_NULL_CHECK(issu_ctrl_unit[unit].ctrl_sync_obj, SHR_E_MEMORY);
        blk = shr_ha_mem_alloc(unit,
                               comp_id,
                               issu_ctrl_unit[unit].sub_comp_id++,
                               (warm ? NULL : ha_id),
                               &blk_size);
        issu_ctrl_unit[unit].comp_id = comp_id;
        issu_ctrl_unit[unit].blk_chain = block;
        issu_ctrl_unit[unit].warm = warm;
    }
    SHR_NULL_CHECK(blk,  SHR_E_MEMORY);
    if (blk_size < desired_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    block->blk = blk;
    block->next = NULL;

    if (warm) {
        if (blk->signature == ISSU_BLK_SIGNATURE) {
            SHR_IF_ERR_EXIT(issu_warm_init(unit, blk_size));
        } else {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        blk->signature = ISSU_BLK_SIGNATURE;
        blk->last = true;
        blk->num_of_elements = 0;
        blk->block_size = STRUCTS_PER_BLK;
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (blk) {
            shr_ha_mem_free(unit, blk);
        }
        if (block) {
            SHR_FREE(block);
        }
        if (unit == BCMISSU_NO_UNIT) {
            issu_gen_comp_s_ctrl = NULL;
        } else {
            issu_ctrl_unit[unit].blk_chain = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

int bcmissu_struct_shutdown(int unit, bool graceful)
{
    ctrl_blk_chain_t *blk, *prev_blk;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);

    if (unit == BCMISSU_NO_UNIT) {
        if (!issu_gen_comp_s_ctrl) {
            SHR_EXIT();
        }
        sal_mutex_destroy(issu_gen_ctrl_sync_obj);
        issu_gen_ctrl_sync_obj = NULL;
        blk = issu_gen_comp_s_ctrl;
        while (blk) {
            prev_blk = blk;
            blk = blk->next;
            SHR_FREE(prev_blk);
        }
        issu_gen_comp_s_ctrl = NULL;
        issu_gen_sub_comp_ids = 0;
    } else {
        if (!issu_ctrl_unit[unit].blk_chain) {
            SHR_EXIT();
        }
        sal_mutex_destroy(issu_ctrl_unit[unit].ctrl_sync_obj);
        issu_ctrl_unit[unit].ctrl_sync_obj = NULL;
        blk = issu_ctrl_unit[unit].blk_chain;
        while (blk) {
            prev_blk = blk;
            blk = blk->next;
            SHR_FREE(prev_blk);
        }
        issu_ctrl_unit[unit].blk_chain = NULL;
        issu_ctrl_unit[unit].sub_comp_id = 0;
        issu_ctrl_unit[unit].comp_id = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

void bcmissu_struct_cleanup()
{
    SHR_FREE(issu_ctrl_unit);
    issu_max_units = 0;
}

int bcmissu_ha_block_extend(int unit,
                            bcmissu_ha_blk_inc_t *ha_blk_info)
{
    uint8_t * src,*dest;
    ctrl_blk_chain_t *block;
    uint8_t *ha_blk = ha_blk_info->blk;
    uint32_t j;
    bcmissu_comp_s_info_t *s_info;
    uint32_t new_blk_size = ha_blk_info->inc + ha_blk_info->orig_size;
    blk_s_info_t *blk_s_info_chain, *blk_s_info_it;
    uint8_t *blk_cpy;
    int data_shift;

    /* Resize the HA block */
    if (unit == BCMISSU_NO_UNIT) {
        ha_blk = shr_ha_gen_mem_realloc(ha_blk, new_blk_size);
        block = issu_gen_comp_s_ctrl;
    } else {
        ha_blk = shr_ha_mem_realloc(unit, ha_blk, new_blk_size);
        block = issu_ctrl_unit[unit].blk_chain;
    }
    if (!ha_blk) {
        return SHR_E_MEMORY;
    }
    ha_blk_info->blk = ha_blk;

    /* Allocate a buffer and copy the block content into it */
    blk_cpy = sal_alloc(ha_blk_info->orig_size, "bcmissuBlkResize");
    if (!blk_cpy) {
        return SHR_E_MEMORY;
    }
    sal_memcpy(blk_cpy, ha_blk, ha_blk_info->orig_size);

    /*
     * Prepare a link list of all the structures within this block. The list
     * is sorted based on structure offset.
     */
    blk_s_info_chain = find_s_info_of_blk(block, ha_blk_info);
    if (!blk_s_info_chain) {
        sal_free(blk_cpy);
        return SHR_E_NONE;
    }
    data_shift = 0;
    for (blk_s_info_it = blk_s_info_chain; blk_s_info_it;) {
        s_info = blk_s_info_it->s_info;
        src = blk_cpy + s_info->offset;
        s_info->offset += data_shift; /* Offset will always stay positive */
        dest = ha_blk + s_info->offset;
        for (j = 0; j < s_info->instances; j++) {
            sal_memcpy(dest, src, s_info->data_size);
            dest += s_info->max_data_size;
            src += s_info->data_size;
        }
        data_shift += s_info->instances *
                ((int)s_info->max_data_size - (int)s_info->data_size);
        blk_s_info_chain = blk_s_info_it;
        blk_s_info_it = blk_s_info_it->next;
        sal_free(blk_s_info_chain);
    }

    /* Free the buffer copy */
    sal_free(blk_cpy);
    return SHR_E_NONE;
}

int bcmissu_validate_unit(int unit)
{
    if (unit < 0 || unit >= issu_max_units ||
        !issu_ctrl_unit[unit].blk_chain) {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public API functions
 */
int bcmissu_struct_info_report(int unit,
                               shr_ha_mod_id comp_id,
                               shr_ha_sub_id sub_id,
                               uint32_t offset,
                               uint32_t data_size,
                               size_t instances,
                               bcmissu_struct_id_t id)
{
    ctrl_blk_chain_t *block = NULL;
    ctrl_blk_chain_t *prev_block;
    bcmissu_comp_s_ctrl_blk_t *blk;
    bcmissu_comp_s_info_t *struct_info;
    sal_mutex_t sem;

    SHR_FUNC_ENTER(unit == BCMISSU_NO_UNIT ? BSL_UNIT_UNKNOWN : unit);
    if (instances == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Array size of 0 used for"
                                " bcmissu_struct_array_info_report\n")));
    }
    if (unit == BCMISSU_NO_UNIT) {
        block = issu_gen_comp_s_ctrl;
        sem = issu_gen_ctrl_sync_obj;
    } else {
        if (unit < 0 || unit >= issu_max_units) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        block = issu_ctrl_unit[unit].blk_chain;
        sem = issu_ctrl_unit[unit].ctrl_sync_obj;
    }
    if (!block) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_mutex_take(sem, SAL_MUTEX_FOREVER);

    /* Check if already exist */
    struct_info = bcmissu_find_struct_entry(unit, comp_id, sub_id, id, NULL);
    if (struct_info) {
        /*
         * The structure was reported before update the structure with the new
         * reported information. This gives the caller a chance to update its
         * report once its data structure had been updated.
         */
        struct_info->data_size = data_size;
        struct_info->last_ver_data_size = data_size;
        struct_info->instances = instances;
        struct_info->max_data_size = data_size;
        struct_info->offset = offset;
        /* Inform the caller that the entry has been reported before */
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Sanity, we are not expecting new allocation during warm boot. */
    if (unit != BCMISSU_NO_UNIT && issu_ctrl_unit[unit].warm &&
        unit < BCMDRD_CONFIG_MAX_UNITS && !bcmissu_is_active()) {
        shr_sysm_states_t state;
        int rv;
        rv = shr_sysm_instance_state_get(SHR_SYSM_CAT_UNIT, unit, &state);
        if (SHR_SUCCESS(rv) && state != SHR_SYSM_RUN) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Component %u, sub %u reporting new object "
                                 "0x%" PRIu64 "during warm boot\n"),
                                 comp_id, sub_id, id));
        }
    }
    /* Find the last block */
    while (block->next) {
        block = block->next;
    }
    blk = block->blk;
    if (blk->num_of_elements == blk->block_size) {
        uint32_t desired_size;
        uint32_t blk_size = sizeof(bcmissu_comp_s_ctrl_blk_t);
        char ha_id[MAX_BLK_ID_STR_LEN];

        blk_size += sizeof(bcmissu_comp_s_info_t) * STRUCTS_PER_BLK;
        desired_size = blk_size;
        /*
         * Check if the sub ID got to its max value. We save the last sub ID
         * as general purpose HA block
         */
        if ((unit == BCMISSU_NO_UNIT && issu_gen_sub_comp_ids >= SHR_HA_SUB_IDS-1) ||
            (unit != BCMISSU_NO_UNIT &&
             issu_ctrl_unit[unit].sub_comp_id >= SHR_HA_SUB_IDS)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The number of structures reported to ISSU"
                                 " exceed its memory limitation\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        SHR_ALLOC(block->next, sizeof(ctrl_blk_chain_t), "bcmissuUnitBlks");
        SHR_NULL_CHECK(block->next, SHR_E_MEMORY);
        prev_block = block;
        block = block->next;
        block->next = NULL;
        shr_ha_str_to_id("issu_track",
                         SHR_HA_BLK_USAGE_OTHER,
                         MAX_BLK_ID_STR_LEN,
                         ha_id);
        if (unit == BCMISSU_NO_UNIT) {
            block->blk = shr_ha_gen_mem_alloc(BCMMGMT_ISSU_COMP_ID,
                                              issu_gen_sub_comp_ids++,
                                              ha_id,
                                              &blk_size);
        } else {
            block->blk = shr_ha_mem_alloc(unit,
                                          issu_ctrl_unit[unit].comp_id,
                                          issu_ctrl_unit[unit].sub_comp_id++,
                                          ha_id,
                                          &blk_size);
        }
        SHR_NULL_CHECK(block->blk,  SHR_E_MEMORY);
        if (blk_size < desired_size) {
            if (unit == BCMISSU_NO_UNIT) {
                shr_ha_gen_mem_free(block->blk);
                issu_gen_sub_comp_ids--;
            } else {
                shr_ha_mem_free(unit,  block->blk);
                issu_ctrl_unit[unit].sub_comp_id--;
            }
            prev_block->next = NULL;
            SHR_FREE(block);
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        blk->last = false;
        blk = block->blk;
        blk->signature = ISSU_BLK_SIGNATURE;
        blk->last = true;
        blk->num_of_elements = 0;
        blk->block_size = STRUCTS_PER_BLK;
    }

    struct_info = (bcmissu_comp_s_info_t *)(blk + 1);
    struct_info += blk->num_of_elements;
    struct_info->struct_id = id;
    struct_info->comp_id = comp_id;
    struct_info->sub_id = sub_id;
    struct_info->offset = offset;
    struct_info->data_size = data_size;
    struct_info->last_ver_data_size = data_size;
    struct_info->max_data_size = data_size;
    struct_info->instances = (uint16_t)instances;
    blk->num_of_elements++;
exit:
    if (block) {
        sal_mutex_give(sem);
    }
    SHR_FUNC_EXIT();
}

int bcmissu_struct_info_gen_report(shr_ha_mod_id comp_id,
                                   shr_ha_sub_id sub_id,
                                   uint32_t offset,
                                   uint32_t data_size,
                                   size_t instances,
                                   bcmissu_struct_id_t id)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmissu_struct_info_report(BCMISSU_NO_UNIT, comp_id, sub_id,
                                   offset, data_size, instances, id));
exit:
    SHR_FUNC_EXIT();
}

int bcmissu_struct_info_get(int unit,
                            bcmissu_struct_id_t id,
                            shr_ha_mod_id comp_id,
                            shr_ha_sub_id sub_id,
                            uint32_t *offset)
{
    bcmissu_comp_s_info_t *struct_info;
    bool unit_valid = false;

    SHR_FUNC_ENTER(unit);

    if (unit < 0 || unit >= issu_max_units || !offset) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    unit_valid = true;
    sal_mutex_take(issu_ctrl_unit[unit].ctrl_sync_obj, SAL_MUTEX_FOREVER);

    struct_info = bcmissu_find_struct_entry(unit, comp_id, sub_id, id, NULL);
    if (struct_info) {
        *offset = struct_info->offset;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (unit_valid) {
        sal_mutex_give(issu_ctrl_unit[unit].ctrl_sync_obj);
    }
    SHR_FUNC_EXIT();
}

int bcmissu_struct_info_gen_get(bcmissu_struct_id_t id,
                                shr_ha_mod_id comp_id,
                                shr_ha_sub_id sub_id,
                                uint32_t *offset)
{
    bcmissu_comp_s_info_t *struct_info;
    bool unit_valid = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!offset) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    unit_valid = true;
    sal_mutex_take(issu_gen_ctrl_sync_obj, SAL_MUTEX_FOREVER);

    struct_info = bcmissu_find_struct_entry(BCMISSU_NO_UNIT, comp_id, sub_id,
                                            id, NULL);
    if (struct_info) {
        *offset = struct_info->offset;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (unit_valid) {
        sal_mutex_give(issu_gen_ctrl_sync_obj);
    }
    SHR_FUNC_EXIT();
}

int bcmissu_struct_info_clear(int unit,
                              bcmissu_struct_id_t id,
                              shr_ha_mod_id comp_id,
                              shr_ha_sub_id sub_id)
{
    SHR_FUNC_ENTER(unit);
    if (unit < 0 || unit >= issu_max_units) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (struct_info_clear(unit, id, comp_id,
                           sub_id, issu_ctrl_unit[unit].ctrl_sync_obj));

exit:
    SHR_FUNC_EXIT();
}

int bcmissu_struct_info_gen_clear(bcmissu_struct_id_t id,
                                  shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_VERBOSE_EXIT
        (struct_info_clear(BCMISSU_NO_UNIT, id, comp_id,
                           sub_id, issu_gen_ctrl_sync_obj));

exit:
    SHR_FUNC_EXIT();
}

